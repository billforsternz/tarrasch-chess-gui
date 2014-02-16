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

