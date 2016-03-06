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
#include "AutoTimer.h"
#include "CompressMoves.h"
#include "ListableGameDb.h"
#include "PgnRead.h"
#include "ProgressBar.h"
#include "DbPrimitives.h"
#include "Database.h"
#include "Repository.h"
#if !wxUSE_THREADS
    #error "Requires thread support!"
#endif // wxUSE_THREADS
#include "wx/thread.h"
#include "wx/msgout.h"
#include "wx/progdlg.h"
#include "wx/filename.h"

//
//  This class has evolved into a DatabaseRead facility and should be
//  renamed as such
//

#define NBR_BUCKETS 4096

// Use a worker thread to load tiny database games
class WorkerThread : public wxThread
{
public:
    //WorkerThread();
    //virtual ~WorkerThread();

    // thread execution starts here
    virtual void *Entry();
};


static Database *the_database;

static void LoadInBackground( Database *ptr1 )
{
    the_database = ptr1;
    WorkerThread *thread = new WorkerThread;
    int err = thread->Create();
    cprintf( "LoadInBackground(), err=%d\n", err ); 
    if( err == wxTHREAD_NO_ERROR )
    {
        cprintf( "Running worker thread\n" ); 
        thread->Run();
    }
}

wxMutex s_mutex_tiny_database;
void * WorkerThread::Entry()
{
    static int count;
    int temp = ++count;
    cprintf( "Entering worker thread, may wait for mutex (%d)...\n", temp );
    {
        wxMutexLocker lock(s_mutex_tiny_database);
        {
            cprintf( "... if we waited for mutex, wait is over (%d)\n", temp );
            the_database->LoadAllGamesForPositionSearch( the_database->tiny_db.in_memory_game_cache );
        }
    }
    return 0;
}

wxMutex *KillWorkerThread()
{   
    if( the_database )
        the_database->kill_background_load = true;
    return &s_mutex_tiny_database;
}


wxMutex *WaitForWorkerThread()
{   
    if( wxMUTEX_BUSY == s_mutex_tiny_database.TryLock() )
    {
        ProgressBar progress("Initial Database Load","Loading database into memory",false);
        while( wxMUTEX_BUSY == s_mutex_tiny_database.TryLock() )
        {
            progress.Permill( the_database->background_load_permill );
            wxSafeYield();
        }
    }
    s_mutex_tiny_database.Unlock();
    return &s_mutex_tiny_database;
}

Database::Database( const char *db_file )
{
    gbl_handle = 0;
    background_load_permill = 0;
    kill_background_load = false;
    positions_stmt = 0;
    player_search_stmt = 0;
    gbl_expected = 0;
    gbl_current = 0;
    gbl_count = 0;
    is_tiny_db = false;
    is_pristine = false;

    // Access the database.
    int retval = sqlite3_open( db_file, &gbl_handle );
    is_open = (retval==0);
    std::string nomsg;
    IsOperational( nomsg );
    if( is_open && is_tiny_db  )
    {
#define USE_BACKGROUND_THREAD
#ifdef USE_BACKGROUND_THREAD
        LoadInBackground( this );
#else
        LoadAllGamesForPositionSearch( tiny_db.in_memory_game_cache );
#endif
    }
    
    // If connection failed, handle returns NULL
    dbg_printf( "DATABASE CONSTRUCTOR %s\n", retval ? "FAILED" : "SUCCESSFUL" );
}

Database::~Database()
{
    cprintf( "DATABASE DESTRUCTOR\n" );
    if( positions_stmt )
    {
        sqlite3_finalize(positions_stmt);
        positions_stmt = NULL;
    }
    while( gbl_handle )
    {
        int retval = sqlite3_close(gbl_handle);
        if( retval != SQLITE_BUSY )
        {
            gbl_handle = NULL;
        }
        else
        {
            sqlite3_stmt *p = NULL;
            for(;;)
            {
                sqlite3_stmt *temp = sqlite3_next_stmt(gbl_handle, p );
                if( p )
                    sqlite3_finalize(p);
                p = temp;
                if( p == NULL )
                {
                    break;
                }
                else
                {
                    cprintf( "**** LEAK\n" );
                }
            }
        }
    }
    sqlite3_shutdown();
}

