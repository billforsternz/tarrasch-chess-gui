/****************************************************************************
 *  Database maintenance miscellany - mainly development test only
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <map>
#include "thc.h"
#include "DebugPrintf.h"
#include "PgnRead.h"
#include "CompressMoves.h"
#include "DbPrimitives.h"
#include "DbMaintenance.h"

//
//  This file has evolved into an InternalDatabaseTest facility (mainly) and should be
//  renamed and reorganised (slightly) to reflect this
//

//-- Temp - hardwire .pgn file and database name
#ifdef THC_WINDOWS
#define PGN_FILE        "/Users/Bill/Documents/T3Database/giant123.pgn"
#define PGN_OUT_FILE    "/Users/Bill/Documents/T3Database/x.pgn"
#define QGN_FILE        "/Users/Bill/Documents/T3Database/x.qgn"
#else
#define PGN_FILE        "/Users/billforster/Documents/ChessDatabases/giant123.pgn"
#define PGN_OUT_FILE    "/Users/billforster/Documents/ChessDatabases/twic_minimal_overlap_2.pgn"
#define QGN_FILE        "/Users/billforster/Documents/ChessDatabases/twic_minimal_overlap.qgn"
#endif

static FILE *ifile;
static FILE *ofile;
static FILE *logfile;

static void decompress_game( const char *compressed_header, const char *compressed_moves );
static void game_to_qgn_file( const char *event, const char *site, const char *date, const char *round,
                             const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                             int nbr_moves, thc::Move *moves, uint64_t *hashes );
static int  verify_compression_algorithm( int nbr_moves, thc::Move *moves );
static void players_database_begin();
static void players_database_end();
static int  players_database_total;
static void players_database( const char *event, const char *site, const char *date, const char *round,
                       const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                       int nbr_moves, thc::Move *moves, uint64_t *hashes );

void db_maintenance_decompress_pgn()
{
    static char header_buf[2000];
    static char moves_buf[2000];
    bool ok = false;
    ifile = fopen( QGN_FILE, "rb" );
    if( !ifile )
        cprintf( "Cannot open %s\n", QGN_FILE );
    else
    {
        ofile = fopen( PGN_OUT_FILE, "wb" );
        if( ofile )
            ok = true;
        else
            cprintf( "Cannot open %s\n", PGN_OUT_FILE );
    }
    int nbr_games=0;
    while( ok )
    {
        if( !fgets( header_buf, sizeof(header_buf)-5, ifile ) )
            break;
        if( !fgets( moves_buf, sizeof(moves_buf)-5, ifile ) )
            break;
        size_t len = strlen(header_buf);
        if( len>0 && header_buf[len-1]=='\n' )
            header_buf[len-1] = '\0';
        len = strlen(moves_buf);
        if( len>0 && moves_buf[len-1]=='\n' )
            moves_buf[len-1] = '\0';
        decompress_game( header_buf, moves_buf );
        nbr_games++;
        if(
           ((nbr_games%1000) == 0 ) /* ||
                                      (
                                      (nbr_games < 100) &&
                                      ((nbr_games%10) == 0 )
                                      ) */
           )
            cprintf( "%d games\n", nbr_games );
    }
    cprintf( "%d games\n", nbr_games );
    if( ifile )
        fclose(ifile);
    if( ofile )
        fclose(ofile);
    if( logfile )
        fclose(logfile);
}

void db_maintenance_verify_compression()
{
    ifile = fopen( PGN_FILE, "rt" );
    if( !ifile )
        cprintf( "Cannot open %s\n", PGN_FILE );
    else
    {
        // Verify compression
        void CompressMovesDiagBegin();
        void CompressMovesDiagEnd();
        DebugPrintfTime turn_on_time_reporting;
        CompressMovesDiagBegin();
        PgnRead *pgn = new PgnRead('V');
        cprintf( "Begin\n" );
        pgn->Process(ifile);
        cprintf( "End\n" );
        CompressMovesDiagEnd();
    }
    if( ifile )
        fclose(ifile);
    if( ofile )
        fclose(ofile);
    if( logfile )
        fclose(logfile);
}

void db_maintenance_create_player_database()
{
    //std::string input("C:\\Users\\Bill\\Downloads\\millionbase\\millionbase-2.22");
    std::string input("C:\\Users\\Bill\\Downloads\\millionbase\\twic-2006-2015");
    players_database_begin();
    std::string input_file = input+".pgn";
    std::string output_file = input+"-output.pgn";
    std::string log_file = input+".log";
    ifile = fopen( input_file.c_str() , "rt" );
    if( !ifile )
        cprintf( "Cannot open %s\n", input_file.c_str()  );
    else
    {
        ofile = fopen( output_file.c_str(), "wt" );
        if( !ofile )
            cprintf( "Cannot open %s\n", output_file.c_str() );
        else
        {
            logfile = fopen( log_file.c_str(), "wt" );
            if( !logfile )
                cprintf( "Cannot open %s\n", log_file.c_str() );
            else
            {
                PgnRead *pgn = new PgnRead('G');
                pgn->Process(ifile);
            }
        }
    }
    players_database_end();
    if( ifile )
        fclose(ifile);
    if( ofile )
        fclose(ofile);
    if( logfile )
        fclose(logfile);
}

void db_maintenance_compress_pgn()
{
    ifile = fopen( PGN_FILE, "rt" );
    if( !ifile )
        cprintf( "Cannot open %s\n", PGN_FILE );
    else
    {
        ofile = fopen( QGN_FILE, "wb" );
        if( !ofile )
            cprintf( "Cannot open %s\n", QGN_FILE );
        else
        {
            PgnRead *pgn = new PgnRead('P');
            pgn->Process(ifile);
        }
    }
    if( ifile )
        fclose(ifile);
    if( ofile )
        fclose(ofile);
}

void db_maintenance_create_or_append_to_database(  const char *db_filename, const char *pgn_filename )
{
    ifile = fopen( pgn_filename , "rt" );
    if( !ifile )
        cprintf( "Cannot open %s\n", pgn_filename );
    else
    {
        DebugPrintfTime turn_on_time_display;
        PgnRead *pgn = new PgnRead('A');
        db_primitive_open( db_filename );
        db_primitive_transaction_begin(NULL);
        db_primitive_create_tables();
        db_primitive_count_games();
        pgn->Process(ifile);
        cprintf( "Before db_primitive_transaction_end()\n");
        db_primitive_transaction_end();
        cprintf( "Before db_primitive_close()\n");
        db_primitive_close();
        cprintf( "After db_primitive_close()\n");
    }
    if( ifile )
        fclose(ifile);
    if( ofile )
        fclose(ofile);
}

void db_utility_test();
void db_maintenance_create_indexes(const char *db_filename )
{
    //db_utility_test();
    db_primitive_open(db_filename);
    db_primitive_transaction_begin(NULL);
    db_primitive_create_indexes();
    db_primitive_transaction_end();
    db_primitive_close();
}

extern void pgn_read_hook( const char *white, const char *black, const char *event, const char *site, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  );


