/****************************************************************************
 * Create and manipulate the chess board bitmaps displayed by the chess
 * board controls (currently CtrlChessBoard and CtrlChessPositionSetup)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/rawbmp.h"
#include "wx/file.h"
#include "wx/statline.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "CompressedBitmaps.h"
#include "ChessBoardBitmap.h"
#include "GameLogic.h"
#include "DialogDetect.h"
#include "thc.h"
#include "Objects.h"
#include "Repository.h"

// The simplest possible testbed dialog

// TestbedDialog class declaration
class TestbedDialog: public wxDialog
{    
    DECLARE_CLASS( TestbedDialog )
    DECLARE_EVENT_TABLE()

public:
	wxBitmap bm_;

    // Constructors
    TestbedDialog( wxBitmap &bm, wxWindow* parent = objs.frame,
      wxWindowID id = wxID_ANY,
      const wxString& caption = "Testbed Dialog",
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Run dialog
    bool Run();

    // Creates the controls and sizers
    void CreateControls();

	// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );
};


// Initialise the graphic board
ChessBoardBitmap::ChessBoardBitmap()
{
    pickup_file    = 0;
    pickup_rank    = 0;
    pickup_point.x = 0;
    pickup_point.y = 0;
    sliding = false;
    buf_board = 0;
    buf_box = 0;
	normal_orientation = true;
	is_board_setup = false;
	ok_to_copy_chess_board_to_board_setup = false;
	ClearHighlight1();
	ClearHighlight2();
}

// Cleanup
ChessBoardBitmap::~ChessBoardBitmap()
{
	if( buf_board )
		delete(buf_board);
	if( buf_box )
		delete(buf_box);
}

// Simple bitmap copy with a kludgey extra feature, if a background colour is provided, it is used to reverse the anti-aliasing adjustment
//  back to grey scale
void ChessBoardBitmap::BmpCopy( wxColour *background, wxBitmap &from, int x1, int y1, wxBitmap &to,	int x2, int y2, int w, int h, const char *mask )
{
	int r_background = background ? background->Red()   : 0;
	int g_background = background ? background->Green() : 0;
	int b_background = background ? background->Blue()  : 0;
    wxNativePixelData pixels_src(from);
    wxNativePixelData::Iterator src(pixels_src);
    wxNativePixelData pixels_dst(to);
    wxNativePixelData::Iterator dst(pixels_dst);
    for( int row=0; row<h; row++ )
    {
        src.MoveTo(pixels_src, x1, y1+row );
        dst.MoveTo(pixels_dst, x2, y2+row );
        for( int col=0; col<w; col++ )
        {
			if( mask==NULL || *mask != '0' )
			{
			  	byte r = src.Red();
				byte g = src.Green();
				byte b = src.Blue();

				// Kludge I know, but ....  Optionally undo the anti-aliasing adjustment to restore greyscale values
				//  Restores the original greyscale piece bitmaps, optimised for a white background
				if( background && r!=g || r!=b )
				{
					int use = r;
					int use_background = r_background;
					if( g_background > use_background )		// divide by the largest number for better accuracy
					{
						use = g;
						use_background = g_background;
					}
					if( b_background > use_background )
					{
						use = b;
						use_background = b_background;
					}
					if( use_background > 0 )
					{
						int adjust = (use*255)/use_background;	// reverse adjustment from, eg r = (adjust*r_background) / 255;
						r = adjust;
						g = adjust;
						b = adjust;
					}
				}
				dst.m_ptr[wxNativePixelFormat::RED] = r;
				dst.m_ptr[wxNativePixelFormat::GREEN] = g;
				dst.m_ptr[wxNativePixelFormat::BLUE] = b;
				// dst.m_ptr = src.m_ptr;
			}
            src++;
            dst++;
			if( mask != NULL )
				mask++;
        }
    }
}

// Simple image copy with a kludgey extra feature, if a background colour is provided, it is used to reverse the anti-aliasing adjustment
//  back to grey scale
void ChessBoardBitmap::ImageCopy( wxColour *background, wxBitmap &from, int x1, int y1, wxImage &to, int x2, int y2, int w, int h, const char *mask )
{
	int r_background = background ? background->Red()   : 0;
	int g_background = background ? background->Green() : 0;
	int b_background = background ? background->Blue()  : 0;
    wxNativePixelData pixels_src(from);
    wxNativePixelData::Iterator src(pixels_src);
    for( int row=0; row<h; row++ )
    {
        src.MoveTo(pixels_src, x1, y1+row );
        for( int col=0; col<w; col++ )
        {
			byte r = 0;
			byte g = 0;
			byte b = 0;
			bool transparent = true;
			if( mask==NULL || *mask != '0' )
			{
			  	r = src.Red();
				g = src.Green();
				b = src.Blue();
				transparent = false;

				// Kludge I know, but ....  Optionally undo the anti-aliasing adjustment to restore greyscale values
				//  Restores the original greyscale piece bitmaps, optimised for a white background
				if( background && r!=g || r!=b )
				{
					int use = r;
					int use_background = r_background;
					if( g_background > use_background )		// divide by the largest number for better accuracy
					{
						use = g;
						use_background = g_background;
					}
					if( b_background > use_background )
					{
						use = b;
						use_background = b_background;
					}
					if( use_background > 0 )
					{
						int adjust = (use*255)/use_background;	// reverse adjustment from, eg r = (adjust*r_background) / 255;
						r = adjust;
						g = adjust;
						b = adjust;
					}
				}
			}
			to.SetRGB(x2+col, y2+row, r,g,b);
			to.SetAlpha(x2+col, y2+row, transparent?wxIMAGE_ALPHA_TRANSPARENT:wxIMAGE_ALPHA_OPAQUE );
            src++;
			if( mask != NULL )
				mask++;
        }
    }
}

// Calculate the dimensions of a BoardSetup bitmap
/*														   
	   +---------------------------------------------+     	Input:
	   |									  		 |	   	dim_sz = size of whole thing
	   |	X								  	X    |	   	
	   |			 +-----------------+	  		 |	   	Outputs:
	   |	X		 |				   |		X    |	   	dim_pix = size of one chessboard square
	   |	 		 |				   |	  	     |		dim_board = coordinates of 8x8 square board in the middle
	   |	X		 |				   |	  	X    |		dim_pickup_left = coordinates of the top left X (one square sized)
	   |	 		 |				   |	  	     |		dim_pickup_right = coordinates of the top right X (one square sized)
	   |	X		 |				   |	  	X    |		dim_pickup_pitch = vertical distance between X pickup pieces
	   |	 		 |				   |	  	     |
	   |	X		 |				   |	  	X    |
	   |	 		 +-----------------+		     |
	   |	X								  	X    |
 	   |									  	     |
	   +---------------------------------------------+

*/
void ChessBoardBitmap::BoardSetupDim( const wxSize sz )
{
	// Starting point was 364 * 294 pixel bitmap used 34 pix squares
	dim_sz = sz;
	uint32_t w = sz.x;
	uint32_t h = sz.y;
	uint32_t pix1 = (w*100) / (1000 + 50);	// divide by 10 (at least)
	uint32_t pix2 = (h*100) / (800 + 30);	// divide by 8 (at least)
	int pix = pix1<pix2 ? pix1 : pix2;	// min()
	dim_pix = pix;
	cprintf( "pix=%d, pix1=%d, pix2=%d\n", pix, pix1, pix2 );
	uint32_t vert_gap = ((h - pix*6)*100)/700; // pix*6 + 7*vert_gap = h => vert_gap = (h - pix*6) / 7
	uint32_t x = (w-8*pix)/2;
	uint32_t y = (h-8*pix)/2;
	uint32_t x1 = ((w-8*pix)/2 - pix)/2;
	uint32_t y1 = vert_gap;
	dim_pickup_pitch = pix+vert_gap;
	dim_board.x = x;
	dim_board.y = y;
	dim_pickup_left.x = x1;
	dim_pickup_left.y = y1;
	dim_pickup_right.x = w-x1-pix;
	dim_pickup_right.y = y1;
}

void ChessBoardBitmap::CreateAsChessBoardOnly( const wxSize sz, bool normal_orientation_, const thc::ChessPosition &cp, const bool *highlight )
{
	normal_orientation = normal_orientation_;
	is_board_setup = false;
	int pix = sz.x/8;
	ChessBoardCreate( pix, cp, highlight );
}

void ChessBoardBitmap::CreateAsBoardSetup( const wxSize sz, bool normal_orientation_, const thc::ChessPosition &cp, const bool *highlight )
{
	normal_orientation = normal_orientation_;
	ok_to_copy_chess_board_to_board_setup = false;
	is_board_setup = true;
	thc::ChessPosition start_position;
	BoardSetupDim( sz );							// calculate dimenstions
	ChessBoardCreate( dim_pix, start_position );	// create inner chess board first
	BoardSetupCreate();								// then the surrounds, including the pickup pieces
	BoardSetupCustomCursorsCreate();				// finally the pickup pieces
	ok_to_copy_chess_board_to_board_setup = true;
	SetChessPosition( cp, highlight );
}

