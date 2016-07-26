/****************************************************************************
 * Control - My own popup menu (wxWidget popup menu is Windows specific)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "Appdefs.h"
#include "GameLogic.h"
#include "thc.h"
#include "DebugPrintf.h"
#include "Objects.h"
#include "PopupControl.h"
using namespace std;
using namespace thc;

PopupControl::PopupControl
(
    wxWindow* parent,
    wxArrayString &strs,
    wxArrayString &terses,
    wxArrayString &book,
    POPUP_MODE     popup_mode,  
    wxRect        &hover,
    wxWindowID     id, //= wxID_ANY,
    const wxPoint &point //= wxDefaultPosition
)  : wxControl( parent, id, point, wxDefaultSize, wxNO_BORDER )
/*#define wxDOUBLE_BORDER         wxBORDER_DOUBLE
#define wxSUNKEN_BORDER         wxBORDER_SUNKEN
#define wxRAISED_BORDER         wxBORDER_RAISED
#define wxBORDER                wxBORDER_SIMPLE
#define wxSIMPLE_BORDER         wxBORDER_SIMPLE
#define wxSTATIC_BORDER         wxBORDER_STATIC
#define wxNO_BORDER             wxBORDER_NONE */

{
    upside_down = false;
    this->popup_mode = popup_mode;
    this->point      = point;
    this->hover      = hover;
    wxClientDC dc(parent);
    dc.SetFont(*wxNORMAL_FONT);
    int h, w, max_w=0, max_h=0, n=strs.GetCount();
    for( int i=0; i<n; i++ )
    {
        wxString s = strs[i];
        dc.GetTextExtent( s, &w, &h );
        if( w > max_w )
            max_w = w;
        if( h > max_h )
            max_h = h;
    }
    max_w = (max_w*50)/38;
    max_h = (max_h*50)/38;
    if( max_w < 4*max_h )
        max_w = 4*max_h;
    this->height = max_h;
    wxSize  size( max_w, n*max_h );
    wxPoint pos = point;
    release_button_to_exit = (popup_mode==CLICK_ON_DESTINATION);
    wxRect rect = objs.canvas->GetRect();
    if( popup_mode != BOOK_HOVER )
    {
        pos.x -= (4*size.x)/5;
        pos.y -= (max_h/2);
    }

    // If there's not enough room - do it upside down
    if( popup_mode != BOOK_HOVER && (pos.y+size.y) >= (rect.height-4) )
        upside_down = true;
    dbg_printf( "rect.x=%d rect.y=%d rect.width=%d rect.height=%d\n", rect.x, rect.y, rect.width, rect.height );
    dbg_printf( "pos.x=%d, pos.y=%d, size.y=%d\n", pos.x, pos.y, size.y );
    dbg_printf( "upside_down = %s\n", upside_down?"true":"false" );
    if( upside_down )
        pos.y -= (size.y-height);
    SetPosition( pos );
    SetSize    ( size );
    dbg_printf( "*PopupControl::CaptureMouse*\n" );
    CaptureMouse();
    done   = false;
    terse_move[0] = '\0';
    this->strs = strs;
    this->terses = terses;
    this->book   = book;
    count = strs.GetCount();
    if( count == 0 )
        count = 1;
    sel = 0;
}

void PopupControl::OnMouseMove( wxMouseEvent& event )
{
    bool in_region=false;
    wxSize size = GetClientSize();
    //int height = size.y / count;
    wxPoint point_ = event.GetPosition();
    dbg_printf( "popup; point.x=%d, point.y=%d\n", point_.x, point_.y );
    if( popup_mode==BOOK_HOVER && 0<=point_.x && point_.x<hover.width && (0-hover.height)<=point_.y && point_.y<0 )
    {
        in_region = true;
        dbg_printf( "In region 1\n" );
    }
    if( 0<=point_.x && point_.x<size.x && 0<=point_.y && point_.y<size.y )
    {
        in_region = true;
        dbg_printf( "In region 2\n" );
        int new_sel = point_.y/height;
        if( upside_down )
            new_sel = count-1-new_sel;  // 0->count-1, count-1->0
        if( new_sel!=sel && new_sel<count )
        {
            sel = new_sel;
            Refresh(false);
        }
    }
    if( popup_mode==BOOK_HOVER && !in_region )
        Shutdown(false);
}

