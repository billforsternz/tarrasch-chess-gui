/****************************************************************************
 * Chess classes - A raw chess position, could be used as a C style POD
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSPOSITIONRAW_H
#define CHESSPOSITIONRAW_H

#include "ChessDefs.h"

// TripleHappyChess
namespace thc
{

// ChessPositionRaw - A complete representation of the position on the
//  board. Corresponds broadly to fields of Forsyth representation
struct ChessPositionRaw
{
    // Who to play
    bool white;

    // Pieces on board in readable form; Forsyth like but without compression,
    //  eg "rnbqkbnr"
    //     "pppppppp"
    //     "        "
    //     "        "
    //     "        "
    //     "        "
    //     "PPPPPPPP"
    //     "RNBQKBNR"
    //  (represents starting position)
    char squares[64 +1]; // +1 allows a trailing '\0'
    // note indexed according to Square convention, a8=0 etc.

    // Half moves since pawn move or capture (for 50 move rule)
    //  eg after 1.e4 it's 0
    int  half_move_clock;

    // Full move count. Initially 1 and increments after black moves
    //  eg after 1.e4 it's 1
    //  eg after 1... d6 it's 2
    int  full_move_count;

    // The following are deemed "details", and must be stored at the
    //  end of the structure. Search for DETAIL for, ahem, details.
    //  For performance reasons we want the details to be able to fit
    //  into 32 bits.
    Square enpassant_target : 8;
    Square wking_square     : 8;
    Square bking_square     : 8;
    int  wking              : 1;    // Castling still allowed flags
    int  wqueen             : 1;    //  unfortunately if the castling
    int  bking              : 1;    //  flags are declared as bool, 
    int  bqueen             : 1;    //  with Visual C++ at least, 
                                    //  the details blow out and use
                                    //  another 32 bits (??!!)
    // Note that for say white king side castling to be allowed in
    //  in the same sense as the Forsyth representation, not only
    //  must wking be true, but the  white king and king rook must
    //  be present and in position, see the wking_allowed() etc
    //  methods in class ChessPosition, these are used for the ChessPosition
    //  == operator.
};

} //namespace thc

#endif //CHESSPOSITIONRAW_H
