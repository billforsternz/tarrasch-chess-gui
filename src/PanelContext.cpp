/****************************************************************************
 * Top level UI panel for providing context, kibitzer info etc.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/button.h"
#include "wx/sysopt.h"
#include "wx/notebook.h"
#include "Appdefs.h"
#include "CtrlBox.h"
#include "CtrlBox2.h"
#include "CtrlBoxBookMoves.h"
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
// Locate windows
//-----------------------------------------------------------------------------

enum
{
    wid_kibitz,
    wid_kibitz_button1,
    wid_kibitz_button2,
    NBR_OF_WIDS
};

#define XDELTA 0
#define YDELTA 0
static int window_id_positions[NBR_OF_WIDS][4] =
{
    {200+XDELTA,10+YDELTA,560,99},           // kibitz
    {700+XDELTA,70+YDELTA,60,20},           // kibitz_button1
    {700+XDELTA,100+YDELTA,60,20}           // kibitz_button2
};

static bool using_large_chess_board = false;
#define LOCATE_R( name, rec  )      rec.x = window_id_positions[ wid_ ## name ][0], rec.y = window_id_positions[ wid_ ## name ][1], rec.width = window_id_positions[ wid_ ## name ][2], rec.height  = window_id_positions[ wid_ ## name ][3]
#define LOCATE_P( name, pos )       pos.x = window_id_positions[ wid_ ## name ][0], pos.y = window_id_positions[ wid_ ## name ][1]
#define LOCATE_XY( name, x, y )     x     = window_id_positions[ wid_ ## name ][0], y = window_id_positions[ wid_ ## name ][1]
#define LOCATE_2( name, pos, siz )  pos.x = window_id_positions[ wid_ ## name ][0], pos.y = window_id_positions[ wid_ ## name ][1], siz.x = window_id_positions[ wid_ ## name ][2], siz.y = window_id_positions[ wid_ ## name ][3]
#define LOCATE_4( name, x, y, w, h) x     = window_id_positions[ wid_ ## name ][0], y = window_id_positions[ wid_ ## name ][1], w = window_id_positions[ wid_ ## name ][2], h  = window_id_positions[ wid_ ## name ][3]

//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PanelContext, wxPanel)
//    EVT_SIZE( PanelContext::OnSize )
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
    EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, PanelContext::OnTabSelected)
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


#define MARGIN 10
#define SMALL_MARGIN 4
#define FIELD 20

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

    // moving to wx3.1 we needed just 1 more pixel to avoid a weird erasure effect on the first tab header
    //  when creating subsequent tabs
    //wxSystemOptions::SetOption("msw.notebook.themed-background", 0);
#define NB_TEMP
#ifdef NB_TEMP
    wxPanel *notebook_page1 = new wxPanel(this, wxID_ANY );
#else
    notebook = new wxNotebook(this, wxID_ANY, pt2, sz2 );
    wxPanel *notebook_page1 = new wxPanel(notebook, wxID_ANY );
    //wxWindow *notebook_page1 = new wxWindow(notebook, wxID_ANY );
    notebook->AddPage(notebook_page1,"New Game",true);
#endif


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
    
//    font_clock = new wxFont( 14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,   false );

/*
    white_player = new wxStaticTextSub(this,ID_WHITE_PLAYER,"",pos,size,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
    dash_player  = new wxStaticTextSub(this,ID_DASH_PLAYER,"",pos,size,wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
    black_player = new wxStaticTextSub(this,ID_BLACK_PLAYER,"",pos,size,wxALIGN_LEFT|wxST_NO_AUTORESIZE);
  */

    // Create Status box
    box = new wxStaticBox( this,ID_BOX,"");
    box->SetLabel( "Suggestions" );
    box->SetPosition( wxPoint(10,10) );

    // Create buttons
    button1 = new wxButton( this, ID_BUTTON1, "New Game",       wxDefaultPosition, wxDefaultSize );
    button2 = new wxButton( this, ID_BUTTON2, "Setup Position", wxDefaultPosition, wxDefaultSize );
    button3 = new wxButton( this, ID_BUTTON3, "Play as White",  wxDefaultPosition, wxDefaultSize );
    button4 = new wxButton( this, ID_BUTTON4, "Play as Black",  wxDefaultPosition, wxDefaultSize );
