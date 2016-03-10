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
#include "wx/filename.h"
#include "Appdefs.h"
#include "thc.h"
#include "sqlite3.h"
#include "CompressMoves.h"
#include "PgnRead.h"
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

// Build the default database if .pgn exists and .tdb doesn't
void db_primitive_build_default_database( const char *db_file_name )
{
    bool build = true;
    std::string error_msg;
    std::string db(db_file_name);
    std::string pgn;
    int offset = db.find(".tdb");
    if( offset == std::string::npos )
    {
        cprintf( "Database file extension not found in %s?!\n", db_file_name );
        build = false;
    }
    else
    {
        pgn = db.substr(0,offset) + ".pgn";
        cprintf( "Possible pgn file to build default database is %s\n", pgn.c_str() );
        wxFileName wxpgn( pgn.c_str() );
        wxFileName wxdb( db_file_name );
        bool exists_p = wxpgn.FileExists();;
        bool exists_d = wxdb.FileExists() && (wxdb.GetSize()>0);
        build = exists_p && !exists_d;      // pgn present, but not database
        cprintf( "exists_p=%s, exists_d=%s, build=%s\n", exists_p?"true":"false", exists_d?"true":"false", build?"true":"false" );
    }
    if( build )
    {
        db_primitive_error_msg();   // clear error reporting mechanism
        bool ok = db_primitive_open( db_file_name, true );
        if( ok )
            ok = db_primitive_transaction_begin(NULL);
        if( ok )
            ok = db_primitive_create_tables(true);
        if( ok )
            ok = (db_primitive_count_games()>=0); // to set game_id to zero
        FILE *ifile = fopen( pgn.c_str(), "rt" );
        if( !ifile )
        {
            error_msg = "Cannot open ";
            error_msg += pgn;
            ok = false;
        }
        else
        {
            std::string title( "Creating database");
            std::string desc("Reading file ");
            wxFileName wxpgn( pgn.c_str() );
            desc += wxpgn.GetFullName().c_str();
            ProgressBar progress_bar( title, desc, true, NULL, ifile );
            PgnRead *pr = new PgnRead('A',&progress_bar);
            bool aborted = pr->Process(ifile);
            if( aborted )
            {
                error_msg = db_primitive_error_msg();
                if( error_msg == "" )
                    error_msg = "cancel";
                ok = false;
            }
            delete pr;
            fclose(ifile);
        }
        if( ok )
            ok = db_primitive_flush();
        if( ok )
            ok = db_primitive_transaction_end();
        if( ok )
            ok = db_primitive_create_indexes(true);
        if( ok )
            db_primitive_close();
        if( !ok )
        {
            if( error_msg == "" )
                error_msg = db_primitive_error_msg();
            if( error_msg == "cancel" )
                error_msg = "Database creation cancelled";
            wxMessageBox( error_msg.c_str(), "Database creation failed", wxOK|wxICON_ERROR );
            db_primitive_close();
            unlink(db.c_str());
        }
    }
}

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
    cprintf( "db_primitive_transaction_end() begin\n" );
    ProgressBar prog( "Creating database", "Completing save", true, window_parent );
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
    cprintf( "db_primitive_transaction_end() end\n" );
    return ok;
}

