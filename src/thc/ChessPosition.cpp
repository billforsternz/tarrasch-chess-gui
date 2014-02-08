/****************************************************************************
 * Chess classes - Representation of the position on the board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

/* Some project notes */
/*====================*/

/****************************************************************************
 * Class Hierarchy:
 *
 *  The Chess classes are intended to provide a reusable chess subsystem. To
 *  understand the hierarchy of classes, think in terms of the Java concept
 *  of derived classes "extending" base classes.
 *
 *  So
 *    ChessPositionRaw
 *      ;A simple C style structure that holds a chess position
 *    ChessPosition   extends ChessPositionRaw
 *      ;ChessPosition adds constructors, and other simple facilities
 *    ChessRules      extends ChessPosition
 *      ;ChessRules adds all the rules of standard chess
 *    ChessEvaluation extends ChessRules
 *      ;ChessEvaluation is a simple scoring function for a chess engine
 *      ;it attempts to calculate a score (more +ve numbers for better white
 *      ;positions, more -ve numbers for better black positions) for a
 *      ;position. Tarrasch GUI uses this only to attempt to list the most
 *      ;plausible moves first when you click on a square that can receive
 *      ;multiple moves
 *    ChessEngine     extends ChessEvaluation
 *      ;ChessEngine adds a search tree to ChessEvaluation to make a
 *      ;complete simple engine (the Tarrasch Toy Engine)
 ****************************************************************************/

/****************************************************************************
 * Explanation of namespace thc:
 * 
 *  The Chess classes use the C++ namespace facility to ensure they can be
 *  used without name conflicts with other 3rd party code. A short, but
 *  highly likely to be unique namespace name is best, we choose "thc" which
 *  is short for TripleHappyChess.
 ****************************************************************************/

/****************************************************************************
 * My original license, now replaced by MIT license full text of which is
 * in file LICENSE in project's root directory.
 * 
 * Licensing provisions for all TripleHappy Chess source code;
 *
 * Start Date: 15 February 2003.
 * This software is licensed to be used freely. The following licensing
 * provisions apply;
 * 
 * 1) The 'author' is asserted to be the original author of the code, Bill
 *    Forster of Wellington, New Zealand.
 * 2) The 'licensee' is anyone else who wishes to use the software.
 * 3) The 'licensee' is entitled to do anything they wish with the software
 *    EXCEPT for any action that attempts to restrict in any way the rights
 *    granted in 4) below.
 * 4) The 'author' is entitled to do anything he wishes with the software.
 *
 * The intent of this license is to allow the licensees wide freedom to
 * incorporate, modify and sell the software, with the single caveat that
 * they cannot prevent the author from either further development or future
 * commercial use of the software.
 ****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ChessPosition.h"
#include "Move.h"
#include "PrivateChessDefs.h"
#include "HashLookup.h"
#include "DebugPrintf.h"
using namespace std;
using namespace thc;

// Return true if ChessPositions are the same (including counts)
bool ChessPosition::CmpStrict( const ChessPosition &other ) const
{
    return( *this == other  &&
            half_move_clock == other.half_move_clock &&
            full_move_count == other.full_move_count
         );
}

std::string ChessPosition::ToDebugStr( const char *label )
{
    std::string s;
    const char *p = squares;
    if( label )
        s = label;
    s += (white ? "\nWhite to move\n" : "\nBlack to move\n");
    for( int row=0; row<8; row++ )
    {
        for( int col=0; col<8; col++ )
        {
            char c = *p++;
            if( c==' ' )
                c = '.';
           s += c;
        }
        s +=  '\n';
    }
    return s;
}

/****************************************************************************
 * Set up position on board from Forsyth string with extensions
 *   return bool okay
 ****************************************************************************/
