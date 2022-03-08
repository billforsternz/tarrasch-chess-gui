/****************************************************************************
 *  In memory position search - find position in games without database index
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <wx/utils.h>
#include "AutoTimer.h"
#include "ProgressBar.h"
#include "MemoryPositionSearch.h"
#include "CompressMoves.h"  //temp testing

// Note that much of this code duplicates the algorithms implemented
// in class CompressMoves - This is because we want to play through games
// encoded by CompressMoves extremely quickly - which precludes us using
// the slow(ish) CompressMoves code. Well, actually it's not that slow
// but it is organised around converting to and from thc::Move which we
// don't want to do - we want to act as if the compressed moves are
// the native move representation. So in a way we proved that the
// ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES refinement makes sense.

// Comment below from CompressMoves implementation;
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

#define K_VECTOR_N          0x01
#define K_VECTOR_NE         0x02
#define K_VECTOR_E          0x03
#define K_VECTOR_SE         0x04
#define K_VECTOR_S          0x05
#define K_VECTOR_SW         0x06
#define K_VECTOR_W          0x07
#define K_VECTOR_NW         0x08
#define K_K_CASTLING        0x09
#define K_Q_CASTLING        0x0b

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

// We are moving towards '.' instead of ' ', but we can't rely on that until
//  thc.h uses '.' exclusively. So for now we still use space
#define STILL_USING_SPACE
#ifdef  STILL_USING_SPACE
    #define EMPTY_CHARACTER ' '
    #define START_POSITION "rnbqkdnrpppppppp                                PPPPPPPPRNDQKBNR"  // note used of 'd'/'D' for dark squared bishop
#else
    #define EMPTY_CHARACTER '.'
    #define START_POSITION "rnbqkdnrpppppppp................................PPPPPPPPRNDQKBNR"
#endif


void MemoryPositionSearch::Init()
{
    in_memory_game_cache.clear();
    search_position_set=false;
    search_source = &in_memory_game_cache;
    thc::ChessPosition *cp = static_cast<thc::ChessPosition *>(&msi.cr);
    cp->Init();
    msi.cr.squares[ thc::c1 ] = 'D';     // Impose the distinct dark squared bishop = 'd'/'D' convention over the top
    msi.cr.squares[ thc::f8 ] = 'd';
    MpsSide      sides[2];
    sides[0].white=true;
    sides[0].fast_mode=false;
    sides[1].white=false;
    sides[1].fast_mode=false;
    TryFastMode( &sides[0]);
    TryFastMode( &sides[1]);
    mqi_init.side_white = sides[0];
    mqi_init.side_black = sides[1];
    msi.sides[0] = sides[0];
    msi.sides[1] = sides[0];
    memcpy( mqi_init.squares, START_POSITION, 64 );
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
bool MemoryPositionSearch::TryFastMode( MpsSide *side )
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
        if( msi.cr.squares[j] == (side->white?'P':'p') )
        {
            if( side->nbr_pawns < 8 )
                side->pawns[side->nbr_pawns++] = j;
            else
                okay = false;
        }

        // Other pieces are traversed in normal square convention order
        if( msi.cr.squares[i] == (side->white?'R':'r') )
        {
            if( side->nbr_rooks < 2 )
                side->rooks[side->nbr_rooks++] = i;
            else
                okay = false;
        }
        else if( msi.cr.squares[i] == (side->white?'N':'n') )
        {
            if( side->nbr_knights < 2 )
                side->knights[side->nbr_knights++] = i;
            else
                okay = false;
        }
        else if( msi.cr.squares[i] == (side->white?'D':'d') )
        {
            side->bishop_dark = i;
            if( side->nbr_dark_bishops < 1 )
                side->nbr_dark_bishops++;
            else
                okay = false;
        }
        else if( msi.cr.squares[i] == (side->white?'B':'b') )
        {
            side->bishop_light = i;
            if( side->nbr_light_bishops < 1 )
                side->nbr_light_bishops++;
            else
                okay = false;
        }
        else if( msi.cr.squares[i] == (side->white?'Q':'q') )
        {
            if( side->nbr_queens < 2 )
                side->queens[side->nbr_queens++] = i;
            else
                okay = false;
        }
        else if( msi.cr.squares[i] == (side->white?'K':'k') )
        {
            side->king = i;
        }
    }
    if( side->nbr_queens==2 && side->nbr_pawns>6 )
        okay = false;
    side->fast_mode = okay;
    return okay;
}

int  MemoryPositionSearch::DoSearch( const thc::ChessPosition &cp, ProgressBar *progress )
{
    return DoSearch(cp,progress,&in_memory_game_cache);
}

int  MemoryPositionSearch::DoSearch( const thc::ChessPosition &cp, ProgressBar *progress, std::vector< smart_ptr<ListableGame> > *source )
{
    games_found.clear();
    search_position = cp;
    search_position_set = true;
    search_source = source;

    // Set up counts of total pieces, and individual pieces in the target position
    ms.total_count_target = 64;     // reverse count non-pieces from 64
    ms.black_count_target = 0;
    ms.black_pawn_count_target = 0;
    ms.white_count_target = 0;
    ms.white_pawn_count_target = 0;
    mq.white_pawn_target = 0;
    mq.white_rook_target = 0;
    mq.white_knight_target = 0;
    mq.white_light_bishop_target = 0;
    mq.white_dark_bishop_target = 0;
    mq.white_queen_target = 0;
    mq.black_pawn_target = 0;
    mq.black_rook_target = 0;
    mq.black_knight_target = 0;
    mq.black_light_bishop_target = 0;
    mq.black_dark_bishop_target = 0;
    mq.black_queen_target = 0;
    for( int i=0; i<64; i++ )
    {
        char piece = cp.squares[i];
        switch(piece)
        {
            case 'P':
            {
                ms.white_count_target++;
                ms.white_pawn_count_target++;
                mq.white_pawn_target++;
                break;
            }
            case 'R':
            {
                ms.white_count_target++;
                mq.white_rook_target++;
                break;
            }
            case 'N':
            {
                ms.white_count_target++;
                mq.white_knight_target++;
                break;
            }
            case 'B':
            {
                ms.white_count_target++;
                if( !is_dark(i)  )
                    mq.white_light_bishop_target++;
                else
                {
                    mq.white_dark_bishop_target++;
                    piece = 'D';
                }
                break;
            }
            case 'Q':
            {
                ms.white_count_target++;
                mq.white_queen_target++;
                break;
            }
            case 'K':
            {
                ms.total_count_target--;  // kings don't count for this target
                break;
            }
            case 'p':
            {
                ms.black_count_target++;
                ms.black_pawn_count_target++;
                mq.black_pawn_target++;
                break;
            }
            case 'r':
            {
                ms.black_count_target++;
                mq.black_rook_target++;
                break;
            }
            case 'n':
            {
                ms.black_count_target++;
                mq.black_knight_target++;
                break;
            }
            case 'b':
            {
                ms.black_count_target++;
                if( !is_dark(i)  )
                    mq.black_light_bishop_target++;
                else
                {
                    mq.black_dark_bishop_target++;
                    piece = 'd';
                }
                break;
            }
            case 'q':
            {
                ms.black_count_target++;
                mq.black_queen_target++;
                break;
            }
            case 'k':
            {
                ms.total_count_target--;  // kings don't count for this target
                break;
            }
            default:
            {
                piece = ' ';
                ms.total_count_target--;
                break;
            }
        }

        // Also set up a target position, with the refinement that D/d is a dark bishop B/b is a light bishop
        ms.slow_target_squares[i] = piece;
        mq.target_squares[i] = piece;
    }

    // Set up white_home_mask and white_home_pawns to support the following logic;
    //  bool home_pawns_still_in_place = ((white_home_mask&*mq.rank2_ptr) == white_home_pawns );
    //
    //  A home row pawn from the target position must be in place in all positions leading
    //   to the target position - if it's not then this game doesn't include the target position
    //   e.g. After 1.e4 we can already conclude the game is never going to include a position
    //   in which a white pawn is on e2
    char *mask  = reinterpret_cast<char *>(&white_home_mask);
    char *pawns = reinterpret_cast<char *>(&white_home_pawns);
    for( int i=0; i<8; i++ )
    {
        if( mq.target_squares[48+i] == 'P' )
        {
            mask[i] = '\x7f';
            pawns[i] = 'P';
        }
        else
        {
            mask[i] = 0;
            pawns[i] = 0;
        }
    }

    // Do the same for the black home row
    mask  = reinterpret_cast<char *>(&black_home_mask);
    pawns = reinterpret_cast<char *>(&black_home_pawns);
    for( int i=0; i<8; i++ )
    {
        if( mq.target_squares[8+i] == 'p' )
        {
            mask[i] = '\x7f';
            pawns[i] = 'p';
        }
        else
        {
            mask[i] = 0;
            pawns[i] = 0;
        }
    }

    mq.rank3_target = *mq.rank3_target_ptr;
    mq.rank4_target = *mq.rank4_target_ptr;
    mq.rank5_target = *mq.rank5_target_ptr;
    mq.rank6_target = *mq.rank6_target_ptr;
    mq.rank7_target = *mq.rank7_target_ptr;
    mq.rank8_target = *mq.rank8_target_ptr;
    mq.rank1_target = *mq.rank1_target_ptr;
    mq.rank2_target = *mq.rank2_target_ptr;
    int nbr = source->size();
    {
        AutoTimer at("Search time");

        // Leave only one defined
        //#define CONSERVATIVE
        //#define NO_PROMOTIONS_FLAWED
        #define CORRECT_BEST_PRACTICE
        for( int i=0; i<nbr; i++ )
        {
            smart_ptr<ListableGame> p = (*source)[i];
            const char *fen = p->Fen();
            if( fen && *fen )
                continue;   // a partial game in the clipboard
            DoSearchFoundGame dsfg;
            dsfg.idx = i;
            dsfg.game_id = p->game_id;
            dsfg.offset_first=0;
            dsfg.offset_last=0;
            /* Roster r = in_memory_game_cache[i]->RefRoster();
            cprintf( "idx=%d, white=%s[%s], black=%s[%s], blob=%s\n",
                        in_memory_game_cache[i]->game_id,
                        in_memory_game_cache[i]->White(),  r.white.c_str(),
                        in_memory_game_cache[i]->Black(),  r.black.c_str(),
                        in_memory_game_cache[i]->CompressedMoves() ); */
            bool promotion_in_game = p->TestPromotion();
            bool game_found;
            #ifdef CONSERVATIVE
            game_found = SearchGameSlowPromotionAllowed( std::string(p->CompressedMoves()), dsfg.offset_first, dsfg.offset_last  );
            #endif
            #ifdef NO_PROMOTIONS_FLAWED
            game_found = SearchGameOptimisedNoPromotionAllowed( std::string(p->CompressedMoves()), dsfg.offset_first, dsfg.offset_last  );
            #endif
            #ifdef CORRECT_BEST_PRACTICE
            if( promotion_in_game )
                game_found = SearchGameSlowPromotionAllowed( std::string(p->CompressedMoves()), dsfg.offset_first, dsfg.offset_last  );
            else
                game_found = SearchGameOptimisedNoPromotionAllowed( p->CompressedMoves(), dsfg.offset_first, dsfg.offset_last );
            #endif
            if( game_found )
            {
                games_found.push_back( dsfg );
            }
            if( (i&0xff)==0 && progress )
                progress->Perfraction( i, nbr );
        }
    }
    return games_found.size();
}

