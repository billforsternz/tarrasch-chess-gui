/****************************************************************************
 *  Database subsystem for looking up positions from a database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef Database_H
#define Database_H
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <unordered_set>
#include "sqlite3.h"
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
    std::string str_blob;
    int transpo_nbr;
    
    std::string db_calculate_move_txt( uint64_t hash_to_match );
    int  db_calculate_move_vector( std::vector<thc::Move> &moves, uint64_t hash_to_match  );
    std::string Description();
};

class Database
{
public:
    Database();
    ~Database();

    int SetPosition( thc::ChessRules &cr );
    int SetPosition( thc::ChessRules &cr, std::string &player_name );
    int GetNbrGames( thc::ChessRules &cr );
    int GetRow( DB_GAME_INFO *info, int row );
    int GetRowRaw( DB_GAME_INFO *info, int row );
    int LoadAllGames( std::vector<DB_GAME_INFO> &cache, int nbr_games );
    bool TestNextRow();
    bool TestPrevRow();
    int GetCurrent();
    int FindRow( std::string &name );
    int LoadGameWithQuery( DB_GAME_INFO *info, int game_id );
    int LoadGamesWithQuery( uint64_t hash, std::vector<DB_GAME_INFO> &games, std::unordered_set<int> &games_set );


  
private:
    // Create a handle for database connection, create a pointer to sqlite3
    sqlite3 *gbl_handle;
    
    // A prepared statement for fetching from positions table
    sqlite3_stmt *gbl_stmt;
    
    // Whereabouts we are in the virtual list control
    int gbl_expected;
    
    // Whereabouts we are in the virtual list control
    int gbl_current;
    
    // Misc
    std::map<int,DB_GAME_INFO>  cache;
    std::list<int>              stack;
    std::string player_name;
    bool is_start_pos;
    std::string where_white;
    std::string white_and;
    
    // Number of elements in the virtual list control
    int gbl_count;
    
    // The position we are looking for
public:
    thc::ChessPosition gbl_position;
    uint64_t gbl_hash;
    
};

#endif // Database_H
