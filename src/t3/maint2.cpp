//
//  db.cpp
//  readpgnfast
//
//  Created by Bill Forster on 6/05/13.
//  Copyright (c) 2013 Bill Forster. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "thc.h"
#include "sqlite3.h"
#include "CompressMoves.h"
#include "maint2.h"
static void purge_bucket(int bucket_idx);
static void purge_buckets();
#define NBR_BUCKETS 4096
#define PURGE_QUOTA 10000

static int report( const char * txt )
{
    static time_t before;
    struct tm *timeinfo;
    time_t rawtime;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char buf[80];
    strcpy( buf, asctime(timeinfo) );
    char *p = strchr(buf,'\n');
    if( p )
        *p = '\0';
    printf( "%s: %s\n", buf, txt );
    int expired = (rawtime-before);
    before = rawtime;
    return expired;
}


// A test program
int db_maint_random_test_program()
{
    // Create an int variable for storing the return code for each call
    int retval;
    
    // A prepered statement for fetching tables
    sqlite3_stmt *stmt;
    
    // Create a handle for database connection, create a pointer to sqlite3
    sqlite3 *handle;
    
    // try to create the database. If it doesnt exist, it would be created
    // pass a pointer to the pointer to sqlite3, in short sqlite3**
    retval = sqlite3_open(DB_FILE,&handle);

    // If connection failed, handle returns NULL
    if(retval)
    {
        printf("Database connection failed\n");
        return -1;
    }
    printf("Connection successful\n");
    
    // Create the SQL query for creating a table
    const char *create_table = "CREATE TABLE IF NOT EXISTS positions (game_id INTEGER, position_hash INTEGER)";
    
    // Execute the query for creating the table
    retval = sqlite3_exec(handle,create_table,0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(CREATE) Failed\n");
        return -1;
    }
    
    
    // Delete previous data
    retval = sqlite3_exec( handle, "DELETE FROM positions",0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(DELETE) Failed\n");
        return -1;
    }
    
    // Insert first row and second row
    char *errmsg;
    retval = sqlite3_exec( handle, "INSERT INTO positions VALUES(2,33334444)",0,0,&errmsg);
    if( retval )
    {
        printf("sqlite3_exec(INSERT 1) Failed %s\n", errmsg );
        return -1;
    }
    
    retval = sqlite3_exec( handle, "INSERT INTO positions VALUES(2,4444)",0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(INSERT 2) Failed\n");
        return -1;
    }
    
 
    // select those rows from the table
    retval = sqlite3_prepare_v2( handle, "SELECT * from positions", -1, &stmt, 0 );
    if( retval )
    {
        printf("Selecting data from DB Failed\n");
        return -1;
    }
    
    // Read the number of rows fetched
    int cols = sqlite3_column_count(stmt);
    
    while(1)
    {
        // fetch a row's status
        retval = sqlite3_step(stmt);
        
        if(retval == SQLITE_ROW)
        {
            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for(int col=0 ; col<cols;col++)
            {
                int val = sqlite3_column_int(stmt,col);
                printf("%s = 0x%08x\t",sqlite3_column_name(stmt,col),val);
            }
            printf("\n");
        }
        else if(retval == SQLITE_DONE)
        {
            // All rows finished
            printf("All rows fetched\n");
            break;
        }
        else
        {
            // Some error encountered
            printf("Some error encountered\n");
            return -1;
        }
    }
    
    // Close the handle to free memory
    sqlite3_close(handle);
    return 0;
}

// Handle for database connection
static sqlite3 *handle;
static int game_id;

void db_maint_open()
{
    printf( "db_maint_open()\n" );

    // Try to create the database. If it doesnt exist, it would be created
    //  pass a pointer to the pointer to sqlite3, in short sqlite3**
    int retval = sqlite3_open(DB_FILE,&handle);
    
    // If connection failed, handle returns NULL
    if(retval)
    {
        printf("DATABASE CONNECTION FAILED\n");
        return;
    }
    printf("Connection successful\n");
    
    // Create tables if not existing
    report( "Create games table");
    retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS games (game_id INTEGER, white TEXT, black TEXT, result TEXT, moves BLOB)",0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(CREATE games) FAILED\n");
        return;
    }
    report( "Create positions table");
    retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS positions (game_id INTEGER, position_hash INTEGER)",0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(CREATE positions) FAILED\n");
        return;
    }
}

