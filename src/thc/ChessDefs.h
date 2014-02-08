/****************************************************************************
 * Chess classes - Common definitions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSDEFS_H
#define CHESSDEFS_H
#include "Portability.h"

// TripleHappyChess
namespace thc
{

// Use the most natural square convention possible; Define Square to
//  correspond to a conventionally oriented chess diagram; Top left corner
//  (square a8) is 0, bottom right corner (square h1) is 63.
// Note that instead of defining a special piece type, we use the built in
//  char type, with 'N'=white knight, 'b'=black bishop etc. and ' '=an
//  empty square.
enum Square
{
    a8=0,
        b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
    SQUARE_INVALID
};

// Special (i.e. not ordinary) move types
enum SPECIAL
{           
    NOT_SPECIAL = 0,
    SPECIAL_KING_MOVE,     // special only because it changes wking_square, bking_square
    SPECIAL_WK_CASTLING,
    SPECIAL_BK_CASTLING,
    SPECIAL_WQ_CASTLING,
    SPECIAL_BQ_CASTLING,
    SPECIAL_PROMOTION_QUEEN,
    SPECIAL_PROMOTION_ROOK,
    SPECIAL_PROMOTION_BISHOP,
    SPECIAL_PROMOTION_KNIGHT,
    SPECIAL_WPAWN_2SQUARES,
    SPECIAL_BPAWN_2SQUARES,
    SPECIAL_WEN_PASSANT,
    SPECIAL_BEN_PASSANT,
};

// Results of a test for legal position, note that they are powers
//  of 2, allowing a mask of reasons
enum ILLEGAL_REASON
{
    IR_NULL=0, IR_PAWN_POSITION=1, //pawns on 1st or 8th rank
    IR_NOT_ONE_KING_EACH=2, IR_CAN_TAKE_KING=4,
    IR_WHITE_TOO_MANY_PIECES=8, IR_WHITE_TOO_MANY_PAWNS=16,
    IR_BLACK_TOO_MANY_PIECES=32, IR_BLACK_TOO_MANY_PAWNS=64
};

// Types of draw checked by IsDraw()
enum DRAWTYPE
{
    NOT_DRAW,
    DRAWTYPE_50MOVE,
    DRAWTYPE_INSUFFICIENT,      // draw if superior side wants it
                                //  since inferior side has insufficent
                                //  mating material
    DRAWTYPE_INSUFFICIENT_AUTO, // don't wait to be asked, eg draw
                                //  immediately if bare kings
    DRAWTYPE_REPITITION,
};

// Stalemate or checkmate game terminations
enum TERMINAL
{
    NOT_TERMINAL = 0,
    TERMINAL_WCHECKMATE = -1,   // White is checkmated
    TERMINAL_WSTALEMATE = -2,   // White is stalemated
    TERMINAL_BCHECKMATE = 1,    // Black is checkmated
    TERMINAL_BSTALEMATE = 2     // Black is stalemated
};

// Calculate an upper limit to the length of a list of moves   
#define MAXMOVES (27 + 2*13 + 2*14 + 2*8 + 8 + 8*4  +  3*27)
                //[Q   2*B    2*R    2*N   K   8*P] +  [3*Q]     
                //             ^                         ^       
                //[calculated practical maximum   ] + [margin]   

// We have developed an algorithm to compress any legal chess position,
//  including who to move, castling allowed flags and enpassant_target
//  into 24 bytes
union CompressedPosition
{
    unsigned char storage[24];
    unsigned int ints[ 24 / sizeof(unsigned int) ];
};

// Types we'd really rather have in PrivateChessDefs.h, but not possible
//  at the moment, so we reluctantly expose them to users of the chess
//  classes.
typedef unsigned char lte;   // lte = lookup table element
typedef int32_t DETAIL;

} //namespace thc

#endif // CHESSDEFS_H
