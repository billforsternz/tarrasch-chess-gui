/****************************************************************************
 * BinDb - A non-sql, compact chess database subsystem
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <vector>
#include <set>
#include <map>
#include <time.h> // time_t
#include <stdio.h>
#include <stdarg.h>
#include <wx/filename.h>
#include "Objects.h"
#include "Repository.h"
#include "CompressMoves.h"
#include "PgnRead.h"
#include "CompactGame.h"
#include "PackedGameBinDb.h"
#include "ListableGameBinDb.h"
#include "BinDb.h"

/*

Some Design notes:

This module is basically C rather than C++. This should probably change in the future

The module is principally concerned with reading and writing databases.

Modes of operation
A) Read a database into the games array (in-memory database) within a Database object
B) Read a database [for append] and pgn games [create or append] into the local games array
C) After B) write the local games array out to a new database file

Main Functions
1) bool    BinDbOpen( const char *db_file, std::string &error_msg )
    Opens file for reading, reads header
    Returns bool ok, if !ok set error_msg to explain
2) uint8_t BinDbReadBegin()
    Get ready to read games, returns newly allocated control block
3) bool BinDbLoadAllGames()
    Either A) [array is the so called tiny_db inside a Database object] or B)
    [array is the glocal games array].
    The games in a database are ordered oldest to newest. This is our preferred order always.
    In case A) we reverse the array order after load - to get the normal presentation order (newest games have smallest game_id and come first)
    In case B) we don't reverse - because we are going to append more older to newer games from pgn (game_id isn't actually important we
    now establish a contiguous range of game_ids in BinDbRemoveDuplicatesAndWrite() because it improves ordering before write)
    Returns bool killed, if killed array is not fully loaded
4) void BinDbClose()
    Closes database file after reading in 3)

The following are not needed for A) only for B) and C)
5) bool bin_db_append( const char *fen, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves )
    One game appended in case B)
6) void BinDbNormaliseOrder( uint32_t begin, uint32_t end )
    After all games from one pgn file appended to games array, normalise their order (older first, newer last)
    This function either leaves the games alone or reverses them
7) bool BinDbRemoveDuplicatesAndWrite( std::string &title, int step, FILE *ofile, bool locked, wxWindow *window )
    New in V3.01a - incorporate write file so can do that before writing dups to TarraschDbDuplicate.pgn
8) bool BinDbWriteOutToFile( FILE *ofile, int nbr_to_omit_from_end, bool locked, ProgressBar *pb )
    Now only called by BinDbRemoveDuplicatesAndWrite()
9) void BinDbCreationEnd()
    clears internal games vector
*/

static uint32_t game_id_bottom = 1;	// reserve 0 as a special value
static uint32_t game_id_top    = GAME_ID_SENTINEL-1;

// Database game ids are allocated from the top
uint32_t GameIdAllocateTop( uint32_t count )
{
    if( game_id_top-count <= game_id_bottom )
    {
        game_id_bottom = 1;
        game_id_top    = GAME_ID_SENTINEL-1;
    }
    game_id_top -= count;
    return game_id_top;
}

// Other game ids are allocated from the bottom
uint32_t GameIdAllocateBottom(uint32_t count)
{
	if (game_id_bottom + count >= game_id_top)
	{
		game_id_bottom = 1;
		game_id_top = GAME_ID_SENTINEL - 1;
	}
	uint32_t temp = game_id_bottom;
	game_id_bottom += count;
	return temp;
}


static FILE         *bin_file;      //temp

// The 1200 byte compatibility header - Prepended to a BinDb formatted database file
//  It makes such a file partially compatible to the original versions of TarraschDb
//  which expect a sqlite file - well compatible enough to read the version number and
//  conclude that they can't use the file so that they fail gracefully.
//  The header looks like a SQLite file with a single TABLE as follows;
//  CREATE TABLE description (description TEXT, version INTEGER)
//  In this the description is;
//    "This header allows the alpha SQL based version of Tarrasch V3 to recognise (but grace
//    fully reject) the newer custom in-memory binary database format"
//  And the version is the last byte of the 1200 byte header = 0x03 = 
//  DATABASE_VERSION_NUMBER_BIN_DB

static uint8_t compatibility_header[] =
{
 /*000000*/  0x53, 0x51, 0x4C, 0x69, 0x74, 0x65, 0x20, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74, 0x20, 0x33, 0x00,  //SQLite format 3.
 /*000010*/  0x04, 0x00, 0x01, 0x01, 0x00, 0x40, 0x20, 0x20, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02,  //.....@  ........
 /*000020*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04,  //................
 /*000030*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,  //................
 /*000040*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000050*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,  //................
 /*000060*/  0x00, 0x2D, 0xE6, 0x07, 0x0D, 0x00, 0x00, 0x00, 0x01, 0x03, 0x9F, 0x00, 0x03, 0x9F, 0x00, 0x00,  //.-..............
 /*000070*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000080*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000090*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0000A0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0000B0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0000C0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0000D0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0000E0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0000F0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................ 
 /*000100*/  0x54, 0x44, 0x42, 0x20, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //TDB format......
 /*000110*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000120*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000130*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000140*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000150*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000160*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000170*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000180*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000190*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0001A0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0001B0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0001C0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0001D0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0001E0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0001F0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000200*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000210*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000220*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000230*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000240*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000250*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000260*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000270*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000280*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000290*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0002A0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0002B0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0002C0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0002D0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0002E0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*0002F0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000300*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000310*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000320*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000330*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000340*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000350*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000360*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000370*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000380*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000390*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F,  //..............._
 /*0003A0*/  0x01, 0x07, 0x17, 0x23, 0x23, 0x01, 0x81, 0x05, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x64, 0x65, 0x73,  //...##...tabledes
 /*0003B0*/  0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74,  //criptiondescript
 /*0003C0*/  0x69, 0x6F, 0x6E, 0x02, 0x43, 0x52, 0x45, 0x41, 0x54, 0x45, 0x20, 0x54, 0x41, 0x42, 0x4C, 0x45,  //ion.CREATE TABLE
 /*0003D0*/  0x20, 0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x28, 0x64, 0x65,  // description (de
 /*0003E0*/  0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x54, 0x45, 0x58, 0x54, 0x2C, 0x20,  //scription TEXT,
 /*0003F0*/  0x76, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x20, 0x49, 0x4E, 0x54, 0x45, 0x47, 0x45, 0x52, 0x29,  //version INTEGER)
 /*000400*/  0x0D, 0x00, 0x00, 0x00, 0x01, 0x00, 0x14, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //................
 /*000410*/  0x00, 0x00, 0x00, 0x00, 0x81, 0x19, 0x01, 0x04, 0x82, 0x35, 0x01, 0x54, 0x68, 0x69, 0x73, 0x20,  //.........5.This
 /*000420*/  0x68, 0x65, 0x61, 0x64, 0x65, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x6F, 0x77, 0x73, 0x20, 0x74, 0x68,  //header allows th
 /*000430*/  0x65, 0x20, 0x61, 0x6C, 0x70, 0x68, 0x61, 0x20, 0x53, 0x51, 0x4C, 0x20, 0x62, 0x61, 0x73, 0x65,  //e alpha SQL base
 /*000440*/  0x64, 0x20, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x20, 0x6F, 0x66, 0x20, 0x54, 0x61, 0x72,  //d version of Tar
 /*000450*/  0x72, 0x61, 0x73, 0x63, 0x68, 0x20, 0x56, 0x33, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x63, 0x6F,  //rasch V3 to reco
 /*000460*/  0x67, 0x6E, 0x69, 0x73, 0x65, 0x20, 0x28, 0x62, 0x75, 0x74, 0x20, 0x67, 0x72, 0x61, 0x63, 0x65,  //gnise (but grace
 /*000470*/  0x66, 0x75, 0x6C, 0x6C, 0x79, 0x20, 0x72, 0x65, 0x6A, 0x65, 0x63, 0x74, 0x29, 0x20, 0x74, 0x68,  //fully reject) th
 /*000480*/  0x65, 0x20, 0x6E, 0x65, 0x77, 0x65, 0x72, 0x20, 0x63, 0x75, 0x73, 0x74, 0x6F, 0x6D, 0x20, 0x69,  //e newer custom i
 /*000490*/  0x6E, 0x2D, 0x6D, 0x65, 0x6D, 0x6F, 0x72, 0x79, 0x20, 0x62, 0x69, 0x6E, 0x61, 0x72, 0x79, 0x20,  //n-memory binary
 /*0004A0*/  0x64, 0x61, 0x74, 0x61, 0x62, 0x61, 0x73, 0x65, 0x20, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74,        //database format
 /*0004AF*/  DATABASE_VERSION_NUMBER_LOCKABLE                                                                 //.
};

