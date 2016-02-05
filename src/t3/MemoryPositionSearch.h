/****************************************************************************
 *  In memory position search - find position in games without database index
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef MEMORY_POSITION_SEARCH_H
#define MEMORY_POSITION_SEARCH_H

#include <algorithm>
#include <vector>
#include <string>
#include "thc.h"

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

class MemoryPositionSearch
{
public:
    MemoryPositionSearch()
    {
        sides[0].white=true;
        sides[0].fast_mode=false;
        sides[1].white=false;
        sides[1].fast_mode=false;
        Init();
        hash_initial = cr.Hash64Calculate();
    }
    bool TryFastMode( MpsSide *side );
    bool SearchGame( std::string &moves_in );
    thc::Move UncompressMove( char c );
    void Init() { thc::ChessPosition *cp = static_cast<thc::ChessPosition *>(&cr); cp->Init(); TryFastMode( &sides[0]); TryFastMode( &sides[1]); }
    int  DoSearch( uint64_t position_hash );
    bool GetGameidFromRow( int row, int &game_id );
    
public:
    thc::ChessRules cr;
    std::vector< std::pair<int,std::string> > in_memory_game_cache;

private:
    uint64_t hash_target;
    uint64_t hash_initial;
    std::vector<int> games_found;
    MpsSide sides[2];
    thc::Move UncompressSlowMode( char code );
    thc::Move UncompressFastMode( char code, MpsSide *side, MpsSide *other );
};

#endif // MEMORY_POSITION_SEARCH_H
