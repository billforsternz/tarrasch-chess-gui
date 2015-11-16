/****************************************************************************
 * Control - The graphic chess board that is the centre-piece of the GUI
 *  Still working on making it a more-self contained and reusable control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/image.h"
#include "wx/rawbmp.h"
#include "wx/file.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "GraphicBoard.h"
#include "GameLogic.h"
#include "thc.h"
#include "Objects.h"
#include "BoardBitmap40.h"
#include "BoardBitmap54.h"
using namespace std;
using namespace thc;


// Initialise the graphic board
GraphicBoard::GraphicBoard
(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& point,
    const wxSize& size,
    int nbr_pixels
)   : wxControl( parent, id, point, size, BORDER_COMMON )
{
    pickup_file    = 0;
    pickup_rank    = 0;
    pickup_point.x = 0;
    pickup_point.y = 0;
    sliding = false;
    BoardBitmap40 bm1;
    BoardBitmap54 bm2;
    BoardBitmap   *bm;
    if( nbr_pixels == 40 )
        bm = &bm1;
    else
        bm = &bm2;
    white_king_mask   = bm->GetWhiteKingMask();
    white_queen_mask  = bm->GetWhiteQueenMask();
    white_knight_mask = bm->GetWhiteKnightMask();
    white_bishop_mask = bm->GetWhiteBishopMask();
    white_rook_mask   = bm->GetWhiteRookMask();
    white_pawn_mask   = bm->GetWhitePawnMask();
    black_king_mask   = bm->GetBlackKingMask();
    black_queen_mask  = bm->GetBlackQueenMask();
    black_knight_mask = bm->GetBlackKnightMask();
    black_bishop_mask = bm->GetBlackBishopMask();
    black_rook_mask   = bm->GetBlackRookMask();
    black_pawn_mask   = bm->GetBlackPawnMask();

    my_chess_bmp = wxBitmap( bm->GetXpm() );

	// Orientation
	normal_orientation = true;

	// Highlights squares
	ClearHighlight1();
	ClearHighlight2();

#ifdef MAC_FIX_LATER
    
    height = my_chess_bmp.GetHeight();
	width_bytes  = 512;   // bytes
	width        = 128;        // pixels
  	assert( (width_bytes%width) == 0 );
	density      = width_bytes/width;
	height       = 128;
    wxAlphaPixelData bmdata(my_chess_bmp);
    height = bmdata.GetHeight();
    width  = bmdata.GetWidth();
    wxPoint x = bmdata.GetOrigin();
    wxSize  z = bmdata.GetSize();
    int row_stride = bmdata.GetRowStride();
    cprintf( "x=%d,%d z=%d,%d, height=%d, width=%d, row_stride=%d\n",x.x,x.y,z.x,z.y,height,width,row_stride);
    width_bytes = row_stride;
#else
	BITMAP info;

	// Get dimensions of the wxBitmap
	//my_chess_bmp.GetBitmap( &info ); // return ::GetObject(m_hObject, sizeof(BITMAP), pBitMap);
    ::GetObject( my_chess_bmp.GetHBITMAP(), sizeof(BITMAP), &info ); //@@
    //this->board_rect = board_rect;
	width_bytes  = info.bmWidthBytes;   // bytes
	width        = info.bmWidth;        // pixels
  	assert( (info.bmWidthBytes%info.bmWidth) == 0 );
	density      = info.bmWidthBytes/info.bmWidth;
	height       = info.bmHeight;
  	dbg_printf( "width_bytes=%lu, width=%lu, height=%lu\n",
						(unsigned long)info.bmWidthBytes,
						(unsigned long)info.bmWidth,
						(unsigned long)info.bmHeight );
#endif
	xborder	     = (width_bytes%8) / 2;
	yborder	     = (height%8) / 2;

	// Allocate an image of the board
	buf_board = new byte[width_bytes*height];
    memset( buf_board, 0, width_bytes*height);
	
	// Allocate an image of the box (pieces off the board)
	buf_box   = new byte[width_bytes*height];

	// Read the initial position displayed on the bitmap
#ifdef MAC_FIX_LATER
    wxAlphaPixelData::Iterator p(bmdata);
    byte *dst = buf_board;
    for( int i=0; i<height*width; i++ )
    {
        *dst++ = p.Alpha();
        *dst++ = p.Red();
        *dst++ = p.Green();
        *dst++ = p.Blue();
        p++;
    }
    cprintf("a,r,g,b = %d,%d,%d,%d\n", buf_board[0], buf_board[1], buf_board[2], buf_board[3] );
#else
	//my_chess_bmp.GetBitmapBits( width_bytes*height, buf_board );   //@@
    int ret= ::GetBitmapBits((HBITMAP)(my_chess_bmp.GetHBITMAP()), width_bytes*height, buf_board );
    //dbg_printf( "::GetBitmapBits() returns %d\n",ret);
#endif

	// Read from position on left below (the .bmp resource) into the box
	//  at right. The box has all the piece/colour combinations we need
    //  (eg only two pawns from each side are needed, one of each colour).
    //  Also note that the two squares marked with * are the two empty
	//	squares (one of each colour) we need.
	//
	//	"rNbQkBnR"		 "rnbqkbnr"
	//	"Pp*qK*  "		 "pp      "
	//	"        "		 "        "
	//	"        "		 "   qk   "
	//	"        "		 "   QK   "
	//	"        "		 "**      "
	//	"        "		 "PP      "
	//	"        "		 "RNBQKBNR"
	//
	// Store these pieces from the board to their fixed
	//  positions in the 'box'
	Get( 'c','7', 'b','8' );  // preload black squares in the box
	Get( 'c','7', 'd','8' );
	Get( 'c','7', 'f','8' );
	Get( 'c','7', 'h','8' );
	Get( 'c','7', 'a','7' );
	Get( 'c','7', 'e','5' );
	Get( 'c','7', 'd','4' );
	Get( 'c','7', 'b','2' );
	Get( 'c','7', 'a','1' );
	Get( 'c','7', 'c','1' );
	Get( 'c','7', 'e','1' );
	Get( 'c','7', 'g','1' );
	Get( 'f','7', 'a','8' );  // preload white squares in the box
	Get( 'f','7', 'c','8' );
	Get( 'f','7', 'e','8' );
	Get( 'f','7', 'g','8' );
	Get( 'f','7', 'b','7' );
	Get( 'f','7', 'd','5' );
	Get( 'f','7', 'e','4' );
	Get( 'f','7', 'a','2' );
	Get( 'f','7', 'b','1' );
	Get( 'f','7', 'd','1' );
	Get( 'f','7', 'f','1' );
	Get( 'f','7', 'h','1' );
    
	Get( 'h','8', 'h','1', white_rook_mask );
	Get( 'b','8', 'g','1', white_knight_mask );
	Get( 'f','8', 'f','1', white_bishop_mask );
	Get( 'e','7', 'e','1', white_king_mask );
	Get( 'd','8', 'd','1', white_queen_mask );
	Get( 'f','8', 'c','1', white_bishop_mask );
    Get( 'b','8', 'b','1', white_knight_mask );
	Get( 'h','8', 'a','1', white_rook_mask );
	
	Get( 'a','7', 'a','2', white_pawn_mask );    // Only need pawns on a2, b2
	Get( 'a','7', 'b','2', white_pawn_mask );

	Get( 'b','7', 'a','7', black_pawn_mask );    // Only need pawns on a7, b7
	Get( 'b','7', 'b','7', black_pawn_mask );

	Get( 'a','8', 'h','8', black_rook_mask );
	Get( 'g','8', 'g','8', black_knight_mask );
	Get( 'c','8', 'f','8', black_bishop_mask );
	Get( 'e','8', 'e','8', black_king_mask );
	Get( 'd','7', 'd','8', black_queen_mask );
	Get( 'c','8', 'c','8', black_bishop_mask );
    Get( 'g','8', 'b','8', black_knight_mask );
	Get( 'a','8', 'a','8', black_rook_mask );

	Get( 'c','7', 'a','3' );  // empty black square
	Get( 'f','7', 'b','3' );  // empty white square
//	Get( 'c','3', 'c','3' );  // square with cross 
//	Get( 'd','3', 'd','3' );  // square with cross 
    
	Get( 'e','8', 'e','5', black_king_mask  );  // Black king on black square
	Get( 'e','7', 'e','4', white_king_mask  );  // White king on white square
	Get( 'd','7', 'd','5', black_queen_mask );  // Black queen on white square
	Get( 'd','8', 'd','4', white_queen_mask );  // White queen on black square
}

// Cleanup
GraphicBoard::~GraphicBoard()
{
	delete(buf_board);
	delete(buf_box);
}

void GraphicBoard::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if( my_chess_bmp.Ok() )
    {
        dc.DrawBitmap( my_chess_bmp, 0, 0, true );// board_rect.x, board_rect.y, true );
    }
}

// Setup a position	on the graphic board
void GraphicBoard::SetPosition( char *position_ascii )
{
#if 0 // Setup a custom position here
    #define CUSTOM_POSITION "rnbqnrk|pp4b|4N|3qk|3QK|8|PPxx|R1BQKBNR| w KQkq - 0 1"
    ChessPosition pos;
    pos.Forsyth( CUSTOM_POSITION );
    position_ascii = pos.squares;
#endif

	char piece;
	int  file=0, rank=7;
	char rev_file='h', rev_rank='1';	// tracks reverse orientation
	char src_file, src_rank;
	char dst_file, dst_rank;
	const char *colour = "wbwbwbwb"
				   "bwbwbwbw"
				   "wbwbwbwb"
				   "bwbwbwbw"
				   "wbwbwbwb"
				   "bwbwbwbw"
				   "wbwbwbwb"
				   "bwbwbwbw";
    strcpy( _position_ascii, position_ascii );

	// Read string backwards for black at bottom
	if( !normal_orientation )
		position_ascii += 63;

	// Loop through all squares
	for( int i=0; i<64; i++ )
	{
		dst_file='a'+file;
		dst_rank='1'+rank;
		file++;
		rev_file--;
		if( file == 8 )
		{
			file=0;
			rank--;
			rev_file='h';
			rev_rank++;
		}

		// Find the piece occupying this square
		if( normal_orientation )
			piece = *position_ascii++;
		else
			piece = *position_ascii--;

		// Is the square black ?
		if( *colour++ == 'b' )
		{

			// Find an appropriate black square piece in the box
			switch(piece)
			{
				case 'R':	src_file='a';	src_rank='1';	break;
				case 'N':	src_file='g';	src_rank='1';	break;
				case 'B':	src_file='c';	src_rank='1';	break;
				case 'Q':	src_file='d';	src_rank='4';	break;
				case 'K':	src_file='e';	src_rank='1';	break;
				case 'P':   src_file='b';	src_rank='2';	break;
				case 'r':	src_file='h';	src_rank='8';	break;
				case 'n':   src_file='b';	src_rank='8';	break;
				case 'b':   src_file='f';	src_rank='8';	break;
				case 'q':   src_file='d';	src_rank='8';	break;
				case 'k':   src_file='e';	src_rank='5';	break;
				case 'p':   src_file='a';	src_rank='7';	break;
                case 'x':   src_file='c';   src_rank='3';   break;
				default:	src_file='a';	src_rank='3';	break;
			}
		}
		else
		{
			
			// Find an appropriate white square piece in the box
			switch(piece)
			{
				case 'R':	src_file='h';	src_rank='1';	break;
				case 'N':	src_file='b';	src_rank='1';	break;
				case 'B':	src_file='f';	src_rank='1';	break;
				case 'Q':	src_file='d';	src_rank='1';	break;
				case 'K':	src_file='e';	src_rank='4';	break;
				case 'P':   src_file='a';	src_rank='2';	break;
				case 'r':	src_file='a';	src_rank='8';	break;
				case 'n':   src_file='g';	src_rank='8';	break;
				case 'b':   src_file='c';	src_rank='8';	break;
				case 'q':   src_file='d';	src_rank='5';	break;
				case 'k':   src_file='e';	src_rank='8';	break;
				case 'p':   src_file='b';	src_rank='7';	break;
                case 'x':   src_file='d';   src_rank='3';   break;
				default:	src_file='b';	src_rank='3';	break;
			}
		}

	    Put( src_file, src_rank, dst_file, dst_rank );
//        if( is_black_square )
//        {
//            Put( 'a', '3', dst_file, dst_rank );    //@ preload black square
//            CPoint no_shift(0,0);
//            if( src_file!='a' || src_rank!='3' )
//                PutEx( piece, dst_file, dst_rank, no_shift );
//        }
//        else
//        {


		    // Copy from the box into the image buffer of the wxBitmap
//		    Put( src_file, src_rank, dst_file, dst_rank );
//        }


	}

	// Copy from the image buffer into the wxBitmap
#ifdef MAC_FIX_LATER
    wxAlphaPixelData bmdata(my_chess_bmp);
    wxAlphaPixelData::Iterator p(bmdata);
    byte *src = buf_board;
    for( int i=0; i<height*width; i++ )
    {
        p.Alpha() = *src++;
        p.Red()   = *src++;
        p.Green() = *src++;
        p.Blue()  = *src++;
        p++;
    }
#else
	//my_chess_bmp.SetBitmapBits( width_bytes*height, buf_board );
    /*int ret = ::*/SetBitmapBits( (HBITMAP)(my_chess_bmp.GetHBITMAP()), width_bytes*height, buf_board );  //@@
    //dbg_printf( "::SetBitmapBits() returns %d\n", ret );
