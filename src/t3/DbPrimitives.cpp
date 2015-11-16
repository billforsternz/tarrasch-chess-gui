/****************************************************************************
 *  Low level database maintenance functions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include "thc.h"
#include "sqlite3.h"
#include "CompressMoves.h"
#include "DbPrimitives.h"
#include "DebugPrintf.h"
static void purge_bucket(int bucket_idx);
static void purge_buckets();
#define NBR_BUCKETS 4096
#define PURGE_QUOTA 10000


// Handle for database connection
static sqlite3 *handle;
static int game_id;


// Returns bool ok
bool db_primitive_open( const char *db_file )
{
    cprintf( "db_primitive_open()\n" );
    
    // Try to create the database. If it doesnt exist, it would be created
    //  pass a pointer to the pointer to sqlite3, in short sqlite3**
    int retval = sqlite3_open(db_file,&handle);
    
    // If connection failed, handle returns NULL
    if(retval)
    {
        cprintf("DATABASE CONNECTION FAILED\n");
        return false;
    }
    cprintf("Connection successful\n");
    return true;
}

// Returns bool ok
bool db_primitive_create_tables()
{
    DebugPrintfTime turn_on_time_reporting;
    cprintf( "db_primitive_create_tables()\n" );
    
    // Create tables if not existing
    cprintf( "Create games table");
    int retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS description (description TEXT, version INTEGER)",0,0,0);
    if( retval )
    {
        cprintf("sqlite3_exec(CREATE description) FAILED\n");
        return false;
    }
    retval = sqlite3_exec(handle,"INSERT INTO description VALUES('Description of database goes here', 1 )",0,0,0);
    if( retval )
    {
        cprintf("sqlite3_exec(INSERT description) FAILED\n");
        return false;
    }
    retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS games (game_id INTEGER, game_hash INT8 UNIQUE, white TEXT, black TEXT, event TEXT, site TEXT, result TEXT, date TEXT, white_elo TEXT, black_elo TEXT, moves BLOB)",0,0,0);
    if( retval )
    {
        cprintf("sqlite3_exec(CREATE games) FAILED\n");
        return false;
    }
    retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS games_duplicates (game_id INTEGER, game_hash INT8, white TEXT, black TEXT, event TEXT, site TEXT, result TEXT, date TEXT, white_elo TEXT, black_elo TEXT, moves BLOB)",0,0,0);
    if( retval )
    {
        cprintf("sqlite3_exec(CREATE games_duplicates) FAILED\n");
        return false;
    }
    cprintf( "Create positions tables");
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
		if( i<20 || i%10 == 0)
			cprintf( "Creating bucket %d\n", i);
        char buf[200];
        sprintf( buf, "CREATE TABLE IF NOT EXISTS positions_%d (game_id INTEGER, position_hash INTEGER)", i );
        retval = sqlite3_exec(handle,buf,0,0,0);
        if( retval )
        {
            cprintf("sqlite3_exec(CREATE positions_%d) FAILED\n",i);
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
        cprintf("sqlite3_exec(DELETE games) Failed\n");
        return false;
    }
    retval = sqlite3_exec( handle, "DELETE FROM positions",0,0,0);
    if( retval )
    {
        cprintf("sqlite3_exec(DELETE positions) Failed\n");
        return false;
    }
    return true;
}


// Returns bool ok
bool db_primitive_transaction_begin()
{
    char *errmsg=NULL;
    char buf[80];
    sprintf( buf, "BEGIN TRANSACTION" );
    int retval = sqlite3_exec( handle, buf,0,0,&errmsg);
    bool ok = (retval==0);
    if( retval && errmsg )
        cprintf("sqlite3_exec(BEGIN TRANSACTION) FAILED [%s]\n", errmsg );
	else if (retval )
		cprintf("sqlite3_exec(BEGIN TRANSACTION) FAILED\n" );
    return ok;
}


// Returns bool ok
bool db_primitive_transaction_end()
{
    char *errmsg;
    char buf[80];
    purge_buckets();
    sprintf( buf, "COMMIT TRANSACTION" );
    int retval = sqlite3_exec( handle, buf,0,0,&errmsg);
    bool ok = (retval==0);
    if( retval )
        cprintf("sqlite3_exec(COMMIT TRANSACTION) FAILED %s\n", errmsg );
    return ok;
}

// Returns bool ok
bool db_primitive_create_indexes()
{
    DebugPrintfTime turn_on_time_reporting;
    purge_buckets();
    cprintf( "Create games index\n");
    int retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_games ON games(game_id)",0,0,0);
    bool ok = (retval==0);
    cprintf( "Create games index end\n");
    if( retval )
    {
        cprintf("sqlite3_exec(CREATE INDEX games) FAILED\n");
        return false;
    }
    return true;
}

// Returns bool ok
bool db_primitive_create_extra_indexes()
{
    bool ok = false;
    DebugPrintfTime turn_on_time_reporting;
    cprintf( "Create games(white) index\n");
    int retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_white ON games(white)",0,0,0);
    if( retval )
    {
        cprintf( "sqlite3_exec() FAILED 1\n" );
        return false;
    }
    cprintf( "Create games(black) index\n");
    retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_black ON games(black)",0,0,0);
    if( retval )
    {
        cprintf( "sqlite3_exec() FAILED 1\n" );
        return false;
    }
    //int retval = sqlite3_exec(handle,"DROP INDEX idx_white",0,0,0);
    cprintf( "Create games(white) index end\n");
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
        char buf[200];
        cprintf( "Create idx%d begin\n", i );
        sprintf( buf, "CREATE INDEX IF NOT EXISTS idx%d ON positions_%d(position_hash,game_id)",i,i);
        retval = sqlite3_exec(handle,buf,0,0,0);
        cprintf( "Create idx%d end\n", i );
        if( retval )
        {
            cprintf( "sqlite3_exec FAILED 3\n" );
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
    purge_buckets();

    // Close the handle to free memory
    sqlite3_close(handle);
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
        cprintf("SELECTING DATA FROM DB FAILED\n");
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
            cprintf("SOME ERROR ENCOUNTERED\n");
            return 0;
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
        else if( c!=' ' && c!='.' && c!=',' && !isalnum(c) )
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


std::vector<std::pair<int,int>> buckets[NBR_BUCKETS];
static void purge_buckets()
{
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
        purge_bucket(i);
    }
}

static void purge_bucket( int bucket_idx )
{
    char *errmsg;
    char insert_buf[2000];
    std::vector<std::pair<int,int>> *bucket = &buckets[bucket_idx];
    int count = bucket->size();
    if( count > 0 )
    {
        std::sort( bucket->begin(), bucket->end() );
        char buf[100];
        //cprintf( "purge bucket %d, %d items", bucket_idx, count );
        for( int j=0; j<count; j++ )
        {
            std::pair<int,int> duo = (*bucket)[j];
            sprintf( insert_buf, "INSERT INTO positions_%d VALUES(%d,%d)", bucket_idx, duo.second, duo.first );
            int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
            if( retval )
            {
                cprintf("sqlite3_exec(INSERT 3) FAILED %s\n", errmsg );
            }
        }
        bucket->clear();
    }
}


// Return true if this game already in database
bool db_primitive_check_for_duplicate( uint64_t game_hash, const char *white, const char *black, const char *result, const char *blob_moves )
{
    for( int table_idx=0; table_idx<2; table_idx++ )
    {
        // select matching rows from the table
        char buf[200];
        sqlite3_stmt *stmt;    // A prepared statement for fetching tables
        sprintf( buf, "SELECT white, black, result, moves from %s WHERE game_hash=%lld", table_idx==0?"games":"games_duplicates", game_hash );
        const char *errmsg;
        int retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, &errmsg );
        if( retval )
        {
            cprintf("DB FAIL db_primitive_check_for_duplicate() 1 %s (%s)\n", errmsg, buf );
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
                cprintf("DB FAIL db_primitive_check_for_duplicate() 2\n");
                return false;
            }
        }
    }
    return false;   // return false unless we explicitly DID find a duplicate
}

// returns bool inserted
bool db_primitive_insert_game( const char *white, const char *black, const char *event, const char *site, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  )
{
    //cprintf( "db_primitive_gameover(%s,%s)\n", white, black );
    //uint32_t *move_ptr = (uint32_t *)moves;
    char *errmsg;
    char insert_buf[2000];
    char blob_txt_buf[4000];    // about 500 moves each
    char blob_buf[2000];
    char white_buf[200];
    char black_buf[200];
    char event_buf[200];
    char site_buf[200];
    if( nbr_moves < 3 )    // skip 'games' with zero, one or two moves
        return false;
    if( white_elo && black_elo )
    {
        int elo_w = atoi(white_elo);
        int elo_b = atoi(black_elo);
        if( 0<elo_w && elo_w<2000 && 0<elo_b && elo_b<2000 )
            return false;   // if any elo information, need at least one good player
    }
    /* if( date && strlen(date)>=4 )
    {
        char buf[5];
        memcpy(buf,date,4);
        buf[4] = '\0';
        int year = atoi(buf);
        if( year >= 1980 )
        {
            if( white_elo && strlen(white_elo)>=4  && black_elo && strlen(black_elo)>=4 )
            {
                int elo_w = atoi(white_elo);
                int elo_b = atoi(black_elo);
                if( elo_w<2000 && elo_b<2000 )
                    return;
            }
        }                                    
    }  */
    bool alphaWhite = sanitise( white,  white_buf, sizeof(white_buf) );
    bool alphaBlack = sanitise( black,  black_buf, sizeof(black_buf) );
    if( !alphaWhite || !alphaBlack )    // skip games with no names
        return false;
    sanitise( event,  event_buf, sizeof(event_buf) );
    sanitise( site, site_buf,  sizeof(site_buf) );
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
    sprintf( insert_buf, "INSERT INTO games VALUES(%d,%lld,'%s','%s','%s','%s','%s','%s','%s','%s',X'%s')",
                        game_id, game_hash, white_buf, black_buf, event_buf, site_buf, result,
                        date, white_elo, black_elo, blob_txt_buf );
    int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
    if( retval )
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
        if( !strstr(buf,"unique") )
        {
            cprintf("DB_FAIL db_primitive_insert_game() 1 %s (%s)\n", errmsg, insert_buf );
            return false;
        }
        //cprintf("Non unique game hash %s (%s)\n", errmsg, insert_buf );

        // Check whether a duplicate of existing game
        bool is_duplicate = db_primitive_check_for_duplicate( game_hash, white_buf, black_buf, result, blob_buf );
        if( is_duplicate )
        {
            return false;  // ZOMBIE bug fix - return here, don't add bogus moves and increment game count
        }
        // If not, put it into the database
        else
        {
            sprintf( insert_buf, "INSERT INTO games VALUES(%d,NULL,'%s','%s','%s','%s','%s','%s','%s','%s',X'%s')",
                    game_id, white_buf, black_buf, event_buf, site_buf, result,
                    date, white_elo, black_elo, blob_txt_buf );
            int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
            if( retval )
            {
                cprintf("DB_FAIL db_primitive_insert_game() 2 %s (%s)\n", errmsg, insert_buf );
                return false;
            }
            sprintf( insert_buf, "INSERT INTO games_duplicates VALUES(%d,%lld,'%s','%s','%s','%s','%s','%s','%s','%s',X'%s')",
                    game_id, game_hash, white_buf, black_buf, event_buf, site_buf, result,
                    date, white_elo, black_elo, blob_txt_buf );
            retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
            if( retval )
            {
                cprintf("DB_FAIL db_primitive_insert_game() 3 %s (%s)\n", errmsg, insert_buf );
                return false;
            }
        }
    }
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
            purge_bucket(table_nbr);
    }
    game_id++;
    return true;
}