int  MemoryPositionSearch::DoPatternSearch( PatternMatch &pm, ProgressBar *progress, PATTERN_STATS &stats )
{
    return DoPatternSearch(pm,progress,stats,&in_memory_game_cache);
}

//static bool debug_trigger;

int  MemoryPositionSearch::DoPatternSearch( PatternMatch &pm, ProgressBar *progress, PATTERN_STATS &stats, std::vector< smart_ptr<ListableGame> > *source )
{
    games_found.clear();
    search_position = pm.parm.cp;
    search_position_set = true;
    search_source = source;

    // Set up counts of total pieces, and individual pieces in the target position
    ms.total_count_target = 64;     // reverse count non-pieces from 64
    ms.black_count_target = 0;
    ms.black_pawn_count_target = 0;
    ms.white_count_target = 0;
    ms.white_pawn_count_target = 0;
    mq.white_pawn_target = 0;
    mq.white_rook_target = 0;
    mq.white_knight_target = 0;
    mq.white_light_bishop_target = 0;
    mq.white_dark_bishop_target = 0;
    mq.white_queen_target = 0;
    mq.black_pawn_target = 0;
    mq.black_rook_target = 0;
    mq.black_knight_target = 0;
    mq.black_light_bishop_target = 0;
    mq.black_dark_bishop_target = 0;
    mq.black_queen_target = 0;
    for( int i=0; i<64; i++ )
    {
        char piece = pm.parm.cp.squares[i];
        switch(piece)
        {
            case 'P':
            {
                ms.white_count_target++;
                ms.white_pawn_count_target++;
                mq.white_pawn_target++;
                break;
            }
            case 'R':
            {
                ms.white_count_target++;
                mq.white_rook_target++;
                break;
            }
            case 'N':
            {
                ms.white_count_target++;
                mq.white_knight_target++;
                break;
            }
            case 'B':
            {
                ms.white_count_target++;
                if( !is_dark(i)  )
                    mq.white_light_bishop_target++;
                else
                {
                    mq.white_dark_bishop_target++;
                    piece = 'D';
                }
                break;
            }
            case 'Q':
            {
                ms.white_count_target++;
                mq.white_queen_target++;
                break;
            }
            case 'K':
            {
                ms.total_count_target--;  // kings don't count for this target
                break;
            }
            case 'p':
            {
                ms.black_count_target++;
                ms.black_pawn_count_target++;
                mq.black_pawn_target++;
                break;
            }
            case 'r':
            {
                ms.black_count_target++;
                mq.black_rook_target++;
                break;
            }
            case 'n':
            {
                ms.black_count_target++;
                mq.black_knight_target++;
                break;
            }
            case 'b':
            {
                ms.black_count_target++;
                if( !is_dark(i)  )
                    mq.black_light_bishop_target++;
                else
                {
                    mq.black_dark_bishop_target++;
                    piece = 'd';
                }
                break;
            }
            case 'q':
            {
                ms.black_count_target++;
                mq.black_queen_target++;
                break;
            }
            case 'k':
            {
                ms.total_count_target--;  // kings don't count for this target
                break;
            }
            default:
            {
                piece = ' ';
                ms.total_count_target--;
                break;
            }
        }

        // Also set up a target position, with the refinement that D/d is a dark bishop B/b is a light bishop
        mq.target_squares[i] = piece;
        ms.slow_target_squares[i] = piece;
    }
    cprintf( "total_count_target=%d\n", ms.total_count_target );

    // Set up white_home_mask and white_home_pawns to support the following logic;
    //  bool home_pawns_still_in_place = ((white_home_mask&*mq.rank2_ptr) == white_home_pawns );
    //
    //  A home row pawn from the target position must be in place in all positions leading
    //   to the target position - if it's not then this game doesn't include the target position
    //   e.g. After 1.e4 we can already conclude the game is never going to include a position
    //   in which a white pawn is on e2
    char *mask  = reinterpret_cast<char *>(&white_home_mask);
    char *pawns = reinterpret_cast<char *>(&white_home_pawns);
    for( int i=0; i<8; i++ )
    {
        if( mq.target_squares[48+i] == 'P' )
        {
            mask[i] = '\x7f';
            pawns[i] = 'P';
        }
        else
        {
            mask[i] = 0;
            pawns[i] = 0;
        }
    }

    // Do the same for the black home row
    mask  = reinterpret_cast<char *>(&black_home_mask);
    pawns = reinterpret_cast<char *>(&black_home_pawns);
    for( int i=0; i<8; i++ )
    {
        if( mq.target_squares[8+i] == 'p' )
        {
            mask[i] = '\x7f';
            pawns[i] = 'p';
        }
        else
        {
            mask[i] = 0;
            pawns[i] = 0;
        }
    }

    // Set up the pattern mask
    pm.Prime(&msi.cr);
    mq.rank3_target = *mq.rank3_target_ptr;
    mq.rank4_target = *mq.rank4_target_ptr;
    mq.rank5_target = *mq.rank5_target_ptr;
    mq.rank6_target = *mq.rank6_target_ptr;
    mq.rank7_target = *mq.rank7_target_ptr;
    mq.rank8_target = *mq.rank8_target_ptr;
    mq.rank1_target = *mq.rank1_target_ptr;
    mq.rank2_target = *mq.rank2_target_ptr;
    int nbr = source->size();
    {
        AutoTimer at("Search time");
        for( int i=0; i<nbr; i++ )
        {
            smart_ptr<ListableGame> p = (*source)[i];
            //if( 0 == strcmp(p->White(),"Gu, Xiaobing") &&  0 == strcmp(p->Black(),"Ryjanova, Julia")  )
            //    debug_trigger = true;
            DoSearchFoundGame dsfg;
            dsfg.idx = i;
            dsfg.game_id = p->game_id;
            dsfg.offset_first=0;
            dsfg.offset_last=0;
            bool promotion_in_game = p->TestPromotion();
            bool game_found, reverse;
            pm.NewGame();
            if( promotion_in_game )
                game_found = PatternSearchGameSlowPromotionAllowed( pm, reverse, std::string(p->CompressedMoves()), dsfg.offset_first, dsfg.offset_last  );
            else
                game_found = PatternSearchGameOptimisedNoPromotionAllowed( pm, reverse, p->CompressedMoves(), dsfg.offset_first, dsfg.offset_last );
            if( game_found )
            {
                stats.nbr_games++;
                if( reverse )
                {
                    stats.nbr_reversed_games++;
                    if( 0 == strcmp(p->Result(),"1-0") )
                        stats.black_wins++;
                    else if( 0 == strcmp(p->Result(),"0-1") )
                        stats.white_wins++;
                    else
                        stats.draws++;
                }
                else
                {
                    if( 0 == strcmp(p->Result(),"1-0") )
                        stats.white_wins++;
                    else if( 0 == strcmp(p->Result(),"0-1") )
                        stats.black_wins++;
                    else
                        stats.draws++;
                }
                games_found.push_back( dsfg );
            }
            if( (i&0xff)==0 && progress )
            {
                double permill = (static_cast<double>(i) * 1000.0) / static_cast<double>(nbr);
                progress->Permill( static_cast<int>(permill) );
            }
        }
    }
    return games_found.size();
}

