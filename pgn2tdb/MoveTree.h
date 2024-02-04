/****************************************************************************
 * Representation of a game's tree of variations
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef MOVE_TREE_H
#define MOVE_TREE_H
#include <string>
#include <vector>
#include "thc.h"

struct GAME_MOVE
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
    GAME_MOVE() { nag_value1=0; nag_value2=0; flag_ingame=0; white_clock_visible=false; black_clock_visible=false;
                  human_is_white=false; human_millisecs_time=0; engine_millisecs_time=0; }
};

struct VARIATION_STACK_ELEMENT;


class MoveTree
{

public:
    thc::ChessPosition *root;
    GAME_MOVE   game_move;
    std::vector<std::vector<MoveTree> > variations;
    MoveTree() { root=NULL; variations.clear(); }
    void Init( thc::ChessPosition &start_position )
    {
        root = &start_position;
        variations.clear();    // start the root variation (there will be only one)
        std::vector<MoveTree> variation;
        variations.push_back(variation);
    }
    void Dump( std::string& str );

    // Promote the entire variation containing a child node
    //  Return ptr to child node in its new position in the promoted variation
    MoveTree *Promote( MoveTree *child );

    // Demote the entire variation containing a child node
    //  Return ptr to child node in its new position in the demoted variation
    MoveTree *Demote( MoveTree *child );

    // Delete the rest of a variation
    void DeleteRestOfVariation( MoveTree *child );

    // Delete variation
    void DeleteVariation( MoveTree *child );

    // Given a child node, find its parent
    MoveTree *Parent( MoveTree *child, thc::ChessRules &cr_out, int &ivar, int &imove );
    MoveTree *ParentCrawler( int& level, bool& first, MoveTree *child, thc::ChessRules &cr, thc::ChessRules &cr_out, int &ivar, int &imove );

    // Find a target node in the tree under here, build a stack of variations leading to the node
    bool Find( MoveTree *target, std::vector<VARIATION_STACK_ELEMENT> &stack );
    bool FindCrawler( int& level,  const MoveTree *target, std::vector<VARIATION_STACK_ELEMENT> &stack );

    // Calculate a ChessRules object with history leading to a position
    //  returns true if found successfully
    bool Seek( const MoveTree *target, thc::ChessRules &cr );
    bool SeekCrawler( int& level, const MoveTree *target, thc::ChessRules &cr, bool done );
};


typedef std::vector<MoveTree> VARIATION;

struct VARIATION_STACK_ELEMENT
{
    VARIATION *v=NULL;
    int imove=-1;
};

#endif //MOVE_TREE_H