void Database::Reopen( const char *db_file )
{
    if( positions_stmt )
    {
        sqlite3_finalize(positions_stmt);
        positions_stmt = NULL;
    }
    if( gbl_handle )
    {
        sqlite3_close(gbl_handle);
        gbl_handle = NULL;
    }
    gbl_handle = 0;
    background_load_permill = 0;
    kill_background_load = false;
    positions_stmt = 0;
    player_search_stmt = 0;
    gbl_expected = 0;
    gbl_current = 0;
    gbl_count = 0;
    is_tiny_db = false;
    tiny_db.Init();
    
    // Access the database.
    int retval = sqlite3_open(db_file,&gbl_handle);
    is_open = (retval==0);
    std::string nomsg;
    IsOperational( nomsg );
    if( is_open && is_tiny_db  )
    {
#ifdef USE_BACKGROUND_THREAD
        LoadInBackground( this );
#else
        LoadAllGamesForPositionSearch( tiny_db.in_memory_game_cache );
#endif
    }
    
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
            operational = true;
            if( version == DATABASE_VERSION_NUMBER_NORMAL )
                is_tiny_db = false;
            else if( version == DATABASE_VERSION_NUMBER_TINY )
                is_tiny_db = true;
            else
            {
                error_msg = "The database file is not in the correct format for this version of Tarrasch";
                operational = false;
            }
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
    bool db_access_required = true;
    this->db_req = db_req;
    if( !gbl_handle )
        return 0;
    gbl_expected = -1;
    int game_count = 0;
    sqlite3_stmt *stmt;
    this->player_name = player_name;
    
    //cr.Forsyth("r1bqk2r/ppp1bppp/2n1pn2/3p4/Q1PP4/P3PN2/1P1N1PPP/R1B1KB1R b KQkq - 0 7");
    position_hash = cr.Hash64Calculate();
    gbl_position = cr;
    cache.clear();
    stack.clear();
    
    // select matching rows from the table
    char buf[1000];
    int table_nbr;
    int hash;
    uint64_t temp=position_hash;
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
        if( is_tiny_db )
        {
            ProgressBar progress("Database search", "Searching...",false);
            game_count = tiny_db.DoSearch( cr, position_hash, &progress );
            db_access_required = false;
        }
        else
        {
            sprintf( buf, "SELECT COUNT(*) from games, positions_%d WHERE %spositions_%d.position_hash=%d AND games.game_id = positions_%d.game_id",
                    table_nbr, white_and.c_str(), table_nbr, hash, table_nbr );
        }
    }
    //sprintf( buf, "SELECT COUNT(*) from games, positions_%d WHERE games.white = 'Carlsen, Magnus'  AND positions_%d.position_hash=%d AND games.game_id = positions_%d.game_id", table_nbr, table_nbr, hash, table_nbr );
    //    sprintf( buf, "SELECT COUNT(*) from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE games.white = 'Carlsen, Magnus' AND positions_%d.position_hash=%d", table_nbr, table_nbr, table_nbr, hash );
    //    sprintf( buf, "SELECT COUNT(*) from positions_%d WHERE position_hash=%d", table_nbr, hash );
    //    sprintf( buf, "SELECT COUNT(*) from games WHERE games.white = 'Carlsen, Magnus' AND games.game_id = positions_%d.game_id AND positions_%d.position_hash=%d", table_nbr, table_nbr, hash );
    if( db_access_required )
    {
        cprintf("QUERY IN: %s\n",buf);
        int retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
        cprintf("QUERY OUT: %s\n",buf);
        if( retval )
        {
            cprintf("SELECTING DATA FROM DB FAILED 1\n");
            return 0;
        }
    
        // Read the number of rows fetched
        int cols = sqlite3_column_count(stmt);
    
        cprintf( "Get games count begin\n");
        while(1)
        {
            // fetch a row's status
            retval = sqlite3_step(stmt);
            if(retval == SQLITE_ROW)
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
                cprintf("SOME ERROR ENCOUNTERED\n");
            }
        }
    }
    cprintf("Get games count end\n");
    gbl_count = game_count;
    return game_count;
}


int Database::LoadGameWithQuery( CompactGame *pact, int game_id )
{
    pact->game_id = game_id;
    if( is_tiny_db && is_pristine )
    {
        tiny_db.in_memory_game_cache[game_id]->GetCompactGame(*pact);
        return 0;
    }
    
    // Get white player
    char buf[1000];
    sqlite3_stmt *stmt;    // A prepared statement for fetching from games table
    int retval;
    sprintf( buf, "SELECT white,black,event,site,round,result,date,white_elo,black_elo,eco,moves from games WHERE game_id=%d", game_id );
    //cprintf("QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
    //cprintf("QUERY OUT: %s\n",buf);
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
                    pact->r.round = val;
                    break;
                }
                case 5:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.result = val;
                    break;
                }
                case 6:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.date = val;
                    break;
                }
                case 7:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.white_elo = val;
                    break;
                }
                case 8:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.black_elo = val;
                    break;
                }
                case 9:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    pact->r.eco = val;
                    break;
                }
                case 10:
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