bool ChessPosition::Forsyth( const char *txt )
{    
    int   file, rank, skip, store, temp;
    int   count_wking=0, count_bking=0;
    char  c, cross;
    char  p;
    bool okay, done;
    const char *reset = txt;

    // When store==0 validate txt without storing results
    for( store=0, okay=true; store<2 && okay; store++ )
    {
        txt = reset;

        // Clear the board
        if( store )
        {
            for( Square square=a8; square<=h1; ++square )
            {
                squares[square]   = ' ';
            }

            // Clear the extension fields
            wking  = false;
            wqueen = false;
            bking  = false;
            bqueen = false;
            enpassant_target = SQUARE_INVALID;
            half_move_clock = 0;
            full_move_count = 1;
        }

        // Loop through the main Forsyth field
        for( file=0, rank=7, done=false; *txt && okay && !done; )
        {
            skip = 1;
            c = *txt++;
            p = ' ';
            cross = ' ';
            switch(c)
            {
                case 'x':   cross = 'x';
                            skip = 1;
                            break;   
                case ' ':
                case '\t':  done = true;            break;
                case 'k':   p = 'k';
                            count_bking++;          break;
                case 'K':   p = 'K';
                            count_wking++;          break;
                case 'p':   
                case 'r':   
                case 'n':   
                case 'b':   
                case 'q':   
                case 'P':   
                case 'R':   
                case 'N':   
                case 'B':   
                case 'Q':   p = c;   break;
                case '1':   case '2':   case '3':   case '4':
                case '5':   case '6':   case '7':   case '8':
                            skip = c-'0';       break;
                case '/':   // official separator
                case '|':   //  .. we'll allow this
                case '\\':  //  .. or this
                {
                    if( file == 0 )
                        skip = 0;        // eg after "rn6/", '/' does nothing
                    else
                        skip = (8-file); // eg after "rn/", '/' skips 6 squares
                    while( *txt == '/'   // allow consecutive '/' characters
                      ||   *txt == '|'   //     (or '|' or '\')
                      ||   *txt == '\\' )
                    {
                        txt++;
                        skip += 8;
                    }   
                    break;
                }
                default:    okay=false;
            }

            // Store single piece or loop over multiple empty squares
            for( int i=0; i<skip && okay && !done; i++ )
            {
                Square sq = SQ('a'+file, '1'+rank);
                if( store )
                {
                    squares[sq] = p;
                    if( p == 'K' )
                        wking_square = sq;
                    else if( p == 'k' )
                        bking_square = sq;
                }
                file++;
                if( file == 8 )
                {
                    file = 0;
                    rank--;
                }
                if( sq == h1 )
                    done = true;        // last square, we're done !            
            }
        }   

        // In validation pass make sure there's 1 white king and 1 black king
   /*   if( store==0 )  // disabled to allow ILLEGAL_NOT_ONE_KING_EACH message
        {
            if( count_wking!=1 || count_bking!=1 )
                okay = false;
        }  */

        // Now support standard FEN notation with extensions. See appendix to
        // .PGN standard on the Internet (and in project documentation)
        //
        // Briefly there are a total of 6 fields, the normal Forsyth encoding
        // (as above) plus 5 extension fields. Format of the 5 extension
        // fields is illustrated by example.
        //
        // Example extension fields; "w KQq e6 0 2"
        //
        // White to move,
        // All types of castling available, except black king side (so black's
        //  king rook has moved, other king and rook pieces unmoved). This
        //  whole field is replaced by '-' character if no castling available.
        // The last move played was a double pawn advance, and hence the
        //  'en-passant target square' field is NOT empty. The field is the
        //  square a potential capturing pawn would end up on (can deduce last
        //  move was e7e5)
        // Number of half-moves or ply since a pawn move or capture (for 50
        //  move rule)
        // Total (full-move) count (starts at 1, increments after black moves)

        // Who to move
        if( okay )
        {
            if( *txt == '/' 
             || *txt == '|' 
             || *txt == '\\' )
                txt++;
            while( *txt==' ' || *txt=='\t' )
                txt++;
            if( *txt=='W' || *txt=='w' )
            {
                if( store )
                    white = true;
                txt++;
            }
            else if( *txt=='B' || *txt=='b' )
            {
                if( store )
                    white = false;
                txt++;
            }
            else
                okay = false;
        }

        // Castling flags
        if( okay )
        {
            while( *txt==' ' || *txt=='\t' )
                txt++;
            if( *txt == '-' )
                txt++;
            else
            {
                for( int i=0; i<4 && okay; i++, txt++ )
                {
                    if( *txt == 'K' )
                    {
                        if( store )
                            wking = true;
                    }
                    else if( *txt == 'Q' )
                    {
                        if( store )
                            wqueen = true;
                    }
                    else if( *txt == 'k' )
                    {
                        if( store )
                            bking = true;
                    }
                    else if( *txt == 'q' )
                    {
                        if( store )
                            bqueen = true;
                    }
                    else if( *txt == '-' )               // allow say "KQ-q "
                        ;
                    else if( *txt == ' ' || *txt=='\t' ) // or "KQq "
                        break;
                    else
                        okay = false;
                }
            }
        }
            
        // Enpassant target
        if( okay )
        {
            while( *txt==' ' || *txt=='\t' )
                txt++;
            if( *txt == '-' )
                txt++;
            else
            {
                char f='a', r='1';
                if( 'a'<=*txt && *txt<='h' )
                    f = *txt++;
                else if( 'A'<=*txt && *txt<='H' )
                {
                    f = *txt++;
                    f = f-'A'+'a';
                }
                else
                    okay = false;
                if( okay )
                {
                    if( '1'<=*txt && *txt<='8' )
                        r = *txt++;
                    else
                        okay = false;
                }
                if( okay && store )
                    enpassant_target = SQ(f,r);
            }
        }

        // Half move clock
        if( okay )
        {
            okay=false;
            while( *txt==' ' || *txt=='\t' )
                txt++;
            temp = atoi(txt);
            if( temp >= 0 )
            {
                while( isascii(*txt) && isdigit(*txt) )
                {
                    okay = true;
                    txt++;
                }
                if( okay && store )
                    half_move_clock = temp;
            }
        }

        // Full move count
        if( okay )
        {
            while( *txt==' ' || *txt=='\t' )
                txt++;
            temp = atoi(txt);
            if( temp < 0 )
                okay = false;
            else if( store )
                full_move_count = temp;
        }
    }
    return( okay );
}


