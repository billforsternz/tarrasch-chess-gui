/****************************************************************************
 * Partial game representation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef GAME_SKELETON_H
#define GAME_SKELETON_H
#include <string>
#include <vector>
#include "ChessRules.h"
#include "MoveTree.h"
#include "GameView.h"
#include "GameLifecycle.h"

class GameSkeleton
{
public:
    GameSkeleton()
    {
        thc::ChessPosition initial_position;
        Init(initial_position);
    }

    void Init( const thc::ChessPosition &start_position )
    {
        this->start_position = start_position;
        game_details_edited = false;
        in_memory           = false;
        game_being_edited        = false;
        selected            = false;
        focus               = false;
        pgn_handle          = 0;
        sort_idx            = 0;
        game_nbr            = 0;
        fposn1              = 0;
        fposn2              = 0;
        fposn3              = 0;
    }     

    // Allow sorts
    bool operator< (const GameSkeleton &rhs) const
    {
        return sort_idx < rhs.sort_idx;
    }

    void ToFileTxtGameDetails( std::string &str );

/*
    // Copy constructor
    GameSkeleton( const GameSkeleton& src )
    {
        *this = src;    // use the assignment operator
    }

    // Assignment operator
    GameSkeleton& operator=( const GameSkeleton& src )
    {

        // Copy all data fields
        game_details_edited = src.game_details_edited;
        in_memory       = src.in_memory; 
        game_being_edited    = src.game_being_edited; 
        pgn_handle      = src.pgn_handle;
        sort_idx        = src.sort_idx;
        focus           = src.focus;
        selected        = src.selected;
        game_nbr        = src.game_nbr;
        white           = src.white;          
        black           = src.black;          
        event           = src.event;          
        site            = src.site;           
        date            = src.date;           
        round           = src.round;          
        result          = src.result;         
        eco             = src.eco;            
        white_elo       = src.white_elo;      
        black_elo       = src.black_elo;      
        start_position  = src.start_position;
        master_position = src.master_position;
        tree            = src.tree;           
        fposn1          = src.fposn1;   
        fposn2          = src.fposn2;   
        fposn3          = src.fposn3;   
        moves_txt       = src.moves_txt;      
        gv              = src.gv;

        // Need to rebuild using our copy of the tree to avoid
        //  raw ptrs to the old tree
        tree.root = &start_position;
        gv.Build( result, &tree, start_position );
        return( *this );
    }

    void Init( const thc::ChessPosition &start_position );
*/


    // Data
    bool        game_details_edited;
    bool        in_memory;
    bool        game_being_edited;
    int         pgn_handle;
    bool        selected;
    bool        focus;
    int         sort_idx;
    int         game_nbr;
    std::string white;          // "White"
    std::string black;          // "Black"
    std::string event;          // "Event"
    std::string site;           // "Site"
    std::string date;           // "Date"
    std::string round;          // "Round"
    std::string result;         // "Result"
    std::string eco;            // "ECO"
    std::string white_elo;      // "WhiteElo"
    std::string black_elo;      // "BlackElo"
    thc::ChessPosition start_position;  // the start position
    unsigned long fposn1;       // offset of tags in .pgn file
    unsigned long fposn2;       // offset where moves are in .pgn file
    unsigned long fposn3;       // offset where moves end in .pgn file
    std::string moves_txt;      // "1.e4 e5 2.Nf3.."
};

#endif //GAME_SKELETON_H
