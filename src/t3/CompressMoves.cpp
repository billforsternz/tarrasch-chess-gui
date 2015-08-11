/****************************************************************************
 *  Compress chess moves into one byte
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#if 0
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include "CompressMoves.h"
#include "DebugPrintf.h"

// Compression method is basically hi nibble indicates one of 16 pieces, lo nibble indicates how piece moves
#define CODE_SAME_FILE                          8          // Rank or file codes, this bit set indicates same file (so remaining 3 bits encode rank)
#define CODE_FALL                               8          // Diagonal codes, either RISE/ or FALL\, other 3 bits encode destination file
#define CODE_SPECIAL_PROMOTION_QUEEN            0x00       // Pawn codes, Lo=2 bit promotion + 2 bit direction
#define CODE_SPECIAL_PROMOTION_ROOK             0x04       //
#define CODE_SPECIAL_PROMOTION_BISHOP           0x08       //
#define CODE_SPECIAL_PROMOTION_KNIGHT           0x0c       //

#define CODE_K_SPECIAL_WK_CASTLING              0x01       // King codes, Hi=0, Lo= non-zero vector or castling type
#define CODE_K_SPECIAL_BK_CASTLING              0x02       //
#define CODE_K_SPECIAL_WQ_CASTLING              0x03       //
#define CODE_K_SPECIAL_BQ_CASTLING              0x04       //
#define CODE_K_VECTOR_0                         0x05       //
#define CODE_K_VECTOR_1                         0x06
#define CODE_K_VECTOR_2                         0x07
#define CODE_K_VECTOR_3                         0x08
#define CODE_K_VECTOR_4                         0x09
#define CODE_K_VECTOR_5                         0x0b            // don't assign 0x0a = '\n', to make string more like a 'line' of text
#define CODE_K_VECTOR_6                         0x0e            // don't assign 0x0d = '\r', to make string more like a 'line' of text
#define CODE_K_VECTOR_7                         0x0f

#define CODE_N_SHADOW                           8          // Knight codes, this bit indicates knight code is a shadowed rank or file
#define CODE_N_VECTOR_0                         0x00       // If CODE_N_SHADOW bit is low, then 8 codes for all N vectors
#define CODE_N_VECTOR_1                         0x01       //
#define CODE_N_VECTOR_2                         0x02
#define CODE_N_VECTOR_3                         0x03
#define CODE_N_VECTOR_4                         0x04
#define CODE_N_VECTOR_5                         0x05
#define CODE_N_VECTOR_6                         0x06
#define CODE_N_VECTOR_7                         0x07

// Square macros
#define FILE(sq)    ( (char) (  ((sq)&0x07) + 'a' ) )               // eg c5->'c'
#define RANK(sq)    ( (char) (  '8' - (((sq)>>3) & 0x07) ) )        // eg c5->'5'
#define SOUTH(sq)   (  (thc::Square)((sq) + 8) )                    // eg c5->c4
#define NORTH(sq)   (  (thc::Square)((sq) - 8) )                    // eg c5->c6

std::string SqToStr( int sq )
{
    char file = FILE(sq);
    char rank = RANK(sq);
    char buf[3];
    buf[0] = file;
    buf[1] = rank;
    buf[2] = '\0';
    std::string s = buf;
    return s;
}

std::string SqToStr( thc::Square sq )
{
    return SqToStr( (int)sq );
}

CompressMoves::CompressMoves( const CompressMoves& copy_from_me )
{
    *this = copy_from_me;
}

CompressMoves & CompressMoves::operator= (const CompressMoves & copy_from_me )
{
    evil_queen_mode = copy_from_me.evil_queen_mode;
    cr = copy_from_me.cr;
	if( !evil_queen_mode )
	{
		for( int i=0; i<16; i++ )
		{
			white_pieces[i] = copy_from_me.white_pieces[i];
			if( copy_from_me.white_pieces[i].shadow_file )
				white_pieces[i].shadow_file = white_pieces + (copy_from_me.white_pieces[i].shadow_file - copy_from_me.white_pieces);
			if( copy_from_me.white_pieces[i].shadow_rank )
				white_pieces[i].shadow_rank = white_pieces + (copy_from_me.white_pieces[i].shadow_rank - copy_from_me.white_pieces);
			if( copy_from_me.white_pieces[i].shadow_rook )
				white_pieces[i].shadow_rook = white_pieces + (copy_from_me.white_pieces[i].shadow_rook - copy_from_me.white_pieces);
			if( copy_from_me.white_pieces[i].shadow_owner )
				white_pieces[i].shadow_owner = white_pieces + (copy_from_me.white_pieces[i].shadow_owner - copy_from_me.white_pieces);
		}
		for( int i=0; i<16; i++ )
		{
			black_pieces[i] = copy_from_me.black_pieces[i];
			if( copy_from_me.black_pieces[i].shadow_file )
				black_pieces[i].shadow_file = black_pieces + (copy_from_me.black_pieces[i].shadow_file - copy_from_me.black_pieces);
			if( copy_from_me.black_pieces[i].shadow_rank )
				black_pieces[i].shadow_rank = black_pieces + (copy_from_me.black_pieces[i].shadow_rank - copy_from_me.black_pieces);
			if( copy_from_me.black_pieces[i].shadow_rook )
				black_pieces[i].shadow_rook = black_pieces + (copy_from_me.black_pieces[i].shadow_rook - copy_from_me.black_pieces);
			if( copy_from_me.black_pieces[i].shadow_owner )
				black_pieces[i].shadow_owner = black_pieces + (copy_from_me.black_pieces[i].shadow_owner - copy_from_me.black_pieces);
		}
		for( int i=0; i<64; i++ )
		{
			Tracker *p = copy_from_me.trackers[64];
			Tracker *q=0;
			if( p )
			{
				if( copy_from_me.white_pieces <= p  &&  p < &copy_from_me.white_pieces[16] )
					q = white_pieces + (p-copy_from_me.white_pieces);
				else if( copy_from_me.black_pieces <= p  &&  p < &copy_from_me.black_pieces[16] )
					q = black_pieces + (p-copy_from_me.black_pieces);
			}
			trackers[i] = q;
		}
	}
    return *this;
}

void CompressMoves::Init()
{
    evil_queen_mode = false;
    cr.Init();
    ((thc::ChessPosition)cr).Init();
    for( int i=0; i<64; i++ )
        trackers[i] = NULL;
    square_init( TI_K,  'K', thc::e1 );
    square_init( TI_KN, 'N', thc::g1 );
    square_init( TI_QN, 'N', thc::b1 );
    square_init( TI_KR, 'R', thc::h1 );
    square_init( TI_QR, 'R', thc::a1 );
    square_init( TI_KB, 'B', thc::f1 );
    square_init( TI_QB, 'B', thc::c1 );
    square_init( TI_Q,  'Q', thc::d1 );
    white_pieces[TI_Q].shadow_rank   = &white_pieces[TI_KN];
    white_pieces[TI_KN].shadow_owner = &white_pieces[TI_Q];
    white_pieces[TI_Q].shadow_file   = &white_pieces[TI_QN];
    white_pieces[TI_QN].shadow_owner = &white_pieces[TI_Q];
    square_init( TI_AP, 'P', thc::a2 );
    square_init( TI_BP, 'P', thc::b2 );
    square_init( TI_CP, 'P', thc::c2 );
    square_init( TI_DP, 'P', thc::d2 );
    square_init( TI_EP, 'P', thc::e2 );
    square_init( TI_FP, 'P', thc::f2 );
    square_init( TI_GP, 'P', thc::g2 );
    square_init( TI_HP, 'P', thc::h2 );
    square_init( TI_K,  'k', thc::e8 );
    square_init( TI_KN, 'n', thc::g8 );
    square_init( TI_QN, 'n', thc::b8 );
    square_init( TI_KR, 'r', thc::h8 );
    square_init( TI_QR, 'r', thc::a8 );
    square_init( TI_KB, 'b', thc::f8 );
    square_init( TI_QB, 'b', thc::c8 );
    square_init( TI_Q,  'q', thc::d8 );
    black_pieces[TI_Q].shadow_rank   = &black_pieces[TI_KN];
    black_pieces[TI_KN].shadow_owner = &black_pieces[TI_Q];
    black_pieces[TI_Q].shadow_file   = &black_pieces[TI_QN];
    black_pieces[TI_QN].shadow_owner = &black_pieces[TI_Q];
    square_init( TI_AP, 'p', thc::a7 );
    square_init( TI_BP, 'p', thc::b7 );
    square_init( TI_CP, 'p', thc::c7 );
    square_init( TI_DP, 'p', thc::d7 );
    square_init( TI_EP, 'p', thc::e7 );
    square_init( TI_FP, 'p', thc::f7 );
    square_init( TI_GP, 'p', thc::g7 );
    square_init( TI_HP, 'p', thc::h7 );
}

void CompressMoves::Init( thc::ChessPosition &cp )
{
	// Temp - for now just use evil queen mode if we start from an arbitrary position
	bool is_initial_position = ( 0 == strcmp(cp.squares,"rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR")
								 ) && cp.white;
	if( is_initial_position )
	{
		Init();
	}
	else
	{
		evil_queen_mode = true;
		cr = cp;
	}
}

bool CompressMoves::Check( bool do_internal_check, const char *description, thc::ChessPosition *external )
{
    bool ok = true;
    std::string pos1 = cr.ToDebugStr();
    std::string pos2;
    if( !do_internal_check )
    {
        cprintf( "Check Fail: internal tracker position not checked\n");
        ok = false;
    }
    else
    {
        thc::ChessPosition cp;
        cp.white = cr.white;
        for( int i=0; i<64; i++ )
        {
            Tracker *pt=trackers[i];
            char piece = ' ';
            if( pt && pt->in_use )
            {
                if( (int)pt->sq != i )
                {
                    cprintf( "Check Fail: square mismatch: %d %d\n", i, (int)pt->sq );
                    ok = false;
                }
                piece = pt->piece;
                if( pt->shadow_owner )
                {
                    Tracker *queen = pt->shadow_owner;
                    if( queen->shadow_rook && queen->shadow_rook==pt )
                        piece = ' ';
                }
            }
            cp.squares[i] = piece;
        }
        pos2 = cp.ToDebugStr();
        if( 0 != pos1.compare(pos2) )
        {
            cprintf( "Check Fail: internal tracker position mismatch\n");
            ok = false;
        }
    }
    if( ok )
    {
        check_last_success = pos1;
        check_last_description = description;
    }
    else
    {
        cprintf( "Compression/Decompression problem: %s\n", description  );
        cprintf( "last success position:%s", check_last_success.c_str() );
        cprintf( "last success description:%s\n", check_last_description.c_str() );
        if( external )
            cprintf( "external position:%s", external->ToDebugStr().c_str() );
        cprintf( "ref position:%s", pos1.c_str() );
        if( do_internal_check )
            cprintf( "tracker position:%s", pos2.c_str() );
    }
    //if( !ok )
    //    exit(-1);
    return ok;
}

int CompressMoves::compress_move( thc::Move mv, char *storage )
{
	int nbr_bytes=1;
    if( evil_queen_mode )
    {
        char ch;
        compress_move_slow_mode( mv, ch );
        *storage = ch;
		cr.PlayMove(mv);
    }
    else
    {
		int code=0;
		int src = mv.src;
		int dst = mv.dst;
		int captured_sq = cr.squares[dst]<'A' ? -1 : dst;
		Tracker *pt = trackers[src];
		int tracker_id = pt->tracker_id;
		char piece = pt->piece;
		switch( mv.special )
		{
			case thc::NOT_SPECIAL:
			case thc::SPECIAL_BPAWN_2SQUARES:
			case thc::SPECIAL_WPAWN_2SQUARES:
			case thc::SPECIAL_KING_MOVE:
			default:
			{
				break;
			}
			case thc::SPECIAL_WK_CASTLING:
			{
				code = CODE_K_SPECIAL_WK_CASTLING;
				Tracker *rook = &white_pieces[TI_KR];
				rook->sq = thc::f1;
				trackers[thc::f1] = rook;
				trackers[thc::h1] = NULL;
				break;
			}
			case thc::SPECIAL_BK_CASTLING:
			{
				code = CODE_K_SPECIAL_BK_CASTLING;
				Tracker *rook = &black_pieces[TI_KR];
				rook->sq = thc::f8;
				trackers[thc::f8] = rook;
				trackers[thc::h8] = NULL;
				break;
			}
			case thc::SPECIAL_WQ_CASTLING:
			{
				code = CODE_K_SPECIAL_WQ_CASTLING;
				Tracker *rook = &white_pieces[TI_QR];
				rook->sq = thc::d1;
				trackers[thc::d1] = rook;
				trackers[thc::a1] = NULL;
				break;
			}
			case thc::SPECIAL_BQ_CASTLING:
			{
				code = CODE_K_SPECIAL_BQ_CASTLING;
				Tracker *rook = &black_pieces[TI_QR];
				rook->sq = thc::d8;
				trackers[thc::d8] = rook;
				trackers[thc::a8] = NULL;
				break;
			}
			case thc::SPECIAL_PROMOTION_QUEEN:
			{
				code = CODE_SPECIAL_PROMOTION_QUEEN;
				pt->piece = cr.white ? 'Q' : 'q';
				Tracker *shadow = (cr.white ? &white_pieces[15] : &black_pieces[15]);
				bool found=false;
				for( int i=0; i<16; i++,shadow-- )
				{
					if( !shadow->in_use )
					{
						pt->shadow_rook = shadow;
						shadow->in_use = true;
						shadow->shadow_owner = pt;
						shadow->piece = cr.white ? 'R' : 'r';   // shadow is a phantom rook, handles rank and file moves for new queen
						found = true;
						break;                                  // shadow successfully found
					}
				}
				if( !found )
				{
					// no shadow available goto evil queen mode
					extern bool gbl_evil_queen;
					gbl_evil_queen = true;
					evil_queen_mode = true;
				}
				break;
			}
			case thc::SPECIAL_PROMOTION_ROOK:
			{
				code = CODE_SPECIAL_PROMOTION_ROOK;
				pt->piece = cr.white ? 'R' : 'r';
				break;
			}
			case thc::SPECIAL_PROMOTION_BISHOP:
			{
				code = CODE_SPECIAL_PROMOTION_BISHOP;
				pt->piece = cr.white ? 'B' : 'b';
				break;
			}
			case thc::SPECIAL_PROMOTION_KNIGHT:
			{
				code = CODE_SPECIAL_PROMOTION_KNIGHT;
				pt->piece = cr.white ? 'N' : 'n';
				break;
			}
			case thc::SPECIAL_WEN_PASSANT:
			{
				captured_sq = SOUTH(dst);
				break;
			}
			case thc::SPECIAL_BEN_PASSANT:
			{
				captured_sq = NORTH(dst);
				break;
			}
		}
    
		switch( piece )
		{
			case 'P':
			{
				if( src-dst == 16 )
					code = 3;               // 2 square advance
				else
					code += (src-dst-7);    // 9\,8| or 7/ -> 2,1 or 0 = NW,N,NE
				// Note += because may already have CODE_SPECIAL_PROMOTION_QUEEN etc
				break;
			}
			case 'p':
			{
				if( dst-src == 16 )
					code = 3;               // 2 square advance
				else
					code += (dst-src-7);    // 9\,8| or 7/ -> 2,1 or 0 = SE,S,SW
				// Note += because may already have CODE_SPECIAL_PROMOTION_QUEEN etc
				break;
			}
			case 'K':
			case 'k':
			{
				switch( src-dst )
				{
					case   9: code = CODE_K_VECTOR_0; break; // 9\ FALL
					case   8: code = CODE_K_VECTOR_1; break; // 8|
					case   7: code = CODE_K_VECTOR_2; break; // 7/
					case   1: code = CODE_K_VECTOR_3; break; // 1-
					case  -1: code = CODE_K_VECTOR_4; break;
					case  -7: code = CODE_K_VECTOR_5; break;
					case  -8: code = CODE_K_VECTOR_6; break;
					case  -9: code = CODE_K_VECTOR_7; break;
				}
			}
			case 'N':
			case 'n':
			{
				switch( src-dst )
				{
					case  17: code = CODE_N_VECTOR_0; break;
					case  15: code = CODE_N_VECTOR_1; break;
					case  10: code = CODE_N_VECTOR_2; break;
					case   6: code = CODE_N_VECTOR_3; break;
					case -17: code = CODE_N_VECTOR_4; break;
					case -15: code = CODE_N_VECTOR_5; break;
					case -10: code = CODE_N_VECTOR_6; break;
					case  -6: code = CODE_N_VECTOR_7; break;
				}
				break;
			}
			case 'r':
			case 'R':
			{
				if( (src&7) == (dst&7) )    // same file ?
					code = CODE_SAME_FILE + ((dst>>3) & 7);   // yes, so encode new rank
				else
					code = dst & 7;                           // no, so encode new file
				break;
			}
			case 'b':
			case 'B':
			{
				int abs = (src>dst ? src-dst : dst-src);
				if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
				{
					code = CODE_FALL + (dst&7); // fall( = \) + dst file
				}
				else // if abs%7 == 0
				{
					code = (dst&7); // rise( = /) + dst file
				}
				break;
			}
			case 'q':
			case 'Q':
			{
				extern bool gbl_double_byte;
				if( (src&7) == (dst&7) )                // same file ?
				{
					if( pt->shadow_rook )
					{
						code = CODE_SAME_FILE + ((dst>>3)&7);   // yes encode rank
						tracker_id = pt->shadow_rook->tracker_id;
					}
					else if( pt->shadow_rank )
					{
						code = CODE_N_SHADOW + ((dst>>3)&7);    // shadow knight encodes rank
						tracker_id = pt->shadow_rank->tracker_id;
					}
					else // no shadow available, use two bytes, first byte is a zero files diagonal move
					{
						code = CODE_FALL + (src&7);
						nbr_bytes = 2;
						gbl_double_byte = true;
					}
				}
				else if( (src&0x38) == (dst&0x38) )     // same rank ?
				{
					if( pt->shadow_rook )
					{
						code = (dst&7);                             // yes encode file
						tracker_id = pt->shadow_rook->tracker_id;
					}
					else if( pt->shadow_file )
					{
						code = CODE_N_SHADOW + (dst&7);             // shadow knight encodes file
						tracker_id = pt->shadow_file->tracker_id;
					}
					else // no shadow available, use two bytes, first byte is a zero files diagonal move
					{
						code = CODE_FALL + (src&7);
						nbr_bytes = 2;
						gbl_double_byte = true;
					}
				}
				else
				{
					int abs = (src>dst ? src-dst : dst-src);
					if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
						code = CODE_FALL + (dst&7); // fall( = \) + dst file
					else
						code = (dst&7); // rise( = /) + dst file
				}
				break;
			}
		}
		if( captured_sq >= 0 )
		{
			Tracker *captured = trackers[captured_sq];
			if( captured )
			{
				trackers[captured_sq] = NULL;
				if( !captured->shadow_owner )
					captured->in_use = false;
				if( captured->piece=='Q' ||captured->piece=='q')
				{
					if( captured->shadow_rook )
						captured->shadow_rook->shadow_owner = NULL;
					if( captured->shadow_rank )
						captured->shadow_rank->shadow_owner = NULL;
					if( captured->shadow_file )
						captured->shadow_file->shadow_owner = NULL;
				}
			}
		}
		pt->sq = (thc::Square)dst;
		trackers[src] = NULL;
		trackers[dst] = pt;
		*storage = (char)(tracker_id + code);
		if( nbr_bytes > 1 )
		{
			storage++;
			*storage = 0x40 | (dst&0x3f);
		}
		cr.PlayMove(mv);
		Check(true,"After compress_moves() tracker check",NULL);
	}
	return nbr_bytes;
}

// If an evil queen appears on the board switch to a much slower scheme for
//  compressing moves into one byte.
//  Scheme is;
//  1) Make a list of all legal moves in standard SAN text format, sorted alphabetically
//  2) Find the move to be compressed in the list
//  3) Encode the move as one byte, value is 255 - list index (to avoid code 0, '\n' etc)
//
//  Why use slow text representation and sorting ? So that a specification for the
//  whole compression scheme can be published if necessary, without relying on implementation
//  details like the order that moves come out of the move generator etc.
//
//  According to http://www.chess.com/forum/view/fun-with-chess/what-chess-position-has-the-most-number-of-possible-moves
//  the maximum number of legal moves available in any position is less than 256,
//  in fact it is 218 in the following position;
//
//  "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1"
//
void CompressMoves::compress_move_slow_mode( thc::Move in, char &out )
{
    /*thc::ChessRules cr2;
    cr2.Forsyth("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1");
    std::vector<thc::Move> moves2;
    cr2.GenLegalMoveList( moves2 );
    size_t len2 = moves2.size();
    cprintf( "Should be 218 moves, actually it is %u\n", len2 );
    cprintf( "By shear coincidence, MAXMOVES is also 218, I think, check: %d\n", MAXMOVES ); */
    
    // Generate a list of all legal moves, in string form, sorted
    std::vector<thc::Move> moves;
    cr.GenLegalMoveList( moves );
    std::vector<std::string> moves_alpha;

    // Coding scheme relies on 255 valid codes and one error code
    size_t idx=255;  //error code
    size_t len = moves.size();
    if( len <= 255 )
    {
        for( size_t i=0; i<len; i++ )
        {
            std::string s = moves[i].NaturalOut(&cr);
            moves_alpha.push_back(s);
        }
        std::sort( moves_alpha.begin(), moves_alpha.end() );
        
        // Find this move in the list
        std::string the_move = in.NaturalOut(&cr);
        for( size_t i=0; i<len; i++ )
        {
            if( moves_alpha[i] == the_move )
            {
                idx = i;
                break;
            }
        }
    }
    
    // char generated is '\xff' for first move in list, '\fe' for second etc
    // '\x00 is an error
    out = static_cast<char>( 255-idx );
}

