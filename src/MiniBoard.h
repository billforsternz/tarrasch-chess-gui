/****************************************************************************
 * Control - A custom control, a mini board to display current position
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef MINI_BOARD_H
#define MINI_BOARD_H

#include "wx/wx.h"
#include "ChessBoardBitmap.h"
#include "GraphicBoardResizable.h"

class MiniBoard : public wxControl
{
public:
    MiniBoard( wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& point = wxDefaultPosition,
                const wxSize&  size = wxDefaultSize );
    void Set( const thc::ChessPosition &cp_ )
	{
		this->cp=cp_;
		cbb.SetChessPosition( cp.squares, true );
		UpdateBoard();
	}
    
private:
    thc::ChessPosition cp;
    ChessBoardBitmap   cbb;
    void UpdateBoard();
    void OnPaint(wxPaintEvent &evt );
	void OnSize( wxSizeEvent &evt );
    void OnEraseBackground(wxEraseEvent &evt );
private:
    DECLARE_EVENT_TABLE()
};

#endif MINI_BOARD_H
