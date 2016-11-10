/****************************************************************************
 * Control - A graphic chess board with (optional) moveable pieces
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef CTRL_CHESS_BOARD_H
#define CTRL_CHESS_BOARD_H

#include "wx/wx.h"
#include "Portability.h"
#include "thc.h"
#include "ChessBoardBitmap.h"

class CtrlChessBoard : public wxControl
{
public:

	// Con/De structor
	CtrlChessBoard( bool interactive_,
					bool normal_orientation,
					wxWindow *parent,
					wxWindowID id=wxID_ANY,
					const wxPoint& point = wxDefaultPosition,
					const wxSize&  size = wxDefaultSize );
    void SetChessPosition( const thc::ChessPosition &cp, const bool *highlight=0 )
	{
		cbb.SetChessPosition( cp, highlight );
		UpdateBoard();
	}

	// Find a square within the graphic board
	void HitTest( wxPoint hit, char &file, char &rank );
    void HitTestEx( char &file, char &rank, wxPoint shift );

	// Draw or Redraw the graphic board
	void UpdateBoard() { Refresh(false); Update(); }	// light
    void SetBoardSize( wxSize &sz );					// heavy
	void Redraw() { wxSize sz=GetSize(); SetBoardSize(sz); }

	// Get/Set orientation
	void SetNormalOrientation( bool normal_orientation )
					{ cbb.normal_orientation = normal_orientation; }
	bool GetNormalOrientation()
					{ return cbb.normal_orientation; }

	// Set highlight squares
	void SetHighlight1( thc::Square sq ) { cbb.SetHighlight1(sq); }
	void SetHighlight2( thc::Square sq ) { cbb.SetHighlight2(sq); }
	void ClearHighlight1()			     { cbb.ClearHighlight1(); }
	void ClearHighlight2()			     { cbb.ClearHighlight2(); }

    void OnPaint(wxPaintEvent& WXUNUSED(evt));
    void OnMouseLeftDown (wxMouseEvent & event);
    void OnMouseLeftUp (wxMouseEvent & event);
    void OnMouseMove (wxMouseEvent & event);
	void OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) );

private:
    DECLARE_EVENT_TABLE()
	ChessBoardBitmap cbb;
	bool interactive;
};

#endif // CTRL_CHESS_BOARD_H