bool hook_gameover( char callback_code, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves, uint64_t *hashes )
{
    bool aborted = false;
    static int counter;
    if( (++counter % 100000) == 0 )
        cprintf( "%d games processed so far\n", counter );
    switch( callback_code )
    {
        // Compress
        case 'P': game_to_qgn_file( event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves, hashes );  break;

        // Great players database
        case 'G': players_database( event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves, hashes );  break;
            
        // Append
        case 'A':
        {
            bool signal_error;
            db_primitive_insert_game( signal_error, white, black, event, site, round, result, date, white_elo, black_elo, eco, nbr_moves, moves, hashes );
            if( signal_error )
                return true;
            break;
        }
            
        // Read one game
        case 'R': pgn_read_hook( white, black, event, site, result, date, white_elo, black_elo, nbr_moves, moves, hashes ); return true;

        // Verify
        case 'V':
        {
            int is_interesting = verify_compression_algorithm( nbr_moves, moves );
            extern int nbr_games_with_two_queens;
            extern int nbr_games_with_promotions;
            extern int nbr_games_with_slow_mode;
            if( is_interesting & 4096 )
            {
                nbr_games_with_two_queens++;
                is_interesting &= (~4096);
            }
            if( is_interesting&512 )
            {
                nbr_games_with_promotions++;
                is_interesting &= (~512);
            }
            if( is_interesting )
            {
                char buf[200];
                buf[0] = '\0';
                if( is_interesting&0x1f)
                    nbr_games_with_slow_mode++;
                if(is_interesting&1)
                    strcat(buf,"Dark Bishop ");
                if(is_interesting&2)
                    strcat(buf,"Light Bishop ");
                if(is_interesting&4)
                    strcat(buf,"Knight ");
                if(is_interesting&8)
                    strcat(buf,"Rook ");
                if(is_interesting&16)
                    strcat(buf,"Queen ");
                if(is_interesting&32)
                    strcat(buf,"Unusual instance of 3 pawn swaps ");
                if(is_interesting&64)
                    strcat(buf,"New record nbr slow moves - queen ");
                if(is_interesting&128)
                    strcat(buf,"New record nbr slow moves - not queen ");
                if(is_interesting&256)
                    strcat(buf,"Verification error ");
                if(is_interesting&1024)
                    strcat(buf,"Ten or more slow moves - queen ");
                if(is_interesting&2048)
                    strcat(buf,"Ten or more slow moves - not queen ");
                cprintf( "Adding game with issues; %s\n", buf );
                FILE *f = fopen( (is_interesting&16) ? "TwoOrMoreQueens.pgn" : "TooManyKnightsEtc.pgn" , "at" );
                if( f )
                {
                    fprintf( f, "[Event \"%s\"]\n", event );
                    fprintf( f, "[Site \"%s - %s\"]\n", buf, site );
                    fprintf( f, "[Date \"%s\"]\n", date );
                    fprintf( f, "[Round \"%s\"]\n", round );
                    fprintf( f, "[White \"%s\"]\n", white );
                    fprintf( f, "[Black \"%s\"]\n", black );
                    fprintf( f, "[Result \"%s\"]\n", result );
                    fprintf( f, "\n" );
                    thc::ChessRules cr;
                    bool end=true;
                    for( int i=0; i<nbr_moves; i++ )
                    {
                        bool start = ((i%10) == 0);
                        if( !start )
                            fprintf( f, " " );
                        if( cr.white )
                            fprintf( f, "%d. ", cr.full_move_count );
                        thc::Move mv = moves[i];
                        std::string s = mv.NaturalOut(&cr);
                        fprintf( f, "%s", s.c_str() );
                        end = (((i+1)%10) == 0);
                        if( end )
                            fprintf( f, "\n" );
                        cr.PlayMove(mv);
                    }
                    if( !end )
                        fprintf( f, " " );
                    fprintf( f, "%s\n\n", result );
                    fclose(f);
                }
            }
            break;
        }
    }
    return aborted;
}


static void players_database( const char *event, const char *site, const char *date, const char *round,
                       const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                       int nbr_moves, thc::Move *moves, uint64_t *hashes );


// Compress
static void game_to_qgn_file( const char *event, const char *site, const char *date, const char *round,
                       const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                       int nbr_moves, thc::Move *moves, uint64_t *hashes )

{
    static char last_event    [ FIELD_BUFLEN + 10];
    static char last_site     [ FIELD_BUFLEN + 10];
    static char last_date     [ FIELD_BUFLEN + 10];
    static char last_round    [ FIELD_BUFLEN + 10];
    static char last_white    [ FIELD_BUFLEN + 10];
    static char last_black    [ FIELD_BUFLEN + 10];
    static char last_result   [ FIELD_BUFLEN + 10];
    static char last_white_elo[ FIELD_BUFLEN + 10];
    static char last_black_elo[ FIELD_BUFLEN + 10];
    static char last_eco      [ FIELD_BUFLEN + 10];
    static char buf[ 10 * sizeof(last_eco) ];
    char *dst = buf;
    char *last;
    const char *src;
    for( int i=0; i<10; i++ )
    {
        switch(i)
        {
            case 0: src=white;      last=last_white;        break;
            case 1: src=white_elo;  last=last_white_elo;    break;
            case 2: src=black;      last=last_black;        break;
            case 3: src=black_elo;  last=last_black_elo;    break;
            case 4: src=result;     last=last_result;       break;
            case 5: src=date;       last=last_date;         break;
            case 6: src=event;      last=last_event;        break;
            case 7: src=site;       last=last_site;         break;
            case 8: src=round;      last=last_round;        break;
            case 9: src=eco;        last=last_eco;          break;
        }
        if( *src )
        {
            bool same               = (0==strcmp(src,last));
            bool same_as_black_name = (i==0 && (0==strcmp(src,last_black))); 
            bool same_as_black_elo  = (i==1 && (0==strcmp(src,last_black_elo))); 
            bool same_as_white_name = (i==2 && (0==strcmp(src,last_white))); 
            bool same_as_white_elo  = (i==3 && (0==strcmp(src,last_white_elo)));
            if( i>3 && !same ) 
                strcpy( last, src );
            else if( i == 3 )
            {
                strcpy( last_white,      white      );
                strcpy( last_white_elo,  white_elo  );
                strcpy( last_black,      black      );
                strcpy( last_black_elo,  black_elo  );
            }
            bool result             = (i==4);
            if( result && 0==strcmp(src,"1-0") ) 
                *dst++ = '1';
            else if( result && 0==strcmp(src,"0-1") ) 
                *dst++ = '0';
            else if( result && 0==strcmp(src,"1/2-1/2") ) 
                *dst++ = '=';
            else if( result )
                *dst++ = '*';
            else if( same )
                *dst++ = '^';
            else if( same_as_black_name )
                *dst++ = '>';
            else if( same_as_black_elo )
                *dst++ = '>';
            else if( same_as_white_name )
                *dst++ = '<';
            else if( same_as_white_elo )
                *dst++ = '<';
            else
            {
                while( *src )
                {
                    if( *src == '|' )
                        *dst++ = '-';
                    else
                        *dst++ = *src;
                    src++;
                }
            }
        }
        if( i+1 < 10 )
            *dst++ = '|';
    }
    *dst = '\0';
    fputs( buf,  ofile );    
    fputs( "\n", ofile );    
    CompressMoves press;
    std::vector<thc::Move> v(moves,moves+nbr_moves);
    std::string compressed = press.Compress(v); 
    fputs( compressed.c_str(), ofile );    
    fputs( "\n", ofile );    
}


