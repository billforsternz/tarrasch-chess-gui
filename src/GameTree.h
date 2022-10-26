/****************************************************************************
 * Representation of a game's tree of variations
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAME_MOVES_H
#define GAME_MOVES_H
#include <string>
#include <vector>
#include "Bytecode.h"
#include "thc.h"

//
// In branch "new-heart" I am starting out on an ambitious plan to transplant
//  a new heart into Tarrasch - based on bytecode. The byte code will be the
//  same one byte per move representation developed to implement the database
//  features (class CompressMoves) enhanced with techniques to represent
//  variations, annotations, comments (and more?). This should be more flexible
//  maintainable, understandable and improveable than the current MoveTree
//  recursive vector representation.
//
//  I am going to try to keep the code compiling and linking, but it won't
//  necessarily be functional for a while.
//

// MovePlus = experimental replacement of GAME_MOVE
struct MovePlus
{
    thc::Move move;
    int   human_millisecs_time;
    int   engine_millisecs_time;
    bool  flag_ingame;
    bool  white_clock_visible;
    bool  black_clock_visible;
    bool  human_is_white;
    std::string pre_comment;
    std::string comment;
    char nag_value1;
    char nag_value2;
    MovePlus() { nag_value1=0; nag_value2=0; flag_ingame=0; white_clock_visible=false; black_clock_visible=false;
                  human_is_white=false; human_millisecs_time=0; engine_millisecs_time=0; }
};

// GameTree = experimental replacement for MoveTree class
class GameTree
{
private:
public:
    Bytecode press;
    std::string bytecode;
    int offset = 0;

    // Initialisation, different flavours
    GameTree() { press.Init(); }
    GameTree( std::string &bytecode_ ) { Init(bytecode_); }
    GameTree( thc::ChessPosition &start_position ) { Init(start_position); }
    GameTree( thc::ChessPosition &start_position, std::string &bytecode_ ) { Init(start_position,bytecode_); }
    void Init( thc::ChessPosition &start_position )
    {
        press.Init(start_position);
        bytecode.clear();
    }
    void Init( thc::ChessPosition &start_position, std::string &bytecode_ )
    {
        press.Init(start_position);
        bytecode = bytecode_;
    }
    void Init( std::string &bytecode_ )
    {
        press.Init();
        bytecode = bytecode_;
    }

    // Get MovePlus at given offset
    MovePlus GetMovePlus( int offset_parm );

    // Promote a variation at given offset
    //  Return offset of promoted variation
    int Promote( int offset_parm );

    // Demote a variation at given offset
    //  Return offset of promoted variation
    int Demote( int offset_parm  );

    // Delete the rest of a variation
    void DeleteRestOfVariation( int offset_parm  );

    // Delete variation
    void DeleteVariation( int offset_parm  );
};

#endif //GAME_MOVES_H
