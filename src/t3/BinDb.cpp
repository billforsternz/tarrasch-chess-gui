/****************************************************************************
 * BinDb - A non-sql, compact chess database subsystem
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include <set>
#include <map>
#include <time.h> // time_t
#include <stdio.h>
#include <stdarg.h>
#include "CompressMoves.h"
#include "PgnRead.h"
#include "CompactGame.h"
#include "PackedGameBinDb.h"
#include "ListableGameBinDb.h"
#include "BinDb.h"

static FILE *bin_file;  //temp

// GameBinary stores the game info temporarily as we build databases - make it
//  a ListableGame so we can temporarily combine GameBinary objects in the same
//  array as other games when we are appending to a database
class GameBinary : public ListableGameBinDb
{
public:
    GameBinary() {}
    GameBinary
    (
        int         game_id,
        std::string event,
        std::string site,
        std::string white,
        std::string black,
        uint32_t    date,
        uint16_t    round,
        uint8_t     result,
        uint16_t    eco,
        uint16_t    white_elo,
        uint16_t    black_elo,
        std::string compressed_moves
    )
    {
        this->game_id          = game_id;
        this->event            = event;
        this->site             = site;
        this->white            = white;
        this->black            = black;
        this->date             = date;
        this->round            = round;
        this->result           = result;
        this->eco              = eco;
        this->white_elo        = white_elo;
        this->black_elo        = black_elo;
        this->compressed_moves = compressed_moves;
    }
    virtual const char *White()     { return white.c_str();   }
    virtual const char *Black()     { return black.c_str();   }
    virtual const char *Event()     { return event.c_str();   }
    virtual const char *Site()      { return site.c_str();    }
    virtual int ResultBin()         { return result;   }
    virtual int RoundBin()          { return round;   }
    virtual int DateBin()           { return date;     }
    virtual int EcoBin()            { return eco;      }
    virtual int WhiteEloBin()       { return white_elo; }
    virtual int BlackEloBin()       { return black_elo; }
    virtual const char *CompressedMoves() {  return compressed_moves.c_str(); }
    virtual const std::string &RefCompressedMoves() { return compressed_moves; }
private:
    std::string white;
    std::string black;
    std::string site;
    std::string event;
    uint32_t    date;
    uint16_t    round;
    uint8_t     result;
    uint16_t    eco;
    uint16_t    white_elo;
    uint16_t    black_elo;
    std::string compressed_moves;
};

static bool predicate_sorts_by_id( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 )
{
    return e1->game_id < e2->game_id;
}

static bool predicate_sorts_by_game_moves( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 )
{
    return e1->RefCompressedMoves() < e2->RefCompressedMoves();
}


bool BinDbOpen( const char *db_file )
{
    /*ListableGame g1;
    ListableGameBinDb g2;
    GameBinary g3;
    g1.RefCompressedMoves();
    g2.RefCompressedMoves();
    g3.RefCompressedMoves();
    predicate_sorts_by_game_moves( g2, g3 ); */
    bool is_bin_db=false;
    if( bin_file )
    {
        fclose( bin_file );
        bin_file = NULL;
    }
    bin_file = fopen( db_file, "rb" );
    if( bin_file )
    {
        char buf[3];
        fread( buf, sizeof(buf), 1, bin_file );
        bool sql_file = (buf[0]=='S' && buf[1]=='Q' && buf[2]=='L');
        if( sql_file )
            fclose(bin_file);
        is_bin_db = !sql_file;    // for now we'll just assume if it's not SQl it *is* our binary format
    }
    return is_bin_db;
}

void BinDbClose()
{
    if( bin_file )
    {
        fclose(bin_file);
        bin_file = NULL;
    }
}

void BinDbGetDatabaseVersion( int &version )
{
    version = DATABASE_VERSION_NUMBER_BIN_DB;
}

void Test()
{
    TestBinaryBlock();
    //Pgn2Tdb( "test.pgn", "test.tdb" );
    //Tdb2Pgn( "test.tdb", "test-reconstituted.pgn" );
    Pgn2Tdb( "extract.pgn", "extract.tdb" );
    Tdb2Pgn( "extract.tdb", "extract-reconstituted.pgn" );
    //Pgn2Tdb( "test3.pgn", "test3.tdb" );
    //Tdb2Pgn( "test3.tdb", "test3-reconstituted.pgn" );
}