void CompressMoves::decompress_move_slow_mode( char in, thc::Move &out )
{
    // Generate a list of all legal moves, in string form, sorted
    std::vector<thc::Move> moves;
    cr.GenLegalMoveList( moves );
    std::vector<std::string> moves_alpha;
    
    // Coding scheme relies on 255 valid codes and one error code
    size_t len = moves.size();
    for( size_t i=0; i<len; i++ )
    {
        std::string s = moves[i].NaturalOut(&cr);
        moves_alpha.push_back(s);
    }
    std::sort( moves_alpha.begin(), moves_alpha.end() );

    // '\xff' (i.e. 255) is first move in list, '\fe' (i.e. 254) is second etc
    size_t code = static_cast<size_t>( in );
    code &= 0xff;
    size_t idx = 255-code;  // 255->0, 254->1 etc.
    if( idx >= len )
        idx = 0;    // all errors resolve to this - take first move from list
    std::string the_move = moves_alpha[idx];
    out.NaturalInFast( &cr, the_move.c_str() );
}

int CompressMoves::decompress_move( const char *storage, thc::Move &mv )
{
    int nbr_bytes=1;
    
    // Slow decompression ?
    if( evil_queen_mode )
    {
        char val = *storage;
        decompress_move_slow_mode( val, mv );
        cr.PlayMove(mv);
    }
    
    // Otherwise normal fast decompression
    else
    {
        char val = *storage;
        int lo = val & 0x0f;
        int hi = 0x0f & (val>>4);
        int tracker_id = hi;
        thc::SPECIAL special = thc::NOT_SPECIAL;
        Tracker *pt = cr.white ? &white_pieces[tracker_id] : &black_pieces[tracker_id];
        char piece = pt->piece;
        int src = (int)pt->sq;
        int dst;
        if( pt->shadow_owner )
        {
            Tracker *queen = pt->shadow_owner;
            if( queen->shadow_rook )  // captured piece that handles all rook moves for queen ?
            {
                src = queen->sq;
                if( lo & CODE_SAME_FILE )
                {
                    // Same file, so change rank
                    dst = ((lo<<3)&0x38) | (src&7);  // change rank
                }
                else
                {
                    // Not same file, so change file
                    dst = (src&0x38) | (lo&7);  // change file
                }
            }
            else if( lo & CODE_N_SHADOW )  // knight that handles rank or file moves for queen ?
            {
                src = queen->sq;
                if( tracker_id == TI_KN )   // Init() sets queen->shadow_rank to KN
                {
                    // Same file, so change rank
                    dst = ((lo<<3)&0x38) | (src&7);  // change rank
                }
                else                        // Init() sets queen->shadow_file to QN
                {
                    // Not same file, so change file
                    dst = (src&0x38) | (lo&7);  // change file
                }
            }
            else // else must be a non-shadowed knight move
            {
                switch( lo )
                {
                    case CODE_N_VECTOR_0: dst = src - 17;  break;
                    case CODE_N_VECTOR_1: dst = src - 15;  break;
                    case CODE_N_VECTOR_2: dst = src - 10;  break;
                    case CODE_N_VECTOR_3: dst = src - 6;   break;
                    case CODE_N_VECTOR_4: dst = src + 17;  break;
                    case CODE_N_VECTOR_5: dst = src + 15;  break;
                    case CODE_N_VECTOR_6: dst = src + 10;  break;
                    case CODE_N_VECTOR_7: dst = src + 6;   break;
                }
            }
        }
        else
        {
            switch( piece )
            {
                case 'N':
                case 'n':
                {
                    switch( lo )
                    {
                        case CODE_N_VECTOR_0: dst = src - 17;  break;
                        case CODE_N_VECTOR_1: dst = src - 15;  break;
                        case CODE_N_VECTOR_2: dst = src - 10;  break;
                        case CODE_N_VECTOR_3: dst = src - 6;   break;
                        case CODE_N_VECTOR_4: dst = src + 17;  break;
                        case CODE_N_VECTOR_5: dst = src + 15;  break;
                        case CODE_N_VECTOR_6: dst = src + 10;  break;
                        case CODE_N_VECTOR_7: dst = src + 6;   break;
                    }
                    break;
                }
                case 'K':
                case 'k':
                {
                    special = thc::SPECIAL_KING_MOVE;
                    switch( lo )
                    {
                        case CODE_K_SPECIAL_WK_CASTLING:
                        {
                            special = thc::SPECIAL_WK_CASTLING;
                            Tracker *rook = &white_pieces[TI_KR];
                            rook->sq = thc::f1;
                            trackers[thc::f1] = rook;
                            trackers[thc::h1] = NULL;
                            dst = thc::g1;
                            break;
                        }
                        case CODE_K_SPECIAL_BK_CASTLING:
                        {
                            special = thc::SPECIAL_BK_CASTLING;
                            Tracker *rook = &black_pieces[TI_KR];
                            rook->sq = thc::f8;
                            trackers[thc::f8] = rook;
                            trackers[thc::h8] = NULL;
                            dst = thc::g8;
                            break;
                        }
                        case CODE_K_SPECIAL_WQ_CASTLING:
                        {
                            special = thc::SPECIAL_WQ_CASTLING;
                            Tracker *rook = &white_pieces[TI_QR];
                            rook->sq = thc::d1;
                            trackers[thc::d1] = rook;
                            trackers[thc::a1] = NULL;
                            dst = thc::c1;
                            break;
                        }
                        case CODE_K_SPECIAL_BQ_CASTLING:
                        {
                            special = thc::SPECIAL_BQ_CASTLING;
                            Tracker *rook = &black_pieces[TI_QR];
                            rook->sq = thc::d8;
                            trackers[thc::d8] = rook;
                            trackers[thc::a8] = NULL;
                            dst = thc::c8;
                            break;
                        }
                        case  CODE_K_VECTOR_0: dst = src - 9; break;
                        case  CODE_K_VECTOR_1: dst = src - 8; break;
                        case  CODE_K_VECTOR_2: dst = src - 7; break;
                        case  CODE_K_VECTOR_3: dst = src - 1; break;
                        case  CODE_K_VECTOR_4: dst = src + 1; break;
                        case  CODE_K_VECTOR_5: dst = src + 7; break;
                        case  CODE_K_VECTOR_6: dst = src + 8; break;
                        case  CODE_K_VECTOR_7: dst = src + 9; break;
                    }
                    break;
                }
                case 'Q':
                case 'q':
                case 'B':
                case 'b':
                {
                    if( lo & CODE_FALL )
                    {
                        // FALL\ + file
                        int file_delta = (lo&7) - (src&7);
                        if( file_delta == 0 ) //exception code signals 2 byte mode
                        {
                            storage++;
                            dst = *storage & 0x3f;
                            nbr_bytes = 2;
                        }
                        else
                            dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                    }
                    else
                    {
                        // RISE/ + file
                        int file_delta = (lo&7) - (src&7);
                        dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                    }
                    break;
                }
                case 'R':
                case 'r':
                {
                    if( lo & CODE_SAME_FILE )
                    {
                        dst = ((lo<<3)&0x38) | (src&7);  // change rank
                    }
                    else
                    {
                        dst = (src&0x38) | (lo&7);  // change file
                    }
                    break;
                }
                case 'P':
                {
                    if( (src&0x38) == 8 ) // if a7(8) - h7(15)
                    {
                        switch( lo & 0x0c )
                        {
                            case CODE_SPECIAL_PROMOTION_QUEEN:
                            {
                                special = thc::SPECIAL_PROMOTION_QUEEN;
                                pt->piece = 'Q';
                                Tracker *shadow = &white_pieces[15];
                                bool found=false;
                                for( int i=0; i<16; i++,shadow-- )
                                {
                                    if( !shadow->in_use )
                                    {
                                        pt->shadow_rook = shadow;
                                        shadow->in_use = true;
                                        shadow->shadow_owner = pt;
                                        shadow->piece = 'R';  // shadow is a phantom rook, handles rank and file moves for new queen
                                        found = true;
                                        break;
                                    }
                                }
                                
                                // If no shadow found we now switch to evil queen mode, starting with next move
                                if( !found )
                                    evil_queen_mode = true;  // no need for double byte coding with this
                                break;
                            }
                            case CODE_SPECIAL_PROMOTION_ROOK:
                            {
                                special = thc::SPECIAL_PROMOTION_ROOK;
                                pt->piece = 'R';
                                break;
                            }
                            case CODE_SPECIAL_PROMOTION_BISHOP:
                            {
                                special = thc::SPECIAL_PROMOTION_BISHOP;
                                pt->piece = 'B';
                                break;
                            }
                            case CODE_SPECIAL_PROMOTION_KNIGHT:
                            {
                                special = thc::SPECIAL_PROMOTION_KNIGHT;
                                pt->piece = 'N';
                                break;
                            }
                        }
                        
                    }
                    if( (lo&3) == 3 )
                    {
                        special = thc::SPECIAL_WPAWN_2SQUARES;
                        dst = src-16;
                    }
                    else
                    {
                        dst = src-(lo&3)-7; // 2\,1| or 0/ -> -9,-8 or -7
                        if( !(lo&1) )    // if( 2\ or 0/ )
                        {
                            if( cr.squares[dst]==' ' && cr.squares[SOUTH(dst)]=='p' )
                                special = thc::SPECIAL_WEN_PASSANT;
                        }
                    }
                    break;
                }
                    
                case 'p':
                {
                    if( (src&0x38) == 0x30 ) // if a2(48) - h2(55)
                    {
                        switch( lo & 0x0c )
                        {
                            case CODE_SPECIAL_PROMOTION_QUEEN:
                            {
                                special = thc::SPECIAL_PROMOTION_QUEEN;
                                pt->piece = 'q';
                                Tracker *shadow = &black_pieces[15];
                                bool found = false;
                                for( int i=0; i<16; i++,shadow-- )
                                {
                                    if( !shadow->in_use )
                                    {
                                        pt->shadow_rook = shadow;
                                        shadow->in_use = true;
                                        shadow->shadow_owner = pt;
                                        shadow->piece = 'r';  // shadow is a phantom rook, handles rank and file moves for new queen
                                        found = true;
                                        break;
                                    }
                                }
                                // If no shadow found we now switch to evil queen mode, starting with next move
                                if( !found )
                                    evil_queen_mode = true;  // no need for double byte coding with this
                                break;
                            }
                            case CODE_SPECIAL_PROMOTION_ROOK:
                            {
                                special = thc::SPECIAL_PROMOTION_ROOK;
                                pt->piece = 'r';
                                break;
                            }
                            case CODE_SPECIAL_PROMOTION_BISHOP:
                            {
                                special = thc::SPECIAL_PROMOTION_BISHOP;
                                pt->piece = 'b';
                                break;
                            }
                            case CODE_SPECIAL_PROMOTION_KNIGHT:
                            {
                                special = thc::SPECIAL_PROMOTION_KNIGHT;
                                pt->piece = 'n';
                                break;
                            }
                        }
                        
                    }
                    if( (lo&3) == 3 )
                    {
                        special = thc::SPECIAL_BPAWN_2SQUARES;
                        dst = src+16;
                    }
                    else
                    {
                        dst = src+(lo&3)+7; // 2\,1| or 0/ -> +9,+8 or +7
                        if( !(lo&1) )    // if( 2\ or 0/ )
                        {
                            if( cr.squares[dst]==' ' && cr.squares[NORTH(dst)]=='P' )
                                special = thc::SPECIAL_BEN_PASSANT;
                        }
                    }
                    break;
                }
            }
        }
        
        mv.src = (thc::Square)src;
        mv.dst = (thc::Square)dst;
        mv.special = special;
        mv.capture = ' ';
        int captured_sq = cr.squares[dst]<'A' ? -1 : dst;
        if( special == thc::SPECIAL_BEN_PASSANT )
            captured_sq = NORTH(dst);
        else if( special == thc::SPECIAL_WEN_PASSANT )
            captured_sq = SOUTH(dst);
        pt = trackers[src];
        char desc[100];
        sprintf( desc, "pt is NULL; decompress_move() code=%02x, src=%s, dst=%s", val&0xff, SqToStr(src).c_str(), SqToStr(dst).c_str() );
        if( pt == NULL )
        {
            Check( false, desc, NULL );
        }
        if( captured_sq >= 0 )
        {
            mv.capture = cr.squares[captured_sq];
            Tracker *captured = trackers[captured_sq];
            if( captured )
            {
                trackers[captured_sq] = NULL;
                if( !captured->shadow_owner )
                    captured->in_use = false;
                if( captured->piece=='Q' ||captured->piece=='q')
                {
                    if( captured->shadow_rook )
                        captured->shadow_rook->shadow_owner = NULL;
                    if( captured->shadow_rank )
                        captured->shadow_rank->shadow_owner = NULL;
                    if( captured->shadow_file )
                        captured->shadow_file->shadow_owner = NULL;
                }
            }
        }
        pt->sq = (thc::Square)dst;
        trackers[src] = NULL;
        trackers[dst] = pt;
        cr.PlayMove(mv);
        Check( true, desc+12, NULL );
    }
    return nbr_bytes;
}

