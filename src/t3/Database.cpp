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
#include "DbPrimitives.h"
#include "Database.h"
#include "wx/msgout.h"
#include "wx/progdlg.h"


#define NBR_BUCKETS 4096

Database::Database()
{
    gbl_handle = 0;
    gbl_stmt = 0;
    player_search_stmt = 0;
    gbl_expected = 0;
    gbl_current = 0;
    gbl_count = 0;
    
    // Access the database.
    int retval = sqlite3_open(DB_FILE,&gbl_handle);
    
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

int Database::SetPosition( thc::ChessRules &cr )
{
    std::string empty;
    return SetPosition( cr, empty );
}


int Database::SetPosition( thc::ChessRules &cr, std::string &player_name )
{
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
    thc::ChessPosition start_pos;
    is_start_pos = false;
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
    if( cr == start_pos )
    {
        is_start_pos = true;
        sprintf( buf, "SELECT COUNT(*) from games%s", where_white.c_str() );
    }
    else
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

int Database::GetNbrGames( thc::ChessRules &cr )
{
    if( !gbl_handle )
        return 0;
    char buf[1000];
    uint64_t temp = cr.Hash64Calculate();
    int table_nbr = ((int)(temp>>32))&(NBR_BUCKETS-1);
    int hash = (int)(temp);
    int game_count=0;
    thc::ChessPosition start_pos;
    is_start_pos = false;
    if( cr == start_pos )
    {
        sprintf( buf, "SELECT COUNT(*) from games"  );
    }
    else
    {
        sprintf( buf, "SELECT COUNT(*) from games, positions_%d WHERE positions_%d.position_hash=%d AND games.game_id = positions_%d.game_id",
                table_nbr, table_nbr, hash, table_nbr );
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


int Database::LoadGameWithQuery( DB_GAME_INFO *info, int game_id )
{
    info->game_id = game_id;
    
    // Get white player
    char buf[1000];
    sqlite3_stmt *stmt;    // A prepared statement for fetching from games table
    int retval;
    sprintf( buf, "SELECT white,black,event,site,result,date,white_elo,black_elo,moves from games WHERE game_id=%d", game_id );
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &stmt, 0 );
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
                    info->r.white = val;
                    break;
                }
                case 1:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->r.black = val;
                    break;
                }
                case 2:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->r.event = val;
                    break;
                }
                case 3:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->r.site = val;
                    break;
                }
                case 4:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->r.result = val;
                    break;
                }
                case 5:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->r.date = val;
                    break;
                }
                case 6:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->r.white_elo = val;
                    break;
                }
                case 7:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->r.black_elo = val;
                    break;
                }
                case 8:
                {
                    int len = sqlite3_column_bytes(stmt,col);
                    //fprintf(f,"Move len = %d\n",len);
                    const char *blob = (const char*)sqlite3_column_blob(stmt,col);
                    std::string str_blob(blob,len);
                    info->str_blob = str_blob;
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


int Database::LoadGamesWithQuery(  std::string &player_name, bool white, std::vector< smart_ptr<MagicBase> > &games )
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
    cprintf( "LoadGamesWithQuery() START query: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
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
            DB_GAME_INFO info;
            
            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                if( col == 0 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.game_id = atoi(val);
                }
                else if( col == 1 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.result = val ? std::string(val) : "*";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    int len = sqlite3_column_bytes(gbl_stmt,col);
                    //fprintf(f,"Move len = %d\n",len);
                    const char *blob = (const char*)sqlite3_column_blob(gbl_stmt,col);
                    if( len && blob )
                    {
                        std::string str_blob(blob,len);
                        info.str_blob = str_blob;
                    }
                    else
                        info.str_blob = "";
                    make_smart_ptr( DB_GAME_INFO, new_info, info );
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


int Database::LoadGamesWithQuery( uint64_t hash, std::vector< smart_ptr<MagicBase> > &games, std::unordered_set<int> &games_set )
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
    cprintf( "LoadGamesWithQuery() START query: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
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
            DB_GAME_INFO info;
            
            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                if( col == 0 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.game_id = atoi(val);
                    int count = games_set.count(info.game_id);
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
                    info.r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.result = val ? std::string(val) : "*";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    int len = sqlite3_column_bytes(gbl_stmt,col);
                    //fprintf(f,"Move len = %d\n",len);
                    const char *blob = (const char*)sqlite3_column_blob(gbl_stmt,col);
                    if( len && blob )
                    {
                        std::string str_blob(blob,len);
                        info.str_blob = str_blob;
                    }
                    else
                        info.str_blob = "";
                    make_smart_ptr( DB_GAME_INFO, new_info, info );
                    games.push_back( std::move(new_info) );
                    games_set.insert(info.game_id);
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


std::string DB_GAME_INFO::Description()
{
    std::string white = this->r.white;
    std::string black = this->r.black;
    size_t comma = white.find(',');
    if( comma != std::string::npos )
        white = white.substr( 0, comma );
    comma = black.find(',');
    if( comma != std::string::npos )
        black = black.substr( 0, comma );
    int move_cnt = str_blob.length();
    std::string label = white;
    if( r.white_elo != "" )
    {
        label += " (";
        label += r.white_elo;
        label += ")";
    }
    label += " - ";
    label += black;
    if( r.black_elo != "" )
    {
        label += " (";
        label += r.black_elo;
        label += ")";
    }
    if( r.site != ""  && r.site != "?" )
    {
        label += ", ";
        label += r.site;
    }
    else if( r.event != "" && r.event != "?"  )
    {
        label += ", ";
        label += r.event;
    }
    if( r.date.length() >= 4 )
    {
        label += " ";
        label += r.date.substr(0,4);
    }
    bool result_or_moves = false;
    if( r.result != "*" )
    {
        result_or_moves = true;
        label += ", ";
        label += r.result;
        if( move_cnt > 0 )
            label += " in";
    }
    if( move_cnt > 0 )
    {
        if( !result_or_moves )
            label += ", ";
        char buf[100];
        sprintf( buf, " %d moves", (move_cnt+1)/2 );
        label += std::string(buf);
    }
    return label;
}

#if 0
std::string DB_GAME_INFO::db_calculate_move_txt( uint64_t hash_to_match )
{
    CompressMoves press;
    bool have_start_position = HaveStartPosition();
    if( have_start_position )
        press.Init( GetStartPosition() );
    std::string move_txt;
    size_t len = str_blob.length();
    const char *blob = (const char*)str_blob.c_str();
    uint64_t hash = press.cr.Hash64Calculate();
    bool triggered=(hash==hash_to_match), first=true;
	if( have_start_position )
		triggered = true;
    for( int count=0, nbr=0; nbr<len; count++ )
    {
        
        thc::ChessRules cr = press.cr;
        thc::Move mv;
        int nbr_used = press.decompress_move( blob, mv );
        if( nbr_used == 0 )
            break;
        blob += nbr_used;
        nbr += nbr_used;
        if( triggered )
        {
            std::string s = mv.NaturalOut(&cr);
            if( cr.white || first )
            {
                first = false;
                char buf[20];
                sprintf( buf, "%d%s", cr.full_move_count, cr.white?".":"..." );
                move_txt += buf;
            }
            move_txt += s;
            move_txt += " ";
            if( nbr >= len )
            {
                move_txt += r.result;
            }
            else if( nbr<len-5 && move_txt.length()>100 )
            {
                move_txt += "...";  // very long lines get over truncated by the list control (sad but true), see example below
                break;
            }
        }
        hash = cr.Hash64Update( hash, mv );
        if( hash == hash_to_match )
            triggered = true;
    }
    return move_txt;
    //fprintf(f,"\n");
    
    // very long line example;
    // move_txt = "3...a6 4.Bxc6 bxc6 5.d3 e6 6.O-O Nf6 7.e5 Nd5 8.c4 Ne7 9.Nc3 Ng6 10.Ne4 Qc7 11.Be3"; //Nxe5 12.Nxc5 Nxf3+ 13.Qxf3 Bd6 14.Ne4 O-O 15.Nxd6 Qxd6 16.d4 f5 17.c5 Qb8 18.Bf4 Qxb2 19.Be5 Qd2 20.Qg3 Qh6 21.f4 Kf7 22.Rf3 Rg8 23.Qf2 a5 24.a4 Ba6 25.Rb1 Qh5 26.Rh3 Qe2 27.Qxe2 Bxe2 28.Rb7 Bg4 29.Rhb3 Rgd8 30.Rc7 Ke8 31.Rbb7 Bd1 32.Bxg7 Rac8 33.Rxc8 Rxc8 34.Ra7 Bxa4 35.Rxa5 Bd1 36.Be5 Bh5 37.Ra7 Rd8 38.Bc7 Rc8 39.Bd6 Rd8 40.Kf2 Kf7 41.Ke3 h6 42.Kd2 Kf6 43.Be5+ Ke7 44.Kc3 Be2 45.g3 Bf3 46.Kb4 Kf7 47.Ka5 Kg6 48.Kb6 Kh5 49.Kc7 Rf8 50.Bd6 Rf7 51.Kd8 Bd5 52.Rb7 Kg4 53.Be7 h5 54.Rb2 Rh7 55.Rf2 Rh8+ 56.Kxd7 Ra8 57.Re2 Ra4 58.Bf6 Kh3 59.Rxe6 Kxh2 60.Bh4 Bxe6+ 61.Kxe6 Rxd4 62.Kxf5 Rd5+ 63.Ke6 Rxc5 64.f5 Rc2 65.f6 Rf2 66.f7 Re2+ 67.Kf6 Rf2+ 68.Ke7 Re2+ 69.Kf6 Ra2 70.Kg6 Ra8 71.Kxh5 Rf8 72.Kg6 Kh3 73.Kg7 Rxf7+ 74.Kxf7 c5 1/2-1/2";

}
#endif

void DB_GAME_INFO::Upscale( GameDocument &gd )
{
    CompactGame pact;
    GetCompactGame( pact );
    gd.r = pact.r;
    gd.start_position =  pact.start_position;
    std::vector<MoveTree> &variation = gd.tree.variations[0];
    variation.clear();
    MoveTree m;
    for( int i=0; i<pact.moves.size(); i++ )
    {
        m.game_move.move = pact.moves[i];
        variation.push_back(m);
    }
    gd.Rebuild();
}


void DB_GAME_INFO::Downscale( GameDocument &gd )
{
    r = gd.r;
    transpo_nbr = 0;
    CompressMoves press;
    std::vector<MoveTree> &variation = gd.tree.variations[0];
    std::string str;
    for( int i=0; i<variation.size(); i++ )
    {
        thc::Move mv = variation[i].game_move.move;
        char ch;
        press.compress_move( mv, &ch );
        str.push_back(ch);
    }
    str_blob = str;
}

void DB_GAME_INFO_FEN::Downscale( GameDocument &gd )
{
    r = gd.r;
    transpo_nbr = 0;
	start_position = gd.start_position;
    CompressMoves press;
	press.Init( start_position );  // temp temp slow
    std::vector<MoveTree> &variation = gd.tree.variations[0];
    std::string str;
    for( int i=0; i<variation.size(); i++ )
    {
        thc::Move mv = variation[i].game_move.move;
        char ch;
        press.compress_move( mv, &ch );
        str.push_back(ch);
    }
    str_blob = str;
}

std::string CompactGame::Description()
{
    std::string white = this->r.white;
    std::string black = this->r.black;
    size_t comma = white.find(',');
    if( comma != std::string::npos )
        white = white.substr( 0, comma );
    comma = black.find(',');
    if( comma != std::string::npos )
        black = black.substr( 0, comma );
    int move_cnt = moves.size();
    std::string label = white;
    if( r.white_elo != "" )
    {
        label += " (";
        label += r.white_elo;
        label += ")";
    }
    label += " - ";
    label += black;
    if( r.black_elo != "" )
    {
        label += " (";
        label += r.black_elo;
        label += ")";
    }
    if( r.site != ""  && r.site != "?" )
    {
        label += ", ";
        label += r.site;
    }
    else if( r.event != "" && r.event != "?"  )
    {
        label += ", ";
        label += r.event;
    }
    if( r.date.length() >= 4 )
    {
        label += " ";
        label += r.date.substr(0,4);
    }
    bool result_or_moves = false;
    if( r.result != "*" )
    {
        result_or_moves = true;
        label += ", ";
        label += r.result;
        if( move_cnt > 0 )
            label += " in";
    }
    if( move_cnt > 0 )
    {
        if( !result_or_moves )
            label += ", ";
        char buf[100];
        sprintf( buf, " %d moves", (move_cnt+1)/2 );
        label += std::string(buf);
    }
    return label;
}

#if 0
std::string CompactGame::db_calculate_move_txt( uint64_t hash_to_match )
{
    thc::ChessRules cr = start_position;
    std::string move_txt;
    size_t len = moves.size();
    uint64_t hash = cr.Hash64Calculate();
    bool triggered=(hash==hash_to_match), first=true;
    bool is_initial_position = ( 0 == strcmp(start_position.squares,"rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR")
                                ) && start_position.white;

	if( !is_initial_position )
		triggered = true;
    for( int i=0; i<len; i++ )
    {
        thc::Move mv = moves[i];
        if( triggered )
        {
            std::string s = mv.NaturalOut(&cr);
            if( cr.white || first )
            {
                first = false;
                char buf[20];
                sprintf( buf, "%d%s", cr.full_move_count, cr.white?".":"..." );
                move_txt += buf;
            }
            move_txt += s;
            move_txt += " ";
            if( i+1 >= len )
            {
                move_txt += r.result;
            }
            else if( i+1<len-5 && move_txt.length()>100 )
            {
                move_txt += "...";  // very long lines get over truncated by the list control (sad but true), see example below
                break;
            }
        }
        hash = cr.Hash64Update( hash, mv );
        cr.PlayMove(mv);
        if( hash == hash_to_match )
            triggered = true;
    }
    return move_txt;
    //fprintf(f,"\n");
    
    // very long line example;
    // move_txt = "3...a6 4.Bxc6 bxc6 5.d3 e6 6.O-O Nf6 7.e5 Nd5 8.c4 Ne7 9.Nc3 Ng6 10.Ne4 Qc7 11.Be3"; //Nxe5 12.Nxc5 Nxf3+ 13.Qxf3 Bd6 14.Ne4 O-O 15.Nxd6 Qxd6 16.d4 f5 17.c5 Qb8 18.Bf4 Qxb2 19.Be5 Qd2 20.Qg3 Qh6 21.f4 Kf7 22.Rf3 Rg8 23.Qf2 a5 24.a4 Ba6 25.Rb1 Qh5 26.Rh3 Qe2 27.Qxe2 Bxe2 28.Rb7 Bg4 29.Rhb3 Rgd8 30.Rc7 Ke8 31.Rbb7 Bd1 32.Bxg7 Rac8 33.Rxc8 Rxc8 34.Ra7 Bxa4 35.Rxa5 Bd1 36.Be5 Bh5 37.Ra7 Rd8 38.Bc7 Rc8 39.Bd6 Rd8 40.Kf2 Kf7 41.Ke3 h6 42.Kd2 Kf6 43.Be5+ Ke7 44.Kc3 Be2 45.g3 Bf3 46.Kb4 Kf7 47.Ka5 Kg6 48.Kb6 Kh5 49.Kc7 Rf8 50.Bd6 Rf7 51.Kd8 Bd5 52.Rb7 Kg4 53.Be7 h5 54.Rb2 Rh7 55.Rf2 Rh8+ 56.Kxd7 Ra8 57.Re2 Ra4 58.Bf6 Kh3 59.Rxe6 Kxh2 60.Bh4 Bxe6+ 61.Kxe6 Rxd4 62.Kxf5 Rd5+ 63.Ke6 Rxc5 64.f5 Rc2 65.f6 Rf2 66.f7 Re2+ 67.Kf6 Rf2+ 68.Ke7 Re2+ 69.Kf6 Ra2 70.Kg6 Ra8 71.Kxh5 Rf8 72.Kg6 Kh3 73.Kg7 Rxf7+ 74.Kxf7 c5 1/2-1/2";
    
}
#endif

void CompactGame::Upscale( GameDocument &gd )
{
    gd.r = r;
    bool have_start_position = HaveStartPosition();
    if( have_start_position )
        gd.start_position =  GetStartPosition();
    else
        gd.start_position.Init();
    std::vector<MoveTree> &variation = gd.tree.variations[0];
    variation.clear();
    MoveTree m;
    for( int i=0; i<moves.size(); i++ )
    {
        m.game_move.move = moves[i];
        variation.push_back(m);
    }
    gd.Rebuild();
}

void CompactGame::Downscale( GameDocument &gd )
{
    r = gd.r;
    transpo_nbr = 0;
	start_position = gd.start_position;
    std::vector<MoveTree> &variation = gd.tree.variations[0];
    std::string str;
    for( int i=0; i<variation.size(); i++ )
    {
        thc::Move mv = variation[i].game_move.move;
        moves.push_back(mv);
    }
}

int Database::GetRow( DB_GAME_INFO *info, int row )
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

int Database::GetRowRaw( DB_GAME_INFO *info, int row )
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
                            retval = LoadGameWithQuery( info, game_id );
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
            if( is_start_pos )
            {
                sprintf( buf, 
                        "SELECT games.game_id from games%s ORDER BY games.white ASC LIMIT %d,100", where_white.c_str(), row );
                  //      "SELECT games.game_id from games%s ORDER BY games.rowid LIMIT %d,100", where_white.c_str(), row );
            }
            else
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


int Database::LoadAllGames( std::vector< smart_ptr<MagicBase> > &cache, int nbr_games )
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
    if( is_start_pos )
    {
        sprintf( buf,
                "SELECT games.game_id from games%s ORDER BY games.white ASC", where_white.c_str() );
    }
    else
    {
        sprintf( buf,
#if 1 //def NO_REVERSE
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d",
#else
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d ORDER BY games.rowid DESC",
#endif
                table_nbr, table_nbr, white_and.c_str(), table_nbr, hash);
    }
    cprintf( "LoadAllGames() START query: %s\n",buf);
    retval = sqlite3_prepare_v2( gbl_handle, buf, -1, &gbl_stmt, 0 );
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
            DB_GAME_INFO info;

            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for( int col=0; col<cols; col++ )
            {
                if( col == 0 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.game_id = atoi(val);
                }
                else if( col == 1 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.result = val ? std::string(val) : "*";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.r.black_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 9 )
                {
                    int len = sqlite3_column_bytes(gbl_stmt,col);
                    //fprintf(f,"Move len = %d\n",len);
                    const char *blob = (const char*)sqlite3_column_blob(gbl_stmt,col);
                    if( len && blob )
                    {
                        std::string str_blob(blob,len);
                        info.str_blob = str_blob;
                    }
                    else
                        info.str_blob = "";
                }
            }
            make_smart_ptr( DB_GAME_INFO, new_info, info );
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
        retval = sqlite3_prepare_v2( gbl_handle, white ? "SELECT games.white from games ORDER BY games.white ASC" : "SELECT games.black from games ORDER BY games.white ASC", -1, &player_search_stmt, 0 );
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