void ChessBoardBitmap::BoardSetupCreate()
{
    wxBitmap board_setup;
	board_setup.Create(dim_sz.x,dim_sz.y,24);
    wxMemoryDC dc;
    dc.SelectObject(board_setup);
    dc.SetMapMode(wxMM_TEXT);

	// Set all of board_setup to white, with one pixel black border
    wxColour white(255,255,255);
    wxBrush white_brush(white);
    dc.SetBrush( white_brush );
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(0,0,board_setup.GetWidth(),board_setup.GetHeight());

	// To get access to all possible piece/ colour combinations set the standard chess position
	//  on the chess board plus extra kings and queens
	thc::ChessPosition start_position;
	start_position.squares[thc::d2] = 'Q';
	start_position.squares[thc::e2] = 'K';
	start_position.squares[thc::d7] = 'q';
	start_position.squares[thc::e7] = 'k';
	SetChessPosition( start_position );

	// Add pieces to be picked up on the left side (use light squared pieces - then adding
	//  light square colour as first parameter to BmpCopy undoes the anti-aliasing adjustment
	//  leaving greyscale anti-aliasing optimised for a white background)
	for( int i=0; i<6; i++ )
	{
		const char *mask;
		char file, rank;
		if( normal_orientation )
		{
			switch(i)	// normal orientation left = White K,Q,R,B,N,P top to bottom
			{
				default:
				case 0:		file='e';	rank='2';	mask = white_king_mask;		break;
				case 1:		file='d';	rank='1';   mask = white_queen_mask;	break;
				case 2:		file='h';	rank='1';   mask = white_rook_mask;		break;
				case 3:		file='f';	rank='1';   mask = white_bishop_mask;	break;
				case 4:		file='b';	rank='1';   mask = white_knight_mask;	break;
				case 5:		file='a';	rank='2';   mask = white_pawn_mask;		break;
			}
		}
		else
		{
			switch(i)	// reverse orientation left = Black K,Q,R,B,N,P top to bottom
			{
				default:
				case 0:		file='e';	rank='8';	mask = black_king_mask;		break;
				case 1:     file='d';	rank='7';   mask = black_queen_mask;	break;
				case 2:     file='a';	rank='8';   mask = black_rook_mask;		break;
				case 3:     file='c';	rank='8';	mask = black_bishop_mask;	break;
				case 4:     file='g';	rank='8';	mask = black_knight_mask;	break;
				case 5:     file='b';	rank='7';   mask = black_pawn_mask;		break;
			}
		}

		// Top left = 0,0 is a8 if normal orientation h1 if reverse orientation
		//            1,1 is b7 if normal orientation g2 if reverse orientation
		int x = normal_orientation ? (file-'a') : ('h'-file);
		int y = normal_orientation ? ('8'-rank) : (rank-'1');
		BmpCopy( &light_colour, chess_board_bmp, x*dim_pix, y*dim_pix, board_setup, dim_pickup_left.x, dim_pickup_left.y + i*dim_pickup_pitch, dim_pix, dim_pix, mask );
	}

	// Add pieces to be picked up on the right side
	for( int i=0; i<6; i++ )
	{
		const char *mask;
		char file, rank;
		if( normal_orientation )
		{
			switch(i)	// normal orientation right = Black K,Q,R,B,N,P bottom to top
			{
				default:
				case 5:		file='e';	rank='8';	mask = black_king_mask;		break;
				case 4:		file='d';	rank='7';	mask = black_queen_mask;	break;
				case 3:		file='a';	rank='8';	mask = black_rook_mask;		break;
				case 2:		file='c';	rank='8';	mask = black_bishop_mask;	break;
				case 1:		file='g';	rank='8';	mask = black_knight_mask;	break;
				case 0:		file='b';	rank='7';	mask = black_pawn_mask;		break;
			}
		}
		else
		{
			switch(i)	// reverse orientation right = White K,Q,R,B,N,P bottom to top
			{
				default:
				case 5:		file='e';	rank='2';	mask = white_king_mask;		break;
				case 4:		file='d';	rank='1';	mask = white_queen_mask;	break;
				case 3:		file='h';	rank='1';	mask = white_rook_mask;		break;
				case 2:		file='f';	rank='1';	mask = white_bishop_mask;	break;
				case 1:		file='b';	rank='1';	mask = white_knight_mask;	break;
				case 0:		file='a';	rank='2';	mask = white_pawn_mask;		break;
			}
		}

		// Top left = 0,0 is a8 if normal orientation h1 if reverse orientation
		//            1,1 is b7 if normal orientation g2 if reverse orientation
		int x = normal_orientation ? (file-'a') : ('h'-file);
		int y = normal_orientation ? ('8'-rank) : (rank-'1');
		BmpCopy( &light_colour, chess_board_bmp, x*dim_pix, y*dim_pix, board_setup, dim_pickup_right.x, dim_pickup_right.y + i*dim_pickup_pitch, dim_pix, dim_pix, mask );
	}
	board_setup_bmp = board_setup;
}

void ChessBoardBitmap::BoardSetupCustomCursorsCreate()
{
	for( int i=0; i<12; i++ )
	{
		char file, rank;
		const char *mask;
		wxImage *ptr;
		switch(i)
		{
			default:
			case 0: file='e';	rank='2';
					mask = white_king_mask;
					ptr  = &white_king_cursor;		break;
			case 1: file='d';	rank='1';
			        mask = white_queen_mask;
			        ptr  = &white_queen_cursor;		break;
			case 2: file='h';	rank='1';
			        mask = white_rook_mask;
			        ptr  = &white_rook_cursor;		break;
			case 3: file='f';	rank='1';
			        mask = white_bishop_mask;
			        ptr  = &white_bishop_cursor;	break;
			case 4: file='b';	rank='1';
			        mask = white_knight_mask;
			        ptr  = &white_knight_cursor;	break;
			case 5: file='a';	rank='2';
			        mask = white_pawn_mask;
			        ptr  = &white_pawn_cursor;		break;
			case 6: file='e';	rank='8';
					mask = black_king_mask;
					ptr  = &black_king_cursor;		break;
			case 7: file='d';	rank='7';
			        mask = black_queen_mask;
			        ptr  = &black_queen_cursor;		break;
			case 8: file='a';	rank='8';
			        mask = black_rook_mask;
			        ptr  = &black_rook_cursor;		break;
			case 9: file='c';	rank='8';
			        mask = black_bishop_mask;
			        ptr  = &black_bishop_cursor;	break;
			case 10: file='g';	rank='8';
			        mask = black_knight_mask;
			        ptr  = &black_knight_cursor;	break;
			case 11: file='b';	rank='7';
			        mask = black_pawn_mask;
			        ptr  = &black_pawn_cursor;		break;
		}

		// Find a smaller rectangle, within dim_pix by dim_pix square, containing maximum extent of the actual piece
		//  Idea is cursor is dimensioned according to size of piece, not size of square
		//
		/*	eg imagine a pawn like this, dimension of cursor will be height = 9-2 = 7, width = 9-1 = 8

				0000000000
				0000000000
				0000110000		 <-top=2
				0001221000
				0000110000
				0000110000
				0001221000
				0011111100		   
				0111111110		           
				0000000000		 <-bottom=9
				 ^       ^
				 left=1  right=9
		*/
		const char *peek = mask;
		int top = 1000000000;
		int left = 1000000000;
		int bottom = 0;
		int right = 0;
		for( int y=0; y<dim_pix; y++ )
		{
			for( int x=0; x<dim_pix; x++ )
			{
				bool hit = *peek!='0';
				peek++;
				if( hit )
				{
					if( top > y )
						top = y;
					if( bottom < y )
						bottom = y;
					if( left > x )
						left = x;
					if( right < x )
						right = x;
				}
			}
		}
		right++;	// one beyond right as per convention
		bottom++;	// one beyond bottom as per convention

		// Square it up (avoids some weird distortions if images larger than 32x32 are scaled)
		if( bottom-top > right-left )
		{
			int widen = (bottom-top) - (right-left);
			left -= (widen/2);
			right = (bottom-top) + left;
			if( left < 0 )
				left = 0;
			if( right > dim_pix )
				right = dim_pix;
		}
		else if( right-left > bottom-top )
		{
			int lengthen = (right-left) - (bottom-top);
			top -= (lengthen/2);
			bottom = (right-left) + top;
			if( top < 0 )
				top = 0;
			if( bottom > dim_pix )
				bottom = dim_pix;
		}

		// Coordinates on source chess board
		int x = normal_orientation ? (file-'a') : ('h'-file);
		int y = normal_orientation ? ('8'-rank) : (rank-'1');

		// If we found a maximum extents rectangle, use that
		if( bottom>top && right>left && 
			0<=left && left<=dim_pix && 
			0<=right && right<=dim_pix &&
			0<=top && top<=dim_pix &&
			0<=bottom && bottom<=dim_pix )
		{
			ptr->Create( right-left, bottom-top, false );
			ptr->InitAlpha();
			std::string s;
			const char *peek2 = mask;  // need an adjusted mask covering the maximum extents rectangle only
			for( int yy=0; yy<dim_pix; yy++ )
			{
				for( int xx=0; xx<dim_pix; xx++ )
				{
					char ch = *peek2++;
					if( left<=xx && xx<right && top<=yy && yy<bottom )
						s.push_back(ch);
				}
			}

			ImageCopy( &light_colour, chess_board_bmp, x*dim_pix+left, y*dim_pix+top, *ptr, 0, 0, right-left, bottom-top, s.c_str() );
		}

		// Else just use the whole square
		else
		{
			ptr->Create( dim_pix, dim_pix, false );
			ptr->InitAlpha();
			ImageCopy( &light_colour, chess_board_bmp, x*dim_pix, y*dim_pix, *ptr, 0, 0, dim_pix, dim_pix, mask );
		}
	}
}

