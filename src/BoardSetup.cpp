/****************************************************************************
 * Draw the graphic board in the board setup control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
****************************************************************************/
#include "wx/wx.h"
#include "wx/rawbmp.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "BoardSetup.h"
#include "thc.h"

#define SQUARE_HEIGHT 34
#define SQUARE_WIDTH  34

// Initialise the graphic board
BoardSetup::BoardSetup( wxBitmap *bitmap, wxWindow *parent, int XBORDER, int YBORDER, bool normal_orientation )
{
    this->bitmap  = bitmap;
    this->parent  = parent;
    this->XBORDER = XBORDER;
    this->YBORDER = YBORDER;
	this->normal_orientation = normal_orientation;
    cprintf( "Normal orientation(2) = %s\n", normal_orientation?"true":"false" );


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
  	dbg_printf( "<4>ibWB=%lu, ibW=%lu, ibH=%lu\n",
						(unsigned long)info.bmWidthBytes,
						(unsigned long)info.bmWidth,
						(unsigned long)info.bmHeight );
#else
#ifdef THC_MAC
    wxAlphaPixelData bmdata(*bitmap);
#else
    wxNativePixelData bmdata(*bitmap);
#endif
    height = bmdata.GetHeight();
    width  = bmdata.GetWidth();
    wxPoint x = bmdata.GetOrigin();
    wxSize  z = bmdata.GetSize();
    int row_stride = bmdata.GetRowStride();
    cprintf( "x=%d,%d z=%d,%d, height=%d, width=%d, row_stride=%d\n",x.x,x.y,z.x,z.y,height,width,row_stride);
    width_bytes = row_stride;
    density = width_bytes/width;
#endif

	xborder	     = XBORDER * density;
	yborder	     = YBORDER;

    // Highlight dimensions
    unsigned long thickness = SQUARE_HEIGHT/16;
    unsigned long indent    = thickness;
    highlight_y_lo1 = indent;
    highlight_y_lo2 = highlight_y_lo1 + thickness;
    highlight_y_hi2 = SQUARE_HEIGHT - indent;
    highlight_y_hi1 = highlight_y_hi2 - thickness;
    highlight_x_lo1 = indent;
    highlight_x_lo2 = highlight_x_lo1 + thickness;
	highlight_x_hi2 = (width_bytes-2*xborder)/(8*density) - indent;
    highlight_x_hi1 = highlight_x_hi2 - thickness;

	// Allocate an image of the board
	buf_board = new byte[width_bytes*height];
    memset( buf_board, 0, width_bytes*height);
	
	// Allocate an image of the box (pieces off the board)
	buf_box   = new byte[width_bytes*height];

	// Read the initial position displayed on the bitmap
#ifdef THC_WINDOWS
	//bitmap->GetBitmapBits( width_bytes*height, buf_board );   //@@
    ::GetBitmapBits((HBITMAP)(bitmap->GetHBITMAP()), width_bytes*height, buf_board );
#else
#ifdef THC_MAC
    wxAlphaPixelData::Iterator p(bmdata);
#else
    wxNativePixelData::Iterator p(bmdata);
#endif
    byte *dst = buf_board;
    for( int i=0; i<height*width; i++ )
    {
#ifdef THC_MAC
        *dst++ = p.Alpha();
#endif
        *dst++ = p.Red();
        *dst++ = p.Green();
        *dst++ = p.Blue();
        p++;
    }
#endif

	// Do an initial copy of all pieces into the box
	for( char file='a'; file<='h'; file++ )
		for( char rank='1'; rank<='8'; rank++ )
			Get(file,rank,file,rank);

    if( !normal_orientation )
        SwapPickupPieces();
}

// Cleanup
BoardSetup::~BoardSetup()
{
	delete(buf_board);
	delete(buf_box);
}

// Setup a position	on the graphic board
void BoardSetup::Set( const thc::ChessPosition &cp, const bool *highlight )
{

#if 0 // Setup a custom position here
    #define CUSTOM_POSITION "rnbqnrk|pp4b|4N|3qk|3QK|8|PPxx|R1BQKBNR| w KQkq - 0 1"
    cp.Forsyth( CUSTOM_POSITION );
#endif

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

	// Read string backwards for black at bottom
    const char *position_ascii = cp.squares;
    const bool *highlight_ptr = highlight;
	if( !normal_orientation )
    {
		position_ascii += 63;
        if( highlight_ptr )
            highlight_ptr += 63;
    }

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
		char piece = *position_ascii;
        bool highlight_f = highlight_ptr ? *highlight_ptr : false;
		if( normal_orientation )
        {
			position_ascii++;
            if( highlight_ptr )
                highlight_ptr++;
        }
        else
        {
			position_ascii--;
            if( highlight_ptr )
                highlight_ptr--;
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

	    Put( src_file, src_rank, dst_file, dst_rank, highlight_f );
	}

	// Copy from the image buffer into the wxBitmap
#ifdef THC_WINDOWS
	//bitmap->SetBitmapBits( width_bytes*height, buf_board );
    /*int ret = */::SetBitmapBits( (HBITMAP)(bitmap->GetHBITMAP()), width_bytes*height, buf_board );  //@@
#else
#ifdef THC_MAC
    wxAlphaPixelData bmdata(*bitmap);
    wxAlphaPixelData::Iterator p(bmdata);
#else
    wxNativePixelData bmdata(*bitmap);
    wxNativePixelData::Iterator p(bmdata);
#endif
    byte *src = buf_board;
    for( i=0; i<height*width; i++ )
    {
#ifdef THC_MAC
        p.Alpha() = *src++;
#endif
        p.Red()   = *src++;
        p.Green() = *src++;
        p.Blue()  = *src++;
        p++;
    }
#endif
}


