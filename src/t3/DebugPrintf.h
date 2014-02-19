/****************************************************************************
 * A debug only (needs double parenthesis) printf() like debug facility
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DEBUG_PRINTF_H
#define DEBUG_PRINTF_H
#include "Portability.h"

// Use double parenthesis to make DebugPrintf() a conditional compilation, eg;
//  DebugPrintf(("Hello world, x=%d",x));   // <- eliminated unless _DEBUG build
#ifdef _DEBUG
    #define DebugPrintf(x) DebugPrintfInner x
#else
    #define DebugPrintf(x)
#endif
int DebugPrintfInner( const char *fmt, ... );
int DebugPrintfInnerTime( const char *fmt, ... );
int DebugPrintfInnerVoid( const char *fmt, ... );
#define cprintf DebugPrintfInner      // simple console printf
#define tprintf DebugPrintfInnerTime  // time prepended console printf
#ifdef _DEBUG
    #define dprintf DebugPrintfInner        // debug build only printf
#else
    #define dprintf DebugPrintfInnerVoid
#endif

#endif			// DEBUG_PRINTF_H
