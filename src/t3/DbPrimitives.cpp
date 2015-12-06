/****************************************************************************
 *  Low level database maintenance functions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

//
//  This file has evolved into a DatabaseWrite facility and should be
//  renamed as such and wrapped in a class to complement DatabaseRead
//

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include "Appdefs.h"
#include "thc.h"
#include "sqlite3.h"
#include "CompressMoves.h"
#include "ProgressBar.h"
#include "DbPrimitives.h"
#include "DebugPrintf.h"
static bool purge_bucket(int bucket_idx);
static bool purge_buckets( bool force=false );
#define NBR_BUCKETS 4096
#define PURGE_QUOTA 10000


// Handle for database connection
static sqlite3 *handle;
static int game_id;
static std::string error_msg;
static bool create_mode;

// Error reporting mechanism
std::string db_primitive_error_msg()
{
    std::string msg = error_msg;
    error_msg = "";
    return msg;
}


// Returns bool ok
bool db_primitive_open( const char *db_file, bool create_mode_parm )
{
    cprintf( "db_primitive_open()\n" );
    create_mode = create_mode_parm;
    
    // Try to create the database. If it doesnt exist, it would be created
    //  pass a pointer to the pointer to sqlite3, in short sqlite3**
    int retval = sqlite3_open(db_file,&handle);
    
    // If connection failed, handle returns NULL
    if( retval )
    {
        error_msg = "Database error: CONNECTION FAILED";
        cprintf( error_msg.c_str() );
        cprintf( "\n" );
        return false;
    }
    cprintf("Connection successful\n");
    return true;
}

static wxWindow *window_parent;

// Returns bool ok
bool db_primitive_transaction_begin( wxWindow *parent )
{
    window_parent = parent;
    char *errmsg=NULL;
    int retval = sqlite3_exec( handle, "BEGIN TRANSACTION",0,0,&errmsg);
    bool ok = (retval==0);
    if( retval && errmsg )
    {
        char buf[1000];
        sprintf( buf, "Database error: sqlite3_exec(BEGIN TRANSACTION) FAILED [%s]", errmsg );
        error_msg = buf;
    }
	else if (!ok )
    {
        error_msg = "Database error: sqlite3_exec(BEGIN TRANSACTION) FAILED";
    }
    return ok;
}


// Returns bool ok
bool db_primitive_transaction_end()
{
    bool ok = purge_buckets();
    if( ok )
    {
        char *errmsg=NULL;
        int retval = sqlite3_exec( handle, "COMMIT TRANSACTION", 0, 0, &errmsg);
        bool ok = (retval==0);
        if( retval && errmsg )
        {
            char buf[1000];
            sprintf( buf, "Database error: sqlite3_exec(COMMIT TRANSACTION) FAILED %s", errmsg );
            error_msg = buf;
        }
        else if( !ok )
        {
            error_msg = "Database error: sqlite3_exec(COMMIT TRANSACTION) FAILED";
        }
    }
    return ok;
}

// Returns bool ok
bool db_primitive_create_tables()
{
    ProgressBar prog( "Creating database, step 1 of 4", "Creating database tables", window_parent );
    
    cprintf( "db_primitive_create_tables()\n" );
    
    // Create tables if not existing
    cprintf( "Create games table");
    int retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS description (description TEXT, version INTEGER)",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(CREATE description) FAILED";
        return false;
    }
    if( prog.Percent(2) )
    {
        error_msg = "cancel";
        return false;
    }
    char buf[200];
    sprintf( buf,"INSERT INTO description VALUES('Description of database, not currently used for anything, goes here', %d )", DATABASE_VERSION_NUMBER_SUPPORTED );
    retval = sqlite3_exec(handle,buf,0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(INSERT description) FAILED";
        return false;
    }
    if( prog.Percent(4) )
    {
        error_msg = "cancel";
        return false;
    }
    retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS games (game_id INTEGER, game_hash INT8 UNIQUE, white TEXT, black TEXT, event TEXT, site TEXT, round TEXT, result TEXT, date TEXT, white_elo TEXT, black_elo TEXT, eco TEXT, moves BLOB)",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(CREATE games) FAILED";
        return false;
    }
    if( prog.Percent(6) )
    {
        error_msg = "cancel";
        return false;
    }
    retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS games_duplicates (game_id INTEGER, game_hash INT8, white TEXT, black TEXT, event TEXT, site TEXT, round TEXT, result TEXT, date TEXT, white_elo TEXT, black_elo TEXT, eco TEXT, moves BLOB)",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(CREATE games_duplicates) FAILED";
        return false;
    }
    if( prog.Percent(8) )
    {
        error_msg = "cancel";
        return false;
    }
    cprintf( "Create positions tables");
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
        if( prog.Permill( 100 + (i*900) / NBR_BUCKETS ) )
        {
            error_msg = "cancel";
            return false;
        }
		if( i%10 == 0 )
			cprintf( "Creating bucket %d\n", i);
        char buf[200];
        sprintf( buf, "CREATE TABLE IF NOT EXISTS positions_%d (game_id INTEGER, position_hash INTEGER)", i );
        retval = sqlite3_exec(handle,buf,0,0,0);
        if( retval )
        {
            char buf[100];
            sprintf( buf, "Database error: sqlite3_exec(CREATE positions_%d) FAILED\n",i);
            error_msg = buf;
            return false;
        }
    }
    cprintf( "Create positions tables end");
    return true;
}

// Returns bool ok
bool db_primitive_delete_previous_data()
{
    int retval = sqlite3_exec( handle, "DELETE FROM games",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(DELETE games) Failed";
        return false;
    }
    retval = sqlite3_exec( handle, "DELETE FROM positions",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(DELETE positions) Failed";
        return false;
    }
    return true;
}

// Returns bool ok
bool db_primitive_create_indexes()
{
    ProgressBar prog( "Creating database, step 4 of 4", "Indexing positions", window_parent );
    DebugPrintfTime turn_on_time_reporting;
    bool ok = purge_buckets();
    if( !ok )
        return false;
    cprintf( "Create games index\n");
    int retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_games ON games(game_id)",0,0,0);
    cprintf( "Create games index end\n");
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(CREATE INDEX games) FAILED";
        return false;
    }
    if( prog.Percent(1) )
    {
        error_msg = "cancel";
        return false;
    }
    cprintf( "Create games(white) index\n");
    retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_white ON games(white)",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec() FAILED 1";
        return false;
    }
    if( prog.Percent(2) )
    {
        error_msg = "cancel";
        return false;
    }
    cprintf( "Create games(black) index\n");
    retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_black ON games(black)",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec() FAILED 2";
        return false;
    }
    if( prog.Percent(3) )
    {
        error_msg = "cancel";
        return false;
    }
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
        if( prog.Permill( 30 + (i*970) / NBR_BUCKETS ) )
        {
            error_msg = "cancel";
            return false;
        }
        char buf[200];
        if( i%100 == 0 )
            cprintf( "Create idx%d begin\n", i );
        sprintf( buf, "CREATE INDEX IF NOT EXISTS idx%d ON positions_%d(position_hash,game_id)",i,i);
        retval = sqlite3_exec(handle,buf,0,0,0);
        if( i%100 == 0 )
            cprintf( "Create idx%d end\n", i );
        if( retval )
        {
            error_msg = "Database error: sqlite3_exec() FAILED 3";
            return false;
        }
    }
    return true;
}

/*
unsigned int levenshtein_distance(const std::string &s1, const std::string &s2)
{
	const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<unsigned int> col(len2+1), prevCol(len2+1);
    
	for (unsigned int i = 0; i < prevCol.size(); i++)
		prevCol[i] = i;
	for (unsigned int i = 0; i < len1; i++)
    {
		col[0] = i+1;
		for (unsigned int j = 0; j < len2; j++)
        {
			col[j+1] = std::min( std::min(prevCol[1 + j] + 1, col[j] + 1),
								prevCol[j] + (s1[i]==s2[j] ? 0 : 1) );
        }
		col.swap(prevCol);
	}
	return prevCol[len2];
} 

static void t( const char *left, const char *right )
{
    unsigned int x = levenshtein_distance(left,right);
    cprintf( "[%s] [%s] %u\n", left, right, x );
} */