void Pgn2Tdb( const char *infile, const char *outfile )
{
    FILE *fin = fopen( infile, "rt" );
    if( fin )
    {
        FILE *fout = fopen( outfile, "wb" );
        if( outfile )
        {
            Pgn2Tdb( fin, fout );
            fclose(fout);
        }
        fclose(fin);
    }
}

void Pgn2Tdb( FILE *fin, FILE *fout )
{
    #if 0
    int game=0;  // some code to extract a partial .pgn
    for(;;)
    {
        int typ;
        char buf[2048];
        PgnStateMachine(fin,typ,buf,sizeof(buf)-2);
        if( typ == 'G' )
            game++;
        if( 630758<=game && game<=630791 )
            fputs(buf,fout);
        if( game > 630791 )
            break;
    }
    #else
    PgnRead pr('B',0);
    pr.Process(fin);
    BinDbWriteOutToFile(fout);
    #endif
}

void Tdb2Pgn( const char *infile, const char *outfile )
{
    FILE *fin = fopen( infile, "rb" );
    if( fin )
    {
        FILE *fout = fopen( outfile, "wt" );
        if( outfile )
        {
            // later recover Tdb2Pgn() from git history, Tdb2Pgn( fin, fout );
            fclose(fout);
        }
        fclose(fin);
    }
}

// Use 19 bits with format yyyyyyyyyymmmmddddd
// y year, 10 bits, values are 0=unknown, 1-1000 are years 1501-2500 (so fixed offset of 1500), 1001-1023 are reserved
// m month, 4 bits, values are 0=unknown, 1=January..12=December, 13-15 reserved
// d day,   5 bits, values are valid, 0=unknown, 1-31 = conventional date days
uint32_t Date2Bin( const char *date )
{
    uint32_t dat=0;
    uint32_t yyyy=0, mm=0, dd=0;
    int state=0;
    while( *date && state<3 )
    {
        char c = *date++;
        if( c == '?' )
            c = '0';
        bool is_digit = isascii(c) && isdigit(c);
        if( is_digit )
        {
            dat = dat*10;
            dat += (c-'0');
        }
        else
        {
            switch( state )
            {
                case 0:  state=1;   yyyy=dat;   dat=0;    break;
                case 1:  state=2;   mm=dat;     dat=0;    break;
                case 2:  state=3;   dd=dat;               break;
                default: break;
            }
        }
    }
    switch( state )
    {
        case 0:  yyyy=dat;   break;
        case 1:  mm=dat;     break;
        case 2:  dd=dat;     break;
        default:             break;
    }
    if( yyyy<1501 || 2500<yyyy )
        yyyy = 1500;
    if( mm<1 || 12<mm )
        mm = 0;
    if( dd<1 || 31<dd )
        dd = 0;    // future: possibly add validation for values 29,30, or 31
    return ((yyyy-1500)<<9) + (mm<<5) + dd;
}

void Bin2Date( uint32_t bin, std::string &date )
{
    char buf[80];
    int yyyy = (bin>>9) & 0x3ff;
    if( yyyy > 0 )
        yyyy += 1500;
    int mm   = (bin>>5) & 0x0f;
    int dd   = (bin)    & 0x1f;
    sprintf( buf, "%04d.%02d.%02d", yyyy, mm, dd );
    if( yyyy == 0 )
    {
        buf[0] = '?';
        buf[1] = '?';
        buf[2] = '?';
        buf[3] = '?';
    }
    if( mm == 0 )
    {
        buf[5] = '?';
        buf[6] = '?';
    }
    if( dd == 0 )
    {
        buf[8] = '?';
        buf[9] = '?';
    }
    date = buf;
}


// for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
uint16_t Round2Bin( const char *round )
{
    if( !round )
        return 0;
    uint16_t rnd=0,brd=0,bin=0;
    int state=0;
    while( *round && state<2 )
    {
        char c = *round++;
        if( c == '?' )
            c = '0';
        bool is_digit = isascii(c) && isdigit(c);
        if( is_digit )
        {
            bin = (bin*10);
            bin += (c-'0');
        }
        else
        {
            switch( state )
            {
                case 0:  state=1; rnd=bin; bin=0; break;
                case 1:  state=2; brd=bin; bin=0; break;
                default: break;
            }
        }
    }
    switch( state )
    {
        case 0:  rnd = bin;  break;
        case 1:  brd = bin;  break;
        default: break;
    }
    if( rnd > 63 )
        rnd = 63;
    if( brd > 1023 )
        brd = 1023;
    bin = (rnd<<10) + brd;
    return bin;
}

