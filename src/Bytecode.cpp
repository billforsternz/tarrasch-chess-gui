/****************************************************************************
 *  Compress chess moves into one byte
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include "Bytecode.h"
#include "DebugPrintf.h"
#include "Lang.h"
#include "GameTree.h"  // temp todo

const int MAX_DEPTH = 30;

//
// In branch "new-heart" I am starting out on an ambitious plan to transplant
//  a new heart into Tarrasch - based on bytecode. The byte code will be the
//  same one byte per move representation developed to implement the database
//  features (class CompressMoves) enhanced with techniques to represent
//  variations, annotations, comments (and more?). This should be more flexible
//  maintainable, understandable and improveable than the current MoveTree
//  recursive vector representation.
//
//  I am going to try to keep the code compiling and linking, but it won't
//  necessarily be functional for a while.
//

#define DIAG_ONLY(x)

// Lookup tables for fast SAN move generation
struct RayValues
{
    int8_t ray_offset;
    int8_t ray_rook_offset;
    int8_t ray_bishop_offset;
    int8_t ray_square;
    int8_t ray_count;
};
static RayValues ray_lookup_table[64][64];


const char *codepoint_type_array[] =
{
    "ct_move",
    "ct_variation_start",        
    "ct_variation_end",        
    "ct_comment_start",        
    "ct_comment_end",        
    "ct_meta_start",        
    "ct_meta_end",        
    "ct_escape",
    "ct_comment_txt",
    "ct_meta_data",
    "ct_escape_code"
};

const char *codepoint_type_txt( codepoint_type ct )
{
    if( (size_t)ct < sizeof(codepoint_type_array)/sizeof(codepoint_type_array[0]) )
        return codepoint_type_array[ct];
    else
        return "??";
}

static unsigned long nbr_compress_fast;
static unsigned long nbr_compress_slow;
static unsigned long nbr_uncompress_fast;
static unsigned long nbr_uncompress_slow;
static unsigned long nbr_pawn_swaps_histo[8];
static unsigned long nbr_pawn_swaps_histo_wl[8];
static unsigned long nbr_pawn_swaps_histo_wr[8];
static unsigned long nbr_pawn_swaps_histo_bl[8];
static unsigned long nbr_pawn_swaps_histo_br[8];
static unsigned long nbr_knight_moves;
static unsigned long nbr_knight_swaps;
static unsigned long nbr_knight_swaps_alt;
static unsigned long nbr_rook_moves;
static unsigned long nbr_rook_swaps;
static unsigned long nbr_rook_swaps_alt;
static unsigned long nbr_queen_swaps;
int max_nbr_slow_moves_other;
int max_nbr_slow_moves_queen;
int nbr_games_with_promotions;
int nbr_games_with_slow_mode;
int nbr_games_with_two_queens;

void BytecodeDiagBegin()
{
    nbr_compress_fast = 0;
    nbr_compress_slow = 0;
    nbr_uncompress_fast = 0;
    nbr_uncompress_slow = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_wl[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_wr[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_bl[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_br[i] = 0;
    nbr_knight_moves = 0;
    nbr_knight_swaps = 0;
    nbr_knight_swaps_alt = 0;
    nbr_rook_moves = 0;
    nbr_rook_swaps = 0;
    nbr_rook_swaps_alt = 0;
    nbr_queen_swaps = 0;
    max_nbr_slow_moves_other = 0;
    max_nbr_slow_moves_queen = 0;
    nbr_games_with_promotions = 0;
    nbr_games_with_slow_mode = 0;
    nbr_games_with_two_queens = 0;
}

void BytecodeDiagEnd()
{
    cprintf( "nbr_compress_fast   = %lu\n", nbr_compress_fast );
    cprintf( "nbr_compress_slow   = %lu\n", nbr_compress_slow );
    cprintf( "nbr_uncompress_fast = %lu\n", nbr_uncompress_fast );
    cprintf( "nbr_uncompress_slow = %lu\n", nbr_uncompress_slow );  // It turns out about 0.015% of moves are in slow mode
    for(int i=0; i<8; i++)
        cprintf( "nbr_pawn_swaps_histo_wl[%d] = %lu\n", i, nbr_pawn_swaps_histo_wl[i] );
    for(int i=0; i<8; i++)
        cprintf( "nbr_pawn_swaps_histo_wr[%d] = %lu\n", i, nbr_pawn_swaps_histo_wr[i] );
    for(int i=0; i<8; i++)
        cprintf( "nbr_pawn_swaps_histo_bl[%d] = %lu\n", i, nbr_pawn_swaps_histo_bl[i] );
    for(int i=0; i<8; i++)
        cprintf( "nbr_pawn_swaps_histo_br[%d] = %lu\n", i, nbr_pawn_swaps_histo_br[i] );
    cprintf( "nbr_knight_moves     = %lu\n",       nbr_knight_moves );
    cprintf( "nbr_knight_swaps     = %lu\n",       nbr_knight_swaps );
    cprintf( "nbr_knight_swaps_alt = %lu\n",       nbr_knight_swaps_alt );
    cprintf( "nbr_rook_moves       = %lu\n",         nbr_rook_moves );
    cprintf( "nbr_rook_swaps       = %lu\n",         nbr_rook_swaps );
    cprintf( "nbr_rook_swaps_alt   = %lu\n",         nbr_rook_swaps_alt );
    cprintf( "nbr_queen_swaps      = %lu\n",         nbr_queen_swaps );
    cprintf( "nbr_games_with_promotions = %lu\n",     nbr_games_with_promotions );
    cprintf( "nbr_games_with_slow_mode  = %lu\n",     nbr_games_with_slow_mode  );
    cprintf( "nbr games_with_two_queens = %lu\n",     nbr_games_with_two_queens );
}


// The ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES algorithm
//  anticipates future optimised implementations - Such implementations
//  might implement making moves using thc::Move independently
//  and without reference to the thc classes until they are forced
//  to use slow_mode. Upon transition to slow_mode, such implementations
//  rely on move generation algorithms, which in turn need to
//  know whether the kings and rooks have ever moved. Relieve the
//  future implementation's responsibility of tracking this information
//  by always acting as if the kings and rooks haven't moved (if they
//  are on their home squares).  This doesn't cause possible illegality
//  since Compress() only compresses legal moves, so reversing this
//  with Uncompress() only produces legal moves.

#define CODE_KING           0x00
#define CODE_KNIGHT         0x10
#define CODE_ROOK_LO        0x20
#define CODE_ROOK_HI        0x30
#define CODE_BISHOP_DARK    0x40
#define CODE_BISHOP_LIGHT   0x50
#define CODE_QUEEN_ROOK     0x60
#define CODE_QUEEN_BISHOP   0x70
#define CODE_PAWN           0x80
#define CODE_PAWN_0         0x80
#define CODE_PAWN_1         0x90
#define CODE_PAWN_2         0xa0
#define CODE_PAWN_3         0xb0
#define CODE_PAWN_4         0xc0
#define CODE_PAWN_5         0xd0
#define CODE_PAWN_6         0xe0
#define CODE_PAWN_7         0xf0

#define K_VECTOR_N          0x08
#define K_VECTOR_NE         0x09
#define K_VECTOR_E          0x0a
#define K_VECTOR_SE         0x0b
#define K_VECTOR_S          0x0c
#define K_VECTOR_SW         0x0d
#define K_VECTOR_W          0x0e
#define K_VECTOR_NW         0x0f

#define N_VECTOR_NNE        0x00
#define N_VECTOR_NEE        0x01
#define N_VECTOR_SEE        0x02
#define N_VECTOR_SSE        0x03
#define N_VECTOR_SSW        0x04
#define N_VECTOR_SWW        0x05
#define N_VECTOR_NWW        0x06
#define N_VECTOR_NNW        0x07

#define P_DOUBLE            0     // 0000   // 4 non promoting moves
#define P_SINGLE            1     // 0001
#define P_LEFT              2     // 0010
#define P_RIGHT             3     // 0011
#define P_QUEEN             0     // dd00   // dd is direction of promoting move one of P_SINGLE, P_LEFT, P_RIGHT
#define P_ROOK              1     // dd01
#define P_BISHOP            2     // dd10
#define P_KNIGHT            3     // dd11

#define R_RANK              8     // Rank or file (i.e. rook) codes, this bit set indicates same file (so remaining 3 bits encode rank)
#define R_FILE              0

#define B_FALL              8     // Diagonal (i.e. bishop) codes, either RISE/ or FALL\, other 3 bits encode destination file
#define B_RISE              0

#define N_HI                8     // CODE_KNIGHT encodes 8 vectors for each of 2 knights (HI and LO)
#define N_LO                0

// Pawns for each side are assigned logical numbers from 0 to nbr_pawns-1
//  The ordering of the numbers is determined by consulting this table...
static int pawn_ordering[64] =
{
    7, 15, 23, 31, 39, 47, 55, 63,
    6, 14, 22, 30, 38, 46, 54, 62,
    5, 13, 21, 29, 37, 45, 53, 61,
    4, 12, 20, 28, 36, 44, 52, 60,
    3, 11, 19, 27, 35, 43, 51, 59,
    2, 10, 18, 26, 34, 42, 50, 58,
    1,  9, 17, 25, 33, 41, 49, 57,
    0,  8, 16, 24, 32, 40, 48, 56
};

// ...to initially assign logical pawn numbers according to pawn_ordering[]
//  above, traverse the squares in this order and assign 0,1,2... etc as each
//  pawn is found
static thc::Square traverse_order[64] =
{
    thc::a1, thc::a2, thc::a3, thc::a4, thc::a5, thc::a6, thc::a7, thc::a8,
    thc::b1, thc::b2, thc::b3, thc::b4, thc::b5, thc::b6, thc::b7, thc::b8,
    thc::c1, thc::c2, thc::c3, thc::c4, thc::c5, thc::c6, thc::c7, thc::c8,
    thc::d1, thc::d2, thc::d3, thc::d4, thc::d5, thc::d6, thc::d7, thc::d8,
    thc::e1, thc::e2, thc::e3, thc::e4, thc::e5, thc::e6, thc::e7, thc::e8,
    thc::f1, thc::f2, thc::f3, thc::f4, thc::f5, thc::f6, thc::f7, thc::f8,
    thc::g1, thc::g2, thc::g3, thc::g4, thc::g5, thc::g6, thc::g7, thc::g8,
    thc::h1, thc::h2, thc::h3, thc::h4, thc::h5, thc::h6, thc::h7, thc::h8
};

// Try to set fast mode, return bool okay
bool Bytecode::TryFastMode( Army *side )
{
    bool okay = true;
    side->nbr_pawns   = 0;
    side->nbr_knights = 0;
    side->nbr_rooks   = 0;
    side->nbr_dark_bishops  = 0;
    side->nbr_light_bishops = 0;
    side->nbr_queens        = 0;
    for( int i=0; okay && i<64; i++ )
    {

        // Pawns are traversed according to pawn_ordering[]
        int j = static_cast<int>(traverse_order[i]);
        if( cr.squares[j] == (side->white?'P':'p') )
        {
            if( side->nbr_pawns < 8 )
                side->pawns[side->nbr_pawns++] = j;
            else
                okay = false;
        }

        // Other pieces are traversed in normal square convention order
        if( cr.squares[i] == (side->white?'R':'r') )
        {
            if( side->nbr_rooks < 2 )
                side->rooks[side->nbr_rooks++] = i;
            else
            {
                DIAG_ONLY( is_interesting |= 8 );
                okay = false;
            }
        }
        else if( cr.squares[i] == (side->white?'N':'n') )
        {
            if( side->nbr_knights < 2 )
                side->knights[side->nbr_knights++] = i;
            else
            {
                DIAG_ONLY( is_interesting |= 4 );
                okay = false;
            }
        }
        else if( cr.squares[i] == (side->white?'B':'b') )
        {
            if( is_dark(i) )
            {
                side->bishop_dark = i;
                if( side->nbr_dark_bishops < 1 )
                    side->nbr_dark_bishops++;
                else
                {
                    DIAG_ONLY( is_interesting  |= 1 );
                    okay = false;
                }
            }
            else
            {
                side->bishop_light = i;
                if( side->nbr_light_bishops < 1 )
                    side->nbr_light_bishops++;
                else
                {
                    DIAG_ONLY( is_interesting |= 2 );
                    okay = false;
                }
            }
        }
        else if( cr.squares[i] == (side->white?'Q':'q') )
        {
            if( side->nbr_queens < 2 )
                side->queens[side->nbr_queens++] = i;
            else
            {
                DIAG_ONLY( is_interesting |= 16 );  // 3 or more queens!, or more likely see below ....
                okay = false;
            }
        }
        else if( cr.squares[i] == (side->white?'K':'k') )
        {
            side->king = i;
        }
    }
    if( side->nbr_queens==2 && side->nbr_pawns>6 )
    {
        DIAG_ONLY( is_interesting |= 16 );  // .... 2 queens and 7 pawns
        okay = false;
    }
    side->fast_mode = okay;
    return okay;
}

std::string Bytecode::Compress( thc::ChessPosition &cp, const std::vector<thc::Move> &moves_in )
{
    Init( cp );
    return Bytecode::Compress( moves_in );
}


std::string Bytecode::Compress( const std::vector<thc::Move> &moves_in )
{
    std::string ret;
    sides[0].fast_mode=false;
    sides[1].fast_mode=false;
    int len = moves_in.size();
    for( int i=0; i<len; i++ )
    {
        Army *side  = cr.white ? &sides[0] : &sides[1];
        Army *other = cr.white ? &sides[1] : &sides[0];
        char c;
        thc::Move mv = moves_in[i];
        if( side->fast_mode )
        {
            c = CompressFastMode(mv,side,other);
        }
        else if( TryFastMode(side) )
        {
            c = CompressFastMode(mv,side,other);
        }
        else
        {
            c = CompressSlowMode(mv);
            other->fast_mode = false;   // force other side to reset and retry
        }
        cr.PlayMove(mv);
        ret.push_back(c);
    }
    return ret;
}

std::vector<thc::Move> Bytecode::Uncompress( thc::ChessPosition &cp, std::string &moves_in )
{
    Init( cp );
    return Uncompress( moves_in );
}

std::vector<thc::Move> Bytecode::Uncompress( std::string &moves_in )
{
    std::vector<thc::Move> ret;
    sides[0].fast_mode=false;
    sides[1].fast_mode=false;
    int len = moves_in.size();
    for( int i=0; i<len; i++ )
    {
        Army *side  = cr.white ? &sides[0] : &sides[1];
        Army *other = cr.white ? &sides[1] : &sides[0];
        char code = moves_in[i];
        thc::Move mv;
        if( side->fast_mode )
        {
            mv = UncompressFastMode(code,side,other);
        }
        else if( TryFastMode(side) )
        {
            mv = UncompressFastMode(code,side,other);
        }
        else
        {
            mv = UncompressSlowMode(code);
            other->fast_mode = false;   // force other side to reset and retry
        }
        cr.PlayMove(mv);
        ret.push_back(mv);
    }
    return ret;
}

#ifdef _WINDOWS
#define EOL "\r\n"
#else
#define EOL "\n"
#endif
#define WRAP_COLUMN 79

void Bytecode::IterateOver( const std::string& bc, void *utility, void (*callback_func)(void *utility, const std::string& bc, size_t offset, Codepoint &cpt) )
{
    enum
    {
        IN_MOVES,
        IN_COMMENT,
        IN_META,
        AFTER_ESCAPE
    } state = IN_MOVES;

    // Allow stacking of the key state variables
    struct STACK_ELEMENT
    {
        thc::ChessRules cr;
        thc::Move       mv;
        int             variation_move_count=0;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    int stk_idx = 0;
    STACK_ELEMENT *stk = &stk_array[stk_idx];
    std::string s;
    sides[0].fast_mode = false;
    sides[1].fast_mode = false;
    size_t len = bc.size();
    Codepoint cpt;
    cpt.move_nbr_needed = true;
    for( size_t i=0; i<len; i++)
    {
        char code = bc[i];
        cpt.raw     = code;
        cpt.is_move = false;
        cpt.depth   = stk_idx;
        cpt.cr      = &cr;
        switch( state )
        {
            case AFTER_ESCAPE:
            {

                cpt.ct = ct_escape_code;
                state = IN_MOVES;
                break;
            }
            case IN_META:
            {
                if( code != BC_META_END )
                    cpt.ct = ct_meta_data;
                else
                {
                    cpt.ct = ct_meta_end;
                    state = IN_MOVES;
                }
                break;
            }
            case IN_COMMENT:
            {
                if( code != BC_COMMENT_END )
                {
                    cpt.ct = ct_comment_txt;
                    cpt.comment_offset++;
                    cpt.comment_txt += code;
                }
                else
                {
                    cpt.ct = ct_comment_end;
                    state = IN_MOVES;
                }
                break;
            }
            case IN_MOVES:
            {
                cpt.ct = ct_move;
                if( code == BC_VARIATION_START )
                {

                    // Push current state onto a stack
                    cpt.ct  = ct_variation_start;
                    cpt.move_nbr_needed = true;
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
                else if( code == BC_VARIATION_END )
                {
                    cpt.ct  = ct_variation_end;
                    cpt.move_nbr_needed = true;

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
                else if( code == BC_COMMENT_START )
                {
                    state = IN_COMMENT;
                    cpt.ct  = ct_comment_start;
                    cpt.comment_offset = 0;
                    cpt.comment_txt.clear();
                    cpt.move_nbr_needed = true;
                    break;
                }
                else if( code == BC_META_START )
                {
                    state = IN_META;
                    cpt.ct  = ct_meta_start;
                    break;
                }
                else if( code == BC_ESCAPE )
                {
                    state = AFTER_ESCAPE;
                    cpt.ct = ct_escape;
                    break;
                }

                // Uncompress move
                Army* side = cr.white ? &sides[0] : &sides[1];
                Army* other = cr.white ? &sides[1] : &sides[0];
                bool have_san_move = false;
                size_t san_move_len = 0;
                std::string san_move;
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

                cpt.san_move = have_san_move ? san_move : stk->mv.NaturalOut(&cr);

                // Check, is it mate ? (UncompressFastMode() leaves this to the caller
                //  in order to be as fast as possible)
                if( have_san_move && san_move[san_move_len-1]=='+' )
                {
                    cr.PushMove(stk->mv);   // need to temporarily play the move
                    thc::TERMINAL score_terminal;
                    cr.Evaluate( score_terminal );
                    if( score_terminal == thc::TERMINAL_WCHECKMATE || score_terminal == thc::TERMINAL_BCHECKMATE )
                        cpt.san_move[san_move_len-1] = '#';
                    cr.PopMove(stk->mv);
                }
                cpt.is_move = true;
                callback_func( utility, bc, i, cpt );   // callback before playing the move
                cpt.move_nbr_needed = false;            // false during uninterrupted moves
                cr.PlayMove(stk->mv);
                stk->variation_move_count++;
                break;
            }
        }

        // If it was a move, callback has already been called
        if( !cpt.is_move )
            callback_func( utility, bc, i, cpt );
    }
}


std::string Bytecode::PgnOut( const std::string& bc_moves_in, const std::string& result )
{
    // Allow stacking of the key state variables
    struct STACK_ELEMENT
    {
        thc::ChessRules cr;
        thc::Move       mv;
        int             move_nbr=1;
        int variation_move_count=0;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    int stk_idx = 0;
    STACK_ELEMENT *stk = &stk_array[stk_idx];
    bool need_move_number = true;
    int state =0;
    int col = 0;
    std::string s;
    sides[0].fast_mode = false;
    sides[1].fast_mode = false;
    int len = bc_moves_in.size();
    for( int i = 0; i < len; i++)
    {
        char code = bc_moves_in[i];
        switch( state )
        {
            case 4:
            {
                state = 0;
                break;
            }
            case 3:
            {
                if( code == BC_META_END )
                {
                    state = 0;
                }
                break;
            }
            case 2:
            {
                if( code == BC_COMMENT_END )
                {
                    state = 0;
                    s += "} ";
                }
                else
                {
                    s += code;
                }
                break;
            }
            case 0:
            {
                if( code == BC_VARIATION_START )
                {
                    s += " (";
                    need_move_number = true;

                    // Push current state onto a stack
                    stk->cr    = cr;
                    if( stk_idx+1 < MAX_DEPTH )
                    {

                        // Pop off most recent move
                        int move_nbr = stk->move_nbr;
                        if( stk->variation_move_count > 0 )
                        {
                            if( cr.white )
                                move_nbr--;
                            cr.PopMove( stk->mv );
                        }
                        stk_idx++;
                        stk = &stk_array[stk_idx];
                        stk->variation_move_count = 0;
                        stk->move_nbr = move_nbr;

                        // Disrupting encoding, force rescan
                        sides[0].fast_mode=false;
                        sides[1].fast_mode=false;
                    }
                    break;
                }
                else if( code == BC_VARIATION_END )
                {
                    s += ") ";
                    need_move_number = true;

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
                else if( code == BC_COMMENT_START )
                {
                    state = 2;
                    s += " {";
                    need_move_number = true;
                    break;
                }
                else if( code == BC_META_START )
                {
                    state = 3;
                    break;
                }
                else if( code == BC_ESCAPE )
                {
                    state = 4;
                    break;
                }

                // Uncompress move
                Army* side = cr.white ? &sides[0] : &sides[1];
                Army* other = cr.white ? &sides[1] : &sides[0];
                bool have_san_move = false;
                size_t san_move_len = 0;
                std::string san_move;
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
                if( cr.white || need_move_number )
                {
                    char buf[40];
                    _itoa(stk->move_nbr, buf, 10);
                    std::string t(buf);
                    t += (cr.white ? "." : "...");
                    if( col + t.length() >= WRAP_COLUMN )
                    {
                        s += EOL;
                        col = 0;
                    }
                    if( col != 0 )
                    {
                        s += ' ';
                        col++;
                    }
                    s += t;
                    col += t.length();
                }
                need_move_number = false;
                if( !cr.white )
                    stk->move_nbr++;

                std::string t = have_san_move ? san_move : stk->mv.NaturalOut(&cr);
                cr.PlayMove(stk->mv);
                stk->variation_move_count++;

                // Check, is it mate ? [we did have a check for last move - but that was much easier if it's
                //  a bare sequence though, rather than this, the general case]
                if( have_san_move && san_move[san_move_len-1]=='+' )
                {
                    thc::TERMINAL score_terminal;
                    cr.Evaluate( score_terminal );
                    if( score_terminal == thc::TERMINAL_WCHECKMATE || score_terminal == thc::TERMINAL_BCHECKMATE )
                        t[san_move_len-1] = '#';
                }

                /* if( col + t.length() >= WRAP_COLUMN )
                {
                    s += EOL;
                    col = 0;
                }
                if( col != 0 )
                {
                    s += ' ';
                    col++;
                }
                col += t.length(); */
                s += ' ';
                s += t;
                break;
            }
        }
    }
    /* if( col + result.length() >= WRAP_COLUMN )
    {
        s += EOL;
        col = 0;
    }
    if( col != 0 )
    {
        s += ' ';
        col++;
    }   */
    s += ' ';
    s += result;
    s += EOL;
    return s;
}