void db_primitive_close()
{
    // Close the handle to free memory
    if( handle )
    {
        sqlite3_close(handle);
        handle = 0;
    }
}


int db_primitive_count_games()
{
    int game_count=0;
    
    // select matching rows from the table
    char buf[100];
    sqlite3_stmt *stmt;    // A prepared statement for fetching tables
    sprintf( buf, "SELECT COUNT(*) from games" );
    int retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, 0 );
    if( retval )
    {
        error_msg = "Database error: SELECT COUNT(*) from games FAILED";
        return -1;
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
                //cprintf("%s:%s\t",sqlite3_column_name(gbl_stmt,col),val);
                if( col == 0 )
                {
                    //int game_id = atoi(val);
                    //game_ids.push_back(game_id);
                    game_count = atoi(val);
                    cprintf( "Game count = %d\n", game_count );
                }
            }
        }
        else if( retval == SQLITE_DONE )
        {
            // All rows finished
            sqlite3_finalize(stmt);
            break;
        }
        else
        {
            // Some error encountered
            error_msg = "Database error: Calculating games count FAILED";
            return -1;
        }
    }
    cprintf("Get games count end - %d games\n", game_count );
    game_id = game_count;
    return game_count;
}


// Returns bool hasAlpha
static bool sanitise( const char *in, char *out, int out_siz )
{
    bool hasAlpha=false;
    int len=0;
    while( *in && len<out_siz-3 )
    {
        char c = *in++;
        if( !isascii(c) )
            *out = '_';
        else if( c!=' ' &&  c!='-' &&  c!='.' && c!=',' && !isalnum(c) )
            *out = '_';
        else
        {
            *out = c;
            if( !hasAlpha && isalpha(c) )
                hasAlpha = true;
        }
        out++;
        len++;
    }
    *out = '\0';
    return hasAlpha;
}

