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
#include "ProgressBar.h"
#include "ListableGame.h"
#include "MemoryPositionSearchSide.h"
#include "PatternMatch.h"

// For standard algorithm, works for any game
struct MpsSlow
{
    int total_count_target;
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
    uint64_t slow_rank8_mask;
    uint64_t slow_rank7_mask;
    uint64_t slow_rank6_mask;
    uint64_t slow_rank5_mask;
    uint64_t slow_rank4_mask;
    uint64_t slow_rank3_mask;
    uint64_t slow_rank2_mask;
    uint64_t slow_rank1_mask;
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
    const uint64_t *rank8_target_ptr;
    const uint64_t *rank7_target_ptr;
    const uint64_t *rank6_target_ptr;
    const uint64_t *rank5_target_ptr;
    const uint64_t *rank4_target_ptr;
    const uint64_t *rank3_target_ptr;
    const uint64_t *rank2_target_ptr;
    const uint64_t *rank1_target_ptr;
    uint64_t rank3_target;
    uint64_t rank4_target;
    uint64_t rank5_target;
    uint64_t rank6_target;
    uint64_t rank7_target;
    uint64_t rank8_target;
    uint64_t rank1_target;
    uint64_t rank2_target;
    char     target_squares[64];
};

// This part is used for fast re-initiation at the start of each game
struct MpsQuickInit
{
    MpsSide side_white;
    MpsSide side_black;
    char squares[64];
};

struct DoSearchFoundGame
{
    int idx;            // index into memory db
    int game_id;
    unsigned short offset_first;
    unsigned short offset_last;
};

class MemoryPositionSearch
{
public:
    MemoryPositionSearch()
    {
        Init();
    }
    void Init();
    bool TryFastMode( MpsSide *side );
    bool SearchGameOptimisedNoPromotionAllowed( const char *moves_in, unsigned short &offset_first, unsigned short &offset_last  );    // much faster
    bool SearchGameSlowPromotionAllowed(  const std::string &moves_in, unsigned short &offset_first, unsigned short &offset_last  );          // semi fast
    bool PatternSearchGameOptimisedNoPromotionAllowed( PatternMatch &pm, bool &reverse, const char *moves_in, unsigned short &offset_first, unsigned short &offset_last  );    // much faster
    bool PatternSearchGameSlowPromotionAllowed( PatternMatch &pm, bool &reverse, const std::string &moves_in, unsigned short &offset_first, unsigned short &offset_last  );          // semi fast
    int  GetNbrGamesFound() { return games_found.size(); }
    std::vector< smart_ptr<ListableGame> > *search_source;
    std::vector< smart_ptr<ListableGame> >  &GetVectorSourceGames()   { return *search_source; }
    std::vector<DoSearchFoundGame>          &GetVectorGamesFound() { return games_found; }
    int  DoSearch( const thc::ChessPosition &cp, ProgressBar *progress );
    int  DoSearch( const thc::ChessPosition &cp, ProgressBar *progress, std::vector< smart_ptr<ListableGame> > *source );
    int  DoPatternSearch( PatternMatch &pm, ProgressBar *progress, PATTERN_STATS &stats );
    int  DoPatternSearch( PatternMatch &pm, ProgressBar *progress, PATTERN_STATS &stats, std::vector< smart_ptr<ListableGame> > *source );
    bool IsThisSearchPosition( const thc::ChessPosition &cp )
        { return search_position_set && cp==search_position; }

public:
    std::vector< smart_ptr<ListableGame> > in_memory_game_cache;

private:
    thc::ChessPosition search_position;
    bool search_position_set;
    std::vector<DoSearchFoundGame> games_found;
    MpsSlow      ms;
    MpsSlowInit  msi;
    MpsQuick     mq;
    MpsQuickInit mqi_init;
    MpsQuickInit mqi;
    uint64_t white_home_mask;
    uint64_t white_home_pawns;
    uint64_t black_home_mask;
    uint64_t black_home_pawns;
    void QuickGameInit()
    {
        mqi = mqi_init;
    }
    void SlowGameInit()
    {
        thc::ChessPosition *cp = static_cast<thc::ChessPosition *>(&msi.cr);
        cp->Init();
        msi.cr.squares[ thc::c1 ] = 'D';     // Impose the distinct dark squared bishop = 'd'/'D' convention over the top
        msi.cr.squares[ thc::f8 ] = 'd';
        msi.sides[0] = mqi_init.side_white;
        msi.sides[1] = mqi_init.side_black;
    }
    thc::Move UncompressSlowMode( char code );
    thc::Move UncompressFastMode( char code, MpsSide *side, MpsSide *other );
};

#endif // MEMORY_POSITION_SEARCH_H