/*    wxSize button_sz = button1->GetSize();
    button_sz.y -= 4;
    button1->SetSize( button_sz );
    button_sz = button2->GetSize();
    button_sz.y -= 4;
    button2->SetSize( button_sz );
    button_sz = button3->GetSize();
    button_sz.y -= 4;
    button3->SetSize( button_sz );
    button_sz = button4->GetSize();
    button_sz.y -= 4;
    button4->SetSize( button_sz ); */
    PositionButtons(false,false);
    wxSize sz1 = this->GetSize();
    wxSize sz2 = box->GetSize();
    // Create Kibitz box
    wxPoint kpos;
    wxSize  ksiz;
    kpos.x = 10 + sz2.x + 10;
    kpos.y = 10;
    ksiz.x = sz1.x - 2*kpos.x;
    ksiz.y = sz2.y;
/*  kibitz_box = new wxStaticBox( this,ID_KIBITZ_BOX,"");
    LOCATE_2( kibitz_box, kpos, ksiz );
    kibitz_box->SetPosition( kpos );
    kibitz_box->SetSize( ksiz );
    LOCATE_2( kibitz, kpos, ksiz );
    extern bool gbl_small_screen_detected;
    if( !gbl_small_screen_detected && ksiz.y==80 )
        ksiz.y=99;  // small board graphics on a big screen, show 4 kibitz lines not 3 */
    kibitz_ctrl = new wxListCtrl( this, ID_KIBITZ, kpos, ksiz, BORDER_COMMON|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_NO_HEADER );
    k_sz_base = ksiz;
    wxListItem col1;
    col1.SetColumn(0);
    kibitz_ctrl->InsertColumn(0,col1);
    kibitz_ctrl->SetColumnWidth(0,k_sz_base.x-18);
    wxListItem col2;
    col2.SetColumn(1);
    kibitz_ctrl->InsertColumn(1,col2);
    kibitz_ctrl->SetColumnWidth(1,4);
    kibitz_ctrl->InsertItem( 0, "", 0 );
//    kibitz_ctrl->SetItem( 0, 0, "As I played this I was convinced it must be game over (I was right) but I also had the uneasy feeling that I might have to find one more good move and that with less than 5 minutes left I might fail (I was right again!)" );
//    kibitz_ctrl->SetItem( 0, 1, "1111" );
    kibitz_ctrl->InsertItem( 1, "", 0 );
//    kibitz_ctrl->SetItem( 1, 0, "bbbb" );
//    kibitz_ctrl->SetItem( 1, 1, "2222" );
    kibitz_ctrl->InsertItem( 2, "", 0 );
//    kibitz_ctrl->SetItem( 2, 0, "cccc" );
//    kibitz_ctrl->SetItem( 2, 1, "3333" );
    kibitz_ctrl->InsertItem( 3, "", 0 );
//    kibitz_ctrl->SetItem( 3, 0, "dddd" );
//    kibitz_ctrl->SetItem( 3, 1, "4444" );
//    if( !gbl_small_screen_detected )
        kibitz_ctrl->InsertItem( 4, "", 0 );    // on a small screen show only 3