bool db_primitive_flush()
{
    bool ok=purge_buckets(true);
    return ok;
}

std::vector<std::pair<int,int>> buckets[NBR_BUCKETS];
static bool purge_buckets( bool force )
{
    bool ok=true;
    bool required=force;
    for( int i=0; !required && i<NBR_BUCKETS; i++ )
    {
        std::vector<std::pair<int,int>> *bucket = &buckets[i];
        int count = bucket->size();
        if( count > 0 )
            required = true;
    }
    if( required )
    {
        ProgressBar pb( create_mode ? "Creating database, step 3 of 4" : "Adding games to database, step 2 of 2", "Writing to database" , window_parent );
        for( int i=0; ok && i<NBR_BUCKETS; i++ )
        {
            ok = purge_bucket(i);
            if( ok )
            {
                if( i%10 == 0 )
                    cprintf( "Purging bucket %d\n", i );
                if( pb.Permill( (i*1000) / NBR_BUCKETS ) )
                {
                    error_msg = "cancel";
                    ok = false;
                }
            }
        }
    }
    return ok;
}

static bool purge_bucket( int bucket_idx )
{
    char *errmsg;
    char insert_buf[2000];
    std::vector<std::pair<int,int>> *bucket = &buckets[bucket_idx];
    int count = bucket->size();
    if( count > 0 )
    {
        std::sort( bucket->begin(), bucket->end() );
        for( int j=0; j<count; j++ )
        {
            std::pair<int,int> duo = (*bucket)[j];
            sprintf( insert_buf, "INSERT INTO positions_%d VALUES(%d,%d)", bucket_idx, duo.second, duo.first );
            int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
            if( retval && errmsg )
            {
                char buf[1000];
                sprintf( buf, "Database error: purging bucket, sqlite3_exec(INSERT 3) FAILED %s", errmsg );
                error_msg = buf;
                return false;
            }
            else if( retval )
            {
                error_msg = "Database error: purging bucket, sqlite3_exec(INSERT 3) FAILED";
                return false;
            }
        }
        bucket->clear();
    }
    return true;
}


