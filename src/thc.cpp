/****************************************************************************
 * Triple Happy Chess library = thc library
 *  This is thc rendered as a single thc.h header + thc.cpp source file to
 *  avoid the complications of libraries - Inspired by sqlite.c
 *
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

/*
    thc.cpp The basic idea is to concatenate the following into one .cpp file;

        Portability.cpp
        PrivateChessDefs.h
        HashLookup.h
        ChessPosition.cpp
        ChessRules.cpp
        ChessEvaluation.cpp
        Move.cpp
        PrivateChessDefs.cpp
         nested inline expansion of -> GeneratedLookupTables.h
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <algorithm>
#include "thc.h"
using namespace std;
using namespace thc;
/****************************************************************************
 * Portability.cpp Simple definitions to aid platform portability
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

// return 0 if case insensitive match
int strcmp_ignore( const char *s, const char *t )
{
    bool same=true;
    while( *s && *t && same )
    {
        char c = *s++;
        char d = *t++;
        same = (c==d) || (isascii(c) && isascii(d) && toupper(c)==toupper(d));
    }
    if( *s || *t )
        same = false;
    return same?0:1;
}

/****************************************************************************
 * PrivateChessDefs.h Chess classes - Internal implementation details
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PRIVATE_CHESS_DEFS_H_INCLUDED
#define PRIVATE_CHESS_DEFS_H_INCLUDED

// TripleHappyChess
namespace thc
{

// Check whether a piece is black, white or an empty square, should really make
//  these and most other macros into inline functions
#define IsEmptySquare(p) ((p)==' ')
#define IsBlack(p) ((p)>'a')              // all lower case pieces
#define IsWhite(p) ((p)<'a' && (p)!=' ')  // all upper case pieces, and not empty

// Allow easy iteration through squares
inline Square& operator++ ( Square& sq )
{
    sq = (Square)(sq+1);
    return sq;
}

// Macro to convert chess notation to Square convention,   
//  eg SQ('c','5') -> c5
//  (We didn't always have such a sensible Square convention. SQ() remains
//  useful for cases like SQ(file,rank), but you may actually see examples
//  like the hardwired SQ('c','5') which can safely be changed to simply
//  c5).
#define SQ(f,r)  ( (Square) ( ('8'-(r))*8 + ((f)-'a') )   )

// More Square macros
#define FILE(sq)    ( (char) (  ((sq)&0x07) + 'a' ) )           // eg c5->'c'
#define RANK(sq)    ( (char) (  '8' - (((sq)>>3) & 0x07) ) )    // eg c5->'5'
#define IFILE(sq)   (  (int)(sq) & 0x07 )                       // eg c5->2
#define IRANK(sq)   (  7 - ((((int)(sq)) >>3) & 0x07) )         // eg c5->4
#define SOUTH(sq)   (  (Square)((sq) + 8) )                     // eg c5->c4
#define NORTH(sq)   (  (Square)((sq) - 8) )                     // eg c5->c6
#define SW(sq)      (  (Square)((sq) + 7) )                     // eg c5->b4
#define SE(sq)      (  (Square)((sq) + 9) )                     // eg c5->d4
#define NW(sq)      (  (Square)((sq) - 9) )                     // eg c5->b6
#define NE(sq)      (  (Square)((sq) - 7) )                     // eg c5->d6

// Utility macro
#ifndef nbrof
    #define nbrof(array) (sizeof((array))/sizeof((array)[0]))
#endif

/* DETAIL is shorthand for the section of type ChessPosition that looks
   like this;

    Square enpassant_target : 8;
    Square wking_square     : 8;
    Square bking_square     : 8;
    int  wking              : 1;
    int  wqueen             : 1;
    int  bking              : 1;
    int  bqueen             : 1;

  We assume it is located in the last 4 bytes of ChessPosition,
  hence the definition of typedef DETAIL as unsigned long, and
  of DETAIL_ADDR below. We assume that ANDing the unsigned
  character at this address + 3, with ~WKING, where WKING
  is defined as unsigned char 0x01, will clear wking. See the
  definition of DETAIL_CASTLING and castling_prohibited_table[].
  These assumptions are likely not portable and are tested in
  TestInternals(). If porting this code, step through that code
  first and make any adjustments necessary */

#define DETAIL_ADDR         ( (DETAIL*) ((char *)this + sizeof(ChessPosition) - sizeof(DETAIL))  )
#define DETAIL_SAVE         DETAIL tmp = *DETAIL_ADDR
#define DETAIL_RESTORE      *DETAIL_ADDR = tmp
#define DETAIL_EQ_ALL               ( (*DETAIL_ADDR&0x0fffffff) == (tmp&0x0fffffff) )
#define DETAIL_EQ_CASTLING          ( (*DETAIL_ADDR&0x0f000000) == (tmp&0x0f000000) )
#define DETAIL_EQ_KING_POSITIONS    ( (*DETAIL_ADDR&0x00ffff00) == (tmp&0x00ffff00) )
#define DETAIL_EQ_EN_PASSANT        ( (*DETAIL_ADDR&0x000000ff) == (tmp&0x000000ff) )
#define DETAIL_PUSH         detail_stack[detail_idx++] = *DETAIL_ADDR
#define DETAIL_POP          *DETAIL_ADDR = detail_stack[--detail_idx]
#define DETAIL_CASTLING(sq) *( 3 + (unsigned char*)DETAIL_ADDR ) &= castling_prohibited_table[sq]

// Bits corresponding to detail bits wking, wqueen, bking, bqueen for
//  DETAIL_CASTLING
#define WKING   0x01    
#define WQUEEN  0x02    
#define BKING   0x04    
#define BQUEEN  0x08


// Convert piece, eg 'N' to bitmask in lookup tables. See automatically
//  PrivateChessDefs.cpp and GeneratedLookupTables.h for format of
//  lookup tables
extern lte to_mask[];

// Lookup squares a queen can move to
extern const lte *queen_lookup[];

// Lookup squares a rook can move to
extern const lte *rook_lookup[];

// Lookup squares a bishop can move to
extern const lte *bishop_lookup[];

// Lookup squares a knight can move to
extern const lte *knight_lookup[];

// Lookup squares a king can move to
extern const lte *king_lookup[];

// Lookup squares a white pawn can move to
extern const lte *pawn_white_lookup[];

// Lookup squares a black pawn can move to
extern const lte *pawn_black_lookup[];

// Lookup good squares for enemy king when a king is on a square in an endgame
extern const lte *good_king_position_lookup[];

// Lookup squares from which an enemy pawn attacks white
extern const lte *pawn_attacks_white_lookup[];

// Lookup squares from which an enemy pawn attacks black
extern const lte *pawn_attacks_black_lookup[];

// Lookup squares from which enemy pieces attack white
extern const lte *attacks_white_lookup[];

// Lookup squares from which enemy pieces attack black
extern const lte *attacks_black_lookup[];

} //namespace thc

