/****************************************************************************
 *  Compress chess moves into one byte
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include "CompressMoves.h"

#define DIAG_ONLY(x) x

static unsigned long nbr_compress_fast;
static unsigned long nbr_compress_slow;
static unsigned long nbr_uncompress_fast;
static unsigned long nbr_uncompress_slow;
static unsigned long nbr_pawn_swaps_histo[8];
static unsigned long nbr_pawn_swaps_histo_wl[8];
static unsigned long nbr_pawn_swaps_histo_wr[8];
static unsigned long nbr_pawn_swaps_histo_bl[8];
static unsigned long nbr_pawn_swaps_histo_br[8];
static unsigned long nbr_knight_moves;
static unsigned long nbr_knight_swaps;
static unsigned long nbr_knight_swaps_alt;
static unsigned long nbr_rook_moves;
static unsigned long nbr_rook_swaps;
static unsigned long nbr_rook_swaps_alt;
static unsigned long nbr_queen_swaps;
static unsigned long max_nbr_slow_moves_other;
static unsigned long max_nbr_slow_moves_queen;
static unsigned long nbr_games_with_promotions;
static unsigned long nbr_games_with_slow_mode;
static unsigned long nbr_games_with_two_queens;

void CompressMovesDiagBegin()
{
    nbr_compress_fast = 0;
    nbr_compress_slow = 0;
    nbr_uncompress_fast = 0;
    nbr_uncompress_slow = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_wl[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_wr[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_bl[i] = 0;
    for(int i=0; i<8; i++)
        nbr_pawn_swaps_histo_br[i] = 0;
    nbr_knight_moves = 0;
    nbr_knight_swaps = 0;
    nbr_knight_swaps_alt = 0;
    nbr_rook_moves = 0;
    nbr_rook_swaps = 0;
    nbr_rook_swaps_alt = 0;
    nbr_queen_swaps = 0;
    max_nbr_slow_moves_other = 0;
    max_nbr_slow_moves_queen = 0;
    nbr_games_with_promotions = 0;
    nbr_games_with_slow_mode = 0;
    nbr_games_with_two_queens = 0;
}

void CompressMovesDiagEnd()
{
    printf( "nbr_compress_fast   = %lu\n", nbr_compress_fast );
    printf( "nbr_compress_slow   = %lu\n", nbr_compress_slow );
    printf( "nbr_uncompress_fast = %lu\n", nbr_uncompress_fast );
    printf( "nbr_uncompress_slow = %lu\n", nbr_uncompress_slow );  // It turns out about 0.015% of moves are in slow mode
    for(int i=0; i<8; i++)
        printf( "nbr_pawn_swaps_histo[%d] = %lu\n", i, nbr_pawn_swaps_histo[i] );
    for(int i=0; i<8; i++)
        printf( "nbr_pawn_swaps_histo_wl[%d] = %lu\n", i, nbr_pawn_swaps_histo_wl[i] );
    for(int i=0; i<8; i++)
        printf( "nbr_pawn_swaps_histo_wr[%d] = %lu\n", i, nbr_pawn_swaps_histo_wr[i] );
    for(int i=0; i<8; i++)
        printf( "nbr_pawn_swaps_histo_bl[%d] = %lu\n", i, nbr_pawn_swaps_histo_bl[i] );
    for(int i=0; i<8; i++)
        printf( "nbr_pawn_swaps_histo_br[%d] = %lu\n", i, nbr_pawn_swaps_histo_br[i] );
    printf( "nbr_knight_moves     = %lu\n",       nbr_knight_moves );
    printf( "nbr_knight_swaps     = %lu\n",       nbr_knight_swaps );
    printf( "nbr_knight_swaps_alt = %lu\n",       nbr_knight_swaps_alt );
    printf( "nbr_rook_moves       = %lu\n",         nbr_rook_moves );
    printf( "nbr_rook_swaps       = %lu\n",         nbr_rook_swaps );
    printf( "nbr_rook_swaps_alt   = %lu\n",         nbr_rook_swaps_alt );
    printf( "nbr_queen_swaps      = %lu\n",         nbr_queen_swaps );
    printf( "nbr_games_with_promotions = %lu\n",     nbr_games_with_promotions );
    printf( "nbr_games_with_slow_mode  = %lu\n",     nbr_games_with_slow_mode  );
    printf( "nbr games_with_two_queens = %lu\n",     nbr_games_with_two_queens );
}


// The ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES algorithm
//  anticipates future optimised implementations - Such implementations
//  might implement making moves using thc::Move independently
//  and without reference to the thc classes until they are forced
//  to use slow_mode. Upon transition to slow_mode, such implementations
//  rely on move generation algorithms, which in turn need to
//  know whether the kings and rooks have ever moved. Relieve the
//  future implementation's responsibility of tracking this information
//  by always acting as if the kings and rooks haven't moved (if they
//  are on their home squares).  This doesn't cause possible illegality
//  since Compress() only compresses legal moves, so reversing this
//  with Uncompress() only produces legal moves.

#define CODE_KING           0x00
#define CODE_KNIGHT         0x10
#define CODE_ROOK_LO        0x20
#define CODE_ROOK_HI        0x30
#define CODE_BISHOP_DARK    0x40
#define CODE_BISHOP_LIGHT   0x50
#define CODE_QUEEN_ROOK     0x60
#define CODE_QUEEN_BISHOP   0x70
#define CODE_PAWN           0x80
#define CODE_PAWN_0         0x80
#define CODE_PAWN_1         0x90
#define CODE_PAWN_2         0xa0
#define CODE_PAWN_3         0xb0
#define CODE_PAWN_4         0xc0
#define CODE_PAWN_5         0xd0
#define CODE_PAWN_6         0xe0
#define CODE_PAWN_7         0xf0

#define K_VECTOR_N          0x01
#define K_VECTOR_NE         0x02
#define K_VECTOR_E          0x03
#define K_VECTOR_SE         0x04
#define K_VECTOR_S          0x05
#define K_VECTOR_SW         0x06
#define K_VECTOR_W          0x07
#define K_VECTOR_NW         0x08
#define K_K_CASTLING        0x09
#define K_Q_CASTLING        0x0b

#define N_VECTOR_NNE        0x00
#define N_VECTOR_NEE        0x01
#define N_VECTOR_SEE        0x02
#define N_VECTOR_SSE        0x03
#define N_VECTOR_SSW        0x04
#define N_VECTOR_SWW        0x05
#define N_VECTOR_NWW        0x06
#define N_VECTOR_NNW        0x07

#define P_DOUBLE            0     // 0000   // 4 non promoting moves
#define P_SINGLE            1     // 0001
#define P_LEFT              2     // 0010
#define P_RIGHT             3     // 0011
#define P_QUEEN             0     // dd00   // dd is direction of promoting move one of P_SINGLE, P_LEFT, P_RIGHT
#define P_ROOK              1     // dd01
#define P_BISHOP            2     // dd10
#define P_KNIGHT            3     // dd11

#define R_RANK              8     // Rank or file (i.e. rook) codes, this bit set indicates same file (so remaining 3 bits encode rank)
#define R_FILE              0

#define B_FALL              8     // Diagonal (i.e. bishop) codes, either RISE/ or FALL\, other 3 bits encode destination file
#define B_RISE              0

#define N_HI                8     // CODE_KNIGHT encodes 8 vectors for each of 2 knights (HI and LO)
#define N_LO                0

// Pawns for each side are assigned logical numbers from 0 to nbr_pawns-1
//  The ordering of the numbers is determined by consulting this table...
static int pawn_ordering[64] =
{
    7, 15, 23, 31, 39, 47, 55, 63,
    6, 14, 22, 30, 38, 46, 54, 62,
    5, 13, 21, 29, 37, 45, 53, 61,
    4, 12, 20, 28, 36, 44, 52, 60,
    3, 11, 19, 27, 35, 43, 51, 59,
    2, 10, 18, 26, 34, 42, 50, 58,
    1,  9, 17, 25, 33, 41, 49, 57,
    0,  8, 16, 24, 32, 40, 48, 56
};

// ...to initially assign logical pawn numbers according to pawn_ordering[]
//  above, traverse the squares in this order and assign 0,1,2... etc as each
//  pawn is found
static thc::Square traverse_order[64] =
{
    thc::a1, thc::a2, thc::a3, thc::a4, thc::a5, thc::a6, thc::a7, thc::a8,
    thc::b1, thc::b2, thc::b3, thc::b4, thc::b5, thc::b6, thc::b7, thc::b8,
    thc::c1, thc::c2, thc::c3, thc::c4, thc::c5, thc::c6, thc::c7, thc::c8,
    thc::d1, thc::d2, thc::d3, thc::d4, thc::d5, thc::d6, thc::d7, thc::d8,
    thc::e1, thc::e2, thc::e3, thc::e4, thc::e5, thc::e6, thc::e7, thc::e8,
    thc::f1, thc::f2, thc::f3, thc::f4, thc::f5, thc::f6, thc::f7, thc::f8,
    thc::g1, thc::g2, thc::g3, thc::g4, thc::g5, thc::g6, thc::g7, thc::g8,
    thc::h1, thc::h2, thc::h3, thc::h4, thc::h5, thc::h6, thc::h7, thc::h8
};

// Try to set fast mode, return bool okay
bool CompressMoves::TryFastMode( Side *side )
{
    bool okay = true;
    side->nbr_pawns   = 0;
    side->nbr_knights = 0;
    side->nbr_rooks   = 0;
    side->nbr_dark_bishops  = 0;
    side->nbr_light_bishops = 0;
    side->nbr_queens        = 0;
    for( int i=0; okay && i<64; i++ )
    {

        // Pawns are traversed according to pawn_ordering[]
        int j = static_cast<int>(traverse_order[i]);
        if( cr.squares[j] == (side->white?'P':'p') )
        {
            if( side->nbr_pawns < 8 )
                side->pawns[side->nbr_pawns++] = j;
            else
                okay = false;
        }

        // Other pieces are traversed in normal square convention order
        if( cr.squares[i] == (side->white?'R':'r') )
        {
            if( side->nbr_rooks < 2 )
                side->rooks[side->nbr_rooks++] = i;
            else
            {
                DIAG_ONLY( is_interesting |= 8 );
                okay = false;
            }
        }
        else if( cr.squares[i] == (side->white?'N':'n') )
        {
            if( side->nbr_knights < 2 )
                side->knights[side->nbr_knights++] = i;
            else
            {
                DIAG_ONLY( is_interesting |= 4 );
                okay = false;
            }
        }
        else if( cr.squares[i] == (side->white?'B':'b') )
        {
            if( is_dark(i) )
            {
                side->bishop_dark = i;
                if( side->nbr_dark_bishops < 1 )
                    side->nbr_dark_bishops++;
                else
                {
                    DIAG_ONLY( is_interesting  |= 1 );
                    okay = false;
                }
            }
            else
            {
                side->bishop_light = i;
                if( side->nbr_light_bishops < 1 )
                    side->nbr_light_bishops++;
                else
                {
                    DIAG_ONLY( is_interesting |= 2 );
                    okay = false;
                }
            }
        }
        else if( cr.squares[i] == (side->white?'Q':'q') )
        {
            if( side->nbr_queens < 2 )
                side->queens[side->nbr_queens++] = i;
            else
            {
                DIAG_ONLY( is_interesting |= 16 );  // 3 or more queens!, or more likely see below ....
                okay = false;
            }
        }
        else if( cr.squares[i] == (side->white?'K':'k') )
        {
            side->king = i;
        }
    }
    if( side->nbr_queens==2 && side->nbr_pawns>6 )
    {
        DIAG_ONLY( is_interesting |= 16 );  // .... 2 queens and 7 pawns
        okay = false;
    }
    side->fast_mode = okay;
    return okay;
}

std::string CompressMoves::Compress( thc::ChessPosition &cp, std::vector<thc::Move> &moves_in )
{
    Init( cp );
    return CompressMoves::Compress( moves_in );
}


std::string CompressMoves::Compress( std::vector<thc::Move> &moves_in )
{
    std::string ret;
    sides[0].fast_mode=false;
    sides[1].fast_mode=false;
    int len = moves_in.size();
    for( int i=0; i<len; i++ )
    {
        Side *side  = cr.white ? &sides[0] : &sides[1];
        Side *other = cr.white ? &sides[1] : &sides[0];
        char c;
        thc::Move mv = moves_in[i];
        if( side->fast_mode )
        {
            c = CompressFastMode(mv,side,other);
        }
        else if( TryFastMode(side) )
        {
            c = CompressFastMode(mv,side,other);
        }
        else
        {
            c = CompressSlowMode(mv);
            other->fast_mode = false;   // force other side to reset and retry
        }
        cr.PlayMove(mv);
        ret.push_back(c);
    }
    return ret;
}

std::vector<thc::Move> CompressMoves::Uncompress( thc::ChessPosition &cp, std::string &moves_in )
{
    Init( cp );
    return Uncompress( moves_in );
}

std::vector<thc::Move> CompressMoves::Uncompress( std::string &moves_in )
{
    std::vector<thc::Move> ret;
    sides[0].fast_mode=false;
    sides[1].fast_mode=false;
    int len = moves_in.size();
    for( int i=0; i<len; i++ )
    {
        Side *side  = cr.white ? &sides[0] : &sides[1];
        Side *other = cr.white ? &sides[1] : &sides[0];
        char code = moves_in[i];
        thc::Move mv;
        if( side->fast_mode )
        {
            mv = UncompressFastMode(code,side,other);
        }
        else if( TryFastMode(side) )
        {
            mv = UncompressFastMode(code,side,other);
        }
        else
        {
            mv = UncompressSlowMode(code);
            other->fast_mode = false;   // force other side to reset and retry
        }
        cr.PlayMove(mv);
        ret.push_back(mv);
    }
    return ret;
}

char CompressMoves::CompressMove( thc::Move mv )
{
    Side *side  = cr.white ? &sides[0] : &sides[1];
    Side *other = cr.white ? &sides[1] : &sides[0];
    char c;
    if( side->fast_mode )
    {
        c = CompressFastMode(mv,side,other);
    }
    else if( TryFastMode(side) )
    {
        c = CompressFastMode(mv,side,other);
    }
    else
    {
        c = CompressSlowMode(mv);
        other->fast_mode = false;   // force other side to reset and retry
    }
    cr.PlayMove(mv);
    return c;
}

thc::Move CompressMoves::UncompressMove( char c )
{
    Side *side  = cr.white ? &sides[0] : &sides[1];
    Side *other = cr.white ? &sides[1] : &sides[0];
    thc::Move mv;
    if( side->fast_mode )
    {
        mv = UncompressFastMode(c,side,other);
    }
    else if( TryFastMode(side) )
    {
        mv = UncompressFastMode(c,side,other);
    }
    else
    {
        mv = UncompressSlowMode(c);
        other->fast_mode = false;   // force other side to reset and retry
    }
    cr.PlayMove(mv);
    return mv;
}

// A slow method of compressing a move into one byte
//  Scheme is;
//  1) Make a list of all legal moves in UCI text format, sorted alphabetically
//  2) Find the move to be compressed in the list
//  3) Encode the move as one byte, value is 255 - list index (to avoid code 0, '\n' etc)
//
//  Why use slow text representation and sorting ? So that a specification for the
//  whole compression scheme can be published if necessary, without relying on implementation
//  details like the order that moves come out of the move generator etc.
//
//  Note that we use the ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES algorithm when
//  generating the list of moves
//
//  According to http://www.chess.com/forum/view/fun-with-chess/what-chess-position-has-the-most-number-of-possible-moves
//  the maximum number of legal moves available in any position is less than 256,
//  in fact it is 218 in the following position;
//
//  "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1"
//


char CompressMoves::CompressSlowMode( thc::Move mv )
{
    DIAG_ONLY( nbr_compress_slow++ );
    int code=0;

    // Support algorithm ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES
    cr.wking = 1;
    cr.wqueen = 1;
    cr.bking = 1;
    cr.bqueen = 1;

    // Generate a list of all legal moves, in string form, sorted
    std::vector<thc::Move> moves;
    cr.GenLegalMoveList( moves );
    std::vector<std::string> moves_alpha;
    size_t len = moves.size();
    for( size_t i=0; i<len; i++ )
    {
        #define TERSE
        #ifdef TERSE
        std::string s = moves[i].TerseOut();
        #else
        std::string s = moves[i].NaturalOut(&cr);
        #endif
        moves_alpha.push_back(s);
    }
    std::sort( moves_alpha.begin(), moves_alpha.end() );

    // Find this move in the list
    #ifdef TERSE
    std::string the_move = mv.TerseOut();
    #else
    std::string the_move = mv.NaturalOut(&cr);
    #endif
    size_t idx=256;  // so that error if not found
    for( size_t i=0; i<len; i++ )
    {
        if( moves_alpha[i] == the_move )
        {
            idx = i;
            break;
        }
    }

    // char generated is '\xff' for first move in list, '\fe' for second etc
    // '\x01' is an error
    code = 255-idx;
    if( code < 1 )
        code = 1;
    DIAG_ONLY( nbr_slow_moves++; )
    return static_cast<char>(code);
}


char CompressMoves::CompressFastMode( thc::Move mv, Side *side, Side *other )
{
    DIAG_ONLY( nbr_compress_fast++ );
    int src = static_cast<int>(mv.src);
    int dst = static_cast<int>(mv.dst);
    int capture_location = dst;
    char piece = cr.squares[mv.src];
    bool making_capture = (isalpha(cr.squares[capture_location]) ? true : false);
    int  code=0;
    switch( tolower(piece) )
    {
        case 'k':
        {
            side->king = dst;
            switch(dst-src)          // 0, 1, 2
            {                        // 8, 9, 10
                                     // 16,17,18
                case -9:    code = CODE_KING + K_VECTOR_NW;    break;  // 0-9
                case -8:    code = CODE_KING + K_VECTOR_N;     break;  // 1-9
                case -7:    code = CODE_KING + K_VECTOR_NE;    break;  // 2-9
                case -1:    code = CODE_KING + K_VECTOR_W;     break;  // 8-9
                case 1:     code = CODE_KING + K_VECTOR_E;     break;  // 10-9
                case 7:     code = CODE_KING + K_VECTOR_SW;    break;  // 16-9
                case 8:     code = CODE_KING + K_VECTOR_S;     break;  // 17-9
                case 9:     code = CODE_KING + K_VECTOR_SE;    break;  // 18-9
                case 2:
                {
                    code = CODE_KING + K_K_CASTLING;
                    int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                    side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                    // note that there is no way the rooks ordering can swap during castling
                    DIAG_ONLY( if( side->nbr_rooks==2 && pawn_ordering[side->rooks[0]]>pawn_ordering[side->rooks[1]] ) nbr_rook_swaps_alt++; ) // but alt swap possible
                    DIAG_ONLY( nbr_rook_moves++; )
                    break;
                }
                case -2:
                {
                    code = CODE_KING + K_Q_CASTLING;
                    int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                    side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                    // note that there is no way the rooks ordering can swap during castling
                    DIAG_ONLY( if( side->nbr_rooks==2 && pawn_ordering[side->rooks[0]]>pawn_ordering[side->rooks[1]] ) nbr_rook_swaps_alt++; ) // but alt swap possible
                    DIAG_ONLY( nbr_rook_moves++; )
                    break;
                }
            }
            break;
        }

        case 'r':
        {
            DIAG_ONLY( nbr_rook_moves++; )
            int rook_offset = (side->rooks[0]==src ? 0 : 1);
            code = (rook_offset==0 ? CODE_ROOK_LO : CODE_ROOK_HI);
            side->rooks[rook_offset] = dst;
            if( (src&7) == (dst&7) )                // if same file
            {
                code = code + R_RANK + ((dst>>3)&7);    // encode rank
            }
            else //if( (src&0x38) == (dst&0x38) )   // if same rank
            {
                code = code + R_FILE + (dst&7);         // encode file
            }

            // swap ?
            if( side->nbr_rooks==2 && side->rooks[0]>side->rooks[1] )
            {
                int temp = side->rooks[0];
                side->rooks[0] = side->rooks[1];
                side->rooks[1] = temp;
                DIAG_ONLY( nbr_rook_swaps++; )
            }
            DIAG_ONLY( if( side->nbr_rooks==2 && pawn_ordering[side->rooks[0]]>pawn_ordering[side->rooks[1]] ) nbr_rook_swaps_alt++; )
            break;
        }

        case 'b':
        {
            if( is_dark(src) )
            {
                code = CODE_BISHOP_DARK;
                side->bishop_dark = dst;
            }
            else
            {
                code = CODE_BISHOP_LIGHT;
                side->bishop_light = dst;
            }
            int abs = (src>dst ? src-dst : dst-src);
            if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
                code = code + B_FALL + (dst&7); // fall( = \) + dst file
            else
                code = code + B_RISE + (dst&7); // rise( = /) + dst file
            break;
        }

        case 'q':
        {
            int queen_offset = (side->queens[0]==src ? 0 : 1);
            int base = (queen_offset==0 ? CODE_QUEEN_ROOK : CODE_PAWN_6 );
            side->queens[queen_offset] = dst;
            if( (src&7) == (dst&7) )                        // if same file
            {
                code = base + R_RANK + ((dst>>3)&7); // encode rank
            }
            else if( (src&0x38) == (dst&0x38) )             // if same rank
            {
                code = base + R_FILE + (dst&7);      // encode file
            }
            else
            {
                int abs = (src>dst ? src-dst : dst-src);
                if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
                    code = base + (CODE_QUEEN_BISHOP-CODE_QUEEN_ROOK) + B_FALL + (dst&7); // fall( = \) + dst file
                else
                    code = base + (CODE_QUEEN_BISHOP-CODE_QUEEN_ROOK) + B_RISE + (dst&7); // rise( = /) + dst file
            }

            // swap ?
            if( side->nbr_queens==2 && side->queens[0]>side->queens[1] )
            {
                int temp = side->queens[0];
                side->queens[0] = side->queens[1];
                side->queens[1] = temp;
                DIAG_ONLY( nbr_queen_swaps++; )
            }
            break;
        }

        case 'n':
        {
            DIAG_ONLY( nbr_knight_moves++; )
            int knight_offset = (side->knights[0]==src ? 0 : 1);
            code = (knight_offset==0 ? CODE_KNIGHT+N_LO : CODE_KNIGHT+N_HI );
            side->knights[knight_offset] = dst;
            switch(dst-src)         // 0, 1, 2, 3
            {                       // 8, 9, 10,11
                                    // 16,17,18,19
                                    // 24,25,26,27
                case -15:   code = code + N_VECTOR_NNE;   break; // 2-17
                case -6:    code = code + N_VECTOR_NEE;   break; // 11-17
                case 10:    code = code + N_VECTOR_SEE;   break; // 27-17
                case 17:    code = code + N_VECTOR_SSE;   break; // 27-10
                case 15:    code = code + N_VECTOR_SSW;   break; // 25-10
                case 6:     code = code + N_VECTOR_SWW;   break; // 16-10
                case -10:   code = code + N_VECTOR_NWW;   break; // 0-10
                case -17:   code = code + N_VECTOR_NNW;   break; // 0-17
            }
            // swap ?
            if( side->nbr_knights==2 && side->knights[0]>side->knights[1] )
            {
                int temp = side->knights[0];
                side->knights[0] = side->knights[1];
                side->knights[1] = temp;
                DIAG_ONLY( nbr_knight_swaps++; )
            }
            DIAG_ONLY( if( side->nbr_knights==2 && pawn_ordering[side->knights[0]]>pawn_ordering[side->knights[1]] ) nbr_knight_swaps_alt++; )
            break;
        }

        case 'p':
        {
            int positive_delta = cr.white ? src-dst : dst-src;
            bool promoting = false;
            int pawn_offset = 0;
            int *p = side->pawns;
            for( int i=0; i<side->nbr_pawns; i++,p++ )
            {
                if( *p == src )
                {
                    pawn_offset = i;
                    *p = dst;
                    break;
                }
            }
            switch( positive_delta )
            {
                case 16: code = P_DOUBLE;  break;
                case 8:  code = P_SINGLE;  break;
                case 9:  code = P_LEFT;    break;
                case 7:  code = P_RIGHT;   break;
            }
            switch( mv.special )
            {
                case thc::SPECIAL_WEN_PASSANT:
                    capture_location = dst+8;
                    making_capture = true;
                    break;
                case thc::SPECIAL_BEN_PASSANT:
                    capture_location = dst-8;
                    making_capture = true;
                    break;
                case thc::SPECIAL_PROMOTION_QUEEN:  code = (code<<2) + P_QUEEN;
                    promoting = true;
                    DIAG_ONLY( if(side->nbr_queens>0) is_interesting|=4096; )
                    break;
                case thc::SPECIAL_PROMOTION_ROOK:   code = (code<<2) + P_ROOK;
                    promoting = true;
                    break;
                case thc::SPECIAL_PROMOTION_BISHOP: code = (code<<2) + P_BISHOP;
                    promoting = true;
                    break;
                case thc::SPECIAL_PROMOTION_KNIGHT: code = (code<<2) + P_KNIGHT;
                    promoting = true;
                    break;
            }

            // If promoting piece, force a reset and retry next time for this side. This
            //  way we accommodate our pawn disappearing, and a new piece appearing in
            //  its place, and the possibility that we cannot remain in fast mode
            //  (because we now have too many queens or other pieces). If the reset
            //  and retry fails this side will generate slow mode moves but keep
            //  retrying until fast is possible again.
            if( promoting )
            {
                side->fast_mode = false;
                DIAG_ONLY( is_interesting |= 512; )
            }
            else
            {
                DIAG_ONLY( bool br_or_wl=false; int nbr_swaps=0; )
                bool reordering_possible = (code==P_LEFT || code==P_RIGHT);
                if( reordering_possible )
                {
                    if( pawn_ordering[dst] > pawn_ordering[src] ) // increasing capture?
                    {
                        for( int i=pawn_offset; i+1<side->nbr_pawns && pawn_ordering[side->pawns[i]]>pawn_ordering[side->pawns[i+1]]; i++ )
                        {
                            DIAG_ONLY( nbr_swaps++; )
                            int temp = side->pawns[i];
                            side->pawns[i] = side->pawns[i+1];
                            side->pawns[i+1] = temp;
                        }
                        if( cr.white )
                            { DIAG_ONLY( nbr_pawn_swaps_histo_wr[nbr_swaps]++; ) }
                        else
                            { DIAG_ONLY( br_or_wl=true; nbr_pawn_swaps_histo_br[nbr_swaps]++; ) }
                    }
                    else // else decreasing capture
                    {
                        for( int i=pawn_offset; i-1>=0 && pawn_ordering[side->pawns[i-1]]>pawn_ordering[side->pawns[i]]; i-- )
                        {
                            DIAG_ONLY( nbr_swaps++; )
                            int temp = side->pawns[i-1];
                            side->pawns[i-1] = side->pawns[i];
                            side->pawns[i] = temp;
                        }
                        if( cr.white )
                            { DIAG_ONLY( br_or_wl=true; nbr_pawn_swaps_histo_wl[nbr_swaps]++; ) }
                        else
                            { DIAG_ONLY( nbr_pawn_swaps_histo_bl[nbr_swaps]++; ) }
                    }
                }
                DIAG_ONLY( if( nbr_swaps>=3 && br_or_wl ) is_interesting|=32; )
                DIAG_ONLY( nbr_pawn_swaps_histo[nbr_swaps]++; )
            }
            code = CODE_PAWN + (pawn_offset<<4) + code;
            break;
        }   // end PAWN
    }   // end switch on moving piece

    // Accomodate captured piece on other side, if other side is in fast mode
    if( making_capture && other->fast_mode )
    {
        switch( tolower(cr.squares[capture_location]) )
        {
            case 'n':
            {
                if( other->nbr_knights==2 && other->knights[0]==capture_location )
                    other->knights[0] = other->knights[1];
                other->nbr_knights--;
                break;
            }
            case 'r':
            {
                if( other->nbr_rooks==2 && other->rooks[0]==capture_location )
                    other->rooks[0] = other->rooks[1];
                other->nbr_rooks--;
                break;
            }
            case 'b':
            {
                if( is_dark(capture_location) )
                    other->nbr_dark_bishops--;
                else
                    other->nbr_light_bishops--;
                break;
            }
            case 'q':
            {
                if( other->nbr_queens==2 && other->queens[0]==capture_location )
                    other->queens[0] = other->queens[1];
                other->nbr_queens--;
                break;
            }
            case 'p':
            {
                int other_pawn_offset=0;
                for( int i=0; i<other->nbr_pawns; i++ )
                {
                    if( other->pawns[i] == capture_location )
                    {
                        other_pawn_offset = i;
                        break;
                    }
                }
                for( int i=other_pawn_offset; i+1<other->nbr_pawns; i++ )
                {
                    other->pawns[i] = other->pawns[i+1];
                }
                other->nbr_pawns--;
                break;
            }
        }
    }
    return static_cast<char>(code);
}

thc::Move CompressMoves::UncompressSlowMode( char code )
{
    DIAG_ONLY( nbr_uncompress_slow++ );

    // Support algorithm ALLOW_CASTLING_EVEN_AFTER_KING_AND_ROOK_MOVES
    cr.wking = 1;
    cr.wqueen = 1;
    cr.bking = 1;
    cr.bqueen = 1;

    // Generate a list of all legal moves, in string form, sorted
    std::vector<thc::Move> moves;
    cr.GenLegalMoveList( moves );
    std::vector<std::string> moves_alpha;

    // Coding scheme relies on 254 valid codes 0x02-0xff and one error code 0x01,
    size_t len = moves.size();
    for( size_t i=0; i<len; i++ )
    {
        #ifdef TERSE
        std::string s = moves[i].TerseOut();
        #else
        std::string s = moves[i].NaturalOut(&cr);
        #endif
        moves_alpha.push_back(s);
    }
    std::sort( moves_alpha.begin(), moves_alpha.end() );

    // '\xff' (i.e. 255) is first move in list, '\fe' (i.e. 254) is second etc
    unsigned int ucode = static_cast<size_t>( code );
    ucode &= 0xff;
    size_t idx = 255-ucode;  // 255->0, 254->1 etc.
    if( idx >= len )
        idx = 0;    // all errors resolve to this - take first move from list
    std::string the_move = moves_alpha[idx];
    thc::Move mv;
    #ifdef TERSE
    mv.TerseIn( &cr, the_move.c_str() );
    #else
    mv.NaturalInFast( &cr, the_move.c_str() );
    #endif
    return mv;
}

thc::Move CompressMoves::UncompressFastMode( char code, Side *side, Side *other )
{
    DIAG_ONLY( nbr_uncompress_fast++ );
    int src=0;
    int dst=0;
    int hi_nibble = code&0xf0;
    thc::SPECIAL special = thc::NOT_SPECIAL;
    switch( hi_nibble )
    {
        case CODE_KING:
        {
            special = thc::SPECIAL_KING_MOVE;
            src = side->king;
            int delta=0;
            switch( code&0x0f )     // 0, 1, 2
            {                       // 8, 9, 10
                                    // 16,17,18
                case K_VECTOR_NW:    delta = -9; break;  // 0-9
                case K_VECTOR_N:     delta = -8; break;  // 1-9
                case K_VECTOR_NE:    delta = -7; break;  // 2-9
                case K_VECTOR_W:     delta = -1; break;  // 8-9
                case K_VECTOR_E:     delta =  1; break;  // 10-9
                case K_VECTOR_SW:    delta =  7; break;  // 16-9
                case K_VECTOR_S:     delta =  8; break;  // 17-9
                case K_VECTOR_SE:    delta =  9; break;  // 18-9
                case K_K_CASTLING:
                {
                    special = cr.white ? thc::SPECIAL_WK_CASTLING : thc::SPECIAL_BK_CASTLING;
                    delta = 2;
                    int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                    side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                    // note that there is no way the rooks ordering can swap during castling
                    break;
                }
                case K_Q_CASTLING:
                {
                    special = cr.white ? thc::SPECIAL_WQ_CASTLING : thc::SPECIAL_BQ_CASTLING;
                    delta = -2;
                    int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                    side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                    // note that there is no way the rooks ordering can swap during castling
                    break;
                }
            }
            side->king = dst = src+delta;
            break;
        }

        case CODE_ROOK_LO:
        case CODE_ROOK_HI:
        {
            int rook_offset = (hi_nibble==CODE_ROOK_LO ? 0 : 1 );
            src = side->rooks[rook_offset];
            if( code & R_RANK )                // code encodes rank ?
            {
                dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
            }
            else
            {
                dst = (src&0x38) | (code&7);        // same rank as src, file from code
            }
            side->rooks[rook_offset] = dst;

            // swap ?
            if( side->nbr_rooks==2 && side->rooks[0]>side->rooks[1] )
            {
                int temp = side->rooks[0];
                side->rooks[0] = side->rooks[1];
                side->rooks[1] = temp;
            }
            break;
        }

        case CODE_BISHOP_DARK:
        {
            src = side->bishop_dark;
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=b8(1), dst=h2(55), file_delta=6  -> 9*6 =54
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
            side->bishop_dark = dst;
            break;
        }

        case CODE_BISHOP_LIGHT:
        {
            src = side->bishop_light;
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=g8(6), dst=a2(48), file_delta=6  -> 7*6 =42
            side->bishop_light = dst;
            break;
        }

        case CODE_QUEEN_ROOK:
        {
            src = side->queens[0];
            if( code & R_RANK )                // code encodes rank ?
                dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
            else
                dst = (src&0x38) | (code&7);        // same rank as src, file from code
            side->queens[0] = dst;

            // swap ?
            if( side->nbr_queens==2 && side->queens[0]>side->queens[1] )
            {
                int temp = side->queens[0];
                side->queens[0] = side->queens[1];
                side->queens[1] = temp;
            }
            break;
        }

        case CODE_QUEEN_BISHOP:
        {
            src = side->queens[0];
            int file_delta = (code&7) - (src&7);
            if( code & B_FALL )  // FALL\ + file
                dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
            else                  // RISE/ + file
                dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
            side->queens[0] = dst;

            // swap ?
            if( side->nbr_queens==2 && side->queens[0]>side->queens[1] )
            {
                int temp = side->queens[0];
                side->queens[0] = side->queens[1];
                side->queens[1] = temp;
            }
            break;
        }

        case CODE_KNIGHT:
        {
            int knight_offset = ((code&N_HI) ? 1 : 0 );
            src = side->knights[knight_offset];
            int delta=0;
            switch(code&7)          // 0, 1, 2, 3
            {                       // 8, 9, 10,11
                                    // 16,17,18,19
                                    // 24,25,26,27
                case N_VECTOR_NNE:   delta = -15;   break; // 2-17
                case N_VECTOR_NEE:   delta = -6;    break; // 11-17
                case N_VECTOR_SEE:   delta = 10;    break; // 27-17
                case N_VECTOR_SSE:   delta = 17;    break; // 27-10
                case N_VECTOR_SSW:   delta = 15;    break; // 25-10
                case N_VECTOR_SWW:   delta = 6;     break; // 16-10
                case N_VECTOR_NWW:   delta = -10;   break; // 0-10
                case N_VECTOR_NNW:   delta = -17;   break; // 0-17
            }
            dst = src+delta;
            side->knights[knight_offset] = dst;

            // swap ?
            if( side->nbr_knights==2 && side->knights[0]>side->knights[1] )
            {
                int temp = side->knights[0];
                side->knights[0] = side->knights[1];
                side->knights[1] = temp;
            }
            break;
        }

        case CODE_PAWN_6:   // if nbr_queens==2 this is the hi QUEEN_ROOK
        {
            if( side->nbr_queens < 2 )
                ;   // fall through
            else
            {
                src = side->queens[1];
                if( code & R_RANK )                // code encodes rank ?
                    dst = ((code<<3)&0x38) | (src&7);   // same file as src, rank from code
                else
                    dst = (src&0x38) | (code&7);        // same rank as src, file from code
                side->queens[1] = dst;

                // swap ?
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }
                break;
            }
        }

        case CODE_PAWN_7:   // if nbr_queens==2 this is the hi QUEEN_BISHOP:
        {
            if( side->nbr_queens < 2 )
                ;   // fall through
            else
            {
                src = side->queens[1];
                int file_delta = (code&7) - (src&7);
                if( code & B_FALL )  // FALL\ + file
                    dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                else                  // RISE/ + file
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                side->queens[1] = dst;

                // swap ?
                if( side->queens[0]>side->queens[1] )
                {
                    int temp = side->queens[0];
                    side->queens[0] = side->queens[1];
                    side->queens[1] = temp;
                }
                break;
            }
        }

        // PAWN
        default:
        {
            int pawn_offset = (code>>4)&0x07;
            bool promoting = false;
            src = side->pawns[pawn_offset];
            bool reordering_possible = false;
            bool white = cr.white;
            int delta=0;
            switch( code&0x0f )
            {
                case P_DOUBLE:      special = white ? thc::SPECIAL_WPAWN_2SQUARES : thc::SPECIAL_BPAWN_2SQUARES;
                                    delta = white?-16:16; break;
                case P_SINGLE:      delta = white?-8:8;   break;
                case P_LEFT:
                {
                    reordering_possible = true;
                    delta = white?-9:9;
                    if( !isalpha(cr.squares[src+delta]) )
                        special = (white ? thc::SPECIAL_WEN_PASSANT : thc::SPECIAL_BEN_PASSANT);
                    break;
                }
                case P_RIGHT:
                {
                    reordering_possible = true;
                    delta = white?-7:7;
                    if( !isalpha(cr.squares[src+delta]) )
                        special = (white ? thc::SPECIAL_WEN_PASSANT : thc::SPECIAL_BEN_PASSANT);
                    break;
                }
                default:
                {
                    promoting = true;
                    switch( (code>>2)&3 )
                    {
                        case P_SINGLE:     delta = white?-8:8;   break;
                        case P_LEFT:       delta = white?-9:9;   break;
                        case P_RIGHT:      delta = white?-7:7;   break;
                    }
                    switch( code&3 )
                    {
                        case P_QUEEN:      special = thc::SPECIAL_PROMOTION_QUEEN;    break;
                        case P_ROOK:       special = thc::SPECIAL_PROMOTION_ROOK;     break;
                        case P_BISHOP:     special = thc::SPECIAL_PROMOTION_BISHOP;   break;
                        case P_KNIGHT:     special = thc::SPECIAL_PROMOTION_KNIGHT;   break;
                    }
                    break;
                }
            }
            side->pawns[pawn_offset] = dst = src+delta;

            // If promoting piece, force a reset and retry next time for this side. This
            //  way we accommodate our pawn disappearing, and a new piece appearing in
            //  its place, and the possibility that we cannot remain in fast mode
            //  (because we now have too many queens or other pieces). If the reset
            //  and retry fails this side will generate slow mode moves but keep
            //  retrying until fast is possible again.
            if( promoting )
                side->fast_mode = false;
            else if( reordering_possible )
            {
                if( pawn_ordering[dst] > pawn_ordering[src] ) // increasing capture?
                {
                    for( int i=pawn_offset; i+1<side->nbr_pawns && pawn_ordering[side->pawns[i]]>pawn_ordering[side->pawns[i+1]]; i++ )
                    {
                        int temp = side->pawns[i];
                        side->pawns[i] = side->pawns[i+1];
                        side->pawns[i+1] = temp;
                    }
                }
                else // else decreasing capture
                {
                    for( int i=pawn_offset; i-1>=0 && pawn_ordering[side->pawns[i-1]]>pawn_ordering[side->pawns[i]]; i-- )
                    {
                        int temp = side->pawns[i-1];
                        side->pawns[i-1] = side->pawns[i];
                        side->pawns[i] = temp;
                    }
                }
            }
            break;
        }   // end PAWN
    }   // end switch on moving piece

    // Accomodate captured piece on other side, if other side is in fast mode
    int  capture_location=dst;
    if( special == thc::SPECIAL_WEN_PASSANT )
        capture_location = dst+8;
    else if( special == thc::SPECIAL_BEN_PASSANT )
        capture_location = dst-8;
    char capture = cr.squares[capture_location];
    bool making_capture = (isalpha(capture) ? true : false);
    if( making_capture && other->fast_mode )
    {
        switch( tolower(capture) )
        {
            case 'n':
            {
                if( other->nbr_knights==2 && other->knights[0]==capture_location )
                    other->knights[0] = other->knights[1];
                other->nbr_knights--;
                break;
            }
            case 'r':
            {
                if( other->nbr_rooks==2 && other->rooks[0]==capture_location )
                    other->rooks[0] = other->rooks[1];
                other->nbr_rooks--;
                break;
            }
            case 'b':
            {
                if( is_dark(capture_location) )
                    other->nbr_dark_bishops--;
                else
                    other->nbr_light_bishops--;
                break;
            }
            case 'q':
            {
                if( other->nbr_queens==2 && other->queens[0]==capture_location )
                    other->queens[0] = other->queens[1];
                other->nbr_queens--;
                break;
            }
            case 'p':
            {
                int other_pawn_offset=0;
                for( int i=0; i<other->nbr_pawns; i++ )
                {
                    if( other->pawns[i] == capture_location )
                    {
                        other_pawn_offset = i;
                        break;
                    }
                }
                for( int i=other_pawn_offset; i+1<other->nbr_pawns; i++ )
                {
                    other->pawns[i] = other->pawns[i+1];
                }
                other->nbr_pawns--;
            }
        }
    }
    thc::Move mv;
    mv.src = static_cast<thc::Square>(src);
    mv.dst = static_cast<thc::Square>(dst);
    mv.special = special;
    mv.capture = capture;
    return mv;
}
