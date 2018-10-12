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
    Database( const char *db_file, bool another_instance_running );
    void Reopen( const char *db_file );
    bool IsOperational( std::string &error_msg );
	int  SetDbPosition(DB_REQ db_req);
    int  GetRow( int row, CompactGame *pact );
    bool LoadAllGamesForPositionSearch( std::vector< smart_ptr<ListableGame> > &mega_cache );
    int  FindPlayer( std::string &name, std::string &current, int start_row, bool white );
    int LoadPlayerGamesWithQuery( std::string &player_name, bool white, std::vector< smart_ptr<ListableGame> > &games );
    MemoryPositionSearch tiny_db;
    int background_load_permill;
    bool kill_background_load;
    std::string GetStatus();
  
private:
    std::string db_filename;
    DB_REQ db_req;
    bool is_open;
    std::string database_error_msg; // explanation if is_open is false
    bool player_search_in_progress;

    // Misc
    std::string prev_name;
    std::string prev_current;
    bool        prev_white;
    int         prev_row;
    
    // The position we are looking for
public:
    uint64_t GetPositionHash()                  { return position_hash; }
    void     SetPositionHash( uint64_t hash )   { position_hash=hash; }
    thc::ChessPosition gbl_position;
private:
    uint64_t position_hash;
    
};

#endif // Database_H
