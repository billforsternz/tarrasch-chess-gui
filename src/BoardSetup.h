/****************************************************************************
 * Draw the graphic board in the board setup control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BOARD_SETUP_H
#define BOARD_SETUP_H
#include "wx/wx.h"
#include "thc.h"
#include "Portability.h"

class BoardSetup
{
public:
    void Debug();

	// Con/De structor
	BoardSetup( wxBitmap *bitmap, wxWindow *parent, int XBORDER, int YBORDER, bool normal_orientation=true );
	~BoardSetup();

    // Figure out whether a piece or square is pointed to
    bool HitTest( wxPoint &point, char &piece, char &file, char &rank );

	// Setup a position	on the graphic board
	void Set( const thc::ChessPosition &cp, const bool *highlight=0 );

	// Draw the graphic board
    void Draw();

private:

	// Data members
    wxBrush brush;
	wxMemoryDC dcmem;
    wxPen pen;
    wxBitmap *bitmap;
    wxWindow *parent;
    int XBORDER;
    int YBORDER;
	byte         *buf_board;
	byte         *buf_box;
	unsigned long width_bytes, height, width, xborder, yborder, density;
    unsigned long highlight_y_lo1;
    unsigned long highlight_y_lo2;
    unsigned long highlight_y_hi2;
    unsigned long highlight_y_hi1;
    unsigned long highlight_x_lo1;
    unsigned long highlight_x_lo2;
	unsigned long highlight_x_hi2;
    unsigned long highlight_x_hi1;
	bool		 normal_orientation;

	// Helpers
	unsigned long   Offset( char file, char rank );
	void Get( char src_file, char src_rank, char dst_file, char dst_rank );
	void Put( char src_file, char src_rank, char dst_file, char dst_rank, bool highlight_f );
    void SwapPickupPieces();

};

#endif // BOARD_SETUP_H

