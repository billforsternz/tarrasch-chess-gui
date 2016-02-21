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

// For standard algorithm, works for any game
struct MpsSlow
{
    int black_count_target;
    int black_pawn_count_target;
    int white_count_target;
    int white_pawn_count_target;
    uint64_t *slow_rank8_ptr;
    uint64_t *slow_rank7_ptr;
    uint64_t *slow_rank6_ptr;
    uint64_t *slow_rank5_ptr;
    uint64_t *slow_rank4_ptr;
    uint64_t *slow_rank3_ptr;
    uint64_t *slow_rank2_ptr;
    uint64_t *slow_rank1_ptr;
    uint64_t *slow_rank8_target_ptr;
    uint64_t *slow_rank7_target_ptr;
    uint64_t *slow_rank6_target_ptr;
    uint64_t *slow_rank5_target_ptr;
    uint64_t *slow_rank4_target_ptr;
    uint64_t *slow_rank3_target_ptr;
    uint64_t *slow_rank2_target_ptr;
    uint64_t *slow_rank1_target_ptr;
    uint64_t slow_white_home_mask;
    uint64_t slow_white_home_pawns;
    uint64_t slow_black_home_mask;
    uint64_t slow_black_home_pawns;
    char     slow_target_squares[64];
};

// This part is used for fast re-initiation at the start of each game
struct MpsSlowInit
{
    MpsSide sides[2];
    thc::ChessRules cr;
};

// For quick (or fast or accelerated) algorithm, use for games without promotions
struct MpsQuick
{
    int black_dark_bishop_target;
    int black_light_bishop_target;
    int black_rook_target;
    int black_queen_target;
    int black_knight_target;
    int black_pawn_target;
    int white_dark_bishop_target;
    int white_light_bishop_target;
    int white_rook_target;
    int white_queen_target;
    int white_knight_target;
    int white_pawn_target;
    uint64_t *rank8_ptr;
    uint64_t *rank7_ptr;
    uint64_t *rank6_ptr;
    uint64_t *rank5_ptr;
    uint64_t *rank4_ptr;
    uint64_t *rank3_ptr;
    uint64_t *rank2_ptr;
    uint64_t *rank1_ptr;
    uint64_t *rank8_target_ptr;
    uint64_t *rank7_target_ptr;
    uint64_t *rank6_target_ptr;
    uint64_t *rank5_target_ptr;
    uint64_t *rank4_target_ptr;
    uint64_t *rank3_target_ptr;
    uint64_t *rank2_target_ptr;
    uint64_t *rank1_target_ptr;
    char     target_squares[64];
};

// This part is used for fast re-initiation at the start of each game
struct MpsQuickInit
{
    MpsSide side_white;
    MpsSide side_black;
    char squares[64];
};

class MemoryPositionSearch
{
public:
    MemoryPositionSearch()
    {
        Init();
        hash_initial = msi.cr.Hash64Calculate();
    }
    bool TryFastMode( MpsSide *side );
    bool SearchGameBase( std::string &moves_in );   // the original version
    bool SearchGameOptimisedNoPromotionAllowed( std::string &moves_in );    // much faster
    bool SearchGameSlowPromotionAllowed(  std::string &moves_in );          // semi fast
    void Init()
    {
        thc::ChessPosition *cp = static_cast<thc::ChessPosition *>(&msi.cr);
        cp->Init();
        MpsSide      sides[2];
        sides[0].white=true;
        sides[0].fast_mode=false;
        sides[1].white=false;
        sides[1].fast_mode=false;
        TryFastMode( &sides[0]);
        TryFastMode( &sides[1]);
        mqi_init.side_white = sides[0];
        mqi_init.side_black = sides[1];
        memcpy( mqi_init.squares, "rnbqkdnrpppppppp................................PPPPPPPPRNDQKBNR", 64 );
        mq.rank8_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[ 0]);
        mq.rank7_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[ 8]);
        mq.rank6_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[16]);
        mq.rank5_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[24]);
        mq.rank4_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[32]);
        mq.rank3_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[40]);
        mq.rank2_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[48]);
        mq.rank1_ptr = reinterpret_cast<uint64_t*>(&mqi.squares[56]);
        mq.rank8_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[ 0]);
        mq.rank7_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[ 8]);
        mq.rank6_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[16]);
        mq.rank5_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[24]);
        mq.rank4_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[32]);
        mq.rank3_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[40]);
        mq.rank2_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[48]);
        mq.rank1_target_ptr = reinterpret_cast<uint64_t*>(&mq.target_squares[56]);
        ms.slow_rank8_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[ 0]);
        ms.slow_rank7_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[ 8]);
        ms.slow_rank6_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[16]);
        ms.slow_rank5_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[24]);
        ms.slow_rank4_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[32]);
        ms.slow_rank3_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[40]);
        ms.slow_rank2_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[48]);
        ms.slow_rank1_ptr = reinterpret_cast<uint64_t*>(&msi.cr.squares[56]);
        ms.slow_rank8_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[ 0]);
        ms.slow_rank7_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[ 8]);
        ms.slow_rank6_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[16]);
        ms.slow_rank5_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[24]);
        ms.slow_rank4_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[32]);
        ms.slow_rank3_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[40]);
        ms.slow_rank2_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[48]);
        ms.slow_rank1_target_ptr = reinterpret_cast<uint64_t*>(&ms.slow_target_squares[56]);
    }
    int  DoSearch( const thc::ChessPosition &cp, uint64_t position_hash );
    bool GetGameidFromRow( int row, int &game_id );

public:
    std::vector< std::pair<int,std::string> > in_memory_game_cache;

private:
    std::vector<int> games_found;
    MpsSlow      ms;
    MpsSlowInit  msi;
    MpsQuick     mq;
    MpsQuickInit mqi_init;
    MpsQuickInit mqi;
    uint64_t white_home_mask;
    uint64_t white_home_pawns;
    uint64_t black_home_mask;
    uint64_t black_home_pawns;
    uint64_t hash_target;
    uint64_t hash_initial;
    void QuickGameInit()
    {
        mqi = mqi_init;
    }
    void SlowGameInit()
    {
        thc::ChessPosition *cp = static_cast<thc::ChessPosition *>(&msi.cr);
        cp->Init();
        msi.sides[0] = mqi_init.side_white;
        msi.sides[1] = mqi_init.side_black;
    }
    thc::Move UncompressSlowMode( char code );
    thc::Move UncompressFastMode( char code, MpsSide *side, MpsSide *other );
};

#endif // MEMORY_POSITION_SEARCH_H
