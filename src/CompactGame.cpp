/****************************************************************************
 * Compact game representation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "GameDocument.h"

std::string CompactGame::Description()
{
    std::string white = this->r.white;
    std::string black = this->r.black;
    size_t comma = white.find(',');
    if( comma != std::string::npos )
        white = white.substr( 0, comma );
    comma = black.find(',');
    if( comma != std::string::npos )
        black = black.substr( 0, comma );
    int move_cnt = moves.size();
    std::string label = white;
    if( r.white_elo != "" )
    {
        label += " (";
        label += r.white_elo;
        label += ")";
    }
    label += " - ";
    label += black;
    if( r.black_elo != "" )
    {
        label += " (";
        label += r.black_elo;
        label += ")";
    }
    if( r.site != ""  && r.site != "?" )
    {
        label += ", ";
        label += r.site;
    }
    else if( r.event != "" && r.event != "?"  )
    {
        label += ", ";
        label += r.event;
    }
    if( r.date.length() >= 4 )
    {
        label += " ";
        label += r.date.substr(0,4);
    }
    bool result_or_moves = false;
    if( r.result != "*" )
    {
        result_or_moves = true;
        label += ", ";
        label += r.result;
        if( move_cnt > 0 )
            label += " in";
    }
    if( move_cnt > 0 )
    {
        if( !result_or_moves )
            label += ", ";
        char buf[100];
        sprintf( buf, " %d moves", (move_cnt+1)/2 );
        label += std::string(buf);
    }
    return label;
}


void CompactGame::Upscale( GameDocument &gd )
{
    gd.r = r;
    gd.game_id = game_id;
    bool have_start_position = HaveStartPosition();
    if( have_start_position )
        gd.start_position =  GetStartPosition();
    else
        gd.start_position.Init();
    std::vector<MoveTree> &variation = gd.tree.variations[0];
    variation.clear();
    MoveTree m;
    for( size_t i=0; i<moves.size(); i++ )
    {
        m.game_move.move = moves[i];
        variation.push_back(m);
    }
    gd.Rebuild();
}

void CompactGame::Downscale( GameDocument &gd )
{
    r = gd.r;
    game_id = gd.game_id;
    transpo_nbr = 0;
    start_position = gd.start_position;
    std::vector<MoveTree> &variation = gd.tree.variations[0];
    moves.clear();
    for( size_t i=0; i<variation.size(); i++ )
    {
        thc::Move mv = variation[i].game_move.move;
        moves.push_back(mv);
    }
}

