/****************************************************************************
 *  ECO opening codes
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef ECO_H
#define ECO_H

void eco_begin();
const char *eco_ref( const char *eco_in );
const char *eco_calculate( std::string compressed_moves );

#endif // ECO_H