void CompressMoves::decompress_move_stay( const char *storage, thc::Move &mv ) const // decompress but don't advance
{
    int nbr_bytes=1;
    char val = *storage;
    int lo = val & 0x0f;
    int hi = 0x0f & (val>>4);
    int tracker_id = hi;
    thc::SPECIAL special = thc::NOT_SPECIAL;
    const Tracker *pt = cr.white ? &white_pieces[tracker_id] : &black_pieces[tracker_id];
    char piece = pt->piece;
    int src = (int)pt->sq;
    int dst;
    if( pt->shadow_owner )
    {
        Tracker *queen = pt->shadow_owner;
        if( queen->shadow_rook )  // captured piece that handles all rook moves for queen ?
        {
            src = queen->sq;
            if( lo & CODE_SAME_FILE )
            {
                // Same file, so change rank
                dst = ((lo<<3)&0x38) | (src&7);  // change rank
            }
            else
            {
                // Not same file, so change file
                dst = (src&0x38) | (lo&7);  // change file
            }
        }
        else if( lo & CODE_N_SHADOW )  // knight that handles rank or file moves for queen ?
        {
            src = queen->sq;
            if( tracker_id == TI_KN )   // Init() sets queen->shadow_rank to KN
            {
                // Same file, so change rank
                dst = ((lo<<3)&0x38) | (src&7);  // change rank
            }
            else                        // Init() sets queen->shadow_file to QN
            {
                // Not same file, so change file
                dst = (src&0x38) | (lo&7);  // change file
            }
        }
        else // else must be a non-shadowed knight move
        {
            switch( lo )
            {
                case CODE_N_VECTOR_0: dst = src - 17;  break;
                case CODE_N_VECTOR_1: dst = src - 15;  break;
                case CODE_N_VECTOR_2: dst = src - 10;  break;
                case CODE_N_VECTOR_3: dst = src - 6;   break;
                case CODE_N_VECTOR_4: dst = src + 17;  break;
                case CODE_N_VECTOR_5: dst = src + 15;  break;
                case CODE_N_VECTOR_6: dst = src + 10;  break;
                case CODE_N_VECTOR_7: dst = src + 6;   break;
            }
        }
    }
    else
    {
        switch( piece )
        {
            case 'N':
            case 'n':
            {
                switch( lo )
                {
                    case CODE_N_VECTOR_0: dst = src - 17;  break;
                    case CODE_N_VECTOR_1: dst = src - 15;  break;
                    case CODE_N_VECTOR_2: dst = src - 10;  break;
                    case CODE_N_VECTOR_3: dst = src - 6;   break;
                    case CODE_N_VECTOR_4: dst = src + 17;  break;
                    case CODE_N_VECTOR_5: dst = src + 15;  break;
                    case CODE_N_VECTOR_6: dst = src + 10;  break;
                    case CODE_N_VECTOR_7: dst = src + 6;   break;
                }
                break;
            }
            case 'K':
            case 'k':
            {
                special = thc::SPECIAL_KING_MOVE;
                switch( lo )
                {
                    case CODE_K_SPECIAL_WK_CASTLING:
                    {
                        special = thc::SPECIAL_WK_CASTLING;
                        dst = thc::g1;
                        break;
                    }
                    case CODE_K_SPECIAL_BK_CASTLING:
                    {
                        special = thc::SPECIAL_BK_CASTLING;
                        dst = thc::g8;
                        break;
                    }
                    case CODE_K_SPECIAL_WQ_CASTLING:
                    {
                        special = thc::SPECIAL_WQ_CASTLING;
                        dst = thc::c1;
                        break;
                    }
                    case CODE_K_SPECIAL_BQ_CASTLING:
                    {
                        special = thc::SPECIAL_BQ_CASTLING;
                        dst = thc::c8;
                        break;
                    }
                    case  CODE_K_VECTOR_0: dst = src - 9; break;
                    case  CODE_K_VECTOR_1: dst = src - 8; break;
                    case  CODE_K_VECTOR_2: dst = src - 7; break;
                    case  CODE_K_VECTOR_3: dst = src - 1; break;
                    case  CODE_K_VECTOR_4: dst = src + 1; break;
                    case  CODE_K_VECTOR_5: dst = src + 7; break;
                    case  CODE_K_VECTOR_6: dst = src + 8; break;
                    case  CODE_K_VECTOR_7: dst = src + 9; break;
                }
                break;
            }
            case 'Q':
            case 'q':
            case 'B':
            case 'b':
            {
                if( lo & CODE_FALL )
                {
                    // FALL\ + file
                    int file_delta = (lo&7) - (src&7);
                    if( file_delta == 0 ) //exception code signals 2 byte mode
                    {
                        storage++;
                        dst = *storage & 0x3f;
                        nbr_bytes = 2;
                    }
                    else
                        dst = src + 9*file_delta;   // eg src=a8(0), dst=h1(63), file_delta=7  -> 9*7 =63
                }
                else
                {
                    // RISE/ + file
                    int file_delta = (lo&7) - (src&7);
                    dst = src - 7*file_delta;   // eg src=h8(7), dst=a1(56), file_delta=7  -> 7*7 =49
                }
                break;
            }
            case 'R':
            case 'r':
            {
                if( lo & CODE_SAME_FILE )
                {
                    dst = ((lo<<3)&0x38) | (src&7);  // change rank
                }
                else
                {
                    dst = (src&0x38) | (lo&7);  // change file
                }
                break;
            }
            case 'P':
            {
                if( (src&0x38) == 8 ) // if a7(8) - h7(15)
                {
                    switch( lo & 0x0c )
                    {
                        case CODE_SPECIAL_PROMOTION_QUEEN:
                        {
                            special = thc::SPECIAL_PROMOTION_QUEEN;
                            break;
                        }
                        case CODE_SPECIAL_PROMOTION_ROOK:
                        {
                            special = thc::SPECIAL_PROMOTION_ROOK;
                            break;
                        }
                        case CODE_SPECIAL_PROMOTION_BISHOP:
                        {
                            special = thc::SPECIAL_PROMOTION_BISHOP;
                            break;
                        }
                        case CODE_SPECIAL_PROMOTION_KNIGHT:
                        {
                            special = thc::SPECIAL_PROMOTION_KNIGHT;
                            break;
                        }
                    }
                    
                }
                if( (lo&3) == 3 )
                {
                    special = thc::SPECIAL_WPAWN_2SQUARES;
                    dst = src-16;
                }
                else
                {
                    dst = src-(lo&3)-7; // 2\,1| or 0/ -> -9,-8 or -7
                    if( !(lo&1) )    // if( 2\ or 0/ )
                    {
                        if( cr.squares[dst]==' ' && cr.squares[SOUTH(dst)]=='p' )
                            special = thc::SPECIAL_WEN_PASSANT;
                    }
                }
                break;
            }
                
            case 'p':
            {
                if( (src&0x38) == 0x30 ) // if a2(48) - h2(55)
                {
                    switch( lo & 0x0c )
                    {
                        case CODE_SPECIAL_PROMOTION_QUEEN:
                        {
                            special = thc::SPECIAL_PROMOTION_QUEEN;
                            break;
                        }
                        case CODE_SPECIAL_PROMOTION_ROOK:
                        {
                            special = thc::SPECIAL_PROMOTION_ROOK;
                            break;
                        }
                        case CODE_SPECIAL_PROMOTION_BISHOP:
                        {
                            special = thc::SPECIAL_PROMOTION_BISHOP;
                            break;
                        }
                        case CODE_SPECIAL_PROMOTION_KNIGHT:
                        {
                            special = thc::SPECIAL_PROMOTION_KNIGHT;
                            break;
                        }
                    }
                    
                }
                if( (lo&3) == 3 )
                {
                    special = thc::SPECIAL_BPAWN_2SQUARES;
                    dst = src+16;
                }
                else
                {
                    dst = src+(lo&3)+7; // 2\,1| or 0/ -> +9,+8 or +7
                    if( !(lo&1) )    // if( 2\ or 0/ )
                    {
                        if( cr.squares[dst]==' ' && cr.squares[NORTH(dst)]=='P' )
                            special = thc::SPECIAL_BEN_PASSANT;
                    }
                }
                break;
            }
        }
    }
    
    mv.src = (thc::Square)src;
    mv.dst = (thc::Square)dst;
    mv.special = special;
    mv.capture = ' ';
    int captured_sq = cr.squares[dst]<'A' ? -1 : dst;
    if( special == thc::SPECIAL_BEN_PASSANT )
        captured_sq = NORTH(dst);
    else if( special == thc::SPECIAL_WEN_PASSANT )
        captured_sq = SOUTH(dst);
    if( captured_sq >= 0 )
    {
        mv.capture = cr.squares[captured_sq];
    }
}