void ChessBoardBitmap::GetCustomCursor( char piece, wxImage &img )
{
	wxImage *ptr;
    switch( piece )
    {
        default:
        case 'P':   ptr = &white_pawn_cursor;    break;
        case 'N':   ptr = &white_knight_cursor;  break;
        case 'B':   ptr = &white_bishop_cursor;  break;
        case 'R':   ptr = &white_rook_cursor;    break;
        case 'Q':   ptr = &white_queen_cursor;   break;
        case 'K':   ptr = &white_king_cursor;    break;
        case 'p':   ptr = &black_pawn_cursor;    break;
        case 'n':   ptr = &black_knight_cursor;  break;
        case 'b':   ptr = &black_bishop_cursor;  break;
        case 'r':   ptr = &black_rook_cursor;    break;
        case 'q':   ptr = &black_queen_cursor;   break;
        case 'k':   ptr = &black_king_cursor;    break;
    }
	img = *ptr;
}


// Figure out whether a piece or square is pointed to
bool ChessBoardBitmap::BoardSetupHitTest( const wxPoint &point, char &piece, char &file, char &rank )
{
	int w = dim_sz.x;
    int xborder  = dim_board.x;
    int yborder  = dim_board.y;
    bool hit = false;
    piece = '\0';
    file  = '\0';
    rank  = '\0';
	unsigned long row = ( (point.y - yborder) / dim_pix );

    // Main board
    if( xborder<=point.x && point.x< w-xborder )
    {
        hit = true;
	    unsigned long col = ( (point.x-xborder) / dim_pix );
        if( !normal_orientation )
        {
            row = 7-row;
            col = 7-col;
        }
		rank = '1'+ (int)(7-row);
		file = 'a'+ (int)col;
    }

    // Maybe pickup pieces
    else
    {
        int top = dim_pickup_left.y; // offset before first piece
        int found = -1;
        for( int i=0; i<6; i++ )    // six pieces
        {
            if( top<=point.y && point.y<top+dim_pix )
            {
                found = i;
                break;
            }
            top += dim_pickup_pitch;
        }

        // Right side pickup pieces
        if( found>=0 &&
             dim_pickup_right.x  < point.x && point.x < dim_pickup_right.x+dim_pix
          )
        {
            hit = true;
            switch( found )
            {
                default:    hit = false;    break;
                case 0:     piece = normal_orientation?'p':'P';    break;
                case 1:     piece = normal_orientation?'n':'N';    break;
                case 2:     piece = normal_orientation?'b':'B';    break;
                case 3:     piece = normal_orientation?'r':'R';    break;
                case 4:     piece = normal_orientation?'q':'Q';    break;
                case 5:     piece = normal_orientation?'k':'K';    break;
            }
        }

        // Left side pickup pieces
        else if( found>=0 &&
             dim_pickup_left.x  < point.x && point.x < dim_pickup_left.x+dim_pix
          )
        {
            hit = true;
            switch( found )
            {
                default:    hit = false;    break;
                case 0:     piece = normal_orientation?'K':'k';    break;
                case 1:     piece = normal_orientation?'Q':'q';    break;
                case 2:     piece = normal_orientation?'R':'r';    break;
                case 3:     piece = normal_orientation?'B':'b';    break;
                case 4:     piece = normal_orientation?'N':'n';    break;
                case 5:     piece = normal_orientation?'P':'p';    break;
            }
        }
    }
    return hit;
}

