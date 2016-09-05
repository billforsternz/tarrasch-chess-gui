/****************************************************************************
 * Top level UI panel for providing context, kibitzer info etc.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/button.h"
#include "wx/sysopt.h"
#include "Appdefs.h"
#include "CtrlBox.h"
#include "CtrlBox2.h"
#include "CtrlBoxBookMoves.h"
#include "GraphicBoardResizable.h"
#include "PanelContext.h"
#include "GameLogic.h"
#include "CentralWorkSaver.h"
#include "thc.h"
#include "Rybka.h"
#include "DebugPrintf.h"
#include "Log.h"
#include "Session.h"
#include "DbPrimitives.h"
#include "Database.h"
#include "Book.h"
#include "Tabs.h"
#include "Repository.h"
#include "Objects.h"

//temp
bool view_flags_book_moves;

//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PanelContext, wxPanel)
    EVT_SIZE( PanelContext::OnSize )
    EVT_CHAR( PanelContext::OnChar )
    EVT_LEFT_DOWN (PanelContext::OnMouseLeftDown)
    EVT_MOTION (PanelContext::OnMouseMove)
    EVT_PAINT (PanelContext::OnPaint)
    EVT_MOUSE_CAPTURE_LOST(PanelContext::OnMouseCaptureLost)
    EVT_MOUSEWHEEL(PanelContext::OnMouseWheel)
    EVT_BUTTON(ID_BUTTON1,PanelContext::OnButton1)
    EVT_BUTTON(ID_BUTTON2,PanelContext::OnButton2)
    EVT_BUTTON(ID_BUTTON3,PanelContext::OnButton3)
    EVT_BUTTON(ID_BUTTON4,PanelContext::OnButton4)
    EVT_BUTTON(ID_KIBITZ_BUTTON1,PanelContext::OnKibitzButton1)
    EVT_BUTTON(ID_KIBITZ_BUTTON2,PanelContext::OnKibitzButton2)
//    EVT_ERASE_BACKGROUND(  PanelContext::OnEraseBackground)
END_EVENT_TABLE()

IMPLEMENT_CLASS( wxStaticTextSub, wxStaticText )
BEGIN_EVENT_TABLE( wxStaticTextSub, wxStaticText )
    EVT_LEFT_DOWN (wxStaticTextSub::OnMouseLeftDown)
END_EVENT_TABLE()


// Destructor
PanelContext::~PanelContext()
{
    if( font1 )
    {
        delete font1;
        font1 = NULL;
    }
    if( font2 )
    {
        delete font2;
        font2 = NULL;
    }
    if( font3 )
    {
        delete font3;
        font3 = NULL;
    }
    if( font_book )
    {
        delete font_book;
        font_book = NULL;
    }
    if( box )
    {
        delete box;
        box = NULL;
    }
}

// Constructor
PanelContext::PanelContext
(
    wxWindow *parent, 
    wxWindowID id,
    const wxPoint &point,
    const wxSize &siz,
    PanelBoard *pb_,
    CtrlChessTxt *lb_
)
    : wxPanel( parent, id, point, siz, wxNO_BORDER )
{
    resize_ready = false;
    popup = NULL;
    pb = pb_;
    lb = lb_;
    font1 = NULL;
    font2 = NULL;
    font3 = NULL;
    font_book = NULL;
    box = NULL;
    view_flags_book_moves = true;
    parent_sz_base  = parent->GetSize();

    // Create labels
    font1      = new wxFont( 14, wxFONTFAMILY_ROMAN,  wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD,   false );
    font2      = new wxFont( 14, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
    font3      = new wxFont( 12, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
    wxPoint pos;
    wxSize  size;

    font_book = new wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,   false );
    extern CtrlBoxBookMoves *gbl_book_moves;
    book_moves = gbl_book_moves;
    book_moves->SetBackgroundColour( *wxGREEN );
    book_moves->SetBackgroundStyle( wxBG_STYLE_COLOUR );
    book_moves->SetForegroundColour( *wxBLACK );
    book_moves->SetFont( *font_book );
    book_moves->SetTxt("no book moves",false);
    
    // Create Status box
    box = new wxStaticBox( this,ID_BOX,"",wxDefaultPosition, wxDefaultSize );
    box_horiz = true;   // normally horizontal, except when kibitz shown
    box->SetLabel( "Suggestions" );

    // Create flexible buttons
    button1 = new wxButton( this, ID_BUTTON1, "New Game",       wxDefaultPosition, wxDefaultSize );
    button2 = new wxButton( this, ID_BUTTON2, "Setup Position", wxDefaultPosition, wxDefaultSize );
    button3 = new wxButton( this, ID_BUTTON3, "Play as White",  wxDefaultPosition, wxDefaultSize );
    button4 = new wxButton( this, ID_BUTTON4, "Play as Black",  wxDefaultPosition, wxDefaultSize );

    // Create Kibitz box
    kibitz_ctrl = new wxListCtrl( this, ID_KIBITZ, wxDefaultPosition, wxDefaultSize , BORDER_COMMON|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_NO_HEADER );
    wxListItem col1;
    col1.SetColumn(0);
    kibitz_ctrl->InsertColumn(0,col1);
    wxListItem col2;
    col2.SetColumn(1);
    kibitz_ctrl->InsertColumn(1,col2);
    kibitz_ctrl->InsertItem( 0, "", 0 );
    kibitz_ctrl->InsertItem( 1, "", 0 );
    kibitz_ctrl->InsertItem( 2, "", 0 );
    kibitz_ctrl->InsertItem( 3, "", 0 );
    kibitz_ctrl->InsertItem( 4, "", 0 );
    wxClientDC dc(kibitz_ctrl);
    wxCoord width, height, descent, external_leading;
    dc.GetTextExtent( "AbWy123456789", &width, &height, &descent, &external_leading );
    dbg_printf( "Kibitz box font height=%d, descent=%d, external_leading=%d\n", height, descent, external_leading );

    // Create kibitz buttons
    kibitz_button1 = new wxButton( this, ID_KIBITZ_BUTTON1, "Capture one",  wxDefaultPosition, wxDefaultSize );
    kibitz_button2 = new wxButton( this, ID_KIBITZ_BUTTON2, "Capture all",  wxDefaultPosition, wxDefaultSize );

    // Fit everything to size
    Layout( siz );
}


void PanelContext::SetMinimumKibitzSize()
{
    wxSize sz;
    sz = GetParent()->GetSize();
    int min_y = parent_sz_base.y;
    if( min_y > sz.y )
    {
        sz.y = min_y;
        GetParent()->SetSize( sz );
    }
}

void PanelContext::OnMove()
{
    if( objs.repository )
    {
        wxPoint pos = GetParent()->GetPosition();
        objs.repository->nv.m_x = pos.x;
        objs.repository->nv.m_y = pos.y;
    }
}

void PanelContext::OnSize( wxSizeEvent &evt )
{
    cprintf( "OnSize()\n" );
    if( resize_ready )
    {
        wxSize siz = evt.GetSize();
        Layout( siz );
    }
}

void PanelContext::Layout( wxSize const &siz )
{
    // Position suggestion box and buttons
    int box_width_if_vertical = PositionButtons(box_horiz);

    // Size Kibitz box
    wxPoint kpos;
    wxSize  ksiz;
    kpos.x = 10 + box_width_if_vertical + 10;
    kpos.y = 10;
    ksiz.x = siz.x - 2*kpos.x;
    ksiz.y = 99; //sz2.y;
    kibitz_ctrl->SetPosition(kpos);
    kibitz_ctrl->SetSize(ksiz);
    kibitz_ctrl->SetColumnWidth(0,ksiz.x-18);
    kibitz_ctrl->SetColumnWidth(1,4);

    // Position kibitz buttons
    kpos.x += ksiz.x;
    kpos.x += 10;
    wxSize button_sz = kibitz_button1->GetSize();
    kpos.y += (ksiz.y - button_sz.y*2 - 10)/2;

    kibitz_button1->SetPosition(kpos);
    kpos.y += button_sz.y;
    kpos.y += 10;
    kibitz_button2->SetPosition(kpos);

    // Make them the same width
    wxSize button1_size = kibitz_button1->GetSize();    
    wxSize button2_size = kibitz_button2->GetSize();    
    if( button1_size.x > button2_size.x )
        button2_size.x = button1_size.x;
    else
        button1_size.x = button2_size.x;
    kibitz_button1->SetSize( button1_size);    
    kibitz_button2->SetSize( button2_size);    
}

int PanelContext::PositionButtons( bool horiz ) 
{
    box_horiz = horiz;
    int i;
    wxSize sz;
    wxButton *button;
    int x=0, y=0, h=0, w=0;
    int max_width_so_far=0;
    int max_height_so_far=0;
    #define SPACER 5
    wxPoint pos(SPACER*2,SPACER*2);
    box->SetPosition( pos );
    x = pos.x + SPACER;
    y = pos.y + SPACER*4;
    w = SPACER;
    h = SPACER*4;
    for( i=0; i<4; i++ )
    {
        switch(i)
        {
            default:
            case 0: button = button1;  break;
            case 1: button = button2;  break;
            case 2: button = button3;  break;
            case 3: button = button4;  break;
        }
        if( button->IsShown() )
        {
            button->SetPosition(wxPoint(x,y));
            sz = button->GetSize();
            if( horiz )
            {
                w += (sz.x + SPACER);
                x += (sz.x + SPACER);
            }
            else
            {
                h += (sz.y + SPACER);
                y += (sz.y + SPACER);
            }
            if( sz.x > max_width_so_far )
                max_width_so_far = sz.x;
            if( sz.y > max_height_so_far )
                max_height_so_far = sz.y;
        }
    }
    for( i=0; !horiz && i<4; i++ )
    {
        switch(i)
        {
            default:
            case 0: button = button1;  break;
            case 1: button = button2;  break;
            case 2: button = button3;  break;
            case 3: button = button4;  break;
        }
        if( button->IsShown() )
        {
            sz = button->GetSize();
            sz.x = max_width_so_far;
            button->SetSize(sz);
        }
    }
    if( horiz )
        h = max_height_so_far+SPACER*2;
    else
        w = max_width_so_far+SPACER*2;
    sz.x = w;
    sz.y = h;
    box->SetSize(sz);
    return max_width_so_far+SPACER*2;   // returns width if rendered vertical
}

void PanelContext::BookUpdate( bool suppress )
{
    std::vector<thc::Move> bmoves;
    bool have_book_moves=false;
    if( objs.gl )
        have_book_moves = objs.book->Lookup( objs.gl->gd.master_position, bmoves );
    else // during startup
    {
        thc::ChessPosition temp;
        have_book_moves = objs.book->Lookup( temp, bmoves );
    }
    if( suppress )
        have_book_moves = false;
    view_flags_book_moves = have_book_moves;
    if( !view_flags_book_moves )
        book_moves->Show(false);
    else
    {
        wxString txt;
        int n = bmoves.size();
        if( n == 1 )
            txt = "1 book move";
        else
            txt.Printf( "%d book moves", n );
        const char *s = txt.c_str();
        cprintf( "Book moves updated: %s\n", s );
        book_moves->Show(true);
        book_moves->SetTxt(txt);
    }
}

void PanelContext::OnPaint (wxPaintEvent &)
{
    wxPaintDC dc(this);
    PrepareDC( dc );
}

void PanelContext::ButtonCmd( int cmd )
{
    switch( cmd )
    {
        case ID_CMD_FLIP:           objs.gl->CmdFlip();         break;
        case ID_CMD_SET_POSITION:   objs.gl->CmdSetPosition();  break;
        case ID_CMD_NEW_GAME:       objs.gl->CmdNewGame();      break;
        case ID_CMD_TAKEBACK:       objs.gl->CmdTakeback();     break;
        case ID_CMD_MOVENOW:        objs.gl->CmdMoveNow();      break;
        case ID_CMD_WHITE_RESIGNS:  objs.gl->CmdWhiteResigns(); break;
        case ID_CMD_BLACK_RESIGNS:  objs.gl->CmdBlackResigns(); break;
        case ID_CMD_DRAW:           objs.gl->CmdDraw();         break;
        case ID_CMD_PLAY_WHITE:     objs.gl->CmdPlayWhite();    break;
        case ID_CMD_PLAY_BLACK:     objs.gl->CmdPlayBlack();    break;
        case ID_CMD_SWAP_SIDES:     objs.gl->CmdSwapSides();    break;    
        case ID_CMD_NEXT_GAME:      objs.gl->CmdNextGame();     break;
        case ID_CMD_PREVIOUS_GAME:  objs.gl->CmdPreviousGame(); break;
        case ID_FILE_TAB_NEW:        objs.gl->CmdTabNew();       break;
        case ID_FILE_TAB_CLOSE:      objs.gl->CmdTabClose();     break;
        case ID_FILE_TAB_INCLUDE:    objs.gl->CmdTabInclude();   break;
        case wxID_NEW:              objs.gl->CmdFileNew();      break;
        case wxID_OPEN:             objs.gl->CmdFileOpen();     break;
        case wxID_SAVE:             objs.gl->CmdFileSave();     break;
        case wxID_UNDO:             objs.gl->CmdEditUndo();     break;
        case wxID_REDO:             objs.gl->CmdEditRedo();     break;
    }
}

void PanelContext::OnButton1( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button1_cmd );
}

void PanelContext::OnButton2( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button2_cmd );
}

void PanelContext::OnButton3( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button3_cmd );
}

void PanelContext::OnButton4( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button4_cmd );
}

void PanelContext::OnKibitzButton1( wxCommandEvent& WXUNUSED(event) )
{
    objs.gl->CmdKibitzCaptureOne();
}

void PanelContext::OnKibitzButton2( wxCommandEvent& WXUNUSED(event) )
{
    objs.gl->CmdKibitzCaptureAll();
}

void wxStaticTextSub::OnMouseLeftDown( wxMouseEvent &WXUNUSED(event) )
{
    objs.gl->CmdPlayers();
}

void PanelContext::OnMouseWheel( wxMouseEvent &event )
{
    lb->GetEventHandler()->ProcessEvent(event);
}

void PanelContext::OnChar( wxKeyEvent &event )
{
    lb->GetEventHandler()->ProcessEvent(event);
}

void PanelContext::OnMouseLeftDown( wxMouseEvent &WXUNUSED(event) )
{
}

void PanelContext::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    //ReleaseMouse();
    //event.Skip(true);
    //event.StopPropagation();
}

void PanelContext::OnMouseMove( wxMouseEvent& WXUNUSED(event) )
{
}

void PanelContext::Kibitz( int idx, const wxString &txt )
{
    switch( idx )
    {
        default:
        case 0:
        {
            if( str_kibitz0 != txt )
            {
                str_kibitz0 = txt;
                kibitz_ctrl->SetItem( 0, 0, txt );
            }
            break;
        }
        case 1:
        {
            if( str_kibitz1 != txt )
            {
                str_kibitz1 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz1 = %s\n", temp );
                //kibitz1->SetLabel(txt);
                kibitz_ctrl->SetItem( 1, 0, txt );
            }
            break;
        }
        case 2:
        {
            if( str_kibitz2 != txt )
            {
                str_kibitz2 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz2 = %s\n", temp );
                //kibitz2->SetLabel(txt);
                kibitz_ctrl->SetItem( 2, 0, txt );
            }
            break;
        }
        case 3:
        {
            if( str_kibitz3 != txt )
            {
                str_kibitz3 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz3 = %s\n", temp );
                //kibitz3->SetLabel(txt);
                kibitz_ctrl->SetItem( 3, 0, txt );
            }
            break;
        }
        case 4:
        {
            if( str_kibitz4 != txt )
            {
                str_kibitz4 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz4 = %s\n", temp );
                //kibitz4->SetLabel(txt);
                extern bool gbl_small_screen_detected;
                if( !gbl_small_screen_detected )      // on a small screen show only 3
                    kibitz_ctrl->SetItem( 4, 0, txt );
            }
            break;
        }
    }
}

void PanelContext::KibitzScroll( const wxString &txt )
{
    wxString line4 = str_kibitz3;
    wxString line3 = str_kibitz2;
    wxString line2 = str_kibitz1;
    wxString line1 = txt;
    Kibitz( 1, line1 );
    Kibitz( 2, line2 );
    Kibitz( 3, line3 );
    Kibitz( 4, line4 );
}