// EXPERIMENTAL
#endif
#if 1

#include <algorithm>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "thc.h"

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

// The simplest possible strategy is to automatically go to slow mode if
//  promoting. The UNDO_LAST_SLOW_MODE_MOVE algorithm anticipates future
//  optimised implementations that avoid going to slow mode for
//  promotions that are immediately captured - or promotions that don't
//  introduce a third knight/rook second queen/light bishop/dark bishop.
//  Because of UNDO_LAST_SLOW_MODE_MOVE those future implementations will
//  generate the same code for the move immediately after promotion as
//  this slow implementation (since the one slow mode move this implementation
//  generates in the simple promotion cases is UNDOne, and replaced with
//  a fast mode move).
 
#define CODE_KING           0x00
#define CODE_KNIGHT_LO      0x10
#define CODE_KNIGHT_HI      0x18
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

#define R_RANK              8          // Rank or file (i.e. rook) codes, this bit set indicates same file (so remaining 3 bits encode rank)
#define R_FILE              0
#define B_FALL              8          // Diagonal (i.e. bishop) codes, either RISE/ or FALL\, other 3 bits encode destination file
#define B_RISE              0

static inline bool is_dark( int sq )
{
    bool dark = (!(sq&8) &&  (sq&1))    // eg (a8,b8,c8...h8) && (b8|d8|f8|h8)
             || ( (sq&8) && !(sq&1));   // eg (a7,b7,c7...c7) && (a7|c7|e7|g7)
    return dark;
}

