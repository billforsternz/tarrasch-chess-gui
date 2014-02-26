/****************************************************************************
 * Complement PrivateChessClasses.h by providing a shared instantation of
 *  the automatically generated lookup tables.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "PrivateChessDefs.h"
namespace thc
{

// All the lookup tables
#define P 1
#define B 2
#define N 4
#define R 8
#define Q 16
#define K 32

// GeneratedLookupTables.inc assumes a suitable type lte = lookup tables element
//  plus a bitmask convention for pieces using identifiers P,R,N,B,Q,K is 
//  defined
#include "GeneratedLookupTables.h"

// A lookup table to convert our character piece convention to the lookup
//  convention.
// Note for future reference. We could get a small performance boost, at the
//  cost of debuggability, by using this bitmask convention rather than our
//  ascii convention for pieces. The advantage would simply be that the
//  to_mask[] conversion would not then be required. In fact that used to be
//  how we did it, but we changed to the ascii convention which has many
//  advantages. Maybe a compromise where debug builds use the ascii convention
//  and release builds use a faster binary convention will be the ultimate
//  solution. We'll need a new api to convert to ascii convention for display
//  of board positions etc. if we do this.
lte to_mask[] =
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 0x00-0x0f
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 0x10-0x1f
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 0x20-0x2f
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 0x30-0x3f
     0,0,B,0,0,0,0,0,0,0,0,K,0,0,N,0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
     P,Q,R,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
     0,0,B,0,0,0,0,0,0,0,0,K,0,0,N,0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
     P,Q,R,0,0,0,0,0,0,0,0,0,0,0,0,0};  // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
#undef P
#undef B
#undef N
#undef R
#undef Q
#undef K

}
