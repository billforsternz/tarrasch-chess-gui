/****************************************************************************
 * Control - The graphic chess board that is the centre-piece of the GUI
 *  Still working on making it a more-self contained and reusable control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/rawbmp.h"
#include "wx/file.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "CompressedBitmaps.h"
#include "GraphicBoardResizable.h"
#include "GameLogic.h"
#include "thc.h"
#include "Objects.h"
#include "Repository.h"

// Initialise the graphic board
GraphicBoardResizable::GraphicBoardResizable
(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& point,
    const wxSize& size
)   : wxControl( parent, id, point, size, wxBORDER_NONE ) //wxBORDER_SIMPLE ) //wxBORDER_NONE /*BORDER_COMMON*/ )
            // for the moment we get a strange artifact effect on resize sometimes unless wxBORDER_NONE
{
    pickup_file    = 0;
    pickup_rank    = 0;
    pickup_point.x = 0;
    pickup_point.y = 0;
    sliding = false;
    buf_board = 0;
    buf_box = 0;
    strcpy( _position_ascii, "rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR" );
    Init( size );
}

GraphicBoardResizable::GraphicBoardResizable( int pix_ )
{
    pickup_file    = 0;
    pickup_rank    = 0;
    pickup_point.x = 0;
    pickup_point.y = 0;
    sliding = false;
    buf_board = 0;
    buf_box = 0;
    strcpy( _position_ascii, "rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR" );
	wxSize size( pix_*8, pix_*8 );
	Init( size );
	SetChessPosition( 
        "rnbqnrk "
        "pp    b "
        "    N   "
        "   qk   "
        "   QK   "
        "        "
        "PP      "
        "RNBQKBNR"
	);
}

void GraphicBoardResizable::Init
(
    const wxSize& size
)