void PopupControl::Shutdown( bool inject_move )
{
    ReleaseMouse();
    //wxPoint point = event.GetPosition();
    //dbg_printf( "*PopupControl::ReleaseMouse*: Up x=%d, y=%d\n", point.x, point.y );
    memset( terse_move, 0, sizeof(terse_move) );
    if( inject_move && 0<=sel && sel<count )
    {
        wxString s = terses[sel];
        memcpy( terse_move, s.c_str(),sizeof(terse_move)-1 );
    }
    done = true;
}

void PopupControl::OnMouseLeftUp( wxMouseEvent& WXUNUSED(event) )
{
    if( release_button_to_exit )
        Shutdown( true );
}

void PopupControl::OnMouseLeftDown( wxMouseEvent& WXUNUSED(event) )
{
    if( !release_button_to_exit )
        Shutdown( true );
}

void PopupControl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    wxSize size = GetClientSize();
    wxString s;
    int y=0; //, height;
    dc.SetFont(*wxNORMAL_FONT);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxLIGHT_GREY_PEN);
    //s = strs[0];
    //dc.GetTextExtent(s, &w, &height);
    //height = this->height; //size.y / (count?count:1);
    dbg_printf( "size.x=%d, size.y=%d\n", size.x, size.y  );
 // dc.DrawRectangle( 0, 0, 68, 16 );      // need to get the rectangles overlapping, so 
 // dc.DrawRectangle( 0, 15, 68, 17 );     //  tweak 16 to 15 or 17, see tweaks below
 // dc.DrawRectangle( 0, 31, 68, 17 );
 // dc.DrawRectangle( 0, 47, 68, 17 );
 // dc.DrawRectangle( 0, 0, size.x, size.y );
    for( int i=0; i<count; i++ )
    {
        int idx = (upside_down ? count-1-i : i);
        s = strs[idx];
        if( idx == sel )
        {
            dc.SetBrush(*wxLIGHT_GREY_BRUSH);
            dbg_printf( "LIGHT_GREY_BRUSH\n" );
        }
        else if( book[idx]=="B" && popup_mode!=BOOK_HOVER )
        {
            dc.SetBrush(*wxGREEN_BRUSH);
            dbg_printf( "GREEN_BRUSH\n" );
        }
        else
        {
            dc.SetBrush(*wxWHITE_BRUSH);
            dbg_printf( "WHITE_BRUSH\n" );
        }
        int X = 0;
        int Y = (i==0?y:y-1);       // tweak 16 to 15 when i=0
        int W = size.x;
        int H = height + (i==0?0:1);  // tweak 16 to 17 when i!=0
        dc.DrawRectangle( X, Y, W, H );
        dbg_printf( "height = %d\n", height );
        dbg_printf( "DrawRectangle %d, %d, %d, %d\n", X, Y, W, H );
        //dc.DrawLine(0, y+height -1, size.x -1, y+height  -1);
        //dbg_printf( "DrawLine %d, %d, %d, %d\n", 0, y+height, size.x, y+height );
        dc.DrawText( s, 0+4, y+2 );
        dbg_printf( "DrawText %d, %d, %s\n", 0+4, y+2, s );
        y += height;
    }
 /*
    s = strs[1];
    dc.GetTextExtent(s, &w, &h);
    dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*1));

    s.Printf(wxT("Dock: %d Row: %d"), 24, 25 );
    dc.GetTextExtent(s, &w, &h);
    dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*2));

    s.Printf(wxT("Position: %d"), 27 );
    dc.GetTextExtent(s, &w, &h);
    dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*3));

    s.Printf(wxT("Proportion: %d"), 28 );
    dc.GetTextExtent(s, &w, &h);
    dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*4)); */
}

void PopupControl::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

void PopupControl::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    Refresh();
}

void PopupControl::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    Shutdown(false);
    //event.Skip(true);
    //event.StopPropagation();
}

BEGIN_EVENT_TABLE(PopupControl, wxControl)
    EVT_PAINT(PopupControl::OnPaint)
    EVT_SIZE(PopupControl::OnSize)
    EVT_MOUSE_CAPTURE_LOST(PopupControl::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(PopupControl::OnMouseEvent)
    EVT_LEFT_UP (PopupControl::OnMouseLeftUp)
    EVT_LEFT_DOWN (PopupControl::OnMouseLeftDown)
    EVT_MOTION (PopupControl::OnMouseMove)
    EVT_ERASE_BACKGROUND(PopupControl::OnEraseBackground)
END_EVENT_TABLE()