void Bytecode::GameViewOut( const std::string& bc_moves_in, const std::string& result, std::vector<GameViewElement> &expansion )
{
    int offset = 0;
    expansion.clear();

    // Simple state machine to interpret byte code
    enum
    {
        IN_MOVE,
        IN_ESCAPE,
        IN_COMMENT,
        IN_VARIATION,
        IN_META
    } state = IN_MOVE;

    // Allow stacking of the key state variables
    struct STACK_ELEMENT
    {
        thc::ChessRules cr;
        thc::Move       mv;
        int             move_nbr=1;
        int variation_move_count=0;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    int stk_idx = 0;
    STACK_ELEMENT *stk = &stk_array[stk_idx];
    bool need_move_number = true;
    std::string comment;
    sides[0].fast_mode = false;
    sides[1].fast_mode = false;
    int len = bc_moves_in.size();
    for( int i = 0; i < len; i++)
    {
        char code = bc_moves_in[i];
        switch( state )
        {
            case IN_ESCAPE:
            {
                state = IN_MOVE;
                break;
            }
            case IN_META:
            {
                if( code == BC_META_END )
                {
                    state = IN_MOVE;
                }
                break;
            }
            case IN_COMMENT:
            {
                if( code == BC_COMMENT_END )
                {
                    GameViewElement gve;
                    gve.type    = COMMENT;
                    gve.level   = stk_idx;
                    gve.str     = comment;
                    gve.offset1 = offset;
                    offset += comment.length();
                    gve.offset2 = offset;
                    expansion.push_back(gve);
                    state = IN_MOVE;
                }
                else
                {
                    if( code >= ' ' )
                        comment += code;
                }
                break;
            }
            case IN_MOVE:
            {
                if( code == BC_VARIATION_START )
                {
                    need_move_number = true;

                    // Push current state onto a stack
                    stk->cr    = cr;
                    if( stk_idx+1 < MAX_DEPTH )
                    {

                        GameViewElement gve;
                        gve.type    = NEWLINE;
                        gve.level   = stk_idx;
                        gve.offset1 = offset;
                        offset++;   // "\n"
                        gve.offset2 = offset;
                        expansion.push_back(gve);
                        gve.type    = START_OF_VARIATION;
                        gve.level   = stk_idx;
                        gve.offset1 = offset;
                        offset++;   // "("
                        gve.offset2 = offset;
                        expansion.push_back(gve);

                        // Pop off most recent move
                        int move_nbr = stk->move_nbr;
                        if( stk->variation_move_count > 0 )
                        {
                            if( cr.white )
                                move_nbr--;
                            cr.PopMove( stk->mv );
                        }
                        stk_idx++;
                        stk = &stk_array[stk_idx];
                        stk->variation_move_count = 0;
                        stk->move_nbr = move_nbr;

                        // Disrupting encoding, force rescan
                        sides[0].fast_mode=false;
                        sides[1].fast_mode=false;
                    }
                    break;
                }
                else if( code == BC_VARIATION_END )
                {
                    need_move_number = true;

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
                else if( code == BC_COMMENT_START )
                {
                    state = IN_COMMENT;
                    need_move_number = true;
                    comment.clear();
                    break;
                }
                else if( code == BC_META_START )
                {
                    state = IN_META;
                    break;
                }
                else if( code == BC_ESCAPE )
                {
                    state = IN_ESCAPE;
                    break;
                }

                // Uncompress move
                std::string s;
                Army* side = cr.white ? &sides[0] : &sides[1];
                Army* other = cr.white ? &sides[1] : &sides[0];
                bool have_san_move = false;
                size_t san_move_len = 0;
                std::string san_move;
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
                if( cr.white || need_move_number )
                {
                    char buf[40];
                    _itoa(stk->move_nbr, buf, 10);
                    std::string t(buf);
                    t += (cr.white ? "." : "...");
                    s += t;
                }
                need_move_number = false;
                if( !cr.white )
                    stk->move_nbr++;

                std::string t = have_san_move ? san_move : stk->mv.NaturalOut(&cr);
                cr.PlayMove(stk->mv);
                stk->variation_move_count++;

                // Check, is it mate ? [we did have a check for last move - but that was much easier if it's
                //  a bare sequence though, rather than this, the general case]
                if( have_san_move && san_move[san_move_len-1]=='+' )
                {
                    thc::TERMINAL score_terminal;
                    cr.Evaluate( score_terminal );
                    if( score_terminal == thc::TERMINAL_WCHECKMATE || score_terminal == thc::TERMINAL_BCHECKMATE )
                        t[san_move_len-1] = '#';
                }

                s += ' ';
                s += t;
                GameViewElement gve;
                if( stk->variation_move_count == 0 )
                {
                    gve.type = MOVE0;
                    gve.level   = stk_idx;
                    gve.mv = stk->mv;
                    gve.offset1 = offset;
                    gve.offset2 = offset;
                    expansion.push_back(gve);
                }
                gve.type = MOVE;
                gve.level   = stk_idx;
                gve.mv = stk->mv;
                gve.str  = s;
                gve.offset1 = offset;
                offset += s.length();
                gve.offset2 = offset;
                expansion.push_back(gve);
                break;
            }
        }
    }
    GameViewElement gve;
    gve.type    = END_OF_GAME;
    gve.level   = stk_idx;
    gve.str     = " ";
    gve.str     += result;
    gve.offset1 = offset;
    offset      += gve.str.length();
    gve.offset2 = offset;
    expansion.push_back(gve);
}

char Bytecode::CompressMove( thc::Move mv )
{
    Army *side  = cr.white ? &sides[0] : &sides[1];
    Army *other = cr.white ? &sides[1] : &sides[0];
    char c;
    if( side->fast_mode )
    {
        c = CompressFastMode(mv,side,other);
    }
    else if( TryFastMode(side) )
    {
        c = CompressFastMode(mv,side,other);
    }
    else
    {
        c = CompressSlowMode(mv);
        other->fast_mode = false;   // force other side to reset and retry
    }
    cr.PlayMove(mv);
    return c;
}

thc::Move Bytecode::UncompressMove( char c )
{
    Army *side  = cr.white ? &sides[0] : &sides[1];
    Army *other = cr.white ? &sides[1] : &sides[0];
    thc::Move mv;
    if( side->fast_mode )
    {
        mv = UncompressFastMode(c,side,other);
    }
    else if( TryFastMode(side) )
    {
        mv = UncompressFastMode(c,side,other);
    }
    else
    {
        mv = UncompressSlowMode(c);
        other->fast_mode = false;   // force other side to reset and retry
    }
    cr.PlayMove(mv);
    return mv;
}

// A slow method of compressing a move into one byte
//  Scheme is;
//  1) Make a list of all legal moves in UCI text format, sorted alphabetically
//  2) Find the move to be compressed in the list
//  3) Encode the move as one byte, value is 255 - list index (to avoid code 0, '\n' etc)
//
//  Why use slow text representation and sorting ? So that a specification for the
//  whole compression scheme can be published if necessary, without relying on implementation
//  details like the order that moves come out of the move generator etc.
//
//  Note that we use the ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES algorithm when
//  generating the list of moves
//
//  According to http://www.chess.com/forum/view/fun-with-chess/what-chess-position-has-the-most-number-of-possible-moves
//  the maximum number of legal moves available in any position is less than 256,
//  in fact it is 218 in the following position;
//
//  "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1"
//

char Bytecode::CompressSlowMode( thc::Move mv )
{
    DIAG_ONLY( nbr_compress_slow++ );
    int code=0;

    // Support algorithm ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES
    cr.wking = 1;
    cr.wqueen = 1;
    cr.bking = 1;
    cr.bqueen = 1;

    // Generate a list of all legal moves, in string form, sorted
    std::vector<thc::Move> moves;
    cr.GenLegalMoveList( moves );
    std::vector<std::string> moves_alpha;
    size_t len = moves.size();
    for( size_t i=0; i<len; i++ )
    {
        #define TERSE
        #ifdef TERSE
        std::string s = moves[i].TerseOut();
        #else
        std::string s = moves[i].NaturalOut(&cr);
        #endif
        moves_alpha.push_back(s);
    }
    std::sort( moves_alpha.begin(), moves_alpha.end() );

    // Find this move in the list
    #ifdef TERSE
    std::string the_move = mv.TerseOut();
    #else
    std::string the_move = mv.NaturalOut(&cr);
    #endif
    size_t idx=256;  // so that error if not found
    for( size_t i=0; i<len; i++ )
    {
        if( moves_alpha[i] == the_move )
        {
            idx = i;
            break;
        }
    }

    // char generated is '\xff' for first move in list, '\fe' for second etc
    // '\x01' is an error
    code = 255-idx;
    if( code < 8 )
        code = 8;
    DIAG_ONLY( nbr_slow_moves++; )
    return static_cast<char>(code);
}


char Bytecode::CompressFastMode( thc::Move mv, Army *side, Army *other )
{
    DIAG_ONLY( nbr_compress_fast++ );
    int src = static_cast<int>(mv.src);
    int dst = static_cast<int>(mv.dst);
    int capture_location = dst;
    char piece = cr.squares[mv.src];
    bool making_capture = (isalpha(cr.squares[capture_location]) ? true : false);
    int  code=0;
    switch( tolower(piece) )
    {
        case 'k':
        {
            side->king = dst;
            switch(dst-src)          // 0, 1, 2
            {                        // 8, 9, 10
                                     // 16,17,18
                case -9:    code = CODE_KING + K_VECTOR_NW;    break;  // 0-9
                case -8:    code = CODE_KING + K_VECTOR_N;     break;  // 1-9
                case -7:    code = CODE_KING + K_VECTOR_NE;    break;  // 2-9
                case -1:    code = CODE_KING + K_VECTOR_W;     break;  // 8-9
                case 1:     code = CODE_KING + K_VECTOR_E;     break;  // 10-9
                case 7:     code = CODE_KING + K_VECTOR_SW;    break;  // 16-9
                case 8:     code = CODE_KING + K_VECTOR_S;     break;  // 17-9
                case 9:     code = CODE_KING + K_VECTOR_SE;    break;  // 18-9
                case 2:
                {
                    code = CODE_KING + (side->white ? K_VECTOR_SE : K_VECTOR_NE);     // Kingside castling
                    int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                    side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                    // note that there is no way the rooks ordering can swap during castling
                    DIAG_ONLY( if( side->nbr_rooks==2 && pawn_ordering[side->rooks[0]]>pawn_ordering[side->rooks[1]] ) nbr_rook_swaps_alt++; ) // but alt swap possible
                    DIAG_ONLY( nbr_rook_moves++; )
                    break;
                }
                case -2:
                {
                    code = CODE_KING + (side->white ? K_VECTOR_SW : K_VECTOR_NW);     // Queenside castling
                    int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                    side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                    // note that there is no way the rooks ordering can swap during castling
                    DIAG_ONLY( if( side->nbr_rooks==2 && pawn_ordering[side->rooks[0]]>pawn_ordering[side->rooks[1]] ) nbr_rook_swaps_alt++; ) // but alt swap possible
                    DIAG_ONLY( nbr_rook_moves++; )
                    break;
                }
            }
            break;
        }

        case 'r':
        {
            DIAG_ONLY( nbr_rook_moves++; )
            int rook_offset = (side->rooks[0]==src ? 0 : 1);
            code = (rook_offset==0 ? CODE_ROOK_LO : CODE_ROOK_HI);
            side->rooks[rook_offset] = dst;
            if( (src&7) == (dst&7) )                // if same file
            {
                code = code + R_RANK + ((dst>>3)&7);    // encode rank
            }
            else //if( (src&0x38) == (dst&0x38) )   // if same rank
            {
                code = code + R_FILE + (dst&7);         // encode file
            }

            // swap ?
            if( side->nbr_rooks==2 && side->rooks[0]>side->rooks[1] )
            {
                int temp = side->rooks[0];
                side->rooks[0] = side->rooks[1];
                side->rooks[1] = temp;
                DIAG_ONLY( nbr_rook_swaps++; )
            }
            DIAG_ONLY( if( side->nbr_rooks==2 && pawn_ordering[side->rooks[0]]>pawn_ordering[side->rooks[1]] ) nbr_rook_swaps_alt++; )
            break;
        }

        case 'b':
        {
            if( is_dark(src) )
            {
                code = CODE_BISHOP_DARK;
                side->bishop_dark = dst;
            }
            else
            {
                code = CODE_BISHOP_LIGHT;
                side->bishop_light = dst;
            }
            int abs = (src>dst ? src-dst : dst-src);
            if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
                code = code + B_FALL + (dst&7); // fall( = \) + dst file
            else
                code = code + B_RISE + (dst&7); // rise( = /) + dst file
            break;
        }

        case 'q':
        {
            int queen_offset = (side->queens[0]==src ? 0 : 1);
            int base = (queen_offset==0 ? CODE_QUEEN_ROOK : CODE_PAWN_6 );
            side->queens[queen_offset] = dst;
            if( (src&7) == (dst&7) )                        // if same file
            {
                code = base + R_RANK + ((dst>>3)&7); // encode rank
            }
            else if( (src&0x38) == (dst&0x38) )             // if same rank
            {
                code = base + R_FILE + (dst&7);      // encode file
            }
            else
            {
                int abs = (src>dst ? src-dst : dst-src);
                if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
                    code = base + (CODE_QUEEN_BISHOP-CODE_QUEEN_ROOK) + B_FALL + (dst&7); // fall( = \) + dst file
                else
                    code = base + (CODE_QUEEN_BISHOP-CODE_QUEEN_ROOK) + B_RISE + (dst&7); // rise( = /) + dst file
            }

            // swap ?
            if( side->nbr_queens==2 && side->queens[0]>side->queens[1] )
            {
                int temp = side->queens[0];
                side->queens[0] = side->queens[1];
                side->queens[1] = temp;
                DIAG_ONLY( nbr_queen_swaps++; )
            }
            break;
        }

        case 'n':
        {
            DIAG_ONLY( nbr_knight_moves++; )
            int knight_offset = (side->knights[0]==src ? 0 : 1);
            code = (knight_offset==0 ? CODE_KNIGHT+N_LO : CODE_KNIGHT+N_HI );
            side->knights[knight_offset] = dst;
            switch(dst-src)         // 0, 1, 2, 3
            {                       // 8, 9, 10,11
                                    // 16,17,18,19
                                    // 24,25,26,27
                case -15:   code = code + N_VECTOR_NNE;   break; // 2-17
                case -6:    code = code + N_VECTOR_NEE;   break; // 11-17
                case 10:    code = code + N_VECTOR_SEE;   break; // 27-17
                case 17:    code = code + N_VECTOR_SSE;   break; // 27-10
                case 15:    code = code + N_VECTOR_SSW;   break; // 25-10
                case 6:     code = code + N_VECTOR_SWW;   break; // 16-10
                case -10:   code = code + N_VECTOR_NWW;   break; // 0-10
                case -17:   code = code + N_VECTOR_NNW;   break; // 0-17
            }
            // swap ?
            if( side->nbr_knights==2 && side->knights[0]>side->knights[1] )
            {
                int temp = side->knights[0];
                side->knights[0] = side->knights[1];
                side->knights[1] = temp;
                DIAG_ONLY( nbr_knight_swaps++; )
            }
            DIAG_ONLY( if( side->nbr_knights==2 && pawn_ordering[side->knights[0]]>pawn_ordering[side->knights[1]] ) nbr_knight_swaps_alt++; )
            break;
        }

        case 'p':
        {
            int positive_delta = cr.white ? src-dst : dst-src;
            bool promoting = false;
            int pawn_offset = 0;
            int *p = side->pawns;
            for( int i=0; i<side->nbr_pawns; i++,p++ )
            {
                if( *p == src )
                {
                    pawn_offset = i;
                    *p = dst;
                    break;
                }
            }
            switch( positive_delta )
            {
                case 16: code = P_DOUBLE;  break;
                case 8:  code = P_SINGLE;  break;
                case 9:  code = P_LEFT;    break;
                case 7:  code = P_RIGHT;   break;
            }
            switch( mv.special )
            {
                case thc::SPECIAL_WEN_PASSANT:
                    capture_location = dst+8;
                    making_capture = true;
                    break;
                case thc::SPECIAL_BEN_PASSANT:
                    capture_location = dst-8;
                    making_capture = true;
                    break;
                case thc::SPECIAL_PROMOTION_QUEEN:  code = (code<<2) + P_QUEEN;
                    promoting = true;
                    DIAG_ONLY( if(side->nbr_queens>0) is_interesting|=4096; )
                    break;
                case thc::SPECIAL_PROMOTION_ROOK:   code = (code<<2) + P_ROOK;
                    promoting = true;
                    break;
                case thc::SPECIAL_PROMOTION_BISHOP: code = (code<<2) + P_BISHOP;
                    promoting = true;
                    break;
                case thc::SPECIAL_PROMOTION_KNIGHT: code = (code<<2) + P_KNIGHT;
                    promoting = true;
                    break;
            }

            // If promoting piece, force a reset and retry next time for this side. This
            //  way we accommodate our pawn disappearing, and a new piece appearing in
            //  its place, and the possibility that we cannot remain in fast mode
            //  (because we now have too many queens or other pieces). If the reset
            //  and retry fails this side will generate slow mode moves but keep
            //  retrying until fast is possible again.
            if( promoting )
            {
                side->fast_mode = false;
                DIAG_ONLY( is_interesting |= 512; )
            }
            else
            {
                DIAG_ONLY( bool br_or_wl=false; int nbr_swaps=0; )
                bool reordering_possible = (code==P_LEFT || code==P_RIGHT);
                if( reordering_possible )
                {
                    if( pawn_ordering[dst] > pawn_ordering[src] ) // increasing capture?
                    {
                        for( int i=pawn_offset; i+1<side->nbr_pawns && pawn_ordering[side->pawns[i]]>pawn_ordering[side->pawns[i+1]]; i++ )
                        {
                            DIAG_ONLY( nbr_swaps++; )
                            int temp = side->pawns[i];
                            side->pawns[i] = side->pawns[i+1];
                            side->pawns[i+1] = temp;
                        }
                        if( cr.white )
                            { DIAG_ONLY( nbr_pawn_swaps_histo_wr[nbr_swaps]++; ) }
                        else
                            { DIAG_ONLY( br_or_wl=true; nbr_pawn_swaps_histo_br[nbr_swaps]++; ) }
                    }
                    else // else decreasing capture
                    {
                        for( int i=pawn_offset; i-1>=0 && pawn_ordering[side->pawns[i-1]]>pawn_ordering[side->pawns[i]]; i-- )
                        {
                            DIAG_ONLY( nbr_swaps++; )
                            int temp = side->pawns[i-1];
                            side->pawns[i-1] = side->pawns[i];
                            side->pawns[i] = temp;
                        }
                        if( cr.white )
                            { DIAG_ONLY( br_or_wl=true; nbr_pawn_swaps_histo_wl[nbr_swaps]++; ) }
                        else
                            { DIAG_ONLY( nbr_pawn_swaps_histo_bl[nbr_swaps]++; ) }
                    }
                }
                DIAG_ONLY( if( nbr_swaps>=3 && br_or_wl ) is_interesting|=32; )
                DIAG_ONLY( nbr_pawn_swaps_histo[nbr_swaps]++; )
            }
            code = CODE_PAWN + (pawn_offset<<4) + code;
            break;
        }   // end PAWN
    }   // end switch on moving piece

    // Accomodate captured piece on other side, if other side is in fast mode
    if( making_capture && other->fast_mode )
    {
        switch( tolower(cr.squares[capture_location]) )
        {
            case 'n':
            {
                if( other->nbr_knights==2 && other->knights[0]==capture_location )
                    other->knights[0] = other->knights[1];
                other->nbr_knights--;
                break;
            }
            case 'r':
            {
                if( other->nbr_rooks==2 && other->rooks[0]==capture_location )
                    other->rooks[0] = other->rooks[1];
                other->nbr_rooks--;
                break;
            }
            case 'b':
            {
                if( is_dark(capture_location) )
                    other->nbr_dark_bishops--;
                else
                    other->nbr_light_bishops--;
                break;
            }
            case 'q':
            {
                if( other->nbr_queens==2 && other->queens[0]==capture_location )
                    other->queens[0] = other->queens[1];
                other->nbr_queens--;
                break;
            }
            case 'p':
            {
                int other_pawn_offset=0;
                for( int i=0; i<other->nbr_pawns; i++ )
                {
                    if( other->pawns[i] == capture_location )
                    {
                        other_pawn_offset = i;
                        break;
                    }
                }
                for( int i=other_pawn_offset; i+1<other->nbr_pawns; i++ )
                {
                    other->pawns[i] = other->pawns[i+1];
                }
                other->nbr_pawns--;
                break;
            }
        }
    }
    return static_cast<char>(code);
}

thc::Move Bytecode::UncompressSlowMode( char code )
{
    DIAG_ONLY( nbr_uncompress_slow++ );

    // Support algorithm ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES
    cr.wking = 1;
    cr.wqueen = 1;
    cr.bking = 1;
    cr.bqueen = 1;

    // Generate a list of all legal moves, in string form, sorted
    std::vector<thc::Move> moves;
    cr.GenLegalMoveList( moves );
    std::vector<std::string> moves_alpha;

    // Coding scheme relies on 247 valid codes 0x09-0xff and one error code 0x08,
    size_t len = moves.size();
    for( size_t i=0; i<len; i++ )
    {
        #ifdef TERSE
        std::string s = moves[i].TerseOut();
        #else
        std::string s = moves[i].NaturalOut(&cr);
        #endif
        moves_alpha.push_back(s);
    }
    std::sort( moves_alpha.begin(), moves_alpha.end() );

    // '\xff' (i.e. 255) is first move in list, '\fe' (i.e. 254) is second etc
    unsigned int ucode = static_cast<size_t>( code );
    ucode &= 0xff;
    size_t idx = 255-ucode;  // 255->0, 254->1 etc.
    if( idx >= len )
        idx = 0;    // all errors resolve to this - take first move from list
    std::string the_move = moves_alpha[idx];
    thc::Move mv;
    #ifdef TERSE
    mv.TerseIn( &cr, the_move.c_str() );
    #else
    mv.NaturalInFast( &cr, the_move.c_str() );
    #endif
    return mv;
}

thc::Move Bytecode::UncompressFastMode( char code, Army *side, Army *other )
{
    DIAG_ONLY( nbr_uncompress_fast++ );
    int src=0;
    int dst=0;
    int hi_nibble = code&0xf0;
    thc::SPECIAL special = thc::NOT_SPECIAL;
    switch( hi_nibble )
    {
        case CODE_KING:
        {
            special = thc::SPECIAL_KING_MOVE;
            src = side->king;
            int delta=0;
            if( side->white )
            {
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case K_VECTOR_NW:    delta = -9; break;  // 0-9
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_NE:    delta = -7; break;  // 2-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:
                    {
                        if( src != thc::e1 )
                            delta =  9; // 18-9  normal SE king move   
                        else
                        {
                            // White kingside castling is encoded as K_VECTOR_SE
                            special = thc::SPECIAL_WK_CASTLING;
                            delta = 2;
                            int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                            side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                            // note that there is no way the rooks ordering can swap during castling
                        }
                        break;
                    }
                    case K_VECTOR_SW:
                    {
                        if( src != thc::e1 )
                            delta =  7; // 16-9  normal SE king move   
                        else
                        {
                            // White queenside castling is encoded as K_VECTOR_SW
                            special = thc::SPECIAL_WQ_CASTLING;
                            delta = -2;
                            int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                            side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                            // note that there is no way the rooks ordering can swap during castling
                        }
                        break;
                    }
                }
            }
            else
            {
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_SW:    delta =  7; break;  // 16-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:    delta =  9; break;  // 18-9
                    case K_VECTOR_NE:
                    {
                        if( src != thc::e8 )
                            delta =  -7; // 2-9  normal NE king move   
                        else
                        {
                            // Black kingside castling is encoded as K_VECTOR_NE
                            special = thc::SPECIAL_BK_CASTLING;
                            delta = 2;
                            int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                            side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                            // note that there is no way the rooks ordering can swap during castling
                        }
                        break;
                    }
                    case K_VECTOR_NW:    
                    {
                        if( src != thc::e8 )
                            delta = -9;  // 0-9  normal NW king move   
                        else
                        {
                            // Black queenside castling is encoded as K_VECTOR_NW
                            special = thc::SPECIAL_BQ_CASTLING;
                            delta = -2;
                            int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                            side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                            // note that there is no way the rooks ordering can swap during castling
                        }
                        break;
                    }
                }
            }
            side->king = dst = src+delta;
            break;
        }

        case CODE_ROOK_LO:
        case CODE_ROOK_HI:
        {
            int rook_offset = (hi_nibble==CODE_ROOK_LO ? 0 : 1 );
            src = side->rooks[rook_offset];
            if( code & R_RANK )                // code encodes rank ?
            {
                dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
            }
            else
            {
                dst = (src&0x38) | (code&7);        // same rank as src, file from code
            }
            side->rooks[rook_offset] = dst;

            // swap ?
            if( side->nbr_rooks==2 && side->rooks[0]>side->rooks[1] )
            {
                int temp = side->rooks[0];
                side->rooks[0] = side->rooks[1];
                side->rooks[1] = temp;
            }
            break;
        }

        case CODE_BISHOP_DARK:
        {
            src = side->bishop_dark;
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=b8(1), dst=h2(55), file_delta=6  -> 9*6 =54
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
            side->bishop_dark = dst;
            break;
        }

        case CODE_BISHOP_LIGHT:
        {
            src = side->bishop_light;
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=g8(6), dst=a2(48), file_delta=6  -> 7*6 =42
            side->bishop_light = dst;
            break;
        }

        case CODE_QUEEN_ROOK:
        {
            src = side->queens[0];
            if( code & R_RANK )                // code encodes rank ?
                dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
            else
                dst = (src&0x38) | (code&7);        // same rank as src, file from code
            side->queens[0] = dst;

            // swap ?
            if( side->nbr_queens==2 && side->queens[0]>side->queens[1] )
            {
                int temp = side->queens[0];
                side->queens[0] = side->queens[1];
                side->queens[1] = temp;
            }
            break;
        }

        case CODE_QUEEN_BISHOP:
        {
            src = side->queens[0];
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
            side->queens[0] = dst;

            // swap ?
            if( side->nbr_queens==2 && side->queens[0]>side->queens[1] )
            {
                int temp = side->queens[0];
                side->queens[0] = side->queens[1];
                side->queens[1] = temp;
            }
            break;
        }

        case CODE_KNIGHT:
        {
            int knight_offset = ((code&N_HI) ? 1 : 0 );
            src = side->knights[knight_offset];
            int delta=0;
            switch(code&7)          // 0, 1, 2, 3
            {                       // 8, 9, 10,11
                                    // 16,17,18,19
                                    // 24,25,26,27
                case N_VECTOR_NNE:   delta = -15;   break; // 2-17
                case N_VECTOR_NEE:   delta = -6;    break; // 11-17
                case N_VECTOR_SEE:   delta = 10;    break; // 27-17
                case N_VECTOR_SSE:   delta = 17;    break; // 27-10
                case N_VECTOR_SSW:   delta = 15;    break; // 25-10
                case N_VECTOR_SWW:   delta = 6;     break; // 16-10
                case N_VECTOR_NWW:   delta = -10;   break; // 0-10
                case N_VECTOR_NNW:   delta = -17;   break; // 0-17
            }
            dst = src+delta;
            side->knights[knight_offset] = dst;

            // swap ?
            if( side->nbr_knights==2 && side->knights[0]>side->knights[1] )
            {
                int temp = side->knights[0];
                side->knights[0] = side->knights[1];
                side->knights[1] = temp;
            }
            break;
        }

        case CODE_PAWN_6:   // if nbr_queens==2 this is the hi QUEEN_ROOK
        {
            if( side->nbr_queens < 2 )
                ;   // fall through
            else
            {
                src = side->queens[1];
                if( code & R_RANK )                // code encodes rank ?
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                else
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                side->queens[1] = dst;

                // swap ?
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }
                break;
            }
        }

        case CODE_PAWN_7:   // if nbr_queens==2 this is the hi QUEEN_BISHOP:
        {
            if( side->nbr_queens < 2 )
                ;   // fall through
            else
            {
                src = side->queens[1];
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                side->queens[1] = dst;

                // swap ?
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }
                break;
            }
        }

        // PAWN
        default:
        {
            int pawn_offset = (code>>4)&0x07;
            bool promoting = false;
            src = side->pawns[pawn_offset];
            bool reordering_possible = false;
            bool white = cr.white;
            int delta=0;
            switch( code&0x0f )
            {
                case P_DOUBLE:      special = white ? thc::SPECIAL_WPAWN_2SQUARES : thc::SPECIAL_BPAWN_2SQUARES;
                                    delta = white?-16:16; break;
                case P_SINGLE:      delta = white?-8:8;   break;
                case P_LEFT:
                {
                    reordering_possible = true;
                    delta = white?-9:9;
                    if( !isalpha(cr.squares[src+delta]) )
                        special = (white ? thc::SPECIAL_WEN_PASSANT : thc::SPECIAL_BEN_PASSANT);
                    break;
                }
                case P_RIGHT:
                {
                    reordering_possible = true;
                    delta = white?-7:7;
                    if( !isalpha(cr.squares[src+delta]) )
                        special = (white ? thc::SPECIAL_WEN_PASSANT : thc::SPECIAL_BEN_PASSANT);
                    break;
                }
                default:
                {
                    promoting = true;
                    switch( (code>>2)&3 )
                    {
                        case P_SINGLE:     delta = white?-8:8;   break;
                        case P_LEFT:       delta = white?-9:9;   break;
                        case P_RIGHT:      delta = white?-7:7;   break;
                    }
                    switch( code&3 )
                    {
                        case P_QUEEN:      special = thc::SPECIAL_PROMOTION_QUEEN;    break;
                        case P_ROOK:       special = thc::SPECIAL_PROMOTION_ROOK;     break;
                        case P_BISHOP:     special = thc::SPECIAL_PROMOTION_BISHOP;   break;
                        case P_KNIGHT:     special = thc::SPECIAL_PROMOTION_KNIGHT;   break;
                    }
                    break;
                }
            }
            side->pawns[pawn_offset] = dst = src+delta;

            // If promoting piece, force a reset and retry next time for this side. This
            //  way we accommodate our pawn disappearing, and a new piece appearing in
            //  its place, and the possibility that we cannot remain in fast mode
            //  (because we now have too many queens or other pieces). If the reset
            //  and retry fails this side will generate slow mode moves but keep
            //  retrying until fast is possible again.
            if( promoting )
                side->fast_mode = false;
            else if( reordering_possible )
            {
                if( pawn_ordering[dst] > pawn_ordering[src] ) // increasing capture?
                {
                    for( int i=pawn_offset; i+1<side->nbr_pawns && pawn_ordering[side->pawns[i]]>pawn_ordering[side->pawns[i+1]]; i++ )
                    {
                        int temp = side->pawns[i];
                        side->pawns[i] = side->pawns[i+1];
                        side->pawns[i+1] = temp;
                    }
                }
                else // else decreasing capture
                {
                    for( int i=pawn_offset; i-1>=0 && pawn_ordering[side->pawns[i-1]]>pawn_ordering[side->pawns[i]]; i-- )
                    {
                        int temp = side->pawns[i-1];
                        side->pawns[i-1] = side->pawns[i];
                        side->pawns[i] = temp;
                    }
                }
            }
            break;
        }   // end PAWN
    }   // end switch on moving piece

    // Accomodate captured piece on other side, if other side is in fast mode
    int  capture_location=dst;
    if( special == thc::SPECIAL_WEN_PASSANT )
        capture_location = dst+8;
    else if( special == thc::SPECIAL_BEN_PASSANT )
        capture_location = dst-8;
    char capture = cr.squares[capture_location];
    bool making_capture = (isalpha(capture) ? true : false);
    if( making_capture && other->fast_mode )
    {
        switch( tolower(capture) )
        {
            case 'n':
            {
                if( other->nbr_knights==2 && other->knights[0]==capture_location )
                    other->knights[0] = other->knights[1];
                other->nbr_knights--;
                break;
            }
            case 'r':
            {
                if( other->nbr_rooks==2 && other->rooks[0]==capture_location )
                    other->rooks[0] = other->rooks[1];
                other->nbr_rooks--;
                break;
            }
            case 'b':
            {
                if( is_dark(capture_location) )
                    other->nbr_dark_bishops--;
                else
                    other->nbr_light_bishops--;
                break;
            }
            case 'q':
            {
                if( other->nbr_queens==2 && other->queens[0]==capture_location )
                    other->queens[0] = other->queens[1];
                other->nbr_queens--;
                break;
            }
            case 'p':
            {
                int other_pawn_offset=0;
                for( int i=0; i<other->nbr_pawns; i++ )
                {
                    if( other->pawns[i] == capture_location )
                    {
                        other_pawn_offset = i;
                        break;
                    }
                }
                for( int i=other_pawn_offset; i+1<other->nbr_pawns; i++ )
                {
                    other->pawns[i] = other->pawns[i+1];
                }
                other->nbr_pawns--;
            }
        }
    }
    thc::Move mv;
    mv.src = static_cast<thc::Square>(src);
    mv.dst = static_cast<thc::Square>(dst);
    mv.special = special;
    mv.capture = capture;
    return mv;
}

