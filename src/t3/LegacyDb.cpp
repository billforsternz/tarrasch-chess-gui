/****************************************************************************
 * LegacyDb - Partial support for old SQL chess database subsystem
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include <set>
#include <map>
#include <time.h> // time_t
#include <stdio.h>
#include <stdarg.h>
#include "sqlite3.h"
#include "CompressMoves.h"
#include "PgnRead.h"
#include "CompactGame.h"
#include "ListableGameDb.h"
#include "LegacyDb.h"

// Handle for database connection
static sqlite3 *gbl_handle;

// Returns bool okay
bool LegacyDbLoadAllGames(  const char *db_file, bool for_db_append, std::vector< smart_ptr<ListableGame> > &mega_cache, int &background_load_permill, bool &kill_background_load, ProgressBar *pb )
{
    bool ok = true;
    mega_cache.clear();
    cprintf( "LegacyDbLoadAllGames() db=%s\n", db_file );

    // Try to create the database. If it doesnt exist, it would be created
    //  pass a pointer to the pointer to sqlite3, in short sqlite3**
    int retval = sqlite3_open( db_file, &gbl_handle );
    
    // If connection failed, handle returns NULL
    if( retval )
    {
        cprintf( "Database error: CONNECTION FAILED\n" );
        ok = false;
    }

    background_load_permill = 0;
    kill_background_load = false;
    int idx=0;
    int cols=0;
    int nbr_games=0;
    int nbr_promotion_games=0;
    sqlite3_stmt *stmt;
    mega_cache.clear();

    // Calculate number of games
    int game_count = 0;
    if( ok )
    {
        const char *sql = "SELECT COUNT(*) from games";
        cprintf("QUERY IN: %s\n",sql);
        retval = sqlite3_prepare_v2( gbl_handle, sql, -1, &stmt, 0 );
        cprintf("QUERY OUT: %s\n",sql);
        if( retval )
        {
            cprintf("SELECTING DATA FROM DB FAILED 1\n");
            ok = false;
        }
    }
    
    // Read the number of rows fetched
    if( ok )
    {
        cols = sqlite3_column_count(stmt);
        cprintf( "Get games count begin\n");
        bool have_game_count=false;
        while( !have_game_count )
        {
            // fetch a row's status
            retval = sqlite3_step(stmt);
            if( retval == SQLITE_ROW )
            {
                // SQLITE_ROW means fetched a row
            
                // sqlite3_column_text returns a const void* , typecast it to const char*
                for( int col=0; col<cols; col++ )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    if( col == 0 )
                    {
                        game_count = atoi(val);
                        cprintf( "Game count = %d\n", game_count );
                        have_game_count = true;
                        break;
                    }
                }
            }
            else if( retval == SQLITE_DONE )
            {
                // All rows finished
                sqlite3_finalize(stmt);
                stmt = NULL;
                break;
            }
            else
            {
                // Some error encountered
                cprintf("SOME DB ERROR ENCOUNTERED\n");
                ok = false;
                break;
            }
        }
    }

    // Select matching rows from the table
    if( ok )
    {
        char buf[1000];
        sprintf( buf,
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.round, games.result, games.date, games.white_elo, games.black_elo, games.eco, games.moves from games" );
        cprintf( "LoadAllGames() QUERY IN: %s\n",buf);
        retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
        cprintf( "LoadAllGames() QUERY OUT: %s\n",buf);
        if( retval )
        {
            cprintf("SELECTING DATA FROM DB FAILED 2\n");
            ok = false;
        }
    }
    
    // Read the game info
    while( ok && !kill_background_load )
    {
        retval = sqlite3_step(stmt);
        if( retval == SQLITE_ROW )
        {
            // SQLITE_ROW means fetched a row
            int game_id=0;
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            bool promotion=false;
            const char *val        = (const char*)sqlite3_column_text(stmt,0);
            game_id                = atoi(val);
            const char      *white = (const char*)sqlite3_column_text(stmt,1);
            int              len1  = sqlite3_column_bytes(stmt,1);
            const char      *black = (const char*)sqlite3_column_text(stmt,2);
            int               len2 = sqlite3_column_bytes(stmt,2);
            const char      *event = (const char*)sqlite3_column_text(stmt,3);
            int               len3 = sqlite3_column_bytes(stmt,3);
            const char       *site = (const char*)sqlite3_column_text(stmt,4);
            int               len4 = sqlite3_column_bytes(stmt,4);
            const char      *round = (const char*)sqlite3_column_text(stmt,5);
            int               len6 = sqlite3_column_bytes(stmt,5);
            const char     *result = (const char*)sqlite3_column_text(stmt,6);
            int               len5 = sqlite3_column_bytes(stmt,6);
            const char       *date = (const char*)sqlite3_column_text(stmt,7);
            int               len7 = sqlite3_column_bytes(stmt,7);
            const char  *white_elo = (const char*)sqlite3_column_text(stmt,8);
            int               len9 = sqlite3_column_bytes(stmt,8);
            const char  *black_elo = (const char*)sqlite3_column_text(stmt,9);
            int              len10 = sqlite3_column_bytes(stmt,9);
            const char        *eco = (const char*)sqlite3_column_text(stmt,10);
            int               len8 = sqlite3_column_bytes(stmt,10);
            const char *moves_blob = (const char*)sqlite3_column_text(stmt,11);
            int              len12 = sqlite3_column_bytes(stmt,11);
            ListableGameDb info
            (
                game_id,
                white, len1,
                black, len2,
                event, len3,
                site, len4,
                result, len5,
                round, len6,
                date, len7,
                eco, len8,
                white_elo, len9,
                black_elo, len10,
                "", 0, //fen
                moves_blob, len12
            );
            make_smart_ptr( ListableGameDb, new_info, info );
            mega_cache.push_back( std::move(new_info) );
            int num = idx;
            int den = game_count?game_count:1;
            if( den > 1000000 )
                background_load_permill = num / (den/1000);
            else
                background_load_permill = (num*1000) / den;
            if( info.game_attributes )
                nbr_promotion_games++;
            nbr_games++;
            if(
                ((nbr_games%10000) == 0 ) /* ||
                                            (
                                            (nbr_games < 100) &&
                                            ((nbr_games%10) == 0 )
                                            ) */
                )
            {
                cprintf( "%d games (%d include promotion)\n", nbr_games, nbr_promotion_games );
            }

        }
        else if( retval == SQLITE_DONE )
        {
            // All rows finished
            sqlite3_finalize(stmt);
            stmt = NULL;
            //cprintf( "Reversing order to get newest games first\n" );
            //std::reverse( cache.begin(), cache.end() );
            //cprintf("LoadAllGames(): %u game_ids loaded\n", cache.size() );
            break;
        }
        else
        {
            // Some error encountered
            cprintf("SOME ERROR ENCOUNTERED\n");
            ok = false;
        }
    }
    int cache_nbr = mega_cache.size();
    cprintf( "Number of games = %d\n", cache_nbr );
    if( 2 < cache_nbr )
    {
        smart_ptr<ListableGame> p = mega_cache[2];
        cprintf( "White=%s Black=%s Result=%s Round=%s\n", p->White(), p->Black(), p->Result(), p->Round() );
    }
    cprintf( "%d games (%d include promotion)\n", nbr_games, nbr_promotion_games );
    if( gbl_handle )
    {
        sqlite3_close(gbl_handle);
        gbl_handle = 0;
    }
    return ok;
}
