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
#include "thc.h"
#include "GameDocument.h"
#include "MemoryPositionSearch.h"
#include "GamesCache.h"

enum DB_REQ
{
    REQ_POSITION, REQ_SHOW_ALL, REQ_PLAYERS, REQ_PATTERN
};


class Database
{
public:
    Database( const char *db_file );
    ~Database();
    void Reopen( const char *db_file );
    bool IsOperational( std::string &error_msg );
    bool GetDatabaseVersion( int &version );
    int SetDbPosition( DB_REQ db_req, thc::ChessRules &cr );
    int SetDbPosition( DB_REQ db_req, thc::ChessRules &cr, std::string &player_name );
    int GetGameCount();
    int GetRow( int row, CompactGame *info );
    int GetRowRaw( CompactGame *info, int row );
    bool LoadAllGamesForPositionSearch( std::vector< smart_ptr<ListableGame> > &mega_cache );
    bool TestNextRow();
    bool TestPrevRow();
    int GetCurrent();
    int  FindPlayer( std::string &name, std::string &current, int start_row, bool white );
    int LoadGameWithQuery( CompactGame *info, int game_id );
    //int LoadGamesWithQuery( const thc::ChessPosition &cp, uint64_t hash, std::vector< smart_ptr<ListableGame> > &games, std::unordered_set<int> &games_set );
    int LoadPlayerGamesWithQuery( std::string &player_name, bool white, std::vector< smart_ptr<ListableGame> > &games );
    MemoryPositionSearch tiny_db;
    int background_load_permill;
    bool kill_background_load;
  
private:
    DB_REQ db_req;
    bool is_open;
    bool is_bin_db; //TODO - get rid of this because since NOSQL db is alway bin_b
    bool is_pristine;
    bool player_search_stmt_bin_db;

    // Whereabouts we are in the virtual list control
    int gbl_expected;
    
    // Whereabouts we are in the virtual list control
    int gbl_current;
    
    // Misc
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
    uint64_t GetPositionHash()                  { return position_hash; }
    void     SetPositionHash( uint64_t hash )   { position_hash=hash; }
    thc::ChessPosition gbl_position;
private:
    uint64_t position_hash;
    
};

#endif // Database_H
