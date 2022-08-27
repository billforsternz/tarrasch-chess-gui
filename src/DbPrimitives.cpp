/****************************************************************************
 *  Low level database maintenance functions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

//
//  This file has evolved into a DatabaseWrite facility and should be
//  renamed as such and wrapped in a class to complement DatabaseRead
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include "wx/filename.h"
#include "Appdefs.h"
#include "thc.h"
#include "CompressMoves.h"
#include "PgnRead.h"
#include "ProgressBar.h"
#include "DbPrimitives.h"
#include "DebugPrintf.h"

// Error reporting mechanism
static std::string error_msg;
std::string db_primitive_error_msg()
{
    std::string msg = error_msg;
    error_msg = "";
    return msg;
}

// Returns bool hasAlpha
#if 0
static bool sanitise( const char *in, char *out, int out_siz )
{
    bool hasAlpha=false;
    int len=0;
    while( *in && len<out_siz-3 )
    {
        char c = *in++;
        if( !isascii(c) )
            *out = '_';
        else if( c!=' ' &&  c!='-' &&  c!='.' && c!=',' && !isalnum(c) )
            *out = '_';
        else
        {
            *out = c;
            if( !hasAlpha && isalpha(c) )
                hasAlpha = true;
        }
        out++;
        len++;
    }
    *out = '\0';
    return hasAlpha;
}

// Split out printable, 2 character or more uppercased tokens from input string
static void Split( const char *in, std::vector<std::string> &out )
{
    bool in_word=true;
    out.clear();
    std::string token;
    while( *in )
    {
        char c = *in++;
        bool delimit = (c<=' ' || c==',' || c=='.' || c==':' || c==';');
        if( !in_word )
        {
            if( !delimit )
            {
                in_word = true;
                token = c;
            }
        }
        else
        {
            if( delimit )
            {
                in_word = false;
                if( token.length() > 1 )
                    out.push_back(token);
            }
            else
            {
                if( isalpha(c) )
                    c = toupper(c);
                token += c;
            }
        }
    }
    if( in_word && token.length() > 1 )
        out.push_back(token);
}

static bool IsPlayerMatch( const char *player, std::vector<std::string> &tokens )
{
    std::vector<std::string> tokens2;
    Split( player, tokens2 );
    int len=tokens.size();
    int len2=tokens2.size();
    for( int i=0; i<len; i++ )
    {
        for( int j=0; j<len2; j++ )
        {
            if( tokens[i] == tokens2[j] )
                return true;
        }
    }
    return false;
}

static void DateParse( const char *date, int &yyyy, int &mm, int &dd )
{
    int state=0;
    int n=0, y=0, m=0, d=0;
    while( *date && state<3 )
    {
        char c = *date++;
        if( '0'<=c && c<='9' )
            n = n*10 + c-'0';
        else if( c=='.' || c=='-' || c==' ' || c==':' )
        {
            switch( state )
            {
                case 0: y = n;  break;
                case 1: m = n;  break;
                case 2: d = n;  break;
            }
            state++;
            n = 0;
        }
        else
            break;
    }
    switch( state )
    {
        case 0: y = n;  break;
        case 1: m = n;  break;
        case 2: d = n;  break;
    }
    yyyy = (1<=y && y<=3000) ? y : 0;
    mm   = (1<=m && m<=12)   ? m : 0;
    dd   = (1<=d && d<=31)   ? d : 0;
}

static bool IsDateMatch( const char *date, int yyyy, int mm, int dd )
{
    int y, m, d;
    DateParse( date, y, m, d );
    bool match = (yyyy == y);   // must match
    if( match )
    {
        match = (mm==m || mm==0 || m==0);    // if month unspecified still matches
        if( match )
            match = (dd==d || dd==0 || d==0);    // if day unspecified still matches
    }
    return match;
}
#endif