void db_maint_open_multi()
{
    printf( "db_maint_open_multi()\n" );
    
    // Try to create the database. If it doesnt exist, it would be created
    //  pass a pointer to the pointer to sqlite3, in short sqlite3**
    int retval = sqlite3_open(DB_FILE,&handle);
    
    // If connection failed, handle returns NULL
    if(retval)
    {
        printf("DATABASE CONNECTION FAILED\n");
        return;
    }
    printf("Connection successful\n");
    
    // Create tables if not existing
    report( "Create games table");
    retval = sqlite3_exec(handle,"CREATE TABLE IF NOT EXISTS games (game_id INTEGER, white TEXT, black TEXT, result TEXT, moves BLOB)",0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(CREATE games) FAILED\n");
        return;
    }
    report( "Create positions tables");
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
        char buf[200];
        sprintf( buf, "CREATE TABLE IF NOT EXISTS positions_%d (game_id INTEGER, position_hash INTEGER)", i );
        retval = sqlite3_exec(handle,buf,0,0,0);
        if( retval )
        {
            printf("sqlite3_exec(CREATE positions_%d) FAILED\n",i);
            return;
        }
    }
    report( "Create positions tables end");
}

void db_maint_delete_previous_data()
{
    int retval = sqlite3_exec( handle, "DELETE FROM games",0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(DELETE games) Failed\n");
        return;
    }
    retval = sqlite3_exec( handle, "DELETE FROM positions",0,0,0);
    if( retval )
    {
        printf("sqlite3_exec(DELETE positions) Failed\n");
        return;
    }
}


void db_maint_transaction_begin()
{
    char *errmsg;
    char buf[80];
    sprintf( buf, "BEGIN TRANSACTION" );
    int retval = sqlite3_exec( handle, buf,0,0,&errmsg);
    if( retval )
        printf("sqlite3_exec(BEGIN TRANSACTION) FAILED %s\n", errmsg );
}


void db_maint_transaction_end()
{
    char *errmsg;
    char buf[80];
    purge_buckets();
    sprintf( buf, "COMMIT TRANSACTION" );
    int retval = sqlite3_exec( handle, buf,0,0,&errmsg);
    if( retval )
        printf("sqlite3_exec(COMMIT TRANSACTION) FAILED %s\n", errmsg );
}


void db_maint_create_indexes()
{
    char buf[200];
    purge_buckets();
    report( "Create positions index" );
    sprintf( buf, "CREATE INDEX IF NOT EXISTS idx2 ON positions(position_hash)");
    int retval = sqlite3_exec(handle,buf,0,0,0);
    report( "Create positions index end" );
    if( retval )
    {
        printf( "sqlite3_exec(%s) FAILED\n", buf );
        return;
    }
    report( "Create games index");
    retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx3 ON games(game_id)",0,0,0);
    report( "Create games index end");
    if( retval )
    {
        printf("sqlite3_exec(CREATE INDEX games) FAILED\n");
    }
}


void db_maint_create_indexes_multi()
{
    purge_buckets();
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
        char buf[200];
        sprintf( buf, "Create positions_%d index", i );
        report( buf );
        sprintf( buf, "CREATE INDEX IF NOT EXISTS idx%d ON positions_%d(position_hash)",i,i);
        int retval = sqlite3_exec(handle,buf,0,0,0);
        if( retval )
        {
            printf( "sqlite3_exec(%s) FAILED\n", buf );
            return;
        }
    }
    report( "Create games index");
    int retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_games ON games(game_id)",0,0,0);
    report( "Create games index end");
    if( retval )
    {
        printf("sqlite3_exec(CREATE INDEX games) FAILED\n");
    }
}

void db_maint_create_extra_indexes()
{
    report( "Create games(white) index");
    int retval = sqlite3_exec(handle,"CREATE INDEX IF NOT EXISTS idx_white ON games(white)",0,0,0);
    if( retval )
    {
        printf( "sqlite3_exec() FAILED 1\n" );
        return;
    }
    //int retval = sqlite3_exec(handle,"DROP INDEX idx_white",0,0,0);
    report( "Create games(white) index end");
    for( int i=0; i<NBR_BUCKETS; i++ )
    {
        char buf[200];
        sprintf( buf, "DROP INDEX idx%d", i );
        report( buf );
        retval = sqlite3_exec(handle,buf,0,0,0);
        if( retval )
        {
            printf( "sqlite3_exec() FAILED 2\n" );
            return;
        }
        sprintf( buf, "Create idx%d begin", i );
        report( buf );
        sprintf( buf, "CREATE INDEX IF NOT EXISTS idx%d ON positions_%d(position_hash,game_id)",i,i);
        retval = sqlite3_exec(handle,buf,0,0,0);
        sprintf( buf, "Create idx%d end", i );
        report( buf );
        if( retval )
        {
            printf( "sqlite3_exec FAILED 3\n" );
            return;
        }
    }
}