struct Side
{
    bool white;
    bool fast_mode;
    int rooks[2];       // locations of each dynamic piece
    int knights[2];     //
    int pawns[8];
    int nbr_pawns;      // 0-8
    int nbr_rooks;      // 0,1 or 2
    int nbr_knights;    // 0,1 or 2
    int nbr_light_bishops;
    int nbr_dark_bishops;
    int nbr_queens;
};

class CompressMoves
{
public:
    CompressMoves()
    {
        sides[0].white=true;
        sides[1].white=false;
    }
    bool TryFastMode( Side *side );
    std::string Compress( std::vector<thc::Move> &moves_in );
    std::string Compress( thc::ChessPosition &cp, std::vector<thc::Move> &moves_in );
    std::vector<thc::Move> Uncompress( std::string &moves_in );
    std::vector<thc::Move> Uncompress( thc::ChessPosition &cp, std::string &moves_in );
        
private:
    thc::ChessRules cr;
    Side sides[2];
    char CompressFastMode( thc::Move mv, Side *side, Side *other );
    char CompressSlowMode( thc::Move mv );

};

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
    for( int i=0; i<64; i++ )
    {
        int j = static_cast<int>(traverse_order[i]);
        if( cr.squares[j] == (side->white?'P':'p') )
        {
            if( side->nbr_pawns < 8 )
                side->pawns[side->nbr_pawns++] = j;
            else
                okay = false;
        }
        if( cr.squares[i] == (side->white?'R':'r') )
        {
            if( side->nbr_rooks < 2 )
                side->rooks[side->nbr_rooks++] = i;
            else
                okay = false;
        }
        if( cr.squares[i] == (side->white?'N':'n') )
        {
            if( side->nbr_knights < 2 )
                side->knights[side->nbr_knights++] = i;
            else
                okay = false;
        }
        if( cr.squares[i] == (side->white?'B':'b') )
        {
            if( is_dark(i) )
            {
                if( side->nbr_dark_bishops < 1 )
                    side->nbr_dark_bishops++;
                else
                    okay = false;
            }
            else
            {
                if( side->nbr_light_bishops < 1 )
                    side->nbr_light_bishops++;
                else
                    okay = false;
            }
        }
        if( cr.squares[j] == (side->white?'Q':'q') )
        {
            if( side->nbr_queens < 1 )
                side->nbr_queens++;
            else
                okay = false;
        }
    }
    side->fast_mode = okay;
    return okay;
}
    