/****************************************************************************
 * Publish chess position and supplementary info in forsyth notation
 ****************************************************************************/
std::string ChessPosition::ForsythPublish()
{
    int i, empty=0, file=0, rank=7, save_file=0, save_rank=0;
    Square sq;
    char p;
    std::string str;

    // Squares
    for( i=0; i<64; i++ )
    {
        sq = SQ('a'+file,'1'+rank);
        if( sq == enpassant_target )
        {
            save_file = file;
            save_rank = rank;
        }           
        p = squares[sq];
        if( p == ' ' )
            empty++;
        else
        {
            if( empty )
            {
                char count = '0' + (char)empty;
                str += count;
                empty = 0;
            }
            str += p;
        }
        file++;
        if( file == 8 )
        {
            if( empty )
            {
                char count = '0'+(char)empty;
                str += count;
            }
            if( rank )
                str += '/';
            empty = 0;
            file = 0;
            rank--;
        }
    }

    // Who to move
    str += ' ';
    str += (white?'w':'b');

    // Castling flags
    str += ' ';
    if( !wking_allowed() && !wqueen_allowed() && !bking_allowed() && !bqueen_allowed() )
        str += '-';
    else
    {
        if( wking_allowed() )
            str += 'K';
        if( wqueen_allowed() )
            str += 'Q';
        if( bking_allowed() )
            str += 'k';
        if( bqueen_allowed() )
            str += 'q';
    }
    
    // Enpassant target square
    str += ' ';
    if( enpassant_target==SQUARE_INVALID || save_rank==0 )
        str += '-';
    else
    {
        char file = 'a'+(char)save_file;
        str += file;
        char rank = '1'+(char)save_rank;
        str += rank;
    }

    // Counts
    char buf[40];
    sprintf( buf, " %d %d", half_move_clock, full_move_count );
    str += buf;
    return( str );
}


/****************************************************************************
 * Compress chess position
 ****************************************************************************/