void db_maint_speed_tests()
{
    printf( "db_maint_speed_tests()\n" );
    
    // Try to create the database. If it doesnt exist, it would be created
    //  pass a pointer to the pointer to sqlite3, in short sqlite3**
    int retval = sqlite3_open(DB_FILE,&handle);
    
    // If connection failed, handle returns NULL
    if(retval)
    {
        printf("DATABASE CONNECTION FAILED\n");
        return;
    }
    printf("Connection successful\n");
    
    const char *magnus = "SELECT COUNT(*) from games, positions_2569 WHERE games.white = 'Carlsen, Magnus' "
                         "AND positions_2569.position_hash=44349918 AND games.game_id = positions_2569.game_id";
    const char *stamp =  "SELECT COUNT(*) from games, positions_3386 WHERE positions_3386.position_hash=2007903353 "
                         "AND games.game_id = positions_3386.game_id";
    const char *sicilian="SELECT COUNT(*) from games, positions_391 WHERE positions_391.position_hash=1592470315 "
                         "AND games.game_id = positions_391.game_id";

    char buf[1000];
    sqlite3_stmt *stmt;    // A prepared statement for fetching tables
    printf( "Database is %s\n", DB_FILE );
    int results[5][3];
    time_t start_time;
    time ( &start_time );
    for( int i=0; i<5*3; i++ )
    {
        const char *query, *txt;
        switch(i%3)
        {
            case 0: query = magnus;     txt="Magnus"; break;
            case 1: query = stamp;      txt="Stamp"; break;
            case 2: query = sicilian;   txt="Sicilian"; break;
        }
        sprintf( buf, "Test %d, %s; begin", i/3+1, txt );
        report( buf );
        retval = sqlite3_prepare_v2( handle, query, -1, &stmt, 0 );
        if( retval )
        {
            printf("SELECTING DATA FROM DB FAILED 1\n");
        }
        retval = sqlite3_step(stmt);
        if( retval != SQLITE_ROW )
            printf("SELECTING DATA FROM DB FAILED 2\n");
        else
        {
            const char *val = (const char*)sqlite3_column_text(stmt,0);
            printf("Game count=%d\n", atoi(val) );
        }
        sprintf( buf, "Test %d, %s; end", i/3+1, txt );
        int expired = report( buf );
        results[i/3][i%3] = expired;
        sqlite3_finalize(stmt);
    }
    
    for( int i=0; i<5*3; i++ )
    {
        int expired = results[i%5][i/5];
        if( i%5 == 0 )
        {
            const char *txt;
            switch(i/5)
            {
                case 0: txt="Magnus:";   break;
                case 1: txt="Stamp:";    break;
                case 2: txt="Sicilian:"; break;
            }
            printf( "%-10s", txt );
        }
        char buf[100];
        sprintf( buf, "%d:%02d", expired/60, expired%60 );
        printf( "%8s", buf );
        if( (i+1)%5 == 0 )
            printf( "\n" );
        else
            printf( "," );
    }
    time_t end_time;
    time ( &end_time );
    int total_elapsed = (end_time-start_time);
    printf( "Total elapsed: %d:%02d\n", total_elapsed/60, total_elapsed%60 );
    
    // Close the handle to free memory
    sqlite3_close(handle);
}


void db_maint_close()
{
    purge_buckets();

    // Close the handle to free memory
    sqlite3_close(handle);
}