// Draw the graphic board
void BoardSetup::Draw()
{
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
void BoardSetup::Put( char src_file, char src_rank, char dst_file, char dst_rank, bool highlight_f )
{
    if( highlight_f )
        cprintf( "Square %c%c locked down\n", dst_file, dst_rank );

    //dbg_printf("%c%c\n", dst_file, dst_rank );
	if( density != 4 )
	{

		// 16 bit (and other) graphics code
		unsigned int i, j, k;
		byte *src, *src_blue, *dst;
		for( i=0; i < SQUARE_HEIGHT; i++ )
		{
			src			  =
				(byte *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			src_blue     =
				(byte *)(buf_box   + Offset('b','5')           + i*width_bytes);
			dst			  =
				(byte *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
			for( j=0; j < (width_bytes-2*xborder)/(8*density); j++ )
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
	else
	{

		// 32 bit graphics code
		unsigned int i, j;
		uint32_t *src, *src_blue, *dst;
		for( i=0; i < SQUARE_HEIGHT; i++ )
		{
			src			  =
				(uint32_t *)(buf_box   + Offset(src_file,src_rank) + i*width_bytes);
			src_blue      =
				(uint32_t *)(buf_box   + Offset('b','5')           + i*width_bytes);
			dst			  =
				(uint32_t *)(buf_board + Offset(dst_file,dst_rank) + i*width_bytes);
			for( j=0; j < (width_bytes-2*xborder)/32; j++ )
            {
                bool yhit =         (highlight_y_lo1<=i && i<highlight_y_lo2) ||
                                    (highlight_y_hi1<=i && i<highlight_y_hi2);
                bool xhit =         (highlight_x_lo1<=j && j<highlight_x_lo2) ||
                                    (highlight_x_hi1<=j && j<highlight_x_hi2);
                bool yrange =       (highlight_y_lo1<=i && i<highlight_y_hi2);
                bool xrange =       (highlight_x_lo1<=j && j<highlight_x_hi2);
                bool blue =         highlight_f && ((xhit && yrange) || (yhit && xrange));
			    uint32_t dat = blue? *src_blue : *src;
			    *dst++ = dat;
                src++;
                src_blue++;
            }
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

        // Right side pickup pieces
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
                 (unsigned)spacer_width_bytes
                 < point_x_bytes && point_x_bytes <
                       (xborder - spacer_width_bytes)
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


// To help make it clear the orientation is reversed - swap the pickup pieces on the
//  left (normally white) and right (normally black) of the board.
void BoardSetup::SwapPickupPieces()
{
    // Geometry of pickup pieces
    #define NPIECES 6
    int width_board_bytes = (width_bytes-2*xborder);
    int spacer_width_bytes = (xborder - (width_board_bytes/8) ) / 2;
    int top = yborder+9; // offset before first piece

    // Start with the kings - offset_w points initially points at the white king (left top)
    // and offset_w initially points at the black king (right bottom)
    int offset_w = (top)*width_bytes +
               spacer_width_bytes;
    int offset_b = (top)*width_bytes +
                (width_bytes - xborder + spacer_width_bytes) +
                (SQUARE_HEIGHT+10)*(NPIECES-1)*width_bytes; // + 10 = gap between pieces
    cprintf( "width_bytes=%d\n", width_bytes );
    cprintf( "offset_w = %d\n", offset_w );
    cprintf( "offset_b = %d\n", offset_b );
    
    for( int n=0; n<NPIECES; n++ )
    {
		unsigned int i, j, k;
		byte *src, *dst;
		for( i=0; i < SQUARE_HEIGHT; i++ )
		{
			src			  =
				(byte *)(buf_board + offset_w + i*width_bytes);
			dst			  =
				(byte *)(buf_board + offset_b + i*width_bytes);
			for( j=0; j < (width_bytes-2*xborder)/(8*density); j++ )
            {
                for( k=0; k<density; k++ )
                {
       	            byte temp = *dst;
       	            *dst++ = *src;
       	            *src++ = temp;
                }
            }
		}
        offset_w += (SQUARE_HEIGHT+10)*width_bytes;
        offset_b -= (SQUARE_HEIGHT+10)*width_bytes;
    }
}