unsigned short ChessPosition::Compress( CompressedPosition &dst ) const
{
    ChessPosition src = *this;
    int i;

    // Optimisation: Clear the last 1/3 of the 24 bytes of storage only
    //  as the first 2/3 are cleared inherently as even 64 empty squares
    //  require 64 * 2 bits = 16 bytes.
    for( i = ( 2 * nbrof(dst.ints) ) / 3; i<nbrof(dst.ints); i++ )
    {
        dst.ints[i] = 0;
    }

    // Encode "castling possible" as opposition pawn on rook's home square
    //  (which is otherwise impossible)
    if( squares[e1] == 'K' )
    {
        if( src.wking && src.squares[h1]=='R' )
            src.squares[h1] = 'p';
        if( src.wqueen && src.squares[a1]=='R' )
            src.squares[a1] = 'p';
    }
    if( src.squares[e8] == 'k' )
    {
        if( src.bking && src.squares[h8]=='r' )
            src.squares[h8] = 'P';
        if( src.bqueen && src.squares[a8]=='r' )
            src.squares[a8] = 'P';
    }

    // Encode enpassant as friendly pawn on 1st rank (otherwise impossible).
    //  Since a friendly pawn must be on 4th rank, save the actual 1st rank
    //  occupant in the 4th rank square.
    // Don't encode it unless an enpassant move is actually possible. Eg
    // after 1.c3 e5 2.c4 we should have the sicilian defence position with
    // colours reversed. Don't make it different to the position after
    // 1.e4 c5 because c6 is an enpassant target in the latter case.

    // We now use a nice improved enpassant algorithm - the old ugly algorithm
    //  has a bug.
    //  To get the old book format exactly, use the ugly algorithm, change the
    //  book version back to 3, and undo the other bug fix in this file, by
    //  changing the hash shift from 16 (correct) back to 4 (incorrect - search
    //  for the other bug comment in this function).
    #if 1
    // White captures enpassant on a6,b6...h6
    Square ep_target = src.groomed_enpassant_target();
    if( src.white && ep_target!=SQUARE_INVALID )
    {
        int idx = ep_target+8; //idx = SOUTH(enpassant_target) is black pawn
        src.squares[idx] = src.squares[idx-24]; // store 1st rank
        src.squares[idx-24] = 'p';              // indicate ep
    }        

    // Black captures enpassant on a3,b3...h3
    else if( !src.white && ep_target!=SQUARE_INVALID )
    {
        int idx = ep_target-8; //idx = NORTH(enpassant_target) is white pawn
        src.squares[idx] = src.squares[idx+24]; // store 1st rank
        src.squares[idx+24] = 'P';              // indicate ep
    }        

    // This old ugly one has a bug ... (search for bug below)
    #else

    // White captures enpassant on a6,b6...h6
    bool swap = false;
    if( src.white && a6<=src.enpassant_target && src.enpassant_target<=h6 )
    {
        int idx = src.enpassant_target+8; //idx = SOUTH(src.enpassant_target)
        if( src.enpassant_target==a6 && src.squares[idx+1]=='P' )
            swap = true;
        else if( src.enpassant_target==h6 && src.squares[idx-1]=='P' )
            swap = true;
        else if( src.squares[idx-1]=='P' || src.squares[idx+1]=='P' )
            // BUG! src.enpassant_target can be a6 or h6 in this clause
            swap = true;
        if( swap )
        {
            src.squares[idx] = src.squares[idx-24]; // store 1st rank
            src.squares[idx-24] = 'p';              // indicate ep
        }
    }        
    
    // Black captures enpassant on a3,b3...h3
    else if( !src.white && a3<=src.enpassant_target && src.enpassant_target<=h3 )
    {
        int idx = src.enpassant_target-8; //idx = NORTH(src.enpassant_target)
        if( src.enpassant_target==a3 && src.squares[idx+1]=='p' )
            swap = true;
        else if( src.enpassant_target==h3 && src.squares[idx-1]=='p' )
            swap = true;
        else if( src.squares[idx-1]=='p' || src.squares[idx+1]=='p' )
            // BUG! src.enpassant_target can be a3 or h3 in this clause
            swap = true;
        if( swap )
        {
            src.squares[idx] = src.squares[idx+24]; // store 1st rank
            src.squares[idx+24] = 'P';              // indicate ep
        }
    }        
    #endif
    
    
    // Encode empty square with 2 bits, and other 12 possibilities with
    //  all the other possible nibbles that don't start with those 2 bits
    #define CEMPTY      2   // 10
    #define CWROOK      0   // 0000
    #define CWKNIGHT    1   // 0001
    #define CWBISHOP    2   // 0010
    #define CWQUEEN     3   // 0011
    #define CWKING      4   // 0100
    #define CWPAWN      5   // 0101
    #define CBROOK      6   // 0110
    #define CBKNIGHT    7   // 0111
    #define CBBISHOP   12   // 1100
    #define CBQUEEN    13   // 1101
    #define CBKING     14   // 1110
    #define CBPAWN     15   // 1111

    unsigned char c, *p, *base;
    int bit=7;
    base = p = dst.storage;
    bool second_king=false;
    int kings=0;
    bool err=false;
    for( i=0; !err && i<64; i++ )
    {
        if( p-base >= 24 )   // never need more than 24 bytes in legal position
        {
            err = true;
            break;
        }
        bool empty=false;
        c = CEMPTY;
        switch( src.squares[i] )
        {
            case 'R':   c = CWROOK;     break;
            case 'N':   c = CWKNIGHT;   break;
            case 'B':   c = CWBISHOP;   break;
            case 'Q':   c = CWQUEEN;    break;
            case 'K':   c = CWKING;     kings++; if( kings==2 ) second_king=true; break;
            case 'P':   c = CWPAWN;     break;
            case 'r':   c = CBROOK;     break;
            case 'n':   c = CBKNIGHT;   break;
            case 'b':   c = CBBISHOP;   break;
            case 'q':   c = CBQUEEN;    break;
            case 'k':   c = CBKING;     kings++; if( kings==2 ) second_king=true; break;
            case 'p':   c = CBPAWN;     break;
            case ' ':
            default:    empty = true;   break;
        }

        // Encode black to move as two kings same colour (2 white kings if
        //  white king first, 2 black kings otherwise)
        if( second_king )
        {
            second_king = false;
            if( !src.white )
            {
                if( c == CBKING )
                    c = CWKING;
                else
                    c = CBKING;
            }
        }

        // Shift and mask 2 or 4 bits into storage
        if( empty )
        {
            switch( bit )
            {
                case 7: *p = (CEMPTY<<6);
                        bit = 5;
                        break;
                case 5: *p = (*p&0xc0) | (CEMPTY<<4);
                        bit = 3;
                        break;
                case 3: *p = (*p&0xf0) | (CEMPTY<<2);
                        bit = 1;
                        break;
                case 1: *p = (*p&0xfc) | (CEMPTY);
                        p++;
                        bit = 7;
                        break;
            }
        }
        else
        {
            switch( bit )
            {
                case 7: *p = (c<<4);
                        bit = 3;
                        break;
                case 5: *p = (*p&0xc0) | (c<<2);
                        bit = 1;
                        break;
                case 3: *p = (*p&0xf0) | (c);
                        p++;
                        bit = 7;
                        break;
                case 1:
                {
                        *p = (*p&0xfc) | ((c>>2)&0x03);
                        p++;
                        if( p-base >= 24 )
                            err = true;
                        else
                        {
                            *p = ((c<<6)&0xc0);
                            bit = 5;
                        }
                        break;
                }
            }
        }
    }

    // Create hash
    unsigned int big_hash = 0;
    for( i=0; i<nbrof(dst.ints); i++ )
    {
        big_hash ^= dst.ints[i];
    }
    unsigned short hash=0;
    for( i=0; i < sizeof(unsigned int)/sizeof(unsigned short); i++ )
    {
        hash ^= (big_hash&0xffff);
        big_hash = big_hash >> 16;  // BUG FIX! This used to be >> 4 which
                                    //  results in much worse hash distribution
                                    //  than ideal. Fixed at the same time as
                                    //  change of book version from 3 -> 4
    }
    return hash;
}

