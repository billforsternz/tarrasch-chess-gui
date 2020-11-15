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

// Some experiments on indexing the database for more speed - eg if searching for a position with a
//  White Knight on f3, don't bother searching the 10% or so of games where no white knight ever lands
//  on f3. The experiments are aimed at finding the best 8,16,32 or 64 piece square combos (like white
//  knight on f3 - likely a good one because many positions will have a white knight on f3, but a useful
//  proportion of games will never see a white knight there)
//
//#define DATABASE_EXPERIMENTS
#ifndef DURING_DEVELOPMENT
#undef  DATABASE_EXPERIMENTS	// Never unless during development
#endif
#ifdef DATABASE_EXPERIMENTS

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

struct PieceSquareCombo
{
    char piece;
    char file;
    char rank;
    bool hit;
    bool always_hit;
    unsigned long game_count;
    unsigned long ply_array[100];
    double hit_rate;
    double search_frequency;
    double effectiveness;
};
static PieceSquareCombo stats[16][64];
static int nbr_games_this_long[100];
static const int PLY_MIN = 4;
static const int PLY_MAX = 30;

int func_E( const void *q1, const void *q2 )
{
    const PieceSquareCombo *p1 = (PieceSquareCombo *)q1;
    const PieceSquareCombo *p2 = (PieceSquareCombo *)q2;
    if( p2->effectiveness > p1->effectiveness )
        return 1;
    if( p2->effectiveness < p1->effectiveness )
        return -1;
    return 0;
}

int func_H( const void *q1, const void *q2 )
{
    const PieceSquareCombo *p1 = (PieceSquareCombo *)q1;
    const PieceSquareCombo *p2 = (PieceSquareCombo *)q2;
    if( p2->hit_rate > p1->hit_rate )
        return 1;
    if( p2->hit_rate < p1->hit_rate )
        return -1;
    return 0;
}

int func_S( const void *q1, const void *q2 )
{
    const PieceSquareCombo *p1 = (PieceSquareCombo *)q1;
    const PieceSquareCombo *p2 = (PieceSquareCombo *)q2;
    if( p2->search_frequency > p1->search_frequency )
        return 1;
    if( p2->search_frequency < p1->search_frequency )
        return -1;
    return 0;
}