#endif // PRIVATE_CHESS_DEFS_H_INCLUDED
/****************************************************************************
 * HashLookup.h Quickly generate position hash codes with these lookup tables
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
static uint32_t hash_lookup[64]['r'-'B'+1] =
{  // B                          K              N            P          Q          R
    { 0x8c7f0aac,0,0,0,0,0,0,0,0,0x97c4aa2f,0,0,0xb716a675,0,0xd821ccc0,0x9a4eb343,0xdba252fb, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x8b7d76c3,             // 'S'->'a', where 'a' is a proxy for ' ' or '.'
      0xd8e57d67,0,0,0,0,0,0,0,0,0x6c74a409,0,0,0x9fa1ded3,0,0xa5595115,0x6266d6f2,0x7005b724  // 'b'->'r'
    },
    { 0x4c2b3a57,0,0,0,0,0,0,0,0,0xe44b3c46,0,0,0x0e84bdd8,0,0xf6b29a58,0x45cccd8c,0x6229393a, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x7a4842c1,
      0xcaae7de6,0,0,0,0,0,0,0,0,0xcfea4a27,0,0,0x8765a857,0,0x7adfc8ae,0x916b5e58,0x648d8b51  // 'b'->'r'
    },
    { 0xecf3e6a5,0,0,0,0,0,0,0,0,0xd6094219,0,0,0x122f6b4d,0,0x565f9848,0x164e1b09,0xa5ee9794, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x052d0873,
      0x5e4513d0,0,0,0,0,0,0,0,0,0xd52692f3,0,0,0xf5081ec5,0,0xc73547fe,0x23ee074f,0xdeb91daf  // 'b'->'r'
    },
    { 0xdebe09c0,0,0,0,0,0,0,0,0,0xfa86bb52,0,0,0x793e6063,0,0xcc95a7d8,0xcd087cb1,0x762382f3, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x853e031d,
      0xc7d0c293,0,0,0,0,0,0,0,0,0xadcb0c93,0,0,0x1e473b8e,0,0xb87b61a7,0xa3d1dd20,0x94ff3fc1  // 'b'->'r'
    },
    { 0x24b2cd09,0,0,0,0,0,0,0,0,0x89914ab9,0,0,0xf1d5d27f,0,0xc234a220,0x8597da1f,0x1b1cc2ca, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x6a2748f4,
      0x793de097,0,0,0,0,0,0,0,0,0x43b9eaa3,0,0,0x2fb379fe,0,0xc6342dcb,0xbca6ab72,0x74c644b7  // 'b'->'r'
    },
    { 0x376fd81c,0,0,0,0,0,0,0,0,0x9184e322,0,0,0x229da880,0,0x04cf6880,0x52fae7a4,0x9e1d5c35, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x26511785,
      0x9cb24e26,0,0,0,0,0,0,0,0,0x38ea0de8,0,0,0x9def62f4,0,0x62f0f111,0xf199794f,0xe710b184  // 'b'->'r'
    },
    { 0xae8bc669,0,0,0,0,0,0,0,0,0x732fec2a,0,0,0x5c08b5ba,0,0x9cf1ba1f,0x6fe15378,0xe7005101, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xb297f541,
      0x196a6fe7,0,0,0,0,0,0,0,0,0x0f6aefa9,0,0,0xf8456839,0,0xaab13923,0xa7342f66,0xabaeec77  // 'b'->'r'
    },
    { 0x2bc0bb0b,0,0,0,0,0,0,0,0,0x35dba1ae,0,0,0x5bafdc52,0,0x2101505b,0xc02cf780,0x50bfe98e, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x9b9aca63,
      0x5d1c2635,0,0,0,0,0,0,0,0,0x53364b8c,0,0,0x91f86a79,0,0x09d63faa,0x70483054,0xa25fc8cb  // 'b'->'r'
    },
    { 0xfd061144,0,0,0,0,0,0,0,0,0xf57db306,0,0,0x1a1f9bc4,0,0xa71d442f,0x3578f27f,0xa29337f4, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x294b9483,
      0xfecbf3cc,0,0,0,0,0,0,0,0,0xa7321b64,0,0,0x94f424b4,0,0x40d7b7e8,0x6a140f4e,0x7760248f  // 'b'->'r'
    },
    { 0x7985c694,0,0,0,0,0,0,0,0,0x3e92ace3,0,0,0x9f9e5bba,0,0x28b23b17,0x5687aacf,0x1c418b8d, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xacbc9175,
      0xa8053755,0,0,0,0,0,0,0,0,0x51342230,0,0,0x235ff531,0,0xc741a645,0x325338a9,0xf31716a3  // 'b'->'r'
    },
    { 0x5e64c5c0,0,0,0,0,0,0,0,0,0xa99b5c5f,0,0,0xd22c9cc5,0,0x03796e5e,0x18dba100,0x9f72d771, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xd6838eb2,
      0xac74f524,0,0,0,0,0,0,0,0,0x1899e7a2,0,0,0xf8d16330,0,0xf9f93f5d,0xe0d14983,0x77f98662  // 'b'->'r'
    },
    { 0x8276be2a,0,0,0,0,0,0,0,0,0xfa0d03cd,0,0,0x0e435170,0,0x9ad727e7,0x737f2b95,0xbd4060c9, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x051de97f,
      0x0a083600,0,0,0,0,0,0,0,0,0x7113f78a,0,0,0x48660972,0,0xfac6322b,0x1ec533ba,0x5c048d7f  // 'b'->'r'
    },
    { 0x4bcfd817,0,0,0,0,0,0,0,0,0x7b1bd6bb,0,0,0x1e64f082,0,0xb04c1979,0x51675862,0xe166de3e, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x6a0d23a3,
      0xeb117ade,0,0,0,0,0,0,0,0,0x106bf87b,0,0,0x3781a7c3,0,0xb145da52,0x90b037ae,0x910ccae3  // 'b'->'r'
    },
    { 0xdd775c94,0,0,0,0,0,0,0,0,0x43f090d1,0,0,0x824bca32,0,0x85f3959b,0xeaae5b0e,0x180c7c29, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xebd0fc3a,
      0x93713ac1,0,0,0,0,0,0,0,0,0x1546dc24,0,0,0xede65b0a,0,0x47189056,0x518dbc2b,0x02653368  // 'b'->'r'
    },
    { 0xaadb680b,0,0,0,0,0,0,0,0,0xd7a3bb02,0,0,0x21bd8133,0,0xa5ad3450,0xb7613820,0xd76514b6, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x4a168480,
      0x43c55b26,0,0,0,0,0,0,0,0,0x2ee5a113,0,0,0x65d794ae,0,0x9625b62a,0x8d85b573,0x0525c4b8  // 'b'->'r'
    },
    { 0x2a3989bc,0,0,0,0,0,0,0,0,0xd43569e8,0,0,0x5eabbe4d,0,0x0133b91e,0x257d3518,0xad85627d, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x91d28302,
      0x451f3e03,0,0,0,0,0,0,0,0,0xb428205e,0,0,0xbc35ace2,0,0x49d9976b,0xf651fd0d,0x6eebf770  // 'b'->'r'
    },
    { 0x3fae4928,0,0,0,0,0,0,0,0,0xc1903548,0,0,0x937f0c13,0,0x6566b25f,0x97900f48,0xe562c59a, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x927f19c2,
      0xa39054f8,0,0,0,0,0,0,0,0,0x391be0b4,0,0,0xe43ce943,0,0xf3e75bec,0xae181f3d,0x7276cf0e  // 'b'->'r'
    },
    { 0x72fe9f60,0,0,0,0,0,0,0,0,0xd8ae3d04,0,0,0xfa839fc3,0,0xb31112ed,0x1dbf688b,0x4c24d3fc, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc45baa56,
      0xd0550dcd,0,0,0,0,0,0,0,0,0x696d0b79,0,0,0x6581666d,0,0xace9934b,0xe18ffab8,0x3ff2a610  // 'b'->'r'
    },
    { 0x94ce4c98,0,0,0,0,0,0,0,0,0x502f139d,0,0,0xe1b96895,0,0xf725846e,0xb149c019,0x96a5a5d0, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xb9aa43bc,
      0xa8e00779,0,0,0,0,0,0,0,0,0x8056cb76,0,0,0x88803475,0,0xf4c1e5bd,0x3b043653,0xa4dc8aa1  // 'b'->'r'
    },
    { 0x65162768,0,0,0,0,0,0,0,0,0x6c81c3a0,0,0,0x9e6a3ce4,0,0x9b3c95fb,0x7990eafb,0x04e9d879, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x785a9546,
      0x4d3401d5,0,0,0,0,0,0,0,0,0xb750a91f,0,0,0xa901220d,0,0x49b9c747,0x4a4286b8,0x622a9498  // 'b'->'r'
    },
    { 0x9e36424f,0,0,0,0,0,0,0,0,0xbfc99829,0,0,0x6dc3c912,0,0xe0e23e28,0x22ae6db6,0x1a5540cf, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x4c5c3b0b,
      0x17a5d0a6,0,0,0,0,0,0,0,0,0x91e9386f,0,0,0x5aa2cd5d,0,0x97436ff9,0x8d43d481,0x9306fadf  // 'b'->'r'
    },
    { 0x089ba776,0,0,0,0,0,0,0,0,0xa7382b2c,0,0,0xf80de0d8,0,0xa6f03d7d,0x522ce018,0x6e717043, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x38a4abd2,
      0xe58413ef,0,0,0,0,0,0,0,0,0x2429df03,0,0,0x5e1888ea,0,0x18e606cc,0x6f94d7e6,0xfbea3123  // 'b'->'r'
    },
    { 0xe45516d6,0,0,0,0,0,0,0,0,0x42a5b3fe,0,0,0xce62babd,0,0x897a4ec5,0xb4320ad7,0x72ab4a2b, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x19a87820,
      0x197d5c0b,0,0,0,0,0,0,0,0,0xeb633668,0,0,0x5a3118d4,0,0xb6d8848a,0x7820b6b6,0xffb46feb  // 'b'->'r'
    },
    { 0xd754f5a5,0,0,0,0,0,0,0,0,0x26423e7d,0,0,0xe796fe9c,0,0xde3d826f,0x099d7de8,0x29992302, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x8220f61b,
      0x9d954fd3,0,0,0,0,0,0,0,0,0x2ab684d9,0,0,0x1fb2aa97,0,0xc76fe335,0xd9171133,0xdd6c44ae  // 'b'->'r'
    },
    { 0xceac7494,0,0,0,0,0,0,0,0,0x69514bb5,0,0,0x91b0961d,0,0x23d53e43,0x683d2a23,0x08814327, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x11b4ed89,
      0xfb8a0849,0,0,0,0,0,0,0,0,0xb28ab129,0,0,0x5f8ffb97,0,0x741b5f83,0x6b8a0f2e,0xb8d8a2da  // 'b'->'r'
    },
    { 0x0cf357b2,0,0,0,0,0,0,0,0,0xddcb3b6c,0,0,0x5d912703,0,0xf9bbc71f,0x0441bb09,0xdb15ed8a, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3b11ee1b,
      0x02ffb1ad,0,0,0,0,0,0,0,0,0xc3d140c7,0,0,0x5c2785a7,0,0xf1b2143d,0xbae0a955,0xbffff361  // 'b'->'r'
    },
    { 0x2befec2c,0,0,0,0,0,0,0,0,0x56e32b22,0,0,0x8562a7a2,0,0x7d531458,0x0de91821,0x56c7ba85, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3332f8e8,
      0x2df312ff,0,0,0,0,0,0,0,0,0x04bdd824,0,0,0x2bc5c700,0,0xcb2fc5cb,0x76a4b922,0x395320c5  // 'b'->'r'
    },
    { 0xdfe4037e,0,0,0,0,0,0,0,0,0x5868f7b5,0,0,0xf1b1d4fe,0,0xed96bc50,0x9bb675be,0xb4548088, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x98be68bd,
      0x08269881,0,0,0,0,0,0,0,0,0xc89ce8d1,0,0,0x2a296570,0,0x8001b923,0x9f193578,0x0ce50d5b  // 'b'->'r'
    },
    { 0x93c540a8,0,0,0,0,0,0,0,0,0xb2f81774,0,0,0x3ce68b24,0,0xfe0db0b0,0xef28a619,0x446b5143, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x9d2cdf67,
      0xadd8e1fc,0,0,0,0,0,0,0,0,0x891f3b23,0,0,0xdd418c72,0,0x9704571e,0xc037541d,0xbae946f1  // 'b'->'r'
    },
    { 0xf6e8cd21,0,0,0,0,0,0,0,0,0x4fdba092,0,0,0x8de2d511,0,0x65f1d0dd,0x365f3954,0x35b851fd, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x38f20a02,
      0x2faa5845,0,0,0,0,0,0,0,0,0x37fff565,0,0,0xf1c2638c,0,0x91cf922c,0xbd533375,0x73bd6afd  // 'b'->'r'
    },
    { 0x7d8eb542,0,0,0,0,0,0,0,0,0xf8616e6f,0,0,0x3a37d85b,0,0xae382d55,0x411d81a7,0x15d5ee27, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x0edaffcb,
      0x0e716e96,0,0,0,0,0,0,0,0,0x6f35ed9e,0,0,0x7ce2ee91,0,0x4fd1dac6,0xe18983c7,0xb2439112  // 'b'->'r'
    },
    { 0xf9f5a35c,0,0,0,0,0,0,0,0,0x60b4582b,0,0,0x9e1ed453,0,0x2dfa81b1,0x8ae13329,0x0651585d, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xdac7f4ae,
      0x11374595,0,0,0,0,0,0,0,0,0xbe6bf0c9,0,0,0xadecaf59,0,0x7a8549f2,0x742579e0,0xad5537db  // 'b'->'r'
    },
    { 0x895d4149,0,0,0,0,0,0,0,0,0x9b674e1c,0,0,0xe58c3feb,0,0xb6f660d1,0xfd86da69,0x7830f7ba, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x37868f80,
      0x74bd5fd6,0,0,0,0,0,0,0,0,0xa9bf7e3f,0,0,0xe80b0410,0,0x4369186a,0x2320e0a4,0x0549625e  // 'b'->'r'
    },
    { 0x3aae1e18,0,0,0,0,0,0,0,0,0xc2251a74,0,0,0xe1af94bf,0,0x51eca4c3,0xe7886533,0x622ab088, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xa55223b8,
      0x969bf35b,0,0,0,0,0,0,0,0,0x531e6c5d,0,0,0xd4bf977b,0,0x850bcaee,0xa104f457,0x0003a0a0  // 'b'->'r'
    },
    { 0xdf660893,0,0,0,0,0,0,0,0,0x4fd61248,0,0,0x4606d9c7,0,0x6cea6457,0xcc4ccc0d,0xe2a57d3a, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x2f85d651,
      0xae0c9478,0,0,0,0,0,0,0,0,0xf3ea2774,0,0,0x74c4ebb7,0,0xafff3b40,0x7bc0aacb,0x372b82dc  // 'b'->'r'
    },
    { 0xc9ead3a4,0,0,0,0,0,0,0,0,0xf286e119,0,0,0x3abcb320,0,0xbb195daa,0xe15b2f0e,0x410251d6, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x504e251c,
      0x369b9d14,0,0,0,0,0,0,0,0,0xf51b7fd2,0,0,0x84a8cd44,0,0x78c4b616,0x0691d4e3,0xb62a5b7a  // 'b'->'r'
    },
    { 0x351cc253,0,0,0,0,0,0,0,0,0x27588287,0,0,0x6cb82fc8,0,0xbafe423d,0x5fc99a8d,0xa5719605, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x76ace100,
      0x37026c88,0,0,0,0,0,0,0,0,0x4712accf,0,0,0x2fbbb9cf,0,0x96377fb5,0xcebd948b,0xdd25a404  // 'b'->'r'
    },
    { 0xbf4099a7,0,0,0,0,0,0,0,0,0x1e16915c,0,0,0xacc2cbad,0,0x8472f51a,0x46e2824a,0x21cf3734, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x2cc6d3ee,
      0xb7841db1,0,0,0,0,0,0,0,0,0xb4586cdb,0,0,0x65642b33,0,0x769102e3,0x90bf7369,0xd7265312  // 'b'->'r'
    },
    { 0x2eeb6d75,0,0,0,0,0,0,0,0,0x34721522,0,0,0x2514be33,0,0x2a3abe9e,0x7cf141b5,0x1ff50f3a, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x5b096fab,
      0xb8da4737,0,0,0,0,0,0,0,0,0xf0c025fc,0,0,0x07cbc3fc,0,0xc3ec5b12,0xbf3b03ad,0xbfa86b57  // 'b'->'r'
    },
    { 0x17b461c1,0,0,0,0,0,0,0,0,0xe75a2d46,0,0,0x37aad5ea,0,0x155b2c35,0xbfcf2330,0x8d5c7c5e, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xbb50483b,
      0x95a03950,0,0,0,0,0,0,0,0,0x0bad669a,0,0,0xf641767c,0,0x358b50a3,0x4aca2e3a,0x497343b1  // 'b'->'r'
    },
    { 0x3da6f46a,0,0,0,0,0,0,0,0,0xad6120c9,0,0,0x19acdd2c,0,0x1023470d,0x0434bb79,0x8e3f0746, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xedf5a226,
      0x025d8ea7,0,0,0,0,0,0,0,0,0xab7fa688,0,0,0xd541fc0d,0,0xc8ffc7f8,0xfbfd0387,0x481f76d0  // 'b'->'r'
    },
    { 0xb4183bf8,0,0,0,0,0,0,0,0,0x961efa16,0,0,0x2e7f61f8,0,0x105f5f4f,0x832c37d9,0x7c521708, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x94982ee3,
      0xfa3d1f06,0,0,0,0,0,0,0,0,0xc99c5cd1,0,0,0xe062a5c7,0,0x9b41f9d4,0x569195d9,0x37e93fc2  // 'b'->'r'
    },
    { 0xf629763c,0,0,0,0,0,0,0,0,0x7485f190,0,0,0x3b50cc38,0,0xe0fd9b72,0xf3068eed,0x7e054a97, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf0fe2118,
      0xb72f0404,0,0,0,0,0,0,0,0,0xcc988a64,0,0,0x7c74f3ec,0,0xa1650931,0xb5636957,0xdfd1561e  // 'b'->'r'
    },
    { 0x7f861e36,0,0,0,0,0,0,0,0,0x4b036099,0,0,0xd8346f14,0,0xd9545d61,0x31c06965,0x9e2d2ab9, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc5f8b197,
      0x03637d9b,0,0,0,0,0,0,0,0,0xf969041d,0,0,0x58e44ba1,0,0xdcc05573,0x25ec8f35,0xc7ca0a77  // 'b'->'r'
    },
    { 0xfb592bb3,0,0,0,0,0,0,0,0,0xfc2b1356,0,0,0x7a7679f6,0,0xc0e9f007,0x7f550a69,0x01094bf1, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xa3b47889,
      0x44fc9ab6,0,0,0,0,0,0,0,0,0x5e5b8f80,0,0,0x69160353,0,0x230be578,0x6da013a4,0xd2764ed1  // 'b'->'r'
    },
    { 0x4c3f5c94,0,0,0,0,0,0,0,0,0x3099df75,0,0,0x66b09bf0,0,0x82e5cd03,0x1ee3607e,0x396cd72a, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xfb0f2241,
      0x190c5614,0,0,0,0,0,0,0,0,0x67f78324,0,0,0xdcb89544,0,0x91b7cbd0,0xf9114070,0x57f687af  // 'b'->'r'
    },
    { 0xf5f9428a,0,0,0,0,0,0,0,0,0xc9f390ed,0,0,0xe8140568,0,0x694fb3de,0xc627f75b,0x5bf9362b, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x5549003f,
      0x66458f9f,0,0,0,0,0,0,0,0,0x14c30f94,0,0,0x4d44c9c6,0,0x6840f509,0xc674cdbc,0x3b73b25b  // 'b'->'r'
    },
    { 0xed1c4a6f,0,0,0,0,0,0,0,0,0x21eab5a3,0,0,0x53478953,0,0x0dad674c,0xf3ef5512,0xb9c08d71, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x03921f4a,
      0x02ece8e2,0,0,0,0,0,0,0,0,0x889134e1,0,0,0xc544c7ab,0,0x4df91683,0x259e4b8c,0xe2031ce4  // 'b'->'r'
    },
    { 0x145b8f3a,0,0,0,0,0,0,0,0,0x4028cf81,0,0,0x16f03971,0,0xad6adc80,0xac0b5327,0xcf77f418, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3ed062ba,
      0x6ea14124,0,0,0,0,0,0,0,0,0x6ba87963,0,0,0xc08be345,0,0x8eafb886,0xd460d003,0xdc4d14e2  // 'b'->'r'
    },
    { 0x61085b79,0,0,0,0,0,0,0,0,0xba1f92a8,0,0,0x18b779bc,0,0x453435a1,0x41925d1c,0x21a8db44, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x9789101a,
      0x0e2d02e0,0,0,0,0,0,0,0,0,0x79fa68f8,0,0,0x4d35916d,0,0x7ce947b3,0x431a2cc9,0x756135b5  // 'b'->'r'
    },
    { 0x74c5a0c5,0,0,0,0,0,0,0,0,0x864bb3a1,0,0,0xaeeb8687,0,0x7127ea7d,0xb214825e,0xda464848, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x4894b0f6,
      0x6ef5db54,0,0,0,0,0,0,0,0,0x6142e487,0,0,0xd3adc6c3,0,0x2e5fe8d5,0x82643ddb,0xc9de1e6c  // 'b'->'r'
    },
    { 0x161ccd43,0,0,0,0,0,0,0,0,0x0e8d9866,0,0,0xa8f85f54,0,0xb26e6947,0x34e36253,0xc75894df, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xd8e70900,
      0xc7042e85,0,0,0,0,0,0,0,0,0xae6d8d5b,0,0,0x4269846b,0,0x2da97b9e,0x5fb237c9,0x11e247d3  // 'b'->'r'
    },
    { 0x966cee07,0,0,0,0,0,0,0,0,0x027aec95,0,0,0x45d7a7e5,0,0xe45d5ddc,0x5ef03588,0x222ac6ab, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3272262e,
      0xc7792000,0,0,0,0,0,0,0,0,0x75b91d68,0,0,0xecd782b3,0,0x0b6bb626,0xb715f459,0xccbf6c4a  // 'b'->'r'
    },
    { 0x7da649f3,0,0,0,0,0,0,0,0,0x13b36ae2,0,0,0x78310a7b,0,0x84d26157,0xe1f93c60,0x4e8b1b53, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x7d08711a,
      0x93d9dace,0,0,0,0,0,0,0,0,0x6a211820,0,0,0xf59d6c73,0,0x2c9299c6,0xa5441761,0x79ac91ac  // 'b'->'r'
    },
    { 0x090d833b,0,0,0,0,0,0,0,0,0xc89d2739,0,0,0x6e2edab2,0,0x8e7228ad,0x829076e9,0x28ed0c84, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x8942edb9,
      0x24d2005d,0,0,0,0,0,0,0,0,0xae6fbd5b,0,0,0xa6433591,0,0x471089a3,0x8a0a8ec2,0x20fd0194  // 'b'->'r'
    },
    { 0x536013ad,0,0,0,0,0,0,0,0,0x648664b9,0,0,0x25a2b3cf,0,0xf4d70177,0x28ed3ea4,0x2fe7cf69, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x21212abe,
      0xe76b7e04,0,0,0,0,0,0,0,0,0x943441f1,0,0,0x8b36ddf2,0,0x179e5ccd,0x74f8259e,0xe919756d  // 'b'->'r'
    },
    { 0xe1cd7757,0,0,0,0,0,0,0,0,0x153da2e2,0,0,0x756711a3,0,0xcce59a49,0xb9630cda,0xe08ba7b7, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x6626861a,
      0x17ecf576,0,0,0,0,0,0,0,0,0xe76f7416,0,0,0x6d2261cc,0,0xb0a57acf,0x7924fd62,0xb31a6e5a  // 'b'->'r'
    },
    { 0x9487cc33,0,0,0,0,0,0,0,0,0x53e57be6,0,0,0xb75bc72e,0,0xc1bc3ed0,0x06edfe3d,0xa2d4e5bc, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xbb3cdb2f,
      0x3d71f7fa,0,0,0,0,0,0,0,0,0xc457b868,0,0,0x29191280,0,0x02800d8a,0xcbe04fcb,0x4eebd78d  // 'b'->'r'
    },
    { 0xf58bf147,0,0,0,0,0,0,0,0,0x3b9d125e,0,0,0x75489606,0,0x80e09ead,0x974abcf5,0xf427159e, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xdb93b60f,
      0x8eccb8a9,0,0,0,0,0,0,0,0,0x750c98a6,0,0,0x18f3b535,0,0xf3ae0bab,0x9f265252,0x93646d87  // 'b'->'r'
    },
    { 0xdcef0cdc,0,0,0,0,0,0,0,0,0xd21dcb41,0,0,0x285a96a9,0,0xe8a9fb42,0xfe0fdc72,0xd0c62b5c, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x15c2a14e,
      0x28cf62e5,0,0,0,0,0,0,0,0,0x182e64db,0,0,0xa0ff7cf6,0,0xa2342064,0x65ffc99f,0xf30528dd  // 'b'->'r'
    },
    { 0x100df4b2,0,0,0,0,0,0,0,0,0xefce9dfc,0,0,0x6c8d60ae,0,0x7287625d,0x42391e72,0xba4a4ea1, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xd95a930c,
      0xbe034ee0,0,0,0,0,0,0,0,0,0x0886a6e9,0,0,0x4e96a350,0,0xf57fe442,0x1ea955c8,0x5af973f3  // 'b'->'r'
    },
    { 0x71a2087d,0,0,0,0,0,0,0,0,0x5b51248a,0,0,0x644b5270,0,0x042e1ada,0x8827449b,0x2f6b62b8, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xd8695c78,
      0x66b8f141,0,0,0,0,0,0,0,0,0x894949c0,0,0,0xede60ac5,0,0xae262f58,0x19805d22,0x9bf30fcf  // 'b'->'r'
    },
    { 0xf1ff4803,0,0,0,0,0,0,0,0,0x1935dabc,0,0,0xde96ccee,0,0x178f1ea5,0x7443fcab,0x0e53c6d3, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x53a2ab58,
      0x1626fe46,0,0,0,0,0,0,0,0,0x3b951e94,0,0,0x3cb76386,0,0x9d4d8f1c,0xd6ea5273,0x08779386  // 'b'->'r'
    },
    { 0x85ba1342,0,0,0,0,0,0,0,0,0x03fec25c,0,0,0x8358dfdc,0,0x6dc58e66,0xa65b6365,0x116d4d7b, // 'B'->'R'
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x8b6a4ec5,
      0x407f346d,0,0,0,0,0,0,0,0,0x084fa549,0,0,0x389e0064,0,0x9484d2b6,0x40d1234d,0xc5661795  // 'b'->'r'
    }
};

static uint64_t hash64_lookup[64]['r'-'B'+1] =
{
    { 0x218cd5fb8c7f0aac,0,0,0,0,0,0,0,0,0x6050629f97c4aa2f,0,0,                     // 'B'->'K'+2
        0x0314ce51b716a675,0,0x7db3cc23d821ccc0,0x1d9060ed9a4eb343,0xfb4cbcf3dba252fb, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x9e54b8fa8b7d76c3,                                // 'S'->'a', where 'a' is a proxy for ' ' or '.'
        0x3ea17988d8e57d67,0,0,0,0,0,0,0,0,0xf968dafe6c74a409,0,0,                     // 'b'->'k'+2
        0x5fd3a5199fa1ded3,0,0xfd874015a5595115,0x0bb059ad6266d6f2,0x68b7c4e57005b724  // 'n'->'r'
    },
    { 0x4f6097d64c2b3a57,0,0,0,0,0,0,0,0,0x29b76190e44b3c46,0,0,                     // 'B'->'K'+2
        0xd4de74990e84bdd8,0,0xa385e3eef6b29a58,0xce990c7745cccd8c,0x7d84a6a56229393a, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xa3d89f7f7a4842c1,
        0xfd49f581caae7de6,0,0,0,0,0,0,0,0,0x5e3bf585cfea4a27,0,0,                     // 'b'->'k'+2
        0x10b7c6c68765a857,0,0x5010998c7adfc8ae,0xc8820d5a916b5e58,0xcd45224a648d8b51  // 'n'->'r'
    },
    { 0x49d47bfbecf3e6a5,0,0,0,0,0,0,0,0,0x1208d3b6d6094219,0,0,                     // 'B'->'K'+2
        0x3dcd9c4e122f6b4d,0,0xaefea33e565f9848,0xa999e648164e1b09,0x617778c7a5ee9794, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3efdff2d052d0873,
        0xa2494c855e4513d0,0,0,0,0,0,0,0,0,0xaa75be2fd52692f3,0,0,                     // 'b'->'k'+2
        0xed47f2bbf5081ec5,0,0x846e54aac73547fe,0xda9bd1c323ee074f,0x6c91188adeb91daf  // 'n'->'r'
    },
    { 0x7f67d2f2debe09c0,0,0,0,0,0,0,0,0,0x8e000539fa86bb52,0,0,                     // 'B'->'K'+2
        0x6d868ddb793e6063,0,0x497c3559cc95a7d8,0xd2934183cd087cb1,0xb4e2147d762382f3, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xbcfc6ace853e031d,
        0x6a340f52c7d0c293,0,0,0,0,0,0,0,0,0x727804c5adcb0c93,0,0,                     // 'b'->'k'+2
        0x5c4cb6ba1e473b8e,0,0xf80a0784b87b61a7,0xd422dc11a3d1dd20,0x5cf822c594ff3fc1  // 'n'->'r'
    },
    { 0xeccaa1bf24b2cd09,0,0,0,0,0,0,0,0,0x65c4c15e89914ab9,0,0,                     // 'B'->'K'+2
        0x0bc72298f1d5d27f,0,0xbd1a4e83c234a220,0x3b8d71458597da1f,0x72f721a81b1cc2ca, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x593890a46a2748f4,
        0xeff1de3a793de097,0,0,0,0,0,0,0,0,0xd0a1a4b143b9eaa3,0,0,                     // 'b'->'k'+2
        0x41da0db72fb379fe,0,0xfc492a98c6342dcb,0x61bb02a1bca6ab72,0xf80e879274c644b7  // 'n'->'r'
    },
    { 0xb277df61376fd81c,0,0,0,0,0,0,0,0,0xe7aab1ce9184e322,0,0,                     // 'B'->'K'+2
        0xe5a662f1229da880,0,0x4beb1c8704cf6880,0x1efdc7b552fae7a4,0xfdf472eb9e1d5c35, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3dd5f02e26511785,
        0x3fd9fdf09cb24e26,0,0,0,0,0,0,0,0,0x3a6f7bf438ea0de8,0,0,                     // 'b'->'k'+2
        0x1b1caa7f9def62f4,0,0x7d507ba162f0f111,0xf371a151f199794f,0xe43ad49de710b184  // 'n'->'r'
    },
    { 0x3bc16e0cae8bc669,0,0,0,0,0,0,0,0,0x5bacee76732fec2a,0,0,                     // 'B'->'K'+2
        0xb094a72e5c08b5ba,0,0x629eeb769cf1ba1f,0x0ef071206fe15378,0xeaae9f22e7005101, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xbb0fc073b297f541,
        0x1d231657196a6fe7,0,0,0,0,0,0,0,0,0xe1b86a7c0f6aefa9,0,0,                     // 'b'->'k'+2
        0xa1917199f8456839,0,0x45be6caeaab13923,0x220029f2a7342f66,0x6109df6babaeec77  // 'n'->'r'
    },
    { 0x5fce7e342bc0bb0b,0,0,0,0,0,0,0,0,0x5fd1dfe935dba1ae,0,0,                     // 'B'->'K'+2
        0x530c326e5bafdc52,0,0xbfb096402101505b,0xae1c0d4cc02cf780,0x3ce0ef7650bfe98e, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xcba82a499b9aca63,
        0x2bfe90925d1c2635,0,0,0,0,0,0,0,0,0x8101cb0453364b8c,0,0,                     // 'b'->'k'+2
        0x7304c70791f86a79,0,0x4bd68a8309d63faa,0x4df1a43070483054,0xe2ce6c4ca25fc8cb  // 'n'->'r'
    },
    { 0xd6d51925fd061144,0,0,0,0,0,0,0,0,0x5a143074f57db306,0,0,                     // 'B'->'K'+2
        0x3cdca5ed1a1f9bc4,0,0xbd072630a71d442f,0x809c986d3578f27f,0x8e2c27d2a29337f4, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf14d28b3294b9483,
        0x3396aa31fecbf3cc,0,0,0,0,0,0,0,0,0xa24dac47a7321b64,0,0,                     // 'b'->'k'+2
        0x8c6bbf5a94f424b4,0,0xde06adb140d7b7e8,0x85074fee6a140f4e,0xf0b1951d7760248f  // 'n'->'r'
    },
    { 0x5949d2037985c694,0,0,0,0,0,0,0,0,0xc032204a3e92ace3,0,0,                     // 'B'->'K'+2
        0x064d7e549f9e5bba,0,0xb31759ea28b23b17,0x2619ad415687aacf,0xf7cc97771c418b8d, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x21c10e14acbc9175,
        0xfe910cd0a8053755,0,0,0,0,0,0,0,0,0xb53a142a51342230,0,0,                     // 'b'->'k'+2
        0x73aa95f2235ff531,0,0xb585c01cc741a645,0x1224859a325338a9,0x9c9b8b57f31716a3  // 'n'->'r'
    },
    { 0x4af48cb45e64c5c0,0,0,0,0,0,0,0,0,0xac021930a99b5c5f,0,0,                     // 'B'->'K'+2
        0x2700b7c2d22c9cc5,0,0x7290666603796e5e,0x6ae0630918dba100,0xb2321d029f72d771, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x219c2d74d6838eb2,
        0x60d9fb6cac74f524,0,0,0,0,0,0,0,0,0x9aa776e91899e7a2,0,0,                     // 'b'->'k'+2
        0x199bb359f8d16330,0,0x61ffb57cf9f93f5d,0xf5d36375e0d14983,0xe538026477f98662  // 'n'->'r'
    },
    { 0x128b105a8276be2a,0,0,0,0,0,0,0,0,0xf7c16444fa0d03cd,0,0,                     // 'B'->'K'+2
        0x04f0e2690e435170,0,0x8c00a60a9ad727e7,0xfac5500c737f2b95,0x465ad668bd4060c9, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x2602a8e1051de97f,
        0x979c69a50a083600,0,0,0,0,0,0,0,0,0x423a50a77113f78a,0,0,                     // 'b'->'k'+2
        0xe59223a048660972,0,0x372ce57afac6322b,0x681fad211ec533ba,0x9475239a5c048d7f  // 'n'->'r'
    },
    { 0x8d5500634bcfd817,0,0,0,0,0,0,0,0,0xf9cadcd97b1bd6bb,0,0,                     // 'B'->'K'+2
        0x458b09321e64f082,0,0x45e3e958b04c1979,0x7497fcd251675862,0xf856d714e166de3e, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x66d6b2de6a0d23a3,
        0x0686fe9ceb117ade,0,0,0,0,0,0,0,0,0x3f980648106bf87b,0,0,                     // 'b'->'k'+2
        0xe356d5123781a7c3,0,0x81807599b145da52,0xb567639890b037ae,0x4f751e27910ccae3  // 'n'->'r'
    },
    { 0x471c2ceadd775c94,0,0,0,0,0,0,0,0,0x5f7f367b43f090d1,0,0,                     // 'B'->'K'+2
        0xe515c11c824bca32,0,0x8664769885f3959b,0x06ca2e92eaae5b0e,0xc026fec3180c7c29, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xa029b8acebd0fc3a,
        0x5560bed393713ac1,0,0,0,0,0,0,0,0,0x545ce92d1546dc24,0,0,                     // 'b'->'k'+2
        0xec95ab36ede65b0a,0,0xf795400047189056,0xdcc0e88a518dbc2b,0x2b27d2c302653368  // 'n'->'r'
    },
    { 0x0c76f786aadb680b,0,0,0,0,0,0,0,0,0xe6c796dbd7a3bb02,0,0,                     // 'B'->'K'+2
        0x40eeb76321bd8133,0,0x5476a9b9a5ad3450,0x7235efc6b7613820,0xe3a4cb93d76514b6, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x1ac8e4a24a168480,
        0x8330083243c55b26,0,0,0,0,0,0,0,0,0x5935b8f22ee5a113,0,0,                     // 'b'->'k'+2
        0x82b8936665d794ae,0,0xbd7708149625b62a,0x63071aab8d85b573,0xae3632760525c4b8  // 'n'->'r'
    },
    { 0x87b3e8802a3989bc,0,0,0,0,0,0,0,0,0x9f5495e4d43569e8,0,0,                     // 'B'->'K'+2
        0xf1d8596d5eabbe4d,0,0xb5e530690133b91e,0xe4929ec2257d3518,0x3475e0e1ad85627d, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xad575e8c91d28302,
        0x5779870f451f3e03,0,0,0,0,0,0,0,0,0xa3974c76b428205e,0,0,                     // 'b'->'k'+2
        0xad1e3a21bc35ace2,0,0x8c61689749d9976b,0xe115878cf651fd0d,0x45ce8f856eebf770  // 'n'->'r'
    },
    { 0x8b2e7c263fae4928,0,0,0,0,0,0,0,0,0x25f4ef01c1903548,0,0,                     // 'B'->'K'+2
        0x485f010a937f0c13,0,0xdb36ac066566b25f,0x07bd7f7d97900f48,0xa2931506e562c59a, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xb5d8a266927f19c2,
        0x4cb56fa6a39054f8,0,0,0,0,0,0,0,0,0x0204b14b391be0b4,0,0,                     // 'b'->'k'+2
        0x75d0f572e43ce943,0,0x5f67e555f3e75bec,0x06476d7eae181f3d,0x87d149727276cf0e  // 'n'->'r'
    },
    { 0x8edd5ab272fe9f60,0,0,0,0,0,0,0,0,0xec0f7f33d8ae3d04,0,0,                     // 'B'->'K'+2
        0x5c746d02fa839fc3,0,0x16e91d4cb31112ed,0x80b1929d1dbf688b,0x67ebd6094c24d3fc, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x27542273c45baa56,
        0x063963c8d0550dcd,0,0,0,0,0,0,0,0,0x6cee1185696d0b79,0,0,                     // 'b'->'k'+2
        0x57b55a746581666d,0,0xe7fae0b5ace9934b,0x9f4a88ebe18ffab8,0x022dd0c13ff2a610  // 'n'->'r'
    },
    { 0x4770ff2b94ce4c98,0,0,0,0,0,0,0,0,0xf8077a4f502f139d,0,0,                     // 'B'->'K'+2
        0x35b22c3ee1b96895,0,0xb0de694cf725846e,0x1d9eba2fb149c019,0x19735a4c96a5a5d0, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x93bf7695b9aa43bc,
        0x4a2ec078a8e00779,0,0,0,0,0,0,0,0,0xb1fd37128056cb76,0,0,                     // 'b'->'k'+2
        0x8a00b95988803475,0,0xac05620df4c1e5bd,0xb961c0de3b043653,0xf2e88e5ca4dc8aa1  // 'n'->'r'
    },
    { 0x638cd0ec65162768,0,0,0,0,0,0,0,0,0x00b128696c81c3a0,0,0,                     // 'B'->'K'+2
        0x3a222cd59e6a3ce4,0,0xa5aeae069b3c95fb,0x7471b2647990eafb,0x99b34e3004e9d879, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf8da6628785a9546,
        0x96b7a39a4d3401d5,0,0,0,0,0,0,0,0,0xd5751c9eb750a91f,0,0,                     // 'b'->'k'+2
        0xf6781d85a901220d,0,0xea2fb88f49b9c747,0x0451b3bf4a4286b8,0xaad32de5622a9498  // 'n'->'r'
    },
    { 0xb24cac259e36424f,0,0,0,0,0,0,0,0,0x70cf7e3bbfc99829,0,0,                     // 'B'->'K'+2
        0xd04d3e716dc3c912,0,0xaf62305fe0e23e28,0x8282b96022ae6db6,0xa60a89f01a5540cf, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x557eb8d04c5c3b0b,
        0x0773192017a5d0a6,0,0,0,0,0,0,0,0,0xca74803e91e9386f,0,0,                     // 'b'->'k'+2
        0xeb66698a5aa2cd5d,0,0x18e4e98c97436ff9,0x8ade765d8d43d481,0x71293f409306fadf  // 'n'->'r'
    },
    { 0xfdbb759d089ba776,0,0,0,0,0,0,0,0,0x851b995aa7382b2c,0,0,                     // 'B'->'K'+2
        0x621307e7f80de0d8,0,0xb1a72096a6f03d7d,0xce81e193522ce018,0x174449856e717043, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x76048b2438a4abd2,
        0x3a4e7253e58413ef,0,0,0,0,0,0,0,0,0xd3aac07f2429df03,0,0,                     // 'b'->'k'+2
        0x690985e45e1888ea,0,0x8a03a70518e606cc,0x9f903d5c6f94d7e6,0xe323c9ccfbea3123  // 'n'->'r'
    },
    { 0xe30fdc9ae45516d6,0,0,0,0,0,0,0,0,0xe14eaef142a5b3fe,0,0,                     // 'B'->'K'+2
        0x9e6b6453ce62babd,0,0x540baf8c897a4ec5,0x222a87e6b4320ad7,0x6788648772ab4a2b, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xfb074cb419a87820,
        0x8b6e0250197d5c0b,0,0,0,0,0,0,0,0,0xdf2d599deb633668,0,0,                     // 'b'->'k'+2
        0x9f2744005a3118d4,0,0x80b24ed7b6d8848a,0x611d7acf7820b6b6,0xec1eff4affb46feb  // 'n'->'r'
    },
    { 0x8ac8fefcd754f5a5,0,0,0,0,0,0,0,0,0x8a97ee8326423e7d,0,0,                     // 'B'->'K'+2
        0x1ef4923de796fe9c,0,0xec5dc943de3d826f,0xae422c90099d7de8,0xd474277029992302, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x4980a5c98220f61b,
        0xf7e19e549d954fd3,0,0,0,0,0,0,0,0,0x0b0080742ab684d9,0,0,                     // 'b'->'k'+2
        0xeb76e2e51fb2aa97,0,0x1db89effc76fe335,0x09398338d9171133,0x1b3ef53edd6c44ae  // 'n'->'r'
    },
    { 0x2cbddbb8ceac7494,0,0,0,0,0,0,0,0,0x51055a3069514bb5,0,0,                     // 'B'->'K'+2
        0x639e549191b0961d,0,0x3f1ce77523d53e43,0xf3bf9fb9683d2a23,0x90128e0908814327, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x4cceb1ec11b4ed89,
        0xe806d4befb8a0849,0,0,0,0,0,0,0,0,0x2914684fb28ab129,0,0,                     // 'b'->'k'+2
        0x9481e2e65f8ffb97,0,0xe2e4dcce741b5f83,0x991261a16b8a0f2e,0x7246e784b8d8a2da  // 'n'->'r'
    },
    { 0x3cf06b900cf357b2,0,0,0,0,0,0,0,0,0xe86a88e4ddcb3b6c,0,0,                     // 'B'->'K'+2
        0x94b507475d912703,0,0x2904dd3ef9bbc71f,0x2557bf170441bb09,0xa93eff5bdb15ed8a, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x55ebe96e3b11ee1b,
        0x70b5200702ffb1ad,0,0,0,0,0,0,0,0,0xa02299fdc3d140c7,0,0,                     // 'b'->'k'+2
        0x1394a3165c2785a7,0,0x5c7e8762f1b2143d,0xb24b689ebae0a955,0xe936e6f6bffff361  // 'n'->'r'
    },
    { 0x3f5586502befec2c,0,0,0,0,0,0,0,0,0xe0e4e49b56e32b22,0,0,                     // 'B'->'K'+2
        0x0a243e2f8562a7a2,0,0x6f24d1ef7d531458,0x0f5882720de91821,0x84823a0b56c7ba85, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x0fa2d7d73332f8e8,
        0x7983d74e2df312ff,0,0,0,0,0,0,0,0,0xe8612fe704bdd824,0,0,                     // 'b'->'k'+2
        0x2f32631e2bc5c700,0,0xbd69085acb2fc5cb,0x6f4c792276a4b922,0xe5e76a36395320c5  // 'n'->'r'
    },
    { 0x30036751dfe4037e,0,0,0,0,0,0,0,0,0xac2e5aa35868f7b5,0,0,                     // 'B'->'K'+2
        0xb717dc47f1b1d4fe,0,0x87689b6ded96bc50,0x85e3bf3b9bb675be,0x4df03270b4548088, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x521091a598be68bd,
        0xff7a744408269881,0,0,0,0,0,0,0,0,0x00cb21b5c89ce8d1,0,0,                     // 'b'->'k'+2
        0x5cb86c0d2a296570,0,0x98e8d8658001b923,0x787c73139f193578,0xa629ff6b0ce50d5b  // 'n'->'r'
    },
    { 0x60d4ad0893c540a8,0,0,0,0,0,0,0,0,0x01541b60b2f81774,0,0,                     // 'B'->'K'+2
        0xfac246d13ce68b24,0,0xf86074c5fe0db0b0,0x2cb6697cef28a619,0xa9f32064446b5143, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x53f705819d2cdf67,
        0x4b0b82fdadd8e1fc,0,0,0,0,0,0,0,0,0xae2b549d891f3b23,0,0,                     // 'b'->'k'+2
        0x334b1385dd418c72,0,0x102e5fd09704571e,0xecfad0e6c037541d,0x9b74b1f8bae946f1  // 'n'->'r'
    },
    { 0x4f2b17a8f6e8cd21,0,0,0,0,0,0,0,0,0x7a470a964fdba092,0,0,                     // 'B'->'K'+2
        0x67394c1e8de2d511,0,0x48b438ff65f1d0dd,0xa2105a13365f3954,0x3d08a70435b851fd, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xe401d29438f20a02,
        0x83b71b6a2faa5845,0,0,0,0,0,0,0,0,0x6ebf4a4a37fff565,0,0,                     // 'b'->'k'+2
        0x5e30807bf1c2638c,0,0xed9fbdc391cf922c,0x74de7cd6bd533375,0x9792993c73bd6afd  // 'n'->'r'
    },
    { 0x5664eae57d8eb542,0,0,0,0,0,0,0,0,0x6e8974faf8616e6f,0,0,                     // 'B'->'K'+2
        0xf872465d3a37d85b,0,0x97c072a9ae382d55,0x22291628411d81a7,0x5ac0e8bc15d5ee27, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x18c824d10edaffcb,
        0xa83272b30e716e96,0,0,0,0,0,0,0,0,0x57e885286f35ed9e,0,0,                     // 'b'->'k'+2
        0xa6fdb83b7ce2ee91,0,0x974d89ea4fd1dac6,0xd25c25dee18983c7,0xa8bfca1db2439112  // 'n'->'r'
    },
    { 0x671bb8e1f9f5a35c,0,0,0,0,0,0,0,0,0x65b29e5e60b4582b,0,0,                     // 'B'->'K'+2
        0x099ddcee9e1ed453,0,0xffd0aea52dfa81b1,0xfc10d17e8ae13329,0x5a15a8d80651585d, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x72b97d10dac7f4ae,
        0xb8ae1b6311374595,0,0,0,0,0,0,0,0,0xf008dca7be6bf0c9,0,0,                     // 'b'->'k'+2
        0xa3389e9dadecaf59,0,0xac0ae1fc7a8549f2,0xd0244c41742579e0,0x44463f17ad5537db  // 'n'->'r'
    },
    { 0xf9e9ecc8895d4149,0,0,0,0,0,0,0,0,0x906685809b674e1c,0,0,                     // 'B'->'K'+2
        0xe3c98457e58c3feb,0,0xf4f98f25b6f660d1,0xc3ba556ffd86da69,0x1d0d69bf7830f7ba, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xb2c0699a37868f80,
        0x45b0d2d974bd5fd6,0,0,0,0,0,0,0,0,0x55e33559a9bf7e3f,0,0,                     // 'b'->'k'+2
        0x1501bc86e80b0410,0,0x25ceb8d84369186a,0xc3e8c5f72320e0a4,0x1008e9cf0549625e  // 'n'->'r'
    },
    { 0x058d64653aae1e18,0,0,0,0,0,0,0,0,0x3ded4529c2251a74,0,0,                     // 'B'->'K'+2
        0x2322e791e1af94bf,0,0x6ea9ffd051eca4c3,0x42b88f70e7886533,0x85a18c2b622ab088, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xdeff446ea55223b8,
        0xc5e8cdc5969bf35b,0,0,0,0,0,0,0,0,0x2dedd176531e6c5d,0,0,                     // 'b'->'k'+2
        0xf5700b93d4bf977b,0,0x6eeb5aef850bcaee,0x1e08b716a104f457,0x533aed6d0003a0a0  // 'n'->'r'
    },
    { 0x1b649f8bdf660893,0,0,0,0,0,0,0,0,0x9559fa0b4fd61248,0,0,                     // 'B'->'K'+2
        0x96f63ba14606d9c7,0,0xefbc5f876cea6457,0xbed26877cc4ccc0d,0xf0d7111ae2a57d3a, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xd92062302f85d651,
        0x0d712cc7ae0c9478,0,0,0,0,0,0,0,0,0xef92b0eff3ea2774,0,0,                     // 'b'->'k'+2
        0x98a14ab674c4ebb7,0,0xfbc1d260afff3b40,0xe44d11647bc0aacb,0x66596752372b82dc  // 'n'->'r'
    },
    { 0xc7eb1a57c9ead3a4,0,0,0,0,0,0,0,0,0x615da595f286e119,0,0,                     // 'B'->'K'+2
        0x362715fe3abcb320,0,0x25d6ca02bb195daa,0xc3d6ad08e15b2f0e,0xaf57dd0a410251d6, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x30666905504e251c,
        0xa8218d26369b9d14,0,0,0,0,0,0,0,0,0xe6151ba6f51b7fd2,0,0,                     // 'b'->'k'+2
        0xdcb0226484a8cd44,0,0x024951c178c4b616,0x18e5e6750691d4e3,0x1719823bb62a5b7a  // 'n'->'r'
    },
    { 0x7f6e4ea4351cc253,0,0,0,0,0,0,0,0,0x9e82c38527588287,0,0,                     // 'B'->'K'+2
        0x94c1c9056cb82fc8,0,0x61e79826bafe423d,0x3dd6aea35fc99a8d,0x18b85858a5719605, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x2b453fb176ace100,
        0xfc2496cf37026c88,0,0,0,0,0,0,0,0,0xdc0d7d544712accf,0,0,                     // 'b'->'k'+2
        0xe58659722fbbb9cf,0,0x0efc250996377fb5,0x4567aff7cebd948b,0x787a9c84dd25a404  // 'n'->'r'
    },
    { 0x7922fbb1bf4099a7,0,0,0,0,0,0,0,0,0x1da74c181e16915c,0,0,                     // 'B'->'K'+2
        0x7ad1265eacc2cbad,0,0x7501cd828472f51a,0x8225df5346e2824a,0xfadecf8321cf3734, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf79e4e4d2cc6d3ee,
        0x6c78050bb7841db1,0,0,0,0,0,0,0,0,0x1996e18db4586cdb,0,0,                     // 'b'->'k'+2
        0xdb6c89e465642b33,0,0x0a268851769102e3,0x1e08649a90bf7369,0x7bd42fc6d7265312  // 'n'->'r'
    },
    { 0x4570c6af2eeb6d75,0,0,0,0,0,0,0,0,0x7550766234721522,0,0,                     // 'B'->'K'+2
        0x675ef6152514be33,0,0x3c3a2f942a3abe9e,0x665a16aa7cf141b5,0x0f121f7a1ff50f3a, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xabdfc9b95b096fab,
        0x1fb44491b8da4737,0,0,0,0,0,0,0,0,0x583f7265f0c025fc,0,0,                     // 'b'->'k'+2
        0xdb544fd907cbc3fc,0,0xb6b96cf4c3ec5b12,0x665099d5bf3b03ad,0xa3a1c0b7bfa86b57  // 'n'->'r'
    },
    { 0x61a33e6417b461c1,0,0,0,0,0,0,0,0,0x662f6cc4e75a2d46,0,0,                     // 'B'->'K'+2
        0x05a0106f37aad5ea,0,0x6e87d4e2155b2c35,0x5fefcb0bbfcf2330,0x9d51afe28d5c7c5e, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xa95f1861bb50483b,
        0x11ee48ee95a03950,0,0,0,0,0,0,0,0,0xa01779930bad669a,0,0,                     // 'b'->'k'+2
        0xd2893f70f641767c,0,0xe9fb540b358b50a3,0xa7469be14aca2e3a,0xabfab619497343b1  // 'n'->'r'
    },
    { 0xb9f194613da6f46a,0,0,0,0,0,0,0,0,0x52f7155fad6120c9,0,0,                     // 'B'->'K'+2
        0x8973c92d19acdd2c,0,0x50f3cb061023470d,0x1c483a590434bb79,0xd5542f588e3f0746, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x67b069ededf5a226,
        0xc69b0a26025d8ea7,0,0,0,0,0,0,0,0,0x67c69033ab7fa688,0,0,                     // 'b'->'k'+2
        0x47f9a410d541fc0d,0,0x522fdf97c8ffc7f8,0xb6c3eedafbfd0387,0x07aad303481f76d0  // 'n'->'r'
    },
    { 0x8fc3875ab4183bf8,0,0,0,0,0,0,0,0,0xbcbcb1ae961efa16,0,0,                     // 'B'->'K'+2
        0x66501e7a2e7f61f8,0,0x1c1ad59d105f5f4f,0x87e67302832c37d9,0x9b3702857c521708, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x15d1d57994982ee3,
        0xb4060fc7fa3d1f06,0,0,0,0,0,0,0,0,0xaa69db0fc99c5cd1,0,0,                     // 'b'->'k'+2
        0xa2815b4ee062a5c7,0,0x079625a39b41f9d4,0xf58a58b4569195d9,0xe3a8556737e93fc2  // 'n'->'r'
    },
    { 0x1a71bf76f629763c,0,0,0,0,0,0,0,0,0xae9f59077485f190,0,0,                     // 'B'->'K'+2
        0xddfeaab33b50cc38,0,0xe42d2d66e0fd9b72,0x077902f9f3068eed,0x004f23b37e054a97, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x88f04f9df0fe2118,
        0xefed92b7b72f0404,0,0,0,0,0,0,0,0,0x677c8271cc988a64,0,0,                     // 'b'->'k'+2
        0xf553a18b7c74f3ec,0,0x8630e9c3a1650931,0xcbdb96cdb5636957,0x5d772f7adfd1561e  // 'n'->'r'
    },
    { 0xfdf88d377f861e36,0,0,0,0,0,0,0,0,0x30c8fe584b036099,0,0,                     // 'B'->'K'+2
        0x5af009fbd8346f14,0,0x04e53cfcd9545d61,0xd92cd1f831c06965,0x84a577449e2d2ab9, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc54aec75c5f8b197,
        0xd7c0397303637d9b,0,0,0,0,0,0,0,0,0xb8747dd6f969041d,0,0,                     // 'b'->'k'+2
        0x5f8b14b958e44ba1,0,0xa227795ddcc05573,0x390d9a6625ec8f35,0x4eb0338bc7ca0a77  // 'n'->'r'
    },
    { 0x149dbecefb592bb3,0,0,0,0,0,0,0,0,0x898b1e52fc2b1356,0,0,                     // 'B'->'K'+2
        0x15d6efd97a7679f6,0,0x464a8cecc0e9f007,0x38ad70037f550a69,0x023980a501094bf1, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x199a8587a3b47889,
        0x0662c71d44fc9ab6,0,0,0,0,0,0,0,0,0x43d9a44e5e5b8f80,0,0,                     // 'b'->'k'+2
        0x24efef0b69160353,0,0x10ef2bc9230be578,0x8385c20b6da013a4,0x10cb7b10d2764ed1  // 'n'->'r'
    },
    { 0x39001c3c4c3f5c94,0,0,0,0,0,0,0,0,0xdb34ea7b3099df75,0,0,                     // 'B'->'K'+2
        0x5e96e06766b09bf0,0,0x2980b6bf82e5cd03,0x0e1b82bb1ee3607e,0x8f475881396cd72a, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xe9443792fb0f2241,
        0xc6012eb5190c5614,0,0,0,0,0,0,0,0,0xe28c351c67f78324,0,0,                     // 'b'->'k'+2
        0x74d741c0dcb89544,0,0xe5b3f35591b7cbd0,0x27432ed8f9114070,0x550a764957f687af  // 'n'->'r'
    },
    { 0x3318bb88f5f9428a,0,0,0,0,0,0,0,0,0xb6825fd2c9f390ed,0,0,                     // 'B'->'K'+2
        0x6ed80d8be8140568,0,0x6d4c29dc694fb3de,0x873a5975c627f75b,0x4d3bb7b35bf9362b, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x5974eb515549003f,
        0x120671b766458f9f,0,0,0,0,0,0,0,0,0xc80f0c0f14c30f94,0,0,                     // 'b'->'k'+2
        0x6bfe00834d44c9c6,0,0xc042f46a6840f509,0x6368e9d8c674cdbc,0xed5dc0d53b73b25b  // 'n'->'r'
    },
    { 0x290f8d1ced1c4a6f,0,0,0,0,0,0,0,0,0x07696f3a21eab5a3,0,0,                     // 'B'->'K'+2
        0x59a72ed553478953,0,0xe54c064e0dad674c,0x187a8b6ff3ef5512,0x647f6341b9c08d71, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xdf00934c03921f4a,
        0xe0dd050b02ece8e2,0,0,0,0,0,0,0,0,0xaae76519889134e1,0,0,                     // 'b'->'k'+2
        0xb0d74567c544c7ab,0,0x3155f70b4df91683,0xfcc2e35a259e4b8c,0xeb0812ede2031ce4  // 'n'->'r'
    },
    { 0xc26055a2145b8f3a,0,0,0,0,0,0,0,0,0x9465f7814028cf81,0,0,                     // 'B'->'K'+2
        0x5d52cbfe16f03971,0,0x74b5c290ad6adc80,0x8045ad01ac0b5327,0xa4712a10cf77f418, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x605b0a213ed062ba,
        0x4107cc216ea14124,0,0,0,0,0,0,0,0,0x5d6adcac6ba87963,0,0,                     // 'b'->'k'+2
        0x8d1385e6c08be345,0,0x42caa3288eafb886,0x21c46948d460d003,0x7ef7e8cddc4d14e2  // 'n'->'r'
    },
    { 0x3355046a61085b79,0,0,0,0,0,0,0,0,0xae868253ba1f92a8,0,0,                     // 'B'->'K'+2
        0x839912f218b779bc,0,0x46ffc5a0453435a1,0x18f5539741925d1c,0x863dc14421a8db44, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xda13159f9789101a,
        0x1e0ca2960e2d02e0,0,0,0,0,0,0,0,0,0x65425c4079fa68f8,0,0,                     // 'b'->'k'+2
        0x28eb9ee54d35916d,0,0x3baafd1f7ce947b3,0x0bfb804f431a2cc9,0x95dee053756135b5  // 'n'->'r'
    },
    { 0xf881db0f74c5a0c5,0,0,0,0,0,0,0,0,0xbf30ab33864bb3a1,0,0,                     // 'B'->'K'+2
        0x00fcfdf9aeeb8687,0,0x9be6ad8c7127ea7d,0x2db76aadb214825e,0x943a140ada464848, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x9ce4d5f44894b0f6,
        0x2b8ba6696ef5db54,0,0,0,0,0,0,0,0,0x14d4a3506142e487,0,0,                     // 'b'->'k'+2
        0xafb56c46d3adc6c3,0,0xe1c3f8152e5fe8d5,0xfa0faede82643ddb,0xb83906c3c9de1e6c  // 'n'->'r'
    },
    { 0x9bccc550161ccd43,0,0,0,0,0,0,0,0,0xf76555400e8d9866,0,0,                     // 'B'->'K'+2
        0x5410a481a8f85f54,0,0x81f46732b26e6947,0xcd7d3b6034e36253,0x4ce7416bc75894df, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xd61bf3fbd8e70900,
        0x8cac11d2c7042e85,0,0,0,0,0,0,0,0,0x6a4fc952ae6d8d5b,0,0,                     // 'b'->'k'+2
        0xee4a22b94269846b,0,0x6a654d962da97b9e,0x85509f0f5fb237c9,0xefc6238f11e247d3  // 'n'->'r'
    },
    { 0x4467ae1c966cee07,0,0,0,0,0,0,0,0,0xb8e10d47027aec95,0,0,                     // 'B'->'K'+2
        0xe09d351b45d7a7e5,0,0x086c38d8e45d5ddc,0x5f3611395ef03588,0x6d0acb3a222ac6ab, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x005aa8d83272262e,
        0x180977ccc7792000,0,0,0,0,0,0,0,0,0x3f68490d75b91d68,0,0,                     // 'b'->'k'+2
        0xfa821792ecd782b3,0,0x5177c04e0b6bb626,0x93411fceb715f459,0xdbd8d714ccbf6c4a  // 'n'->'r'
    },
    { 0xa4c816477da649f3,0,0,0,0,0,0,0,0,0x756076b713b36ae2,0,0,                     // 'B'->'K'+2
        0x5d4d6d2c78310a7b,0,0x71d078eb84d26157,0xaaef5cf5e1f93c60,0x560acc5b4e8b1b53, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x9264582c7d08711a,
        0xe1741f0f93d9dace,0,0,0,0,0,0,0,0,0xc651a7fd6a211820,0,0,                     // 'b'->'k'+2
        0xf1ed4657f59d6c73,0,0xe66d17cb2c9299c6,0xfdec864da5441761,0x6780a1fe79ac91ac  // 'n'->'r'
    },
    { 0x60721d9d090d833b,0,0,0,0,0,0,0,0,0xe535565fc89d2739,0,0,                     // 'B'->'K'+2
        0xf755706e6e2edab2,0,0x10a93ed28e7228ad,0xcab89e84829076e9,0xaf7b721c28ed0c84, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xacf9f6c18942edb9,
        0x3b27683124d2005d,0,0,0,0,0,0,0,0,0x3eb113b1ae6fbd5b,0,0,                     // 'b'->'k'+2
        0x89f34957a6433591,0,0x3769be39471089a3,0xc5e59ed08a0a8ec2,0x2a821c2920fd0194  // 'n'->'r'
    },
    { 0xbf2025da536013ad,0,0,0,0,0,0,0,0,0xec39c6f0648664b9,0,0,                     // 'B'->'K'+2
        0xb8afa4c225a2b3cf,0,0x4b489baff4d70177,0x7b9ec91128ed3ea4,0x73fdfc142fe7cf69, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x31ab7aa621212abe,
        0x7e740540e76b7e04,0,0,0,0,0,0,0,0,0x349ffe86943441f1,0,0,                     // 'b'->'k'+2
        0xc73640ec8b36ddf2,0,0x89147296179e5ccd,0xd81fc84f74f8259e,0x7014984ae919756d  // 'n'->'r'
    },
    { 0x239a44a2e1cd7757,0,0,0,0,0,0,0,0,0xf998eea8153da2e2,0,0,                     // 'B'->'K'+2
        0x6d499aa6756711a3,0,0x4d02cbf6cce59a49,0xd7c55695b9630cda,0x14375f22e08ba7b7, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xd16aa1756626861a,
        0xba14aca017ecf576,0,0,0,0,0,0,0,0,0x1a37c9aae76f7416,0,0,                     // 'b'->'k'+2
        0xf9b5657f6d2261cc,0,0x2808bd2cb0a57acf,0xc83702637924fd62,0x4ddff729b31a6e5a  // 'n'->'r'
    },
    { 0x3f3e5af99487cc33,0,0,0,0,0,0,0,0,0x134abb6753e57be6,0,0,                     // 'B'->'K'+2
        0x095c0f9bb75bc72e,0,0x6cb6b6fbc1bc3ed0,0x682c093006edfe3d,0x1b8cd57fa2d4e5bc, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xe0b4468abb3cdb2f,
        0x9176b5443d71f7fa,0,0,0,0,0,0,0,0,0x6e972511c457b868,0,0,                     // 'b'->'k'+2
        0x3f1e5b0329191280,0,0x48b9de6502800d8a,0x98afd65ecbe04fcb,0xef6ff1d74eebd78d  // 'n'->'r'
    },
    { 0x1e16291df58bf147,0,0,0,0,0,0,0,0,0xc5b332663b9d125e,0,0,                     // 'B'->'K'+2
        0xf9d3867b75489606,0,0x9c699c9e80e09ead,0xeebc54cb974abcf5,0x00af71c2f427159e, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x644ccddddb93b60f,
        0xee771d3c8eccb8a9,0,0,0,0,0,0,0,0,0x3dff6a87750c98a6,0,0,                     // 'b'->'k'+2
        0xac809ab318f3b535,0,0x401a18f4f3ae0bab,0x3976aa499f265252,0x7bbfadd793646d87  // 'n'->'r'
    },
    { 0x065637c0dcef0cdc,0,0,0,0,0,0,0,0,0x0a3cf7f0d21dcb41,0,0,                     // 'B'->'K'+2
        0x01e97191285a96a9,0,0xa3c3b6a8e8a9fb42,0xed6a40f3fe0fdc72,0x68868a15d0c62b5c, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xb172765e15c2a14e,
        0x609db79a28cf62e5,0,0,0,0,0,0,0,0,0xb4848c8f182e64db,0,0,                     // 'b'->'k'+2
        0xcf328af0a0ff7cf6,0,0x9897b711a2342064,0xb583590a65ffc99f,0x89884691f30528dd  // 'n'->'r'
    },
    { 0xf44f3883100df4b2,0,0,0,0,0,0,0,0,0x74a0cbe7efce9dfc,0,0,                     // 'B'->'K'+2
        0x5a187e496c8d60ae,0,0x99a6431d7287625d,0xe5c55cc342391e72,0xa771e9c7ba4a4ea1, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc51c91d0d95a930c,
        0x5f7f88e2be034ee0,0,0,0,0,0,0,0,0,0x5b802e840886a6e9,0,0,                     // 'b'->'k'+2
        0xab1161bd4e96a350,0,0x9f25775af57fe442,0x74f664641ea955c8,0x49dfb8865af973f3  // 'n'->'r'
    },
    { 0x15c6f02071a2087d,0,0,0,0,0,0,0,0,0xdfd8ac515b51248a,0,0,                     // 'B'->'K'+2
        0x3bb14677644b5270,0,0x1cc80976042e1ada,0x14d4c6f58827449b,0x366219252f6b62b8, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x526fba77d8695c78,
        0x2edb451b66b8f141,0,0,0,0,0,0,0,0,0x9c7ff433894949c0,0,0,                     // 'b'->'k'+2
        0x672cb64eede60ac5,0,0x5e1cbfbbae262f58,0xbec7965619805d22,0xb9ea065f9bf30fcf  // 'n'->'r'
    },
    { 0x86e36808f1ff4803,0,0,0,0,0,0,0,0,0xad954d381935dabc,0,0,                     // 'B'->'K'+2
        0x7cd85b1dde96ccee,0,0x250a7a9c178f1ea5,0x0023c7df7443fcab,0x2b04d5a20e53c6d3, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x6ce720ba53a2ab58,
        0x107ed77f1626fe46,0,0,0,0,0,0,0,0,0x10450f7e3b951e94,0,0,                     // 'b'->'k'+2
        0x43ab4fd13cb76386,0,0x355015dc9d4d8f1c,0x592de86fd6ea5273,0xeeb15e9008779386  // 'n'->'r'
    },
    { 0x16976fc885ba1342,0,0,0,0,0,0,0,0,0x3724563b03fec25c,0,0,                     // 'B'->'K'+2
        0x9a2bb0f78358dfdc,0,0xdbbc03ec6dc58e66,0xb6e15407a65b6365,0xcd8a6b8c116d4d7b, // 'N'->'R'
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x283fb5208b6a4ec5,
        0x28bdced4407f346d,0,0,0,0,0,0,0,0,0x579652eb084fa549,0,0,                     // 'b'->'k'+2
        0x9b0ff04b389e0064,0,0xba2d0cdd9484d2b6,0x1d9c042840d1234d,0xa9cf3400c5661795  // 'n'->'r'
    }
};


/****************************************************************************
 * ChessPosition.cpp Chess classes - Representation of the position on the board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

/* Some project notes */
/*====================*/

