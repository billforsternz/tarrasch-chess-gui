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

enum DB_REQ
{
    REQ_POSITION, REQ_SHOW_ALL, REQ_PLAYERS
};


class Database
{
public:
    Database();
    ~Database();
    void Reopen( const char *db_file );
    int SetDbPosition( DB_REQ db_req, thc::ChessRules &cr );
    int SetDbPosition( DB_REQ db_req, thc::ChessRules &cr, std::string &player_name );
    // int GetNbrGames( thc::ChessRules &cr );
    int GetRow( int row, CompactGame *info );
    int GetRowRaw( CompactGame *info, int row );
    int LoadAllGames( std::vector< smart_ptr<MagicBase> > &cache, int nbr_games );
    bool TestNextRow();
    bool TestPrevRow();
    int GetCurrent();
    int  FindPlayer( std::string &name, std::string &current, int start_row, bool white );
    void FindPlayerEnd();

    int LoadGameWithQuery( CompactGame *info, int game_id );
    int LoadGamesWithQuery( uint64_t hash, std::vector< smart_ptr<MagicBase> > &games, std::unordered_set<int> &games_set );
    int LoadGamesWithQuery( std::string &player_name, bool white, std::vector< smart_ptr<MagicBase> > &games );
  
private:
    DB_REQ db_req;

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
    std::map<int,CompactGame>  cache;
    std::list<int>              stack;
    std::string player_name;
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