#endif

    // Now use GDI to add highlights
    int row, col, x, y;
    int square_width = width/8;
    int square_height= height/8;
    char highlight_file = highlight_file1;
    char highlight_rank = highlight_rank1;
    bool is_highlight = false;
    //highlight_file = 'f';
    //highlight_rank = '4';
    for( int i=0; i<2; i++ )
    {
        if( highlight_file )
        {
            if( normal_orientation )
            {
                col = highlight_file-'a';       // 'a'->0, 'b'->1 .. 'h'->7
                row = 7-(highlight_rank-'1');   // '1'->7, '2'->6 .. '8'->0
            }
            else
            {
                col = 7-(highlight_file-'a');   // 'h'->0, 'g'->1 .. 'a'->7
                row = highlight_rank-'1';       // '1'->0, '2'->1 .. '8'->7
            }
            x = col*square_width;
            y = row*square_height;
            if( !is_highlight )
            {
       	        /*@@restore = */dcmem.SelectObject( my_chess_bmp );
                is_highlight = true;
            }
            wxPoint rect[5];
            rect[0].x = x;
            rect[0].y = y;
            rect[1].x = x + square_width-1;
            rect[1].y = y;
            rect[2].x = x + square_width-1;
            rect[2].y = y + square_height-1;
            rect[3].x = x;
            rect[3].y = y + square_height-1;
            rect[4].x = x;
            rect[4].y = y;
            dcmem.DrawLines( 4, rect );
        }
        highlight_file = highlight_file2;
        highlight_rank = highlight_rank2;
    }
    //@@ if( is_highlight )
    //@@    dcmem.SelectObject( *restore );
}


