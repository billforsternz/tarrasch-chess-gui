/****************************************************************************
 * LegacyDb - Partial support for old SQL chess database subsystem
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LEGACYDB_H
#define LEGACYDB_H
#include <vector>
#include "Appdefs.h"
#include "ProgressBar.h"
#include "BinaryConversions.h"
#include "ListableGame.h"

bool LegacyDbLoadAllGames( const char *filename, bool for_db_append, std::vector< smart_ptr<ListableGame> > &mega_cache, int &background_load_permill, bool &kill_background_load, ProgressBar *pb=NULL  );
std::vector< smart_ptr<ListableGame> > &BinDbLoadAllGamesGetVector();


#endif  // LEGACYDB_H