void ChessBoardBitmap::ChessBoardCreate( int pix, const thc::ChessPosition &cp, const bool *highlight )
{
	wxSize size(pix*8,pix*8);
	int r0 = objs.repository->general.m_light_colour_r;
	int g0 = objs.repository->general.m_light_colour_g;
	int b0 = objs.repository->general.m_light_colour_b;
	wxColour c0(r0,g0,b0);
	light_colour = c0;
	int r1 = objs.repository->general.m_dark_colour_r;
	int g1 = objs.repository->general.m_dark_colour_g;
	int b1 = objs.repository->general.m_dark_colour_b;
	wxColour c1(r1,g1,b1);
	dark_colour = c1;
	int r2 = objs.repository->general.m_highlight_light_colour_r;
	int g2 = objs.repository->general.m_highlight_light_colour_g;
	int b2 = objs.repository->general.m_highlight_light_colour_b;
	wxColour c2(r2,g2,b2);
	highlight_light_colour = c2;
	int r3 = objs.repository->general.m_highlight_dark_colour_r;
	int g3 = objs.repository->general.m_highlight_dark_colour_g;
	int b3 = objs.repository->general.m_highlight_dark_colour_b;
	wxColour c3(r3,g3,b3);
	highlight_dark_colour = c3;
	int r4 = objs.repository->general.m_highlight_line_colour_r;
	int g4 = objs.repository->general.m_highlight_line_colour_g;
	int b4 = objs.repository->general.m_highlight_line_colour_b;
	wxColour c4(r4,g4,b4);
	highlight_line_colour = c4;
    const char **xpm=0;

	// Create a new bitmap of the right size
    wxBitmap new_chess_board_bmp;
	density = 3;
    new_chess_board_bmp.Create(pix*8,pix*8,density*8);

	// Create a bitmap with the 12 pieces, on a magenta background so we can build masks
    int pitch;
    const CompressedXpm *compressed_xpm = GetBestFit( pix, pitch );
    wxMemoryDC dc;
    dc.SelectObject(new_chess_board_bmp);
    dc.SetMapMode(wxMM_TEXT);
    CompressedXpmProcessor processor(compressed_xpm);
    xpm = (const char **)processor.GetXpm();
    wxBitmap lookup(xpm);

    // Start by filling our chessboard with the light square colour - transparent pen means
	//  no unwanted single pixel line border
    wxBrush light_brush(light_colour);
    dc.SetBrush( light_brush );
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0,0,new_chess_board_bmp.GetWidth(),new_chess_board_bmp.GetHeight());

	// Line highlight characteristics
	if( is_board_setup )
		highlight_mode = line_thick;
	else
	{
		if( objs.repository->general.m_suppress_highlight )
			highlight_mode = none;
		else
			highlight_mode = objs.repository->general.m_line_highlight ? line_thin : square_colour;
	}
    unsigned long thickness = pix/16;
    unsigned long indent    = thickness;
	if( highlight_mode == line_thin )
	{
		indent = 0;
		thickness = 1 + (pix/25);
		if( thickness > 4 )
			thickness = 4;
	}
    highlight_y_lo1 = indent;
    highlight_y_lo2 = highlight_y_lo1 + thickness;
    highlight_y_hi2 = pix - indent;
    highlight_y_hi1 = highlight_y_hi2 - thickness;
    highlight_x_lo1 = indent;
    highlight_x_lo2 = highlight_x_lo1 + thickness;
	highlight_x_hi2 = pix - indent;
    highlight_x_hi1 = highlight_x_hi2 - thickness;

	// Create a "box" containing all combinations of piece on square colour, subsequently
	//  build positions by copying the right pieces from the box
	const char *board[] =	// a picture of the bitmap comprising just 12 pieces on magenta
    {
        "rNbQkBnR",
        "Pp1qK234",			// 1,2,3,4 are placeholders - use them to make empty squares of
							//  different colours, see big comment below about how the "box"
							//  is organised
    };
    dc.SetBackgroundMode( wxPENSTYLE_TRANSPARENT );
    int x = 0;
    int y = 0;
    for( int board_idx=0; board_idx<2; board_idx++ )
    {
        const char *s = board[board_idx];
        bool dark = ((board_idx&1)==1);
        x = 0;
        for( int j=0; j<8; j++ )
        {
            char c = *s++;
            if( c == '1' )  // dark square (note we skip light colour since whole board already initialised to that)
            {
                wxBrush dark_brush(dark_colour);
                dc.SetBrush( dark_brush );
                dc.DrawRectangle(x<0?0:x, y, pix, pix );
            }
            else if( c == '2' )  // highlight light square
            {
                wxBrush highlight_light_brush(highlight_light_colour);
                dc.SetBrush( highlight_light_brush );
                dc.DrawRectangle(x<0?0:x, y, pix, pix );
            }
            else if( c == '3' )  // highlight dark square
            {
                wxBrush highlight_dark_brush(highlight_dark_colour);
                dc.SetBrush( highlight_dark_brush );
                dc.DrawRectangle(x<0?0:x, y, pix, pix );
            }
			else if( c == '4' )	// highlight line coloured square
			{
				// Make square h7 (h5 in box) a nice blue colour
				wxBrush ocean_blue_brush(highlight_line_colour);
				dc.SetBrush( ocean_blue_brush );
                dc.DrawRectangle(x<0?0:x, y, pix, pix );
			}
            else
            {
                // fill square with magenta
                wxColour colour_magenta(255,0,255);
                wxBrush magenta_brush(colour_magenta);
                dc.SetBrush( magenta_brush );
                dc.DrawRectangle(x<0?0:x, y, pix, pix );
                int from_idx=0;
                std::string str;
                std::string *p_str=0;
                const char **p_mask=0;
                switch( c )
                {
                    default:
                    case 'K':   from_idx = 0;
                                p_str =  &str_white_king_mask;
                                p_mask = &white_king_mask;
                                break;
                    case 'Q':   from_idx = 1; 
                                p_str =  &str_white_queen_mask;
                                p_mask = &white_queen_mask;
                                break;
                    case 'R':   from_idx = 2; 
                                p_str =  &str_white_rook_mask;
                                p_mask = &white_rook_mask;
                                break;
                    case 'B':   from_idx = 3; 
                                p_str =  &str_white_bishop_mask;
                                p_mask = &white_bishop_mask;
                                break;
                    case 'N':   from_idx = 4; 
                                p_str =  &str_white_knight_mask;
                                p_mask = &white_knight_mask;
                                break;
                    case 'P':   from_idx = 5; 
                                p_str =  &str_white_pawn_mask;
                                p_mask = &white_pawn_mask;
                                break;
                    case 'k':   from_idx = 6; 
                                p_str =  &str_black_king_mask;
                                p_mask = &black_king_mask;
                                break;
                    case 'q':   from_idx = 7; 
                                p_str =  &str_black_queen_mask;
                                p_mask = &black_queen_mask;
                                break;
                    case 'r':   from_idx = 8; 
                                p_str =  &str_black_rook_mask;
                                p_mask = &black_rook_mask;
                                break;
                    case 'b':   from_idx = 9; 
                                p_str =  &str_black_bishop_mask;
                                p_mask = &black_bishop_mask;
                                break;
                    case 'n':   from_idx = 10;
                                p_str =  &str_black_knight_mask;
                                p_mask = &black_knight_mask;
                                break;
                    case 'p':   from_idx = 11;
                                p_str =  &str_black_pawn_mask;
                                p_mask = &black_pawn_mask;
                                break;
                }

                // select out the bitmap before using wxNativePixelData
                dc.SelectObject(wxNullBitmap);

                wxNativePixelData pixels_src(lookup);
                wxNativePixelData::Iterator src(pixels_src);
                wxNativePixelData pixels_dst(new_chess_board_bmp);
                wxNativePixelData::Iterator dst(pixels_dst);

                // Copy from xpm, centred in destination square
                for( int row=0; row<pitch; row++ )
                {
                    src.MoveTo(pixels_src, from_idx * pitch, row );
                    dst.MoveTo(pixels_dst, x + (pix-pitch)/2, row + y + 2*(pix-pitch)/3 );
                    for( int col=0; col<pitch; col++ )
                    {
                        byte r = src.Red();
                        byte g = src.Green();
                        byte b = src.Blue();
                        dst.m_ptr[wxNativePixelFormat::RED] = r;
                        dst.m_ptr[wxNativePixelFormat::GREEN] = g;
                        dst.m_ptr[wxNativePixelFormat::BLUE] = b;
                        src++;
                        dst++;
                    }
                }

				// Use the magenta background, to calculate mask
                for( int row=0; row<pix; row++ )
                {
                    dst.MoveTo(pixels_dst, x, y+row );
                    for( int col=0; col<pix; col++ )
                    {
                        byte r = dst.Red();
                        byte g = dst.Green();
                        byte b = dst.Blue();
                        bool magenta = (r==255 && g==0 && b==255);
                        str += (magenta?'0':'1');
                        dst++;
                    }
                }

				// Make a string of similar format as the mask, to represent distance from nearest magenta pixel
				//  mask: "0000011111111...." '0' = magenta, '1' = not magenta
				//  dist: "0000012345678...." '0' = magenta, '1'-'9'(max) = distance from nearest magenta pixel, up or down

				/*	The dist string for a white king 'K' ends up like this;

			  	   "0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000111111111111111111110000000000000000000000000
					0000000000000000000000000122222222222222222210000000000000000000000000
					0000000000000000000000000123333333333333333210000000000000000000000000
					0000000000000000000000000123444444444444443210000000000000000000000000
					0000000000000000000000000123455555555555543210000000000000000000000000
					0000000000000000000000000123456666666666543210000000000000000000000000
					0000000000000000000000000123456777777776543210000000000000000000000000
					0000000000000000000000000123456788888876543210000000000000000000000000
					0000000000000000000000000123456789999876543210000000000000000000000000
					0000000000000000000000000123456789999876543210000000000000000000000000
					0000000000011111111100000123456789999876543210000011111111100000000000
					0000000011122222222211000123456789999876543210001122222222211100000000
					0000000122233333333322110123456789999876543210112233333333322210000000
					0000001233344444444433221234567899999987654321223344444444433321000000
					0000012344455555555544332345678999999998765432334455555555544432100000
					0000123455566666666655443456789999999999876543445566666666655543210000
					0000123456677777777766554567899999999999987654556677777777766543210000
					0001234567788888888877665678999999999999998765667788888888877654321000
					0001234567899999999988776789999999999999999876778899999999987654321000
					0001234567899999999999887899999999999999999987889999999999987654321000
					0001234567899999999999998999999999999999999998999999999999987654321000
					0001234567899999999999999999999999999999999999999999999999987654321000
					0001234567899999999999999999999999999999999999999999999999987654321000
					0001234567899999999999999999999999999999999999999999999999987654321000
					0001234567899999999999999999999999999999999999999999999999987654321000
					0001234567899999999999999999999999999999999999999999999999987654321000
					0000123456789999999999999999999999999999999999999999999999876543210000
					0000123456789999999999999999999999999999999999999999999999876543210000
					0000012345678999999999999999999999999999999999999999999999876543210000
					0000012345678999999999999999999999999999999999999999999998765432100000
					0000012345678999999999999999999999999999999999999999999998765432100000
					0000001234567899999999999999999999999999999999999999999987654321000000
					0000000123456789999999999999999999999999999999999999999876543210000000
					0000000123456789999999999999999999999999999999999999999876543210000000
					0000000012345678999999999999999999999999999999999999998765432100000000
					0000000001234567899999999999999999999999999999999999987654321000000000
					0000000000123456789999999999999999999999999999999999876543210000000000
					0000000000012345678999999999999999999999999999999998765432100000000000
					0000000000001234567899999999999999999999999999999987654321000000000000
					0000000000000123456789999999999999999999999999999876543210000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678999999999999999999999999998765432100000000000000
					0000000000000012345678888888899999999999988888888765432100000000000000
					0000000000000012345667777777788888888888877777777665432100000000000000
					0000000000000012344556666666677777777777766666666554432100000000000000
					0000000000000001233445555555566666666666655555555443321000000000000000
					0000000000000000122334444444455555555555544444444332210000000000000000
					0000000000000000011223333333344444444444433333333221100000000000000000
					0000000000000000000112222222233333333333322222222110000000000000000000
					0000000000000000000001111111122222222222211111111000000000000000000000
					0000000000000000000000000000011111111111100000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000
					0000000000000000000000000000000000000000000000000000000000000000000000"
					*/

				std::string dist = str;
                for( int row=0; row<pix; row++ )	// Left to right
				{
					char dh='0'; // distance from magenta
                    for( int col=0; col<pix; col++ )
					{
						int offset = row*pix + col;
						char ch = str[offset];
						if( ch == '0' ) // magenta ?
							dh = '0';
						else
							dh = (dh<'9'?dh+1:'9');
						dist[offset] = dh;
					}
				}
                for( int row=0; row<pix; row++ )   // then right to left
				{
					char dh='0'; // distance from magenta
                    for( int col=pix-1; col>=0; col-- )
					{
						int offset = row*pix + col;
						char ch = str[offset];
						if( ch == '0' ) // magenta ?
							dh = '0';
						else
						{
							dh = (dh<'9'?dh+1:'9');
							if( dist[offset] < dh ) // take minimum value
								dh = dist[offset];
						}
						dist[offset] = dh;
					}
				}
                for( int col=0; col<pix; col++ )	// then up to down
				{
					char dh='0'; // distance from magenta
	                for( int row=0; row<pix; row++ )
					{
						int offset = row*pix + col;
						char ch = str[offset];
						if( ch == '0' ) // magenta ?
							dh = '0';
						else
						{
							dh = (dh<'9'?dh+1:'9');
							if( dist[offset] < dh ) // take minimum value
								dh = dist[offset];
						}
						dist[offset] = dh;
					}
				}
                for( int col=0; col<pix; col++ )	// then down to up
				{
					char dh='0'; // distance from magenta
	                for( int row=pix-1; row>=0; row-- )
					{
						int offset = row*pix + col;
						char ch = str[offset];
						if( ch == '0' ) // magenta ?
							dh = '0';
						else
						{
							dh = (dh<'9'?dh+1:'9');
							if( dist[offset] < dh ) // take minimum value
								dh = dist[offset];
						}
						dist[offset] = dh;
					}
				}

				// Save the mask, in fact we now use the dist as the mask, so that the extra information
				//  is available for anti-aliasing purposes
                *p_str = dist;
                *p_mask = p_str->c_str();

				// Adjust greyscale anti-alias values near the magenta background, squeeze them from
				//  range bright white to black into range grey to black - this avoids a nasty white
				//  jaggy around every piece - it originates from the original .pdf render which assumes
				//  a white background
           /*   This was the old approach - we now adjust anti-aliasing for each background colour
				separately and store the results in the "box"
				for( int row=0; row<pix; row++ )
                {
                    dst.MoveTo(pixels_dst, x, y+row );
                    for( int col=0; col<pix; col++ )
                    {
                        byte r = dst.Red();
                        byte g = dst.Green();
                        byte b = dst.Blue();
						dst.m_ptr[2] = r;
						dst.m_ptr[1] = g;
						dst.m_ptr[0] = b;
                        dst++;
                    }
                } */

				// Show mask and dist strings during debug
		     /* if( c == 'K'  )
                {
                    for( size_t i=0; i<str.length(); i++ )
                    {
                        cprintf( "%c", str[i] );
                        if( ((i+1) % pix) == 0 )
                            cprintf("\n");
                    }
                    cprintf("\n");
                    for( size_t i=0; i<dist.length(); i++ )
                    {
                        cprintf( "%c", dist[i] );
                        if( ((i+1) % pix) == 0 )
                            cprintf("\n");
                    }
                }  */  

                // reselect bitmap
                dc.SelectObject(new_chess_board_bmp);

            }
            dark = !dark;
            x += pix;
        }
        y += pix;
    }

    wxNativePixelData bmdata(new_chess_board_bmp);
    height = pix*8;
    width  = pix*8;
    width_bytes = density*width;
	
   	// Allocate an image of the board
    if( buf_board )
        delete buf_board;
	buf_board = new byte[width_bytes*height];
    memset( buf_board, 0, width_bytes*height);
	
	// Allocate an image of the box (pieces off the board)
    if( buf_box )
        delete buf_box;
	buf_box   = new byte[width_bytes*height];

	// Read the initial position displayed on the bitmap
    wxNativePixelData::Iterator p(bmdata);
    byte *dst = buf_board;
    for( unsigned int row=0; row<height; row++ )
    {
        p.MoveTo(bmdata, 0, row );
        for( unsigned int col=0; col<width; col++ )
        {
            *dst++ = p.Red();
            *dst++ = p.Green();
            *dst++ = p.Blue();
            p++;
        }
    }

	// Read from position on left below (our looked up .bmp) into the box
	//  at right. The box has all the piece/colour combinations we need
    //  (eg only two pawns from each side are needed, one of each colour).
    //  1,2,3 and 4 are empty squares used to setup colours.
	//	
    //  On the right squares marked with * are empty squares.
	//
	//	"rNbQkBnR"		 "rnbqkbnr"	 <- normal colours
	//	"Pp1qK234"		 "pp*qk   "	 <- normal colours
	//	"        "		 "rnbqkbnr"	 <- highlight colours
	//	"        "		 "pp*qk   "	 <- highlight colours
	//	"        "		 "PP*QK   "	 <- highlight colours
	//	"        "		 "RNBQKBNR"	 <- highlight colours
	//	"        "		 "PP*QK   "	 <- normal colours
	//	"        "		 "RNBQKBNR"  <- normal
	//
	// Store these pieces from the board to their fixed
	//  positions in the 'box'

	// Copy empty light square to all used normal light squares in the box
	//  Note that '1'=c7, '2'=f7, '3'=g7, '4'=h7 are dark, highlight light
	//  highlight dark, and  highlight line (ocean blue by default) colours
	//  respectively and that the first six rows of the board (eg a6) are
	//  initialised to light colour
	Get( 'a','6', 'a','8' );  
	Get( 'a','6', 'c','8' );
	Get( 'a','6', 'e','8' );
	Get( 'a','6', 'g','8' );
	Get( 'a','6', 'b','7' );
	Get( 'a','6', 'd','7' );
	Get( 'a','6', 'a','2' );
	Get( 'a','6', 'c','2' );
	Get( 'a','6', 'e','2' );
	Get( 'a','6', 'b','1' );
	Get( 'a','6', 'd','1' );
	Get( 'a','6', 'f','1' );
	Get( 'a','6', 'h','1' );

	// Copy empty dark square to all used normal dark squares in the box
	Get( 'c','7', 'b','8' );
	Get( 'c','7', 'd','8' );
	Get( 'c','7', 'f','8' );
	Get( 'c','7', 'h','8' );
	Get( 'c','7', 'a','7' );
	Get( 'c','7', 'c','7' );
	Get( 'c','7', 'e','7' );
	Get( 'c','7', 'b','2' );
	Get( 'c','7', 'd','2' );
	Get( 'c','7', 'a','1' );
	Get( 'c','7', 'c','1' );
	Get( 'c','7', 'e','1' );
	Get( 'c','7', 'g','1' );

	// Copy empty highlight light square to all used highlight light squares in the box
	Get( 'f','7', 'a','6' );  
	Get( 'f','7', 'c','6' );
	Get( 'f','7', 'e','6' );
	Get( 'f','7', 'g','6' );
	Get( 'f','7', 'b','5' );
	Get( 'f','7', 'd','5' );
	Get( 'f','7', 'a','4' );
	Get( 'f','7', 'c','4' );
	Get( 'f','7', 'e','4' );
	Get( 'f','7', 'b','3' );
	Get( 'f','7', 'd','3' );
	Get( 'f','7', 'f','3' );
	Get( 'f','7', 'h','3' );

	// Copy empty highlight dark square to all used highlight dark squares in the box
	Get( 'g','7', 'b','6' );
	Get( 'g','7', 'd','6' );
	Get( 'g','7', 'f','6' );
	Get( 'g','7', 'h','6' );
	Get( 'g','7', 'a','5' );
	Get( 'g','7', 'c','5' );
	Get( 'g','7', 'e','5' );
	Get( 'g','7', 'b','4' );
	Get( 'g','7', 'd','4' );
	Get( 'g','7', 'a','3' );
	Get( 'g','7', 'c','3' );
	Get( 'g','7', 'e','3' );
	Get( 'g','7', 'g','3' );

	// Copy white pieces to the box (normal)
	Get( 'h','8', 'h','1', white_rook_mask,		&light_colour );
	Get( 'b','8', 'g','1', white_knight_mask,	&dark_colour  );
	Get( 'f','8', 'f','1', white_bishop_mask,	&light_colour );
	Get( 'e','7', 'e','1', white_king_mask,		&dark_colour  );
	Get( 'e','7', 'e','2', white_king_mask,		&light_colour );		// White king on white square
	Get( 'd','8', 'd','1', white_queen_mask,	&light_colour );
	Get( 'd','8', 'd','2', white_queen_mask,	&dark_colour  );		// White queen on black square
	Get( 'f','8', 'c','1', white_bishop_mask,	&dark_colour  );
    Get( 'b','8', 'b','1', white_knight_mask,   &light_colour );
	Get( 'h','8', 'a','1', white_rook_mask,		&dark_colour  );
	Get( 'a','7', 'a','2', white_pawn_mask,		&light_colour );		// Only need pawns on a2, b2
	Get( 'a','7', 'b','2', white_pawn_mask,		&dark_colour  );

	// Copy white pieces to the box (highlight)
	Get( 'h','8', 'h','3', white_rook_mask,		&highlight_light_colour );
	Get( 'b','8', 'g','3', white_knight_mask,	&highlight_dark_colour  );
	Get( 'f','8', 'f','3', white_bishop_mask,	&highlight_light_colour );
	Get( 'e','7', 'e','3', white_king_mask,		&highlight_dark_colour  );
	Get( 'e','7', 'e','4', white_king_mask,		&highlight_light_colour );		// White king on white square
	Get( 'd','8', 'd','3', white_queen_mask,	&highlight_light_colour );
	Get( 'd','8', 'd','4', white_queen_mask,	&highlight_dark_colour  );		// White queen on black square
	Get( 'f','8', 'c','3', white_bishop_mask,	&highlight_dark_colour  );
    Get( 'b','8', 'b','3', white_knight_mask,	&highlight_light_colour );
	Get( 'h','8', 'a','3', white_rook_mask,		&highlight_dark_colour  );
	Get( 'a','7', 'a','4', white_pawn_mask,		&highlight_light_colour );		// Only need pawns on a4, b4
	Get( 'a','7', 'b','4', white_pawn_mask,		&highlight_dark_colour  );

	// Copy black pieces to the box (normal)
	Get( 'a','8', 'h','8', black_rook_mask,		&dark_colour  );
	Get( 'g','8', 'g','8', black_knight_mask,	&light_colour );
	Get( 'c','8', 'f','8', black_bishop_mask,	&dark_colour  );
	Get( 'e','8', 'e','8', black_king_mask,		&light_colour );
	Get( 'e','8', 'e','7', black_king_mask,		&dark_colour  );		// Black king on black square
	Get( 'd','7', 'd','8', black_queen_mask,	&dark_colour  );
	Get( 'd','7', 'd','7', black_queen_mask,	&light_colour );		// Black queen on white square
	Get( 'c','8', 'c','8', black_bishop_mask,	&light_colour );
    Get( 'g','8', 'b','8', black_knight_mask,	&dark_colour  );
	Get( 'a','8', 'a','8', black_rook_mask,		&light_colour );
	Get( 'b','7', 'a','7', black_pawn_mask,		&dark_colour  );		// Only need pawns on a7, b7
	Get( 'b','7', 'b','7', black_pawn_mask,		&light_colour );

	// Copy black pieces to the box (highlight)
	Get( 'a','8', 'h','6', black_rook_mask,		&highlight_dark_colour  );
	Get( 'g','8', 'g','6', black_knight_mask,	&highlight_light_colour );
	Get( 'c','8', 'f','6', black_bishop_mask,	&highlight_dark_colour  );
	Get( 'e','8', 'e','6', black_king_mask,		&highlight_light_colour );
	Get( 'e','8', 'e','5', black_king_mask,		&highlight_dark_colour  );		// Black king on black square
	Get( 'd','7', 'd','6', black_queen_mask,	&highlight_dark_colour  );
	Get( 'd','7', 'd','5', black_queen_mask,	&highlight_light_colour );		// Black queen on white square
	Get( 'c','8', 'c','6', black_bishop_mask,	&highlight_light_colour );
    Get( 'g','8', 'b','6', black_knight_mask,	&highlight_dark_colour  );
	Get( 'a','8', 'a','6', black_rook_mask,		&highlight_light_colour );
	Get( 'b','7', 'a','5', black_pawn_mask,		&highlight_dark_colour  );		// Only need pawns on a5, b5
	Get( 'b','7', 'b','5', black_pawn_mask,		&highlight_light_colour );

	// Empty squares
	Get( 'a','6', 'c','2' );  // empty light square
	Get( 'c','7', 'c','7' );  // empty dark square
	Get( 'f','7', 'c','4' );  // empty highlight light square
	Get( 'g','7', 'c','5' );  // empty highlight dark square

	// Highlight line colour in h5
	Get( 'h','7', 'h','5' );  // ocean blue by default
    chess_board_bmp = new_chess_board_bmp;

	// Setup the position
	SetChessPosition( cp, highlight );
}

