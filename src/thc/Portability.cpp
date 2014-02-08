/****************************************************************************
 * Simple definitions to aid platform portability
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Portability.h"

// return 0 if case insensitive match
#ifdef THC_MAC
int strcmpi( const char *s, const char *t )
{
    bool same=true;
    while( *s && *t && same )
    {
        char c = *s++;
        char d = *t++;
        same = (c==d) || (isascii(c) && isascii(d) && toupper(c)==toupper(d));
    }
    if( *s || *t )
        same = false;
    return same?0:1;
}
#endif


