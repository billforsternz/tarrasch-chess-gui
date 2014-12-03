//
//  db.h
//  readpgnfast
//
//  Created by Bill Forster on 6/05/13.
//  Copyright (c) 2013 Bill Forster. All rights reserved.
//

#ifndef __readpgnfast__db__
#define __readpgnfast__db__
#include "thc.h"
#include <stdint.h>

//#define DB_FILE  "/Users/billforster/Documents/chessdb_small_blob.sqlite3"
//#define DB_FILE  "/Users/billforster/Documents/ChessDatabases/chessdb_giant_part1_multi_4096.sqlite3"
#define DB_FILE    "/Users/billforster/Documents/ChessDatabases/recipe3.sqlite3"

void db_maint_open();
void db_maint_open_multi();
void db_maint_delete_previous_data();
void db_maint_transaction_begin();
void db_maint_transaction_end();
void db_maint_create_indexes();
void db_maint_create_indexes_multi();
void db_maint_create_extra_indexes();
void db_maint_close();
int  db_maint_count_games();
void db_maint_insert_game( const char *white, const char *black, const char *event, const char *site, const char *result, int nbr_moves, thc::Move *moves, uint32_t *hashes  );
void db_maint_insert_game_multi( const char *white, const char *black, const char *event, const char *site, const char *result, int nbr_moves, thc::Move *moves, uint64_t *hashes  );

int  db_maint_random_test_program();
void db_maint_show_games( bool connect );
void db_maint_speed_tests();


#endif /* defined(__readpgnfast__db__) */