/****************************************************************************
 * Class Hierarchy:
 *
 *  The Chess classes are intended to provide a reusable chess subsystem. To
 *  understand the hierarchy of classes, think in terms of the Java concept
 *  of derived classes "extending" base classes.
 *
 *  So
 *    ChessPositionRaw
 *      ;A simple C style structure that holds a chess position
 *    ChessPosition   extends ChessPositionRaw
 *      ;ChessPosition adds constructors, and other simple facilities
 *    ChessRules      extends ChessPosition
 *      ;ChessRules adds all the rules of standard chess
 *    ChessEvaluation extends ChessRules
 *      ;ChessEvaluation is a simple scoring function for a chess engine
 *      ;it attempts to calculate a score (more +ve numbers for better white
 *      ;positions, more -ve numbers for better black positions) for a
 *      ;position. Tarrasch GUI uses this only to attempt to list the most
 *      ;plausible moves first when you click on a square that can receive
 *      ;multiple moves
 *    ChessEngine     extends ChessEvaluation
 *      ;ChessEngine adds a search tree to ChessEvaluation to make a
 *      ;complete simple engine (the Tarrasch Toy Engine)
 *      ;ChessEngine is now split off from the rest of the thc chess library
 *      ;and is part of the Tarrasch Toy Engine project instead
 ****************************************************************************/

/****************************************************************************
 * Explanation of namespace thc:
 *
 *  The Chess classes use the C++ namespace facility to ensure they can be
 *  used without name conflicts with other 3rd party code. A short, but
 *  highly likely to be unique namespace name is best, we choose "thc" which
 *  is short for TripleHappyChess.
 ****************************************************************************/

/****************************************************************************
 * My original license, now replaced by MIT license full text of which is
 * in file LICENSE in project's root directory.
 *
 * Licensing provisions for all TripleHappy Chess source code;
 *
 * Start Date: 15 February 2003.
 * This software is licensed to be used freely. The following licensing
 * provisions apply;
 *
 * 1) The 'author' is asserted to be the original author of the code, Bill
 *    Forster of Wellington, New Zealand.
 * 2) The 'licensee' is anyone else who wishes to use the software.
 * 3) The 'licensee' is entitled to do anything they wish with the software
 *    EXCEPT for any action that attempts to restrict in any way the rights
 *    granted in 4) below.
 * 4) The 'author' is entitled to do anything he wishes with the software.
 *
 * The intent of this license is to allow the licensees wide freedom to
 * incorporate, modify and sell the software, with the single caveat that
 * they cannot prevent the author from either further development or future
 * commercial use of the software.
 ****************************************************************************/


// Return true if ChessPositions are the same (including counts)
bool ChessPosition::CmpStrict( const ChessPosition &other ) const
{
    return( *this == other  &&
            half_move_clock == other.half_move_clock &&
            full_move_count == other.full_move_count
         );
}

std::string ChessPosition::ToDebugStr( const char *label )
{
    std::string s;
    const char *p = squares;
    if( label )
        s = label;
    s += (white ? "\nWhite to move\n" : "\nBlack to move\n");
    for( int row=0; row<8; row++ )
    {
        for( int col=0; col<8; col++ )
        {
            char c = *p++;
            if( c==' ' )
                c = '.';
           s += c;
        }
        s +=  '\n';
    }
    return s;
}

/****************************************************************************
 * Set up position on board from Forsyth string with extensions
 *   return bool okay
 ****************************************************************************/
bool ChessPosition::Forsyth( const char *txt )
{
    int   file, rank, skip, store, temp;
    int   count_wking=0, count_bking=0;
    char  c, cross;
    char  p;
    bool okay, done;
    const char *reset = txt;

    // When store==0 validate txt without storing results
    for( store=0, okay=true; store<2 && okay; store++ )
    {
        txt = reset;

        // Clear the board
        if( store )
        {
            for( Square square=a8; square<=h1; ++square )
            {
                squares[square]   = ' ';
            }

            // Clear the extension fields
            wking  = false;
            wqueen = false;
            bking  = false;
            bqueen = false;
            enpassant_target = SQUARE_INVALID;
            half_move_clock = 0;
            full_move_count = 1;
        }

        // Loop through the main Forsyth field
        for( file=0, rank=7, done=false; *txt && okay && !done; )
        {
            skip = 1;
            c = *txt++;
            p = ' ';
            cross = ' ';
            switch(c)
            {
                case 'x':   cross = 'x';
                            skip = 1;
                            break;
                case ' ':
                case '\t':  done = true;            break;
                case 'k':   p = 'k';
                            count_bking++;          break;
                case 'K':   p = 'K';
                            count_wking++;          break;
                case 'p':
                case 'r':
                case 'n':
                case 'b':
                case 'q':
                case 'P':
                case 'R':
                case 'N':
                case 'B':
                case 'Q':   p = c;   break;
                case '1':   case '2':   case '3':   case '4':
                case '5':   case '6':   case '7':   case '8':
                            skip = c-'0';       break;
                case '/':   // official separator
                case '|':   //  .. we'll allow this
                case '\\':  //  .. or this
                {
                    if( file == 0 )
                        skip = 0;        // eg after "rn6/", '/' does nothing
                    else
                        skip = (8-file); // eg after "rn/", '/' skips 6 squares
                    while( *txt == '/'   // allow consecutive '/' characters
                      ||   *txt == '|'   //     (or '|' or '\')
                      ||   *txt == '\\' )
                    {
                        txt++;
                        skip += 8;
                    }
                    break;
                }
                default:    okay=false;
            }

            // Store single piece or loop over multiple empty squares
            for( int i=0; i<skip && okay && !done; i++ )
            {
                Square sq = SQ('a'+file, '1'+rank);
                if( store )
                {
                    squares[sq] = p;
                    if( p == 'K' )
                        wking_square = sq;
                    else if( p == 'k' )
                        bking_square = sq;
                }
                file++;
                if( file == 8 )
                {
                    file = 0;
                    rank--;
                }
                if( sq == h1 )
                    done = true;        // last square, we're done !
            }
        }

        // In validation pass make sure there's 1 white king and 1 black king
   /*   if( store==0 )  // disabled to allow ILLEGAL_NOT_ONE_KING_EACH message
        {
            if( count_wking!=1 || count_bking!=1 )
                okay = false;
        }  */

        // Now support standard FEN notation with extensions. See appendix to
        // .PGN standard on the Internet (and in project documentation)
        //
        // Briefly there are a total of 6 fields, the normal Forsyth encoding
        // (as above) plus 5 extension fields. Format of the 5 extension
        // fields is illustrated by example.
        //
        // Example extension fields; "w KQq e6 0 2"
        //
        // White to move,
        // All types of castling available, except black king side (so black's
        //  king rook has moved, other king and rook pieces unmoved). This
        //  whole field is replaced by '-' character if no castling available.
        // The last move played was a double pawn advance, and hence the
        //  'en-passant target square' field is NOT empty. The field is the
        //  square a potential capturing pawn would end up on (can deduce last
        //  move was e7e5)
        // Number of half-moves or ply since a pawn move or capture (for 50
        //  move rule)
        // Total (full-move) count (starts at 1, increments after black moves)

        // Who to move
        if( okay )
        {
            if( *txt == '/'
             || *txt == '|'
             || *txt == '\\' )
                txt++;
            while( *txt==' ' || *txt=='\t' )
                txt++;
            if( *txt=='W' || *txt=='w' )
            {
                if( store )
                    white = true;
                txt++;
            }
            else if( *txt=='B' || *txt=='b' )
            {
                if( store )
                    white = false;
                txt++;
            }
            else
                okay = false;
        }

        // Castling flags
        if( okay )
        {
            while( *txt==' ' || *txt=='\t' )
                txt++;
            if( *txt == '-' )
                txt++;
            else
            {
                for( int i=0; i<4 && okay; i++, txt++ )
                {
                    if( *txt == 'K' )
                    {
                        if( store )
                            wking = true;
                    }
                    else if( *txt == 'Q' )
                    {
                        if( store )
                            wqueen = true;
                    }
                    else if( *txt == 'k' )
                    {
                        if( store )
                            bking = true;
                    }
                    else if( *txt == 'q' )
                    {
                        if( store )
                            bqueen = true;
                    }
                    else if( *txt == '-' )               // allow say "KQ-q "
                        ;
                    else if( *txt == ' ' || *txt=='\t' ) // or "KQq "
                        break;
                    else
                        okay = false;
                }
            }
        }

        // Enpassant target
        if( okay )
        {
            while( *txt==' ' || *txt=='\t' )
                txt++;
            if( *txt == '-' )
                txt++;
            else
            {
                char f='a', r='1';
                if( 'a'<=*txt && *txt<='h' )
                    f = *txt++;
                else if( 'A'<=*txt && *txt<='H' )
                {
                    f = *txt++;
                    f = f-'A'+'a';
                }
                else
                    okay = false;
                if( okay )
                {
                    if( '1'<=*txt && *txt<='8' )
                        r = *txt++;
                    else
                        okay = false;
                }
                if( okay && store )
                    enpassant_target = SQ(f,r);
            }
        }

        // Half move clock
        if( okay )
        {
            okay=false;
            while( *txt==' ' || *txt=='\t' )
                txt++;
            temp = atoi(txt);
            if( temp >= 0 )
            {
                while( isascii(*txt) && isdigit(*txt) )
                {
                    okay = true;
                    txt++;
                }
                if( okay && store )
                    half_move_clock = temp;
            }
        }

        // Full move count
        if( okay )
        {
            while( *txt==' ' || *txt=='\t' )
                txt++;
            temp = atoi(txt);
            if( temp < 0 )
                okay = false;
            else if( store )
                full_move_count = temp;
        }
    }
    return( okay );
}


/****************************************************************************
 * Publish chess position and supplementary info in forsyth notation
 ****************************************************************************/
std::string ChessPosition::ForsythPublish()
{
    int i, empty=0, file=0, rank=7, save_file=0, save_rank=0;
    Square sq;
    char p;
    std::string str;

    // Squares
    for( i=0; i<64; i++ )
    {
        sq = SQ('a'+file,'1'+rank);
        if( sq == enpassant_target )
        {
            save_file = file;
            save_rank = rank;
        }
        p = squares[sq];
        if( p == ' ' )
            empty++;
        else
        {
            if( empty )
            {
                char count = '0' + (char)empty;
                str += count;
                empty = 0;
            }
            str += p;
        }
        file++;
        if( file == 8 )
        {
            if( empty )
            {
                char count = '0'+(char)empty;
                str += count;
            }
            if( rank )
                str += '/';
            empty = 0;
            file = 0;
            rank--;
        }
    }

    // Who to move
    str += ' ';
    str += (white?'w':'b');

    // Castling flags
    str += ' ';
    if( !wking_allowed() && !wqueen_allowed() && !bking_allowed() && !bqueen_allowed() )
        str += '-';
    else
    {
        if( wking_allowed() )
            str += 'K';
        if( wqueen_allowed() )
            str += 'Q';
        if( bking_allowed() )
            str += 'k';
        if( bqueen_allowed() )
            str += 'q';
    }

    // Enpassant target square
    str += ' ';
    if( enpassant_target==SQUARE_INVALID || save_rank==0 )
        str += '-';
    else
    {
        char file2 = 'a'+(char)save_file;
        str += file2;
        char rank2 = '1'+(char)save_rank;
        str += rank2;
    }

    // Counts
    char buf[40];
    sprintf( buf, " %d %d", half_move_clock, full_move_count );
    str += buf;
    return( str );
}


/****************************************************************************
 * Compress chess position
 ****************************************************************************/