{
	current_size = size;
	int r1 = objs.repository->general.m_light_colour_r;
	int g1 = objs.repository->general.m_light_colour_g;
	int b1 = objs.repository->general.m_light_colour_b;
	wxColour c1(r1,g1,b1);
	light_colour = c1;
	int r2 = objs.repository->general.m_dark_colour_r;
	int g2 = objs.repository->general.m_dark_colour_g;
	int b2 = objs.repository->general.m_dark_colour_b;
	wxColour c2(r2,g2,b2);
	dark_colour = c2;
    const char **xpm=0;
    int min = size.x<size.y ? size.x : size.y;
    pix = min/8;
    wxBitmap my_chess_bmp_;
    my_chess_bmp_.Create(pix*8,pix*8,24);
    int pitch;
    const CompressedXpm *compressed_xpm = GetBestFit( pix, pitch );
    wxMemoryDC dc;
    dc.SelectObject(my_chess_bmp_);
    dc.SetMapMode(wxMM_TEXT);
    CompressedXpmProcessor processor(compressed_xpm);
    xpm = (const char **)processor.GetXpm();
    wxBitmap lookup(xpm);

    wxColour legacy_dark(200,200,200);
    wxColour magic(255,0,255);
    wxBrush light_brush(light_colour);
    dc.SetBrush( light_brush );
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0,0,my_chess_bmp_.GetWidth(),my_chess_bmp_.GetHeight());
    //
    int x = 0;
    int y = 0;
    const char *board[] =
    {
        "rNbQkBnR",
        "Pp#qK   ",
    };
    dc.SetBackgroundMode( wxPENSTYLE_TRANSPARENT );
    for( int board_idx=0; board_idx<2; board_idx++ )
    {
        const char *s = board[board_idx];
        bool dark = ((board_idx&1)==1);
        x = 0;
        for( int j=0; j<8; j++ )
        {
            char c = *s++;
            if( c == '#' )  // dark square
            {
                wxBrush dark_brush(dark_colour);
                dc.SetBrush( dark_brush );
                dc.DrawRectangle(x<0?0:x, y, pix, pix );
            }
            else if( c == ' ' )  // light square
            {
                // whole board already initialised to light colour
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
                wxNativePixelData pixels_src(lookup);
                wxNativePixelData::Iterator src(pixels_src);
                wxNativePixelData pixels_dst(my_chess_bmp_);
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
                        dst.m_ptr[2] = r;
                        dst.m_ptr[1] = g;
                        dst.m_ptr[0] = b;
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

                // Save the mask
                *p_str = str;
                *p_mask = p_str->c_str();

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

				// Adjust greyscale anti-alias values near the magenta background, squeeze them from
				//  range bright white to black into range grey to black - this avoids a nasty white
				//  jaggy around every piece - it originates from the original .pdf render which assumes
				//  a white background
                for( int row=0; row<pix; row++ )
                {
                    dst.MoveTo(pixels_dst, x, y+row );
                    for( int col=0; col<pix; col++ )
                    {
                        byte r = dst.Red();
                        byte g = dst.Green();
                        byte b = dst.Blue();
						if( r==g && r==b )
						{
							int adjust = r;
							if( 0<adjust && adjust<255 ) // if not already black or white
							{
								int offset = row*pix + col;
								char ch = dist[offset];
								if( ch < '3' )	// only if near magenta background - retain full
												// anti-aliasing dynamic range inside the piece
								{
									//adjust /= 2;  // make near white (255)  grey (128)
									adjust = (adjust * 80) / 128;	// experimental 64 = 50% is too much, 92 = 75% not enough ?
									r = static_cast<int>(adjust);
									g = r;
									b = r;
									#if 1  // Disable this only if you want a bad look!
									dst.m_ptr[2] = r;
									dst.m_ptr[1] = g;
									dst.m_ptr[0] = b;
									#endif
								}
							}
                        }
                        dst++;
                    }
                }

				// Show mask and dist strings during debug
		     /*	if( c == 'K'  )
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
                } */
            }
            dark = !dark;
            x += pix;
        }
        y += pix;
    }

    wxNativePixelData bmdata(my_chess_bmp_);
    height = bmdata.GetHeight();
    width  = bmdata.GetWidth();
    int row_stride = bmdata.GetRowStride();
    if( row_stride < 0 )
        row_stride = 0-row_stride;
    width_bytes = row_stride;
    if( width )
	    density  = width_bytes/width;
	xborder	     = (width_bytes%8) / 2;
	yborder	     = (height%8) / 2;

    dc.SetBrush( *wxBLUE_BRUSH );
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBackgroundMode( wxPENSTYLE_SOLID );
    //dc.DrawRectangle(pix*3-2, pix*3-2, 10, 10 );
    //bool ok = dc.FloodFill( pix*3, pix*3, *wxBLACK, wxFLOOD_BORDER );
    //cprintf( "FloodFill returns %s\n", ok?"true":"false" );

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

	// Orientation
	normal_orientation = true;

	// Highlights squares
	ClearHighlight1();
	ClearHighlight2();

	// The location of the pieces in the bitmap, a8 = top left, c7=#
	// is a dark square, the rest of the board is light coloured, the
	// background colour of the 12 squares with pieces is irrelevant,
	// we use the mask to eliminate the background
    // "rNbQkBnR",
    // "Pp#qK",

	// The box, colours are as per a normal chess board h1=white etc
	// "rnbqkbnr"
	// "pp......"
	// "........"
	// "...qk..."
	// "...QK..."
	// "........"
	// "PP......"
	// "RNBQKBNR"

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

	// Copy empty black square to all used black squares in the box
	Get( 'c','7', 'b','8' );
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

	// Copy empty white square to all used white squares in the box
	Get( 'f','7', 'a','8' );  
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

	// Copy white pieces to the box
	Get( 'h','8', 'h','1', white_rook_mask );
	Get( 'b','8', 'g','1', white_knight_mask );
	Get( 'f','8', 'f','1', white_bishop_mask );
	Get( 'e','7', 'e','1', white_king_mask );
	Get( 'e','7', 'e','4', white_king_mask  );		// White king on white square
	Get( 'd','8', 'd','1', white_queen_mask );
	Get( 'd','8', 'd','4', white_queen_mask );		// White queen on black square
	Get( 'f','8', 'c','1', white_bishop_mask );
    Get( 'b','8', 'b','1', white_knight_mask );
	Get( 'h','8', 'a','1', white_rook_mask );
	Get( 'a','7', 'a','2', white_pawn_mask );		// Only need pawns on a2, b2
	Get( 'a','7', 'b','2', white_pawn_mask );


	// Copy black pieces to the box
	Get( 'a','8', 'h','8', black_rook_mask );
	Get( 'g','8', 'g','8', black_knight_mask );
	Get( 'c','8', 'f','8', black_bishop_mask );
	Get( 'e','8', 'e','8', black_king_mask );
	Get( 'e','8', 'e','5', black_king_mask  );		// Black king on black square
	Get( 'd','7', 'd','8', black_queen_mask );
	Get( 'd','7', 'd','5', black_queen_mask );		// Black queen on white square
	Get( 'c','8', 'c','8', black_bishop_mask );
    Get( 'g','8', 'b','8', black_knight_mask );
	Get( 'a','8', 'a','8', black_rook_mask );
	Get( 'b','7', 'a','7', black_pawn_mask );		// Only need pawns on a7, b7
	Get( 'b','7', 'b','7', black_pawn_mask );

	// Two empty squares
	Get( 'c','7', 'a','3' );  // empty black square
	Get( 'f','7', 'b','3' );  // empty white square
    my_chess_bmp = my_chess_bmp_;
}

