/****************************************************************************
 * We need to be able to seek to positions beyond 32 bit size limitations
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef FSEEK64_H_INCLUDED
#define FSEEK64_H_INCLUDED
#include <stdio.h>
#include <stdint.h>
#include "Portability.h"

#ifdef THC_WINDOWS
inline int fseek64( FILE *file, int64_t fposn, int origin )
    { return _fseeki64( file, fposn, origin ); }

inline int64_t ftell64( FILE *file )
    { return _ftelli64( file); }
#endif

#ifdef THC_UNIX
inline int fseek64( FILE *file, int64_t fposn, int origin )
    { return _fseeko( file, fposn, origin ); }

inline int64_t ftell64( FILE *file )
    { return _ftello( file); }
#endif

#endif // FSEEK64_H_INCLUDED
