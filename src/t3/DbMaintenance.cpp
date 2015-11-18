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

static void decompress_game( const char *compressed_header, const char *compressed_moves );
static void game_to_qgn_file( const char *event, const char *site, const char *date, const char *round,
                             const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                             int nbr_moves, thc::Move *moves, uint64_t *hashes );
static int  verify_compression_algorithm( int nbr_moves, thc::Move *moves );

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
        CompressMovesDiagBegin();
        PgnRead *pgn = new PgnRead('V');
        pgn->Process(ifile);
        CompressMovesDiagEnd();
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
        db_primitive_transaction_begin();
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
    db_primitive_transaction_begin();
    db_primitive_create_indexes();
    db_primitive_create_extra_indexes();
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
    int is_interesting = 0;
    if( (++counter % 100000) == 0 )
        cprintf( "%d games processed so far\n", counter );
    switch( callback_code )
    {
        // Compress
        case 'P': game_to_qgn_file( event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves, hashes );  break;
            
        // Append
        case 'A':  db_primitive_insert_game( white, black, event, site, result, date, white_elo, black_elo, nbr_moves, moves, hashes );   break;
            
        // Read one game
        case 'R': pgn_read_hook( white, black, event, site, result, date, white_elo, black_elo, nbr_moves, moves, hashes ); return true;

        // Verify
        case 'V': is_interesting = verify_compression_algorithm( nbr_moves, moves ); break;
    }
    extern int nbr_games_with_two_queens;
    extern int nbr_games_with_promotions;
    extern int nbr_games_with_slow_mode;
    if( is_interesting&4096 )
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
            strcat(buf,"New record pawn swap ");
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
        if(is_interesting!=16)
            cprintf( "Adding game with issues; %s\n", buf );
        FILE *f = fopen( (is_interesting&0x0f) ? "TooManyKnightsEtc.pgn" : "TwoOrMoreQueens.pgn", "at" );
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
    return aborted;
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
    int is_interesting = false;
    extern int max_nbr_slow_moves_other;
    extern int max_nbr_slow_moves_queen;
    std::vector<thc::Move> moves_in(moves,moves+nbr_moves);
    CompressMoves press;
    std::string compressed = press.Compress(moves_in);
    std::vector<thc::Move> unpacked = press.Uncompress(compressed);
    is_interesting = press.is_interesting;
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
    bool match = (0 == memcmp( &moves_in[0], &unpacked[0], nbr_moves*sizeof(thc::Move)));
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
}