void Bin2Round( uint32_t bin, std::string &round )
{
    char buf[80];
    int r = (bin>>10) & 0x3f;
    int b = (bin) & 0x3ff;
    if( b == 0 )
        sprintf( buf, "%d", r );
    else
        sprintf( buf, "%d.%d", r, b );
    if( r == 0 )
        buf[0] = '?';
    round = buf;
}

// For now 500 codes (9 bits) (A..E)(00..99), 0 (or A00) if unknown
uint16_t Eco2Bin( const char *eco )
{
    if( !eco )
        return 0;
    uint16_t bin=0;
    if( 'A'<=eco[0] && eco[0]<='E' &&
        '0'<=eco[1] && eco[1]<='9' &&
        '0'<=eco[2] && eco[2]<='9'
      )
    {
        bin = (eco[0]-'A')*100 + (eco[1]-'0')*10 + (eco[2]-'0');
    }
    return bin;
}

void Bin2Eco( uint32_t bin, std::string &eco )
{
    char buf[4];
    int hundreds = bin/100;         // eg bin = 473 -> 4
    int tens     = (bin%100)/10;    // eg bin = 473 -> 73 -> 7
    int ones     = (bin%10);        // eg bin = 473 -> 3
    buf[0] = 'A'+hundreds;
    buf[1] = '0'+tens;
    buf[2] = '0'+ones;
    buf[3] = '\0';
    eco = buf;
}

// 4 codes (2 bits)
uint8_t Result2Bin( const char *result )
{
    if( !result )
        return 0;
    uint8_t bin=0;
    if( 0 == strcmp(result,"*") )
        bin = 0;
    else if( 0 == strcmp(result,"1-0") )
        bin = 1;
    else if( 0 == strcmp(result,"0-1") )
        bin = 2;
    else if( 0 == strcmp(result,"1/2-1/2") )
        bin = 3;
    return bin;
}

void Bin2Result( uint32_t bin, std::string &result )
{
    switch( bin )
    {
        default:
        case 0:     result = "*";          break;
        case 1:     result = "1-0";        break;
        case 2:     result = "0-1";        break;
        case 3:     result = "1/2-1/2";    break;
    }
}


// 12 bits (range 0..4095)
uint16_t Elo2Bin( const char *elo )
{
    if( !elo )
        return 0;
    uint16_t bin=0;
    bin = atoi(elo);
    if( bin > 4095 )
        bin = 4095;
    else if( bin < 0 )
        bin = 0;
    return bin;
}

void Bin2Elo( uint32_t bin, std::string &elo )
{
    char buf[80];
    if( bin > 4095 )
        bin = 4095;
    else if( bin < 0 )
        bin = 0;
	if( bin )
		sprintf( buf, "%d", bin );
	else
		buf[0] = '\0';
    elo = buf;
}

static std::set<std::string> set_player;
static std::set<std::string> set_site;
static std::set<std::string> set_event;
static std::vector< smart_ptr<ListableGame> > games;

static int game_counter;
bool bin_db_append( const char *fen, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves )
{
    bool aborted = false;
    int id = game_counter;
    if( (++game_counter % 10000) == 0 )
        cprintf( "%d games read from input .pgn so far\n", game_counter );
    if( fen )
        return false;
    if( nbr_moves < 3 )    // skip 'games' with zero, one or two moves
        return false;           // not inserted, not error
    int elo_w = Elo2Bin(white_elo);
    int elo_b = Elo2Bin(black_elo);
    if( 0<elo_w && elo_w<2000 && 0<elo_b && elo_b<2000 )
        // if any elo information, need at least one good player
        return false;   // not inserted, not error
    const char *s =  white;
    while( *s==' ' )
        s++;
    if( *s == '\0' )
        return false;
    s =  black;
    while( *s==' ' )
        s++;
    if( *s == '\0' )
        return false;

    CompressMoves press;
    std::vector<thc::Move> v(moves,moves+nbr_moves);
    std::string compressed_moves = press.Compress(v);
    std::string sevent(event);
    std::string ssite(site);
    std::string swhite(white);
    std::string sblack(black);
    GameBinary gb
    ( 
        id,
        sevent,
        ssite,
        swhite,
        sblack,
        Date2Bin(date),
        Round2Bin(round),
        Result2Bin(result),
        Eco2Bin(eco),
        elo_w,
        elo_b,
        compressed_moves
    );
    set_event.insert(sevent);
    set_site.insert(ssite);
    set_player.insert(swhite);
    set_player.insert(sblack);
    make_smart_ptr( GameBinary, new_gb, gb );
    games.push_back( std::move(new_gb) );
    return aborted;
}

