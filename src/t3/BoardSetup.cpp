/****************************************************************************
 * Draw the graphic board in the board setup control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/rawbmp.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "BoardSetup.h"
#include "ChessRules.h"

using namespace std;
using namespace thc;

//#define XBORDER 46
//#define YBORDER 11
#define SQUARE_HEIGHT 34
#define SQUARE_WIDTH  34

#if 1
void BoardSetup::Debug()
{
}
#endif

// Wake up this code to generate some board .xpm data
//  (the link between original .bmps and generated .xpms
//   has been muddied and perhaps even lost)  
#if 0
#define nbrof(a) ( sizeof(a) / sizeof((a)[0]) )
void BoardSetup::Debug()
{
    //#define TUNE_IT_FIRST
    #ifdef  TUNE_IT_FIRST
    static char codes[1000];
    static int reds[1000];
    static int greens[1000];
    static int blues[1000];
    int nbr_of=0;
    #else
    static char codes[] = " .-,`~o{}=+&$*@#";
    static int reds[]   = {0xff,0xf5,0xec,0xe1,0xd7,0xcc,0xc0,0xb4,0xa7,0x99,0x8b,0x7a,0x68,0x53,0x39,0x00};
    static int greens[] = {0xff,0xf5,0xec,0xe1,0xd7,0xcc,0xc0,0xb4,0xa7,0x99,0x8b,0x7a,0x68,0x53,0x39,0x00};
    static int blues[]  = {0xff,0xf5,0xec,0xe1,0xd7,0xcc,0xc0,0xb4,0xa7,0x99,0x8b,0x7a,0x68,0x53,0x39,0x00};
    int nbr_of=nbrof(reds);
    #endif
    static int done;
    if( done == 0 )
    {
        done = 1;
        FILE *f;
        f = fopen("bitmap3.txt","wt");
        if( f )
        {
            byte a,b,c,x, *p;
            int row,col;
            #ifdef TUNE_IF_FIRST
            for( row=0; row<height; row++ )
            {
                p = buf_board + row*width_bytes;
                for( col=0; col<width; col++ )
                {
                    a = *p++;
                    b = *p++;
                    c = *p++;
                    double min_diff_so_far = 100000000000.0;
                    int min_i_so_far = 0;
                    for( int i=0; i<nbr_of; i++ )
                    {
                        double diff1 = ((double)a - (double)reds[i]);
                        double diff2 = ((double)b - (double)greens[i]);
                        double diff3 = ((double)c - (double)blues[i]);
                        double diff = diff1*diff1 + diff2*diff2 + diff3*diff3;
                        if( diff < min_diff_so_far )
                        {
                            min_diff_so_far = diff;
                            min_i_so_far = i;
                        }
                    }
                    if( reds[min_i_so_far]==a && greens[min_i_so_far]==b &&  blues[min_i_so_far]==c )
                        ;
                    else
                    {
                        codes[nbr_of]  = ' '+nbr_of;
                        reds[nbr_of]   = a;
                        greens[nbr_of] = b;
                        blues[nbr_of]  = c;
                        if( nbr_of+1 < nbrof(reds) )
                            nbr_of++;
                    }
                }
            }
            #endif
            fprintf( f, "static char *board_setup_bitmap_xpm[] = {\n" );
            fprintf( f, "\"%d %d %d 1\",\n", width, height, nbr_of );
            for( int i=0; i<nbr_of; i++ )
            {
                fprintf( f, "\"%c c #%02x%02x%02x\",\n", codes[i], reds[i], greens[i], blues[i] );
            }
            for( row=0; row<height; row++ )
            {
                p = buf_board + row*width_bytes;
                for( col=0; col<width; col++ )
                {
                    a = *p++;
                    b = *p++;
                    c = *p++;
                    double min_diff_so_far = 100000000000.0;
                    int min_i_so_far = 0;
                    for( int i=0; i<nbr_of; i++ )
                    {
                        double diff1 = ((double)a - (double)reds[i]);
                        double diff2 = ((double)b - (double)greens[i]);
                        double diff3 = ((double)c - (double)blues[i]);
                        double diff = diff1*diff1 + diff2*diff2 + diff3*diff3;
                        if( diff < min_diff_so_far )
                        {
                            min_diff_so_far = diff;
                            min_i_so_far = i;
                        }
                    }
                    if( col == 0 )
                        fprintf( f, "\"" );
                    if( reds[min_i_so_far]==a && greens[min_i_so_far]==b &&  blues[min_i_so_far]==c )
                        fprintf( f, "%c", codes[min_i_so_far] );
                    else
                        fprintf( f, "[ #%02x%02x%02x ]", a, b, c );
                }
                if( row+1 < height )
                    fprintf( f, "\",\n" );
                else
                    fprintf( f, "\"\n" );
            }
            fprintf( f, "};\n" );
            fclose(f);
        }
    }
}
#endif

// Initialise the graphic board
BoardSetup::BoardSetup( wxBitmap *bitmap, wxWindow *parent, int XBORDER, int YBORDER )
{
    this->bitmap  = bitmap;
    this->parent  = parent;
    this->XBORDER = XBORDER;
    this->YBORDER = YBORDER;

	// Orientation
	normal_orientation = true;

	// Highlights squares
	ClearHighlight1();
	ClearHighlight2();

	// Get the ChessButton and the wxBitmap
	//wxClientDC dcscreen(w);
    //dcmem.SelectObject( *bitmap );
    //dcmem.SetBrush( *wxWHITE_BRUSH );
   	//dcmem.SetPen  ( *wxRED_PEN     );

#ifdef THC_WINDOWS
	BITMAP info;

	// Get dimensions of the wxBitmap
	//bitmap->GetBitmap( &info ); // return ::GetObject(m_hObject, sizeof(BITMAP), pBitMap);
    ::GetObject( bitmap->GetHBITMAP(), sizeof(BITMAP), &info ); //@@
	width_bytes  = info.bmWidthBytes;   // bytes
	width        = info.bmWidth;        // pixels
   	assert( (info.bmWidthBytes%info.bmWidth) == 0 );
	density      = info.bmWidthBytes/info.bmWidth;
	height       = info.bmHeight;
  	DebugPrintf(( "<4>ibWB=%lu, ibW=%lu, ibH=%lu\n",
						(unsigned long)info.bmWidthBytes,
						(unsigned long)info.bmWidth,
						(unsigned long)info.bmHeight ));

#endif

#ifdef THC_MAC
    wxAlphaPixelData bmdata(*bitmap);
    height = bmdata.GetHeight();
    width  = bmdata.GetWidth();
    wxPoint x = bmdata.GetOrigin();
    wxSize  z = bmdata.GetSize();
    int row_stride = bmdata.GetRowStride();
    cprintf( "x=%d,%d z=%d,%d, height=%d, width=%d, row_stride=%d\n",x.x,x.y,z.x,z.y,height,width,row_stride);
    width_bytes = row_stride;
    density = width_bytes/width;
#endif

	xborder	     = XBORDER * density; //@@(width_bytes%8) / 2;
	yborder	     = YBORDER;  //@@(height%8) / 2;

	// Allocate an image of the board
	buf_board = new byte[width_bytes*height];
    memset( buf_board, 0, width_bytes*height);
	
	// Allocate an image of the box (pieces off the board)
	buf_box   = new byte[width_bytes*height];

	// Read the initial position displayed on the bitmap
#ifdef THC_WINDOWS
	//bitmap->GetBitmapBits( width_bytes*height, buf_board );   //@@
    ::GetBitmapBits((HBITMAP)(bitmap->GetHBITMAP()), width_bytes*height, buf_board );
#endif
#ifdef THC_MAC
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
#endif

	// Read from position on left below (the .bmp resource) into the box
	//  at right. The box has all the piece/colour combinations we need
    //  (eg only two pawns from each side are needed, one of each colour).
    //  Also note that the two squares marked with * are the two empty
	//	squares (one of each colour) we need.
	//
	//	"rnbqnrk "		 "rnbqkbnr"
	//	"ppppppbp"		 "pp      "
	//	"    N p "		 "        "
	//	"   qk   "		 "   qk   "
	//	"  PQK   "		 "   QK   "
	//	"  N     "		 "**      "
	//	"PP   PPP"		 "PP      "
	//	"R BQKBNR"		 "RNBQKBNR"
	//
	// Store these pieces from the board to their fixed
	//  positions in the 'box'
	Get( 'h','1', 'h','1' );
	Get( 'g','1', 'g','1' );
	Get( 'f','1', 'f','1' );
	Get( 'e','1', 'e','1' );
	Get( 'd','1', 'd','1' );
	Get( 'c','1', 'c','1' );
    //Get( 'c','3', 'b','1' ); // Relocating from c3 to b1 shifts
							   //  this white knight to wrong colour
	Get( 'a','1', 'a','1' );
	
	//Get( 'h','2', 'h','2' ); // Only need pawns on a2, b2
	//Get( 'g','2', 'g','2' );
	//Get( 'f','2', 'f','2' );
	//Get( 'e','5', 'e','2' );
	//Get( 'd','4', 'd','2' );
	//Get( 'c','4', 'c','2' );
	Get( 'b','2', 'b','2' );
	Get( 'a','2', 'a','2' );

	//Get( 'h','7', 'h','7' ); // Only need pawns on a7, b7
	//Get( 'g','6', 'g','7' );
	//Get( 'f','7', 'f','7' );
	//Get( 'e','7', 'e','7' );
	//Get( 'd','7', 'd','7' );
	//Get( 'c','7', 'c','7' );
	Get( 'b','7', 'b','7' );
	Get( 'a','7', 'a','7' );

	Get( 'f','8', 'h','8' );
	Get( 'e','8', 'g','8' );
	Get( 'g','7', 'f','8' );
	Get( 'g','8', 'e','8' );
	Get( 'd','8', 'd','8' );
	Get( 'c','8', 'c','8' );
	Get( 'b','8', 'b','8' );
	Get( 'a','8', 'a','8' );

	Get( 'c','2', 'c','2' );  // square with red highlight box	
	Get( 'a','3', 'a','3' );  // empty black square
	Get( 'b','3', 'b','3' );  // empty white square
	Get( 'c','3', 'c','3' );  // square with cross 
	Get( 'd','3', 'd','3' );  // square with cross 
    
	Get( 'e','6', 'b','1' );  // White knight on white square
	Get( 'e','5', 'e','5' );  // Black king on black square
	Get( 'e','4', 'e','4' );  // White king on white square
	Get( 'd','5', 'd','5' );  // Black queen on white square
	Get( 'd','4', 'd','4' );  // White queen on black square
    Debug();
}

// Cleanup
BoardSetup::~BoardSetup()
{
	delete(buf_board);
	delete(buf_box);
}

// Setup a position	on the graphic board
void BoardSetup::SetPosition( const char *position_ascii )
{
#if 0 // Setup a custom position here
    #define CUSTOM_POSITION "rnbqnrk|pp4b|4N|3qk|3QK|8|PPxx|R1BQKBNR| w KQkq - 0 1"
    ChessPosition pos;
    pos.Forsyth( CUSTOM_POSITION );
    position_ascii = pos.squares;
#endif

	char piece;
	int  i, file=0, rank=7;
	char rev_file='h', rev_rank='1';	// tracks reverse orientation
	char src_file, src_rank;
	char dst_file, dst_rank;
	const char *colour =
        "wbwbwbwb"
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
	for( i=0; i<64; i++ )
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
    //DebugPrintf(( "BoardSetup::Debug() called\n"));
    //Debug();

	// Copy from the image buffer into the wxBitmap
    #ifdef THC_WINDOWS
	//bitmap->SetBitmapBits( width_bytes*height, buf_board );
    /*int ret = */::SetBitmapBits( (HBITMAP)(bitmap->GetHBITMAP()), width_bytes*height, buf_board );  //@@
	#endif

    #ifdef THC_MAC
    wxAlphaPixelData bmdata(*bitmap);
    wxAlphaPixelData::Iterator p(bmdata);
    byte *src = buf_board;
    for( i=0; i<height*width; i++ )
    {
        p.Alpha() = *src++;
        p.Red()   = *src++;
        p.Green() = *src++;
        p.Blue()  = *src++;
        p++;
    }
    #endif

    // Now use GDI to add highlights
    int row, col, x, y;
    char highlight_file = highlight_file1;
    char highlight_rank = highlight_rank1;
    bool is_highlight = false;
    //highlight_file = 'f';
    //highlight_rank = '4';
    for( i=0; i<2; i++ )
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
            x = col*SQUARE_WIDTH;
            y = row*SQUARE_HEIGHT;
            if( !is_highlight )
            {
       	        /*@@restore = */dcmem.SelectObject( *bitmap );
                is_highlight = true;
            }
            wxPoint rect[5];
            rect[0].x = x;
            rect[0].y = y;
            rect[1].x = x + SQUARE_WIDTH-1;
            rect[1].y = y;
            rect[2].x = x + SQUARE_WIDTH-1;
            rect[2].y = y + SQUARE_HEIGHT-1;
            rect[3].x = x;
            rect[3].y = y + SQUARE_HEIGHT-1;
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
void BoardSetup::Draw()
{
    //DebugPrintf(( "Square a8 at Draw()\n" ));
    //Debug();
    parent->Refresh(false);
    parent->Update();
}