#define COMPATIBILITY_HEADER_SIZE 1200  // = 0x4b0
static uint32_t compatibility_header_size=COMPATIBILITY_HEADER_SIZE;

// Return bool ok, if !ok set error_msg to explain
bool BinDbOpen( const char *db_file, std::string &error_msg )
{
    bool ok=false;
    error_msg = "";
    if( bin_file )
    {
        fclose( bin_file );
        bin_file = NULL;
    }
    bin_file = fopen( db_file, "rb" );
    if( !bin_file )
    {
        error_msg = "Cannot open  " + std::string(db_file);
    }
    else
    {
        // Read the 1200 byte compatibility header - if present it makes a BinDb formatted
        //  database compatible to the original versions of TarraschDb which expect a sqlite
        //  file - well compatible enough to read the version number and conclude that they
        //  can't use the file so that they fail gracefully.
        unsigned char buf[COMPATIBILITY_HEADER_SIZE];
        int n = fread( buf, 1, sizeof(buf), bin_file );
        if( n != COMPATIBILITY_HEADER_SIZE )
            error_msg = "File  " + std::string(db_file) + " does not appear to be a Tarrasch database file";
        else
        {
            if( 0 == memcmp(&buf[0x100], "TDB format", 10) )  // is this the compatibility header ?
            {
                compatibility_header_size = *( reinterpret_cast<uint32_t *>(&buf[0x0f0]) );
                if( compatibility_header_size<0x10b || compatibility_header_size>COMPATIBILITY_HEADER_SIZE )    // a little future-proofing, support possible
                    compatibility_header_size = COMPATIBILITY_HEADER_SIZE;                                      //  smaller compatibility header size
                int version = buf[compatibility_header_size-1];
                char vtxt[40];
                sprintf( vtxt, "%d", version );
                bool lockable = false;
                if( version == DATABASE_VERSION_NUMBER_BIN_DB )
                    ok = true;
                else if( version < DATABASE_VERSION_NUMBER_BIN_DB )
                {
                    error_msg = "Tarrasch database file " + std::string(db_file) + " uses an old Tarrasch format (DB format =" + std::string(vtxt) + ") and is incompatible with this version of Tarrasch. "
                        "Try Tarrasch V3.03, it might be able to read it. "
                        "If that works, append a small (even empty) pgn to rewrite to a newer format. "
                        "Tarrasch V3.03 can be downloaded from https://triplehappy.com/downloads/portable-tarrasch-v3.03a-g.zip.";
                }
                else if( version == DATABASE_VERSION_NUMBER_LOCKABLE )
                {
                    lockable = true;
                    ok = true;
                }
                else if( version > DATABASE_VERSION_NUMBER_LOCKABLE )
                {
                    error_msg = "Tarrasch database file " + std::string(db_file) + " expects a more recent version of Tarrasch (DB format =" + std::string(vtxt) + "), it is incompatible with this older version of Tarrasch";
                }
            } 
            else if( 0 == memcmp(&buf[0], "SQLite format 3", 15) )  // is it an earlier Tarrasch DB SQL based format ?
            {
                // version = DATABASE_VERSION_NUMBER_LEGACY
                error_msg = "File " + std::string(db_file) + " is an SQLITE file. This version of Tarrasch cannot read it. "
                    "Try Tarrasch V3.03, it might be able to. "
                    "If that works, append a small (even empty) pgn to rewrite to a newer format. "
                    "Tarrasch V3.03 can be downloaded from https://triplehappy.com/downloads/portable-tarrasch-v3.03a-g.zip.";
            }
        }
    }
    if( bin_file && !ok )
    {
        fclose(bin_file);
        bin_file = NULL;
    }
    return ok;
}

