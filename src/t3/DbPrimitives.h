/****************************************************************************
 *  Low level database maintenance functions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DB_PRIMITIVES_H
#define DB_PRIMITIVES_H
#include "thc.h"
#include <stdint.h>

//#define DB_FILE  "/Users/billforster/Documents/chessdb_small_blob.sqlite3"
//#define DB_FILE  "/Users/billforster/Documents/ChessDatabases/chessdb_giant_part1_multi_4096.sqlite3"

#ifdef THC_MAC
#define DB_FILE                         "/Users/billforster/Documents/ChessDatabases/next_generation_6.sqlite3"   // 5 has zombie issues
#define DB_MAINTENANCE_FILE             "/Users/billforster/Documents/ChessDatabases/next_generation_6.sqlite3"
#define DB_MAINTENANCE_PGN_FILE         "/Users/billforster/Documents/ChessDatabases/giant-base-part1-rebuilt.pgn"
#else
#define DB_FILE                         "/Users/Bill/Documents/T3Database/next_generation_7.sqlite3"
#define DB_MAINTENANCE_FILE             "/Users/Bill/Documents/T3Database/next_generation_8.sqlite3"
#define DB_MAINTENANCE_PGN_FILE         "/Users/Bill/Documents/T3Database/twic1011-1050.pgn"
#endif

void db_primitive_open();
void db_primitive_create_tables();
void db_primitive_delete_previous_data();
void db_primitive_transaction_begin();
void db_primitive_transaction_end();
void db_primitive_create_indexes();
void db_primitive_create_extra_indexes();
void db_primitive_close();
int  db_primitive_count_games();
void db_primitive_insert_game( const char *white, const char *black, const char *event, const char *site, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  );

int  db_primitive_random_test_program();
void db_primitive_show_games( bool connect );
void db_primitive_speed_tests();

void db_test_after_1a4();

#endif // DB_PRIMITIVES_H