static void decompress_game( const char *compressed_header, const char *compressed_moves )
{
    static char event         [ FIELD_BUFLEN + 10];
    static char site          [ FIELD_BUFLEN + 10];
    static char date          [ FIELD_BUFLEN + 10];
    static char round         [ FIELD_BUFLEN + 10];
    static char white         [ FIELD_BUFLEN + 10];
    static char black         [ FIELD_BUFLEN + 10];
    static char result        [ FIELD_BUFLEN + 10];
    static char white_elo     [ FIELD_BUFLEN + 10];
    static char black_elo     [ FIELD_BUFLEN + 10];
    static char eco           [ FIELD_BUFLEN + 10];
    static char last_event    [ FIELD_BUFLEN + 10];
    static char last_site     [ FIELD_BUFLEN + 10];
    static char last_date     [ FIELD_BUFLEN + 10];
    static char last_round    [ FIELD_BUFLEN + 10];
    static char last_white    [ FIELD_BUFLEN + 10];
    static char last_black    [ FIELD_BUFLEN + 10];
    static char last_result   [ FIELD_BUFLEN + 10];
    static char last_white_elo[ FIELD_BUFLEN + 10];
    static char last_black_elo[ FIELD_BUFLEN + 10];
    static char last_eco      [ FIELD_BUFLEN + 10];
    char *dst, *last;
    const char *src = compressed_header;
    for( int i=0; i<10; i++ )
    {
        switch(i)
        {
            case 0: dst=white;      last=last_white;        break;
            case 1: dst=white_elo;  last=last_white_elo;    break;
            case 2: dst=black;      last=last_black;        break;
            case 3: dst=black_elo;  last=last_black_elo;    break;
            case 4: dst=result;     last=last_result;       break;
            case 5: dst=date;       last=last_date;         break;
            case 6: dst=event;      last=last_event;        break;
            case 7: dst=site;       last=last_site;         break;
            case 8: dst=round;      last=last_round;        break;
            case 9: dst=eco;        last=last_eco;          break;
        }
        bool same               = (0==strcmp(src,"^")) || (0==memcmp(src,"^|",2));
        bool same_as_black_name = (i==0 && (0==memcmp(src,">|",2))); 
        bool same_as_black_elo  = (i==1 && (0==memcmp(src,">|",2))); 
        bool same_as_white_name = (i==2 && (0==memcmp(src,"<|",2))); 
        bool same_as_white_elo  = (i==3 && (0==memcmp(src,"<|",2)));
        bool result             = (i==4);
        if( same )
            strcpy( dst, last );
        else if( same_as_black_name )
            strcpy( dst, last_black );
        else if( same_as_black_elo )
            strcpy( dst, last_black_elo );
        else if( same_as_white_name )
            strcpy( dst, last_white );
        else if( same_as_white_elo )
            strcpy( dst, last_white_elo );
        else if( result && 0==memcmp(src,"1|",2) ) 
            strcpy( dst, "1-0" );
        else if( result && 0==memcmp(src,"0|",2) ) 
            strcpy( dst, "0-1" );
        else if( result && 0==memcmp(src,"=|",2) ) 
            strcpy( dst, "1/2-1/2" );
        else if( result )
            strcpy( dst, "*" );
        else
        {
            char *p = dst;
            while( *src != '|' && *src != '\0' && p < dst+sizeof(eco)-5 )
                *p++ = *src++;
            *p = '\0';
        }
        if( i > 3 ) 
            strcpy( last, dst );
        else if( i == 3 )
        {
            strcpy( last_white,      white      );
            strcpy( last_white_elo,  white_elo  );
            strcpy( last_black,      black      );
            strcpy( last_black_elo,  black_elo  );
        }
        while( *src != '|' && *src != '\0' )
            src++;
        if( *src == '|' )
            src++;
    }
    fprintf( ofile, "[Event \"%s\"]\n",     event );
    fprintf( ofile, "[Site \"%s\"]\n",      site );
    fprintf( ofile, "[Date \"%s\"]\n",      date );
    fprintf( ofile, "[Round \"%s\"]\n",     round );
    fprintf( ofile, "[White \"%s\"]\n",     white );
    fprintf( ofile, "[Black \"%s\"]\n",     black );
    fprintf( ofile, "[Result \"%s\"]\n",    result );
    fprintf( ofile, "[WhiteElo \"%s\"]\n",  white_elo );
    fprintf( ofile, "[BlackElo \"%s\"]\n",  black_elo );
    fprintf( ofile, "[ECO \"%s\"]\n\n",     eco );

    CompressMoves press;
    std::string compressed( compressed_moves, compressed_moves+strlen(compressed_moves) );
    std::vector<thc::Move> moves = press.Uncompress(compressed);
    int nbr_moves = moves.size();
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
}

static int verify_compression_algorithm( int nbr_moves, thc::Move *moves )
{
    #if 0
    return 0;
    #else
    int is_interesting = 0;
    extern int max_nbr_slow_moves_other;
    extern int max_nbr_slow_moves_queen;
    std::vector<thc::Move> moves_in(moves,moves+nbr_moves);
    CompressMoves press2;
    std::string compressed = press2.Compress(moves_in);
    CompressMoves press;
    std::vector<thc::Move> unpacked = press.Uncompress(compressed);
    is_interesting = press.is_interesting | press2.is_interesting;
    if( is_interesting & 0x0f )
    {
        if( press.nbr_slow_moves>0 && press.nbr_slow_moves>=max_nbr_slow_moves_other )
        {
            max_nbr_slow_moves_other = press.nbr_slow_moves;
            is_interesting |= 128;
        }
        else if( press.nbr_slow_moves >= 10 )
        {
            is_interesting |= 2048;
        }
    }
    else
    {
        if( press.nbr_slow_moves>0 && press.nbr_slow_moves>=max_nbr_slow_moves_queen )
        {
            max_nbr_slow_moves_queen = press.nbr_slow_moves;
            is_interesting |= 64;
        }
        else if( press.nbr_slow_moves >= 10 )
        {
            is_interesting |= 1024;
        }
    }
    bool match = true;
    if( nbr_moves )
        match = (0 == memcmp( &moves_in[0], &unpacked[0], nbr_moves*sizeof(thc::Move)));
    if( !match )
    {
        is_interesting |= 256;
        cprintf( "Boo hoo, doesn't match\n" );
        for( int i=0; i<nbr_moves; i++ )
        {
            bool match = (0 == memcmp( &moves[i], &unpacked[i], sizeof(thc::Move)));
            if( !match )
            {
                cprintf( "Fail at idx %d, %s:%s", i, moves[i].TerseOut().c_str(), unpacked[i].TerseOut().c_str() );
                for( int k=0; k<2; k++ )
                {
                    unsigned char *s = (k==0?(unsigned char *)&moves[i]:(unsigned char *)&unpacked[i]);
                    cprintf("\n");
                    for( int j=0; j<4; j++ )
                    {
                        cprintf( " %02x", *s++ );
                    }
                }
                cprintf("\n");
                break;
            }
        }
    }
    return is_interesting;
    #endif
}


