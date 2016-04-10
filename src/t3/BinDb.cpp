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
#include "BinDb.h"

static FILE *bin_file;  //temp

bool BinDbOpen( const char *db_file )
{
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
    WriteOutToFile(fout);
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

struct GameBinary
{
    std::string event;
    std::string site;
    std::string white;
    std::string black;
    uint32_t    date;
    uint16_t    round;
    uint8_t     result;
    uint16_t    eco;
    uint16_t    white_elo;
    uint16_t    black_elo;
    std::string compressed_moves;
    GameBinary
    (
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
};

std::set<std::string> set_player;
std::set<std::string> set_site;
std::set<std::string> set_event;
std::vector<GameBinary> games;

bool bin_db_append( const char *fen, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves )
{
    bool aborted = false;
    static int counter;
    if( (++counter % 10000) == 0 )
        cprintf( "%d games read from input .pgn so far\n", counter );
    if( fen )
        return false;

    CompressMoves press;
    std::vector<thc::Move> v(moves,moves+nbr_moves);
    std::string compressed_moves = press.Compress(v);
    GameBinary gp1
    ( 
        std::string(event),
        std::string(site),
        std::string(white),
        std::string(black),
        Date2Bin(date),
        Round2Bin(round),
        Result2Bin(result),
        Eco2Bin(eco),
        Elo2Bin(white_elo),
        Elo2Bin(black_elo),
        compressed_moves
    );
    set_event.insert(gp1.event);
    set_site.insert(gp1.site);
    set_player.insert(gp1.white);
    set_player.insert(gp1.black);
    games.push_back( gp1 );
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
void WriteOutToFile( FILE *ofile )
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
    while( it != set_player.end() )
    {
        std::string str = *it;
        map_player[str] = idx++;
        const char *s = str.c_str();
        fwrite( s, strlen(s)+1, 1, ofile );
        it++;
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
    static int nbr_games_written;
    for( int i=0; i<fh.nbr_games; i++ )
    {
        GameBinary *ptr = &games[i];
        //std::set<std::string>::iterator it = set_player.find(ptr->white);
        //int white_offset = std::distance(player_begin,it);
        int white_offset = map_player[ptr->white];
        //it = set_player.find(ptr->black);
        //int black_offset = std::distance(player_begin,it);
        int black_offset = map_player[ptr->black];
        //it = set_event.find(ptr->event);
        //int event_offset = std::distance(event_begin,it);
        int event_offset = map_event[ptr->event];
        //it = set_site.find(ptr->site);
        //int site_offset = std::distance(site_begin,it);
        int site_offset = map_site[ptr->site];
        bb.Write(0,event_offset);       // Event
        bb.Write(1,site_offset);        // Site
        bb.Write(2,white_offset);       // White
        bb.Write(3,black_offset);       // Black
        bb.Write(4,ptr->date);          // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
        bb.Write(5,ptr->round);         // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
        bb.Write(6,ptr->eco);           // ECO For now 500 codes (9 bits) (A..E)(00..99)
        bb.Write(7,ptr->result);        // Result (2 bits)
        bb.Write(8,ptr->white_elo);     // WhiteElo 12 bits (range 0..4095)
        bb.Write(9,ptr->black_elo);     // BlackElo
        fwrite( bb.GetPtr(), bb_sz, 1, ofile );
        int n = ptr->compressed_moves.length() + 1;
        const char *cstr = ptr->compressed_moves.c_str();
        fwrite( cstr, n, 1, ofile );
        if( (++nbr_games_written % 10000) == 0 )
            cprintf( "%d games written to compressed file so far\n", nbr_games_written );
    }
    cprintf( "%d games written to compressed file\n", nbr_games_written );
}

std::vector<std::string> players;
std::vector<std::string> events;
std::vector<std::string> sites;

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

void BinDbLoadAllGames(  std::vector< smart_ptr<ListableGame> > &mega_cache, int &background_load_permill, bool &kill_background_load )
{
    PackedGameBinDbCommonData& common = PackedGameBinDb::GetCommonData();
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
        ListableGameBinDb info( i, blob );
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

