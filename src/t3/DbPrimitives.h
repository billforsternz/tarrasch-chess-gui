/****************************************************************************
 *  Low level database maintenance functions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DB_PRIMITIVES_H
#define DB_PRIMITIVES_H
#include <stdint.h>
#include "thc.h"
#include <wx/window.h>

#ifdef THC_MAC
#define DB_FILE                         "/Users/billforster/Documents/ChessDatabases/giant123.tarrasch_db" //improved_compression1.sqlite3" //next_generation_8.sqlite3"   // 5 has zombie issues
#define DB_MAINTENANCE_FILE             "/Users/billforster/Documents/ChessDatabases/improved_compression2.sqlite3" //next_generation_9.sqlite3"
#define DB_MAINTENANCE_PGN_FILE         "/Users/billforster/Documents/ChessDatabases/giant123.pgn" //giant-base-part1-rebuilt.pgn"
#else
#define DB_FILE                         "/Users/Bill/Documents/T3Database/improved_compression1.sqlite3"
#define DB_MAINTENANCE_FILE             "/Users/Bill/Documents/T3Database/improved_compression2.sqlite3"
#define DB_MAINTENANCE_PGN_FILE         "/Users/Bill/Documents/T3Database/giant123.pgn"
#endif

std::string db_primitive_error_msg();
bool db_primitive_open( const char *db_filename, bool create_mode_parm=true );
bool db_primitive_create_tables();
bool db_primitive_delete_previous_data();
bool db_primitive_flush();
bool db_primitive_transaction_begin( wxWindow *parent );
bool db_primitive_transaction_end();
bool db_primitive_create_indexes();
void db_primitive_close();
int  db_primitive_count_games();
bool db_primitive_insert_game( bool &signal_error, const char *white, const char *black, const char *event, const char *site, const char *round, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo, const char *eco,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  );


#endif // DB_PRIMITIVES_H
