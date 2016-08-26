/****************************************************************************
 * Control - The graphic chess board that is the centre-piece of the GUI
 *  Still working on making it a more-self contained and reusable control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

// This resizable version is just getting started - don't #define RESIZABLE_BOARD
// in Appdefs.h until it's much more mature! 

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/rawbmp.h"
#include "wx/file.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "GraphicBoardResizable.h"
#include "GameLogic.h"
#include "thc.h"
#include "Objects.h"

// Initialise the graphic board
GraphicBoardResizable::GraphicBoardResizable
(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& point,
    const wxSize& size
)   : wxControl( parent, id, point, size, BORDER_COMMON )
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

void GraphicBoardResizable::Init
(
    const wxSize& size
)

{
    int min = size.x<size.y ? size.x : size.y;
    PIX = min/8;
    wxBitmap my_chess_bmp_;
    my_chess_bmp_.Create(PIX*8,PIX*8,24);
    wxMemoryDC dc;
    dc.SelectObject(my_chess_bmp_);
    dc.SetMapMode(wxMM_TEXT);

    wxColour icon_light(255,226,179);
    wxColour legacy_dark(200,200,200);
    wxColour icon_dark(220,162,116);
    wxColour magic(255,0,255);
    wxBrush light_brush(icon_light);
    dc.SetBrush( light_brush );
    //dc.SetBrush( *wxWHITE_BRUSH );

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0,0,my_chess_bmp_.GetWidth(),my_chess_bmp_.GetHeight());
    //dc.SetBrush( *wxCYAN_BRUSH );
    //dc.SetBrush( *wxGREEN_BRUSH );
    //dc.SetPen( *wxRED_PEN );
    //dc.SetPen(*wxTRANSPARENT_PEN);
    //dc.DrawRectangle(120,120,200,200);

    // Font characters for each piece, by observation
    unsigned char xlat[256];
    memset( xlat, ' ', sizeof(xlat) );
    xlat[' '] = 0x20;
    xlat['K'] = 0x30;
    xlat['k'] = 0x32;
    xlat['Q'] = 0x47;
    xlat['q'] = 0x49;
    xlat['R'] = 0x57;
    xlat['r'] = 0x59;
    xlat['B'] = 0x6d;
    xlat['b'] = 0x6f;
    xlat['N'] = 0xa9;
    xlat['n'] = 0xab;
    xlat['P'] = 0xb9;
    xlat['p'] = 0xbb;

    //
    const int MARGIN=0;
    wxFontInfo wfi( wxSize(PIX-1,PIX-1) );
    wfi.AntiAliased(true);
    wfi.FaceName( "GC2004D" );
    dc.SetFont( wfi );
    int x = MARGIN;
    int y = MARGIN;
    const char *board[] =
    {
        "rNbQkBnR",
        "Pp#qK   ",
    };
    dc.SetTextForeground( *wxBLACK );
    dc.SetBackgroundMode( wxPENSTYLE_TRANSPARENT );
    for( int i=0; i<2; i++ )
    {
        const char *s = board[i];
        bool dark = ((i&1)==1);
        x = 0;
        for( int j=0; j<8; j++ )
        {
            char d = *s++;
            char c = xlat[d];
            if( d == '#' )  // dark square
            {
                wxColour icon_light(255,226,179);
                wxColour legacy_dark(200,200,200);
                wxColour icon_dark(220,162,116);
                wxBrush dark_brush(icon_dark);
                dc.SetBrush( dark_brush );
                //dc.SetBrush( *wxCYAN_BRUSH );
                //dc.SetPen(*wxCYAN_PEN);
                dc.DrawRectangle(x<0?0:x, y, PIX, PIX );
           /*
                wxColour magic(255,0,255);
                wxBrush magicBrush(magic);
                //dc.SetBrush( magicBrush );
                dc.SetBrush( *wxCYAN_BRUSH );
                dc.SetBrush( *wxGREEN_BRUSH ); */
            }
            else
            {
                char buf[2];
                buf[0] = c;
                buf[1] = '\0';
                dc.SetTextBackground( dark ? *wxBLUE : *wxRED );
                dc.DrawText( buf, x<0?0:x, y );
            }
            dark = !dark;
            x += PIX;
        }
        y += PIX;
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
    //dc.DrawRectangle(PIX*3-2, PIX*3-2, 10, 10 );
    //bool ok = dc.FloodFill( PIX*3, PIX*3, *wxBLACK, wxFLOOD_BORDER );
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

	char *buf_mask_raw    = new char[ width*height];
	char *buf_mask_cooked = new char[ width*height];
    for( unsigned int row=0; row<height/4; row++ )
    {
        p.MoveTo(bmdata, 0, row );
        char *s = &buf_mask_raw[ row*width ];
        for( unsigned int col=0; col<width; col++ )
        {
            uint8_t r = p.Red();
            uint8_t g = p.Green();
            uint8_t b = p.Blue();
            p++;
            char c = (r==255 && g==226 && b==179) /*(r==255 && g==255 && b==255)*/ ? '0' : '1';
            s[col] = c;
        }
    }
    int sq_width = width/8;
    int sq_height = height/8;
    #define OFFSET(sq_row,sq_col) ((sq_row)*sq_height*width) + ((sq_col)*sq_width)

    std::string *p_str=0;
    const char **p_mask=0;
    for( int piece=0; piece<12; piece++ )
    {
        const char *pri;
        const char *sec;
        char *mask;
        switch( piece )
        {
            // White pawn
            case 0:
            {
                p_str =  &str_white_pawn_mask;
                p_mask = &white_pawn_mask;
                pri  = buf_mask_raw    + OFFSET(1,0);
                mask = buf_mask_cooked + OFFSET(1,0);
                sec  = buf_mask_raw    + OFFSET(1,1);   // sec is black pawn
                break;
            }

            // Black pawn
            case 1:
            {
                p_str =  &str_black_pawn_mask;
                p_mask = &black_pawn_mask;
                pri  = buf_mask_raw    + OFFSET(1,1);
                mask = buf_mask_cooked + OFFSET(1,1);
                sec  = buf_mask_cooked + OFFSET(1,0);   // sec is cooked white pawn
                break;
            }

            // White knight
            case 2:
            {
                p_str =  &str_white_knight_mask;
                p_mask = &white_knight_mask;
                pri  = buf_mask_raw    + OFFSET(0,1);
                mask = buf_mask_cooked + OFFSET(0,1);
                sec  = buf_mask_raw    + OFFSET(0,6);   // sec is black knight
                break;
            }

            // Black knight
            case 3:
            {
                p_str =  &str_black_knight_mask;
                p_mask = &black_knight_mask;
                pri  = buf_mask_raw    + OFFSET(0,6);
                mask = buf_mask_cooked + OFFSET(0,6);
                sec  = buf_mask_cooked + OFFSET(0,1);   // sec is cooked white knight
                break;
            }

            // White bishop
            case 4:
            {
                p_str =  &str_white_bishop_mask;
                p_mask = &white_bishop_mask;
                pri  = buf_mask_raw    + OFFSET(0,5);
                mask = buf_mask_cooked + OFFSET(0,5);
                sec  = buf_mask_raw    + OFFSET(0,2);   // sec is black bishop
                break;
            }

            // Black bishop
            case 5:
            {
                p_str =  &str_black_bishop_mask;
                p_mask = &black_bishop_mask;
                pri  = buf_mask_raw    + OFFSET(0,2);
                mask = buf_mask_cooked + OFFSET(0,2);
                sec  = buf_mask_cooked + OFFSET(0,5);   // sec is cooked white bishop
                break;
            }

            // White rook
            case 6:
            {
                p_str =  &str_white_rook_mask;
                p_mask = &white_rook_mask;
                pri  = buf_mask_raw    + OFFSET(0,7);
                mask = buf_mask_cooked + OFFSET(0,7);
                sec  = buf_mask_raw    + OFFSET(0,0);   // sec is black rook
                break;
            }

            // Black rook
            case 7:
            {
                p_str =  &str_black_rook_mask;
                p_mask = &black_rook_mask;
                pri  = buf_mask_raw    + OFFSET(0,0);
                mask = buf_mask_cooked + OFFSET(0,0);
                sec  = buf_mask_cooked + OFFSET(0,7);   // sec is cooked white rook
                break;
            }

            // White queen
            case 8:
            {
                p_str =  &str_white_queen_mask;
                p_mask = &white_queen_mask;
                pri  = buf_mask_raw    + OFFSET(0,3);
                mask = buf_mask_cooked + OFFSET(0,3);
                sec  = buf_mask_raw    + OFFSET(1,3);   // sec is black queen
                break;
            }

            // Black queen
            case 9:
            {
                p_str =  &str_black_queen_mask;
                p_mask = &black_queen_mask;
                pri  = buf_mask_raw    + OFFSET(1,3);
                mask = buf_mask_cooked + OFFSET(1,3);
                sec  = buf_mask_cooked + OFFSET(0,3);   // sec is cooked white queen
                break;
            }

            // White king
            case 10:
            {
                p_str =  &str_white_king_mask;
                p_mask = &white_king_mask;
                pri  = buf_mask_raw    + OFFSET(1,4);
                mask = buf_mask_cooked + OFFSET(1,4);
                sec  = buf_mask_raw    + OFFSET(0,4);   // sec is black king
                break;
            }

            // Black king
            case 11:
            {
                p_str =  &str_black_king_mask;
                p_mask = &black_king_mask;
                pri  = buf_mask_raw    + OFFSET(0,4);
                mask = buf_mask_cooked + OFFSET(0,4);
                sec  = buf_mask_cooked + OFFSET(1,4);   // sec is cooked white king
                break;
            }
        }
        char *save_mask = mask;

        int state = 0;      // start, dark, light
        int count = 0;      // nbr of light pixels in a row
        for( int i=0; i<sq_height; i++ )
        {
            state = 0; // start
            memset( mask, '1', sq_width );  // default state
            for( int j=0; j<sq_width; j++ )
            {
                bool debug = false; //(i==6 && j==15);
                char c = *pri++;
                sec++;
                switch( state )
                {
                    case 0: // start
                    {
                        if( c == '0' )
                            mask[j] = '0';
                        else
                            state = 1;  // dark
                        break;
                    }
                    case 1: // dark
                    {
                        if( c == '0' )
                        {
                            state = 2;      // light
                            count = 1;      // nbr of light pixels in a row
                        }
                        break;
                    }
                    case 2: // light
                    {
                        if( c == '0' )
                            count++;    // nbr of light pixels in a row
                        else
                        {
                            state = 1;  // dark

                            // Check whether the stretch of light pixels is real by
                            //  looking at the peer image, which is similar but darker
                            //  (idea is to eliminate internal light pixels - in
                            //   effect this is a smart flood fill - necessary because
                            //   the piece's dark boundaries might have micro gaps)
                            bool peer_indicates_real_light_stretch = false;
                            int matches=0;
                            if( debug )
                            {
                                cprintf( "pri> " );
                                for( int k=14; k>=0; k-- )
                                    cprintf( "%c", *(pri-k) );
                                cprintf("\n");
                                cprintf( "sec> " );
                                for( int k=14; k>=0; k-- )
                                    cprintf( "%c", *(sec-k) );
                                cprintf("\n");
                            }  
                            for( int n=1; n<=count+1; n++ )
                            {
                                if( *(pri-n) == *(sec-n) )
                                    matches++;
                            }
                            if( count+1 <= 2 )
                                peer_indicates_real_light_stretch = (matches == count+1);
                            else if( count+1 <= 3 )
                                peer_indicates_real_light_stretch = ((matches*100)/(count+1) >= 60);
                            else if( count+1 <= 4 )
                                peer_indicates_real_light_stretch = ((matches*100)/(count+1) >= 65);
                            else if( count+1 <= 8 )
                                peer_indicates_real_light_stretch = ((matches*100)/(count+1) >= 70);
                            else if( count+1 <= 16 )
                                peer_indicates_real_light_stretch = ((matches*100)/(count+1) >= 75);
                            else
                                peer_indicates_real_light_stretch = ((matches*100)/(count+1) >= 85);
                            if( debug )
                                cprintf( "count=%d, matches=%d, peer_indicates_real_light_stretch=%s\n", count, matches, peer_indicates_real_light_stretch?"true":"false");
                            if( peer_indicates_real_light_stretch )
                            {
                                bool middle = (j>=sq_width/2 && (j-count)<=sq_width/2);
                                bool lower  = (i>sq_height/2);
                                bool knight = (piece==2 || piece==3);
                                bool bishop = (piece==4 || piece==5);
                            //  bool rook   = (piece==6 || piece==7);
                                bool queen  = (piece==8 || piece==9);
                                bool nostril= (knight && i>(sq_height*55)/100 && i<(sq_height*70)/100); // knight's nostril shouldn't be real light stretch
                                if( nostril )
                                    peer_indicates_real_light_stretch = false;
                                else if( bishop || queen )
                                {
                                    if( middle && lower )
                                        peer_indicates_real_light_stretch = false;
                                    else if( bishop && lower )
                                        peer_indicates_real_light_stretch = false;
                                }
                                else
                                {
                                    if( middle )
                                        peer_indicates_real_light_stretch = false;
                                }
                            }
                            if( peer_indicates_real_light_stretch )
                            {
                                for( int n=1; n<=count; n++ )
                                    mask[j-n] = '0';    
                            }  // else leave as '1'
                        }
                        break;
                    }
                }
            }
            if( state == 2 )  // expected
            {
                for( int n=1; n<=count; n++ )
                    mask[sq_width-n] = '0';    
            }
            pri -= sq_width;
            pri += width;
            sec -= sq_width;
            sec += width;
            mask+= width;
        }

        mask = save_mask;
        std::string str;
        for( int i=0; i<sq_height; i++ )
        {
            for( int j=0; j<sq_width; j++ )
            {
                char c = *mask++;
                str += c;
                if( piece == 10 )
                    cprintf( "%c", c );
            }
            if( piece == 10 )
                cprintf( "\n" );
            mask -= sq_width;
            mask+= width;
        }
        if( piece == 10 )
            cprintf( "\n" );
        *p_str = str;
        *p_mask = p_str->c_str();
    }

	delete(buf_mask_cooked);
	delete(buf_mask_raw);


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
void GraphicBoardResizable::SetPosition( char *position_ascii )
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
    for( int row=0; row<height; row++ )
    {
        p.MoveTo(bmdata, 0, row );
        for( int col=0; col<width; col++ )
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
    for( int row=0; row<height; row++ )
    {
        p.MoveTo(bmdata, 0, row );
        for( int col=0; col<width; col++ )
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


void GraphicBoardResizable::OnSize( wxSizeEvent& event )
{
    wxSize sz = event.GetSize();
    cprintf( "resize x=%d, y=%d\n", sz.x, sz.y );
    char temp[ sizeof(_position_ascii) + 1 ];
    strcpy( temp, _position_ascii );
    Init(sz);
    SetPosition( temp );
    Refresh();
    Update();
}

BEGIN_EVENT_TABLE(GraphicBoardResizable, wxControl)
    EVT_PAINT(GraphicBoardResizable::OnPaint)
    EVT_SIZE(GraphicBoardResizable::OnSize)
//    EVT_MOUSE_CAPTURE_LOST(GraphicBoardResizable::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(GraphicBoardResizable::OnMouseEvent)
    EVT_LEFT_UP (GraphicBoardResizable::OnMouseLeftUp)
    EVT_LEFT_DOWN (GraphicBoardResizable::OnMouseLeftDown)
    EVT_MOTION (GraphicBoardResizable::OnMouseMove)
//    EVT_ERASE_BACKGROUND(GraphicBoardResizable::OnEraseBackground)
//    EVT_TIMER( TIMER_ID, GraphicBoardResizable::OnTimeout)
END_EVENT_TABLE()
