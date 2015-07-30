/****************************************************************************
 *  Various database maintenance commands
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "thc.h"
#include "DebugPrintf.h"
#include "PgnRead.h"
#include "CompressMoves.h"
#include "DbPrimitives.h"
#include "DbMaintenance.h"

//-- Temp - hardwire .pgn file and database name
#ifdef THC_WINDOWS
#define PGN_FILE        "/Users/Maria/Documents/Tarrasch/giant-base-part1-rebuilt.pgn"
#define PGN_OUT_FILE    "/Users/Maria/Documents/BillsFiles/twic/twic_minimal_overlap_2.pgn"
#define QGN_FILE        "/Users/Maria/Documents/BillsFiles/twic/twic_minimal_overlap.qgn"
#else
#define PGN_FILE        "/Users/billforster/Documents/ChessDatabases/twic-948-1010.pgn"
#define PGN_OUT_FILE    "/Users/billforster/Documents/ChessDatabases/twic_minimal_overlap_2.pgn"
#define QGN_FILE        "/Users/billforster/Documents/ChessDatabases/twic_minimal_overlap.qgn"
#endif

static FILE *ifile;
static FILE *ofile;

static void decompress_game( const char *compressed_header, const char *compressed_moves );
static void game_to_qgn_file( const char *event, const char *site, const char *date, const char *round,
                             const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                             int nbr_moves, thc::Move *moves, uint64_t *hashes );
static void compress_moves_to_str( int nbr_moves, thc::Move *moves, char *dst, thc::ChessPosition *positions );
static void decompress_moves_from_str( int nbr_moves, char *src, thc::Move *moves, thc::ChessPosition *positions  );
static void verify_compression_algorithm( int nbr_moves, thc::Move *moves );

void db_maintenance_speed_tests()
{
    db_primitive_speed_tests();
}

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
           ((nbr_games%100) == 0 ) /* ||
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
}

void db_maintenance_verify_compression()
{
    ifile = fopen( PGN_FILE, "rt" );
    if( !ifile )
        cprintf( "Cannot open %s\n", PGN_FILE );
    else
    {
        PgnRead *pgn = new PgnRead('V');
        pgn->Process(ifile);
    }
    if( ifile )
        fclose(ifile);
    if( ofile )
        fclose(ofile);
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


void db_maintenance_create_or_append_to_database(  const char *pgn_filename )
{
    ifile = fopen( pgn_filename , "rt" );
    if( !ifile )
        cprintf( "Cannot open %s\n", pgn_filename );
    else
    {
        DebugPrintfTime turn_on_time_display;
        PgnRead *pgn = new PgnRead('A');
        db_primitive_open();
        db_primitive_transaction_begin();
        db_primitive_create_tables();
        db_primitive_count_games();
        pgn->Process(ifile);
        db_primitive_transaction_end();
        db_primitive_close();
    }
    if( ifile )
        fclose(ifile);
    if( ofile )
        fclose(ofile);
}

void db_utility_test();
void db_maintenance_create_indexes()
{
    //db_utility_test();
    db_primitive_open();
    db_primitive_transaction_begin();
    db_primitive_create_indexes();
    db_primitive_create_extra_indexes();
    db_primitive_transaction_end();
    db_primitive_close();
}



bool gbl_evil_queen;
bool gbl_double_byte;
void hook_gameover( char callback_code, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves, uint64_t *hashes )
{
    static int counter;
    if( (++counter % 1000) == 0 )
        cprintf( "%d games processed so far\n", counter );
    switch( callback_code )
    {
        // Compress
        case 'P': game_to_qgn_file( event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves, hashes );  break;
            
        // Append
        case 'A': db_primitive_insert_game( white, black, event, site, result, date, white_elo, black_elo, nbr_moves, moves, hashes ); break;
            
        // Verify
        case 'V': verify_compression_algorithm( nbr_moves, moves ); break;
    }
    if( gbl_evil_queen || gbl_double_byte )
    {
        static int evil_queen_count;
        static int double_byte_count;
        if( gbl_evil_queen )
            evil_queen_count++;
        if( gbl_double_byte )
            double_byte_count++;
        cprintf( "%d evil games, %d double byte games, %s-%s, %s\n",  evil_queen_count, double_byte_count, white, black, gbl_double_byte?"needs double bytes":"doesn't need double bytes");
        gbl_evil_queen = false;
        gbl_double_byte = false;
        thc::ChessRules cr;
        std::string sgame;
        for( int i=0; i<nbr_moves; i++ )
        {
            thc::Move mv = moves[i];
            std::string s = mv.NaturalOut( &cr );
            sgame += s;
            if( i+1 < nbr_moves )
                sgame += " ";
            cr.PlayMove(mv);
        }
        cprintf( "%s\n", sgame.c_str() );
    }
}


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
    dst = buf;
    for( int i=0; i<nbr_moves && dst<buf+sizeof(buf)-5; i++ )
    {
        thc::Move mv = *moves++;
        int nbr = press.compress_move( mv, dst );
        dst += nbr;
    }
    *dst = '\0';
    fputs( buf, ofile );    
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
    src = compressed_moves;
    std::string moves_txt;
    int count=0;
    while( *src )
    {
        thc::Move mv;
        thc::ChessRules before = press.cr;
        int nbr = press.decompress_move( src, mv );
        if( nbr == 0 )
            break;
        src += nbr;
        char buf[20];
        if( count%2 == 0 )
        {
            if( count == 0 )
                sprintf( buf, "%d. ", count/2+1 );
            else
                sprintf( buf, " %d. ", count/2+1 );
        }
        else
            strcpy( buf, " " );
        moves_txt += buf;
        moves_txt += mv.NaturalOut( &before );
        count++;    
    }
    if( count > 0 )
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
            if( col_end_of_next_word > 76 )
            {
                c = '\n';
                col = 0;
            }
        }
        justified += c;
    }
    fprintf( ofile, "%s\n\n",  justified.c_str() );
}


static void compress_moves_to_str( int nbr_moves, thc::Move *moves, char *dst, thc::ChessPosition *positions )
{
    CompressMoves press;
    for( int i=0; i<nbr_moves; i++ )
    {
        thc::Move mv = *moves++;
        int nbr = press.compress_move( mv, dst );
        std::string s = mv.TerseOut();
//#define VERBOSE
#ifdef VERBOSE
        if( nbr == 1 )
            cprintf( "compress temp> %s -> %02x\n", s.c_str(), *dst&0xff );
        else
            cprintf( "compress temp> %s -> %02x,%02x\n", s.c_str(), *dst&0xff, *(dst+1)&0xff );
#endif
        dst += nbr;
        if( positions[i] != press.cr )
        {
            press.Check( false, "Compress progress check", &positions[i] );
        }
    }
    *dst = '\0';
}

static void decompress_moves_from_str( int nbr_moves, char *src, thc::Move *moves, thc::ChessPosition *positions  )
{
    CompressMoves press;
    for( int i=0; i<nbr_moves; i++ )
    {
        thc::Move mv;
        int nbr = press.decompress_move( src, mv );
        std::string s = mv.TerseOut();
#ifdef VERBOSE
        if( nbr == 1 )
            cprintf( "decompress temp> %02x -> %s\n", *src&0xff, s.c_str() );
        else
            cprintf( "decompress temp> %02x,%02x -> %s\n", *src&0xff, *(src+1)&0xff, s.c_str() );
#endif
        src += nbr;
        *moves++ = mv;
        if( positions[i] != press.cr )
        {
            press.Check( false, "Decompress progress check", &positions[i] );
        }
    }
}

// Verify compression
static void verify_compression_algorithm( int nbr_moves, thc::Move *moves )
{
    char buf[2000];
    thc::Move unpacked[1000];
    static thc::ChessPosition positions_buffer[1000];
    thc::ChessRules cr;
    if( sizeof(buf) < 2*nbr_moves )
    {
        cprintf( "Buffer too small" );
        return;
    }
    for( int i=0; i<nbr_moves; i++ )
    {
        thc::Move mv = moves[i];
        cr.PlayMove(mv);
        positions_buffer[i] = cr;
    }
    compress_moves_to_str( nbr_moves, moves, buf, positions_buffer );
#ifdef VERBOSE
    for( int i=0; i<nbr_moves; i++ )
    {
        thc::Move mv = moves[i];
        std::string s = mv.TerseOut();
        if( i==0 )
            cprintf( "Moves>" );
        cprintf( " " );
        cprintf( s.c_str() );
    }
    cprintf( "\n" );
    for( int i=0; ; i++ )
    {
        char c = buf[i];
        if( c == '\0' )
            break;
        if( i==0 )
            cprintf( "Compressed>" );
        cprintf( " " );
        cprintf( "%02x", c&0xff );
    }
    cprintf( "\n" );
#endif
    decompress_moves_from_str( nbr_moves, buf, unpacked, positions_buffer );
#ifdef VERBOSE
    for( int i=0; i<nbr_moves; i++ )
    {
        thc::Move mv = unpacked[i];
        std::string s = mv.TerseOut();
        if( i==0 )
            cprintf( "Unpacked Moves>" );
        cprintf( " " );
        cprintf( s.c_str() );
    }
    cprintf( "\n" );
#endif
    bool match = (0 == memcmp( moves, unpacked, nbr_moves*sizeof(thc::Move)));
    if( match )
    {
#ifdef VERBOSE
        cprintf( "Woo hoo, %d games match\n", ++nbr_games );
#endif
    }
    else
    {
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
}


