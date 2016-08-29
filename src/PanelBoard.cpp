/****************************************************************************
 * Top level UI panel for providing chess board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "DebugPrintf.h"
#include "PanelBoard.h"

//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PanelBoard, wxPanel)
    EVT_SIZE( PanelBoard::OnSize )
/*  EVT_CHAR( PanelBoard::OnChar )
    EVT_LEFT_DOWN (PanelBoard::OnMouseLeftDown)
    EVT_MOTION (PanelBoard::OnMouseMove)
    EVT_PAINT (PanelBoard::OnPaint)
    EVT_MOUSE_CAPTURE_LOST(PanelBoard::OnMouseCaptureLost)
    EVT_MOUSEWHEEL(PanelBoard::OnMouseWheel)
    EVT_BUTTON(ID_BUTTON1,PanelBoard::OnButton1)
    EVT_BUTTON(ID_BUTTON2,PanelBoard::OnButton2)
    EVT_BUTTON(ID_BUTTON3,PanelBoard::OnButton3)
    EVT_BUTTON(ID_BUTTON4,PanelBoard::OnButton4)
    EVT_BUTTON(ID_KIBITZ_BUTTON1,PanelBoard::OnKibitzButton1)
    EVT_BUTTON(ID_KIBITZ_BUTTON2,PanelBoard::OnKibitzButton2)
    EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, PanelBoard::OnTabSelected)
//    EVT_ERASE_BACKGROUND(  PanelBoard::OnEraseBackground) */
END_EVENT_TABLE()


// Constructor
PanelBoard::PanelBoard
(
    wxWindow *parent, 
    wxWindowID id,
    const wxPoint &point,
    const wxSize &siz
)
    : wxPanel( parent, id, point, siz, wxNO_BORDER )
{
    int w = siz.x;
    int h = siz.y;
    int x = ((5*w)/100);
    int y = ((5*h)/100);
    w = ((90*w)/100);
    h = ((90*h)/100);
    gb = new GraphicBoardResizable( this,
                            wxID_ANY,
                            wxPoint(x,y), wxSize(w,h) );
}

void PanelBoard::OnSize( wxSizeEvent &evt )
{
    wxSize sz = evt.GetSize();
    cprintf( "Parent: resize x=%d, y=%d\n", sz.x, sz.y );
    int w = sz.x;
    int h = sz.y;
    int x = ((5*w)/100);
    int y = ((5*h)/100);
    gb->SetPosition(wxPoint(x,y));
    int w2 = ((90*w)/100);
    int h2 = ((90*h)/100);
    if( h2 > w2 )
        h2 = w2;
    else if( w2 > h2 )
        w2 = h2;
    wxSize sz2(w2,h2);
    gb->SetBoardSize(sz2);
    #if 0
    if( resize_ready )
    {
        wxSize sz = GetParent()->GetSize();
        if( sz.x < parent_sz_base.x )
            sz.x = parent_sz_base.x;
        objs.repository->nv.m_w = sz.x;
        objs.repository->nv.m_h = sz.y;
        //wxSize lb_sz = lb_sz_base;
        //lb_sz.x += (sz.x-parent_sz_base.x);
        //wxSize lb_sz_previous = lb->GetSize();
        //lb->SetSize(lb_sz);
        //TEMP wxSize notebook_sz = notebook->GetSize();
        //TEMP notebook_sz.x += (lb_sz.x-lb_sz_previous.x);
        //TEMP notebook->SetSize( notebook_sz );
        wxSize ksiz = k_sz_base;
        ksiz.x += (sz.x-parent_sz_base.x);
        kibitz_ctrl->SetSize(ksiz);
        kibitz_ctrl->SetColumnWidth(0,ksiz.x-18);
        wxPoint pos=kbut1_base;
        pos.x += (sz.x-parent_sz_base.x);
        kibitz_button1->SetPosition(pos);
        pos=kbut2_base;
        pos.x += (sz.x-parent_sz_base.x);
        kibitz_button2->SetPosition(pos);
   }
    #endif
}

#if 0
void PanelBoard::SetPlayers( const char *white, const char *black )
{
}

bool PanelBoard::GetNormalOrientation()
{
}

void PanelBoard::SetNormalOrientation( bool normal )
{
}

void PanelBoard::SetPosition( thc::ChessPosition &pos )
{
}

void PanelBoard::SetPosition()
{
}

void PanelBoard::ClocksVisible()
{
}

void PanelBoard::OnPaint (wxPaintEvent &)
{
    wxPaintDC dc(this);
    PrepareDC( dc );
}


void PanelBoard::RedrawClocks()
{
    white_clock->Refresh(false);
    white_clock->Update();
    black_clock->Refresh(false);
    black_clock->Update();
}

#endif