unsigned short ChessPosition::Compress( CompressedPosition &dst ) const
{
    ChessPosition src = *this;
    int i;

    // Optimisation: Clear the last 1/3 of the 24 bytes of storage only
    //  as the first 2/3 are cleared inherently as even 64 empty squares
    //  require 64 * 2 bits = 16 bytes.
    for( i = ( 2 * nbrof(dst.ints) ) / 3; i<nbrof(dst.ints); i++ )
    {
        dst.ints[i] = 0;
    }

    // Encode "castling possible" as opposition pawn on rook's home square
    //  (which is otherwise impossible)
    if( squares[e1] == 'K' )
    {
        if( src.wking && src.squares[h1]=='R' )
            src.squares[h1] = 'p';
        if( src.wqueen && src.squares[a1]=='R' )
            src.squares[a1] = 'p';
    }
    if( src.squares[e8] == 'k' )
    {
        if( src.bking && src.squares[h8]=='r' )
            src.squares[h8] = 'P';
        if( src.bqueen && src.squares[a8]=='r' )
            src.squares[a8] = 'P';
    }

    // Encode enpassant as friendly pawn on 1st rank (otherwise impossible).
    //  Since a friendly pawn must be on 4th rank, save the actual 1st rank
    //  occupant in the 4th rank square.
    // Don't encode it unless an enpassant move is actually possible. Eg
    // after 1.c3 e5 2.c4 we should have the sicilian defence position with
    // colours reversed. Don't make it different to the position after
    // 1.e4 c5 because c6 is an enpassant target in the latter case.

    // We now use a nice improved enpassant algorithm - the old ugly algorithm
    //  has a bug.
    //  To get the old book format exactly, use the ugly algorithm, change the
    //  book version back to 3, and undo the other bug fix in this file, by
    //  changing the hash shift from 16 (correct) back to 4 (incorrect - search
    //  for the other bug comment in this function).
    #if 1
    // White captures enpassant on a6,b6...h6
    Square ep_target = src.groomed_enpassant_target();
    if( src.white && ep_target!=SQUARE_INVALID )
    {
        int idx = ep_target+8; //idx = SOUTH(enpassant_target) is black pawn
        src.squares[idx] = src.squares[idx-24]; // store 1st rank
        src.squares[idx-24] = 'p';              // indicate ep
    }

    // Black captures enpassant on a3,b3...h3
    else if( !src.white && ep_target!=SQUARE_INVALID )
    {
        int idx = ep_target-8; //idx = NORTH(enpassant_target) is white pawn
        src.squares[idx] = src.squares[idx+24]; // store 1st rank
        src.squares[idx+24] = 'P';              // indicate ep
    }

    // This old ugly one has a bug ... (search for bug below)
    #else

    // White captures enpassant on a6,b6...h6
    bool swap = false;
    if( src.white && a6<=src.enpassant_target && src.enpassant_target<=h6 )
    {
        int idx = src.enpassant_target+8; //idx = SOUTH(src.enpassant_target)
        if( src.enpassant_target==a6 && src.squares[idx+1]=='P' )
            swap = true;
        else if( src.enpassant_target==h6 && src.squares[idx-1]=='P' )
            swap = true;
        else if( src.squares[idx-1]=='P' || src.squares[idx+1]=='P' )
            // BUG! src.enpassant_target can be a6 or h6 in this clause
            swap = true;
        if( swap )
        {
            src.squares[idx] = src.squares[idx-24]; // store 1st rank
            src.squares[idx-24] = 'p';              // indicate ep
        }
    }

    // Black captures enpassant on a3,b3...h3
    else if( !src.white && a3<=src.enpassant_target && src.enpassant_target<=h3 )
    {
        int idx = src.enpassant_target-8; //idx = NORTH(src.enpassant_target)
        if( src.enpassant_target==a3 && src.squares[idx+1]=='p' )
            swap = true;
        else if( src.enpassant_target==h3 && src.squares[idx-1]=='p' )
            swap = true;
        else if( src.squares[idx-1]=='p' || src.squares[idx+1]=='p' )
            // BUG! src.enpassant_target can be a3 or h3 in this clause
            swap = true;
        if( swap )
        {
            src.squares[idx] = src.squares[idx+24]; // store 1st rank
            src.squares[idx+24] = 'P';              // indicate ep
        }
    }
    #endif


    // Encode empty square with 2 bits, and other 12 possibilities with
    //  all the other possible nibbles that don't start with those 2 bits
    #define CEMPTY      2   // 10
    #define CWROOK      0   // 0000
    #define CWKNIGHT    1   // 0001
    #define CWBISHOP    2   // 0010
    #define CWQUEEN     3   // 0011
    #define CWKING      4   // 0100
    #define CWPAWN      5   // 0101
    #define CBROOK      6   // 0110
    #define CBKNIGHT    7   // 0111
    #define CBBISHOP   12   // 1100
    #define CBQUEEN    13   // 1101
    #define CBKING     14   // 1110
    #define CBPAWN     15   // 1111

    unsigned char c, *p, *base;
    int bit=7;
    base = p = dst.storage;
    bool second_king=false;
    int kings=0;
    bool err=false;
    for( i=0; !err && i<64; i++ )
    {
        if( p-base >= 24 )   // never need more than 24 bytes in legal position
        {
            err = true;
            break;
        }
        bool empty=false;
        c = CEMPTY;
        switch( src.squares[i] )
        {
            case 'R':   c = CWROOK;     break;
            case 'N':   c = CWKNIGHT;   break;
            case 'B':   c = CWBISHOP;   break;
            case 'Q':   c = CWQUEEN;    break;
            case 'K':   c = CWKING;     kings++; if( kings==2 ) second_king=true; break;
            case 'P':   c = CWPAWN;     break;
            case 'r':   c = CBROOK;     break;
            case 'n':   c = CBKNIGHT;   break;
            case 'b':   c = CBBISHOP;   break;
            case 'q':   c = CBQUEEN;    break;
            case 'k':   c = CBKING;     kings++; if( kings==2 ) second_king=true; break;
            case 'p':   c = CBPAWN;     break;
            case ' ':
            default:    empty = true;   break;
        }

        // Encode black to move as two kings same colour (2 white kings if
        //  white king first, 2 black kings otherwise)
        if( second_king )
        {
            second_king = false;
            if( !src.white )
            {
                if( c == CBKING )
                    c = CWKING;
                else
                    c = CBKING;
            }
        }

        // Shift and mask 2 or 4 bits into storage
        if( empty )
        {
            switch( bit )
            {
                case 7: *p = (CEMPTY<<6);
                        bit = 5;
                        break;
                case 5: *p = (*p&0xc0) | (CEMPTY<<4);
                        bit = 3;
                        break;
                case 3: *p = (*p&0xf0) | (CEMPTY<<2);
                        bit = 1;
                        break;
                case 1: *p = (*p&0xfc) | (CEMPTY);
                        p++;
                        bit = 7;
                        break;
            }
        }
        else
        {
            switch( bit )
            {
                case 7: *p = (c<<4);
                        bit = 3;
                        break;
                case 5: *p = (*p&0xc0) | (c<<2);
                        bit = 1;
                        break;
                case 3: *p = (*p&0xf0) | (c);
                        p++;
                        bit = 7;
                        break;
                case 1:
                {
                        *p = (*p&0xfc) | ((c>>2)&0x03);
                        p++;
                        if( p-base >= 24 )
                            err = true;
                        else
                        {
                            *p = ((c<<6)&0xc0);
                            bit = 5;
                        }
                        break;
                }
            }
        }
    }

    // Create hash
    unsigned int big_hash = 0;
    for( i=0; i<nbrof(dst.ints); i++ )
    {
        big_hash ^= dst.ints[i];
    }
    unsigned short hash=0;
    for( i=0; i < sizeof(unsigned int)/sizeof(unsigned short); i++ )
    {
        hash ^= (big_hash&0xffff);
        big_hash = big_hash >> 16;  // BUG FIX! This used to be >> 4 which
                                    //  results in much worse hash distribution
                                    //  than ideal. Fixed at the same time as
                                    //  change of book version from 3 -> 4
    }
    return hash;
}

/****************************************************************************
 * Decompress chess position
 ****************************************************************************/
