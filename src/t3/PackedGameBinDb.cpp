/****************************************************************************
 * Pack a complete game into a single string, BinDb version
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include <string>
#include "CompressMoves.h"
#include "BinDb.h"
#include "BinaryBlock.h"
#include "PackedGameBinDb.h"
#include "ListableGameBinDb.h"

// later, set all these up appropriately
static BinaryBlock bb;
static std::vector<std::string> players;
static std::vector<std::string> events;
static std::vector<std::string> sites;

// later - Pack
void PackedGameBinDb::Pack( Roster &r, std::string &blob )
{
}

void PackedGameBinDb::Pack( CompactGame &pact )
{
}

void PackedGameBinDb::Unpack( CompactGame &pact )
{
    std::string blob;
    Unpack( pact.r, blob );
    CompressMoves press( pact.GetStartPosition() );
    pact.moves = press.Uncompress( blob );
}


void PackedGameBinDb::Unpack( Roster &r, std::string &blob )
{
    Unpack(blob);
    Unpack(r);
}

void PackedGameBinDb::Unpack( std::string &blob )
{
    blob = fields.substr( bb.Size() );
}

void PackedGameBinDb::Unpack( Roster &r )
{
    int ievent = bb.Read(0);       // Event
    int isite  = bb.Read(1);       // Site
    int iwhite = bb.Read(2);       // White
    int iblack = bb.Read(3);       // Black
    uint32_t date = bb.Read(4);    // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    int round = bb.Read(5);        // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
    int eco = bb.Read(6);          // ECO For now 500 codes (9 bits) (A..E)(00..99)
    int result = bb.Read(7);       // Result (2 bits)
    int white_elo = bb.Read(8);    // WhiteElo 12 bits (range 0..4095)
    int black_elo = bb.Read(9);    // BlackElo 12 bits (range 0..4095)
    std::string sdate;
    std::string sround;  
    std::string seco;    
    std::string sresult; 
    Bin2Date  (date,sdate);
    Bin2Round (round,sround);
    Bin2Eco   (eco,seco);
    Bin2Result(result,sresult);
    std::string swhite_elo;
    std::string sblack_elo;
    Bin2Elo   (white_elo,swhite_elo);
    Bin2Elo   (black_elo,sblack_elo);
    r.event = events[ievent];
    r.site  = sites[isite];
    r.date  = sdate;
    r.round = sround;
    r.white = players[iwhite];
    r.black = players[iblack];
    r.result = sresult;
    r.eco    = seco;
    r.white_elo = swhite_elo;
    r.black_elo = sblack_elo;
}

const char *PackedGameBinDb::Event()
{
    int i = bb.Read(0);
    return events[i].c_str();
}

const char *PackedGameBinDb::Site()
{
    int i = bb.Read(1);
    return sites[i].c_str();
}

const char *PackedGameBinDb::White()
{
    int i = bb.Read(2);
    return players[i].c_str();
}

const char *PackedGameBinDb::Black()
{
    int i = bb.Read(3);
    return players[i].c_str();
}

const char *PackedGameBinDb::Result()
{
    static std::string sresult; 
    int result = bb.Read(7);       // Result (2 bits)
    Bin2Result(result,sresult);
    return sresult.c_str();
}

const char *PackedGameBinDb::Round()
{
    static std::string sround;  
    int round = bb.Read(5);        // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
    Bin2Round (round,sround);
    return sround.c_str();
}

const char *PackedGameBinDb::Date()
{
    static std::string sdate;
    uint32_t date = bb.Read(4);    // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    Bin2Date  (date,sdate);
    return sdate.c_str();
}

const char *PackedGameBinDb::Eco()
{
    static std::string seco;    
    int eco = bb.Read(6);          // ECO For now 500 codes (9 bits) (A..E)(00..99)
    Bin2Eco   (eco,seco);
    return seco.c_str();
}

const char *PackedGameBinDb::WhiteElo()
{
    int white_elo = bb.Read(8);    // WhiteElo 12 bits (range 0..4095)
    static std::string swhite_elo;
    Bin2Elo   (white_elo,swhite_elo);
    return swhite_elo.c_str();
}

const char *PackedGameBinDb::BlackElo()
{
    int black_elo = bb.Read(9);    // BlackElo 12 bits (range 0..4095)
    static std::string sblack_elo;
    Bin2Elo   (black_elo,sblack_elo);
    return sblack_elo.c_str();
}

const char *PackedGameBinDb::Blob()
{
    return &fields[ bb.Size() ];
}

