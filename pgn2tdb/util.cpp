/*

    Utility functions
                                                        Bill Forster, October 2018

*/


#include <iostream>
#include <string>
#include <stdarg.h>  // For va_start, etc.
#include <cstring>
#include "util.h"

namespace util
{

void putline(std::ostream &out,const std::string &line)
{
    out.write( line.c_str(), line.length() );
    out.write( "\n", 1 );
}

void puts(std::ostream &out,const std::string &line)
{
    out.write( line.c_str(), line.length() );
}

std::string sprintf( const char *fmt, ... )
{
    int size = static_cast<int>(strlen(fmt)) * 3;   // guess at size
    std::string str;
    va_list ap;
    for(;;)
    {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt, ap);
        va_end(ap);
        if( n>-1 && n<size )    // are we done yet?
        {
            str.resize(n);
            return str;
        }
        if( n > size )  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 4;      // Guess at a larger size
    }
    return str;
}

bool prefix( const std::string &s, const std::string prefix )
{
    size_t offset = s.find(prefix);
    return (offset == 0);
}

bool suffix( const std::string &s, const std::string suffix )
{
    return( s.length() >= suffix.length() &&
            suffix == s.substr(s.length()-suffix.length(), suffix.length())
          );
}

bool prefix_remove( std::string &s, const std::string prefix )
{
    size_t offset = s.find(prefix);
    bool found = (offset == 0);
    if( found )
        s = s.substr(prefix.length());
    return found;
}

void ltrim( std::string &s )
{
    size_t first_char_offset = s.find_first_not_of(" \n\r\t");
    if( first_char_offset == std::string::npos )
        s.clear();
    else
        s = s.substr(first_char_offset);
}

void rtrim( std::string &s )
{
    size_t final_char_offset = s.find_last_not_of(" \n\r\t");
    if( final_char_offset == std::string::npos )
        s.clear();
    else
        s.erase(final_char_offset+1);
}

// Try for a little efficiency, return true if changes made
bool trim( std::string &s )
{
    bool changed=false;
    size_t len = s.length();
    if( len > 0 )
    {
        size_t first_char_offset = s.find_first_not_of(" \n\r\t");
        if( first_char_offset == std::string::npos )
        {
            s.clear();  // string is all whitespace
            return true;
        }
        else if( first_char_offset > 0 )
        {
            s = s.substr(first_char_offset);   // effect left trim
            changed = true;
        }
        size_t final_char_offset = s.find_last_not_of(" \n\r\t");
        if( final_char_offset != std::string::npos && final_char_offset < len-1 )
        {
            s.erase(final_char_offset+1);     // effect right trim
            changed = true;
        }
    }
    return changed;
}

static bool test_expect( std::string test_id, bool b, bool b_expected, const std::string s, const std::string s_expected )
{
    bool ok=true;
    if( b != b_expected )
    {
        printf( "test %s: failed, 1st parameter wrong\n", test_id.c_str() );
        ok = false;
    }
    if( s != s_expected )
    {
        printf( "test %s: failed, 2nd parameter was %s\n", test_id.c_str(), s.c_str() );
        ok = false;
    }
    return ok;
}

void tests()
{
    std::string s;
    s = " ";
    bool changed = trim(s);
    test_expect( "1.0", changed, true, s, "" );
    s = " hello";
    changed = trim(s);
    test_expect( "1.1", changed, true, s, "hello" );
    s = "hello ";
    changed = trim(s);
    test_expect( "1.2", changed, true, s, "hello" );
    s = " hello ";
    changed = trim(s);
    test_expect( "1.3", changed, true, s, "hello" );
    s = "hello";
    changed = trim(s);
    test_expect( "1.4", changed, false, s, "hello" );
}

void replace_all( std::string &s, const std::string from, const std::string to )
{
    size_t next = 0;
    for(;;)
    {
        size_t offset = s.find(from,next);
        if( offset == std::string::npos )
            break;
        else
            s.replace(offset,from.length(),to);
        next = offset+to.length();
    }
}

void replace_once( std::string &s, const std::string from, const std::string to )
{
    size_t offset = s.find(from);
    if( offset != std::string::npos )
        s.replace(offset,from.length(),to);
}

void split( std::string &s, std::vector<std::string> &fields )
{
    fields.clear();
    size_t next = 0;
    bool more = true;
    while( more && s.length()>next)
    {
        size_t offset = s.find('\t',next);
        std::string frag;
        if( offset != std::string::npos )
            frag = s.substr(next,offset-next);
        else
        {
            frag = s.substr(next);
            more = false;
        }
        fields.push_back(frag);
        if( more )
            next = offset+1;
    }
}

std::string toupper( const std::string &s )
{
    std::string r = s;
    for( unsigned int i=0; i<r.length(); i++ )
    {
        char c = r[i];
        if( 'a'<=c && c<='z' )
        {
            c -= 0x20;
            r[i] = c;
        }
    }
    return r;
}

std::string tolower( const std::string &s )
{
    std::string r = s;
    for( unsigned int i=0; i<r.length(); i++ )
    {
        char c = r[i];
        if( 'A'<=c && c<='Z' )
        {
            c += 0x20;
            r[i] = c;
        }
    }
    return r;
}


} //namespace util

