/****************************************************************************
 * Simple definitions to aid platform portability
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PORTABILITY_H
#define PORTABILITY_H
#include <stdint.h>     // int32_t etc. 

// Windows
#ifdef WIN32
#include <Windows.h>
#include <string.h>
#define strcmpi _strcmpi
#define THC_WINDOWS         // Triple Happy Chess, Windows specific code
#define WINDOWS_FIX_LATER   // Windows only fix later on Mac

// Mac
#else
unsigned long GetTickCount();
typedef unsigned char byte;
int strcmpi( const char *s, const char *t ); // return 0 if case insensitive match
#define THC_MAC             // Triple Happy Chess, Mac specific code
#define MAC_FIX_LATER       // Things not yet done on the Mac
#endif

//#define DebugPrintfInner printf //temp

#endif // PORTABILITY_H
