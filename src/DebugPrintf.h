/****************************************************************************
 * A debug build only printf() like debug facility
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DEBUG_PRINTF_H
#define DEBUG_PRINTF_H
#include "wx/log.h"
#include "Portability.h"
#pragma warning(disable:4100)

// We used to use the old C double parenthesis trick to make DebugPrintf() a conditional compilation, eg;
//  dbg_printf(("Hello world, x=%d",x));    // <- eliminated unless _DEBUG build
// ... but now we use variadic macro feature of C++
//  dbg_printf("Hello world, x=%d",x);      // <- eliminated unless _DEBUG build

// When to use which printf() function;
// release_printf()  -  outputs in debug or release, except if KILL_DEBUG_COMPLETELY #defined
// dbg_printf()      -  outputs in debug, [potentially] archived
// cprintf()         -  outputs in debug or release, things we are working on Currently
// core_printf()     -  don't use this directly
//#define KILL_DEBUG_COMPLETELY
#ifdef KILL_DEBUG_COMPLETELY
    #define release_printf(...)
    #define dbg_printf(...)
    #define cprintf(...)
    #define core_printf(...)
#else
    #define release_printf(...)   core_printf ( __VA_ARGS__ )
    #ifdef _DEBUG
        #define dbg_printf(...) core_printf ( __VA_ARGS__ )
        #define cprintf(...)    core_printf ( __VA_ARGS__ )
    #else
        #define dbg_printf(...)
        #define cprintf(...)    core_printf ( __VA_ARGS__ )
    #endif
    int core_printf( const char *fmt, ... );
#endif

// Enable time logging by instantiating instances of this
class DebugPrintfTime
{
public:
    DebugPrintfTime();
    ~DebugPrintfTime();
};

#endif          // DEBUG_PRINTF_H