thc::Move MemoryPositionSearch::UncompressSlowMode( char code )
{
    // Horrible kludge necessitated by our 'd'/'D' = dark bishops stuff
    thc::ChessRules temp = msi.cr;
    for( int i=0; i<64; i++ )
    {
        char c = msi.cr.squares[i];
        if( c=='d' )
            temp.squares[i] = 'b';
        if( c=='D' )
            temp.squares[i] = 'B';
    }

    // Support algorithm ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES
    temp.wking = 1;
    temp.wqueen = 1;
    temp.bking = 1;
    temp.bqueen = 1;

    // Generate a list of all legal moves, in string form, sorted
    std::vector<thc::Move> moves;
    temp.GenLegalMoveList( moves );
    std::vector<std::string> moves_alpha;

    // Coding scheme relies on 254 valid codes 0x02-0xff and one error code 0x01,
    size_t len = moves.size();
    for( size_t i=0; i<len; i++ )
    {
        std::string s = moves[i].TerseOut();
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
    mv.TerseIn( &temp, the_move.c_str() );
    return mv;
}

thc::Move MemoryPositionSearch::UncompressFastMode( char code, MpsSide *side, MpsSide *other )
{
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
            switch( code&0x0f )     // 0, 1, 2
            {                       // 8, 9, 10
                                    // 16,17,18
                case K_VECTOR_NW:    delta = -9; break;  // 0-9
                case K_VECTOR_N:     delta = -8; break;  // 1-9
                case K_VECTOR_NE:    delta = -7; break;  // 2-9
                case K_VECTOR_W:     delta = -1; break;  // 8-9
                case K_VECTOR_E:     delta =  1; break;  // 10-9
                case K_VECTOR_SW:    delta =  7; break;  // 16-9
                case K_VECTOR_S:     delta =  8; break;  // 17-9
                case K_VECTOR_SE:    delta =  9; break;  // 18-9
                case K_K_CASTLING:
                {
                    special = msi.cr.white ? thc::SPECIAL_WK_CASTLING : thc::SPECIAL_BK_CASTLING;
                    delta = 2;
                    int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                    side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                    // note that there is no way the rooks ordering can swap during castling
                    break;
                }
                case K_Q_CASTLING:
                {
                    special = msi.cr.white ? thc::SPECIAL_WQ_CASTLING : thc::SPECIAL_BQ_CASTLING;
                    delta = -2;
                    int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                    side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                    // note that there is no way the rooks ordering can swap during castling
                    break;
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
            int knight_offset = ((code&N_HI) ? 1 : 0 );;
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
            bool white = msi.cr.white;
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
                    if( !isalpha(msi.cr.squares[src+delta]) )
                        special = (white ? thc::SPECIAL_WEN_PASSANT : thc::SPECIAL_BEN_PASSANT);
                    break;
                }
                case P_RIGHT:
                {
                    reordering_possible = true;
                    delta = white?-7:7;
                    if( !isalpha(msi.cr.squares[src+delta]) )
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
    char capture = msi.cr.squares[capture_location];
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
            case 'd':
            {
                other->nbr_dark_bishops--;
                break;
            }
            case 'b':
            {
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


#define WHITE_HOME_ROW_TEST ((*mq.rank2_ptr & white_home_mask) == white_home_pawns)
#define BLACK_HOME_ROW_TEST ((*mq.rank7_ptr & black_home_mask) == black_home_pawns)
#define SLOW_WHITE_HOME_ROW_TEST ((*ms.slow_rank2_ptr & white_home_mask) == white_home_pawns)
#define SLOW_BLACK_HOME_ROW_TEST ((*ms.slow_rank7_ptr & black_home_mask) == black_home_pawns)


bool MemoryPositionSearch::SearchGameOptimisedNoPromotionAllowed( const char *moves_in, unsigned short &offset_first, unsigned short &offset_last )
{
    unsigned short offset=0;
    bool target_white = search_position.white;  // searching for position with white to move?
    QuickGameInit();
    for(;;)
    {
        // Check for match before every move
        if(
            target_white &&
            #if 1
            *mq.rank3_ptr == mq.rank3_target &&
            *mq.rank4_ptr == mq.rank4_target &&
            *mq.rank5_ptr == mq.rank5_target &&
            *mq.rank6_ptr == mq.rank6_target &&
            *mq.rank7_ptr == mq.rank7_target &&
            *mq.rank8_ptr == mq.rank8_target &&
            *mq.rank1_ptr == mq.rank1_target &&
            *mq.rank2_ptr == mq.rank2_target
            #else
            *mq.rank3_ptr == *mq.rank3_target_ptr &&
            *mq.rank4_ptr == *mq.rank4_target_ptr &&
            *mq.rank5_ptr == *mq.rank5_target_ptr &&
            *mq.rank6_ptr == *mq.rank6_target_ptr &&
            *mq.rank7_ptr == *mq.rank7_target_ptr &&
            *mq.rank8_ptr == *mq.rank8_target_ptr &&
            *mq.rank1_ptr == *mq.rank1_target_ptr &&
            *mq.rank2_ptr == *mq.rank2_target_ptr
            #endif
        )
        {
            offset_last = offset_first = offset;    // later - separate offset_first and offset_last
            return true;
        }

        // White move
        char code = *moves_in++;
        offset++;
        int src=0;
        int dst=0;
        int hi_nibble = code&0xf0;
        char captured=EMPTY_CHARACTER;
        switch( hi_nibble )
        {
            case CODE_KING:
            {
                src = mqi.side_white.king;
                int delta=0;
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case 0: return false;   // CODE_KING = 0, so '\0' string terminator
                    case K_VECTOR_NW:    delta = -9; break;  // 0-9
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_NE:    delta = -7; break;  // 2-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_SW:    delta =  7; break;  // 16-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:    delta =  9; break;  // 18-9
                    case K_K_CASTLING:
                    {
                        // Idea: replace this with a single 32 bit ptr store of ' RK '
                        delta = 2;
                        int rook_offset = (mqi.side_white.rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                        mqi.side_white.rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src+1] = 'R';    //f1
                        mqi.squares[src+3] = EMPTY_CHARACTER;    //h1
                        break;
                    }
                    case K_Q_CASTLING:
                    {
                        delta = -2;
                        int rook_offset = (mqi.side_white.rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                        mqi.side_white.rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src-1] = 'R';    //d1
                        mqi.squares[src-4] = EMPTY_CHARACTER;    //a1
                        break;
                    }
                }
                mqi.side_white.king = dst = src+delta;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'K';
                mqi.squares[src] = EMPTY_CHARACTER;
                break;
            }

            case CODE_ROOK_LO:
            case CODE_ROOK_HI:
            {
                int rook_offset = (hi_nibble==CODE_ROOK_LO ? 0 : 1 );
                src = mqi.side_white.rooks[rook_offset];
                if( code & R_RANK )                // code encodes rank ?
                {
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                }
                else
                {
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                }
                mqi.side_white.rooks[rook_offset] = dst;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'R';
                mqi.squares[src] = EMPTY_CHARACTER;

                // swap ?
                if( mqi.side_white.nbr_rooks==2 && mqi.side_white.rooks[0]>mqi.side_white.rooks[1] )
                {
                    int temp = mqi.side_white.rooks[0];
                    mqi.side_white.rooks[0] = mqi.side_white.rooks[1];
                    mqi.side_white.rooks[1] = temp;
                }
                break;
            }

            case CODE_BISHOP_DARK:
            {
                src = mqi.side_white.bishop_dark;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=b8(1), dst=h2(55), file_delta=6  -> 9*6 =54
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'D';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.bishop_dark = dst;
                break;
            }

            case CODE_BISHOP_LIGHT:
            {
                src = mqi.side_white.bishop_light;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=g8(6), dst=a2(48), file_delta=6  -> 7*6 =42
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'B';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.bishop_light = dst;
                break;
            }

            case CODE_QUEEN_ROOK:
            {
                src = mqi.side_white.queens[0];
                if( code & R_RANK )                // code encodes rank ?
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                else
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'Q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.queens[0] = dst;
                break;
            }

            case CODE_QUEEN_BISHOP:
            {
                src = mqi.side_white.queens[0];
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'Q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.queens[0] = dst;
                break;
            }

            case CODE_KNIGHT:
            {
                int knight_offset = ((code&N_HI) ? 1 : 0 );
                src = mqi.side_white.knights[knight_offset];
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
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'N';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.knights[knight_offset] = dst;

                // swap ?
                if( mqi.side_white.nbr_knights==2 && mqi.side_white.knights[0]>mqi.side_white.knights[1] )
                {
                    int temp = mqi.side_white.knights[0];
                    mqi.side_white.knights[0] = mqi.side_white.knights[1];
                    mqi.side_white.knights[1] = temp;
                }
                break;
            }

            // PAWN
            default:
            {
                int pawn_offset = (code>>4)&0x07;
                src = mqi.side_white.pawns[pawn_offset];
                int delta;
                switch( code&0x0f )
                {
                    default:
                    {
                        return false;   // this code doesn't handle promotions
                    }

                    case P_DOUBLE:
                    {
                        delta = -16;
                        dst = src+delta;
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        if( !((*mq.rank2_ptr & white_home_mask) == white_home_pawns) ) //WHITE_HOME_ROW_TEST )
                            return false;
                        break;
                    }
                    case P_SINGLE:
                    {
                        delta = -8;
                        dst = src+delta;
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        if( 48<=src && src<56 && !WHITE_HOME_ROW_TEST )
                            return false;
                        break;
                    }
                    case P_LEFT:
                    {
                        delta = -9;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'p';     // en-passant
                            mqi.squares[dst=src-1] = EMPTY_CHARACTER;
                        }
                        else if( 48<=src && src<56 && !WHITE_HOME_ROW_TEST )
                            return false;

                        // Decreasing capture possibly requires reordering White pawns
                        for( int i=pawn_offset; i-1>=0 && pawn_ordering[mqi.side_white.pawns[i-1]]>pawn_ordering[mqi.side_white.pawns[i]]; i-- )
                        {
                            int temp = mqi.side_white.pawns[i-1];
                            mqi.side_white.pawns[i-1] = mqi.side_white.pawns[i];
                            mqi.side_white.pawns[i] = temp;
                        }
                        break;
                    }

                    case P_RIGHT:
                    {
                        delta = -7;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'p';     // en-passant
                            mqi.squares[dst=src+1] = EMPTY_CHARACTER;
                        }
                        else if( 48<=src && src<56 && !WHITE_HOME_ROW_TEST )    // en-passant capture can't be from home row
                            return false;

                        // Increasing capture possibly requires reordering White pawns
                        for( int i=pawn_offset; i+1<mqi.side_white.nbr_pawns && pawn_ordering[mqi.side_white.pawns[i]]>pawn_ordering[mqi.side_white.pawns[i+1]]; i++ )
                        {
                            int temp = mqi.side_white.pawns[i];
                            mqi.side_white.pawns[i] = mqi.side_white.pawns[i+1];
                            mqi.side_white.pawns[i+1] = temp;
                        }
                        break;
                    }
                }
            }  // end switch on pawn types
        }   // end switch all pieces

        // White captures black
        if( captured != EMPTY_CHARACTER )
        {
            switch( captured )
            {
                case 'p':
                {
                    if( 8<=dst && dst<16 && !BLACK_HOME_ROW_TEST )
                        return false;
                    bool zap=false;
                    for( int i=0; i<mqi.side_black.nbr_pawns-1; i++ )
                    {
                        if( zap )
                            mqi.side_black.pawns[i] = mqi.side_black.pawns[i+1];
                        else if( mqi.side_black.pawns[i] == dst )
                        {
                            zap = true;
                            mqi.side_black.pawns[i] = mqi.side_black.pawns[i+1];
                        }
                    }
                    if( --mqi.side_black.nbr_pawns < mq.black_pawn_target )
                        return false;
                    break;
                }
                case 'q':
                {
                    if( mq.black_queen_target == 1 )
                        return false;
                    break;
                }
                case 'r':
                {
                    if( mqi.side_black.nbr_rooks==2 && mqi.side_black.rooks[0]==dst )
                        mqi.side_black.rooks[0] = mqi.side_black.rooks[1];
                    if( --mqi.side_black.nbr_rooks < mq.black_rook_target )
                        return false;
                    break;
                }
                case 'n':
                {
                    if( mqi.side_black.nbr_knights==2 && mqi.side_black.knights[0]==dst )
                        mqi.side_black.knights[0] = mqi.side_black.knights[1];
                    if( --mqi.side_black.nbr_knights < mq.black_knight_target )
                        return false;
                    break;
                }
                case 'b':
                {
                    if( mq.black_light_bishop_target == 1 )
                        return false;
                    break;
                }
                case 'd':
                {
                    if( mq.black_dark_bishop_target == 1 )
                        return false;
                    break;
                }
            }
        }

        if(
            !target_white &&
            #if 1
            *mq.rank3_ptr == mq.rank3_target &&
            *mq.rank4_ptr == mq.rank4_target &&
            *mq.rank5_ptr == mq.rank5_target &&
            *mq.rank6_ptr == mq.rank6_target &&
            *mq.rank7_ptr == mq.rank7_target &&
            *mq.rank8_ptr == mq.rank8_target &&
            *mq.rank1_ptr == mq.rank1_target &&
            *mq.rank2_ptr == mq.rank2_target
            #else
            *mq.rank3_ptr == *mq.rank3_target_ptr &&
            *mq.rank4_ptr == *mq.rank4_target_ptr &&
            *mq.rank5_ptr == *mq.rank5_target_ptr &&
            *mq.rank6_ptr == *mq.rank6_target_ptr &&
            *mq.rank7_ptr == *mq.rank7_target_ptr &&
            *mq.rank8_ptr == *mq.rank8_target_ptr &&
            *mq.rank1_ptr == *mq.rank1_target_ptr &&
            *mq.rank2_ptr == *mq.rank2_target_ptr
            #endif
        )
        {
            offset_last = offset_first = offset;    // later - separate offset_first and offset_last
            return true;
        }

        // Black move
        code = *moves_in++;
        offset++;
        src=0;
        dst=0;
        hi_nibble = code&0xf0;
        captured=EMPTY_CHARACTER;
        switch( hi_nibble )
        {
            case CODE_KING:
            {
                src = mqi.side_black.king;
                int delta=0;
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case 0: return false;   // CODE_KING = 0, so '\0' string terminator
                    case K_VECTOR_NW:    delta = -9; break;  // 0-9
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_NE:    delta = -7; break;  // 2-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_SW:    delta =  7; break;  // 16-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:    delta =  9; break;  // 18-9
                    case K_K_CASTLING:
                    {
                        delta = 2;
                        int rook_offset = (mqi.side_black.rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                        mqi.side_black.rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src+1] = 'r';    //f8
                        mqi.squares[src+3] = EMPTY_CHARACTER;    //h8
                        break;
                    }
                    case K_Q_CASTLING:
                    {
                        delta = -2;
                        int rook_offset = (mqi.side_black.rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                        mqi.side_black.rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src-1] = 'r';    //d8
                        mqi.squares[src-4] = EMPTY_CHARACTER;    //a8
                        break;
                    }
                }
                mqi.side_black.king = dst = src+delta;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'k';
                mqi.squares[src] = EMPTY_CHARACTER;
                break;
            }

            case CODE_ROOK_LO:
            case CODE_ROOK_HI:
            {
                int rook_offset = (hi_nibble==CODE_ROOK_LO ? 0 : 1 );
                src = mqi.side_black.rooks[rook_offset];
                if( code & R_RANK )                // code encodes rank ?
                {
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                }
                else
                {
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                }
                mqi.side_black.rooks[rook_offset] = dst;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'r';
                mqi.squares[src] = EMPTY_CHARACTER;

                // swap ?
                if( mqi.side_black.nbr_rooks==2 && mqi.side_black.rooks[0]>mqi.side_black.rooks[1] )
                {
                    int temp = mqi.side_black.rooks[0];
                    mqi.side_black.rooks[0] = mqi.side_black.rooks[1];
                    mqi.side_black.rooks[1] = temp;
                }
                break;
            }

            case CODE_BISHOP_DARK:
            {
                src = mqi.side_black.bishop_dark;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=b8(1), dst=h2(55), file_delta=6  -> 9*6 =54
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'd';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.bishop_dark = dst;
                break;
            }

            case CODE_BISHOP_LIGHT:
            {
                src = mqi.side_black.bishop_light;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=g8(6), dst=a2(48), file_delta=6  -> 7*6 =42
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'b';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.bishop_light = dst;
                break;
            }

            case CODE_QUEEN_ROOK:
            {
                src = mqi.side_black.queens[0];
                if( code & R_RANK )                // code encodes rank ?
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                else
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.queens[0] = dst;
                break;
            }

            case CODE_QUEEN_BISHOP:
            {
                src = mqi.side_black.queens[0];
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.queens[0] = dst;
                break;
            }

            case CODE_KNIGHT:
            {
                int knight_offset = ((code&N_HI) ? 1 : 0 );
                src = mqi.side_black.knights[knight_offset];
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
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'n';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.knights[knight_offset] = dst;

                // swap ?
                if( mqi.side_black.nbr_knights==2 && mqi.side_black.knights[0]>mqi.side_black.knights[1] )
                {
                    int temp = mqi.side_black.knights[0];
                    mqi.side_black.knights[0] = mqi.side_black.knights[1];
                    mqi.side_black.knights[1] = temp;
                }
                break;
            }

            // PAWN
            default:
            {
                int pawn_offset = (code>>4)&0x07;
                src = mqi.side_black.pawns[pawn_offset];
                int delta;
                switch( code&0x0f )
                {
                    default:
                    {
                        return false;   // this code doesn't handle promotions
                    }

                    case P_DOUBLE:
                    {
                        delta = 16;
                        dst = src+delta;
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        if( !BLACK_HOME_ROW_TEST )
                            return false;
                        break;
                    }
                    case P_SINGLE:
                    {
                        delta = 8;
                        dst = src+delta;
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        if( 8<=src && src<16 && !BLACK_HOME_ROW_TEST )
                            return false;
                        break;
                    }
                    case P_LEFT:
                    {
                        delta = 9;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'P';     // en-passant
                            mqi.squares[dst=src+1] = EMPTY_CHARACTER;
                        }
                        else if( 8<=src && src<16 && !BLACK_HOME_ROW_TEST )
                            return false;

                        // Increasing capture possibly requires reordering Black pawns
                        for( int i=pawn_offset; i+1<mqi.side_black.nbr_pawns && pawn_ordering[mqi.side_black.pawns[i]]>pawn_ordering[mqi.side_black.pawns[i+1]]; i++ )
                        {
                            int temp = mqi.side_black.pawns[i];
                            mqi.side_black.pawns[i] = mqi.side_black.pawns[i+1];
                            mqi.side_black.pawns[i+1] = temp;
                        }
                        break;
                    }

                    case P_RIGHT:
                    {
                        delta = 7;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'P';     // en-passant
                            mqi.squares[dst=src-1] = EMPTY_CHARACTER;
                        }
                        else if( 8<=src && src<16 && !BLACK_HOME_ROW_TEST )    // en-passant capture can't be from home row
                            return false;

                        // Decreasing capture possibly requires reordering Black pawns
                        for( int i=pawn_offset; i-1>=0 && pawn_ordering[mqi.side_black.pawns[i-1]]>pawn_ordering[mqi.side_black.pawns[i]]; i-- )
                        {
                            int temp = mqi.side_black.pawns[i-1];
                            mqi.side_black.pawns[i-1] = mqi.side_black.pawns[i];
                            mqi.side_black.pawns[i] = temp;
                        }
                        break;
                    }
                }
            }  // end switch on pawn types
        }   // end switch all pieces

        // Black captures white
        if( captured != EMPTY_CHARACTER )
        {
            switch( captured )
            {
                case 'P':
                {
                    if( 48<=dst && dst<56 && !WHITE_HOME_ROW_TEST )
                        return false;
                    bool zap=false;
                    for( int i=0; i<mqi.side_white.nbr_pawns-1; i++ )
                    {
                        if( zap )
                            mqi.side_white.pawns[i] = mqi.side_white.pawns[i+1];
                        else if( mqi.side_white.pawns[i] == dst )
                        {
                            zap = true;
                            mqi.side_white.pawns[i] = mqi.side_white.pawns[i+1];
                        }
                    }
                    if( --mqi.side_white.nbr_pawns < mq.white_pawn_target )
                        return false;
                    break;
                }
                case 'Q':
                {
                    if( mq.white_queen_target == 1 )
                        return false;
                    break;
                }
                case 'R':
                {
                    if( mqi.side_white.nbr_rooks==2 && mqi.side_white.rooks[0]==dst )
                        mqi.side_white.rooks[0] = mqi.side_white.rooks[1];
                    if( --mqi.side_white.nbr_rooks < mq.white_rook_target )
                        return false;
                    break;
                }
                case 'N':
                {
                    if( mqi.side_white.nbr_knights==2 && mqi.side_white.knights[0]==dst )
                        mqi.side_white.knights[0] = mqi.side_white.knights[1];
                    if( --mqi.side_white.nbr_knights < mq.white_knight_target )
                        return false;
                    break;
                }
                case 'B':
                {
                    if( mq.white_light_bishop_target == 1 )
                        return false;
                    break;
                }
                case 'D':
                {
                    if( mq.white_dark_bishop_target == 1 )
                        return false;
                    break;
                }
            }
        }
    }
    //return false;     // unreachable
}

