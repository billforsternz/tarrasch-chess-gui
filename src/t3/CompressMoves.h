/****************************************************************************
 *  Compress chess moves into one byte
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef COMPRESS_MOVES_H
#define COMPRESS_MOVES_H

#include <algorithm>
#include <vector>
#include <string>
#include "thc.h"

// Old version
#define USE_OLD_VERSION // #define this until new version is fully tested
#ifdef  USE_OLD_VERSION

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
	void Init( thc::ChessPosition &cp );
    int  compress_move( thc::Move mv, char *storage );
    int  decompress_move( const char *storage, thc::Move &mv );
    void decompress_move_stay( const char *storage, thc::Move &mv ) const;  // decompress but don't advance
    
    
private:
    bool evil_queen_mode;
    std::string check_last_success;
    std::string check_last_description;
    void compress_move_slow_mode( thc::Move mv, char &out );
    void decompress_move_slow_mode( char in, thc::Move &out );
};

// New version
#else

struct Side
{
    bool white;
    bool fast_mode;
    int rooks[2];       // locations of each dynamic piece
    int knights[2];     //
    int pawns[8];
    int bishop_dark;
    int bishop_light;
    int queen;
    int king;
    int nbr_pawns;      // 0-8
    int nbr_rooks;      // 0,1 or 2
    int nbr_knights;    // 0,1 or 2
    int nbr_light_bishops;
    int nbr_dark_bishops;
    int nbr_queens;
};

class CompressMoves
{
public:
    CompressMoves()
    {
        sides[0].white=true;
        sides[0].fast_mode=false;
        sides[1].white=false;
        sides[1].fast_mode=false;
        is_interesting = false;
    }
    bool TryFastMode( Side *side );
    std::string Compress( std::vector<thc::Move> &moves_in );
    std::string Compress( thc::ChessPosition &cp, std::vector<thc::Move> &moves_in );
    std::vector<thc::Move> Uncompress( std::string &moves_in );
    std::vector<thc::Move> Uncompress( thc::ChessPosition &cp, std::string &moves_in );
    
    // For compatibility with old version, progressively replace
    CompressMoves( const CompressMoves& copy_from_me ) { cr=copy_from_me.cr; sides[0]=copy_from_me.sides[0]; sides[1]=copy_from_me.sides[1]; }
    CompressMoves & operator= (const CompressMoves & copy_from_me ) { cr=copy_from_me.cr; sides[0]=copy_from_me.sides[0]; sides[1]=copy_from_me.sides[1]; return *this; }
    
    bool Check( bool do_internal_check, const char *description, thc::ChessPosition *external ) { return true; }
    void Init() { TryFastMode( &sides[0]); TryFastMode( &sides[1]); }
	void Init( thc::ChessPosition &cp ) { cr = cp; Init(); }
    int  compress_move( thc::Move mv, char *storage );
    int  decompress_move( const char *storage, thc::Move &mv );
    void decompress_move_stay( const char *storage, thc::Move &mv );  // decompress but don't advance
    
public:
    thc::ChessRules cr;
    bool is_interesting;
private:
    Side sides[2];
    char CompressSlowMode( thc::Move mv );
    char CompressFastMode( thc::Move mv, Side *side, Side *other );
    thc::Move UncompressSlowMode( char code );
    thc::Move UncompressFastMode( char code, Side *side, Side *other );
};

#endif

#endif // COMPRESS_MOVES_H
