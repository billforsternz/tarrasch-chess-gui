/****************************************************************************
 *  Database subsystem for looking up positions from a database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <unordered_set>
#include "thc.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "CompressMoves.h"
#include "ListableGameDb.h"
#include "DbPrimitives.h"
#include "Database.h"
#include "Repository.h"
#include "wx/msgout.h"
#include "wx/progdlg.h"

//
//  This class has evolved into a DatabaseRead facility and should be
//  renamed as such
//


#define NBR_BUCKETS 4096

Database::Database( const char *db_file )
{
    gbl_handle = 0;
    gbl_stmt = 0;
    player_search_stmt = 0;
    gbl_expected = 0;
    gbl_current = 0;
    gbl_count = 0;
    
    // Access the database.
    int retval = sqlite3_open( db_file, &gbl_handle );
    is_open = (retval==0);
    
    // If connection failed, handle returns NULL
    dbg_printf( "DATABASE CONSTRUCTOR %s\n", retval ? "FAILED" : "SUCCESSFUL" );
}

Database::~Database()
{
    cprintf( "DATABASE DESTRUCTOR\n" );
    if( gbl_stmt )
    {
        sqlite3_finalize(gbl_stmt);
        gbl_stmt = NULL;
    }
    if( gbl_handle )
    {
        sqlite3_close(gbl_handle);
        gbl_handle = NULL;
    }
}

void Database::Reopen( const char *db_file )
{
    if( gbl_stmt )
    {
        sqlite3_finalize(gbl_stmt);
        gbl_stmt = NULL;
    }
    if( gbl_handle )
    {
        sqlite3_close(gbl_handle);
        gbl_handle = NULL;
    }
    gbl_handle = 0;
    gbl_stmt = 0;
    player_search_stmt = 0;
    gbl_expected = 0;
    gbl_current = 0;
    gbl_count = 0;
    
    // Access the database.
    int retval = sqlite3_open(db_file,&gbl_handle);
    is_open = (retval==0);
    
    // If connection failed, handle returns NULL
    dbg_printf( "DATABASE REOPEN %s\n", retval ? "FAILED" : "SUCCESSFUL" );
}

// Return bool operational
bool Database::IsOperational( std::string &error_msg )
{
    bool operational = false;
    error_msg = "";
    if( !is_open )
        error_msg = "Could not open database";
    else
    {
        int version;
        bool ok = GetDatabaseVersion( version );
        if( !ok )
            error_msg = "Could not read version number from database";
        else
        {
            if( version != DATABASE_VERSION_NUMBER_SUPPORTED )
                error_msg = "Could not read version number from database";
            else
                operational = true;
        }
    }
    return operational;
}

// Return bool okay
bool Database::GetDatabaseVersion( int &version )
{
    bool ok=false;
    version = 0;
    sqlite3_stmt *stmt;
    int retval;
    retval = sqlite3_prepare_v2( gbl_handle, "SELECT description,version from description", -1, &stmt, 0 );
    if( retval )
    {
        cprintf("SELECT description,version FROM DB FAILED\n");
        return false;
    }
    
    // Read the number of rows fetched
    int cols = sqlite3_column_count(stmt);
    retval = sqlite3_step(stmt);
    if( retval == SQLITE_ROW )
    {
        for( int col=0; col<cols; col++ )
        {
            switch(col)
            {
                case 0:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    cprintf( "Description = %s\n", val );
                    break;
                }
                case 1:
                {
                    int val = (int)sqlite3_column_int(stmt,col);
                    cprintf( "Version integer = %d\n", val );
                    version = val;
                    ok = true;
                    break;
                }
            }
        }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
    return ok;
}


int Database::SetDbPosition( DB_REQ db_req, thc::ChessRules &cr )
{
    std::string empty;
    return SetDbPosition( db_req, cr, empty );
}


int Database::SetDbPosition( DB_REQ db_req, thc::ChessRules &cr, std::string &player_name )
{
    this->db_req = db_req;
    if( !gbl_handle )
        return 0;
    gbl_expected = -1;
    int game_count = 0;
    this->player_name = player_name;
    
    //cr.Forsyth("r1bqk2r/ppp1bppp/2n1pn2/3p4/Q1PP4/P3PN2/1P1N1PPP/R1B1KB1R b KQkq - 0 7");
    gbl_hash = cr.Hash64Calculate();
    gbl_position = cr;
    cache.clear();
    stack.clear();
    
    // select matching rows from the table
    char buf[1000];
    int table_nbr;
    int hash;
    uint64_t temp=gbl_hash;
    table_nbr = ((int)(temp>>32))&(NBR_BUCKETS-1);
    hash = (int)(temp);
    if( player_name.length() == 0 )
        where_white="";
    else
    {
        where_white = " WHERE games.white='";
        where_white += player_name;
        where_white += + "'";
    }
    if( player_name.length() == 0 )
        white_and = "";
    else
    {
        white_and = "games.white='";
        white_and += player_name;
        white_and += + "' AND ";
    }
    if( db_req == REQ_PLAYERS )
    {
        sprintf( buf, "SELECT COUNT(*) from games%s", where_white.c_str() );
    }
    else if( db_req == REQ_SHOW_ALL )
    {
        sprintf( buf, "SELECT COUNT(*) from games" );
    }
    else if( db_req == REQ_POSITION )
    {
        sprintf( buf, "SELECT COUNT(*) from games, positions_%d WHERE %spositions_%d.position_hash=%d AND games.game_id = positions_%d.game_id",
                table_nbr, white_and.c_str(), table_nbr, hash, table_nbr );
    }
    //sprintf( buf, "SELECT COUNT(*) from games, positions_%d WHERE games.white = 'Carlsen, Magnus'  AND positions_%d.position_hash=%d AND games.game_id = positions_%d.game_id", table_nbr, table_nbr, hash, table_nbr );
    //    sprintf( buf, "SELECT COUNT(*) from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE games.white = 'Carlsen, Magnus' AND positions_%d.position_hash=%d", table_nbr, table_nbr, table_nbr, hash );
    //    sprintf( buf, "SELECT COUNT(*) from positions_%d WHERE position_hash=%d", table_nbr, hash );
    //    sprintf( buf, "SELECT COUNT(*) from games WHERE games.white = 'Carlsen, Magnus' AND games.game_id = positions_%d.game_id AND positions_%d.position_hash=%d", table_nbr, table_nbr, hash );
    cprintf("QUERY IN: %s\n",buf);
    int retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
    cprintf("QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 1\n");
        return 0;
    }
    
    // Read the number of rows fetched
    int cols = sqlite3_column_count(gbl_stmt);
    
    cprintf( "Get games count begin\n");
    while(1)
    {
        // fetch a row's status
        retval = sqlite3_step(gbl_stmt);
        if(retval == SQLITE_ROW)
        {
            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                if( col == 0 )
                {
                    game_count = atoi(val);
                    cprintf( "Game count = %d\n", game_count );
                }
            }
        }
        else if( retval == SQLITE_DONE )
        {
            // All rows finished
            sqlite3_finalize(gbl_stmt);
            gbl_stmt = NULL;
            break;
        }
        else
        {
            // Some error encountered
            cprintf("SOME ERROR ENCOUNTERED\n");
        }
    }
    cprintf("Get games count end\n");
    gbl_count = game_count;
    return game_count;
}

#if 0
int Database::GetNbrGames( DB_REQ db_req, thc::ChessRules &cr )
{
    if( !gbl_handle )
        return 0;
    char buf[1000];
    uint64_t temp = cr.Hash64Calculate();
    int table_nbr = ((int)(temp>>32))&(NBR_BUCKETS-1);
    int hash = (int)(temp);
    int game_count=0;
    if( db_req == REQ_POSITION )
    {
        sprintf( buf, "SELECT COUNT(*) from games, positions_%d WHERE positions_%d.position_hash=%d AND games.game_id = positions_%d.game_id",
                table_nbr, table_nbr, hash, table_nbr );
    }
    else
    {
        sprintf( buf, "SELECT COUNT(*) from games"  );
    }
    cprintf("QUERY IN: %s\n",buf);
    int retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
    cprintf("QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 1\n");
        return 0;
    }
    
    // Read the number of rows fetched
    int cols = sqlite3_column_count(gbl_stmt);
    cprintf( "Get games count begin\n");
    while(1)
    {
        // fetch a row's status
        retval = sqlite3_step(gbl_stmt);
        
        if(retval == SQLITE_ROW)
        {
            // SQLITE_ROW means fetched a row
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                if( col == 0 )
                {
                    game_count = atoi(val);
                    cprintf( "Game count = %d\n", game_count );
                }
            }
        }
        else if( retval == SQLITE_DONE )
        {
            // All rows finished
            sqlite3_finalize(gbl_stmt);
            gbl_stmt = NULL;
            break;
        }
        else
        {
            // Some error encountered
            cprintf("SOME ERROR ENCOUNTERED\n");
        }
    }
    cprintf("Get games count end\n");
    return game_count;
}
#endif

int Database::LoadGameWithQuery( CompactGame *pact, int game_id )
{
    pact->game_id = game_id;
    
    // Get white player
    char buf[1000];
    sqlite3_stmt *stmt;    // A prepared statement for fetching from games table
    int retval;
    sprintf( buf, "SELECT white,black,event,site,result,date,white_elo,black_elo,moves from games WHERE game_id=%d", game_id );
    cprintf("QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
    cprintf("QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED\n");
        return retval;
    }
    // Read the number of rows fetched
    int cols = sqlite3_column_count(stmt);
    retval = sqlite3_step(stmt);
    if( retval == SQLITE_ROW )
    {
        for( int col=0; col<cols; col++ )
        {
            switch(col)
            {
                case 0:
                {
                    // sqlite3_column_text returns a const void* , typecast it to const char*
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.white = val;
                    break;
                }
                case 1:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.black = val;
                    break;
                }
                case 2:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.event = val;
                    break;
                }
                case 3:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.site = val;
                    break;
                }
                case 4:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.result = val;
                    break;
                }
                case 5:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.date = val;
                    break;
                }
                case 6:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.white_elo = val;
                    break;
                }
                case 7:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.black_elo = val;
                    break;
                }
                case 8:
                {
                    int len = sqlite3_column_bytes(stmt,col);
                    const char *blob = (const char*)sqlite3_column_blob(stmt,col);
                    std::string str_blob;
                    if( len && blob )
                    {
                        std::string temp(blob,len);
                        str_blob = temp;
                    }
                    CompressMoves press;
                    pact->moves = press.Uncompress(str_blob);
                    break;
                }
            }
        }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
    return 0;
}

static bool gbl_protect_recursion;   // FIXME


int Database::LoadGamesWithQuery(  std::string &player_name, bool white, std::vector< smart_ptr<ListableGame> > &games )
{
    int nbr_before = games.size();
    int retval=-1;
    if( !gbl_handle )
        return -1;
    gbl_expected = -1;
    
    // select matching rows from the table
    char buf[1000];
    sprintf( buf,
            "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games "
            "WHERE games.%s='%s'",  white?"white":"black",  player_name.c_str() );
    cprintf( "LoadGamesWithQuery(), player; QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
    cprintf( "LoadGamesWithQuery(), player; QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 2\n");
        return -1;
    }
    
    // Read the game info
    int cols = sqlite3_column_count(gbl_stmt);
    for(;;)
    {
        retval = sqlite3_step(gbl_stmt);
        if( retval == SQLITE_ROW )
        {
            int game_id=0;
            Roster r;
            std::string str_blob;
            
            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                if( col == 0 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    game_id = atoi(val);
                }
                else if( col == 1 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.result = val ? std::string(val) : "*";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    int len = sqlite3_column_bytes(gbl_stmt,col);
                    const char *blob = (const char*)sqlite3_column_blob(gbl_stmt,col);
                    if( len && blob )
                    {
                        std::string temp(blob,len);
                        str_blob = temp;
                    }
                    ListableGameDb info( game_id, r, str_blob );
                    make_smart_ptr( ListableGameDb, new_info, info );
                    games.push_back( std::move(new_info) );
                }
            }
        }
        else if( retval == SQLITE_DONE )
        {
            // All rows finished
            sqlite3_finalize(gbl_stmt);
            gbl_stmt = NULL;
            int nbr_after = games.size();
            cprintf("LoadGamesWithQuery(): %d games loaded\n", nbr_after-nbr_before );
            return nbr_after-nbr_before;
        }
        else
        {
            // Some error encountered
            cprintf("SOME ERROR ENCOUNTERED\n");
            return -1;
        }
    }
    return -1;
}


int Database::LoadGamesWithQuery( uint64_t hash, std::vector< smart_ptr<ListableGame> > &games, std::unordered_set<int> &games_set )
{
    int retval=-1;
    int nbr_before = games.size();
    wxProgressDialog progress( "Searching for extra games", "Searching for extra games", 100, NULL,
                              wxPD_APP_MODAL+
                              wxPD_AUTO_HIDE+
                              wxPD_ELAPSED_TIME+
                              wxPD_ESTIMATED_TIME );
    
    
    // select matching rows from the table
    char buf[1000];
    gbl_expected = -1;
    int old_percent=0, nbr_new=0, nbr_already=0;
    int hash32 = (int)(hash);
    int table_nbr = (int)((hash>>32)&(NBR_BUCKETS-1));
    sprintf( buf,
            "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d",
            table_nbr, table_nbr, white_and.c_str(), table_nbr, hash32);
    cprintf( "LoadGamesWithQuery(), position; QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
    cprintf( "LoadGamesWithQuery(), position; QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 2\n");
        return retval;
    }
    
    // Read the game info
    int cols = sqlite3_column_count(gbl_stmt);
    for(;;)
    {
        retval = sqlite3_step(gbl_stmt);
        if( retval == SQLITE_ROW )
        {
            int game_id=0;
            Roster r;
            std::string str_blob;
            
            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                if( col == 0 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    game_id = atoi(val);
                    int count = games_set.count(game_id);
                    bool already = (count > 0);
                    if( already )
                        nbr_already++;
                    else
                        nbr_new++;
                    if( already )
                        break;
                }
                else if( col == 1 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.result = val ? std::string(val) : "*";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    int len = sqlite3_column_bytes(gbl_stmt,col);
                    //fprintf(f,"Move len = %d\n",len);
                    const char *blob = (const char*)sqlite3_column_blob(gbl_stmt,col);
                    if( len && blob )
                    {
                        std::string temp(blob,len);
                        str_blob = temp;
                    }
                    ListableGameDb info( game_id, r, str_blob );
                    make_smart_ptr( ListableGameDb, new_info, info );
                    games.push_back( std::move(new_info) );
                    games_set.insert( info.GetGameId() );
                    if( nbr_new > 5 )
                    {
                        int estimated_total = (nbr_before * (nbr_new+nbr_already) ) / (nbr_already?nbr_already:1);
                        int percent = (nbr_new + nbr_before * 100) / (estimated_total?estimated_total:1);
                        if( percent < 1 )
                            percent = 1;
                        if( percent > 90 )
                            percent = 90;
                        if( percent > old_percent )
                        {
                            progress.Update( percent>100 ? 100 : percent );
                            old_percent = percent;
                        }
                    }
                }
            }
        }
        else if( retval == SQLITE_DONE )
        {
            // All rows finished
            sqlite3_finalize(gbl_stmt);
            gbl_stmt = NULL;
            int nbr_after = games.size();
            cprintf("LoadGamesWithQuery(): %d extra games loaded\n", nbr_after-nbr_before );
            break;
        }
        else
        {
            // Some error encountered
            cprintf("SOME ERROR ENCOUNTERED\n");
            return retval;
        }
    }
    return retval;
}


int Database::GetRow( int row, CompactGame *info )
{
    int ret=0;
    if( 0 != cache.count(row) )
    {
        *info = cache.at(row);
        cprintf( "Cache hit, row %d\n", row );
    }
    else
    {
        ret = GetRowRaw( info, row );
        if( ret == 0 )
        {
            cache[row] = *info;
            stack.push_back(row);
            if( stack.size() < 100 )
            {
                cprintf( "Add row %d to cache, no removal\n", row );
            }
            else
            {
                int front = *stack.begin();
                cprintf( "Add row %d and remove row %d from cache\n", row, front );
                stack.pop_front();
                cache.erase(front);
            }
        }
    }
    return ret;
}

int Database::GetRowRaw( CompactGame *pact, int row )
{
    if( gbl_protect_recursion )
    {
        cprintf( "Didn't expect that\n");
        return 0;
    }
    gbl_current = row;
    cprintf( "Database::GetRow() IN row=%d, expected=%d,%smatch\n", row, gbl_expected, row==gbl_expected?" ":" no " );
    int retval = -1;
    if( !gbl_handle || row>=gbl_count )
    {
        return retval;
    }
    static int cols;
    for(;;)
    {
        if( row == gbl_expected )
        {
            gbl_expected++;
            for(;;)
            {
                // fetch a row's status
                retval = sqlite3_step(gbl_stmt);
                if( retval == SQLITE_ROW )
                {
                    // SQLITE_ROW means fetched a row
                    
                    // sqlite3_column_text returns a const void* , typecast it to const char*
                    for( int col=0; col<cols; col++ )
                    {
                        const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                        //cprintf("%s:%s\t",sqlite3_column_name(gbl_stmt,col),val);
                        if( col == 0 )
                        {
                            int game_id = atoi(val);
                            retval = LoadGameWithQuery( pact, game_id );
                            cprintf( "Database::GetRow() SUCCESS game_id = %d\n", game_id );
                            return retval;
                        }
                    }
                }
                else if( retval == SQLITE_DONE )
                {
                    // All rows finished
                    sqlite3_finalize(gbl_stmt);
                    gbl_stmt = NULL;
                    gbl_expected = -1;
                    cprintf( "Limit reached\n");
                    break;
                }
                else
                {
                    // Some error encountered
                    cprintf("SOME ERROR ENCOUNTERED\n");
                    return retval;
                }
            }
        }
        else
        {
            // select matching rows from the table
            char buf[1000];
            gbl_expected = row;
            // sprintf( buf, "SELECT game_id from positions WHERE position_hash=%d LIMIT %d,100", gbl_hash, row );
//            sprintf( buf, "SELECT positions.game_id from positions JOIN games ON games.game_id = positions.game_id AND positions.position_hash=%d ORDER BY games.white LIMIT %d,100", gbl_hash, row );
            //sprintf( buf, "SELECT positions.game_id from positions JOIN games ON games.game_id = positions.game_id AND positions.position_hash=%d LIMIT %d,100", gbl_hash, row );
            uint64_t temp = gbl_hash;
            int hash = (int)(temp);
            int table_nbr = (int)((temp>>32)&(NBR_BUCKETS-1));
            // sprintf( buf, "SELECT positions_%d.game_id from positions_%d JOIN games ON games.game_id = positions_%d.game_id AND positions_%d.position_hash=%d LIMIT %d,100",
            //        table_nbr, table_nbr, table_nbr, table_nbr, hash, row );
            if( db_req == REQ_PLAYERS )
            {
                sprintf( buf, 
                        "SELECT games.game_id from games%s ORDER BY games.white ASC LIMIT %d,100", where_white.c_str(), row );
                  //      "SELECT games.game_id from games%s ORDER BY games.rowid LIMIT %d,100", where_white.c_str(), row );
            }
            else if( db_req == REQ_SHOW_ALL )
            {
                sprintf( buf,
//#define NO_REVERSE
#ifdef NO_REVERSE
                        "SELECT games.game_id from games LIMIT %d,100", row );
#else
                        "SELECT games.game_id from games ORDER BY games.rowid DESC LIMIT %d,100", row );
#endif
            }
            else if( db_req == REQ_POSITION )
            {
                sprintf( buf,
//#define NO_REVERSE
#ifdef NO_REVERSE
                        "SELECT games.game_id from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d LIMIT %d,100",
#else
                        "SELECT games.game_id from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d ORDER BY games.rowid DESC LIMIT %d,100",
#endif
                        table_nbr, table_nbr, white_and.c_str(), table_nbr, hash, row );
            }
            retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
            cprintf( "Database::GetRow() START query: %s\n",buf);
            if( retval )
            {
                cprintf("SELECTING DATA FROM DB FAILED 2\n");
                return retval;
            }

            // Read the number of rows fetched
            cols = sqlite3_column_count(gbl_stmt);
        }
    }
}


int Database::LoadAllGames( std::vector< smart_ptr<ListableGame> > &cache, int nbr_games )
{
    gbl_protect_recursion = true;

    wxProgressDialog progress( "Loading games", "Loading games", 100, NULL,
                              wxPD_APP_MODAL+
                              wxPD_AUTO_HIDE+
                              wxPD_ELAPSED_TIME+
                              wxPD_CAN_ABORT+
                              wxPD_ESTIMATED_TIME );
    
    int retval=-1;
    cache.clear();
    
    // select matching rows from the table
    char buf[1000];
    gbl_expected = -1;
    uint64_t temp = gbl_hash;
    int hash = (int)(temp);
    int table_nbr = (int)((temp>>32)&(NBR_BUCKETS-1));
    if( db_req == REQ_PLAYERS )
    {
        sprintf( buf,
                "SELECT games.game_id from games%s ORDER BY games.white ASC", where_white.c_str() );
    }
    else if( db_req == REQ_SHOW_ALL )
    {
        sprintf( buf,
#if 1 //def NO_REVERSE
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games" );
#else
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games ORDER BY games.rowid DESC" );
#endif
    }
    else if( db_req == REQ_POSITION )
    {
        sprintf( buf,
#if 1 //def NO_REVERSE
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d",
#else
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d ORDER BY games.rowid DESC",
#endif
                table_nbr, table_nbr, white_and.c_str(), table_nbr, hash);
    }
    cprintf( "LoadAllGames() QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
    cprintf( "LoadAllGames() QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 2\n");
        gbl_protect_recursion = false;
        return retval;
    }
    
    // Read the game info
    int cols = sqlite3_column_count(gbl_stmt);
    for(;;)
    {
        retval = sqlite3_step(gbl_stmt);
        if( retval == SQLITE_ROW )
        {
            int game_id=0;
            Roster r;
            std::string str_blob;

            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                if( col == 0 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    game_id = atoi(val);
                }
                else if( col == 1 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.result = val ? std::string(val) : "*";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    int len = sqlite3_column_bytes(gbl_stmt,col);
                    //fprintf(f,"Move len = %d\n",len);
                    const char *blob = (const char*)sqlite3_column_blob(gbl_stmt,col);
                    if( len && blob )
                    {
                        std::string temp(blob,len);
                        str_blob = temp;
                    }
                }
            }
            ListableGameDb info( game_id, r, str_blob );
            make_smart_ptr( ListableGameDb, new_info, info );
            cache.push_back( std::move(new_info) );

            int percent = (cache.size()*100) / (nbr_games?nbr_games:1);
            if( percent < 1 )
                percent = 1;
            if( !progress.Update( percent>100 ? 100 : percent ) )
            {
                cache.clear();
                break;
            }
        }
        else if( retval == SQLITE_DONE )
        {
            // All rows finished
            sqlite3_finalize(gbl_stmt);
            gbl_stmt = NULL;
            cprintf("LoadAllGames(): %u game_ids loaded\n", cache.size() );
            break;
        }
        else
        {
            // Some error encountered
            cprintf("SOME ERROR ENCOUNTERED\n");
            gbl_protect_recursion = false;
            return retval;
        }
    }
    gbl_protect_recursion = false;
    return retval;
}

void Database::FindPlayerEnd()
{
    if( player_search_stmt )
    {
        sqlite3_finalize(player_search_stmt);
        player_search_stmt = NULL;
    }
}

// Returns row
int Database::FindPlayer( std::string &name, std::string &current, int start_row, bool white )
{
    bool okay = true;
    int retval = -1;                               
    int row = 0;
    gbl_expected = -1;
    std::string input(name);
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    std::transform(current.begin(), current.end(), current.begin(), ::tolower);
    int input_len = input.length();
    if( player_search_stmt &&
        prev_name == name &&
        prev_current == current &&
        prev_white == white &&
        prev_row == start_row
      )
    {
        row = start_row+1;
    }
    else
    {
        if( player_search_stmt )
        {
            sqlite3_finalize(player_search_stmt);
            player_search_stmt = NULL;
        }
        const char *txt = white ? "SELECT games.white from games ORDER BY games.white ASC" : "SELECT games.black from games ORDER BY games.white ASC";
        cprintf( "FindPlayer() QUERY IN: %s\n",txt);
        retval = sqlite3_prepare_v2( gbl_handle, txt, -1, &player_search_stmt, 0 );
        cprintf( "FindPlayer() QUERY OUT: %s\n",txt);
        okay = !retval;
    }
    while( okay )
    {
        retval = sqlite3_step(player_search_stmt);
        okay = (retval == SQLITE_ROW);
        if( okay )
        {
            const char *val = (const char*)sqlite3_column_text(player_search_stmt,0);
            std::string player(val);
            std::transform(player.begin(), player.end(), player.begin(), ::tolower);
            int player_len = player.length();
            if( player!=current && input_len<=player_len && player.substr(0,input_len)==input )
            {
                prev_name = name;
                prev_current = player;
                prev_white = white;
                prev_row = row;
                return row;
            }
            row++;
        }
    }
    sqlite3_finalize(player_search_stmt);
    player_search_stmt = NULL;
    return start_row;
}




// Returns row
bool Database::TestNextRow()
{
    int next = gbl_current+1;
    return( gbl_count>0 && 0<=next && next<gbl_count );
}


bool Database::TestPrevRow()
{
    int next = gbl_current-1;
    return( gbl_count>0 && 0<=next && next<gbl_count );
}

int Database::GetCurrent()
{
    return gbl_current;
}

