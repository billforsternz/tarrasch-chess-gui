/****************************************************************************
 * Simple definitions to aid platform portability
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PORTABILITY_H
#define PORTABILITY_H
#include <stdint.h>     // int32_t etc. 
#include "wx/wx.h"

#if defined __WXMSW__
    #define THC_WINDOWS     // THC = triplehappy chess
#elif defined __WXGTK__
    #define THC_LINUX
    #define THC_UNIX
#elif defined __WXOSX__
    #define THC_MAC
    #define THC_UNIX
#else
    #error "Unknown target platform - not Windows, Linux GTK or OSX" 
#endif

#ifdef THC_WINDOWS
   #define WINDOWS_FIX_LATER   // Windows only, fix later on Mac, Linux
#endif

#ifdef THC_WINDOWS
   #include <Windows.h>
   #include <string.h>
   #define strcmpi _strcmpi
#else
   #include <string.h>
   typedef unsigned char byte;
   unsigned long GetTickCount();
   int strcmpi( const char *s, const char *t ); // return 0 if case insensitive match
#endif

#endif // PORTABILITY_H
