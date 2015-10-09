/****************************************************************************
 * Compact game representation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef COMPACT_GAME_H
#define COMPACT_GAME_H
#include <string>
#include <vector>
#include <memory>
#include "thc.h"

class Roster
{
public:
    std::string white;
    std::string black;
    std::string event;
    std::string site;
    std::string result;
    std::string round;
    std::string date;
    std::string eco;
    std::string white_elo;
    std::string black_elo;
    std::string fen;
};

class GameDocument;

class CompactGame
{
public:
    CompactGame() { game_id=0; transpo_nbr=0; }
    Roster r;
    thc::ChessPosition start_position;
    std::vector< thc::Move > moves;
    
    // temp stuff hopefully
    int game_id;
    int transpo_nbr;
    
    std::string Description();
    void Upscale( GameDocument &gd );       // to GameDocument
    void Downscale( GameDocument &gd );     // from GameDocument
    bool HaveStartPosition() { return (r.fen.length() > 0 ); }
    thc::ChessPosition &GetStartPosition() { if( r.fen.length()>0 ) start_position.Forsyth(r.fen.c_str()); return start_position; }
    
    // Return index into vector where start position found
    bool FindPositionInGame( uint64_t hash_to_match, int &idx )
    {
        thc::ChessRules cr = GetStartPosition();
        size_t len = moves.size();
        uint64_t hash = cr.Hash64Calculate();
        bool found = (hash==hash_to_match);
        idx = 0;
        for( size_t i=0; !found && i<len; i++  )
        {
            thc::Move mv = moves[i];
            hash = cr.Hash64Update( hash, mv );
            if( hash == hash_to_match )
            {
                found = true;
                idx = static_cast<int>(i+1);
                break;
            }
            cr.PlayMove(mv);
        }
        return found;
    }
    
};

#endif  // COMPACT_GAME_H