int Database::GetGameCount()
{
    if( !gbl_handle )
        return 0;
    int game_count = 0;
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(*) from games";
    cprintf("QUERY IN: %s\n",sql);
    int retval = sqlite3_prepare_v2( gbl_handle, sql, -1, &stmt, 0 );
    cprintf("QUERY OUT: %s\n",sql);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 1\n");
        return 0;
    }
    
    // Read the number of rows fetched
    int cols = sqlite3_column_count(stmt);
    
    cprintf( "Get games count begin\n");
    while(1)
    {
        // fetch a row's status
        retval = sqlite3_step(stmt);
        if(retval == SQLITE_ROW)
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
            cprintf("SOME ERROR ENCOUNTERED\n");
        }
    }
    cprintf("Get game count end\n");
    return game_count;
}

static bool gbl_protect_recursion;   // FIXME


int Database::LoadPlayerGamesWithQuery(  std::string &player_name, bool white, std::vector< smart_ptr<ListableGame> > &games )
{
    int nbr_before = games.size();
    int retval=-1;
    if( !gbl_handle )
        return -1;
    gbl_expected = -1;
    sqlite3_stmt *stmt;    // A prepared statement for fetching from games table
    
    // select matching rows from the table
    char buf[1000];
    sprintf( buf,
            "SELECT games.game_id, games.white, games.black, games.event, games.site, games.round, games.result, games.date, games.white_elo, games.black_elo, games.eco, games.moves from games "
            "WHERE games.%s='%s'",  white?"white":"black",  player_name.c_str() );
    cprintf( "LoadPlayerGamesWithQuery(), player; QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
    cprintf( "LoadPlayerGamesWithQuery(), player; QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 2\n");
        return -1;
    }
    
    // Read the game info
    int cols = sqlite3_column_count(stmt);
    for(;;)
    {
        retval = sqlite3_step(stmt);
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
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    game_id = atoi(val);
                }
                else if( col == 1 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.round = val ? std::string(val) : "Whoops";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.result = val ? std::string(val) : "*";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 10 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.eco = val ? std::string(val) : "Whoops";
                }
                else if( col == 11 )
                {
                    int len = sqlite3_column_bytes(stmt,col);
                    const char *blob = (const char*)sqlite3_column_blob(stmt,col);
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
            sqlite3_finalize(stmt);
            stmt = NULL;
            int nbr_after = games.size();
            cprintf("LoadPlayerGamesWithQuery(): %d games loaded\n", nbr_after-nbr_before );
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


//Legacy only
int Database::LoadGamesWithQuery( const thc::ChessPosition &cp, uint64_t hash, std::vector< smart_ptr<ListableGame> > &games, std::unordered_set<int> &games_set )
{
    int retval=-1;
    int nbr_before = games.size();
    sqlite3_stmt *stmt;    // A prepared statement for fetching from games table
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
            "SELECT games.game_id, games.white, games.black, games.event, games.site, games.round, games.result, games.date, games.white_elo, games.black_elo, games.eco, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d",
            table_nbr, table_nbr, white_and.c_str(), table_nbr, hash32);
    cprintf( "LoadGamesWithQuery(), position; QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
    cprintf( "LoadGamesWithQuery(), position; QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 2\n");
        return retval;
    }
    
    // Read the game info
    int cols = sqlite3_column_count(stmt);
    for(;;)
    {
        retval = sqlite3_step(stmt);
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
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
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
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.round = val ? std::string(val) : "Whoops";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.result = val ? std::string(val) : "*";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 10)
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    r.eco = val ? std::string(val) : "Whoops";
                }
                else if( col == 11 )
                {
                    int len = sqlite3_column_bytes(stmt,col);
                    //fprintf(f,"Move len = %d\n",len);
                    const char *blob = (const char*)sqlite3_column_blob(stmt,col);
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
            sqlite3_finalize(stmt);
            stmt = NULL;
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
    if( is_tiny_db && db_req != REQ_PLAYERS )
    {
        return 0;
    }
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
                retval = sqlite3_step(positions_stmt);
                if( retval == SQLITE_ROW )
                {
                    // SQLITE_ROW means fetched a row
                    
                    // sqlite3_column_text returns a const void* , typecast it to const char*
                    for( int col=0; col<cols; col++ )
                    {
                        const char *val = (const char*)sqlite3_column_text(positions_stmt,col);
                        //cprintf("%s:%s\t",sqlite3_column_name(positions_stmt,col),val);
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
                    sqlite3_finalize(positions_stmt);
                    positions_stmt = NULL;
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
            uint64_t temp = position_hash;
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
            if( positions_stmt )
                sqlite3_finalize(positions_stmt);
            positions_stmt = NULL;
            retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &positions_stmt, 0 );
            cprintf( "Database::GetRow() START query: %s\n",buf);
            if( retval )
            {
                cprintf("SELECTING DATA FROM DB FAILED 2\n");
                return retval;
            }

            // Read the number of rows fetched
            cols = sqlite3_column_count(positions_stmt);
        }
    }
}


