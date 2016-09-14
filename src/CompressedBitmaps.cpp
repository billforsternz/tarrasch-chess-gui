/****************************************************************************
 * Access Chess Graphics which are stored in compressed xpm/bitmap format using
 *  a simple bespoke scheme
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#include <stdint.h>
#include "CompressedBitmaps.h"

// The bitmaps themselves in compressed form
#include "bitmaps\pitch-015-batch2-compressed.h"
#include "bitmaps\pitch-018-2-batch2-compressed.h"
#include "bitmaps\pitch-021-batch2-compressed.h"
#include "bitmaps\pitch-025-batch2-compressed.h"
#include "bitmaps\pitch-029-batch2-compressed.h"
#include "bitmaps\pitch-033-batch2-compressed.h"
#include "bitmaps\pitch-037-batch2-compressed.h"
#include "bitmaps\pitch-040-batch2-compressed.h"
#include "bitmaps\pitch-042-batch2-compressed.h"
#include "bitmaps\pitch-045-batch2-compressed.h"
#include "bitmaps\pitch-050-batch2-compressed.h"
#include "bitmaps\pitch-055-batch2-compressed.h"
#include "bitmaps\pitch-060-batch2-compressed.h"
#include "bitmaps\pitch-065-batch2-compressed.h"
#include "bitmaps\pitch-070-batch2-compressed.h"
#include "bitmaps\pitch-075-batch2-compressed.h"
#include "bitmaps\pitch-080-batch2-compressed.h"
#include "bitmaps\pitch-085-batch2-compressed.h"
#include "bitmaps\pitch-090-batch2-compressed.h"
#include "bitmaps\pitch-095-batch2-compressed.h"
#include "bitmaps\pitch-100-batch2-compressed.h"
#include "bitmaps\pitch-110-batch2-compressed.h"
#include "bitmaps\pitch-120-batch2-compressed.h"
#include "bitmaps\pitch-130-batch2-compressed.h"
#include "bitmaps\pitch-140-batch2-compressed.h"
#include "bitmaps\pitch-150-batch2-compressed.h"
#include "bitmaps\pitch-160-batch2-compressed.h"
#include "bitmaps\pitch-170-batch2-compressed.h"
#include "bitmaps\pitch-180-batch2-compressed.h"
#include "bitmaps\pitch-190-batch2-compressed.h"
#include "bitmaps\pitch-200-batch2-compressed.h"
#include "bitmaps\pitch-215-batch1-compressed.h"
#include "bitmaps\pitch-231-batch1-compressed.h"
#include "bitmaps\pitch-246-batch1-compressed.h"
#include "bitmaps\pitch-261-batch1-compressed.h"
#include "bitmaps\pitch-277-batch1-compressed.h"
#include "bitmaps\pitch-292-batch1-compressed.h"
#include "bitmaps\pitch-306-batch1-compressed.h"

// Find best fit
CompressedXpm *GetBestFit( int pitch_in, int &pitch_out )
{
    CompressedXpm *compressed_xpm = 0;
    if( pitch_in >= 306 ) compressed_xpm = &pitch_306_compressed_xpm, pitch_out = 306;
    #define ELSEIF( in, xpm, out ) else if( pitch_in < (in) ) compressed_xpm = &(xpm), pitch_out = (out)
    ELSEIF( 18,  pitch_015_compressed_xpm, 15 );
    ELSEIF( 21,  pitch_018_compressed_xpm, 18 );
    ELSEIF( 25,  pitch_021_compressed_xpm, 21 );
    ELSEIF( 29,  pitch_025_compressed_xpm, 25 );
    ELSEIF( 33,  pitch_029_compressed_xpm, 29 );
    ELSEIF( 37,  pitch_033_compressed_xpm, 33 );
    ELSEIF( 40,  pitch_037_compressed_xpm, 37 );
    ELSEIF( 42,  pitch_040_compressed_xpm, 40 );
    ELSEIF( 45,  pitch_042_compressed_xpm, 42 );
    ELSEIF( 50,  pitch_045_compressed_xpm, 45 );
    ELSEIF( 55,  pitch_050_compressed_xpm, 50 );
    ELSEIF( 60,  pitch_055_compressed_xpm, 55 );
    ELSEIF( 65,  pitch_060_compressed_xpm, 60 );
    ELSEIF( 70,  pitch_065_compressed_xpm, 65 );
    ELSEIF( 75,  pitch_070_compressed_xpm, 70 );
    ELSEIF( 80,  pitch_075_compressed_xpm, 75 );
    ELSEIF( 85,  pitch_080_compressed_xpm, 80 );
    ELSEIF( 90,  pitch_085_compressed_xpm, 85 );
    ELSEIF( 95,  pitch_090_compressed_xpm, 90 );
    ELSEIF( 100, pitch_095_compressed_xpm, 95 );
    ELSEIF( 110, pitch_100_compressed_xpm, 100 );
    ELSEIF( 120, pitch_110_compressed_xpm, 110 );
    ELSEIF( 130, pitch_120_compressed_xpm, 120 );
    ELSEIF( 140, pitch_130_compressed_xpm, 130 );
    ELSEIF( 150, pitch_140_compressed_xpm, 140 );
    ELSEIF( 160, pitch_150_compressed_xpm, 150 );
    ELSEIF( 170, pitch_160_compressed_xpm, 160 );
    ELSEIF( 180, pitch_170_compressed_xpm, 170 );
    ELSEIF( 190, pitch_180_compressed_xpm, 180 );
    ELSEIF( 200, pitch_190_compressed_xpm, 190 );
    ELSEIF( 215, pitch_200_compressed_xpm, 200 );
    ELSEIF( 231, pitch_215_compressed_xpm, 215 );
    ELSEIF( 246, pitch_231_compressed_xpm, 231 );
    ELSEIF( 261, pitch_246_compressed_xpm, 246 );
    ELSEIF( 277, pitch_261_compressed_xpm, 261 );
    ELSEIF( 292, pitch_277_compressed_xpm, 277 );
    ELSEIF( 306, pitch_292_compressed_xpm, 292 );
    return compressed_xpm;
}

// CompressedXpmProcessor decompresses into a single char **xpm pointer that can be used
//  for xpm -> bitmap generation within wxWidgets
CompressedXpmProcessor::CompressedXpmProcessor( const CompressedXpm *encoded )
{
    int dbg=0;
    bool dbg_enable=false;
    #define DBG_NBR_OUT 20

    /*
        // Format we are going for;
        static const char *bitmap_xpm[] = {
            "6 2 3 1",      // width 6, height 2, 3 colours
            "M c #ff00ff",
            "B c #0000ff",
            "R c #ff0000",
            "RRBBRR",
            "RRMMRR"
        };

    */

    // Calculate storage requirements    
    int nbr_colours = 1 /*background*/ + encoded->nbr_other_colours;
    if( encoded->kludge_idx != -1 )
        nbr_colours++;
    nbr_xpm_strings = 1 /*header*/ + nbr_colours + encoded->height;
    char hdr[100];
    sprintf( hdr, "%d %d %d 1", encoded->width, encoded->height, nbr_colours );
    size_t string_storage_required = strlen(hdr)+1;
    string_storage_required += (nbr_colours * (11 + 1));  // strlen("M c #ff00ff") == 11
    string_storage_required += (encoded->height * (encoded->width+1)); // height * width C strings with +1 for trailing '\0'

    // Dynamic allocation