typedef struct
{
    const char *full;
    int year;
    bool long_career;
    const char *name;
    const char *extra;
    int count;
} PLAYER;


PLAYER players[] =
    {
{ "Adams, Michael",           1995, false,  "adams", "michael" },
{ "Agdestein, Simen",         1990, false,  "agdestein", "simen" },
{ "Ahues, Carl",              1930, false,  "ahues", "carl" },
{ "Akopian, Vladimir",        2000, false,  "akopian", "vladimir" },
{ "Alapin, Semyon",           1900, false,  "alapin", "semyon" },
{ "Alatortsev, Vladimir",     1940, false,  "alatortsev", "vladimir" },
{ "Alekhine, Alexander",      1930, false,  "alekhine", "alexander" },
{ "Aleksandrov, Aleksej",     2005, false,  "aleksandrov", "aleksej" },
{ "Almasi, Zoltan",           2005, false,  "almasi", "zoltan" },
{ "Amin, Bassem",             2015, false,  "amin", "bassem" },
{ "Anand, Viswanathan",       2000, false,  "anand", "viswanathan" },
{ "Anderssen, Adolf",         1870, false,  "anderssen", "adolf" },
{ "Andersson, Ulf",           1980, false,  "andersson", "ulf" },
{ "Andreikin, Dmitry",        2015, false,  "andreikin", "dmitry" },
{ "Areshchenko, Alexander",   2015, false,  "areshchenko", "alexander" },
{ "Aronian, Levon",           2005, false,  "aronian", "levon" },
{ "Artemiev, Vladislav",      2015, false,  "artemiev", "vladislav" },
{ "Aseev, Konstantin",        1990, false,  "aseev", "konstantin" },
{ "Asztalos, Lajos",          1920, false,  "asztalos", "lajos" },
{ "Averbakh, Yury",           1950, true,   "averbakh", "yury" },
{ "Averbakh, Yury",           1950, true,   "averbach", "yury" },
{ "Avrukh, Boris",            2005, false,  "avrukh", "boris" },
{ "Azmaiparashvili, Zurab",   1990, false,  "azmaiparashvili", "zurab" },
{ "Bacrot, Etienne",          2005, false,  "bacrot", "etienne" },
{ "Balashov, Yury",           1970, false,  "balashov", "yury" },
{ "Bareev, Evgeny",           1990, false,  "bareev", "evgeny" },
{ "Bauer, Christian",         2005, false,  "bauer", "christian" },
{ "Becker, Albert",           1930, false,  "becker", "albert" },
{ "Belavenets, Sergey",       1940, false,  "belavenets", "sergey" },
{ "Beliavsky, Alexander",     1990, false,  "beliavsky", "alexander" },
{ "Bernstein, Ossip",         1910, true,   "bernstein", "ossip" },
{ "Bird, Henry",              1880, false,  "bird", "henry" },
{ "Blackburne, Joseph",       1890, true,   "blackburne", "joseph" },
{ "Bogoljubow, Efim",         1935, false,  "bogoljubow", "efim" },
{ "Boleslavsky, Isaak",       1950, false,  "boleslavsky", "isaak" },
{ "Bondarevsky, Igor",        1950, false,  "bondarevsky", "igor" },
{ "Bologan, Viktor",          2000, false,  "bologan", "viktor" },
{ "Botvinnik, Mikhail",       1950, true,   "botvinnik", "mikhail" },
{ "Botvinnik, Mikhail",       1950, true,   "botwinnik", "mikhail" },
{ "Breyer, Gyula",            1920, false,  "breyer", "gyula" },
{ "Bronstein, David",         1950, true,   "bronstein", "david" },
{ "Bruzon, Lazaro",           2005, false,  "bruzon", "lazaro" },
{ "Bu Xiangzhi",              2015, false,  "bu xiangzhi", "" },
{ "Bukavshin, Ivan",          2015, false,  "bukavshin", "ivan" },
{ "Burn, Amos",               1890, false,  "burn", "amos" },
{ "Canal, Esteban",           1930, false,  "canal", "esteban" },
{ "Capablanca, Jose",         1920, false,  "capablanca", "jose" },
{ "Carlsen, Magnus",          2015, false,  "carlsen", "magnus" },
{ "Caruana, Fabiano",         2015, false,  "caruana", "fabiano" },
{ "Chandler, Murray",         1990, false,  "chandler", "murray" },
{ "Charousek, Rezso",         1900, false,  "charousek", "rezso" },
{ "Chekhover, Vitaly",        1940, false,  "chekhover", "vitaly" },
{ "Cheparinov, Ivan",         2015, false,  "cheparinov", "ivan" },
{ "Chigorin, Mikhail",        1890, false,  "chigorin", "mikhail" },
{ "Cohn, Wilhelm",            1900, false,  "cohn", "wilhelm" },
{ "Colle, Edgar",             1930, false,  "colle", "edgar" },
{ "Dautov, Rustem",           1990, false,  "dautov", "rustem" },
{ "Denker, Arnold",           1935, true,   "denker", "arnold" },
{ "Ding Liren",               2015, false,  "ding liren", "" },
{ "Dolmatov, Sergey",         1980, false,  "dolmatov", "sergey" },
{ "Dominguez, Lenier",        2005, false,  "dominguez", "lenier" },
{ "Dreev, Alexey",            2000, false,  "dreev", "alexey" },
{ "Dubov, Daniil",            2015, false,  "dubov", "daniil" },
{ "Duda, Jan-Krzysztof",      2015, false,  "duda", "jan-krzysztof" },
{ "Duras, Oldrich",           1910, false,  "duras", "oldrich" },
{ "Dus-Chotimirsky, Fedor",   1910, true,   "dus-chotimirsky", "fedor" },
{ "Dus-Chotimirsky, Fedor",   1910, true,   "dus chotimirsky", "fedor" },
{ "Dvoiris, Semen",           1990, false,  "dvoiris", "semen" },
{ "Ehlvest, Jaan",            1990, false,  "ehlvest", "jaan" },
{ "Eliskases, Erich",         1940, false,  "eliskases", "erich" },
{ "Eljanov, Pavel",           2005, false,  "eljanov", "pavel" },
{ "Engels, Ludwig",           1940, false,  "engels", "ludwig" },
{ "Englisch, Berthold",       1880, false,  "englisch", "berthold" },
{ "Epishin, Vladimir",        2005, false,  "epishin", "vladimir" },
{ "Euwe, Max",                1935, false,  "euwe", "max" },
{ "Fedorov, Alexei",          2000, false,  "fedorov", "alexei" },
{ "Fedoseev, Vladimir",       2015, false,  "fedoseev", "vladimir" },
{ "Filippov, Valerij",        2005, false,  "filippov", "valerij" },
{ "Fine, Reuben",             1940, false,  "fine", "reuben" },
{ "Fischer, Robert J",        1960, false,  "fischer", "bobby" },
{ "Fischer, Robert J",        1960, false,  "fischer", "robert" },
{ "Flohr, Salo",              1935, false,  "flohr", "salo" },
{ "Forgacs, Leo",             1910, false,  "forgacs", "leo" },
{ "Fressinet, Laurent",       2005, false,  "fressinet", "laurent" },
{ "Frydman, Paulin",          1935, false,  "frydman", "paulin" },
{ "Fuderer, Andrija",         1960, false,  "fuderer", "andrija" },
{ "Furman, Semen",            1950, false,  "furman", "semen" },
{ "Gelfand, Boris",           2000, false,  "gelfand", "boris" },
{ "Geller, Efim",             1960, true,   "geller", "efim" },
{ "Georgadze, Tamaz",         1980, false,  "georgadze", "tamaz" },
{ "Georgiev, Kiril",          2000, false,  "georgiev", "kiril" },
{ "Ghaem Maghami, Ehsan",     2005, false,  "ghaem maghami", "ehsan" },
{ "Gharamian, Tigran",        2015, false,  "gharamian", "tigran" },
{ "Gheorghiu, Florin",        1980, false,  "gheorghiu", "florin" },
{ "Gilg, Karl",               1930, false,  "gilg", "karl" },
{ "Gipslis, Aivars",          1970, false,  "gipslis", "aivars" },
{ "Giri, Anish",              2015, false,  "giri", "anish" },
{ "Gligoric, Svetozar",       1960, true,   "gligoric", "svetozar" },
{ "Goldin, Alexander",        2005, false,  "goldin", "alexander" },
{ "Graf, Alexander",          2005, false,  "graf", "alexander" },
{ "Granda Zuniga, Julio",     1990, false,  "granda zuniga", "julio" },
{ "Grischuk, Alexander",      2005, false,  "grischuk", "alexander" },
{ "Grunfeld, Ernst",          1930, false,  "grunfeld", "ernst" },
{ "Gulko, Boris",             1980, false,  "gulko", "boris" },
{ "Gunsberg, Isidor",         1890, false,  "gunsberg", "isidor" },
{ "Gurevich, Mikhail",        2000, false,  "gurevich", "mikhail" },
{ "Gustafsson, Jan",          2005, false,  "gustafsson", "jan" },
{ "Hammer, Jon Ludvig",       2015, false,  "hammer", "jon ludvig" },
{ "Hansen, Curt",             2000, false,  "hansen", "curt" },
{ "Harikrishna, Pentala",     2005, false,  "harikrishna", "pentala" },
{ "Hjartarson, Johann",       2005, false,  "hjartarson", "johann" },
{ "Horowitz, Al",             1940, false,  "horowitz", "al" },
{ "Hort, Vlastimil",          1970, true,   "hort", "vlastimil" },
{ "Hou Yifan",                2015, false,  "hou yifan", "" },
{ "Howell, David",            2015, false,  "howell", "david" },
{ "Hubner, Robert",           1980, true,   "hubner", "robert" },
{ "Hubner, Robert",           1980, true,   "huebner", "robert" },
{ "Illescas, Miguel",         2005, false,  "illescas", "miguel" },
{ "Inarkiev, Ernesto",        2015, false,  "inarkiev", "ernesto" },
{ "Iordachescu, Viorel",      2005, false,  "iordachescu", "viorel" },
{ "Istratescu, Andrei",       2005, false,  "istratescu", "andrei" },
{ "Ivanchuk, Vassily",        2000, false,  "ivanchuk", "vassily" },
{ "Ivanisevic, Ivan",         2015, false,  "ivanisevic", "ivan" },
{ "Ivkov, Borislav",          1960, true,   "ivkov", "borislav" },
{ "Jakovenko, Dmitry",        2015, false,  "jakovenko", "dmitry" },
{ "Janowsky, Dawid",          1910, false,  "janowsky", "dawid" },
{ "Jobava, Baadur",           2005, false,  "jobava", "baadur" },
{ "Johner, Paul",             1920, false,  "johner", "paul" },
{ "Jones, Gawain",            2010, false,  "jones", "gawain" },
{ "Jussupow, Artur",          1990, false,  "jussupow", "artur" },
{ "Kaidanov, Gregory",        2005, false,  "kaidanov", "gregory" },
{ "Kamsky, Gata",             1995, false,  "kamsky", "gata" },
{ "Karjakin, Sergey",         2015, false,  "karjakin", "sergey" },
{ "Karpov, Anatoly",          1980, true,   "karpov", "anatoly" },
{ "Kashdan, Isaac",           1930, false,  "kashdan", "isaac" },
{ "Kasimdzhanov, Rustam",     2005, false,  "kasimdzhanov", "rustam" },
{ "Kasparov, Garry",          1990, false,  "kasparov", "garry" },
{ "Kazhgaleyev, Murtas",      2005, false,  "kazhgaleyev", "murtas" },
{ "Kempinski, Robert",        2005, false,  "kempinski", "robert" },
{ "Keres, Paul",              1950, false,  "keres", "paul" },
{ "Khalifman, Alexander",     2000, false,  "khalifman", "alexander" },
{ "Kharlov, Andrei",          2005, false,  "kharlov", "andrei" },
{ "Khenkin, Igor",            2000, false,  "khenkin", "igor" },
{ "Khismatullin, Denis",      2015, false,  "khismatullin", "denis" },
{ "Kholmov, Ratmir",          1960, true,   "kholmov", "ratmir" },
{ "Kmoch, Hans",              1930, false,  "kmoch", "hans" },
{ "Kobalia, Mikhail",         2005, false,  "kobalia", "mikhail" },
{ "Kolisch, Ignatz",          1880, false,  "kolisch", "ignatz" },
{ "Korchnoi, Viktor",         1980, false,  "korchnoi", "viktor" },
{ "Korobov, Anton",           2015, false,  "korobov", "anton" },
{ "Kostic, Boris",            1930, false,  "kostic", "boris" },
{ "Kotov, Alexander",         1950, false,  "kotov", "alexander" },
{ "Kovalenko, Igor",          2015, false,  "kovalenko", "igor" },
{ "Kramnik, Vladimir",        2005, false,  "kramnik", "vladimir" },
{ "Krasenkow, Michal",        2000, false,  "krasenkow", "michal" },
{ "Krogius, Nikolay",         1970, false,  "krogius", "nikolay" },
{ "Kryvoruchko, Yuriy",       2015, false,  "kryvoruchko", "yuriy" },
{ "Larsen, Bent",             1970, false,  "larsen", "bent" },
{ "Lasker, Emanuel",          1910, true,   "lasker", "emanuel" },
{ "Lautier, Joel",            2000, false,  "lautier", "joel" },
{ "Laznicka, Viktor",         2015, false,  "laznicka", "viktor" },
{ "Le, Quang Liem",           2015, false,  "le", "quang liem" },
{ "Leko, Peter",              2000, false,  "leko", "peter" },
{ "Leonhardt, Paul",          1910, false,  "leonhardt", "paul" },
{ "Levenfish, Grigory",       1920, false,  "levenfish", "grigory" },
{ "Li Chao",                  2015, false,  "li chao", "" },
{ "Li Chao",                  2015, false,  "li chao2", "" },
{ "Li Chao",                  2015, false,  "li chao b", "" },
{ "Liberzon, Vladimir",       1970, false,  "liberzon", "vladimir" },
{ "Lichtenstein, Bernhard",   1930, false,  "lichtenstein", "bernhard" },
{ "Lilienthal, Andor",        1935, false,  "lilienthal", "andor" },
{ "Lipke, Paul",              1900, false,  "lipke", "paul" },
{ "Lipschutz, Samuel",        1890, false,  "lipschutz", "samuel" },
{ "Ljubojevic, Ljubomir",     1980, false,  "ljubojevic", "ljubomir" },
{ "Lokvenc, Josef",           1940, false,  "lokvenc", "josef" },
{ "Lputian, Smbat",           1990, false,  "lputian", "smbat" },
{ "Lutz, Christopher",        2005, false,  "lutz", "christopher" },
{ "Lysyj, Igor",              2015, false,  "lysyj", "igor" },
{ "Macieja, Bartlomiej",      2005, false,  "macieja", "bartlomiej" },
{ "Mackenzie, George",        1880, false,  "mackenzie", "george" },
{ "Makarichev, Sergey",       1980, false,  "makarichev", "sergey" },
{ "Makogonov, Vladimir",      1940, false,  "makogonov", "vladimir" },
{ "Malakhov, Vladimir",       2005, false,  "malakhov", "vladimir" },
{ "Mamedov, Rauf",            2015, false,  "mamedov", "rauf" },
{ "Mamedyarov, Shakhriyar",   2005, false,  "mamedyarov", "shakhriyar" },
{ "Marco, Georg",             1900, false,  "marco", "georg" },
{ "Maroczy, Geza",            1910, false,  "maroczy", "geza" },
{ "Marshall, Frank",          1920, false,  "marshall", "frank" },
{ "Mason, James",             1880, false,  "mason", "james" },
{ "Matanovic, Aleksandar",    1960, false,  "matanovic", "aleksandar" },
{ "Matisons, Hermanis",       1930, false,  "matisons", "hermanis" },
{ "Matlakov, Maxim",          2015, false,  "matlakov", "maxim" },
{ "Matulovic, Milan",         1970, false,  "matulovic", "milan" },
{ "McShane, Luke",            2005, false,  "mcshane", "luke" },
{ "Meier, Georg",             2015, false,  "meier", "georg" },
{ "Mieses, Jacques",          1910, true,   "mieses", "jacques" },
{ "Mikenas, Vladas",          1935, false,  "mikenas", "vladas" },
{ "Milov, Vadim",             2000, false,  "milov", "vadim" },
{ "Miroshnichenko, Evgenij",  2005, false,  "miroshnichenko", "evgenij" },
{ "Moiseenko, Alexander",     2005, false,  "moiseenko", "alexander" },
{ "Morozevich, Alexander",    2000, false,  "morozevich", "alexander" },
{ "Motylev, Alexander",       2005, false,  "motylev", "alexander" },
{ "Movsesian, Sergei",        2000, false,  "movsesian", "sergei" },
{ "Naiditsch, Arkadij",       2005, false,  "naiditsch", "arkadij" },
{ "Najdorf, Miguel",          1950, true,   "najdorf", "miguel" },
{ "Najer, Evgeniy",           2005, false,  "najer", "evgeniy" },
{ "Nakamura, Hikaru",         2005, false,  "nakamura", "hikaru" },
{ "Navara, David",            2005, false,  "navara", "david" },
{ "Negi, Parimarjan",         2015, false,  "negi", "parimarjan" },
{ "Nepomniachtchi, Ian",      2015, false,  "nepomniachtchi", "ian" },
{ "Ni Hua",                   2005, false,  "ni hua", "" },
{ "Nielsen, Peter Heine",     2005, false,  "nielsen", "peter heine" },
{ "Nikolic, Predrag",         1990, false,  "nikolic", "predrag" },
{ "Nimzowitsch, Aron",        1920, false,  "nimzowitsch", "aron" },
{ "Nisipeanu, Liviu-Dieter",  2005, false,  "nisipeanu", "liviu-dieter" },
{ "Nunn, John",               1990, false,  "nunn", "john" },
{ "Olafsson, Fridrik",        1960, false,  "olafsson", "fridrik" },
{ "Onischuk, Alexander",      2000, false,  "onischuk", "alexander" },
{ "Osnos, Viacheslav",        1970, false,  "osnos", "viacheslav" },
{ "Owen, John",               1880, false,  "owen", "john" },
{ "Pachman, Ludek",           1960, false,  "pachman", "ludek" },
{ "Panno, Oscar",             1960, true,   "panno", "oscar" },
{ "Paulsen, Louis",           1880, false,  "paulsen", "louis" },
{ "Perlis, Julius",           1910, false,  "perlis", "julius" },
{ "Petrosian, Tigran",        1960, false,  "petrosian", "tigran" },
{ "Petrovs, Vladimirs",       1940, false,  "petrovs", "vladimirs" },
{ "Piket, Jeroen",            2000, false,  "piket", "jeroen" },
{ "Pillsbury, Harry",         1900, false,  "pillsbury", "harry" },
{ "Pirc, Vasja",              1935, false,  "pirc", "vasja" },
{ "Polgar, Judit",            2000, false,  "polgar", "judit" },
{ "Polugaevsky, Lev",         1970, false,  "polugaevsky", "lev" },
{ "Ponomariov, Ruslan",       2000, false,  "ponomariov", "ruslan" },
{ "Porat, Yosef",             1940, false,  "porat", "yosef" },
{ "Porat, Yosef",             1940, false,  "porath", "yosef" },
{ "Portisch, Lajos",          1970, true,   "portisch", "lajos" },
{ "Postny, Evgeny",           2015, false,  "postny", "evgeny" },
{ "Przepiorka, Dawid",        1920, false,  "przepiorka", "dawid" },
{ "Psakhis, Lev",             1990, false,  "psakhis", "lev" },
{ "Rabinovich, Ilya",         1940, false,  "rabinovich", "ilya" },
{ "Radjabov, Teimour",        2005, false,  "radjabov", "teimour" },
{ "Ragger, Markus",           2015, false,  "ragger", "markus" },
{ "Ragozin, Viacheslav",      1940, false,  "ragozin", "viacheslav" },
{ "Rapport, Richard",         2015, false,  "rapport", "richard" },
{ "Reshevsky, Samuel",        1945, true,   "reshevsky", "samuel" },
{ "Reti, Richard",            1920, false,  "reti", "richard" },
{ "Ribli, Zoltan",            1980, false,  "ribli", "zoltan" },
{ "Richter, Kurt",            1935, false,  "richter", "kurt" },
{ "Robson, Ray",              2015, false,  "robson", "ray" },
{ "Rodshtein, Maxim",         2015, false,  "rodshtein", "maxim" },
{ "Romanishin, Oleg",         1980, false,  "romanishin", "oleg" },
{ "Rosenthal, Samuel",        1880, false,  "rosenthal", "samuel" },
{ "Rubinstein, Akiba",        1910, false,  "rubinstein", "akiba" },
{ "Rublevsky, Sergei",        2000, false,  "rublevsky", "sergei" },
{ "Sadler, Matthew",          2000, false,  "sadler", "matthew" },
{ "Sadvakasov, Darmen",       2005, false,  "sadvakasov", "darmen" },
{ "Safarli, Eltaj",           2015, false,  "safarli", "eltaj" },
{ "Sakaev, Konstantin",       2000, false,  "sakaev", "konstantin" },
{ "Salov, Valery",            1990, false,  "salov", "valery" },
{ "Salwe, Georg",             1910, false,  "salwe", "georg" },
{ "Saemisch, Friedrich",      1930, false,  "samisch", "friedrich" },
{ "Saemisch, Friedrich",      1930, false,  "saemisch", "friedrich" },
{ "Sargissian, Gabriel",      2015, false,  "sargissian", "gabriel" },
{ "Sasikiran, Krishnan",      2005, false,  "sasikiran", "krishnan" },
{ "Savon, Vladimir",          1970, false,  "savon", "vladimir" },
{ "Sax, Gyula",               1980, false,  "sax", "gyula" },
{ "Schallopp, Emil",          1880, false,  "schallopp", "emil" },
{ "Schiffers, Emanuel",       1900, false,  "schiffers", "emanuel" },
{ "Schlechter, Carl",         1900, false,  "schlechter", "carl" },
{ "Schmid, Lothar",           1970, false,  "schmid", "lothar" },
{ "Schottlander, Arnold",     1880, false,  "schottlander", "arnold" },
{ "Schwarz, Adolf",           1880, false,  "schwarz", "adolf" },
{ "Seirawan, Yasser",         1990, false,  "seirawan", "yasser" },
{ "Shipov, Sergei",           2000, false,  "shipov", "sergei" },
{ "Shirov, Alexei",           2000, false,  "shirov", "alexei" },
{ "Short, Nigel",             1990, false,  "short", "nigel" },
{ "Showalter, Jackson",       1900, false,  "showalter", "jackson" },
{ "Smejkal, Jan",             1980, false,  "smejkal", "jan" },
{ "Smirin, Ilia",             2000, false,  "smirin", "ilia" },
{ "Smirnov, Pavel",           2005, false,  "smirnov", "pavel" },
{ "Smyslov, Vassily",         1950, true,   "smyslov", "vassily" },
{ "So, Wesley",               2015, false,  "so", "wesley" },
{ "Sokolov, Ivan",            2000, false,  "sokolov", "ivan" },
{ "Spassky, Boris",           1970, true,   "spassky", "boris" },
{ "Speelman, Jonathan",       1990, false,  "speelman", "jonathan" },
{ "Spielmann, Rudolf",        1920, false,  "spielmann", "rudolf" },
{ "Stahlberg, Gideon",        1940, false,  "stahlberg", "gideon" },
{ "Stein, Leonid",            1970, false,  "stein", "leonid" },
{ "Steiner, Endre",           1930, false,  "steiner", "endre" },
{ "Steiner, Lajos",           1935, false,  "steiner", "lajos" },
{ "Steinitz, Wilhelm",        1890, false,  "steinitz", "wilhelm" },
{ "Stoltz, Gosta",            1935, false,  "stoltz", "gosta" },
{ "Sturua, Zurab",            1990, false,  "sturua", "zurab" },
{ "Suetin, Alexey",           1970, false,  "suetin", "alexey" },
{ "Sultan Khan, Mir",         1935, false,  "sultan khan", "mir" },
{ "Sutovsky, Emil",           2005, false,  "sutovsky", "emil" },
{ "Sveshnikov, Evgeny",       1980, false,  "sveshnikov", "evgeny" },
{ "Svidler, Peter",           2000, false,  "svidler", "peter" },
{ "Szabo, Laszlo",            1950, true,   "szabo", "laszlo" },
{ "Taimanov, Mark",           1960, true,   "taimanov", "mark" },
{ "Tal, Mikhail",             1970, false,  "tal", "mikhail" },
{ "Tarrasch, Siegbert",       1900, false,  "tarrasch", "siegbert" },
{ "Tartakower, Saviely",      1920, true,   "tartakower", "saviely" },
{ "Teichmann, Richard",       1910, false,  "teichmann", "richard" },
{ "Timman, Jan",              1980, true,   "timman", "jan" },
{ "Timofeev, Artyom",         2005, false,  "timofeev", "artyom" },
{ "Tiviakov, Sergei",         2005, false,  "tiviakov", "sergei" },
{ "Tkachiev, Vladislav",      2000, false,  "tkachiev", "vladislav" },
{ "Tolush, Alexander",        1945, false,  "tolush", "alexander" },
{ "Tomashevsky, Evgeny",      2015, false,  "tomashevsky", "evgeny" },
{ "Topalov, Veselin",         2000, false,  "topalov", "veselin" },
{ "Tregubov, Pavel",          2005, false,  "tregubov", "pavel" },
{ "Treybal, Karel",           1930, false,  "treybal", "karel" },
{ "Trifunovic, Petar",        1950, false,  "trifunovic", "petar" },
{ "Tseshkovsky, Vitaly",      1980, false,  "tseshkovsky", "vitaly" },
{ "Tukmakov, Vladimir",       1980, false,  "tukmakov", "vladimir" },
{ "Turover, Isador",          1930, false,  "turover", "isador" },
{ "Uhlmann, Wolfgang",        1960, true,   "uhlmann", "wolfgang" },
{ "Unzicker, Wolfgang",       1960, true,   "unzicker", "wolfgang" },
{ "Vachier-Lagrave, Maxime",  2015, false,  "vachier-lagrave", "maxime" },
{ "Vachier-Lagrave, Maxime",  2015, false,  "vachier lagrave", "maxime" },
{ "Vaganian, Rafael",         1980, true,   "vaganian", "rafael" },
{ "Vajda, Arpad",             1930, false,  "vajda", "arpad" },
{ "Vallejo Pons, Francisco",  2005, false,  "vallejo pons", "francisco" },
{ "van Wely, Loek",           2000, false,  "van wely", "loek" },
{ "Vescovi, Giovanni",        2005, false,  "vescovi", "giovanni" },
{ "Vidmar, Milan",            1920, false,  "vidmar", "milan" },
{ "Vitiugov, Nikita",         2015, false,  "vitiugov", "nikita" },
{ "Vladimirov, Evgeny",       1990, false,  "vladimirov", "evgeny" },
{ "Voigt, Hermann",           1900, false,  "voigt", "hermann" },
{ "Volokitin, Andrei",        2005, false,  "volokitin", "andrei" },
{ "von Bardeleben, Curt",     1890, false,  "von bardeleben", "curt" },
{ "Vyzmanavin, Alexey",       1990, false,  "vyzmanavin", "alexey" },
{ "Walbrodt, Carl",           1900, false,  "walbrodt", "carl" },
{ "Wang Hao",                 2015, false,  "wang hao", "" },
{ "Wang Yue",                 2015, false,  "wang yue", "" },
{ "Wei Yi",                   2015, false,  "wei yi", "" },
{ "Weiss, Miksa",             1890, false,  "weiss", "miksa" },
{ "Winawer, Szymon",          1880, false,  "winawer", "szymon" },
{ "Wojtaszek, Radoslaw",      2015, false,  "wojtaszek", "radoslaw" },
{ "Yates, Frederick",         1930, false,  "yates", "frederick" },
{ "Ye Jiangchuan",            2005, false,  "ye jiangchuan", "" },
{ "Yu Yangyi",                2015, false,  "yu yangyi", "" },
{ "Yudasin, Leonid",          1990, false,  "yudasin", "leonid" },
{ "Zhigalko, Sergei",         2015, false,  "zhigalko", "sergei" },
{ "Znosko-Borovsky, Eugene",  1920, false,  "znosko-borovsky", "eugene" },
{ "Znosko-Borovsky, Eugene",  1920, false,  "znosko borovsky", "eugene" },
{ "Zukertort, Johannes",      1880, false,  "zukertort", "johannes" },
{ "Zvjaginsev, Vadim",        2000, false,  "zvjaginsev", "vadim" }
    };