//    kibitz_ctrl->SetItem( 4, 0, "eeee" );
//    kibitz_ctrl->SetItem( 4, 1, "5555" );
    wxClientDC dc(kibitz_ctrl);
    wxCoord width, height, descent, external_leading;
    dc.GetTextExtent( "AbWy123456789", &width, &height, &descent, &external_leading );
    dbg_printf( "Kibitz box height=%d; font height=%d, descent=%d, external_leading=%d\n", ksiz.y, height, descent, external_leading );
    // results of this: Kibitz box height=99; font height=16, descent=3, external_leading=0
    //  Theory, min height of kibitz box to avoid scroll bars is 99 = (16+3)*5 + 4
    //  So if only 4 lines say, 80
    wxSize client_siz = parent->GetClientSize();
    dbg_printf( "Client size=%d,%d\n", client_siz.x, client_siz.y );       // 692,629         tl=10,80  br=697,704   delta=687,624
                                                                              // status box                  =20,564    delta=10,484

    // Is kibitz text same size as on dev system ?
    if( height==16 && descent==3 && external_leading==0 )
        ;   // yes, don't change anything

    // Else no, resize box to avoid scroll bars
    else
    {
        int y_needed = (height+descent+external_leading)*(true/*gbl_small_screen_detected*/?4:5) + 4;
        ksiz.y =  y_needed + 4;  // safety margin
        kibitz_ctrl->SetSize( ksiz );
        int calculated_y = kpos.y + ksiz.y + 4; // + 4 = safety margin
        int actual_y = client_siz.y;
        wxSize sz_ = parent->GetSize();
        sz_.y += (calculated_y-actual_y);
        parent->SetSize(sz_);
    }    

    // Create kibitz buttons
    LOCATE_2( kibitz_button1, kpos, ksiz );
    kbut1_base = kpos;
    kibitz_button1 = new wxButton( this, ID_KIBITZ_BUTTON1, "Capture one",  kpos, wxDefaultSize );
    LOCATE_2( kibitz_button2, kpos, ksiz );
    kbut2_base = kpos;
    kibitz_button2 = new wxButton( this, ID_KIBITZ_BUTTON2, "Capture all", kpos, wxDefaultSize );

    // Make them the same width
    wxSize button1_size = kibitz_button1->GetSize();    
    wxSize button2_size = kibitz_button2->GetSize();    
    if( button1_size.x > button2_size.x )
        button2_size.x = button1_size.x;
    else
        button1_size.x = button2_size.x;
    kibitz_button1->SetSize( button1_size);    
    kibitz_button2->SetSize( button2_size);    

/*  kibitz0 = new wxStaticText(this,ID_KIBITZ0,"",kpos,ksiz,wxALIGN_LEFT|wxST_NO_AUTORESIZE);
    kpos.y += ksiz.y;
    kibitz1 = new wxStaticText(this,ID_KIBITZ1,"",kpos,ksiz,wxALIGN_LEFT|wxST_NO_AUTORESIZE);
    kpos.y += ksiz.y;
    kibitz2 = new wxStaticText(this,ID_KIBITZ2,"",kpos,ksiz,wxALIGN_LEFT|wxST_NO_AUTORESIZE);
    kpos.y += ksiz.y;
    kibitz3 = new wxStaticText(this,ID_KIBITZ3,"",kpos,ksiz,wxALIGN_LEFT|wxST_NO_AUTORESIZE);
    kpos.y += ksiz.y;
    kibitz4 = new wxStaticText(this,ID_KIBITZ4,"",kpos,ksiz,wxALIGN_LEFT|wxST_NO_AUTORESIZE); */
    //kibitz1->SetFont( *font2 );
    //kibitz2->SetFont( *font2 );
    //kibitz3->SetFont( *font2 );
    //kibitz4->SetFont( *font2 );
    
    // Init game logic only after everything setup
    objs.session    = new Session;
    objs.log        = new Log;
    objs.book       = new Book;
    objs.cws        = new CentralWorkSaver;
    objs.tabs       = new Tabs;
    objs.gl         = NULL;
    GameLogic *gl   = new GameLogic( this, lb );

    // Hook up connections to GameLogic
    objs.gl         = gl;
    gl->gd.gl       = gl;
    gl->gd.gv.gl    = gl;
    lb->gl          = gl;
    objs.tabs->gl   = gl;
    GameDocument    blank;
    objs.tabs->TabNew( blank );
    objs.cws->Init( &objs.gl->undo, &objs.gl->gd, &objs.gl->gc_pgn, &objs.gl->gc_clipboard ); 
    SetPlayers( "", "" );
    PositionButtons();
