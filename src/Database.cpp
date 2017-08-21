/****************************************************************************
 *  Database subsystem for looking up positions from a database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
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

// pieces 'B','K','N','P','Q','R','b','k','n','p','q','r'
static int piece_to_idx[128] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // 0x00
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // 0x10
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // 0x20
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // 0x30
    0,0,1,0,0,0,0,0,0,0,0,2,0,0,3,0,    // 0x40, 0x41='A'
    4,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,    // 0x50='P'
    0,0,7,0,0,0,0,0,0,0,0,8,0,0,9,0,    // 0x60, 0x61='a'
    10,11,12,0,0,0,0,0,0,0,0,0,0,0,0,0  // 0x70='p'
};
static int idx_to_piece[16] =
{
    0,'B','K','N','P','Q','R','b','k','n','p','q','r'
};

struct data_point
{
    bool hit;
    unsigned long game_count;
    unsigned long ply_array[100];
    float avg_search_proportion;
    float effectiveness;
};
static data_point stats[16][64];
static int nbr_games_this_long[100];
wxMutex s_mutex_tiny_database;

static const int PLY_MIN = 4;
static const int PLY_MAX = 4;

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

            // put investigation stuff here
#if 1
            int nbr_games = the_database->tiny_db.in_memory_game_cache.size();
            for( int i=0; i<nbr_games; i++ )
	        {
                int modulo = i<10000 ? 1000 : (i<10000?10000:100000);
                if( i%modulo == 0 )
                    cprintf( "Investigation %d games\n", i );
                for( int j=0; j<16; j++ )
                {
                    for( int k=0; k<64; k++ )
                    {
                        data_point *p = &stats[j][k];
                        p->hit = false;
                    }
                }
		        smart_ptr<ListableGame> ptr = the_database->tiny_db.in_memory_game_cache[i];
                CompactGame pact;
		        ptr->GetCompactGame( pact );
                int nbr_moves = pact.moves.size();
                thc::ChessRules cr;
                for( int ply=0; ply<nbr_moves; ply++ )
                {
                    thc::Move mv = pact.moves[ply];
                    unsigned int src = static_cast<unsigned int>(mv.src);
                    char piece = cr.squares[src];
                    switch( mv.special )
                    {
                        default: break;
                        case thc::SPECIAL_PROMOTION_BISHOP: piece = (piece=='p'?'b':'B'); break;
                        case thc::SPECIAL_PROMOTION_KNIGHT: piece = (piece=='p'?'n':'N'); break;
                        case thc::SPECIAL_PROMOTION_ROOK:   piece = (piece=='p'?'r':'R'); break;
                        case thc::SPECIAL_PROMOTION_QUEEN:  piece = (piece=='p'?'q':'Q'); break;
                    }
                    int piece_idx = piece_to_idx[piece];

                    // A piece lands on a square - indicate that for this is a game where piece hit that square
                    unsigned int sq = static_cast<unsigned int>(mv.dst);
                    data_point *p = &stats[piece_idx][sq];
                    if( !p->hit )
                    {
                        p->hit = true;
                        p->game_count++;
                    }
                    cr.PlayMove(mv);

                    // After every move, update a ply index count for every piece/square combination
                    if( ply < 100 )
                    {
                        nbr_games_this_long[ply]++;
                        for( int jj=1; jj<=12; jj++ )
                        {
                            char rover_piece = idx_to_piece[jj];
                            for( int kk=0; kk<64; kk++ )
                            {
                                if( cr.squares[kk] == rover_piece )
                                {
                                    data_point *pp = &stats[jj][kk];
                                    pp->ply_array[ply]++;
                                }
                            }
                        }
                    }
                }
            }

            // Display game hits
            cprintf( "Investigation complete %d games\n", nbr_games );
            cprintf( "Game hit rates\n", nbr_games );
            for( int i=1; i<=12; i++ )
            {
                char piece = idx_to_piece[i];
                for( int sq=0; sq<64; sq++ )
                {
                    if( piece=='p' && sq<16 )
                        continue;
                    if( piece=='p' && sq>=56 )
                        continue;
                    if( piece=='P' && sq<8 )
                        continue;
                    if( piece=='P' && sq>=48 )
                        continue;
                    char file = 'a' + (sq%8);
                    char rank = '8' - ((sq>>3)&7);
                    if( file == 'a' )
                        cprintf( "\n" );
                    if( piece=='r' && (sq==0||sq==7) )
                        continue;
                    if( piece=='n' && (sq==1||sq==6) )
                        continue;
                    if( piece=='b' && (sq==2||sq==5) )
                        continue;
                    if( piece=='q' && sq==3 )
                        continue;
                    if( piece=='k' && sq==4 )
                        continue;
                    if( piece=='R' && (sq==56||sq==63) )
                        continue;
                    if( piece=='N' && (sq==57||sq==62) )
                        continue;
                    if( piece=='B' && (sq==58||sq==61) )
                        continue;
                    if( piece=='Q' && sq==59 )
                        continue;
                    if( piece=='K' && sq==60 )
                        continue;
                    data_point *p = &stats[i][sq];
                    cprintf( "%c%c%c %2.1f%%", piece, file, rank, (float)(p->game_count) * 100.0 / (float)(nbr_games) );
                    if( file !='h' )
                        cprintf( " " );
                }
            }

            // Display ply stats for Nf3
            cprintf( "\n\n" );
            cprintf( "Stats for Nf3\n");
            data_point *p1 = &stats[piece_to_idx['N']][45];
            for( int ply=0; ply<100; ply++ )
            {
                if( nbr_games_this_long[ply] )
                    cprintf( "ply %d: %2.1f%%\n", ply, (float)(p1->ply_array[ply]) * 100.0 / (float)(nbr_games_this_long[ply]) );
            }

            // Display ply stats
            cprintf( "\n\n" );
            cprintf( "Ply average hit rates\n" );
            for( int i=1; i<=12; i++ )
            {
                char piece = idx_to_piece[i];
                for( int sq=0; sq<64; sq++ )
                {
                    if( piece=='p' && sq<16 )
                        continue;
                    if( piece=='p' && sq>=56 )
                        continue;
                    if( piece=='P' && sq<8 )
                        continue;
                    if( piece=='P' && sq>=48 )
                        continue;
                    char file = 'a' + (sq%8);
                    char rank = '8' - ((sq>>3)&7);
                    if( file == 'a' )
                        cprintf( "\n" );
                    if( piece=='r' && (sq==0||sq==7) )
                        continue;
                    if( piece=='n' && (sq==1||sq==6) )
                        continue;
                    if( piece=='b' && (sq==2||sq==5) )
                        continue;
                    if( piece=='q' && sq==3 )
                        continue;
                    if( piece=='k' && sq==4 )
                        continue;
                    if( piece=='R' && (sq==56||sq==63) )
                        continue;
                    if( piece=='N' && (sq==57||sq==62) )
                        continue;
                    if( piece=='B' && (sq==58||sq==61) )
                        continue;
                    if( piece=='Q' && sq==59 )
                        continue;
                    if( piece=='K' && sq==60 )
                        continue;
                    data_point *p2 = &stats[i][sq];

                    float sum = 0.0;
                    for( int ply=PLY_MIN; ply<=PLY_MAX; ply++ )
                    {
                        float ply_hit_rate = 0.0;
                        if( nbr_games_this_long[ply] > 0 )
                            ply_hit_rate =  (float)(p2->ply_array[ply]) / (float)(nbr_games_this_long[ply]);
                        sum += ply_hit_rate;
                    }

                    // AVG = Average rate at which piece/square combo appears in position between PLY_MIN and PLY_MAX
                    float avg = sum / (PLY_MAX-PLY_MIN+1);

                    // If search position contains the combo, then no need to search games where combo not hit
                    // So EFFECTIVENESS = AVG * proportion of games where combo not hit
                    //  eg if Nf3 appears in 40% of positions, and is hit somewhere in 90% of games
                    //  effectiveness = 40% * 10% = 4% speedup if we know which games Nf3 hits somewhere
                    float effectiveness = avg * (float)(nbr_games - p2->game_count)  / (float)(nbr_games);
                    p2->avg_search_proportion = avg;
                    p2->effectiveness = effectiveness;
                    cprintf( "%c%c%c %2.1f%%", piece, file, rank, (float)(avg) * 100.0  );
                    if( file !='h' )
                        cprintf( " " );
                }
            }

            // Display effectiveness
            cprintf( "\n\n" );
            cprintf( "Estimate of improved search time as a percentage of unimproved search time = %02.1f%%\n",
                ((100.00 - 05.01) / 100.0) *
                ((100.00 - 05.06) / 100.0) *
                ((100.00 - 05.20) / 100.0) *
                ((100.00 - 05.38) / 100.0) *
                ((100.00 - 06.08) / 100.0) *
                ((100.00 - 06.08) / 100.0) *
                ((100.00 - 06.32) / 100.0) *
                ((100.00 - 06.44) / 100.0) *
                ((100.00 - 06.56) / 100.0) *
                ((100.00 - 07.41) / 100.0) *
                ((100.00 - 07.46) / 100.0) *
                ((100.00 - 08.55) / 100.0) *
                ((100.00 - 09.84) / 100.0) *
                ((100.00 - 11.76) / 100.0) *
                ((100.00 - 13.79) / 100.0) * 100.0 );
            cprintf( "Effectiveness...\n" );
            return 0;
            for( int i=1; i<=12; i++ )
            {
                char piece = idx_to_piece[i];
                for( int sq=0; sq<64; sq++ )
                {
                    if( piece=='p' && sq<16 )
                        continue;
                    if( piece=='p' && sq>=56 )
                        continue;
                    if( piece=='P' && sq<8 )
                        continue;
                    if( piece=='P' && sq>=48 )
                        continue;
                    char file = 'a' + (sq%8);
                    char rank = '8' - ((sq>>3)&7);
                    if( piece=='r' && (sq==0||sq==7) )
                        continue;
                    if( piece=='n' && (sq==1||sq==6) )
                        continue;
                    if( piece=='b' && (sq==2||sq==5) )
                        continue;
                    if( piece=='q' && sq==3 )
                        continue;
                    if( piece=='k' && sq==4 )
                        continue;
                    if( piece=='R' && (sq==56||sq==63) )
                        continue;
                    if( piece=='N' && (sq==57||sq==62) )
                        continue;
                    if( piece=='B' && (sq==58||sq==61) )
                        continue;
                    if( piece=='Q' && sq==59 )
                        continue;
                    if( piece=='K' && sq==60 )
                        continue;
                    data_point *p3 = &stats[i][sq];
                    cprintf( "%02.2f%% %c%c%c\n", (float)(p3->effectiveness) * 100.0, piece, file, rank );
                    if( piece=='P' && file=='c' && rank=='4' )
                        return 0;
                }
            }

#endif
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
    if( !is_open )
        error_msg = database_version==-1 ? "Could not open database, file is not in Tarrasch database format" : "Could not open database";
    else if( database_version==0 || database_version>DATABASE_VERSION_NUMBER_BIN_DB )
    {
        char buf[200];
        sprintf( buf, "The database file is not in the correct format for this version of Tarrasch (db version=%d)", database_version );                
        error_msg = std::string(buf);
    }
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

// SetDbPosition() no longer sets the position! - requires a little bit of refactoring
int Database::SetDbPosition( DB_REQ db_req_ )
{
    this->db_req = db_req_;
    extern void BinDbDatabaseInitialSort( std::vector< smart_ptr<ListableGame> > &games, bool sort_by_player_name );
    BinDbDatabaseInitialSort( objs.db->tiny_db.in_memory_game_cache, db_req_==REQ_PLAYERS );
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
        BinDbClose();
    }
    int cache_nbr = mega_cache.size();
    cprintf( "Number of games = %d\n", cache_nbr );
    if( cache_nbr > 0 )
    {
        smart_ptr<ListableGame> p = mega_cache[0];
        cprintf( "Most recent game first: White=%s Black=%s Result=%s Round=%s\n", p->White(), p->Black(), p->Result(), p->Round() );
    }
    return cache_nbr>0;
}

// Transform to lower case, collapse multiple spaces to 1, remove spaces after comma
void Normalise( std::string &in, std::string &out ) 
{
	bool nuke_spaces=false;
	size_t len=in.length();
	out.clear();
	for( size_t i=0; i<len; i++ )
	{
		int ch = in[i];

		// To lower case
		if( isascii(ch) && isupper(ch) )
			ch = tolower(ch);

		// After ',' or space, remove immediately following spaces
		if( nuke_spaces )
		{
			if( ch != ' ' )
			{
				out.push_back(ch);
				nuke_spaces=false;
			}
		}
		else
		{
			out.push_back(ch);
			if( ch==' ' || ch==',' )
				nuke_spaces = true;
		}
	}
}


// Returns row
int Database::FindPlayer( std::string &name, std::string &current, int start_row, bool white )
{
    unsigned int row = 0;
    std::string input;
	Normalise(name,input);
	std::string normalised_current;
	Normalise(current, normalised_current );
    int input_len = input.length();

    // If continue search, step
    if( player_search_in_progress &&
        prev_name == name &&
        prev_current == normalised_current &&
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
        std::string raw_player(val);
        std::string player;
		Normalise(raw_player,player);
        int player_len = player.length();

        // If white we want to skip further games played by the same player, (since the list is sorted by white
        //  player the user can already see those games - so go look for another player of the same name)
        bool match;
        if( white )
        {
            match = (player!=normalised_current && input_len<=player_len && player.substr(0,input_len)==input);
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