// Calculate an offset into the wxBitmap's image buffer
unsigned long BoardSetup::Offset( char file, char rank )
{
	byte col  = file-'a';		     // 0-7 => file a-h
	byte row  = 7-(rank-'1');		 // 0-7 => rank 8-1
	unsigned long offset =						
		yborder * width_bytes
      + ( row * SQUARE_HEIGHT ) * width_bytes
	  +	xborder
      + ( col * ((width_bytes-2*xborder)/8) );
	return( offset );	
}

// Get a piece from board, put into box
void BoardSetup::Get( char src_file, char src_rank, char dst_file, char dst_rank )
{
	unsigned int i, j;
	byte *src, *dst;
	for( i=0; i<SQUARE_HEIGHT; i++ )
	{
		src = buf_board + Offset(src_file,src_rank) + i*width_bytes;
		dst = buf_box   + Offset(dst_file,dst_rank) + i*width_bytes;
		for( j=0; j < (width_bytes-2*xborder)/8; j++ )
			*dst++ = *src++;
	}
}

// Put a piece from box onto board
void BoardSetup::Put( char src_file, char src_rank, char dst_file, char dst_rank )
{
    //DebugPrintf(("%c%c\n", dst_file, dst_rank ));
	if( density != 4 )
	{

		// 16 bit (and other) graphics code
		unsigned int i, j, k;
		byte *src, *dst;
		for( i=0; i < SQUARE_HEIGHT; i++ )
		{
			src			  =
				(byte *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			dst			  =
				(byte *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
			for( j=0; j < (width_bytes-2*xborder)/(8*density); j++ )
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
		for( i=0; i < SQUARE_HEIGHT; i++ )
		{
			src			  =
				(uint32_t *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			dst			  =
				(uint32_t *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
			for( j=0; j < (width_bytes-2*xborder)/32; j++ )
			    *dst++ = *src++;
		}
	}
}


// Figure out whether a piece or square is pointed to
bool BoardSetup::HitTest( wxPoint &point, char &piece, char &file, char &rank )
{
    bool hit = false;
    piece = '\0';
    file  = '\0';
    rank  = '\0';
    unsigned long width_board_bytes = (width_bytes-2*xborder);
    unsigned long point_x_bytes = point.x*density;
	unsigned long row = ( (point.y   - yborder) / SQUARE_HEIGHT );

    // Main board
    if( xborder<=point_x_bytes && point_x_bytes<width_bytes-xborder )
    {
        hit = true;
	    unsigned long col = ( (point_x_bytes-xborder) / (width_board_bytes/8) );
		rank = '1'+ (int)(7-row);
		file = 'a'+ (int)col;
    }

    // Maybe pickup pieces
    else
    {
        int spacer_width_bytes = (xborder - (width_board_bytes/8) ) / 2;
        int top = yborder+9; // offset before first piece
        int found = -1;
	    int y = point.y - yborder;
        for( int i=0; i<6; i++ )    // six pieces
        {
            if( top<=y && y<top+SQUARE_HEIGHT )
            {
                found = i;
                break;
            }
            top += SQUARE_HEIGHT;
            top += 10;  // gap between pieces
        }

        // Black pieces
        if( found>=0 &&
             (width_bytes - xborder + spacer_width_bytes)
                 < point_x_bytes && point_x_bytes <
             (width_bytes - spacer_width_bytes)
          )
        {
            hit = true;
            switch( found )
            {
                default:    hit = false;    break;
                case 0:     piece = 'p';    break;
                case 1:     piece = 'n';    break;
                case 2:     piece = 'b';    break;
                case 3:     piece = 'r';    break;
                case 4:     piece = 'q';    break;
                case 5:     piece = 'k';    break;
            }
        }

        // White pieces
        else if( found>=0 &&
                 (unsigned)spacer_width_bytes
                 < point_x_bytes && point_x_bytes <
                       (xborder - spacer_width_bytes)
          )
        {
            hit = true;
            switch( found )
            {
                default:    hit = false;    break;
                case 0:     piece = 'K';    break;
                case 1:     piece = 'Q';    break;
                case 2:     piece = 'R';    break;
                case 3:     piece = 'B';    break;
                case 4:     piece = 'N';    break;
                case 5:     piece = 'P';    break;
            }
        }
    }
    return hit;
}

