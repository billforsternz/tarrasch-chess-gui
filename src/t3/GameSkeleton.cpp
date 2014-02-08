/****************************************************************************
 * Partial game representation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "Appdefs.h"
#include "GameSkeleton.h"
using namespace std;
using namespace thc;

void GameSkeleton::ToFileTxtGameDetails( std::string &str )
{
    #ifdef _WINDOWS
    #define EOL "\r\n"
    #else
    #define EOL "\n"
    #endif
    string str1;
    str1 += "[Event \"";
    str1 += (event=="" ? "?" : event);
    str1 += "\"]" EOL;
    str1 += "[Site \"";
    str1 += (site=="" ? "?" : site);
    str1 += "\"]" EOL;
    str1 += "[Date \"";
    str1 += (date=="" ? "????.??.??" : date);
    str1 += "\"]" EOL;
    str1 += "[Round \"";
    str1 += (round=="" ? "?": round);
    str1 += "\"]" EOL;
    str1 += "[White \"";
    str1 += (white=="" ? "?": white);
    str1 += "\"]" EOL;
    str1 += "[Black \"";
    str1 += (black=="" ? "?": black);
    str1 += "\"]" EOL;
    str1 += "[Result \"";
    str1 += (result=="" ? "*" : result);
    str1 += "\"]" EOL;
    if( eco != "" )
    {
        str1 += "[ECO \"";
        str1 += eco;
        str1 += "\"]" EOL;
    }
    if( white_elo != "" )
    {
        str1 += "[WhiteElo \"";
        str1 += white_elo;
        str1 += "\"]" EOL;
    }
    if( black_elo != "" )
    {
        str1 += "[BlackElo \"";
        str1 += black_elo;
        str1 += "\"]" EOL;
    }
    str1 += EOL;
    str = str1;
}

