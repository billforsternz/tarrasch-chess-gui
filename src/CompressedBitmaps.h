/****************************************************************************
 * Access Chess Graphics which are stored in compressed xpm/bitmap format using
 *  a simple bespoke scheme
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef COMPRESSED_BITMAPS_H
#define COMPRESSED_BITMAPS_H

#include "Portability.h"
#include "DebugPrintf.h"

// struct CompressedXpm is the basis of a simple bespoke bmp compression scheme.
struct CompressedXpm;
CompressedXpm *GetBestFit( int pitch_in, int &pitch_out );

/* We want a decent set of pre-rendered chess graphic bitmaps and in .xpm form they are
    very big. This simple scheme encodes in about 7 times less space in the executable
    program.

    Compression system works for up to 17 colours (a background colour plus up to 16
    other colours) - uses run length encoding and nibble coding for the non-background
    colours.

    Byte by byte coding system
    --------------------------
    up to 2^3  =8    successive codes cccc                0nnncccc
    up to 2^9  =512  successive codes cccc                100nnnnn nnnncccc
    up to 2^13 =8192 successive background codes          101nnnnn nnnnnnnn
    up to 2^5  =32   arbitrary codes                      110nnnnn followed by nibble codes
    up to 2^13 =8192 arbitrary codes                      111nnnnn nnnnnnnn followed by nibble codes

    In this cccc is a nibble code 0-15 and encodes a colour. And nnnn is a run length code
    that encodes from 1 to 2^m for an m bit field. So the code includes an offset of 1, so
    for example nnn=000 encodes 1 not 0, which means single byte 0000cccc encodes one pixel
    of colour cccc. Nibble encoding is least significant nibble first.

    Kludge added later: Support an 18th colour (it turned out we needed to) using a quirk
    of the coding system. Code 110nnnnn is never used for nnnnn=00000 since one code can
    be more efficiently encoded with 0nnncccc for nnn=000. So make use of the spare code
    freed up (=0xc0) to directly represent the 18th colour. I dynamically calculate the least
    frequently encountered colour code to be represented by the kludge code.

*/

struct CompressedXpm
{
    uint32_t width;
    uint32_t height;
    uint32_t background_colour;
    uint32_t kludge_colour;
    int      kludge_idx;
    int      nbr_other_colours;
    uint32_t other_colours[16];
    uint32_t binary_len;
    uint8_t  *bin;
};

// CompressedXpmProcessor decompresses into a single char **xpm pointer that can be used
//  for xpm -> bitmap generation within wxWidgets
class CompressedXpmProcessor
{
public:
    CompressedXpmProcessor( const CompressedXpm *encoded );
    ~CompressedXpmProcessor();
    char **GetXpm()       {return xpm;}
    int    GetXpmLen()    {return nbr_xpm_strings;}
private:
    char **xpm;
    int  nbr_xpm_strings;
    char *string_storage;
};

#endif // COMPRESSED_BITMAPS_H
