/****************************************************************************
 * Control - The graphic chess board that is the centre-piece of the GUI
 *  Still working on making it a more-self contained and reusable control
 ****************************************************************************/
#ifndef GRPHICBOARD_RESIZABLE_H
#define GRAPHICBOARD_RESIZABLE_H

// This resizable version is just getting started - don't #define RESIZABLE_BOARD
// in Canvas.h until it's much more mature! 


#include "wx/wx.h"
#include "Portability.h"
#include "thc.h"

class GraphicBoardResizable : public wxControl
{
public:

	// Con/De structor
	GraphicBoardResizable( wxWindow *parent,
                  wxWindowID id,
                  const wxPoint& point,
                  const wxSize& size,
                  int nbr_pixels  );
	~GraphicBoardResizable();

	// Find a square within the graphic board
	void HitTest( wxPoint hit, char &file, char &rank );
    void HitTestEx( char &file, char &rank, wxPoint shift );

	// Setup a position	on the graphic board
	void SetPosition( char *position_ascii );

	// Draw the graphic board
    void Draw();
    void OnPaint(wxPaintEvent& WXUNUSED(evt));

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

    void OnMouseLeftDown (wxMouseEvent & event);
    void OnMouseLeftUp (wxMouseEvent & event);
    void OnMouseMove (wxMouseEvent & event);

private:

	// Data members
    wxBrush      brush;
	wxMemoryDC   dcmem;
    wxPen        pen;
    wxBitmap     my_chess_bmp;
	byte         *buf_board;
	byte         *buf_box;
	unsigned long width_bytes, height, width, xborder, yborder, density;
	bool		 normal_orientation;
	char		 highlight_file1, highlight_rank1;
	char		 highlight_file2, highlight_rank2;
    char         _position_ascii[100];
    const char  *white_king_mask;
    const char  *white_queen_mask;
    const char  *white_knight_mask;
    const char  *white_bishop_mask;
    const char  *white_rook_mask;
    const char  *white_pawn_mask;
    const char  *black_king_mask;
    const char  *black_queen_mask;
    const char  *black_knight_mask;
    const char  *black_bishop_mask;
    const char  *black_rook_mask;
    const char  *black_pawn_mask;

	// Helpers
	unsigned long   Offset( char file, char rank );
	void Get( char src_file, char src_rank, char dst_file, char dst_rank, const char *mask = NULL );
	void Put( char src_file, char src_rank, char dst_file, char dst_rank );

    // Put a shifted, masked piece from box onto board
    void PutEx( char piece, char dst_file, char dst_rank, wxPoint shift );

private:
    DECLARE_EVENT_TABLE()
    bool         sliding;
    char         pickup_file;
    char         pickup_rank;
    wxPoint      pickup_point;
    thc::ChessPosition     slide_pos;
};

#endif // GRAPHICBOARD_RESIZABLE_H

