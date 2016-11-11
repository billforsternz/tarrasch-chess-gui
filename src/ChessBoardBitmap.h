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

	// Create bitmap, either board setup bitmap or naked chessboard
	void CreateAsChessBoardOnly( const wxSize sz, bool normal_orientation, const thc::ChessPosition &cp, const bool *highlight=0 );
	void CreateAsBoardSetup( const wxSize sz, bool normal_orientation, const thc::ChessPosition &cp, const bool *highlight=0 );

	// Chessboard bitmap
private:
    wxBitmap  chess_board_bmp;
public:
	wxBitmap *GetChessBoardBmp() { return &chess_board_bmp; }
	void SetChessPosition( const thc::ChessPosition &pos, const bool *highlight=0 );
    void SetChessPositionShiftedPiece( const thc::ChessPosition &pos, bool blank_other_squares, char pickup_file, char pickup_rank, wxPoint shift );

	// Board Setup bitmap
private:
    wxBitmap  board_setup_bmp;
public:
	wxBitmap *GetBoardSetupBmp() { return &board_setup_bmp; }
	void GetCustomCursor( char piece, wxImage &img );
	bool BoardSetupHitTest( const wxPoint &point, char &piece, char &file, char &rank );

	// Get/Set orientation
	void SetNormalOrientation( bool _normal_orientation )
					{ normal_orientation = _normal_orientation; }
	bool GetNormalOrientation()
					{ return normal_orientation; }

	// Set highlight squares
	void SetHighlight1( thc::Square sq ) { highlight_square1=sq; }
	void SetHighlight2( thc::Square sq ) { highlight_square2=sq; }
	void ClearHighlight1()			     { highlight_square1=thc::SQUARE_INVALID; }
	void ClearHighlight2()			     { highlight_square2=thc::SQUARE_INVALID; }

public:
	int			 dim_pix;
	thc::Square	 highlight_square1;
	thc::Square	 highlight_square2;
	bool		 normal_orientation;
    bool         sliding;
    char         pickup_file;
    char         pickup_rank;
    wxPoint      pickup_point;
	unsigned long width_bytes, height, width, density;
    thc::ChessPosition	pos_slide;
    thc::ChessPosition	pos_current;
	bool				highlight_current[64];
private:
	bool         is_board_setup;
	bool		 ok_to_copy_chess_board_to_board_setup;
	byte         *buf_board;
	byte         *buf_box;
	enum { none, line_thin, line_thick, square_colour } highlight_mode;

	// Data members
	wxColour	 light_colour;
	wxColour	 dark_colour;
	wxColour	 highlight_light_colour;
	wxColour	 highlight_dark_colour;
	wxColour	 highlight_line_colour;
    wxBrush      brush;
	wxMemoryDC   dcmem;
    wxPen        pen;
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
    unsigned long highlight_y_lo1;
    unsigned long highlight_y_lo2;
    unsigned long highlight_y_hi2;
    unsigned long highlight_y_hi1;
    unsigned long highlight_x_lo1;
    unsigned long highlight_x_lo2;
	unsigned long highlight_x_hi2;
    unsigned long highlight_x_hi1;

	// Dimensions of constructed board setup bitmap
	wxSize		dim_sz;
	wxPoint		dim_board;
	wxPoint		dim_pickup_left;
	wxPoint		dim_pickup_right;
	int			dim_pickup_pitch;
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

	// Helpers
	void BoardSetupDim( const wxSize sz );
	void BoardSetupCreate();
	void BoardSetupCustomCursorsCreate();
    void ChessBoardCreate( int pix, const thc::ChessPosition &cp, const bool *highlight=0 );

	unsigned long   Offset( char file, char rank );
	void Get( char src_file, char src_rank, char dst_file, char dst_rank, const char *mask = NULL );
	void Put( char src_file, char src_rank, char dst_file, char dst_rank, bool highlight_f );

    // Put a shifted, masked piece from box onto board
    void PutEx( char piece, char dst_file, char dst_rank, wxPoint shift );
};

#endif // CHESS_BOARD_BITMAP_H