static std::map<std::string,int> lookup;
static void players_database_begin()
{
    int nbr = sizeof(players)/(sizeof(players[0]));
    for( int i=0; i<nbr; i++ )
    {
        players[i].count = 0;
        const char *name = players[i].name;
        lookup[name] = i;
    }
}

static void players_database_end()
{
    char buf[1000];
    int nbr = sizeof(players)/(sizeof(players[0]));
    for( int i=0; i<nbr; i++ )
    {
        int count = players[i].count;
        const char *name = players[i].full;
        sprintf( buf, "%s, count=%d\n", name, count );
        cprintf( "%s", buf );
        fprintf( logfile, "%s", buf );
    }
    sprintf( buf, "games generated=%d\n", players_database_total );
    cprintf( "%s", buf );
    fprintf( logfile, "%s", buf );
}

static int candidate( const char *name )
{
    //if( strstr(name,"Ding Liren") )
    //    cprintf( "Ding Liren\n" );
    char surname[1000];
    int i=0;
    const char *q = name;
    char c;
    while( *q )
    {
        c = *q;
        if( c==',' || c=='(' )
            break;
        if( isascii(c) && isupper(c) )
            c = tolower(c);
        surname[i++] = c;
        q++;
    }
    surname[i] = '\0';
    char initial = '\0';
    if( c != ',' )  
    {
        q--;
        i--;
        while( i>=0 && *q==' ')
        {
            surname[i--] = '\0';
            q--;
        }
        // no initial        
    }
    else
    {
        while( *q )
        {
            char c = *q++;
            if( isascii(c) && isalpha(c) )
            {
                if( c=='T' && *q=='L' && 0==strcmp(surname,"petrosian") )
                    return -1; // special special special case -> filter out Tigran Petrosian Junior!
                initial = tolower(c);
                break;
            }
        }
    }
    std::map<std::string,int>::iterator it = lookup.find(surname);
    if( it != lookup.end() && players[it->second].extra[0]==initial )
        return it->second;
    else
        return -1;
}

