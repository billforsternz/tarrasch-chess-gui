/****************************************************************************
 *  Database subsystem for looking up positions from a database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DB_H
#define DB_H
#include <stdint.h>
#include <string>
#include <vector>
#include "thc.h"
#include "GameDocument.h"

struct DB_GAME_INFO
{
    int game_id;
    std::string white;
    std::string black;
    std::string event;
    std::string site;
    std::string result;
    std::string date;
    std::string white_elo;
    std::string black_elo;
    std::string move_txt;
    std::string str_blob;
    int transpo_nbr;
    std::string Description()
    {
        std::string white = this->white;
        std::string black = this->black;
        size_t comma = white.find(',');
        if( comma != std::string::npos )
            white = white.substr( 0, comma );
        comma = black.find(',');
        if( comma != std::string::npos )
            black = black.substr( 0, comma );
        int move_cnt = str_blob.length();
        std::string label = white;
        if( white_elo != "" )
        {
            label += " (";
            label += white_elo;
            label += ")";
        }
        label += " - ";
        label += black;
        if( black_elo != "" )
        {
            label += " (";
            label += black_elo;
            label += ")";
        }
        if( site != "" )
        {
            label += ", ";
            label += site;
        }
        else if( event != "" )
        {
            label += ", ";
            label += event;
        }
        if( date.length() >= 4 )
        {
            label += " ";
            label += date.substr(0,4);
        }
        bool result_or_moves = false;
        if( result != "*" )
        {
            result_or_moves = true;
            label += ", ";
            label += result;
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

};

//FIXME - reorganise these
void db_calculate_move_txt( DB_GAME_INFO *info );
int  db_calculate_move_vector( DB_GAME_INFO *info, std::vector<thc::Move> &moves );

class Database
{
public:
    Database();
    ~Database();

    int SetPosition( thc::ChessRules &cr );
    int SetPosition( thc::ChessRules &cr, std::string &player_name );
    int GetRow( DB_GAME_INFO *info, int row );
    int LoadAllGames( std::vector<DB_GAME_INFO> &cache, int nbr_games );
    bool TestNextRow();
    bool TestPrevRow();
    int GetCurrent();
    int FindRow( std::string &name );
    
private:
    std::string player_name;
    bool is_start_pos;
    std::string where_white;
    std::string white_and;
};

#endif // DB_H