//    objs.db         = new Database( objs.repository->database.m_file.c_str() );

    lb_sz_base           = lb->GetSize();
    parent_sz_base       = parent->GetSize();
    wxSize  parent_sz    = this->parent_sz_base;
    wxPoint parent_pos   = parent->GetPosition();
    resize_ready = true;
    if( objs.repository->nv.m_x>=0 &&
        objs.repository->nv.m_y>=0 &&
        objs.repository->nv.m_w>=0 &&
        objs.repository->nv.m_h>=0
      )
    {
        int disp_width, disp_height;
        wxDisplaySize(&disp_width, &disp_height);
        if( objs.repository->nv.m_x+objs.repository->nv.m_w <= disp_width &&
            objs.repository->nv.m_y+objs.repository->nv.m_h <= disp_height
          )
        {
            wxPoint pos_( objs.repository->nv.m_x, objs.repository->nv.m_y );
            wxSize  sz_ ( objs.repository->nv.m_w, objs.repository->nv.m_h );
            if( sz_.x < parent_sz_base.x )
                sz_.x = parent_sz_base.x;  // minimum startup width
            parent->SetPosition(parent_pos = pos_);
            parent->SetSize    (parent_sz  = sz_);
        }
    }
    objs.repository->nv.m_x = parent_pos.x;
    objs.repository->nv.m_y = parent_pos.y;
    objs.repository->nv.m_w = parent_sz.x;
    objs.repository->nv.m_h = parent_sz.y;
    //notebook->AdvanceSelection();
    //notebook->AdvanceSelection();
}

void PanelContext::SetMinimumPlaySize()
{
/*    wxSize      sz1;
    wxSize      sz2;
    wxSize      sz3;
    wxPoint     pos1;
    wxPoint     pos2;
    wxPoint     pos3;
    pos1 = GetParent()->GetScreenPosition();
    pos2 = GetScreenPosition();
    pos3 = box->GetScreenPosition();
    sz1 = GetParent()->GetSize();
    sz2 = GetSize();
    sz3 = box->GetSize();    */
/*  
    +------------------------+
    |          h1            |
    | +--------------------+ |
    | |                    | |
    | |                    | |
    | |                    | |
    | |       canvas       | |
    | |                    | |
    | |                    | |
    | |                    | |
    | +--------------------+ |
    |          h2            |
    +------------------------+

 */

/*    int h1 = pos2.y-pos1.y;
    int h2 = sz1.y-sz2.y-h1;
    int min_y = (pos3.y - pos1.y +  // to the top of the status box
                  + sz3.y           // + height of the status box
                  + 10              // + a little gap
                  + h2);            // + height of status bar
    if( min_y > sz1.y )
    {
        sz1.y = min_y;
        GetParent()->SetSize( sz1 );
    }      */
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

void PanelContext::OnSize( wxSizeEvent &WXUNUSED(event) )
{
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
void PanelContext::AdjustPosition( bool have_players )
{
    int i;
    wxRect r;
    wxButton *button;
    int shift = (have_players?0:FIELD);

    // Position box
    wxPoint pos;
    LOCATE_P( box, pos );
    pos.y -= shift;
    box->SetPosition( pos );

    // Position status
    LOCATE_P( status, pos );
    pos.y -= shift;
    status->SetPosition( pos );

    // Position buttons
    int x, y;
    LOCATE_XY( button, x, y );
    y -= shift;
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
        r = button->GetRect();
        button->SetSize(r);
    }

    LOCATE_P( white_player, pos );
    pos.y += shift;
    white_player->SetPosition( pos );
    white_player->Show( have_players );
    LOCATE_P( dash_player,  pos);
    pos.y += shift;
    dash_player->SetPosition( pos );
    dash_player->Show( have_players );
    LOCATE_P( black_player, pos );
    pos.y += shift;
    black_player->SetPosition( pos );
    black_player->Show( have_players );
}        
#endif

int PanelContext::PositionButtons( bool horiz, bool test_shown ) 
{
    int i;
    wxSize sz;
    wxButton *button;
    int x=0, y=0, h=0, w=0;
    int max_width_so_far=0;
    int max_height_so_far=0;
    wxPoint pos = box->GetPosition();
    x = pos.x + 5;
    y = pos.y + 20;
    w = 5;
    h = 5;
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
        if( !test_shown || button->IsShown() )
        {
            button->SetPosition(wxPoint(x,y));
            sz = button->GetSize();
            if( horiz )
            {
                w += (sz.x + 5);
                x += (sz.x + 5);
            }
            else
            {
                h += (sz.y + 5);
                y += (sz.y + 5);
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
        h = max_height_so_far+10;
    else
        w = max_width_so_far+10;
    sz.x = w;
    sz.y = h;
    box->SetSize(sz);
    return w;
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

void PanelContext::OnTabSelected( wxBookCtrlEvent& event )
{
    if( resize_ready )
        objs.gl->OnTabSelected(event.GetSelection());
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


