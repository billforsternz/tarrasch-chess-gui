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
    font1      = new wxFont( 14, wxFONTFAMILY_ROMAN,  wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD,   false );
    font2      = new wxFont( 14, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
    font3      = new wxFont( 12, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
    font4      = new wxFont( 8, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
    who_top     = new wxStaticText(this,wxID_ANY,"B");
    who_top->SetFont( *font1 );
    who_bottom  = new wxStaticText(this,wxID_ANY,"W");
    who_bottom->SetFont( *font1 );
    name_top    = new wxStaticText(this,wxID_ANY,"Fischer, Robert");
    name_top->SetFont( *font2 );
    name_bottom = new wxStaticText(this,wxID_ANY,"Botvinnik, Mikhail");
    name_bottom->SetFont( *font2 );
    time_top    = new wxStaticText(this,wxID_ANY,"120:00");
    time_top->SetFont( *font2 );
    time_bottom = new wxStaticText(this,wxID_ANY,"120:00");
    time_bottom->SetFont( *font2 );
    coord1     = new wxStaticText(this,wxID_ANY,"1");
    coord1->SetFont( *font4 );
    coord2     = new wxStaticText(this,wxID_ANY,"2");
    coord2->SetFont( *font4 );
    coord3     = new wxStaticText(this,wxID_ANY,"3");
    coord3->SetFont( *font4 );
    coord4     = new wxStaticText(this,wxID_ANY,"4");
    coord4->SetFont( *font4 );
    coord5     = new wxStaticText(this,wxID_ANY,"5");
    coord5->SetFont( *font4 );
    coord6     = new wxStaticText(this,wxID_ANY,"6");
    coord6->SetFont( *font4 );
    coord7     = new wxStaticText(this,wxID_ANY,"7");
    coord7->SetFont( *font4 );
    coord8     = new wxStaticText(this,wxID_ANY,"8");
    coord8->SetFont( *font4 );
    coorda     = new wxStaticText(this,wxID_ANY,"a");
    coorda->SetFont( *font4 );
    coordb     = new wxStaticText(this,wxID_ANY,"b");
    coordb->SetFont( *font4 );
    coordc     = new wxStaticText(this,wxID_ANY,"c");
    coordc->SetFont( *font4 );
    coordd     = new wxStaticText(this,wxID_ANY,"d");
    coordd->SetFont( *font4 );
    coorde     = new wxStaticText(this,wxID_ANY,"e");
    coorde->SetFont( *font4 );
    coordf     = new wxStaticText(this,wxID_ANY,"f");
    coordf->SetFont( *font4 );
    coordg     = new wxStaticText(this,wxID_ANY,"g");
    coordg->SetFont( *font4 );
    coordh     = new wxStaticText(this,wxID_ANY,"h");
    coordh->SetFont( *font4 );

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
/*                                 |
                                   v
       +--------------------------------------------+
       |      Fischer, Robert            45:00      |
       |     +-------------------------------+      |
       |   8 |              ^      ^         |      |
       |     |              |      |         |  B   |
       |   7 |              |      |         |      |
       |     |              |      t         |      |
       |   6 |              |           r--->|      |<-
     ->|     |<----l        |                |      |
       |   5 |              |                |      |
       |     |              |                |      |
       |   4 |<------------dim-------------->|      |
       |     |              |                |      |
       |   3 |              |                |      |
       |     |              |        b       |      |
       |   2 |              |        |       |      |
       |     |              |        |       |  W   |
       |   1 |              v        v       |      |
       |     +-------------------------------+      |
       |       a   b   c   d   e   f   g   h        |
       |      Botvinnik, Mikhail         23:00      |
       +--------------------------------------------+
                                     ^
                                     |

       Entire window has dimensions w,h
       l+dim+r = w OR t+dim+b = h
       dim = w-l-r OR
           = h-t-b  whichever is smaller

*/
    wxSize siz = evt.GetSize();
    int w = siz.x;
    int h = siz.y;
    cprintf( "Parent: resize x=%d, y=%d\n", w, h );

    wxClientDC dc1(time_top);
    wxCoord txt_width, txt_height, txt_descent, txt_external_leading;
    dc1.GetTextExtent( "120:00", &txt_width, &txt_height, &txt_descent, &txt_external_leading );
    int t = 110*(txt_height + txt_descent)/100;
    int name_top_y_offset = 105*(txt_height + txt_descent)/100;
    int time_x_offset = txt_width;
    wxClientDC dc2(coorda);
    dc2.GetTextExtent( "w", &txt_width, &txt_height, &txt_descent, &txt_external_leading );
    int l = 110*(txt_width)/100;
    int coord1_x_offset = 105*(txt_width)/100;
    dc2.GetTextExtent( "g", &txt_width, &txt_height, &txt_descent, &txt_external_leading );
    int b1 = 110*(txt_height + txt_descent)/100;
    int coorda_y_offset = 5*(txt_height + txt_descent)/100;
    int name_bottom_y_offset = 110*(txt_height + txt_descent)/100;
    wxClientDC dc3(name_bottom);
    dc3.GetTextExtent( "g", &txt_width, &txt_height, &txt_descent, &txt_external_leading );
    int b2 = 110*(txt_height + txt_descent)/100;
    int b =  b1+b2;

    wxClientDC dc4(who_bottom);
    dc4.GetTextExtent( "W", &txt_width, &txt_height, &txt_descent, &txt_external_leading );
    int r = 110*(txt_width)/100;
    int who_x_offset = 5*(txt_width)/100;

    // decide whether we have excess space above+below or left+right
    int dim;
    if( w-l-r < h-t-b )
    {   // excess space above+below
        dim = w-l-r;
        int diff = (h-t-b) - (w-l-r);
        t += (diff/3);  // about a third of the way down
        b += (diff - diff/3);
    }
    else
    {   // excess space left+right
        dim = h-t-b;
        int diff = (w-l-r) - (h-t-b);
        l += (diff/2);
        r += (diff/2);
    }

    // Position of the board
    int origin_x = l;
    int origin_y = t;
    int x = origin_x;
    int y = origin_y;
    gb->SetPosition(wxPoint(x,y));
    wxSize sz(dim,dim);
    gb->SetBoardSize(sz);

    // Name and time, top
    x = origin_x;
    y = origin_y - name_top_y_offset;
    name_top->SetPosition( wxPoint(x,y) );
    sz = name_top->GetSize();
    sz.x = dim-time_x_offset;
    name_top->SetSize(sz);
    name_bottom->SetSize(sz);
    x = origin_x + dim - time_x_offset;
    time_top->SetPosition(wxPoint(x,y));
    sz = time_top->GetSize();
    sz.x = time_x_offset;
    time_top->SetSize(sz);
    time_bottom->SetSize(sz);

    // Coords 1-8
    x = origin_x - coord1_x_offset;
    y = origin_y + dim/16;          // 1/16,3/16,5,16,7/16,9/16,11/16,13/16,15/16
    coord8->SetPosition(wxPoint(x,y));
    y = origin_y + (3*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coord7->SetPosition(wxPoint(x,y));
    y = origin_y + (5*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coord6->SetPosition(wxPoint(x,y));
    y = origin_y + (7*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coord5->SetPosition(wxPoint(x,y));
    y = origin_y + (9*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coord4->SetPosition(wxPoint(x,y));
    y = origin_y + (11*dim)/16;     // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coord3->SetPosition(wxPoint(x,y));
    y = origin_y + (13*dim)/16;     // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coord2->SetPosition(wxPoint(x,y));
    y = origin_y + (15*dim)/16;     // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coord1->SetPosition(wxPoint(x,y));

    // Who top and bottom
    x = origin_x + dim + who_x_offset;
    y = origin_y + (2*dim)/16;
    who_top->SetPosition(wxPoint(x,y));
    y = origin_y + (14*dim)/16;
    who_bottom->SetPosition(wxPoint(x,y));

    // Coords a-h
    y = origin_y + dim + coorda_y_offset;     
    x = origin_x + dim/16;          // 1/16,3/16,5,16,7/16,9/16,11/16,13/16,15/16
    coorda->SetPosition(wxPoint(x,y));
    x = origin_x + (3*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coordb->SetPosition(wxPoint(x,y));
    x = origin_x + (5*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coordc->SetPosition(wxPoint(x,y));
    x = origin_x + (7*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coordd->SetPosition(wxPoint(x,y));
    x = origin_x + (9*dim)/16;      // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coorde->SetPosition(wxPoint(x,y));
    x = origin_x + (11*dim)/16;     // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coordf->SetPosition(wxPoint(x,y));
    x = origin_x + (13*dim)/16;     // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coordg->SetPosition(wxPoint(x,y));
    x = origin_x + (15*dim)/16;     // 1/16,3/16,5/16,7/16,9/16,11/16,13/16,15/16
    coordh->SetPosition(wxPoint(x,y));
    
    // Name and time, bottom
    x = origin_x;
    y = origin_y + dim + name_bottom_y_offset;
    name_bottom->SetPosition(wxPoint(x,y));
    x = origin_x + dim - time_x_offset;
    time_bottom->SetPosition(wxPoint(x,y));
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