static void players_database( const char *event, const char *site, const char *date, const char *round,
                       const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                       int nbr_moves, thc::Move *moves, uint64_t *hashes )
{
    int candw = candidate(white);
    int candb = candidate(black);
    if( candw>=0 && candb>=0 )
    {
        char buf[1000];
        strcpy( buf, event );
        char *q = buf;
        while( *q )
        {
            char c = *q;
            if( isascii(c) && isupper(c) )
                *q = tolower(c);
            q++;
        }
        char buf2[1000];
        strcpy( buf2, site );
        q = buf2;
        while( *q )
        {
            char c = *q;
            if( isascii(c) && isupper(c) )
                *q = tolower(c);
            q++;
        }
        bool rapid = strstr(buf,"rapid") || strstr(buf2,"rapid");
        bool blitz = strstr(buf,"blitz") || strstr(buf2,"blitz");
        if( !rapid && !blitz )
        {
            int year = atoi( date );
            int elow = atoi( white_elo );
            int elob = atoi( black_elo );
            bool keep=true;
            int stage = 0;
            if( year >= 2010 )
                stage = 2010;
            else if( year >= 2005 )
                stage = 2005;
            else if( year >= 2000 )
                stage = 2000;
            else if( year >= 1995 )
                stage = 1995;
            else if( year >= 1990 )
                stage = 1990;
            else if( year >= 1980 )
                stage = 1980;
            else if( year >= 1970 )
                stage = 1970;
            else
                stage = 0;
            switch( stage )
            {
                case 2010:  keep = (elow>=2550 && elob>=2550);  break;
                case 2005:  keep = (elow>=2500 && elob>=2500);  break;
                case 2000:  keep = (elow>=2450 && elob>=2450);  break;
                case 1995:  keep = (elow>=2400 && elob>=2400);  break;
                case 1990:  keep = (elow>=2300 && elob>=2300);  break;
                case 1980:  keep = (elow==0&&elob==0) || (elow>=2250 && elob>=2250);  break;
                case 1970:  keep = (elow==0&&elob==0) || (elow>=2200 && elob>=2200);  break;
                default:    keep = (year>=1880);                        break;
            }
            if( keep )
            {
                char buf[1000];
                players[candw].count++;
                players[candb].count++;
                if( elow && elow<2400 )
                {
                    sprintf( buf, "check %s-%s %s %d white_elo=%d?\n", white, black, site, year, elow );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                if( elob && elob<2400 )
                {
                    sprintf( buf, "check %s-%s %s %d black_elo=%d?\n", white, black, site, year, elob );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                int peakw = players[candw].year;
                if( year-peakw > (players[candw].long_career?50:30) ) 
                {
                    sprintf( buf, "%s %d too old? > %s-%s\n", players[candw].full, year, white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                if( peakw-year > (players[candw].long_career?30:20) ) 
                {
                    sprintf( buf, "%s %d too young? > %s-%s\n", players[candw].full, year, white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                int peakb = players[candb].year;
                if( year-peakb > (players[candb].long_career?50:30) ) 
                {
                    sprintf( buf, "%s %d too old? > %s-%s\n", players[candb].full, year, white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                if( peakb-year > (players[candb].long_career?30:20) )
                { 
                    sprintf( buf, "%s %d too young? > %s-%s\n", players[candb].full, year, white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                fprintf( ofile, "[Event \"%s\"]\n",     event );
                fprintf( ofile, "[Site \"%s\"]\n",      site );
                fprintf( ofile, "[Date \"%s\"]\n",      date );
                fprintf( ofile, "[Round \"%s\"]\n",     round );
                fprintf( ofile, "[White \"%s\"]\n",     players[candw].full );
                fprintf( ofile, "[Black \"%s\"]\n",     players[candb].full );
                fprintf( ofile, "[Result \"%s\"]\n",    result );
                if( elow )
                    fprintf( ofile, "[WhiteElo \"%s\"]\n",  white_elo );
                if( elob )
                    fprintf( ofile, "[BlackElo \"%s\"]\n",  black_elo );
                if( *eco )
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
                players_database_total++;
            }
        }
    }
}