namespace thc
{
    extern const lte *knight_lookup[];
}

// Set flag if piece on 'from' attacks a piece on 'to', along offset_type ray
#define ATTACKS( flag, from, to, offset_type )    \
    RayValues *r1 = &ray_lookup_table[from][to];  \
    flag = (r1->offset_type != 0);                \
    for( int sq=to+r1->offset_type; flag && sq!=from; sq+=r1->offset_type )  \
    {                                             \
        if( cr.squares[sq] != ' ' )               \
            flag = false;                         \
    }

// Set flag if a move from src to dst exposes the king on king_sq to attack along offset_type ray
#define DISCOVERY_CHECK( flag, white_move, src, dst, king_sq, offset_type )  \
    /* Look from king position to edge for a queen/rook/bishop on the newly vacated */ \
    /*  rank, file or diagonal. There must be at least two steps to the edge,       */ \
    /*  the square we are vacating cannot be the edge square                        */ \
    RayValues *r2 = &ray_lookup_table[src][king_sq];                                   \
    if( r2->offset_type!=0 && r2->ray_count>1 && src!=r2->ray_square )                 \
    { \
        char queen              = 'Q' + (white_move ? 0 : ' '); \
        char rook_or_bishop     = (r2->ray_rook_offset ? 'R' :  'B') + (white_move ? 0 : ' ');          \
        for( int i=0, sq = king_sq+r2->offset_type;  sq!=dst && i<r2->ray_count; i++, sq+=r2->offset_type )  \
        {                          \
            if( cr.squares[sq] == queen || cr.squares[sq] == rook_or_bishop )                           \
            {                                                                                           \
                flag = true;   /* if we haven't reached the src square yet that's strange because */    \
                               /*  the opponent was in check before we made the move              */    \
                break;                                                                                  \
            }                                                                                           \
            if( sq!=src && cr.squares[sq] != ' ' )  /* check for any interruption */                    \
                break;  \
        }               \
    }