// get one game
static void dump_game( FILE *f, int game_id )
{
    // Get white player
    char buf[100];
    sqlite3_stmt *stmt;    // A prepared statement for fetching tables
    int retval;
    sprintf( buf, "SELECT white from games WHERE game_id=%d", game_id );
    retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, 0 );
    if( retval )
    {
        printf("SELECTING DATA FROM DB FAILED\n");
    }
    retval = sqlite3_step(stmt);
    if(retval == SQLITE_ROW)
    {
        const char *val = (const char*)sqlite3_column_text(stmt,0);
        fprintf(f,"White:%s\n",val);
    }

    // Get black player
    sprintf( buf, "SELECT black from games WHERE game_id=%d", game_id );
    retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, 0 );
    if( retval )
    {
        printf("SELECTING DATA FROM DB FAILED\n");
    }
    retval = sqlite3_step(stmt);
    if(retval == SQLITE_ROW)
    {
        const char *val = (const char*)sqlite3_column_text(stmt,0);
        fprintf(f,"Black:%s\n",val);
    }
    
    // Get result
    sprintf( buf, "SELECT result from games WHERE game_id=%d", game_id );
    retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, 0 );
    if( retval )
    {
        printf("SELECTING DATA FROM DB FAILED\n");
    }
    retval = sqlite3_step(stmt);
    if(retval == SQLITE_ROW)
    {
        const char *val = (const char*)sqlite3_column_text(stmt,0);
        fprintf(f,"Result:%s\n",val);
    }
    
    // Get moves
    sprintf( buf, "SELECT moves from games WHERE game_id=%d", game_id );
    retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, 0 );
    if( retval )
    {
        printf("SELECTING DATA FROM DB FAILED\n");
    }
    retval = sqlite3_step(stmt);
    if(retval == SQLITE_ROW)
    {
        int len = sqlite3_column_bytes(stmt,0);
        fprintf(f,"Move len = %d\n",len);
        const char *blob = (const char*)sqlite3_column_blob(stmt,0);
        fprintf(f,"Moves:\n");
        //for( int i=0; i<len; i++ )
        //    fprintf(f," %02x", *blob++ & 0x0ff );
        CompressMoves press;
        for( int nbr=0; nbr<len;  )
        {
            thc::ChessRules cr = press.cr;
            thc::Move mv;
            int nbr_used = press.decompress_move( blob, mv );
            if( nbr_used == 0 )
                break;
            std::string s = mv.NaturalOut(&cr);
            fprintf(f," %s",s.c_str());
            blob += nbr_used;
            nbr += nbr_used;
        }
        fprintf(f,"\n");
    }
    
    /*
     *pnBlob = sqlite3_column_bytes(pStmt, 0);
     *pzBlob = (unsigned char *)malloc(*pnBlob);
     memcpy(*pzBlob, sqlite3_column_blob(pStmt, 0), *pnBlob);
    
    thc::ChessRules cr;
    for( int i=0; i<moves.size(); i++ )
    {
        int move = moves[i];
        thc::Move mv;
        memcpy(&mv,&move,sizeof(thc::Move));
        std::string s = mv.NaturalOut(&cr);
        cr.PlayMove(mv);
        fprintf(f," %s",s.c_str());
    }
    fprintf(f,"\n");
     */
    
}

// A real database test
void db_maint_show_games( bool connect )
{
    printf( "db_maint_show_games()\n" );
    int retval;
    if( connect )
    {
        // Try to create the database. If it doesnt exist, it would be created
        //  pass a pointer to the pointer to sqlite3, in short sqlite3**
        retval = sqlite3_open(DB_FILE,&handle);
        
        // If connection failed, handle returns NULL
        if(retval)
        {
            printf("DATABASE CONNECTION FAILED\n");
            return;
        }
        printf("Connection successful\n");
        
        
    }
    
    thc::ChessRules cr;
    //cr.Forsyth("r1bqk2r/ppp1bppp/2n1pn2/3p4/Q1PP4/P3PN2/1P1N1PPP/R1B1KB1R b KQkq - 0 7");   // Bogo indian position after 8 moves or so 46 games
    cr.Forsyth("rnbqk2r/pppp1ppp/4pn2/8/1bPP4/2N5/PP2PPPP/R1BQKBNR w KQkq - 2 4");          // Nimzo-Indian start position 24000 games
    uint32_t hash = cr.HashCalculate();
    

    // select matching rows from the table
    char buf[100];
    sqlite3_stmt *stmt;    // A prepared statement for fetching tables
    sprintf( buf, "SELECT * from positions WHERE position_hash=%d", hash );
    retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, 0 );
    if( retval )
    {
        printf("SELECTING DATA FROM DB FAILED\n");
    }
    
    // Read the number of rows fetched
    int cols = sqlite3_column_count(stmt);
    std::vector<int> games;

    report( "Fetching games begin");
    while(1)
    {
        // fetch a row's status
        retval = sqlite3_step(stmt);
        
        if(retval == SQLITE_ROW)
        {
            // SQLITE_ROW means fetched a row
            
            // sqlite3_column_text returns a const void* , typecast it to const char*
            for(int col=0 ; col<cols;col++)
            {
                const char *val = (const char*)sqlite3_column_text(stmt,col);
                //printf("%s:%s\t",sqlite3_column_name(stmt,col),val);
                if( col == 0 )
                {
                    int game_id = atoi(val);
                    games.push_back(game_id);
                }
            }
            //printf("\n");
        }
        else if(retval == SQLITE_DONE)
        {
            // All rows finished
            break;
        }
        else
        {
            // Some error encountered
            printf("SOME ERROR ENCOUNTERED\n");
        }
    }
    report("Fetching games end");

    FILE *pgn_out = fopen("/Users/billforster/Documents/games.txt","wt");
    report("Dumping games begin");
    if( pgn_out )
    {
        for( int i=0; i<games.size(); i++ )
        {
            char buf[100];
            if( i%100 == 0 || i==games.size()-1 )
            {
                sprintf( buf,"Game %d",i+1);
                report(buf);
            }
            dump_game( pgn_out, games[i] );
        }
        fclose(pgn_out);
    }
    report("Dumping games end");
    
    
    // Close the handle to free memory
    sqlite3_close(handle);
}