/****************************************************************************
 * Decompress chess position
 ****************************************************************************/
void ChessPosition::Decompress( const CompressedPosition &src )
{
    unsigned char c;
    const unsigned char *p;
    int idx, bit=7;
    p = src.storage;
    char first_king='\0';
    bool second_king=false;
    int kings=0;

    // Clear some supplementary type info
    half_move_clock = 0;
    full_move_count = 1;
    enpassant_target = SQUARE_INVALID;
    white  = false;
    wking  = false;
    wqueen = false;
    bking  = false;
    bqueen = false;

    // The stages of the decompression process must be the reverse of the
    //  compression process. So unscramble 64 squares, then enpassant, then
    //  castling
    for( idx=0; idx<64; idx++ )
    {
        bool empty = false;
        c = ' ';
        switch( bit )
        {
            case 7:
            {
                if( ((*p>>6)&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 5;
                }
                else
                {
                    c = (*p>>4) & 0x0f;
                    bit = 3;
                }
                break;
            }
            case 5:
            {
                if( ((*p>>4)&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 3;
                }
                else
                {
                    c = (*p>>2) & 0x0f;
                    bit = 1;
                }
                break;
            }
            case 3:
            {
                if( ((*p>>2)&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 1;
                }
                else
                {
                    c = *p & 0x0f;
                    bit = 7;
                    p++;
                }
                break;
            }
            case 1:
            {
                if( (*p&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 7;
                    p++;
                }
                else
                {
                    c  = ( (*p<<2) & 0x0c );
                    bit = 5;
                    p++;
                    c |= ( (*p>>6) & 0x03 );
                }
                break;
            }
        }
        if( !empty )
        {
            switch( c )
            {
                default:         c = ' ';   break;
                case CWROOK:     c = 'R';   break;
                case CWKNIGHT:   c = 'N';   break;
                case CWBISHOP:   c = 'B';   break;
                case CWQUEEN:    c = 'Q';   break;
                case CWKING:     c = 'K';   kings++;
                                 if( kings == 1 )
                                     first_king = c;
                                 else if( kings == 2)
                                     second_king=true;
                                 break;
                case CWPAWN:     c = 'P';   break;
                case CBROOK:     c = 'r';   break;
                case CBKNIGHT:   c = 'n';   break;
                case CBBISHOP:   c = 'b';   break;
                case CBQUEEN:    c = 'q';   break;
                case CBKING:     c = 'k';   kings++;
                                 if( kings == 1 )
                                     first_king = c;
                                 else if( kings == 2)
                                     second_king=true;
                                 break;
                case CBPAWN:     c = 'p';   break;
            }
        }

        // Decode black to move as two kings same colour (2 white kings if
        //  white king first, 2 black kings otherwise)
        if( second_king )
        {
            second_king = false;
            white = (c!=first_king);
            if( !white )    // if black, swap second king
            {
                if( c == 'K' )
                    c = 'k';
                else
                    c = 'K';
            }
        }
        squares[idx] = c;
    }
    squares[idx] = '\0';    // idx=64, terminating '\0'

    // Decode enpassant as friendly pawn on 1st rank (otherwise impossible).
    //  Since a friendly pawn must be on 4th rank, save the actual 1st rank
    //  occupant in the 4th rank square.
    bool ep=false;
    for( idx=0; !ep && idx<8; idx++ )   // loop thru black 1st rank
    {
        if( squares[idx] == 'p' )
        {
            ep = true;
            squares[idx]     = squares[idx+24];
            squares[idx+24]  = 'p';
            enpassant_target = (Square)(idx+16);
        }
    }
    for( idx=56; !ep && idx<64; idx++ ) // loop thru white 1st rank
    {
        if( squares[idx] == 'P' )
        {
            ep = true;
            squares[idx]     = squares[idx-24];
            squares[idx-24]  = 'P';
            enpassant_target = (Square)(idx-16);
        }
    }
    
    // Decode "castling possible" as opposition pawn on rook's home square
    //  (which is otherwise impossible)
    if( squares[e1] == 'K' )
    {
        if( squares[h1] == 'p' )
        {
            squares[h1] = 'R';
            wking = true;
        }
        if( squares[a1] == 'p' )
        {
            squares[a1] = 'R';
            wqueen = true;
        }
    }
    if( squares[e8] == 'k' )
    {
        if( squares[h8] == 'P' )
        {
            squares[h8] = 'r';
            bking = true;
        }
        if( squares[a8] == 'P' )
        {
            squares[a8] = 'r';
            bqueen = true;
        }
    }
}

/****************************************************************************
 * Calculate a hash value for position (not same as CompressPosition algorithm hash)
 ****************************************************************************/
uint32_t ChessPosition::HashCalculate()
{
    int32_t hash = 0;
    char *p = squares;
    for( int i=0; i<64; i++ )
    {
        char c = *p++;
        if( c < 'B' )
            c = 'a';
        else if( c > 'r' )
            c = 'a';
        hash ^= hash_lookup[i][c-'B'];
    }
    return hash;
}


/****************************************************************************
 * Incremental hash value update
 ****************************************************************************/
uint32_t ChessPosition::HashUpdate( uint32_t hash_in, Move move )
{
    uint32_t hash = hash_in;
    switch( move.special )
    {
        default:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WK_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[g1][target-'B'];  // remove target piece
            hash ^= hash_lookup[g1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash_lookup[h1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[h1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[f1][target-'B'];  // remove target piece
            hash ^= hash_lookup[f1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BK_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[g8][target-'B'];  // remove target piece
            hash ^= hash_lookup[g8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash_lookup[h8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[h8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[f8][target-'B'];  // remove target piece
            hash ^= hash_lookup[f8][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WQ_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[c1][target-'B'];  // remove target piece
            hash ^= hash_lookup[c1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash_lookup[a1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[a1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[d1][target-'B'];  // remove target piece
            hash ^= hash_lookup[d1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BQ_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[c8][target-'B'];  // remove target piece
            hash ^= hash_lookup[c8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash_lookup[a8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[a8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[d8][target-'B'];  // remove target piece
            hash ^= hash_lookup[d8][piece-'B'];   // replace with moving piece
            
            break;
        }
        case SPECIAL_PROMOTION_QUEEN:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'Q':'q')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_ROOK:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'R':'r')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_BISHOP:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'B':'b')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_KNIGHT:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'N':'n')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WEN_PASSANT:
        {
            char piece  = 'P';
            char target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash_lookup[SOUTH(move.dst)]['p'-'B'];  // remove black pawn south of dst
            hash ^= hash_lookup[SOUTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
        case SPECIAL_BEN_PASSANT:
        {
            char piece  = 'p';
            char target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash_lookup[NORTH(move.dst)]['P'-'B'];  // remove white pawn north of dst
            hash ^= hash_lookup[NORTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
    }
    return hash;
}

/****************************************************************************
 * Calculate a hash value for position (64 bit version)
 ****************************************************************************/
uint64_t ChessPosition::Hash64Calculate()
{
    int64_t hash = 0;
    char *p = squares;
    for( int i=0; i<64; i++ )
    {
        char c = *p++;
        if( c < 'B' )
            c = 'a';
        else if( c > 'r' )
            c = 'a';
        hash ^= hash64_lookup[i][c-'B'];
    }
    return hash;
}

/****************************************************************************
 * Incremental hash value update (64 bit version)
 ****************************************************************************/
uint64_t ChessPosition::Hash64Update( uint64_t hash_in, Move move )
{
    uint64_t hash = hash_in;
    switch( move.special )
    {
        default:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WK_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash64_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[g1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[g1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash64_lookup[h1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[h1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[f1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[f1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BK_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash64_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[g8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[g8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash64_lookup[h8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[h8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[f8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[f8][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WQ_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash64_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[c1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[c1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash64_lookup[a1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[a1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[d1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[d1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BQ_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash64_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[c8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[c8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash64_lookup[a8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[a8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[d8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[d8][piece-'B'];   // replace with moving piece
            
            break;
        }
        case SPECIAL_PROMOTION_QUEEN:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'Q':'q')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_ROOK:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'R':'r')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_BISHOP:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'B':'b')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_KNIGHT:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'N':'n')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WEN_PASSANT:
        {
            char piece  = 'P';
            char target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash64_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash64_lookup[SOUTH(move.dst)]['p'-'B'];  // remove black pawn south of dst
            hash ^= hash64_lookup[SOUTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
        case SPECIAL_BEN_PASSANT:
        {
            char piece  = 'p';
            char target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash64_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash64_lookup[NORTH(move.dst)]['P'-'B'];  // remove white pawn north of dst
            hash ^= hash64_lookup[NORTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
    }
    return hash;
}