struct FileHeader
{
    int version;
    int nbr_players;
    int nbr_events;
    int nbr_sites;
    int nbr_games;
};

bool TestBinaryBlock()
{
    bool ok;
    BinaryBlock bb;
    bb.Next(3);
    int n3 = bb.Size();
    bb.Next(20);
    int n23 = bb.Size();
    bb.Next(2);
    int n25 = bb.Size();
    bb.Next(8);
    int n33 = bb.Size();
    bb.Next(7);
    int n40 = bb.Size();

    bb.Write( 0, 6 );           // 3 bits
    bb.Write( 1, 0xabcde );     // 20 bits
    bb.Write( 2, 3 );           // 2 bits
    bb.Write( 3, 250 );         // 8 bits
    bb.Write( 4, 0 );           // 7 bits

    uint32_t x3  = bb.Read(0);
    uint32_t x20 = bb.Read(1);
    uint32_t x2  = bb.Read(2);
    uint32_t x8  = bb.Read(3);
    uint32_t x7  = bb.Read(4);
    ok = (x3==6 && x20==0xabcde && x2==3 && x8==250 && x7==0);
    cprintf( "%s: x3=%u, x20=%u, x2=%u, x8=%u, x7=%u\n", ok?"OK":"FAIL", x3, x20, x2, x8, x7 );
    if( !ok )
        return ok;
    ok = (n3==1 && n23==3 && n25==4 && n33==5 && n40==5);
    cprintf( "%s: n3=%d, n23=%d, n25=%d, n33=%d, n40=%d\n", ok?"OK":"FAIL", n3, n23, n25, n33, n40 );
    if( !ok )
        return ok;

    // reverse order
    //bb.Write( 4, 0 );           // 7 bits
    bb.Write( 3, 250 );         // 8 bits
    //bb.Write( 2, 3 );           // 2 bits
    //bb.Write( 1, 0xabcde );     // 20 bits
    //bb.Write( 0, 6 );           // 3 bits

    x3  = bb.Read(0);
    x20 = bb.Read(1);
    x2  = bb.Read(2);
    x8  = bb.Read(3);
    x7  = bb.Read(4);
    ok = (x3==6 && x20==0xabcde && x2==3 && x8==250 && x7==0);
    if( !ok )
        return ok;
    cprintf( "%s: x3=%u, x20=%u, x2=%u, x8=%u, x7=%u\n", ok?"OK":"FAIL", x3, x20, x2, x8, x7 );
    ok = (n3==1 && n23==3 && n25==4 && n33==5 && n40==5);
    cprintf( "%s: n3=%d, n23=%d, n25=%d, n33=%d, n40=%d\n", ok?"OK":"FAIL", n3, n23, n25, n33, n40 );
    return ok;
}

int BitsRequired( int max )
{
    //assert( max < 0x1000000 );
    int n = 24;
    if( max < 0x02 )
        n = 1;
    else if( max < 0x04  )
        n = 2;
    else if( max < 0x08 )
        n = 3;
    else if( max < 0x10 )
        n = 4;
    else if( max < 0x20 )
        n = 5;
    else if( max < 0x40 )
        n = 6;
    else if( max < 0x80 )
        n = 7;
    else if( max < 0x100 )
        n = 8;
    else if( max < 0x200 )
        n = 9;
    else if( max < 0x400  )
        n = 10;
    else if( max < 0x800 )
        n = 11;
    else if( max < 0x1000 )
        n = 12;
    else if( max < 0x2000 )
        n = 13;
    else if( max < 0x4000 )
        n = 14;
    else if( max < 0x8000 )
        n = 15;
    else if( max < 0x10000 )
        n = 16;
    else if( max < 0x20000 )
        n = 17;
    else if( max < 0x40000 )
        n = 18;
    else if( max < 0x80000 )
        n = 19;
    else if( max < 0x100000 )
        n = 20;
    else if( max < 0x200000 )
        n = 21;
    else if( max < 0x400000 )
        n = 22;
    else if( max < 0x800000 )
        n = 23;
    return n;
}

