/****************************************************************************
 * These lookup tables are machine generated
 *  They require prior definitions of;
 *   squares (a1,a2..h8)
 *   pieces (P,N,B,N,R,Q,K)
 *   lte (=lookup table element, a type for the lookup tables, eg int or unsigned char)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

// Queen, up to 8 rays
static const lte queen_lookup_a1[] =
{
(lte)3
    ,(lte)7 ,(lte)b1 ,(lte)c1 ,(lte)d1 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)a2 ,(lte)a3 ,(lte)a4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
    ,(lte)7 ,(lte)b2 ,(lte)c3 ,(lte)d4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
};
static const lte queen_lookup_a2[] =
{
(lte)5
    ,(lte)7 ,(lte)b2 ,(lte)c2 ,(lte)d2 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)a1 
    ,(lte)6 ,(lte)a3 ,(lte)a4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
    ,(lte)6 ,(lte)b3 ,(lte)c4 ,(lte)d5 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)1 ,(lte)b1 
};
static const lte queen_lookup_a3[] =
{
(lte)5
    ,(lte)7 ,(lte)b3 ,(lte)c3 ,(lte)d3 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)a2 ,(lte)a1 
    ,(lte)5 ,(lte)a4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
    ,(lte)5 ,(lte)b4 ,(lte)c5 ,(lte)d6 ,(lte)e7 ,(lte)f8 
    ,(lte)2 ,(lte)b2 ,(lte)c1 
};
static const lte queen_lookup_a4[] =
{
(lte)5
    ,(lte)7 ,(lte)b4 ,(lte)c4 ,(lte)d4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
    ,(lte)4 ,(lte)b5 ,(lte)c6 ,(lte)d7 ,(lte)e8 
    ,(lte)3 ,(lte)b3 ,(lte)c2 ,(lte)d1 
};
static const lte queen_lookup_a5[] =
{
(lte)5
    ,(lte)7 ,(lte)b5 ,(lte)c5 ,(lte)d5 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)3 ,(lte)a6 ,(lte)a7 ,(lte)a8 
    ,(lte)3 ,(lte)b6 ,(lte)c7 ,(lte)d8 
    ,(lte)4 ,(lte)b4 ,(lte)c3 ,(lte)d2 ,(lte)e1 
};
static const lte queen_lookup_a6[] =
{
(lte)5
    ,(lte)7 ,(lte)b6 ,(lte)c6 ,(lte)d6 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)a5 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)2 ,(lte)a7 ,(lte)a8 
    ,(lte)2 ,(lte)b7 ,(lte)c8 
    ,(lte)5 ,(lte)b5 ,(lte)c4 ,(lte)d3 ,(lte)e2 ,(lte)f1 
};
static const lte queen_lookup_a7[] =
{
(lte)5
    ,(lte)7 ,(lte)b7 ,(lte)c7 ,(lte)d7 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)a6 ,(lte)a5 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)1 ,(lte)a8 
    ,(lte)1 ,(lte)b8 
    ,(lte)6 ,(lte)b6 ,(lte)c5 ,(lte)d4 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte queen_lookup_a8[] =
{
(lte)3
    ,(lte)7 ,(lte)b8 ,(lte)c8 ,(lte)d8 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)a7 ,(lte)a6 ,(lte)a5 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)7 ,(lte)b7 ,(lte)c6 ,(lte)d5 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte queen_lookup_b1[] =
{
(lte)5
    ,(lte)1 ,(lte)a1 
    ,(lte)6 ,(lte)c1 ,(lte)d1 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)b2 ,(lte)b3 ,(lte)b4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
    ,(lte)1 ,(lte)a2 
    ,(lte)6 ,(lte)c2 ,(lte)d3 ,(lte)e4 ,(lte)f5 ,(lte)g6 ,(lte)h7 
};
static const lte queen_lookup_b2[] =
{
(lte)8
    ,(lte)1 ,(lte)a2 
    ,(lte)6 ,(lte)c2 ,(lte)d2 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)b1 
    ,(lte)6 ,(lte)b3 ,(lte)b4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
    ,(lte)1 ,(lte)a1 
    ,(lte)1 ,(lte)a3 
    ,(lte)6 ,(lte)c3 ,(lte)d4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)1 ,(lte)c1 
};
static const lte queen_lookup_b3[] =
{
(lte)8
    ,(lte)1 ,(lte)a3 
    ,(lte)6 ,(lte)c3 ,(lte)d3 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)b2 ,(lte)b1 
    ,(lte)5 ,(lte)b4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
    ,(lte)1 ,(lte)a2 
    ,(lte)1 ,(lte)a4 
    ,(lte)5 ,(lte)c4 ,(lte)d5 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)2 ,(lte)c2 ,(lte)d1 
};
static const lte queen_lookup_b4[] =
{
(lte)8
    ,(lte)1 ,(lte)a4 
    ,(lte)6 ,(lte)c4 ,(lte)d4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
    ,(lte)1 ,(lte)a3 
    ,(lte)1 ,(lte)a5 
    ,(lte)4 ,(lte)c5 ,(lte)d6 ,(lte)e7 ,(lte)f8 
    ,(lte)3 ,(lte)c3 ,(lte)d2 ,(lte)e1 
};
static const lte queen_lookup_b5[] =
{
(lte)8
    ,(lte)1 ,(lte)a5 
    ,(lte)6 ,(lte)c5 ,(lte)d5 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)3 ,(lte)b6 ,(lte)b7 ,(lte)b8 
    ,(lte)1 ,(lte)a4 
    ,(lte)1 ,(lte)a6 
    ,(lte)3 ,(lte)c6 ,(lte)d7 ,(lte)e8 
    ,(lte)4 ,(lte)c4 ,(lte)d3 ,(lte)e2 ,(lte)f1 
};
static const lte queen_lookup_b6[] =
{
(lte)8
    ,(lte)1 ,(lte)a6 
    ,(lte)6 ,(lte)c6 ,(lte)d6 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)b5 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)2 ,(lte)b7 ,(lte)b8 
    ,(lte)1 ,(lte)a5 
    ,(lte)1 ,(lte)a7 
    ,(lte)2 ,(lte)c7 ,(lte)d8 
    ,(lte)5 ,(lte)c5 ,(lte)d4 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte queen_lookup_b7[] =
{
(lte)8
    ,(lte)1 ,(lte)a7 
    ,(lte)6 ,(lte)c7 ,(lte)d7 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)b6 ,(lte)b5 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)1 ,(lte)b8 
    ,(lte)1 ,(lte)a6 
    ,(lte)1 ,(lte)a8 
    ,(lte)1 ,(lte)c8 
    ,(lte)6 ,(lte)c6 ,(lte)d5 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte queen_lookup_b8[] =
{
(lte)5
    ,(lte)1 ,(lte)a8 
    ,(lte)6 ,(lte)c8 ,(lte)d8 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)b7 ,(lte)b6 ,(lte)b5 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)1 ,(lte)a7 
    ,(lte)6 ,(lte)c7 ,(lte)d6 ,(lte)e5 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte queen_lookup_c1[] =
{
(lte)5
    ,(lte)2 ,(lte)b1 ,(lte)a1 
    ,(lte)5 ,(lte)d1 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)c2 ,(lte)c3 ,(lte)c4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
    ,(lte)2 ,(lte)b2 ,(lte)a3 
    ,(lte)5 ,(lte)d2 ,(lte)e3 ,(lte)f4 ,(lte)g5 ,(lte)h6 
};
static const lte queen_lookup_c2[] =
{
(lte)8
    ,(lte)2 ,(lte)b2 ,(lte)a2 
    ,(lte)5 ,(lte)d2 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)c1 
    ,(lte)6 ,(lte)c3 ,(lte)c4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
    ,(lte)1 ,(lte)b1 
    ,(lte)2 ,(lte)b3 ,(lte)a4 
    ,(lte)5 ,(lte)d3 ,(lte)e4 ,(lte)f5 ,(lte)g6 ,(lte)h7 
    ,(lte)1 ,(lte)d1 
};
static const lte queen_lookup_c3[] =
{
(lte)8
    ,(lte)2 ,(lte)b3 ,(lte)a3 
    ,(lte)5 ,(lte)d3 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)c2 ,(lte)c1 
    ,(lte)5 ,(lte)c4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
    ,(lte)2 ,(lte)b2 ,(lte)a1 
    ,(lte)2 ,(lte)b4 ,(lte)a5 
    ,(lte)5 ,(lte)d4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)2 ,(lte)d2 ,(lte)e1 
};
static const lte queen_lookup_c4[] =
{
(lte)8
    ,(lte)2 ,(lte)b4 ,(lte)a4 
    ,(lte)5 ,(lte)d4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
    ,(lte)2 ,(lte)b3 ,(lte)a2 
    ,(lte)2 ,(lte)b5 ,(lte)a6 
    ,(lte)4 ,(lte)d5 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)3 ,(lte)d3 ,(lte)e2 ,(lte)f1 
};
static const lte queen_lookup_c5[] =
{
(lte)8
    ,(lte)2 ,(lte)b5 ,(lte)a5 
    ,(lte)5 ,(lte)d5 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)3 ,(lte)c6 ,(lte)c7 ,(lte)c8 
    ,(lte)2 ,(lte)b4 ,(lte)a3 
    ,(lte)2 ,(lte)b6 ,(lte)a7 
    ,(lte)3 ,(lte)d6 ,(lte)e7 ,(lte)f8 
    ,(lte)4 ,(lte)d4 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte queen_lookup_c6[] =
{
(lte)8
    ,(lte)2 ,(lte)b6 ,(lte)a6 
    ,(lte)5 ,(lte)d6 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)c5 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)2 ,(lte)c7 ,(lte)c8 
    ,(lte)2 ,(lte)b5 ,(lte)a4 
    ,(lte)2 ,(lte)b7 ,(lte)a8 
    ,(lte)2 ,(lte)d7 ,(lte)e8 
    ,(lte)5 ,(lte)d5 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte queen_lookup_c7[] =
{
(lte)8
    ,(lte)2 ,(lte)b7 ,(lte)a7 
    ,(lte)5 ,(lte)d7 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)c6 ,(lte)c5 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)1 ,(lte)c8 
    ,(lte)2 ,(lte)b6 ,(lte)a5 
    ,(lte)1 ,(lte)b8 
    ,(lte)1 ,(lte)d8 
    ,(lte)5 ,(lte)d6 ,(lte)e5 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte queen_lookup_c8[] =
{
(lte)5
    ,(lte)2 ,(lte)b8 ,(lte)a8 
    ,(lte)5 ,(lte)d8 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)c7 ,(lte)c6 ,(lte)c5 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)2 ,(lte)b7 ,(lte)a6 
    ,(lte)5 ,(lte)d7 ,(lte)e6 ,(lte)f5 ,(lte)g4 ,(lte)h3 
};
static const lte queen_lookup_d1[] =
{
(lte)5
    ,(lte)3 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)4 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)d2 ,(lte)d3 ,(lte)d4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
    ,(lte)3 ,(lte)c2 ,(lte)b3 ,(lte)a4 
    ,(lte)4 ,(lte)e2 ,(lte)f3 ,(lte)g4 ,(lte)h5 
};
static const lte queen_lookup_d2[] =
{
(lte)8
    ,(lte)3 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)4 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)d1 
    ,(lte)6 ,(lte)d3 ,(lte)d4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
    ,(lte)1 ,(lte)c1 
    ,(lte)3 ,(lte)c3 ,(lte)b4 ,(lte)a5 
    ,(lte)4 ,(lte)e3 ,(lte)f4 ,(lte)g5 ,(lte)h6 
    ,(lte)1 ,(lte)e1 
};
static const lte queen_lookup_d3[] =
{
(lte)8
    ,(lte)3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)4 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)d2 ,(lte)d1 
    ,(lte)5 ,(lte)d4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
    ,(lte)2 ,(lte)c2 ,(lte)b1 
    ,(lte)3 ,(lte)c4 ,(lte)b5 ,(lte)a6 
    ,(lte)4 ,(lte)e4 ,(lte)f5 ,(lte)g6 ,(lte)h7 
    ,(lte)2 ,(lte)e2 ,(lte)f1 
};
static const lte queen_lookup_d4[] =
{
(lte)8
    ,(lte)3 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
    ,(lte)3 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)3 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)3 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte queen_lookup_d5[] =
{
(lte)8
    ,(lte)3 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)4 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)3 ,(lte)d6 ,(lte)d7 ,(lte)d8 
    ,(lte)3 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)3 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)3 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)4 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte queen_lookup_d6[] =
{
(lte)8
    ,(lte)3 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)4 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)d5 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)2 ,(lte)d7 ,(lte)d8 
    ,(lte)3 ,(lte)c5 ,(lte)b4 ,(lte)a3 
    ,(lte)2 ,(lte)c7 ,(lte)b8 
    ,(lte)2 ,(lte)e7 ,(lte)f8 
    ,(lte)4 ,(lte)e5 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte queen_lookup_d7[] =
{
(lte)8
    ,(lte)3 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)4 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)d6 ,(lte)d5 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)1 ,(lte)d8 
    ,(lte)3 ,(lte)c6 ,(lte)b5 ,(lte)a4 
    ,(lte)1 ,(lte)c8 
    ,(lte)1 ,(lte)e8 
    ,(lte)4 ,(lte)e6 ,(lte)f5 ,(lte)g4 ,(lte)h3 
};
static const lte queen_lookup_d8[] =
{
(lte)5
    ,(lte)3 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)4 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)d7 ,(lte)d6 ,(lte)d5 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)3 ,(lte)c7 ,(lte)b6 ,(lte)a5 
    ,(lte)4 ,(lte)e7 ,(lte)f6 ,(lte)g5 ,(lte)h4 
};
static const lte queen_lookup_e1[] =
{
(lte)5
    ,(lte)4 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)3 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)e2 ,(lte)e3 ,(lte)e4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
    ,(lte)4 ,(lte)d2 ,(lte)c3 ,(lte)b4 ,(lte)a5 
    ,(lte)3 ,(lte)f2 ,(lte)g3 ,(lte)h4 
};
static const lte queen_lookup_e2[] =
{
(lte)8
    ,(lte)4 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)3 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)e1 
    ,(lte)6 ,(lte)e3 ,(lte)e4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
    ,(lte)1 ,(lte)d1 
    ,(lte)4 ,(lte)d3 ,(lte)c4 ,(lte)b5 ,(lte)a6 
    ,(lte)3 ,(lte)f3 ,(lte)g4 ,(lte)h5 
    ,(lte)1 ,(lte)f1 
};
static const lte queen_lookup_e3[] =
{
(lte)8
    ,(lte)4 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)e2 ,(lte)e1 
    ,(lte)5 ,(lte)e4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
    ,(lte)2 ,(lte)d2 ,(lte)c1 
    ,(lte)4 ,(lte)d4 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)3 ,(lte)f4 ,(lte)g5 ,(lte)h6 
    ,(lte)2 ,(lte)f2 ,(lte)g1 
};
static const lte queen_lookup_e4[] =
{
(lte)8
    ,(lte)4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)3 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
    ,(lte)3 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)3 ,(lte)f5 ,(lte)g6 ,(lte)h7 
    ,(lte)3 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte queen_lookup_e5[] =
{
(lte)8
    ,(lte)4 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)3 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)3 ,(lte)e6 ,(lte)e7 ,(lte)e8 
    ,(lte)4 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)3 ,(lte)d6 ,(lte)c7 ,(lte)b8 
    ,(lte)3 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)3 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte queen_lookup_e6[] =
{
(lte)8
    ,(lte)4 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)3 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)e5 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)2 ,(lte)e7 ,(lte)e8 
    ,(lte)4 ,(lte)d5 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)2 ,(lte)d7 ,(lte)c8 
    ,(lte)2 ,(lte)f7 ,(lte)g8 
    ,(lte)3 ,(lte)f5 ,(lte)g4 ,(lte)h3 
};
static const lte queen_lookup_e7[] =
{
(lte)8
    ,(lte)4 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)3 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)e6 ,(lte)e5 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)1 ,(lte)e8 
    ,(lte)4 ,(lte)d6 ,(lte)c5 ,(lte)b4 ,(lte)a3 
    ,(lte)1 ,(lte)d8 
    ,(lte)1 ,(lte)f8 
    ,(lte)3 ,(lte)f6 ,(lte)g5 ,(lte)h4 
};
static const lte queen_lookup_e8[] =
{
(lte)5
    ,(lte)4 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)3 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)e7 ,(lte)e6 ,(lte)e5 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)4 ,(lte)d7 ,(lte)c6 ,(lte)b5 ,(lte)a4 
    ,(lte)3 ,(lte)f7 ,(lte)g6 ,(lte)h5 
};
static const lte queen_lookup_f1[] =
{
(lte)5
    ,(lte)5 ,(lte)e1 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)2 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)f2 ,(lte)f3 ,(lte)f4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
    ,(lte)5 ,(lte)e2 ,(lte)d3 ,(lte)c4 ,(lte)b5 ,(lte)a6 
    ,(lte)2 ,(lte)g2 ,(lte)h3 
};
static const lte queen_lookup_f2[] =
{
(lte)8
    ,(lte)5 ,(lte)e2 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)f1 
    ,(lte)6 ,(lte)f3 ,(lte)f4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
    ,(lte)1 ,(lte)e1 
    ,(lte)5 ,(lte)e3 ,(lte)d4 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)2 ,(lte)g3 ,(lte)h4 
    ,(lte)1 ,(lte)g1 
};
static const lte queen_lookup_f3[] =
{
(lte)8
    ,(lte)5 ,(lte)e3 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)2 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)f2 ,(lte)f1 
    ,(lte)5 ,(lte)f4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
    ,(lte)2 ,(lte)e2 ,(lte)d1 
    ,(lte)5 ,(lte)e4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)2 ,(lte)g4 ,(lte)h5 
    ,(lte)2 ,(lte)g2 ,(lte)h1 
};
static const lte queen_lookup_f4[] =
{
(lte)8
    ,(lte)5 ,(lte)e4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)2 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
    ,(lte)3 ,(lte)e3 ,(lte)d2 ,(lte)c1 
    ,(lte)4 ,(lte)e5 ,(lte)d6 ,(lte)c7 ,(lte)b8 
    ,(lte)2 ,(lte)g5 ,(lte)h6 
    ,(lte)2 ,(lte)g3 ,(lte)h2 
};
static const lte queen_lookup_f5[] =
{
(lte)8
    ,(lte)5 ,(lte)e5 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)2 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)3 ,(lte)f6 ,(lte)f7 ,(lte)f8 
    ,(lte)4 ,(lte)e4 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)3 ,(lte)e6 ,(lte)d7 ,(lte)c8 
    ,(lte)2 ,(lte)g6 ,(lte)h7 
    ,(lte)2 ,(lte)g4 ,(lte)h3 
};
static const lte queen_lookup_f6[] =
{
(lte)8
    ,(lte)5 ,(lte)e6 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)2 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)f5 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)2 ,(lte)f7 ,(lte)f8 
    ,(lte)5 ,(lte)e5 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)2 ,(lte)e7 ,(lte)d8 
    ,(lte)2 ,(lte)g7 ,(lte)h8 
    ,(lte)2 ,(lte)g5 ,(lte)h4 
};
static const lte queen_lookup_f7[] =
{
(lte)8
    ,(lte)5 ,(lte)e7 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)2 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)f6 ,(lte)f5 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)1 ,(lte)f8 
    ,(lte)5 ,(lte)e6 ,(lte)d5 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)1 ,(lte)e8 
    ,(lte)1 ,(lte)g8 
    ,(lte)2 ,(lte)g6 ,(lte)h5 
};
static const lte queen_lookup_f8[] =
{
(lte)5
    ,(lte)5 ,(lte)e8 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)2 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)f7 ,(lte)f6 ,(lte)f5 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)5 ,(lte)e7 ,(lte)d6 ,(lte)c5 ,(lte)b4 ,(lte)a3 
    ,(lte)2 ,(lte)g7 ,(lte)h6 
};
static const lte queen_lookup_g1[] =
{
(lte)5
    ,(lte)6 ,(lte)f1 ,(lte)e1 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)1 ,(lte)h1 
    ,(lte)7 ,(lte)g2 ,(lte)g3 ,(lte)g4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
    ,(lte)6 ,(lte)f2 ,(lte)e3 ,(lte)d4 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)1 ,(lte)h2 
};
static const lte queen_lookup_g2[] =
{
(lte)8
    ,(lte)6 ,(lte)f2 ,(lte)e2 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)1 ,(lte)h2 
    ,(lte)1 ,(lte)g1 
    ,(lte)6 ,(lte)g3 ,(lte)g4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
    ,(lte)1 ,(lte)f1 
    ,(lte)6 ,(lte)f3 ,(lte)e4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)1 ,(lte)h3 
    ,(lte)1 ,(lte)h1 
};
static const lte queen_lookup_g3[] =
{
(lte)8
    ,(lte)6 ,(lte)f3 ,(lte)e3 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)1 ,(lte)h3 
    ,(lte)2 ,(lte)g2 ,(lte)g1 
    ,(lte)5 ,(lte)g4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
    ,(lte)2 ,(lte)f2 ,(lte)e1 
    ,(lte)5 ,(lte)f4 ,(lte)e5 ,(lte)d6 ,(lte)c7 ,(lte)b8 
    ,(lte)1 ,(lte)h4 
    ,(lte)1 ,(lte)h2 
};
static const lte queen_lookup_g4[] =
{
(lte)8
    ,(lte)6 ,(lte)f4 ,(lte)e4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)1 ,(lte)h4 
    ,(lte)3 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
    ,(lte)3 ,(lte)f3 ,(lte)e2 ,(lte)d1 
    ,(lte)4 ,(lte)f5 ,(lte)e6 ,(lte)d7 ,(lte)c8 
    ,(lte)1 ,(lte)h5 
    ,(lte)1 ,(lte)h3 
};
static const lte queen_lookup_g5[] =
{
(lte)8
    ,(lte)6 ,(lte)f5 ,(lte)e5 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)1 ,(lte)h5 
    ,(lte)4 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)3 ,(lte)g6 ,(lte)g7 ,(lte)g8 
    ,(lte)4 ,(lte)f4 ,(lte)e3 ,(lte)d2 ,(lte)c1 
    ,(lte)3 ,(lte)f6 ,(lte)e7 ,(lte)d8 
    ,(lte)1 ,(lte)h6 
    ,(lte)1 ,(lte)h4 
};
static const lte queen_lookup_g6[] =
{
(lte)8
    ,(lte)6 ,(lte)f6 ,(lte)e6 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)1 ,(lte)h6 
    ,(lte)5 ,(lte)g5 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)2 ,(lte)g7 ,(lte)g8 
    ,(lte)5 ,(lte)f5 ,(lte)e4 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)2 ,(lte)f7 ,(lte)e8 
    ,(lte)1 ,(lte)h7 
    ,(lte)1 ,(lte)h5 
};
static const lte queen_lookup_g7[] =
{
(lte)8
    ,(lte)6 ,(lte)f7 ,(lte)e7 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)1 ,(lte)h7 
    ,(lte)6 ,(lte)g6 ,(lte)g5 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)1 ,(lte)g8 
    ,(lte)6 ,(lte)f6 ,(lte)e5 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)1 ,(lte)f8 
    ,(lte)1 ,(lte)h8 
    ,(lte)1 ,(lte)h6 
};
static const lte queen_lookup_g8[] =
{
(lte)5
    ,(lte)6 ,(lte)f8 ,(lte)e8 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)1 ,(lte)h8 
    ,(lte)7 ,(lte)g7 ,(lte)g6 ,(lte)g5 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)6 ,(lte)f7 ,(lte)e6 ,(lte)d5 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)1 ,(lte)h7 
};
static const lte queen_lookup_h1[] =
{
(lte)3
    ,(lte)7 ,(lte)g1 ,(lte)f1 ,(lte)e1 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)7 ,(lte)h2 ,(lte)h3 ,(lte)h4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
    ,(lte)7 ,(lte)g2 ,(lte)f3 ,(lte)e4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
};
static const lte queen_lookup_h2[] =
{
(lte)5
    ,(lte)7 ,(lte)g2 ,(lte)f2 ,(lte)e2 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)1 ,(lte)h1 
    ,(lte)6 ,(lte)h3 ,(lte)h4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
    ,(lte)1 ,(lte)g1 
    ,(lte)6 ,(lte)g3 ,(lte)f4 ,(lte)e5 ,(lte)d6 ,(lte)c7 ,(lte)b8 
};
static const lte queen_lookup_h3[] =
{
(lte)5
    ,(lte)7 ,(lte)g3 ,(lte)f3 ,(lte)e3 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)2 ,(lte)h2 ,(lte)h1 
    ,(lte)5 ,(lte)h4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
    ,(lte)2 ,(lte)g2 ,(lte)f1 
    ,(lte)5 ,(lte)g4 ,(lte)f5 ,(lte)e6 ,(lte)d7 ,(lte)c8 
};
static const lte queen_lookup_h4[] =
{
(lte)5
    ,(lte)7 ,(lte)g4 ,(lte)f4 ,(lte)e4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)3 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
    ,(lte)3 ,(lte)g3 ,(lte)f2 ,(lte)e1 
    ,(lte)4 ,(lte)g5 ,(lte)f6 ,(lte)e7 ,(lte)d8 
};
static const lte queen_lookup_h5[] =
{
(lte)5
    ,(lte)7 ,(lte)g5 ,(lte)f5 ,(lte)e5 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)4 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)3 ,(lte)h6 ,(lte)h7 ,(lte)h8 
    ,(lte)4 ,(lte)g4 ,(lte)f3 ,(lte)e2 ,(lte)d1 
    ,(lte)3 ,(lte)g6 ,(lte)f7 ,(lte)e8 
};
static const lte queen_lookup_h6[] =
{
(lte)5
    ,(lte)7 ,(lte)g6 ,(lte)f6 ,(lte)e6 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)5 ,(lte)h5 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)2 ,(lte)h7 ,(lte)h8 
    ,(lte)5 ,(lte)g5 ,(lte)f4 ,(lte)e3 ,(lte)d2 ,(lte)c1 
    ,(lte)2 ,(lte)g7 ,(lte)f8 
};
static const lte queen_lookup_h7[] =
{
(lte)5
    ,(lte)7 ,(lte)g7 ,(lte)f7 ,(lte)e7 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)6 ,(lte)h6 ,(lte)h5 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)1 ,(lte)h8 
    ,(lte)6 ,(lte)g6 ,(lte)f5 ,(lte)e4 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)1 ,(lte)g8 
};
static const lte queen_lookup_h8[] =
{
(lte)3
    ,(lte)7 ,(lte)g8 ,(lte)f8 ,(lte)e8 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)7 ,(lte)h7 ,(lte)h6 ,(lte)h5 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)7 ,(lte)g7 ,(lte)f6 ,(lte)e5 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
};

// queen_lookup
const lte *queen_lookup[] =
{
    queen_lookup_a8,
    queen_lookup_b8,
    queen_lookup_c8,
    queen_lookup_d8,
    queen_lookup_e8,
    queen_lookup_f8,
    queen_lookup_g8,
    queen_lookup_h8,
    queen_lookup_a7,
    queen_lookup_b7,
    queen_lookup_c7,
    queen_lookup_d7,
    queen_lookup_e7,
    queen_lookup_f7,
    queen_lookup_g7,
    queen_lookup_h7,
    queen_lookup_a6,
    queen_lookup_b6,
    queen_lookup_c6,
    queen_lookup_d6,
    queen_lookup_e6,
    queen_lookup_f6,
    queen_lookup_g6,
    queen_lookup_h6,
    queen_lookup_a5,
    queen_lookup_b5,
    queen_lookup_c5,
    queen_lookup_d5,
    queen_lookup_e5,
    queen_lookup_f5,
    queen_lookup_g5,
    queen_lookup_h5,
    queen_lookup_a4,
    queen_lookup_b4,
    queen_lookup_c4,
    queen_lookup_d4,
    queen_lookup_e4,
    queen_lookup_f4,
    queen_lookup_g4,
    queen_lookup_h4,
    queen_lookup_a3,
    queen_lookup_b3,
    queen_lookup_c3,
    queen_lookup_d3,
    queen_lookup_e3,
    queen_lookup_f3,
    queen_lookup_g3,
    queen_lookup_h3,
    queen_lookup_a2,
    queen_lookup_b2,
    queen_lookup_c2,
    queen_lookup_d2,
    queen_lookup_e2,
    queen_lookup_f2,
    queen_lookup_g2,
    queen_lookup_h2,
    queen_lookup_a1,
    queen_lookup_b1,
    queen_lookup_c1,
    queen_lookup_d1,
    queen_lookup_e1,
    queen_lookup_f1,
    queen_lookup_g1,
    queen_lookup_h1
};

// Rook, up to 4 rays
static const lte rook_lookup_a1[] =
{
(lte)2
    ,(lte)7 ,(lte)b1 ,(lte)c1 ,(lte)d1 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)a2 ,(lte)a3 ,(lte)a4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
};
static const lte rook_lookup_a2[] =
{
(lte)3
    ,(lte)7 ,(lte)b2 ,(lte)c2 ,(lte)d2 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)a1 
    ,(lte)6 ,(lte)a3 ,(lte)a4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
};
static const lte rook_lookup_a3[] =
{
(lte)3
    ,(lte)7 ,(lte)b3 ,(lte)c3 ,(lte)d3 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)a2 ,(lte)a1 
    ,(lte)5 ,(lte)a4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
};
static const lte rook_lookup_a4[] =
{
(lte)3
    ,(lte)7 ,(lte)b4 ,(lte)c4 ,(lte)d4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)4 ,(lte)a5 ,(lte)a6 ,(lte)a7 ,(lte)a8 
};
static const lte rook_lookup_a5[] =
{
(lte)3
    ,(lte)7 ,(lte)b5 ,(lte)c5 ,(lte)d5 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)3 ,(lte)a6 ,(lte)a7 ,(lte)a8 
};
static const lte rook_lookup_a6[] =
{
(lte)3
    ,(lte)7 ,(lte)b6 ,(lte)c6 ,(lte)d6 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)a5 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)2 ,(lte)a7 ,(lte)a8 
};
static const lte rook_lookup_a7[] =
{
(lte)3
    ,(lte)7 ,(lte)b7 ,(lte)c7 ,(lte)d7 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)a6 ,(lte)a5 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
    ,(lte)1 ,(lte)a8 
};
static const lte rook_lookup_a8[] =
{
(lte)2
    ,(lte)7 ,(lte)b8 ,(lte)c8 ,(lte)d8 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)a7 ,(lte)a6 ,(lte)a5 ,(lte)a4 ,(lte)a3 ,(lte)a2 ,(lte)a1 
};
static const lte rook_lookup_b1[] =
{
(lte)3
    ,(lte)1 ,(lte)a1 
    ,(lte)6 ,(lte)c1 ,(lte)d1 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)b2 ,(lte)b3 ,(lte)b4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
};
static const lte rook_lookup_b2[] =
{
(lte)4
    ,(lte)1 ,(lte)a2 
    ,(lte)6 ,(lte)c2 ,(lte)d2 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)b1 
    ,(lte)6 ,(lte)b3 ,(lte)b4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
};
static const lte rook_lookup_b3[] =
{
(lte)4
    ,(lte)1 ,(lte)a3 
    ,(lte)6 ,(lte)c3 ,(lte)d3 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)b2 ,(lte)b1 
    ,(lte)5 ,(lte)b4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
};
static const lte rook_lookup_b4[] =
{
(lte)4
    ,(lte)1 ,(lte)a4 
    ,(lte)6 ,(lte)c4 ,(lte)d4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)4 ,(lte)b5 ,(lte)b6 ,(lte)b7 ,(lte)b8 
};
static const lte rook_lookup_b5[] =
{
(lte)4
    ,(lte)1 ,(lte)a5 
    ,(lte)6 ,(lte)c5 ,(lte)d5 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)3 ,(lte)b6 ,(lte)b7 ,(lte)b8 
};
static const lte rook_lookup_b6[] =
{
(lte)4
    ,(lte)1 ,(lte)a6 
    ,(lte)6 ,(lte)c6 ,(lte)d6 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)b5 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)2 ,(lte)b7 ,(lte)b8 
};
static const lte rook_lookup_b7[] =
{
(lte)4
    ,(lte)1 ,(lte)a7 
    ,(lte)6 ,(lte)c7 ,(lte)d7 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)b6 ,(lte)b5 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
    ,(lte)1 ,(lte)b8 
};
static const lte rook_lookup_b8[] =
{
(lte)3
    ,(lte)1 ,(lte)a8 
    ,(lte)6 ,(lte)c8 ,(lte)d8 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)b7 ,(lte)b6 ,(lte)b5 ,(lte)b4 ,(lte)b3 ,(lte)b2 ,(lte)b1 
};
static const lte rook_lookup_c1[] =
{
(lte)3
    ,(lte)2 ,(lte)b1 ,(lte)a1 
    ,(lte)5 ,(lte)d1 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)c2 ,(lte)c3 ,(lte)c4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
};
static const lte rook_lookup_c2[] =
{
(lte)4
    ,(lte)2 ,(lte)b2 ,(lte)a2 
    ,(lte)5 ,(lte)d2 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)c1 
    ,(lte)6 ,(lte)c3 ,(lte)c4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
};
static const lte rook_lookup_c3[] =
{
(lte)4
    ,(lte)2 ,(lte)b3 ,(lte)a3 
    ,(lte)5 ,(lte)d3 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)c2 ,(lte)c1 
    ,(lte)5 ,(lte)c4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
};
static const lte rook_lookup_c4[] =
{
(lte)4
    ,(lte)2 ,(lte)b4 ,(lte)a4 
    ,(lte)5 ,(lte)d4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)4 ,(lte)c5 ,(lte)c6 ,(lte)c7 ,(lte)c8 
};
static const lte rook_lookup_c5[] =
{
(lte)4
    ,(lte)2 ,(lte)b5 ,(lte)a5 
    ,(lte)5 ,(lte)d5 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)3 ,(lte)c6 ,(lte)c7 ,(lte)c8 
};
static const lte rook_lookup_c6[] =
{
(lte)4
    ,(lte)2 ,(lte)b6 ,(lte)a6 
    ,(lte)5 ,(lte)d6 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)c5 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)2 ,(lte)c7 ,(lte)c8 
};
static const lte rook_lookup_c7[] =
{
(lte)4
    ,(lte)2 ,(lte)b7 ,(lte)a7 
    ,(lte)5 ,(lte)d7 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)c6 ,(lte)c5 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
    ,(lte)1 ,(lte)c8 
};
static const lte rook_lookup_c8[] =
{
(lte)3
    ,(lte)2 ,(lte)b8 ,(lte)a8 
    ,(lte)5 ,(lte)d8 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)c7 ,(lte)c6 ,(lte)c5 ,(lte)c4 ,(lte)c3 ,(lte)c2 ,(lte)c1 
};
static const lte rook_lookup_d1[] =
{
(lte)3
    ,(lte)3 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)4 ,(lte)e1 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)d2 ,(lte)d3 ,(lte)d4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
};
static const lte rook_lookup_d2[] =
{
(lte)4
    ,(lte)3 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)4 ,(lte)e2 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)d1 
    ,(lte)6 ,(lte)d3 ,(lte)d4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
};
static const lte rook_lookup_d3[] =
{
(lte)4
    ,(lte)3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)4 ,(lte)e3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)d2 ,(lte)d1 
    ,(lte)5 ,(lte)d4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
};
static const lte rook_lookup_d4[] =
{
(lte)4
    ,(lte)3 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)4 ,(lte)e4 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)4 ,(lte)d5 ,(lte)d6 ,(lte)d7 ,(lte)d8 
};
static const lte rook_lookup_d5[] =
{
(lte)4
    ,(lte)3 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)4 ,(lte)e5 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)3 ,(lte)d6 ,(lte)d7 ,(lte)d8 
};
static const lte rook_lookup_d6[] =
{
(lte)4
    ,(lte)3 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)4 ,(lte)e6 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)d5 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)2 ,(lte)d7 ,(lte)d8 
};
static const lte rook_lookup_d7[] =
{
(lte)4
    ,(lte)3 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)4 ,(lte)e7 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)d6 ,(lte)d5 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
    ,(lte)1 ,(lte)d8 
};
static const lte rook_lookup_d8[] =
{
(lte)3
    ,(lte)3 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)4 ,(lte)e8 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)d7 ,(lte)d6 ,(lte)d5 ,(lte)d4 ,(lte)d3 ,(lte)d2 ,(lte)d1 
};
static const lte rook_lookup_e1[] =
{
(lte)3
    ,(lte)4 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)3 ,(lte)f1 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)e2 ,(lte)e3 ,(lte)e4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
};
static const lte rook_lookup_e2[] =
{
(lte)4
    ,(lte)4 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)3 ,(lte)f2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)e1 
    ,(lte)6 ,(lte)e3 ,(lte)e4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
};
static const lte rook_lookup_e3[] =
{
(lte)4
    ,(lte)4 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)3 ,(lte)f3 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)e2 ,(lte)e1 
    ,(lte)5 ,(lte)e4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
};
static const lte rook_lookup_e4[] =
{
(lte)4
    ,(lte)4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)3 ,(lte)f4 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)4 ,(lte)e5 ,(lte)e6 ,(lte)e7 ,(lte)e8 
};
static const lte rook_lookup_e5[] =
{
(lte)4
    ,(lte)4 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)3 ,(lte)f5 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)3 ,(lte)e6 ,(lte)e7 ,(lte)e8 
};
static const lte rook_lookup_e6[] =
{
(lte)4
    ,(lte)4 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)3 ,(lte)f6 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)e5 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)2 ,(lte)e7 ,(lte)e8 
};
static const lte rook_lookup_e7[] =
{
(lte)4
    ,(lte)4 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)3 ,(lte)f7 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)e6 ,(lte)e5 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
    ,(lte)1 ,(lte)e8 
};
static const lte rook_lookup_e8[] =
{
(lte)3
    ,(lte)4 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)3 ,(lte)f8 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)e7 ,(lte)e6 ,(lte)e5 ,(lte)e4 ,(lte)e3 ,(lte)e2 ,(lte)e1 
};
static const lte rook_lookup_f1[] =
{
(lte)3
    ,(lte)5 ,(lte)e1 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)2 ,(lte)g1 ,(lte)h1 
    ,(lte)7 ,(lte)f2 ,(lte)f3 ,(lte)f4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
};
static const lte rook_lookup_f2[] =
{
(lte)4
    ,(lte)5 ,(lte)e2 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)2 ,(lte)g2 ,(lte)h2 
    ,(lte)1 ,(lte)f1 
    ,(lte)6 ,(lte)f3 ,(lte)f4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
};
static const lte rook_lookup_f3[] =
{
(lte)4
    ,(lte)5 ,(lte)e3 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)2 ,(lte)g3 ,(lte)h3 
    ,(lte)2 ,(lte)f2 ,(lte)f1 
    ,(lte)5 ,(lte)f4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
};
static const lte rook_lookup_f4[] =
{
(lte)4
    ,(lte)5 ,(lte)e4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)2 ,(lte)g4 ,(lte)h4 
    ,(lte)3 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)4 ,(lte)f5 ,(lte)f6 ,(lte)f7 ,(lte)f8 
};
static const lte rook_lookup_f5[] =
{
(lte)4
    ,(lte)5 ,(lte)e5 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)2 ,(lte)g5 ,(lte)h5 
    ,(lte)4 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)3 ,(lte)f6 ,(lte)f7 ,(lte)f8 
};
static const lte rook_lookup_f6[] =
{
(lte)4
    ,(lte)5 ,(lte)e6 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)2 ,(lte)g6 ,(lte)h6 
    ,(lte)5 ,(lte)f5 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)2 ,(lte)f7 ,(lte)f8 
};
static const lte rook_lookup_f7[] =
{
(lte)4
    ,(lte)5 ,(lte)e7 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)2 ,(lte)g7 ,(lte)h7 
    ,(lte)6 ,(lte)f6 ,(lte)f5 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
    ,(lte)1 ,(lte)f8 
};
static const lte rook_lookup_f8[] =
{
(lte)3
    ,(lte)5 ,(lte)e8 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)2 ,(lte)g8 ,(lte)h8 
    ,(lte)7 ,(lte)f7 ,(lte)f6 ,(lte)f5 ,(lte)f4 ,(lte)f3 ,(lte)f2 ,(lte)f1 
};
static const lte rook_lookup_g1[] =
{
(lte)3
    ,(lte)6 ,(lte)f1 ,(lte)e1 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)1 ,(lte)h1 
    ,(lte)7 ,(lte)g2 ,(lte)g3 ,(lte)g4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
};
static const lte rook_lookup_g2[] =
{
(lte)4
    ,(lte)6 ,(lte)f2 ,(lte)e2 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)1 ,(lte)h2 
    ,(lte)1 ,(lte)g1 
    ,(lte)6 ,(lte)g3 ,(lte)g4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
};
static const lte rook_lookup_g3[] =
{
(lte)4
    ,(lte)6 ,(lte)f3 ,(lte)e3 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)1 ,(lte)h3 
    ,(lte)2 ,(lte)g2 ,(lte)g1 
    ,(lte)5 ,(lte)g4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
};
static const lte rook_lookup_g4[] =
{
(lte)4
    ,(lte)6 ,(lte)f4 ,(lte)e4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)1 ,(lte)h4 
    ,(lte)3 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)4 ,(lte)g5 ,(lte)g6 ,(lte)g7 ,(lte)g8 
};
static const lte rook_lookup_g5[] =
{
(lte)4
    ,(lte)6 ,(lte)f5 ,(lte)e5 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)1 ,(lte)h5 
    ,(lte)4 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)3 ,(lte)g6 ,(lte)g7 ,(lte)g8 
};
static const lte rook_lookup_g6[] =
{
(lte)4
    ,(lte)6 ,(lte)f6 ,(lte)e6 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)1 ,(lte)h6 
    ,(lte)5 ,(lte)g5 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)2 ,(lte)g7 ,(lte)g8 
};
static const lte rook_lookup_g7[] =
{
(lte)4
    ,(lte)6 ,(lte)f7 ,(lte)e7 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)1 ,(lte)h7 
    ,(lte)6 ,(lte)g6 ,(lte)g5 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
    ,(lte)1 ,(lte)g8 
};
static const lte rook_lookup_g8[] =
{
(lte)3
    ,(lte)6 ,(lte)f8 ,(lte)e8 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)1 ,(lte)h8 
    ,(lte)7 ,(lte)g7 ,(lte)g6 ,(lte)g5 ,(lte)g4 ,(lte)g3 ,(lte)g2 ,(lte)g1 
};
static const lte rook_lookup_h1[] =
{
(lte)2
    ,(lte)7 ,(lte)g1 ,(lte)f1 ,(lte)e1 ,(lte)d1 ,(lte)c1 ,(lte)b1 ,(lte)a1 
    ,(lte)7 ,(lte)h2 ,(lte)h3 ,(lte)h4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
};
static const lte rook_lookup_h2[] =
{
(lte)3
    ,(lte)7 ,(lte)g2 ,(lte)f2 ,(lte)e2 ,(lte)d2 ,(lte)c2 ,(lte)b2 ,(lte)a2 
    ,(lte)1 ,(lte)h1 
    ,(lte)6 ,(lte)h3 ,(lte)h4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
};
static const lte rook_lookup_h3[] =
{
(lte)3
    ,(lte)7 ,(lte)g3 ,(lte)f3 ,(lte)e3 ,(lte)d3 ,(lte)c3 ,(lte)b3 ,(lte)a3 
    ,(lte)2 ,(lte)h2 ,(lte)h1 
    ,(lte)5 ,(lte)h4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
};
static const lte rook_lookup_h4[] =
{
(lte)3
    ,(lte)7 ,(lte)g4 ,(lte)f4 ,(lte)e4 ,(lte)d4 ,(lte)c4 ,(lte)b4 ,(lte)a4 
    ,(lte)3 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)4 ,(lte)h5 ,(lte)h6 ,(lte)h7 ,(lte)h8 
};
static const lte rook_lookup_h5[] =
{
(lte)3
    ,(lte)7 ,(lte)g5 ,(lte)f5 ,(lte)e5 ,(lte)d5 ,(lte)c5 ,(lte)b5 ,(lte)a5 
    ,(lte)4 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)3 ,(lte)h6 ,(lte)h7 ,(lte)h8 
};
static const lte rook_lookup_h6[] =
{
(lte)3
    ,(lte)7 ,(lte)g6 ,(lte)f6 ,(lte)e6 ,(lte)d6 ,(lte)c6 ,(lte)b6 ,(lte)a6 
    ,(lte)5 ,(lte)h5 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)2 ,(lte)h7 ,(lte)h8 
};
static const lte rook_lookup_h7[] =
{
(lte)3
    ,(lte)7 ,(lte)g7 ,(lte)f7 ,(lte)e7 ,(lte)d7 ,(lte)c7 ,(lte)b7 ,(lte)a7 
    ,(lte)6 ,(lte)h6 ,(lte)h5 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
    ,(lte)1 ,(lte)h8 
};
static const lte rook_lookup_h8[] =
{
(lte)2
    ,(lte)7 ,(lte)g8 ,(lte)f8 ,(lte)e8 ,(lte)d8 ,(lte)c8 ,(lte)b8 ,(lte)a8 
    ,(lte)7 ,(lte)h7 ,(lte)h6 ,(lte)h5 ,(lte)h4 ,(lte)h3 ,(lte)h2 ,(lte)h1 
};

// rook_lookup
const lte *rook_lookup[] =
{
    rook_lookup_a8,
    rook_lookup_b8,
    rook_lookup_c8,
    rook_lookup_d8,
    rook_lookup_e8,
    rook_lookup_f8,
    rook_lookup_g8,
    rook_lookup_h8,
    rook_lookup_a7,
    rook_lookup_b7,
    rook_lookup_c7,
    rook_lookup_d7,
    rook_lookup_e7,
    rook_lookup_f7,
    rook_lookup_g7,
    rook_lookup_h7,
    rook_lookup_a6,
    rook_lookup_b6,
    rook_lookup_c6,
    rook_lookup_d6,
    rook_lookup_e6,
    rook_lookup_f6,
    rook_lookup_g6,
    rook_lookup_h6,
    rook_lookup_a5,
    rook_lookup_b5,
    rook_lookup_c5,
    rook_lookup_d5,
    rook_lookup_e5,
    rook_lookup_f5,
    rook_lookup_g5,
    rook_lookup_h5,
    rook_lookup_a4,
    rook_lookup_b4,
    rook_lookup_c4,
    rook_lookup_d4,
    rook_lookup_e4,
    rook_lookup_f4,
    rook_lookup_g4,
    rook_lookup_h4,
    rook_lookup_a3,
    rook_lookup_b3,
    rook_lookup_c3,
    rook_lookup_d3,
    rook_lookup_e3,
    rook_lookup_f3,
    rook_lookup_g3,
    rook_lookup_h3,
    rook_lookup_a2,
    rook_lookup_b2,
    rook_lookup_c2,
    rook_lookup_d2,
    rook_lookup_e2,
    rook_lookup_f2,
    rook_lookup_g2,
    rook_lookup_h2,
    rook_lookup_a1,
    rook_lookup_b1,
    rook_lookup_c1,
    rook_lookup_d1,
    rook_lookup_e1,
    rook_lookup_f1,
    rook_lookup_g1,
    rook_lookup_h1
};

// Bishop, up to 4 rays
static const lte bishop_lookup_a1[] =
{
(lte)1
    ,(lte)7 ,(lte)b2 ,(lte)c3 ,(lte)d4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
};
static const lte bishop_lookup_a2[] =
{
(lte)2
    ,(lte)6 ,(lte)b3 ,(lte)c4 ,(lte)d5 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)1 ,(lte)b1 
};
static const lte bishop_lookup_a3[] =
{
(lte)2
    ,(lte)5 ,(lte)b4 ,(lte)c5 ,(lte)d6 ,(lte)e7 ,(lte)f8 
    ,(lte)2 ,(lte)b2 ,(lte)c1 
};
static const lte bishop_lookup_a4[] =
{
(lte)2
    ,(lte)4 ,(lte)b5 ,(lte)c6 ,(lte)d7 ,(lte)e8 
    ,(lte)3 ,(lte)b3 ,(lte)c2 ,(lte)d1 
};
static const lte bishop_lookup_a5[] =
{
(lte)2
    ,(lte)3 ,(lte)b6 ,(lte)c7 ,(lte)d8 
    ,(lte)4 ,(lte)b4 ,(lte)c3 ,(lte)d2 ,(lte)e1 
};
static const lte bishop_lookup_a6[] =
{
(lte)2
    ,(lte)2 ,(lte)b7 ,(lte)c8 
    ,(lte)5 ,(lte)b5 ,(lte)c4 ,(lte)d3 ,(lte)e2 ,(lte)f1 
};
static const lte bishop_lookup_a7[] =
{
(lte)2
    ,(lte)1 ,(lte)b8 
    ,(lte)6 ,(lte)b6 ,(lte)c5 ,(lte)d4 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte bishop_lookup_a8[] =
{
(lte)1
    ,(lte)7 ,(lte)b7 ,(lte)c6 ,(lte)d5 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte bishop_lookup_b1[] =
{
(lte)2
    ,(lte)1 ,(lte)a2 
    ,(lte)6 ,(lte)c2 ,(lte)d3 ,(lte)e4 ,(lte)f5 ,(lte)g6 ,(lte)h7 
};
static const lte bishop_lookup_b2[] =
{
(lte)4
    ,(lte)1 ,(lte)a1 
    ,(lte)1 ,(lte)a3 
    ,(lte)6 ,(lte)c3 ,(lte)d4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)1 ,(lte)c1 
};
static const lte bishop_lookup_b3[] =
{
(lte)4
    ,(lte)1 ,(lte)a2 
    ,(lte)1 ,(lte)a4 
    ,(lte)5 ,(lte)c4 ,(lte)d5 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)2 ,(lte)c2 ,(lte)d1 
};
static const lte bishop_lookup_b4[] =
{
(lte)4
    ,(lte)1 ,(lte)a3 
    ,(lte)1 ,(lte)a5 
    ,(lte)4 ,(lte)c5 ,(lte)d6 ,(lte)e7 ,(lte)f8 
    ,(lte)3 ,(lte)c3 ,(lte)d2 ,(lte)e1 
};
static const lte bishop_lookup_b5[] =
{
(lte)4
    ,(lte)1 ,(lte)a4 
    ,(lte)1 ,(lte)a6 
    ,(lte)3 ,(lte)c6 ,(lte)d7 ,(lte)e8 
    ,(lte)4 ,(lte)c4 ,(lte)d3 ,(lte)e2 ,(lte)f1 
};
static const lte bishop_lookup_b6[] =
{
(lte)4
    ,(lte)1 ,(lte)a5 
    ,(lte)1 ,(lte)a7 
    ,(lte)2 ,(lte)c7 ,(lte)d8 
    ,(lte)5 ,(lte)c5 ,(lte)d4 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte bishop_lookup_b7[] =
{
(lte)4
    ,(lte)1 ,(lte)a6 
    ,(lte)1 ,(lte)a8 
    ,(lte)1 ,(lte)c8 
    ,(lte)6 ,(lte)c6 ,(lte)d5 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte bishop_lookup_b8[] =
{
(lte)2
    ,(lte)1 ,(lte)a7 
    ,(lte)6 ,(lte)c7 ,(lte)d6 ,(lte)e5 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte bishop_lookup_c1[] =
{
(lte)2
    ,(lte)2 ,(lte)b2 ,(lte)a3 
    ,(lte)5 ,(lte)d2 ,(lte)e3 ,(lte)f4 ,(lte)g5 ,(lte)h6 
};
static const lte bishop_lookup_c2[] =
{
(lte)4
    ,(lte)1 ,(lte)b1 
    ,(lte)2 ,(lte)b3 ,(lte)a4 
    ,(lte)5 ,(lte)d3 ,(lte)e4 ,(lte)f5 ,(lte)g6 ,(lte)h7 
    ,(lte)1 ,(lte)d1 
};
static const lte bishop_lookup_c3[] =
{
(lte)4
    ,(lte)2 ,(lte)b2 ,(lte)a1 
    ,(lte)2 ,(lte)b4 ,(lte)a5 
    ,(lte)5 ,(lte)d4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)2 ,(lte)d2 ,(lte)e1 
};
static const lte bishop_lookup_c4[] =
{
(lte)4
    ,(lte)2 ,(lte)b3 ,(lte)a2 
    ,(lte)2 ,(lte)b5 ,(lte)a6 
    ,(lte)4 ,(lte)d5 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)3 ,(lte)d3 ,(lte)e2 ,(lte)f1 
};
static const lte bishop_lookup_c5[] =
{
(lte)4
    ,(lte)2 ,(lte)b4 ,(lte)a3 
    ,(lte)2 ,(lte)b6 ,(lte)a7 
    ,(lte)3 ,(lte)d6 ,(lte)e7 ,(lte)f8 
    ,(lte)4 ,(lte)d4 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte bishop_lookup_c6[] =
{
(lte)4
    ,(lte)2 ,(lte)b5 ,(lte)a4 
    ,(lte)2 ,(lte)b7 ,(lte)a8 
    ,(lte)2 ,(lte)d7 ,(lte)e8 
    ,(lte)5 ,(lte)d5 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte bishop_lookup_c7[] =
{
(lte)4
    ,(lte)2 ,(lte)b6 ,(lte)a5 
    ,(lte)1 ,(lte)b8 
    ,(lte)1 ,(lte)d8 
    ,(lte)5 ,(lte)d6 ,(lte)e5 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte bishop_lookup_c8[] =
{
(lte)2
    ,(lte)2 ,(lte)b7 ,(lte)a6 
    ,(lte)5 ,(lte)d7 ,(lte)e6 ,(lte)f5 ,(lte)g4 ,(lte)h3 
};
static const lte bishop_lookup_d1[] =
{
(lte)2
    ,(lte)3 ,(lte)c2 ,(lte)b3 ,(lte)a4 
    ,(lte)4 ,(lte)e2 ,(lte)f3 ,(lte)g4 ,(lte)h5 
};
static const lte bishop_lookup_d2[] =
{
(lte)4
    ,(lte)1 ,(lte)c1 
    ,(lte)3 ,(lte)c3 ,(lte)b4 ,(lte)a5 
    ,(lte)4 ,(lte)e3 ,(lte)f4 ,(lte)g5 ,(lte)h6 
    ,(lte)1 ,(lte)e1 
};
static const lte bishop_lookup_d3[] =
{
(lte)4
    ,(lte)2 ,(lte)c2 ,(lte)b1 
    ,(lte)3 ,(lte)c4 ,(lte)b5 ,(lte)a6 
    ,(lte)4 ,(lte)e4 ,(lte)f5 ,(lte)g6 ,(lte)h7 
    ,(lte)2 ,(lte)e2 ,(lte)f1 
};
static const lte bishop_lookup_d4[] =
{
(lte)4
    ,(lte)3 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)3 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)4 ,(lte)e5 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)3 ,(lte)e3 ,(lte)f2 ,(lte)g1 
};
static const lte bishop_lookup_d5[] =
{
(lte)4
    ,(lte)3 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)3 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)3 ,(lte)e6 ,(lte)f7 ,(lte)g8 
    ,(lte)4 ,(lte)e4 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte bishop_lookup_d6[] =
{
(lte)4
    ,(lte)3 ,(lte)c5 ,(lte)b4 ,(lte)a3 
    ,(lte)2 ,(lte)c7 ,(lte)b8 
    ,(lte)2 ,(lte)e7 ,(lte)f8 
    ,(lte)4 ,(lte)e5 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte bishop_lookup_d7[] =
{
(lte)4
    ,(lte)3 ,(lte)c6 ,(lte)b5 ,(lte)a4 
    ,(lte)1 ,(lte)c8 
    ,(lte)1 ,(lte)e8 
    ,(lte)4 ,(lte)e6 ,(lte)f5 ,(lte)g4 ,(lte)h3 
};
static const lte bishop_lookup_d8[] =
{
(lte)2
    ,(lte)3 ,(lte)c7 ,(lte)b6 ,(lte)a5 
    ,(lte)4 ,(lte)e7 ,(lte)f6 ,(lte)g5 ,(lte)h4 
};
static const lte bishop_lookup_e1[] =
{
(lte)2
    ,(lte)4 ,(lte)d2 ,(lte)c3 ,(lte)b4 ,(lte)a5 
    ,(lte)3 ,(lte)f2 ,(lte)g3 ,(lte)h4 
};
static const lte bishop_lookup_e2[] =
{
(lte)4
    ,(lte)1 ,(lte)d1 
    ,(lte)4 ,(lte)d3 ,(lte)c4 ,(lte)b5 ,(lte)a6 
    ,(lte)3 ,(lte)f3 ,(lte)g4 ,(lte)h5 
    ,(lte)1 ,(lte)f1 
};
static const lte bishop_lookup_e3[] =
{
(lte)4
    ,(lte)2 ,(lte)d2 ,(lte)c1 
    ,(lte)4 ,(lte)d4 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)3 ,(lte)f4 ,(lte)g5 ,(lte)h6 
    ,(lte)2 ,(lte)f2 ,(lte)g1 
};
static const lte bishop_lookup_e4[] =
{
(lte)4
    ,(lte)3 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)3 ,(lte)f5 ,(lte)g6 ,(lte)h7 
    ,(lte)3 ,(lte)f3 ,(lte)g2 ,(lte)h1 
};
static const lte bishop_lookup_e5[] =
{
(lte)4
    ,(lte)4 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)3 ,(lte)d6 ,(lte)c7 ,(lte)b8 
    ,(lte)3 ,(lte)f6 ,(lte)g7 ,(lte)h8 
    ,(lte)3 ,(lte)f4 ,(lte)g3 ,(lte)h2 
};
static const lte bishop_lookup_e6[] =
{
(lte)4
    ,(lte)4 ,(lte)d5 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)2 ,(lte)d7 ,(lte)c8 
    ,(lte)2 ,(lte)f7 ,(lte)g8 
    ,(lte)3 ,(lte)f5 ,(lte)g4 ,(lte)h3 
};
static const lte bishop_lookup_e7[] =
{
(lte)4
    ,(lte)4 ,(lte)d6 ,(lte)c5 ,(lte)b4 ,(lte)a3 
    ,(lte)1 ,(lte)d8 
    ,(lte)1 ,(lte)f8 
    ,(lte)3 ,(lte)f6 ,(lte)g5 ,(lte)h4 
};
static const lte bishop_lookup_e8[] =
{
(lte)2
    ,(lte)4 ,(lte)d7 ,(lte)c6 ,(lte)b5 ,(lte)a4 
    ,(lte)3 ,(lte)f7 ,(lte)g6 ,(lte)h5 
};
static const lte bishop_lookup_f1[] =
{
(lte)2
    ,(lte)5 ,(lte)e2 ,(lte)d3 ,(lte)c4 ,(lte)b5 ,(lte)a6 
    ,(lte)2 ,(lte)g2 ,(lte)h3 
};
static const lte bishop_lookup_f2[] =
{
(lte)4
    ,(lte)1 ,(lte)e1 
    ,(lte)5 ,(lte)e3 ,(lte)d4 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)2 ,(lte)g3 ,(lte)h4 
    ,(lte)1 ,(lte)g1 
};
static const lte bishop_lookup_f3[] =
{
(lte)4
    ,(lte)2 ,(lte)e2 ,(lte)d1 
    ,(lte)5 ,(lte)e4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)2 ,(lte)g4 ,(lte)h5 
    ,(lte)2 ,(lte)g2 ,(lte)h1 
};
static const lte bishop_lookup_f4[] =
{
(lte)4
    ,(lte)3 ,(lte)e3 ,(lte)d2 ,(lte)c1 
    ,(lte)4 ,(lte)e5 ,(lte)d6 ,(lte)c7 ,(lte)b8 
    ,(lte)2 ,(lte)g5 ,(lte)h6 
    ,(lte)2 ,(lte)g3 ,(lte)h2 
};
static const lte bishop_lookup_f5[] =
{
(lte)4
    ,(lte)4 ,(lte)e4 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)3 ,(lte)e6 ,(lte)d7 ,(lte)c8 
    ,(lte)2 ,(lte)g6 ,(lte)h7 
    ,(lte)2 ,(lte)g4 ,(lte)h3 
};
static const lte bishop_lookup_f6[] =
{
(lte)4
    ,(lte)5 ,(lte)e5 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)2 ,(lte)e7 ,(lte)d8 
    ,(lte)2 ,(lte)g7 ,(lte)h8 
    ,(lte)2 ,(lte)g5 ,(lte)h4 
};
static const lte bishop_lookup_f7[] =
{
(lte)4
    ,(lte)5 ,(lte)e6 ,(lte)d5 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)1 ,(lte)e8 
    ,(lte)1 ,(lte)g8 
    ,(lte)2 ,(lte)g6 ,(lte)h5 
};
static const lte bishop_lookup_f8[] =
{
(lte)2
    ,(lte)5 ,(lte)e7 ,(lte)d6 ,(lte)c5 ,(lte)b4 ,(lte)a3 
    ,(lte)2 ,(lte)g7 ,(lte)h6 
};
static const lte bishop_lookup_g1[] =
{
(lte)2
    ,(lte)6 ,(lte)f2 ,(lte)e3 ,(lte)d4 ,(lte)c5 ,(lte)b6 ,(lte)a7 
    ,(lte)1 ,(lte)h2 
};
static const lte bishop_lookup_g2[] =
{
(lte)4
    ,(lte)1 ,(lte)f1 
    ,(lte)6 ,(lte)f3 ,(lte)e4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
    ,(lte)1 ,(lte)h3 
    ,(lte)1 ,(lte)h1 
};
static const lte bishop_lookup_g3[] =
{
(lte)4
    ,(lte)2 ,(lte)f2 ,(lte)e1 
    ,(lte)5 ,(lte)f4 ,(lte)e5 ,(lte)d6 ,(lte)c7 ,(lte)b8 
    ,(lte)1 ,(lte)h4 
    ,(lte)1 ,(lte)h2 
};
static const lte bishop_lookup_g4[] =
{
(lte)4
    ,(lte)3 ,(lte)f3 ,(lte)e2 ,(lte)d1 
    ,(lte)4 ,(lte)f5 ,(lte)e6 ,(lte)d7 ,(lte)c8 
    ,(lte)1 ,(lte)h5 
    ,(lte)1 ,(lte)h3 
};
static const lte bishop_lookup_g5[] =
{
(lte)4
    ,(lte)4 ,(lte)f4 ,(lte)e3 ,(lte)d2 ,(lte)c1 
    ,(lte)3 ,(lte)f6 ,(lte)e7 ,(lte)d8 
    ,(lte)1 ,(lte)h6 
    ,(lte)1 ,(lte)h4 
};
static const lte bishop_lookup_g6[] =
{
(lte)4
    ,(lte)5 ,(lte)f5 ,(lte)e4 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)2 ,(lte)f7 ,(lte)e8 
    ,(lte)1 ,(lte)h7 
    ,(lte)1 ,(lte)h5 
};
static const lte bishop_lookup_g7[] =
{
(lte)4
    ,(lte)6 ,(lte)f6 ,(lte)e5 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
    ,(lte)1 ,(lte)f8 
    ,(lte)1 ,(lte)h8 
    ,(lte)1 ,(lte)h6 
};
static const lte bishop_lookup_g8[] =
{
(lte)2
    ,(lte)6 ,(lte)f7 ,(lte)e6 ,(lte)d5 ,(lte)c4 ,(lte)b3 ,(lte)a2 
    ,(lte)1 ,(lte)h7 
};
static const lte bishop_lookup_h1[] =
{
(lte)1
    ,(lte)7 ,(lte)g2 ,(lte)f3 ,(lte)e4 ,(lte)d5 ,(lte)c6 ,(lte)b7 ,(lte)a8 
};
static const lte bishop_lookup_h2[] =
{
(lte)2
    ,(lte)1 ,(lte)g1 
    ,(lte)6 ,(lte)g3 ,(lte)f4 ,(lte)e5 ,(lte)d6 ,(lte)c7 ,(lte)b8 
};
static const lte bishop_lookup_h3[] =
{
(lte)2
    ,(lte)2 ,(lte)g2 ,(lte)f1 
    ,(lte)5 ,(lte)g4 ,(lte)f5 ,(lte)e6 ,(lte)d7 ,(lte)c8 
};
static const lte bishop_lookup_h4[] =
{
(lte)2
    ,(lte)3 ,(lte)g3 ,(lte)f2 ,(lte)e1 
    ,(lte)4 ,(lte)g5 ,(lte)f6 ,(lte)e7 ,(lte)d8 
};
static const lte bishop_lookup_h5[] =
{
(lte)2
    ,(lte)4 ,(lte)g4 ,(lte)f3 ,(lte)e2 ,(lte)d1 
    ,(lte)3 ,(lte)g6 ,(lte)f7 ,(lte)e8 
};
static const lte bishop_lookup_h6[] =
{
(lte)2
    ,(lte)5 ,(lte)g5 ,(lte)f4 ,(lte)e3 ,(lte)d2 ,(lte)c1 
    ,(lte)2 ,(lte)g7 ,(lte)f8 
};
static const lte bishop_lookup_h7[] =
{
(lte)2
    ,(lte)6 ,(lte)g6 ,(lte)f5 ,(lte)e4 ,(lte)d3 ,(lte)c2 ,(lte)b1 
    ,(lte)1 ,(lte)g8 
};
static const lte bishop_lookup_h8[] =
{
(lte)1
    ,(lte)7 ,(lte)g7 ,(lte)f6 ,(lte)e5 ,(lte)d4 ,(lte)c3 ,(lte)b2 ,(lte)a1 
};

// bishop_lookup
const lte *bishop_lookup[] =
{
    bishop_lookup_a8,
    bishop_lookup_b8,
    bishop_lookup_c8,
    bishop_lookup_d8,
    bishop_lookup_e8,
    bishop_lookup_f8,
    bishop_lookup_g8,
    bishop_lookup_h8,
    bishop_lookup_a7,
    bishop_lookup_b7,
    bishop_lookup_c7,
    bishop_lookup_d7,
    bishop_lookup_e7,
    bishop_lookup_f7,
    bishop_lookup_g7,
    bishop_lookup_h7,
    bishop_lookup_a6,
    bishop_lookup_b6,
    bishop_lookup_c6,
    bishop_lookup_d6,
    bishop_lookup_e6,
    bishop_lookup_f6,
    bishop_lookup_g6,
    bishop_lookup_h6,
    bishop_lookup_a5,
    bishop_lookup_b5,
    bishop_lookup_c5,
    bishop_lookup_d5,
    bishop_lookup_e5,
    bishop_lookup_f5,
    bishop_lookup_g5,
    bishop_lookup_h5,
    bishop_lookup_a4,
    bishop_lookup_b4,
    bishop_lookup_c4,
    bishop_lookup_d4,
    bishop_lookup_e4,
    bishop_lookup_f4,
    bishop_lookup_g4,
    bishop_lookup_h4,
    bishop_lookup_a3,
    bishop_lookup_b3,
    bishop_lookup_c3,
    bishop_lookup_d3,
    bishop_lookup_e3,
    bishop_lookup_f3,
    bishop_lookup_g3,
    bishop_lookup_h3,
    bishop_lookup_a2,
    bishop_lookup_b2,
    bishop_lookup_c2,
    bishop_lookup_d2,
    bishop_lookup_e2,
    bishop_lookup_f2,
    bishop_lookup_g2,
    bishop_lookup_h2,
    bishop_lookup_a1,
    bishop_lookup_b1,
    bishop_lookup_c1,
    bishop_lookup_d1,
    bishop_lookup_e1,
    bishop_lookup_f1,
    bishop_lookup_g1,
    bishop_lookup_h1
};

// Knight, up to 8 squares
static const lte knight_lookup_a1[] =
{
    (lte)2, (lte)c2, (lte)b3
};
static const lte knight_lookup_a2[] =
{
    (lte)3, (lte)c1, (lte)c3, (lte)b4
};
static const lte knight_lookup_a3[] =
{
    (lte)4, (lte)c2, (lte)c4, (lte)b1, (lte)b5
};
static const lte knight_lookup_a4[] =
{
    (lte)4, (lte)c3, (lte)c5, (lte)b2, (lte)b6
};
static const lte knight_lookup_a5[] =
{
    (lte)4, (lte)c4, (lte)c6, (lte)b3, (lte)b7
};
static const lte knight_lookup_a6[] =
{
    (lte)4, (lte)c5, (lte)c7, (lte)b4, (lte)b8
};
static const lte knight_lookup_a7[] =
{
    (lte)3, (lte)c6, (lte)c8, (lte)b5
};
static const lte knight_lookup_a8[] =
{
    (lte)2, (lte)c7, (lte)b6
};
static const lte knight_lookup_b1[] =
{
    (lte)3, (lte)a3, (lte)d2, (lte)c3
};
static const lte knight_lookup_b2[] =
{
    (lte)4, (lte)a4, (lte)d1, (lte)d3, (lte)c4
};
static const lte knight_lookup_b3[] =
{
    (lte)6, (lte)a1, (lte)a5, (lte)d2, (lte)d4, (lte)c1, (lte)c5
};
static const lte knight_lookup_b4[] =
{
    (lte)6, (lte)a2, (lte)a6, (lte)d3, (lte)d5, (lte)c2, (lte)c6
};
static const lte knight_lookup_b5[] =
{
    (lte)6, (lte)a3, (lte)a7, (lte)d4, (lte)d6, (lte)c3, (lte)c7
};
static const lte knight_lookup_b6[] =
{
    (lte)6, (lte)a4, (lte)a8, (lte)d5, (lte)d7, (lte)c4, (lte)c8
};
static const lte knight_lookup_b7[] =
{
    (lte)4, (lte)a5, (lte)d6, (lte)d8, (lte)c5
};
static const lte knight_lookup_b8[] =
{
    (lte)3, (lte)a6, (lte)d7, (lte)c6
};
static const lte knight_lookup_c1[] =
{
    (lte)4, (lte)a2, (lte)b3, (lte)e2, (lte)d3
};
static const lte knight_lookup_c2[] =
{
    (lte)6, (lte)a1, (lte)a3, (lte)b4, (lte)e1, (lte)e3, (lte)d4
};
static const lte knight_lookup_c3[] =
{
    (lte)8, (lte)a2, (lte)a4, (lte)b1, (lte)b5, (lte)e2, (lte)e4, (lte)d1, (lte)d5
};
static const lte knight_lookup_c4[] =
{
    (lte)8, (lte)a3, (lte)a5, (lte)b2, (lte)b6, (lte)e3, (lte)e5, (lte)d2, (lte)d6
};
static const lte knight_lookup_c5[] =
{
    (lte)8, (lte)a4, (lte)a6, (lte)b3, (lte)b7, (lte)e4, (lte)e6, (lte)d3, (lte)d7
};
static const lte knight_lookup_c6[] =
{
    (lte)8, (lte)a5, (lte)a7, (lte)b4, (lte)b8, (lte)e5, (lte)e7, (lte)d4, (lte)d8
};
static const lte knight_lookup_c7[] =
{
    (lte)6, (lte)a6, (lte)a8, (lte)b5, (lte)e6, (lte)e8, (lte)d5
};
static const lte knight_lookup_c8[] =
{
    (lte)4, (lte)a7, (lte)b6, (lte)e7, (lte)d6
};
static const lte knight_lookup_d1[] =
{
    (lte)4, (lte)b2, (lte)c3, (lte)f2, (lte)e3
};
static const lte knight_lookup_d2[] =
{
    (lte)6, (lte)b1, (lte)b3, (lte)c4, (lte)f1, (lte)f3, (lte)e4
};
static const lte knight_lookup_d3[] =
{
    (lte)8, (lte)b2, (lte)b4, (lte)c1, (lte)c5, (lte)f2, (lte)f4, (lte)e1, (lte)e5
};
static const lte knight_lookup_d4[] =
{
    (lte)8, (lte)b3, (lte)b5, (lte)c2, (lte)c6, (lte)f3, (lte)f5, (lte)e2, (lte)e6
};
static const lte knight_lookup_d5[] =
{
    (lte)8, (lte)b4, (lte)b6, (lte)c3, (lte)c7, (lte)f4, (lte)f6, (lte)e3, (lte)e7
};
static const lte knight_lookup_d6[] =
{
    (lte)8, (lte)b5, (lte)b7, (lte)c4, (lte)c8, (lte)f5, (lte)f7, (lte)e4, (lte)e8
};
static const lte knight_lookup_d7[] =
{
    (lte)6, (lte)b6, (lte)b8, (lte)c5, (lte)f6, (lte)f8, (lte)e5
};
static const lte knight_lookup_d8[] =
{
    (lte)4, (lte)b7, (lte)c6, (lte)f7, (lte)e6
};
static const lte knight_lookup_e1[] =
{
    (lte)4, (lte)c2, (lte)d3, (lte)g2, (lte)f3
};
static const lte knight_lookup_e2[] =
{
    (lte)6, (lte)c1, (lte)c3, (lte)d4, (lte)g1, (lte)g3, (lte)f4
};
static const lte knight_lookup_e3[] =
{
    (lte)8, (lte)c2, (lte)c4, (lte)d1, (lte)d5, (lte)g2, (lte)g4, (lte)f1, (lte)f5
};
static const lte knight_lookup_e4[] =
{
    (lte)8, (lte)c3, (lte)c5, (lte)d2, (lte)d6, (lte)g3, (lte)g5, (lte)f2, (lte)f6
};
static const lte knight_lookup_e5[] =
{
    (lte)8, (lte)c4, (lte)c6, (lte)d3, (lte)d7, (lte)g4, (lte)g6, (lte)f3, (lte)f7
};
static const lte knight_lookup_e6[] =
{
    (lte)8, (lte)c5, (lte)c7, (lte)d4, (lte)d8, (lte)g5, (lte)g7, (lte)f4, (lte)f8
};
static const lte knight_lookup_e7[] =
{
    (lte)6, (lte)c6, (lte)c8, (lte)d5, (lte)g6, (lte)g8, (lte)f5
};
static const lte knight_lookup_e8[] =
{
    (lte)4, (lte)c7, (lte)d6, (lte)g7, (lte)f6
};
static const lte knight_lookup_f1[] =
{
    (lte)4, (lte)d2, (lte)e3, (lte)h2, (lte)g3
};
static const lte knight_lookup_f2[] =
{
    (lte)6, (lte)d1, (lte)d3, (lte)e4, (lte)h1, (lte)h3, (lte)g4
};
static const lte knight_lookup_f3[] =
{
    (lte)8, (lte)d2, (lte)d4, (lte)e1, (lte)e5, (lte)h2, (lte)h4, (lte)g1, (lte)g5
};
static const lte knight_lookup_f4[] =
{
    (lte)8, (lte)d3, (lte)d5, (lte)e2, (lte)e6, (lte)h3, (lte)h5, (lte)g2, (lte)g6
};
static const lte knight_lookup_f5[] =
{
    (lte)8, (lte)d4, (lte)d6, (lte)e3, (lte)e7, (lte)h4, (lte)h6, (lte)g3, (lte)g7
};
static const lte knight_lookup_f6[] =
{
    (lte)8, (lte)d5, (lte)d7, (lte)e4, (lte)e8, (lte)h5, (lte)h7, (lte)g4, (lte)g8
};
static const lte knight_lookup_f7[] =
{
    (lte)6, (lte)d6, (lte)d8, (lte)e5, (lte)h6, (lte)h8, (lte)g5
};
static const lte knight_lookup_f8[] =
{
    (lte)4, (lte)d7, (lte)e6, (lte)h7, (lte)g6
};
static const lte knight_lookup_g1[] =
{
    (lte)3, (lte)e2, (lte)f3, (lte)h3
};
static const lte knight_lookup_g2[] =
{
    (lte)4, (lte)e1, (lte)e3, (lte)f4, (lte)h4
};
static const lte knight_lookup_g3[] =
{
    (lte)6, (lte)e2, (lte)e4, (lte)f1, (lte)f5, (lte)h1, (lte)h5
};
static const lte knight_lookup_g4[] =
{
    (lte)6, (lte)e3, (lte)e5, (lte)f2, (lte)f6, (lte)h2, (lte)h6
};
static const lte knight_lookup_g5[] =
{
    (lte)6, (lte)e4, (lte)e6, (lte)f3, (lte)f7, (lte)h3, (lte)h7
};
static const lte knight_lookup_g6[] =
{
    (lte)6, (lte)e5, (lte)e7, (lte)f4, (lte)f8, (lte)h4, (lte)h8
};
static const lte knight_lookup_g7[] =
{
    (lte)4, (lte)e6, (lte)e8, (lte)f5, (lte)h5
};
static const lte knight_lookup_g8[] =
{
    (lte)3, (lte)e7, (lte)f6, (lte)h6
};
static const lte knight_lookup_h1[] =
{
    (lte)2, (lte)f2, (lte)g3
};
static const lte knight_lookup_h2[] =
{
    (lte)3, (lte)f1, (lte)f3, (lte)g4
};
static const lte knight_lookup_h3[] =
{
    (lte)4, (lte)f2, (lte)f4, (lte)g1, (lte)g5
};
static const lte knight_lookup_h4[] =
{
    (lte)4, (lte)f3, (lte)f5, (lte)g2, (lte)g6
};
static const lte knight_lookup_h5[] =
{
    (lte)4, (lte)f4, (lte)f6, (lte)g3, (lte)g7
};
static const lte knight_lookup_h6[] =
{
    (lte)4, (lte)f5, (lte)f7, (lte)g4, (lte)g8
};
static const lte knight_lookup_h7[] =
{
    (lte)3, (lte)f6, (lte)f8, (lte)g5
};
static const lte knight_lookup_h8[] =
{
    (lte)2, (lte)f7, (lte)g6
};

// knight_lookup
const lte *knight_lookup[] =
{
    knight_lookup_a8,
    knight_lookup_b8,
    knight_lookup_c8,
    knight_lookup_d8,
    knight_lookup_e8,
    knight_lookup_f8,
    knight_lookup_g8,
    knight_lookup_h8,
    knight_lookup_a7,
    knight_lookup_b7,
    knight_lookup_c7,
    knight_lookup_d7,
    knight_lookup_e7,
    knight_lookup_f7,
    knight_lookup_g7,
    knight_lookup_h7,
    knight_lookup_a6,
    knight_lookup_b6,
    knight_lookup_c6,
    knight_lookup_d6,
    knight_lookup_e6,
    knight_lookup_f6,
    knight_lookup_g6,
    knight_lookup_h6,
    knight_lookup_a5,
    knight_lookup_b5,
    knight_lookup_c5,
    knight_lookup_d5,
    knight_lookup_e5,
    knight_lookup_f5,
    knight_lookup_g5,
    knight_lookup_h5,
    knight_lookup_a4,
    knight_lookup_b4,
    knight_lookup_c4,
    knight_lookup_d4,
    knight_lookup_e4,
    knight_lookup_f4,
    knight_lookup_g4,
    knight_lookup_h4,
    knight_lookup_a3,
    knight_lookup_b3,
    knight_lookup_c3,
    knight_lookup_d3,
    knight_lookup_e3,
    knight_lookup_f3,
    knight_lookup_g3,
    knight_lookup_h3,
    knight_lookup_a2,
    knight_lookup_b2,
    knight_lookup_c2,
    knight_lookup_d2,
    knight_lookup_e2,
    knight_lookup_f2,
    knight_lookup_g2,
    knight_lookup_h2,
    knight_lookup_a1,
    knight_lookup_b1,
    knight_lookup_c1,
    knight_lookup_d1,
    knight_lookup_e1,
    knight_lookup_f1,
    knight_lookup_g1,
    knight_lookup_h1
};

// King, up to 8 squares
static const lte king_lookup_a1[] =
{
    (lte)3, (lte)b2, (lte)b1, (lte)a2
};
static const lte king_lookup_a2[] =
{
    (lte)5, (lte)b1, (lte)b3, (lte)b2, (lte)a1, (lte)a3
};
static const lte king_lookup_a3[] =
{
    (lte)5, (lte)b2, (lte)b4, (lte)b3, (lte)a2, (lte)a4
};
static const lte king_lookup_a4[] =
{
    (lte)5, (lte)b3, (lte)b5, (lte)b4, (lte)a3, (lte)a5
};
static const lte king_lookup_a5[] =
{
    (lte)5, (lte)b4, (lte)b6, (lte)b5, (lte)a4, (lte)a6
};
static const lte king_lookup_a6[] =
{
    (lte)5, (lte)b5, (lte)b7, (lte)b6, (lte)a5, (lte)a7
};
static const lte king_lookup_a7[] =
{
    (lte)5, (lte)b6, (lte)b8, (lte)b7, (lte)a6, (lte)a8
};
static const lte king_lookup_a8[] =
{
    (lte)3, (lte)b7, (lte)b8, (lte)a7
};
static const lte king_lookup_b1[] =
{
    (lte)5, (lte)a2, (lte)c2, (lte)a1, (lte)c1, (lte)b2
};
static const lte king_lookup_b2[] =
{
    (lte)8, (lte)a1, (lte)a3, (lte)c1, (lte)c3, (lte)a2, (lte)c2, (lte)b1, (lte)b3
};
static const lte king_lookup_b3[] =
{
    (lte)8, (lte)a2, (lte)a4, (lte)c2, (lte)c4, (lte)a3, (lte)c3, (lte)b2, (lte)b4
};
static const lte king_lookup_b4[] =
{
    (lte)8, (lte)a3, (lte)a5, (lte)c3, (lte)c5, (lte)a4, (lte)c4, (lte)b3, (lte)b5
};
static const lte king_lookup_b5[] =
{
    (lte)8, (lte)a4, (lte)a6, (lte)c4, (lte)c6, (lte)a5, (lte)c5, (lte)b4, (lte)b6
};
static const lte king_lookup_b6[] =
{
    (lte)8, (lte)a5, (lte)a7, (lte)c5, (lte)c7, (lte)a6, (lte)c6, (lte)b5, (lte)b7
};
static const lte king_lookup_b7[] =
{
    (lte)8, (lte)a6, (lte)a8, (lte)c6, (lte)c8, (lte)a7, (lte)c7, (lte)b6, (lte)b8
};
static const lte king_lookup_b8[] =
{
    (lte)5, (lte)a7, (lte)c7, (lte)a8, (lte)c8, (lte)b7
};
static const lte king_lookup_c1[] =
{
    (lte)5, (lte)b2, (lte)d2, (lte)b1, (lte)d1, (lte)c2
};
static const lte king_lookup_c2[] =
{
    (lte)8, (lte)b1, (lte)b3, (lte)d1, (lte)d3, (lte)b2, (lte)d2, (lte)c1, (lte)c3
};
static const lte king_lookup_c3[] =
{
    (lte)8, (lte)b2, (lte)b4, (lte)d2, (lte)d4, (lte)b3, (lte)d3, (lte)c2, (lte)c4
};
static const lte king_lookup_c4[] =
{
    (lte)8, (lte)b3, (lte)b5, (lte)d3, (lte)d5, (lte)b4, (lte)d4, (lte)c3, (lte)c5
};
static const lte king_lookup_c5[] =
{
    (lte)8, (lte)b4, (lte)b6, (lte)d4, (lte)d6, (lte)b5, (lte)d5, (lte)c4, (lte)c6
};
static const lte king_lookup_c6[] =
{
    (lte)8, (lte)b5, (lte)b7, (lte)d5, (lte)d7, (lte)b6, (lte)d6, (lte)c5, (lte)c7
};
static const lte king_lookup_c7[] =
{
    (lte)8, (lte)b6, (lte)b8, (lte)d6, (lte)d8, (lte)b7, (lte)d7, (lte)c6, (lte)c8
};
static const lte king_lookup_c8[] =
{
    (lte)5, (lte)b7, (lte)d7, (lte)b8, (lte)d8, (lte)c7
};
static const lte king_lookup_d1[] =
{
    (lte)5, (lte)c2, (lte)e2, (lte)c1, (lte)e1, (lte)d2
};
static const lte king_lookup_d2[] =
{
    (lte)8, (lte)c1, (lte)c3, (lte)e1, (lte)e3, (lte)c2, (lte)e2, (lte)d1, (lte)d3
};
static const lte king_lookup_d3[] =
{
    (lte)8, (lte)c2, (lte)c4, (lte)e2, (lte)e4, (lte)c3, (lte)e3, (lte)d2, (lte)d4
};
static const lte king_lookup_d4[] =
{
    (lte)8, (lte)c3, (lte)c5, (lte)e3, (lte)e5, (lte)c4, (lte)e4, (lte)d3, (lte)d5
};
static const lte king_lookup_d5[] =
{
    (lte)8, (lte)c4, (lte)c6, (lte)e4, (lte)e6, (lte)c5, (lte)e5, (lte)d4, (lte)d6
};
static const lte king_lookup_d6[] =
{
    (lte)8, (lte)c5, (lte)c7, (lte)e5, (lte)e7, (lte)c6, (lte)e6, (lte)d5, (lte)d7
};
static const lte king_lookup_d7[] =
{
    (lte)8, (lte)c6, (lte)c8, (lte)e6, (lte)e8, (lte)c7, (lte)e7, (lte)d6, (lte)d8
};
static const lte king_lookup_d8[] =
{
    (lte)5, (lte)c7, (lte)e7, (lte)c8, (lte)e8, (lte)d7
};
static const lte king_lookup_e1[] =
{
    (lte)5, (lte)d2, (lte)f2, (lte)d1, (lte)f1, (lte)e2
};
static const lte king_lookup_e2[] =
{
    (lte)8, (lte)d1, (lte)d3, (lte)f1, (lte)f3, (lte)d2, (lte)f2, (lte)e1, (lte)e3
};
static const lte king_lookup_e3[] =
{
    (lte)8, (lte)d2, (lte)d4, (lte)f2, (lte)f4, (lte)d3, (lte)f3, (lte)e2, (lte)e4
};
static const lte king_lookup_e4[] =
{
    (lte)8, (lte)d3, (lte)d5, (lte)f3, (lte)f5, (lte)d4, (lte)f4, (lte)e3, (lte)e5
};
static const lte king_lookup_e5[] =
{
    (lte)8, (lte)d4, (lte)d6, (lte)f4, (lte)f6, (lte)d5, (lte)f5, (lte)e4, (lte)e6
};
static const lte king_lookup_e6[] =
{
    (lte)8, (lte)d5, (lte)d7, (lte)f5, (lte)f7, (lte)d6, (lte)f6, (lte)e5, (lte)e7
};
static const lte king_lookup_e7[] =
{
    (lte)8, (lte)d6, (lte)d8, (lte)f6, (lte)f8, (lte)d7, (lte)f7, (lte)e6, (lte)e8
};
static const lte king_lookup_e8[] =
{
    (lte)5, (lte)d7, (lte)f7, (lte)d8, (lte)f8, (lte)e7
};
static const lte king_lookup_f1[] =
{
    (lte)5, (lte)e2, (lte)g2, (lte)e1, (lte)g1, (lte)f2
};
static const lte king_lookup_f2[] =
{
    (lte)8, (lte)e1, (lte)e3, (lte)g1, (lte)g3, (lte)e2, (lte)g2, (lte)f1, (lte)f3
};
static const lte king_lookup_f3[] =
{
    (lte)8, (lte)e2, (lte)e4, (lte)g2, (lte)g4, (lte)e3, (lte)g3, (lte)f2, (lte)f4
};
static const lte king_lookup_f4[] =
{
    (lte)8, (lte)e3, (lte)e5, (lte)g3, (lte)g5, (lte)e4, (lte)g4, (lte)f3, (lte)f5
};
static const lte king_lookup_f5[] =
{
    (lte)8, (lte)e4, (lte)e6, (lte)g4, (lte)g6, (lte)e5, (lte)g5, (lte)f4, (lte)f6
};
static const lte king_lookup_f6[] =
{
    (lte)8, (lte)e5, (lte)e7, (lte)g5, (lte)g7, (lte)e6, (lte)g6, (lte)f5, (lte)f7
};
static const lte king_lookup_f7[] =
{
    (lte)8, (lte)e6, (lte)e8, (lte)g6, (lte)g8, (lte)e7, (lte)g7, (lte)f6, (lte)f8
};
static const lte king_lookup_f8[] =
{
    (lte)5, (lte)e7, (lte)g7, (lte)e8, (lte)g8, (lte)f7
};
static const lte king_lookup_g1[] =
{
    (lte)5, (lte)f2, (lte)h2, (lte)f1, (lte)h1, (lte)g2
};
static const lte king_lookup_g2[] =
{
    (lte)8, (lte)f1, (lte)f3, (lte)h1, (lte)h3, (lte)f2, (lte)h2, (lte)g1, (lte)g3
};
static const lte king_lookup_g3[] =
{
    (lte)8, (lte)f2, (lte)f4, (lte)h2, (lte)h4, (lte)f3, (lte)h3, (lte)g2, (lte)g4
};
static const lte king_lookup_g4[] =
{
    (lte)8, (lte)f3, (lte)f5, (lte)h3, (lte)h5, (lte)f4, (lte)h4, (lte)g3, (lte)g5
};
static const lte king_lookup_g5[] =
{
    (lte)8, (lte)f4, (lte)f6, (lte)h4, (lte)h6, (lte)f5, (lte)h5, (lte)g4, (lte)g6
};
static const lte king_lookup_g6[] =
{
    (lte)8, (lte)f5, (lte)f7, (lte)h5, (lte)h7, (lte)f6, (lte)h6, (lte)g5, (lte)g7
};
static const lte king_lookup_g7[] =
{
    (lte)8, (lte)f6, (lte)f8, (lte)h6, (lte)h8, (lte)f7, (lte)h7, (lte)g6, (lte)g8
};
static const lte king_lookup_g8[] =
{
    (lte)5, (lte)f7, (lte)h7, (lte)f8, (lte)h8, (lte)g7
};
static const lte king_lookup_h1[] =
{
    (lte)3, (lte)g2, (lte)g1, (lte)h2
};
static const lte king_lookup_h2[] =
{
    (lte)5, (lte)g1, (lte)g3, (lte)g2, (lte)h1, (lte)h3
};
static const lte king_lookup_h3[] =
{
    (lte)5, (lte)g2, (lte)g4, (lte)g3, (lte)h2, (lte)h4
};
static const lte king_lookup_h4[] =
{
    (lte)5, (lte)g3, (lte)g5, (lte)g4, (lte)h3, (lte)h5
};
static const lte king_lookup_h5[] =
{
    (lte)5, (lte)g4, (lte)g6, (lte)g5, (lte)h4, (lte)h6
};
static const lte king_lookup_h6[] =
{
    (lte)5, (lte)g5, (lte)g7, (lte)g6, (lte)h5, (lte)h7
};
static const lte king_lookup_h7[] =
{
    (lte)5, (lte)g6, (lte)g8, (lte)g7, (lte)h6, (lte)h8
};
static const lte king_lookup_h8[] =
{
    (lte)3, (lte)g7, (lte)g8, (lte)h7
};

// king_lookup
const lte *king_lookup[] =
{
    king_lookup_a8,
    king_lookup_b8,
    king_lookup_c8,
    king_lookup_d8,
    king_lookup_e8,
    king_lookup_f8,
    king_lookup_g8,
    king_lookup_h8,
    king_lookup_a7,
    king_lookup_b7,
    king_lookup_c7,
    king_lookup_d7,
    king_lookup_e7,
    king_lookup_f7,
    king_lookup_g7,
    king_lookup_h7,
    king_lookup_a6,
    king_lookup_b6,
    king_lookup_c6,
    king_lookup_d6,
    king_lookup_e6,
    king_lookup_f6,
    king_lookup_g6,
    king_lookup_h6,
    king_lookup_a5,
    king_lookup_b5,
    king_lookup_c5,
    king_lookup_d5,
    king_lookup_e5,
    king_lookup_f5,
    king_lookup_g5,
    king_lookup_h5,
    king_lookup_a4,
    king_lookup_b4,
    king_lookup_c4,
    king_lookup_d4,
    king_lookup_e4,
    king_lookup_f4,
    king_lookup_g4,
    king_lookup_h4,
    king_lookup_a3,
    king_lookup_b3,
    king_lookup_c3,
    king_lookup_d3,
    king_lookup_e3,
    king_lookup_f3,
    king_lookup_g3,
    king_lookup_h3,
    king_lookup_a2,
    king_lookup_b2,
    king_lookup_c2,
    king_lookup_d2,
    king_lookup_e2,
    king_lookup_f2,
    king_lookup_g2,
    king_lookup_h2,
    king_lookup_a1,
    king_lookup_b1,
    king_lookup_c1,
    king_lookup_d1,
    king_lookup_e1,
    king_lookup_f1,
    king_lookup_g1,
    king_lookup_h1
};

// White pawn, capture ray followed by advance ray
static const lte pawn_white_lookup_a1[] =
{
    (lte)1, (lte)b2,
    (lte)1, (lte)a2
};
static const lte pawn_white_lookup_a2[] =
{
    (lte)1, (lte)b3,
    (lte)2, (lte)a3, (lte)a4
};
static const lte pawn_white_lookup_a3[] =
{
    (lte)1, (lte)b4,
    (lte)1, (lte)a4
};
static const lte pawn_white_lookup_a4[] =
{
    (lte)1, (lte)b5,
    (lte)1, (lte)a5
};
static const lte pawn_white_lookup_a5[] =
{
    (lte)1, (lte)b6,
    (lte)1, (lte)a6
};
static const lte pawn_white_lookup_a6[] =
{
    (lte)1, (lte)b7,
    (lte)1, (lte)a7
};
static const lte pawn_white_lookup_a7[] =
{
    (lte)1, (lte)b8,
    (lte)1, (lte)a8
};
static const lte pawn_white_lookup_a8[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_white_lookup_b1[] =
{
    (lte)2, (lte)a2, (lte)c2,
    (lte)1, (lte)b2
};
static const lte pawn_white_lookup_b2[] =
{
    (lte)2, (lte)a3, (lte)c3,
    (lte)2, (lte)b3, (lte)b4
};
static const lte pawn_white_lookup_b3[] =
{
    (lte)2, (lte)a4, (lte)c4,
    (lte)1, (lte)b4
};
static const lte pawn_white_lookup_b4[] =
{
    (lte)2, (lte)a5, (lte)c5,
    (lte)1, (lte)b5
};
static const lte pawn_white_lookup_b5[] =
{
    (lte)2, (lte)a6, (lte)c6,
    (lte)1, (lte)b6
};
static const lte pawn_white_lookup_b6[] =
{
    (lte)2, (lte)a7, (lte)c7,
    (lte)1, (lte)b7
};
static const lte pawn_white_lookup_b7[] =
{
    (lte)2, (lte)a8, (lte)c8,
    (lte)1, (lte)b8
};
static const lte pawn_white_lookup_b8[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_white_lookup_c1[] =
{
    (lte)2, (lte)b2, (lte)d2,
    (lte)1, (lte)c2
};
static const lte pawn_white_lookup_c2[] =
{
    (lte)2, (lte)b3, (lte)d3,
    (lte)2, (lte)c3, (lte)c4
};
static const lte pawn_white_lookup_c3[] =
{
    (lte)2, (lte)b4, (lte)d4,
    (lte)1, (lte)c4
};
static const lte pawn_white_lookup_c4[] =
{
    (lte)2, (lte)b5, (lte)d5,
    (lte)1, (lte)c5
};
static const lte pawn_white_lookup_c5[] =
{
    (lte)2, (lte)b6, (lte)d6,
    (lte)1, (lte)c6
};
static const lte pawn_white_lookup_c6[] =
{
    (lte)2, (lte)b7, (lte)d7,
    (lte)1, (lte)c7
};
static const lte pawn_white_lookup_c7[] =
{
    (lte)2, (lte)b8, (lte)d8,
    (lte)1, (lte)c8
};
static const lte pawn_white_lookup_c8[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_white_lookup_d1[] =
{
    (lte)2, (lte)c2, (lte)e2,
    (lte)1, (lte)d2
};
static const lte pawn_white_lookup_d2[] =
{
    (lte)2, (lte)c3, (lte)e3,
    (lte)2, (lte)d3, (lte)d4
};
static const lte pawn_white_lookup_d3[] =
{
    (lte)2, (lte)c4, (lte)e4,
    (lte)1, (lte)d4
};
static const lte pawn_white_lookup_d4[] =
{
    (lte)2, (lte)c5, (lte)e5,
    (lte)1, (lte)d5
};
static const lte pawn_white_lookup_d5[] =
{
    (lte)2, (lte)c6, (lte)e6,
    (lte)1, (lte)d6
};
static const lte pawn_white_lookup_d6[] =
{
    (lte)2, (lte)c7, (lte)e7,
    (lte)1, (lte)d7
};
static const lte pawn_white_lookup_d7[] =
{
    (lte)2, (lte)c8, (lte)e8,
    (lte)1, (lte)d8
};
static const lte pawn_white_lookup_d8[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_white_lookup_e1[] =
{
    (lte)2, (lte)d2, (lte)f2,
    (lte)1, (lte)e2
};
static const lte pawn_white_lookup_e2[] =
{
    (lte)2, (lte)d3, (lte)f3,
    (lte)2, (lte)e3, (lte)e4
};
static const lte pawn_white_lookup_e3[] =
{
    (lte)2, (lte)d4, (lte)f4,
    (lte)1, (lte)e4
};
static const lte pawn_white_lookup_e4[] =
{
    (lte)2, (lte)d5, (lte)f5,
    (lte)1, (lte)e5
};
static const lte pawn_white_lookup_e5[] =
{
    (lte)2, (lte)d6, (lte)f6,
    (lte)1, (lte)e6
};
static const lte pawn_white_lookup_e6[] =
{
    (lte)2, (lte)d7, (lte)f7,
    (lte)1, (lte)e7
};
static const lte pawn_white_lookup_e7[] =
{
    (lte)2, (lte)d8, (lte)f8,
    (lte)1, (lte)e8
};
static const lte pawn_white_lookup_e8[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_white_lookup_f1[] =
{
    (lte)2, (lte)e2, (lte)g2,
    (lte)1, (lte)f2
};
static const lte pawn_white_lookup_f2[] =
{
    (lte)2, (lte)e3, (lte)g3,
    (lte)2, (lte)f3, (lte)f4
};
static const lte pawn_white_lookup_f3[] =
{
    (lte)2, (lte)e4, (lte)g4,
    (lte)1, (lte)f4
};
static const lte pawn_white_lookup_f4[] =
{
    (lte)2, (lte)e5, (lte)g5,
    (lte)1, (lte)f5
};
static const lte pawn_white_lookup_f5[] =
{
    (lte)2, (lte)e6, (lte)g6,
    (lte)1, (lte)f6
};
static const lte pawn_white_lookup_f6[] =
{
    (lte)2, (lte)e7, (lte)g7,
    (lte)1, (lte)f7
};
static const lte pawn_white_lookup_f7[] =
{
    (lte)2, (lte)e8, (lte)g8,
    (lte)1, (lte)f8
};
static const lte pawn_white_lookup_f8[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_white_lookup_g1[] =
{
    (lte)2, (lte)f2, (lte)h2,
    (lte)1, (lte)g2
};
static const lte pawn_white_lookup_g2[] =
{
    (lte)2, (lte)f3, (lte)h3,
    (lte)2, (lte)g3, (lte)g4
};
static const lte pawn_white_lookup_g3[] =
{
    (lte)2, (lte)f4, (lte)h4,
    (lte)1, (lte)g4
};
static const lte pawn_white_lookup_g4[] =
{
    (lte)2, (lte)f5, (lte)h5,
    (lte)1, (lte)g5
};
static const lte pawn_white_lookup_g5[] =
{
    (lte)2, (lte)f6, (lte)h6,
    (lte)1, (lte)g6
};
static const lte pawn_white_lookup_g6[] =
{
    (lte)2, (lte)f7, (lte)h7,
    (lte)1, (lte)g7
};
static const lte pawn_white_lookup_g7[] =
{
    (lte)2, (lte)f8, (lte)h8,
    (lte)1, (lte)g8
};
static const lte pawn_white_lookup_g8[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_white_lookup_h1[] =
{
    (lte)1, (lte)g2,
    (lte)1, (lte)h2
};
static const lte pawn_white_lookup_h2[] =
{
    (lte)1, (lte)g3,
    (lte)2, (lte)h3, (lte)h4
};
static const lte pawn_white_lookup_h3[] =
{
    (lte)1, (lte)g4,
    (lte)1, (lte)h4
};
static const lte pawn_white_lookup_h4[] =
{
    (lte)1, (lte)g5,
    (lte)1, (lte)h5
};
static const lte pawn_white_lookup_h5[] =
{
    (lte)1, (lte)g6,
    (lte)1, (lte)h6
};
static const lte pawn_white_lookup_h6[] =
{
    (lte)1, (lte)g7,
    (lte)1, (lte)h7
};
static const lte pawn_white_lookup_h7[] =
{
    (lte)1, (lte)g8,
    (lte)1, (lte)h8
};
static const lte pawn_white_lookup_h8[] =
{
    (lte)0,
    (lte)0
};

// pawn_white_lookup
const lte *pawn_white_lookup[] =
{
    pawn_white_lookup_a8,
    pawn_white_lookup_b8,
    pawn_white_lookup_c8,
    pawn_white_lookup_d8,
    pawn_white_lookup_e8,
    pawn_white_lookup_f8,
    pawn_white_lookup_g8,
    pawn_white_lookup_h8,
    pawn_white_lookup_a7,
    pawn_white_lookup_b7,
    pawn_white_lookup_c7,
    pawn_white_lookup_d7,
    pawn_white_lookup_e7,
    pawn_white_lookup_f7,
    pawn_white_lookup_g7,
    pawn_white_lookup_h7,
    pawn_white_lookup_a6,
    pawn_white_lookup_b6,
    pawn_white_lookup_c6,
    pawn_white_lookup_d6,
    pawn_white_lookup_e6,
    pawn_white_lookup_f6,
    pawn_white_lookup_g6,
    pawn_white_lookup_h6,
    pawn_white_lookup_a5,
    pawn_white_lookup_b5,
    pawn_white_lookup_c5,
    pawn_white_lookup_d5,
    pawn_white_lookup_e5,
    pawn_white_lookup_f5,
    pawn_white_lookup_g5,
    pawn_white_lookup_h5,
    pawn_white_lookup_a4,
    pawn_white_lookup_b4,
    pawn_white_lookup_c4,
    pawn_white_lookup_d4,
    pawn_white_lookup_e4,
    pawn_white_lookup_f4,
    pawn_white_lookup_g4,
    pawn_white_lookup_h4,
    pawn_white_lookup_a3,
    pawn_white_lookup_b3,
    pawn_white_lookup_c3,
    pawn_white_lookup_d3,
    pawn_white_lookup_e3,
    pawn_white_lookup_f3,
    pawn_white_lookup_g3,
    pawn_white_lookup_h3,
    pawn_white_lookup_a2,
    pawn_white_lookup_b2,
    pawn_white_lookup_c2,
    pawn_white_lookup_d2,
    pawn_white_lookup_e2,
    pawn_white_lookup_f2,
    pawn_white_lookup_g2,
    pawn_white_lookup_h2,
    pawn_white_lookup_a1,
    pawn_white_lookup_b1,
    pawn_white_lookup_c1,
    pawn_white_lookup_d1,
    pawn_white_lookup_e1,
    pawn_white_lookup_f1,
    pawn_white_lookup_g1,
    pawn_white_lookup_h1
};

// Black pawn, capture ray followed by advance ray
static const lte pawn_black_lookup_a1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_a2[] =
{
    (lte)1, (lte)b1,
    (lte)1, (lte)a1
};
static const lte pawn_black_lookup_a3[] =
{
    (lte)1, (lte)b2,
    (lte)1, (lte)a2
};
static const lte pawn_black_lookup_a4[] =
{
    (lte)1, (lte)b3,
    (lte)1, (lte)a3
};
static const lte pawn_black_lookup_a5[] =
{
    (lte)1, (lte)b4,
    (lte)1, (lte)a4
};
static const lte pawn_black_lookup_a6[] =
{
    (lte)1, (lte)b5,
    (lte)1, (lte)a5
};
static const lte pawn_black_lookup_a7[] =
{
    (lte)1, (lte)b6,
    (lte)2, (lte)a6, (lte)a5
};
static const lte pawn_black_lookup_a8[] =
{
    (lte)1, (lte)b7,
    (lte)1, (lte)a7
};
static const lte pawn_black_lookup_b1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_b2[] =
{
    (lte)2, (lte)a1, (lte)c1,
    (lte)1, (lte)b1
};
static const lte pawn_black_lookup_b3[] =
{
    (lte)2, (lte)a2, (lte)c2,
    (lte)1, (lte)b2
};
static const lte pawn_black_lookup_b4[] =
{
    (lte)2, (lte)a3, (lte)c3,
    (lte)1, (lte)b3
};
static const lte pawn_black_lookup_b5[] =
{
    (lte)2, (lte)a4, (lte)c4,
    (lte)1, (lte)b4
};
static const lte pawn_black_lookup_b6[] =
{
    (lte)2, (lte)a5, (lte)c5,
    (lte)1, (lte)b5
};
static const lte pawn_black_lookup_b7[] =
{
    (lte)2, (lte)a6, (lte)c6,
    (lte)2, (lte)b6, (lte)b5
};
static const lte pawn_black_lookup_b8[] =
{
    (lte)2, (lte)a7, (lte)c7,
    (lte)1, (lte)b7
};
static const lte pawn_black_lookup_c1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_c2[] =
{
    (lte)2, (lte)b1, (lte)d1,
    (lte)1, (lte)c1
};
static const lte pawn_black_lookup_c3[] =
{
    (lte)2, (lte)b2, (lte)d2,
    (lte)1, (lte)c2
};
static const lte pawn_black_lookup_c4[] =
{
    (lte)2, (lte)b3, (lte)d3,
    (lte)1, (lte)c3
};
static const lte pawn_black_lookup_c5[] =
{
    (lte)2, (lte)b4, (lte)d4,
    (lte)1, (lte)c4
};
static const lte pawn_black_lookup_c6[] =
{
    (lte)2, (lte)b5, (lte)d5,
    (lte)1, (lte)c5
};
static const lte pawn_black_lookup_c7[] =
{
    (lte)2, (lte)b6, (lte)d6,
    (lte)2, (lte)c6, (lte)c5
};
static const lte pawn_black_lookup_c8[] =
{
    (lte)2, (lte)b7, (lte)d7,
    (lte)1, (lte)c7
};
static const lte pawn_black_lookup_d1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_d2[] =
{
    (lte)2, (lte)c1, (lte)e1,
    (lte)1, (lte)d1
};
static const lte pawn_black_lookup_d3[] =
{
    (lte)2, (lte)c2, (lte)e2,
    (lte)1, (lte)d2
};
static const lte pawn_black_lookup_d4[] =
{
    (lte)2, (lte)c3, (lte)e3,
    (lte)1, (lte)d3
};
static const lte pawn_black_lookup_d5[] =
{
    (lte)2, (lte)c4, (lte)e4,
    (lte)1, (lte)d4
};
static const lte pawn_black_lookup_d6[] =
{
    (lte)2, (lte)c5, (lte)e5,
    (lte)1, (lte)d5
};
static const lte pawn_black_lookup_d7[] =
{
    (lte)2, (lte)c6, (lte)e6,
    (lte)2, (lte)d6, (lte)d5
};
static const lte pawn_black_lookup_d8[] =
{
    (lte)2, (lte)c7, (lte)e7,
    (lte)1, (lte)d7
};
static const lte pawn_black_lookup_e1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_e2[] =
{
    (lte)2, (lte)d1, (lte)f1,
    (lte)1, (lte)e1
};
static const lte pawn_black_lookup_e3[] =
{
    (lte)2, (lte)d2, (lte)f2,
    (lte)1, (lte)e2
};
static const lte pawn_black_lookup_e4[] =
{
    (lte)2, (lte)d3, (lte)f3,
    (lte)1, (lte)e3
};
static const lte pawn_black_lookup_e5[] =
{
    (lte)2, (lte)d4, (lte)f4,
    (lte)1, (lte)e4
};
static const lte pawn_black_lookup_e6[] =
{
    (lte)2, (lte)d5, (lte)f5,
    (lte)1, (lte)e5
};
static const lte pawn_black_lookup_e7[] =
{
    (lte)2, (lte)d6, (lte)f6,
    (lte)2, (lte)e6, (lte)e5
};
static const lte pawn_black_lookup_e8[] =
{
    (lte)2, (lte)d7, (lte)f7,
    (lte)1, (lte)e7
};
static const lte pawn_black_lookup_f1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_f2[] =
{
    (lte)2, (lte)e1, (lte)g1,
    (lte)1, (lte)f1
};
static const lte pawn_black_lookup_f3[] =
{
    (lte)2, (lte)e2, (lte)g2,
    (lte)1, (lte)f2
};
static const lte pawn_black_lookup_f4[] =
{
    (lte)2, (lte)e3, (lte)g3,
    (lte)1, (lte)f3
};
static const lte pawn_black_lookup_f5[] =
{
    (lte)2, (lte)e4, (lte)g4,
    (lte)1, (lte)f4
};
static const lte pawn_black_lookup_f6[] =
{
    (lte)2, (lte)e5, (lte)g5,
    (lte)1, (lte)f5
};
static const lte pawn_black_lookup_f7[] =
{
    (lte)2, (lte)e6, (lte)g6,
    (lte)2, (lte)f6, (lte)f5
};
static const lte pawn_black_lookup_f8[] =
{
    (lte)2, (lte)e7, (lte)g7,
    (lte)1, (lte)f7
};
static const lte pawn_black_lookup_g1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_g2[] =
{
    (lte)2, (lte)f1, (lte)h1,
    (lte)1, (lte)g1
};
static const lte pawn_black_lookup_g3[] =
{
    (lte)2, (lte)f2, (lte)h2,
    (lte)1, (lte)g2
};
static const lte pawn_black_lookup_g4[] =
{
    (lte)2, (lte)f3, (lte)h3,
    (lte)1, (lte)g3
};
static const lte pawn_black_lookup_g5[] =
{
    (lte)2, (lte)f4, (lte)h4,
    (lte)1, (lte)g4
};
static const lte pawn_black_lookup_g6[] =
{
    (lte)2, (lte)f5, (lte)h5,
    (lte)1, (lte)g5
};
static const lte pawn_black_lookup_g7[] =
{
    (lte)2, (lte)f6, (lte)h6,
    (lte)2, (lte)g6, (lte)g5
};
static const lte pawn_black_lookup_g8[] =
{
    (lte)2, (lte)f7, (lte)h7,
    (lte)1, (lte)g7
};
static const lte pawn_black_lookup_h1[] =
{
    (lte)0,
    (lte)0
};
static const lte pawn_black_lookup_h2[] =
{
    (lte)1, (lte)g1,
    (lte)1, (lte)h1
};
static const lte pawn_black_lookup_h3[] =
{
    (lte)1, (lte)g2,
    (lte)1, (lte)h2
};
static const lte pawn_black_lookup_h4[] =
{
    (lte)1, (lte)g3,
    (lte)1, (lte)h3
};
static const lte pawn_black_lookup_h5[] =
{
    (lte)1, (lte)g4,
    (lte)1, (lte)h4
};
static const lte pawn_black_lookup_h6[] =
{
    (lte)1, (lte)g5,
    (lte)1, (lte)h5
};
static const lte pawn_black_lookup_h7[] =
{
    (lte)1, (lte)g6,
    (lte)2, (lte)h6, (lte)h5
};
static const lte pawn_black_lookup_h8[] =
{
    (lte)1, (lte)g7,
    (lte)1, (lte)h7
};

// pawn_black_lookup
const lte *pawn_black_lookup[] =
{
    pawn_black_lookup_a8,
    pawn_black_lookup_b8,
    pawn_black_lookup_c8,
    pawn_black_lookup_d8,
    pawn_black_lookup_e8,
    pawn_black_lookup_f8,
    pawn_black_lookup_g8,
    pawn_black_lookup_h8,
    pawn_black_lookup_a7,
    pawn_black_lookup_b7,
    pawn_black_lookup_c7,
    pawn_black_lookup_d7,
    pawn_black_lookup_e7,
    pawn_black_lookup_f7,
    pawn_black_lookup_g7,
    pawn_black_lookup_h7,
    pawn_black_lookup_a6,
    pawn_black_lookup_b6,
    pawn_black_lookup_c6,
    pawn_black_lookup_d6,
    pawn_black_lookup_e6,
    pawn_black_lookup_f6,
    pawn_black_lookup_g6,
    pawn_black_lookup_h6,
    pawn_black_lookup_a5,
    pawn_black_lookup_b5,
    pawn_black_lookup_c5,
    pawn_black_lookup_d5,
    pawn_black_lookup_e5,
    pawn_black_lookup_f5,
    pawn_black_lookup_g5,
    pawn_black_lookup_h5,
    pawn_black_lookup_a4,
    pawn_black_lookup_b4,
    pawn_black_lookup_c4,
    pawn_black_lookup_d4,
    pawn_black_lookup_e4,
    pawn_black_lookup_f4,
    pawn_black_lookup_g4,
    pawn_black_lookup_h4,
    pawn_black_lookup_a3,
    pawn_black_lookup_b3,
    pawn_black_lookup_c3,
    pawn_black_lookup_d3,
    pawn_black_lookup_e3,
    pawn_black_lookup_f3,
    pawn_black_lookup_g3,
    pawn_black_lookup_h3,
    pawn_black_lookup_a2,
    pawn_black_lookup_b2,
    pawn_black_lookup_c2,
    pawn_black_lookup_d2,
    pawn_black_lookup_e2,
    pawn_black_lookup_f2,
    pawn_black_lookup_g2,
    pawn_black_lookup_h2,
    pawn_black_lookup_a1,
    pawn_black_lookup_b1,
    pawn_black_lookup_c1,
    pawn_black_lookup_d1,
    pawn_black_lookup_e1,
    pawn_black_lookup_f1,
    pawn_black_lookup_g1,
    pawn_black_lookup_h1
};

// Good king positions in the ending are often a knight move (2-1) or
//  a (2-0) move towards the centre
static const lte good_king_position_lookup_a1[] =
{
    (lte)4, (lte)c1, (lte)c2, (lte)a3, (lte)b3
};
static const lte good_king_position_lookup_a2[] =
{
    (lte)5, (lte)c1, (lte)c2, (lte)c3, (lte)a4, (lte)b4
};
static const lte good_king_position_lookup_a3[] =
{
    (lte)7, (lte)c2, (lte)c3, (lte)c4, (lte)a1, (lte)b1, (lte)a5, (lte)b5
};
static const lte good_king_position_lookup_a4[] =
{
    (lte)7, (lte)c3, (lte)c4, (lte)c5, (lte)a2, (lte)b2, (lte)a6, (lte)b6
};
static const lte good_king_position_lookup_a5[] =
{
    (lte)7, (lte)c4, (lte)c5, (lte)c6, (lte)a3, (lte)b3, (lte)a7, (lte)b7
};
static const lte good_king_position_lookup_a6[] =
{
    (lte)7, (lte)c5, (lte)c6, (lte)c7, (lte)a4, (lte)b4, (lte)a8, (lte)b8
};
static const lte good_king_position_lookup_a7[] =
{
    (lte)5, (lte)c6, (lte)c7, (lte)c8, (lte)a5, (lte)b5
};
static const lte good_king_position_lookup_a8[] =
{
    (lte)4, (lte)c7, (lte)c8, (lte)a6, (lte)b6
};
static const lte good_king_position_lookup_b1[] =
{
    (lte)5, (lte)d1, (lte)d2, (lte)a3, (lte)b3, (lte)c3
};
static const lte good_king_position_lookup_b2[] =
{
    (lte)6, (lte)d1, (lte)d2, (lte)d3, (lte)a4, (lte)b4, (lte)c4
};
static const lte good_king_position_lookup_b3[] =
{
    (lte)9, (lte)d2, (lte)d3, (lte)d4, (lte)a1, (lte)b1, (lte)c1, (lte)a5, (lte)b5, (lte)c5
};
static const lte good_king_position_lookup_b4[] =
{
    (lte)9, (lte)d3, (lte)d4, (lte)d5, (lte)a2, (lte)b2, (lte)c2, (lte)a6, (lte)b6, (lte)c6
};
static const lte good_king_position_lookup_b5[] =
{
    (lte)9, (lte)d4, (lte)d5, (lte)d6, (lte)a3, (lte)b3, (lte)c3, (lte)a7, (lte)b7, (lte)c7
};
static const lte good_king_position_lookup_b6[] =
{
    (lte)9, (lte)d5, (lte)d6, (lte)d7, (lte)a4, (lte)b4, (lte)c4, (lte)a8, (lte)b8, (lte)c8
};
static const lte good_king_position_lookup_b7[] =
{
    (lte)6, (lte)d6, (lte)d7, (lte)d8, (lte)a5, (lte)b5, (lte)c5
};
static const lte good_king_position_lookup_b8[] =
{
    (lte)5, (lte)d7, (lte)d8, (lte)a6, (lte)b6, (lte)c6
};
static const lte good_king_position_lookup_c1[] =
{
    (lte)7, (lte)a1, (lte)a2, (lte)e1, (lte)e2, (lte)b3, (lte)c3, (lte)d3
};
static const lte good_king_position_lookup_c2[] =
{
    (lte)9, (lte)a1, (lte)a2, (lte)a3, (lte)e1, (lte)e2, (lte)e3, (lte)b4, (lte)c4, (lte)d4
};
static const lte good_king_position_lookup_c3[] =
{
    (lte)12, (lte)a2, (lte)a3, (lte)a4, (lte)e2, (lte)e3, (lte)e4, (lte)b1, (lte)c1, (lte)d1, (lte)b5, (lte)c5, (lte)d5
};
static const lte good_king_position_lookup_c4[] =
{
    (lte)12, (lte)a3, (lte)a4, (lte)a5, (lte)e3, (lte)e4, (lte)e5, (lte)b2, (lte)c2, (lte)d2, (lte)b6, (lte)c6, (lte)d6
};
static const lte good_king_position_lookup_c5[] =
{
    (lte)12, (lte)a4, (lte)a5, (lte)a6, (lte)e4, (lte)e5, (lte)e6, (lte)b3, (lte)c3, (lte)d3, (lte)b7, (lte)c7, (lte)d7
};
static const lte good_king_position_lookup_c6[] =
{
    (lte)12, (lte)a5, (lte)a6, (lte)a7, (lte)e5, (lte)e6, (lte)e7, (lte)b4, (lte)c4, (lte)d4, (lte)b8, (lte)c8, (lte)d8
};
static const lte good_king_position_lookup_c7[] =
{
    (lte)9, (lte)a6, (lte)a7, (lte)a8, (lte)e6, (lte)e7, (lte)e8, (lte)b5, (lte)c5, (lte)d5
};
static const lte good_king_position_lookup_c8[] =
{
    (lte)7, (lte)a7, (lte)a8, (lte)e7, (lte)e8, (lte)b6, (lte)c6, (lte)d6
};
static const lte good_king_position_lookup_d1[] =
{
    (lte)7, (lte)b1, (lte)b2, (lte)f1, (lte)f2, (lte)c3, (lte)d3, (lte)e3
};
static const lte good_king_position_lookup_d2[] =
{
    (lte)9, (lte)b1, (lte)b2, (lte)b3, (lte)f1, (lte)f2, (lte)f3, (lte)c4, (lte)d4, (lte)e4
};
static const lte good_king_position_lookup_d3[] =
{
    (lte)12, (lte)b2, (lte)b3, (lte)b4, (lte)f2, (lte)f3, (lte)f4, (lte)c1, (lte)d1, (lte)e1, (lte)c5, (lte)d5, (lte)e5
};
static const lte good_king_position_lookup_d4[] =
{
    (lte)12, (lte)b3, (lte)b4, (lte)b5, (lte)f3, (lte)f4, (lte)f5, (lte)c2, (lte)d2, (lte)e2, (lte)c6, (lte)d6, (lte)e6
};
static const lte good_king_position_lookup_d5[] =
{
    (lte)12, (lte)b4, (lte)b5, (lte)b6, (lte)f4, (lte)f5, (lte)f6, (lte)c3, (lte)d3, (lte)e3, (lte)c7, (lte)d7, (lte)e7
};
static const lte good_king_position_lookup_d6[] =
{
    (lte)12, (lte)b5, (lte)b6, (lte)b7, (lte)f5, (lte)f6, (lte)f7, (lte)c4, (lte)d4, (lte)e4, (lte)c8, (lte)d8, (lte)e8
};
static const lte good_king_position_lookup_d7[] =
{
    (lte)9, (lte)b6, (lte)b7, (lte)b8, (lte)f6, (lte)f7, (lte)f8, (lte)c5, (lte)d5, (lte)e5
};
static const lte good_king_position_lookup_d8[] =
{
    (lte)7, (lte)b7, (lte)b8, (lte)f7, (lte)f8, (lte)c6, (lte)d6, (lte)e6
};
static const lte good_king_position_lookup_e1[] =
{
    (lte)7, (lte)c1, (lte)c2, (lte)g1, (lte)g2, (lte)d3, (lte)e3, (lte)f3
};
static const lte good_king_position_lookup_e2[] =
{
    (lte)9, (lte)c1, (lte)c2, (lte)c3, (lte)g1, (lte)g2, (lte)g3, (lte)d4, (lte)e4, (lte)f4
};
static const lte good_king_position_lookup_e3[] =
{
    (lte)12, (lte)c2, (lte)c3, (lte)c4, (lte)g2, (lte)g3, (lte)g4, (lte)d1, (lte)e1, (lte)f1, (lte)d5, (lte)e5, (lte)f5
};
static const lte good_king_position_lookup_e4[] =
{
    (lte)12, (lte)c3, (lte)c4, (lte)c5, (lte)g3, (lte)g4, (lte)g5, (lte)d2, (lte)e2, (lte)f2, (lte)d6, (lte)e6, (lte)f6
};
static const lte good_king_position_lookup_e5[] =
{
    (lte)12, (lte)c4, (lte)c5, (lte)c6, (lte)g4, (lte)g5, (lte)g6, (lte)d3, (lte)e3, (lte)f3, (lte)d7, (lte)e7, (lte)f7
};
static const lte good_king_position_lookup_e6[] =
{
    (lte)12, (lte)c5, (lte)c6, (lte)c7, (lte)g5, (lte)g6, (lte)g7, (lte)d4, (lte)e4, (lte)f4, (lte)d8, (lte)e8, (lte)f8
};
static const lte good_king_position_lookup_e7[] =
{
    (lte)9, (lte)c6, (lte)c7, (lte)c8, (lte)g6, (lte)g7, (lte)g8, (lte)d5, (lte)e5, (lte)f5
};
static const lte good_king_position_lookup_e8[] =
{
    (lte)7, (lte)c7, (lte)c8, (lte)g7, (lte)g8, (lte)d6, (lte)e6, (lte)f6
};
static const lte good_king_position_lookup_f1[] =
{
    (lte)7, (lte)d1, (lte)d2, (lte)h1, (lte)h2, (lte)e3, (lte)f3, (lte)g3
};
static const lte good_king_position_lookup_f2[] =
{
    (lte)9, (lte)d1, (lte)d2, (lte)d3, (lte)h1, (lte)h2, (lte)h3, (lte)e4, (lte)f4, (lte)g4
};
static const lte good_king_position_lookup_f3[] =
{
    (lte)12, (lte)d2, (lte)d3, (lte)d4, (lte)h2, (lte)h3, (lte)h4, (lte)e1, (lte)f1, (lte)g1, (lte)e5, (lte)f5, (lte)g5
};
static const lte good_king_position_lookup_f4[] =
{
    (lte)12, (lte)d3, (lte)d4, (lte)d5, (lte)h3, (lte)h4, (lte)h5, (lte)e2, (lte)f2, (lte)g2, (lte)e6, (lte)f6, (lte)g6
};
static const lte good_king_position_lookup_f5[] =
{
    (lte)12, (lte)d4, (lte)d5, (lte)d6, (lte)h4, (lte)h5, (lte)h6, (lte)e3, (lte)f3, (lte)g3, (lte)e7, (lte)f7, (lte)g7
};
static const lte good_king_position_lookup_f6[] =
{
    (lte)12, (lte)d5, (lte)d6, (lte)d7, (lte)h5, (lte)h6, (lte)h7, (lte)e4, (lte)f4, (lte)g4, (lte)e8, (lte)f8, (lte)g8
};
static const lte good_king_position_lookup_f7[] =
{
    (lte)9, (lte)d6, (lte)d7, (lte)d8, (lte)h6, (lte)h7, (lte)h8, (lte)e5, (lte)f5, (lte)g5
};
static const lte good_king_position_lookup_f8[] =
{
    (lte)7, (lte)d7, (lte)d8, (lte)h7, (lte)h8, (lte)e6, (lte)f6, (lte)g6
};
static const lte good_king_position_lookup_g1[] =
{
    (lte)5, (lte)e1, (lte)e2, (lte)f3, (lte)g3, (lte)h3
};
static const lte good_king_position_lookup_g2[] =
{
    (lte)6, (lte)e1, (lte)e2, (lte)e3, (lte)f4, (lte)g4, (lte)h4
};
static const lte good_king_position_lookup_g3[] =
{
    (lte)9, (lte)e2, (lte)e3, (lte)e4, (lte)f1, (lte)g1, (lte)h1, (lte)f5, (lte)g5, (lte)h5
};
static const lte good_king_position_lookup_g4[] =
{
    (lte)9, (lte)e3, (lte)e4, (lte)e5, (lte)f2, (lte)g2, (lte)h2, (lte)f6, (lte)g6, (lte)h6
};
static const lte good_king_position_lookup_g5[] =
{
    (lte)9, (lte)e4, (lte)e5, (lte)e6, (lte)f3, (lte)g3, (lte)h3, (lte)f7, (lte)g7, (lte)h7
};
static const lte good_king_position_lookup_g6[] =
{
    (lte)9, (lte)e5, (lte)e6, (lte)e7, (lte)f4, (lte)g4, (lte)h4, (lte)f8, (lte)g8, (lte)h8
};
static const lte good_king_position_lookup_g7[] =
{
    (lte)6, (lte)e6, (lte)e7, (lte)e8, (lte)f5, (lte)g5, (lte)h5
};
static const lte good_king_position_lookup_g8[] =
{
    (lte)5, (lte)e7, (lte)e8, (lte)f6, (lte)g6, (lte)h6
};
static const lte good_king_position_lookup_h1[] =
{
    (lte)4, (lte)f1, (lte)f2, (lte)g3, (lte)h3
};
static const lte good_king_position_lookup_h2[] =
{
    (lte)5, (lte)f1, (lte)f2, (lte)f3, (lte)g4, (lte)h4
};
static const lte good_king_position_lookup_h3[] =
{
    (lte)7, (lte)f2, (lte)f3, (lte)f4, (lte)g1, (lte)h1, (lte)g5, (lte)h5
};
static const lte good_king_position_lookup_h4[] =
{
    (lte)7, (lte)f3, (lte)f4, (lte)f5, (lte)g2, (lte)h2, (lte)g6, (lte)h6
};
static const lte good_king_position_lookup_h5[] =
{
    (lte)7, (lte)f4, (lte)f5, (lte)f6, (lte)g3, (lte)h3, (lte)g7, (lte)h7
};
static const lte good_king_position_lookup_h6[] =
{
    (lte)7, (lte)f5, (lte)f6, (lte)f7, (lte)g4, (lte)h4, (lte)g8, (lte)h8
};
static const lte good_king_position_lookup_h7[] =
{
    (lte)5, (lte)f6, (lte)f7, (lte)f8, (lte)g5, (lte)h5
};
static const lte good_king_position_lookup_h8[] =
{
    (lte)4, (lte)f7, (lte)f8, (lte)g6, (lte)h6
};

// good_king_position_lookup
const lte *good_king_position_lookup[] =
{
    good_king_position_lookup_a8,
    good_king_position_lookup_b8,
    good_king_position_lookup_c8,
    good_king_position_lookup_d8,
    good_king_position_lookup_e8,
    good_king_position_lookup_f8,
    good_king_position_lookup_g8,
    good_king_position_lookup_h8,
    good_king_position_lookup_a7,
    good_king_position_lookup_b7,
    good_king_position_lookup_c7,
    good_king_position_lookup_d7,
    good_king_position_lookup_e7,
    good_king_position_lookup_f7,
    good_king_position_lookup_g7,
    good_king_position_lookup_h7,
    good_king_position_lookup_a6,
    good_king_position_lookup_b6,
    good_king_position_lookup_c6,
    good_king_position_lookup_d6,
    good_king_position_lookup_e6,
    good_king_position_lookup_f6,
    good_king_position_lookup_g6,
    good_king_position_lookup_h6,
    good_king_position_lookup_a5,
    good_king_position_lookup_b5,
    good_king_position_lookup_c5,
    good_king_position_lookup_d5,
    good_king_position_lookup_e5,
    good_king_position_lookup_f5,
    good_king_position_lookup_g5,
    good_king_position_lookup_h5,
    good_king_position_lookup_a4,
    good_king_position_lookup_b4,
    good_king_position_lookup_c4,
    good_king_position_lookup_d4,
    good_king_position_lookup_e4,
    good_king_position_lookup_f4,
    good_king_position_lookup_g4,
    good_king_position_lookup_h4,
    good_king_position_lookup_a3,
    good_king_position_lookup_b3,
    good_king_position_lookup_c3,
    good_king_position_lookup_d3,
    good_king_position_lookup_e3,
    good_king_position_lookup_f3,
    good_king_position_lookup_g3,
    good_king_position_lookup_h3,
    good_king_position_lookup_a2,
    good_king_position_lookup_b2,
    good_king_position_lookup_c2,
    good_king_position_lookup_d2,
    good_king_position_lookup_e2,
    good_king_position_lookup_f2,
    good_king_position_lookup_g2,
    good_king_position_lookup_h2,
    good_king_position_lookup_a1,
    good_king_position_lookup_b1,
    good_king_position_lookup_c1,
    good_king_position_lookup_d1,
    good_king_position_lookup_e1,
    good_king_position_lookup_f1,
    good_king_position_lookup_g1,
    good_king_position_lookup_h1
};

// Attack from up to 2 black pawns on a white piece
static const lte pawn_attacks_white_lookup_a1[] =
{
    (lte)1, (lte)b2
};
static const lte pawn_attacks_white_lookup_a2[] =
{
    (lte)1, (lte)b3
};
static const lte pawn_attacks_white_lookup_a3[] =
{
    (lte)1, (lte)b4
};
static const lte pawn_attacks_white_lookup_a4[] =
{
    (lte)1, (lte)b5
};
static const lte pawn_attacks_white_lookup_a5[] =
{
    (lte)1, (lte)b6
};
static const lte pawn_attacks_white_lookup_a6[] =
{
    (lte)1, (lte)b7
};
static const lte pawn_attacks_white_lookup_a7[] =
{
    (lte)1, (lte)b8
};
static const lte pawn_attacks_white_lookup_a8[] =
{
    (lte)0
};
static const lte pawn_attacks_white_lookup_b1[] =
{
    (lte)2, (lte)a2, (lte)c2
};
static const lte pawn_attacks_white_lookup_b2[] =
{
    (lte)2, (lte)a3, (lte)c3
};
static const lte pawn_attacks_white_lookup_b3[] =
{
    (lte)2, (lte)a4, (lte)c4
};
static const lte pawn_attacks_white_lookup_b4[] =
{
    (lte)2, (lte)a5, (lte)c5
};
static const lte pawn_attacks_white_lookup_b5[] =
{
    (lte)2, (lte)a6, (lte)c6
};
static const lte pawn_attacks_white_lookup_b6[] =
{
    (lte)2, (lte)a7, (lte)c7
};
static const lte pawn_attacks_white_lookup_b7[] =
{
    (lte)2, (lte)a8, (lte)c8
};
static const lte pawn_attacks_white_lookup_b8[] =
{
    (lte)0
};
static const lte pawn_attacks_white_lookup_c1[] =
{
    (lte)2, (lte)b2, (lte)d2
};
static const lte pawn_attacks_white_lookup_c2[] =
{
    (lte)2, (lte)b3, (lte)d3
};
static const lte pawn_attacks_white_lookup_c3[] =
{
    (lte)2, (lte)b4, (lte)d4
};
static const lte pawn_attacks_white_lookup_c4[] =
{
    (lte)2, (lte)b5, (lte)d5
};
static const lte pawn_attacks_white_lookup_c5[] =
{
    (lte)2, (lte)b6, (lte)d6
};
static const lte pawn_attacks_white_lookup_c6[] =
{
    (lte)2, (lte)b7, (lte)d7
};
static const lte pawn_attacks_white_lookup_c7[] =
{
    (lte)2, (lte)b8, (lte)d8
};
static const lte pawn_attacks_white_lookup_c8[] =
{
    (lte)0
};
static const lte pawn_attacks_white_lookup_d1[] =
{
    (lte)2, (lte)c2, (lte)e2
};
static const lte pawn_attacks_white_lookup_d2[] =
{
    (lte)2, (lte)c3, (lte)e3
};
static const lte pawn_attacks_white_lookup_d3[] =
{
    (lte)2, (lte)c4, (lte)e4
};
static const lte pawn_attacks_white_lookup_d4[] =
{
    (lte)2, (lte)c5, (lte)e5
};
static const lte pawn_attacks_white_lookup_d5[] =
{
    (lte)2, (lte)c6, (lte)e6
};
static const lte pawn_attacks_white_lookup_d6[] =
{
    (lte)2, (lte)c7, (lte)e7
};
static const lte pawn_attacks_white_lookup_d7[] =
{
    (lte)2, (lte)c8, (lte)e8
};
static const lte pawn_attacks_white_lookup_d8[] =
{
    (lte)0
};
static const lte pawn_attacks_white_lookup_e1[] =
{
    (lte)2, (lte)d2, (lte)f2
};
static const lte pawn_attacks_white_lookup_e2[] =
{
    (lte)2, (lte)d3, (lte)f3
};
static const lte pawn_attacks_white_lookup_e3[] =
{
    (lte)2, (lte)d4, (lte)f4
};
static const lte pawn_attacks_white_lookup_e4[] =
{
    (lte)2, (lte)d5, (lte)f5
};
static const lte pawn_attacks_white_lookup_e5[] =
{
    (lte)2, (lte)d6, (lte)f6
};
static const lte pawn_attacks_white_lookup_e6[] =
{
    (lte)2, (lte)d7, (lte)f7
};
static const lte pawn_attacks_white_lookup_e7[] =
{
    (lte)2, (lte)d8, (lte)f8
};
static const lte pawn_attacks_white_lookup_e8[] =
{
    (lte)0
};
static const lte pawn_attacks_white_lookup_f1[] =
{
    (lte)2, (lte)e2, (lte)g2
};
static const lte pawn_attacks_white_lookup_f2[] =
{
    (lte)2, (lte)e3, (lte)g3
};
static const lte pawn_attacks_white_lookup_f3[] =
{
    (lte)2, (lte)e4, (lte)g4
};
static const lte pawn_attacks_white_lookup_f4[] =
{
    (lte)2, (lte)e5, (lte)g5
};
static const lte pawn_attacks_white_lookup_f5[] =
{
    (lte)2, (lte)e6, (lte)g6
};
static const lte pawn_attacks_white_lookup_f6[] =
{
    (lte)2, (lte)e7, (lte)g7
};
static const lte pawn_attacks_white_lookup_f7[] =
{
    (lte)2, (lte)e8, (lte)g8
};
static const lte pawn_attacks_white_lookup_f8[] =
{
    (lte)0
};
static const lte pawn_attacks_white_lookup_g1[] =
{
    (lte)2, (lte)f2, (lte)h2
};
static const lte pawn_attacks_white_lookup_g2[] =
{
    (lte)2, (lte)f3, (lte)h3
};
static const lte pawn_attacks_white_lookup_g3[] =
{
    (lte)2, (lte)f4, (lte)h4
};
static const lte pawn_attacks_white_lookup_g4[] =
{
    (lte)2, (lte)f5, (lte)h5
};
static const lte pawn_attacks_white_lookup_g5[] =
{
    (lte)2, (lte)f6, (lte)h6
};
static const lte pawn_attacks_white_lookup_g6[] =
{
    (lte)2, (lte)f7, (lte)h7
};
static const lte pawn_attacks_white_lookup_g7[] =
{
    (lte)2, (lte)f8, (lte)h8
};
static const lte pawn_attacks_white_lookup_g8[] =
{
    (lte)0
};
static const lte pawn_attacks_white_lookup_h1[] =
{
    (lte)1, (lte)g2
};
static const lte pawn_attacks_white_lookup_h2[] =
{
    (lte)1, (lte)g3
};
static const lte pawn_attacks_white_lookup_h3[] =
{
    (lte)1, (lte)g4
};
static const lte pawn_attacks_white_lookup_h4[] =
{
    (lte)1, (lte)g5
};
static const lte pawn_attacks_white_lookup_h5[] =
{
    (lte)1, (lte)g6
};
static const lte pawn_attacks_white_lookup_h6[] =
{
    (lte)1, (lte)g7
};
static const lte pawn_attacks_white_lookup_h7[] =
{
    (lte)1, (lte)g8
};
static const lte pawn_attacks_white_lookup_h8[] =
{
    (lte)0
};

// pawn_attacks_white_lookup
const lte *pawn_attacks_white_lookup[] =
{
    pawn_attacks_white_lookup_a8,
    pawn_attacks_white_lookup_b8,
    pawn_attacks_white_lookup_c8,
    pawn_attacks_white_lookup_d8,
    pawn_attacks_white_lookup_e8,
    pawn_attacks_white_lookup_f8,
    pawn_attacks_white_lookup_g8,
    pawn_attacks_white_lookup_h8,
    pawn_attacks_white_lookup_a7,
    pawn_attacks_white_lookup_b7,
    pawn_attacks_white_lookup_c7,
    pawn_attacks_white_lookup_d7,
    pawn_attacks_white_lookup_e7,
    pawn_attacks_white_lookup_f7,
    pawn_attacks_white_lookup_g7,
    pawn_attacks_white_lookup_h7,
    pawn_attacks_white_lookup_a6,
    pawn_attacks_white_lookup_b6,
    pawn_attacks_white_lookup_c6,
    pawn_attacks_white_lookup_d6,
    pawn_attacks_white_lookup_e6,
    pawn_attacks_white_lookup_f6,
    pawn_attacks_white_lookup_g6,
    pawn_attacks_white_lookup_h6,
    pawn_attacks_white_lookup_a5,
    pawn_attacks_white_lookup_b5,
    pawn_attacks_white_lookup_c5,
    pawn_attacks_white_lookup_d5,
    pawn_attacks_white_lookup_e5,
    pawn_attacks_white_lookup_f5,
    pawn_attacks_white_lookup_g5,
    pawn_attacks_white_lookup_h5,
    pawn_attacks_white_lookup_a4,
    pawn_attacks_white_lookup_b4,
    pawn_attacks_white_lookup_c4,
    pawn_attacks_white_lookup_d4,
    pawn_attacks_white_lookup_e4,
    pawn_attacks_white_lookup_f4,
    pawn_attacks_white_lookup_g4,
    pawn_attacks_white_lookup_h4,
    pawn_attacks_white_lookup_a3,
    pawn_attacks_white_lookup_b3,
    pawn_attacks_white_lookup_c3,
    pawn_attacks_white_lookup_d3,
    pawn_attacks_white_lookup_e3,
    pawn_attacks_white_lookup_f3,
    pawn_attacks_white_lookup_g3,
    pawn_attacks_white_lookup_h3,
    pawn_attacks_white_lookup_a2,
    pawn_attacks_white_lookup_b2,
    pawn_attacks_white_lookup_c2,
    pawn_attacks_white_lookup_d2,
    pawn_attacks_white_lookup_e2,
    pawn_attacks_white_lookup_f2,
    pawn_attacks_white_lookup_g2,
    pawn_attacks_white_lookup_h2,
    pawn_attacks_white_lookup_a1,
    pawn_attacks_white_lookup_b1,
    pawn_attacks_white_lookup_c1,
    pawn_attacks_white_lookup_d1,
    pawn_attacks_white_lookup_e1,
    pawn_attacks_white_lookup_f1,
    pawn_attacks_white_lookup_g1,
    pawn_attacks_white_lookup_h1
};

// Attack from up to 2 white pawns on a black piece
static const lte pawn_attacks_black_lookup_a1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_a2[] =
{
    (lte)1, (lte)b1
};
static const lte pawn_attacks_black_lookup_a3[] =
{
    (lte)1, (lte)b2
};
static const lte pawn_attacks_black_lookup_a4[] =
{
    (lte)1, (lte)b3
};
static const lte pawn_attacks_black_lookup_a5[] =
{
    (lte)1, (lte)b4
};
static const lte pawn_attacks_black_lookup_a6[] =
{
    (lte)1, (lte)b5
};
static const lte pawn_attacks_black_lookup_a7[] =
{
    (lte)1, (lte)b6
};
static const lte pawn_attacks_black_lookup_a8[] =
{
    (lte)1, (lte)b7
};
static const lte pawn_attacks_black_lookup_b1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_b2[] =
{
    (lte)2, (lte)a1, (lte)c1
};
static const lte pawn_attacks_black_lookup_b3[] =
{
    (lte)2, (lte)a2, (lte)c2
};
static const lte pawn_attacks_black_lookup_b4[] =
{
    (lte)2, (lte)a3, (lte)c3
};
static const lte pawn_attacks_black_lookup_b5[] =
{
    (lte)2, (lte)a4, (lte)c4
};
static const lte pawn_attacks_black_lookup_b6[] =
{
    (lte)2, (lte)a5, (lte)c5
};
static const lte pawn_attacks_black_lookup_b7[] =
{
    (lte)2, (lte)a6, (lte)c6
};
static const lte pawn_attacks_black_lookup_b8[] =
{
    (lte)2, (lte)a7, (lte)c7
};
static const lte pawn_attacks_black_lookup_c1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_c2[] =
{
    (lte)2, (lte)b1, (lte)d1
};
static const lte pawn_attacks_black_lookup_c3[] =
{
    (lte)2, (lte)b2, (lte)d2
};
static const lte pawn_attacks_black_lookup_c4[] =
{
    (lte)2, (lte)b3, (lte)d3
};
static const lte pawn_attacks_black_lookup_c5[] =
{
    (lte)2, (lte)b4, (lte)d4
};
static const lte pawn_attacks_black_lookup_c6[] =
{
    (lte)2, (lte)b5, (lte)d5
};
static const lte pawn_attacks_black_lookup_c7[] =
{
    (lte)2, (lte)b6, (lte)d6
};
static const lte pawn_attacks_black_lookup_c8[] =
{
    (lte)2, (lte)b7, (lte)d7
};
static const lte pawn_attacks_black_lookup_d1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_d2[] =
{
    (lte)2, (lte)c1, (lte)e1
};
static const lte pawn_attacks_black_lookup_d3[] =
{
    (lte)2, (lte)c2, (lte)e2
};
static const lte pawn_attacks_black_lookup_d4[] =
{
    (lte)2, (lte)c3, (lte)e3
};
static const lte pawn_attacks_black_lookup_d5[] =
{
    (lte)2, (lte)c4, (lte)e4
};
static const lte pawn_attacks_black_lookup_d6[] =
{
    (lte)2, (lte)c5, (lte)e5
};
static const lte pawn_attacks_black_lookup_d7[] =
{
    (lte)2, (lte)c6, (lte)e6
};
static const lte pawn_attacks_black_lookup_d8[] =
{
    (lte)2, (lte)c7, (lte)e7
};
static const lte pawn_attacks_black_lookup_e1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_e2[] =
{
    (lte)2, (lte)d1, (lte)f1
};
static const lte pawn_attacks_black_lookup_e3[] =
{
    (lte)2, (lte)d2, (lte)f2
};
static const lte pawn_attacks_black_lookup_e4[] =
{
    (lte)2, (lte)d3, (lte)f3
};
static const lte pawn_attacks_black_lookup_e5[] =
{
    (lte)2, (lte)d4, (lte)f4
};
static const lte pawn_attacks_black_lookup_e6[] =
{
    (lte)2, (lte)d5, (lte)f5
};
static const lte pawn_attacks_black_lookup_e7[] =
{
    (lte)2, (lte)d6, (lte)f6
};
static const lte pawn_attacks_black_lookup_e8[] =
{
    (lte)2, (lte)d7, (lte)f7
};
static const lte pawn_attacks_black_lookup_f1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_f2[] =
{
    (lte)2, (lte)e1, (lte)g1
};
static const lte pawn_attacks_black_lookup_f3[] =
{
    (lte)2, (lte)e2, (lte)g2
};
static const lte pawn_attacks_black_lookup_f4[] =
{
    (lte)2, (lte)e3, (lte)g3
};
static const lte pawn_attacks_black_lookup_f5[] =
{
    (lte)2, (lte)e4, (lte)g4
};
static const lte pawn_attacks_black_lookup_f6[] =
{
    (lte)2, (lte)e5, (lte)g5
};
static const lte pawn_attacks_black_lookup_f7[] =
{
    (lte)2, (lte)e6, (lte)g6
};
static const lte pawn_attacks_black_lookup_f8[] =
{
    (lte)2, (lte)e7, (lte)g7
};
static const lte pawn_attacks_black_lookup_g1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_g2[] =
{
    (lte)2, (lte)f1, (lte)h1
};
static const lte pawn_attacks_black_lookup_g3[] =
{
    (lte)2, (lte)f2, (lte)h2
};
static const lte pawn_attacks_black_lookup_g4[] =
{
    (lte)2, (lte)f3, (lte)h3
};
static const lte pawn_attacks_black_lookup_g5[] =
{
    (lte)2, (lte)f4, (lte)h4
};
static const lte pawn_attacks_black_lookup_g6[] =
{
    (lte)2, (lte)f5, (lte)h5
};
static const lte pawn_attacks_black_lookup_g7[] =
{
    (lte)2, (lte)f6, (lte)h6
};
static const lte pawn_attacks_black_lookup_g8[] =
{
    (lte)2, (lte)f7, (lte)h7
};
static const lte pawn_attacks_black_lookup_h1[] =
{
    (lte)0
};
static const lte pawn_attacks_black_lookup_h2[] =
{
    (lte)1, (lte)g1
};
static const lte pawn_attacks_black_lookup_h3[] =
{
    (lte)1, (lte)g2
};
static const lte pawn_attacks_black_lookup_h4[] =
{
    (lte)1, (lte)g3
};
static const lte pawn_attacks_black_lookup_h5[] =
{
    (lte)1, (lte)g4
};
static const lte pawn_attacks_black_lookup_h6[] =
{
    (lte)1, (lte)g5
};
static const lte pawn_attacks_black_lookup_h7[] =
{
    (lte)1, (lte)g6
};
static const lte pawn_attacks_black_lookup_h8[] =
{
    (lte)1, (lte)g7
};

// pawn_attacks_black_lookup
const lte *pawn_attacks_black_lookup[] =
{
    pawn_attacks_black_lookup_a8,
    pawn_attacks_black_lookup_b8,
    pawn_attacks_black_lookup_c8,
    pawn_attacks_black_lookup_d8,
    pawn_attacks_black_lookup_e8,
    pawn_attacks_black_lookup_f8,
    pawn_attacks_black_lookup_g8,
    pawn_attacks_black_lookup_h8,
    pawn_attacks_black_lookup_a7,
    pawn_attacks_black_lookup_b7,
    pawn_attacks_black_lookup_c7,
    pawn_attacks_black_lookup_d7,
    pawn_attacks_black_lookup_e7,
    pawn_attacks_black_lookup_f7,
    pawn_attacks_black_lookup_g7,
    pawn_attacks_black_lookup_h7,
    pawn_attacks_black_lookup_a6,
    pawn_attacks_black_lookup_b6,
    pawn_attacks_black_lookup_c6,
    pawn_attacks_black_lookup_d6,
    pawn_attacks_black_lookup_e6,
    pawn_attacks_black_lookup_f6,
    pawn_attacks_black_lookup_g6,
    pawn_attacks_black_lookup_h6,
    pawn_attacks_black_lookup_a5,
    pawn_attacks_black_lookup_b5,
    pawn_attacks_black_lookup_c5,
    pawn_attacks_black_lookup_d5,
    pawn_attacks_black_lookup_e5,
    pawn_attacks_black_lookup_f5,
    pawn_attacks_black_lookup_g5,
    pawn_attacks_black_lookup_h5,
    pawn_attacks_black_lookup_a4,
    pawn_attacks_black_lookup_b4,
    pawn_attacks_black_lookup_c4,
    pawn_attacks_black_lookup_d4,
    pawn_attacks_black_lookup_e4,
    pawn_attacks_black_lookup_f4,
    pawn_attacks_black_lookup_g4,
    pawn_attacks_black_lookup_h4,
    pawn_attacks_black_lookup_a3,
    pawn_attacks_black_lookup_b3,
    pawn_attacks_black_lookup_c3,
    pawn_attacks_black_lookup_d3,
    pawn_attacks_black_lookup_e3,
    pawn_attacks_black_lookup_f3,
    pawn_attacks_black_lookup_g3,
    pawn_attacks_black_lookup_h3,
    pawn_attacks_black_lookup_a2,
    pawn_attacks_black_lookup_b2,
    pawn_attacks_black_lookup_c2,
    pawn_attacks_black_lookup_d2,
    pawn_attacks_black_lookup_e2,
    pawn_attacks_black_lookup_f2,
    pawn_attacks_black_lookup_g2,
    pawn_attacks_black_lookup_h2,
    pawn_attacks_black_lookup_a1,
    pawn_attacks_black_lookup_b1,
    pawn_attacks_black_lookup_c1,
    pawn_attacks_black_lookup_d1,
    pawn_attacks_black_lookup_e1,
    pawn_attacks_black_lookup_f1,
    pawn_attacks_black_lookup_g1,
    pawn_attacks_black_lookup_h1
};

// Attack from up to 8 rays on a white piece
static const lte attacks_white_lookup_a1[] =
{
(lte)3
    ,(lte)7 ,(lte)b1,(lte)(K|R|Q)   ,(lte)c1,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)a2,(lte)(K|R|Q)   ,(lte)a3,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)7 ,(lte)b2,(lte)(K|P|B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
};
static const lte attacks_white_lookup_a2[] =
{
(lte)5
    ,(lte)7 ,(lte)b2,(lte)(K|R|Q)   ,(lte)c2,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)a1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)a3,(lte)(K|R|Q)   ,(lte)a4,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)6 ,(lte)b3,(lte)(K|P|B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)1 ,(lte)b1,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_a3[] =
{
(lte)5
    ,(lte)7 ,(lte)b3,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)a2,(lte)(K|R|Q)   ,(lte)a1,(lte)(R|Q) 
    ,(lte)5 ,(lte)a4,(lte)(K|R|Q)   ,(lte)a5,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)5 ,(lte)b4,(lte)(K|P|B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)f8,(lte)(B|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|B|Q)   ,(lte)c1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_a4[] =
{
(lte)5
    ,(lte)7 ,(lte)b4,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)a3,(lte)(K|R|Q)   ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)4 ,(lte)a5,(lte)(K|R|Q)   ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)4 ,(lte)b5,(lte)(K|P|B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)e8,(lte)(B|Q) 
    ,(lte)3 ,(lte)b3,(lte)(K|B|Q)   ,(lte)c2,(lte)(B|Q) ,(lte)d1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_a5[] =
{
(lte)5
    ,(lte)7 ,(lte)b5,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)a4,(lte)(K|R|Q)   ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)3 ,(lte)a6,(lte)(K|R|Q)   ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)3 ,(lte)b6,(lte)(K|P|B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)d8,(lte)(B|Q) 
    ,(lte)4 ,(lte)b4,(lte)(K|B|Q)   ,(lte)c3,(lte)(B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)e1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_a6[] =
{
(lte)5
    ,(lte)7 ,(lte)b6,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)a5,(lte)(K|R|Q)   ,(lte)a4,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)2 ,(lte)a7,(lte)(K|R|Q)   ,(lte)a8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|P|B|Q) ,(lte)c8,(lte)(B|Q) 
    ,(lte)5 ,(lte)b5,(lte)(K|B|Q)   ,(lte)c4,(lte)(B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_a7[] =
{
(lte)5
    ,(lte)7 ,(lte)b7,(lte)(K|R|Q)   ,(lte)c7,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)a6,(lte)(K|R|Q)   ,(lte)a5,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)1 ,(lte)a8,(lte)(K|R|Q)   
    ,(lte)1 ,(lte)b8,(lte)(K|P|B|Q) 
    ,(lte)6 ,(lte)b6,(lte)(K|B|Q)   ,(lte)c5,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_a8[] =
{
(lte)3
    ,(lte)7 ,(lte)b8,(lte)(K|R|Q)   ,(lte)c8,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)a7,(lte)(K|R|Q)   ,(lte)a6,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)7 ,(lte)b7,(lte)(K|B|Q)   ,(lte)c6,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_b1[] =
{
(lte)5
    ,(lte)1 ,(lte)a1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c1,(lte)(K|R|Q)   ,(lte)d1,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)b2,(lte)(K|R|Q)   ,(lte)b3,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a2,(lte)(K|P|B|Q) 
    ,(lte)6 ,(lte)c2,(lte)(K|P|B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
};
static const lte attacks_white_lookup_b2[] =
{
(lte)8
    ,(lte)1 ,(lte)a2,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c2,(lte)(K|R|Q)   ,(lte)d2,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)b1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)b3,(lte)(K|R|Q)   ,(lte)b4,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a1,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)a3,(lte)(K|P|B|Q) 
    ,(lte)6 ,(lte)c3,(lte)(K|P|B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)1 ,(lte)c1,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_b3[] =
{
(lte)8
    ,(lte)1 ,(lte)a3,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c3,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|R|Q)   ,(lte)b1,(lte)(R|Q) 
    ,(lte)5 ,(lte)b4,(lte)(K|R|Q)   ,(lte)b5,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a2,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)a4,(lte)(K|P|B|Q) 
    ,(lte)5 ,(lte)c4,(lte)(K|P|B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)2 ,(lte)c2,(lte)(K|B|Q)   ,(lte)d1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_b4[] =
{
(lte)8
    ,(lte)1 ,(lte)a4,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c4,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)b3,(lte)(K|R|Q)   ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)4 ,(lte)b5,(lte)(K|R|Q)   ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a3,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)a5,(lte)(K|P|B|Q) 
    ,(lte)4 ,(lte)c5,(lte)(K|P|B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)f8,(lte)(B|Q) 
    ,(lte)3 ,(lte)c3,(lte)(K|B|Q)   ,(lte)d2,(lte)(B|Q) ,(lte)e1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_b5[] =
{
(lte)8
    ,(lte)1 ,(lte)a5,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c5,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)b4,(lte)(K|R|Q)   ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)3 ,(lte)b6,(lte)(K|R|Q)   ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a4,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)a6,(lte)(K|P|B|Q) 
    ,(lte)3 ,(lte)c6,(lte)(K|P|B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)e8,(lte)(B|Q) 
    ,(lte)4 ,(lte)c4,(lte)(K|B|Q)   ,(lte)d3,(lte)(B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_b6[] =
{
(lte)8
    ,(lte)1 ,(lte)a6,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c6,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)b5,(lte)(K|R|Q)   ,(lte)b4,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|R|Q)   ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a5,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)a7,(lte)(K|P|B|Q) 
    ,(lte)2 ,(lte)c7,(lte)(K|P|B|Q) ,(lte)d8,(lte)(B|Q) 
    ,(lte)5 ,(lte)c5,(lte)(K|B|Q)   ,(lte)d4,(lte)(B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_b7[] =
{
(lte)8
    ,(lte)1 ,(lte)a7,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c7,(lte)(K|R|Q)   ,(lte)d7,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)b6,(lte)(K|R|Q)   ,(lte)b5,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)1 ,(lte)b8,(lte)(K|R|Q)   
    ,(lte)1 ,(lte)a6,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)a8,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)c8,(lte)(K|P|B|Q) 
    ,(lte)6 ,(lte)c6,(lte)(K|B|Q)   ,(lte)d5,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_b8[] =
{
(lte)5
    ,(lte)1 ,(lte)a8,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c8,(lte)(K|R|Q)   ,(lte)d8,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)b7,(lte)(K|R|Q)   ,(lte)b6,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)1 ,(lte)a7,(lte)(K|B|Q)   
    ,(lte)6 ,(lte)c7,(lte)(K|B|Q)   ,(lte)d6,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_white_lookup_c1[] =
{
(lte)5
    ,(lte)2 ,(lte)b1,(lte)(K|R|Q)   ,(lte)a1,(lte)(R|Q) 
    ,(lte)5 ,(lte)d1,(lte)(K|R|Q)   ,(lte)e1,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)c2,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|P|B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)5 ,(lte)d2,(lte)(K|P|B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h6,(lte)(B|Q) 
};
static const lte attacks_white_lookup_c2[] =
{
(lte)8
    ,(lte)2 ,(lte)b2,(lte)(K|R|Q)   ,(lte)a2,(lte)(R|Q) 
    ,(lte)5 ,(lte)d2,(lte)(K|R|Q)   ,(lte)e2,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)c1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c3,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)1 ,(lte)b1,(lte)(K|B|Q)   
    ,(lte)2 ,(lte)b3,(lte)(K|P|B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)5 ,(lte)d3,(lte)(K|P|B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
    ,(lte)1 ,(lte)d1,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_c3[] =
{
(lte)8
    ,(lte)2 ,(lte)b3,(lte)(K|R|Q)   ,(lte)a3,(lte)(R|Q) 
    ,(lte)5 ,(lte)d3,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)c2,(lte)(K|R|Q)   ,(lte)c1,(lte)(R|Q) 
    ,(lte)5 ,(lte)c4,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|B|Q)   ,(lte)a1,(lte)(B|Q) 
    ,(lte)2 ,(lte)b4,(lte)(K|P|B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)5 ,(lte)d4,(lte)(K|P|B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)2 ,(lte)d2,(lte)(K|B|Q)   ,(lte)e1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_c4[] =
{
(lte)8
    ,(lte)2 ,(lte)b4,(lte)(K|R|Q)   ,(lte)a4,(lte)(R|Q) 
    ,(lte)5 ,(lte)d4,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)c3,(lte)(K|R|Q)   ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)4 ,(lte)c5,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b3,(lte)(K|B|Q)   ,(lte)a2,(lte)(B|Q) 
    ,(lte)2 ,(lte)b5,(lte)(K|P|B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|P|B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)3 ,(lte)d3,(lte)(K|B|Q)   ,(lte)e2,(lte)(B|Q) ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_c5[] =
{
(lte)8
    ,(lte)2 ,(lte)b5,(lte)(K|R|Q)   ,(lte)a5,(lte)(R|Q) 
    ,(lte)5 ,(lte)d5,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)c4,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)3 ,(lte)c6,(lte)(K|R|Q)   ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b4,(lte)(K|B|Q)   ,(lte)a3,(lte)(B|Q) 
    ,(lte)2 ,(lte)b6,(lte)(K|P|B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)3 ,(lte)d6,(lte)(K|P|B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)f8,(lte)(B|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|B|Q)   ,(lte)e3,(lte)(B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_c6[] =
{
(lte)8
    ,(lte)2 ,(lte)b6,(lte)(K|R|Q)   ,(lte)a6,(lte)(R|Q) 
    ,(lte)5 ,(lte)d6,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)c5,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)2 ,(lte)c7,(lte)(K|R|Q)   ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b5,(lte)(K|B|Q)   ,(lte)a4,(lte)(B|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|P|B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)2 ,(lte)d7,(lte)(K|P|B|Q) ,(lte)e8,(lte)(B|Q) 
    ,(lte)5 ,(lte)d5,(lte)(K|B|Q)   ,(lte)e4,(lte)(B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_c7[] =
{
(lte)8
    ,(lte)2 ,(lte)b7,(lte)(K|R|Q)   ,(lte)a7,(lte)(R|Q) 
    ,(lte)5 ,(lte)d7,(lte)(K|R|Q)   ,(lte)e7,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)c6,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)1 ,(lte)c8,(lte)(K|R|Q)   
    ,(lte)2 ,(lte)b6,(lte)(K|B|Q)   ,(lte)a5,(lte)(B|Q) 
    ,(lte)1 ,(lte)b8,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)d8,(lte)(K|P|B|Q) 
    ,(lte)5 ,(lte)d6,(lte)(K|B|Q)   ,(lte)e5,(lte)(B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_white_lookup_c8[] =
{
(lte)5
    ,(lte)2 ,(lte)b8,(lte)(K|R|Q)   ,(lte)a8,(lte)(R|Q) 
    ,(lte)5 ,(lte)d8,(lte)(K|R|Q)   ,(lte)e8,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)c7,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|B|Q)   ,(lte)a6,(lte)(B|Q) 
    ,(lte)5 ,(lte)d7,(lte)(K|B|Q)   ,(lte)e6,(lte)(B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_white_lookup_d1[] =
{
(lte)5
    ,(lte)3 ,(lte)c1,(lte)(K|R|Q)   ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)4 ,(lte)e1,(lte)(K|R|Q)   ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)d2,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c2,(lte)(K|P|B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)4 ,(lte)e2,(lte)(K|P|B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h5,(lte)(B|Q) 
};
static const lte attacks_white_lookup_d2[] =
{
(lte)8
    ,(lte)3 ,(lte)c2,(lte)(K|R|Q)   ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)4 ,(lte)e2,(lte)(K|R|Q)   ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)d1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)d3,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)1 ,(lte)c1,(lte)(K|B|Q)   
    ,(lte)3 ,(lte)c3,(lte)(K|P|B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)4 ,(lte)e3,(lte)(K|P|B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h6,(lte)(B|Q) 
    ,(lte)1 ,(lte)e1,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_d3[] =
{
(lte)8
    ,(lte)3 ,(lte)c3,(lte)(K|R|Q)   ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)4 ,(lte)e3,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)d2,(lte)(K|R|Q)   ,(lte)d1,(lte)(R|Q) 
    ,(lte)5 ,(lte)d4,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)2 ,(lte)c2,(lte)(K|B|Q)   ,(lte)b1,(lte)(B|Q) 
    ,(lte)3 ,(lte)c4,(lte)(K|P|B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|P|B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
    ,(lte)2 ,(lte)e2,(lte)(K|B|Q)   ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_d4[] =
{
(lte)8
    ,(lte)3 ,(lte)c4,(lte)(K|R|Q)   ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)d3,(lte)(K|R|Q)   ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c3,(lte)(K|B|Q)   ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)3 ,(lte)c5,(lte)(K|P|B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|P|B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)3 ,(lte)e3,(lte)(K|B|Q)   ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_d5[] =
{
(lte)8
    ,(lte)3 ,(lte)c5,(lte)(K|R|Q)   ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)3 ,(lte)d6,(lte)(K|R|Q)   ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c4,(lte)(K|B|Q)   ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)3 ,(lte)c6,(lte)(K|P|B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)3 ,(lte)e6,(lte)(K|P|B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|B|Q)   ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_d6[] =
{
(lte)8
    ,(lte)3 ,(lte)c6,(lte)(K|R|Q)   ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)4 ,(lte)e6,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)d5,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)2 ,(lte)d7,(lte)(K|R|Q)   ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c5,(lte)(K|B|Q)   ,(lte)b4,(lte)(B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)2 ,(lte)c7,(lte)(K|P|B|Q) ,(lte)b8,(lte)(B|Q) 
    ,(lte)2 ,(lte)e7,(lte)(K|P|B|Q) ,(lte)f8,(lte)(B|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|B|Q)   ,(lte)f4,(lte)(B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_white_lookup_d7[] =
{
(lte)8
    ,(lte)3 ,(lte)c7,(lte)(K|R|Q)   ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)4 ,(lte)e7,(lte)(K|R|Q)   ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)d6,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)1 ,(lte)d8,(lte)(K|R|Q)   
    ,(lte)3 ,(lte)c6,(lte)(K|B|Q)   ,(lte)b5,(lte)(B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)1 ,(lte)c8,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)e8,(lte)(K|P|B|Q) 
    ,(lte)4 ,(lte)e6,(lte)(K|B|Q)   ,(lte)f5,(lte)(B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_white_lookup_d8[] =
{
(lte)5
    ,(lte)3 ,(lte)c8,(lte)(K|R|Q)   ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)4 ,(lte)e8,(lte)(K|R|Q)   ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)d7,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)3 ,(lte)c7,(lte)(K|B|Q)   ,(lte)b6,(lte)(B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)4 ,(lte)e7,(lte)(K|B|Q)   ,(lte)f6,(lte)(B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_white_lookup_e1[] =
{
(lte)5
    ,(lte)4 ,(lte)d1,(lte)(K|R|Q)   ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)3 ,(lte)f1,(lte)(K|R|Q)   ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)e2,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)4 ,(lte)d2,(lte)(K|P|B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)3 ,(lte)f2,(lte)(K|P|B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_white_lookup_e2[] =
{
(lte)8
    ,(lte)4 ,(lte)d2,(lte)(K|R|Q)   ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)3 ,(lte)f2,(lte)(K|R|Q)   ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)e1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)e3,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)1 ,(lte)d1,(lte)(K|B|Q)   
    ,(lte)4 ,(lte)d3,(lte)(K|P|B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|P|B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h5,(lte)(B|Q) 
    ,(lte)1 ,(lte)f1,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_e3[] =
{
(lte)8
    ,(lte)4 ,(lte)d3,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|R|Q)   ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)e2,(lte)(K|R|Q)   ,(lte)e1,(lte)(R|Q) 
    ,(lte)5 ,(lte)e4,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)2 ,(lte)d2,(lte)(K|B|Q)   ,(lte)c1,(lte)(B|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|P|B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)3 ,(lte)f4,(lte)(K|P|B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h6,(lte)(B|Q) 
    ,(lte)2 ,(lte)f2,(lte)(K|B|Q)   ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_e4[] =
{
(lte)8
    ,(lte)4 ,(lte)d4,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)3 ,(lte)f4,(lte)(K|R|Q)   ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)e3,(lte)(K|R|Q)   ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)3 ,(lte)d3,(lte)(K|B|Q)   ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|P|B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f5,(lte)(K|P|B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|B|Q)   ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_e5[] =
{
(lte)8
    ,(lte)4 ,(lte)d5,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)3 ,(lte)f5,(lte)(K|R|Q)   ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)3 ,(lte)e6,(lte)(K|R|Q)   ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|B|Q)   ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)3 ,(lte)d6,(lte)(K|P|B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|P|B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f4,(lte)(K|B|Q)   ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_white_lookup_e6[] =
{
(lte)8
    ,(lte)4 ,(lte)d6,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|R|Q)   ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)e5,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)2 ,(lte)e7,(lte)(K|R|Q)   ,(lte)e8,(lte)(R|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|B|Q)   ,(lte)c4,(lte)(B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)2 ,(lte)d7,(lte)(K|P|B|Q) ,(lte)c8,(lte)(B|Q) 
    ,(lte)2 ,(lte)f7,(lte)(K|P|B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f5,(lte)(K|B|Q)   ,(lte)g4,(lte)(B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_white_lookup_e7[] =
{
(lte)8
    ,(lte)4 ,(lte)d7,(lte)(K|R|Q)   ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)3 ,(lte)f7,(lte)(K|R|Q)   ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)e6,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)1 ,(lte)e8,(lte)(K|R|Q)   
    ,(lte)4 ,(lte)d6,(lte)(K|B|Q)   ,(lte)c5,(lte)(B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)1 ,(lte)d8,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)f8,(lte)(K|P|B|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|B|Q)   ,(lte)g5,(lte)(B|Q) ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_white_lookup_e8[] =
{
(lte)5
    ,(lte)4 ,(lte)d8,(lte)(K|R|Q)   ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)3 ,(lte)f8,(lte)(K|R|Q)   ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)e7,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)4 ,(lte)d7,(lte)(K|B|Q)   ,(lte)c6,(lte)(B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)3 ,(lte)f7,(lte)(K|B|Q)   ,(lte)g6,(lte)(B|Q) ,(lte)h5,(lte)(B|Q) 
};
static const lte attacks_white_lookup_f1[] =
{
(lte)5
    ,(lte)5 ,(lte)e1,(lte)(K|R|Q)   ,(lte)d1,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)2 ,(lte)g1,(lte)(K|R|Q)   ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)f2,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)5 ,(lte)e2,(lte)(K|P|B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|P|B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_white_lookup_f2[] =
{
(lte)8
    ,(lte)5 ,(lte)e2,(lte)(K|R|Q)   ,(lte)d2,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|R|Q)   ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)f1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)f3,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)1 ,(lte)e1,(lte)(K|B|Q)   
    ,(lte)5 ,(lte)e3,(lte)(K|P|B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)2 ,(lte)g3,(lte)(K|P|B|Q) ,(lte)h4,(lte)(B|Q) 
    ,(lte)1 ,(lte)g1,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_f3[] =
{
(lte)8
    ,(lte)5 ,(lte)e3,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)2 ,(lte)g3,(lte)(K|R|Q)   ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)f2,(lte)(K|R|Q)   ,(lte)f1,(lte)(R|Q) 
    ,(lte)5 ,(lte)f4,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)2 ,(lte)e2,(lte)(K|B|Q)   ,(lte)d1,(lte)(B|Q) 
    ,(lte)5 ,(lte)e4,(lte)(K|P|B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g4,(lte)(K|P|B|Q) ,(lte)h5,(lte)(B|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|B|Q)   ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_white_lookup_f4[] =
{
(lte)8
    ,(lte)5 ,(lte)e4,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)2 ,(lte)g4,(lte)(K|R|Q)   ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|R|Q)   ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)4 ,(lte)f5,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)3 ,(lte)e3,(lte)(K|B|Q)   ,(lte)d2,(lte)(B|Q) ,(lte)c1,(lte)(B|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|P|B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g5,(lte)(K|P|B|Q) ,(lte)h6,(lte)(B|Q) 
    ,(lte)2 ,(lte)g3,(lte)(K|B|Q)   ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_white_lookup_f5[] =
{
(lte)8
    ,(lte)5 ,(lte)e5,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)2 ,(lte)g5,(lte)(K|R|Q)   ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)f4,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|R|Q)   ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|B|Q)   ,(lte)d3,(lte)(B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)3 ,(lte)e6,(lte)(K|P|B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)c8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g6,(lte)(K|P|B|Q) ,(lte)h7,(lte)(B|Q) 
    ,(lte)2 ,(lte)g4,(lte)(K|B|Q)   ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_white_lookup_f6[] =
{
(lte)8
    ,(lte)5 ,(lte)e6,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)2 ,(lte)g6,(lte)(K|R|Q)   ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)f5,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)2 ,(lte)f7,(lte)(K|R|Q)   ,(lte)f8,(lte)(R|Q) 
    ,(lte)5 ,(lte)e5,(lte)(K|B|Q)   ,(lte)d4,(lte)(B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)2 ,(lte)e7,(lte)(K|P|B|Q) ,(lte)d8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|P|B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g5,(lte)(K|B|Q)   ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_white_lookup_f7[] =
{
(lte)8
    ,(lte)5 ,(lte)e7,(lte)(K|R|Q)   ,(lte)d7,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|R|Q)   ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)f6,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)1 ,(lte)f8,(lte)(K|R|Q)   
    ,(lte)5 ,(lte)e6,(lte)(K|B|Q)   ,(lte)d5,(lte)(B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)1 ,(lte)e8,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)g8,(lte)(K|P|B|Q) 
    ,(lte)2 ,(lte)g6,(lte)(K|B|Q)   ,(lte)h5,(lte)(B|Q) 
};
static const lte attacks_white_lookup_f8[] =
{
(lte)5
    ,(lte)5 ,(lte)e8,(lte)(K|R|Q)   ,(lte)d8,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)2 ,(lte)g8,(lte)(K|R|Q)   ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)f7,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)5 ,(lte)e7,(lte)(K|B|Q)   ,(lte)d6,(lte)(B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|B|Q)   ,(lte)h6,(lte)(B|Q) 
};
static const lte attacks_white_lookup_g1[] =
{
(lte)5
    ,(lte)6 ,(lte)f1,(lte)(K|R|Q)   ,(lte)e1,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)1 ,(lte)h1,(lte)(K|R|Q)   
    ,(lte)7 ,(lte)g2,(lte)(K|R|Q)   ,(lte)g3,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)6 ,(lte)f2,(lte)(K|P|B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)1 ,(lte)h2,(lte)(K|P|B|Q) 
};
static const lte attacks_white_lookup_g2[] =
{
(lte)8
    ,(lte)6 ,(lte)f2,(lte)(K|R|Q)   ,(lte)e2,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)1 ,(lte)h2,(lte)(K|R|Q)   
    ,(lte)1 ,(lte)g1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)g3,(lte)(K|R|Q)   ,(lte)g4,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)1 ,(lte)f1,(lte)(K|B|Q)   
    ,(lte)6 ,(lte)f3,(lte)(K|P|B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h3,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)h1,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_g3[] =
{
(lte)8
    ,(lte)6 ,(lte)f3,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)1 ,(lte)h3,(lte)(K|R|Q)   
    ,(lte)2 ,(lte)g2,(lte)(K|R|Q)   ,(lte)g1,(lte)(R|Q) 
    ,(lte)5 ,(lte)g4,(lte)(K|R|Q)   ,(lte)g5,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)2 ,(lte)f2,(lte)(K|B|Q)   ,(lte)e1,(lte)(B|Q) 
    ,(lte)5 ,(lte)f4,(lte)(K|P|B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h4,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)h2,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_g4[] =
{
(lte)8
    ,(lte)6 ,(lte)f4,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)1 ,(lte)h4,(lte)(K|R|Q)   
    ,(lte)3 ,(lte)g3,(lte)(K|R|Q)   ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)4 ,(lte)g5,(lte)(K|R|Q)   ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|B|Q)   ,(lte)e2,(lte)(B|Q) ,(lte)d1,(lte)(B|Q) 
    ,(lte)4 ,(lte)f5,(lte)(K|P|B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)c8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h5,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)h3,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_g5[] =
{
(lte)8
    ,(lte)6 ,(lte)f5,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)1 ,(lte)h5,(lte)(K|R|Q)   
    ,(lte)4 ,(lte)g4,(lte)(K|R|Q)   ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)3 ,(lte)g6,(lte)(K|R|Q)   ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)4 ,(lte)f4,(lte)(K|B|Q)   ,(lte)e3,(lte)(B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)c1,(lte)(B|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|P|B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)d8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h6,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)h4,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_g6[] =
{
(lte)8
    ,(lte)6 ,(lte)f6,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)1 ,(lte)h6,(lte)(K|R|Q)   
    ,(lte)5 ,(lte)g5,(lte)(K|R|Q)   ,(lte)g4,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|R|Q)   ,(lte)g8,(lte)(R|Q) 
    ,(lte)5 ,(lte)f5,(lte)(K|B|Q)   ,(lte)e4,(lte)(B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)2 ,(lte)f7,(lte)(K|P|B|Q) ,(lte)e8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h7,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)h5,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_g7[] =
{
(lte)8
    ,(lte)6 ,(lte)f7,(lte)(K|R|Q)   ,(lte)e7,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)1 ,(lte)h7,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)g6,(lte)(K|R|Q)   ,(lte)g5,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)1 ,(lte)g8,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)f6,(lte)(K|B|Q)   ,(lte)e5,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)1 ,(lte)f8,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)h8,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)h6,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_g8[] =
{
(lte)5
    ,(lte)6 ,(lte)f8,(lte)(K|R|Q)   ,(lte)e8,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)1 ,(lte)h8,(lte)(K|R|Q)   
    ,(lte)7 ,(lte)g7,(lte)(K|R|Q)   ,(lte)g6,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)6 ,(lte)f7,(lte)(K|B|Q)   ,(lte)e6,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)1 ,(lte)h7,(lte)(K|B|Q)   
};
static const lte attacks_white_lookup_h1[] =
{
(lte)3
    ,(lte)7 ,(lte)g1,(lte)(K|R|Q)   ,(lte)f1,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)7 ,(lte)h2,(lte)(K|R|Q)   ,(lte)h3,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)g2,(lte)(K|P|B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
};
static const lte attacks_white_lookup_h2[] =
{
(lte)5
    ,(lte)7 ,(lte)g2,(lte)(K|R|Q)   ,(lte)f2,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)1 ,(lte)h1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)h3,(lte)(K|R|Q)   ,(lte)h4,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)1 ,(lte)g1,(lte)(K|B|Q)   
    ,(lte)6 ,(lte)g3,(lte)(K|P|B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
};
static const lte attacks_white_lookup_h3[] =
{
(lte)5
    ,(lte)7 ,(lte)g3,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)2 ,(lte)h2,(lte)(K|R|Q)   ,(lte)h1,(lte)(R|Q) 
    ,(lte)5 ,(lte)h4,(lte)(K|R|Q)   ,(lte)h5,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|B|Q)   ,(lte)f1,(lte)(B|Q) 
    ,(lte)5 ,(lte)g4,(lte)(K|P|B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)c8,(lte)(B|Q) 
};
static const lte attacks_white_lookup_h4[] =
{
(lte)5
    ,(lte)7 ,(lte)g4,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)3 ,(lte)h3,(lte)(K|R|Q)   ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)4 ,(lte)h5,(lte)(K|R|Q)   ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)3 ,(lte)g3,(lte)(K|B|Q)   ,(lte)f2,(lte)(B|Q) ,(lte)e1,(lte)(B|Q) 
    ,(lte)4 ,(lte)g5,(lte)(K|P|B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)d8,(lte)(B|Q) 
};
static const lte attacks_white_lookup_h5[] =
{
(lte)5
    ,(lte)7 ,(lte)g5,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)4 ,(lte)h4,(lte)(K|R|Q)   ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)3 ,(lte)h6,(lte)(K|R|Q)   ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)4 ,(lte)g4,(lte)(K|B|Q)   ,(lte)f3,(lte)(B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)d1,(lte)(B|Q) 
    ,(lte)3 ,(lte)g6,(lte)(K|P|B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)e8,(lte)(B|Q) 
};
static const lte attacks_white_lookup_h6[] =
{
(lte)5
    ,(lte)7 ,(lte)g6,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)5 ,(lte)h5,(lte)(K|R|Q)   ,(lte)h4,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)2 ,(lte)h7,(lte)(K|R|Q)   ,(lte)h8,(lte)(R|Q) 
    ,(lte)5 ,(lte)g5,(lte)(K|B|Q)   ,(lte)f4,(lte)(B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)c1,(lte)(B|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|P|B|Q) ,(lte)f8,(lte)(B|Q) 
};
static const lte attacks_white_lookup_h7[] =
{
(lte)5
    ,(lte)7 ,(lte)g7,(lte)(K|R|Q)   ,(lte)f7,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)6 ,(lte)h6,(lte)(K|R|Q)   ,(lte)h5,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)1 ,(lte)h8,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)g6,(lte)(K|B|Q)   ,(lte)f5,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)1 ,(lte)g8,(lte)(K|P|B|Q) 
};
static const lte attacks_white_lookup_h8[] =
{
(lte)3
    ,(lte)7 ,(lte)g8,(lte)(K|R|Q)   ,(lte)f8,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)7 ,(lte)h7,(lte)(K|R|Q)   ,(lte)h6,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)g7,(lte)(K|B|Q)   ,(lte)f6,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
};

// attacks_white_lookup
const lte *attacks_white_lookup[] =
{
    attacks_white_lookup_a8,
    attacks_white_lookup_b8,
    attacks_white_lookup_c8,
    attacks_white_lookup_d8,
    attacks_white_lookup_e8,
    attacks_white_lookup_f8,
    attacks_white_lookup_g8,
    attacks_white_lookup_h8,
    attacks_white_lookup_a7,
    attacks_white_lookup_b7,
    attacks_white_lookup_c7,
    attacks_white_lookup_d7,
    attacks_white_lookup_e7,
    attacks_white_lookup_f7,
    attacks_white_lookup_g7,
    attacks_white_lookup_h7,
    attacks_white_lookup_a6,
    attacks_white_lookup_b6,
    attacks_white_lookup_c6,
    attacks_white_lookup_d6,
    attacks_white_lookup_e6,
    attacks_white_lookup_f6,
    attacks_white_lookup_g6,
    attacks_white_lookup_h6,
    attacks_white_lookup_a5,
    attacks_white_lookup_b5,
    attacks_white_lookup_c5,
    attacks_white_lookup_d5,
    attacks_white_lookup_e5,
    attacks_white_lookup_f5,
    attacks_white_lookup_g5,
    attacks_white_lookup_h5,
    attacks_white_lookup_a4,
    attacks_white_lookup_b4,
    attacks_white_lookup_c4,
    attacks_white_lookup_d4,
    attacks_white_lookup_e4,
    attacks_white_lookup_f4,
    attacks_white_lookup_g4,
    attacks_white_lookup_h4,
    attacks_white_lookup_a3,
    attacks_white_lookup_b3,
    attacks_white_lookup_c3,
    attacks_white_lookup_d3,
    attacks_white_lookup_e3,
    attacks_white_lookup_f3,
    attacks_white_lookup_g3,
    attacks_white_lookup_h3,
    attacks_white_lookup_a2,
    attacks_white_lookup_b2,
    attacks_white_lookup_c2,
    attacks_white_lookup_d2,
    attacks_white_lookup_e2,
    attacks_white_lookup_f2,
    attacks_white_lookup_g2,
    attacks_white_lookup_h2,
    attacks_white_lookup_a1,
    attacks_white_lookup_b1,
    attacks_white_lookup_c1,
    attacks_white_lookup_d1,
    attacks_white_lookup_e1,
    attacks_white_lookup_f1,
    attacks_white_lookup_g1,
    attacks_white_lookup_h1
};

// Attack from up to 8 rays on a black piece
static const lte attacks_black_lookup_a1[] =
{
(lte)3
    ,(lte)7 ,(lte)b1,(lte)(K|R|Q)   ,(lte)c1,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)a2,(lte)(K|R|Q)   ,(lte)a3,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)7 ,(lte)b2,(lte)(K|B|Q)   ,(lte)c3,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
};
static const lte attacks_black_lookup_a2[] =
{
(lte)5
    ,(lte)7 ,(lte)b2,(lte)(K|R|Q)   ,(lte)c2,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)a1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)a3,(lte)(K|R|Q)   ,(lte)a4,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)6 ,(lte)b3,(lte)(K|B|Q)   ,(lte)c4,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)1 ,(lte)b1,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_a3[] =
{
(lte)5
    ,(lte)7 ,(lte)b3,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)a2,(lte)(K|R|Q)   ,(lte)a1,(lte)(R|Q) 
    ,(lte)5 ,(lte)a4,(lte)(K|R|Q)   ,(lte)a5,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)5 ,(lte)b4,(lte)(K|B|Q)   ,(lte)c5,(lte)(B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)f8,(lte)(B|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|P|B|Q) ,(lte)c1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_a4[] =
{
(lte)5
    ,(lte)7 ,(lte)b4,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)a3,(lte)(K|R|Q)   ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)4 ,(lte)a5,(lte)(K|R|Q)   ,(lte)a6,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)4 ,(lte)b5,(lte)(K|B|Q)   ,(lte)c6,(lte)(B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)e8,(lte)(B|Q) 
    ,(lte)3 ,(lte)b3,(lte)(K|P|B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)d1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_a5[] =
{
(lte)5
    ,(lte)7 ,(lte)b5,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)a4,(lte)(K|R|Q)   ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)3 ,(lte)a6,(lte)(K|R|Q)   ,(lte)a7,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)3 ,(lte)b6,(lte)(K|B|Q)   ,(lte)c7,(lte)(B|Q) ,(lte)d8,(lte)(B|Q) 
    ,(lte)4 ,(lte)b4,(lte)(K|P|B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)e1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_a6[] =
{
(lte)5
    ,(lte)7 ,(lte)b6,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)a5,(lte)(K|R|Q)   ,(lte)a4,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)2 ,(lte)a7,(lte)(K|R|Q)   ,(lte)a8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|B|Q)   ,(lte)c8,(lte)(B|Q) 
    ,(lte)5 ,(lte)b5,(lte)(K|P|B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_a7[] =
{
(lte)5
    ,(lte)7 ,(lte)b7,(lte)(K|R|Q)   ,(lte)c7,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)a6,(lte)(K|R|Q)   ,(lte)a5,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)1 ,(lte)a8,(lte)(K|R|Q)   
    ,(lte)1 ,(lte)b8,(lte)(K|B|Q)   
    ,(lte)6 ,(lte)b6,(lte)(K|P|B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_a8[] =
{
(lte)3
    ,(lte)7 ,(lte)b8,(lte)(K|R|Q)   ,(lte)c8,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)a7,(lte)(K|R|Q)   ,(lte)a6,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)7 ,(lte)b7,(lte)(K|P|B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_b1[] =
{
(lte)5
    ,(lte)1 ,(lte)a1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c1,(lte)(K|R|Q)   ,(lte)d1,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)b2,(lte)(K|R|Q)   ,(lte)b3,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a2,(lte)(K|B|Q)   
    ,(lte)6 ,(lte)c2,(lte)(K|B|Q)   ,(lte)d3,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
};
static const lte attacks_black_lookup_b2[] =
{
(lte)8
    ,(lte)1 ,(lte)a2,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c2,(lte)(K|R|Q)   ,(lte)d2,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)b1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)b3,(lte)(K|R|Q)   ,(lte)b4,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a1,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)a3,(lte)(K|B|Q)   
    ,(lte)6 ,(lte)c3,(lte)(K|B|Q)   ,(lte)d4,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)1 ,(lte)c1,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_b3[] =
{
(lte)8
    ,(lte)1 ,(lte)a3,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c3,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|R|Q)   ,(lte)b1,(lte)(R|Q) 
    ,(lte)5 ,(lte)b4,(lte)(K|R|Q)   ,(lte)b5,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a2,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)a4,(lte)(K|B|Q)   
    ,(lte)5 ,(lte)c4,(lte)(K|B|Q)   ,(lte)d5,(lte)(B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)2 ,(lte)c2,(lte)(K|P|B|Q) ,(lte)d1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_b4[] =
{
(lte)8
    ,(lte)1 ,(lte)a4,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c4,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)b3,(lte)(K|R|Q)   ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)4 ,(lte)b5,(lte)(K|R|Q)   ,(lte)b6,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a3,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)a5,(lte)(K|B|Q)   
    ,(lte)4 ,(lte)c5,(lte)(K|B|Q)   ,(lte)d6,(lte)(B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)f8,(lte)(B|Q) 
    ,(lte)3 ,(lte)c3,(lte)(K|P|B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)e1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_b5[] =
{
(lte)8
    ,(lte)1 ,(lte)a5,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c5,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)b4,(lte)(K|R|Q)   ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)3 ,(lte)b6,(lte)(K|R|Q)   ,(lte)b7,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a4,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)a6,(lte)(K|B|Q)   
    ,(lte)3 ,(lte)c6,(lte)(K|B|Q)   ,(lte)d7,(lte)(B|Q) ,(lte)e8,(lte)(B|Q) 
    ,(lte)4 ,(lte)c4,(lte)(K|P|B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_b6[] =
{
(lte)8
    ,(lte)1 ,(lte)a6,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c6,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)b5,(lte)(K|R|Q)   ,(lte)b4,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|R|Q)   ,(lte)b8,(lte)(R|Q) 
    ,(lte)1 ,(lte)a5,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)a7,(lte)(K|B|Q)   
    ,(lte)2 ,(lte)c7,(lte)(K|B|Q)   ,(lte)d8,(lte)(B|Q) 
    ,(lte)5 ,(lte)c5,(lte)(K|P|B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_b7[] =
{
(lte)8
    ,(lte)1 ,(lte)a7,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c7,(lte)(K|R|Q)   ,(lte)d7,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)b6,(lte)(K|R|Q)   ,(lte)b5,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)1 ,(lte)b8,(lte)(K|R|Q)   
    ,(lte)1 ,(lte)a6,(lte)(K|P|B|Q) 
    ,(lte)1 ,(lte)a8,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)c8,(lte)(K|B|Q)   
    ,(lte)6 ,(lte)c6,(lte)(K|P|B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_b8[] =
{
(lte)5
    ,(lte)1 ,(lte)a8,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c8,(lte)(K|R|Q)   ,(lte)d8,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)b7,(lte)(K|R|Q)   ,(lte)b6,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) 
    ,(lte)1 ,(lte)a7,(lte)(K|P|B|Q) 
    ,(lte)6 ,(lte)c7,(lte)(K|P|B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_black_lookup_c1[] =
{
(lte)5
    ,(lte)2 ,(lte)b1,(lte)(K|R|Q)   ,(lte)a1,(lte)(R|Q) 
    ,(lte)5 ,(lte)d1,(lte)(K|R|Q)   ,(lte)e1,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)c2,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|B|Q)   ,(lte)a3,(lte)(B|Q) 
    ,(lte)5 ,(lte)d2,(lte)(K|B|Q)   ,(lte)e3,(lte)(B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h6,(lte)(B|Q) 
};
static const lte attacks_black_lookup_c2[] =
{
(lte)8
    ,(lte)2 ,(lte)b2,(lte)(K|R|Q)   ,(lte)a2,(lte)(R|Q) 
    ,(lte)5 ,(lte)d2,(lte)(K|R|Q)   ,(lte)e2,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)c1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)c3,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)1 ,(lte)b1,(lte)(K|P|B|Q) 
    ,(lte)2 ,(lte)b3,(lte)(K|B|Q)   ,(lte)a4,(lte)(B|Q) 
    ,(lte)5 ,(lte)d3,(lte)(K|B|Q)   ,(lte)e4,(lte)(B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
    ,(lte)1 ,(lte)d1,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_c3[] =
{
(lte)8
    ,(lte)2 ,(lte)b3,(lte)(K|R|Q)   ,(lte)a3,(lte)(R|Q) 
    ,(lte)5 ,(lte)d3,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)c2,(lte)(K|R|Q)   ,(lte)c1,(lte)(R|Q) 
    ,(lte)5 ,(lte)c4,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b2,(lte)(K|P|B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)2 ,(lte)b4,(lte)(K|B|Q)   ,(lte)a5,(lte)(B|Q) 
    ,(lte)5 ,(lte)d4,(lte)(K|B|Q)   ,(lte)e5,(lte)(B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)2 ,(lte)d2,(lte)(K|P|B|Q) ,(lte)e1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_c4[] =
{
(lte)8
    ,(lte)2 ,(lte)b4,(lte)(K|R|Q)   ,(lte)a4,(lte)(R|Q) 
    ,(lte)5 ,(lte)d4,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)c3,(lte)(K|R|Q)   ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)4 ,(lte)c5,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b3,(lte)(K|P|B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)2 ,(lte)b5,(lte)(K|B|Q)   ,(lte)a6,(lte)(B|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|B|Q)   ,(lte)e6,(lte)(B|Q) ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)3 ,(lte)d3,(lte)(K|P|B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_c5[] =
{
(lte)8
    ,(lte)2 ,(lte)b5,(lte)(K|R|Q)   ,(lte)a5,(lte)(R|Q) 
    ,(lte)5 ,(lte)d5,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)c4,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)3 ,(lte)c6,(lte)(K|R|Q)   ,(lte)c7,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b4,(lte)(K|P|B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)2 ,(lte)b6,(lte)(K|B|Q)   ,(lte)a7,(lte)(B|Q) 
    ,(lte)3 ,(lte)d6,(lte)(K|B|Q)   ,(lte)e7,(lte)(B|Q) ,(lte)f8,(lte)(B|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|P|B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_c6[] =
{
(lte)8
    ,(lte)2 ,(lte)b6,(lte)(K|R|Q)   ,(lte)a6,(lte)(R|Q) 
    ,(lte)5 ,(lte)d6,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)c5,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)2 ,(lte)c7,(lte)(K|R|Q)   ,(lte)c8,(lte)(R|Q) 
    ,(lte)2 ,(lte)b5,(lte)(K|P|B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|B|Q)   ,(lte)a8,(lte)(B|Q) 
    ,(lte)2 ,(lte)d7,(lte)(K|B|Q)   ,(lte)e8,(lte)(B|Q) 
    ,(lte)5 ,(lte)d5,(lte)(K|P|B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_c7[] =
{
(lte)8
    ,(lte)2 ,(lte)b7,(lte)(K|R|Q)   ,(lte)a7,(lte)(R|Q) 
    ,(lte)5 ,(lte)d7,(lte)(K|R|Q)   ,(lte)e7,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)c6,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)1 ,(lte)c8,(lte)(K|R|Q)   
    ,(lte)2 ,(lte)b6,(lte)(K|P|B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)1 ,(lte)b8,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)d8,(lte)(K|B|Q)   
    ,(lte)5 ,(lte)d6,(lte)(K|P|B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_black_lookup_c8[] =
{
(lte)5
    ,(lte)2 ,(lte)b8,(lte)(K|R|Q)   ,(lte)a8,(lte)(R|Q) 
    ,(lte)5 ,(lte)d8,(lte)(K|R|Q)   ,(lte)e8,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)c7,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) 
    ,(lte)2 ,(lte)b7,(lte)(K|P|B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)5 ,(lte)d7,(lte)(K|P|B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_black_lookup_d1[] =
{
(lte)5
    ,(lte)3 ,(lte)c1,(lte)(K|R|Q)   ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)4 ,(lte)e1,(lte)(K|R|Q)   ,(lte)f1,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)d2,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c2,(lte)(K|B|Q)   ,(lte)b3,(lte)(B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)4 ,(lte)e2,(lte)(K|B|Q)   ,(lte)f3,(lte)(B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h5,(lte)(B|Q) 
};
static const lte attacks_black_lookup_d2[] =
{
(lte)8
    ,(lte)3 ,(lte)c2,(lte)(K|R|Q)   ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)4 ,(lte)e2,(lte)(K|R|Q)   ,(lte)f2,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)d1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)d3,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)1 ,(lte)c1,(lte)(K|P|B|Q) 
    ,(lte)3 ,(lte)c3,(lte)(K|B|Q)   ,(lte)b4,(lte)(B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)4 ,(lte)e3,(lte)(K|B|Q)   ,(lte)f4,(lte)(B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h6,(lte)(B|Q) 
    ,(lte)1 ,(lte)e1,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_d3[] =
{
(lte)8
    ,(lte)3 ,(lte)c3,(lte)(K|R|Q)   ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)4 ,(lte)e3,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)d2,(lte)(K|R|Q)   ,(lte)d1,(lte)(R|Q) 
    ,(lte)5 ,(lte)d4,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)2 ,(lte)c2,(lte)(K|P|B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)3 ,(lte)c4,(lte)(K|B|Q)   ,(lte)b5,(lte)(B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|B|Q)   ,(lte)f5,(lte)(B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
    ,(lte)2 ,(lte)e2,(lte)(K|P|B|Q) ,(lte)f1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_d4[] =
{
(lte)8
    ,(lte)3 ,(lte)c4,(lte)(K|R|Q)   ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)d3,(lte)(K|R|Q)   ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c3,(lte)(K|P|B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)3 ,(lte)c5,(lte)(K|B|Q)   ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|B|Q)   ,(lte)f6,(lte)(B|Q) ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)3 ,(lte)e3,(lte)(K|P|B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_d5[] =
{
(lte)8
    ,(lte)3 ,(lte)c5,(lte)(K|R|Q)   ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)3 ,(lte)d6,(lte)(K|R|Q)   ,(lte)d7,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c4,(lte)(K|P|B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)3 ,(lte)c6,(lte)(K|B|Q)   ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)3 ,(lte)e6,(lte)(K|B|Q)   ,(lte)f7,(lte)(B|Q) ,(lte)g8,(lte)(B|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|P|B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_d6[] =
{
(lte)8
    ,(lte)3 ,(lte)c6,(lte)(K|R|Q)   ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)4 ,(lte)e6,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)d5,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)2 ,(lte)d7,(lte)(K|R|Q)   ,(lte)d8,(lte)(R|Q) 
    ,(lte)3 ,(lte)c5,(lte)(K|P|B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)2 ,(lte)c7,(lte)(K|B|Q)   ,(lte)b8,(lte)(B|Q) 
    ,(lte)2 ,(lte)e7,(lte)(K|B|Q)   ,(lte)f8,(lte)(B|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|P|B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_black_lookup_d7[] =
{
(lte)8
    ,(lte)3 ,(lte)c7,(lte)(K|R|Q)   ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)4 ,(lte)e7,(lte)(K|R|Q)   ,(lte)f7,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)d6,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)1 ,(lte)d8,(lte)(K|R|Q)   
    ,(lte)3 ,(lte)c6,(lte)(K|P|B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)1 ,(lte)c8,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)e8,(lte)(K|B|Q)   
    ,(lte)4 ,(lte)e6,(lte)(K|P|B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_black_lookup_d8[] =
{
(lte)5
    ,(lte)3 ,(lte)c8,(lte)(K|R|Q)   ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)4 ,(lte)e8,(lte)(K|R|Q)   ,(lte)f8,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)d7,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) 
    ,(lte)3 ,(lte)c7,(lte)(K|P|B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)4 ,(lte)e7,(lte)(K|P|B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_black_lookup_e1[] =
{
(lte)5
    ,(lte)4 ,(lte)d1,(lte)(K|R|Q)   ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)3 ,(lte)f1,(lte)(K|R|Q)   ,(lte)g1,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)e2,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)4 ,(lte)d2,(lte)(K|B|Q)   ,(lte)c3,(lte)(B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a5,(lte)(B|Q) 
    ,(lte)3 ,(lte)f2,(lte)(K|B|Q)   ,(lte)g3,(lte)(B|Q) ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_black_lookup_e2[] =
{
(lte)8
    ,(lte)4 ,(lte)d2,(lte)(K|R|Q)   ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)3 ,(lte)f2,(lte)(K|R|Q)   ,(lte)g2,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)e1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)e3,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)1 ,(lte)d1,(lte)(K|P|B|Q) 
    ,(lte)4 ,(lte)d3,(lte)(K|B|Q)   ,(lte)c4,(lte)(B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|B|Q)   ,(lte)g4,(lte)(B|Q) ,(lte)h5,(lte)(B|Q) 
    ,(lte)1 ,(lte)f1,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_e3[] =
{
(lte)8
    ,(lte)4 ,(lte)d3,(lte)(K|R|Q)   ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|R|Q)   ,(lte)g3,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)e2,(lte)(K|R|Q)   ,(lte)e1,(lte)(R|Q) 
    ,(lte)5 ,(lte)e4,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)2 ,(lte)d2,(lte)(K|P|B|Q) ,(lte)c1,(lte)(B|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|B|Q)   ,(lte)c5,(lte)(B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)3 ,(lte)f4,(lte)(K|B|Q)   ,(lte)g5,(lte)(B|Q) ,(lte)h6,(lte)(B|Q) 
    ,(lte)2 ,(lte)f2,(lte)(K|P|B|Q) ,(lte)g1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_e4[] =
{
(lte)8
    ,(lte)4 ,(lte)d4,(lte)(K|R|Q)   ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)3 ,(lte)f4,(lte)(K|R|Q)   ,(lte)g4,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)e3,(lte)(K|R|Q)   ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)3 ,(lte)d3,(lte)(K|P|B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|B|Q)   ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f5,(lte)(K|B|Q)   ,(lte)g6,(lte)(B|Q) ,(lte)h7,(lte)(B|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|P|B|Q) ,(lte)g2,(lte)(B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_e5[] =
{
(lte)8
    ,(lte)4 ,(lte)d5,(lte)(K|R|Q)   ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)3 ,(lte)f5,(lte)(K|R|Q)   ,(lte)g5,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)3 ,(lte)e6,(lte)(K|R|Q)   ,(lte)e7,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) 
    ,(lte)4 ,(lte)d4,(lte)(K|P|B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)3 ,(lte)d6,(lte)(K|B|Q)   ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|B|Q)   ,(lte)g7,(lte)(B|Q) ,(lte)h8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f4,(lte)(K|P|B|Q) ,(lte)g3,(lte)(B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_black_lookup_e6[] =
{
(lte)8
    ,(lte)4 ,(lte)d6,(lte)(K|R|Q)   ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|R|Q)   ,(lte)g6,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)e5,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)2 ,(lte)e7,(lte)(K|R|Q)   ,(lte)e8,(lte)(R|Q) 
    ,(lte)4 ,(lte)d5,(lte)(K|P|B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)2 ,(lte)d7,(lte)(K|B|Q)   ,(lte)c8,(lte)(B|Q) 
    ,(lte)2 ,(lte)f7,(lte)(K|B|Q)   ,(lte)g8,(lte)(B|Q) 
    ,(lte)3 ,(lte)f5,(lte)(K|P|B|Q) ,(lte)g4,(lte)(B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_black_lookup_e7[] =
{
(lte)8
    ,(lte)4 ,(lte)d7,(lte)(K|R|Q)   ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)3 ,(lte)f7,(lte)(K|R|Q)   ,(lte)g7,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)e6,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)1 ,(lte)e8,(lte)(K|R|Q)   
    ,(lte)4 ,(lte)d6,(lte)(K|P|B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)1 ,(lte)d8,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)f8,(lte)(K|B|Q)   
    ,(lte)3 ,(lte)f6,(lte)(K|P|B|Q) ,(lte)g5,(lte)(B|Q) ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_black_lookup_e8[] =
{
(lte)5
    ,(lte)4 ,(lte)d8,(lte)(K|R|Q)   ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)3 ,(lte)f8,(lte)(K|R|Q)   ,(lte)g8,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)e7,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) 
    ,(lte)4 ,(lte)d7,(lte)(K|P|B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a4,(lte)(B|Q) 
    ,(lte)3 ,(lte)f7,(lte)(K|P|B|Q) ,(lte)g6,(lte)(B|Q) ,(lte)h5,(lte)(B|Q) 
};
static const lte attacks_black_lookup_f1[] =
{
(lte)5
    ,(lte)5 ,(lte)e1,(lte)(K|R|Q)   ,(lte)d1,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)2 ,(lte)g1,(lte)(K|R|Q)   ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)f2,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)5 ,(lte)e2,(lte)(K|B|Q)   ,(lte)d3,(lte)(B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b5,(lte)(B|Q) ,(lte)a6,(lte)(B|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|B|Q)   ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_black_lookup_f2[] =
{
(lte)8
    ,(lte)5 ,(lte)e2,(lte)(K|R|Q)   ,(lte)d2,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|R|Q)   ,(lte)h2,(lte)(R|Q) 
    ,(lte)1 ,(lte)f1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)f3,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)1 ,(lte)e1,(lte)(K|P|B|Q) 
    ,(lte)5 ,(lte)e3,(lte)(K|B|Q)   ,(lte)d4,(lte)(B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)2 ,(lte)g3,(lte)(K|B|Q)   ,(lte)h4,(lte)(B|Q) 
    ,(lte)1 ,(lte)g1,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_f3[] =
{
(lte)8
    ,(lte)5 ,(lte)e3,(lte)(K|R|Q)   ,(lte)d3,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)2 ,(lte)g3,(lte)(K|R|Q)   ,(lte)h3,(lte)(R|Q) 
    ,(lte)2 ,(lte)f2,(lte)(K|R|Q)   ,(lte)f1,(lte)(R|Q) 
    ,(lte)5 ,(lte)f4,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)2 ,(lte)e2,(lte)(K|P|B|Q) ,(lte)d1,(lte)(B|Q) 
    ,(lte)5 ,(lte)e4,(lte)(K|B|Q)   ,(lte)d5,(lte)(B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g4,(lte)(K|B|Q)   ,(lte)h5,(lte)(B|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|P|B|Q) ,(lte)h1,(lte)(B|Q) 
};
static const lte attacks_black_lookup_f4[] =
{
(lte)8
    ,(lte)5 ,(lte)e4,(lte)(K|R|Q)   ,(lte)d4,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)2 ,(lte)g4,(lte)(K|R|Q)   ,(lte)h4,(lte)(R|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|R|Q)   ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)4 ,(lte)f5,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)3 ,(lte)e3,(lte)(K|P|B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)c1,(lte)(B|Q) 
    ,(lte)4 ,(lte)e5,(lte)(K|B|Q)   ,(lte)d6,(lte)(B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g5,(lte)(K|B|Q)   ,(lte)h6,(lte)(B|Q) 
    ,(lte)2 ,(lte)g3,(lte)(K|P|B|Q) ,(lte)h2,(lte)(B|Q) 
};
static const lte attacks_black_lookup_f5[] =
{
(lte)8
    ,(lte)5 ,(lte)e5,(lte)(K|R|Q)   ,(lte)d5,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)2 ,(lte)g5,(lte)(K|R|Q)   ,(lte)h5,(lte)(R|Q) 
    ,(lte)4 ,(lte)f4,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|R|Q)   ,(lte)f7,(lte)(R|Q) ,(lte)f8,(lte)(R|Q) 
    ,(lte)4 ,(lte)e4,(lte)(K|P|B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)3 ,(lte)e6,(lte)(K|B|Q)   ,(lte)d7,(lte)(B|Q) ,(lte)c8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g6,(lte)(K|B|Q)   ,(lte)h7,(lte)(B|Q) 
    ,(lte)2 ,(lte)g4,(lte)(K|P|B|Q) ,(lte)h3,(lte)(B|Q) 
};
static const lte attacks_black_lookup_f6[] =
{
(lte)8
    ,(lte)5 ,(lte)e6,(lte)(K|R|Q)   ,(lte)d6,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)2 ,(lte)g6,(lte)(K|R|Q)   ,(lte)h6,(lte)(R|Q) 
    ,(lte)5 ,(lte)f5,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)2 ,(lte)f7,(lte)(K|R|Q)   ,(lte)f8,(lte)(R|Q) 
    ,(lte)5 ,(lte)e5,(lte)(K|P|B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)2 ,(lte)e7,(lte)(K|B|Q)   ,(lte)d8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|B|Q)   ,(lte)h8,(lte)(B|Q) 
    ,(lte)2 ,(lte)g5,(lte)(K|P|B|Q) ,(lte)h4,(lte)(B|Q) 
};
static const lte attacks_black_lookup_f7[] =
{
(lte)8
    ,(lte)5 ,(lte)e7,(lte)(K|R|Q)   ,(lte)d7,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|R|Q)   ,(lte)h7,(lte)(R|Q) 
    ,(lte)6 ,(lte)f6,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)1 ,(lte)f8,(lte)(K|R|Q)   
    ,(lte)5 ,(lte)e6,(lte)(K|P|B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)1 ,(lte)e8,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)g8,(lte)(K|B|Q)   
    ,(lte)2 ,(lte)g6,(lte)(K|P|B|Q) ,(lte)h5,(lte)(B|Q) 
};
static const lte attacks_black_lookup_f8[] =
{
(lte)5
    ,(lte)5 ,(lte)e8,(lte)(K|R|Q)   ,(lte)d8,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)2 ,(lte)g8,(lte)(K|R|Q)   ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)f7,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)f5,(lte)(R|Q) ,(lte)f4,(lte)(R|Q) ,(lte)f3,(lte)(R|Q) ,(lte)f2,(lte)(R|Q) ,(lte)f1,(lte)(R|Q) 
    ,(lte)5 ,(lte)e7,(lte)(K|P|B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b4,(lte)(B|Q) ,(lte)a3,(lte)(B|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|P|B|Q) ,(lte)h6,(lte)(B|Q) 
};
static const lte attacks_black_lookup_g1[] =
{
(lte)5
    ,(lte)6 ,(lte)f1,(lte)(K|R|Q)   ,(lte)e1,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)1 ,(lte)h1,(lte)(K|R|Q)   
    ,(lte)7 ,(lte)g2,(lte)(K|R|Q)   ,(lte)g3,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)6 ,(lte)f2,(lte)(K|B|Q)   ,(lte)e3,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c5,(lte)(B|Q) ,(lte)b6,(lte)(B|Q) ,(lte)a7,(lte)(B|Q) 
    ,(lte)1 ,(lte)h2,(lte)(K|B|Q)   
};
static const lte attacks_black_lookup_g2[] =
{
(lte)8
    ,(lte)6 ,(lte)f2,(lte)(K|R|Q)   ,(lte)e2,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)1 ,(lte)h2,(lte)(K|R|Q)   
    ,(lte)1 ,(lte)g1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)g3,(lte)(K|R|Q)   ,(lte)g4,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)1 ,(lte)f1,(lte)(K|P|B|Q) 
    ,(lte)6 ,(lte)f3,(lte)(K|B|Q)   ,(lte)e4,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h3,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)h1,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_g3[] =
{
(lte)8
    ,(lte)6 ,(lte)f3,(lte)(K|R|Q)   ,(lte)e3,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)1 ,(lte)h3,(lte)(K|R|Q)   
    ,(lte)2 ,(lte)g2,(lte)(K|R|Q)   ,(lte)g1,(lte)(R|Q) 
    ,(lte)5 ,(lte)g4,(lte)(K|R|Q)   ,(lte)g5,(lte)(R|Q) ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)2 ,(lte)f2,(lte)(K|P|B|Q) ,(lte)e1,(lte)(B|Q) 
    ,(lte)5 ,(lte)f4,(lte)(K|B|Q)   ,(lte)e5,(lte)(B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h4,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)h2,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_g4[] =
{
(lte)8
    ,(lte)6 ,(lte)f4,(lte)(K|R|Q)   ,(lte)e4,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)1 ,(lte)h4,(lte)(K|R|Q)   
    ,(lte)3 ,(lte)g3,(lte)(K|R|Q)   ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)4 ,(lte)g5,(lte)(K|R|Q)   ,(lte)g6,(lte)(R|Q) ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)3 ,(lte)f3,(lte)(K|P|B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)d1,(lte)(B|Q) 
    ,(lte)4 ,(lte)f5,(lte)(K|B|Q)   ,(lte)e6,(lte)(B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)c8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h5,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)h3,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_g5[] =
{
(lte)8
    ,(lte)6 ,(lte)f5,(lte)(K|R|Q)   ,(lte)e5,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)1 ,(lte)h5,(lte)(K|R|Q)   
    ,(lte)4 ,(lte)g4,(lte)(K|R|Q)   ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)3 ,(lte)g6,(lte)(K|R|Q)   ,(lte)g7,(lte)(R|Q) ,(lte)g8,(lte)(R|Q) 
    ,(lte)4 ,(lte)f4,(lte)(K|P|B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)c1,(lte)(B|Q) 
    ,(lte)3 ,(lte)f6,(lte)(K|B|Q)   ,(lte)e7,(lte)(B|Q) ,(lte)d8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h6,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)h4,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_g6[] =
{
(lte)8
    ,(lte)6 ,(lte)f6,(lte)(K|R|Q)   ,(lte)e6,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)1 ,(lte)h6,(lte)(K|R|Q)   
    ,(lte)5 ,(lte)g5,(lte)(K|R|Q)   ,(lte)g4,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|R|Q)   ,(lte)g8,(lte)(R|Q) 
    ,(lte)5 ,(lte)f5,(lte)(K|P|B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)2 ,(lte)f7,(lte)(K|B|Q)   ,(lte)e8,(lte)(B|Q) 
    ,(lte)1 ,(lte)h7,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)h5,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_g7[] =
{
(lte)8
    ,(lte)6 ,(lte)f7,(lte)(K|R|Q)   ,(lte)e7,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)1 ,(lte)h7,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)g6,(lte)(K|R|Q)   ,(lte)g5,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)1 ,(lte)g8,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)f6,(lte)(K|P|B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
    ,(lte)1 ,(lte)f8,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)h8,(lte)(K|B|Q)   
    ,(lte)1 ,(lte)h6,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_g8[] =
{
(lte)5
    ,(lte)6 ,(lte)f8,(lte)(K|R|Q)   ,(lte)e8,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)1 ,(lte)h8,(lte)(K|R|Q)   
    ,(lte)7 ,(lte)g7,(lte)(K|R|Q)   ,(lte)g6,(lte)(R|Q) ,(lte)g5,(lte)(R|Q) ,(lte)g4,(lte)(R|Q) ,(lte)g3,(lte)(R|Q) ,(lte)g2,(lte)(R|Q) ,(lte)g1,(lte)(R|Q) 
    ,(lte)6 ,(lte)f7,(lte)(K|P|B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c4,(lte)(B|Q) ,(lte)b3,(lte)(B|Q) ,(lte)a2,(lte)(B|Q) 
    ,(lte)1 ,(lte)h7,(lte)(K|P|B|Q) 
};
static const lte attacks_black_lookup_h1[] =
{
(lte)3
    ,(lte)7 ,(lte)g1,(lte)(K|R|Q)   ,(lte)f1,(lte)(R|Q) ,(lte)e1,(lte)(R|Q) ,(lte)d1,(lte)(R|Q) ,(lte)c1,(lte)(R|Q) ,(lte)b1,(lte)(R|Q) ,(lte)a1,(lte)(R|Q) 
    ,(lte)7 ,(lte)h2,(lte)(K|R|Q)   ,(lte)h3,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)7 ,(lte)g2,(lte)(K|B|Q)   ,(lte)f3,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)d5,(lte)(B|Q) ,(lte)c6,(lte)(B|Q) ,(lte)b7,(lte)(B|Q) ,(lte)a8,(lte)(B|Q) 
};
static const lte attacks_black_lookup_h2[] =
{
(lte)5
    ,(lte)7 ,(lte)g2,(lte)(K|R|Q)   ,(lte)f2,(lte)(R|Q) ,(lte)e2,(lte)(R|Q) ,(lte)d2,(lte)(R|Q) ,(lte)c2,(lte)(R|Q) ,(lte)b2,(lte)(R|Q) ,(lte)a2,(lte)(R|Q) 
    ,(lte)1 ,(lte)h1,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)h3,(lte)(K|R|Q)   ,(lte)h4,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)1 ,(lte)g1,(lte)(K|P|B|Q) 
    ,(lte)6 ,(lte)g3,(lte)(K|B|Q)   ,(lte)f4,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)d6,(lte)(B|Q) ,(lte)c7,(lte)(B|Q) ,(lte)b8,(lte)(B|Q) 
};
static const lte attacks_black_lookup_h3[] =
{
(lte)5
    ,(lte)7 ,(lte)g3,(lte)(K|R|Q)   ,(lte)f3,(lte)(R|Q) ,(lte)e3,(lte)(R|Q) ,(lte)d3,(lte)(R|Q) ,(lte)c3,(lte)(R|Q) ,(lte)b3,(lte)(R|Q) ,(lte)a3,(lte)(R|Q) 
    ,(lte)2 ,(lte)h2,(lte)(K|R|Q)   ,(lte)h1,(lte)(R|Q) 
    ,(lte)5 ,(lte)h4,(lte)(K|R|Q)   ,(lte)h5,(lte)(R|Q) ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)2 ,(lte)g2,(lte)(K|P|B|Q) ,(lte)f1,(lte)(B|Q) 
    ,(lte)5 ,(lte)g4,(lte)(K|B|Q)   ,(lte)f5,(lte)(B|Q) ,(lte)e6,(lte)(B|Q) ,(lte)d7,(lte)(B|Q) ,(lte)c8,(lte)(B|Q) 
};
static const lte attacks_black_lookup_h4[] =
{
(lte)5
    ,(lte)7 ,(lte)g4,(lte)(K|R|Q)   ,(lte)f4,(lte)(R|Q) ,(lte)e4,(lte)(R|Q) ,(lte)d4,(lte)(R|Q) ,(lte)c4,(lte)(R|Q) ,(lte)b4,(lte)(R|Q) ,(lte)a4,(lte)(R|Q) 
    ,(lte)3 ,(lte)h3,(lte)(K|R|Q)   ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)4 ,(lte)h5,(lte)(K|R|Q)   ,(lte)h6,(lte)(R|Q) ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)3 ,(lte)g3,(lte)(K|P|B|Q) ,(lte)f2,(lte)(B|Q) ,(lte)e1,(lte)(B|Q) 
    ,(lte)4 ,(lte)g5,(lte)(K|B|Q)   ,(lte)f6,(lte)(B|Q) ,(lte)e7,(lte)(B|Q) ,(lte)d8,(lte)(B|Q) 
};
static const lte attacks_black_lookup_h5[] =
{
(lte)5
    ,(lte)7 ,(lte)g5,(lte)(K|R|Q)   ,(lte)f5,(lte)(R|Q) ,(lte)e5,(lte)(R|Q) ,(lte)d5,(lte)(R|Q) ,(lte)c5,(lte)(R|Q) ,(lte)b5,(lte)(R|Q) ,(lte)a5,(lte)(R|Q) 
    ,(lte)4 ,(lte)h4,(lte)(K|R|Q)   ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)3 ,(lte)h6,(lte)(K|R|Q)   ,(lte)h7,(lte)(R|Q) ,(lte)h8,(lte)(R|Q) 
    ,(lte)4 ,(lte)g4,(lte)(K|P|B|Q) ,(lte)f3,(lte)(B|Q) ,(lte)e2,(lte)(B|Q) ,(lte)d1,(lte)(B|Q) 
    ,(lte)3 ,(lte)g6,(lte)(K|B|Q)   ,(lte)f7,(lte)(B|Q) ,(lte)e8,(lte)(B|Q) 
};
static const lte attacks_black_lookup_h6[] =
{
(lte)5
    ,(lte)7 ,(lte)g6,(lte)(K|R|Q)   ,(lte)f6,(lte)(R|Q) ,(lte)e6,(lte)(R|Q) ,(lte)d6,(lte)(R|Q) ,(lte)c6,(lte)(R|Q) ,(lte)b6,(lte)(R|Q) ,(lte)a6,(lte)(R|Q) 
    ,(lte)5 ,(lte)h5,(lte)(K|R|Q)   ,(lte)h4,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)2 ,(lte)h7,(lte)(K|R|Q)   ,(lte)h8,(lte)(R|Q) 
    ,(lte)5 ,(lte)g5,(lte)(K|P|B|Q) ,(lte)f4,(lte)(B|Q) ,(lte)e3,(lte)(B|Q) ,(lte)d2,(lte)(B|Q) ,(lte)c1,(lte)(B|Q) 
    ,(lte)2 ,(lte)g7,(lte)(K|B|Q)   ,(lte)f8,(lte)(B|Q) 
};
static const lte attacks_black_lookup_h7[] =
{
(lte)5
    ,(lte)7 ,(lte)g7,(lte)(K|R|Q)   ,(lte)f7,(lte)(R|Q) ,(lte)e7,(lte)(R|Q) ,(lte)d7,(lte)(R|Q) ,(lte)c7,(lte)(R|Q) ,(lte)b7,(lte)(R|Q) ,(lte)a7,(lte)(R|Q) 
    ,(lte)6 ,(lte)h6,(lte)(K|R|Q)   ,(lte)h5,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)1 ,(lte)h8,(lte)(K|R|Q)   
    ,(lte)6 ,(lte)g6,(lte)(K|P|B|Q) ,(lte)f5,(lte)(B|Q) ,(lte)e4,(lte)(B|Q) ,(lte)d3,(lte)(B|Q) ,(lte)c2,(lte)(B|Q) ,(lte)b1,(lte)(B|Q) 
    ,(lte)1 ,(lte)g8,(lte)(K|B|Q)   
};
static const lte attacks_black_lookup_h8[] =
{
(lte)3
    ,(lte)7 ,(lte)g8,(lte)(K|R|Q)   ,(lte)f8,(lte)(R|Q) ,(lte)e8,(lte)(R|Q) ,(lte)d8,(lte)(R|Q) ,(lte)c8,(lte)(R|Q) ,(lte)b8,(lte)(R|Q) ,(lte)a8,(lte)(R|Q) 
    ,(lte)7 ,(lte)h7,(lte)(K|R|Q)   ,(lte)h6,(lte)(R|Q) ,(lte)h5,(lte)(R|Q) ,(lte)h4,(lte)(R|Q) ,(lte)h3,(lte)(R|Q) ,(lte)h2,(lte)(R|Q) ,(lte)h1,(lte)(R|Q) 
    ,(lte)7 ,(lte)g7,(lte)(K|P|B|Q) ,(lte)f6,(lte)(B|Q) ,(lte)e5,(lte)(B|Q) ,(lte)d4,(lte)(B|Q) ,(lte)c3,(lte)(B|Q) ,(lte)b2,(lte)(B|Q) ,(lte)a1,(lte)(B|Q) 
};

// attacks_black_lookup
const lte *attacks_black_lookup[] =
{
    attacks_black_lookup_a8,
    attacks_black_lookup_b8,
    attacks_black_lookup_c8,
    attacks_black_lookup_d8,
    attacks_black_lookup_e8,
    attacks_black_lookup_f8,
    attacks_black_lookup_g8,
    attacks_black_lookup_h8,
    attacks_black_lookup_a7,
    attacks_black_lookup_b7,
    attacks_black_lookup_c7,
    attacks_black_lookup_d7,
    attacks_black_lookup_e7,
    attacks_black_lookup_f7,
    attacks_black_lookup_g7,
    attacks_black_lookup_h7,
    attacks_black_lookup_a6,
    attacks_black_lookup_b6,
    attacks_black_lookup_c6,
    attacks_black_lookup_d6,
    attacks_black_lookup_e6,
    attacks_black_lookup_f6,
    attacks_black_lookup_g6,
    attacks_black_lookup_h6,
    attacks_black_lookup_a5,
    attacks_black_lookup_b5,
    attacks_black_lookup_c5,
    attacks_black_lookup_d5,
    attacks_black_lookup_e5,
    attacks_black_lookup_f5,
    attacks_black_lookup_g5,
    attacks_black_lookup_h5,
    attacks_black_lookup_a4,
    attacks_black_lookup_b4,
    attacks_black_lookup_c4,
    attacks_black_lookup_d4,
    attacks_black_lookup_e4,
    attacks_black_lookup_f4,
    attacks_black_lookup_g4,
    attacks_black_lookup_h4,
    attacks_black_lookup_a3,
    attacks_black_lookup_b3,
    attacks_black_lookup_c3,
    attacks_black_lookup_d3,
    attacks_black_lookup_e3,
    attacks_black_lookup_f3,
    attacks_black_lookup_g3,
    attacks_black_lookup_h3,
    attacks_black_lookup_a2,
    attacks_black_lookup_b2,
    attacks_black_lookup_c2,
    attacks_black_lookup_d2,
    attacks_black_lookup_e2,
    attacks_black_lookup_f2,
    attacks_black_lookup_g2,
    attacks_black_lookup_h2,
    attacks_black_lookup_a1,
    attacks_black_lookup_b1,
    attacks_black_lookup_c1,
    attacks_black_lookup_d1,
    attacks_black_lookup_e1,
    attacks_black_lookup_f1,
    attacks_black_lookup_g1,
    attacks_black_lookup_h1
};
