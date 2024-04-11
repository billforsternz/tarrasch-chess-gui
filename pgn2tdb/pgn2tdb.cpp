// pgn2tdb.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "shim.h"
#include "util.h"
#include "BinDb.h"
#include "Objects.h"

#define TEST_FILE_IN  "/users/bill/documents/chess/some-big-databases/Caissabase_2022_01_08.pgn"
#define TEST_FILE_OUT "test4-caissa-2022-01-08.tdb"
static bool temp( const char *filename );

int AutoTimer::instance_cnt;
AutoTimer *AutoTimer::instance_ptr;
Objects objs;

int main( int argc, const char *argv[] )
{
    std::vector<std::string> fin;
    std::string fout;
    int  elo_cutoff = 2000;
    bool elo_cutoff_ignore = true;
    bool elo_cutoff_one  = false;
    bool elo_cutoff_both = false;
    bool elo_cutoff_fail = false;
    bool elo_cutoff_pass = true;
    bool elo_cutoff_pass_before = false;
    int  elo_cutoff_before_year = 1990;
    bool generate_dup_pgn_file = false;
#ifdef _DEBUG
    const char *test_args[] =
    {
        "dont_care.exe",
     //   "-g",
     //   "-b2750",
     //   "-ufail",
        TEST_FILE_IN,
        TEST_FILE_OUT
    };
    argc = sizeof( test_args ) / sizeof( test_args[0] );
    argv = test_args;
#endif
    bool ok = true;
    ok = (argc>=3);
    int i_first_file = 1;
    if( ok )
    {
        for( int i=1; i<argc; i++ )
        {
            std::string arg(argv[i]);
            if( !util::prefix(arg,"-") )
            {
                i_first_file = i;
                break;
            }
            //if( arg == "-g" )
            //    generate_dup_pgn_file = true;
            if( arg == "-ufail" )
                elo_cutoff_fail = true;
            else if( arg == "-upass" )
                elo_cutoff_pass = true;
            else if( util::prefix(arg,"-u") )
            {
                ok = false;
                if( arg.length() > 2 )
                {
                    int yyyy = atoi( arg.substr(2).c_str() );
                    if( yyyy > 0 )
                    {
                        ok                = true;
                        elo_cutoff        = yyyy;
                        elo_cutoff_pass_before = true;
                    }
                }
            }
            else if( util::prefix(arg,"-e") )
            {
                ok = false;
                if( arg.length() > 2 )
                {
                    int elo = atoi( arg.substr(2).c_str() );
                    if( elo > 0 )
                    {
                        ok                = true;
                        elo_cutoff        = elo;
                        elo_cutoff_ignore = false;
                        elo_cutoff_one    = true;
                    }
                }
            }
            else if( util::prefix(arg,"-b") )
            {
                ok = false;
                if( arg.length() > 2 )
                {
                    int elo = atoi( arg.substr(2).c_str() );
                    if( elo > 0 )
                    {
                        ok                = true;
                        elo_cutoff        = elo;
                        elo_cutoff_ignore = false;
                        elo_cutoff_both   = true;
                    }
                }
            }
            else
            {
                ok = false;
            }
        }
    }
    if( ok )
    {
        if( i_first_file > argc-2 ) // eg if no flags, argc=3, i_first_file=1
            ok = false;
        else if( elo_cutoff_pass_before && !elo_cutoff_both && !elo_cutoff_one )
        {
            printf( "Unrated year cutoff specified, without specifying a Elo cutoff\n" );
            ok = false;
        }
    }
    if( ok )
    {
        fout = std::string(argv[argc-1]);
        bool is_tdb = util::suffix( util::tolower(fout), ".tdb" );
        if( !is_tdb )
        {
            printf( "Error: Argument %s should be a .tdb file\n", fout.c_str() );
            ok = false;
        }
        for( int i=i_first_file; ok && i<argc-1; i++ )
        {
            std::string s = argv[i];
            bool is_pgn = util::suffix( util::tolower(s), ".pgn" );
            if( is_pgn )
                fin.push_back(s);
            else
            {
                printf( "Error: Argument %s should be a .pgn file\n", s.c_str() );
                ok = false;
            }
        }
    }
    if( !ok )
    {
        printf( "pgn2tdb V1.00 - Generate Tarrash database files from the command line\n" );
        printf( " Published by Bill Forster, https://github.com/billforsternz/tarrasch-chess-gui\n" );
        printf( "Usage: pgn2tdb [-g] [-e2000] [-ufail|-upass|u1990] pgnfiles tdbfile\n" );
        printf( " -e2000   Set Elo rating cutoff (at least one player) to 2000 (for example)\n" );
        printf( " -b2000   Set Elo rating cutoff (both players) to 2000 (for example)\n" );
        printf( " -upass   Unrated players pass cutoff (the default)\n" );
        printf( " -ufail   Unrated players fail cutoff\n" );
        printf( " -u1990   Unrated players pass for games before 1990 (for example)\n" );
        printf( " pgnfiles One or more pgnfiles (wildcards not supported, sorry)\n" );
        printf( " tdbfile  The tdb file to generate\n" );
        return -1;
    }
    objs.repository = new Repository;
    objs.repository->database.m_elo_cutoff             = elo_cutoff;
    objs.repository->database.m_elo_cutoff_ignore      = elo_cutoff_ignore;
    objs.repository->database.m_elo_cutoff_one         = elo_cutoff_one;
    objs.repository->database.m_elo_cutoff_both        = elo_cutoff_both;
    objs.repository->database.m_elo_cutoff_fail        = elo_cutoff_fail;
    objs.repository->database.m_elo_cutoff_pass        = elo_cutoff_pass;
    objs.repository->database.m_elo_cutoff_pass_before = elo_cutoff_pass_before;
    objs.repository->database.m_elo_cutoff_before_year = elo_cutoff_before_year;
    shim_app_begin();
    extern void compress_temp_lookup_gen_function();
    compress_temp_lookup_gen_function();
    Pgn2Tdb( fin, fout, generate_dup_pgn_file );
    if( objs.repository ) delete objs.repository;
    shim_app_end();
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