std::map<std::string,int> map_player;
std::map<std::string,int> map_event;
std::map<std::string,int> map_site;

void BinDbWriteClear()
{
    game_counter = 0;
    set_player.clear();
    set_site.clear();
    set_event.clear();
    games.clear();
    map_player.clear();
    map_event.clear();
    map_site.clear();
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

static bool DupDetect( smart_ptr<ListableGame> p1, std::vector<std::string> &white_tokens1, std::vector<std::string> &black_tokens1, smart_ptr<ListableGame> p2 )
{
    bool white_match = (0 == strcmp(p1->White(),p2->White()) );
    if( !white_match )
        white_match = IsPlayerMatch(p2->White(),white_tokens1);
    bool black_match = (0 == strcmp(p1->Black(),p2->Black()) );
    if( !black_match )
        black_match = IsPlayerMatch(p2->Black(),black_tokens1);
    bool result_match = (p1->ResultBin() == p2->ResultBin() );
    bool dup = (white_match && black_match && /*date_match &&*/ result_match);
    return dup;
}

bool BinDbDuplicateRemoval( ProgressBar *pb )
{
    pb->DrawNow();
    std::sort( games.begin(), games.end(), predicate_sorts_by_game_moves );
    int nbr_games = games.size();
    bool in_dups=false;
    int start;
    for( int i=0; i<nbr_games-1; i++ )
    {
        if( pb->Perfraction( i,nbr_games-1) )
            return false;   // abort
        bool more = (i+1<games.size()-1);
        bool next_matches = (0 == strcmp(games[i]->CompressedMoves(),games[i+1]->CompressedMoves()) );
        bool eval_dups = (in_dups && !more);
        if( !in_dups )
        {
            if( next_matches )
            {
                start = i;
                in_dups = true;
            }
        }
        else
        {
            if( !next_matches )
            {
                in_dups = false;
                eval_dups = true;
            }
        }

        // For subranges with identical moves, mark dups with id -1
        if( eval_dups )
        {
            int end = i+1;
            std::sort( games.begin()+start, games.begin()+end, predicate_sorts_by_id );
            static int trigger = 3;
            if( false ) //end-start > 8 ) //trigger > 0 )
            {
                printf( "Eval Dups in\n" );
                for( int idx=start; idx<end; idx++ )
                {
                    smart_ptr<ListableGame> p = games[idx];
                    cprintf( "%d: %s-%s, %s %d ply\n", p->game_id, p->White(), p->Black(), p->Event(), strlen(p->CompressedMoves()) );
                }
            }

            for( int idx=start; idx<end; idx++ )
            {
                smart_ptr<ListableGame> p = games[idx];
                if( p->game_id != -1 )
                {
                    std::vector<std::string> white_tokens;
                    Split(p->White(),white_tokens);
                    std::vector<std::string> black_tokens;
                    Split(p->Black(),black_tokens);
                    for( int j=idx+1; j<end; j++ )
                    {
                        smart_ptr<ListableGame> q = games[j];
                        if( q->game_id!=-1 && DupDetect(p,white_tokens,black_tokens,q) )
                            q->game_id = -1;    
                    }
                }
            }

            if( false ) //end-start > 8 ) //trigger > 0 )
            {
                //trigger--;
                printf( "Eval Dups out\n" );
                for( int idx=start; idx<end; idx++ )
                {
                    smart_ptr<ListableGame> p = games[idx];
                    cprintf( "%d: %s-%s, %s %d ply\n", p->game_id, p->White(), p->Black(), p->Event(), strlen(p->CompressedMoves()) );
                }
            }
        }
    }
    std::sort( games.begin(), games.end(), predicate_sorts_by_id );
    int nbr_deleted=0;
    for( int i=0; i<games.size(); i++ )
    {
        if( games[i].get()->game_id == -1 )
            nbr_deleted++;
        else
            break;
    }
    cprintf( "Number of duplicates deleted: %d\n", nbr_deleted );
    games.erase( games.begin(), games.begin()+nbr_deleted );
    return true;
}


// Return bool okay
bool BinDbWriteOutToFile( FILE *ofile, ProgressBar *pb )
{
    std::set<std::string>::iterator it = set_player.begin();
    FileHeader fh;
    fh.nbr_players = std::distance( set_player.begin(), set_player.end() );
    fh.nbr_events  = std::distance( set_event.begin(),  set_event.end() );
    fh.nbr_sites   = std::distance( set_site.begin(),   set_site.end() );
    fh.nbr_games   = games.size();
    cprintf( "%d games, %d players, %d events, %d sites\n", fh.nbr_games, fh.nbr_players, fh.nbr_events, fh.nbr_sites );
    int nbr_bits_player = BitsRequired(fh.nbr_players);
    int nbr_bits_event  = BitsRequired(fh.nbr_events);  
    int nbr_bits_site   = BitsRequired(fh.nbr_sites);   
    cprintf( "%d player bits, %d event bits, %d site bits\n", nbr_bits_player, nbr_bits_event, nbr_bits_site );
    fwrite( &fh, sizeof(fh), 1, ofile );
    int idx=0;
    int total_strings = fh.nbr_players + fh.nbr_events + fh.nbr_sites + fh.nbr_games;
    int nbr_strings_so_far = 0;
    while( it != set_player.end() )
    {
        std::string str = *it;
        map_player[str] = idx++;
        const char *s = str.c_str();
        fwrite( s, strlen(s)+1, 1, ofile );
        it++;
        nbr_strings_so_far++;
        if( pb )
            if( pb->Perfraction( nbr_strings_so_far, total_strings ) )
                return false;   // abort
    }
    it = set_event.begin();
    idx=0;
    while( it != set_event.end() )
    {
        std::string str = *it;
        map_event[str] = idx++;
        const char *s = str.c_str();
        fwrite( s, strlen(s)+1, 1, ofile );
        it++;
        nbr_strings_so_far++;
        if( pb )
            if( pb->Perfraction( nbr_strings_so_far, total_strings ) )
                return false;   // abort
    }
    it = set_site.begin();
    idx=0;
    while( it != set_site.end() )
    {
        std::string str = *it;
        map_site[str] = idx++;
        const char *s = str.c_str();
        fwrite( s, strlen(s)+1, 1, ofile );
        it++;
        nbr_strings_so_far++;
        if( pb )
            if( pb->Perfraction( nbr_strings_so_far, total_strings ) )
                return false;   // abort
    }
    std::set<std::string>::iterator player_begin = set_player.begin();
    std::set<std::string>::iterator player_end   = set_player.end();
    std::set<std::string>::iterator event_begin  = set_event.begin();
    std::set<std::string>::iterator event_end    = set_event.end();
    std::set<std::string>::iterator site_begin   = set_site.begin();
    std::set<std::string>::iterator site_end     = set_site.end();
    BinaryBlock bb;
    bb.Next(nbr_bits_event);    // Event
    bb.Next(nbr_bits_site);     // Site
    bb.Next(nbr_bits_player);   // White
    bb.Next(nbr_bits_player);   // Black
    bb.Next(19);                // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    bb.Next(16);                // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
    bb.Next(9);                 // ECO For now 500 codes (9 bits) (A..E)(00..99)
    bb.Next(2);                 // Result (2 bits)
    bb.Next(12);                // WhiteElo 12 bits (range 0..4095)
    bb.Next(12);                // BlackElo
    int bb_sz = bb.Size();
    cprintf( "bb_sz=%d\n", bb_sz );
    for( int i=0; i<fh.nbr_games; i++ )
    {
        smart_ptr<ListableGame> ptr = games[i];
        int white_offset = map_player[std::string(ptr->White())];
        int black_offset = map_player[std::string(ptr->Black())];
        int event_offset = map_event[std::string(ptr->Event())];
        int site_offset = map_site[std::string(ptr->Site())];
        bb.Write(0,event_offset);           // Event
        bb.Write(1,site_offset);            // Site
        bb.Write(2,white_offset);           // White
        bb.Write(3,black_offset);           // Black
        bb.Write(4,ptr->DateBin());         // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
        bb.Write(5,ptr->RoundBin());        // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
        bb.Write(6,ptr->EcoBin());          // ECO For now 500 codes (9 bits) (A..E)(00..99)
        bb.Write(7,ptr->ResultBin());       // Result (2 bits)
        bb.Write(8,ptr->WhiteEloBin());     // WhiteElo 12 bits (range 0..4095)                                                                 
        bb.Write(9,ptr->BlackEloBin());     // BlackElo
        fwrite( bb.GetPtr(), bb_sz, 1, ofile );
        int n = strlen(ptr->CompressedMoves()) + 1;
        const char *cstr = ptr->CompressedMoves();
        fwrite( cstr, n, 1, ofile );
        if( (i % 10000) == 0 )
            cprintf( "%d games written to compressed file so far\n", i );
        nbr_strings_so_far++;
        if( pb )
            if( pb->Perfraction( nbr_strings_so_far, total_strings ) )
                return false;   // abort
    }
    cprintf( "%d games written to compressed file\n", fh.nbr_games );
    return true;
}

static std::vector<std::string> players;
static std::vector<std::string> events;
static std::vector<std::string> sites;

void ReadStrings( FILE *fin, int nbr_strings, std::vector<std::string> &strings )
{
    for( int i=0; i<nbr_strings; i++ )
    {
        std::string s;
        int ch = fgetc(fin);
        while( ch && ch!=EOF )
        {
            s += static_cast<char>(ch);
            ch = fgetc(fin);
        }
        strings.push_back(s);
        if( ch == EOF )
            break;    
    }
 /*   for( int i=0; i<10; i++ )
        cprintf( "%s\n", strings[i].c_str() );
    cprintf( "...\n" );
    for( int i=10; i>0; i-- )
        cprintf( "%s\n", strings[strings.size()-i].c_str() ); */
}

void BinDbLoadAllGames(  std::vector< smart_ptr<ListableGame> > &mega_cache, int &background_load_permill, bool &kill_background_load, ProgressBar *pb )
{
    int cb_idx = PackedGameBinDb::AllocateNewControlBlock();
    PackedGameBinDbControlBlock& common = PackedGameBinDb::GetControlBlock(cb_idx);
    FileHeader fh;
    FILE *fin = bin_file;   // later allow this to be closed!
    rewind(fin);
    fread( &fh, sizeof(fh), 1, fin );
    cprintf( "%d games, %d players, %d events, %d sites\n", fh.nbr_games, fh.nbr_players, fh.nbr_events, fh.nbr_sites );
    int nbr_bits_player = BitsRequired(fh.nbr_players);
    int nbr_bits_event  = BitsRequired(fh.nbr_events);  
    int nbr_bits_site   = BitsRequired(fh.nbr_sites);   
    cprintf( "%d player bits, %d event bits, %d site bits\n", nbr_bits_player, nbr_bits_event, nbr_bits_site );
    ReadStrings( fin, fh.nbr_players, common.players );
    ReadStrings( fin, fh.nbr_events, common.events );
    ReadStrings( fin, fh.nbr_sites, common.sites );

    common.bb.Next(nbr_bits_event);    // Event
    common.bb.Next(nbr_bits_site);     // Site
    common.bb.Next(nbr_bits_player);   // White
    common.bb.Next(nbr_bits_player);   // Black
    common.bb.Next(19);                // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    common.bb.Next(16);                // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), common.bb=board(0-1023)
    common.bb.Next(9);                 // ECO For now 500 codes (9 bits) (A..E)(00..99)
    common.bb.Next(2);                 // Result (2 bits)
    common.bb.Next(12);                // WhiteElo 12 bits (range 0..4095)
    common.bb.Next(12);                // BlackElo
    common.bb.Freeze();
    int bb_sz = common.bb.FrozenSize();
    int game_count = fh.nbr_games;
    int nbr_games=0;
    int nbr_promotion_games=0;  // later
    for( int i=0; i<game_count && !kill_background_load; i++ )
    {
        if( pb )
            pb->Perfraction(i,game_count);
        char buf[sizeof(common.bb)];
        fread( buf, bb_sz, 1, fin );
        std::string blob(buf,bb_sz);
        int ch = fgetc(fin);
        while( ch && ch!=EOF )
        {
            blob += static_cast<char>(ch);
            ch = fgetc(fin);
        }
        if( ch == EOF )
            cprintf( "Whoops\n" );
        ListableGameBinDb info( cb_idx, i, blob );
        make_smart_ptr( ListableGameBinDb, new_info, info );
        mega_cache.push_back( std::move(new_info) );
        background_load_permill = (i*1000) / (game_count?game_count:1);
        nbr_games++;
        if(
            ((nbr_games%10000) == 0 ) /* ||
                                        (
                                        (nbr_games < 100) &&
                                        ((nbr_games%10) == 0 )
                                        ) */
            )
        {
            cprintf( "%d games (%d include promotion)\n", nbr_games, nbr_promotion_games );
        }
    }
}