// Draw the graphic board
void GraphicBoard::Draw()
{
    Refresh(false);
    Update();
}

// Calculate an offset into the wxBitmap's image buffer
unsigned long GraphicBoard::Offset( char file, char rank )
{
	byte col  = file-'a';		     // 0-7 => file a-h
	byte row  = 7-(rank-'1');		 // 0-7 => rank 8-1
	unsigned long offset =						
		yborder * width_bytes
      + ( row * (height/8) ) * width_bytes
	  +	( xborder + col * (width_bytes/8) );
	return( offset );	
}

// Get a piece from board, put into box
void GraphicBoard::Get( char src_file, char src_rank, char dst_file, char dst_rank, const char *mask )
{
	unsigned int i, j;
	byte *src, *dst;
	for( i=0; i < height/8; i++ )
	{
		src = buf_board + Offset(src_file,src_rank) + i*width_bytes;
		dst = buf_box   + Offset(dst_file,dst_rank) + i*width_bytes;
        unsigned int density_idx = 0;
		for( j=0; j < width_bytes/8; j++ )
        {
            if( !mask )
			    *dst++ = *src++;
            else
            {
                if( *mask == '1' )
			        *dst = *src;
                dst++;
                src++;
                density_idx++;
                if( density_idx >= density )
                {
                    density_idx = 0;
                    mask++;
                }    
            }
        }
	}
}

