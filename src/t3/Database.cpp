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

Database::~Database()
{
}

void Database::Reopen( const char *db_file )
{
    background_load_permill = 0;
    kill_background_load = false;
    player_search_stmt_bin_db = false;
    gbl_expected = 0;
    gbl_current = 0;
    gbl_count = 0;
    is_pristine = false;
    tiny_db.Init();

    // Access the database.
    is_open = BinDbOpen( db_file );
    is_bin_db = is_open;
    std::string nomsg;
    IsOperational( nomsg );
    if( is_open )
    {
#define USE_BACKGROUND_THREAD
#ifdef USE_BACKGROUND_THREAD
        LoadInBackground( this );
#else
        LoadAllGamesForPositionSearch( tiny_db.in_memory_game_cache );
#endif
    }
    
    // If connection failed, handle returns NULL
    dbg_printf( "DATABASE STARTUP %s\n", is_open ? "SUCCESSFUL" : "FAILED" );
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
            if( version != DATABASE_VERSION_NUMBER_BIN_DB )
            {
                char buf[200];
                sprintf( buf, "The database file is not in the correct format for this version of Tarrasch (db version=%d)", version );                
                error_msg = std::string(buf);
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
    if( is_bin_db )
    {
        BinDbGetDatabaseVersion( version );
        ok = true;
    }
    return ok;
}

bool predicate_sorts_by_player( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 )
{
	return e1->WhiteBin() < e2->WhiteBin();
}

bool predicate_sorts_by_game_id( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 )
{
	return e1->GetGameId() < e2->GetGameId();
}


int Database::SetDbPosition( DB_REQ db_req, thc::ChessRules &cr )
{
    std::string empty;
    return SetDbPosition( db_req, cr, empty );
}


int Database::SetDbPosition( DB_REQ db_req, thc::ChessRules &cr, std::string &player_name )
{
    extern void db_make_small_db_file();
    bool db_access_required = true;
    this->db_req = db_req;
    if( !is_bin_db )
        return 0;
	std::sort( objs.db->tiny_db.in_memory_game_cache.begin(), objs.db->tiny_db.in_memory_game_cache.end(), db_req==REQ_PLAYERS ? predicate_sorts_by_player : predicate_sorts_by_game_id );
	smart_ptr<ListableGame> p = objs.db->tiny_db.in_memory_game_cache[0];
	cprintf( "index 0: %s\n", p->White() );
	return tiny_db.in_memory_game_cache.size();
}


int Database::LoadGameWithQuery( CompactGame *pact, int game_id )
{
    pact->game_id = game_id;
    if( !is_bin_db )
        return 0;
    tiny_db.in_memory_game_cache[game_id]->GetCompactGame(*pact);
    return 0;
}

int Database::GetGameCount()
{
    if( !is_bin_db )
        return 0;
	return tiny_db.in_memory_game_cache.size();
}

static bool gbl_protect_recursion;   // FIXME

int Database::LoadPlayerGamesWithQuery(  std::string &player_name, bool white, std::vector< smart_ptr<ListableGame> > &games )
{
	int nbr_before = games.size();
    if( !is_bin_db )
    	return -1;
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
    if( is_bin_db && db_req == REQ_PLAYERS )
    {
		smart_ptr<ListableGame> p = tiny_db.in_memory_game_cache[row];
		p->GetCompactGame(*pact);
        return 0;
    }
}

bool Database::LoadAllGamesForPositionSearch( std::vector< smart_ptr<ListableGame> > &mega_cache )
{
    AutoTimer at("Load all games");
    bool ok=false;
    if( is_bin_db )
        ok = LoadAllGamesForPositionSearchBinDb( mega_cache );
    return ok;
}



bool Database::LoadAllGamesForPositionSearchBinDb( std::vector< smart_ptr<ListableGame> > &mega_cache )
{
    is_pristine = true;
    background_load_permill = 0;
    kill_background_load = false;
    mega_cache.clear();
    cache.clear();
    BinDbLoadAllGames( false, mega_cache, background_load_permill, kill_background_load );
    cprintf( "Reversing BinDb order begin\n" );
    //std::reverse( mega_cache.begin(), mega_cache.end() ); 
    cprintf( "Reversing BinDb order end\n" );
    BinDbClose();
    int cache_nbr = mega_cache.size();
    cprintf( "Number of games = %d, is_pristine=%s\n", cache_nbr, is_pristine?"true":"false" );
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
    gbl_expected = -1;
    std::string input(name);
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    std::transform(current.begin(), current.end(), current.begin(), ::tolower);
    int input_len = input.length();
    bool search_in_progress = player_search_stmt_bin_db;

    // If continue search step
    if( search_in_progress &&
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
        if( is_bin_db )
        {
            player_search_stmt_bin_db = true;
            okay = true;
        }
        else
        {
            okay = false;
        }
    }

    // Loop looking for matches
    while( okay )
    {
        if( is_bin_db )
            okay = (row < tiny_db.in_memory_game_cache.size());
        else
            okay = false;
        if( okay )
        {
            const char *val;
            if( is_bin_db )
            {
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
        }
    }
    if( is_bin_db )
        player_search_stmt_bin_db = false;
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

