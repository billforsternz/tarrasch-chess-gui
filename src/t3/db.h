//
//  db.h
//  readpgnfast
//
//  Created by Bill Forster on 6/05/13.
//  Copyright (c) 2013 Bill Forster. All rights reserved.
//

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
    std::string result;
    std::string move_txt;
    std::string str_blob;
    std::string next_move;
    int transpo_nbr;
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