// Return true if this game already in database
bool db_primitive_check_for_duplicate( bool &signal_error, uint64_t game_hash, const char *white, const char *black, const char *result, const char *blob_moves )
{
    signal_error = false;
    for( int table_idx=0; table_idx<2; table_idx++ )
    {
        // select matching rows from the table
        char buf[200];
        sqlite3_stmt *stmt;    // A prepared statement for fetching tables
        sprintf( buf, "SELECT white, black, result, moves from %s WHERE game_hash=%lld", table_idx==0?"games":"games_duplicates", game_hash );
        const char *errmsg;
        int retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, &errmsg );
        if( retval && errmsg )
        {
            char buf2[1000];
            sprintf( buf2, "Database error: db_primitive_check_for_duplicate() 1 %s (%s)", errmsg, buf );
            error_msg = buf2;
            signal_error = true;
            return false;
        }
        else if( retval )
        {
            char buf2[1000];
            sprintf( buf2, "Database error: db_primitive_check_for_duplicate() 1 (%s)",buf );
            error_msg = buf2;
            signal_error = true;
            return false;
        }
        
        // Read the game info
        bool white_match=false;
        bool black_match=false;
        bool result_match=false;
        bool moves_match=false;
        int cols = sqlite3_column_count(stmt);
        for(;;)  // Read one row from games, potentially many rows in games_duplicates
        {
            retval = sqlite3_step(stmt);
            if( retval == SQLITE_ROW )
            {
                for( int col=0; col<cols; col++ )
                {
                    if( col == 0 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        if( val )
                            white_match = (0 == strcmp(white,val));
                    }
                    else if( col == 1 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        if( val )
                            black_match = (0 == strcmp(black,val));
                    }
                    else if( col == 2 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        if( val )
                            result_match = (0 == strcmp(result,val));
                    }
                    else if( col == 3 )
                    {
                        int len = sqlite3_column_bytes(stmt,col);
                        //fprintf(f,"Move len = %d\n",len);
                        //if( len > 10 )
                        //    cprintf( "A real game....\n" );
                        const char *blob = (const char*)sqlite3_column_blob(stmt,col);
                        if( blob )
                            moves_match = (0 == memcmp(blob_moves,blob,len));
                    }
                }
            }
            else if( retval == SQLITE_DONE )
            {
                // All rows finished
                sqlite3_finalize(stmt);
                stmt = NULL;
                if( white_match && black_match && result_match && moves_match )
                {
                    cprintf( "Duplicate game %s-%s %s found\n", white,black,result);
                    return true;    // duplicate found
                }
                break;
            }
            else
            {
                // Some error encountered
                error_msg = "Database error: db_primitive_check_for_duplicate() 2";
                signal_error = true;
                return false;
            }
        }
    }
    return false;   // return false unless we explicitly DID find a duplicate
}