void ChessPosition::Decompress( const CompressedPosition &src )
{
    unsigned char c;
    const unsigned char *p;
    int idx, bit=7;
    p = src.storage;
    char first_king='\0';
    bool second_king=false;
    int kings=0;

    // Clear some supplementary type info
    half_move_clock = 0;
    full_move_count = 1;
    enpassant_target = SQUARE_INVALID;
    white  = false;
    wking  = false;
    wqueen = false;
    bking  = false;
    bqueen = false;

    // The stages of the decompression process must be the reverse of the
    //  compression process. So unscramble 64 squares, then enpassant, then
    //  castling
    for( idx=0; idx<64; idx++ )
    {
        bool empty = false;
        c = ' ';
        switch( bit )
        {
            case 7:
            {
                if( ((*p>>6)&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 5;
                }
                else
                {
                    c = (*p>>4) & 0x0f;
                    bit = 3;
                }
                break;
            }
            case 5:
            {
                if( ((*p>>4)&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 3;
                }
                else
                {
                    c = (*p>>2) & 0x0f;
                    bit = 1;
                }
                break;
            }
            case 3:
            {
                if( ((*p>>2)&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 1;
                }
                else
                {
                    c = *p & 0x0f;
                    bit = 7;
                    p++;
                }
                break;
            }
            case 1:
            {
                if( (*p&0x03) == CEMPTY )
                {
                    empty = true;
                    bit = 7;
                    p++;
                }
                else
                {
                    c  = ( (*p<<2) & 0x0c );
                    bit = 5;
                    p++;
                    c |= ( (*p>>6) & 0x03 );
                }
                break;
            }
        }
        if( !empty )
        {
            switch( c )
            {
                default:         c = ' ';   break;
                case CWROOK:     c = 'R';   break;
                case CWKNIGHT:   c = 'N';   break;
                case CWBISHOP:   c = 'B';   break;
                case CWQUEEN:    c = 'Q';   break;
                case CWKING:     c = 'K';   kings++;
                                 if( kings == 1 )
                                     first_king = c;
                                 else if( kings == 2)
                                     second_king=true;
                                 break;
                case CWPAWN:     c = 'P';   break;
                case CBROOK:     c = 'r';   break;
                case CBKNIGHT:   c = 'n';   break;
                case CBBISHOP:   c = 'b';   break;
                case CBQUEEN:    c = 'q';   break;
                case CBKING:     c = 'k';   kings++;
                                 if( kings == 1 )
                                     first_king = c;
                                 else if( kings == 2)
                                     second_king=true;
                                 break;
                case CBPAWN:     c = 'p';   break;
            }
        }

        // Decode black to move as two kings same colour (2 white kings if
        //  white king first, 2 black kings otherwise)
        if( second_king )
        {
            second_king = false;
            white = (c!=first_king);
            if( !white )    // if black, swap second king
            {
                if( c == 'K' )
                    c = 'k';
                else
                    c = 'K';
            }
        }
        squares[idx] = c;
    }
    squares[idx] = '\0';    // idx=64, terminating '\0'

    // Decode enpassant as friendly pawn on 1st rank (otherwise impossible).
    //  Since a friendly pawn must be on 4th rank, save the actual 1st rank
    //  occupant in the 4th rank square.
    bool ep=false;
    for( idx=0; !ep && idx<8; idx++ )   // loop thru black 1st rank
    {
        if( squares[idx] == 'p' )
        {
            ep = true;
            squares[idx]     = squares[idx+24];
            squares[idx+24]  = 'p';
            enpassant_target = (Square)(idx+16);
        }
    }
    for( idx=56; !ep && idx<64; idx++ ) // loop thru white 1st rank
    {
        if( squares[idx] == 'P' )
        {
            ep = true;
            squares[idx]     = squares[idx-24];
            squares[idx-24]  = 'P';
            enpassant_target = (Square)(idx-16);
        }
    }

    // Decode "castling possible" as opposition pawn on rook's home square
    //  (which is otherwise impossible)
    if( squares[e1] == 'K' )
    {
        if( squares[h1] == 'p' )
        {
            squares[h1] = 'R';
            wking = true;
        }
        if( squares[a1] == 'p' )
        {
            squares[a1] = 'R';
            wqueen = true;
        }
    }
    if( squares[e8] == 'k' )
    {
        if( squares[h8] == 'P' )
        {
            squares[h8] = 'r';
            bking = true;
        }
        if( squares[a8] == 'P' )
        {
            squares[a8] = 'r';
            bqueen = true;
        }
    }
}

/****************************************************************************
 * Calculate a hash value for position (not same as CompressPosition algorithm hash)
 ****************************************************************************/
uint32_t ChessPosition::HashCalculate()
{
    int32_t hash = 0;
    char *p = squares;
    for( int i=0; i<64; i++ )
    {
        char c = *p++;
        if( c < 'B' )
            c = 'a';
        else if( c > 'r' )
            c = 'a';
        hash ^= hash_lookup[i][c-'B'];
    }
    return hash;
}


/****************************************************************************
 * Incremental hash value update
 ****************************************************************************/
uint32_t ChessPosition::HashUpdate( uint32_t hash_in, Move move )
{
    uint32_t hash = hash_in;
    switch( move.special )
    {
        default:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WK_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[g1][target-'B'];  // remove target piece
            hash ^= hash_lookup[g1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash_lookup[h1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[h1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[f1][target-'B'];  // remove target piece
            hash ^= hash_lookup[f1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BK_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[g8][target-'B'];  // remove target piece
            hash ^= hash_lookup[g8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash_lookup[h8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[h8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[f8][target-'B'];  // remove target piece
            hash ^= hash_lookup[f8][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WQ_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[c1][target-'B'];  // remove target piece
            hash ^= hash_lookup[c1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash_lookup[a1][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[a1]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[d1][target-'B'];  // remove target piece
            hash ^= hash_lookup[d1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BQ_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[c8][target-'B'];  // remove target piece
            hash ^= hash_lookup[c8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash_lookup[a8][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[a8]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[d8][target-'B'];  // remove target piece
            hash ^= hash_lookup[d8][piece-'B'];   // replace with moving piece

            break;
        }
        case SPECIAL_PROMOTION_QUEEN:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'Q':'q')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_ROOK:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'R':'r')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_BISHOP:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'B':'b')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_KNIGHT:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash_lookup[move.dst][(piece=='P'?'N':'n')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WEN_PASSANT:
        {
            char piece  = 'P';
            char target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash_lookup[SOUTH(move.dst)]['p'-'B'];  // remove black pawn south of dst
            hash ^= hash_lookup[SOUTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
        case SPECIAL_BEN_PASSANT:
        {
            char piece  = 'p';
            char target = 'a';
            hash ^= hash_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash_lookup[NORTH(move.dst)]['P'-'B'];  // remove white pawn north of dst
            hash ^= hash_lookup[NORTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
    }
    return hash;
}

/****************************************************************************
 * Calculate a hash value for position (64 bit version)
 ****************************************************************************/
uint64_t ChessPosition::Hash64Calculate()
{
    int64_t hash = 0;
    char *p = squares;
    for( int i=0; i<64; i++ )
    {
        char c = *p++;
        if( c < 'B' )
            c = 'a';
        else if( c > 'r' )
            c = 'a';
        hash ^= hash64_lookup[i][c-'B'];
    }
    return hash;
}

/****************************************************************************
 * Incremental hash value update (64 bit version)
 ****************************************************************************/
uint64_t ChessPosition::Hash64Update( uint64_t hash_in, Move move )
{
    uint64_t hash = hash_in;
    switch( move.special )
    {
        default:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WK_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash64_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[g1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[g1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash64_lookup[h1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[h1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[f1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[f1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BK_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash64_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[g8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[g8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash64_lookup[h8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[h8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[f8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[f8][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WQ_CASTLING:
        {
            char piece  = 'K';
            char target = 'a';
            hash ^= hash64_lookup[e1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[c1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[c1][piece-'B'];   // replace with moving piece
            piece  = 'R';
            target = 'a';
            hash ^= hash64_lookup[a1][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[a1]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[d1][target-'B'];  // remove target piece
            hash ^= hash64_lookup[d1][piece-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_BQ_CASTLING:
        {
            char piece  = 'k';
            char target = 'a';
            hash ^= hash64_lookup[e8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[e8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[c8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[c8][piece-'B'];   // replace with moving piece
            piece  = 'r';
            target = 'a';
            hash ^= hash64_lookup[a8][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[a8]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[d8][target-'B'];  // remove target piece
            hash ^= hash64_lookup[d8][piece-'B'];   // replace with moving piece

            break;
        }
        case SPECIAL_PROMOTION_QUEEN:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'Q':'q')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_ROOK:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'R':'r')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_BISHOP:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'B':'b')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_PROMOTION_KNIGHT:
        {
            char piece  = squares[move.src];
            char target = squares[move.dst];
            if( IsEmptySquare(target) )
                target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];   // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];     // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];  // remove target piece
            hash ^= hash64_lookup[move.dst][(piece=='P'?'N':'n')-'B'];   // replace with moving piece
            break;
        }
        case SPECIAL_WEN_PASSANT:
        {
            char piece  = 'P';
            char target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash64_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash64_lookup[SOUTH(move.dst)]['p'-'B'];  // remove black pawn south of dst
            hash ^= hash64_lookup[SOUTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
        case SPECIAL_BEN_PASSANT:
        {
            char piece  = 'p';
            char target = 'a';
            hash ^= hash64_lookup[move.src][piece-'B'];       // remove moving piece
            hash ^= hash64_lookup[move.src]['a'-'B'];         // replace with empty square
            hash ^= hash64_lookup[move.dst][target-'B'];      // remove target piece
            hash ^= hash64_lookup[move.dst][piece-'B'];       // replace with moving piece
            hash ^= hash64_lookup[NORTH(move.dst)]['P'-'B'];  // remove white pawn north of dst
            hash ^= hash64_lookup[NORTH(move.dst)]['a'-'B'];  // replace with empty square
            break;
        }
    }
    return hash;
}

/****************************************************************************
 * ChessRules.cpp Chess classes - Rules of chess
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

// Table indexed by Square, gives mask for DETAIL_CASTLING, such that a move
//  to (or from) that square results in castling being prohibited, eg a move
//  to e8 means that subsequently black kingside and black queenside castling
//  is prohibited
static unsigned char castling_prohibited_table[] =
{
    (unsigned char)(~BQUEEN), 0xff, 0xff, 0xff,                             // a8-d8
    (unsigned char)(~(BQUEEN+BKING)), 0xff, 0xff, (unsigned char)(~BKING),  // e8-h8
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a7-h7
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a6-h6
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a5-h5
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a4-h4
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a3-h3
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // a2-h2
    (unsigned char)(~WQUEEN), 0xff, 0xff, 0xff,                             // a1-d1
    (unsigned char)(~(WQUEEN+WKING)),  0xff, 0xff, (unsigned char)(~WKING)  // e1-h1
};

/****************************************************************************
 * Test internals, for porting to new environments etc
 *   For the moment at least, this is best used by stepping through it
 *   thoughtfully with a debugger. It is not set up to automatically
 *   check whether THC is going to work in the new environment
 ****************************************************************************/
static int log_discard( const char *, ... ) { return 0; }
bool ChessRules::TestInternals( int (*log)(const char *,...) )
{
    if( log == NULL )
        log = log_discard;
    Init();
    Move mv;
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"g1f3");
    PlayMove(mv);
    mv.TerseIn(this,"g8f6");
    PlayMove(mv);
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    bool KQkq_allowed   = wking && bking && wqueen && bqueen;
    bool KQkq_allowed_f = wking_allowed() && bking_allowed() && wqueen_allowed() && bqueen_allowed();
    mv.TerseIn(this,"h1g1");
    PlayMove(mv);
    mv.TerseIn(this,"h8g8");
    PlayMove(mv);
    mv.TerseIn(this,"g1h1");  // return of rook to h1 clears wking
    PlayMove(mv);
    log( "WKING castling not allowed %08x\n", *DETAIL_ADDR );
    bool Qkq_allowed  = !wking && bking && wqueen && bqueen;
    bool Qkq_allowed_f = !wking_allowed() && !bking_allowed() && wqueen_allowed() && bqueen_allowed();
    // The _f version is different, !bking_allowed() because it checks and finds no rook on h8
    mv.TerseIn(this,"g8h8");  // return of rook to h8 clears bking
    PlayMove(mv);
    log( "WKING BKING castling not allowed %08x\n", *DETAIL_ADDR );
    bool Qq_allowed   = !wking && !bking && wqueen && bqueen;
    bool Qq_allowed_f = !wking_allowed() && !bking_allowed() && wqueen_allowed() && bqueen_allowed();
    mv.TerseIn(this,"b1c3");
    PlayMove(mv);
    mv.TerseIn(this,"b8c6");
    PlayMove(mv);
    mv.TerseIn(this,"a1b1");
    PlayMove(mv);
    mv.TerseIn(this,"a8b8");
    PlayMove(mv);
    mv.TerseIn(this,"b1a1");  // return of rook to a1 clears wqueen
    PlayMove(mv);
    log( "WKING BKING WQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool q_allowed   = !wking && !bking && !wqueen && bqueen;
    bool q_allowed_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    // The _f version is different, !bqueen_allowed() because it checks and finds no rook on a8
    mv.TerseIn(this,"b8a8");  // return of rook to a8 clears bqueen
    PlayMove(mv);
    log( "WKING BKING WQUEEN BQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool none_allowed   = !wking && !bking && !wqueen && !bqueen;
    bool none_allowed_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    ChessPosition::Init();
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"e2e3");
    PlayMove(mv);
    mv.TerseIn(this,"e7e6");
    PlayMove(mv);
    log( "All castling allowed %08x\n", *DETAIL_ADDR );
    mv.TerseIn(this,"e1e2");
    PlayMove(mv);
    mv.TerseIn(this,"e8e7");
    PlayMove(mv);
    mv.TerseIn(this,"e2e1");  // return of king to e1 clears wking, wqueen
    PlayMove(mv);
    log( "WKING WQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool kq_allowed   = !wking && bking && !wqueen && bqueen;
    bool kq_allowed_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    // The _f version is different, !bking_allowed() and !bqueen_allowed() because they check and finds no king on e8
    mv.TerseIn(this,"e7e8");
    PlayMove(mv);
    log( "WKING WQUEEN BKING BQUEEN castling not allowed %08x\n", *DETAIL_ADDR );
    bool none_allowed2   = !wking && !bking && !wqueen && !bqueen;
    bool none_allowed2_f = !wking_allowed() && !bking_allowed() && !wqueen_allowed() && !bqueen_allowed();
    const char *fen = "b3k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
    Move move;
    Forsyth(fen);
    log( "Addresses etc.;\n" );
    log( " this = 0x%p\n",                         this );
    log( " (void *)this = 0x%p\n",                 (void *)this );
    log( " &white = 0x%p\n",                       &white );
    log( " &squares[0] = 0x%p\n",                  &squares[0] );
    log( " &half_move_clock = 0x%p\n",             &half_move_clock );
    log( " &full_move_count = 0x%p\n",             &full_move_count );
    log( " size to end of full_move_count = %lu", ((char *)&full_move_count - (char *)this) + sizeof(full_move_count) );
    log( " sizeof(ChessPosition) = %lu (should be 4 more than size to end of full_move_count)\n",
           sizeof(ChessPosition) );
    log( " sizeof(Move) = %lu\n",                  sizeof(Move) );

    log( " sizeof(ChessPositionRaw) = %lu\n", sizeof(ChessPositionRaw) );
    log( " (offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL) =");
    log( " %lu + %lu + %lu = %lu\n",
           offsetof(ChessPositionRaw,full_move_count), sizeof(full_move_count), sizeof(DETAIL),
           offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL)
           );
    for( int i=0; i<6; i++ )
    {
        switch(i)
        {
            case 0: move.TerseIn(this,"h1h2");    break;
            case 1: move.TerseIn(this,"a8h1");    break;
            case 2: move.TerseIn(this,"e1c1");    break;
            case 3: move.TerseIn(this,"h1a8");    break;
            case 4: move.TerseIn(this,"c1b1");    break;
            case 5: move.TerseIn(this,"e8g8");    break;
        }
        unsigned char *p = (unsigned char *)DETAIL_ADDR;
        log( " DETAIL_ADDR = 0x%p\n",  p );
        log( " DETAIL_ADDR[0] = %02x\n",  p[0] );
        log( " DETAIL_ADDR[1] = %02x\n",  p[1] );
        log( " DETAIL_ADDR[2] = %02x\n",  p[2] );
        log( " DETAIL_ADDR[3] = %02x\n",  p[3] );
        log( "Before %s: enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               move.TerseOut().c_str(),
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PushMove(move);
        log( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PopMove(move);
        log( "After PopMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
        PushMove(move);
        log( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
               " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
               enpassant_target,
               wking_square,
               bking_square,
               wking ?"true":"false",
               wqueen?"true":"false",
               bking ?"true":"false",
               bqueen?"true":"false" );
    }

    // Later, extend this to check addresses etc
    return KQkq_allowed && Qkq_allowed && Qq_allowed && q_allowed && none_allowed && kq_allowed && none_allowed2 &&
           KQkq_allowed_f && Qkq_allowed_f && Qq_allowed_f && q_allowed_f && none_allowed_f && kq_allowed_f && none_allowed2_f;
}

/****************************************************************************
 * Play a move
 ****************************************************************************/
void ChessRules::PlayMove( Move imove )
{
    // Legal move - save it in history
    history[history_idx++] = imove; // ring array

    // Update full move count
    if( !white )
        full_move_count++;

    // Update half move clock
    if( squares[imove.src] == 'P' || squares[imove.src] == 'p' )
        half_move_clock=0;   // pawn move
    else if( !IsEmptySquare(imove.capture) )
        half_move_clock=0;   // capture
    else
        half_move_clock++;   // neither pawn move or capture

    // Actually play the move
    PushMove( imove );
}


/****************************************************************************
 * Create a list of all legal moves in this position
 ****************************************************************************/
void ChessRules::GenLegalMoveList( vector<Move> &moves )
{
    MOVELIST movelist;
    GenLegalMoveList( &movelist );
    for( int i=0; i<movelist.count; i++ )
        moves.push_back( movelist.moves[i] );
}

/****************************************************************************
 * Create a list of all legal moves in this position, with extra info
 ****************************************************************************/
void ChessRules::GenLegalMoveList( vector<Move>  &moves,
                                   vector<bool>  &check,
                                   vector<bool>  &mate,
                                   vector<bool>  &stalemate )
{
    MOVELIST movelist;
    bool bcheck[MAXMOVES];
    bool bmate[MAXMOVES];
    bool bstalemate[MAXMOVES];
    GenLegalMoveList( &movelist, bcheck, bmate, bstalemate  );
    for( int i=0; i<movelist.count; i++ )
    {
        moves.push_back    ( movelist.moves[i] );
        check.push_back    ( bcheck[i] );
        mate.push_back     ( bmate[i] );
        stalemate.push_back( bstalemate[i] );
    }
}

/****************************************************************************
 * Create a list of all legal moves in this position
 ****************************************************************************/
void ChessRules::GenLegalMoveList( MOVELIST *list )
{
    int i, j;
    bool okay;
    MOVELIST list2;

    // Generate all moves, including illegal (eg put king in check) moves
    GenMoveList( &list2 );

    // Loop copying the proven good ones
    for( i=j=0; i<list2.count; i++ )
    {
        PushMove( list2.moves[i] );
        okay = Evaluate();
        PopMove( list2.moves[i] );
        if( okay )
            list->moves[j++] = list2.moves[i];
    }
    list->count  = j;
}

/****************************************************************************
 * Create a list of all legal moves in this position, with extra info
 ****************************************************************************/
void ChessRules::GenLegalMoveList( MOVELIST *list, bool check[MAXMOVES],
                                                    bool mate[MAXMOVES],
                                                    bool stalemate[MAXMOVES] )
{
    int i, j;
    bool okay;
    TERMINAL terminal_score;
    MOVELIST list2;

    // Generate all moves, including illegal (eg put king in check) moves
    GenMoveList( &list2 );

    // Loop copying the proven good ones
    for( i=j=0; i<list2.count; i++ )
    {
        PushMove( list2.moves[i] );
        okay = Evaluate(terminal_score);
        Square king_to_move = (Square)(white ? wking_square : bking_square );
        bool bcheck = false;
        if( AttackedPiece(king_to_move) )
            bcheck = true;
        PopMove( list2.moves[i] );
        if( okay )
        {
            stalemate[j] = (terminal_score==TERMINAL_WSTALEMATE ||
                            terminal_score==TERMINAL_BSTALEMATE);
            mate[j]      = (terminal_score==TERMINAL_WCHECKMATE ||
                            terminal_score==TERMINAL_BCHECKMATE);
            check[j]     = mate[j] ? false : bcheck;
            list->moves[j++] = list2.moves[i];
        }
    }
    list->count  = j;
}

/****************************************************************************
 * Check draw rules (50 move rule etc.)
 ****************************************************************************/
bool ChessRules::IsDraw( bool white_asks, DRAWTYPE &result )
{
    bool   draw=false;

    // Insufficient mating material
    draw =  IsInsufficientDraw( white_asks, result );

    // 50 move rule
    if( !draw && half_move_clock>=100 )
    {
        result = DRAWTYPE_50MOVE;
        draw = true;
    }

    // 3 times repetition,
    if( !draw && GetRepetitionCount()>=3 )
    {
        result = DRAWTYPE_REPITITION;
        draw = true;
    }

    if( !draw )
        result = NOT_DRAW;
    return( draw );
}

/****************************************************************************
 * Get number of times position has been repeated
 ****************************************************************************/
int ChessRules::GetRepetitionCount()
{
    int matches=0;

    //  Save those aspects of current position that are changed by multiple
    //  PopMove() calls as we search backwards (i.e. squares, white,
    //  detail, detail_idx)
    char save_squares[sizeof(squares)];
    memcpy( save_squares, squares, sizeof(save_squares) );
    unsigned char save_detail_idx = detail_idx;  // must be unsigned char
    bool          save_white      = white;
    unsigned char idx             = history_idx; // must be unsigned char
    DETAIL_SAVE;

    // Search backwards ....
    int nbr_half_moves = (full_move_count-1)*2 + (!white?1:0);
    if( nbr_half_moves > nbrof(history)-1 )
        nbr_half_moves = nbrof(history)-1;
    if( nbr_half_moves > nbrof(detail_stack)-1 )
        nbr_half_moves = nbrof(detail_stack)-1;
    for( int i=0; i<nbr_half_moves; i++ )
    {
        Move m = history[--idx];
        if( m.src == m.dst )
            break;  // unused history is set to zeroed memory
        PopMove(m);

        // ... looking for matching positions
        if( white    == save_white      && // quick ones first!
            DETAIL_EQ_KING_POSITIONS    &&
            0 == memcmp(squares,save_squares,sizeof(squares) )
            )
        {
            matches++;
            if( !DETAIL_EQ_ALL )    // Castling flags and/or enpassant target different?
            {
                // It might not be a match (but it could be - we have to unpack what the differences
                //  really mean)
                bool revoke_match = false;

                // Revoke match if different value of en-passant target square means different
                //  en passant possibilities
                if( !DETAIL_EQ_EN_PASSANT )
                {
                    int ep_saved = (int)(tmp&0xff);
                    int ep_now   = (int)(*DETAIL_ADDR&0xff);

                    // Work out whether each en_passant is a real one, i.e. is there an opposition
                    //  pawn in place to capture (if not it's just a double pawn advance with no
                    //  actual enpassant consequences)
                    bool real=false;
                    int ep = ep_saved;
                    char const *squ = save_squares;
                    for( int j=0; j<2; j++ )
                    {
                        if( ep == 0x10 )    // 0x10 = a6
                        {
                             real = (squ[SE(ep)] == 'P');
                        }
                        else if( 0x10<ep && ep<0x17 )   // 0x10 = h6
                        {
                             real = (squ[SW(ep)] == 'P' || squ[SE(ep)] == 'P');
                        }
                        else if( ep==0x17 )
                        {
                             real = (squ[SW(ep)] == 'P');
                        }
                        else if( 0x28==ep )   // 0x28 = a3
                        {
                             real = (squ[NE(ep)] == 'p');
                        }
                        else if( 0x28<ep && ep<0x2f )   // 0x2f = h3
                        {
                             real = (squ[NE(ep)] == 'p' || squ[NW(ep)] == 'p');
                        }
                        else if( ep==0x2f )
                        {
                             real = (squ[NW(ep)] == 'p' );
                        }
                        if( j > 0 )
                            ep_now = real?ep:0x40;      // evaluate second time through
                        else
                        {
                            ep_saved = real?ep:0x40;    // evaluate first time through
                            ep = ep_now;                // setup second time through
                            squ = squares;
                            real = false;
                        }
                    }

                    // If for example one en_passant is real and the other not, it's not a real match
                    if( ep_saved != ep_now )
                        revoke_match = true;
                }

                // Revoke match if different value of castling flags means different
                //  castling possibilities
                if( !revoke_match && !DETAIL_EQ_CASTLING )
                {
                    bool wking_saved  = save_squares[e1]=='K' && save_squares[h1]=='R' && (int)(tmp&(WKING<<24));
                    bool wking_now    = squares[e1]=='K' && squares[h1]=='R' && (int)(*DETAIL_ADDR&(WKING<<24));
                    bool bking_saved  = save_squares[e8]=='k' && save_squares[h8]=='r' && (int)(tmp&(BKING<<24));
                    bool bking_now    = squares[e8]=='k' && squares[h8]=='r' && (int)(*DETAIL_ADDR&(BKING<<24));
                    bool wqueen_saved = save_squares[e1]=='K' && save_squares[a1]=='R' && (int)(tmp&(WQUEEN<<24));
                    bool wqueen_now   = squares[e1]=='K' && squares[a1]=='R' && (int)(*DETAIL_ADDR&(WQUEEN<<24));
                    bool bqueen_saved = save_squares[e8]=='k' && save_squares[a8]=='r' && (int)(tmp&(BQUEEN<<24));
                    bool bqueen_now   = squares[e8]=='k' && squares[a8]=='r' && (int)(*DETAIL_ADDR&(BQUEEN<<24));
                    revoke_match = ( wking_saved != wking_now ||
                                     bking_saved != bking_now ||
                                     wqueen_saved != wqueen_now ||
                                     bqueen_saved != bqueen_now );
                }

                // If the real castling or enpassant possibilities differ, it's not a match
                //  At one stage we just did a naive binary match of the details - not good enough. For example
                //  a rook moving away from h1 doesn't affect the WKING flag, but does disallow white king side
                //  castling
                if( revoke_match )
                     matches--;
            }
        }

        // For performance reasons, abandon search early if pawn move
        //  or capture
        if( squares[m.src]=='P' || squares[m.src]=='p' || !IsEmptySquare(m.capture) )
            break;
    }

    // Restore current position
    memcpy( squares, save_squares, sizeof(squares) );
    white      = save_white;
    detail_idx = save_detail_idx;
    DETAIL_RESTORE;
    return( matches+1 );  // +1 counts original position
}

/****************************************************************************
 * Check insufficient material draw rule
 ****************************************************************************/
bool ChessRules::IsInsufficientDraw( bool white_asks, DRAWTYPE &result )
{
    char   piece;
    int    piece_count=0;
    bool   bishop_or_knight=false, lone_wking=true, lone_bking=true;
    bool   draw=false;

    // Loop through the board
    for( Square square=a8; square<=h1; ++square )
    {
        piece = squares[square];
        switch( piece )
        {
            case 'B':
            case 'b':
            case 'N':
            case 'n':       bishop_or_knight=true;  // and fall through
            case 'Q':
            case 'q':
            case 'R':
            case 'r':
            case 'P':
            case 'p':       piece_count++;
                            if( isupper(piece) )
                                lone_wking = false;
                            else
                                lone_bking = false;
                            break;
        }
        if( !lone_wking && !lone_bking )
            break;  // quit early for performance
    }

    // Automatic draw if K v K or K v K+N or K v K+B
    //  (note that K+B v K+N etc. is not auto granted due to
    //   selfmates in the corner)
    if( piece_count==0 ||
        (piece_count==1 && bishop_or_knight)
      )
    {
        draw = true;
        result = DRAWTYPE_INSUFFICIENT_AUTO;
    }
    else
    {

        // Otherwise side playing against lone K can claim a draw
        if( white_asks && lone_bking )
        {
            draw   = true;
            result = DRAWTYPE_INSUFFICIENT;
        }
        else if( !white_asks && lone_wking )
        {
            draw   = true;
            result = DRAWTYPE_INSUFFICIENT;
        }
    }
    return( draw );
}

/****************************************************************************
 * Generate a list of all possible moves in a position
 ****************************************************************************/
void ChessRules::GenMoveList( MOVELIST *l )
{
    Square square;

    // Convenient spot for some asserts
    //  Have a look at TestInternals() for this,
    //   A ChessPositionRaw should finish with 32 bits of detail information
    //   (see DETAIL macros, this assert() checks that)
    assert( sizeof(ChessPositionRaw) ==
               (offsetof(ChessPositionRaw,full_move_count) + sizeof(full_move_count) + sizeof(DETAIL))
          );

    // We also rely on Moves being 32 bits for the implementation of Move
    //  bitwise == and != operators
    assert( sizeof(Move) == sizeof(int32_t) );

    // Clear move list
    l->count  = 0;   // set each field for each move

    // Loop through all squares
    for( square=a8; square<=h1; ++square )
    {

        // If square occupied by a piece of the right colour
        char piece=squares[square];
        if( (white&&IsWhite(piece)) || (!white&&IsBlack(piece)) )
        {

            // Generate moves according to the occupying piece
            switch( piece )
            {
                case 'P':
                {
                    WhitePawnMoves( l, square );
                    break;
                }
                case 'p':
                {
                    BlackPawnMoves( l, square );
                    break;
                }
                case 'N':
                case 'n':
                {
                    const lte *ptr = knight_lookup[square];
                    ShortMoves( l, square, ptr, NOT_SPECIAL );
                    break;
                }
                case 'B':
                case 'b':
                {
                    const lte *ptr = bishop_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'R':
                case 'r':
                {
                    const lte *ptr = rook_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'Q':
                case 'q':
                {
                    const lte *ptr = queen_lookup[square];
                    LongMoves( l, square, ptr );
                    break;
                }
                case 'K':
                case 'k':
                {
                    KingMoves( l, square );
                    break;
                }
            }
        }
    }
}

/****************************************************************************
 * Generate moves for pieces that move along multi-move rays (B,R,Q)
 ****************************************************************************/
void ChessRules::LongMoves( MOVELIST *l, Square square, const lte *ptr )
{
    Move *m=&l->moves[l->count];
    Square dst;
    lte nbr_rays = *ptr++;
    while( nbr_rays-- )
    {
        lte ray_len = *ptr++;
        while( ray_len-- )
        {
            dst = (Square)*ptr++;
            char piece=squares[dst];

            // If square not occupied (empty), add move to list
            if( IsEmptySquare(piece) )
            {
                m->src     = square;
                m->dst     = dst;
                m->capture = ' ';
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }

            // Else must move to end of ray
            else
            {
                ptr += ray_len;
                ray_len = 0;

                // If not occupied by our man add a capture
                if( (white&&IsBlack(piece)) || (!white&&IsWhite(piece)) )
                {
                    m->src     = square;
                    m->dst     = dst;
                    m->special = NOT_SPECIAL;
                    m->capture = piece;
                    l->count++;
                    m++;
                }
            }
        }
    }
}

/****************************************************************************
 * Generate moves for pieces that move along single move rays (N,K)
 ****************************************************************************/
void ChessRules::ShortMoves( MOVELIST *l, Square square,
                                         const lte *ptr, SPECIAL special  )
{
    Move *m=&l->moves[l->count];
    Square dst;
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        dst = (Square)*ptr++;
        char piece = squares[dst];

        // If square not occupied (empty), add move to list
        if( IsEmptySquare(piece) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Else if occupied by enemy man, add move to list as a capture
        else if( (white&&IsBlack(piece)) || (!white&&IsWhite(piece)) )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = special;
            m->capture = piece;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of king moves
 ****************************************************************************/
void ChessRules::KingMoves( MOVELIST *l, Square square )
{
    const lte *ptr = king_lookup[square];
    ShortMoves( l, square, ptr, SPECIAL_KING_MOVE );

    // Generate castling king moves
    Move *m;
    m = &l->moves[l->count];

    // White castling
    if( square == e1 )   // king on e1 ?
    {

        // King side castling
        if(
            squares[g1] == ' '   &&
            squares[f1] == ' '   &&
            squares[h1] == 'R'   &&
            (wking)            &&
            !AttackedSquare(e1,false) &&
            !AttackedSquare(f1,false) &&
            !AttackedSquare(g1,false)
          )
        {
            m->src     = e1;
            m->dst     = g1;
            m->special = SPECIAL_WK_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Queen side castling
        if(
            squares[b1] == ' '         &&
            squares[c1] == ' '         &&
            squares[d1] == ' '         &&
            squares[a1] == 'R'         &&
            (wqueen)                 &&
            !AttackedSquare(e1,false)  &&
            !AttackedSquare(d1,false)  &&
            !AttackedSquare(c1,false)
          )
        {
            m->src     = e1;
            m->dst     = c1;
            m->special = SPECIAL_WQ_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }
    }

    // Black castling
    if( square == e8 )   // king on e8 ?
    {

        // King side castling
        if(
            squares[g8] == ' '         &&
            squares[f8] == ' '         &&
            squares[h8] == 'r'         &&
            (bking)                  &&
            !AttackedSquare(e8,true) &&
            !AttackedSquare(f8,true) &&
            !AttackedSquare(g8,true)
          )
        {
            m->src     = e8;
            m->dst     = g8;
            m->special = SPECIAL_BK_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }

        // Queen side castling
        if(
            squares[b8] == ' '         &&
            squares[c8] == ' '         &&
            squares[d8] == ' '         &&
            squares[a8] == 'r'         &&
            (bqueen)                 &&
            !AttackedSquare(e8,true) &&
            !AttackedSquare(d8,true) &&
            !AttackedSquare(c8,true)
          )
        {
            m->src     = e8;
            m->dst     = c8;
            m->special = SPECIAL_BQ_CASTLING;
            m->capture = ' ';
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of white pawn moves
 ****************************************************************************/
void ChessRules::WhitePawnMoves( MOVELIST *l,  Square square )
{
    Move *m = &l->moves[l->count];
    const lte *ptr = pawn_white_lookup[square];
    bool promotion = (RANK(square) == '7');

    // Capture ray
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        Square dst = (Square)*ptr++;
        if( dst == enpassant_target )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = SPECIAL_WEN_PASSANT;
            m->capture = 'p';
            m++;
            l->count++;
        }
        else if( IsBlack(squares[dst]) )
        {
            m->src    = square;
            m->dst    = dst;
            m->capture = squares[dst];
            if( !promotion )
            {
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }
            else
            {

                // Generate (under)promotions in the order (Q),N,B,R
                //  but we no longer rely on this elsewhere as it
                //  stops us reordering moves
                m->special   = SPECIAL_PROMOTION_QUEEN;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_KNIGHT;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_BISHOP;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_ROOK;
                m++;
                l->count++;
            }
        }
    }

    // Advance ray
    nbr_moves = *ptr++;
    for( lte i=0; i<nbr_moves; i++ )
    {
        Square dst = (Square)*ptr++;

        // If square occupied, end now
        if( !IsEmptySquare(squares[dst]) )
            break;
        m->src     = square;
        m->dst     = dst;
        m->capture = ' ';
        if( !promotion )
        {
            m->special  =  (i==0 ? NOT_SPECIAL : SPECIAL_WPAWN_2SQUARES);
            m++;
            l->count++;
        }
        else
        {

            // Generate (under)promotions in the order (Q),N,B,R
            //  but we no longer rely on this elsewhere as it
            //  stops us reordering moves
            m->special   = SPECIAL_PROMOTION_QUEEN;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_KNIGHT;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_BISHOP;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_ROOK;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Generate list of black pawn moves
 ****************************************************************************/
void ChessRules::BlackPawnMoves( MOVELIST *l, Square square )
{
    Move *m = &l->moves[l->count];
    const lte *ptr = pawn_black_lookup[square];
    bool promotion = (RANK(square) == '2');

    // Capture ray
    lte nbr_moves = *ptr++;
    while( nbr_moves-- )
    {
        Square dst = (Square)*ptr++;
        if( dst == enpassant_target )
        {
            m->src     = square;
            m->dst     = dst;
            m->special = SPECIAL_BEN_PASSANT;
            m->capture = 'P';
            m++;
            l->count++;
        }
        else if( IsWhite(squares[dst]) )
        {
            m->src  = square;
            m->dst    = dst;
            m->capture = squares[dst];
            if( !promotion )
            {
                m->special = NOT_SPECIAL;
                m++;
                l->count++;
            }
            else
            {

                // Generate (under)promotions in the order (Q),N,B,R
                //  but we no longer rely on this elsewhere as it
                //  stops us reordering moves
                m->special   = SPECIAL_PROMOTION_QUEEN;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_KNIGHT;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_BISHOP;
                m++;
                l->count++;
                m->src       = square;
                m->dst       = dst;
                m->capture   = squares[dst];
                m->special   = SPECIAL_PROMOTION_ROOK;
                m++;
                l->count++;
            }
        }
    }

    // Advance ray
    nbr_moves = *ptr++;
    for( int i=0; i<nbr_moves; i++ )
    {
        Square dst = (Square)*ptr++;

        // If square occupied, end now
        if( !IsEmptySquare(squares[dst]) )
            break;
        m->src  = square;
        m->dst  = dst;
        m->capture = ' ';
        if( !promotion )
        {
            m->special  =  (i==0 ? NOT_SPECIAL : SPECIAL_BPAWN_2SQUARES);
            m++;
            l->count++;
        }
        else
        {

            // Generate (under)promotions in the order (Q),N,B,R
            //  but we no longer rely on this elsewhere as it
            //  stops us reordering moves
            m->special   = SPECIAL_PROMOTION_QUEEN;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_KNIGHT;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_BISHOP;
            m++;
            l->count++;
            m->src       = square;
            m->dst       = dst;
            m->capture   = ' ';
            m->special   = SPECIAL_PROMOTION_ROOK;
            m++;
            l->count++;
        }
    }
}

/****************************************************************************
 * Make a move (with the potential to undo)
 ****************************************************************************/
void ChessRules::PushMove( Move& m )
{
    // Push old details onto stack
    DETAIL_PUSH;

    // Update castling prohibited flags for destination square, eg h8 -> bking
    DETAIL_CASTLING(m.dst);
                    // IMPORTANT - only dst is required since we also qualify
                    //  castling with presence of rook and king on right squares.
                    //  (I.E. if a rook or king leaves its original square, the
                    //  castling prohibited flag is unaffected, but it doesn't
                    //  matter since we won't castle unless rook and king are
                    //  present on the right squares. If subsequently a king or
                    //  rook returns, that's okay too because the  castling flag
                    //  is cleared by its arrival on the m.dst square, so
                    //  castling remains prohibited).
    enpassant_target = SQUARE_INVALID;

    // Special handling might be required
    switch( m.special )
    {
        default:
        squares[m.dst] = squares[m.src];
        squares[m.src] = ' ';
        break;

        // King move updates king position in details field
        case SPECIAL_KING_MOVE:
        squares[m.dst] = squares[m.src];
        squares[m.src] = ' ';
        if( white )
            wking_square = m.dst;
        else
            bking_square = m.dst;
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_QUEEN:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'Q':'q');
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_ROOK:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'R':'r');
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_BISHOP:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'B':'b');
        break;

        // In promotion case, dst piece doesn't equal src piece
        case SPECIAL_PROMOTION_KNIGHT:
        squares[m.src] = ' ';
        squares[m.dst] = (white?'N':'n');
        break;

        // White enpassant removes pawn south of destination
        case SPECIAL_WEN_PASSANT:
        squares[m.src] = ' ';
        squares[m.dst] = 'P';
        squares[ SOUTH(m.dst) ] = ' ';
        break;

        // Black enpassant removes pawn north of destination
        case SPECIAL_BEN_PASSANT:
        squares[m.src] = ' ';
        squares[m.dst] = 'p';
        squares[ NORTH(m.dst) ] = ' ';
        break;

        // White pawn advances 2 squares sets an enpassant target
        case SPECIAL_WPAWN_2SQUARES:
        squares[m.src] = ' ';
        squares[m.dst] = 'P';
        enpassant_target = SOUTH(m.dst);
        break;

        // Black pawn advances 2 squares sets an enpassant target
        case SPECIAL_BPAWN_2SQUARES:
        squares[m.src] = ' ';
        squares[m.dst] = 'p';
        enpassant_target = NORTH(m.dst);
        break;

        // Castling moves update 4 squares each
        case SPECIAL_WK_CASTLING:
        squares[e1] = ' ';
        squares[f1] = 'R';
        squares[g1] = 'K';
        squares[h1] = ' ';
        wking_square = g1;
        break;
        case SPECIAL_WQ_CASTLING:
        squares[e1] = ' ';
        squares[d1] = 'R';
        squares[c1] = 'K';
        squares[a1] = ' ';
        wking_square = c1;
        break;
        case SPECIAL_BK_CASTLING:
        squares[e8] = ' ';
        squares[f8] = 'r';
        squares[g8] = 'k';
        squares[h8] = ' ';
        bking_square = g8;
        break;
        case SPECIAL_BQ_CASTLING:
        squares[e8] = ' ';
        squares[d8] = 'r';
        squares[c8] = 'k';
        squares[a8] = ' ';
        bking_square = c8;
        break;
    }

    // Toggle who-to-move
    Toggle();
}

/****************************************************************************
 * Undo a move
 ****************************************************************************/
void ChessRules::PopMove( Move& m )
{
    // Previous detail field
    DETAIL_POP;

    // Toggle who-to-move
    Toggle();

    // Special handling might be required
    switch( m.special )
    {
        default:
        squares[m.src] = squares[m.dst];
        squares[m.dst] = m.capture;
        break;

        // For promotion, src piece was a pawn
        case SPECIAL_PROMOTION_QUEEN:
        case SPECIAL_PROMOTION_ROOK:
        case SPECIAL_PROMOTION_BISHOP:
        case SPECIAL_PROMOTION_KNIGHT:
        if( white )
            squares[m.src] = 'P';
        else
            squares[m.src] = 'p';
        squares[m.dst] = m.capture;
        break;

        // White enpassant re-insert black pawn south of destination
        case SPECIAL_WEN_PASSANT:
        squares[m.src] = 'P';
        squares[m.dst] = ' ';
        squares[SOUTH(m.dst)] = 'p';
        break;

        // Black enpassant re-insert white pawn north of destination
        case SPECIAL_BEN_PASSANT:
        squares[m.src] = 'p';
        squares[m.dst] = ' ';
        squares[NORTH(m.dst)] = 'P';
        break;

        // Castling moves update 4 squares each
        case SPECIAL_WK_CASTLING:
        squares[e1] = 'K';
        squares[f1] = ' ';
        squares[g1] = ' ';
        squares[h1] = 'R';
        break;
        case SPECIAL_WQ_CASTLING:
        squares[e1] = 'K';
        squares[d1] = ' ';
        squares[c1] = ' ';
        squares[a1] = 'R';
        break;
        case SPECIAL_BK_CASTLING:
        squares[e8] = 'k';
        squares[f8] = ' ';
        squares[g8] = ' ';
        squares[h8] = 'r';
        break;
        case SPECIAL_BQ_CASTLING:
        squares[e8] = 'k';
        squares[d8] = ' ';
        squares[c8] = ' ';
        squares[a8] = 'r';
        break;
    }
}


/****************************************************************************
 * Determine if an occupied square is attacked
 ****************************************************************************/
bool ChessRules::AttackedPiece( Square square )
{
    bool enemy_is_white  =  IsBlack(squares[square]);
    return( AttackedSquare(square,enemy_is_white) );
}

/****************************************************************************
 * Is a square is attacked by enemy ?
 ****************************************************************************/
bool ChessRules::AttackedSquare( Square square, bool enemy_is_white )
{
    Square dst;
    const lte *ptr = (enemy_is_white ? attacks_black_lookup[square] : attacks_white_lookup[square] );
    lte nbr_rays = *ptr++;
    while( nbr_rays-- )
    {
        lte ray_len = *ptr++;
        while( ray_len-- )
        {
            dst = (Square)*ptr++;
            char piece=squares[dst];

            // If square not occupied (empty), continue
            if( IsEmptySquare(piece) )
                ptr++;  // skip mask

            // Else if occupied
            else
            {
                lte mask = *ptr++;

                // White attacker ?
                if( IsWhite(piece) && enemy_is_white )
                {
                    if( to_mask[piece] & mask )
                        return true;
                }

                // Black attacker ?
                else if( IsBlack(piece) && !enemy_is_white )
                {
                    if( to_mask[piece] & mask )
                        return true;
                }

                // Goto end of ray
                ptr += (2*ray_len);
                ray_len = 0;
            }
        }
    }

    ptr = knight_lookup[square];
    lte nbr_squares = *ptr++;
    while( nbr_squares-- )
    {
        dst = (Square)*ptr++;
        char piece=squares[dst];

        // If occupied by an enemy knight, we have found an attacker
        if( (enemy_is_white&&piece=='N') || (!enemy_is_white&&piece=='n') )
            return true;
    }
    return false;
}

/****************************************************************************
 * Evaluate a position, returns bool okay (not okay means illegal position)
 ****************************************************************************/
bool ChessRules::Evaluate()
{
    Square enemy_king = (Square)(white ? bking_square : wking_square);
    // Enemy king is attacked and our move, position is illegal
    return !AttackedPiece(enemy_king);
}

bool ChessRules::Evaluate( TERMINAL &score_terminal )
{
    return( Evaluate(NULL,score_terminal) );
}

bool ChessRules::Evaluate( MOVELIST *p, TERMINAL &score_terminal )
{
    /* static ;remove for thread safety */ MOVELIST local_list;
    MOVELIST &list = p?*p:local_list;
    int i, any;
    Square my_king, enemy_king;
    bool okay;
    score_terminal=NOT_TERMINAL;

    //DIAG_evaluate_count++;

    // Enemy king is attacked and our move, position is illegal
    enemy_king = (Square)(white ? bking_square : wking_square);
    if( AttackedPiece(enemy_king) )
        okay = false;

    // Else legal position
    else
    {
        okay = true;

        // Work out if the game is over by checking for any legal moves
        GenMoveList( &list );
        for( any=i=0 ; i<list.count && any==0 ; i++ )
        {
            PushMove( list.moves[i] );
            my_king = (Square)(white ? bking_square : wking_square);
            if( !AttackedPiece(my_king) )
                any++;
            PopMove( list.moves[i] );
        }

        // If no legal moves, position is either checkmate or stalemate
        if( any == 0 )
        {
            my_king = (Square)(white ? wking_square : bking_square);
            if( AttackedPiece(my_king) )
                score_terminal = (white ? TERMINAL_WCHECKMATE
                                        : TERMINAL_BCHECKMATE);
            else
                score_terminal = (white ? TERMINAL_WSTALEMATE
                                        : TERMINAL_BSTALEMATE);
        }
    }
    return(okay);
}

// Transform a position with W to move into an equivalent with B to move and vice-versa
void ChessRules::Transform()
{
    Toggle();
    Square wking_square_ = (Square)0;
    Square bking_square_ = (Square)0;
    Square enpassant_target_ = (Square)this->enpassant_target;

    // swap wking <-> bking
    int tmp = wking;
    wking  = bking;
    bking  = tmp;

    // swap wqueen <-> bqueen
    tmp    = wqueen;
    wqueen = bqueen;
    bqueen = tmp;
    Square src, dst;
    char file, r1, r2;

    // Loop through the board
    for( file='a'; file<='h'; file++ )
    {
        for( r1='1',r2='8'; r1<='8'; r1++,r2-- )
        {
            src = SQ(file,r1);
            dst = SQ(file,r2);
            if( wking_square == src )
                bking_square_ = dst;
            if( bking_square == src )
                wking_square_ = dst;
            if( enpassant_target == src )
                enpassant_target_ = dst;
        }
    }
    this->wking_square      = wking_square_;
    this->bking_square      = bking_square_;
    this->enpassant_target  = enpassant_target_;

    // Loop through half the board
    for( file='a'; file<='h'; file++ )
    {
        for( r1='1',r2='8'; r1<='4'; r1++,r2-- )
        {
            src = SQ(file ,r1);
            dst = SQ(file ,r2);
            char from = squares[src];
            char tmpc = squares[dst];
            for( int i=0; i<2; i++ )
            {
                switch( from )
                {
                    case 'K':   squares[dst] = 'k';   break;
                    case 'Q':   squares[dst] = 'q';   break;
                    case 'R':   squares[dst] = 'r';   break;
                    case 'B':   squares[dst] = 'b';   break;
                    case 'N':   squares[dst] = 'n';   break;
                    case 'P':   squares[dst] = 'p';   break;
                    case 'k':   squares[dst] = 'K';   break;
                    case 'q':   squares[dst] = 'Q';   break;
                    case 'r':   squares[dst] = 'R';   break;
                    case 'b':   squares[dst] = 'B';   break;
                    case 'n':   squares[dst] = 'N';   break;
                    case 'p':   squares[dst] = 'P';   break;
                    default:    squares[dst] = from;
                }
                from = tmpc;
                dst  = src;
            }
        }
    }
}


// Transform a W move in a transformed position to a B one and vice-versa
Move ChessRules::Transform( Move move )
{
    Move ret;
    Square src=a8, dst=a8, from, to;
    char file, r1, r2;

    // Loop through the board (this is a very lazy and slow way to do it!)
    for( file='a'; file<='h'; file++ )
    {
        for( r1='1', r2='8'; r1<='8'; r1++,r2-- )
        {
            from = SQ(file,r1);
            to = SQ(file,r2);
            if( move.src == from )
                src = to;
            if( move.dst == from )
                dst = to;
        }
    }
    ret.src = src;
    ret.dst = dst;

    // Special moves
    switch( move.special )
    {
        case SPECIAL_WK_CASTLING:       ret.special = SPECIAL_BK_CASTLING;          break;
        case SPECIAL_BK_CASTLING:       ret.special = SPECIAL_WK_CASTLING;          break;
        case SPECIAL_WQ_CASTLING:       ret.special = SPECIAL_BQ_CASTLING;          break;
        case SPECIAL_BQ_CASTLING:       ret.special = SPECIAL_WQ_CASTLING;          break;
        case SPECIAL_WPAWN_2SQUARES:    ret.special = SPECIAL_BPAWN_2SQUARES;       break;
        case SPECIAL_BPAWN_2SQUARES:    ret.special = SPECIAL_WPAWN_2SQUARES;       break;
        case SPECIAL_WEN_PASSANT:       ret.special = SPECIAL_BEN_PASSANT;          break;
        case SPECIAL_BEN_PASSANT:       ret.special = SPECIAL_WEN_PASSANT;          break;
        default: break;
    }

    // Captures
    if( islower(move.capture) )
        ret.capture = toupper(move.capture);
    else if( isupper(move.capture) )
        ret.capture = tolower(move.capture);
    return ret;
}

/****************************************************************************
 *  Test for legal position, sets reason to a mask of possibly multiple reasons
 ****************************************************************************/
bool ChessRules::IsLegal( ILLEGAL_REASON& reason )
{
    int  ireason = 0;
    int  wkings=0, bkings=0, wpawns=0, bpawns=0, wpieces=0, bpieces=0;
    bool legal = true;
    int  file, rank;
    char p;
    Square opposition_king_location = SQUARE_INVALID;

    // Loop through the board
    file=0;     // go from a8,b8..h8,a7,b7..h1
    rank=7;
    for( ;;)
    {
        Square sq = SQ('a'+file, '1'+rank);
        p = squares[sq];
        if( (p=='P'||p=='p') && (rank==0||rank==7) )
        {
            legal = false;
            ireason |= IR_PAWN_POSITION;
        }
        if( IsWhite(p) )
        {
            if( p == 'P' )
                wpawns++;
            else
            {
                wpieces++;
                if( p == 'K' )
                {
                    wkings++;
                    if( !white )
                        opposition_king_location = sq;
                }
            }
        }
        else if( IsBlack(p) )
        {
            if( p == 'p' )
                bpawns++;
            else
            {
                bpieces++;
                if( p == 'k' )
                {
                    bkings++;
                    if( white )
                        opposition_king_location = sq;
                }
            }
        }
        if( sq == h1 )
            break;
        else
        {
            file++;
            if( file == 8 )
            {
                file = 0;
                rank--;
            }
        }
    }
    if( wkings!=1 || bkings!=1 )
    {
        legal = false;
        ireason |= IR_NOT_ONE_KING_EACH;
    }
    if( opposition_king_location!=SQUARE_INVALID && AttackedPiece(opposition_king_location) )
    {
        legal = false;
        ireason |= IR_CAN_TAKE_KING;
    }
    if( wpieces>8 && (wpieces+wpawns)>16 )
    {
        legal = false;
        ireason |= IR_WHITE_TOO_MANY_PIECES;
    }
    if( bpieces>8 && (bpieces+bpawns)>16 )
    {
        legal = false;
        ireason |= IR_BLACK_TOO_MANY_PIECES;
    }
    if( wpawns > 8 )
    {
        legal = false;
        ireason |= IR_WHITE_TOO_MANY_PAWNS;
    }
    if( bpawns > 8 )
    {
        legal = false;
        ireason |= IR_BLACK_TOO_MANY_PAWNS;
    }
    reason = (ILLEGAL_REASON)ireason;
    return( legal );
}

/****************************************************************************
 * ChessEvaluation.cpp Chess classes - Simple chess AI, leaf scoring function for position
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

//-- preferences
#define USE_CHASE_PAWNS
//#define USE_WEAKER_CENTRAL
#define USE_STRONG_KING
#define USE_IN_THE_SQUARE
#define USE_LIQUIDATION

// Do we check for mate at a leaf node, and if so how do we do it ?
#define CHECK_FOR_LEAF_MATE
// #define CHECK_FOR_LEAF_MATE_USE_EVALUATE // not the fastest way

// Lookup table for quick calculation of material value of any piece
static int either_colour_material[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,500,  0,  0, 30,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
   10, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,500,  0,  0, 30,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
   10, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

/****************************************************************************
 * Calculate material that side to play can win directly
 *  Fast white to move version
 ****************************************************************************/
int ChessEvaluation::EnpriseWhite()
{

    int best_so_far=0;  // amount of material that can be safely captured

    // Locals
    unsigned char defenders_buf[32];
    unsigned char *defenders;
    unsigned char attackers_buf[32];
    unsigned char *attackers;
    unsigned char *reorder_base, *base;
    unsigned char target;
    unsigned char attacker;
    Square square;
    Square attack_square;
    const lte *ptr;
    lte nbr_rays, nbr_squares;

    // For all squares
    for( square=a8; square<=h1; ++square )
    {

        // If piece on square is black, it's a potential target
        target = squares[square];
        if( IsBlack(target) )
        {
            attackers = attackers_buf;

            // It could be attacked by up to 2 white pawns
            ptr = pawn_attacks_black_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'P' )
                    *attackers++ = attacker;
            }

            // It could be attacked by up to 8 white knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'N' )
                    *attackers++ = attacker;
            }

            // From here on we may need to reorder the attackers
            reorder_base = base = attackers;

            // Move along each queen ray from the square being evaluated to
            //  each possible attacking square (looking for white attackers
            //  of a black piece)
            ptr = attacks_black_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    attack_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    attacker = squares[attack_square];

                    // If the square is occupied by an attacking (white) piece, and
                    //  it matches a piece that attacks down that ray we have found
                    //  an attacker
                    if( IsWhite(attacker) && (to_mask[attacker]&mask) )
                    {
                        if( attacker != 'P' ) // we've already done pawn
                            *attackers++ = attacker;
                        if( attacker == 'K' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }

                    // Any other piece or a defender, must move to end of ray
                    else if( !IsEmptySquare(attacker) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  attackers I'm afraid
                // If this ray generated attackers, and there were attackers
                //  before this ray
                if( attackers>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base==('K') )
                        swap = true;
                    else if( *reorder_base==('Q') && *base!=('K') )
                        swap = true;
                    else if( *reorder_base==('R') && *base==('B') )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    attackers
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != attackers )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != attackers )
                            *dst++ = *src++;
                    }
                }
                base = attackers;
            }

            // Any attackers ?
            if( attackers == attackers_buf )
                continue;  // No - continue to next square

            // Locals
            unsigned char defender;
            Square defend_square;

            // The target itself counts as a defender
            defenders = defenders_buf;
            *defenders++ = target;

            // It could be defended by up to 2 black pawns
            ptr = pawn_attacks_white_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'p' )
                    *defenders++ = defender;
            }

            // It could be defended by up to 8 black knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'n' )
                    *defenders++ = defender;
            }

            // From here on we may need to reorder the defenders
            reorder_base = base = defenders;

            // Move along each queen ray from the square being evaluated to
            //  each possible defending square (looking for black defenders
            //  of a black piece)
            ptr = attacks_white_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    defend_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    defender = squares[defend_square];

                    // If the square is occupied by an defending (black) piece, and
                    //  it matches a piece that defends down that ray we have found
                    //  a defender
                    if( IsBlack(defender) && (to_mask[defender]&mask) )
                    {
                        if( defender != 'p' ) // we've already done pawn
                            *defenders++ = defender;
                        if( defender == 'k' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }

                    // Any other piece or an attacker, must move to end of ray
                    else if( !IsEmptySquare(defender) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  defenders I'm afraid

                // If this ray generated defenders, and there were defenders
                //  before this ray
                if( defenders>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base=='k' )
                        swap = true;
                    else if( *reorder_base=='q' && *base!='k' )
                        swap = true;
                    else if( *reorder_base=='r' && *base=='b' )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    defenders
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != defenders )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != defenders )
                            *dst++ = *src++;
                    }
                }
                base = defenders;
            }

            // Figure out the net effect of the capturing sequence
            const unsigned char *a=attackers_buf;
            const unsigned char *d=defenders_buf;
            #define POS_INFINITY  1000000000
            #define NEG_INFINITY -1000000000
            int min = POS_INFINITY;
            int max = NEG_INFINITY;
            int net=0;  // net gain

            // While there are attackers and defenders
            while( a<attackers && d<defenders )
            {

                // Attacker captures
                net += either_colour_material[*d++];

                // Defender can elect to stop here
                if( net < min )
                    min = net;

                // Can defender recapture ?
                if( d == defenders )
                {
                    if( net > max )
                        max = net;
                    break;  // no
                }

                // Defender recaptures
                net -= either_colour_material[*a++];

                // Attacker can elect to stop here
                if( net > max )
                    max = net;
            }

            // Result is the lowest of best attacker can do and
            //  best defender can do
            int score = min<=max ? min : max;
            if( score > best_so_far )
                best_so_far = score;
        }   // end if black target
    } // end square loop

    // After looking at every square, return the best result
    return best_so_far;
}


/****************************************************************************
 * Calculate material that side to play can win directly
 *  Fast black to move version
 ****************************************************************************/
int ChessEvaluation::EnpriseBlack()
{
    int best_so_far=0;  // amount of material that can be safely captured

    // Locals
    unsigned char defenders_buf[32];
    unsigned char *defenders;
    unsigned char attackers_buf[32];
    unsigned char *attackers;
    unsigned char *reorder_base, *base;
    unsigned char target;
    unsigned char attacker;
    Square square;
    Square attack_square;
    const lte *ptr;
    lte nbr_rays, nbr_squares;

    // For all squares
    for( square=a8; square<=h1; ++square )
    {

        // If piece on square is white, it's a potential target
        target = squares[square];
        if( IsWhite(target) )
        {
            attackers = attackers_buf;

            // It could be attacked by up to 2 black pawns
            ptr = pawn_attacks_white_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'p' )
                    *attackers++ = attacker;
            }

            // It could be attacked by up to 8 black knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'n' )
                    *attackers++ = attacker;
            }

            // From here on we may need to reorder the attackers
            reorder_base = base = attackers;

            // Move along each queen ray from the square being evaluated to
            //  each possible attacking square (looking for black attackers
            //  of a white piece)
            ptr = attacks_white_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    attack_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    attacker = squares[attack_square];

                    // If the square is occupied by an attacking (black) piece, and
                    //  it matches a piece that attacks down that ray we have found
                    //  an attacker
                    if( IsBlack(attacker) && (to_mask[attacker]&mask) )
                    {
                        if( attacker != 'p' ) // we've already done pawn
                            *attackers++ = attacker;
                        if( attacker == 'k' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }

                    // Any other piece or a defender, must move to end of ray
                    else if( !IsEmptySquare(attacker) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  attackers I'm afraid
                // If this ray generated attackers, and there were attackers
                //  before this ray
                if( attackers>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base==('k') )
                        swap = true;
                    else if( *reorder_base==('q') && *base!=('k') )
                        swap = true;
                    else if( *reorder_base==('r') && *base==('b') )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    attackers
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != attackers )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != attackers )
                            *dst++ = *src++;
                    }
                }
                base = attackers;
            }

            // Any attackers ?
            if( attackers == attackers_buf )
                continue;  // No - continue to next square

            // Locals
            unsigned char defender;
            Square defend_square;

            // The target itself counts as a defender
            defenders = defenders_buf;
            *defenders++ = target;

            // It could be defended by up to 2 white pawns
            ptr = pawn_attacks_black_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'P' )
                    *defenders++ = defender;
            }

            // It could be defended by up to 8 white knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'N' )
                    *defenders++ = defender;
            }

            // From here on we may need to reorder the defenders
            reorder_base = base = defenders;

            // Move along each queen ray from the square being evaluated to
            //  each possible defending square (looking for white defenders
            //  of a white piece)
            ptr = attacks_black_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    defend_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    defender = squares[defend_square];

                    // If the square is occupied by an defending (white) piece, and
                    //  it matches a piece that defends down that ray we have found
                    //  a defender
                    if( IsWhite(defender) && (to_mask[defender]&mask) )
                    {
                        if( defender != 'P' ) // we've already done pawn
                            *defenders++ = defender;
                        if( defender == 'K' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }

                    // Any other piece or an attacker, must move to end of ray
                    else if( !IsEmptySquare(defender) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  defenders I'm afraid

                // If this ray generated defenders, and there were defenders
                //  before this ray
                if( defenders>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base==('K') )
                        swap = true;
                    else if( *reorder_base==('Q') && *base!=('K') )
                        swap = true;
                    else if( *reorder_base==('R') && *base==('B') )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    defenders
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != defenders )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != defenders )
                            *dst++ = *src++;
                    }
                }
                base = defenders;
            }

            // Figure out the net effect of the capturing sequence
            const unsigned char *a=attackers_buf;
            const unsigned char *d=defenders_buf;
            int min = POS_INFINITY;
            int max = NEG_INFINITY;
            int net=0;  // net gain

            // While there are attackers and defenders
            while( a<attackers && d<defenders )
            {

                // Attacker captures
                net += either_colour_material[*d++];

                // Defender can elect to stop here
                if( net < min )
                    min = net;

                // Can defender recapture ?
                if( d == defenders )
                {
                    if( net > max )
                        max = net;
                    break;  // no
                }

                // Defender recaptures
                net -= either_colour_material[*a++];

                // Attacker can elect to stop here
                if( net > max )
                    max = net;
            }

            // Result is the lowest of best attacker can do and
            //  best defender can do
            int score = min<=max ? min : max;
            if( score > best_so_far )
                best_so_far = score;
        }   // end if white target
    } // end square loop

    // After looking at every square, return the best result
    return best_so_far;
}



//=========== EVALUATION ===============================================

static int king_ending_bonus_static[] =
{
    #if 1
    /*  0x00-0x07 a8-h8 */ -25,-25,-25,-25,-25,-25,-25,-25,
    /*  0x00-0x0f a7-h7 */ -25,  0,  0,  0,  0,  0,  0,-25,
    /*  0x10-0x17 a6-h6 */ -25,  0, 25, 25, 25, 25,  0,-25,
    /*  0x10-0x1f a5-h5 */ -25,  0, 25, 50, 50, 25,  0,-25,
    /*  0x20-0x27 a4-h4 */ -25,  0, 25, 50, 50, 25,  0,-25,
    /*  0x20-0x2f a3-h3 */ -25,  0, 25, 25, 25, 25,  0,-25,
    /*  0x30-0x37 a2-h2 */ -25,  0,  0,  0,  0,  0,  0,-25,
    /*  0x30-0x3f a1-h1 */ -25,-25,-25,-25,-25,-25,-25,-25
    #else
    /*  0x00-0x07 a8-h8 */ -10,-10,-10,-10,-10,-10,-10,-10,
    /*  0x00-0x0f a7-h7 */ -10,  0,  0,  0,  0,  0,  0,-10,
    /*  0x10-0x17 a6-h6 */ -10,  0, 10, 10, 10, 10,  0,-10,
    /*  0x10-0x1f a5-h5 */ -10,  0, 10, 20, 20, 10,  0,-10,
    /*  0x20-0x27 a4-h4 */ -10,  0, 10, 20, 20, 10,  0,-10,
    /*  0x20-0x2f a3-h3 */ -10,  0, 10, 10, 10, 10,  0,-10,
    /*  0x30-0x37 a2-h2 */ -10,  0,  0,  0,  0,  0,  0,-10,
    /*  0x30-0x3f a1-h1 */ -10,-10,-10,-10,-10,-10,-10,-10
    #endif
};

static int king_ending_bonus_dynamic_white[0x80];
static int king_ending_bonus_dynamic_black[0x80];

// Lookup table for quick calculation of material value of white piece
static int white_material[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,500,  0,  0, 30,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
   10, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

// Lookup table for quick calculation of material value of black piece
static int black_material[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0,-31,  0,  0,  0,  0,  0,  0,  0,  0,-500, 0,  0,-30,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
  -10,-90,-50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

// Lookup table for quick calculation of material value of white piece (not pawn or king)
static int white_pieces[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
    0, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

// Lookup table for quick calculation of material value of black piece (not pawn or king)
static int black_pieces[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
    0, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

/****************************************************************************
 * Do some planning before making a move
 *   (needs a lot of improvement)
 ****************************************************************************/
void ChessEvaluation::Planning()
{
    Square weaker_king, bonus_square;
    char piece;
    int score_black_material = 0;
    int score_white_material = 0;
    const int MATERIAL_ENDING  = (500 + ((8*10+4*30+2*50+90)*1)/3);
    int *bonus_ptr;

    // Get material for both sides
    int score_black_pieces = 0;
    int score_white_pieces = 0;
    for( Square square=a8; square<=h1; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
    }
    int score_white_pawns = score_white_material - 500 // -500 is king
                          - score_white_pieces;
    planning_score_white_pieces = score_white_pieces;
    planning_white_piece_pawn_percent = 1000;
    if( score_white_pawns )
    {
        planning_white_piece_pawn_percent = (100*score_white_pieces) /
                                                score_white_pawns;
        if( planning_white_piece_pawn_percent > 1000 )
            planning_white_piece_pawn_percent = 1000;
    }
    int score_black_pawns = (0-score_black_material) - 500 // -500 is king
                          - score_black_pieces;
    planning_score_black_pieces = score_black_pieces;
    planning_black_piece_pawn_percent = 1000;
    if( score_black_pawns )
    {
        planning_black_piece_pawn_percent = (100*score_black_pieces) /
                                                score_black_pawns;
        if( planning_black_piece_pawn_percent > 1000 )
            planning_black_piece_pawn_percent = 1000;
    }

    // Reset dynamic king position arrays
    memset( king_ending_bonus_dynamic_white,
            0,
            sizeof(king_ending_bonus_dynamic_white) );
    memset( king_ending_bonus_dynamic_black,
            0,
            sizeof(king_ending_bonus_dynamic_black) );

    // Are we in an ending ?
    bool ending = (score_white_material<MATERIAL_ENDING && score_black_material>0-MATERIAL_ENDING);

    // Adjust material for enprise
    if( white )
        score_white_material += EnpriseWhite();
    else
        score_black_material -= EnpriseBlack();
    int sum = score_white_material+score_black_material;

    // Find stronger side
    white_is_better = false;
    black_is_better = false;
    if( sum > 0 )
        white_is_better = true;
    else if( sum < 0 )
        black_is_better = true;

    // Are we in an ending ?
    if( ending )
    {

        // Reset dynamic king position arrays
        memcpy( king_ending_bonus_dynamic_white,
                king_ending_bonus_static,
                sizeof(king_ending_bonus_dynamic_white) );
        memcpy( king_ending_bonus_dynamic_black,
                king_ending_bonus_static,
                sizeof(king_ending_bonus_dynamic_black) );

        // Encourage kings to go where the pawns are
        #ifdef USE_CHASE_PAWNS
        for( Square square=a8; square<=h1; ++square )
        {
            if( squares[square] == 'P' )
                king_ending_bonus_dynamic_black[square] = king_ending_bonus_static[e5];
            if( squares[square] == 'p' )
                king_ending_bonus_dynamic_white[square] = king_ending_bonus_static[e5];
        }
        #endif

        // Reward stronger side putting his K an extended knight's
        //  square away from weaker side's K, if it's toward centre
        if( white_is_better || black_is_better )
        {
            if( white_is_better )
            {
                bonus_ptr   = king_ending_bonus_dynamic_white;
                weaker_king = (Square)bking_square;
            }
            else
            {
                bonus_ptr   = king_ending_bonus_dynamic_black;
                weaker_king = (Square)wking_square;
            }

            const lte *ptr = good_king_position_lookup[weaker_king];
            lte nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                bonus_square = (Square)*ptr++;
                if( king_ending_bonus_static[bonus_square] >
                    king_ending_bonus_static[weaker_king]
                  )
                {
                    #ifdef USE_WEAKER_CENTRAL
                    bonus_ptr[bonus_square] += king_ending_bonus_static[f3];
                    #else
                    bonus_ptr[bonus_square] += king_ending_bonus_static[e5];
                    #endif
                }
            }
        }
    }
}

/****************************************************************************
 * Evaluate a position, leaf node
 *
 *   (this makes a rather ineffectual effort to score positional features
 *    needs a lot of improvement)
 ****************************************************************************/
void ChessEvaluation::EvaluateLeaf( int &material, int &positional )
{
    //DIAG_evaluate_leaf_count++;
    char   piece;
    int file;
    int bonus = 0;
    int score_black_material = 0;
    int score_white_material = 0;
    int black_connected=0;
    int white_connected=0;

    int white_king_safety_bonus          =0;
    int white_king_central_bonus         =0;
    int white_queen_central_bonus        =0;
    int white_queen_developed_bonus      =0;
    int white_queen78_bonus              =0;
    int white_undeveloped_minor_bonus    =0;

    int black_king_safety_bonus          =0;
    int black_king_central_bonus         =0;
    int black_queen_central_bonus        =0;
    int black_queen_developed_bonus      =0;
    int black_queen78_bonus              =0;
    int black_undeveloped_minor_bonus    =0;


#define BONUS_WHITE_SWAP_PIECE          60
#define BONUS_BLACK_SWAP_PIECE          -60
#define BONUS_BLACK_CONNECTED_ROOKS     -10
#define BONUS_BLACK_BLOCKED_BISHOP      10
#define BLACK_UNDEVELOPED_MINOR_BONUS   3
#define BONUS_BLACK_KNIGHT_CENTRAL0     -8
#define BONUS_BLACK_KNIGHT_CENTRAL1     -9
#define BONUS_BLACK_KNIGHT_CENTRAL2     -10
#define BONUS_BLACK_KNIGHT_CENTRAL3     -12
#define BONUS_BLACK_KING_SAFETY         -10
#define BONUS_BLACK_KING_CENTRAL0       -8
#define BONUS_BLACK_KING_CENTRAL1       -9
#define BONUS_BLACK_KING_CENTRAL2       -10
#define BONUS_BLACK_KING_CENTRAL3       -12
#define BONUS_BLACK_QUEEN_CENTRAL       -10
#define BONUS_BLACK_QUEEN_DEVELOPED     -10
#define BONUS_BLACK_QUEEN78             -5
#define BONUS_BLACK_ROOK7               -5
#define BONUS_BLACK_PAWN5               -20     // boosted because now must be passed
#define BONUS_BLACK_PAWN6               -30     // boosted because now must be passed
#define BONUS_BLACK_PAWN7               -40     // boosted because now must be passed
#define BONUS_BLACK_PAWN_CENTRAL        -5

#define BONUS_WHITE_CONNECTED_ROOKS      10
#define BONUS_WHITE_BLOCKED_BISHOP       -10
#define WHITE_UNDEVELOPED_MINOR_BONUS    -3
#define BONUS_WHITE_KNIGHT_CENTRAL0      8
#define BONUS_WHITE_KNIGHT_CENTRAL1      9
#define BONUS_WHITE_KNIGHT_CENTRAL2      10
#define BONUS_WHITE_KNIGHT_CENTRAL3      12
#define BONUS_WHITE_KING_SAFETY          10
#define BONUS_WHITE_KING_CENTRAL0        8
#define BONUS_WHITE_KING_CENTRAL1        9
#define BONUS_WHITE_KING_CENTRAL2        10
#define BONUS_WHITE_KING_CENTRAL3        12
#define BONUS_WHITE_QUEEN_CENTRAL        10
#define BONUS_WHITE_QUEEN_DEVELOPED      10
#define BONUS_WHITE_QUEEN78              5
#define BONUS_WHITE_ROOK7                5
#define BONUS_WHITE_PAWN5                20     // boosted because now must be passed
#define BONUS_WHITE_PAWN6                30     // boosted because now must be passed
#define BONUS_WHITE_PAWN7                40     // boosted because now must be passed
#define BONUS_WHITE_PAWN_CENTRAL         5
#define BONUS_STRONG_KING                50

const int MATERIAL_OPENING = (500 + ((8*10+4*30+2*50+90)*2)/3);
const int MATERIAL_MIDDLE  = (500 + ((8*10+4*30+2*50+90)*1)/3);


    Square black_king_square = SQUARE_INVALID;
    Square white_king_square = SQUARE_INVALID;
    Square black_pawns_buf[16];
    Square white_pawns_buf[16];
    Square black_passers_buf[16];
    Square white_passers_buf[16];
    Square *black_passers =  black_passers_buf;
    Square *white_passers =  white_passers_buf;
    Square *black_pawns   =  black_pawns_buf;
    Square *white_pawns   =  white_pawns_buf;
    int score_black_pieces = 0;
    int score_white_pieces = 0;

    // a8->h8
    for( Square square=a8; square<=h8; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                break;
            }

            case 'r':
            {
                black_connected++;
                if( black_connected == 2 )
                    bonus += BONUS_BLACK_CONNECTED_ROOKS;
                break;
            }

            case 'n':
            {
                black_connected=2;
                black_undeveloped_minor_bonus++;
                break;
            }

            case 'b':
            {
                black_connected=2;
                black_undeveloped_minor_bonus++;
                if( a8==square && IsBlack(squares[b7]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else if( h8==square && IsBlack(squares[g7]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else
                {
                    if( IsBlack(squares[SE(square)]) &&
                        IsBlack(squares[SW(square)])
                      )
                    {
                        bonus += BONUS_BLACK_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'q':
            {
                black_connected=2;
                break;
            }

            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                black_connected=2;
                file = IFILE(square);
                if( file<2 || file>5 )
                    black_king_safety_bonus = BONUS_BLACK_KING_SAFETY;
                break;
            }

            case 'Q':
            {
                white_queen78_bonus = BONUS_WHITE_QUEEN78;
                break;
            }
        }
    }

    // a7->h7
    unsigned int next_passer_mask = 0;
    unsigned int passer_mask = 0;
    unsigned int three_files = 0x1c0;   // 1 1100 0000
    for( Square square=a7; square<=h7; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                break;
            }

            case 'b':
            {
                if( a7==square && IsBlack(squares[b6]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else if( h7==square && IsBlack(squares[g6]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else
                {
                    if( IsBlack(squares[SE(square)]) &&
                        IsBlack(squares[SW(square)])
                      )
                    {
                        bonus += BONUS_BLACK_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'q':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_queen_developed_bonus = BONUS_BLACK_QUEEN_DEVELOPED;
                break;
            }

            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( file<2 || file>5 )
                    black_king_safety_bonus = BONUS_BLACK_KING_SAFETY;
                break;
            }

            case 'R':
            {
                bonus += BONUS_WHITE_ROOK7;
                break;
            }

            case 'Q':
            {
                white_queen78_bonus = BONUS_WHITE_QUEEN78;
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                *white_passers++ = square;
                bonus += BONUS_WHITE_PAWN7;
                #ifdef USE_STRONG_KING
                Square ahead = NORTH(square);
                if( squares[ahead]=='K' && king_ending_bonus_dynamic_white[ahead]==0 )
                    bonus += BONUS_STRONG_KING;
                #endif
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                passer_mask |= three_files;
                break;
            }
        }
        three_files >>= 1;
    }

    // a6->h6
    unsigned int file_mask = 0x80;  // 0 1000 0000
    three_files = 0x1c0;            // 1 1100 0000
    for( Square square=a6; square<=h6; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL0;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL0;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL3;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL3;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'P':
            {
                *white_pawns++ = square;
                if( !(passer_mask&file_mask) )
                {
                    *white_passers++ = square;
                    bonus += BONUS_WHITE_PAWN6;
                    #ifdef USE_STRONG_KING
                    Square ahead = NORTH(square);
                    if( squares[ahead]=='K' && king_ending_bonus_dynamic_white[ahead]==0 )
                        bonus += BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                next_passer_mask |= three_files;
                break;
            }
        }
        file_mask   >>= 1;
        three_files >>= 1;
    }
    passer_mask |= next_passer_mask;

    // a5->h5;
    file_mask   = 0x80;             // 0 1000 0000
//  three_files = 0x1c0;            // 1 1100 0000
    for( Square square=a5; square<=h5; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL1;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL1;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL2;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL2;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'P':
            {
                *white_pawns++ = square;
                file = IFILE(square);
                if( file==3 || file==4 )
                    bonus += BONUS_WHITE_PAWN_CENTRAL;
                if( !(passer_mask&file_mask) )
                {
                    *white_passers++ = square;
                    bonus += BONUS_WHITE_PAWN5;
                    #ifdef USE_STRONG_KING
                    Square ahead = NORTH(square);
                    if( squares[ahead]=='K' && king_ending_bonus_dynamic_white[ahead]==0 )
                        bonus += BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_PAWN_CENTRAL;
                break;
            }
        }
        file_mask   >>= 1;
        // three_files >>= 1;
    }

    // a2->h2
    next_passer_mask = 0;
    passer_mask = 0;
    three_files = 0x1c0;   // 1 1100 0000
    for( Square square=a2; square<=h2; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                break;
            }

            case 'B':
            {
                if( a2==square && IsWhite(squares[b3]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else if( h2==square && IsWhite(squares[g3]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else
                {
                    if( IsWhite(squares[NW(square)]) &&
                        IsWhite(squares[NE(square)])
                      )
                    {
                        bonus += BONUS_WHITE_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'Q':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_queen_developed_bonus = BONUS_WHITE_QUEEN_DEVELOPED;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( file<2 || file>5 )
                    white_king_safety_bonus = BONUS_WHITE_KING_SAFETY;
                break;
            }

            case 'r':
            {
                bonus += BONUS_BLACK_ROOK7;
                break;
            }

            case 'q':
            {
                black_queen78_bonus = BONUS_BLACK_QUEEN78;
                break;
            }

            case 'p':
            {
                *black_pawns++   = square;
                *black_passers++ = square;
                bonus += BONUS_BLACK_PAWN7;
                #ifdef USE_STRONG_KING
                Square ahead = SOUTH(square);
                if( squares[ahead]=='k' && king_ending_bonus_dynamic_black[ahead]==0 )
                    bonus -= BONUS_STRONG_KING;
                #endif
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                passer_mask |= three_files;
                break;
            }
        }
        three_files >>= 1;
    }

    // a3->h3
    file_mask = 0x80;       // 0 1000 0000
    three_files = 0x1c0;    // 1 1100 0000
    for( Square square=a3; square<=h3; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL3;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL3;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL0;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL0;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                if( !(passer_mask&file_mask) )
                {
                    *black_passers++ = square;
                    bonus += BONUS_BLACK_PAWN6;
                    #ifdef USE_STRONG_KING
                    Square ahead = SOUTH(square);
                    if( squares[ahead]=='k' && king_ending_bonus_dynamic_black[ahead]==0 )
                        bonus -= BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                next_passer_mask |= three_files;
                break;
            }
        }
        file_mask   >>= 1;
        three_files >>= 1;
    }
    passer_mask |= next_passer_mask;

    // a4->h4
    file_mask   = 0x80;             // 0 1000 0000
//  three_files = 0x1c0;            // 1 1100 0000
    for( Square square=a4; square<=h4; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL2;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL2;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL1;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL1;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                file = IFILE(square);
                if( file==3 || file==4 )
                    bonus += BONUS_BLACK_PAWN_CENTRAL;
                if( !(passer_mask&file_mask) )
                {
                    *black_passers++ = square;
                    bonus += BONUS_BLACK_PAWN5;
                    #ifdef USE_STRONG_KING
                    Square ahead = SOUTH(square);
                    if( squares[ahead]=='k' && king_ending_bonus_dynamic_black[ahead]==0 )
                        bonus -= BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_PAWN_CENTRAL;
                break;
            }
        }
        file_mask   >>= 1;
        // three_files >>= 1;
    }

    // a1->h1
    for( Square square=a1; square<=h1; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                break;
            }

            case 'R':
            {
                white_connected++;
                if( white_connected == 2 )
                    bonus += BONUS_WHITE_CONNECTED_ROOKS;
                break;
            }

            case 'N':
            {
                white_connected=2;
                white_undeveloped_minor_bonus++;
                break;
            }

            case 'B':
            {
                white_connected=2;
                white_undeveloped_minor_bonus++;
                if( a1==square && IsWhite(squares[b2]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else if( h1==square && IsWhite(squares[g2]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else
                {
                    if( IsWhite(squares[NW(square)]) &&
                        IsWhite(squares[NE(square)])
                      )
                    {
                        bonus += BONUS_WHITE_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'Q':
            {
                white_connected=2;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                white_connected=2;
                file = IFILE(square);
                if( file<2 || file>5 )
                    white_king_safety_bonus = BONUS_WHITE_KING_SAFETY;
                break;
            }

            case 'q':
            {
                black_queen78_bonus = BONUS_BLACK_QUEEN78;
                break;
            }
        }
    }

    if( score_white_material > MATERIAL_OPENING )
    {
        bonus += white_king_safety_bonus;
        bonus += white_queen_developed_bonus;
        bonus += white_undeveloped_minor_bonus*WHITE_UNDEVELOPED_MINOR_BONUS;
    }
    else if( score_white_material > MATERIAL_MIDDLE )
    {
        bonus += white_king_safety_bonus;
        bonus += white_queen_central_bonus;
    }
    else
    {
        // bonus += white_king_central_bonus;
        bonus += white_queen78_bonus;
    }

    if( score_black_material < -MATERIAL_OPENING )
    {
        bonus += black_king_safety_bonus;
        bonus += black_queen_developed_bonus;
        bonus += black_undeveloped_minor_bonus*BLACK_UNDEVELOPED_MINOR_BONUS;
    }
    else if( score_black_material < -MATERIAL_MIDDLE )
    {
        bonus += black_king_safety_bonus;
        bonus += black_queen_central_bonus;
    }
    else
    {
        // bonus += black_king_central_bonus;
        bonus += black_queen78_bonus;
    }

    material   = score_white_material + score_black_material;
    if( white )
    {
#ifdef CHECK_FOR_LEAF_MATE
        bool mate=false;
        if( AttackedPiece((Square)wking_square) )
 #ifdef CHECK_FOR_LEAF_MATE_USE_EVALUATE
        {
            TERMINAL terminal_score;
            bool okay = Evaluate(terminal_score);
            if( okay && terminal_score==TERMINAL_WCHECKMATE )   // white mated ?
                mate = true;
        }
 #else
        {

            mate = true;
            MOVELIST list;
            GenMoveList( &list );
            for( int i=0; mate && i<list.count; i++ )
            {
                PushMove( list.moves[i] );
                if( !AttackedPiece((Square)wking_square) )
                    mate = false;
                PopMove( list.moves[i] );
            }
        }
 #endif
        if( mate )
            material = -500;    // white is mated
        else
            material += EnpriseWhite();
#else
        material += EnpriseWhite();
#endif
    }
    else
    {
#ifdef CHECK_FOR_LEAF_MATE
        bool mate=false;
        if( AttackedPiece((Square)bking_square) )
 #ifdef CHECK_FOR_LEAF_MATE_USE_EVALUATE
        {
            TERMINAL terminal_score;
            bool okay = Evaluate(terminal_score);
            if( okay && terminal_score==TERMINAL_BCHECKMATE )   // black mated ?
                mate = true;
        }
 #else
        {
            mate = true;
            MOVELIST list;
            GenMoveList( &list );
            for( int i=0; mate && i<list.count; i++ )
            {
                PushMove( list.moves[i] );
                if( !AttackedPiece((Square)bking_square) )
                    mate = false;
                PopMove( list.moves[i] );
            }
        }
 #endif
        if( mate )
            material = 500;    // black is mated
        else
            material -= EnpriseBlack();
#else
        material -= EnpriseBlack();
#endif
    }
    positional = bonus;

    // Reward stronger side with a bonus for swapping pieces not pawns
    if( material>0 && planning_white_piece_pawn_percent ) // if white ahead
                                                          //   and a figure to compare to
    {
        int score_white_pawns = score_white_material - 500 // -500 is king
                                - score_white_pieces;
        int piece_pawn_percent = 1000;
        if( score_white_pawns )
        {
            piece_pawn_percent = (100*score_white_pieces) /
                                      score_white_pawns;
            if( piece_pawn_percent > 1000 )
                piece_pawn_percent = 1000;
        }
        // start of game
        //  piece_pawn_percent = 100* Q+2R+2B+2N/8P = 100 * (190+120)/80
        //                     = 400 (approx)
        // typical endings
        //  piece_pawn_percent = 100* R+B/5P = 100 * (80)/50
        //                     = 160
        //  piece_pawn_percent = 100* R/5P = 100 * 50/50
        //                     = 100  after swapping a bishop
        //  piece_pawn_percent = 100* R+B/P = 100 * (80)/10
        //                     = 800
        //  piece_pawn_percent = 100* R/P = 100 * 50/10
        //                     = 500  after swapping a bishop
        //
        //  Lower numbers are better for the stronger side, calculate
        //  an adjustment as follows;
        //   up to +0.8 pawns for improved ratio for white as stronger side
        //   up to -0.8 pawns for worse ratio for white as stronger side
        int piece_pawn_ratio_adjustment = 8 - (8*piece_pawn_percent)/planning_white_piece_pawn_percent;
        if( piece_pawn_ratio_adjustment < -8 )
            piece_pawn_ratio_adjustment = -8;
        //   eg planning_white_piece_pawn_percent = 160
        //      now            piece_pawn_percent = 160
        //      adjustment = 0
        //   eg planning_white_piece_pawn_percent = 160
        //      now            piece_pawn_percent = 100
        //      adjustment = +3 (i.e. +0.3 pawns)
        //   eg planning_white_piece_pawn_percent = 800
        //      now            piece_pawn_percent = 500
        //      adjustment = +3 (i.e. +0.3 pawns)
        //   eg planning_white_piece_pawn_percent = 100
        //      now            piece_pawn_percent = 160
        //      adjustment = -4 (i.e. -0.4 pawns)
        //   eg planning_white_piece_pawn_percent = 500
        //      now            piece_pawn_percent = 800
        //      adjustment = -4 (i.e. -0.4 pawns)

        // If white is better, positive adjustment increases +ve material advantage
        material += piece_pawn_ratio_adjustment;
    }
    else if( material<0 && planning_black_piece_pawn_percent ) // if black ahead
                                                               //   and a figure to compare to
    {
        int score_black_pawns = (0-score_black_material) - 500 // -500 is king
                                - score_black_pieces;
        int piece_pawn_percent = 1000;
        if( score_black_pawns )
        {
            piece_pawn_percent = (100*score_black_pieces) /
                                      score_black_pawns;
            if( piece_pawn_percent > 1000 )
                piece_pawn_percent = 1000;
        }
        int piece_pawn_ratio_adjustment = 8 - (8*piece_pawn_percent)/planning_black_piece_pawn_percent;
        if( piece_pawn_ratio_adjustment < -8 )
            piece_pawn_ratio_adjustment = -8;

        // If black is better, positive adjustment increases -ve material advantage
        material -= piece_pawn_ratio_adjustment;
    }

    // Check whether white king is in square of black passers
    if( score_white_pieces==0 )
    {
        bool black_will_queen = false;
        #ifdef USE_IN_THE_SQUARE
        while( black_passers>black_passers_buf
                && white_king_square != SQUARE_INVALID
                && !black_will_queen )
        {
            --black_passers;
            Square square = *black_passers;
            int pfile = IFILE(square);
            int prank = IRANK(square);
            int kfile = IFILE(white_king_square);
            int krank = IRANK(white_king_square);

            // Calculations assume it is black to move; so if it is white
            //  shift pawn one more square away from queening square
            if( white )
                prank++;

            // Will queen if eg Pa3, Ka4
            if( prank < krank )
                black_will_queen = true;
            else
            {

                // Will queen if eg Pa3, Kd3
                if( kfile > pfile )
                    black_will_queen = (kfile-pfile > prank); // eg d-a=3 > 2

                // Will queen if eg Ph3, Ke3
                else if( kfile < pfile )
                    black_will_queen = (pfile-kfile > prank); // eg h-e=3 > 2
            }
        }

        // If white has a bare king, a pawn protecting a pawn always wins and a pair of pawns
        //  separated by 5 or 6 files always wins (even if white to move captures one)
        //  Note at the planning stage white had material - so are encouraging liquidation to
        //  easily winning pawn endings
        #ifdef USE_LIQUIDATION
        while( score_white_material==500 && planning_score_white_pieces && black_pawns>black_pawns_buf
                 && !black_will_queen )
        {
            int nbr_separating_files = (white?5:4);
            --black_pawns;
            Square square = *black_pawns;
            int pfile1 = IFILE(square);
            int prank1 = IRANK(square);
            Square *p = black_pawns;
            while( p > black_pawns_buf )
            {
                p--;
                Square square2 = *p;
                int pfile2 = IFILE(square2);
                int prank2 = IRANK(square2);
                if( (prank2==prank1+1 || prank2+1==prank1) &&
                    (pfile2==pfile1+1 || pfile2+1==pfile1)
                  )
                {
                    black_will_queen = true;  // pawn protects pawn
                }
                else if( pfile2>pfile1+nbr_separating_files || pfile1>pfile2+nbr_separating_files )
                {
                    black_will_queen = true;  // pawns separated by 5 or more empty files
                }
            }
        }
        #endif
        if( black_will_queen )
            material -= 65; // almost as good as a black queen (if it's too close we might not actually queen!)
    }

    // Check whether black king is in square of white passers
    if( score_black_pieces==0 )
    {
        bool white_will_queen = false;
        #ifdef USE_IN_THE_SQUARE
        while( white_passers>white_passers_buf
                && black_king_square != SQUARE_INVALID
                && !white_will_queen )
        {
            --white_passers;
            Square square = *white_passers;
            int pfile = IFILE(square);
            int prank = IRANK(square);
            int kfile = IFILE(black_king_square);
            int krank = IRANK(black_king_square);

            // Calculations assume it is white to move; so if it is black
            //  shift pawn one more square away from queening square
            if( !white )
                prank--;

            // Will queen if eg Pa6, Ka5
            if( prank > krank )
                white_will_queen = true;
            else
            {

                // Will queen if eg Pa6, Kd6
                if( kfile > pfile )
                    white_will_queen = (kfile-pfile > 7-prank); // eg d-a=3 > 7-5=2

                // Will queen if eg Ph3, Ke3
                else if( kfile < pfile )
                    white_will_queen = (pfile-kfile > 7-prank); // eg h-e=3 > 7-5=2
            }
        }
        #endif

        // If black has a bare king, a pawn protecting a pawn always wins and a pair of pawns
        //  separated by 5 or 6 files always wins (even if black to move captures one)
        //  Note at the planning stage black had material - so are encouraging liquidation to
        //  easily winning pawn endings
        #ifdef USE_LIQUIDATION
        while( score_black_material==-500 && planning_score_black_pieces && white_pawns>white_pawns_buf
                 && !white_will_queen )
        {
            int nbr_separating_files = (!white?5:4);
            --white_pawns;
            Square square = *white_pawns;
            int pfile1 = IFILE(square);
            int prank1 = IRANK(square);
            Square *p = white_pawns;
            while( p > white_pawns_buf )
            {
                p--;
                Square square2 = *p;
                int pfile2 = IFILE(square2);
                int prank2 = IRANK(square2);
                if( (prank2==prank1+1 || prank2+1==prank1) &&
                    (pfile2==pfile1+1 || pfile2+1==pfile1)
                  )
                {
                    white_will_queen = true;  // pawn protects pawn
                }
                else if( pfile2>pfile1+nbr_separating_files || pfile1>pfile2+nbr_separating_files )
                {
                    white_will_queen = true;  // pawns separated by 5 or more empty files
                }
            }
        }
        #endif
        if( white_will_queen )
            material += 65; // almost as good as a white queen (if it's too close we might not actually queen!)
    }
    #endif
//  int score_test = material*4 /*balance=4*/ + positional;
//  int score_test_cp = (score_test*10)/4;
//  if( score_test_cp > 4000 )
//      cprintf( "too much" );   // Problem fen "k7/8/PPK5/8/8/8/8/8 w - - 0 1"
}


/****************************************************************************
 * Create a list of all legal moves (sorted strongest first, public version)
 ****************************************************************************/
void ChessEvaluation::GenLegalMoveListSorted( vector<Move> &moves )
{
    MOVELIST movelist;
    GenLegalMoveListSorted( &movelist );
    for( int i=0; i<movelist.count; i++ )
        moves.push_back( movelist.moves[i] );
}

/****************************************************************************
 * Create a list of all legal moves (sorted strongest first)
 ****************************************************************************/
// Sort moves according to their score
struct MOVE_IDX
{
    int score;
    int idx;
    bool operator <  (const MOVE_IDX& arg) const { return score <  arg.score; }
    bool operator >  (const MOVE_IDX& arg) const { return score >  arg.score; }
    bool operator == (const MOVE_IDX& arg) const { return score == arg.score; }
};
void ChessEvaluation::GenLegalMoveListSorted( MOVELIST *list )
{
    int i, j;
    bool okay;
    TERMINAL terminal_score;
    MOVELIST list2;
    vector<MOVE_IDX> sortable;

    // Call this before calls to EvaluateLeaf()
    Planning();

    // Generate all moves, including illegal (eg put king in check) moves
    GenMoveList( &list2 );

    // Loop copying the proven good ones
    for( i=j=0; i<list2.count; i++ )
    {
        PushMove( list2.moves[i] );
        okay = Evaluate(terminal_score);
        if( okay )
        {
            int score=0;
            if( terminal_score == TERMINAL_WCHECKMATE )  // White is checkmated
                score = -1000000;
            else if( terminal_score == TERMINAL_BCHECKMATE )  // Black is checkmated
                score = 1000000;
            else if( terminal_score==TERMINAL_WSTALEMATE ||
                     terminal_score==TERMINAL_BSTALEMATE )
                score = 0;
            else
            {
                int material, positional;
                EvaluateLeaf(material,positional);
                score = material*4 /*balance=4*/ + positional;
            }
            MOVE_IDX x;
            x.score = score;
            x.idx   = i;
            sortable.push_back(x);
        }
        PopMove( list2.moves[i] );
    }
    if( white )
        sort( sortable.rbegin(), sortable.rend() );
    else
        sort( sortable.begin(), sortable.end() );
    for( i=0; i<(int)sortable.size(); i++ )
    {
        MOVE_IDX x = sortable[i];
        list->moves[i] = list2.moves[x.idx];
    }
    list->count  = i;
}

/****************************************************************************
 * Move.cpp Chess classes - Move
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

/****************************************************************************
 * Read natural string move eg "Nf3"
 *  return bool okay
 ****************************************************************************/
bool Move::NaturalIn( ChessRules *cr, const char *natural_in )
{
    MOVELIST list;
    int  i, len=0;
    char src_file='\0', src_rank='\0', dst_file='\0', dst_rank='\0';
    char promotion='\0';
    bool enpassant=false;
    bool kcastling=false;
    bool qcastling=false;
    Square dst_=a8;
    Move *m, *found=NULL;
    char *s;
    char  move[10];
    bool  white=cr->white;
    char  piece=(white?'P':'p');
    bool  default_piece=true;

    // Indicate no move found (yet)
    bool okay=true;

    // Copy to read-write variable
    okay = false;
    for( i=0; i<sizeof(move); i++ )
    {
        move[i] = natural_in[i];
        if( move[i]=='\0' || move[i]==' ' || move[i]=='\t' ||
            move[i]=='\r' || move[i]=='\n' )
        {
            move[i] = '\0';
            okay = true;
            break;
        }
    }
    if( okay )
    {

        // Trim string from end
        s = strchr(move,'\0') - 1;
        while( s>=move && !(isascii(*s) && isalnum(*s)) )
            *s-- = '\0';

        // Trim string from start
        s = move;
        while( *s==' ' || *s=='\t' )
            s++;
        len = (int)strlen(s);
        for( i=0; i<len+1; i++ )  // +1 gets '\0' at end
            move[i] = *s++;  // if no leading space this does
                            //  nothing, but no harm either

        // Trim enpassant
        if( len>=2 && move[len-1]=='p' )
        {
            if( 0 == strcmp(&move[len-2],"ep") )
            {
                move[len-2] = '\0';
                enpassant = true;
            }
            else if( len>=3 && 0==strcmp(&move[len-3],"e.p") )
            {
                move[len-3] = '\0';
                enpassant = true;
            }

            // Trim string from end, again
            s = strchr(move,'\0') - 1;
            while( s>=move && !(isascii(*s) && isalnum(*s)) )
                *s-- = '\0';
            len = (int)strlen(move);
        }

        // Promotion
        if( len>2 )  // We are supporting "ab" to mean Pawn a5xb6 (say), and this test makes sure we don't
        {            // mix that up with a lower case bishop promotion, and that we don't reject "ef" say
                     // on the basis that 'F' is not a promotion indication. We've never supported "abQ" say
                     // as a7xb8=Q, and we still don't so "abb" as a bishop promotion doesn't work, but we
                     // continue to support "ab=Q", and even "ab=b".
                     // The test also ensures we can access move[len-2] below
                     // These comments added when we changed the logic to support "b8Q" and "a7xb8Q", the
                     // '=' can optionally be omitted in such cases, the first change in this code for many,
                     // many years.
            char last = move[len-1];
            bool is_file = ('1'<=last && last<='8');
            if( !is_file )
            {
                switch( last )
                {
                    case 'O':
                    case 'o':   break;  // Allow castling!
                    case 'q':
                    case 'Q':   promotion='Q';  break;
                    case 'r':
                    case 'R':   promotion='R';  break;
                    case 'b':   if( len==3 && '2'<=move[1] && move[1]<='7' )
                                    break;  // else fall through to promotion - allows say "a5b" as disambiguating
                                            //  version of "ab" if there's more than one "ab" available! Something
                                            //  of an ultra refinement
                    case 'B':   promotion='B';  break;
                    case 'n':
                    case 'N':   promotion='N';  break;
                    default:    okay = false;   break;   // Castling and promotions are the only cases longer than 2
                                                         //  chars where a non-file ends a move. (Note we still accept
                                                         //  2 character pawn captures like "ef").
                }
                if( promotion )
                {
                    switch( move[len-2] )
                    {
                        case '=':
                        case '1':   // we now allow '=' to be omitted, as eg ChessBase mobile seems to (sometimes?)
                        case '8':   break;
                        default:    okay = false;   break;
                    }
                    if( okay )
                    {

                        // Trim string from end, again
                        move[len-1] = '\0';     // Get rid of 'Q', 'N' etc
                        s = move + len-2;
                        while( s>=move && !(isascii(*s) && isalnum(*s)) )
                            *s-- = '\0';    // get rid of '=' but not '1','8'
                        len = (int)strlen(move);
                    }
                }
            }
        }
    }

    // Castling
    if( okay )
    {
        if( 0==strcmp_ignore(move,"oo") || 0==strcmp_ignore(move,"o-o") )
        {
            strcpy( move, (white?"e1g1":"e8g8") );
            len       = 4;
            piece     = (white?'K':'k');
            default_piece = false;
            kcastling = true;
        }
        else if( 0==strcmp_ignore(move,"ooo") || 0==strcmp_ignore(move,"o-o-o") )
        {
            strcpy( move, (white?"e1c1":"e8c8") );
            len       = 4;
            piece     = (white?'K':'k');
            default_piece = false;
            qcastling = true;
        }
    }

    // Destination square for all except pawn takes pawn (eg "ef")
    if( okay )
    {
        if( len==2 && 'a'<=move[0] && move[0]<='h'
                   && 'a'<=move[1] && move[1]<='h' )
        {
            src_file = move[0]; // eg "ab" pawn takes pawn
            dst_file = move[1];
        }
        else if( len==3 && 'a'<=move[0] && move[0]<='h'
                        && '2'<=move[1] && move[1]<='7'
                        && 'a'<=move[2] && move[2]<='h' )
        {
            src_file = move[0]; // eg "a3b"  pawn takes pawn
            dst_file = move[2];
        }
        else if( len>=2 && 'a'<=move[len-2] && move[len-2]<='h'
                        && '1'<=move[len-1] && move[len-1]<='8' )
        {
            dst_file = move[len-2];
            dst_rank = move[len-1];
            dst_ = SQ(dst_file,dst_rank);
        }
        else
            okay = false;
    }

    // Source square and or piece
    if( okay )
    {
        if( len > 2 )
        {
            if( 'a'<=move[0] && move[0]<='h' &&
                '1'<=move[1] && move[1]<='8' )
            {
                src_file = move[0];
                src_rank = move[1];
            }
            else
            {
                switch( move[0] )
                {
                    case 'K':   piece = (white?'K':'k');    default_piece=false; break;
                    case 'Q':   piece = (white?'Q':'q');    default_piece=false; break;
                    case 'R':   piece = (white?'R':'r');    default_piece=false; break;
                    case 'N':   piece = (white?'N':'n');    default_piece=false; break;
                    case 'P':   piece = (white?'P':'p');    default_piece=false; break;
                    case 'B':   piece = (white?'B':'b');    default_piece=false; break;
                    default:
                    {
                        if( 'a'<=move[0] && move[0]<='h' )
                            src_file = move[0]; // eg "ef4"
                        else
                            okay = false;
                        break;
                    }
                }
                if( len>3  && src_file=='\0' )  // not eg "ef4" above
                {
                    if( '1'<=move[1] && move[1]<='8' )
                        src_rank = move[1];
                    else if( 'a'<=move[1] && move[1]<='h' )
                    {
                        src_file = move[1];
                        if( len>4 && '1'<=move[2] && move[2]<='8' )
                            src_rank = move[2];
                    }
                }
            }
        }
    }

    // Check against all possible moves
    if( okay )
    {
        cr->GenLegalMoveList( &list );

        // Have source and destination, eg "d2d3"
        if( enpassant )
            src_rank = dst_rank = '\0';
        if( src_file && src_rank && dst_file && dst_rank )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( (default_piece || piece==cr->squares[m->src])  &&
                    src_file  ==   FILE(m->src)       &&
                    src_rank  ==   RANK(m->src)       &&
                    dst_       ==   m->dst
                )
                {
                    if( kcastling )
                    {
                        if( m->special ==
                             (white?SPECIAL_WK_CASTLING:SPECIAL_BK_CASTLING) )
                            found = m;
                    }
                    else if( qcastling )
                    {
                        if( m->special ==
                             (white?SPECIAL_WQ_CASTLING:SPECIAL_BQ_CASTLING) )
                            found = m;
                    }
                    else
                        found = m;
                    break;
                }
            }
        }

        // Have source file only, eg "Rae1"
        else if( src_file && dst_file && dst_rank )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]  &&
                    src_file  ==   FILE(m->src)         &&
                 /* src_rank  ==   RANK(m->src)  */
                    dst_       ==   m->dst
                )
                {
                    found = m;
                    break;
                }
            }
        }

        // Have source rank only, eg "R2d2"
        else if( src_rank && dst_file && dst_rank )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]   &&
                 /* src_file  ==   FILE(m->src) */
                    src_rank  ==   RANK(m->src)          &&
                    dst_       ==   m->dst
                )
                {
                    found = m;
                    break;
                }
            }
        }

        // Have destination file only eg e4f (because 2 ef moves are possible)
        else if( src_file && src_rank && dst_file )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]      &&
                    src_file  ==   FILE(m->src)             &&
                    src_rank  ==   RANK(m->src)             &&
                    dst_file  ==   FILE(m->dst)
                )
                {
                    found = m;
                    break;
                }
            }
        }

        // Have files only, eg "ef"
        else if( src_file && dst_file )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]      &&
                    src_file  ==   FILE(m->src)             &&
                 /* src_rank  ==   RANK(m->src) */
                    dst_file  ==   FILE(m->dst)
                )
                {
                    if( enpassant )
                    {
                        if( m->special ==
                             (white?SPECIAL_WEN_PASSANT:SPECIAL_BEN_PASSANT) )
                            found = m;
                    }
                    else
                        found = m;
                    break;
                }
            }
        }

        // Have destination square only eg "a4"
        else if( dst_rank && dst_file )
        {
            for( i=0; i<list.count; i++ )
            {
                m = &list.moves[i];
                if( piece     ==   cr->squares[m->src]          &&
                    dst_       ==   m->dst
                )
                {
                    found = m;
                    break;
                }
            }
        }
    }

    // Copy found move
    if( okay && found )
    {
        bool found_promotion =
            ( found->special == SPECIAL_PROMOTION_QUEEN ||
              found->special == SPECIAL_PROMOTION_ROOK ||
              found->special == SPECIAL_PROMOTION_BISHOP ||
              found->special == SPECIAL_PROMOTION_KNIGHT
            );
        if( promotion && !found_promotion )
            okay = false;
        if( found_promotion )
        {
            switch( promotion )
            {
                default:
                case 'Q': found->special = SPECIAL_PROMOTION_QUEEN;   break;
                case 'R': found->special = SPECIAL_PROMOTION_ROOK;    break;
                case 'B': found->special = SPECIAL_PROMOTION_BISHOP;  break;
                case 'N': found->special = SPECIAL_PROMOTION_KNIGHT;  break;
            }
        }
    }
    if( !found )
        okay = false;
    if( okay )
        *this = *found;
    return okay;
}

/****************************************************************************
 * Read natural string move eg "Nf3"
 *  return bool okay
 * Fast alternative for known good input
 ****************************************************************************/
bool Move::NaturalInFast( ChessRules *cr, const char *natural_in )
{
    bool err = false;
    bool found = false;
    bool capture_ = false;
    Move mv;

    /*
     Handles moves of the following type
     exd8=N
     e8=B
     exd8N
     e8B
     exd5
     e4
     Nf3
     Nxf3
     Nef3
     Nexf3
     N2f3
     N2xf3
     O-O
     O-O-O
     */

//
    mv.special = NOT_SPECIAL;
    mv.capture = ' ';
    char f = *natural_in++;

    // WHITE MOVE
    if( cr->white )
    {

        // Pawn move ?
        if( 'a'<=f && f<='h' )
        {
            char r = *natural_in++;
            if( r != 'x')
            {

                // Non capturing, non promoting pawn move
                if( '3'<= r && r<= '7')
                {
                    mv.dst = SQ(f,r);
                    mv.src = SOUTH(mv.dst);
                    if( cr->squares[mv.src]=='P' && cr->squares[mv.dst]==' ')
                        found =true;
                    else if( r=='4' && cr->squares[mv.src]==' ' && cr->squares[mv.dst]==' ')
                    {
                        mv.special = SPECIAL_WPAWN_2SQUARES;
                        mv.src = SOUTH(mv.src);
                        found = (cr->squares[mv.src]=='P');
                    }
                }

                // Non capturing, promoting pawn move
                else if( r=='8' )
                {
                    if( *natural_in == '=' )    // now optional
                        natural_in++;
                    mv.dst = SQ(f,r);
                    mv.src = SOUTH(mv.dst);
                    if( cr->squares[mv.src]=='P' && cr->squares[mv.dst]==' ')
                    {
                        switch( *natural_in++ )
                        {
                            default:
                            {
                                break;
                            }
                            case 'Q':
                            case 'q':
                            {
                                mv.special = SPECIAL_PROMOTION_QUEEN;
                                found = true;
                                break;
                            }
                            case 'R':
                            case 'r':
                            {
                                mv.special = SPECIAL_PROMOTION_ROOK;
                                found = true;
                                break;
                            }
                            case 'N':
                            case 'n':
                            {
                                mv.special = SPECIAL_PROMOTION_KNIGHT;
                                found = true;
                                break;
                            }
                            case 'B':
                            case 'b':
                            {
                                mv.special = SPECIAL_PROMOTION_BISHOP;
                                found = true;
                                break;
                            }
                        }
                    }
                }
            }
            else // if ( r == 'x' )
            {
                char g = *natural_in++;
                if( 'a'<=g && g<='h' )
                {
                    r = *natural_in++;

                    // Non promoting, capturing pawn move
                    if( '3'<= r && r<= '7')
                    {
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r-1);
                        if( cr->squares[mv.src]=='P' && IsBlack(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            found = true;
                        }
                        else if( r=='6' && cr->squares[mv.src]=='P' && cr->squares[mv.dst]==' '  && mv.dst==cr->enpassant_target && cr->squares[SOUTH(mv.dst)]=='p' )
                        {
                            mv.capture = 'p';
                            mv.special = SPECIAL_WEN_PASSANT;
                            found = true;
                        }
                    }

                    // Promoting, capturing pawn move
                    else if( r=='8' )
                    {
                        if( *natural_in == '=' )    // now optional
                            natural_in++;
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r-1);
                        if( cr->squares[mv.src]=='P' && IsBlack(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            switch( *natural_in++ )
                            {
                                default:
                                {
                                    break;
                                }
                                case 'Q':
                                case 'q':
                                {
                                    mv.special = SPECIAL_PROMOTION_QUEEN;
                                    found = true;
                                    break;
                                }
                                case 'R':
                                case 'r':
                                {
                                    mv.special = SPECIAL_PROMOTION_ROOK;
                                    found = true;
                                    break;
                                }
                                case 'N':
                                case 'n':
                                {
                                    mv.special = SPECIAL_PROMOTION_KNIGHT;
                                    found = true;
                                    break;
                                }
                                case 'B':
                                case 'b':
                                {
                                    mv.special = SPECIAL_PROMOTION_BISHOP;
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {

            // Piece move
            const lte **ray_lookup = queen_lookup;
            switch( f )
            {
                case 'O':
                {
                    if( 0 == memcmp(natural_in,"-O-O",4) )
                    {
                        natural_in += 4;
                        mv.src = e1;
                        mv.dst = c1;
                        mv.capture = ' ';
                        mv.special = SPECIAL_WQ_CASTLING;
                        found = true;
                    }
                    else if( 0 == memcmp(natural_in,"-O",2) )
                    {
                        natural_in += 2;
                        mv.src = e1;
                        mv.dst = g1;
                        mv.capture = ' ';
                        mv.special = SPECIAL_WK_CASTLING;
                        found = true;
                    }
                    break;
                }

                // King is simple special case - there's only one king so no disambiguation
                case 'K':
                {
                    f = *natural_in++;
                    if( f == 'x' )
                    {
                        capture_ = true;
                        f = *natural_in++;
                    }
                    if( 'a'<= f && f<='h' )
                    {
                        char r = *natural_in++;
                        if( '1'<=r && r<='8' )
                        {
                            mv.src = cr->wking_square;
                            mv.dst = SQ(f,r);
                            mv.special = SPECIAL_KING_MOVE;
                            mv.capture = cr->squares[mv.dst];
                            found = ( capture_ ? IsBlack(mv.capture) : IsEmptySquare(mv.capture) );
                        }
                    }
                    break;
                }

                // Other pieces may need to check legality for disambiguation
                case 'Q':   ray_lookup = queen_lookup;                      // fall through
                case 'R':   if( f=='R' )    ray_lookup = rook_lookup;       // fall through
                case 'B':   if( f=='B' )    ray_lookup = bishop_lookup;     // fall through
                case 'N':
                {
                    char piece = f;
                    f = *natural_in++;
                    char src_file='\0';
                    char src_rank='\0';
                    if( f == 'x' )
                    {
                        capture_ = true;
                        f = *natural_in++;
                    }
                    if( '1'<=f && f<='8' )
                    {
                        src_rank = f;
                    }
                    else if( 'a'<= f && f<='h' )
                    {
                        src_file = f;
                    }
                    else
                        err = true;
                    if( !err )
                    {
                        char g = *natural_in++;
                        if( g == 'x' )
                        {
                            if( capture_ )
                                err = true;
                            else
                            {
                                capture_ = true;
                                g = *natural_in++;
                            }
                        }
                        if( '1'<=g && g<='8' )
                        {
                            if( src_file )
                            {
                                mv.dst = SQ(src_file,g);
                                src_file = '\0';
                            }
                            else
                                err = true;
                        }
                        else if( 'a'<=g && g<='h' )
                        {
                            char dst_rank = *natural_in++;
                            if( '1'<=dst_rank && dst_rank<='8' )
                                mv.dst = SQ(g,dst_rank);
                            else
                                err = true;
                        }
                        else
                            err = true;
                        if( !err )
                        {
                            if( capture_ ? IsBlack(cr->squares[mv.dst]) : cr->squares[mv.dst]==' ' )
                            {
                                mv.capture = cr->squares[mv.dst];
                                if( piece == 'N' )
                                {
                                    int count=0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = knight_lookup[mv.dst];
                                        lte nbr_moves = *ptr++;
                                        while( !found && nbr_moves-- )
                                        {
                                            Square src_ = (Square)*ptr++;
                                            if( cr->squares[src_]=='N' )
                                            {
                                                bool src_file_ok = !src_file || FILE(src_)==src_file;
                                                bool src_rank_ok = !src_rank || RANK(src_)==src_rank;
                                                if( src_file_ok && src_rank_ok )
                                                {
                                                    mv.src = src_;
                                                    if( probe == 0 )
                                                        count++;
                                                    else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                        // we are not exposing white king to check.
                                                    {
                                                        char temp = cr->squares[mv.dst];
                                                        cr->squares[mv.dst] = 'N';  // temporarily make move
                                                        cr->squares[src_] = ' ';
                                                        found = !cr->AttackedSquare( cr->wking_square, false ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                        cr->squares[mv.dst] = temp;  // now undo move
                                                        cr->squares[src_] = 'N';
                                                    }
                                                }
                                            }
                                        }
                                        if( probe==0 && count==1 )
                                            found = true; // done, no need for disambiguation by check
                                    }
                                }
                                else // if( rook, bishop, queen )
                                {
                                    int count = 0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = ray_lookup[mv.dst];
                                        lte nbr_rays = *ptr++;
                                        while( !found && nbr_rays-- )
                                        {
                                            lte ray_len = *ptr++;
                                            while( !found && ray_len-- )
                                            {
                                                Square src_ = (Square)*ptr++;
                                                if( !IsEmptySquare(cr->squares[src_]) )
                                                {
                                                    // Any piece, friend or enemy must move to end of ray
                                                    ptr += ray_len;
                                                    ray_len = 0;
                                                    if( cr->squares[src_] == piece )
                                                    {
                                                        bool src_file_ok = !src_file || FILE(src_)==src_file;
                                                        bool src_rank_ok = !src_rank || RANK(src_)==src_rank;
                                                        if( src_file_ok && src_rank_ok )
                                                        {
                                                            mv.src = src_;
                                                            if( probe == 0 )
                                                                count++;
                                                            else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                                // we are not exposing white king to check.
                                                            {
                                                                char temp = cr->squares[mv.dst];
                                                                cr->squares[mv.dst] = piece;  // temporarily make move
                                                                cr->squares[mv.src] = ' ';
                                                                found = !cr->AttackedSquare( cr->wking_square, false ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                                cr->squares[mv.dst] = temp;  // now undo move
                                                                cr->squares[mv.src] = piece;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        if( probe==0 && count==1 )
                                            found = true; // done, no need for disambiguation by check
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // BLACK MOVE
    else
    {
        // Pawn move ?
        if( 'a'<=f && f<='h' )
        {
            char r = *natural_in++;
            if( r != 'x')
            {

                // Non capturing, non promoting pawn move
                if( '2'<= r && r<= '6')
                {
                    mv.dst = SQ(f,r);
                    mv.src = NORTH(mv.dst);
                    if( cr->squares[mv.src]=='p' && cr->squares[mv.dst]==' ')
                        found =true;
                    else if( r=='5' && cr->squares[mv.src]==' ' && cr->squares[mv.dst]==' ')
                    {
                        mv.special = SPECIAL_BPAWN_2SQUARES;
                        mv.src = NORTH(mv.src);
                        found = (cr->squares[mv.src]=='p');
                    }
                }

                // Non capturing, promoting pawn move
                else if( r=='1' )
                {
                    if( *natural_in == '=' )    // now optional
                        natural_in++;
                    mv.dst = SQ(f,r);
                    mv.src = NORTH(mv.dst);
                    if( cr->squares[mv.src]=='p' && cr->squares[mv.dst]==' ')
                    {
                        switch( *natural_in++ )
                        {
                            default:
                            {
                                break;
                            }
                            case 'Q':
                            case 'q':
                            {
                                mv.special = SPECIAL_PROMOTION_QUEEN;
                                found = true;
                                break;
                            }
                            case 'R':
                            case 'r':
                            {
                                mv.special = SPECIAL_PROMOTION_ROOK;
                                found = true;
                                break;
                            }
                            case 'N':
                            case 'n':
                            {
                                mv.special = SPECIAL_PROMOTION_KNIGHT;
                                found = true;
                                break;
                            }
                            case 'B':
                            case 'b':
                            {
                                mv.special = SPECIAL_PROMOTION_BISHOP;
                                found = true;
                                break;
                            }
                        }
                    }
                }
            }
            else // if ( r == 'x' )
            {
                char g = *natural_in++;
                if( 'a'<=g && g<='h' )
                {
                    r = *natural_in++;

                    // Non promoting, capturing pawn move
                    if( '2'<= r && r<= '6')
                    {
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r+1);
                        if( cr->squares[mv.src]=='p' && IsWhite(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            found = true;
                        }
                        else if( r=='3' && cr->squares[mv.src]=='p' && cr->squares[mv.dst]==' '  && mv.dst==cr->enpassant_target && cr->squares[NORTH(mv.dst)]=='P' )
                        {
                            mv.capture = 'P';
                            mv.special = SPECIAL_BEN_PASSANT;
                            found = true;
                        }
                    }

                    // Promoting, capturing pawn move
                    else if( r=='1' )
                    {
                        if( *natural_in == '=' )    // now optional
                            natural_in++;
                        mv.dst = SQ(g,r);
                        mv.src = SQ(f,r+1);
                        if( cr->squares[mv.src]=='p' && IsWhite(cr->squares[mv.dst]) )
                        {
                            mv.capture = cr->squares[mv.dst];
                            switch( *natural_in++ )
                            {
                                default:
                                {
                                    break;
                                }
                                case 'Q':
                                case 'q':
                                {
                                    mv.special = SPECIAL_PROMOTION_QUEEN;
                                    found = true;
                                    break;
                                }
                                case 'R':
                                case 'r':
                                {
                                    mv.special = SPECIAL_PROMOTION_ROOK;
                                    found = true;
                                    break;
                                }
                                case 'N':
                                case 'n':
                                {
                                    mv.special = SPECIAL_PROMOTION_KNIGHT;
                                    found = true;
                                    break;
                                }
                                case 'B':
                                case 'b':
                                {
                                    mv.special = SPECIAL_PROMOTION_BISHOP;
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {

            // Piece move
            const lte **ray_lookup=queen_lookup;
            switch( f )
            {
                case 'O':
                {
                    if( 0 == memcmp(natural_in,"-O-O",4) )
                    {
                        natural_in += 4;
                        mv.src = e8;
                        mv.dst = c8;
                        mv.capture = ' ';
                        mv.special = SPECIAL_BQ_CASTLING;
                        found = true;
                    }
                    else if( 0 == memcmp(natural_in,"-O",2) )
                    {
                        natural_in += 2;
                        mv.src = e8;
                        mv.dst = g8;
                        mv.capture = ' ';
                        mv.special = SPECIAL_BK_CASTLING;
                        found = true;
                    }
                    break;
                }

                // King is simple special case - there's only one king so no disambiguation
                case 'K':
                {
                    f = *natural_in++;
                    if( f == 'x' )
                    {
                        capture_ = true;
                        f = *natural_in++;
                    }
                    if( 'a'<= f && f<='h' )
                    {
                        char r = *natural_in++;
                        if( '1'<=r && r<='8' )
                        {
                            mv.src = cr->bking_square;
                            mv.dst = SQ(f,r);
                            mv.special = SPECIAL_KING_MOVE;
                            mv.capture = cr->squares[mv.dst];
                            found = ( capture_ ? IsWhite(mv.capture) : IsEmptySquare(mv.capture) );
                        }
                    }
                    break;
                }

                // Other pieces may need to check legality for disambiguation
                case 'Q':   ray_lookup = queen_lookup;                      // fall through
                case 'R':   if( f=='R' )    ray_lookup = rook_lookup;       // fall through
                case 'B':   if( f=='B' )    ray_lookup = bishop_lookup;     // fall through
                case 'N':
                {
                    char piece = static_cast<char>(tolower(f));
                    f = *natural_in++;
                    char src_file='\0';
                    char src_rank='\0';
                    if( f == 'x' )
                    {
                        capture_ = true;
                        f = *natural_in++;
                    }
                    if( '1'<=f && f<='8' )
                    {
                        src_rank = f;
                    }
                    else if( 'a'<= f && f<='h' )
                    {
                        src_file = f;
                    }
                    else
                        err = true;
                    if( !err )
                    {
                        char g = *natural_in++;
                        if( g == 'x' )
                        {
                            if( capture_ )
                                err = true;
                            else
                            {
                                capture_ = true;
                                g = *natural_in++;
                            }
                        }
                        if( '1'<=g && g<='8' )
                        {
                            if( src_file )
                            {
                                mv.dst = SQ(src_file,g);
                                src_file = '\0';
                            }
                            else
                                err = true;
                        }
                        else if( 'a'<=g && g<='h' )
                        {
                            char dst_rank = *natural_in++;
                            if( '1'<=dst_rank && dst_rank<='8' )
                                mv.dst = SQ(g,dst_rank);
                            else
                                err = true;
                        }
                        else
                            err = true;
                        if( !err )
                        {
                            if( capture_ ? IsWhite(cr->squares[mv.dst]) : cr->squares[mv.dst]==' ' )
                            {
                                mv.capture = cr->squares[mv.dst];
                                if( piece == 'n' )
                                {
                                    int count=0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = knight_lookup[mv.dst];
                                        lte nbr_moves = *ptr++;
                                        while( !found && nbr_moves-- )
                                        {
                                            Square src_ = (Square)*ptr++;
                                            if( cr->squares[src_]=='n' )
                                            {
                                                bool src_file_ok = !src_file || FILE(src_)==src_file;
                                                bool src_rank_ok = !src_rank || RANK(src_)==src_rank;
                                                if( src_file_ok && src_rank_ok )
                                                {
                                                    mv.src = src_;
                                                    if( probe == 0 )
                                                        count++;
                                                    else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                        // we are not exposing black king to check.
                                                    {
                                                        char temp = cr->squares[mv.dst];
                                                        cr->squares[mv.dst] = 'n';  // temporarily make move
                                                        cr->squares[mv.src] = ' ';
                                                        found = !cr->AttackedSquare( cr->bking_square, true ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                        cr->squares[mv.dst] = temp;  // now undo move
                                                        cr->squares[mv.src] = 'n';
                                                    }
                                                }
                                            }
                                        }
                                        if( probe==0 && count==1 )
                                            found = true; // done, no need for disambiguation by check
                                    }
                                }
                                else // if( rook, bishop, queen )
                                {
                                    int count = 0;
                                    for( int probe=0; !found && probe<2; probe++ )
                                    {
                                        const lte *ptr = ray_lookup[mv.dst];
                                        lte nbr_rays = *ptr++;
                                        while( !found && nbr_rays-- )
                                        {
                                            lte ray_len = *ptr++;
                                            while( !found && ray_len-- )
                                            {
                                                Square src_ = (Square)*ptr++;
                                                if( !IsEmptySquare(cr->squares[src_]) )
                                                {
                                                    // Any piece, friend or enemy must move to end of ray
                                                    ptr += ray_len;
                                                    ray_len = 0;
                                                    if( cr->squares[src_] == piece )
                                                    {
                                                        bool src_file_ok = !src_file || FILE(src_)==src_file;
                                                        bool src_rank_ok = !src_rank || RANK(src_)==src_rank;
                                                        if( src_file_ok && src_rank_ok )
                                                        {
                                                            mv.src = src_;
                                                            if( probe == 0 )
                                                                count++;
                                                            else // probe==1 means disambiguate by testing whether move is legal, found will be set if
                                                                // we are not exposing black king to check.
                                                            {
                                                                char temp = cr->squares[mv.dst];
                                                                cr->squares[mv.dst] = piece;  // temporarily make move
                                                                cr->squares[mv.src] = ' ';
                                                                found = !cr->AttackedSquare( cr->bking_square, true ); //bool AttackedSquare( Square square, bool enemy_is_white );
                                                                cr->squares[mv.dst] = temp;  // now undo move
                                                                cr->squares[mv.src] = piece;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        if( probe==0 && count==1 )
                                            found = true; // done, no need for disambiguation by check
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if( found )
    {
        char c = *natural_in;
        bool problem = (isascii(c) && isalnum(c));
        if( problem )
            found = false;
        else
            *this = mv;
    }
    return found;
}

/****************************************************************************
 * Read terse string move eg "g1f3"
 *  return bool okay
 ****************************************************************************/
bool Move::TerseIn( ChessRules *cr, const char *tmove )
{
    MOVELIST list;
    int i;
    bool okay=false;
    if( strlen(tmove)>=4 && 'a'<=tmove[0] && tmove[0]<='h'
                         && '1'<=tmove[1] && tmove[1]<='8'
                         && 'a'<=tmove[2] && tmove[2]<='h'
                         && '1'<=tmove[3] && tmove[3]<='8' )
    {
        Square src_   = SQ(tmove[0],tmove[1]);
        Square dst_   = SQ(tmove[2],tmove[3]);
        char   expected_promotion_if_any = 'Q';
        if( tmove[4] )
        {
            if( tmove[4]=='n' || tmove[4]=='N' )
                expected_promotion_if_any = 'N';
            else if( tmove[4]=='b' || tmove[4]=='B' )
                expected_promotion_if_any = 'B';
            else if( tmove[4]=='r' || tmove[4]=='R' )
                expected_promotion_if_any = 'R';
        }

        // Generate legal moves, then search for this move
        cr->GenLegalMoveList( &list );
        for( i=0; !okay && i<list.count; i++ )
        {
            if( list.moves[i].dst==dst_ && list.moves[i].src==src_ )
            {
                switch( list.moves[i].special )
                {
                    default:    okay=true;  break;
                    case SPECIAL_PROMOTION_QUEEN:
                    {
                        if( expected_promotion_if_any == 'Q' )
                            okay = true;
                        break;
                    }
                    case SPECIAL_PROMOTION_ROOK:
                    {
                        if( expected_promotion_if_any == 'R' )
                            okay = true;
                        break;
                    }
                    case SPECIAL_PROMOTION_BISHOP:
                    {
                        if( expected_promotion_if_any == 'B' )
                            okay = true;
                        break;
                    }
                    case SPECIAL_PROMOTION_KNIGHT:
                    {
                        if( expected_promotion_if_any == 'N' )
                            okay = true;
                        break;
                    }
                }
            }
            if( okay )
                *this = list.moves[i];
        }
    }
    return okay;
}

/****************************************************************************
 * Convert to natural string
 *    eg "Nf3"
 ****************************************************************************/
std::string Move::NaturalOut( ChessRules *cr )
{

// Improved algorithm

    /* basic procedure is run the following algorithms in turn:
        pawn move     ?
        castling      ?
        Nd2 or Nxd2   ? (loop through all legal moves check if unique)
        Nbd2 or Nbxd2 ? (loop through all legal moves check if unique)
        N1d2 or N1xd2 ? (loop through all legal moves check if unique)
        Nb1d2 or Nb1xd2 (fallback if nothing else works)
    */

    char nmove[10];
    nmove[0] = '-';
    nmove[1] = '-';
    nmove[2] = '\0';
    MOVELIST list;
    bool check[MAXMOVES];
    bool mate[MAXMOVES];
    bool stalemate[MAXMOVES];
    enum
    {
        ALG_PAWN_MOVE,
        ALG_CASTLING,
        ALG_ND2,
        ALG_NBD2,
        ALG_N1D2,
        ALG_NB1D2
    };
    bool done=false;
    bool found = false;
    char append='\0';
    cr->GenLegalMoveList( &list, check, mate, stalemate );
    Move mfound = list.moves[0];   // just to prevent a bogus compiler uninitialized var warning
    for( int i=0; !found && i<list.count; i++ )
    {
        mfound = list.moves[i];
        if( mfound == *this )
        {
            found = true;
            if( mate[i] )
                append = '#';
            else if( check[i] )
                append = '+';
        }
    }

    // Loop through algorithms
    for( int alg=ALG_PAWN_MOVE; found && !done && alg<=ALG_NB1D2; alg++ )
    {
        bool do_loop = (alg==ALG_ND2 || alg==ALG_NBD2 || alg==ALG_N1D2);
        int matches=0;
        Move m;

        // Run the algorithm on the input move (i=-1) AND on all legal moves
        //  in a loop if do_loop set for this algorithm (i=0 to i=count-1)
        for( int i=-1; !done && i<(do_loop?list.count:0); i++ )
        {
            char *str_dst;
            char compare[10];
            if( i == -1 )
            {
                m = *this;
                str_dst = nmove;
            }
            else
            {
                m = list.moves[i];
                str_dst = compare;
            }
            Square src_ = m.src;
            Square dst_ = m.dst;
            char t, p = cr->squares[src_];
            if( islower(p) )
                p = (char)toupper(p);
            if( !IsEmptySquare(m.capture) ) // until we did it this way, enpassant was '-' instead of 'x'
                t = 'x';
            else
                t = '-';
            switch( alg )
            {
                // pawn move ? "e4" or "exf6", plus "=Q" etc if promotion
                case ALG_PAWN_MOVE:
                {
                    if( p == 'P' )
                    {
                        done = true;
                        if( t == 'x' )
                            sprintf( nmove, "%cx%c%c", FILE(src_),FILE(dst_),RANK(dst_) );
                        else
                            sprintf( nmove, "%c%c",FILE(dst_),RANK(dst_) );
                        char *s = strchr(nmove,'\0');
                        switch( m.special )
                        {
                            case SPECIAL_PROMOTION_QUEEN:
                                strcpy( s, "=Q" );  break;
                            case SPECIAL_PROMOTION_ROOK:
                                strcpy( s, "=R" );  break;
                            case SPECIAL_PROMOTION_BISHOP:
                                strcpy( s, "=B" );  break;
                            case SPECIAL_PROMOTION_KNIGHT:
                                strcpy( s, "=N" );  break;
                            default:
                                break;
                        }
                    }
                    break;
                }

                // castling ?
                case ALG_CASTLING:
                {
                    if( m.special==SPECIAL_WK_CASTLING || m.special==SPECIAL_BK_CASTLING )
                    {
                        strcpy( nmove, "O-O" );
                        done = true;
                    }
                    else if( m.special==SPECIAL_WQ_CASTLING || m.special==SPECIAL_BQ_CASTLING )
                    {
                        strcpy( nmove, "O-O-O" );
                        done = true;
                    }
                    break;
                }

                // Nd2 or Nxd2
                case ALG_ND2:
                {
                    if( t == 'x' )
                        sprintf( str_dst, "%cx%c%c", p, FILE(dst_), RANK(dst_) );
                    else
                        sprintf( str_dst, "%c%c%c", p, FILE(dst_), RANK(dst_) );
                    if( i >= 0 )
                    {
                        if( 0 == strcmp(nmove,compare) )
                            matches++;
                    }
                    break;
                }

                // Nbd2 or Nbxd2
                case ALG_NBD2:
                {
                    if( t == 'x' )
                        sprintf( str_dst, "%c%cx%c%c", p, FILE(src_), FILE(dst_), RANK(dst_) );
                    else
                        sprintf( str_dst, "%c%c%c%c", p, FILE(src_), FILE(dst_), RANK(dst_) );
                    if( i >= 0 )
                    {
                        if( 0 == strcmp(nmove,compare) )
                            matches++;
                    }
                    break;
                }

                // N1d2 or N1xd2
                case ALG_N1D2:
                {
                    if( t == 'x' )
                        sprintf( str_dst, "%c%cx%c%c", p, RANK(src_), FILE(dst_), RANK(dst_) );
                    else
                        sprintf( str_dst, "%c%c%c%c", p, RANK(src_), FILE(dst_), RANK(dst_) );
                    if( i >= 0 )
                    {
                        if( 0 == strcmp(nmove,compare) )
                            matches++;
                    }
                    break;
                }

                //  Nb1d2 or Nb1xd2
                case ALG_NB1D2:
                {
                    done = true;
                    if( t == 'x' )
                        sprintf( nmove, "%c%c%cx%c%c", p, FILE(src_), RANK(src_), FILE(dst_), RANK(dst_) );
                    else
                        sprintf( nmove, "%c%c%c%c%c", p, FILE(src_), RANK(src_), FILE(dst_), RANK(dst_) );
                    break;
                }
            }
        }   // end loop for all legal moves with given algorithm

        // If it's a looping algorithm and only one move matches nmove, we're done
        if( do_loop && matches==1 )
            done = true;
    }   // end loop for all algorithms
    if( append )
    {
        char *s = strchr(nmove,'\0');
        *s++ = append;
        *s = '\0';
    }
    return nmove;
}

/****************************************************************************
 * Convert to terse string eg "e7e8q"
 ****************************************************************************/
std::string Move::TerseOut()
{
    char tmove[6];
    if( src == dst )   // null move should be "0000" according to UCI spec
    {
        tmove[0] = '0';
        tmove[1] = '0';
        tmove[2] = '0';
        tmove[3] = '0';
        tmove[4] = '\0';
    }
    else
    {
        tmove[0] = FILE(src);
        tmove[1] = RANK(src);
        tmove[2] = FILE(dst);
        tmove[3] = RANK(dst);
        if( special == SPECIAL_PROMOTION_QUEEN )
            tmove[4] = 'q';
        else if( special == SPECIAL_PROMOTION_ROOK )
            tmove[4] = 'r';
        else if( special == SPECIAL_PROMOTION_BISHOP )
            tmove[4] = 'b';
        else if( special == SPECIAL_PROMOTION_KNIGHT )
            tmove[4] = 'n';
        else
            tmove[4] = '\0';
        tmove[5] = '\0';
    }
    return tmove;
}

/****************************************************************************
 * PrivateChessDefs.cpp Complement PrivateChessDefs.h by providing a shared instantation of
 *  the automatically generated lookup tables.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
namespace thc
{

// All the lookup tables
#define P 1
#define B 2
#define N 4
#define R 8
#define Q 16
#define K 32

// GeneratedLookupTables.h assumes a suitable type lte = lookup tables element
//  plus a bitmask convention for pieces using identifiers P,R,N,B,Q,K is
//  defined
// #include "GeneratedLookupTables.h"
/****************************************************************************
 * GeneratedLookupTables.h These lookup tables are machine generated
 *  They require prior definitions of;
 *   squares (a1,a2..h8)
 *   pieces (P,N,B,N,R,Q,K)
 *   lte (=lookup table element, a type for the lookup tables, eg int or unsigned char)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
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


