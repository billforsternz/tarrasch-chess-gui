/****************************************************************************
 * Chess Board bitmap utilities
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESS_BOARD_BITMAP_H
#define CHESS_BOARD_BITMAP_H

#include "wx/wx.h"
#include "Portability.h"
#include "thc.h"

class ChessBoardBitmap
{
public:

	// Con/De structor
	ChessBoardBitmap();
	~ChessBoardBitmap();
    void Init( int pix );
	void BuildMiniboardBitmap( int pix, wxBitmap &bm );
	void BuildBoardSetupBitmap( int pix, wxBitmap &bm );
	void BuildCustomCursors( int pix );

	// Setup a position	on the graphic board
	void SetChessPosition( const char *position_ascii );

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

    // Setup a position	on the graphic board
    void SetPositionEx( thc::ChessPosition pos, bool blank_other_squares, char pickup_file, char pickup_rank, wxPoint shift );

    wxBitmap    my_chess_bmp;
	wxImage		white_king_cursor;	
	wxImage		white_queen_cursor;	
	wxImage		white_rook_cursor;	
	wxImage		white_bishop_cursor;
	wxImage		white_knight_cursor;
	wxImage		white_pawn_cursor;	
	wxImage		black_king_cursor;	
	wxImage		black_queen_cursor;	
	wxImage		black_rook_cursor;	
	wxImage		black_bishop_cursor;
	wxImage		black_knight_cursor;
	wxImage		black_pawn_cursor;	

private:

	// Data members
	wxColour	 light_colour;
	wxColour	 dark_colour;
	wxSize       current_size;
    wxBrush      brush;
	wxMemoryDC   dcmem;
    wxPen        pen;
	byte         *buf_board;
	byte         *buf_box;
	unsigned long width_bytes, height, width, xborder, yborder, density;
	bool		 normal_orientation;
	char		 highlight_file1, highlight_rank1;
	char		 highlight_file2, highlight_rank2;
    char         _position_ascii[100];
    std::string  str_white_king_mask;
    const char  *white_king_mask;
    std::string  str_white_queen_mask;
    const char  *white_queen_mask;
    std::string  str_white_knight_mask;
    const char  *white_knight_mask;
    std::string  str_white_bishop_mask;
    const char  *white_bishop_mask;
    std::string  str_white_rook_mask;
    const char  *white_rook_mask;
    std::string  str_white_pawn_mask;
    const char  *white_pawn_mask;
    std::string  str_black_king_mask;
    const char  *black_king_mask;
    std::string  str_black_queen_mask;
    const char  *black_queen_mask;
    std::string  str_black_knight_mask;
    const char  *black_knight_mask;
    std::string  str_black_bishop_mask;
    const char  *black_bishop_mask;
    std::string  str_black_rook_mask;
    const char  *black_rook_mask;
    std::string  str_black_pawn_mask;
    const char  *black_pawn_mask;

	// Helpers
	unsigned long   Offset( char file, char rank );
	void Get( char src_file, char src_rank, char dst_file, char dst_rank, const char *mask = NULL );
	void Put( char src_file, char src_rank, char dst_file, char dst_rank );

    // Put a shifted, masked piece from box onto board
    void PutEx( char piece, char dst_file, char dst_rank, wxPoint shift );

private:
    bool         sliding;
    char         pickup_file;
    char         pickup_rank;
    wxPoint      pickup_point;
    thc::ChessPosition     slide_pos;
};

#endif // CHESS_BOARD_BITMAP_H