std::string CompressMoves::Compress( std::vector<thc::Move> &moves_in )
{
    thc::ChessPosition cp;
    return CompressMoves::Compress( cp, moves_in );
}


std::string CompressMoves::Compress( thc::ChessPosition &cp, std::vector<thc::Move> &moves_in )
{
    std::string ret;
    cr = cp;
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

// A slow method of compressing a move into one byte
//  Scheme is;
//  1) Make a list of all legal moves in standard SAN text format, sorted alphabetically
//  2) Find the move to be compressed in the list
//  3) Encode the move as one byte, value is 255 - list index (to avoid code 0, '\n' etc)
//
//  Why use slow text representation and sorting ? So that a specification for the
//  whole compression scheme can be published if necessary, without relying on implementation
//  details like the order that moves come out of the move generator etc.
//
//  According to http://www.chess.com/forum/view/fun-with-chess/what-chess-position-has-the-most-number-of-possible-moves
//  the maximum number of legal moves available in any position is less than 256,
//  in fact it is 218 in the following position;
//
//  "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1"
//


char CompressMoves::CompressSlowMode( thc::Move mv )
{
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
        std::string s = moves[i].NaturalOut(&cr);
        moves_alpha.push_back(s);
    }
    std::sort( moves_alpha.begin(), moves_alpha.end() );
        
    // Find this move in the list
    std::string the_move = mv.NaturalOut(&cr);
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
    // '\x01 is an error
    code = 255-idx;
    if( code < 1 )
        code = 1;
    return static_cast<char>(code);
}


