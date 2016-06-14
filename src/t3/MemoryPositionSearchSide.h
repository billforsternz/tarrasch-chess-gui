/****************************************************************************
 *  In memory position search - fundamental data structure
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef MEMORY_POSITION_SEARCH_SIDE_H
#define MEMORY_POSITION_SEARCH_SIDE_H

// Same as Side in CompressMoves
struct MpsSide
{
    bool white;
    bool fast_mode;
    int rooks[2];       // locations of each dynamic piece
    int knights[2];     //
    int queens[2];      //
    int pawns[8];
    int bishop_dark;
    int bishop_light;
    int king;
    int nbr_pawns;      // 0-8
    int nbr_rooks;      // 0,1 or 2
    int nbr_knights;    // 0,1 or 2
    int nbr_queens;     // 0,1 or 2
    int nbr_light_bishops;  // 0 or 1
    int nbr_dark_bishops;   // 0 or 1
};

#endif //  MEMORY_POSITION_SEARCH_SIDE_H