//#define C_STYLE
#ifdef C_STYLE
    string_storage = (char *) malloc( string_storage_required );
    xpm = (char **) malloc( nbr_xpm_strings * sizeof( char **) );
#else
    string_storage = new char[ string_storage_required + 100 ];     // note my silly little safety margin
    xpm = new char *[nbr_xpm_strings];
#endif
    char *s = string_storage;
    int line_idx=0;
    strcpy( s , hdr );
    xpm[line_idx++] = s;
    s += (strlen(s) + 1);
    sprintf( s, "%c c #%06x", ' ', 0xffffff & encoded->background_colour );   // background first
    xpm[line_idx++] = s;
    s += (strlen(s) + 1);
    char proxy='!';
    char proxy_kludge='z';
    char proxy_colours[16];
    for( int i=0; i<encoded->nbr_other_colours; i++ )
    {
        if( i == encoded->kludge_idx )
        {
            sprintf( s, "%c c #%06x", proxy, 0xffffff & encoded->kludge_colour ); 
            xpm[line_idx++] = s;
            s += (strlen(s) + 1);
            proxy_kludge = proxy;
            proxy++;
            while( proxy == '\'' || proxy=='\"' )
                proxy++; 
        }
        sprintf( s, "%c c #%06x", proxy, 0xffffff & encoded->other_colours[i] ); 
        xpm[line_idx++] = s;
        s += (strlen(s) + 1);
        proxy_colours[i] = proxy;
        proxy++;
        while( proxy == '\'' || proxy=='\"' )
            proxy++; 
    }

    int len = encoded->binary_len;
    const uint8_t *src = encoded->bin;

    /*
        Byte by byte coding system
        --------------------------
        up to 2^3  =8    successive codes cccc                0nnncccc
        up to 2^9  =512  successive codes cccc                100nnnnn nnnncccc
        up to 2^13 =8192 successive background codes          101nnnnn nnnnnnnn
        up to 2^5  =32   arbitrary codes                      110nnnnn followed by codes
        up to 2^13 =8192 arbitrary codes                      111nnnnn nnnnnnnn followed by codes
    */

    xpm[line_idx++] = s;
    uint32_t offset=0;  // from 0 to width*height
    while( len > 0 )
    {
        uint8_t c = *src++;
        len--;
        bool arbitrary_data=false;
        char proxy=' ';
        uint32_t n=0;
        if( c == 0xc0 )   // This is the kludge code - search for "kludge" for explanation
        {
            proxy = proxy_kludge;
            n = 1;
        }
        else if( (c&0x80) == 0 )
        {
            // Sequence of one colour (short)
            uint32_t cccc = c&0x0f;
            proxy = proxy_colours[cccc];
            n = ((c>>4)&0x07) + 1;
            if( dbg_enable && dbg++ < DBG_NBR_OUT )
                cprintf( "Seq short\n" );
        }
        else if( (c&0xe0) == 0xc0 )
        {
            // Arbitrary data (short)
            n = (c&0x1f) + 1;
            arbitrary_data = true;
            if( dbg_enable && dbg++ < DBG_NBR_OUT )
                cprintf( "Data short\n" );
        }
        else
        {
            uint8_t d = *src++;
            len--;
            if( (c&0xe0) == 0x80 )
            {
                // Sequence of one colour (long)
                uint32_t cccc = d&0x0f;
                proxy = proxy_colours[cccc];
                n =  ((d>>4)&0x0f);
                n += ((c&0x1f)<<4);
                n++;
                if( dbg_enable && dbg++ < DBG_NBR_OUT )
                    cprintf( "Seq long\n" );
            }
            else if( (c&0xe0) == 0xa0 )
            {
                // Sequence of background colour
                proxy = ' ';
                n =  d;
                n += ((c&0x1f)<<8);
                n++;
                if( dbg_enable && dbg++ < DBG_NBR_OUT )
                    cprintf( "Magenta\n" );
            }
            else if( (c&0xe0) == 0xe0 )
            {
                // Arbitrary data (long)
                n =  d;
                n += ((c&0x1f)<<8);
                n++;
                arbitrary_data = true;
                if( dbg_enable && dbg++ < DBG_NBR_OUT )
                    cprintf( "Data long\n" );
            }
        }

        // Generate decoded data at end of allowed storage space, if you
        //  think about it carefully, you realise that it can't be
        //  overwritten before it is used
        char *buf = string_storage + string_storage_required - n;
        if( !arbitrary_data )
            memset( buf, proxy, n );
        else
        {
            for( unsigned int i=0; i<n; i++ )
            {
                uint8_t dat = *src;
                if( (i&1) == 0 )
                {
                    buf[i] = proxy_colours[dat&0x0f];
                    if( i+1 == n )
                    {        
                        src++;
                        len--;
                    }
                }
                else
                {
                    buf[i] = proxy_colours[(dat>>4)&0x0f];
                    src++;
                    len--;
                }
            }
        }
        if( dbg_enable && dbg < DBG_NBR_OUT )
        {
            cprintf( "dat (%d) >", n );
            if( arbitrary_data )
            {
                for( uint32_t i=0; i<n && i<10; i++ )
                {
                    char c = buf[i];
                    if( c == ' ' )
                        cprintf( "magenta " );
                    else
                    {
                        for( int j=0; j<16; j++ )
                        {
                            if( c == proxy_colours[j] )
                            {
                                cprintf( "%02x ", j );
                                break;
                            }
                        }
                    }
                }
            }
            cprintf( "\n" );
        }

        // Move generated data into final resting place, interspercing end of string after each line (=row of pixels)
        for( unsigned int i=0; i<n; i++ )
        {
            *s++ = buf[i];
            offset++;
            if( offset == encoded->height*encoded->width )
            {
                *s++ = '\0';
                cprintf( "Check 1: %s\n", len == 0 ? "Pass" : "Fail" );
                cprintf( "Check 2: %s\n", s == (string_storage+string_storage_required) ? "Pass" : "Fail" );
                cprintf( "Check 3: %s\n", line_idx == nbr_xpm_strings ? "Pass" : "Fail" );
            }
            else if( (offset%encoded->width) == 0 )
            {
                *s++ = '\0';
                xpm[line_idx++] = s;
            }
        }
    }
}

CompressedXpmProcessor::~CompressedXpmProcessor()
{
#ifdef C_STYLE
    free(xpm);
    free(string_storage);
#else
    delete[](xpm);
    delete[](string_storage);
#endif
}