void db_temporary_hack( const char *white, const char *black, const char *event, const char *site, const char *round, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo, const char *eco,
                                    int nbr_moves, thc::Move *moves )
{
    FILE *ofile = fopen( "C:/Users/Bill/Downloads/millionbase/EdwardLaskerFalsePositives.pgn", "at" );
    if( ofile )
    {
        fprintf( ofile, "[Event \"%s\"]\n",     event );
        fprintf( ofile, "[Site \"%s\"]\n",      site );
        fprintf( ofile, "[Date \"%s\"]\n",      date );
        fprintf( ofile, "[Round \"%s\"]\n",     round );
        fprintf( ofile, "[White \"%s\"]\n",     white );
        fprintf( ofile, "[Black \"%s\"]\n",     black );
        fprintf( ofile, "[Result \"%s\"]\n",    result );
        fprintf( ofile, "[ECO \"%s\"]\n\n",     eco );

        std::string moves_txt;
        thc::ChessRules cr;
        for( int i=0; i<nbr_moves; i++ )
        {
            if( i != 0 )
                moves_txt += " ";
            if( cr.white )
            {
                char buf[100];
                sprintf( buf, "%d. ", cr.full_move_count );
                moves_txt += buf;
            }
            thc::Move mv = moves[i];
            std::string s = mv.NaturalOut(&cr);
            moves_txt += s;
            cr.PlayMove(mv);
        }
        moves_txt += " ";
        moves_txt += result;
        std::string justified;
        const char *s = moves_txt.c_str();
        int col=0;
        while( *s )
        {
            char c = *s++;
            col++;
            if( c == ' ' )
            {
                const char *t = s;
                int col_end_of_next_word = col+1;
                while( *t!=' ' && *t!='\0' )
                {
                    col_end_of_next_word++;    
                    t++;
                }
                if( col_end_of_next_word > 81 )
                {
                    c = '\n';
                    col = 0;
                }
            }
            justified += c;
        }
        fprintf( ofile, "%s\n\n",  justified.c_str() );
        fclose(ofile);
    }
}

