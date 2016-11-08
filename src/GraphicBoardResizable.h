/****************************************************************************
 * Control - The graphic chess board that is the centre-piece of the GUI
 *  Still working on making it a more-self contained and reusable control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef GRAPHICBOARD_RESIZABLE_H
#define GRAPHICBOARD_RESIZABLE_H

#include "wx/wx.h"
#include "Portability.h"
#include "thc.h"
#include "ChessBoardBitmap.h"

class GraphicBoardResizable : public wxControl
{
public:

	// Con/De structor
	GraphicBoardResizable( wxWindow *parent,
			//ADDED BEGIN
                  wxWindowID id=wxID_ANY,
                const wxPoint& point = wxDefaultPosition,
                const wxSize&  size = wxDefaultSize );
    void Set( const thc::ChessPosition &cp_ )
	{
		this->cp=cp_;
		cbb.SetChessPosition( cp.squares, cbb.normal_orientation );
		UpdateBoard();
	}
    thc::ChessPosition cp;
	void UpdateBoard() { Refresh(false); Update(); }
	bool interactive;
			//ADDED END

	void Init( int pix_ );

	// Find a square within the graphic board
	void HitTest( wxPoint hit, char &file, char &rank );
    void HitTestEx( char &file, char &rank, wxPoint shift );

	// Setup a position	on the graphic board
	void SetChessPosition( char *position_ascii );

	// Draw the graphic board
    void Draw();
    void OnPaint(wxPaintEvent& WXUNUSED(evt));
    void SetBoardSize( wxSize &sz );
	void Redraw() { wxSize sz=GetSize(); SetBoardSize(sz); }

	// Get/Set orientation
	void SetNormalOrientation( bool _normal_orientation )
					{ cbb.normal_orientation = _normal_orientation; }
	bool GetNormalOrientation()
					{ return cbb.normal_orientation; }

	// Set highlight squares
	void SetHighlight1( char file, char rank ) { cbb.highlight_file1=file;
											     cbb.highlight_rank1=rank; }
	void SetHighlight2( char file, char rank ) { cbb.highlight_file2=file;
	                                             cbb.highlight_rank2=rank; }
	void ClearHighlight1()			   { cbb.highlight_file1='\0'; }
	void ClearHighlight2()			   { cbb.highlight_file2='\0'; }

    // Setup a position	on the graphic board
    //void SetPositionEx( thc::ChessPosition pos, bool blank_other_squares, char pickup_file, char pickup_rank, wxPoint shift );

    void OnMouseLeftDown (wxMouseEvent & event);
    void OnMouseLeftUp (wxMouseEvent & event);
    void OnMouseMove (wxMouseEvent & event);
	void OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) );

private:
    DECLARE_EVENT_TABLE()
	ChessBoardBitmap cbb;
    int          PIX;
    int          pix;
};

#endif // GRAPHICBOARD_RESIZABLE_H