thc::Move Bytecode::UncompressFastMode( char code, Army *side, Army *other, std::string &san_move )
{
    DIAG_ONLY( nbr_uncompress_fast++ );
    int src=0;
    int dst=0;
    int hi_nibble = code&0xf0;
    san_move.clear();
    thc::SPECIAL special = thc::NOT_SPECIAL;
    switch( hi_nibble )
    {
        case CODE_KING:
        {
            special = thc::SPECIAL_KING_MOVE;
            src = side->king;
            int delta=0;
            bool castling = false;
            int attack_sq=0;
            if( side->white )
            {
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case K_VECTOR_NW:    delta = -9; break;  // 0-9
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_NE:    delta = -7; break;  // 2-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:
                    {
                        if( src != thc::e1 )
                            delta =  9; // 18-9  normal SE king move   
                        else
                        {
                            // White kingside castling is encoded as K_VECTOR_SE
                            special = thc::SPECIAL_WK_CASTLING;
                            delta = 2;
                            int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                            side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                            // note that there is no way the rooks ordering can swap during castling
                            attack_sq = src+1;
                            san_move = "O-O";
                            castling = true;
                        }
                        break;
                    }
                    case K_VECTOR_SW:
                    {
                        if( src != thc::e1 )
                            delta =  7; // 16-9  normal SE king move   
                        else
                        {
                            // White queenside castling is encoded as K_VECTOR_SW
                            special = thc::SPECIAL_WQ_CASTLING;
                            delta = -2;
                            int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                            side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                            // note that there is no way the rooks ordering can swap during castling
                            attack_sq = src-1;
                            san_move = "O-O-O";
                            castling = true;
                        }
                        break;
                    }
                }
            }
            else
            {
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_SW:    delta =  7; break;  // 16-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:    delta =  9; break;  // 18-9
                    case K_VECTOR_NE:
                    {
                        if( src != thc::e8 )
                            delta =  -7; // 2-9  normal NE king move   
                        else
                        {
                            // Black kingside castling is encoded as K_VECTOR_NE
                            special = thc::SPECIAL_BK_CASTLING;
                            delta = 2;
                            int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                            side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                            // note that there is no way the rooks ordering can swap during castling
                            attack_sq = src+1;
                            san_move = "O-O";
                            castling = true;
                        }
                        break;
                    }
                    case K_VECTOR_NW:    
                    {
                        if( src != thc::e8 )
                            delta = -9;  // 0-9  normal NW king move   
                        else
                        {
                            // Black queenside castling is encoded as K_VECTOR_NW
                            special = thc::SPECIAL_BQ_CASTLING;
                            delta = -2;
                            int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                            side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                            // note that there is no way the rooks ordering can swap during castling
                            attack_sq = src-1;
                            san_move = "O-O-O";
                            castling = true;
                        }
                        break;
                    }
                }
            }
            side->king = dst = src+delta;

            // Test whether the King is giving check directly (castling only) and/or discovering an attack by a Queen, Rook or Bishop
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king

            // Direct check (castling only) ?
            if( castling )
            {
                // Direct check ?
                ATTACKS( check, attack_sq, king_sq, ray_rook_offset )
            }

            // Discovery (not castling only) ?
            else
            {
                // Discovered check on any ray (not necessarily) vacated by the king?
                DISCOVERY_CHECK( check, cr.white, src, dst, king_sq, ray_offset )
            }
            char f = get_file((thc::Square)dst);
            char r = get_rank((thc::Square)dst);
            if( !castling )
            {
                san_move = "K";
                if( cr.squares[dst] != ' ')
                    san_move += 'x';
                san_move += f;
                san_move += r;
            }
            if( check )
                san_move += '+';
            break;
        }

        case CODE_ROOK_LO:
        case CODE_ROOK_HI:
        {
            bool ambig = false;
            int rook_offset = (hi_nibble==CODE_ROOK_LO ? 0 : 1 );
            src = side->rooks[rook_offset];
            if( code & R_RANK )                // code encodes rank ?
            {
                dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
            }
            else
            {
                dst = (src&0x38) | (code&7);        // same rank as src, file from code
            }
            side->rooks[rook_offset] = dst;
            int other_rook_ambig = 0;
            if( side->nbr_rooks==2 )
            {
                int other_rook_offset = (hi_nibble==CODE_ROOK_LO ? 1 : 0 );
                int other_rook = other_rook_ambig = side->rooks[other_rook_offset];

                // Set ambig flag if the other rook can go to ('attacks') the dst square too
                ATTACKS( ambig, other_rook, dst, ray_rook_offset )
                if( ambig )
                {
                    int our_king_sq = cr.white ? cr.wking_square : cr.bking_square;
                    bool exposes_our_king=false;

                    // Does the other rook moving to the same dst expose our king to check along any ray ?
                    DISCOVERY_CHECK( exposes_our_king, !cr.white, other_rook_ambig, dst, our_king_sq, ray_offset )
                    if( exposes_our_king )
                        ambig = false;  // if so it's an illegal move, so no need to disambiguate
                }

                // swap ?
                if( side->rooks[0] > side->rooks[1] )
                {
                    int temp = side->rooks[0];
                    side->rooks[0] = side->rooks[1];
                    side->rooks[1] = temp;
                }

            }

            // Test whether the Rook is giving check directly and/or discovering an attack by a Queen or Bishop
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king

            // Direct check ?
            ATTACKS( check, dst, king_sq, ray_rook_offset )

            // Discovery ?
            if( !check )    // no need to test for discovery if already found direct attack
            {

                // Discovered check on a bishop ray vacated by the rook?
                DISCOVERY_CHECK( check, cr.white, src, dst, king_sq, ray_bishop_offset )
            }
            char f = get_file((thc::Square)dst);
            char r = get_rank((thc::Square)dst);
            san_move = "R";
            if( ambig )
            {
                char f_src = get_file((thc::Square)src);
                char r_src = get_rank((thc::Square)src);
                char f_other = get_file((thc::Square)other_rook_ambig);
                if( f_src != f_other )
                    san_move += f_src;
                else
                    san_move += r_src;
            }
            if( cr.squares[dst] != ' ')
                san_move += 'x';
            san_move += f;
            san_move += r;
            if( check )
                san_move += '+';
            break;
        }

        case CODE_BISHOP_DARK:
        {
            src = side->bishop_dark;
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=b8(1), dst=h2(55), file_delta=6  -> 9*6 =54
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
            side->bishop_dark = dst;

            // Test whether the Bishop is giving check directly and/or discovering an attack by a Queen or Rook
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king

            // Direct check ?
            ATTACKS( check, dst, king_sq, ray_bishop_offset )

            // Discovery ?
            if( !check )    // no need to test for discovery if already found direct attack
            {

                // Discovered check on a rook ray vacated by the bishop?
                DISCOVERY_CHECK( check, cr.white, src, dst, king_sq, ray_rook_offset )
            }
            char f = get_file((thc::Square)dst);
            char r = get_rank((thc::Square)dst);
            san_move = "B";
            if( cr.squares[dst] != ' ')
                san_move += 'x';
            san_move += f;
            san_move += r;
            if( check )
                san_move += '+';
            break;
        }

        case CODE_BISHOP_LIGHT:
        {
            src = side->bishop_light;
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=g8(6), dst=a2(48), file_delta=6  -> 7*6 =42
            side->bishop_light = dst;

            // Test whether the Bishop is giving check directly and/or discovering an attack by a Queen or Rook
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king

            // Direct check ?
            ATTACKS( check, dst, king_sq, ray_bishop_offset )

            // Discovery ?
            if( !check )    // no need to test for discovery if already found direct attack
            {

                // Discovered check on a rook ray vacated by the bishop?
                DISCOVERY_CHECK( check, cr.white, src, dst, king_sq, ray_rook_offset )
            }

            char f = get_file((thc::Square)dst);
            char r = get_rank((thc::Square)dst);
            san_move = "B";
            if( cr.squares[dst] != ' ')
                san_move += 'x';
            san_move += f;
            san_move += r;
            if( check )
                san_move += '+';
            break;
        }

        case CODE_QUEEN_ROOK:
        {
            src = side->queens[0];
            if( code & R_RANK )                // code encodes rank ?
                dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
            else
                dst = (src&0x38) | (code&7);        // same rank as src, file from code
            side->queens[0] = dst;

            // swap ?
            if( side->nbr_queens == 2 )
            {
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }
                break;  // Don't attempt SAN calculation - ambiguity is too tricky
            }

            // Test whether the Queen is giving check (directly - queens can't discover check)
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king
            ATTACKS( check, dst, king_sq, ray_offset )

            char f = get_file((thc::Square)dst);
            char r = get_rank((thc::Square)dst);
            san_move = "Q";
            if( cr.squares[dst] != ' ')
                san_move += 'x';
            san_move += f;
            san_move += r;
            if( check )
                san_move += '+';
            break;
        }

        case CODE_QUEEN_BISHOP:
        {
            src = side->queens[0];
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
            side->queens[0] = dst;

            // swap ?
            if( side->nbr_queens == 2 )
            {
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }
                break;  // Don't attempt SAN calculation - ambiguity is too tricky
            }

            // Test whether the Queen is giving check (directly - queens can't discover check)
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king
            ATTACKS( check, dst, king_sq, ray_offset )

            char f = get_file((thc::Square)dst);
            char r = get_rank((thc::Square)dst);
            san_move = "Q";
            if( cr.squares[dst] != ' ')
                san_move += 'x';
            san_move += f;
            san_move += r;
            if( check )
                san_move += '+';
            break;
        }

        case CODE_KNIGHT:
        {
            bool ambig = false;
            int knight_offset = ((code&N_HI) ? 1 : 0 );
            src = side->knights[knight_offset];
            int delta=0;
            switch(code&7)          // 0, 1, 2, 3
            {                       // 8, 9, 10,11
                                    // 16,17,18,19
                                    // 24,25,26,27
                case N_VECTOR_NNE:   delta = -15;   break; // 2-17
                case N_VECTOR_NEE:   delta = -6;    break; // 11-17
                case N_VECTOR_SEE:   delta = 10;    break; // 27-17
                case N_VECTOR_SSE:   delta = 17;    break; // 27-10
                case N_VECTOR_SSW:   delta = 15;    break; // 25-10
                case N_VECTOR_SWW:   delta = 6;     break; // 16-10
                case N_VECTOR_NWW:   delta = -10;   break; // 0-10
                case N_VECTOR_NNW:   delta = -17;   break; // 0-17
            }
            dst = src+delta;
            side->knights[knight_offset] = dst;

            int other_knight=0;
            if( side->nbr_knights==2 )
            {
                // Set ambig flag if the other king can go to ('attacks') the dst square too
                int other_offset = ((code&N_HI) ? 0 : 1 );
                other_knight = side->knights[other_offset];
                const unsigned char *ptr = thc::knight_lookup[dst];
                unsigned char nbr_moves = *ptr++;
                while( nbr_moves-- )
                {
                    unsigned char sq = *ptr++;
                    if( sq == (unsigned char)other_knight )
                    {
                        ambig = true;
                        break;
                    }
                }
                if( ambig )
                {
                    int our_king_sq = cr.white ? cr.wking_square : cr.bking_square;
                    bool exposes_our_king=false;

                    // Does the other knight moving to the same dst expose our king to check along any ray ?
                    DISCOVERY_CHECK( exposes_our_king, !cr.white, other_knight, dst, our_king_sq, ray_offset )
                    if( exposes_our_king )
                        ambig = false;  // if so it's an illegal move, so no need to disambiguate
                }


                // swap ?
                if( side->knights[0]>side->knights[1] )
                {
                    int temp = side->knights[0];
                    side->knights[0] = side->knights[1];
                    side->knights[1] = temp;
                }
            }

            // Test whether the Knight is giving check directly and/or discovering an attack by a Queen, Rook or Bishop
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king

            // Direct check ?
            const unsigned char *ptr = thc::knight_lookup[king_sq];
            unsigned char nbr_moves = *ptr++;
            while( nbr_moves-- )
            {
                unsigned char sq = *ptr++;
                if( sq == (unsigned char)dst )
                {
                    check = true;
                    break;
                }
            }

            // Discovered check ?
            if( !check )    // no need to test for discovery if already found direct attack
            {

                // Discovered check on any ray vacated by the knight ?
                DISCOVERY_CHECK( check, cr.white, src, dst, king_sq, ray_offset )
            }

            char f = get_file((thc::Square)dst);
            char r = get_rank((thc::Square)dst);
            san_move = "N";
            if( ambig )
            {
                char f_src = get_file((thc::Square)src);
                char r_src = get_rank((thc::Square)src);
                char f_other = get_file((thc::Square)other_knight);
                if( f_src != f_other )
                    san_move += f_src;
                else
                    san_move += r_src;
            }
            if( cr.squares[dst] != ' ')
                san_move += 'x';
            san_move += f;
            san_move += r;
            if( check )
                san_move += '+';
            break;
        }

        case CODE_PAWN_6:   // if nbr_queens==2 this is the hi QUEEN_ROOK
        {
            if( side->nbr_queens < 2 )
                ;   // fall through
            else
            {
                src = side->queens[1];
                if( code & R_RANK )                // code encodes rank ?
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                else
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                side->queens[1] = dst;

                // swap ?
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }

                // Don't attempt SAN calculation, ambiguity is too tricky
                break;
            }
        }

        case CODE_PAWN_7:   // if nbr_queens==2 this is the hi QUEEN_BISHOP:
        {
            if( side->nbr_queens < 2 )
                ;   // fall through
            else
            {
                src = side->queens[1];
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                side->queens[1] = dst;

                // swap ?
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }

                // Don't attempt SAN calculation, ambiguity is too tricky
                break;
            }
        }

        // PAWN
        default:
        {
            int pawn_offset = (code>>4)&0x07;
            src = side->pawns[pawn_offset];
            bool reordering_possible = false;
            bool white = cr.white;
            int delta=0;
            char promotion_char = 0;
            bool enpassant = false;
            switch (code&0x0f)
            {
                case P_DOUBLE:      special = white ? thc::SPECIAL_WPAWN_2SQUARES : thc::SPECIAL_BPAWN_2SQUARES;
                    delta = white?-16:16; break;
                case P_SINGLE:      delta = white?-8:8;   break;
                case P_LEFT:
                {
                    reordering_possible = true;
                    delta = white?-9:9;
                    if(!isalpha(cr.squares[src+delta]))
                    {
                        special = (white ? thc::SPECIAL_WEN_PASSANT : thc::SPECIAL_BEN_PASSANT);
                        enpassant = true;
                    }
                    break;
                }
                case P_RIGHT:
                {
                    reordering_possible = true;
                    delta = white?-7:7;
                    if(!isalpha(cr.squares[src+delta]))
                    {
                        special = (white ? thc::SPECIAL_WEN_PASSANT : thc::SPECIAL_BEN_PASSANT);
                        enpassant = true;
                    }
                    break;
                }
                default:
                {
                    switch ((code>>2)&3)
                    {
                        case P_SINGLE:     delta = white?-8:8;   break;
                        case P_LEFT:       delta = white?-9:9;   break;
                        case P_RIGHT:      delta = white?-7:7;   break;
                    }
                    switch (code&3)
                    {
                        case P_QUEEN:      special = thc::SPECIAL_PROMOTION_QUEEN;    promotion_char='Q'; break;
                        case P_ROOK:       special = thc::SPECIAL_PROMOTION_ROOK;     promotion_char='R'; break;
                        case P_BISHOP:     special = thc::SPECIAL_PROMOTION_BISHOP;   promotion_char='B'; break;
                        case P_KNIGHT:     special = thc::SPECIAL_PROMOTION_KNIGHT;   promotion_char='N'; break;
                    }
                    break;
                }
            }
            side->pawns[pawn_offset] = dst = src+delta;

            // If promoting piece, force a reset and retry next time for this side. This
            //  way we accommodate our pawn disappearing, and a new piece appearing in
            //  its place, and the possibility that we cannot remain in fast mode
            //  (because we now have too many queens or other pieces). If the reset
            //  and retry fails this side will generate slow mode moves but keep
            //  retrying until fast is possible again.
            if( promotion_char )
                side->fast_mode = false;
            else if( reordering_possible )
            {
                if( pawn_ordering[dst] > pawn_ordering[src] ) // increasing capture?
                {
                    for( int i=pawn_offset; i+1<side->nbr_pawns && pawn_ordering[side->pawns[i]]>pawn_ordering[side->pawns[i+1]]; i++)
                    {
                        int temp = side->pawns[i];
                        side->pawns[i] = side->pawns[i+1];
                        side->pawns[i+1] = temp;
                    }
                }
                else // else decreasing capture
                {
                    for( int i=pawn_offset; i-1>=0 && pawn_ordering[side->pawns[i-1]]>pawn_ordering[side->pawns[i]]; i--)
                    {
                        int temp = side->pawns[i-1];
                        side->pawns[i-1] = side->pawns[i];
                        side->pawns[i] = temp;
                    }
                }
            }

            // Test whether the Pawn is giving check directly and/or discovering an attack by a Queen, Rook or Bishop
            bool check = false;
            int king_sq = cr.white ? cr.bking_square : cr.wking_square;   // the other king
            int king_file = king_sq&7;
            int dst_file = dst&7;

            // Direct check ?
            int check_delta = king_sq - dst;
            if( cr.white )
                check = (dst_file+1==king_file && check_delta==-7) || (dst_file-1==king_file && check_delta==-9);
            else
                check = (dst_file-1==king_file && check_delta==7) || (dst_file+1==king_file && check_delta==9);

            // Discovered (or other tricky) check ?
            if( !check )    // no need to test for discovery if already found direct attack
            {
                if( enpassant )
                {
                    int captured_pawn = cr.white ? dst+8 : dst-8;
                    cr.squares[dst]           = cr.squares[src];
                    cr.squares[captured_pawn] = ' ';
                    cr.squares[src]           = ' ';
                    check = cr.AttackedPiece(thc::Square(king_sq));
                    cr.squares[src]           = cr.squares[dst];
                    cr.squares[captured_pawn] = cr.white ? 'p' : 'P';
                    cr.squares[dst]           = ' ';
                }
                else if( promotion_char )
                {
                    char temp = cr.squares[dst];
                    cr.squares[dst]           = promotion_char + (cr.white ? 0 : ' ');
                    cr.squares[src]           = ' ';
                    check = cr.AttackedPiece(thc::Square(king_sq));
                    cr.squares[src]           = cr.white ? 'P' : 'p';
                    cr.squares[dst]           = temp;
                }
                else
                {
                    // Discovered check on any ray (not necessarily) vacated by the pawn?
                    DISCOVERY_CHECK( check, cr.white, src, dst, king_sq, ray_offset )
                }

            }

            char f1 = get_file((thc::Square)src);
            char f2 = get_file((thc::Square)dst);
            char r2 = get_rank((thc::Square)dst);
            if( f1 == f2)
            {
                san_move += f2;
                san_move += r2;
            }
            else
            {
                san_move += f1;
                san_move += 'x';
                san_move += f2;
                san_move += r2;
            }
            if( promotion_char )
            {
                san_move += '=';
                san_move += promotion_char;
            }
            if( check )
                san_move += '+';
            break;
        }   // end PAWN
    }   // end switch on moving piece

    // Accomodate captured piece on other side, if other side is in fast mode
    int  capture_location=dst;
    if( special == thc::SPECIAL_WEN_PASSANT )
        capture_location = dst+8;
    else if( special == thc::SPECIAL_BEN_PASSANT )
        capture_location = dst-8;
    char capture = cr.squares[capture_location];
    bool making_capture = (isalpha(capture) ? true : false);
    if( making_capture && other->fast_mode )
    {
        switch( tolower(capture) )
        {
            case 'n':
            {
                if( other->nbr_knights==2 && other->knights[0]==capture_location )
                    other->knights[0] = other->knights[1];
                other->nbr_knights--;
                break;
            }
            case 'r':
            {
                if( other->nbr_rooks==2 && other->rooks[0]==capture_location )
                    other->rooks[0] = other->rooks[1];
                other->nbr_rooks--;
                break;
            }
            case 'b':
            {
                if( is_dark(capture_location) )
                    other->nbr_dark_bishops--;
                else
                    other->nbr_light_bishops--;
                break;
            }
            case 'q':
            {
                if( other->nbr_queens==2 && other->queens[0]==capture_location )
                    other->queens[0] = other->queens[1];
                other->nbr_queens--;
                break;
            }
            case 'p':
            {
                int other_pawn_offset=0;
                for( int i=0; i<other->nbr_pawns; i++ )
                {
                    if( other->pawns[i] == capture_location )
                    {
                        other_pawn_offset = i;
                        break;
                    }
                }
                for( int i=other_pawn_offset; i+1<other->nbr_pawns; i++ )
                {
                    other->pawns[i] = other->pawns[i+1];
                }
                other->nbr_pawns--;
            }
        }
    }
    thc::Move mv;
    mv.src = static_cast<thc::Square>(src);
    mv.dst = static_cast<thc::Square>(dst);
    mv.special = special;
    mv.capture = capture;
    return mv;
}

