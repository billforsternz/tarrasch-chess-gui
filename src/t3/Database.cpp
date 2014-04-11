/****************************************************************************
 *  Database subsystem for looking up positions from a database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include "thc.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "sqlite3.h"
#include "CompressMoves.h"
#include "DbPrimitives.h"
#include "Database.h"
#include "wx/msgout.h"
#include "wx/progdlg.h"


// Create a handle for database connection, create a pointer to sqlite3
static sqlite3 *gbl_handle;

// A prepared statement for fetching from positions table
static sqlite3_stmt *gbl_stmt;

// Whereabouts we are in the virtual list control
static int gbl_expected;

// Whereabouts we are in the virtual list control
static int gbl_current;

// Number of elements in the virtual list control
static int gbl_count;

// The position we are looking for
thc::ChessPosition gbl_position;
uint64_t gbl_hash;
void db_set_gbl_position( thc::ChessPosition &pos )   // FIXME this is an abomination
{
    uint64_t hash = pos.Hash64Calculate();
    gbl_hash = hash;
    gbl_position = pos;
}

#define NBR_BUCKETS 4096

Database::Database()
{
    // Access the database.
    int retval = sqlite3_open(DB_FILE,&gbl_handle);
    
    // If connection failed, handle returns NULL
    tprintf( "DATABASE CONSTRUCTOR %s\n", retval ? "FAILED" : "SUCCESSFUL" );
}

Database::~Database()
{
    cprintf( "DATABASE DESTRUCTOR\n" );
#if 0
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
#endif
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
                //printf("%s:%s\t",sqlite3_column_name(gbl_stmt,col),val);
                if( col == 0 )
                {
                    //int game_id = atoi(val);
                    //game_ids.push_back(game_id);
                    game_count = atoi(val);
                    tprintf( "Game count = %d\n", game_count );
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
    tprintf("Get games count end\n");
    gbl_count = game_count;
    return game_count;
}


static int virtual_dump_game( DB_GAME_INFO *info, int game_id )
{
    // Get white player
    char buf[100];
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
                    info->white = val;
                    break;
                }
                case 1:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->black = val;
                    break;
                }
                case 2:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->event = val;
                    break;
                }
                case 3:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->site = val;
                    break;
                }
                case 4:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->result = val;
                    break;
                }
                case 5:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->date = val;
                    break;
                }
                case 6:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->white_elo = val;
                    break;
                }
                case 7:
                {
                    const char *val = (const char*)sqlite3_column_text(stmt,col);
                    info->black_elo = val;
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

void db_calculate_move_txt( DB_GAME_INFO *info )
{
    CompressMoves press;
    std::string move_txt;
    size_t len = info->str_blob.length();
    const char *blob = (const char*)info->str_blob.c_str();
    uint64_t hash = press.cr.Hash64Calculate();
    bool triggered=(hash==gbl_hash), first=true;
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
            if( count%2 == 0 || first )
            {
                first = false;
                char buf[20];
                sprintf( buf, "%d%s", count/2+1, count%2==0?".":"..." );
                move_txt += buf;
            }
            move_txt += s;
            move_txt += " ";
            if( nbr >= len )
                move_txt += info->result;
                else if( nbr<len-5 && move_txt.length()>100 )
                {
                    move_txt += "...";  // very long lines get over truncated by the list control (sad but true), see example below
                    break;
                }
        }
        hash = cr.Hash64Update( hash, mv );
        if( hash == gbl_hash )
            triggered = true;
    }
    info->move_txt = move_txt;
    //fprintf(f,"\n");
    
    // very long line example;
    //info->move_txt = "3...a6 4.Bxc6 bxc6 5.d3 e6 6.O-O Nf6 7.e5 Nd5 8.c4 Ne7 9.Nc3 Ng6 10.Ne4 Qc7 11.Be3"; //Nxe5 12.Nxc5 Nxf3+ 13.Qxf3 Bd6 14.Ne4 O-O 15.Nxd6 Qxd6 16.d4 f5 17.c5 Qb8 18.Bf4 Qxb2 19.Be5 Qd2 20.Qg3 Qh6 21.f4 Kf7 22.Rf3 Rg8 23.Qf2 a5 24.a4 Ba6 25.Rb1 Qh5 26.Rh3 Qe2 27.Qxe2 Bxe2 28.Rb7 Bg4 29.Rhb3 Rgd8 30.Rc7 Ke8 31.Rbb7 Bd1 32.Bxg7 Rac8 33.Rxc8 Rxc8 34.Ra7 Bxa4 35.Rxa5 Bd1 36.Be5 Bh5 37.Ra7 Rd8 38.Bc7 Rc8 39.Bd6 Rd8 40.Kf2 Kf7 41.Ke3 h6 42.Kd2 Kf6 43.Be5+ Ke7 44.Kc3 Be2 45.g3 Bf3 46.Kb4 Kf7 47.Ka5 Kg6 48.Kb6 Kh5 49.Kc7 Rf8 50.Bd6 Rf7 51.Kd8 Bd5 52.Rb7 Kg4 53.Be7 h5 54.Rb2 Rh7 55.Rf2 Rh8+ 56.Kxd7 Ra8 57.Re2 Ra4 58.Bf6 Kh3 59.Rxe6 Kxh2 60.Bh4 Bxe6+ 61.Kxe6 Rxd4 62.Kxf5 Rd5+ 63.Ke6 Rxc5 64.f5 Rc2 65.f6 Rf2 66.f7 Re2+ 67.Kf6 Rf2+ 68.Ke7 Re2+ 69.Kf6 Ra2 70.Kg6 Ra8 71.Kxh5 Rf8 72.Kg6 Kh3 73.Kg7 Rxf7+ 74.Kxf7 c5 1/2-1/2";

}

// Return index into vector where start position found
int db_calculate_move_vector( DB_GAME_INFO *info, std::vector<thc::Move> &moves )
{
    CompressMoves press;
    size_t len = info->str_blob.length();
    const char *blob = (const char*)info->str_blob.c_str();
    uint64_t hash = press.cr.Hash64Calculate();
    moves.clear();
    int ret=0;
    for( int nbr=0; nbr<len;  )
    {
        thc::Move mv;
        thc::ChessRules cr = press.cr;
        int nbr_used = press.decompress_move( blob, mv );
        if( nbr_used == 0 )
            break;
        moves.push_back(mv);
        blob += nbr_used;
        nbr += nbr_used;
        hash = cr.Hash64Update( hash, mv );
        if( hash == gbl_hash )
            ret = moves.size();
    }
    return ret;
}

static bool gbl_protect_recursion;   // FIXME


int Database::GetRow( DB_GAME_INFO *info, int row )
{
    if( gbl_protect_recursion )
    {
        cprintf( "Didn't expect that\n");
        return 0;
    }
    gbl_current = row;
    cprintf( "db_virtual_row() IN row=%d, expected=%d,%smatch\n", row, gbl_expected, row==gbl_expected?" ":" no " );
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
                        //printf("%s:%s\t",sqlite3_column_name(gbl_stmt,col),val);
                        if( col == 0 )
                        {
                            int game_id = atoi(val);
                            retval = virtual_dump_game( info, game_id );
                            db_calculate_move_txt(info);
                            cprintf( "db_virtual_row() SUCCESS game_id = %d\n", game_id );
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
            cprintf( "db_virtual_row() START query: %s\n",buf);
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


int Database::LoadAllGames( std::vector<DB_GAME_INFO> &cache, int nbr_games )
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
#ifdef NO_REVERSE
//                sprintf( buf, "SELECT white,black,event,site,result,date,white_elo,black_elo,moves from games WHERE game_id=%d", game_id );
                "SELECT games.game_id, games.white, games.black, games.event, games.site, games.result, games.date, games.white_elo, games.black_elo, games.moves from games, positions_%d WHERE games.game_id = positions_%d.game_id AND %spositions_%d.position_hash=%d",
                //"SELECT games.game_id, games.white, games.black, games.result, games.moves from games JOIN positions_%d ON games.game_id = positions_%d.game_id WHERE %spositions_%d.position_hash=%d",
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
                    info.white = val ? std::string(val) : "Whoops";
                }
                else if( col == 2 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.black = val ? std::string(val) : "Whoops";
                }
                else if( col == 3 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.event = val ? std::string(val) : "Whoops";
                }
                else if( col == 4 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.site = val ? std::string(val) : "Whoops";
                }
                else if( col == 5 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.result = val ? std::string(val) : "*";
                }
                else if( col == 6 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.date = val ? std::string(val) : "Whoops";
                }
                else if( col == 7 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.white_elo = val ? std::string(val) : "Whoops";
                }
                else if( col == 8 )
                {
                    const char *val = (const char*)sqlite3_column_text(gbl_stmt,col);
                    info.black_elo = val ? std::string(val) : "Whoops";
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
            cache.push_back( info );

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

// Returns row
int Database::FindRow( std::string &name )
{
    int row=0;
    char upper='A', lower='a';
    if( isalpha(name[0]) )
    {
        upper = name[0];
        lower = tolower(upper);
        upper = toupper(lower);
    }
    int retval = sqlite3_prepare_v2( gbl_handle, "SELECT games.white from games ORDER BY games.white ASC", -1, &gbl_stmt, 0 );
    bool okay = !retval;
    while( okay )
    {
        retval = sqlite3_step(gbl_stmt);
        okay = ( retval == SQLITE_ROW );
        if( okay )
        {
            const char *val = (const char*)sqlite3_column_text(gbl_stmt,0);
            std::string srow(val);
            if( srow >= name ) //&& (*val==lower || *val==upper) )
                break;
            row++;
        }
    }
    sqlite3_finalize(gbl_stmt);
    gbl_stmt = NULL;
    gbl_expected = -1;
    return row;
}

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

