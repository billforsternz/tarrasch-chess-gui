/****************************************************************************
 * Chess classes - Move
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <string>
#include "DebugPrintf.h"
#include "PrivateChessDefs.h"
#include "ChessRules.h"
#include "Move.h"
using namespace std;
using namespace thc;

/****************************************************************************
 * Read natural string move eg "Nf3"
 *  return bool okay
 ****************************************************************************/
bool Move::NaturalIn( ChessRules *cr, const char *natural_in )
{
    MOVELIST list;
    int  i, len=0;
    char src_file='\0', src_rank='\0', dst_file='\0', dst_rank='\0';
    char promotion='\0';
    bool enpassant=false;
    bool kcastling=false;
    bool qcastling=false;
    Square dst=a8;
    Move *m, *found=NULL;
    char *s;
    char  move[10];
    bool  white=cr->white;
    char  piece=(white?'P':'p');
    bool  default_piece=true;
    
    // Indicate no move found (yet)
    bool okay=true;

    // Copy to read-write variable
    okay = false;
    for( i=0; i<sizeof(move); i++ )
    {
        move[i] = natural_in[i];
        if( move[i]=='\0' || move[i]==' ' || move[i]=='\t' ||
            move[i]=='\r' || move[i]=='\n' )
        {
            move[i] = '\0';
            okay = true;
            break;
        }
    }
    if( okay )
    {

        // Trim string from end
        s = strchr(move,'\0') - 1;
        while( s>=move && !(isascii(*s) && isalnum(*s)) )
            *s-- = '\0';

        // Trim string from start
        s = move;
        while( *s==' ' || *s=='\t' )
            s++;
        len = (int)strlen(s);
        for( i=0; i<len+1; i++ )  // +1 gets '\0' at end
            move[i] = *s++;  // if no leading space this does
                            //  nothing, but no harm either

        // Trim enpassant
        if( len>=2 && move[len-1]=='p' )
        {
            if( 0 == strcmp(&move[len-2],"ep") )
            {
                move[len-2] = '\0';
                enpassant = true;
            }
            else if( len>=3 && 0==strcmp(&move[len-3],"e.p") )
            {
                move[len-3] = '\0';
                enpassant = true;
            }

            // Trim string from end, again
            s = strchr(move,'\0') - 1;
            while( s>=move && !(isascii(*s) && isalnum(*s)) )
                *s-- = '\0';
            len = (int)strlen(move);
        }

        // Promotion
        if( strchr(move,'=') )
        {
            switch( move[len-1] )
            {
                case 'q':
                case 'Q':   promotion='Q';  break;
                case 'r':
                case 'R':   promotion='R';  break;
                case 'b':
                case 'B':   promotion='B';  break;
                case 'n':
                case 'N':   promotion='N';  break;
                default:    okay = false;   break;
            }
            if( okay )
            {

                // Trim string from end, again
                move[len-1] = '\0';
                s = strchr(move,'\0') - 1;
                while( s>=move && !(isascii(*s) && isalnum(*s)) )
                    *s-- = '\0';
                len = (int)strlen(move);
            }
        }    
    }

    // Castling
    if( okay )
    {
        if( 0==strcmpi(move,"oo") || 0==strcmpi(move,"o-o") )
        {
            strcpy( move, (white?"e1g1":"e8g8") );
            len       = 4;
            piece     = (white?'K':'k');
            default_piece = false;
            kcastling = true;
        }
        else if( 0==strcmpi(move,"ooo") || 0==strcmpi(move,"o-o-o") )
        {
            strcpy( move, (white?"e1c1":"e8c8") );
            len       = 4;
            piece     = (white?'K':'k');
            default_piece = false;
            qcastling = true;
        }
    }

    // Destination square for all except pawn takes pawn (eg "ef")
    if( okay )
    {
        if( len==2 && 'a'<=move[0] && move[0]<='h'
                   && 'a'<=move[1] && move[1]<='h' )
        {
            src_file = move[0]; // pawn takes pawn
            dst_file = move[1];
        }
        else if( len>=2 && 'a'<=move[len-2] && move[len-2]<='h'
                        && '1'<=move[len-1] && move[len-1]<='8' )
        {
            dst_file = move[len-2];
            dst_rank = move[len-1];
            dst = SQ(dst_file,dst_rank);
        }
        else
            okay = false;
    }

    // Source square and or piece
    if( okay )
    {
        if( len > 2 )
        {
            if( 'a'<=move[0] && move[0]<='h' &&
                '1'<=move[1] && move[1]<='8' )
            {
                src_file = move[0];
                src_rank = move[1];
            }
            else
            {
                switch( move[0] )
                {
                    case 'K':   piece = (white?'K':'k');    default_piece=false; break;
                    case 'Q':   piece = (white?'Q':'q');    default_piece=false; break;
                    case 'R':   piece = (white?'R':'r');    default_piece=false; break;
                    case 'N':   piece = (white?'N':'n');    default_piece=false; break;
                    case 'P':   piece = (white?'P':'p');    default_piece=false; break;
                    case 'B':   piece = (white?'B':'b');    default_piece=false; break;
                    default:
                    {
                        if( 'a'<=move[0] && move[0]<='h' )
                            src_file = move[0]; // eg "ef4"
                        else
                            okay = false;
                        break;
                    }
                }       
                if( len>3  && src_file=='\0' )  // not eg "ef4" above
                {
                    if( '1'<=move[1] && move[1]<='8' )
                        src_rank = move[1];
                    else if( 'a'<=move[1] && move[1]<='h' )
                    {
                        src_file = move[1];
                        if( len>4 && '1'<=move[2] && move[2]<='8' )
                            src_rank = move[2];
                    }
                }
            }
        }
    }

    // Check against all possible moves
    if( okay )
    {
        cr->GenLegalMoveList( &list );

        // Have source and destination, eg "d2d3"
        if( enpassant )
            src_rank = dst_rank = '\0';
        if( src_file && src_rank && dst_file && dst_rank )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( (default_piece || piece==cr->squares[m->src])  &&
                    src_file  ==   FILE(m->src)       &&
                    src_rank  ==   RANK(m->src)       &&
                    dst       ==   m->dst
                )
                {
                    if( kcastling )
                    {
                        if( m->special ==
                             (white?SPECIAL_WK_CASTLING:SPECIAL_BK_CASTLING) )
                            found = m;
                    }
                    else if( qcastling )
                    {
                        if( m->special ==
                             (white?SPECIAL_WQ_CASTLING:SPECIAL_BQ_CASTLING) )
                            found = m;
                    }
                    else
                        found = m;
                    break;
                }
            }
        }

        // Have source file only, eg "Rae1"
        else if( src_file && dst_file && dst_rank )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]  &&
                    src_file  ==   FILE(m->src)         &&
                 /* src_rank  ==   RANK(m->src)  */
                    dst       ==   m->dst
                )
                {
                    found = m;
                    break;
                }
            }
        }

        // Have source rank only, eg "R2d2"
        else if( src_rank && dst_file && dst_rank )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]   &&
                 /* src_file  ==   FILE(m->src) */
                    src_rank  ==   RANK(m->src)          &&        
                    dst       ==   m->dst
                )
                {
                    found = m;
                    break;
                }
            }
        }

        // Have destination file only eg e4f (because 2 ef moves are possible)
        else if( src_file && src_rank && dst_file )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]      &&
                    src_file  ==   FILE(m->src)             &&
                    src_rank  ==   RANK(m->src)             &&
                    dst_file  ==   FILE(m->dst)
                )
                {
                    found = m;
                    break;
                }
            }
        }

        // Have files only, eg "ef"
        else if( src_file && dst_file )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]      &&
                    src_file  ==   FILE(m->src)             &&
                 /* src_rank  ==   RANK(m->src) */                  
                    dst_file  ==   FILE(m->dst)
                )
                {
                    if( enpassant )
                    {
                        if( m->special ==
                             (white?SPECIAL_WEN_PASSANT:SPECIAL_BEN_PASSANT) )
                            found = m;
                    }
                    else
                        found = m;
                    break;
                }
            }
        }

        // Have destination square only eg "a4"
        else if( dst_rank && dst_file )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]          &&
                    dst       ==   m->dst
                )
                {
                    found = m;
                    break;
                }
            }
        }
    }

    // Copy found move
    if( okay && found )
    {
        bool found_promotion =
            ( found->special == SPECIAL_PROMOTION_QUEEN ||
              found->special == SPECIAL_PROMOTION_ROOK ||
              found->special == SPECIAL_PROMOTION_BISHOP ||
              found->special == SPECIAL_PROMOTION_KNIGHT
            );
        if( promotion && !found_promotion )
            okay = false;
        if( found_promotion )
        {
            switch( promotion )
            {
                default:
                case 'Q': found->special = SPECIAL_PROMOTION_QUEEN;   break;
                case 'R': found->special = SPECIAL_PROMOTION_ROOK;    break;
                case 'B': found->special = SPECIAL_PROMOTION_BISHOP;  break;
                case 'N': found->special = SPECIAL_PROMOTION_KNIGHT;  break;
            }
        }
    }
    if( !found )
        okay = false;
    if( okay )
        *this = *found;
    return okay;
}

