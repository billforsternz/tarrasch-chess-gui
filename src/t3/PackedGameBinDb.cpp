/****************************************************************************
 * Pack a complete game into a single string, BinDb version
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include <map>
#include <string>
#include "CompressMoves.h"
#include "BinDb.h"
#include "BinaryBlock.h"
#include "PackedGameBinDb.h"
#include "ListableGameBinDb.h"

std::vector<PackedGameBinDbControlBlock> bin_db_control_blocks;
static std::map<std::string,int> unordered_players;
static std::map<std::string,int> unordered_events;
static std::map<std::string,int> unordered_sites;

void PackedGameBinDb::AllocateUnorderedControlBlock( bool clear )
{
    if( clear )
    {
        unordered_players.clear();
        unordered_events.clear();
        unordered_sites.clear();
    }
    if( bin_db_control_blocks.size() == 0 )
    {
        PackedGameBinDbControlBlock cb;
        cb.bb.Next(24);                // Event
        cb.bb.Next(24);                // Site
        cb.bb.Next(24);                // White
        cb.bb.Next(24);                // Black
        cb.bb.Next(19);                // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
        cb.bb.Next(16);                // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
        cb.bb.Next(9);                 // ECO For now 500 codes (9 bits) (A..E)(00..99)
        cb.bb.Next(2);                 // Result (2 bits)
        cb.bb.Next(12);                // WhiteElo 12 bits (range 0..4095)
        cb.bb.Next(12);                // BlackElo
        cb.bb.Freeze();
        bin_db_control_blocks.push_back(cb);
    }
}

int PackedGameBinDb::AllocateNewControlBlock()
{
    if( bin_db_control_blocks.size() == 0 )
        AllocateUnorderedControlBlock(true);
    PackedGameBinDbControlBlock cb;
    bin_db_control_blocks.push_back(cb);
    return bin_db_control_blocks.size() - 1;
}

PackedGameBinDbControlBlock& PackedGameBinDb::GetControlBlock( int cb_idx )
{
    return bin_db_control_blocks[cb_idx];
}

// Insert a PackedGameBinDb into the special unordered control block 0
PackedGameBinDb::PackedGameBinDb(
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
    PackedGameBinDbControlBlock &cb = bin_db_control_blocks[0];
    int event_offset = cb.events.size();
    if( 1 == unordered_events.count(event) )
        event_offset = unordered_events[event];
    else
    {
        unordered_events[event] = event_offset;
        cb.events.push_back(event);
    }
    cb.bb.Write(0,event_offset);           // Event
    int site_offset = cb.sites.size();
    if( 1 == unordered_sites.count(site) )
        site_offset = unordered_sites[site];
    else
    {
        unordered_sites[site] = site_offset;
        cb.sites.push_back(site);
    }
    cb.bb.Write(1,site_offset);            // Site
    int white_offset = cb.players.size();
    if( 1 == unordered_players.count(white) )
        white_offset = unordered_players[white];
    else
    {
        unordered_players[white] = white_offset;
        cb.players.push_back(white);
    }
    cb.bb.Write(2,white_offset);           // White
    int black_offset = cb.players.size();
    if( 1 == unordered_players.count(black) )
        black_offset = unordered_players[black];
    else
    {
        unordered_players[black] = black_offset;
        cb.players.push_back(black);
    }
    cb.bb.Write(3,black_offset);            // Black
    cb.bb.Write(4,date);                    // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    cb.bb.Write(5,round);                   // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
    cb.bb.Write(6,eco);                     // ECO For now 500 codes (9 bits) (A..E)(00..99)
    cb.bb.Write(7,result);                  // Result (2 bits)
    cb.bb.Write(8,white_elo);               // WhiteElo 12 bits (range 0..4095)                                                                 
    cb.bb.Write(9,black_elo);               // BlackElo
    std::string fields = std::string( cb.bb.GetPtr(), cb.bb.FrozenSize() );
    fields += compressed_moves;
    this->cb_idx=0;
    this->fields=fields;
}


#define POOL_SIZE 8 // must be power of 2
static std::string pool[POOL_SIZE];
static int pool_idx;

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
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    blob = fields.substr( cb->bb.Size() );
}

void PackedGameBinDb::Unpack( Roster &r )
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    int ievent = cb->bb.Read(0,&fields[0]);       // Event
    int isite  = cb->bb.Read(1,&fields[0]);       // Site
    int iwhite = cb->bb.Read(2,&fields[0]);       // White
    int iblack = cb->bb.Read(3,&fields[0]);       // Black
    uint32_t date = cb->bb.Read(4,&fields[0]);    // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    int round = cb->bb.Read(5,&fields[0]);        // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
    int eco = cb->bb.Read(6,&fields[0]);          // ECO For now 500 codes (9 bits) (A..E)(00..99)
    int result = cb->bb.Read(7,&fields[0]);       // Result (2 bits)
    int white_elo = cb->bb.Read(8,&fields[0]);    // WhiteElo 12 bits (range 0..4095)
    int black_elo = cb->bb.Read(9,&fields[0]);    // BlackElo 12 bits (range 0..4095)
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
    r.event = cb->events[ievent];
    r.site  = cb->sites[isite];
    r.date  = sdate;
    r.round = sround;
    r.white = cb->players[iwhite];
    r.black = cb->players[iblack];
    r.result = sresult;
    r.eco    = seco;
    r.white_elo = swhite_elo;
    r.black_elo = sblack_elo;
}

const char *PackedGameBinDb::Event()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    int i = cb->bb.Read(0,&fields[0]);
    return cb->events[i].c_str();
}

const char *PackedGameBinDb::Site()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    int i = cb->bb.Read(1,&fields[0]);
    return cb->sites[i].c_str();
}

const char *PackedGameBinDb::White()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    int i = cb->bb.Read(2,&fields[0]);
    return cb->players[i].c_str();
}

const char *PackedGameBinDb::Black()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    int i = cb->bb.Read(3,&fields[0]);
    return cb->players[i].c_str();
}

const char *PackedGameBinDb::Result()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    static std::string sresult; 
    int result = cb->bb.Read(7,&fields[0]);       // Result (2 bits)
    Bin2Result(result,sresult);
    return sresult.c_str();
}

const char *PackedGameBinDb::Round()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    std::string& sround = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    int round = cb->bb.Read(5,&fields[0]);        // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), cb->bb=board(0-1023)
    Bin2Round (round,sround);
    return sround.c_str();
}

const char *PackedGameBinDb::Date()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    std::string& sdate = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    uint32_t date = cb->bb.Read(4,&fields[0]);    // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    Bin2Date  (date,sdate);
    return sdate.c_str();
}

const char *PackedGameBinDb::Eco()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    std::string& seco = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    int eco = cb->bb.Read(6,&fields[0]);          // ECO For now 500 codes (9 bits) (A..E)(00..99)
    Bin2Eco   (eco,seco);
    return seco.c_str();
}

const char *PackedGameBinDb::WhiteElo()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    int white_elo = cb->bb.Read(8,&fields[0]);    // WhiteElo 12 bits (range 0..4095)
    std::string& swhite_elo = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    Bin2Elo   (white_elo,swhite_elo);
    return swhite_elo.c_str();
}

const char *PackedGameBinDb::BlackElo()
{
    PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
    int black_elo = cb->bb.Read(9,&fields[0]);    // BlackElo 12 bits (range 0..4095)
    std::string& sblack_elo = pool[pool_idx++];
    pool_idx &= (POOL_SIZE-1);
    Bin2Elo   (black_elo,sblack_elo);
    return sblack_elo.c_str();
}