// Call this function *once* at startup to build the fast ray_lookup_table[64][64]
void compress_temp_lookup_gen_function()
{

#define SW_OFFSET 7;
#define NE_OFFSET -7
#define NW_OFFSET -9
#define SE_OFFSET 9

    // Calculate the end square and count to that square of every diagonal ray
    //  from every square
    int sw_square[64];
    int sw_count[64];
    int ne_square[64];
    int ne_count[64];
    int nw_square[64];
    int nw_count[64];
    int se_square[64];
    int se_count[64];
    for( int8_t sq=0; sq<64; sq++ )
    {
        for( int8_t i=0; i<4; i++ )
        {
            int offset=0, rank=0, file=0;
            int *ptr_square=0;
            int *ptr_count=0;
            switch( i )
            {
                // SW
                case 0: offset = SW_OFFSET;
                        file   = 0;
                        rank   = 0x38;
                        ptr_square = sw_square;
                        ptr_count  = sw_count;
                        break;
                // NE
                case 1: offset = NE_OFFSET;
                        file   = 7;
                        rank   = 0;
                        ptr_square = ne_square;
                        ptr_count  = ne_count;
                        break;
                // NW
                case 2: offset = NW_OFFSET;
                        file   = 0;
                        rank   = 0;
                        ptr_square = nw_square;
                        ptr_count  = nw_count;
                        break;
                // SE
                case 3: offset = SE_OFFSET;
                        file   = 7;
                        rank   = 0x38;
                        ptr_square = se_square;
                        ptr_count  = se_count;
                        break;
            }


            *(ptr_square+sq) = 0;
            *(ptr_count+sq)  = 0;
            int rover = sq;
            for( int j=0; j<8; j++ )
            {
                if( (rover&7)==file || (rover&0x38)==rank )
                {
                    *(ptr_square+sq) = rover;
                    *(ptr_count+sq) = j;
                    break;
                }
                rover += offset;
            }
        }
    }

    // Now build main table a 64x64 table of to quickly look up the ray
    //  from all squares for any square. eg (e4,g6) is the SW diagonal
    //  from g6 through e4 to b1. There are 8 possible rays, of which
    //  either zero (most common) or one applies for any given 2 squares
    for( int8_t sq=0; sq<64; sq++ )
    {
        for( int8_t king_sq=0; king_sq<64; king_sq++ )
        {
            RayValues *r = &ray_lookup_table[sq][king_sq];
            int8_t king_file = king_sq&7;
            int8_t king_rank = king_sq&0x38;
            int8_t src = sq;
            int8_t src_file = sq&7;
            int8_t src_rank = sq&0x38;
            int8_t discovery_ray_offset = 0;
            int8_t discovery_ray_count = 0;
            int8_t discovery_ray_square = 0;

            // Are squares on same rank ?
            if( src_rank == king_rank )                                         
            {                                                                   
                if( src < king_sq )                                             
                {                                                               
                    discovery_ray_offset = -1;                                  
                    discovery_ray_square = king_sq & 0xf8;  // set file to 0    
                    discovery_ray_count  = king_file;                           
                }                                                               
                else                                                            
                {                                                               
                    discovery_ray_offset = +1;                                  
                    discovery_ray_square = king_sq | 7;     // set file to 7 
                    discovery_ray_count  = 7-king_file;                         
                }                                                               
            }                                                                   

            // Are squares on same file ?
            else if( src_file == king_file )                                    
            {                                                                   
                if( src < king_sq )                                             
                {                                                               
                    discovery_ray_offset = -8;                                  
                    discovery_ray_square = king_sq & 0xc7;    // set rank to 0 
                    discovery_ray_count  = king_rank>>3;                        
                }                                                               
                else                                                            
                {                                                               
                    discovery_ray_offset = +8;                                  
                    discovery_ray_square = king_sq | 0x38;   // set rank to 0x38   
                    discovery_ray_count  = 7 - (king_rank>>3);                     
                }                                                                  
            }

            // Is it one of the four "rook" rays above ?
            if( discovery_ray_offset != 0 )
            {
                r->ray_rook_offset = discovery_ray_offset;
                r->ray_offset = discovery_ray_offset;
                r->ray_count  = discovery_ray_count;
                r->ray_square = discovery_ray_square;
                continue;
            }

            // Look for a northward "bishop" ray
            if( src < king_sq )            
            {                              
                if( src_file < king_file  )
                {                          
                    if( king_file-src_file == ((king_rank-src_rank)>>3) )
                    {                                                    
                        discovery_ray_square = nw_square[king_sq];
                        discovery_ray_count  = nw_count [king_sq];
                        discovery_ray_offset = NW_OFFSET;         
                    }                                             
                }                                                 
                else                                              
                {                                                 
                    if( src_file-king_file == ((king_rank-src_rank)>>3) )
                    {                                                    
                        discovery_ray_square = ne_square[king_sq];
                        discovery_ray_count  = ne_count [king_sq];
                        discovery_ray_offset = NE_OFFSET;         
                    }                                             
                }                                                 
            }                                                     

            // Else look for a southward "bishop" ray
            else                                                  
            {                                                     
                if( src_file < king_file )                        
                {                                                 
                    if( king_file-src_file == ((src_rank-king_rank)>>3) )
                    {                                                    
                        discovery_ray_square = sw_square[king_sq];
                        discovery_ray_count  = sw_count [king_sq];
                        discovery_ray_offset = SW_OFFSET;         
                    }                                             
                }                                                 
                else                                              
                {                                                 
                    if( src_file-king_file == ((src_rank-king_rank)>>3) )
                    {                                                    
                        discovery_ray_square = se_square[king_sq];
                        discovery_ray_count  = se_count [king_sq];
                        discovery_ray_offset = SE_OFFSET;         
                    }                                             
                }                                                 
            }

            // Is it one of the four "bishop" rays above ?
            if( discovery_ray_offset != 0 )
            {
                r->ray_bishop_offset = discovery_ray_offset;
                r->ray_offset = discovery_ray_offset;
                r->ray_count  = discovery_ray_count;
                r->ray_square = discovery_ray_square;
            }
        }
    }
}

