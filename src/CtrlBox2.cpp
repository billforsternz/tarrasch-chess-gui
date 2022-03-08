/****************************************************************************
 * Control - A coloured rectangular box, subclass for user interaction
 *           This is an alternative implementation to CtrlBox2
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "Objects.h"
#include "GameLogic.h"
#include "DebugPrintf.h"
#include "CtrlBox2.h"
#include "CtrlBoxBookMoves.h"

IMPLEMENT_CLASS( CtrlBox2, wxTextCtrl )
BEGIN_EVENT_TABLE( CtrlBox2, wxTextCtrl )
//    EVT_PAINT(CtrlBox2::OnPaint)
//    EVT_SIZE(CtrlBox2::OnSize)
    EVT_MOUSE_CAPTURE_LOST(CtrlBox2::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(CtrlBox2::OnMouseEvent)
//    EVT_LEFT_UP (CtrlBox2::OnMouseLeftUp)
    EVT_SET_CURSOR(CtrlBox2::OnSetCursor)
    EVT_LEFT_DOWN (CtrlBox2::OnMouseLeftDown)
//    EVT_MOTION (CtrlBox2::OnMouseMove)
    EVT_ERASE_BACKGROUND(CtrlBox2::OnEraseBackGround)
//    EVT_TIMER( TIMER_ID, CtrlBox2::OnTimeout)
END_EVENT_TABLE()

// Initialise the box
CtrlBox2::CtrlBox2
(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& point,
    const wxSize& size,
    long style
)   : wxTextCtrl( parent, id, "", point, size, wxTE_CENTRE|wxTE_READONLY|style )
{
    r.x = point.x;
    r.y = point.y;
    r.width  = size.x;
    r.height = size.y;
    txt = "";
    hack_its_a_clock = false;
}

// Write the text
void CtrlBox2::SetTxt( const wxString &txt_, bool WXUNUSED(redraw) )
{
    if( this->txt != txt_ )
    {
        this->txt = txt_;
        Clear();
        WriteText(txt_);
     /*   if( redraw )
        {
            Refresh(true);
            Update();
        } */
    }
}

/*
void CtrlBox2::OnPaint( wxPaintEvent& WXUNUSED(event) )
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
        dbg_printf( "CtrlBox2::OnPaint(%s)", txt.c_str() );
    }
}

        */

void CtrlBox2::OnMouseLeftDown( wxMouseEvent &WXUNUSED(event) )
{
    if( hack_its_a_clock )
        objs.gl->CmdClocks();
}

void CtrlBox2::OnSetCursor( wxSetCursorEvent &event )
{
    // Without this the native control sets a beam cursor
    wxCursor x(wxCURSOR_ARROW);
    SetCursor(x);
    event.Skip();   // let wxWidgets continue to process the event, somehow
                    //  it stops the native control setting the beam cursor
}

void CtrlBox2::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    //event.Skip(true);
    //event.StopPropagation();
}

