/****************************************************************************
 * A queue of lines of kibitz text from a kibitzing chess engine
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef KIBITZQ_H
#define KIBITZQ_H

#include <string.h>
#ifndef nbrof
    #define nbrof(x) ( sizeof(x) / sizeof((x)[0]) )
#endif

class Kibitzq
{
    #define KQDEPTH 64
    #define KQBUFSIZE 256
    char buf[KQDEPTH][KQBUFSIZE];
    bool cleared;
    unsigned int kqdepth;
    unsigned int put;
    unsigned int get;
    unsigned int  Bump( unsigned int put_or_get ) { return( put_or_get+1 >= kqdepth ? 0 : put_or_get+1 ); }
    bool Full()  { return( Bump(put) == get ); }
    bool Empty() { return( put == get ); }

public:
    Kibitzq() { kqdepth=KQDEPTH; cleared=false; put=get=0; }
    void SetDepth( int depth ) { if( depth<=KQDEPTH ) kqdepth=depth; }
    void Clear() { cleared=true; put=0; get=0; }
    bool TestCleared() { bool temp=cleared; cleared=false; return temp; }

    void Put( const char *txt, int depth )
    {
        bool full = Full();
        unsigned int temp = put;
        unsigned int len = strlen(txt);
        char *dst = &buf[temp][0];
        unsigned int maxlen = KQBUFSIZE-1;
        sprintf( dst, " depth %d ", depth );
        unsigned int offset = strlen(dst);
        dst += offset;
        maxlen -= offset;
        if( len <= maxlen )
            strcpy( dst, txt );
        else
        {
            memcpy( dst, txt, maxlen );
            dst[maxlen] = '\0';
        }
        put = Bump(temp);
        if( full )
            get = Bump(get);
    }

    void Put( const char *txt )
    {
        bool full = Full();
        unsigned int temp = put;
        unsigned int len = strlen(txt);
        if( len < KQBUFSIZE )
            strcpy( &buf[temp][0], txt );
        else
        {
            memcpy( &buf[temp][0], txt, KQBUFSIZE-1 );
            buf[temp][KQBUFSIZE-1] = '\0';
        }
        put = Bump(temp);
        if( full )
            get = Bump(get);
    }
    bool Get( char buf_[], unsigned int max_strlen )
    {
        bool data_read = false;
        if( !Empty() )
        {
            unsigned int temp = get;
            const char *src = &this->buf[temp][0];
            unsigned int len = strlen(src);
            if( len <= max_strlen )
                strcpy( buf_, src );
            else
            {
                memcpy( buf_, src, max_strlen );
                buf_[max_strlen] = '\0';
            }
            get = Bump(temp);
            data_read = true;
        }
        return data_read;
    }
};

#endif // KITBITZQ