// Cleanup
GraphicBoardResizable::~GraphicBoardResizable()
{
	delete(buf_board);
	delete(buf_box);
}

void GraphicBoardResizable::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if( my_chess_bmp.Ok() )
    {
        dc.DrawBitmap( my_chess_bmp, 0, 0, true );// board_rect.x, board_rect.y, true );
    }
}

// Setup a position	on the graphic board
void GraphicBoardResizable::SetChessPosition( char *position_ascii )
{
#if 0 // Setup a custom position here
    #define CUSTOM_POSITION "rnbqnrk|pp4b|4N|3qk|3QK|8|PPxx|R1BQKBNR| w KQkq - 0 1"
    thc::ChessPosition pos;
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
	}

	// Copy from the image buffer into the wxBitmap
    wxNativePixelData bmdata(my_chess_bmp);
    wxNativePixelData::Iterator p(bmdata);
    byte *src = buf_board;
    for( unsigned int row=0; row<height; row++ )
    {
        p.MoveTo(bmdata, 0, row );
        for( unsigned int col=0; col<width; col++ )
        {
            p.Red()   = *src++; 
            p.Green() = *src++; 
            p.Blue()  = *src++; 
            p++;
        }
    }

    // Now use GDI to add highlights (removed)
}


// Draw the graphic board
void GraphicBoardResizable::Draw()
{
    Refresh(false);
    Update();
}

// Calculate an offset into the wxBitmap's image buffer
unsigned long GraphicBoardResizable::Offset( char file, char rank )
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
void GraphicBoardResizable::Get( char src_file, char src_rank, char dst_file, char dst_rank, const char *mask )
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
void GraphicBoardResizable::Put( char src_file, char src_rank, char dst_file, char dst_rank )
{
//	if( density != 4 )
//	{

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
#if 0
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
#endif
}


// Figure out which square is clicked on the board
void GraphicBoardResizable::HitTest( wxPoint hit, char &file, char &rank )
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
void GraphicBoardResizable::HitTestEx( char &file, char &rank, wxPoint shift )
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
void GraphicBoardResizable::SetPositionEx( thc::ChessPosition pos, bool blank_other_squares, char pickup_file_, char pickup_rank_, wxPoint shift )
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
    wxNativePixelData bmdata(my_chess_bmp);
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

    // Now use GDI to add highlights (removed)
}


// Put a shifted, masked piece from box onto board
void GraphicBoardResizable::PutEx( char piece,
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

void GraphicBoardResizable::OnMouseLeftDown( wxMouseEvent &event )
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

void GraphicBoardResizable::OnMouseMove( wxMouseEvent &event )
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

void GraphicBoardResizable::OnMouseLeftUp( wxMouseEvent &event )
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


void GraphicBoardResizable::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    //event.Skip(true);
    //event.StopPropagation();
}



void GraphicBoardResizable::SetBoardSize( wxSize &sz )
{
    cprintf( "Child: resize x=%d, y=%d\n", sz.x, sz.y );
    char temp[ sizeof(_position_ascii) + 1 ];
    strcpy( temp, _position_ascii );
    SetSize(sz);
    Init(sz);
    SetChessPosition( temp );
    Refresh();
    Update();
}


BEGIN_EVENT_TABLE(GraphicBoardResizable, wxControl)
    EVT_PAINT(GraphicBoardResizable::OnPaint)
    EVT_MOUSE_CAPTURE_LOST(GraphicBoardResizable::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(GraphicBoardResizable::OnMouseEvent)
    EVT_LEFT_UP (GraphicBoardResizable::OnMouseLeftUp)
    EVT_LEFT_DOWN (GraphicBoardResizable::OnMouseLeftDown)
    EVT_MOTION (GraphicBoardResizable::OnMouseMove)
//    EVT_ERASE_BACKGROUND(GraphicBoardResizable::OnEraseBackground)
//    EVT_TIMER( TIMER_ID, GraphicBoardResizable::OnTimeout)
END_EVENT_TABLE()