bool MemoryPositionSearch::PatternSearchGameOptimisedNoPromotionAllowed( PatternMatch &pm, bool &reverse, const char *moves_in, unsigned short &offset_first, unsigned short &offset_last )
{
    unsigned short offset=0;
    QuickGameInit();
    int total_count=30;     // 32 - 2 kings
    for(;;)
    {

        // Check for match before every move
        /*if( debug_trigger )
        {
            char buf[9];
            buf[8] = '\0';
            cprintf( "before\n" );
            for( int i=0; i<64; i+=8 )
            {
                memcpy(buf,&mqi.squares[i],8);
                for( int k=0; k<8; k++ )
                    if( buf[k] == ' ' ) buf[k]='.';
                cprintf( "%s\n", buf );
            }
        } */
        bool match = pm.Test( reverse, &mqi.side_white, &mqi.side_black, true, mqi.squares, false );
        if( match )
        {
            /*if( debug_trigger )
            {
                cprintf( "Match!\n" );
                debug_trigger = false;
            } */
            offset_last = offset_first = offset;    // later - separate offset_first and offset_last
            return true;
        }
        #define SIMPLE_PATTERN_COUNT_OPTIMISATION
        #ifdef SIMPLE_PATTERN_COUNT_OPTIMISATION
        if( total_count < ms.total_count_target )
            return false;
        #endif

        // White move
        char code = *moves_in++;
        offset++;
        int src=0;
        int dst=0;
        int hi_nibble = code&0xf0;
        char captured=EMPTY_CHARACTER;
        switch( hi_nibble )
        {
            case CODE_KING:
            {
                src = mqi.side_white.king;
                int delta=0;
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case 0: return false;   // CODE_KING = 0, so '\0' string terminator
                    case K_VECTOR_NW:    delta = -9; break;  // 0-9
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_NE:    delta = -7; break;  // 2-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_SW:    delta =  7; break;  // 16-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:    delta =  9; break;  // 18-9
                    case K_K_CASTLING:
                    {
                        // Idea: replace this with a single 32 bit ptr store of ' RK '
                        delta = 2;
                        int rook_offset = (mqi.side_white.rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                        mqi.side_white.rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src+1] = 'R';    //f1
                        mqi.squares[src+3] = EMPTY_CHARACTER;    //h1
                        break;
                    }
                    case K_Q_CASTLING:
                    {
                        delta = -2;
                        int rook_offset = (mqi.side_white.rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                        mqi.side_white.rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src-1] = 'R';    //d1
                        mqi.squares[src-4] = EMPTY_CHARACTER;    //a1
                        break;
                    }
                }
                mqi.side_white.king = dst = src+delta;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'K';
                mqi.squares[src] = EMPTY_CHARACTER;
                break;
            }

            case CODE_ROOK_LO:
            case CODE_ROOK_HI:
            {
                int rook_offset = (hi_nibble==CODE_ROOK_LO ? 0 : 1 );
                src = mqi.side_white.rooks[rook_offset];
                if( code & R_RANK )                // code encodes rank ?
                {
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                }
                else
                {
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                }
                mqi.side_white.rooks[rook_offset] = dst;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'R';
                mqi.squares[src] = EMPTY_CHARACTER;

                // swap ?
                if( mqi.side_white.nbr_rooks==2 && mqi.side_white.rooks[0]>mqi.side_white.rooks[1] )
                {
                    int temp = mqi.side_white.rooks[0];
                    mqi.side_white.rooks[0] = mqi.side_white.rooks[1];
                    mqi.side_white.rooks[1] = temp;
                }
                break;
            }

            case CODE_BISHOP_DARK:
            {
                src = mqi.side_white.bishop_dark;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=b8(1), dst=h2(55), file_delta=6  -> 9*6 =54
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'D';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.bishop_dark = dst;
                break;
            }

            case CODE_BISHOP_LIGHT:
            {
                src = mqi.side_white.bishop_light;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=g8(6), dst=a2(48), file_delta=6  -> 7*6 =42
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'B';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.bishop_light = dst;
                break;
            }

            case CODE_QUEEN_ROOK:
            {
                src = mqi.side_white.queens[0];
                if( code & R_RANK )                // code encodes rank ?
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                else
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'Q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.queens[0] = dst;
                break;
            }

            case CODE_QUEEN_BISHOP:
            {
                src = mqi.side_white.queens[0];
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'Q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.queens[0] = dst;
                break;
            }

            case CODE_KNIGHT:
            {
                int knight_offset = ((code&N_HI) ? 1 : 0 );
                src = mqi.side_white.knights[knight_offset];
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
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'N';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_white.knights[knight_offset] = dst;

                // swap ?
                if( mqi.side_white.nbr_knights==2 && mqi.side_white.knights[0]>mqi.side_white.knights[1] )
                {
                    int temp = mqi.side_white.knights[0];
                    mqi.side_white.knights[0] = mqi.side_white.knights[1];
                    mqi.side_white.knights[1] = temp;
                }
                break;
            }

            // PAWN
            default:
            {
                int pawn_offset = (code>>4)&0x07;
                src = mqi.side_white.pawns[pawn_offset];
                int delta;
                switch( code&0x0f )
                {
                    default:
                    {
                        return false;   // this code doesn't handle promotions
                    }

                    case P_DOUBLE:
                    {
                        delta = -16;
                        dst = src+delta;
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        break;
                    }
                    case P_SINGLE:
                    {
                        delta = -8;
                        dst = src+delta;
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        break;
                    }
                    case P_LEFT:
                    {
                        delta = -9;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'p';     // en-passant
                            mqi.squares[dst=src-1] = EMPTY_CHARACTER;
                        }

                        // Decreasing capture possibly requires reordering White pawns
                        for( int i=pawn_offset; i-1>=0 && pawn_ordering[mqi.side_white.pawns[i-1]]>pawn_ordering[mqi.side_white.pawns[i]]; i-- )
                        {
                            int temp = mqi.side_white.pawns[i-1];
                            mqi.side_white.pawns[i-1] = mqi.side_white.pawns[i];
                            mqi.side_white.pawns[i] = temp;
                        }
                        break;
                    }

                    case P_RIGHT:
                    {
                        delta = -7;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'P';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_white.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'p';     // en-passant
                            mqi.squares[dst=src+1] = EMPTY_CHARACTER;
                        }

                        // Increasing capture possibly requires reordering White pawns
                        for( int i=pawn_offset; i+1<mqi.side_white.nbr_pawns && pawn_ordering[mqi.side_white.pawns[i]]>pawn_ordering[mqi.side_white.pawns[i+1]]; i++ )
                        {
                            int temp = mqi.side_white.pawns[i];
                            mqi.side_white.pawns[i] = mqi.side_white.pawns[i+1];
                            mqi.side_white.pawns[i+1] = temp;
                        }
                        break;
                    }
                }
            }  // end switch on pawn types
        }   // end switch all pieces

        // White captures black
        if( captured != EMPTY_CHARACTER )
        {
            total_count--;
            switch( captured )
            {
                case 'p':
                {
                    bool zap=false;
                    for( int i=0; i<mqi.side_black.nbr_pawns-1; i++ )
                    {
                        if( zap )
                            mqi.side_black.pawns[i] = mqi.side_black.pawns[i+1];
                        else if( mqi.side_black.pawns[i] == dst )
                        {
                            zap = true;
                            mqi.side_black.pawns[i] = mqi.side_black.pawns[i+1];
                        }
                    }
                    --mqi.side_black.nbr_pawns;
                    break;
                }
                case 'q':
                {
                    --mqi.side_black.nbr_queens;
                    break;
                }
                case 'r':
                {
                    if( mqi.side_black.nbr_rooks==2 && mqi.side_black.rooks[0]==dst )
                        mqi.side_black.rooks[0] = mqi.side_black.rooks[1];
                    --mqi.side_black.nbr_rooks;
                    break;
                }
                case 'n':
                {
                    if( mqi.side_black.nbr_knights==2 && mqi.side_black.knights[0]==dst )
                        mqi.side_black.knights[0] = mqi.side_black.knights[1];
                    --mqi.side_black.nbr_knights;
                    break;
                }
                case 'b':
                {
                    --mqi.side_black.nbr_light_bishops;
                    break;
                }
                case 'd':
                {
                    --mqi.side_black.nbr_dark_bishops;
                    break;
                }
            }
        }
        /*if( debug_trigger )
        {
            char buf[9];
            buf[8] = '\0';
            cprintf( "before\n" );
            for( int i=0; i<64; i+=8 )
            {
                memcpy(buf,&mqi.squares[i],8);
                for( int k=0; k<8; k++ )
                    if( buf[k] == ' ' ) buf[k]='.';
                cprintf( "%s\n", buf );
            }
        } */
        match = pm.Test( reverse, &mqi.side_white, &mqi.side_black, false, mqi.squares, false );
        if( match )
        {
            /*if( debug_trigger )
            {
                cprintf( "Match!\n" );
                debug_trigger = false;
            } */
            offset_last = offset_first = offset;    // later - separate offset_first and offset_last
            return true;
        }
        #ifdef SIMPLE_PATTERN_COUNT_OPTIMISATION
        if( total_count < ms.total_count_target )
            return false;
        #endif

        // Black move
        code = *moves_in++;
        offset++;
        src=0;
        dst=0;
        hi_nibble = code&0xf0;
        captured=EMPTY_CHARACTER;
        switch( hi_nibble )
        {
            case CODE_KING:
            {
                src = mqi.side_black.king;
                int delta=0;
                switch( code&0x0f )     // 0, 1, 2
                {                       // 8, 9, 10
                                        // 16,17,18
                    case 0: return false;   // CODE_KING = 0, so '\0' string terminator
                    case K_VECTOR_NW:    delta = -9; break;  // 0-9
                    case K_VECTOR_N:     delta = -8; break;  // 1-9
                    case K_VECTOR_NE:    delta = -7; break;  // 2-9
                    case K_VECTOR_W:     delta = -1; break;  // 8-9
                    case K_VECTOR_E:     delta =  1; break;  // 10-9
                    case K_VECTOR_SW:    delta =  7; break;  // 16-9
                    case K_VECTOR_S:     delta =  8; break;  // 17-9
                    case K_VECTOR_SE:    delta =  9; break;  // 18-9
                    case K_K_CASTLING:
                    {
                        delta = 2;
                        int rook_offset = (mqi.side_black.rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                        mqi.side_black.rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src+1] = 'r';    //f8
                        mqi.squares[src+3] = EMPTY_CHARACTER;    //h8
                        break;
                    }
                    case K_Q_CASTLING:
                    {
                        delta = -2;
                        int rook_offset = (mqi.side_black.rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                        mqi.side_black.rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                        // note that there is no way the rooks ordering can swap during castling
                        mqi.squares[src-1] = 'r';    //d8
                        mqi.squares[src-4] = EMPTY_CHARACTER;    //a8
                        break;
                    }
                }
                mqi.side_black.king = dst = src+delta;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'k';
                mqi.squares[src] = EMPTY_CHARACTER;
                break;
            }

            case CODE_ROOK_LO:
            case CODE_ROOK_HI:
            {
                int rook_offset = (hi_nibble==CODE_ROOK_LO ? 0 : 1 );
                src = mqi.side_black.rooks[rook_offset];
                if( code & R_RANK )                // code encodes rank ?
                {
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                }
                else
                {
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                }
                mqi.side_black.rooks[rook_offset] = dst;
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'r';
                mqi.squares[src] = EMPTY_CHARACTER;

                // swap ?
                if( mqi.side_black.nbr_rooks==2 && mqi.side_black.rooks[0]>mqi.side_black.rooks[1] )
                {
                    int temp = mqi.side_black.rooks[0];
                    mqi.side_black.rooks[0] = mqi.side_black.rooks[1];
                    mqi.side_black.rooks[1] = temp;
                }
                break;
            }

            case CODE_BISHOP_DARK:
            {
                src = mqi.side_black.bishop_dark;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=b8(1), dst=h2(55), file_delta=6  -> 9*6 =54
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'd';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.bishop_dark = dst;
                break;
            }

            case CODE_BISHOP_LIGHT:
            {
                src = mqi.side_black.bishop_light;
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=g8(6), dst=a2(48), file_delta=6  -> 7*6 =42
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'b';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.bishop_light = dst;
                break;
            }

            case CODE_QUEEN_ROOK:
            {
                src = mqi.side_black.queens[0];
                if( code & R_RANK )                // code encodes rank ?
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                else
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.queens[0] = dst;
                break;
            }

            case CODE_QUEEN_BISHOP:
            {
                src = mqi.side_black.queens[0];
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'q';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.queens[0] = dst;
                break;
            }

            case CODE_KNIGHT:
            {
                int knight_offset = ((code&N_HI) ? 1 : 0 );
                src = mqi.side_black.knights[knight_offset];
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
                captured = mqi.squares[dst];
                mqi.squares[dst] = 'n';
                mqi.squares[src] = EMPTY_CHARACTER;
                mqi.side_black.knights[knight_offset] = dst;

                // swap ?
                if( mqi.side_black.nbr_knights==2 && mqi.side_black.knights[0]>mqi.side_black.knights[1] )
                {
                    int temp = mqi.side_black.knights[0];
                    mqi.side_black.knights[0] = mqi.side_black.knights[1];
                    mqi.side_black.knights[1] = temp;
                }
                break;
            }

            // PAWN
            default:
            {
                int pawn_offset = (code>>4)&0x07;
                src = mqi.side_black.pawns[pawn_offset];
                int delta;
                switch( code&0x0f )
                {
                    default:
                    {
                        return false;   // this code doesn't handle promotions
                    }

                    case P_DOUBLE:
                    {
                        delta = 16;
                        dst = src+delta;
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        break;
                    }
                    case P_SINGLE:
                    {
                        delta = 8;
                        dst = src+delta;
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        break;
                    }
                    case P_LEFT:
                    {
                        delta = 9;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'P';     // en-passant
                            mqi.squares[dst=src+1] = EMPTY_CHARACTER;
                        }

                        // Increasing capture possibly requires reordering Black pawns
                        for( int i=pawn_offset; i+1<mqi.side_black.nbr_pawns && pawn_ordering[mqi.side_black.pawns[i]]>pawn_ordering[mqi.side_black.pawns[i+1]]; i++ )
                        {
                            int temp = mqi.side_black.pawns[i];
                            mqi.side_black.pawns[i] = mqi.side_black.pawns[i+1];
                            mqi.side_black.pawns[i+1] = temp;
                        }
                        break;
                    }

                    case P_RIGHT:
                    {
                        delta = 7;
                        dst = src+delta;
                        captured = mqi.squares[dst];
                        mqi.squares[dst] = 'p';
                        mqi.squares[src] = EMPTY_CHARACTER;
                        mqi.side_black.pawns[pawn_offset] = dst;
                        if( captured == EMPTY_CHARACTER )
                        {
                            captured = 'P';     // en-passant
                            mqi.squares[dst=src-1] = EMPTY_CHARACTER;
                        }

                        // Decreasing capture possibly requires reordering Black pawns
                        for( int i=pawn_offset; i-1>=0 && pawn_ordering[mqi.side_black.pawns[i-1]]>pawn_ordering[mqi.side_black.pawns[i]]; i-- )
                        {
                            int temp = mqi.side_black.pawns[i-1];
                            mqi.side_black.pawns[i-1] = mqi.side_black.pawns[i];
                            mqi.side_black.pawns[i] = temp;
                        }
                        break;
                    }
                }
            }  // end switch on pawn types
        }   // end switch all pieces

        // Black captures white
        if( captured != EMPTY_CHARACTER )
        {
            total_count--;
            switch( captured )
            {
                case 'P':
                {
                    bool zap=false;
                    for( int i=0; i<mqi.side_white.nbr_pawns-1; i++ )
                    {
                        if( zap )
                            mqi.side_white.pawns[i] = mqi.side_white.pawns[i+1];
                        else if( mqi.side_white.pawns[i] == dst )
                        {
                            zap = true;
                            mqi.side_white.pawns[i] = mqi.side_white.pawns[i+1];
                        }
                    }
                    --mqi.side_white.nbr_pawns;
                    break;
                }
                case 'Q':
                {
                    --mqi.side_white.nbr_queens;
                    break;
                }
                case 'R':
                {
                    if( mqi.side_white.nbr_rooks==2 && mqi.side_white.rooks[0]==dst )
                        mqi.side_white.rooks[0] = mqi.side_white.rooks[1];
                    --mqi.side_white.nbr_rooks;
                    break;
                }
                case 'N':
                {
                    if( mqi.side_white.nbr_knights==2 && mqi.side_white.knights[0]==dst )
                        mqi.side_white.knights[0] = mqi.side_white.knights[1];
                    --mqi.side_white.nbr_knights;
                    break;
                }
                case 'B':
                {
                    --mqi.side_white.nbr_light_bishops;
                    break;
                }
                case 'D':
                {
                    --mqi.side_white.nbr_dark_bishops;
                    break;
                }
            }
        }
    }
    //return false;     // unreachable
}