bool Database::LoadAllGames( std::vector< smart_ptr<ListableGame> > &cache, int nbr_games )
{
    if( is_tiny_db && db_req==REQ_POSITION )
    {
        // StatsCalculate() will load games
        cache.clear();
        return true;
    }

    gbl_protect_recursion = true;
    bool ok=true;
    sqlite3_stmt *stmt;
    int retval=-1;
    cache.clear();
    
    // select matching rows from the table
    char buf[1000];
    gbl_expected = -1;
    uint64_t temp = position_hash;
    int hash = (int)(temp);
    int table_nbr = (int)((temp>>32)&(NBR_BUCKETS-1));
    if( is_tiny_db && db_req==REQ_SHOW_ALL )
    {
        ProgressBar progress( "Loading games into memory", "Loading games", false );
        cache.clear();
        int nbr = tiny_db.in_memory_game_cache.size();
        for( int i=0; i<nbr; i++ )
        {
            cache.push_back( tiny_db.in_memory_game_cache[i] );
            progress.Permill( i*1000 / nbr );
        }
        gbl_protect_recursion = false;
        return true;
    }


    ProgressBar progress( "Loading games into memory", "Loading games" );
    if( db_req == REQ_PLAYERS )
    {
        sprintf( buf,
                "SELECT games.game_id from games%s ORDER BY games.white ASC", where_white.c_str() );
    }
    else if( db_req == REQ_SHOW_ALL )
    {
        sprintf( buf,
#if 1 //def NO_REVERSE
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.round, games.result, games.date, games.white_elo, games.black_elo, games.eco, games.moves from games" );
#else
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.round, games.result, games.date, games.white_elo, games.black_elo, games.eco, games.moves from games ORDER BY games.rowid DESC" );
#endif
    }
    else if( db_req == REQ_POSITION )
    {
        sprintf( buf,
#if 1 //def NO_REVERSE
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.round, games.result, games.date, games.white_elo, games.black_elo, games.eco, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d",
#else
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.round, games.result, games.date, games.white_elo, games.black_elo, games.eco, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d ORDER BY games.rowid DESC",
#endif
                table_nbr, table_nbr, white_and.c_str(), table_nbr, hash);
    }
    cprintf( "LoadAllGames() QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
    cprintf( "LoadAllGames() QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 2\n");
        ok = false;
    }
    
    // Read the game info
    if( ok )
    {
        int cols = sqlite3_column_count(stmt);
        int previous_game_id = -1;
        //int dbg_idx=0;
        while(ok)
        {
            retval = sqlite3_step(stmt);
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
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        game_id = atoi(val);
                    }
                    else if( col == 1 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.white = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 2 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.black = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 3 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.event = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 4 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.site = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 5 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.round = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 6 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.result = val ? std::string(val) : "*";
                    }
                    else if( col == 7 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.date = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 8 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.white_elo = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 9 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.black_elo = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 10)
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        r.eco = val ? std::string(val) : "Whoops";
                    }
                    else if( col == 11 )
                    {
                        int len = sqlite3_column_bytes(stmt,col);
                        //fprintf(f,"Move len = %d\n",len);
                        const char *blob = (const char*)sqlite3_column_blob(stmt,col);
                        if( len && blob )
                        {
                            std::string temp(blob,len);
                            str_blob = temp;
                        }
                    }
                }
                //if( game_id == 3619485 )
                //    cprintf( "game_id=%d, dbg_idx=%d\n", game_id, dbg_idx );
                //dbg_idx++;
                if( game_id != previous_game_id )   // TEMP bug workaround. Unfortunately if a position
                                                    // occurs multiple times in a game (through repitition)
                                                    // the database query returns multiple instances of the
                                                    // game
                {
                    previous_game_id = game_id;
                    ListableGameDb info( game_id, r, str_blob );
                    make_smart_ptr( ListableGameDb, new_info, info );
                    cache.push_back( std::move(new_info) );
                }
                int permill = (cache.size()*1000) / (nbr_games?nbr_games:1);
                if( permill < 1 )
                    permill = 1;
                ok = !progress.Permill( permill>1000 ? 1000 : permill );
            }
            else if( retval == SQLITE_DONE )
            {
                // All rows finished
                sqlite3_finalize(stmt);
                stmt = NULL;
                cprintf( "Reversing order to get newest games first\n" );
                std::reverse( cache.begin(), cache.end() );
                cprintf("LoadAllGames(): %u game_ids loaded\n", cache.size() );
                break;
            }
            else
            {
                // Some error encountered
                cprintf("SOME ERROR ENCOUNTERED\n");
                ok = false;
            }
        }
    }
    gbl_protect_recursion = false;
    if( !ok )
        cache.clear();
    return ok;
}


