/****************************************************************************
 *  ECO opening codes
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef ECO_H
#define ECO_H
#include <vector>
#include "thc.h"

const char *eco_ref( const char *eco_in );  // get reference compressed moves for this eco: (for testing)
const char *eco_calculate( const std::vector<thc::Move> &moves );
#define konst   // should be const, Idea: use konst to indicate things to migrate to const 
const char *eco_calculate( konst std::string &compressed_moves );

#endif // ECO_H