// Setup a position	on the graphic board
void ChessBoardBitmap::SetChessPosition( const thc::ChessPosition &pos, const bool *highlight )
{
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
    const bool *highlight_ptr = highlight;
    const char *piece_ptr = pos.squares;
    pos_current = pos;
	if( highlight_ptr )
		memcpy( highlight_current, highlight_ptr, sizeof(highlight_current) );
	else
		memset( highlight_current, 0, sizeof(highlight_current) );
	int high1 = static_cast<int>( highlight_square1 );
	int high2 = static_cast<int>( highlight_square2 );
	if( 0<=high1 && high1<64 && 0<=high2 && high2<64 && highlight_ptr!=highlight_current )
	{
		highlight_current[high1] = true;
		highlight_current[high2] = true;
	}
	highlight_ptr = highlight_current;
	if( highlight_mode == none )
		highlight_ptr = 0;

	// Read string backwards for black at bottom
	if( !normal_orientation )
	{
		piece_ptr += 63;
        if( highlight_ptr )
            highlight_ptr += 63;
	}

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
		char piece = *piece_ptr;
        bool highlight_f = highlight_ptr ? *highlight_ptr : false;
		if( normal_orientation )
        {
			piece_ptr++;
            if( highlight_ptr )
                highlight_ptr++;
        }
        else
        {
			piece_ptr--;
            if( highlight_ptr )
                highlight_ptr--;
        }

		// Is the square dark ?
		if( *colour++ == 'b' )
		{

			// Find an appropriate dark square piece in the box
			if( highlight_f && highlight_mode==square_colour )
			{
				highlight_f = false;	// stops line highlighting as well
				switch( piece )
				{
					default:	src_file='c';	src_rank='5';	break;
					case 'R':	src_file='a';	src_rank='3';	break;
					case 'N':	src_file='g';	src_rank='3';	break;
					case 'B':	src_file='c';	src_rank='3';	break;
					case 'Q':	src_file='d';	src_rank='4';	break;
					case 'K':	src_file='e';	src_rank='3';	break;
					case 'P':   src_file='b';	src_rank='4';	break;
					case 'r':	src_file='h';	src_rank='6';	break;
					case 'n':   src_file='b';	src_rank='6';	break;
					case 'b':   src_file='f';	src_rank='6';	break;
					case 'q':   src_file='d';	src_rank='6';	break;
					case 'k':   src_file='e';	src_rank='5';	break;
					case 'p':   src_file='a';	src_rank='5';	break;
				}
			}
			else
			{
				switch( piece )
				{
					default:	src_file='c';	src_rank='7';	break;
					case 'R':	src_file='a';	src_rank='1';	break;
					case 'N':	src_file='g';	src_rank='1';	break;
					case 'B':	src_file='c';	src_rank='1';	break;
					case 'Q':	src_file='d';	src_rank='2';	break;
					case 'K':	src_file='e';	src_rank='1';	break;
					case 'P':   src_file='b';	src_rank='2';	break;
					case 'r':	src_file='h';	src_rank='8';	break;
					case 'n':   src_file='b';	src_rank='8';	break;
					case 'b':   src_file='f';	src_rank='8';	break;
					case 'q':   src_file='d';	src_rank='8';	break;
					case 'k':   src_file='e';	src_rank='7';	break;
					case 'p':   src_file='a';	src_rank='7';	break;
				}
			}
		}
		else
		{
			
			// Find an appropriate light square piece in the box
			if( highlight_f && highlight_mode==square_colour )
			{
				highlight_f = false;	// stops line highlighting as well
				switch(piece)
				{
					default:	src_file='c';	src_rank='4';	break;
					case 'R':	src_file='h';	src_rank='3';	break;
					case 'N':	src_file='b';	src_rank='3';	break;
					case 'B':	src_file='f';	src_rank='3';	break;
					case 'Q':	src_file='d';	src_rank='3';	break;
					case 'K':	src_file='e';	src_rank='4';	break;
					case 'P':   src_file='a';	src_rank='4';	break;
					case 'r':	src_file='a';	src_rank='6';	break;
					case 'n':   src_file='g';	src_rank='6';	break;
					case 'b':   src_file='c';	src_rank='6';	break;
					case 'q':   src_file='d';	src_rank='5';	break;
					case 'k':   src_file='e';	src_rank='6';	break;
					case 'p':   src_file='b';	src_rank='5';	break;
				}
			}
			else
			{
				switch(piece)
				{
					default:	src_file='c';	src_rank='2';	break;
					case 'R':	src_file='h';	src_rank='1';	break;
					case 'N':	src_file='b';	src_rank='1';	break;
					case 'B':	src_file='f';	src_rank='1';	break;
					case 'Q':	src_file='d';	src_rank='1';	break;
					case 'K':	src_file='e';	src_rank='2';	break;
					case 'P':   src_file='a';	src_rank='2';	break;
					case 'r':	src_file='a';	src_rank='8';	break;
					case 'n':   src_file='g';	src_rank='8';	break;
					case 'b':   src_file='c';	src_rank='8';	break;
					case 'q':   src_file='d';	src_rank='7';	break;
					case 'k':   src_file='e';	src_rank='8';	break;
					case 'p':   src_file='b';	src_rank='7';	break;
				}
			}
		}
	    Put( src_file, src_rank, dst_file, dst_rank, highlight_f );
	}

	// Copy from the image buffer into the wxBitmap
    wxNativePixelData bmdata(chess_board_bmp);
    wxNativePixelData::Iterator p(bmdata);
    byte *src = buf_board;

	// Some fairly brutal and simple code to generate icon xpm strings for Linux icon in main.cpp
	//#define GENERATE_ICON_XPM_FILE
	#define WHOLE_BOARD  //instead generate whole board
	#ifdef GENERATE_ICON_XPM_FILE
	static bool once[5000];
	FILE *f=NULL;
	if( !once[width] )
	{
		char name[80];
#ifdef WHOLE_BOARD
		sprintf( name, "whole-board-%d-%d.xpm", width, height );
#else
		sprintf( name, "icon-basis.xpm" );
#endif
		f = fopen(name,"wt");
	}
	unsigned char red[256];
	unsigned char green[256];
	unsigned char blue[256];
	char ch[256];
	for( int i=0, c=' '; i<sizeof(ch); i++, c++ )
	{
		while( c=='"' || c=='\\' || c=='\'' )
			c++;
		ch[i] = c;
	}
	int nbr_colours=0;
	std::string xpm;
	#endif	// #ifdef GENERATE_ICON_XPM_FILE

	for( unsigned int row=0; row<height; row++ )
	{
		p.MoveTo(bmdata, 0, row );
		for( unsigned int col=0; col<width; col++ )
		{
			unsigned char r = src[0];
			unsigned char g = src[1];
			unsigned char b = src[2];
			src += 3;
			p.Red()   = r; 
			p.Green() = g; 
			p.Blue()  = b; 
			p++;
	#ifndef GENERATE_ICON_XPM_FILE
		}
	}
	#else
			bool found=false;
			char c = 0;
			for( int i=0; !found && i<nbr_colours; i++ )
			{
				if( r==red[i] && g==green[i] && b==blue[i] )
				{
					c = ch[i];
					found = true;
				}
			}
			if( !found )
			{
				red[nbr_colours]=r;
				green[nbr_colours]=g;
				blue[nbr_colours]=b;
				c = ch[nbr_colours];
				if( nbr_colours<255 )
					nbr_colours++;
			}
			// y = first 2/8 of board, x = 5/8 of board => gives f8 = Black bishop on dark square and
			//   f7 = Black pawn on light square
