/****************************************************************************
 *  In memory position search - find position in games without database index
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <wx/utils.h>
#include "MemoryPositionSearch.h"
#include "DebugPrintf.h"


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

static inline bool is_dark( int sq )
{
    bool dark = (!(sq&8) &&  (sq&1))    // eg (a8,b8,c8...h8) && (b8|d8|f8|h8) odd rank + odd file
             || ( (sq&8) && !(sq&1));   // eg (a7,b7,c7...h7) && (a7|c7|e7|g7) even rank + even file
    return dark;
}

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
bool MemoryPositionSearch::TryFastMode( MpsSide *side )
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
                okay = false;
        }
        else if( cr.squares[i] == (side->white?'N':'n') )
        {
            if( side->nbr_knights < 2 )
                side->knights[side->nbr_knights++] = i;
            else
                okay = false;
        }
        else if( cr.squares[i] == (side->white?'B':'b') )
        {
            if( is_dark(i) )
            {
                side->bishop_dark = i;
                if( side->nbr_dark_bishops < 1 )
                    side->nbr_dark_bishops++;
                else
                    okay = false;
            }
            else
            {
                side->bishop_light = i;
                if( side->nbr_light_bishops < 1 )
                    side->nbr_light_bishops++;
                else
                    okay = false;
            }
        }
        else if( cr.squares[i] == (side->white?'Q':'q') )
        {
            if( side->nbr_queens < 2 )
                side->queens[side->nbr_queens++] = i;
            else
                okay = false;
        }
        else if( cr.squares[i] == (side->white?'K':'k') )
        {
            side->king = i;
        }
    }
    if( side->nbr_queens==2 && side->nbr_pawns>6 )
        okay = false;
    side->fast_mode = okay;
    return okay;
}
    
int  MemoryPositionSearch::DoSearch( uint64_t position_hash )
{
    wxBell();
    games_found.clear();
    hash_target = position_hash;
    int nbr = in_memory_game_cache.size();
    for( int i=0; i<nbr; i++ )
    {
        bool game_found = SearchGame( in_memory_game_cache[i].second );
        if( game_found )
            games_found.push_back( in_memory_game_cache[i].first );
    }    
    return games_found.size();
}

bool MemoryPositionSearch::GetGameidFromRow( int row, int &game_id )
{
    bool ok=true;
    int nbr = games_found.size();
    if( 0<=row && row<nbr )
        game_id = games_found[nbr-1-row];
    else
    {
        ok = false;
        game_id = 0;
    }
    return ok;
}

bool MemoryPositionSearch::SearchGame( std::string &moves_in )
{
    Init();
    uint64_t hash = hash_initial;
    int len = moves_in.size();
    if( hash == hash_target )
        return true;
    for( int i=0; i<len; i++ )
    {
        MpsSide *side  = cr.white ? &sides[0] : &sides[1];
        MpsSide *other = cr.white ? &sides[1] : &sides[0];
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
        hash = cr.Hash64Update(hash,mv);
        if( hash == hash_target )
            return true;
        cr.PlayMove(mv);
    }
    return false;
}

thc::Move MemoryPositionSearch::UncompressMove( char c )
{
    MpsSide *side  = cr.white ? &sides[0] : &sides[1];
    MpsSide *other = cr.white ? &sides[1] : &sides[0];
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

thc::Move MemoryPositionSearch::UncompressSlowMode( char code )
{

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
        std::string s = moves[i].TerseOut();
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
    mv.TerseIn( &cr, the_move.c_str() );
    return mv;
}

thc::Move MemoryPositionSearch::UncompressFastMode( char code, MpsSide *side, MpsSide *other )
{
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
            int delta;
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
            int knight_offset = ((code&N_HI) ? 1 : 0 );;
            src = side->knights[knight_offset];
            int delta;
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
            int delta;
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


