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
#include "BinDb.h"
#include "LegacyDb.h"
#include "Database.h"
#include "Repository.h"
#if !wxUSE_THREADS
    #error "Requires thread support!"
#endif // wxUSE_THREADS
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/thread.h"
#include "wx/msgout.h"
#include "wx/progdlg.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"

//
//  This class has evolved into a DatabaseRead facility and should be
//  renamed as such
//

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
        int base = the_database->background_load_permill;
        ProgressBar progress("Completing Initial Database Load","Loading database into memory",false);
        while( wxMUTEX_BUSY == s_mutex_tiny_database.TryLock() )
        {
            //static int now_before=-1;
            int now = the_database->background_load_permill;
            //if( now != now_before )
            //    cprintf( "base=%d, now=%d, now-base=%d, 1000-base=%d\n", base, now, now-base, 1000-base );
            //now_before = now;
            progress.Perfraction( now-base, 1000-base );
            wxSafeYield();
        }
    }
    wxSafeYield();
    s_mutex_tiny_database.Unlock();
    return &s_mutex_tiny_database;
}

Database::Database( const char *db_file )
{
    Reopen( db_file );
}

void Database::Reopen( const char *db_file )
{
    background_load_permill = 0;
    kill_background_load = false;
    player_search_in_progress = false;
    tiny_db.Init();

    // Access the database.
    cprintf( "Database startup %s\n", db_file );
    db_filename = std::string(db_file);
    is_open = BinDbOpen( db_file, database_version );
    std::string nomsg;
    is_open = IsOperational( nomsg );
    if( is_open )
        LoadInBackground( this );
    
    // If connection failed, handle returns NULL
    cprintf( "Startup %s\n", is_open ? "SUCCESSFUL" : "FAILED" );
}


// Return bool operational
bool Database::IsOperational( std::string &error_msg )
{
    bool operational = false;
    error_msg = "";
    if( database_version==0 || database_version>DATABASE_VERSION_NUMBER_BIN_DB )
    {
        char buf[200];
        sprintf( buf, "The database file is not in the correct format for this version of Tarrasch (db version=%d)", database_version );                
        error_msg = std::string(buf);
    }
    else if( !is_open )
        error_msg = "Could not open database";
    else
        operational = true;
    if( !operational )
    {
        if( is_open )
            BinDbClose();
        is_open = false;
    }
    return operational;
}

int Database::SetDbPosition( DB_REQ db_req, thc::ChessRules &cr )
{
    std::string empty;
    return SetDbPosition( db_req, cr, empty );
}

int Database::SetDbPosition( DB_REQ db_req, thc::ChessRules &cr, std::string &player_name )
{
    this->db_req = db_req;
    extern void BinDbDatabaseInitialSort( std::vector< smart_ptr<ListableGame> > &games, bool sort_by_player_name );
    BinDbDatabaseInitialSort( objs.db->tiny_db.in_memory_game_cache, db_req == REQ_PLAYERS );
	int nbr = tiny_db.in_memory_game_cache.size();
    if( nbr )
    {
	    smart_ptr<ListableGame> p = objs.db->tiny_db.in_memory_game_cache[0];
	    cprintf( "index 0: %s\n", p->White() );
    }
	return nbr;
}

int Database::LoadPlayerGamesWithQuery(  std::string &player_name, bool white, std::vector< smart_ptr<ListableGame> > &games )
{
	int nbr_before = games.size();
    int nbr= tiny_db.in_memory_game_cache.size();
	const char *player = player_name.c_str();
    for( int i=0; i<nbr; i++ )
	{
		smart_ptr<ListableGame> p = tiny_db.in_memory_game_cache[i];
		const char *rover = (white ? p->White() : p->Black()); 
		if( 0 == strcmp(rover,player) )
			games.push_back(p);
	}
	int nbr_after = games.size();
	return nbr_after-nbr_before;
}

int Database::GetRow( int row, CompactGame *pact )
{
	smart_ptr<ListableGame> p = tiny_db.in_memory_game_cache[row];
	p->GetCompactGame(*pact);
    return 0;
}

bool Database::LoadAllGamesForPositionSearch( std::vector< smart_ptr<ListableGame> > &mega_cache )
{
    AutoTimer at("Load all games");
    background_load_permill = 0;
    kill_background_load = false;
    mega_cache.clear();
    if( database_version < DATABASE_VERSION_NUMBER_BIN_DB )
    {
        BinDbClose();
        LegacyDbLoadAllGames( db_filename.c_str(), false, mega_cache, background_load_permill, kill_background_load );
    }
    else
    {
        BinDbLoadAllGames( false, mega_cache, background_load_permill, kill_background_load );
        std::reverse( mega_cache.begin(), mega_cache.end() );
        BinDbClose();
    }
    //cprintf( "Reversing BinDb order begin\n" );
    //std::reverse( mega_cache.begin(), mega_cache.end() ); 
    //cprintf( "Reversing BinDb order end\n" );
    int cache_nbr = mega_cache.size();
    cprintf( "Number of games = %d\n", cache_nbr );
    if( 2 < cache_nbr )
    {
        smart_ptr<ListableGame> p = mega_cache[2];
        cprintf( "White=%s Black=%s Result=%s Round=%s\n", p->White(), p->Black(), p->Result(), p->Round() );
    }
    return cache_nbr>0;
}

// Returns row
int Database::FindPlayer( std::string &name, std::string &current, int start_row, bool white )
{
    bool okay = true;
    int retval = -1;                               
    int row = 0;
    std::string input(name);
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    std::transform(current.begin(), current.end(), current.begin(), ::tolower);
    int input_len = input.length();

    // If continue search step
    if( player_search_in_progress &&
        prev_name == name &&
        prev_current == current &&
        prev_white == white &&
        prev_row == start_row
      )
    {
        row = start_row+1;
    }

    // Else start search
    else
    {
        player_search_in_progress = true;
    }

    // Loop looking for matches
    while( row < tiny_db.in_memory_game_cache.size() ) 
    {
        const char *val;
        smart_ptr<ListableGame> p = tiny_db.in_memory_game_cache[row];
        val = white ? p->White() : p->Black();
        std::string player(val);
        std::transform(player.begin(), player.end(), player.begin(), ::tolower);
        int player_len = player.length();

        // If white we want to skip further games played by the same player, (since the list is sorted by white
        //  player the user can already see those games - so go look for another player of the same name)
        bool match;
        if( white )
        {
            match = (player!=current && input_len<=player_len && player.substr(0,input_len)==input);
        }

        // If black we want to find more black games played by the same player, (since the list is sorted
        //  by white player, black games by a single player are sparsely distributed)
        else
        {
            match = (input_len<=player_len && player.substr(0,input_len)==input);
        }
        if( match )
        {
            prev_name = name;
            prev_current = player;
            prev_white = white;
            prev_row = row;
            return row;
        }
        row++;
    }
    player_search_in_progress = false;
    return start_row;
}