#ifdef WHOLE_BOARD
			if( !once[width] ) // && col>(width*5)/8-10 && col<(width*6)/8+10 && row<(height*2)/8+10 && f )
				xpm += c;
		}
		if( !once[width] ) //&& row<(height*2)/8+10 )
		{
			xpm += "\"";
			if( row+1 < height )
				xpm += ",\n\"";
			else
				xpm += "\n";
		}
#else
			if( !once[width] && col>(width*5)/8-10 && col<(width*6)/8+10 && row<(height*2)/8+10 && f )
				xpm += c;
		}
		if( !once[width] && row<(height*2)/8+10 )
		{
			xpm += "\"";
			if( row+1 < (height*2)/8+10 )
				xpm += ",\n\"";
			else
				xpm += "\n";
		}
#endif
	}
	if( !once[width] && f)
	{
		fputs( "static const char *icon_xpm[] = {\n", f );
		fputs( "/* columns rows colors chars-per-pixel */\n", f );
#ifdef WHOLE_BOARD
		fprintf( f, "\"%lu %lu %d 1\",\n", width, height, nbr_colours );
#else
		fprintf( f, "\"%lu %lu %d 1\",\n", width/8+20-1, (height*2)/8+10, nbr_colours );
#endif
		for( int i=0; i<nbr_colours; i++ )
		{
			fprintf( f, "\"%c c #%02x%02x%02x\",\n", ch[i], red[i]&0xff, green[i]&0xff, blue[i]&0xff );
		}
		fputs( "\"", f );
		fputs( xpm.c_str(), f );
		fputs( "};\n", f );
		fclose(f);
	}
	once[width] = true;
	#endif // #ifdef GENERATE_ICON_XPM_FILE

	// Copy the chess board bitmap into the centre part of the board setup bitmap
	if( is_board_setup && ok_to_copy_chess_board_to_board_setup )
		BmpCopy( NULL, chess_board_bmp, 0, 0, board_setup_bmp, dim_board.x, dim_board.y, 8*dim_pix, 8*dim_pix );
}

