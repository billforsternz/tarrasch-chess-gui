//
//  Adapt Tarrasch code to command line environment for PGN2TDB
//

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>     // va_arg() etc
#include <string>
#include <vector>
#include <map>
#include <set>

#include "shim.h"
#include "Objects.h"

static FILE *teefile;

void shim_app_begin()
{
    // teefile = fopen( "/users/bill/documents/tarrasch/png2tdb_debuglog.txt", "wt" );
}

void shim_app_end()
{
    if( teefile )
        fclose(teefile);
}

int cprintf( const char *fmt, ... )
{
    int ret=0;
	va_list args;
	va_start( args, fmt );
    char buf[1000];
    vsnprintf( buf, sizeof(buf)-2, fmt, args );
    buf[sizeof(buf)-1] = '\0';
    printf("%s",buf);
    if( teefile )
        fputs(buf,teefile);
    va_end(args);
    return ret;
}

bool progress_update( int permill, std::string txt )
{
    static int permill_prev;
    if( permill != permill_prev )
    {
        cprintf( "%s.", txt.c_str() );
        permill_prev = permill;
    }
    return false;
}

std::string macro_substitution( const std::string &input,
                                const std::map<char,std::string> &macros,
                                const std::vector<std::pair<std::string,std::string>> &menu, int menu_idx )
{
    return "not needed or implemented";
}

std::string md( const std::string &in )
{
    return "not needed or implemented";
}

void wxSafeYield() {}
void AcceptAndClose() {}