// returns bool inserted
bool db_primitive_insert_game( bool &signal_error, const char *white, const char *black, const char *event, const char *site, const char *round, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo, const char *eco,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  )
{
    signal_error = false;       // use this mechanism because returning false doesn't necessarily mean error
    char *errmsg;
    char insert_buf[2000];
    char blob_txt_buf[4000];    // about 500 moves each
    char blob_buf[2000];
    char white_buf[200];
    char black_buf[200];
    char event_buf[200];
    char site_buf[200];
    char round_buf[200];
    char eco_buf[200];
    if( nbr_moves < 3 )    // skip 'games' with zero, one or two moves
        return false;           // not inserted, not error
    if( white_elo && black_elo )
    {
        int elo_w = atoi(white_elo);
        int elo_b = atoi(black_elo);
        if( 0<elo_w && elo_w<2000 && 0<elo_b && elo_b<2000 )
            // if any elo information, need at least one good player
            return false;   // not inserted, not error
    }
    bool alphaWhite = sanitise( white,  white_buf, sizeof(white_buf) );
    bool alphaBlack = sanitise( black,  black_buf, sizeof(black_buf) );
    if( !alphaWhite || !alphaBlack )    // skip games with no names
        return false;   // not inserted, not error
    sanitise( event,  event_buf, sizeof(event_buf) );
    sanitise( site, site_buf,  sizeof(site_buf) );
    sanitise( round, round_buf,  sizeof(round_buf) );
    sanitise( eco,  eco_buf,  sizeof(eco_buf) );
    CompressMoves press;
    uint64_t hash = press.cr.Hash64Calculate();
    uint64_t counter=0, game_hash=hash;
    char *put_txt = blob_txt_buf;
    char *put = blob_buf;
    for( int i=0; i<nbr_moves && put_txt<blob_txt_buf+sizeof(blob_txt_buf)-10; i++ )
    {
        thc::Move mv = moves[i];
        char c = press.CompressMove(mv);
        *put++ = c;
        char hi = (c>>4)&0x0f;
        *put_txt++ = (hi>=10 ? hi-10+'A' : hi+'0');
        char lo = c&0x0f;
        *put_txt++ = (lo>=10 ? lo-10+'A' : lo+'0');
        hash = press.cr.Hash64Update( hash, mv );
        game_hash = game_hash ^ hash ^ counter;
        counter++;
    }
    *put_txt = '\0';
    *put = '\0';

    // Try to insert the game
    sprintf( insert_buf, "INSERT INTO games VALUES(%d,%lld,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',X'%s')",
                        game_id, game_hash, white_buf, black_buf, event_buf, site_buf, round_buf, result,
                        date, white_elo, black_elo, eco_buf, blob_txt_buf );
    int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
    
    // This is one spot where a database failure is (kind of) expected - the game_hash has "unique" property
    if( retval && errmsg )
    {
        char buf[200];
        strncpy( buf, errmsg, sizeof(buf)-1 );
        buf[ sizeof(buf)-1 ] = '\0';
        char *p = buf;
        while( *p )
        {
            if( isalpha(*p) && isupper(*p) )
                *p = tolower(*p);
            p++;
        }
        
        // If it wasn't a "unique" issue, we bail out
        if( !strstr(buf,"unique") )
        {
            char buf2[1000];
            sprintf( buf2, "Database error: db_primitive_insert_game() 1 %s (%s)\n", errmsg, insert_buf );
            error_msg = buf2;
            signal_error = true;
            return false;   // not inserted, error
        }
        //cprintf("Non unique game hash %s (%s)\n", errmsg, insert_buf );

        // Otherwise see if the game is a duplicate of existing game (if it's not, then it will be the same
        //  game played on a different occasion)
        bool is_duplicate = db_primitive_check_for_duplicate( signal_error, game_hash, white_buf, black_buf, result, blob_buf );
        if( signal_error )
            return false;   // not inserted, error
        
        // A duplicate game is simply discarded
        if( is_duplicate )
        {
            //cprintf( "***  DUPLICATE GAME DISCARDED\n" );
            // ZOMBIE bug fix - return here, don't add bogus moves and increment game count
            db_temporary_hack( white, black, event, site, round, result,
                                    date, white_elo, black_elo, eco,
                                    nbr_moves, moves  );
            return false;   // not inserted, not error
        }
        
        // If it's the same game played by different players, save it with game_hash NULL, which is doesn't have to
        //  be unique fortunately
        else
        {
            //cprintf( "***  DUPLICATE GAME: %s-%s %s %s %s\n", white_buf, black_buf, event_buf, site_buf, result, date );
            sprintf( insert_buf, "INSERT INTO games VALUES(%d,NULL,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',X'%s')",
                    game_id, white_buf, black_buf, event_buf, site_buf, round_buf, result,
                    date, white_elo, black_elo, eco_buf, blob_txt_buf );
            int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
            if( retval && errmsg )
            {
                char buf2[1000];
                sprintf( buf2, "Database error: db_primitive_insert_game() 2 %s (%s)", errmsg, insert_buf );
                signal_error = true;
                return false;   // not inserted, error
            }
            else if( retval )
            {
                char buf2[1000];
                sprintf( buf2, "Database error: db_primitive_insert_game() 2 (%s)", insert_buf );
                signal_error = true;
                return false;   // not inserted, error
            }
            sprintf( insert_buf, "INSERT INTO games_duplicates VALUES(%d,%lld,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',X'%s')",
                    game_id, game_hash, white_buf, black_buf, event_buf, site_buf, round_buf, result,
                    date, white_elo, black_elo, eco_buf, blob_txt_buf );
            retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
            if( retval && errmsg )
            {
                char buf2[1000];
                sprintf( buf2, "Database error: db_primitive_insert_game() 3 %s (%s)", errmsg, insert_buf );
                signal_error = true;
                return false;   // not inserted, error
            }
            else if( retval )
            {
                char buf2[1000];
                sprintf( buf2, "Database error: db_primitive_insert_game() 3 (%s)", insert_buf );
                signal_error = true;
                return false;   // not inserted, error
            }
        }
    }
    
    // Save the position hashes
    for( int i=0; i<nbr_moves; i++ )
    {
        uint64_t hash64 = *hashes++;
        int hash32 = (int)(hash64);
        int table_nbr = ((int)(hash64>>32))&(NBR_BUCKETS-1);
        std::vector<std::pair<int,int>> *bucket = &buckets[table_nbr];
        std::pair<int,int> duo(hash32,game_id);
        bucket->push_back(duo);
        int count = bucket->size();
        if( count >= PURGE_QUOTA )
        {
            bool ok = purge_bucket(table_nbr);
            if( !ok )
            {
                signal_error = true;
                return false;   // not inserted, error
            }
        }
    }
    game_id++;
    return true;    // inserted, no error
}