// Calculate an offset into the wxBitmap's image buffer
unsigned long ChessBoardBitmap::Offset( char file, char rank )
{
	byte col  = file-'a';		     // 0-7 => file a-h
	byte row  = 7-(rank-'1');		 // 0-7 => rank 8-1
	unsigned long offset =						
        ( row * (height/8) ) * width_bytes
	  +	( col * (width_bytes/8) );
	return( offset );	
}

// Get a piece from board, put into box
void ChessBoardBitmap::Get( char src_file, char src_rank, char dst_file, char dst_rank, const char *mask, const wxColour *background_colour )
{
	int r_background = background_colour ? background_colour->Red()   : 0;
	int g_background = background_colour ? background_colour->Green() : 0;
	int b_background = background_colour ? background_colour->Blue()  : 0;
	unsigned int i, j;
	byte *src, *dst;
	char edge_thickness = (width/8 < 140 ? '3' : '4');	// Used to be fixed at '3', but this worried me for some reason
	for( i=0; i < height/8; i++ )
	{
		src = buf_board + Offset(src_file,src_rank) + i*width_bytes;
		dst = buf_box   + Offset(dst_file,dst_rank) + i*width_bytes;
		for( j=0; j < width/8; j++ )
        {
            if( !mask  )
			{
			    *dst++ = *src++;
			    *dst++ = *src++;
			    *dst++ = *src++;
			}
            else if( *mask == '0' )
			{
				dst += 3;
				src += 3;
			}
            else
            {
                char ch = *mask;
                byte r = *src++;
                byte g = *src++;
                byte b = *src++;
				if( r==g && r==b )	// if grey scale - r==g==b
				{
					int adjust = r;
					if( 0<adjust && adjust<255 ) // if not already black or white
					{
						if( ch < edge_thickness )	// only if near magenta background - retain full
													// anti-aliasing dynamic range inside the piece
						{
							// the value of adjust represents a greyscale colour somewhere between
							//  0 (black) and 255 (white) - make an equivalently scaled version of
							//  the background colour 
							r = (adjust*r_background) / 255;
							g = (adjust*g_background) / 255;
							b = (adjust*b_background) / 255;
						}
					}
				}
			    *dst++ = r;
			    *dst++ = g;
			    *dst++ = b;
            }
            if( mask )
				mask++;
        }
	}
}

