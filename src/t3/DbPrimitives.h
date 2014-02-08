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

// recipe3.sqlite3 is a solid db

#ifdef THC_MAC
#define DB_FILE             "/Users/billforster/Documents/ChessDatabases/rebuild.sqlite3"
#define DB_MAINTENANCE_FILE "/Users/billforster/Documents/ChessDatabases/rebuild.sqlite3"
#else
#define DB_FILE             "/Users/Bill/Documents/T3Database/rebuild.sqlite3"
#define DB_MAINTENANCE_FILE "/Users/Bill/Documents/T3Database/rebuild.sqlite3"
#endif



void db_primitive_open();
void db_primitive_open_multi();
void db_primitive_delete_previous_data();
void db_primitive_transaction_begin();
void db_primitive_transaction_end();
void db_primitive_create_indexes();
void db_primitive_create_indexes_multi();
void db_primitive_create_extra_indexes();
void db_primitive_close();
int  db_primitive_count_games();
void db_primitive_insert_game( const char *white, const char *black, const char *event, const char *site, const char *result, int nbr_moves, thc::Move *moves, uint32_t *hashes  );
void db_primitive_insert_game_multi( const char *white, const char *black, const char *event, const char *site, const char *result, int nbr_moves, thc::Move *moves, uint64_t *hashes  );

int  db_primitive_random_test_program();
void db_primitive_show_games( bool connect );
void db_primitive_speed_tests();


#endif // DB_PRIMITIVES_H
