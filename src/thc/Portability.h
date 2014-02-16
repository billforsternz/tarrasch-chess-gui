/****************************************************************************
 * Simple definitions to aid platform portability
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PORTABILITY_H
#define PORTABILITY_H

//CHOOSE ONE:
//#define TARRASCH_UNIX 1
//#define TARRASCH_WINDOWS 1
//#define TARRASCH_OSX 1

#ifdef WIN32
   #define TARRASCH_WINDOWS 1
#else
   #define TARRASCH_UNIX 1
#endif

#include <stdint.h>     // int32_t etc.

#if TARRASCH_UNIX
   #include <string.h>
   typedef uint8_t byte;
   #define THC_MAC
   #define MAC_FIX_LATER
   unsigned long GetTickCount();
   int strcmpi(const char *s, const char *t);
#elif TARRASCH_WINDOWS
   #include <Windows.h>
   #include <string.h>
   #define strcmpi _strcmpi
   #define THC_WINDOWS         // Triple Happy Chess, Windows specific code
   #define WINDOWS_FIX_LATER   // Windows only fix later on Mac
#elif TARRASCH_OSX
   unsigned long GetTickCount();
   typedef unsigned char byte;
   int strcmpi( const char *s, const char *t ); // return 0 if case insensitive match
   #define THC_MAC             // Triple Happy Chess, Mac specific code
   #define MAC_FIX_LATER       // Things not yet done on the Mac
#else
   #error Unknown Platform!
#endif

//#define DebugPrintfInner printf //temp

#endif // PORTABILITY_H
