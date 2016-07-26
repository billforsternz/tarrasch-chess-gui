/****************************************************************************
 *  Database maintenance miscellany - mainly development test only
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <stdio.h>
#include <map>
#include "thc.h"
#include "DebugPrintf.h"
#include "BinDb.h"
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
#else
#define PGN_FILE        "/Users/billforster/Documents/ChessDatabases/giant123.pgn"
#endif

static FILE *ifile;
static FILE *ofile;
static FILE *logfile;

static int  verify_compression_algorithm( int nbr_moves, thc::Move *moves );
static void players_database_begin();
static void players_database_end();
static int  players_database_total;
static void players_database( const char *event, const char *site, const char *date, const char *round,
                       const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                       int nbr_moves, thc::Move *moves, uint64_t *hashes );

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
    //std::string input("C:\\Users\\Bill\\Documents\\T3Database\\millionbase-2.22");
    std::string input("C:\\Users\\Bill\\Documents\\T3Database\\twic-2006-2015");
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

void db_utility_test();
extern void pgn_read_hook( const char *fen, const char *white, const char *black, const char *event, const char *site, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo, const char *eco, const char *round,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  );


bool hook_gameover( char callback_code, const char *fen, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves, uint64_t *hashes )
{
    bool aborted = false;
    static int counter;
    if( (++counter % 100000) == 0 )
        cprintf( "%d games processed so far\n", counter );
    if( fen && callback_code!='R' )
        return false;
    switch( callback_code )
    {
        // Great players database
        case 'G': players_database( event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves, hashes );  break;
            
        // Binary database append
        case 'B': bin_db_append( fen, event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves );  break;

        // Read one game
        case 'R': pgn_read_hook( fen, white, black, event, site, result, date, white_elo, black_elo, eco, round, nbr_moves, moves, hashes ); return true;

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
            bool match2 = (0 == memcmp( &moves[i], &unpacked[i], sizeof(thc::Move)));
            if( !match2 )
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
    bool draws_false_positives;
    const char *name;
    const char *extra;
    int count;
} PLAYER;


PLAYER players[] =
    {
{ "Adams, Michael",           1995, false,  false,  "adams", "michael" },
{ "Agdestein, Simen",         1990, false,  false,  "agdestein", "simen" },
{ "Ahues, Carl",              1930, false,  false,  "ahues", "carl" },
{ "Akopian, Vladimir",        2000, false,  false,  "akopian", "vladimir" },
{ "Alapin, Semyon",           1900, false,  false,  "alapin", "semyon" },
{ "Alatortsev, Vladimir",     1940, false,  false,  "alatortsev", "vladimir" },
{ "Alekhine, Alexander",      1930, false,  false,  "alekhine", "alexander" },
{ "Aleksandrov, Aleksej",     2005, false,  true,   "aleksandrov", "aleksej" },
{ "Almasi, Zoltan",           2005, false,  false,  "almasi", "zoltan" },
{ "Amin, Bassem",             2015, false,  false,  "amin", "bassem" },
{ "Anand, Viswanathan",       2000, false,  false,  "anand", "viswanathan" },
{ "Anderssen, Adolf",         1870, false,  false,  "anderssen", "adolf" },
{ "Andersson, Ulf",           1980, false,  false,  "andersson", "ulf" },
{ "Andreikin, Dmitry",        2015, false,  false,  "andreikin", "dmitry" },
{ "Areshchenko, Alexander",   2015, false,  false,  "areshchenko", "alexander" },
{ "Aronian, Levon",           2005, false,  false,  "aronian", "levon" },
{ "Artemiev, Vladislav",      2015, false,  false,  "artemiev", "vladislav" },
{ "Aseev, Konstantin",        1990, false,  false,  "aseev", "konstantin" },
{ "Asztalos, Lajos",          1920, false,  false,  "asztalos", "lajos" },
{ "Averbakh, Yury",           1950, true,   false,  "averbakh", "yury" },
{ "Averbakh, Yury",           1950, true,   false,  "averbach", "yury" },
{ "Avrukh, Boris",            2005, false,  false,  "avrukh", "boris" },
{ "Azmaiparashvili, Zurab",   1990, false,  false,  "azmaiparashvili", "zurab" },
{ "Bacrot, Etienne",          2005, false,  false,  "bacrot", "etienne" },
{ "Balashov, Yury",           1970, false,  false,  "balashov", "yury" },
{ "Bareev, Evgeny",           1990, false,  false,  "bareev", "evgeny" },
{ "Bauer, Christian",         2005, false,  false,  "bauer", "christian" },
{ "Becker, Albert",           1930, false,  false,  "becker", "albert" },
{ "Belavenets, Sergey",       1940, false,  false,  "belavenets", "sergey" },
{ "Beliavsky, Alexander",     1990, false,  false,  "beliavsky", "alexander" },
{ "Bernstein, Ossip",         1910, true,   false,  "bernstein", "ossip" },
{ "Bird, Henry",              1880, false,  false,  "bird", "henry" },
{ "Blackburne, Joseph",       1890, true,   false,  "blackburne", "joseph" },
{ "Bogoljubow, Efim",         1935, false,  false,  "bogoljubow", "efim" },
{ "Boleslavsky, Isaak",       1950, false,  false,  "boleslavsky", "isaak" },
{ "Bondarevsky, Igor",        1950, false,  false,  "bondarevsky", "igor" },
{ "Bologan, Viktor",          2000, false,  false,  "bologan", "viktor" },
{ "Botvinnik, Mikhail",       1950, true,   false,  "botvinnik", "mikhail" },
{ "Botvinnik, Mikhail",       1950, true,   false,  "botwinnik", "mikhail" },
{ "Breyer, Gyula",            1920, false,  false,  "breyer", "gyula" },
{ "Bronstein, David",         1950, true,   false,  "bronstein", "david" },
{ "Bruzon, Lazaro",           2005, false,  false,  "bruzon", "lazaro" },
{ "Bu Xiangzhi",              2015, false,  false,  "bu xiangzhi", "" },
{ "Bukavshin, Ivan",          2015, false,  false,  "bukavshin", "ivan" },
{ "Burn, Amos",               1890, false,  false,  "burn", "amos" },
{ "Canal, Esteban",           1930, false,  false,  "canal", "esteban" },
{ "Capablanca, Jose",         1920, false,  false,  "capablanca", "jose" },
{ "Carlsen, Magnus",          2015, false,  false,  "carlsen", "magnus" },
{ "Caruana, Fabiano",         2015, false,  false,  "caruana", "fabiano" },
{ "Chandler, Murray",         1990, false,  false,  "chandler", "murray" },
{ "Charousek, Rezso",         1900, false,  false,  "charousek", "rezso" },
{ "Chekhover, Vitaly",        1940, false,  false,  "chekhover", "vitaly" },
{ "Cheparinov, Ivan",         2015, false,  false,  "cheparinov", "ivan" },
{ "Chigorin, Mikhail",        1890, false,  false,  "chigorin", "mikhail" },
{ "Cohn, Wilhelm",            1900, false,  false,  "cohn", "wilhelm" },
{ "Colle, Edgar",             1930, false,  false,  "colle", "edgar" },
{ "Dautov, Rustem",           1990, false,  false,  "dautov", "rustem" },
{ "Denker, Arnold",           1935, true,   false,  "denker", "arnold" },
{ "Ding Liren",               2015, false,  false,  "ding liren", "" },
{ "Dolmatov, Sergey",         1980, false,  false,  "dolmatov", "sergey" },
{ "Dominguez, Lenier",        2005, false,  false,  "dominguez", "lenier" },
{ "Dreev, Alexey",            2000, false,  false,  "dreev", "alexey" },
{ "Dubov, Daniil",            2015, false,  false,  "dubov", "daniil" },
{ "Duda, Jan-Krzysztof",      2015, false,  false,  "duda", "jan-krzysztof" },
{ "Duras, Oldrich",           1910, false,  false,  "duras", "oldrich" },
{ "Dus-Chotimirsky, Fedor",   1910, true,   false,  "dus-chotimirsky", "fedor" },
{ "Dus-Chotimirsky, Fedor",   1910, true,   false,  "dus chotimirsky", "fedor" },
{ "Dvoiris, Semen",           1990, false,  false,  "dvoiris", "semen" },
{ "Ehlvest, Jaan",            1990, false,  false,  "ehlvest", "jaan" },
{ "Eliskases, Erich",         1940, false,  false,  "eliskases", "erich" },
{ "Eljanov, Pavel",           2005, false,  false,  "eljanov", "pavel" },
{ "Engels, Ludwig",           1940, false,  false,  "engels", "ludwig" },
{ "Englisch, Berthold",       1880, false,  true,   "englisch", "berthold" },
{ "Epishin, Vladimir",        2005, false,  false,  "epishin", "vladimir" },
{ "Euwe, Max",                1935, false,  false,  "euwe", "max" },
{ "Fedorov, Alexei",          2000, false,  false,  "fedorov", "alexei" },
{ "Fedoseev, Vladimir",       2015, false,  false,  "fedoseev", "vladimir" },
{ "Filippov, Valerij",        2005, false,  false,  "filippov", "valerij" },
{ "Fine, Reuben",             1940, false,  false,  "fine", "reuben" },
{ "Fischer, Robert J",        1960, false,  false,  "fischer", "bobby" },
{ "Fischer, Robert J",        1960, false,  false,  "fischer", "robert" },
{ "Flohr, Salo",              1935, false,  false,  "flohr", "salo" },
{ "Forgacs, Leo",             1910, false,  false,  "forgacs", "leo" },
{ "Fressinet, Laurent",       2005, false,  false,  "fressinet", "laurent" },
{ "Frydman, Paulin",          1935, false,  false,  "frydman", "paulin" },
{ "Fuderer, Andrija",         1960, false,  false,  "fuderer", "andrija" },
{ "Furman, Semen",            1950, false,  false,  "furman", "semen" },
{ "Gelfand, Boris",           2000, false,  false,  "gelfand", "boris" },
{ "Geller, Efim",             1960, true,   false,  "geller", "efim" },
{ "Georgadze, Tamaz",         1980, false,  false,  "georgadze", "tamaz" },
// { "Georgiev, Kiril",          2000, false,  false,  "georgiev", "kiril" },
{ "Ghaem Maghami, Ehsan",     2005, false,  false,  "ghaem maghami", "ehsan" },
{ "Gharamian, Tigran",        2015, false,  false,  "gharamian", "tigran" },
{ "Gheorghiu, Florin",        1980, false,  false,  "gheorghiu", "florin" },
{ "Gilg, Karl",               1930, false,  false,  "gilg", "karl" },
{ "Gipslis, Aivars",          1970, false,  false,  "gipslis", "aivars" },
{ "Giri, Anish",              2015, false,  false,  "giri", "anish" },
{ "Gligoric, Svetozar",       1960, true,   false,  "gligoric", "svetozar" },
{ "Goldin, Alexander",        2005, false,  false,  "goldin", "alexander" },
{ "Graf, Alexander",          2005, false,  false,  "graf", "alexander" },
{ "Granda Zuniga, Julio",     1990, false,  false,  "granda zuniga", "julio" },
{ "Grischuk, Alexander",      2005, false,  false,  "grischuk", "alexander" },
{ "Grunfeld, Ernst",          1930, false,  false,  "grunfeld", "ernst" },
{ "Gulko, Boris",             1980, false,  false,  "gulko", "boris" },
{ "Gunsberg, Isidor",         1890, false,  false,  "gunsberg", "isidor" },
{ "Gurevich, Mikhail",        2000, false,  false,  "gurevich", "mikhail" },
{ "Gustafsson, Jan",          2005, false,  false,  "gustafsson", "jan" },
{ "Hammer, Jon Ludvig",       2015, false,  false,  "hammer", "jon ludvig" },
{ "Hansen, Curt",             2000, false,  false,  "hansen", "curt" },
{ "Harikrishna, Pentala",     2005, false,  false,  "harikrishna", "pentala" },
{ "Hjartarson, Johann",       2005, false,  false,  "hjartarson", "johann" },
{ "Horowitz, Al",             1940, false,  false,  "horowitz", "al" },
{ "Hort, Vlastimil",          1970, true,   false,  "hort", "vlastimil" },
{ "Hou Yifan",                2015, false,  false,  "hou yifan", "" },
{ "Howell, David",            2015, false,  false,  "howell", "david" },
{ "Hubner, Robert",           1980, true,   false,  "hubner", "robert" },
{ "Hubner, Robert",           1980, true,   false,  "huebner", "robert" },
{ "Illescas, Miguel",         2005, false,  false,  "illescas", "miguel" },
{ "Inarkiev, Ernesto",        2015, false,  false,  "inarkiev", "ernesto" },
{ "Iordachescu, Viorel",      2005, false,  false,  "iordachescu", "viorel" },
{ "Istratescu, Andrei",       2005, false,  false,  "istratescu", "andrei" },
{ "Ivanchuk, Vassily",        2000, false,  false,  "ivanchuk", "vassily" },
{ "Ivanisevic, Ivan",         2015, false,  false,  "ivanisevic", "ivan" },
{ "Ivkov, Borislav",          1960, true,   false,  "ivkov", "borislav" },
{ "Jakovenko, Dmitry",        2015, false,  false,  "jakovenko", "dmitry" },
{ "Janowsky, Dawid",          1910, false,  false,  "janowsky", "dawid" },
{ "Jobava, Baadur",           2005, false,  false,  "jobava", "baadur" },
{ "Johner, Paul",             1920, false,  false,  "johner", "paul" },
{ "Jones, Gawain",            2010, false,  false,  "jones", "gawain" },
{ "Jussupow, Artur",          1990, false,  false,  "jussupow", "artur" },
{ "Kaidanov, Gregory",        2005, false,  false,  "kaidanov", "gregory" },
{ "Kamsky, Gata",             1995, false,  false,  "kamsky", "gata" },
{ "Karjakin, Sergey",         2015, false,  false,  "karjakin", "sergey" },
{ "Karpov, Anatoly",          1980, true,   false,  "karpov", "anatoly" },
{ "Kashdan, Isaac",           1930, false,  false,  "kashdan", "isaac" },
{ "Kasimdzhanov, Rustam",     2005, false,  false,  "kasimdzhanov", "rustam" },
{ "Kasparov, Garry",          1990, false,  false,  "kasparov", "garry" },
{ "Kazhgaleyev, Murtas",      2005, false,  false,  "kazhgaleyev", "murtas" },
{ "Kempinski, Robert",        2005, false,  false,  "kempinski", "robert" },
{ "Keres, Paul",              1950, false,  false,  "keres", "paul" },
{ "Khalifman, Alexander",     2000, false,  false,  "khalifman", "alexander" },
{ "Kharlov, Andrei",          2005, false,  false,  "kharlov", "andrei" },
{ "Khenkin, Igor",            2000, false,  false,  "khenkin", "igor" },
{ "Khismatullin, Denis",      2015, false,  false,  "khismatullin", "denis" },
{ "Kholmov, Ratmir",          1960, true,   false,  "kholmov", "ratmir" },
{ "Kmoch, Hans",              1930, false,  false,  "kmoch", "hans" },
{ "Kobalia, Mikhail",         2005, false,  false,  "kobalia", "mikhail" },
{ "Kolisch, Ignatz",          1880, false,  false,  "kolisch", "ignatz" },
{ "Korchnoi, Viktor",         1980, false,  false,  "korchnoi", "viktor" },
{ "Korobov, Anton",           2015, false,  false,  "korobov", "anton" },
{ "Kostic, Boris",            1930, false,  false,  "kostic", "boris" },
{ "Kotov, Alexander",         1950, false,  false,  "kotov", "alexander" },
{ "Kovalenko, Igor",          2015, false,  false,  "kovalenko", "igor" },
{ "Kramnik, Vladimir",        2005, false,  false,  "kramnik", "vladimir" },
{ "Krasenkow, Michal",        2000, false,  false,  "krasenkow", "michal" },
{ "Krogius, Nikolay",         1970, false,  false,  "krogius", "nikolay" },
{ "Kryvoruchko, Yuriy",       2015, false,  false,  "kryvoruchko", "yuriy" },
{ "Larsen, Bent",             1970, false,  false,  "larsen", "bent" },
{ "Lasker, Emanuel",          1910, true,   false,  "lasker", "emanuel" },
{ "Lautier, Joel",            2000, false,  false,  "lautier", "joel" },
{ "Laznicka, Viktor",         2015, false,  false,  "laznicka", "viktor" },
{ "Le, Quang Liem",           2015, false,  false,  "le", "quang liem" },
{ "Leko, Peter",              2000, false,  false,  "leko", "peter" },
{ "Leonhardt, Paul",          1910, false,  false,  "leonhardt", "paul" },
{ "Levenfish, Grigory",       1920, false,  false,  "levenfish", "grigory" },
{ "Li Chao",                  2015, false,  false,  "li chao", "" },
{ "Li Chao",                  2015, false,  false,  "li chao2", "" },
{ "Li Chao",                  2015, false,  false,  "li chao b", "" },
{ "Liberzon, Vladimir",       1970, false,  false,  "liberzon", "vladimir" },
{ "Lichtenstein, Bernhard",   1930, false,  false,  "lichtenstein", "bernhard" },
{ "Lilienthal, Andor",        1935, false,  false,  "lilienthal", "andor" },
{ "Lipke, Paul",              1900, false,  false,  "lipke", "paul" },
{ "Lipschutz, Samuel",        1890, false,  false,  "lipschutz", "samuel" },
{ "Ljubojevic, Ljubomir",     1980, false,  false,  "ljubojevic", "ljubomir" },
{ "Lokvenc, Josef",           1940, false,  false,  "lokvenc", "josef" },
{ "Lputian, Smbat",           1990, false,  false,  "lputian", "smbat" },
{ "Lutz, Christopher",        2005, false,  false,  "lutz", "christopher" },
{ "Lysyj, Igor",              2015, false,  false,  "lysyj", "igor" },
{ "Macieja, Bartlomiej",      2005, false,  false,  "macieja", "bartlomiej" },
{ "Mackenzie, George",        1880, false,  false,  "mackenzie", "george" },
{ "Makarichev, Sergey",       1980, false,  false,  "makarichev", "sergey" },
{ "Makogonov, Vladimir",      1940, false,  false,  "makogonov", "vladimir" },
{ "Malakhov, Vladimir",       2005, false,  false,  "malakhov", "vladimir" },
{ "Mamedov, Rauf",            2015, false,  false,  "mamedov", "rauf" },
{ "Mamedyarov, Shakhriyar",   2005, false,  false,  "mamedyarov", "shakhriyar" },
{ "Marco, Georg",             1900, false,  false,  "marco", "georg" },
{ "Maroczy, Geza",            1910, false,  false,  "maroczy", "geza" },
{ "Marshall, Frank",          1920, false,  false,  "marshall", "frank" },
{ "Mason, James",             1880, false,  false,  "mason", "james" },
{ "Matanovic, Aleksandar",    1960, false,  false,  "matanovic", "aleksandar" },
{ "Matisons, Hermanis",       1930, false,  false,  "matisons", "hermanis" },
{ "Matlakov, Maxim",          2015, false,  false,  "matlakov", "maxim" },
{ "Matulovic, Milan",         1970, false,  false,  "matulovic", "milan" },
{ "McShane, Luke",            2005, false,  false,  "mcshane", "luke" },
{ "Meier, Georg",             2015, false,  false,  "meier", "georg" },
{ "Mieses, Jacques",          1910, true,   false,  "mieses", "jacques" },
{ "Mikenas, Vladas",          1935, false,  false,  "mikenas", "vladas" },
{ "Milov, Vadim",             2000, false,  false,  "milov", "vadim" },
{ "Miroshnichenko, Evgenij",  2005, false,  false,  "miroshnichenko", "evgenij" },
{ "Moiseenko, Alexander",     2005, false,  false,  "moiseenko", "alexander" },
{ "Morozevich, Alexander",    2000, false,  false,  "morozevich", "alexander" },
{ "Motylev, Alexander",       2005, false,  false,  "motylev", "alexander" },
{ "Movsesian, Sergei",        2000, false,  false,  "movsesian", "sergei" },
{ "Naiditsch, Arkadij",       2005, false,  false,  "naiditsch", "arkadij" },
{ "Najdorf, Miguel",          1950, true,   false,  "najdorf", "miguel" },
{ "Najer, Evgeniy",           2005, false,  false,  "najer", "evgeniy" },
{ "Nakamura, Hikaru",         2005, false,  false,  "nakamura", "hikaru" },
{ "Navara, David",            2005, false,  false,  "navara", "david" },
{ "Negi, Parimarjan",         2015, false,  false,  "negi", "parimarjan" },
{ "Nepomniachtchi, Ian",      2015, false,  false,  "nepomniachtchi", "ian" },
{ "Ni Hua",                   2005, false,  false,  "ni hua", "" },
{ "Nielsen, Peter Heine",     2000, false,  true,   "nielsen", "peter heine" },
{ "Nikolic, Predrag",         1990, false,  false,  "nikolic", "predrag" },
{ "Nimzowitsch, Aron",        1920, false,  false,  "nimzowitsch", "aron" },
{ "Nisipeanu, Liviu-Dieter",  2005, false,  false,  "nisipeanu", "liviu-dieter" },
{ "Nunn, John",               1990, false,  false,  "nunn", "john" },
{ "Olafsson, Fridrik",        1960, false,  false,  "olafsson", "fridrik" },
{ "Onischuk, Alexander",      2000, false,  false,  "onischuk", "alexander" },
{ "Osnos, Viacheslav",        1970, false,  false,  "osnos", "viacheslav" },
{ "Owen, John",               1880, false,  false,  "owen", "john" },
{ "Pachman, Ludek",           1960, false,  false,  "pachman", "ludek" },
{ "Panno, Oscar",             1960, true,   false,  "panno", "oscar" },
{ "Paulsen, Louis",           1880, false,  false,  "paulsen", "louis" },
{ "Perlis, Julius",           1910, false,  false,  "perlis", "julius" },
{ "Petrosian, Tigran",        1960, false,  false,  "petrosian", "tigran" },
{ "Petrovs, Vladimirs",       1940, false,  false,  "petrovs", "vladimirs" },
{ "Piket, Jeroen",            1995, false,  true,   "piket", "jeroen" },
{ "Pillsbury, Harry",         1900, false,  false,  "pillsbury", "harry" },
{ "Pirc, Vasja",              1935, false,  false,  "pirc", "vasja" },
{ "Polgar, Judit",            2000, false,  false,  "polgar", "judit" },
{ "Polugaevsky, Lev",         1970, false,  false,  "polugaevsky", "lev" },
{ "Ponomariov, Ruslan",       2000, false,  false,  "ponomariov", "ruslan" },
{ "Porat, Yosef",             1940, false,  false,  "porat", "yosef" },
{ "Porat, Yosef",             1940, false,  false,  "porath", "yosef" },
{ "Portisch, Lajos",          1970, true,   false,  "portisch", "lajos" },
{ "Postny, Evgeny",           2015, false,  false,  "postny", "evgeny" },
{ "Przepiorka, Dawid",        1920, false,  false,  "przepiorka", "dawid" },
{ "Psakhis, Lev",             1990, false,  false,  "psakhis", "lev" },
{ "Rabinovich, Ilya",         1940, false,  false,  "rabinovich", "ilya" },
{ "Radjabov, Teimour",        2005, false,  false,  "radjabov", "teimour" },
{ "Ragger, Markus",           2015, false,  false,  "ragger", "markus" },
{ "Ragozin, Viacheslav",      1940, false,  false,  "ragozin", "viacheslav" },
{ "Rapport, Richard",         2015, false,  false,  "rapport", "richard" },
{ "Reshevsky, Samuel",        1945, true,   false,  "reshevsky", "samuel" },
{ "Reti, Richard",            1920, false,  false,  "reti", "richard" },
{ "Ribli, Zoltan",            1980, false,  false,  "ribli", "zoltan" },
{ "Richter, Kurt",            1935, false,  false,  "richter", "kurt" },
{ "Robson, Ray",              2015, false,  false,  "robson", "ray" },
{ "Rodshtein, Maxim",         2015, false,  false,  "rodshtein", "maxim" },
{ "Romanishin, Oleg",         1980, false,  false,  "romanishin", "oleg" },
{ "Rosenthal, Samuel",        1880, false,  false,  "rosenthal", "samuel" },
{ "Rubinstein, Akiba",        1910, false,  false,  "rubinstein", "akiba" },
{ "Rublevsky, Sergei",        2000, false,  false,  "rublevsky", "sergei" },
{ "Sadler, Matthew",          2000, false,  false,  "sadler", "matthew" },
{ "Sadvakasov, Darmen",       2005, false,  false,  "sadvakasov", "darmen" },
{ "Safarli, Eltaj",           2015, false,  false,  "safarli", "eltaj" },
{ "Sakaev, Konstantin",       2000, false,  false,  "sakaev", "konstantin" },
{ "Salov, Valery",            1990, false,  false,  "salov", "valery" },
{ "Salwe, Georg",             1910, false,  false,  "salwe", "georg" },
{ "Saemisch, Friedrich",      1930, false,  false,  "samisch", "friedrich" },
{ "Saemisch, Friedrich",      1930, false,  false,  "saemisch", "friedrich" },
{ "Sargissian, Gabriel",      2015, false,  false,  "sargissian", "gabriel" },
{ "Sasikiran, Krishnan",      2005, false,  false,  "sasikiran", "krishnan" },
{ "Savon, Vladimir",          1970, false,  false,  "savon", "vladimir" },
{ "Sax, Gyula",               1980, false,  false,  "sax", "gyula" },
{ "Schallopp, Emil",          1880, false,  false,  "schallopp", "emil" },
{ "Schiffers, Emanuel",       1900, false,  false,  "schiffers", "emanuel" },
{ "Schlechter, Carl",         1900, false,  false,  "schlechter", "carl" },
{ "Schmid, Lothar",           1970, false,  false,  "schmid", "lothar" },
{ "Schottlander, Arnold",     1880, false,  false,  "schottlander", "arnold" },
{ "Schwarz, Adolf",           1880, false,  false,  "schwarz", "adolf" },
{ "Seirawan, Yasser",         1990, false,  false,  "seirawan", "yasser" },
{ "Shipov, Sergei",           2000, false,  false,  "shipov", "sergei" },
{ "Shirov, Alexei",           2000, false,  false,  "shirov", "alexei" },
{ "Short, Nigel",             1990, false,  false,  "short", "nigel" },
{ "Showalter, Jackson",       1900, false,  false,  "showalter", "jackson" },
{ "Smejkal, Jan",             1980, false,  false,  "smejkal", "jan" },
{ "Smirin, Ilia",             2000, false,  false,  "smirin", "ilia" },
{ "Smirnov, Pavel",           2005, false,  false,  "smirnov", "pavel" },
{ "Smyslov, Vassily",         1950, true,   false,  "smyslov", "vassily" },
{ "So, Wesley",               2015, false,  false,  "so", "wesley" },
{ "Sokolov, Ivan",            2000, false,  false,  "sokolov", "ivan" },
{ "Spassky, Boris",           1970, true,   false,  "spassky", "boris" },
{ "Speelman, Jonathan",       1990, false,  false,  "speelman", "jonathan" },
{ "Spielmann, Rudolf",        1920, false,  false,  "spielmann", "rudolf" },
{ "Stahlberg, Gideon",        1940, false,  false,  "stahlberg", "gideon" },
{ "Stein, Leonid",            1970, false,  false,  "stein", "leonid" },
{ "Steiner, Endre",           1930, false,  false,  "steiner", "endre" },
{ "Steiner, Lajos",           1935, false,  false,  "steiner", "lajos" },
{ "Steinitz, Wilhelm",        1890, false,  false,  "steinitz", "wilhelm" },
{ "Stoltz, Gosta",            1935, false,  false,  "stoltz", "gosta" },
{ "Sturua, Zurab",            1990, false,  false,  "sturua", "zurab" },
{ "Suetin, Alexey",           1970, false,  false,  "suetin", "alexey" },
{ "Sultan Khan, Mir",         1935, false,  false,  "sultan khan", "mir" },
{ "Sutovsky, Emil",           2005, false,  false,  "sutovsky", "emil" },
{ "Sveshnikov, Evgeny",       1980, false,  false,  "sveshnikov", "evgeny" },
{ "Svidler, Peter",           2000, false,  false,  "svidler", "peter" },
{ "Szabo, Laszlo",            1950, true,   false,  "szabo", "laszlo" },
{ "Taimanov, Mark",           1960, true,   false,  "taimanov", "mark" },
{ "Tal, Mikhail",             1970, false,  false,  "tal", "mikhail" },
{ "Tarrasch, Siegbert",       1900, false,  false,  "tarrasch", "siegbert" },
{ "Tartakower, Saviely",      1920, true,   false,  "tartakower", "saviely" },
{ "Teichmann, Richard",       1910, false,  false,  "teichmann", "richard" },
{ "Timman, Jan",              1980, true,   false,  "timman", "jan" },
{ "Timofeev, Artyom",         2005, false,  false,  "timofeev", "artyom" },
{ "Tiviakov, Sergei",         2005, false,  false,  "tiviakov", "sergei" },
{ "Tkachiev, Vladislav",      2000, false,  false,  "tkachiev", "vladislav" },
{ "Tolush, Alexander",        1945, false,  false,  "tolush", "alexander" },
{ "Tomashevsky, Evgeny",      2015, false,  false,  "tomashevsky", "evgeny" },
{ "Topalov, Veselin",         2000, false,  false,  "topalov", "veselin" },
{ "Tregubov, Pavel",          2005, false,  false,  "tregubov", "pavel" },
{ "Treybal, Karel",           1930, false,  false,  "treybal", "karel" },
{ "Trifunovic, Petar",        1950, false,  false,  "trifunovic", "petar" },
{ "Tseshkovsky, Vitaly",      1980, false,  false,  "tseshkovsky", "vitaly" },
{ "Tukmakov, Vladimir",       1980, false,  false,  "tukmakov", "vladimir" },
{ "Turover, Isador",          1930, false,  false,  "turover", "isador" },
{ "Uhlmann, Wolfgang",        1960, true,   false,  "uhlmann", "wolfgang" },
{ "Unzicker, Wolfgang",       1960, true,   false,  "unzicker", "wolfgang" },
{ "Vachier-Lagrave, Maxime",  2015, false,  false,  "vachier-lagrave", "maxime" },
{ "Vachier-Lagrave, Maxime",  2015, false,  false,  "vachier lagrave", "maxime" },
{ "Vaganian, Rafael",         1980, true,   false,  "vaganian", "rafael" },
{ "Vajda, Arpad",             1930, false,  false,  "vajda", "arpad" },
{ "Vallejo Pons, Francisco",  2005, false,  false,  "vallejo pons", "francisco" },
{ "van Wely, Loek",           2000, false,  false,  "van wely", "loek" },
{ "Vescovi, Giovanni",        2005, false,  false,  "vescovi", "giovanni" },
{ "Vidmar, Milan",            1920, false,  false,  "vidmar", "milan" },
{ "Vitiugov, Nikita",         2015, false,  false,  "vitiugov", "nikita" },
{ "Vladimirov, Evgeny",       1990, false,  false,  "vladimirov", "evgeny" },
{ "Voigt, Hermann",           1900, false,  false,  "voigt", "hermann" },
{ "Volokitin, Andrei",        2005, false,  false,  "volokitin", "andrei" },
{ "von Bardeleben, Curt",     1890, false,  false,  "von bardeleben", "curt" },
{ "Vyzmanavin, Alexey",       1990, false,  false,  "vyzmanavin", "alexey" },
{ "Walbrodt, Carl",           1900, false,  false,  "walbrodt", "carl" },
{ "Wang Hao",                 2015, false,  false,  "wang hao", "" },
{ "Wang Yue",                 2015, false,  false,  "wang yue", "" },
{ "Wei Yi",                   2015, false,  false,  "wei yi", "" },
{ "Weiss, Miksa",             1890, false,  true,   "weiss", "miksa" },
{ "Winawer, Szymon",          1880, false,  true,   "winawer", "szymon" },
{ "Wojtaszek, Radoslaw",      2015, false,  false,  "wojtaszek", "radoslaw" },
{ "Yates, Frederick",         1930, false,  false,  "yates", "frederick" },
{ "Ye Jiangchuan",            2005, false,  false,  "ye jiangchuan", "" },
{ "Yu Yangyi",                2015, false,  false,  "yu yangyi", "" },
{ "Yudasin, Leonid",          1990, false,  false,  "yudasin", "leonid" },
{ "Zhigalko, Sergei",         2015, false,  false,  "zhigalko", "sergei" },
{ "Znosko-Borovsky, Eugene",  1920, false,  false,  "znosko-borovsky", "eugene" },
{ "Znosko-Borovsky, Eugene",  1920, false,  false,  "znosko borovsky", "eugene" },
{ "Zukertort, Johannes",      1880, false,  false,  "zukertort", "johannes" },
{ "Zvjaginsev, Vadim",        2000, false,  false,  "zvjaginsev", "vadim" }
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
        if( logfile )
            fprintf( logfile, "%s", buf );
    }
    sprintf( buf, "games generated=%d\n", players_database_total );
    cprintf( "%s", buf );
    if( logfile )
        fprintf( logfile, "%s", buf );
}

static int candidate( bool &warning, const char *name )
{
    warning = false;
    //if( strstr(name,"Ding Liren") )
    //    cprintf( "Ding Liren\n" );
    char surname[1000];
    int i=0;
    const char *q = name;
    char c='\0';
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
    char second_character = '\0';

    // No initial ?
    if( c != ',' )  
    {
        q--;
        i--;
        while( i>=0 && *q==' ')
        {
            surname[i--] = '\0';
            q--;
        }
    }

    // Else look for name after comma
    else
    {
        while( *q )
        {
            char d = *q++;
            if( isascii(d) && isalpha(d) )
            {
                initial = tolower(d);
                d = *q++;
                if( isascii(d) && isalpha(d) )
                {
                    second_character = tolower(d);
                }
                break;
            }
        }
    }
    std::map<std::string,int>::iterator it = lookup.find(surname);
    if( it != lookup.end() && players[it->second].extra[0]==initial )
    {
        if( second_character && players[it->second].extra[1] != second_character )
        {
            // filter out Tigran Petrosian Junior, Edward Lasker and Krum Georgiev
            if(  0==strcmp(surname,"lasker")  || 0==strcmp(surname,"georgiev") )
                warning = true;
            if( 0==strcmp(surname,"petrosian")  )
				return -1;
        }
        return it->second;
    }
    else
        return -1;
}

static void players_database( const char *event, const char *site, const char *date, const char *round,
                       const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                       int nbr_moves, thc::Move *moves, uint64_t *UNUSED(hashes) )
{
    bool warningw;
    bool warningb;
    int candw = candidate( warningw, white );
    int candb = candidate( warningb, black );
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
                char buf3[1000];
                for( int known=0; keep && known<3; known++ )  // known issues
                {
                    switch(known)
                    {
                        case 0: keep   = (0!=strcmp( "Fischer, Robert J", players[candw].full)) ||
                                         (0!=strcmp( "Owen, John", players[candb].full));
                                break;
                        case 1: keep   = (0!=strcmp( "Steinitz, Wilhelm", players[candw].full)) ||
                                         (0!=strcmp( "Short, Nigel", players[candb].full));
                                break;
                        case 2: keep   = (0!=strcmp( "Vajda, Arpad", players[candw].full)) ||
                                         (0!=strcmp( "Carlsen, Magnus", players[candb].full));
                                break;
                    }
                }
                if( !keep )
                {
                    sprintf( buf3, "reject %s-%s %s %d not %s-%s\n", white, black, site, year, players[candw].full, players[candb].full );
                    cprintf( "%s", buf3 );
                    fprintf( logfile, "%s", buf3 );
                }
            }
            if( keep )
            {
                if( candw == candb )
                {
                    sprintf( buf, "reject %s-%s %s %d not both %s?\n", white, black, site, year, players[candw].full );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                    keep = false;
                }
                if( warningw )
                {
                    sprintf( buf, "reject %s-%s %s %d is this really %s?\n", white, black, site, year, players[candw].full );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                    keep = false;
                }
                if( warningb )
                {
                    sprintf( buf, "reject %s-%s %s %d is this really %s?\n", white, black, site, year, players[candb].full );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                    keep = false;
                }
            }
            if( keep )
            {
                if( elow && elow<2300 )
                {
                    sprintf( buf, "check %s-%s %s %d white_elo=%d?\n", white, black, site, year, elow );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                if( elob && elob<2300 )
                {
                    sprintf( buf, "check %s-%s %s %d black_elo=%d?\n", white, black, site, year, elob );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                int peakw = players[candw].year;
                if( year-peakw > (players[candw].long_career?50:30) ) 
                {
                    if( players[candw].draws_false_positives )
                        keep = false;
                    sprintf( buf, "%s(%d) in %d %s > %s-%s\n", players[candw].full, players[candw].year, year, keep? "too old?":"definitely too old rejected", white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                if( peakw-year > (players[candw].long_career?30:20) ) 
                {
                    if( players[candw].draws_false_positives )
                        keep = false;
                    sprintf( buf, "%s(%d) in %d %s > %s-%s\n", players[candw].full, players[candw].year, year, keep? "too young?":"definitely too young rejected", white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
            }
            if( keep )
            {
                int peakb = players[candb].year;
                if( year-peakb > (players[candb].long_career?50:30) ) 
                {
                    if( players[candb].draws_false_positives )
                        keep = false;
                    sprintf( buf, "%s(%d) in %d %s > %s-%s\n", players[candb].full, players[candb].year, year, keep? "too old?":"definitely too old rejected", white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
                if( peakb-year > (players[candb].long_career?30:20) )
                { 
                    if( players[candb].draws_false_positives )
                        keep = false;
                    sprintf( buf, "%s(%d) in %d %s > %s-%s\n", players[candb].full, players[candb].year, year, keep? "too young?":"definitely too young rejected", white, black );
                    cprintf( "%s", buf );
                    fprintf( logfile, "%s", buf );
                }
            }
            if( keep )
            {
                players[candw].count++;
                players[candb].count++;
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
                        char buf3[100];
                        sprintf( buf3, "%d. ", cr.full_move_count );
                        moves_txt += buf3;
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