// Returns bool ok
bool db_primitive_create_tables( bool create_tiny_db )
{
    ProgressBar prog( create_tiny_db?"Creating database, step 1 of 3":"Creating database, step 1 of 4", "Creating database tables", true, window_parent );
    
    cprintf( "db_primitive_create_tables()\n" );
    
    // Create tables if not existing
    cprintf( "Create games table");
    int retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS description (description TEXT, version INTEGER)",0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(CREATE description) FAILED";
        return false;
    }
    if( prog.Percent(create_tiny_db?20:2) )
    {
        error_msg = "cancel";
        return false;
    }
    char buf[200];
    sprintf( buf,"INSERT INTO description VALUES('Description of database, not currently used for anything, goes here', %d )", 
        create_tiny_db ? DATABASE_VERSION_NUMBER_TINY : DATABASE_VERSION_NUMBER_NORMAL );
    retval = sqlite3_exec(handle,buf,0,0,0);
    if( retval )
    {
        error_msg = "Database error: sqlite3_exec(INSERT description) FAILED";
        return false;
    }
    if( prog.Percent(create_tiny_db?40:4) )
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
    if( prog.Percent(create_tiny_db?60:6) )
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
    if( prog.Percent(create_tiny_db?80:8) )
    {
        error_msg = "cancel";
        return false;
    }
    if( !create_tiny_db )
    {
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
    }
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
bool db_primitive_create_indexes( bool create_tiny_db )
{
    ProgressBar prog( create_tiny_db?"Creating database, step 3 of 3":"Creating database, step 4 of 4", create_tiny_db?"Creating indexes":"Indexing positions", true, window_parent );
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
    if( prog.Percent(create_tiny_db?40:1) )
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
    if( prog.Percent(create_tiny_db?70:2) )
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
    if( prog.Percent(create_tiny_db?100:3) )
    {
        error_msg = "cancel";
        return false;
    }
    if( !create_tiny_db )
    {
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


int db_primitive_get_database_version()
{
    int version = 0;
    sqlite3_stmt *stmt;
    int retval;
    retval = sqlite3_prepare_v2( handle, "SELECT description,version from description", -1, &stmt, 0 );
    if( retval )
    {
        error_msg = "Database error: SELECT description,version FROM DB FAILED";
        return -1;
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
                    version = (int)sqlite3_column_int(stmt,col);
                    cprintf( "Version integer = %d\n", version );
                    break;
                }
            }
        }
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
    return version;
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
        ProgressBar pb( create_mode ? "Creating database, step 3 of 4" : "Adding games to database, step 2 of 2", "Writing to database" , true, window_parent );
        for( int i=0; ok && i<NBR_BUCKETS; i++ )
        {
            ok = purge_bucket(i);
            if( ok )
            {
                if( i%10 == 0 )
                    ; //cprintf( "Purging bucket %d\n", i );
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

// Split out printable, 2 character or more uppercased tokens from input string
static void Split( const char *in, std::vector<std::string> &out )
{
    bool in_word=true;
    out.clear();
    std::string token;
    while( *in )
    {
        char c = *in++;
        bool delimit = (c<=' ' || c==',' || c=='.' || c==':' || c==';');
        if( !in_word )
        {
            if( !delimit )
            {
                in_word = true;
                token = c;
            }
        }
        else
        {
            if( delimit )
            {
                in_word = false;
                if( token.length() > 1 )
                    out.push_back(token);
            }
            else
            {
                if( isalpha(c) )
                    c = toupper(c);
                token += c;
            }
        }
    }
    if( in_word && token.length() > 1 )
        out.push_back(token);
}

static bool IsPlayerMatch( const char *player, std::vector<std::string> &tokens )
{
    std::vector<std::string> tokens2;
    Split( player, tokens2 );
    int len=tokens.size();
    int len2=tokens2.size();
    for( int i=0; i<len; i++ )
    {
        for( int j=0; j<len2; j++ )
        {
            if( tokens[i] == tokens2[j] )
                return true;
        }
    }
    return false;
}

static void DateParse( const char *date, int &yyyy, int &mm, int &dd )
{
    int state=0;
    int n=0, y=0, m=0, d=0;
    while( *date && state<3 )
    {
        char c = *date++;
        if( '0'<=c && c<='9' )
            n = n*10 + c-'0';
        else if( c=='.' || c=='-' || c==' ' || c==':' )
        {
            switch( state )
            {
                case 0: y = n;  break;
                case 1: m = n;  break;
                case 2: d = n;  break;
            }
            state++;
            n = 0;
        }
        else
            break;
    }
    switch( state )
    {
        case 0: y = n;  break;
        case 1: m = n;  break;
        case 2: d = n;  break;
    }
    yyyy = (1<=y && y<=3000) ? y : 0;
    mm   = (1<=m && m<=12)   ? m : 0;
    dd   = (1<=d && d<=31)   ? d : 0;
}

static bool IsDateMatch( const char *date, int yyyy, int mm, int dd )
{
    int y, m, d;
    DateParse( date, y, m, d );
    bool match = (yyyy == y);   // must match
    if( match )
    {
        match = (mm==m || mm==0 || m==0);    // if month unspecified still matches 
        if( match )
            match = (dd==d || dd==0 || d==0);    // if day unspecified still matches
    }
    return match;
}

// Return true if this game already in database
bool db_primitive_check_for_duplicate( bool &signal_error, uint64_t game_hash, const char *white, const char *black, const char *date, const char *result, const char *blob_moves )
{
    static int max_so_far;
    signal_error = false;
    bool duplicate_found=false;
    std::vector<std::string> white_tokens;
    std::vector<std::string> black_tokens;
    int yyyy=0,mm,dd;
    sqlite3_stmt *stmt=NULL;    // A prepared statement for fetching tables
    for( int table_idx=0; !duplicate_found && !signal_error && table_idx<2; table_idx++ )
    {
        // select matching rows from the table
        char buf[200];
        sprintf( buf, "SELECT white, black, date, result, moves from %s WHERE game_hash=%lld", table_idx==0?"games":"games_duplicates", game_hash );
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
        int cols = sqlite3_column_count(stmt);
        while(!duplicate_found)  // Read one row from games, potentially many rows in games_duplicates
        {
            retval = sqlite3_step(stmt);
            if( retval == SQLITE_ROW )
            {
                bool white_match=false;
                bool black_match=false;
                bool date_match=false;
                bool result_match=false;
                bool moves_match=false;
                bool imperfect_match=false;
                const char *val_w, *val_b, *val_d;
                for( int col=0; col<cols; col++ )
                {
                    if( col == 0 )
                    {
                        val_w = (const char*)sqlite3_column_text(stmt,col);
                        if( val_w )
                        {
                            white_match = (0 == strcmp(white,val_w));
                            #define SMART_DUP_DETECT
                            #ifdef SMART_DUP_DETECT
                            if( !white_match )
                            {
                                if( white_tokens.size() == 0 )
                                    Split(white,white_tokens);
                                white_match = IsPlayerMatch(val_w,white_tokens);
                                if( white_match )
                                    imperfect_match = true;
                            }
                            #endif
                        }
                    }
                    else if( col == 1 )
                    {
                        val_b = (const char*)sqlite3_column_text(stmt,col);
                        if( val_b )
                        {
                            black_match = (0 == strcmp(black,val_b));
                            #ifdef SMART_DUP_DETECT
                            if( !black_match )
                            {
                                if( black_tokens.size() == 0 )
                                    Split(black,black_tokens);
                                black_match = IsPlayerMatch(val_b,black_tokens);
                                if( black_match )
                                    imperfect_match = true;
                            }
                            #endif
                        }
                    }
                    else if( col == 2 )
                    {
                        val_d = (const char*)sqlite3_column_text(stmt,col);
                        if( val_d )
                        {
                            date_match = (0 == strcmp(date,val_d));
                            #ifdef SMART_DUP_DETECT
                            if( !date_match )
                            {
                                if( yyyy == 0 )
                                    DateParse(date,yyyy,mm,dd);
                                date_match = IsDateMatch(val_d,yyyy,mm,dd);
                                if( date_match )
                                    imperfect_match = true;
                            }
                            #endif
                        }
                    }
                    else if( col == 3 )
                    {
                        const char *val = (const char*)sqlite3_column_text(stmt,col);
                        if( val )
                        {
                            result_match = (0 == strcmp(result,val));
                        }
                    }
                    else if( col == 4 )
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
                const char *dump_game = NULL;
                bool dup1 = (white_match && black_match && /*date_match &&*/ result_match && moves_match);
                bool dup2 = (white_match && black_match && date_match && result_match && moves_match);
                bool dup3 = moves_match;
                if( dup1 && !dup2 )
                    dump_game = "Surprisingly date matters";
                if( dup1 )
                {
                    duplicate_found = true;
                    //cprintf( "Duplicate game %s-%s %s %s found\n", white,black,date,result);
                    if( imperfect_match )
                    {
                        //cprintf( "Imperfect match %s-%s %s %s found\n", white,black,date,result);
                        //cprintf( "                %s-%s %s %s\n", val_w,val_b,val_d,result);
                    }
                }
                else if( strlen(blob_moves) > max_so_far  && moves_match )
                {
                    dump_game = "Longest dual game so far";
                    max_so_far = strlen(blob_moves);
                }
                if( dump_game )
                {
                    cprintf( "%s %s-%s %s %s\n", dump_game, white,black,date,result);
                    cprintf( " and  %s-%s %s %s\n", val_w,val_b,val_d,result);
                    {
                        CompressMoves press;
                        std::string blob(blob_moves);
                        std::vector<thc::Move> v = press.Uncompress(blob);
                        thc::ChessRules cr;
                        std::string txt;
                        for( int i=0; i<v.size(); i++ )
                        {
                            thc::Move mv = v[i];
                            std::string move_txt = mv.NaturalOut( &cr );
                            char buf[80];
                            if( cr.white )
                                sprintf( buf, "%s%d. %s", i==0?"": i%8==0 ? "\n" : " ", cr.full_move_count, move_txt.c_str() );
                            else
                                sprintf( buf, "%s%s", i==0?"": i%8==0 ? "\n" : " ", move_txt.c_str() );
                            txt += std::string(buf);
                            cr.PlayMove(mv);
                        }
                        cprintf( "%s\n", txt.c_str() );
                    }
                }
            }
            else if( retval == SQLITE_DONE )
            {
                // All rows finished
                break;
            }
            else
            {
                // Some error encountered
                error_msg = "Database error: db_primitive_check_for_duplicate() 2";
                signal_error = true;
                break;
            }
        }
    }
    if( stmt != NULL )
    {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }
    return duplicate_found;   // return false unless we explicitly DID find a duplicate
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
bool db_primitive_insert_game( bool &signal_error, bool create_tiny_db, const char *white, const char *black, const char *event, const char *site, const char *round, const char *result,
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
        bool is_duplicate = db_primitive_check_for_duplicate( signal_error, game_hash, white_buf, black_buf, date, result, blob_buf );
        if( signal_error )
            return false;   // not inserted, error
        
        // A duplicate game is simply discarded
        if( is_duplicate )
        {
            //cprintf( "***  DUPLICATE GAME DISCARDED\n" );
            // ZOMBIE bug fix - return here, don't add bogus moves and increment game count
            //db_temporary_hack( white, black, event, site, round, result,
            //                        date, white_elo, black_elo, eco,
            //                        nbr_moves, moves  );
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
    if( !create_tiny_db )
    {
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
    }
    game_id++;
    return true;    // inserted, no error
}