bool MemoryPositionSearch::SearchGameSlowPromotionAllowed( const std::string &moves_in, unsigned short &offset_first, unsigned short &offset_last )          // semi fast
{
    bool target_white = search_position.white;  // searching for position with white to move?
    int black_count=16;
    int black_pawn_count=8;
    int white_count=16;
    int white_pawn_count=8;
    SlowGameInit();
    int len = moves_in.size();
    if(
        (msi.cr.white == target_white) &&
        *ms.slow_rank3_ptr == *ms.slow_rank3_target_ptr &&
        *ms.slow_rank4_ptr == *ms.slow_rank4_target_ptr &&
        *ms.slow_rank5_ptr == *ms.slow_rank5_target_ptr &&
        *ms.slow_rank6_ptr == *ms.slow_rank6_target_ptr &&
        *ms.slow_rank7_ptr == *ms.slow_rank7_target_ptr &&
        *ms.slow_rank8_ptr == *ms.slow_rank8_target_ptr &&
        *ms.slow_rank1_ptr == *ms.slow_rank1_target_ptr &&
        *ms.slow_rank2_ptr == *ms.slow_rank2_target_ptr
    )
    {
        offset_last = offset_first = 0;    // later - separate offset_first and offset_last
        return true;
    }
    for( int i=0; i<len; i++ )
    {
        MpsSide *side  = msi.cr.white ? &msi.sides[0] : &msi.sides[1];
        MpsSide *other = msi.cr.white ? &msi.sides[1] : &msi.sides[0];
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
        char mover = msi.cr.squares[mv.src];
        //std::string mvs = mv.NaturalOut(&msi.cr);
        msi.cr.PlayMove(mv);
        if( mv.special == thc::SPECIAL_PROMOTION_BISHOP ) // Impose the distinct dark squared bishop = 'd'/'D' convention over the top
        {
            if( is_dark(mv.dst) )
            {
                char c = msi.cr.squares[mv.dst];
                if( c == 'B' )
                    c = 'D';
                else if( c == 'b' )
                    c = 'd';
                msi.cr.squares[mv.dst] = c;
            }
        }
        //std::string s = msi.cr.ToDebugStr();
        //cprintf( "After %s\n%s\n", mvs.c_str(), s.c_str() );
        if(
            (msi.cr.white == target_white) &&
            *ms.slow_rank3_ptr == *ms.slow_rank3_target_ptr &&
            *ms.slow_rank4_ptr == *ms.slow_rank4_target_ptr &&
            *ms.slow_rank5_ptr == *ms.slow_rank5_target_ptr &&
            *ms.slow_rank6_ptr == *ms.slow_rank6_target_ptr &&
            *ms.slow_rank7_ptr == *ms.slow_rank7_target_ptr &&
            *ms.slow_rank8_ptr == *ms.slow_rank8_target_ptr &&
            *ms.slow_rank1_ptr == *ms.slow_rank1_target_ptr &&
            *ms.slow_rank2_ptr == *ms.slow_rank2_target_ptr
        )
        {
            offset_last = offset_first = (i+1);    // later - separate offset_first and offset_last
            return true;
        }
        if( mover=='P' && 48<=mv.src && mv.src<56 && !SLOW_WHITE_HOME_ROW_TEST )
            return false;
        if( mover=='p' && 8<=mv.src && mv.src<16 && !SLOW_BLACK_HOME_ROW_TEST )
            return false;
        if( isalpha(mv.capture) )
        {
            if( islower(mv.capture) )
            {
                black_count--;
                if( black_count < ms.black_count_target )
                    return false;
                if( mv.capture == 'p' )
                {
                    black_pawn_count--;  // pawns can also disappear at promotion time
                                         //  disregarding that means we sometimes
                                         //  continue searching unnecessarily but
                                         //  doesn't cause errors
                    if( black_pawn_count < ms.black_pawn_count_target )
                        return false;
                    if(  8<=mv.dst && mv.dst<16 && !SLOW_BLACK_HOME_ROW_TEST )
                        return false;

                }
            }
            else
            {
                white_count--;
                if( white_count < ms.white_count_target )
                    return false;
                if( mv.capture == 'P' )
                {
                    white_pawn_count--;  // pawns can also disappear at promotion time
                                         //  disregarding that means we sometimes
                                         //  continue searching unnecessarily but
                                         //  doesn't cause errors
                    if( white_pawn_count < ms.white_pawn_count_target )
                        return false;
                    if( 48<=mv.dst && mv.dst<56 && !SLOW_WHITE_HOME_ROW_TEST )
                        return false;
                }
            }
        }
    }
    return false;
}

