/****************************************************************************
 * Control - A coloured rectangular box, subclass for user interaction
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
//#include "wx/dcbuffer.h"
#include "Objects.h"
#include "GameLogic.h"
#include "DebugPrintf.h"
#include "CtrlBox.h"
#include "CtrlBoxBookMoves.h"

IMPLEMENT_CLASS( CtrlBox, wxControl )
BEGIN_EVENT_TABLE( CtrlBox, wxControl )
    EVT_PAINT(CtrlBox::OnPaint)
//    EVT_SIZE(CtrlBox::OnSize)
//    EVT_MOUSE_CAPTURE_LOST(CtrlBox::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(CtrlBox::OnMouseEvent)
//    EVT_LEFT_UP (CtrlBox::OnMouseLeftUp)
    EVT_LEFT_DOWN (CtrlBox::OnMouseLeftDown)
//    EVT_MOTION (CtrlBox::OnMouseMove)
//    EVT_ERASE_BACKGROUND(CtrlBox::OnEraseBackground)
//    EVT_TIMER( TIMER_ID, CtrlBox::OnTimeout)
END_EVENT_TABLE()

// Initialise the box
CtrlBox::CtrlBox
(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& point,
    const wxSize& size,
    long style
)   : wxControl( parent, id, point, size, style )
{
    r.x = point.x;
    r.y = point.y;
    r.width  = size.x;
    r.height = size.y;
    txt = "";
    hack_its_a_clock = false;
}

// Write the text
void CtrlBox::SetTxt( const wxString &txt, bool redraw )
{
    if( this->txt != txt )
    {
        this->txt = txt;
        if( redraw )
        {
            Refresh(true);
            Update();
        }
    }
}

void CtrlBox::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    int h, w;
    dc.SetFont( GetFont() );
    dc.GetTextExtent( txt, &w, &h );
    //dc.SetTextForeground( *wxBLACK );
    //dc.SetTextBackground( *wxGREEN );
    //if( w<r.width && h<r.height )
    {
        dc.DrawText( txt, (r.width-w)/2, (r.height-h)/2 );
        dbg_printf( "CtrlBox::OnPaint(%s)", txt.c_str() );
    }
}


void CtrlBox::OnMouseLeftDown( wxMouseEvent &WXUNUSED(event) )
{
    if( hack_its_a_clock )
        objs.gl->CmdClocks();
}