#endif

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
#ifdef DATABASE_EXPERIMENTS
            for( int i=1; i<=12; i++ )
            {
                char piece = idx_to_piece[i];
                for( int sq=0; sq<64; sq++ )
                {
                    PieceSquareCombo *p = &stats[i][sq];
                    p->piece = piece;
                    p->file = 'a' + (sq%8);
                    p->rank = '8' - ((sq>>3)&7);
                    if( piece=='p' && 8<=sq && sq<16 )
                        p->always_hit = true;
                    if( piece=='P' && 48<=sq && sq<56 )
                        p->always_hit = true;
                    if( piece=='r' && (sq==0||sq==7) )
                        p->always_hit = true;
                    if( piece=='n' && (sq==1||sq==6) )
                        p->always_hit = true;
                    if( piece=='b' && (sq==2||sq==5) )
                        p->always_hit = true;
                    if( piece=='q' && sq==3 )
                        p->always_hit = true;
                    if( piece=='k' && sq==4 )
                        p->always_hit = true;
                    if( piece=='R' && (sq==56||sq==63) )
                        p->always_hit = true;
                    if( piece=='N' && (sq==57||sq==62) )
                        p->always_hit = true;
                    if( piece=='B' && (sq==58||sq==61) )
                        p->always_hit = true;
                    if( piece=='Q' && sq==59 )
                        p->always_hit = true;
                    if( piece=='K' && sq==60 )
                        p->always_hit = true;
                }
            }
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
                        PieceSquareCombo *p = &stats[j][k];
                        p->hit = p->always_hit;
                        if( p->hit )
                            p->game_count++;
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
                        case thc::SPECIAL_WK_CASTLING:
                        case thc::SPECIAL_BK_CASTLING:
                        case thc::SPECIAL_WQ_CASTLING:
                        case thc::SPECIAL_BQ_CASTLING:
                        {
                            thc::Square castling_sq=thc::f1;
                            char castling_rook='R';
                            switch( mv.special )
                            {
                                case thc::SPECIAL_WQ_CASTLING:  castling_sq=thc::c1; break;
                                case thc::SPECIAL_BK_CASTLING:  castling_sq=thc::f8; castling_rook='r'; break;
                                case thc::SPECIAL_BQ_CASTLING:  castling_sq=thc::c8; castling_rook='r'; break;
                            }
                            int piece_idx = piece_to_idx[castling_rook];
                            unsigned int sq = static_cast<unsigned int>(castling_sq);
                            PieceSquareCombo *p = &stats[piece_idx][sq];
                            if( !p->hit )
                            {
                                p->hit = true;
                                p->game_count++;
                            }
                        }
                    }
                    int piece_idx = piece_to_idx[piece];

                    // A piece lands on a square - indicate that for this is a game where piece hit that square
                    unsigned int sq = static_cast<unsigned int>(mv.dst);
                    PieceSquareCombo *p = &stats[piece_idx][sq];
                    if( !p->hit )
                    {
                        p->hit = true;
                        p->game_count++;
                    }
                    cr.PlayMove(mv);

                    // After every move, update a ply indexed array for every piece/square combination
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
                                    PieceSquareCombo *pp = &stats[jj][kk];
                                    pp->ply_array[ply]++;
                                }
                            }
                        }
                    }
                }
            }

            // Display stats
            cprintf( "Investigation complete %d games\n", nbr_games );
            cprintf( "Stats for each piece/square combo\n", nbr_games );
            for( int i=1; i<=12; i++ )
            {
                char piece = idx_to_piece[i];
                const char *name="?";
                switch(piece)
                {
                    case 'r': name="Black Rook";        break;
                    case 'n': name="Black Knight";      break;
                    case 'b': name="Black Bishop";      break;
                    case 'q': name="Black Queen";       break;
                    case 'k': name="Black King";        break;
                    case 'p': name="Black Pawn";        break;
                    case 'R': name="White Rook";        break;
                    case 'N': name="White Knight";      break;
                    case 'B': name="White Bishop";      break;
                    case 'Q': name="White Queen";       break;
                    case 'K': name="White King";        break;
                    case 'P': name="White Pawn";        break;
                }
                cprintf( "\n%s\n", name );
                for( int r=0; r<8; r++ )
                {
                    for( int f=0; f<8; f++ )
                    {
                        PieceSquareCombo *p = &stats[i][r*8+f];

                        // Calculate H = hit rate
                        p->hit_rate = (double)(p->game_count) / (double)(nbr_games);

                        // Calculate S = search frequency
                        double sum = 0.0;
                        for( int ply=PLY_MIN; ply<=PLY_MAX; ply++ )
                        {
                            double ply_hit_rate = 0.0;
                            if( nbr_games_this_long[ply] > 0 )
                                ply_hit_rate =  (double)(p->ply_array[ply]) / (double)(nbr_games_this_long[ply]);
                            sum += ply_hit_rate;
                        }
                        p->search_frequency = sum / (PLY_MAX-PLY_MIN+1);   // Average rate at which piece/square combo
                                                                           //  appears in position between PLY_MIN and PLY_MAX

                        // Calculate E = effectiveness,  E = S*(1-H) 
                        // If search position contains the combo, then no need to search games where combo not hit
                        // So EFFECTIVENESS = AVG * proportion of games where combo not hit
                        //  eg if Nf3 appears in 40% of positions, and is hit somewhere in 90% of games
                        //  effectiveness = 40% * 10% = 4% speedup if we know which games Nf3 hits somewhere
                        double effectiveness = p->search_frequency * (double)(nbr_games - p->game_count)  / (double)(nbr_games);
                        p->effectiveness = effectiveness;
                    }
                    for( int f=0; f<8; f++ )
                    {
                        PieceSquareCombo *p = &stats[i][r*8+f];
                        char buf[100];
                        sprintf( buf, "   %c", r==0 ? p->file : ' ' );
                        if( f == 0 )
                            cprintf( "   " );
                        cprintf( "%-8s%c", buf, f==7 ? '\n' : ' ' );
                    }
                    for( int f=0; f<8; f++ )
                    {
                        PieceSquareCombo *p = &stats[i][r*8+f];
                        char buf[100];
                        sprintf( buf, "H=%2.2f%%", p->hit_rate*100.0 );
                        if( 0==strcmp(buf+2,"100.00%") )
                            strcpy(buf+2,"100%");
                        if( f == 0 )
                            cprintf( "   " );
                        cprintf( "%-8s%c", buf, f==7 ? '\n' : ' ' );
                    }
                    for( int f=0; f<8; f++ )
                    {
                        PieceSquareCombo *p = &stats[i][r*8+f];
                        char buf[100];
                        sprintf( buf, "S=%2.2f%%", p->search_frequency*100.0 );
                        if( 0==strcmp(buf+2,"100.00%") )
                            strcpy(buf+2,"100%");
                        if( f == 0 )
                            cprintf( " %c ", p->rank );
                        cprintf( "%-8s%c", buf, f==7 ? '\n' : ' ' );
                    }
                    for( int f=0; f<8; f++ )
                    {
                        PieceSquareCombo *p = &stats[i][r*8+f];
                        char buf[100];
                        sprintf( buf, "E=%2.2f%%", p->effectiveness*100.0 );
                        if( 0==strcmp(buf+2,"100.00%") )
                            strcpy(buf+2,"100%");
                        if( f == 0 )
                            cprintf( "   " );
                        cprintf( "%-8s%c", buf, f==7 ? '\n' : ' ' );
                    }
                }
            }

            for( int i=0; i<3; i++ )
            {
                switch(i)
                {
                    case 0: qsort( &stats[1][0], 12*64, sizeof(PieceSquareCombo), func_E );
                            cprintf( "\nSorting in order of effectiveness E\n" );
                            break;
                    case 1: qsort( &stats[1][0], 12*64, sizeof(PieceSquareCombo), func_H );
                            cprintf( "\nSorting in order of hit rate H\n" );
                            break;
                    case 2: qsort( &stats[1][0], 12*64, sizeof(PieceSquareCombo), func_S );
                            cprintf( "\nSorting in order of search frequency S\n" );
                            break;
                }
                
                PieceSquareCombo *p = &stats[1][0];
                double cumulative = 1.0;
                for( int j=0; j<32; )
                {
                    if( !p->always_hit )
                    {
                        const char *name="?";
                        switch(p->piece)
                        {
                            case 'r': name="Black Rook";        break;
                            case 'n': name="Black Knight";      break;
                            case 'b': name="Black Bishop";      break;
                            case 'q': name="Black Queen";       break;
                            case 'k': name="Black King";        break;
                            case 'p': name="Black Pawn";        break;
                            case 'R': name="White Rook";        break;
                            case 'N': name="White Knight";      break;
                            case 'B': name="White Bishop";      break;
                            case 'Q': name="White Queen";       break;
                            case 'K': name="White King";        break;
                            case 'P': name="White Pawn";        break;
                        }
                        if( i!=0 )
                            cprintf( "%s %c%c, H=%2.2f%%, S=%2.2f%%, E=%2.2f%%\n", name, p->file, p->rank, p->hit_rate*100.0, p->search_frequency*100.0, p->effectiveness*100.0 );
                        else
                        {
                            cumulative *= (1.0 - p->effectiveness);
                            cprintf( "%s %c%c, E=%2.2f%% cumulative E=%2.2f%% with %d bit%s, H=%2.2f%%, S=%2.2f%%\n", name, p->file, p->rank,
                                            p->effectiveness*100.0, (1.0-cumulative)*100.0, j+1, j+1>1?"s":"", p->hit_rate*100.0, p->search_frequency*100.0 );
                        }
                        j++;
                    }
                    p++;
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


wxMutex *WaitForWorkerThread( const char *title )
{   
    if( wxMUTEX_BUSY == s_mutex_tiny_database.TryLock() )
    {
        int base = the_database->background_load_permill;
        ProgressBar progress(title,"Loading database into memory",true,objs.frame);
        while( wxMUTEX_BUSY == s_mutex_tiny_database.TryLock() )
        {
            //static int now_before=-1;
            int now = the_database->background_load_permill;
            //if( now != now_before )
            //    cprintf( "base=%d, now=%d, now-base=%d, 1000-base=%d\n", base, now, now-base, 1000-base );
            //now_before = now;
            if( progress.Perfraction( now-base, 1000-base ) )
			{
				the_database->kill_background_load = true;
				break;
			}
            wxSafeYield();
        }
    }
    wxSafeYield();
    s_mutex_tiny_database.Unlock();
    return &s_mutex_tiny_database;
}

wxMutex *DontWaitForWorkerThread()
{   
    if( the_database )
		the_database->kill_background_load = true;
    s_mutex_tiny_database.TryLock();
    wxSafeYield();
    s_mutex_tiny_database.Unlock();
    return &s_mutex_tiny_database;
}

Database::Database( const char *db_file, bool another_instance_running )
{
    is_open = false;
    is_suspended = another_instance_running;
    if( is_suspended )
        database_error_msg = "Database is not open, database is not automatically loaded if another instance of Tarrasch is running";
    else
	{
		database_error_msg = "Database not yet open";
        Reopen( db_file );
	}
}

std::string Database::GetStatus()
{
    std::string s("Db: ");
    if( is_open )
	{
		wxFileName fn(db_filename.c_str());
		std::string name_without_directory = std::string(fn.GetFullName().c_str());
        s += name_without_directory;
		if( is_partial_load )
	        s += " (partial)"; 
	}
    else if( is_suspended )
        s += "(not loaded)"; 
	else
        s += "(none)"; 
    return s;
}

bool Database::GetFile( std::string &filename )
{
	filename = db_filename;
	return( is_open && !is_partial_load );
}

void Database::Reopen( const char *db_file )
{
	is_suspended = false;
	is_partial_load = false;
    background_load_permill = 0;
    kill_background_load = false;
    player_search_in_progress = false;
    tiny_db.Init();

    // Access the database.
    cprintf( "Database startup %s\n", db_file );
    db_filename = std::string(db_file);
    is_open = BinDbOpen( db_file, database_error_msg );
    if( is_open )
        LoadInBackground( this );
    
    // If connection failed, handle returns NULL
    cprintf( "Startup %s\n", is_open ? "SUCCESSFUL" : "FAILED" );
}


// Return bool operational
bool Database::IsOperational( std::string &error_msg )
{
    error_msg = database_error_msg;
    return is_open;
}

// Check for disabled to save memory
bool Database::IsSuspended()
{
    return is_suspended;
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
    if( 0<=row && static_cast<unsigned int>(row)<tiny_db.in_memory_game_cache.size() )
    {
        smart_ptr<ListableGame> p = tiny_db.in_memory_game_cache[row];
        p->GetCompactGame(*pact);
    }
    return 0;
}

bool Database::LoadAllGamesForPositionSearch( std::vector< smart_ptr<ListableGame> > &mega_cache )
{
    AutoTimer at("Load all games");
    background_load_permill = 0;
    kill_background_load = false;
    mega_cache.clear();
	is_partial_load = false;
    bool locked = false;
    bool killed = BinDbLoadAllGames( locked, false, mega_cache, background_load_permill, kill_background_load );
	is_partial_load = killed;
    BinDbClose();
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