/* static void Delay( int count )
{
    for( int i=0; i<count; i++ )
    {
        char buf[100];
        sprintf( buf, "%d", i );
    }
}  */


//std::vector< smart_ptr<ListableGame> > mega_cache;

bool Database::LoadAllGamesForPositionSearch( std::vector< smart_ptr<ListableGame> > &mega_cache )
{
    AutoTimer at("Load all games");
    is_pristine=true;     // true if game_id = idx for every idx in mega_cache[idx]
    background_load_permill = 0;
    kill_background_load = false;
    int idx=0;
    int nbr_games=0;
    int nbr_promotion_games=0;
    gbl_protect_recursion = true;
    bool ok=true;
    sqlite3_stmt *stmt;
    int retval=-1;
    mega_cache.clear();
    cache.clear();
    int game_count = GetGameCount();
    #if 1
/*    wxProgressDialog progress( "Loading games into memory", "Loading games", 100, NULL,       
                              wxPD_APP_MODAL+
                              wxPD_AUTO_HIDE+
                              wxPD_ELAPSED_TIME+
                              wxPD_CAN_ABORT+
                              wxPD_ESTIMATED_TIME ); */
    
    // select matching rows from the table
    char buf[1000];
    gbl_expected = -1;
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
    
    // Read the game info
    if( ok )
    {
        //int cols = sqlite3_column_count(stmt);
        while(ok && !kill_background_load )
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
                for( int i=0; i<len12; i++ )
                {
                    if( (moves_blob[i]&0x8c) > 0x80 )
                    {
                        promotion = true;
                        break;
                    }
                }
                info.game_attributes = static_cast<unsigned char>(promotion);
                make_smart_ptr( ListableGameDb, new_info, info );
                mega_cache.push_back( std::move(new_info) );
                if( game_id != idx++ )
                    is_pristine = false;
                background_load_permill = (idx*1000) / (game_count?game_count:1);

            /*  ListableGameDb info( game_id, r, str_blob );
                make_smart_ptr( ListableGameDb, new_info, info );
                cache.push_back( std::move(new_info) );
                int percent = (cache.size()*100) / (nbr_games?nbr_games:1);
                if( percent < 1 )
                    percent = 1;
                if( !progress.Update( percent>100 ? 100 : percent ) )
                    ok = false;  */
                if( promotion )
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
    }
    int cache_nbr = mega_cache.size();
    cprintf( "Number of games = %d, is_pristine=%s\n", cache_nbr, is_pristine?"true":"false" );
    if( 2 < cache_nbr )
    {
        smart_ptr<ListableGame> p = mega_cache[2];
        cprintf( "White=%s Black=%s Result=%s Round=%s\n", p->White(), p->Black(), p->Result(), p->Round() );
    }
    #else
    
    // select matching rows from the table
    char buf[1000];
    sprintf( buf,
            "SELECT games.game_id, games.moves from games" );


    cprintf( "LoadAllGames() QUERY IN: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
    cprintf( "LoadAllGames() QUERY OUT: %s\n",buf);
    if( retval )
    {
        cprintf("SELECTING DATA FROM DB FAILED 2\n");
        ok = false;
    }
    
    // Read the game info
    if( ok )
    {
        int cols = sqlite3_column_count(stmt);
        while(ok)
        {
            retval = sqlite3_step(stmt);
            if( retval == SQLITE_ROW )
            {
                int game_id=0;
                std::string str_blob;

                // SQLITE_ROW means fetched a row
            
                // sqlite3_column_text returns a const void* , typecast it to const char*
                bool promotion=false;
                for( int col=0; col<cols; col++ )
                {
                    if( col == 0 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        game_id = atoi(val);
                    }
                    else if( col == 1 )
                    {
                        int len = sqlite3_column_bytes(stmt,col);
                        //fprintf(f,"Move len = %d\n",len);
                        const char *blob = (const char*)sqlite3_column_blob(stmt,col);
                        for( int i=0; i<len; i++ )
                        {
                            if( (blob[i]&0x8c) > 0x80 )
                            {
                                promotion = true;
                                break;
                            }
                        }
                        if( len && blob )
                        {
                            std::string temp(blob,len);
                            str_blob = temp;
                        }
                    }
                }
                std::pair<int, std::string> game(promotion?0-game_id:game_id,str_blob);
                cache.push_back( game );
                //Delay(1000);
                if( promotion )
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
                break;
            }
        }
    }
    #endif
    cprintf( "%d games (%d include promotion)\n", nbr_games, nbr_promotion_games );
    gbl_protect_recursion = false;

    //#define TEST_SEARCH
    #ifdef TEST_SEARCH
    int max_so_far=0;
    for( int i=0; i<mega_cache.size(); i++ )
    {
        smart_ptr<ListableGame> p = mega_cache[i];
        if( (i+1)==10 || ((i+1)%1000==0) )
            cprintf( "Testing search, %d games\n", i+1 );
        std::vector<thc::Move> moves = p->RefMoves();
        thc::ChessRules cr;
        for( int j=0; j<moves.size(); j++ )
        {
            thc::Move mv = moves[j];
            cr.PlayMove(mv);
        }
        bool multiple=false;
        bool found =  tiny_db.DoSearch( cr, 0, 0 );
        if( found )
        {
            found = false;
            std::vector<DoSearchFoundGame> vec = tiny_db.GetVectorGamesFound();
            int nbr = vec.size();
            if( nbr>1 && moves.size()>max_so_far )
            {
                max_so_far = moves.size();
                multiple = true;
            }
            for( int j=0; j<nbr; j++ )
            {
                DoSearchFoundGame& dsfg = vec[j];
                if( dsfg.idx == i )
                    found = true;
            }
        }
        if( !found || multiple )
        {
            std::string txt;
            thc::ChessRules cr2;
            for( int j=0; j<moves.size(); j++ )
            {
                thc::Move mv2 = moves[j];
                std::string s = mv2.NaturalOut(&cr2);
                char buf[20];
                sprintf( buf, "%d.", cr2.full_move_count );
                const char *before = buf;
                if( !cr2.white )
                    before = " ";
                const char *after = "";
                if( j+1 < moves.size() )
                {
                    if( ((j+1)&3) == 0 )
                        after = "\n";
                    else
                        after = " ";
                }
                char buf2[100];
                sprintf( buf2, "%s%s%s", before, s.c_str(), after );
                txt += std::string(buf2);
                cr2.PlayMove(mv2);
            }
            Roster r = p->RefRoster();
            cprintf( "%s %s-%s> %s\n",
                !found ? "Not found" : "Longest possible duplicate game to date",
                r.white.c_str(),
                r.black.c_str(),
                txt.c_str()
            );
            FILE *out = fopen("testing-out.txt","at");
            if( !out )
                cprintf( "whoops?");
            else
            {
                fprintf( out, "%s %s-%s> %s\n",
                    !found ? "Not found" : "Longest possible duplicate game to date",
                    r.white.c_str(),
                    r.black.c_str(),
                    txt.c_str()
                );
                fclose(out);
            }
        }
    }
    #endif
    return ok;
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

            // If white we want to skip further games played by the same player, (since the list is sorted by white
            //  player the user can already see those games - so go look for another player of the same name)
            bool criteria_if_white = (player!=current && input_len<=player_len && player.substr(0,input_len)==input);

            // If black we want to find more black games played by the same player, (since the list is sorted
            //  by white player, black games by a single player are sparsely distributed)
            bool criteria_if_black = (input_len<=player_len && player.substr(0,input_len)==input);
            if( (criteria_if_white&&white) || (criteria_if_black&&!white) )
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

