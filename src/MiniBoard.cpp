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
#include "BoardSetup.h"
#include "GraphicBoardResizable.h"
//#include "Objects.h"
//#include "PanelContext.h"
#include "MiniBoard.h"
using namespace std;
using namespace thc;

MiniBoard::MiniBoard
(
 wxWindow* parent,
 wxWindowID     id, //= wxID_ANY,
 const wxPoint &point //= wxDefaultPosition
 )  : wxControl( parent, id, point, wxDefaultSize, wxBORDER_NONE )
{
    bs = NULL;
    wxClientDC dc(parent);
    //chess_bmp = static_chess_bmp; //wxBitmap( board_setup_bitmap_xpm );
    //chess_bmp = wxBitmap( miniboard_bitmap_xpm );
	//objs.canvas->pb->gb->GenerateMiniboard( 34, chess_bmp );
	GraphicBoardResizable gb(34);		// TEMP TEMP 34 must match SQUARE_HEIGHT _WIDTH in BoardSetup.cpp
	chess_bmp = gb.my_chess_bmp;
    wxSize size( chess_bmp.GetWidth(), chess_bmp.GetHeight() );
    SetSize( size );
    bs = new BoardSetup( &chess_bmp, this, 0, 0 );
}

MiniBoard::~MiniBoard()
{
    if( bs )
        delete bs;
}

void MiniBoard::UpdateBoard()
{
    bs->Set(cp);
    bs->Draw();
}


void MiniBoard::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if( chess_bmp.Ok() )
    {
        dc.DrawBitmap( chess_bmp, 0, 0, true );
    }
}

void MiniBoard::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

void MiniBoard::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    Refresh();
}

BEGIN_EVENT_TABLE(MiniBoard, wxControl)
EVT_PAINT(MiniBoard::OnPaint)
EVT_SIZE(MiniBoard::OnSize)
EVT_ERASE_BACKGROUND(MiniBoard::OnEraseBackground)
END_EVENT_TABLE()

