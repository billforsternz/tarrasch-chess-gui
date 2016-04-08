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

#define POOL_SIZE 8 // must be power of 2
static PackedGameBinDbCommonData common;
static std::string pool[POOL_SIZE];
static int pool_idx;

PackedGameBinDbCommonData& PackedGameBinDb::GetCommonData()
{
    return common;
}

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
    blob = fields.substr( common.bb.Size() );
}

void PackedGameBinDb::Unpack( Roster &r )
{
    int ievent = common.bb.Read(0,&fields[0]);       // Event
    int isite  = common.bb.Read(1,&fields[0]);       // Site
    int iwhite = common.bb.Read(2,&fields[0]);       // White
    int iblack = common.bb.Read(3,&fields[0]);       // Black
    uint32_t date = common.bb.Read(4,&fields[0]);    // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    int round = common.bb.Read(5,&fields[0]);        // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), common.bb=board(0-1023)
    int eco = common.bb.Read(6,&fields[0]);          // ECO For now 500 codes (9 bits) (A..E)(00..99)
    int result = common.bb.Read(7,&fields[0]);       // Result (2 bits)
    int white_elo = common.bb.Read(8,&fields[0]);    // WhiteElo 12 bits (range 0..4095)
    int black_elo = common.bb.Read(9,&fields[0]);    // BlackElo 12 bits (range 0..4095)
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
    r.event = common.events[ievent];
    r.site  = common.sites[isite];
    r.date  = sdate;
    r.round = sround;
    r.white = common.players[iwhite];
    r.black = common.players[iblack];
    r.result = sresult;
    r.eco    = seco;
    r.white_elo = swhite_elo;
    r.black_elo = sblack_elo;
}

const char *PackedGameBinDb::Event()
{
    int i = common.bb.Read(0,&fields[0]);
    return common.events[i].c_str();
}

const char *PackedGameBinDb::Site()
{
    int i = common.bb.Read(1,&fields[0]);
    return common.sites[i].c_str();
}

const char *PackedGameBinDb::White()
{
    int i = common.bb.Read(2,&fields[0]);
    return common.players[i].c_str();
}

const char *PackedGameBinDb::Black()
{
    int i = common.bb.Read(3,&fields[0]);
    return common.players[i].c_str();
}

const char *PackedGameBinDb::Result()
{
    static std::string sresult; 
    int result = common.bb.Read(7,&fields[0]);       // Result (2 bits)
    Bin2Result(result,sresult);
    return sresult.c_str();
}

const char *PackedGameBinDb::Round()
{
    std::string& sround = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    int round = common.bb.Read(5,&fields[0]);        // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), common.bb=board(0-1023)
    Bin2Round (round,sround);
    return sround.c_str();
}

const char *PackedGameBinDb::Date()
{
    std::string& sdate = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    uint32_t date = common.bb.Read(4,&fields[0]);    // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    Bin2Date  (date,sdate);
    return sdate.c_str();
}

const char *PackedGameBinDb::Eco()
{
    std::string& seco = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    int eco = common.bb.Read(6,&fields[0]);          // ECO For now 500 codes (9 bits) (A..E)(00..99)
    Bin2Eco   (eco,seco);
    return seco.c_str();
}

const char *PackedGameBinDb::WhiteElo()
{
    int white_elo = common.bb.Read(8,&fields[0]);    // WhiteElo 12 bits (range 0..4095)
    std::string& swhite_elo = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    Bin2Elo   (white_elo,swhite_elo);
    return swhite_elo.c_str();
}

const char *PackedGameBinDb::BlackElo()
{
    int black_elo = common.bb.Read(9,&fields[0]);    // BlackElo 12 bits (range 0..4095)
    std::string& sblack_elo = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    Bin2Elo   (black_elo,sblack_elo);
    return sblack_elo.c_str();
}

const char *PackedGameBinDb::Blob()
{
    return &fields[ common.bb.Size() ];
}