// Strip out everything except the main line
void bc_mainline( const std::string &bc_in,  std::string &bc_out ) {/*todo*/}

// Find the position and moves leading to this offset in the bytecode
void bc_locate( std::string &bc, int offset, thc::ChessPosition &start, std::vector<thc::Move> var ) {/*todo*/}

// Add a move of bytecode
void bc_insert( std::string &bc, int offset, thc::ChessRules &cr, thc::Move mv ) {/*todo*/}

// Skip to matching end of nested section starting at offset, return 0 if not found
size_t bc_skipover( const std::string &bc, size_t offset )
{
    size_t len = bc.length();
    int depth = 0;
    char begin = bc[offset];
    char end = begin+1;
    if( begin != BC_VARIATION_START && begin != BC_META_START && begin != BC_COMMENT_START )
        return 0;
    for( size_t i=offset+1; i<len; i++  )
    {
        char c = bc[i];
        if( c == begin )
            depth++;
        else if( c == end )
        {
            depth--;
            if( depth==0 )
                return i+1;     // return offset one beyond
        }
    }
    return 0;
}

std::string bc_comment( std::string &bc, size_t offset )
{
    std::string ret;
    size_t end = bc_skipover( bc, offset );
    while( offset < end )
    {
        char c = bc[offset];
        if( c >= ' ' )
            ret += c;
    }
    return ret;
}

