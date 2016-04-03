/****************************************************************************
 * BinDb - A non-sql, compact chess database subsystem
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BINDB_H
#define BINDB_H
#include "Appdefs.h"

bool BinDbOpen( const char *db_file );
void BinDbGetDatabaseVersion( int &version );

bool bin_db_append( const char *fen, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves );

bool TestBinaryBlock();
void WriteOutToFile( FILE *ofile );
bool PgnStateMachine( FILE *pgn_file, int &typ, char *buf, int buflen );

void Pgn2Tdb( const char *infile, const char *outfile );
void Pgn2Tdb( FILE *fin, FILE *fout );
void Tdb2Pgn( const char *infile, const char *outfile );
void Tdb2Pgn( FILE *fin, FILE *fout );

// Use 19 bits with format yyyyyyyyyymmmmddddd
// y year, 10 bits, values are 0=unknown, 1-1000 are years 1501-2500 (so fixed offset of 1500), 1001-1023 are reserved
// m month, 4 bits, values are 0=unknown, 1=January..12=December, 13-15 reserved
// d day,   5 bits, values are valid, 0=unknown, 1-31 = conventional date days
uint32_t Date2Bin( const char *date );
void Bin2Date( uint32_t bin, std::string &date );

// for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
uint16_t Round2Bin( const char *round );
void Bin2Round( uint32_t bin, std::string &round );

// For now 500 codes (9 bits) (A..E)(00..99), 0 (or A00) if unknown
uint16_t Eco2Bin( const char *eco );
void Bin2Eco( uint32_t bin, std::string &eco );

// 4 codes (2 bits)
uint8_t Result2Bin( const char *result );
void Bin2Result( uint32_t bin, std::string &result );

// 12 bits (range 0..4095)
uint16_t Elo2Bin( const char *elo );
void Bin2Elo( uint32_t bin, std::string &elo );

int BitsRequired( int max );
void WriteOutToFile( FILE *ofile );
void ReadStrings( FILE *fin, int nbr_strings, std::vector<std::string> &strings );

#endif  // BINDB_H