/****************************************************************************
 * Read natural string move eg "Nf3"
 *  return bool okay
 * Fast alternative for known good input
 ****************************************************************************/
bool Move::NaturalInFast( ChessRules *cr, const char *natural_in )
{
    bool err = false;
    bool found = false;
    bool capture = false;
    Move mv;

    /*
     Handles moves of the following type
     exd8=N
     e8=B
     exd5
     e4
     Nf3
     Nxf3
     Nef3
     Nexf3
     N2f3
     N2xf3
     O-O
     O-O-O
     */
    
//
    mv.special = NOT_SPECIAL;
    mv.capture = ' ';
    char f = *natural_in++;
   
    // WHITE MOVE
    if( cr->white )
    {
        
        // Pawn move ?
        if( 'a'<=f && f<='h' )
        {
            char r = *natural_in++;
            if( r != 'x')
            {
                
                // Non capturing, non promoting pawn move
                if( '3'<= r && r<= '7')
                {
                    mv.dst = SQ(f,r);
                    mv.src = SOUTH(mv.dst);
                    if( cr->squares[mv.src]=='P' && cr->squares[mv.dst]==' ')
                        found =true;
                    else if( r=='4' && cr->squares[mv.src]==' ' && cr->squares[mv.dst]==' ')
                    {
                        mv.special = SPECIAL_WPAWN_2SQUARES;
                        mv.src = SOUTH(mv.src);
                        found = (cr->squares[mv.src]=='P');
                    }
                }

                // Non capturing, promoting pawn move
                else if( r=='8' && *natural_in++=='=' )
                {
                    mv.dst = SQ(f,r);
                    mv.src = SOUTH(mv.dst);
                    if( cr->squares[mv.src]=='P' && cr->squares[mv.dst]==' ')
                    {
                        switch( *natural_in++ )
                        {
                            default:
                            {
                                break;
                            }
                            case 'Q':
                            case 'q':
                            {
                                mv.special = SPECIAL_PROMOTION_QUEEN;
                                found = true;
                                break;
                            }
                            case 'R':
                            case 'r':
                            {
                                mv.special = SPECIAL_PROMOTION_ROOK;
                                found = true;
                                break;
                            }
                            case 'N':
                            case 'n':
                            {
                                mv.special = SPECIAL_PROMOTION_KNIGHT;
                                found = true;
                                break;
                            }
                            case 'B':
                            case 'b':
                            {
                                mv.special = SPECIAL_PROMOTION_BISHOP;
                                found = true;
                                break;
                            }
                        }
                    }
                }
            }
            else // if ( r == 'x' )
            {
                char g = *natural_in++;
                if( 'a'<=g && g<='h' )
                {
                    r = *natural_in++;
                    
                    // Non promoting, capturing pawn move
                    if( '3'<= r && r<= '7')
                    {
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r-1);
                        if( cr->squares[mv.src]=='P' && IsBlack(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            found = true;
                        }
                        else if( r=='6' && cr->squares[mv.src]=='P' && cr->squares[mv.dst]==' '  && mv.dst==cr->enpassant_target && cr->squares[SOUTH(mv.dst)]=='p' )
                        {
                            mv.capture = 'p';
                            mv.special = SPECIAL_WEN_PASSANT;
                            found = true;
                        }
                    }
                    
                    // Promoting, capturing pawn move
                    else if( r=='8' && *natural_in++=='=' )
                    {
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r-1);
                        if( cr->squares[mv.src]=='P' && IsBlack(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            switch( *natural_in++ )
                            {
                                default:
                                {
                                    break;
                                }
                                case 'Q':
                                case 'q':
                                {
                                    mv.special = SPECIAL_PROMOTION_QUEEN;
                                    found = true;
                                    break;
                                }
                                case 'R':
                                case 'r':
                                {
                                    mv.special = SPECIAL_PROMOTION_ROOK;
                                    found = true;
                                    break;
                                }
                                case 'N':
                                case 'n':
                                {
                                    mv.special = SPECIAL_PROMOTION_KNIGHT;
                                    found = true;
                                    break;
                                }
                                case 'B':
                                case 'b':
                                {
                                    mv.special = SPECIAL_PROMOTION_BISHOP;
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            
            // Piece move
            const lte **ray_lookup;
            switch( f )
            {
                case 'O':
                {
                    if( 0 == memcmp(natural_in,"-O-O",4) )
                    {
                        natural_in += 4;
                        mv.src = e1;
                        mv.dst = c1;
                        mv.capture = ' ';
                        mv.special = SPECIAL_WQ_CASTLING;
                        found = true;
                    }
                    else if( 0 == memcmp(natural_in,"-O",2) )
                    {
                        natural_in += 2;
                        mv.src = e1;
                        mv.dst = g1;
                        mv.capture = ' ';
                        mv.special = SPECIAL_WK_CASTLING;
                        found = true;
                    }
                    break;
                }
                    
                // King is simple special case - there's only one king so no disambiguation
                case 'K':
                {
                    f = *natural_in++;
                    if( f == 'x' )
                    {
                        capture = true;
                        f = *natural_in++;
                    }
                    if( 'a'<= f && f<='h' )
                    {
                        char r = *natural_in++;
                        if( '1'<=r && r<='8' )
                        {
                            mv.src = cr->wking_square;
                            mv.dst = SQ(f,r);
                            mv.special = SPECIAL_KING_MOVE;
                            mv.capture = cr->squares[mv.dst];
                            found = ( capture ? IsBlack(mv.capture) : IsEmptySquare(mv.capture) );
                        }
                    }
                    break;
                }
                    
                // Other pieces may need to check legality for disambiguation
                case 'Q':   ray_lookup = queen_lookup;                      // fall through
                case 'R':   if( f=='R' )    ray_lookup = rook_lookup;       // fall through
                case 'B':   if( f=='B' )    ray_lookup = bishop_lookup;     // fall through
                case 'N':
                {
                    char piece = f;
                    f = *natural_in++;
                    char src_file='\0';
                    char src_rank='\0';
                    if( f == 'x' )
                    {
                        capture = true;
                        f = *natural_in++;
                    }
                    if( '1'<=f && f<='8' )
                    {
                        src_rank = f;
                    }
                    else if( 'a'<= f && f<='h' )
                    {
                        src_file = f;
                    }
                    else
                        err = true;
                    if( !err )
                    {
                        char g = *natural_in++;
                        if( g == 'x' )
                        {
                            if( capture )
                                err = true;
                            else
                            {
                                capture = true;
                                g = *natural_in++;
                            }
                        }
                        if( '1'<=g && g<='8' )
                        {
                            if( src_file )
                            {
                                mv.dst = SQ(src_file,g);
                                src_file = '\0';
                            }
                            else
                                err = true;
                        }
                        else if( 'a'<=g && g<='h' )
                        {
                            char dst_rank = *natural_in++;
                            if( '1'<=dst_rank || dst_rank<='8' )
                                mv.dst = SQ(g,dst_rank);
                            else
                                err = true;
                        }
                        else
                            err = true;
                        if( !err )
                        {
                            if( capture ? IsBlack(cr->squares[mv.dst]) : cr->squares[mv.dst]==' ' )
                            {
                                mv.capture = cr->squares[mv.dst];
                                if( piece == 'N' )
                                {
                                    int count=0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = knight_lookup[mv.dst];
                                        lte nbr_moves = *ptr++;
                                        while( !found && nbr_moves-- )
                                        {
                                            Square src = (Square)*ptr++;
                                            if( cr->squares[src]=='N' )
                                            {
                                                bool src_file_ok = !src_file || FILE(src)==src_file;
                                                bool src_rank_ok = !src_rank || RANK(src)==src_rank;
                                                if( src_file_ok && src_rank_ok )
                                                {
                                                    mv.src = src;
                                                    if( probe == 0 )
                                                        count++;
                                                    else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                        // we are not exposing white king to check.
                                                    {
                                                        char temp = cr->squares[mv.dst];
                                                        cr->squares[mv.dst] = 'N';  // temporarily make move
                                                        cr->squares[src] = ' ';
                                                        found = !cr->AttackedSquare( cr->wking_square, false ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                        cr->squares[mv.dst] = temp;  // now undo move
                                                        cr->squares[src] = 'N';
                                                    }
                                                }
                                            }
                                        }
                                        if( probe==0 && count==1 )
                                            found = true; // done, no need for disambiguation by check
                                    }
                                }
                                else // if( rook, bishop, queen )
                                {
                                    int count = 0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = ray_lookup[mv.dst];
                                        lte nbr_rays = *ptr++;
                                        while( nbr_rays-- )
                                        {
                                            lte ray_len = *ptr++;
                                            while( ray_len-- )
                                            {
                                                Square src = (Square)*ptr++;
                                                if( !IsEmptySquare(cr->squares[src]) )
                                                {
                                                    // Any piece, friend or enemy must move to end of ray
                                                    ptr += ray_len;
                                                    ray_len = 0;
                                                    if( cr->squares[src] == piece )
                                                    {
                                                        bool src_file_ok = !src_file || FILE(src)==src_file;
                                                        bool src_rank_ok = !src_rank || RANK(src)==src_rank;
                                                        if( src_file_ok && src_rank_ok )
                                                        {
                                                            mv.src = src;
                                                            if( probe == 0 )
                                                                count++;
                                                            else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                                // we are not exposing white king to check.
                                                            {
                                                                char temp = cr->squares[mv.dst];
                                                                cr->squares[mv.dst] = piece;  // temporarily make move
                                                                cr->squares[mv.src] = ' ';
                                                                found = !cr->AttackedSquare( cr->wking_square, false ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                                cr->squares[mv.dst] = temp;  // now undo move
                                                                cr->squares[mv.src] = piece;
                                                            }
                                                        }
                                                    }

                                                }
                                            }
                                        }    
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // BLACK MOVE
    else
    {
        // Pawn move ?
        if( 'a'<=f && f<='h' )
        {
            char r = *natural_in++;
            if( r != 'x')
            {
                
                // Non capturing, non promoting pawn move
                if( '2'<= r && r<= '6')
                {
                    mv.dst = SQ(f,r);
                    mv.src = NORTH(mv.dst);
                    if( cr->squares[mv.src]=='p' && cr->squares[mv.dst]==' ')
                        found =true;
                    else if( r=='5' && cr->squares[mv.src]==' ' && cr->squares[mv.dst]==' ')
                    {
                        mv.special = SPECIAL_BPAWN_2SQUARES;
                        mv.src = NORTH(mv.src);
                        found = (cr->squares[mv.src]=='p');
                    }
                }
                
                // Non capturing, promoting pawn move
                else if( r=='1' && *natural_in++=='=' )
                {
                    mv.dst = SQ(f,r);
                    mv.src = NORTH(mv.dst);
                    if( cr->squares[mv.src]=='p' && cr->squares[mv.dst]==' ')
                    {
                        switch( *natural_in++ )
                        {
                            default:
                            {
                                break;
                            }
                            case 'Q':
                            case 'q':
                            {
                                mv.special = SPECIAL_PROMOTION_QUEEN;
                                found = true;
                                break;
                            }
                            case 'R':
                            case 'r':
                            {
                                mv.special = SPECIAL_PROMOTION_ROOK;
                                found = true;
                                break;
                            }
                            case 'N':
                            case 'n':
                            {
                                mv.special = SPECIAL_PROMOTION_KNIGHT;
                                found = true;
                                break;
                            }
                            case 'B':
                            case 'b':
                            {
                                mv.special = SPECIAL_PROMOTION_BISHOP;
                                found = true;
                                break;
                            }
                        }
                    }
                }
            }
            else // if ( r == 'x' )
            {
                char g = *natural_in++;
                if( 'a'<=g && g<='h' )
                {
                    r = *natural_in++;
                    
                    // Non promoting, capturing pawn move
                    if( '2'<= r && r<= '6')
                    {
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r+1);
                        if( cr->squares[mv.src]=='p' && IsWhite(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            found = true;
                        }
                        else if( r=='3' && cr->squares[mv.src]=='p' && cr->squares[mv.dst]==' '  && mv.dst==cr->enpassant_target && cr->squares[NORTH(mv.dst)]=='P' )
                        {
                            mv.capture = 'P';
                            mv.special = SPECIAL_BEN_PASSANT;
                            found = true;
                        }
                    }
                    
                    // Promoting, capturing pawn move
                    else if( r=='1' && *natural_in++=='=' )
                    {
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r+1);
                        if( cr->squares[mv.src]=='p' && IsWhite(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            switch( *natural_in++ )
                            {
                                default:
                                {
                                    break;
                                }
                                case 'Q':
                                case 'q':
                                {
                                    mv.special = SPECIAL_PROMOTION_QUEEN;
                                    found = true;
                                    break;
                                }
                                case 'R':
                                case 'r':
                                {
                                    mv.special = SPECIAL_PROMOTION_ROOK;
                                    found = true;
                                    break;
                                }
                                case 'N':
                                case 'n':
                                {
                                    mv.special = SPECIAL_PROMOTION_KNIGHT;
                                    found = true;
                                    break;
                                }
                                case 'B':
                                case 'b':
                                {
                                    mv.special = SPECIAL_PROMOTION_BISHOP;
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            
            // Piece move
            const lte **ray_lookup;
            switch( f )
            {
                case 'O':
                {
                    if( 0 == memcmp(natural_in,"-O-O",4) )
                    {
                        natural_in += 4;
                        mv.src = e8;
                        mv.dst = c8;
                        mv.capture = ' ';
                        mv.special = SPECIAL_BQ_CASTLING;
                        found = true;
                    }
                    else if( 0 == memcmp(natural_in,"-O",2) )
                    {
                        natural_in += 2;
                        mv.src = e8;
                        mv.dst = g8;
                        mv.capture = ' ';
                        mv.special = SPECIAL_BK_CASTLING;
                        found = true;
                    }
                    break;
                }
                    
                // King is simple special case - there's only one king so no disambiguation
                case 'K':
                {
                    f = *natural_in++;
                    if( f == 'x' )
                    {
                        capture = true;
                        f = *natural_in++;
                    }
                    if( 'a'<= f && f<='h' )
                    {
                        char r = *natural_in++;
                        if( '1'<=r && r<='8' )
                        {
                            mv.src = cr->bking_square;
                            mv.dst = SQ(f,r);
                            mv.special = SPECIAL_KING_MOVE;
                            mv.capture = cr->squares[mv.dst];
                            found = ( capture ? IsWhite(mv.capture) : IsEmptySquare(mv.capture) );
                        }
                    }
                    break;
                }
                    
                // Other pieces may need to check legality for disambiguation
                case 'Q':   ray_lookup = queen_lookup;                      // fall through
                case 'R':   if( f=='R' )    ray_lookup = rook_lookup;       // fall through
                case 'B':   if( f=='B' )    ray_lookup = bishop_lookup;     // fall through
                case 'N':
                {
                    char piece = tolower(f);
                    f = *natural_in++;
                    char src_file='\0';
                    char src_rank='\0';
                    if( f == 'x' )
                    {
                        capture = true;
                        f = *natural_in++;
                    }
                    if( '1'<=f && f<='8' )
                    {
                        src_rank = f;
                    }
                    else if( 'a'<= f && f<='h' )
                    {
                        src_file = f;
                    }
                    else
                        err = true;
                    if( !err )
                    {
                        char g = *natural_in++;
                        if( g == 'x' )
                        {
                            if( capture )
                                err = true;
                            else
                            {
                                capture = true;
                                g = *natural_in++;
                            }
                        }
                        if( '1'<=g && g<='8' )
                        {
                            if( src_file )
                            {
                                mv.dst = SQ(src_file,g);
                                src_file = '\0';
                            }
                            else
                                err = true;
                        }
                        else if( 'a'<=g && g<='h' )
                        {
                            char dst_rank = *natural_in++;
                            if( '1'<=dst_rank || dst_rank<='8' )
                                mv.dst = SQ(g,dst_rank);
                            else
                                err = true;
                        }
                        else
                            err = true;
                        if( !err )
                        {
                            if( capture ? IsWhite(cr->squares[mv.dst]) : cr->squares[mv.dst]==' ' )
                            {
                                mv.capture = cr->squares[mv.dst];
                                if( piece == 'n' )
                                {
                                    int count=0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = knight_lookup[mv.dst];
                                        lte nbr_moves = *ptr++;
                                        while( !found && nbr_moves-- )
                                        {
                                            Square src = (Square)*ptr++;
                                            if( cr->squares[src]=='n' )
                                            {
                                                bool src_file_ok = !src_file || FILE(src)==src_file;
                                                bool src_rank_ok = !src_rank || RANK(src)==src_rank;
                                                if( src_file_ok && src_rank_ok )
                                                {
                                                    mv.src = src;
                                                    if( probe == 0 )
                                                        count++;
                                                    else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                        // we are not exposing black king to check.
                                                    {
                                                        char temp = cr->squares[mv.dst];
                                                        cr->squares[mv.dst] = 'n';  // temporarily make move
                                                        cr->squares[mv.src] = ' ';
                                                        found = !cr->AttackedSquare( cr->bking_square, true ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                        cr->squares[mv.dst] = temp;  // now undo move
                                                        cr->squares[mv.src] = 'n';
                                                    }
                                                }
                                            }
                                        }
                                        if( probe==0 && count==1 )
                                            found = true; // done, no need for disambiguation by check
                                    }
                                }
                                else // if( rook, bishop, queen )
                                {
                                    int count = 0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = ray_lookup[mv.dst];
                                        lte nbr_rays = *ptr++;
                                        while( nbr_rays-- )
                                        {
                                            lte ray_len = *ptr++;
                                            while( ray_len-- )
                                            {
                                                Square src = (Square)*ptr++;
                                                if( !IsEmptySquare(cr->squares[src]) )
                                                {
                                                    // Any piece, friend or enemy must move to end of ray
                                                    ptr += ray_len;
                                                    ray_len = 0;
                                                    if( cr->squares[src] == piece )
                                                    {
                                                        bool src_file_ok = !src_file || FILE(src)==src_file;
                                                        bool src_rank_ok = !src_rank || RANK(src)==src_rank;
                                                        if( src_file_ok && src_rank_ok )
                                                        {
                                                            mv.src = src;
                                                            if( probe == 0 )
                                                                count++;
                                                            else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                                // we are not exposing black king to check.
                                                            {
                                                                char temp = cr->squares[mv.dst];
                                                                cr->squares[mv.dst] = piece;  // temporarily make move
                                                                cr->squares[mv.src] = ' ';
                                                                found = !cr->AttackedSquare( cr->bking_square, true ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                                cr->squares[mv.dst] = temp;  // now undo move
                                                                cr->squares[mv.src] = piece;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if( found )
    {
        char c = *natural_in;
        bool problem = (isascii(c) && isalnum(c));
        if( problem )
            found = false;
        else
            *this = mv;
    }
    return found;
}

/****************************************************************************
 * Read terse string move eg "g1f3"
 *  return bool okay
 ****************************************************************************/
bool Move::TerseIn( ChessRules *cr, const char *tmove )
{
    MOVELIST list;
    int i;
    bool okay=false;
    if( strlen(tmove)>=4 && 'a'<=tmove[0] && tmove[0]<='h'
                         && '1'<=tmove[1] && tmove[1]<='8'
                         && 'a'<=tmove[2] && tmove[2]<='h'
                         && '1'<=tmove[3] && tmove[3]<='8' )
    {
        Square src   = SQ(tmove[0],tmove[1]);
        Square dst   = SQ(tmove[2],tmove[3]);
        char   expected_promotion_if_any = 'Q';
        if( tmove[4] )
        {
            if( tmove[4]=='n' || tmove[4]=='N' )
                expected_promotion_if_any = 'N';
            else if( tmove[4]=='b' || tmove[4]=='B' )
                expected_promotion_if_any = 'B';
            else if( tmove[4]=='r' || tmove[4]=='R' )
                expected_promotion_if_any = 'R';
        }

        // Generate legal moves, then search for this move
        cr->GenLegalMoveList( &list );
        for( i=0; !okay && i<list.count; i++ )
        {
            if( list.moves[i].dst==dst && list.moves[i].src==src )
            {
                switch( list.moves[i].special )
                {
                    default:    okay=true;  break;
                    case SPECIAL_PROMOTION_QUEEN:
                    {
                        if( expected_promotion_if_any == 'Q' )
                            okay = true;
                        break;
                    }
                    case SPECIAL_PROMOTION_ROOK:
                    {
                        if( expected_promotion_if_any == 'R' )
                            okay = true;
                        break;
                    }
                    case SPECIAL_PROMOTION_BISHOP:
                    {
                        if( expected_promotion_if_any == 'B' )
                            okay = true;
                        break;
                    }
                    case SPECIAL_PROMOTION_KNIGHT:
                    {
                        if( expected_promotion_if_any == 'N' )
                            okay = true;
                        break;
                    }
                }
            }
            if( okay )
                *this = list.moves[i];
        }
    }
    return okay;
}

/****************************************************************************
 * Convert to natural string
 *    eg "Nf3"
 ****************************************************************************/
std::string Move::NaturalOut( ChessRules *cr )
{
    
// Improved algorithm

    /* basic procedure is run the following algorithms in turn:
        pawn move     ?
        castling      ?
        Nd2 or Nxd2   ? (loop through all legal moves check if unique)
        Nbd2 or Nbxd2 ? (loop through all legal moves check if unique)   
        N1d2 or N1xd2 ? (loop through all legal moves check if unique)
        Nb1d2 or Nb1xd2 (fallback if nothing else works)
    */

    char nmove[10];
    nmove[0] = '-';    
    nmove[1] = '-';    
    nmove[2] = '\0';    
    MOVELIST list;
    bool check[MAXMOVES];
    bool mate[MAXMOVES];
    bool stalemate[MAXMOVES];
    enum
    {
        ALG_PAWN_MOVE,
        ALG_CASTLING,
        ALG_ND2,
        ALG_NBD2,
        ALG_N1D2,
        ALG_NB1D2
    };
    bool done=false;
    bool found = false;
    char append='\0';
    cr->GenLegalMoveList( &list, check, mate, stalemate );
    Move mfound = list.moves[0];   // just to prevent a bogus compiler uninitialized var warning
    for( int i=0; !found && i<list.count; i++ )
    {
        mfound = list.moves[i];
        if( mfound == *this )
        {
            found = true;
            if( mate[i] )
                append = '#';
            else if( check[i] )
                append = '+';
        }
    }

    // Loop through algorithms
    for( int alg=ALG_PAWN_MOVE; found && !done && alg<=ALG_NB1D2; alg++ )
    {
        bool do_loop = (alg==ALG_ND2 || alg==ALG_NBD2 || alg==ALG_N1D2);
        int matches=0;
        Move m;

        // Run the algorithm on the input move (i=-1) AND on all legal moves
        //  in a loop if do_loop set for this algorithm (i=0 to i=count-1)
        for( int i=-1; !done && i<(do_loop?list.count:0); i++ )
        {
            char *str_dst;
            char compare[10];
            if( i == -1 )
            {
                m = *this;
                str_dst = nmove;
            }
            else
            {
                m = list.moves[i];
                str_dst = compare;
            }
            Square src = m.src;
            Square dst = m.dst;
            char t, p = cr->squares[src];
            if( islower(p) )
                p = (char)toupper(p);
            if( !IsEmptySquare(m.capture) ) // until we did it this way, enpassant was '-' instead of 'x'
                t = 'x';
            else
                t = '-';
            switch( alg )
            {
                // pawn move ? "e4" or "exf6", plus "=Q" etc if promotion
                case ALG_PAWN_MOVE:
                {
                    if( p == 'P' )
                    {
                        done = true;
                        if( t == 'x' )
                            sprintf( nmove, "%cx%c%c", FILE(src),FILE(dst),RANK(dst) );
                        else
                            sprintf( nmove, "%c%c",FILE(dst),RANK(dst) );
                        char *s = strchr(nmove,'\0');
                        switch( m.special )
                        {
                            case SPECIAL_PROMOTION_QUEEN:
                                strcpy( s, "=Q" );  break;
                            case SPECIAL_PROMOTION_ROOK:
                                strcpy( s, "=R" );  break;
                            case SPECIAL_PROMOTION_BISHOP:
                                strcpy( s, "=B" );  break;
                            case SPECIAL_PROMOTION_KNIGHT:
                                strcpy( s, "=N" );  break;
                            default:
                                break;
                        }
                    }
                    break;
                }

                // castling ?
                case ALG_CASTLING:
                {
                    if( m.special==SPECIAL_WK_CASTLING || m.special==SPECIAL_BK_CASTLING )
                    {
                        strcpy( nmove, "O-O" );
                        done = true;
                    }
                    else if( m.special==SPECIAL_WQ_CASTLING || m.special==SPECIAL_BQ_CASTLING )
                    {
                        strcpy( nmove, "O-O-O" );
                        done = true;
                    }
                    break;
                }

                // Nd2 or Nxd2
                case ALG_ND2:
                {
                    if( t == 'x' )
                        sprintf( str_dst, "%cx%c%c", p, FILE(dst), RANK(dst) );
                    else
                        sprintf( str_dst, "%c%c%c", p, FILE(dst), RANK(dst) );
                    if( i >= 0 )
                    {
                        if( 0 == strcmp(nmove,compare) )
                            matches++;
                    }
                    break;
                }

                // Nbd2 or Nbxd2    
                case ALG_NBD2:
                {
                    if( t == 'x' )
                        sprintf( str_dst, "%c%cx%c%c", p, FILE(src), FILE(dst), RANK(dst) );
                    else
                        sprintf( str_dst, "%c%c%c%c", p, FILE(src), FILE(dst), RANK(dst) );
                    if( i >= 0 )
                    {
                        if( 0 == strcmp(nmove,compare) )
                            matches++;
                    }
                    break;
                }

                // N1d2 or N1xd2
                case ALG_N1D2:
                {
                    if( t == 'x' )
                        sprintf( str_dst, "%c%cx%c%c", p, RANK(src), FILE(dst), RANK(dst) );
                    else
                        sprintf( str_dst, "%c%c%c%c", p, RANK(src), FILE(dst), RANK(dst) );
                    if( i >= 0 )
                    {
                        if( 0 == strcmp(nmove,compare) )
                            matches++;
                    }
                    break;
                }

                //  Nb1d2 or Nb1xd2
                case ALG_NB1D2:
                {
                    done = true;
                    if( t == 'x' )
                        sprintf( nmove, "%c%c%cx%c%c", p, FILE(src), RANK(src), FILE(dst), RANK(dst) );
                    else
                        sprintf( nmove, "%c%c%c%c%c", p, FILE(src), RANK(src), FILE(dst), RANK(dst) );
                    break;
                }
            }
        }   // end loop for all legal moves with given algorithm

        // If it's a looping algorithm and only one move matches nmove, we're done
        if( do_loop && matches==1 )
            done = true;
    }   // end loop for all algorithms
    if( append )
    {
        char *s = strchr(nmove,'\0');
        *s++ = append;
        *s = '\0';
    }
    return nmove;
}

/****************************************************************************
 * Convert to terse string eg "e7e8q"
 ****************************************************************************/
std::string Move::TerseOut()
{    
    char tmove[6];
    if( src == dst )   // null move should be "0000" according to UCI spec
    {
        tmove[0] = '0';
        tmove[1] = '0';
        tmove[2] = '0';
        tmove[3] = '0';
        tmove[4] = '\0';
    }
    else
    {
        tmove[0] = FILE(src);
        tmove[1] = RANK(src);
        tmove[2] = FILE(dst);
        tmove[3] = RANK(dst);
        if( special == SPECIAL_PROMOTION_QUEEN )
            tmove[4] = 'q';
        else if( special == SPECIAL_PROMOTION_ROOK )
            tmove[4] = 'r';
        else if( special == SPECIAL_PROMOTION_BISHOP )
            tmove[4] = 'b';
        else if( special == SPECIAL_PROMOTION_KNIGHT )
            tmove[4] = 'n';
        else
            tmove[4] = '\0';
        tmove[5] = '\0';
    }
    return tmove;
}    

