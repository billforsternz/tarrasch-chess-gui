/****************************************************************************
 * Control - A custom control, a mini board to display current position
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/image.h"
#include "wx/file.h"
#include "Appdefs.h"
#include "thc.h"
#include "DebugPrintf.h"
#include "ChessBoardBitmap.h"
#include "MiniBoard.h"
using namespace std;
using namespace thc;

// Initialise the MiniBoard
MiniBoard::MiniBoard
(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& point,
    const wxSize& size
)   : wxControl( parent, id, point, size, wxBORDER_NONE )
{
    strcpy( cbb._position_ascii, "rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR" );
	wxSize sz=size;
    //strcpy( cbb._position_ascii, "rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR" );
	if( sz.x<=0 || sz.y<=0 )
	{
		sz = wxSize(34*8,34*8);
		SetSize(sz);
	}
	cbb.current_size = sz;
    int min = sz.x<sz.y ? sz.x : sz.y;
    int pix_ = min/8;
    cbb.Init( pix_ );
//	cbb.SetChessPosition( "rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR", true );
//	thc::ChessPosition cp_;
//	Set(cp_);
}

void MiniBoard::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if( cbb.my_chess_bmp.Ok() )
    {
        dc.DrawBitmap( cbb.my_chess_bmp, 0, 0, true );// board_rect.x, board_rect.y, true );
    }
}

void MiniBoard::OnSize( wxSizeEvent &evt )
{
    wxSize size = evt.GetSize();
/*	cbb.current_size = size;
    int min = size.x<size.y ? size.x : size.y;
    int pix_ = min/8;
    cbb.Init( pix_ );

  */
	    cprintf( "Child: resize x=%d, y=%d\n", size.x, size.y );
    char temp[ sizeof(cbb._position_ascii) + 1 ];
    strcpy( temp, cbb._position_ascii );
    SetSize(size);
	cbb.current_size = size;
    int min = size.x<size.y ? size.x : size.y;
    int pix_ = min/8;
    cbb.Init( pix_ );
    cbb.SetChessPosition( temp, true );
    Refresh(false);
    Update();

}

void MiniBoard::UpdateBoard()
{
    Refresh(false);
    Update();
}

void MiniBoard::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

BEGIN_EVENT_TABLE(MiniBoard, wxControl)
EVT_PAINT(MiniBoard::OnPaint)
EVT_SIZE(MiniBoard::OnSize)
EVT_ERASE_BACKGROUND(MiniBoard::OnEraseBackground)
END_EVENT_TABLE()