// Put a piece from box onto board
void ChessBoardBitmap::Put( char src_file, char src_rank, char dst_file, char dst_rank, bool highlight_f )
{
	unsigned int i, j, k;
	const byte *src;
	byte *dst, *src_blue;
	if( !highlight_f )	// optimisation - much simpler
	{
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
		for( i=0; i < height/8; i++ )
		{
			src			  =
				(byte *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			src_blue     =
				(byte *)(buf_box   + Offset('h','5')           + i*width_bytes);
			dst			  =
				(byte *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
			for( j=0; j < (width_bytes)/(8*density); j++ )
			{
				bool yhit =         (highlight_y_lo1<=i && i<highlight_y_lo2) ||
									(highlight_y_hi1<=i && i<highlight_y_hi2);
				bool xhit =         (highlight_x_lo1<=j && j<highlight_x_lo2) ||
									(highlight_x_hi1<=j && j<highlight_x_hi2);
				bool yrange =       (highlight_y_lo1<=i && i<highlight_y_hi2);
				bool xrange =       (highlight_x_lo1<=j && j<highlight_x_hi2);
				bool blue =         highlight_f && ((xhit && yrange) || (yhit && xrange));
				for( k=0; k<density; k++ )
				{
					uint8_t dat = blue ? *src_blue : dat = *src;
       				*dst++ = dat;
					src++;
					src_blue++;
				}
			}
		}
	}
}


// Setup a position	on the graphic board with one piece being dragged
void ChessBoardBitmap::SetChessPositionShiftedPiece( const thc::ChessPosition &pos, bool blank_other_squares, char pickup_file_, char pickup_rank_, wxPoint shift )
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
    const char *piece_ptr = pos.squares;
	if( !normal_orientation )
		piece_ptr += 63;

	// Loop through all squares
	for( int i=0; i<64; i++ )
	{
		dst_file='a'+file;
		dst_rank='1'+rank;

		// Find the piece occupying this square
		if( normal_orientation )
			piece = *piece_ptr++;
		else
			piece = *piece_ptr--;

        // Is this square highlighted ? Does it contain a picked up piece ?
		if( normal_orientation )
		{
            if( dst_file==pickup_file_ && dst_rank==pickup_rank_ )
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
            if( rev_file==pickup_file_ && rev_rank==pickup_rank_ )
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

		// Is the square dark ?
		if( *colour++ == 'b' )
		{

			// Find an appropriate dark square piece in the box
			switch( piece )
			{
				default:	src_file='c';	src_rank='7';	break;
				case 'R':	src_file='a';	src_rank='1';	break;
				case 'N':	src_file='g';	src_rank='1';	break;
				case 'B':	src_file='c';	src_rank='1';	break;
				case 'Q':	src_file='d';	src_rank='2';	break;
				case 'K':	src_file='e';	src_rank='1';	break;
				case 'P':   src_file='b';	src_rank='2';	break;
				case 'r':	src_file='h';	src_rank='8';	break;
				case 'n':   src_file='b';	src_rank='8';	break;
				case 'b':   src_file='f';	src_rank='8';	break;
				case 'q':   src_file='d';	src_rank='8';	break;
				case 'k':   src_file='e';	src_rank='7';	break;
				case 'p':   src_file='a';	src_rank='7';	break;
			}
		}
		else
		{
			
			// Find an appropriate light square piece in the box
			switch(piece)
			{
				default:	src_file='c';	src_rank='2';	break;
				case 'R':	src_file='h';	src_rank='1';	break;
				case 'N':	src_file='b';	src_rank='1';	break;
				case 'B':	src_file='f';	src_rank='1';	break;
				case 'Q':	src_file='d';	src_rank='1';	break;
				case 'K':	src_file='e';	src_rank='2';	break;
				case 'P':   src_file='a';	src_rank='2';	break;
				case 'r':	src_file='a';	src_rank='8';	break;
				case 'n':   src_file='g';	src_rank='8';	break;
				case 'b':   src_file='c';	src_rank='8';	break;
				case 'q':   src_file='d';	src_rank='7';	break;
				case 'k':   src_file='e';	src_rank='8';	break;
				case 'p':   src_file='b';	src_rank='7';	break;
			}
		}

		// Copy from the box into the image buffer of the wxBitmap
		Put( src_file, src_rank, dst_file, dst_rank, false );
	}

    // Copy the picked up piece into place
    int row, col;
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
    wxNativePixelData bmdata(chess_board_bmp);
    wxNativePixelData::Iterator p(bmdata);
    byte *src = buf_board;
    for( unsigned int row2=0; row2<height; row2++ )
    {
        p.MoveTo(bmdata, 0, row2 );
        for( unsigned int col2=0; col2<width; col2++ )
        {
            p.Red()   = *src++; 
            p.Green() = *src++; 
            p.Blue()  = *src++; 
            p++;
        }
    }
}


// Put a shifted, masked piece from box onto board
void ChessBoardBitmap::PutEx( char piece,
						char dst_file, char dst_rank, wxPoint shift )
{
    char src_file, src_rank;
    const char *mask=black_rook_mask;
    
	// Find an appropriate white square piece in the box
	//  (Note we pick pieces from dark squares - pieces
	//  from light squares have a whitish jaggy around them
	//  on dark squares)
	switch(piece)
	{
		case 'R':	src_file='a';	src_rank='1';
                    mask = white_rook_mask;           	break;
		case 'N':	src_file='g';	src_rank='1';
                    mask = white_knight_mask;           break;
		case 'B':	src_file='c';	src_rank='1';
                    mask = white_bishop_mask;           break;
		case 'Q':	src_file='d';	src_rank='2';
                    mask = white_queen_mask;           	break;
		case 'K':	src_file='e';	src_rank='1';
                    mask = white_king_mask;           	break;
		case 'P':   src_file='b';	src_rank='2';
                    mask = white_pawn_mask;           	break;
		case 'r':	src_file='h';	src_rank='8';
                    mask = black_rook_mask;           	break;
		case 'n':   src_file='b';	src_rank='8';
                    mask = black_knight_mask;           break;
		case 'b':   src_file='f';	src_rank='8';
                    mask = black_bishop_mask;           break;
		case 'q':   src_file='d';	src_rank='8';
                    mask = black_queen_mask;           	break;
		case 'k':   src_file='e';	src_rank='7';
                    mask = black_king_mask;           	break;
		case 'p':   src_file='a';	src_rank='7';
                    mask = black_pawn_mask;           	break;
		default:	src_file='c';	src_rank='2';
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
                if( *mask++ != '0' )
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
                if( *mask++ != '0' )
                    *dst = *src;
                dst++;
                src++;
            }
		}
	}
}


class BitmapWindow: public wxWindow
{
    DECLARE_CLASS( BitmapWindow )
    DECLARE_EVENT_TABLE()
public:
	wxBitmap bm_;
    BitmapWindow( wxBitmap &bm, wxWindow* parent = NULL,
      wxWindowID id = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize )
		: wxWindow(parent,id,pos,size)
	{
		bm_ = bm;
	}
	void OnPaint( wxPaintEvent& WXUNUSED(event) );
};

// BitmapWindow type definition
IMPLEMENT_CLASS( BitmapWindow, wxWindow )

// BitmapWindow event table definition
BEGIN_EVENT_TABLE( BitmapWindow, wxWindow )
    EVT_PAINT( BitmapWindow::OnPaint )
END_EVENT_TABLE()

void BitmapWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if( bm_.Ok() )
	{
        dc.DrawBitmap( bm_, 0, 0, true );// board_rect.x, board_rect.y, true );
		cprintf( "bm_.Ok() is true\n" );
	}
	else
    {
		cprintf( "bm_.Ok() is false\n" );
    }
}



// TestbedDialog type definition
IMPLEMENT_CLASS( TestbedDialog, wxDialog )

// TestbedDialog event table definition
BEGIN_EVENT_TABLE( TestbedDialog, wxDialog )
    EVT_BUTTON( wxID_OK, TestbedDialog::OnOkClick )
END_EVENT_TABLE()

// TestbedDialog constructors
TestbedDialog::TestbedDialog( wxBitmap &bm, wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
	bm_ = bm;

    // We have to set extra styles before creating the dialog
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS/*|wxDIALOG_EX_CONTEXTHELP*/ );
    if( wxDialog::Create( parent, id, caption, pos, size, style ) )
    {
        CreateControls();

        // This fits the dialog to the minimum size dictated by the sizers
        GetSizer()->Fit(this);
        
        // This ensures that the dialog cannot be sized smaller than the minimum size
        GetSizer()->SetSizeHints(this);

        // Centre the dialog on the parent or (if none) screen
        Centre();
    }
}

#define SMALL 5
#define LARGE 2

// Control creation for TestbedDialog
void TestbedDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, SMALL);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, SMALL);

/*
   +-H1-------------+
   |+-V1--+  +-V2--+|
   ||+---+|  |+---+||
   |||   ||  ||   |||
   ||+---+|  |+---+||
   ||     |  |     ||
   ||+---+|  |+---+||
   |||   ||  ||   |||
   ||+---+|  |+---+||
   |+-----+  +-----+|
   +----------------+

   +-H2-------------+
   |+-----+  +-----+|
   ||     |  |     ||
   |+-----+  +-----+|
   +----------------+

          .....

   +-HN-------------+
   |+-----+  +-----+|
   ||     |  |     ||
   |+-----+  +-----+|
   +----------------+
*/

    //wxBoxSizer* H1 = new wxBoxSizer(wxHORIZONTAL);
    //wxBoxSizer* V1 = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *HV1 = new wxFlexGridSizer( 10, 2, 0, 0 );
    #define WIDTH 700
    #define HEIGHT 500
    wxSize sz=wxSize(WIDTH,HEIGHT);

    // Text control for prefix_txt
    //wxTextCtrl *prefix_ctrl = new wxTextCtrl ( this, wxID_ANY,  wxT(""), wxDefaultPosition, sz, wxTE_MULTILINE );
    //HV1->Add(prefix_ctrl, 0, wxGROW|wxALL, SMALL);
    BitmapWindow *bmw = new BitmapWindow( bm_, this, wxID_ANY,  wxDefaultPosition, sz );
    HV1->Add(bmw, 0, wxGROW|wxALL, SMALL);

    //H1->Add(V2, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);
    box_sizer->Add(HV1, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, SMALL);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);

    // The OK button
    wxButton* ok = new wxButton ( this, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, SMALL);

    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
        wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, SMALL);

    // The Help button
    wxButton* help = new wxButton( this, wxID_HELP, wxT("&Help"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, SMALL);
}


// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void TestbedDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    AcceptAndClose();
}




bool TestbedDialog::Run()
{
    bool ok=false;
	DialogDetect detect;		// an instance of DialogDetect as a local variable allows tracking of open dialogs
    if( wxID_OK == ShowModal() )
        ok = true;
    return ok;
}


#if 0
void Testbed()
{
    wxBitmap bm( board_setup_bitmap_xpm );
	TestbedDialog dialog(bm);
	dialog.Run();
}
#endif
#if 0
void Testbed()
{
	ChessBoardBitmap cbb;
	thc::ChessPosition cp;
	cbb.CreateAsBoardSetup(wxSize(364,294),true,cp);
	TestbedDialog dialog(*cbb.GetBoardSetupBmp());
	dialog.Run();
}
#endif