void BinDbClose()
{
    if( bin_file )
    {
        fclose(bin_file);
        bin_file = NULL;
    }
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
    BinDbWriteOutToFile(fout,0,false);
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
// d day,   5 bits, values are 0=unknown, 1-31 = conventional date days
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

// For now 500 codes (9 bits) (A..E)(00..99), 0 (or A00) if unknown, 500 if empty
uint16_t Eco2Bin( const char *eco )
{
    if( !eco || !*eco )
        return 500; // 500 is empty
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
    if( bin>499 )
        eco = "";   // 500 is empty
    else
    {
        int hundreds = bin/100;         // eg bin = 473 -> 4
        int tens     = (bin%100)/10;    // eg bin = 473 -> 73 -> 7
        int ones     = (bin%10);        // eg bin = 473 -> 3
        buf[0] = 'A'+hundreds;
        buf[1] = '0'+tens;
        buf[2] = '0'+ones;
        buf[3] = '\0';
        eco = buf;
    }
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

// The games vector is used exclusively for creating and appending to databases..
static uint8_t bin_db_append_cb_idx;
static std::vector< smart_ptr<ListableGame> > games;
std::vector< smart_ptr<ListableGame> > &BinDbLoadAllGamesGetVector() { return games; }
static int game_counter;

// ..When we've finished creating and appending to databases, save memory by clearing it (new in V3.01)
void BinDbCreationEnd()
{
    games.clear();  // In the future consider moving the games to the in memory database instead of reloading
                    //  if the user answers yes to "would you like to use the new database now"
}

// Start reading BinDb game data
uint8_t BinDbReadBegin()
{
    game_counter = 0;
    games.clear();
    uint8_t cb_idx = PackedGameBinDb::AllocateNewControlBlock();
    bin_db_append_cb_idx = cb_idx;
    PackedGameBinDbControlBlock& cb = PackedGameBinDb::GetControlBlock(cb_idx);
    cb.bb.Next(24);   // Event
    cb.bb.Next(24);   // Site
    cb.bb.Next(24);   // White
    cb.bb.Next(24);   // Black
    cb.bb.Next(19);   // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    cb.bb.Next(16);   // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), bb=board(0-1023)
    cb.bb.Next(9);    // ECO For now 500 codes (9 bits) (A..E)(00..99)
    cb.bb.Next(2);    // Result (2 bits)
    cb.bb.Next(12);   // WhiteElo 12 bits (range 0..4095)
    cb.bb.Next(12);   // BlackElo
    cb.bb.Freeze();
    return cb_idx;
}

bool bin_db_append( const char *fen, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves )
{
    bool aborted = false;
    if( (++game_counter % 10000) == 0 )
        cprintf( "%d games read from input .pgn so far\n", game_counter );
    if( fen )
        return false;
    if( nbr_moves < 3 )    // skip 'games' with zero, one or two moves
        return false;           // not inserted, not error
	int yyyy=0;		// declare here just to avoid unnecessary recalc
	uint32_t date_bin=0;
	int elo_w = Elo2Bin(white_elo);
	int elo_b = Elo2Bin(black_elo);
	bool elo_cutoff_ignore = objs.repository->database.m_elo_cutoff_ignore;
	if( !elo_cutoff_ignore )
	{
		int elo_cutoff		       = objs.repository->database.m_elo_cutoff;
		int elo_cutoff_before_year = objs.repository->database.m_elo_cutoff_before_year;
		bool elo_cutoff_one	   = objs.repository->database.m_elo_cutoff_one;
		bool elo_cutoff_both   = objs.repository->database.m_elo_cutoff_both;
		bool elo_cutoff_fail   = objs.repository->database.m_elo_cutoff_fail;
		bool elo_cutoff_pass   = objs.repository->database.m_elo_cutoff_pass;
		bool elo_cutoff_pass_before = objs.repository->database.m_elo_cutoff_pass_before;
		bool elo_w_pass = (elo_w >= elo_cutoff);
		bool elo_b_pass = (elo_b >= elo_cutoff);
		if( elo_w==0 || elo_b==0 )
		{
			if( elo_cutoff_fail )
			{
				if( elo_w == 0 )
					elo_w_pass = false;
				if( elo_b == 0 )
					elo_b_pass = false;
			}
			else if( elo_cutoff_pass )
			{
				if( elo_w == 0 )
					elo_w_pass = true;
				if( elo_b == 0 )
					elo_b_pass = true;
			}
			else if( elo_cutoff_pass_before )
			{
				date_bin = Date2Bin(date);
				// Use 19 bits with format yyyyyyyyyymmmmddddd
				// y year, 10 bits, values are 0=unknown, 1-1000 are years 1501-2500 (so fixed offset of 1500), 1001-1023 are reserved
				// m month, 4 bits, values are 0=unknown, 1=January..12=December, 13-15 reserved
				// d day,   5 bits, values are valid, 0=unknown, 1-31 = conventional date days
				yyyy = (date_bin>>9);
				if( 1<=yyyy && yyyy<=1000 )
					yyyy += 1500; 
				else
					yyyy = 1500;	// note that yyyy=0 is not possible if yyyy has been calculated (see use of yyyy to avoid recalculation below)
				bool old_game = (yyyy < elo_cutoff_before_year);
				if( elo_w == 0 )
					elo_w_pass = old_game;
				if( elo_b == 0 )
					elo_b_pass = old_game;
			}
		}
		bool ok = true;
		if( elo_cutoff_one )
			ok = elo_w_pass||elo_b_pass;
		else if( elo_cutoff_both )
			ok = elo_w_pass&&elo_b_pass;
		if( !ok )
			return false;   // not inserted, not error
	}
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
    if( swhite.length()>5 && swhite.substr(swhite.length()-5)==" (wh)" )
        swhite = swhite.substr( 0, swhite.length()-5 );
    std::string sblack(black);
    if( sblack.length()>5 && sblack.substr(sblack.length()-5)==" (bl)" )
        sblack = sblack.substr( 0, sblack.length()-5 );
    ListableGameBinDb gb
    (
        bin_db_append_cb_idx, 
        games.size(),
        sevent,
        ssite,
        swhite,
        sblack,
        yyyy==0 ? Date2Bin(date) : date_bin,
        Round2Bin(round),
        Result2Bin(result),
        Eco2Bin(eco),
        elo_w,
        elo_b,
        compressed_moves
    );
    make_smart_ptr( ListableGameBinDb, new_gb, gb );
    games.push_back( std::move(new_gb) );
    return aborted;
}

struct FileHeader
{
    int hdr_len;        // This was first version, then reserved, now is a future compatibility feature
    int nbr_players;
    int nbr_events;
    int nbr_sites;
    int nbr_games;
    int locked;         // added with DATABASE_VERSION_NUMBER_LOCKABLE
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

// Adapted (just a little) from http://www.talkativeman.com/levenshtein-distance-algorithm-string-comparison/
static int lev_distance( const std::string source, const std::string target )
{
    // Step 1
    const int n = source.length();
    const int m = target.length();
    if( n == 0 )
    {
        return m;
    }
    if( m == 0 )
    {
        return n;
    }

    // Good form to declare a TYPEDEF
    typedef std::vector< std::vector<int> > matrix_t;
    matrix_t matrix(n+1);

    // Size the vectors in the 2nd dimension. Unfortunately C++ doesn't
    // allow for allocation on declaration of 2nd dimension of vec of vec
    for( int i=0; i<=n; i++ )
    {
        matrix[i].resize(m+1);
    }

    // Step 2
    for( int i=0; i<=n; i++ )
    {
        matrix[i][0]=i;
    }
    for( int j=0; j<=m; j++)
    {
        matrix[0][j]=j;
    }

    // Step 3
    for( int i=1; i<=n; i++ )
    {
        const char s_i = source[i-1];

        // Step 4
        for( int j=1; j<=m; j++ )
        {
            const char t_j = target[j-1];

            // Step 5
            int cost;
            if( s_i == t_j )
            {
                cost = 0;
            }
            else
            {
                cost = 1;
            }

            // Step 6
            const int above = matrix[i-1][j];
            const int left = matrix[i][j-1];
            const int diag = matrix[i-1][j-1];
            #define MIN(a,b) ((a)<=(b) ? (a) : (b))
            int temp = MIN( left+1, diag+cost );
            int cell = MIN( above+1, temp );

            // Step 6A: Cover transposition, in addition to deletion,
            // insertion and substitution. This step is taken from:
            // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's
            // Enhanced Dynamic Programming ASM Algorithm"
            // (http://www.acm.org/~hlb/publications/asm/asm.html)
            if( i>2 && j>2 )
            {
                int trans = matrix[i-2][j-2]+1;
                if( source[i-2] != t_j )
                    trans++;
                if( s_i != target[j-2] )
                    trans++;
                if( cell>trans )
                    cell=trans;
            }
            matrix[i][j]=cell;
        }
    }

    // Step 7
    return matrix[n][m];
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
            int dist = lev_distance(tokens[i],tokens2[j]);
            if( tokens[i].length()>8 && tokens2[j].length()>8 && dist <= 2 )
                return true;
            if( tokens[i].length()>4 && tokens2[j].length()>4 && dist <= 1 )
                return true;
        }
    }
    return false;
}

//  match only if years are present and match
static bool IsYearMatch( const char *date1, const char *date2 )
{
    if( !date1 || !date2 )
        return false;
    for( int i=0; i<4; i++ )
    {
        if( *date1=='\0' || *date2=='\0' )
            return false;
        if( *date1++ != *date2++ )
            return false;
    }
    return true;
}

uint32_t BinDbGetGamesSize()
{
    return games.size();
}

void BinDbNormaliseOrder( uint32_t begin, uint32_t end )
{
    cprintf( "BinDbNormaliseOrder(): in\n" );
    bool forward=false, reverse=false, neither=true;
    uint32_t forward_cnt=0, reverse_cnt=0;
    uint32_t sz = end-begin;
    for( int i=0; sz && i<1000; i++ )  // sample 1000 random pairs of dates
    {
        unsigned int j = begin + (rand() % sz);
        unsigned int k = begin + (rand() % sz);
        int jbin = games[j]->DateBin();
        int kbin = games[k]->DateBin();
        if( k>j && kbin>jbin )
            forward_cnt++;        //forward if later in the file has a later date
        if( k>j && kbin<jbin )
            reverse_cnt++;        //reverse if later in the file has an earlier date
    }
    if( forward_cnt > reverse_cnt*5 )
    {
        forward = true;
        neither = false;
    }
    else if( reverse_cnt > forward_cnt*5 )
    {
        reverse = true;
        neither = false;
    }
    cprintf( "BinDbNormaliseOrder(): out forward_cnt=%u, reverse_cnt=%u, forward=%s, reverse=%s\n", forward_cnt, reverse_cnt, forward?"true":"false", reverse?"true (reversing)":"false" );
    if( reverse )
    {
        uint32_t base = games[begin]->game_id;
        std::reverse( games.begin()+begin, games.begin()+end );
        for( uint32_t i=begin; i<end; i++ )
            games[i]->game_id = base+i;
        cprintf( "reverse end\n" );
    }
    if( sz )
    {
        if( begin > 0 )
        {
            smart_ptr<ListableGame> p1 = games[0];
            smart_ptr<ListableGame> p2 = games[begin-1];
            cprintf( "[%d]: game_id=%lu, %s-%s %s\n", 0, p1->game_id, p1->White(), p1->Black(), p1->Date() );
            cprintf( "[%d]: game_id=%lu, %s-%s %s\n", begin-1, p2->game_id, p2->White(), p2->Black(), p2->Date() );
        }
        smart_ptr<ListableGame> p1 = games[begin];
        smart_ptr<ListableGame> p2 = games[end-1];
        cprintf( "[%d]: game_id=%lu, %s-%s %s\n", begin, p1->game_id, p1->White(), p1->Black(), p1->Date() );
        cprintf( "[%d]: game_id=%lu, %s-%s %s\n", end-1, p2->game_id, p2->White(), p2->Black(), p2->Date() );
    }
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
    bool year_match = IsYearMatch( p1->Date(),  p2->Date() );   // best by test - require a yyyy match, but not an exact yyyy-mm-dd match
    bool dup = (white_match && black_match && year_match && result_match);
    return dup;
}

static const uint32_t NBR_STEPS=1000;   // if this isn't 1000, then sort_scan() won't return permill
static uint64_t predicate_count;
static uint64_t predicate_nbr_expected;
static std::vector< smart_ptr<ListableGame> >::iterator predicate_begin;
static bool (*predicate_func)( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 );
static uint32_t predicate_step;
static int permill_initial;
static int permill_max_so_far;
static ProgressBar *predicate_pb; 

static int sort_scan()
{
    if( predicate_step == 0 )
        return 500;

    int permill=1;  // start at 1 since there are only 999 steps in loop
                    // so permill returned will be between 1 and 1000 inclusive
    uint64_t restore_value = predicate_count;
    for( uint32_t i=1; i<NBR_STEPS; i++ )
    {
        uint32_t step = i*predicate_step;      // first value of step is predicate_step
        uint32_t prev = step - predicate_step; // first value of prev is 0
        bool lower = (*predicate_func)( *(predicate_begin+prev), *(predicate_begin+step) );
        if( lower )
            permill++;
    }
    predicate_count = restore_value; // sort_scan() shouldn't affect value of predicate_count
    return permill;
}

static void sort_before( std::vector< smart_ptr<ListableGame> >::iterator begin,
                   std::vector< smart_ptr<ListableGame> >::iterator end,
                   bool (*predicate)( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 ),
                   ProgressBar *pb 
                 )
{
    predicate_count = 0;
    predicate_pb = pb;
    predicate_func = predicate;
    unsigned int dist = std::distance( begin, end );

    // The following formula is based on experiment - std::sort() called the predicate function approx
    //  this many times for random input - hopefully this is approx worse case since in our experiments
    //  as far as we could tell it was actually called less if the input was patterned - including
    //  already sorted and already reverse sorted patterns
    predicate_nbr_expected = static_cast<uint64_t>( 7.5 * dist * log10(static_cast<float>(dist)) );
    predicate_begin = begin;
    predicate_step  = dist/NBR_STEPS;
    permill_initial = sort_scan();
    permill_max_so_far = 0;
    cprintf( "Sorting: nbr_to_sort=%u, nbr_expected=%u, initial permill=%u\n", (unsigned int)dist,  (unsigned int)predicate_nbr_expected, (unsigned int)permill_initial );
}

static void sort_after()
{
    cprintf( "Sorting: nbr_expected=%u, nbr_actual=%u\n", (unsigned int)predicate_nbr_expected, (unsigned int)predicate_count );
}

static void sort_progress_probe()
{
    if( predicate_pb )
    {
        int permill;
        //#define SCAN_BASED
        #ifdef SCAN_BASED
        if( (1000-permill_initial) > 100 ) // scan based approach is useless if input initially nearly sorted
        {
            permill = sort_scan();
            if( permill > permill_max_so_far )
                permill_max_so_far = permill;
            else
                permill = permill_max_so_far;
            predicate_pb->Perfraction( permill-permill_initial, (1000-permill_initial) );
        }
        else
        #endif
        {
            if( predicate_count>=predicate_nbr_expected || predicate_nbr_expected==0 )
                permill = 1000;
            else if( predicate_nbr_expected > 1000000 )
                permill = predicate_count / (predicate_nbr_expected/1000);
            else
                permill = (predicate_count*1000) / predicate_nbr_expected;
            predicate_pb->Permill( permill );
        }
    }
}

static bool predicate_sorts_by_id( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 )
{
    bool ret = e1->game_id < e2->game_id;
    predicate_count++;
    if( (predicate_count & 0xffff) == 0 )
        sort_progress_probe();
    return ret;
}

static bool predicate_sorts_by_game_moves( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 )
{
    bool ret = std::string(e1->CompressedMoves()) < std::string(e2->CompressedMoves());
    predicate_count++;
    if( (predicate_count & 0xffff) == 0 )
        sort_progress_probe();
    return ret;
}

static bool predicate_sorts_by_player( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 )
{
	bool ret;
	bool eq  = e1->WhiteBin() == e2->WhiteBin();
	if( eq )
		ret = e1->game_id < e2->game_id;   // This means the "Show all by player" feature shows each players game in db order
	else
		ret = e1->WhiteBin() < e2->WhiteBin();
    predicate_count++;
    if( (predicate_count & 0xffff) == 0 )
        sort_progress_probe();
    return ret;
}


void BinDbShowDebugOrder( const std::vector< smart_ptr<ListableGame> > &gms, const char *msg )
{
    int sz = gms.size();
    if( sz < 7 )
	{
        std::string s;
        for( int idx=0; idx<sz; idx++ )
        {
            char buf[40];
            sprintf( buf, "%u", gms[idx]->game_id );
            s += std::string(buf);
            if( idx+1 < sz )
                s +=",";
        }
        cprintf( "%s [%s]\n", msg, s.c_str() );
	}
    else
    {
        uint32_t id0 = gms[0]->game_id;
        uint32_t id1 = gms[1]->game_id;
        uint32_t id2 = gms[2]->game_id;
        uint32_t id_end2 = gms[sz-3]->game_id;
        uint32_t id_end1 = gms[sz-2]->game_id;
        uint32_t id_end0 = gms[sz-1]->game_id;
        cprintf( "%s [%lu,%lu,%lu...%lu,%lu,%lu]\n",msg, id0,id1,id2,id_end2,id_end1,id_end0);
    }
}

void BinDbDatabaseInitialSort( std::vector< smart_ptr<ListableGame> > &games_, bool sort_by_player_name )
{

    // Usually the database is sorted according to game_id - bail out quickly if no need to sort
    bool sorted=true;
    if( sort_by_player_name )
    {
        uint32_t prev_id=0;
        int prev_white_bin=0;
        for( uint32_t i=0; sorted && i<games_.size(); i++ )
        {
            uint32_t id = games_[i]->game_id;
            int white_bin = games_[i]->WhiteBin();
        	bool eq = (white_bin == prev_white_bin);
            bool lt;
    	    if( eq )
	    	    lt = id < prev_id;   // This means the "Show all by player" feature shows each players game in db order
	        else
		        lt = white_bin < prev_white_bin;
            if( lt )
                sorted = false;
            prev_id = id;
            prev_white_bin = white_bin;
        }
    }
    else
    {
        uint32_t prev=0;
        for( uint32_t i=0; sorted && i<games_.size(); i++ )
        {
            uint32_t id = games_[i]->game_id;
            if( id < prev )
                sorted = false;
            prev = id;
        }
    }
    cprintf( "%s sorted by %s\n", sorted?"Already":"Not already", sort_by_player_name?"player name":"id" );
    if( sorted )
        return;
    std::string desc(sort_by_player_name?"Sorting by player name":"Initial sort");
    ProgressBar progress_bar( "Sorting", desc, true );
    //progress_bar.DrawNow();
    BinDbShowDebugOrder( games_, "Initial sort before");
    sort_before( games_.begin(), games_.end(), sort_by_player_name ? predicate_sorts_by_player : predicate_sorts_by_id, &progress_bar );
    std::sort( games_.begin(), games_.end(), sort_by_player_name ? predicate_sorts_by_player : predicate_sorts_by_id );
    sort_after();
    BinDbShowDebugOrder( games_, "Initial sort after");
}

#ifdef DURING_DEVELOPMENT
#define EXTRA_DEDUP_DIAGNOSTIC_FILE
#endif
#ifdef  EXTRA_DEDUP_DIAGNOSTIC_FILE
static void replace_once( std::string &s, const std::string from, const std::string to )
{
    size_t offset = s.find(from);
    if( offset != std::string::npos )
        s.replace(offset,from.length(),to);
}
#endif


// New in V3.01a - incorporate write file so can do that before writing dups to TarraschDbDuplicate.pgn
bool BinDbRemoveDuplicatesAndWrite( std::string &title, int step, FILE *ofile, bool locked, wxWindow *window )
{
#ifdef  EXTRA_DEDUP_DIAGNOSTIC_FILE
    wxFileName wfn2(objs.repository->log.m_file.c_str());
    if( !wfn2.IsOk() )
        wfn2.SetFullName("TarraschDbDuplicatesFileSpecial.pgn");
    wfn2.SetExt("pgn");
    wfn2.SetName("TarraschDbDuplicatesFileSpecial");
    wxString dups_filename2 = wfn2.GetFullPath();
    FILE *pgn_dup2 = fopen(dups_filename2.c_str(),"wb");
#endif
    bool ok=true;

    // Bug fix: V3.01 Establish contiguous id range - if we have assembled multiple files it won't have happened
    size_t nbr = games.size();
    uint32_t id = GameIdAllocateTop(nbr);
    for( size_t i=0; i<nbr; i++ )
        games[i]->game_id = id++;

    // Last three steps - remove duplicates, write to file, write duplicates to TarraschDbDuplicates.pgn
    char buf[200];
    sprintf( buf, "%s, step %d of %d", title.c_str(), step, step+2 );
    std::string dup_title(buf);
    sprintf( buf, "%s, step %d of %d", title.c_str(), step+1, step+2 );
    std::string write_title(buf);
    sprintf( buf, "%s, step %d of %d", title.c_str(), step+2, step+2 );
    std::string optional_title(buf);
    {
        BinDbShowDebugOrder( games, "Duplicate Removal - phase 1 before");
        std::string desc("Duplicate Removal - phase 1");
        ProgressBar progress_bar( dup_title, desc, true, window );
        progress_bar.DrawNow();
        sort_before( games.begin(), games.end(), predicate_sorts_by_game_moves, &progress_bar );
        std::sort( games.begin(), games.end(), predicate_sorts_by_game_moves );
        sort_after();
        BinDbShowDebugOrder( games, "Duplicate Removal - phase 1 after");
    }
    {
        BinDbShowDebugOrder( games, "Duplicate Removal - phase 2 before");
        std::string desc("Duplicate Removal - phase 2");
        ProgressBar progress_bar( dup_title, desc, true, window );
        progress_bar.DrawNow();
        ProgressBar *pb = &progress_bar;
        int nbr_games = games.size();

        // State variable - true when traversing duplicates
        bool in_dups=false;

        // State variable - points to first of group of duplicates
        int start=0;
        for( int i=0; i<nbr_games-1; i++ )    // note we stop at second last element
        {
            if( pb->Perfraction( i,nbr_games) )
                return false;   // abort

            // Does this element match the next element? (there's always another element because
            //  remember we stop at second to last element not last element)
            bool next_matches = (0 == strcmp(games[i]->CompressedMoves(),games[i+1]->CompressedMoves()) );

            // end will point beyond end of a group of duplicates if we identify such a group this time
            //  through the loop
            int end = i;

            // State machine to maintain in_dups and start state variables
            if( in_dups )
            {
                if( !next_matches )
                {
                    in_dups = false;
                    end = i+1;  // Leaving a group of duplicates, evaluate them
                }
            }
            else
            {
                if( next_matches )
                {
                    in_dups = true;
                    start = i;
                }
            }

            // One special case - a group of duplicates at the end
            bool last = (i+1 >= nbr_games-1);  // is this the last time thru loop?
            if( last && in_dups )
                end = i+2;  // i points at second last element, end should point beyond last element

            // Found a group of (potential) duplicates. So far we only know that the moves are
            //  identical. We have more work to do to identify the real duplicates. Real duplicates
            //  are marked with id = GAME_ID_SENTINEL so that they group together when we do a final
            //  sort by id.
            if( end > i )
            {

                // First reorder the group by id
                std::sort( games.begin()+start, games.begin()+end, predicate_sorts_by_id );
#if 0
                {
                    cprintf( "Eval Dups in\n" );
                    for( int idx=start; idx<end; idx++ )
                    {
                        smart_ptr<ListableGame> p = games[idx];
                        cprintf( "%d: %s-%s, %s %d ply\n", p->game_id, p->White(), p->Black(), p->Event(), strlen(p->CompressedMoves()) );
                    }
                }
#endif

                // For each game in group of duplicates
                for( int idx=start; idx<end; idx++ )
                {
                    smart_ptr<ListableGame> p = games[idx];

                    // If the game hasn't already been marked as a dup (note first of the group is never marked as a dup)
                    if( p->game_id != GAME_ID_SENTINEL )
                    {

                        // Sweep through the rest of the group and mark each game in turn a dup if it matches
                        std::vector<std::string> white_tokens;
                        Split(p->White(),white_tokens);
                        std::vector<std::string> black_tokens;
                        Split(p->Black(),black_tokens);

                        // Note that this is a O(2) type algorithm sadly, we loop through a group of games for each
                        //  game in main loop
#ifdef EXTRA_DEDUP_DIAGNOSTIC_FILE
                        int nbr_dups=0;
#endif
                        std::string str_dup_games;
                        for( int j=idx+1; j<end; j++ )
                        {
                            smart_ptr<ListableGame> q = games[j];
                            if( q->game_id!=GAME_ID_SENTINEL && DupDetect(p,white_tokens,black_tokens,q) )
                            {
                                q->game_id = GAME_ID_SENTINEL;    
#ifdef EXTRA_DEDUP_DIAGNOSTIC_FILE
                                GameDocument the_game;
                                CompactGame pact;
                                std::string str;
                                if( nbr_dups == 0 )
                                {
                                    p->GetCompactGame( pact );
                                    pact.Upscale(the_game);
                                    the_game.ToFileTxtGameDetails( str );
                                    str_dup_games += str;
                                    the_game.ToFileTxtGameBody( str );
                                    str_dup_games += str;
                                }
                                nbr_dups++;
                                q->GetCompactGame( pact );
                                pact.Upscale(the_game);
                                the_game.ToFileTxtGameDetails( str );
                                    str_dup_games += str;
                                the_game.ToFileTxtGameBody( str );
                                str_dup_games += str;
#endif
                            }
                        }
#ifdef EXTRA_DEDUP_DIAGNOSTIC_FILE
                        if( pgn_dup2 && nbr_dups>0 )
                        {
                            if( nbr_dups > 1 )
                                replace_once( str_dup_games, "[White \"", "[White \"MORE-THAN-2- " ); 
                            fwrite(str_dup_games.c_str(),1,str_dup_games.length(),pgn_dup2);
                        }
#endif
                    }
                }
#if 0           
                {
                    cprintf( "Eval Dups out\n" );
                    for( int idx=start; idx<end; idx++ )
                    {
                        smart_ptr<ListableGame> p = games[idx];
                        cprintf( "%d: %s-%s, %s %d ply\n", p->game_id, p->White(), p->Black(), p->Event(), strlen(p->CompressedMoves()) );
                    }
                }
#endif
            }
        }
        BinDbShowDebugOrder( games, "Duplicate Removal - phase 2 after");
    }
    int nbr_deleted=0;
    {
        BinDbShowDebugOrder( games, "Duplicate Removal - phase 3 before");
        std::string desc("Duplicate Removal - phase 3");
        ProgressBar progress_bar( dup_title, desc, true, window );
        //progress_bar.DrawNow();
        sort_before( games.begin(), games.end(), predicate_sorts_by_id, &progress_bar );
        std::sort( games.begin(), games.end(), predicate_sorts_by_id );
        sort_after();

        // Games to be deleted are at the end - with id GAME_ID_SENTINEL, count them
        for( int i=games.size()-1; i>=0; i-- )
        {
            if( games[i]->game_id != GAME_ID_SENTINEL )
                break;
            else
                nbr_deleted++;
        }
		BinDbShowDebugOrder(games, "Duplicate Removal - phase 3 after");
	}

    // New in V3.01a - incorporate write file so can do that before writing dups to TarraschDbDuplicate.pgn
    if( ofile )
    {
        std::string desc("Writing file");
        ProgressBar progress_bar( write_title, desc, true, window );
        ok = BinDbWriteOutToFile(ofile,nbr_deleted,locked,&progress_bar);
    }

    if( nbr_deleted )
    {
        wxFileName wfn(objs.repository->log.m_file.c_str());
        if( !wfn.IsOk() )
            wfn.SetFullName("TarraschDbDuplicatesFile.pgn");
        wfn.SetExt("pgn");
        wfn.SetName("TarraschDbDuplicatesFile");
        wxString dups_filename = wfn.GetFullPath();
        FILE *pgn_dup = fopen(dups_filename.c_str(),"wb");
        if( pgn_dup )
	    {
		    BinDbShowDebugOrder(games, "Duplicate Removal - phase 4 before");
		    std::string desc("Saving duplicates to TarraschDbDuplicatesFile.pgn, cancel if not needed");
		    ProgressBar progress_bar(optional_title, desc, true, window);
            for( int i=games.size()-1; i>=0; i-- )
            {
                if( games[i]->game_id != GAME_ID_SENTINEL )
                    break;
                else
                {
                    GameDocument  the_game;
                    CompactGame pact;
                    games[i]->GetCompactGame( pact );
                    pact.Upscale(the_game);
                    std::string str;
                    the_game.ToFileTxtGameDetails( str );
                    if( pgn_dup )
                        fwrite(str.c_str(),1,str.length(),pgn_dup);
                    the_game.ToFileTxtGameBody( str );
                    if( pgn_dup )
                        fwrite(str.c_str(),1,str.length(),pgn_dup);
                    if( progress_bar.Perfraction( games.size()-i, nbr_deleted ) )
                        break;
                }
            }
            fclose(pgn_dup);
        }
        games.erase( games.end()-nbr_deleted, games.end() );
        cprintf( "Number of duplicates deleted: %d\n", nbr_deleted );
        BinDbShowDebugOrder( games, "Duplicate Removal - phase 4 after");
    }
#ifdef EXTRA_DEDUP_DIAGNOSTIC_FILE
    if( pgn_dup2 )
        fclose(pgn_dup2);
#endif    
    return true;
}

// Return bool okay
bool BinDbWriteOutToFile( FILE *ofile, int nbr_to_omit_from_end, bool locked, ProgressBar *pb )
{
    std::set<std::string> set_player;
    std::set<std::string> set_site;
    std::set<std::string> set_event;
    uint8_t cb_idx = bin_db_append_cb_idx;
    std::vector<std::string> &players = bin_db_control_blocks[cb_idx].players;
    for( std::vector<std::string>::iterator iter = players.begin(); iter != players.end(); iter++ )
        set_player.insert(*iter);
    std::vector<std::string> &events = bin_db_control_blocks[cb_idx].events;
    for( std::vector<std::string>::iterator iter = events.begin(); iter != events.end(); iter++ )
        set_event.insert(*iter);
    std::vector<std::string> &sites = bin_db_control_blocks[cb_idx].sites;
    for( std::vector<std::string>::iterator iter = sites.begin(); iter != sites.end(); iter++ )
        set_site.insert(*iter);
    std::map<std::string,int> map_player;
    std::map<std::string,int> map_event;
    std::map<std::string,int> map_site;
    if( locked )
        fwrite( &compatibility_header, sizeof(compatibility_header), 1, ofile );
    else
    {
        // If not locked, we may as well make it usable by older versions. Subtle point:
        //  older versions will quite happily accept the new FileHeader below, with the
        //  locked flag they don't know about and don't need. Reason: Older versions know
        //  to use the hdr_len field of the FileHeader rather than their own sizeof(FileHeader)
        //  when stepping beyond the FileHeader as they read the database.
        fwrite( &compatibility_header, sizeof(compatibility_header)-1, 1, ofile );
        uint8_t ver=DATABASE_VERSION_NUMBER_BIN_DB;
        fwrite( &ver, 1, 1, ofile );
    }
    std::set<std::string>::iterator it = set_player.begin();
    FileHeader fh;
    fh.hdr_len     = sizeof(FileHeader);
    fh.nbr_players = std::distance( set_player.begin(), set_player.end() );
    fh.nbr_events  = std::distance( set_event.begin(),  set_event.end() );
    fh.nbr_sites   = std::distance( set_site.begin(),   set_site.end() );
    fh.nbr_games   = games.size() - nbr_to_omit_from_end;
    fh.locked      = locked;
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
        uint16_t eco_bin = ptr->EcoBin();   // ECO 500 codes (9 bits) 0-499 is (A..E)(00..99), 500 is empty
        if( eco_bin >= 500 )                // Sadly older versions cannot cope with 500 = empty
            eco_bin = 0;
        bb.Write(6,eco_bin);                // ECO For now 500 codes (9 bits) 0-499 is (A..E)(00..99), sadly A00 indistinguishable from empty
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
}

// Returns bool killed;
bool BinDbLoadAllGames( bool &locked, bool for_append, std::vector< smart_ptr<ListableGame> > &mega_cache, int &background_load_permill, bool &kill_background_load, ProgressBar *pb )
{
	bool killed=false;
    locked = false;

    // When loading the system database for searches, reverse order so most recent games come first 
    bool do_reverse = !for_append;

	// When loading a database for appending games, use 24 bit string indexes since we don't know the final number of
	//  players, events or sites
	bool translate_to_24_bit = for_append;

    uint8_t cb_idx = BinDbReadBegin();
    PackedGameBinDbControlBlock& cb = PackedGameBinDb::GetControlBlock(cb_idx);
    FileHeader fh;
    FILE *fin = bin_file;
    fseek(fin,compatibility_header_size,SEEK_SET);   // skip over compatibility header
    fread( &fh, sizeof(fh), 1, fin );
    cprintf( "%d games, %d players, %d events, %d sites\n", fh.nbr_games, fh.nbr_players, fh.nbr_events, fh.nbr_sites );
    int nbr_bits_player = BitsRequired(fh.nbr_players);
    int nbr_bits_event  = BitsRequired(fh.nbr_events);  
    int nbr_bits_site   = BitsRequired(fh.nbr_sites);
    cprintf( "%d player bits, %d event bits, %d site bits\n", nbr_bits_player, nbr_bits_event, nbr_bits_site );
    int hdr_len = fh.hdr_len;   // future compatibility feature - if FileHeader gets longer so will fh.hdr_len
    if( hdr_len >= sizeof(FileHeader) )  // we support VERSION_NUMBER_BIN_DB which predates VERSION_NUMBER_BIN_LOCKABLE
    {                                    //  databases of that version have a smaller header without lockable
        locked = static_cast<bool>(fh.locked);
    }
    if( hdr_len != sizeof(FileHeader) )
    {
        fseek(fin,compatibility_header_size+hdr_len,SEEK_SET);  // if necessary skip to a different point than
                                                                //  beyond header
    }
    ReadStrings( fin, fh.nbr_players, cb.players );
    cprintf( "Players ReadStrings() complete\n" );
    ReadStrings( fin, fh.nbr_events, cb.events );
    cprintf( "Events ReadStrings() complete\n" );
    ReadStrings( fin, fh.nbr_sites, cb.sites );
    cprintf( "Sites ReadStrings() complete\n" );

	// Use this BinaryBlock if we need to translate
	BinaryBlock bb;
    bb.Next(nbr_bits_event);    // Event
    bb.Next(nbr_bits_site);     // Site
    bb.Next(nbr_bits_player);   // White
    bb.Next(nbr_bits_player);   // Black
    bb.Next(19);                // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    bb.Next(16);                // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), cb.bb=board(0-1023)
    bb.Next(9);                 // ECO For now 500 codes (9 bits) (A..E)(00..99)
    bb.Next(2);                 // Result (2 bits)
    bb.Next(12);                // WhiteElo 12 bits (range 0..4095)
    bb.Next(12);                // BlackElo
    bb.Freeze();
    int bb_sz = bb.FrozenSize();
    cb.bb.Clear();
    cb.bb.Next( translate_to_24_bit ? 24 : nbr_bits_event  );				   // Event
    cb.bb.Next( translate_to_24_bit ? 24 : nbr_bits_site   );				   // Site
    cb.bb.Next( translate_to_24_bit ? 24 : nbr_bits_player );				   // White
    cb.bb.Next( translate_to_24_bit ? 24 : nbr_bits_player );				   // Black
    cb.bb.Next(19);                // Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
    cb.bb.Next(16);                // Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), cb.bb=board(0-1023)
    cb.bb.Next(9);                 // ECO For now 500 codes (9 bits) (A..E)(00..99)
    cb.bb.Next(2);                 // Result (2 bits)
    cb.bb.Next(12);                // WhiteElo 12 bits (range 0..4095)
    cb.bb.Next(12);                // BlackElo
    cb.bb.Freeze();
	const char *cb_ptr = cb.bb.GetPtr();
	int cb_sz = cb.bb.FrozenSize();
    uint32_t game_count = fh.nbr_games;
    uint32_t nbr_games=0;
    uint32_t nbr_promotion_games=0;
    uint32_t base = GameIdAllocateTop(game_count);
    for( uint32_t i=0; i<game_count; i++ )
    {
		if( kill_background_load )
		{
			killed = true;
			break;
		}
        if( pb && pb->Perfraction(i,game_count) )
		{
			killed = true;
			break;
		}
        char buf[sizeof(cb.bb)];

		// Read the game header into a std::string
		fread( buf, bb_sz, 1, fin );
        std::string game_header(buf,bb_sz);

		// Read the moves, '\0' terminated string follows game_header
        std::string game_moves;
        int ch = fgetc(fin);
        while( ch && ch!=EOF )
        {
            game_moves += static_cast<char>(ch);
            ch = fgetc(fin);
        }
        if( ch == EOF )
            cprintf( "Whoops\n" );

		// If reading to append, need to translate header from logN bits to 24 bits
		if( translate_to_24_bit )
		{
			const char *game_header_ptr = game_header.c_str();
			uint32_t x0 = bb.Read(0,game_header_ptr);	cb.bb.Write(0,x0);		// Event
			uint32_t x1 = bb.Read(1,game_header_ptr);	cb.bb.Write(1,x1);		// Site
			uint32_t x2 = bb.Read(2,game_header_ptr);	cb.bb.Write(2,x2);		// White
			uint32_t x3 = bb.Read(3,game_header_ptr);	cb.bb.Write(3,x3);		// Black
			uint32_t x4 = bb.Read(4,game_header_ptr);	cb.bb.Write(4,x4);		// Date 19 bits, format yyyyyyyyyymmmmddddd, (year values have 1500 offset)
			uint32_t x5 = bb.Read(5,game_header_ptr);	cb.bb.Write(5,x5);		// Round for now 16 bits -> rrrrrrbbbbbbbbbb   rr=round (0-63), cb.bb=board(0-1023)
			uint32_t x6 = bb.Read(6,game_header_ptr);	cb.bb.Write(6,x6);		// ECO For now 500 codes (9 bits) (A..E)(00..99)
			uint32_t x7 = bb.Read(7,game_header_ptr);	cb.bb.Write(7,x7);		// Result (2 bits)
			uint32_t x8 = bb.Read(8,game_header_ptr);	cb.bb.Write(8,x8);		// WhiteElo 12 bits (range 0..4095)
			uint32_t x9 = bb.Read(9,game_header_ptr);	cb.bb.Write(9,x9);		// BlackElo
			game_header = std::string( cb_ptr, cb_sz );
		}

        uint32_t game_id = base;
        game_id += (do_reverse ? game_count-1-i : i);
		std::string blob = game_header + game_moves;
        ListableGameBinDb info( cb_idx, game_id, blob );
        make_smart_ptr( ListableGameBinDb, new_info, info );
        new_info->SetLocked( locked );
        mega_cache.push_back( std::move(new_info) );
        int num = i;
        int den = game_count?game_count:1;
        if( den > 1000000 )
            background_load_permill = num / (den/1000);
        else
            background_load_permill = (num*1000) / den;
        nbr_games++;
        if( info.TestPromotion() )
            nbr_promotion_games++;
        if(
#ifdef _DEBUG
            (nbr_games<10000 && (nbr_games%100)==0) ||
#endif
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
    if( do_reverse )
        std::reverse( mega_cache.begin(), mega_cache.end() );
    if( nbr_games > 0 )
    {
        smart_ptr<ListableGame> p1 = mega_cache[0];
        smart_ptr<ListableGame> p2 = mega_cache[nbr_games-1];
        cprintf( "Load %s\n", killed?"killed":"not killed" );
        cprintf( "First: game_id=%lu, %s-%s %s\n", p1->game_id, p1->White(), p1->Black(), p1->Date() );
        cprintf( "Last:  game_id=%lu, %s-%s %s\n", p2->game_id, p2->White(), p2->Black(), p2->Date() );
    }
	return killed;
}

