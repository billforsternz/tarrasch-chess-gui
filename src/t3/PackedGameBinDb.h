/****************************************************************************
 * Pack a complete game into a single string, BinDb version
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PACKED_GAME_BIN_DB_H
#define PACKED_GAME_BIN_DB_H

#include <vector>
#include <map>
#include "CompactGame.h"
#include "BinaryBlock.h"

struct PackedGameBinDbControlBlock
{
    BinaryBlock bb;
    std::vector<std::string> players;
    std::vector<std::string> events;
    std::vector<std::string> sites;
    std::map<std::string,int> map_players;
    std::map<std::string,int> map_events;
    std::map<std::string,int> map_sites;
};

extern std::vector<PackedGameBinDbControlBlock> bin_db_control_blocks;

class PackedGameBinDb
{
private:
    uint8_t     cb_idx;     // control block idx
    std::string fields;
    
public:
    static int AllocateNewControlBlock();
    static PackedGameBinDbControlBlock& GetControlBlock(int cb_idx);

    bool Empty() { return fields.size() == 0; }
    PackedGameBinDb() {}

    // Create a PackedGameBinDb from binary data read from a .tdb file
    PackedGameBinDb( uint8_t cb_idx, std::string fields ) { this->cb_idx=cb_idx; this->fields=fields; }

    // Create a PackedGameBinDb from game data
    PackedGameBinDb(
        uint8_t     cb_idx,
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
    );

    void Pack( CompactGame &pact );
    void Pack( Roster &r, std::string &blob );
    void Unpack( CompactGame &pact );
    void Unpack( Roster &r, std::string &blob );
    void Unpack( Roster &r );
    void Unpack( std::string &blob );
    const char *White();
    const char *Black();
    const char *Event();
    const char *Site();
    const char *Result();
    const char *Round();
    const char *Date();
    const char *Eco();
    const char *WhiteElo();
    const char *BlackElo();
    const char *Fen() { return NULL; }
    const char *Blob() const
    {
        PackedGameBinDbControlBlock *cb = &bin_db_control_blocks[cb_idx];
        int sz = cb->bb.FrozenSize();
        return fields.c_str() + sz;
    }
	int EventBin()    { return bin_db_control_blocks[cb_idx].bb.Read(0,&fields[0]); }
	int SiteBin()     { return bin_db_control_blocks[cb_idx].bb.Read(1,&fields[0]); }
	int WhiteBin()    { return bin_db_control_blocks[cb_idx].bb.Read(2,&fields[0]); }
	int BlackBin()    { return bin_db_control_blocks[cb_idx].bb.Read(3,&fields[0]); }
    int DateBin()     { return bin_db_control_blocks[cb_idx].bb.Read(4,&fields[0]); }
    int RoundBin()    { return bin_db_control_blocks[cb_idx].bb.Read(5,&fields[0]); }
    int EcoBin()      { return bin_db_control_blocks[cb_idx].bb.Read(6,&fields[0]); }
    int ResultBin()   { return bin_db_control_blocks[cb_idx].bb.Read(7,&fields[0]); }
    int WhiteEloBin() { return bin_db_control_blocks[cb_idx].bb.Read(8,&fields[0]); }
    int BlackEloBin() { return bin_db_control_blocks[cb_idx].bb.Read(9,&fields[0]); }
};

#endif // PACKED_GAME_BIN_DB_H