// Insert into game
void bc_insert_str( std::string &bc_in, int offset, const std::string str ) {/*todo*/}

// Insert NAG
//  (should remove nag of same type [type = nag1 or nag2])
void bc_insert_nag( std::string &bc_in, int offset, int nag )               {/*todo*/}

// Find the index of a move within its variation
//  eg 1. e4 e5 2. Nf3 (2. Nc3 Nf6 3.Bc4 Nxe4)  // idx of 2.Nc3 is 0, idx of 3...Nxe4 is 3)
int bc_variation_idx( const std::string &bc, size_t offset )
{
    struct STACK_ELEMENT
    {
        int variation_move_count=0;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    STACK_ELEMENT *stk = stk_array;
    int stk_idx = 0, in_meta=0, in_comment=0;
    bool escape = false;
    size_t len = bc.length();
    for( size_t i=0; i<offset && i<len; i++ )
    {
        if( escape )
        {
            escape = false;
            continue;
        }
        char c = bc[i];
        if( c == BC_ESCAPE )
        {
            escape = true;
        }
        else if( c == BC_VARIATION_START )
        {
            if( stk_idx < MAX_DEPTH )
            {
                stk_idx++;
                stk = &stk_array[stk_idx];
                stk->variation_move_count = 0;
            }
        }
        else if( c == BC_VARIATION_END )
        {
            if( stk_idx > 0 )
            {
                stk_idx--;
                stk = &stk_array[stk_idx];
            }
        }
        else if( c == BC_COMMENT_START )
        {
            in_comment++;
        }
        else if( c == BC_COMMENT_END )
        {
            in_comment--;
        }
        else if( c == BC_META_START )
        {
            in_meta++;
        }
        else if( c == BC_VARIATION_END )
        {
            in_meta--;
        }
        else if( c >= BC_MOVE_CODES && in_comment==0 && in_meta==0 )
        {
            stk->variation_move_count++;
        }
    }
    return stk->variation_move_count;
}


static int NagAlternative( const char *s )
{
    int nag = 0;
    if( *s == '!' )
    {
        if( 0 == strcmp(s,"!") )        nag=1;  // $1
        else if( 0 == strcmp(s,"!!") )  nag=3;  // $3
        else if( 0 == strcmp(s,"!?") )  nag=5;  // $5
    }
    else if( *s == '?' )
    {
        if( 0 == strcmp(s,"?") )        nag=2;  // $2
        else if( 0 == strcmp(s,"??") )  nag=4;  // $4
        else if( 0 == strcmp(s,"?!") )  nag=6;  // $6
    }
    else if( *s == '=' )
    {
        if( 0 == strcmp(s,"=") )        nag=10;  // $10
        else if( 0 == strcmp(s,"=+") )  nag=15;  // $15
    }
    else if( *s == '+' )
    {
        if( 0 == strcmp(s,"+=") )       nag=14;  // $14
        else if( 0 == strcmp(s,"+/-") ) nag=16;  // $16
        else if( 0 == strcmp(s,"+-")  ) nag=18;  // $18
    }
    else if( *s == '-' )
    {
        if( 0 == strcmp(s,"-/+") )      nag=17;  // $17
        else if( 0 == strcmp(s,"-+") )  nag=19;  // $19
    }
    return nag;
}

static std::string ReplaceAll( const std::string &in, const std::string &from, const std::string &to )
{
    std::string out = in;
    size_t pos = out.find(from);
    while( pos != std::string::npos )
    {
        out.replace( pos, from.length(), to );
        pos = out.find( from, pos+to.length() );
    }
    return out;
}

static bool PgnTestResult( const char *s )
{
    bool is_pgn_result = false;
    if( 0 == strcmp(s,"1-0") )
        is_pgn_result = true;
    else if( 0 == strcmp(s,"0-1") )
        is_pgn_result = true;
    else if( 0 == strcmp(s,"1/2-1/2") )
        is_pgn_result = true;
    else if( 0 == strcmp(s,"*") )
        is_pgn_result = true;
    return is_pgn_result;
}

static std::string RemoveLineEnds( std::string &s )
{
    std::string t;
    int len=s.length();
    bool in_white=false;
    for( int i=0; i<len; i++ )
    {
        char c = s[i];
        if( c=='\n' || c=='\r' )
        {
            if( !in_white )
                t += ' ';
            in_white = true;
        }
        else
        {
            t += c;
            in_white = (c==' ');
        }
    }
    return t;
}

// Later - modify to allow insertion of new material within an existing bytecode (for promote comment feature)
std::string Bytecode::PgnParse( thc::ChessRules &cr2, const std::string str, bool use_semi, int &nbr_converted, bool use_current_language )
{
    Init( cr2 );
    return PgnParse( str );
}

// Later - modify to allow insertion of new material within an existing bytecode (for promote comment feature)
std::string Bytecode::PgnParse( const std::string str )
{
    bool use_semi=false; int nbr_converted=0; bool use_current_language=false;
    sides[0].fast_mode=false;
    sides[1].fast_mode=false;
    std::string bytecode;

    // Main state machine has these states
    enum PSTATE
    {
        BETWEEN_MOVES,
        IN_COMMENT,
        IN_DOLLAR,
        IN_NUMBER,
        IN_MISC,
        IN_STAR,
        IN_MOVE
    };

    PSTATE       state;         // the current state machine state

    // Allow stacking of the key state variables
    struct STACK_ELEMENT
    {
        PSTATE          state;
        thc::ChessRules cr;
        thc::Move       mv;
        int variation_move_count=0;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    int stk_idx = 0;
    STACK_ELEMENT *stk = &stk_array[stk_idx];

    // Misc
    #define Error(x)
    std::string move_str;
    std::string comment_str;
    std::string buffered_comment;
    std::string prefix;
    char comment_ch='\0', previous_ch='\0';
    int nag_value=0;
    PSTATE old_state, save_state=BETWEEN_MOVES;
    char ch, push_back='\0';
    unsigned int str_idx=0;
    nbr_converted = 0;

    state = BETWEEN_MOVES;      // state machine
    old_state = BETWEEN_MOVES;

    //
    //  Later - allow PGN parsing to start at arbitrary offset - legacy code did some weird stack scanning
    //

    // Start loop
    bool okay=true;
    bool eof=false;
    ch = ' ';   // dummy to start loop
    while( okay && !eof )
    {
        push_back = '\0';
        old_state = state;
        if( ch == '\x96' )  // ANSI code for en dash, sometimes used instead of '-'
            ch = '-';
        bool in_number = isascii(ch) && isdigit(ch);
        bool in_move   = isascii(ch) && isalnum(ch);
        bool in_misc   = (ch=='.'||ch=='?'||ch=='!'||ch=='+'||ch=='-'||ch=='='||ch=='/');
        bool in_star   = (ch=='*');
        switch( state )
        {

            // In a comment
            case IN_COMMENT:
            {

                // End of the comment ?
                if(  (comment_ch=='{' && ch=='}') ||
                     (comment_ch==';' && ch!=';' && ch!='\n' && ch!='\r' && (previous_ch=='\n'||previous_ch=='\r') )
                  )
                {
                    if( comment_ch == ';' )
                        push_back = ch;
                    else
                    {
                        // New policy from V2.03c
                        //  Only create '{' comments (most chess software doesn't understand ';' comments)
                        //  If  "}" appears in comment transform to "|>"    (change *is* restored when reading .pgn)
                        //  If  "|>" appears in comment transform to "| >"  (change is *not* restored when reading .pgn)
                        std::string ReplaceAll( const std::string &in, const std::string &from, const std::string &to );
                        comment_str = ReplaceAll( comment_str, "|>", "}" );
                    }
                    size_t len = bytecode.length();
                    if( len > 0 && bytecode[len-1] == BC_COMMENT_END )
                    {
                        bytecode.pop_back();    // remove COMMENT_END and extend existing comment
                        bytecode.push_back( ' ' );
                    }
                    else
                        bytecode.push_back( BC_COMMENT_START );
                    bytecode += comment_str;
                    bytecode.push_back( BC_COMMENT_END );
                    state = save_state;
                }

                // Else continue comment
                else
                {
                    bool skip = (comment_ch==';' && ch==';' && (previous_ch=='\n'||previous_ch=='\r') );
                    if( !skip )
                    {
                        if( ch < 8 )
                            comment_str += '.';
                        else if( ch == '\n' )
                            comment_str += ' ';
                        else if( ch != '\r' )
                            comment_str += (char)ch;
                    }
                }
                break;
            }

            // Creating NAG value
            case IN_DOLLAR:
            {
                if( isascii(ch) && isdigit(ch) )
                    nag_value = nag_value*10+(ch-'0');
                else
                {
                    /*
                    const char *nag_array[] =
                    {
                        "",
                        " !",     // $1
                        " ?",     // $2
                        " !!",    // $3
                        " ??",    // $4
                        " !?",    // $5
                        " ?!",    // $6
                        "",       // $7
                        "",       // $8
                        " ??",    // $9
                        " =",     // $10
                        " =",     // $11
                        " =",     // $12
                        "",       // $13
                        " +=",    // $14
                        " =+",    // $15
                        " +/-",   // $16
                        " -/+",   // $17
                        " +-",    // $18
                        " -+",    // $19
                        " +-",    // $20
                        " -+"     // $21
                    };  */
                    if( 1<=nag_value && nag_value<=21 )
                    {
                        bytecode.push_back( BC_ESCAPE );
                        bytecode.push_back( 8 + nag_value );
                    }
                    push_back = ch;
                    state = save_state;
                }
                break;
            }

            // Waiting for something to happen
            case BETWEEN_MOVES:
            {

                // NAG token ?
                if( ch == '$' )
                {
                    save_state = state;
                    state = IN_DOLLAR;
                    nag_value = 0;
                }

                // Start {comment} style comment ?
                else if( ch == '{' )
                {
                    save_state = state;
                    comment_ch = '{';
                    state = IN_COMMENT;
                    comment_str = "";
                }

                // Start ;comment style comment ?
                else if( use_semi && ch == ';' )
                {
                    save_state = state;
                    comment_ch = ';';
                    state = IN_COMMENT;
                    comment_str = "";
                }

                // Start new variation ?
                else if( ch == '(' )
                {

                    // Push current state onto a stack
                    stk->cr    = cr;
                    stk->state = state;
                    if( stk_idx+1 >= MAX_DEPTH )
                    {
                        Error("Too deep");
                        okay = false;
                    }
                    else
                    {

                        // Pop off most recent move
                        if( stk->variation_move_count > 0 )
                            cr.PopMove( stk->mv );
                        stk_idx++;
                        stk = &stk_array[stk_idx];
                        stk->variation_move_count = 0;

                        // Disrupting encoding, force rescan
                        sides[0].fast_mode=false;
                        sides[1].fast_mode=false;
                        bytecode.push_back( BC_VARIATION_START );
                    }
                }

                // End variation ?
                else if( ch == ')' )
                {

                    // Pop old state off stack
                    if( stk_idx == 0 )
                    {
                        Error("Mismatched )");
                        okay = false;
                    }
                    else
                    {
                        stk_idx--;
                        stk   = &stk_array[stk_idx];
                        cr    = stk->cr;
                        state = stk->state;
                        bytecode.push_back( BC_VARIATION_END );

                        // Disrupting encoding, force rescan
                        sides[0].fast_mode=false;
                        sides[1].fast_mode=false;

                        //
                        // Todo if variation we added was empty, remove it
                        //
                    }
                }

                // Else go into token if possible
                else
                {
                    if( in_number || in_misc || in_star || in_move )
                    {
                        move_str = "";
                        push_back = ch;
                        if( in_number )
                            state = IN_NUMBER;
                        else if( in_move )
                            state = IN_MOVE;
                        else if( in_misc )
                            state = IN_MISC;
                        else if( in_star )
                            state = IN_STAR;
                    }
                }
                break;
            }

            // Collect a token
            case IN_NUMBER:
            case IN_MISC:
            case IN_STAR:
            case IN_MOVE:
            {
                bool keep_buffering=false;
                if( state == IN_NUMBER )
                    keep_buffering = in_number || ch=='-' || ch=='/';   // eg 0-1 or 1/2-1/2
                else if( state == IN_MISC )
                    keep_buffering = in_misc;
                else if( state == IN_STAR )
                    keep_buffering = in_star;
                else if( state == IN_MOVE )
                {
                    if( ch=='=' || ch=='+' || ch=='#' || ch=='-' )  // can also be in moves
                        keep_buffering = true;
                    else
                        keep_buffering = in_move;
                }
                if( keep_buffering )
                    move_str += (char)ch;
                else
                {
                    push_back = ch;
                    bool was_in_move = (state==IN_MOVE);
                    bool do_nothing_move = false;

                    // Support some popular variants
                    if( state == IN_NUMBER )
                    {
                        if( move_str == "0-0" )
                        {
                            move_str = "O-O";
                            was_in_move = true;
                        }
                        else if( move_str=="0-0-0" )
                        {
                            move_str = "O-O-O";
                            was_in_move = true;
                        }
                    }
                    else if( state == IN_MISC )
                    {
                        if( move_str == "--" )     // do nothing or pass move
                        {
                            do_nothing_move = true;
                            was_in_move = true;
                        }

                        // Support '?', '!!' etc as text rather than only as NAG codes
                        int nag_value2 = NagAlternative(move_str.c_str());
                        if( 1<=nag_value2 && nag_value2<=21 && stk->variation_move_count>0 )
                        {
                            bytecode.push_back( BC_ESCAPE );
                            bytecode.push_back( 8 + nag_value2 );
                            nbr_converted++;    // so just a lone += for example counts
                        }
                    }
                    state = BETWEEN_MOVES;
                    if( PgnTestResult(move_str.c_str()) )
                    {
                        eof = true;     // just stop (a bit simplistic)
                    }
                    else if( was_in_move )
                    {
                        std::string temp = move_str;
                        if( use_current_language )
                            LangToEnglish(temp);
                        thc::Move mv;
                        if( !do_nothing_move )
                        {
                            okay = mv.NaturalIn(&cr,temp.c_str());
                        }
                        else
                        {   // Nasty little hack - support "--" = do nothing, create a move from one empty square
                            //  to same empty square, capturing empty - chess engine will "play" that okay
                            okay = false;
                            for( int i=63; i>=0; i-- )  // start search at h1 to avoiding a8a8 which is Invalid move
                            {   // found an empty square yet ?
                                if( cr.squares[i]==' ' || cr.squares[i]=='.' )  // plan to change empty from ' ' to '.', so be prepared
                                {   // C++ can't cast into the bitfield, so do this, aaaaaaaargh
                                    static thc::Square lookup[] = { thc::a8,thc::b8,thc::c8,thc::d8,thc::e8,thc::f8,thc::g8,thc::h8,
                                                                    thc::a7,thc::b7,thc::c7,thc::d7,thc::e7,thc::f7,thc::g7,thc::h7,
                                                                    thc::a6,thc::b6,thc::c6,thc::d6,thc::e6,thc::f6,thc::g6,thc::h6,
                                                                    thc::a5,thc::b5,thc::c5,thc::d5,thc::e5,thc::f5,thc::g5,thc::h5,
                                                                    thc::a4,thc::b4,thc::c4,thc::d4,thc::e4,thc::f4,thc::g4,thc::h4,
                                                                    thc::a3,thc::b3,thc::c3,thc::d3,thc::e3,thc::f3,thc::g3,thc::h3,
                                                                    thc::a2,thc::b2,thc::c2,thc::d2,thc::e2,thc::f2,thc::g2,thc::h2,
                                                                    thc::a1,thc::b1,thc::c1,thc::d1,thc::e1,thc::f1,thc::g1,thc::h1
                                                                    };
                                    mv.src =
                                    mv.dst = lookup[i];
                                    mv.capture = cr.squares[i];  // empty
                                    mv.special = thc::NOT_SPECIAL;
                                    okay = true;
                                    break;
                                }
                            }
                        }
                        if( !okay )
                        {
                            Error("Illegal move");
                            //prefix += move_str;
                            //prefix += push_back;
                        }
                        else
                        {
                            prefix = "";
                            nbr_converted++;
                            Army *side  = cr.white ? &sides[0] : &sides[1];
                            Army *other = cr.white ? &sides[1] : &sides[0];
                            char c;
                            if( side->fast_mode )
                            {
                                c = CompressFastMode(mv,side,other);
                            }
                            else if( TryFastMode(side) )
                            {
                                c = CompressFastMode(mv,side,other);
                            }
                            else
                            {
                                c = CompressSlowMode(mv);
                                other->fast_mode = false;   // force other side to reset and retry
                            }
                            bytecode.push_back(c);
                            cr.PushMove(mv);
                            stk->mv = mv;
                            stk->variation_move_count++;
                        }
                    }
                }
                break;
            }
        }

        // State changes
        if( state != old_state )
        {
            //fprintf( debug_log_file(), "State change %s->%s\n", ShowState(old_state), ShowState(state) );
        }

        // Error handling, append the rest of the string as a comment
        if( !okay )
        {
            std::string comment;
            if( str_idx < str.length() )
            {
                std::string temp( str.begin()+str_idx, str.end() );
                int len = temp.length();
                if( len>0 && temp[len-1]==')' )
                    temp = temp.substr(0,len-1);
                comment += temp;
            }
            comment = RemoveLineEnds(comment);
            if( comment != "" )
            {
                size_t len = bytecode.length();
                if( len > 0 && bytecode[len-1] == BC_COMMENT_END )
                {
                    bytecode.pop_back();    // remove COMMENT_END and extend existing comment
                    bytecode.push_back( ' ' );
                }
                else
                    bytecode.push_back( BC_COMMENT_START );
                bytecode += comment;
                bytecode.push_back( BC_COMMENT_END );
            }
        }

        // Next character
        else if( !eof )
        {
            if( push_back )
                ch = push_back;
            else
            {
                previous_ch = ch;
                if( str_idx < str.length() )
                {
                    ch = str[str_idx];
                    prefix += ch;
                }
                else
                {
                    ch  = ' ';  // add trailing padding to terminate last token
                    if( str_idx > str.length()+2 )
                        eof = true;   // after a trailing ' ' or so, stop
                }
                str_idx++;
            }
        }
    }
    return bytecode;
}