int db_maint_count_games()
{
    int game_count=0;
    
    // select matching rows from the table
    char buf[100];
    sqlite3_stmt *stmt;    // A prepared statement for fetching tables
    sprintf( buf, "SELECT COUNT(*) from games" );
    int retval = sqlite3_prepare_v2( handle, buf, -1, &stmt, 0 );
    if( retval )
    {
        printf("SELECTING DATA FROM DB FAILED\n");
        return 0;
    }

    // Read the number of rows fetched
    int cols = sqlite3_column_count(stmt);

    printf( "Get games count begin\n");
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
                //printf("%s:%s\t",sqlite3_column_name(gbl_stmt,col),val);
                if( col == 0 )
                {
                    //int game_id = atoi(val);
                    //game_ids.push_back(game_id);
                    game_count = atoi(val);
                    printf( "Game count = %d\n", game_count );
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
            printf("SOME ERROR ENCOUNTERED\n");
        }
    }
    printf("Get games count end\n");
    game_id = game_count;
    return game_count;
}


void db_maint_insert_game( const char *white, const char *black, const char *event, const char *site, const char *result, int nbr_moves, thc::Move *moves, uint32_t *hashes  )
{
    //printf( "db_maint_gameover(%s,%s)\n", white, black );
    //uint32_t *move_ptr = (uint32_t *)moves;
    char *errmsg;
    char insert_buf[2000];
    char blob_buf[1000];    // about 500 moves each
    char white_buf[200];
    char black_buf[200];
    strcpy( white_buf, white );
    char *s=white_buf;
    while( *s )
    {
        if( !isascii(*s) )
            *s = '_';
        else if( *s!=' ' && *s!='.' && *s!=',' && !isalnum(*s) )
            *s = '_';
        s++;
    }
    strcpy( black_buf, black );
    s=black_buf;
    while( *s )
    {
        if( !isascii(*s) )
            *s = '_';
        else if( *s!=' ' && *s!='.' && *s!=',' && !isalnum(*s) )
            *s = '_';
        s++;
    }
    CompressMoves press;
    char *put = blob_buf;
    for( int i=0; i<nbr_moves && put<blob_buf+sizeof(blob_buf)-10; i++ )
    {
        char buf[2];
        thc::Move mv = moves[i];
        int nbr = press.compress_move( mv, buf );
     /* std::string s = mv.TerseOut();
        if( nbr == 1 )
            printf( "compress temp> %s -> %02x\n", s.c_str(), buf[0]&0xff );
        else if( nbr == 2 )
            printf( "compress temp> %s -> %02x,%02x\n", s.c_str(), buf[0]&0xff, buf[1]&0xff );
        else
            printf( "nbr == 0 ?\n"); */
        if( nbr == 0 )
            break;
        for( int j=0; j<nbr && j<2; j++ )
        {
            char c = buf[j];
            char hi = (c>>4)&0x0f;
            *put++ = (hi>=10 ? hi-10+'A' : hi+'0');
            char lo = c&0x0f;
            *put++ = (lo>=10 ? lo-10+'A' : lo+'0');
        }
        //printf( "%s %s\n", (i<nbr_moves?"true":"false"), (put<blob_buf+sizeof(blob_buf-10) ? "true" ? "false") );
    }
    *put = '\0';
    //printf( "%d %s\n", nbr_moves, blob_buf );
    sprintf( insert_buf, "INSERT INTO games VALUES(%d,'%s','%s','%s',X'%s')", game_id, white_buf, black_buf, result, blob_buf );
    //printf( "%s\n", insert_buf );
    int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
    if( retval )
    {
        printf("sqlite3_exec(INSERT 1) FAILED %s\n", errmsg );
    }
    for( int i=0; i<nbr_moves; i++ )
    {
        sprintf( insert_buf, "INSERT INTO positions VALUES(%d,%d)", game_id, (int)*hashes++ );
        retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
        if( retval )
        {
            printf("sqlite3_exec(INSERT 3) FAILED %s\n", errmsg );
        }
    }
    game_id++;
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
        sprintf( buf, "purge bucket %d, %d items", bucket_idx, count );
        report( buf );
        for( int j=0; j<count; j++ )
        {
            std::pair<int,int> duo = (*bucket)[j];
            sprintf( insert_buf, "INSERT INTO positions_%d VALUES(%d,%d)", bucket_idx, duo.second, duo.first );
            int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
            if( retval )
            {
                printf("sqlite3_exec(INSERT 3) FAILED %s\n", errmsg );
                return;
            }
        }
        bucket->clear();
    }
}