// Put a piece from box onto board
void GraphicBoard::Put( char src_file, char src_rank, char dst_file, char dst_rank )
{
	if( density != 4 )
	{

		// 16 bit (and other) graphics code
		unsigned int i, j, k;
		byte *src, *dst;
		for( i=0; i < height/8; i++ )
		{
			src			  =
				(byte *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			dst			  =
				(byte *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
			for( j=0; j < width_bytes/(8*density); j++ )
            {
                for( k=0; k<density; k++ )
			        *dst++ = *src++;
            }
		}
	}
	else
	{

		// 32 bit graphics code
		unsigned int i, j;
		uint32_t *src, *dst;
		for( i=0; i < height/8; i++ )
		{
			src			  =
				(uint32_t *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			dst			  =
				(uint32_t *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
			for( j=0; j < width_bytes/32; j++ )
			    *dst++ = *src++;
		}
	}
}


// Figure out which square is clicked on the board
void GraphicBoard::HitTest( wxPoint hit, char &file, char &rank )
{
	unsigned long row = ( (hit.y   - yborder) / (height/8) );
	unsigned long col = ( (hit.x*density - xborder) / (width_bytes/8) );
    //dbg_printf( "Hit test: x=%ld y=%ld\n", hit.x, hit.y );
	if( normal_orientation )
	{
		rank = '1'+ (int)(7-row);
		file = 'a'+ (int)col;
	}
	else
	{
		file = 'a'+ (int)(7-col);
		rank = '1'+ (int)row;
	}
}

// Figure out which square is at shift offset from this square
void GraphicBoard::HitTestEx( char &file, char &rank, wxPoint shift )
{
	long row, y, col, x;
	if( normal_orientation )
	{
		row = 7 - (rank-'1');
		col = file - 'a';
	}
	else
	{
		row = rank-'1';
		col = 7 - (file-'a');
	}
    y = row*(height/8) + yborder;
	x = (col*(width_bytes/8) + xborder ) / density;
    //dbg_printf( "Ex, shift:     x=%ld y=%ld\n", shift.x, shift.y );
    //dbg_printf( "Ex, src raw:   x=%ld y=%ld\n", x, y );
    y += height/16;
	x += (width_bytes/16) / density;
    //dbg_printf( "Ex  src+half:  x=%ld y=%ld\n", x, y );
    y += shift.y;
	x += shift.x;
    //dbg_printf( "Ex  src+shift: x=%ld y=%ld\n", x, y );
    if( y < (int)((height/16) + yborder) )
        y = ((height/16) + yborder);
    else if( y > (int)((height/16) + 7*(height/8) + yborder) )
        y = ((height/16) + 7*(height/8) + yborder);
	if( x < (int)((width_bytes/16 + xborder )/density) )
	    x = (int)((width_bytes/16 + xborder )/density);
	else if( x > (int)(( width_bytes/16 + 7*(width_bytes/8) + xborder ) / density ) )
	    x = (int)( (width_bytes/16 + 7*(width_bytes/8) + xborder ) / density );
    //dbg_printf( "Ex  src+shift constrained: x=%ld y=%ld\n", x, y );
    wxPoint adjusted;
    adjusted.x = x;
    adjusted.y = y;
    HitTest( adjusted, file, rank );
    //dbg_printf( "Ex result: file=%c rank=%c\n", file, rank );
}


// Setup a position	on the graphic board
void GraphicBoard::SetPositionEx( ChessPosition pos, bool blank_other_squares, char pickup_file, char pickup_rank, wxPoint shift )
{
	char piece, save_piece=0;
	int  file=0, rank=7;
	char rev_file='h', rev_rank='1';	// tracks reverse orientation
	char src_file, src_rank;
	char dst_file, dst_rank;
    char pickup2_file=0, pickup2_rank=0;
	const char *colour = "wbwbwbwb"
				   "bwbwbwbw"
				   "wbwbwbwb"
				   "bwbwbwbw"
				   "wbwbwbwb"
				   "bwbwbwbw"
				   "wbwbwbwb"
				   "bwbwbwbw";

	// Read string backwards for black at bottom
    char *position_ascii = pos.squares; //_position_ascii;
	if( !normal_orientation )
		position_ascii += 63;

	// Loop through all squares
	for( int i=0; i<64; i++ )
	{
		dst_file='a'+file;
		dst_rank='1'+rank;

		// Find the piece occupying this square
		if( normal_orientation )
			piece = *position_ascii++;
		else
			piece = *position_ascii--;

        // Is this square highlighted ? Does it contain a picked up piece ?
		if( normal_orientation )
		{
            if( dst_file==pickup_file && dst_rank==pickup_rank )
            {
                save_piece = piece;
                piece = ' ';
                pickup2_file = dst_file;
                pickup2_rank = dst_rank;
            }
            if( blank_other_squares )
                piece = ' ';
		}
		else
		{
            if( rev_file==pickup_file && rev_rank==pickup_rank )
            {
                save_piece = piece;
                piece = ' ';
                pickup2_file = dst_file;
                pickup2_rank = dst_rank;
            }
            if( blank_other_squares )
                piece = ' ';
		}
		file++;
		rev_file--;
		if( file == 8 )
		{
			file=0;
			rank--;
			rev_file='h';
			rev_rank++;
		}

		// Is the square black ?
		if( *colour++ == 'b' )
		{

			// Find an appropriate black square piece in the box
			switch(piece)
			{
				case 'R':	src_file='a';	src_rank='1';	break;
				case 'N':	src_file='g';	src_rank='1';	break;
				case 'B':	src_file='c';	src_rank='1';	break;
				case 'Q':	src_file='d';	src_rank='4';	break;
				case 'K':	src_file='e';	src_rank='1';	break;
				case 'P':   src_file='b';	src_rank='2';	break;
				case 'r':	src_file='h';	src_rank='8';	break;
				case 'n':   src_file='b';	src_rank='8';	break;
				case 'b':   src_file='f';	src_rank='8';	break;
				case 'q':   src_file='d';	src_rank='8';	break;
				case 'k':   src_file='e';	src_rank='5';	break;
				case 'p':   src_file='a';	src_rank='7';	break;
                case 'x':   src_file='c';   src_rank='3';   break;
				default:	src_file='a';	src_rank='3';	break;
			}
		}
		else
		{
			
			// Find an appropriate white square piece in the box
			switch(piece)
			{
				case 'R':	src_file='h';	src_rank='1';	break;
				case 'N':	src_file='b';	src_rank='1';	break;
				case 'B':	src_file='f';	src_rank='1';	break;
				case 'Q':	src_file='d';	src_rank='1';	break;
				case 'K':	src_file='e';	src_rank='4';	break;
				case 'P':   src_file='a';	src_rank='2';	break;
				case 'r':	src_file='a';	src_rank='8';	break;
				case 'n':   src_file='g';	src_rank='8';	break;
				case 'b':   src_file='c';	src_rank='8';	break;
				case 'q':   src_file='d';	src_rank='5';	break;
				case 'k':   src_file='e';	src_rank='8';	break;
				case 'p':   src_file='b';	src_rank='7';	break;
                case 'x':   src_file='d';   src_rank='3';   break;
				default:	src_file='b';	src_rank='3';	break;
			}
		}

		// Copy from the box into the image buffer of the wxBitmap
		Put( src_file, src_rank, dst_file, dst_rank );
	}

    // Copy the picked up piece into place
    int row, col, x, y;
    int square_width = width/8;
    int square_height= height/8;
    col = pickup2_file-'a';       // 'a'->0, 'b'->1 .. 'h'->7
    row = 7-(pickup2_rank-'1');   // '1'->7, '2'->6 .. '8'->0
    if( col*square_width + shift.x > 7*square_width )
        shift.x = (7-col)*square_width;
    else if( col*square_width + shift.x < 0 )
        shift.x = (0-col)*square_width;
    if( row*square_height + shift.y > 7*square_height )
        shift.y = (7-row)*square_height;
    else if( row*square_height + shift.y < 0 )
        shift.y = (0-row)*square_height;
    PutEx( save_piece, pickup2_file, pickup2_rank, shift );

	// Copy from the image buffer into the wxBitmap
#ifdef MAC_FIX_LATER
    wxAlphaPixelData bmdata(my_chess_bmp);
    wxAlphaPixelData::Iterator p(bmdata);
    byte *src = buf_board;
    for( int i=0; i<height*width; i++ )
    {
        p.Alpha() = *src++;
        p.Red()   = *src++;
        p.Green() = *src++;
        p.Blue()  = *src++;
        p++;
    }
#else
	//my_chess_bmp.SetBitmapBits( width_bytes*height, buf_board );
    /*int ret = ::*/SetBitmapBits( (HBITMAP)(my_chess_bmp.GetHBITMAP()), width_bytes*height, buf_board );  //@@
    //dbg_printf( "::SetBitmapBits() returns %d\n", ret );
#endif

    // Now use GDI to add highlights
    char highlight_file = highlight_file1;
    char highlight_rank = highlight_rank1;
    bool is_highlight = false;
    //highlight_file = 'f';
    //highlight_rank = '4';
    for( int i=0; i<2; i++ )
    {
        if( highlight_file )
        {
            if( normal_orientation )
            {
                col = highlight_file-'a';       // 'a'->0, 'b'->1 .. 'h'->7
                row = 7-(highlight_rank-'1');   // '1'->7, '2'->6 .. '8'->0
            }
            else
            {
                col = 7-(highlight_file-'a');   // 'h'->0, 'g'->1 .. 'a'->7
                row = highlight_rank-'1';       // '1'->0, '2'->1 .. '8'->7
            }
            x = col*square_width;
            y = row*square_height;
            if( !is_highlight )
            {
       	        /*@@restore = */dcmem.SelectObject( my_chess_bmp );
                is_highlight = true;
            }
            wxPoint rect[5];
            rect[0].x = x;
            rect[0].y = y;
            rect[1].x = x + square_width-1;
            rect[1].y = y;
            rect[2].x = x + square_width-1;
            rect[2].y = y + square_height-1;
            rect[3].x = x;
            rect[3].y = y + square_height-1;
            rect[4].x = x;
            rect[4].y = y;
            dcmem.DrawLines( 5, rect );
        }
        highlight_file = highlight_file2;
        highlight_rank = highlight_rank2;
    }
}


// Put a shifted, masked piece from box onto board
void GraphicBoard::PutEx( char piece,
						char dst_file, char dst_rank, wxPoint shift )
{
    char src_file, src_rank;
    const char *mask=black_rook_mask;
    
	// Find an appropriate white square piece in the box
	switch(piece)
	{
		case 'R':	src_file='h';	src_rank='1';
                    mask = white_rook_mask;           	break;
		case 'N':	src_file='b';	src_rank='1';
                    mask = white_knight_mask;           break;
		case 'B':	src_file='f';	src_rank='1';
                    mask = white_bishop_mask;           break;
		case 'Q':	src_file='d';	src_rank='1';
                    mask = white_queen_mask;           	break;
		case 'K':	src_file='e';	src_rank='4';
                    mask = white_king_mask;           	break;
		case 'P':   src_file='a';	src_rank='2';
                    mask = white_pawn_mask;           	break;
		case 'r':	src_file='a';	src_rank='8';
                    mask = black_rook_mask;           	break;
		case 'n':   src_file='g';	src_rank='8';
                    mask = black_knight_mask;           break;
		case 'b':   src_file='c';	src_rank='8';
                    mask = black_bishop_mask;           break;
		case 'q':   src_file='d';	src_rank='5';
                    mask = black_queen_mask;           	break;
		case 'k':   src_file='e';	src_rank='8';
                    mask = black_king_mask;           	break;
		case 'p':   src_file='b';	src_rank='7';
                    mask = black_pawn_mask;           	break;
		default:	src_file='b';	src_rank='3';
	}

    //dbg_printf("%c%c\n", dst_file, dst_rank );
	if( density != 4 )
	{
		// 16 bit (and other) graphics code
		unsigned int i, j;
		byte *src, *dst;
//		byte *dst_min, *dst_max;
//      dst_min = (byte *)&buf_board[0];
//      dst_max = (byte *)&buf_board[height*width_bytes];
		for( i=0; i < height/8; i++ )
		{
			src			  =
				(byte *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			dst			  =
				(byte *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
            dst += (width_bytes * shift.y);
            dst += shift.x*density;
			for( j=0; j < width_bytes/(8*density); j++ )
            {
                if( *mask++ == '1' )
                    memcpy( dst, src, density );
                src += density;
                dst += density;
            }
		}
	}
	else
	{

		// 32 bit graphics code
		unsigned int i, j;
		uint32_t *src, *dst;
		//unsigned long *dst_min, *dst_max;
        //dst_min = (unsigned long *)&buf_board[0];
        //dst_max = (unsigned long *)&buf_board[height*width_bytes];
		for( i=0; i < height/8; i++ )
		{
			src			  =
				(uint32_t *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			dst			  =
				(uint32_t *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);

            dst += (width_bytes/4 * shift.y);
            dst += shift.x;
			for( j=0; j < width_bytes/32; j++ )
            {
                if( *mask++ == '1' )
                    *dst = *src;
                dst++;
                src++;
            }
		}
	}
}

void GraphicBoard::OnMouseLeftDown( wxMouseEvent &event )
{
    wxPoint point = event.GetPosition();
    wxPoint click = point;
    wxPoint pos = GetPosition();
    click.x += pos.x;
    click.y += pos.y;
    char file, rank;
    HitTest( point, file, rank );
    if( 'a'<=file && file<='h' && '1'<=rank && rank<='8' )
    {
        pickup_file  = file;
        pickup_rank  = rank;
        pickup_point = point;
        dbg_printf( "TODO check file=%c rank=%c\n", file, rank );
        if( objs.gl->MouseDown(file,rank,click) )
        {
            pickup_file  = file;
            pickup_rank  = rank;
            pickup_point = point;
            sliding = true;
            slide_pos = objs.gl->gd.master_position;
            OnMouseMove(event);     // Draw the position in case peeking allowed
        }
    }
}

void GraphicBoard::OnMouseMove( wxMouseEvent &event )
{
    wxPoint point = event.GetPosition();
    if( sliding )
    {
        dbg_printf( "In board\n" );
        point.x -= pickup_point.x;
        point.y -= pickup_point.y;
        bool blank_other_squares = objs.gl->ShowSlidingPieceOnly();
        SetPositionEx( slide_pos, blank_other_squares, pickup_file, pickup_rank, point );
        Draw();
    }
}

void GraphicBoard::OnMouseLeftUp( wxMouseEvent &event )
{
    sliding = false;
    wxPoint point = event.GetPosition();
    wxPoint click = point;
    wxPoint pos = GetPosition();
    click.x += pos.x;
    click.y += pos.y;
    point.x -= pickup_point.x;
    point.y -= pickup_point.y;
	char file=pickup_file;
    char rank=pickup_rank;
  	HitTestEx( file, rank, point );
    if( 'a'<=file && file<='h' && '1'<=rank && rank<='8' )
    {
        dbg_printf( "UP file=%c rank=%c\n", file, rank );
        objs.gl->MouseUp(file,rank,click);
    }
    else
    {
        dbg_printf( "UP out of range\n" );
        objs.gl->MouseUp();
    }
}


#if 0
#define nbrof(a) ( sizeof(a) / sizeof((a)[0]) )
void TempCreateXpm( wxBitmap &bmp, const char *name )
{
    static char codes[] = " .-,`~{}=+&$*@#0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static int reds[1000];
    static int greens[1000];
    static int blues[1000];
    int nbr_of=0;
    static char filename[100];
    sprintf( filename, "%s.xpm", name );

	// Get dimensions of the wxBitmap
	//bmp.GetBitmap( &info );
	BITMAP info;
    ::GetObject( bmp.GetHBITMAP(), sizeof(BITMAP), &info ); //@@
	int width_bytes  = info.bmWidthBytes;   // bytes
	int width        = info.bmWidth;        // pixels
  	assert( (info.bmWidthBytes%info.bmWidth) == 0 );
	int density      = info.bmWidthBytes/info.bmWidth;
	int height       = info.bmHeight;
    byte                *buf_bmp;
    buf_bmp = new byte[width_bytes*height];
    memset( buf_bmp, 0, width_bytes*height);
    ::GetBitmapBits((HBITMAP)(bmp.GetHBITMAP()), width_bytes*height, buf_bmp );
    FILE *f;
    f = fopen(filename,"wt");
    if( f )
    {
        byte r,g,b, *p;
        int row,col;
        for( row=0; row<height; row++ )
        {
            p = buf_bmp + row*width_bytes;
            for( col=0; col<width; col++ )
            {
                byte *q = p;
                b = *p++;
                g = *p++;
                r = *p++;
                p = q+density;
                bool found = false;
                for( int i=0; !found && i<nbr_of; i++ )
                {
                    if( reds[i]==r && greens[i]==g &&  blues[i]==b )
                        found = true;
                }
                if( !found )
                {
                    reds  [nbr_of] = r;
                    greens[nbr_of] = g;
                    blues [nbr_of] = b;
                    if( nbr_of+1 < nbrof(codes)-1 )
                        nbr_of++;
                }
            }
        }
    /*  for( int i=0; i<nbr_of; i++ )
        {
            double d1 = ((double)reds[i]);
            double d2 = ((double)greens[i]);
            double d3 = ((double)blues[i]);
            double d = d1*d1 + d2*d2 + d3*d3;
            double max_so_far = 0.0;
            int max_so_far_idx = 0;
            for( int j=i+1; j<nbr_of; j++ )
            {
                double e1 = ((double)reds[j]);
                double e2 = ((double)greens[j]);
                double e3 = ((double)blues[j]);
                double e = e1*e1 + e2*e2 + e3*e3;
                if( e > max_so_far )
                {
                    max_so_far = e;
                    max_so_far_idx = j;
                }
            }
            if( max_so_far > d )
            {
                int j = max_so_far_idx;
                int temp  = reds[i];
                reds[i]   = reds[j];
                reds[j]   = temp;
                temp      = greens[i];
                greens[i] = greens[j];
                greens[j] = temp;
                temp      = blues[i];
                blues[i]  = blues[j];
                blues[j]  = temp;
            }
            static char specials[] = " .-,`~{}=+&$*@#0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            if( reds[i]==0xff && blues[i]==0 && greens[i]==0 )
                codes[i] = 'R';
            else if( reds[i]==0 && blues[i]==0 && greens[i]==0xff )
                codes[i] = 'G';
            else if( reds[i]==0 && blues[i]==0xff && greens[i]==0 )
                codes[i] = 'B';
            else if( i >= strlen(specials) )
                codes[i] = 'a' + (i-strlen(specials));
            else
                codes[i] = specials[i];
        } */
        fprintf( f, "static char *%s_xpm[] = {\n", name );
        fprintf( f, "\"%d %d %d 1\",\n", width, height, nbr_of );
        for( int i=0; i<nbr_of; i++ )
        {
            fprintf( f, "\"%c c #%02x%02x%02x\",\n", codes[i], reds[i], greens[i], blues[i] );
        }
        for( row=0; row<height; row++ )
        {
            p = buf_bmp + row*width_bytes;
            for( col=0; col<width; col++ )
            {
                byte *q = p;
                b = *p++;
                g = *p++;
                r = *p++;
                p = q+density;
                int found_idx = 0;
                bool found = false;
                for( int i=0; i<nbr_of; i++ )
                {
                    if(  r == reds[i] &&
                         g == greens[i] &&
                         b == blues[i]
                      )
                    {
                        found = true;
                        found_idx = i;
                        break;
                    }
                }
                if( !found )
                {
                    double min_so_far = 1000000000.0;
                    for( int i=0; i<nbr_of; i++ )
                    {
                        double d1 = ((double)reds[i]   - (double)r );
                        double d2 = ((double)greens[i] - (double)g );
                        double d3 = ((double)blues[i]  - (double)b );
                        double d = d1*d1 + d2*d2 + d3*d3;
                        if( d < min_so_far )
                        {
                            min_so_far = d;
                            found_idx = i;
                            found = true;
                        }
                    }
                }
                if( col == 0 )
                    fprintf( f, "\"" );
                if( found )
                    fprintf( f, "%c", codes[found_idx] );
                else
                    fprintf( f, "[ #%02x%02x%02x ]", r, g, b );
            }
            if( row+1 < height )
                fprintf( f, "\",\n" );
            else
                fprintf( f, "\"\n" );
        }
        fprintf( f, "};\n" );
        fclose(f);
    }
    #if 0
    f = fopen("masks_black.txt","wt");
    if( f )
    {
        byte r,g,b, *p;
        int row,col;
        for( row=0; row<height; row++ )
        {
            p = buf_bmp + row*width_bytes;
            for( col=0; col<width; col++ )
            {
                b = *p++;
                g = *p++;
                r = *p++;
                if( col == 0 )
                    fprintf( f, "\"" );
                fprintf( f, "%c", (r==0xff && g==0 && b==0) ? '0' : '1' );
            }
            if( row+1 < height )
                fprintf( f, "\",\n" );
            else
                fprintf( f, "\"\n" );
        }
        fclose(f);
    }
    f = fopen("masks_white.txt","wt");
    if( f )
    {
        byte r,g,b, *p;
        int row,col;
        for( row=0; row<height; row++ )
        {
            p = buf_bmp + row*width_bytes;
            for( col=0; col<width; col++ )
            {
                b = *p++;
                g = *p++;
                r = *p++;
                if( col == 0 )
                    fprintf( f, "\"" );
                fprintf( f, "%c", (r==0 && g==0 && b==0xff) ? '0' : '1' );
            }
            if( row+1 < height )
                fprintf( f, "\",\n" );
            else
                fprintf( f, "\"\n" );
        }
        fclose(f);
    }
    #endif
    delete(buf_bmp);
}
#endif

BEGIN_EVENT_TABLE(GraphicBoard, wxControl)
    EVT_PAINT(GraphicBoard::OnPaint)
//    EVT_SIZE(GraphicBoard::OnSize)
//    EVT_MOUSE_CAPTURE_LOST(GraphicBoard::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(GraphicBoard::OnMouseEvent)
    EVT_LEFT_UP (GraphicBoard::OnMouseLeftUp)
    EVT_LEFT_DOWN (GraphicBoard::OnMouseLeftDown)
    EVT_MOTION (GraphicBoard::OnMouseMove)
//    EVT_ERASE_BACKGROUND(GraphicBoard::OnEraseBackground)
//    EVT_TIMER( TIMER_ID, GraphicBoard::OnTimeout)
END_EVENT_TABLE()