char CompressMoves::CompressFastMode( thc::Move mv, Side *side, Side *other )
{
    int src = static_cast<int>(mv.src);
    int dst = static_cast<int>(mv.dst);
    int capture_location = dst;
    char piece = cr.squares[mv.src];
    bool making_capture = (isalpha(cr.squares[capture_location]) ? true : false);
    bool promoting = false;
    int  code=0;
    switch( tolower(piece) )
    {
        case 'k':
        {
            switch(dst-src)          // 0, 1, 2
            {                        // 8, 9, 10
                                     // 16,17,18
                case -9:    code = CODE_KING + K_VECTOR_NE;    break;  // 0-9
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
                    break;
                }
                case -2:
                {
                    code = CODE_KING + K_Q_CASTLING;
                    int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                    side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                    // note that there is no way the rooks ordering can swap during castling
                    break;
                }
            }
            break;
        }
            
        case 'r':
        {
            int rook_offset = (side->rooks[0]==src ? 0 : 1);
            code = (rook_offset==0 ? CODE_ROOK_LO : CODE_ROOK_HI);
			if( (src&7) == (dst&7) )                // same file ?
			{
                code = code + R_RANK + ((dst>>3)&7);    // encode rank
			}
			else if( (src&0x38) == (dst&0x38) )     // same rank ?
			{
				code = code + R_FILE + (dst&7);         // encode file
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
            
        case 'b':
        {
            code = is_dark(src) ? CODE_BISHOP_DARK : CODE_BISHOP_LIGHT;
			int abs = (src>dst ? src-dst : dst-src);
			if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
				code = code + B_FALL + (dst&7); // fall( = \) + dst file
			else
				code = code + B_RISE + (dst&7); // rise( = /) + dst file
            break;
        }
            
        case 'q':
        {
			if( (src&7) == (dst&7) )                        // same file ?
			{
                code = CODE_QUEEN_ROOK + R_RANK + ((dst>>3)&7); // encode rank
			}
			else if( (src&0x38) == (dst&0x38) )             // same rank ?
			{
				code = CODE_QUEEN_ROOK + R_FILE + (dst&7);      // encode file
			}
			else
			{
				int abs = (src>dst ? src-dst : dst-src);
				if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
					code = CODE_QUEEN_BISHOP + B_FALL + (dst&7); // fall( = \) + dst file
				else
					code = CODE_QUEEN_BISHOP + B_RISE + (dst&7); // rise( = /) + dst file
			}
            break;
        }
            
        case 'n':
        {
            int knight_offset = (side->knights[0]==src ? 0 : 1);
            code = (knight_offset==0 ? CODE_KNIGHT_LO : CODE_KNIGHT_HI );
            switch(dst-src)         // 0, 1, 2, 3
            {                       // 8, 9, 10,11
                                    // 16,17,18,19
                                    // 24,25,26,27
                case -15:   code = code + N_VECTOR_NNE;   // 2-17
                case -6:    code = code + N_VECTOR_NEE;   // 11-17
                case 10:    code = code + N_VECTOR_SEE;   // 27-17
                case 17:    code = code + N_VECTOR_SSE;   // 27-10
                case 15:    code = code + N_VECTOR_SSW;   // 25-10
                case 6:     code = code + N_VECTOR_SWW;   // 16-10
                case -10:   code = code + N_VECTOR_NWW;   // 0-10
                case -17:   code = code + N_VECTOR_NNW;   // 0-17
            }
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
            
        case 'p':
        {
            int positive_delta = cr.white ? src-dst : dst-src;
            int pawn_offset = 0;
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

            if( !promoting )
            {
                bool reordering_possible = (code==P_LEFT || code==P_RIGHT);
                if( !reordering_possible )
                {
                    int *p = side->pawns;
                    bool found=false;
                    for( int i=0; !found && i<side->nbr_pawns; i++,p++ )
                    {
                        if( *p == src )
                        {
                            found = true;
                            pawn_offset = i;
                            *p = dst;
                        }
                    }
                }
                else
                {
                    if( pawn_ordering[dst] > pawn_ordering[src] ) // increasing capture?
                    {
                        int *p = side->pawns;
                        bool found=false;
                        for( int i=0; !found && i<side->nbr_pawns; i++,p++ )
                        {
                            if( *p == src )
                            {
                                found = true;
                                pawn_offset = i;
                                *p = dst;
                                while( i+1<side->nbr_pawns && pawn_ordering[side->pawns[i]]>pawn_ordering[side->pawns[i+1]] )
                                {
                                    int temp = side->pawns[i];
                                    side->pawns[i] = side->pawns[i+1];
                                    side->pawns[i+1] = temp;
                                    i++;
                                }
                            }
                        }
                    }
                    else // else decreasing capture
                    {
                        int *p = side->pawns + side->nbr_pawns-1;
                        bool found=false;
                        for( int i=side->nbr_pawns-1; !found && i>=0; i--, p-- )
                        {
                            if( *p == src )
                            {
                                found = true;
                                pawn_offset = i;
                                *p = dst;
                                while( i-1>=0 && pawn_ordering[side->pawns[i-1]]>pawn_ordering[side->pawns[i]] )
                                {
                                    int temp = side->pawns[i-1];
                                    side->pawns[i-1] = side->pawns[i];
                                    side->pawns[i] = temp;
                                    i--;
                                }
                            }
                        }
                    }
                }
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
                other->nbr_queens--;
                break;
            }
            case 'p':
            {
                bool found=false;
                for( int i=0; i<other->nbr_pawns; i++ )
                {
                    if( !found )
                    {
                        if( other->pawns[i] == capture_location )
                            found = true;
                    }
                    if( found && i+1<other->nbr_pawns )
                        other->pawns[i] = other->pawns[i+1];
                }
            }
        }
    }

    // If promoting piece, we haven't accommodated our pawn disappearing, or a
    //  a new piece appearing in its place. Nor have we accommodated the possibility
    //  that we cannot remain in fast mode because we now have too many queens or
    //  other pieces. So simply force this side to reset and retry entry to fast
    //  mode next time, if that fails this side will generate slow mode moves.
    if( promoting )
        side->fast_mode = false;

    return static_cast<char>(code);
}

std::vector<thc::Move> CompressMoves::Uncompress( std::string &moves_in )
thc::Move CompressMoves::UncompressFastMode( char code, Side *side, Side *other )
{
    int src = static_cast<int>(mv.src);
    int dst = static_cast<int>(mv.dst);
    int capture_location = dst;
    char piece = cr.squares[mv.src];
    bool making_capture = (isalpha(cr.squares[capture_location]) ? true : false);
    bool promoting = false;
    int  code=0;
    switch( code & 0xf0 )
    {
        case CODE_KING:
        {
            switch(dst-src)          // 0, 1, 2
            {                        // 8, 9, 10
                                     // 16,17,18
                case K_VECTOR_NE:    delta = -9; break;  // 0-9
                case K_VECTOR_N:     delta = -8; break;  // 1-9
                case K_VECTOR_NE:    delta = -7; break;  // 2-9
                case K_VECTOR_W:     delta = -1; break;  // 8-9
                case K_VECTOR_E:     delta =  1; break;  // 10-9
                case K_VECTOR_SW:    delta =  7; break;  // 16-9
                case K_VECTOR_S:     delta =  8; break;  // 17-9
                case K_VECTOR_SE:    delta =  9; break;  // 18-9
                case K_K_CASTLING:
                {
                    delta = 2;
                    code = CODE_KING + K_K_CASTLING;
                    int rook_offset = (side->rooks[0]==src+3 ? 0 : 1);  // a rook will be 3 squares to right of king
                    side->rooks[rook_offset] = src+1;                   // that rook ends up 1 square right of king
                    // note that there is no way the rooks ordering can swap during castling
                    break;
                }
                case K_Q_CASTLING:
                {
                    delta = -2;
                    code = CODE_KING + K_Q_CASTLING;
                    int rook_offset = (side->rooks[0]==src-4 ? 0 : 1);  // a rook will be 4 squares to left of king
                    side->rooks[rook_offset] = src-1;                   // that rook ends up 1 square left of king
                    // note that there is no way the rooks ordering can swap during castling
                    break;
                }
            }
            int dst = src+delta;
            
            break;
        }
            
        case CODE_ROOK_LO:
        case CODE_ROOK_HI:
        {
            int rook_offset = (side->rooks[0]==src ? 0 : 1);
            code = (rook_offset==0 ? CODE_ROOK_LO : CODE_ROOK_HI);
			if( (src&7) == (dst&7) )                // same file ?
			{
                code = code + R_RANK + ((dst>>3)&7);    // encode rank
			}
			else if( (src&0x38) == (dst&0x38) )     // same rank ?
			{
				code = code + R_FILE + (dst&7);         // encode file
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
        case CODE_BISHOP_LIGHT:
        {
            code = is_dark(src) ? CODE_BISHOP_DARK : CODE_BISHOP_LIGHT;
			int abs = (src>dst ? src-dst : dst-src);
			if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
				code = code + B_FALL + (dst&7); // fall( = \) + dst file
			else
				code = code + B_RISE + (dst&7); // rise( = /) + dst file
            break;
        }
            
        case CODE_QUEEN_ROOK:
        case CODE_QUEEN_BISHOP:
        {
			if( (src&7) == (dst&7) )                        // same file ?
			{
                code = CODE_QUEEN_ROOK + R_RANK + ((dst>>3)&7); // encode rank
			}
			else if( (src&0x38) == (dst&0x38) )             // same rank ?
			{
				code = CODE_QUEEN_ROOK + R_FILE + (dst&7);      // encode file
			}
			else
			{
				int abs = (src>dst ? src-dst : dst-src);
				if( abs%9 == 0 )  // do 9 first, as LCD of 9 and 7 is 63, i.e. diff between a8 and h1, a FALL\ diagonal
					code = CODE_QUEEN_BISHOP + B_FALL + (dst&7); // fall( = \) + dst file
				else
					code = CODE_QUEEN_BISHOP + B_RISE + (dst&7); // rise( = /) + dst file
			}
            break;
        }
            
        case CODE_KNIGHT:
        {
            int knight_offset = (side->knights[0]==src ? 0 : 1);
            code = (knight_offset==0 ? CODE_KNIGHT_LO : CODE_KNIGHT_HI );
            switch(dst-src)         // 0, 1, 2, 3
            {                       // 8, 9, 10,11
                                    // 16,17,18,19
                                    // 24,25,26,27
                case -15:   code = code + N_VECTOR_NNE;   // 2-17
                case -6:    code = code + N_VECTOR_NEE;   // 11-17
                case 10:    code = code + N_VECTOR_SEE;   // 27-17
                case 17:    code = code + N_VECTOR_SSE;   // 27-10
                case 15:    code = code + N_VECTOR_SSW;   // 25-10
                case 6:     code = code + N_VECTOR_SWW;   // 16-10
                case -10:   code = code + N_VECTOR_NWW;   // 0-10
                case -17:   code = code + N_VECTOR_NNW;   // 0-17
            }
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
            
        case 'p':
        {
            int positive_delta = cr.white ? src-dst : dst-src;
            int pawn_offset = 0;
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

            if( !promoting )
            {
                bool reordering_possible = (code==P_LEFT || code==P_RIGHT);
                if( !reordering_possible )
                {
                    int *p = side->pawns;
                    bool found=false;
                    for( int i=0; !found && i<side->nbr_pawns; i++,p++ )
                    {
                        if( *p == src )
                        {
                            found = true;
                            pawn_offset = i;
                            *p = dst;
                        }
                    }
                }
                else
                {
                    if( pawn_ordering[dst] > pawn_ordering[src] ) // increasing capture?
                    {
                        int *p = side->pawns;
                        bool found=false;
                        for( int i=0; !found && i<side->nbr_pawns; i++,p++ )
                        {
                            if( *p == src )
                            {
                                found = true;
                                pawn_offset = i;
                                *p = dst;
                                while( i+1<side->nbr_pawns && pawn_ordering[side->pawns[i]]>pawn_ordering[side->pawns[i+1]] )
                                {
                                    int temp = side->pawns[i];
                                    side->pawns[i] = side->pawns[i+1];
                                    side->pawns[i+1] = temp;
                                    i++;
                                }
                            }
                        }
                    }
                    else // else decreasing capture
                    {
                        int *p = side->pawns + side->nbr_pawns-1;
                        bool found=false;
                        for( int i=side->nbr_pawns-1; !found && i>=0; i--, p-- )
                        {
                            if( *p == src )
                            {
                                found = true;
                                pawn_offset = i;
                                *p = dst;
                                while( i-1>=0 && pawn_ordering[side->pawns[i-1]]>pawn_ordering[side->pawns[i]] )
                                {
                                    int temp = side->pawns[i-1];
                                    side->pawns[i-1] = side->pawns[i];
                                    side->pawns[i] = temp;
                                    i--;
                                }
                            }
                        }
                    }
                }
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
                other->nbr_queens--;
                break;
            }
            case 'p':
            {
                bool found=false;
                for( int i=0; i<other->nbr_pawns; i++ )
                {
                    if( !found )
                    {
                        if( other->pawns[i] == capture_location )
                            found = true;
                    }
                    if( found && i+1<other->nbr_pawns )
                        other->pawns[i] = other->pawns[i+1];
                }
            }
        }
    }

    // If promoting piece, we haven't accommodated our pawn disappearing, or a
    //  a new piece appearing in its place. Nor have we accommodated the possibility
    //  that we cannot remain in fast mode because we now have too many queens or
    //  other pieces. So simply force this side to reset and retry entry to fast
    //  mode next time, if that fails this side will generate slow mode moves.
    if( promoting )
        side->fast_mode = false;

    return static_cast<char>(code);
}



#endif
