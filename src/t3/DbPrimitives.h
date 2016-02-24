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
#define DB_FILE                         "/Users/billforster/Documents/ChessDatabases/giant123.tdb"
#define DB_MAINTENANCE_FILE             "/Users/billforster/Documents/ChessDatabases/new.tdb"
#define DB_MAINTENANCE_PGN_FILE         "/Users/billforster/Documents/ChessDatabases/giant123.pgn"
#else
#define DB_FILE                         "/Users/Bill/Documents/T3Database/giant123.tdb"
#define DB_MAINTENANCE_FILE             "/Users/Bill/Documents/T3Database/new.tdb"
#define DB_MAINTENANCE_PGN_FILE         "/Users/Bill/Documents/T3Database/giant123.pgn"
#endif

std::string db_primitive_error_msg();
bool db_primitive_open( const char *db_filename, bool create_mode_parm=true );
bool db_primitive_create_tables( bool create_tiny_db );
bool db_primitive_delete_previous_data();
bool db_primitive_flush();
bool db_primitive_transaction_begin( wxWindow *parent );
bool db_primitive_transaction_end();
bool db_primitive_create_indexes( bool create_tiny_db );
void db_primitive_close();
int  db_primitive_get_database_version();
int  db_primitive_count_games();
bool db_primitive_insert_game( bool &signal_error, bool create_tiny_db, const char *white, const char *black, const char *event, const char *site, const char *round, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo, const char *eco,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  );

// Build the default database if .pgn exists and .tdb doesn't
void db_primitive_build_default_database( const char *db_file_name );


#endif // DB_PRIMITIVES_H
