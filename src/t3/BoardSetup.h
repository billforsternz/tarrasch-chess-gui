/****************************************************************************
 * Draw the graphic board in the board setup control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BOARD_SETUP_H
#define BOARD_SETUP_H
#include "wx/wx.h"
#include "Portability.h"

class BoardSetup
{
public:
    void Debug();

	// Con/De structor
	BoardSetup( wxBitmap *bitmap, wxWindow *parent, int XBORDER, int YBORDER );
	~BoardSetup();

    // Figure out whether a piece or square is pointed to
    bool HitTest( wxPoint &point, char &piece, char &file, char &rank );

	// Setup a position	on the graphic board
	void SetPosition( const char *position_ascii );

	// Setup highlight flags
	void SetLockdown( const bool *lockdown ) { memcpy(this->lockdown,lockdown,sizeof(this->lockdown)); }

	// Draw the graphic board
    void Draw();

	// Get/Set orientation
	void SetNormalOrientation( bool _normal_orientation )
					{ normal_orientation = _normal_orientation; }
	bool GetNormalOrientation()
					{ return normal_orientation; }

	// Set highlight squares
	void SetHighlight1( char file, char rank ) { highlight_file1=file;
											     highlight_rank1=rank; }
	void SetHighlight2( char file, char rank ) { highlight_file2=file;
	                                             highlight_rank2=rank; }
	void ClearHighlight1()			   { highlight_file1='\0'; }
	void ClearHighlight2()			   { highlight_file2='\0'; }

private:

	// Data members
    wxBrush brush;
	wxMemoryDC dcmem;
    wxPen pen;
    wxBitmap *bitmap;
    wxWindow *parent;
    int XBORDER;
    int YBORDER;

  	//CStatic      *board_window;
	//wxBitmap	 *gbl_bm;
	byte         *buf_board;
	byte         *buf_box;
	unsigned long width_bytes, height, width, xborder, yborder, density;
    unsigned long lockdown_y_lo1;
    unsigned long lockdown_y_lo2;
    unsigned long lockdown_y_hi2;
    unsigned long lockdown_y_hi1;
    unsigned long lockdown_x_lo1;
    unsigned long lockdown_x_lo2;
	unsigned long lockdown_x_hi2;
    unsigned long lockdown_x_hi1;

	bool		 normal_orientation;
	char		 highlight_file1, highlight_rank1;
	char		 highlight_file2, highlight_rank2;
    char         _position_ascii[100];
    bool         lockdown[64];

	// Helpers
	unsigned long   Offset( char file, char rank );
	void Get( char src_file, char src_rank, char dst_file, char dst_rank );
	void Put( char src_file, char src_rank, char dst_file, char dst_rank );

    // Put a shifted, masked piece from box onto board
//    void PutEx( char piece, char dst_file, char dst_rank, wxPoint shift );
};

#endif // BOARD_SETUP_H