void db_maint_insert_game_multi( const char *white, const char *black, const char *event, const char *site, const char *result, int nbr_moves, thc::Move *moves, uint64_t *hashes  )
{
    //printf( "db_maint_gameover(%s,%s)\n", white, black );
    //uint32_t *move_ptr = (uint32_t *)moves;
    char *errmsg;
    char insert_buf[2000];
    char blob_buf[1000];    // about 500 moves each
    char white_buf[200];
    char black_buf[200];
    strcpy( white_buf, white );
    char *s=white_buf;
    while( *s )
    {
        if( !isascii(*s) )
            *s = '_';
        else if( *s!=' ' && *s!='.' && *s!=',' && !isalnum(*s) )
            *s = '_';
        s++;
    }
    strcpy( black_buf, black );
    s=black_buf;
    while( *s )
    {
        if( !isascii(*s) )
            *s = '_';
        else if( *s!=' ' && *s!='.' && *s!=',' && !isalnum(*s) )
            *s = '_';
        s++;
    }
    CompressMoves press;
    char *put = blob_buf;
    for( int i=0; i<nbr_moves && put<blob_buf+sizeof(blob_buf)-10; i++ )
    {
        char buf[2];
        thc::Move mv = moves[i];
        int nbr = press.compress_move( mv, buf );
        /* std::string s = mv.TerseOut();
         if( nbr == 1 )
         printf( "compress temp> %s -> %02x\n", s.c_str(), buf[0]&0xff );
         else if( nbr == 2 )
         printf( "compress temp> %s -> %02x,%02x\n", s.c_str(), buf[0]&0xff, buf[1]&0xff );
         else
         printf( "nbr == 0 ?\n"); */
        if( nbr == 0 )
            break;
        for( int j=0; j<nbr && j<2; j++ )
        {
            char c = buf[j];
            char hi = (c>>4)&0x0f;
            *put++ = (hi>=10 ? hi-10+'A' : hi+'0');
            char lo = c&0x0f;
            *put++ = (lo>=10 ? lo-10+'A' : lo+'0');
        }
        //printf( "%s %s\n", (i<nbr_moves?"true":"false"), (put<blob_buf+sizeof(blob_buf-10) ? "true" ? "false") );
    }
    *put = '\0';
    //printf( "%d %s\n", nbr_moves, blob_buf );
    sprintf( insert_buf, "INSERT INTO games VALUES(%d,'%s','%s','%s',X'%s')", game_id, white_buf, black_buf, result, blob_buf );
    //printf( "%s\n", insert_buf );
    int retval = sqlite3_exec( handle, insert_buf,0,0,&errmsg);
    if( retval )
    {
        printf("sqlite3_exec(INSERT 1) FAILED %s\n", errmsg );
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
}