bool MemoryPositionSearch::PatternSearchGameSlowPromotionAllowed( PatternMatch &pm, bool &reverse, const std::string &moves_in, unsigned short &offset_first, unsigned short &offset_last )          // semi fast
{
    int total_count=30;     // 32 - 2 kings
    SlowGameInit();
    int len = moves_in.size();
    bool match = pm.Test( reverse, &msi.sides[0], &msi.sides[1], msi.cr.white, msi.cr.squares, false );
    if( match )
    {
        offset_last = offset_first = 0;    // later - separate offset_first and offset_last
        return true;
    }
    for( int i=0; i<len; i++ )
    {
        MpsSide *side  = msi.cr.white ? &msi.sides[0] : &msi.sides[1];
        MpsSide *other = msi.cr.white ? &msi.sides[1] : &msi.sides[0];
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
        msi.cr.PlayMove(mv);
        if( mv.special == thc::SPECIAL_PROMOTION_BISHOP ) // Impose the distinct dark squared bishop = 'd'/'D' convention over the top
        {
            if( is_dark(mv.dst) )
            {
                char c = msi.cr.squares[mv.dst];
                if( c == 'B' )
                    c = 'D';
                else if( c == 'b' )
                    c = 'd';
                msi.cr.squares[mv.dst] = c;
            }
        }
        match = pm.Test( reverse, &msi.sides[0], &msi.sides[1], msi.cr.white, msi.cr.squares, true );
        if( match )
        {
            offset_last = offset_first = (i+1);    // later - separate offset_first and offset_last
            return true;
        }
        #ifdef SIMPLE_PATTERN_COUNT_OPTIMISATION
        if( isalpha(mv.capture) )
        {
            total_count--;
            if( total_count < ms.total_count_target )
                return false;
        }
        #endif
    }
    return false;
}

