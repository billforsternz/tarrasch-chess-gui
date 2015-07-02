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
#include "GamesCache.h"

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
    int LoadAllGames( std::vector< smart_ptr<MagicBase> > &cache, int nbr_games );
    bool TestNextRow();
    bool TestPrevRow();
    int GetCurrent();
    int  FindPlayer( std::string &name, std::string &current, int start_row, bool white );
    void FindPlayerEnd();

    int LoadGameWithQuery( DB_GAME_INFO *info, int game_id );
    int LoadGamesWithQuery( uint64_t hash, std::vector< smart_ptr<MagicBase> > &games, std::unordered_set<int> &games_set );
    int LoadGamesWithQuery( std::string &player_name, bool white, std::vector< smart_ptr<MagicBase> > &games );
  
private:
    // Create a handle for database connection, create a pointer to sqlite3
    sqlite3 *gbl_handle;
    
    // A prepared statement for fetching from positions table
    sqlite3_stmt *gbl_stmt;
    
    // A prepared statement for player search
    sqlite3_stmt *player_search_stmt;

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
    std::string prev_name;
    std::string prev_current;
    bool        prev_white;
    int         prev_row;
    
    // Number of elements in the virtual list control
    int gbl_count;
    
    // The position we are looking for
public:
    thc::ChessPosition gbl_position;
    uint64_t gbl_hash;
    
};

#endif // Database_H
