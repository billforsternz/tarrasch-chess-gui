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

struct Side
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

class CompressMoves
{
public:
    CompressMoves()
    {
        sides[0].white=true;
        sides[0].fast_mode=false;
        sides[1].white=false;
        sides[1].fast_mode=false;
        is_interesting = 0;
        nbr_slow_moves = 0;
    }
    CompressMoves( thc::ChessPosition &cp )
    {
        sides[0].white=true;
        sides[1].white=false;
        is_interesting = 0;
        nbr_slow_moves = 0;
        Init(cp);
    }
    bool TryFastMode( Side *side );
    std::string Compress( std::vector<thc::Move> &moves_in );
    std::string Compress( thc::ChessPosition &cp, std::vector<thc::Move> &moves_in );
    std::vector<thc::Move> Uncompress( std::string &moves_in );
    std::vector<thc::Move> Uncompress( thc::ChessPosition &cp, std::string &moves_in );
    char      CompressMove( thc::Move mv );
    thc::Move UncompressMove( char c );
    CompressMoves( const CompressMoves& copy_from_me ) { cr=copy_from_me.cr; sides[0]=copy_from_me.sides[0]; sides[1]=copy_from_me.sides[1]; }
    CompressMoves & operator= (const CompressMoves & copy_from_me ) { cr=copy_from_me.cr; sides[0]=copy_from_me.sides[0]; sides[1]=copy_from_me.sides[1]; return *this; }
    void Init() { TryFastMode( &sides[0]); TryFastMode( &sides[1]); }
    void Init( thc::ChessPosition &cp ) { cr = cp; Init(); }

public:
    thc::ChessRules cr;
    int is_interesting;
    int nbr_slow_moves;
private:
    Side sides[2];
    char CompressSlowMode( thc::Move mv );
    char CompressFastMode( thc::Move mv, Side *side, Side *other );
    thc::Move UncompressSlowMode( char code );
    thc::Move UncompressFastMode( char code, Side *side, Side *other );
};

#endif // COMPRESS_MOVES_H
