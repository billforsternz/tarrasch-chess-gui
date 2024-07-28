/****************************************************************************
 *  Iterate through a chess document
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include "Stepper.h"

void Stepper::Next()
{
    idx++;
}

bool Stepper::End()
{
    depth = stk_idx;
    if( state == AFTER_MOVE )
    {
        cr.PlayMove(stk->mv);
        state = IN_MOVES;
    }
    if( idx >= len )
        return true;
    uint8_t code = (uint8_t)bc[idx];
    raw = code;
    switch( state )
    {
        case AFTER_ESCAPE:
        {
            ct    = ct_escape_code;
            state = IN_MOVES;
            break;
        }
        case IN_META:
        {
            if( code != BC_META_END )
                ct = ct_meta_data;
            else
            {
                ct = ct_meta_end;
                state = IN_MOVES;
            }
            break;
        }
        case IN_COMMENT:
        {
            if( code != BC_COMMENT_END )
            {
                ct = ct_comment_txt;
                comment_txt += code;
            }
            else
            {
                ct = ct_comment_end;
                state = IN_MOVES;
            }
            break;
        }
        case IN_MOVES:
        {
            switch( code )
            {
                case BC_VARIATION_START:
                {

                    // Push current state onto a stack
                    ct  = ct_variation_start;
                    stk->cr = cr;
                    if( stk_idx+1 < MAX_DEPTH )
                    {

                        // Pop off most recent move
                        if( stk->variation_move_count > 0 )
                        {
                            if( cr.white )
                            {
                                cr.full_move_count--;   // PushMove()/PopMove()
                                                        //  don't touch move counts
                                                        //  we have to since we are
                                                        //  doing PopMove() without
                                                        //  PushMove()
                            }
                            cr.PopMove( stk->mv );
                        }
                        stk_idx++;
                        stk = &stk_array[stk_idx];
                        stk->variation_move_count = 0;

                        // Disrupting encoding, force rescan
                        sides[0].fast_mode=false;
                        sides[1].fast_mode=false;
                    }
                    break;
                }
                case BC_VARIATION_END:
                {
                    ct  = ct_variation_end;

                    // Pop old state off stack
                    if( stk_idx > 0 )
                    {
                        stk_idx--;
                        stk = &stk_array[stk_idx];
                        cr  = stk->cr;

                        // Disrupting encoding, force rescan
                        sides[0].fast_mode=false;
                        sides[1].fast_mode=false;
                    }
                    break;
                }
                case BC_COMMENT_START:
                {
                    state = IN_COMMENT;
                    ct  = ct_comment_start;
                    comment_txt.clear();
                    break;
                }
                case BC_META_START:
                {
                    state = IN_META;
                    ct  = ct_meta_start;
                    break;
                }

                case BC_ESCAPE:
                {
                    state = AFTER_ESCAPE;
                    ct = ct_escape;
                    break;
                }

                // Uncompress move
                default:
                {
                    if( code < BC_MOVE_CODES )
                    {
                        ct = ct_unknown;
                        break;
                    }
                    ct = ct_move;
                    Army* side  = cr.white ? &sides[0] : &sides[1];
                    Army* other = cr.white ? &sides[1] : &sides[0];
                    bool have_san_move = false;
                    size_t san_move_len = 0;
                    if( side->fast_mode )
                    {
                        stk->mv = UncompressFastMode(code, side, other, san_move);
                        san_move_len = san_move.length();
                        have_san_move = san_move_len > 0;
                    }
                    else if(TryFastMode(side))
                    {
                        stk->mv = UncompressFastMode(code, side, other, san_move);
                        san_move_len = san_move.length();
                        have_san_move = san_move_len > 0;
                    }
                    else
                    {
                        stk->mv = UncompressSlowMode(code);
                        other->fast_mode = false;   // force other side to reset and retry
                    }

                    // Check, is it mate ? (UncompressFastMode() leaves this to the caller
                    //  in order to be as fast as possible)
                    if( have_san_move && san_move[san_move_len-1]=='+' )
                    {

                        // A little optimisation, it can't be mate if the next code is a move
                        bool next_code_is_a_move = (idx+1 < len) && ((uint8_t)bc[idx+1] >= BC_MOVE_CODES);
                        if( !next_code_is_a_move )
                        {
                            cr.PushMove(stk->mv);   // need to temporarily play the move
                            thc::TERMINAL score_terminal;
                            cr.Evaluate( score_terminal );
                            if( score_terminal == thc::TERMINAL_WCHECKMATE || score_terminal == thc::TERMINAL_BCHECKMATE )
                                san_move[san_move_len-1] = '#';
                            cr.PopMove(stk->mv);
                        }
                    }
                    san_move = have_san_move ? san_move : stk->mv.NaturalOut(&cr);
                    state = AFTER_MOVE;
                    stk->variation_move_count++;
                    break;
                }
            }
        }
    }
    return false;
}

