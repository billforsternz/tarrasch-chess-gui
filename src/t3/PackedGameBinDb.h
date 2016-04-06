/****************************************************************************
 * Pack a complete game into a single string, BinDb version
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PACKED_GAME_BIN_DB_H
#define PACKED_GAME_BIN_DB_H

#include "CompactGame.h"
#include "BinaryBlock.h"

struct PackedGameBinDbCommonData
{
    BinaryBlock bb;
    std::vector<std::string> players;
    std::vector<std::string> events;
    std::vector<std::string> sites;
};

class PackedGameBinDb
{
private:
    std::string fields;
    
public:
    static PackedGameBinDbCommonData& PackedGameBinDb::GetCommonData();
    bool Empty() { return fields.size() == 0; }
    PackedGameBinDb() {}
    PackedGameBinDb( std::string fields ) { this->fields = fields; }

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
    const char *Blob();
private:
    const char *GetField( int field_nbr );
};

#endif // PACKED_GAME_BIN_DB_H
