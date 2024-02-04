// pgn2tdb.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "util.h"
#include "BinDb.h"

#define TEST_FILE_IN  "/users/bill/documents/chess/welly/2024/WellingtonChessClub-2024-V1.pgn"
#define TEST_FILE_OUT "WellingtonChessClub-2024-V1.tdb"
static bool temp( const char *filename );

int main()
{
    Pgn2Tdb( TEST_FILE_IN, TEST_FILE_OUT );
    return 0;
}

// Temp
static bool temp( const char *filename )
{
    std::ifstream fin( filename );
    unsigned long line_nbr=0;
    if( !fin )
        return false;
    for(;;)
    {
        std::string line;
        if( !std::getline(fin,line) )
            break;
        line_nbr++;
        util::rtrim(line);
        size_t len = line.length();
        printf( "%s\n", line.c_str() );
    }
    return true;
}
