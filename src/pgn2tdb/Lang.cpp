/****************************************************************************
 * Simple language support (for notation only)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <cstring>
#include "Lang.h"

static char lookup[5+1] = "KQRNB";
static char lookup_check[5+1];

static const char *known_langs[]=
{
    "????? (User defined)",
    "KQRNB (English)",
    "KDTSL (German)",
    "RDTCF (French)",
    "RDTCA (Italian)",
    "RDTCA (Spanish)",
    "RDTCB (Portuguese)",
    NULL
};

// Get a list of known languages, NULL terminated, 1st is a template
const char **LangGetKnownArray()
{
    return known_langs;
}

// Start a check for changed language (return ptr to current lang lookup)
const char * LangCheckDiffBegin()
{
    memcpy( lookup_check, lookup, sizeof(lookup_check) );
    return lookup_check;
}

// End a check for changed language (return ptr to new lang lookup or NULL if no change)
const char * LangCheckDiffEnd()
{
    bool same = (0==memcmp( lookup_check, lookup, 5 ));
    return same ? NULL : lookup;
}

// Set new language (sets English if string is invalid)
#if 0
void LangSet( wxString &txt )
{
    bool okay = LangValidateString(txt);
    memcpy( lookup, okay ? (const char *)txt.c_str() : "KQRNB", 5 );
}
#endif

// Get current language
const char *LangGet()
{
    return lookup;
}

// Convert native move to current language
void LangOut( std::string &s )
{
    switch( s[0] )
    {
        case 'K': s[0] = lookup[0];  break;
        case 'Q': s[0] = lookup[1];  break;
        case 'R': s[0] = lookup[2];  break;
        case 'N': s[0] = lookup[3];  break;
        case 'B': s[0] = lookup[4];  break;
        default:
        {
            size_t idx = s.find_first_of('=');
            if( idx != std::string::npos )
            switch( s[idx+1] )
            {
                case 'Q': s[idx+1] = lookup[1];  break;
                case 'R': s[idx+1] = lookup[2];  break;
                case 'N': s[idx+1] = lookup[3];  break;
                case 'B': s[idx+1] = lookup[4];  break;
            }
        }
    }
}

// Convert move, current language to English
void LangToEnglish( std::string &s, const char *language_lookup )
{
    int len = s.length();
    for( int i=0; i<len; i++ )
    {
        char c = s[i];
        if( 'A'<=c && c<='Z' )
        {
            for( int j=0; j<5; j++ )
            {
                if( c == (language_lookup?language_lookup:lookup)[j] )
                {
                    s[i] = "KQRNB"[j];
                    break;
                }
            }
        }
    }
}

// Convert line of text, any language to any language
void LangLine( std::string &s, const char *from, const char *to )
{
    bool in_comment = false;
    int len = s.length();
    if( !from )
        from = "KQRNB"; // english if not specified
    if( !to )
        to = "KQRNB"; // english if not specified
    if( 0 != memcmp(from,to,5) )
    {
        for( int i=0; i<len; i++ )
        {
            char c = s[i];
            if( in_comment && c=='}' )
                in_comment = false;
            else if( !in_comment )
            {
                if( c == '{' )
                    in_comment = true;
                else if( c == ';' )
                    break;
                else
                {
                    if( 'A'<=c && c<='Z' )
                    {
                        for( int j=0; j<5; j++ )
                        {
                            if( c == from[j] )
                            {
                                s[i] = to[j];
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}


// Check a language specification string, uppercasing it if necessary
#if 0
bool LangValidateString( wxString &txt )
{
    bool okay=false;
    const char *cstr = txt.c_str();
    int i;

    // If it's a known string, definitely okay
    for( i=1; !okay; i++ )  // skip first which is not okay
    {
        if( known_langs[i] == NULL )
            break;
        if( 0 == strcmp(cstr,known_langs[i]) )
        {
            okay = true;
            break;
        }
    }

    // If not a known string validate
    if( !okay )
    {
        okay = true;
        if( strlen(cstr) < 5 )
            okay = false;
        else if( strlen(cstr)>5 && cstr[5]!=' ' && cstr[5]!='\t' )
            okay = false;
        else
        {

            // Either exactly 5 chars, or 5 then space, then more text
            bool flags[26];
            int idx=0;
            for( i=0; i<26; i++ )
                flags[i] = false;
            for( i=0; okay && i<5; i++ )
            {
                char c = txt[i];
                if( 'a'<=c && c<='z' )
                    idx = c-'a';
                else if( 'A'<=c && c<='Z' )
                    idx = c-'A';
                else
                    okay = false;

                // No duplicates
                if( okay )
                {
                    if( flags[idx] )
                        okay = false;
                    else
                        flags[idx] = true;
                }
            }
        }

        // If still okay, uppercase if required
        for( i=0; okay && i<5; i++ )
        {
            char c = txt[i];
            if( 'a'<=c && c<='z' )
                txt[i] = 'A' + (c-'a');
        }
    }
    return okay;
}
#endif

