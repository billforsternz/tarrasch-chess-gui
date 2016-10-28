/****************************************************************************
 * Chess Board bitmap utilities
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
#include "thc.h"
#include "Objects.h"
#include "Repository.h"

// Initialise the graphic board
ChessBoardBitmap::ChessBoardBitmap()
{
    pickup_file    = 0;
    pickup_rank    = 0;
    pickup_point.x = 0;
    pickup_point.y = 0;
    buf_board = 0;
    buf_box = 0;
}


void bmpCopy( wxBitmap &from, int x1, int y1, wxBitmap &to,	int x2, int y2, int w, int h, const char *mask=NULL );
void bmpCopy( wxBitmap &from, int x1, int y1, wxBitmap &to,	int x2, int y2, int w, int h, const char *mask )
{
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
			if( mask==NULL || *mask == '1' )
			{
			  	byte r = src.Red();
				byte g = src.Green();
				byte b = src.Blue();
				dst.m_ptr[2] = r;
				dst.m_ptr[1] = g;
				dst.m_ptr[0] = b;
				// dst.m_ptr = src.m_ptr;
			}
            src++;
            dst++;
			if( mask != NULL )
				mask++;
        }
    }
}


void imageCopy( wxBitmap &from, int x1, int y1, wxImage &to, int x2, int y2, int w, int h, const char *mask=NULL );
void imageCopy( wxBitmap &from, int x1, int y1, wxImage &to, int x2, int y2, int w, int h, const char *mask )
{
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
			if( mask==NULL || *mask == '1' )
			{
			  	r = src.Red();
				g = src.Green();
				b = src.Blue();
				transparent = false;
			}
			to.SetRGB(x2+col, y2+row, r,g,b);
			to.SetAlpha(x2+col, y2+row, transparent?wxIMAGE_ALPHA_TRANSPARENT:wxIMAGE_ALPHA_OPAQUE );
            src++;
			if( mask != NULL )
				mask++;
        }
    }
}

void ChessBoardBitmap::BuildMiniboardBitmap( int pix, wxBitmap &bm )
{
	Init( pix );
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
	bm = my_chess_bmp;
}

void  ChessBoardBitmap::BuildBoardSetupBitmap( int pix, wxBitmap &bm )
{
	Init( pix );
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
	
    wxBitmap board_setup;
	board_setup.Create(364,294,24);
    wxMemoryDC dc;
    dc.SelectObject(board_setup);
    dc.SetMapMode(wxMM_TEXT);

	// Set all of board_setup to white, with one pixel black border
    wxColour white(255,255,255);
    wxBrush white_brush(white);
    dc.SetBrush( white_brush );
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(0,0,board_setup.GetWidth(),board_setup.GetHeight());

	// Copy a chess board into the centre part
	bmpCopy( my_chess_bmp, 0, 0, board_setup, (364-8*pix)/2, (294-8*pix)/2, 8*pix, 8*pix );

	// Make square b5 a nice blue colour
	wxColour ocean_blue(112,146,190);
    wxBrush ocean_blue_brush(ocean_blue);
    dc.SetBrush( ocean_blue_brush );
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle( (364-8*pix)/2 + pix, (294-8*pix)/2 + 3*pix, pix, pix );

	// Add pieces to be picked up on the side
	for( int i=0; i<6; i++ )
	{
		const char *mask;
		char file, rank;
		switch(i)
		{
			default:
			case 0: file='e';	rank='4';
					mask = white_king_mask;		break;
			case 1: file='d';	rank='1';
			        mask = white_queen_mask;	break;
			case 2: file='h';	rank='1';
			        mask = white_rook_mask;		break;
			case 3: file='f';	rank='1';
			        mask = white_bishop_mask;	break;
			case 4: file='b';	rank='1';
			        mask = white_knight_mask;	break;
			case 5: file='a';	rank='2';
			        mask = white_pawn_mask;		break;
		}
		int x = file-'a';
		int y = '8'-rank;
		bmpCopy( my_chess_bmp, x*pix, y*pix, board_setup, 8, 20 + i*44, pix, pix, mask );
	}
	for( int i=0; i<6; i++ )
	{
		const char *mask;
		char file, rank;
		switch(i)
		{
			default:
			case 5: file='g';	rank='8';
					mask = black_king_mask;		break;
			case 4: file='d';	rank='5';
			        mask = black_queen_mask;	break;
			case 3: file='a';	rank='8';
			        mask = black_rook_mask;		break;
			case 2: file='c';	rank='8';
			        mask = black_bishop_mask;	break;
			case 1: file='e';	rank='8';
			        mask = black_knight_mask;	break;
			case 0: file='b';	rank='7';
			        mask = black_pawn_mask;		break;
		}
		int x = file-'a';
		int y = '8'-rank;
		bmpCopy( my_chess_bmp, x*pix, y*pix, board_setup, 325, 20 + i*44, pix, pix, mask );
	}
	bm = board_setup;
}

void ChessBoardBitmap::BuildCustomCursors( int pix )
{
	Init( pix );
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

	for( int i=0; i<12; i++ )
	{
		char file, rank;
		const char *mask;
		wxImage *ptr;
		switch(i)
		{
			default:
			case 0: file='e';	rank='4';
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
			case 6: file='g';	rank='8';
					mask = black_king_mask;
					ptr  = &black_king_cursor;		break;
			case 7: file='d';	rank='5';
			        mask = black_queen_mask;
			        ptr  = &black_queen_cursor;		break;
			case 8: file='a';	rank='8';
			        mask = black_rook_mask;
			        ptr  = &black_rook_cursor;		break;
			case 9: file='c';	rank='8';
			        mask = black_bishop_mask;
			        ptr  = &black_bishop_cursor;	break;
			case 10: file='e';	rank='8';
			        mask = black_knight_mask;
			        ptr  = &black_knight_cursor;	break;
			case 11: file='b';	rank='7';
			        mask = black_pawn_mask;
			        ptr  = &black_pawn_cursor;		break;
		}
		ptr->Create( pix, pix, false );
		ptr->InitAlpha();
		int x = file-'a';
		int y = '8'-rank;
		imageCopy( my_chess_bmp, x*pix, y*pix, *ptr, 0, 0, pix, pix, mask );
	}
}

void ChessBoardBitmap::Init( int pix )
{
	wxSize size(pix*8,pix*8);
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
	//	"rnbqnrk "
	//	"pp	   b "
	//	"    N	 "
	//	"	qk	 "
	//	"	QK	 "
	//	"        "
	//	"PP		 "
	//	"RNBQKBNR"
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
    my_chess_bmp = my_chess_bmp_;
}

// Cleanup
ChessBoardBitmap::~ChessBoardBitmap()
{
	delete(buf_board);
	delete(buf_box);
}

// Setup a position	on the graphic board
void ChessBoardBitmap::SetChessPosition( char *position_ascii )
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


// Calculate an offset into the wxBitmap's image buffer
unsigned long ChessBoardBitmap::Offset( char file, char rank )
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
void ChessBoardBitmap::Get( char src_file, char src_rank, char dst_file, char dst_rank, const char *mask )
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
void ChessBoardBitmap::Put( char src_file, char src_rank, char dst_file, char dst_rank )
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


// Setup a position	on the graphic board
void ChessBoardBitmap::SetPositionEx( thc::ChessPosition pos, bool blank_other_squares, char pickup_file_, char pickup_rank_, wxPoint shift )
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
void ChessBoardBitmap::PutEx( char piece,
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
    }
}


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
    #define WIDTH 600
    #define HEIGHT 400
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
#else
void Testbed()
{
    wxBitmap bm;
	ChessBoardBitmap cbb;
	cbb.BuildBoardSetupBitmap(34,bm);
	TestbedDialog dialog(bm);
	dialog.Run();
}
#endif