/****************************************************************************
 *  Compress chess moves into one byte
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef __readquick__CompressMoves__
#define __readquick__CompressMoves__

#include <string>
#include "thc.h"

std::string SqToStr( int sq );
std::string SqToStr( thc::Square sq );

struct Tracker
{
    int  tracker_id;
    char piece;
    thc::Square sq;
    bool in_use;
    Tracker *shadow_rook;  // For promoted queen -> dead piece used for rank/file moves
    Tracker *shadow_file;  // For initial queen -> initial knight for file moves
    Tracker *shadow_rank;  // For initial queen -> initial knight for rank moves
    Tracker *shadow_owner; // For shadow knight or rook -> queen
    void Init( int tracker_idx, char piece, thc::Square sq )
    {
        in_use = true;
        tracker_id = tracker_idx*16; // Hi nibble of one byte code, so 0x00 -> 0xf0
        this->piece = piece;
        this->sq = sq;
        shadow_rook = NULL;
        shadow_file = NULL;
        shadow_rank = NULL;
        shadow_owner = NULL;
    }
};

class CompressMoves
{
    enum TrackerIdx {
        TI_K  = 0,
        TI_KN = 1,
        TI_QN = 2,
        TI_KR = 3,
        TI_QR = 4,
        TI_KB = 5,
        TI_QB = 6,
        TI_Q  = 7,
        TI_AP = 8,
        TI_BP = 9,
        TI_CP = 10,
        TI_DP = 11,
        TI_EP = 12,
        TI_FP = 13,
        TI_GP = 14,
        TI_HP = 15
    };
    Tracker white_pieces[16];
    Tracker black_pieces[16];
    Tracker *trackers[64];
    void square_init( int tracker_idx, char piece, thc::Square sq )
    {
        Tracker *pt;
        if( isupper(piece) )
            pt = &white_pieces[tracker_idx];
        else
            pt = &black_pieces[tracker_idx];
        pt->Init( tracker_idx, piece, sq );
        trackers[sq] = pt;
    }
    
public:
    thc::ChessRules cr;
    CompressMoves()
    {
        Init();
    }
    CompressMoves( const CompressMoves& copy_from_me );
    CompressMoves & operator= (const CompressMoves & copy_from_me );

    bool Check( bool do_internal_check, const char *description, thc::ChessPosition *external );
    void Init();
    int  compress_move( thc::Move mv, char *storage );
    int  decompress_move( const char *storage, thc::Move &mv );
    void decompress_move_stay( const char *storage, thc::Move &mv ) const;  // decompress but don't advance
    
    
private:
    std::string check_last_success;
    std::string check_last_description;
};

#endif /* defined(__readquick__CompressMoves__) */
