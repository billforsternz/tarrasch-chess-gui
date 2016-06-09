/****************************************************************************
 * Custom canvas that we can draw on
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/button.h"
#include "wx/notebook.h"
#include "Appdefs.h"
#include "CtrlBox.h"
#include "CtrlBox2.h"
#include "CtrlBoxBookMoves.h"
#include "CtrlBoxBookMoves.h"
#include "Canvas.h"
#include "GraphicBoard.h"
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
using namespace std;
using namespace thc;

//temp
bool view_flags_book_moves;

//-----------------------------------------------------------------------------
// Locate windows
//-----------------------------------------------------------------------------

enum
{
    wid_gb = 0,
    wid_who_top,
    wid_who_bottom,
    wid_white_player,
    wid_dash_player,
    wid_black_player,
    wid_box,
    wid_status,
    wid_book_moves,
    wid_wclock,
    wid_bclock,
    wid_moves,
    wid_button,
    wid_kibitz,
    wid_kibitz_button1,
    wid_kibitz_button2,
    NBR_OF_WIDS
};

#ifdef THC_WINDOWS
#define XDELTA 10
#define YDELTA 24
static int window_id_positions_54[NBR_OF_WIDS][4] =
{
    #if 0
    {32+XDELTA,11+YDELTA,435,435},         // gb
    {10,10+YDELTA,20,20},                  // who_top
    {10,422+YDELTA,20,20},                 // who_bottom
    {32+XDELTA,464+YDELTA,206,22},         // white_player
    {238+XDELTA,464+YDELTA,20,22},         // dash_player
    {258+XDELTA,464+YDELTA,206,22},        // black_player
    {10+XDELTA,488+YDELTA,674,37},         // box
    {20+XDELTA,498+YDELTA,286,22},         // status
    {10,425,116,19},                       // book_moves
    {512+XDELTA,480,68,28},                // wclock
    {584+XDELTA,480,68,28},                // bclock
    {472+XDELTA,11+YDELTA,202,434},        // moves          + 3
    {394+XDELTA,498+YDELTA,0,0},           // button
    {10+XDELTA,558,570,99},                // kibitz
    {590+XDELTA,580,60,20},                // kibitz_button1
    {590+XDELTA,608,60,20}                 // kibitz_button2

    #else
    {32+XDELTA,5+YDELTA,435,435},          // gb = graphic board
    {10,10+YDELTA,20,20},                  // who_top
    {10,402+YDELTA,20,20},                 // who_bottom
    {32+XDELTA,446+YDELTA,206,22},         // white_player
    {238+XDELTA,446+YDELTA,20,22},         // dash_player
    {258+XDELTA,446+YDELTA,206,22},        // black_player
    {10+XDELTA,468+YDELTA,674,37},         // box
    {20+XDELTA,478+YDELTA,286,22},         // status
    {10,425,116,19},                       // book_moves
    {512+XDELTA,460,68,28},                // wclock
    {584+XDELTA,460,68,28},                // bclock
    {472+XDELTA,5+YDELTA,202,434},         // moves          + 3
    {394+XDELTA,478+YDELTA,0,0},           // button
    {10+XDELTA,530,570,99},                // kibitz
    {590+XDELTA,550,60,20},                // kibitz_button1
    {590+XDELTA,580,60,20}                 // kibitz_button2
    #endif
};

static int window_id_positions_40[NBR_OF_WIDS][4] =
{
    {32,4,322,322},             // gb                       {32 /*151*/,11,432,432},        // gb
    {10,4,20,20},               // who_top                  {10 /*129*/,10,20,20},          // who_top
    {10,296,20,20},             // who_bottom               {10 /*129*/,422,20,20},         // who_bottom
    {32,334,206,22},            // white_player             {32 /*151*/,454,206,22},        // white_player
    {238,334,20,22},            // dash_player              {238 /*357*/,454,20,22},        // dash_player
    {258,334,206,22},           // black_player             {258 /*377*/,454,206,22},       // black_player
    {10,360,674,37},            // box                      {10,478,674/*574*/,37},         // box
    {20,370,286,22},            // status                   {20,488,286,22},                // status
    {10,313,116,19},            // book_moves               {10,425,116,19},                // book_moves
    {512,334,68,28},            // wclock                   {467+216/2-4-68,450,68,28},     // wclock
    {584,334,68,28},            // bclock                   {467+216/2+4,450,68,28},        // bclock
    {360,4, 320,322},           // moves                    {467 /*10*/,10,216/*116*/,434}, // moves          + 3
    {296,370,0,0},              // button                   {394,488,0,0},                  // button
    {10,402,578,80},            // kibitz               //  {10,518,578,99},                // kibitz
    {600,414,60,20},            // kibitz_button1           {600,540,60,20},                // kibitz_button1
    {600,442,60,20}             // kibitz_button2           {600,568,60,20}                 // kibitz_button2
};
#else
#define YDELTA_TOP 10
#define YDELTA 30
static int window_id_positions_54[NBR_OF_WIDS][4] =
{
    {32,21+YDELTA_TOP,432,432},                // gb
    {10,20+YDELTA_TOP,20,20},                  // who_top
    {10,422+YDELTA,20,20},                 // who_bottom
    {32,454+YDELTA,206,22},                // white_player
    {238,454+YDELTA,20,22},                // dash_player
    {258,454+YDELTA,206,22},               // black_player
    {10,478+YDELTA,674,37+10},                // box
    {20,488+YDELTA,286,22},                // status
    {10,425+YDELTA,116,19},                // book_moves
    {512,450+YDELTA,68,28},                // wclock
    {584,450+YDELTA,68,28},                // bclock
    {470,20+YDELTA_TOP,200,434},               // moves          + 3
    {394+10,488+YDELTA+8,0,0},                  // button
    {10,518+YDELTA,578,99},                // kibitz
    {600,540+YDELTA,60,20},                // kibitz_button1
    {600,568+YDELTA,60,20}                 // kibitz_button2
};

static int window_id_positions_40[NBR_OF_WIDS][4] =
{
    {32,4+YDELTA,322,322},             // gb                       {32 /*151*/,11,432,432},        // gb
    {10,4+YDELTA,20,20},               // who_top                  {10 /*129*/,10,20,20},          // who_top
    {10,296+YDELTA,20,20},             // who_bottom               {10 /*129*/,422,20,20},         // who_bottom
    {32,334+YDELTA,206,22},            // white_player             {32 /*151*/,454,206,22},        // white_player
    {238,334+YDELTA,20,22},            // dash_player              {238 /*357*/,454,20,22},        // dash_player
    {258,334+YDELTA,206,22},           // black_player             {258 /*377*/,454,206,22},       // black_player
    {10,360+YDELTA,674,37},            // box                      {10,478,674/*574*/,37},         // box
    {20,370+YDELTA,286,22},            // status                   {20,488,286,22},                // status
    {10,313+YDELTA,116,19},            // book_moves               {10,425,116,19},                // book_moves
    {512,334+YDELTA,68,28},            // wclock                   {467+216/2-4-68,450,68,28},     // wclock
    {584,334+YDELTA,68,28},            // bclock                   {467+216/2+4,450,68,28},        // bclock
    {360,4+YDELTA, 320,322},           // moves                    {467 /*10*/,10,216/*116*/,434}, // moves          + 3
    {296,370+YDELTA,0,0},              // button                   {394,488,0,0},                  // button
    {10,402+YDELTA,578,80},            // kibitz               //  {10,518,578,99},                // kibitz
    {600,414+YDELTA,60,20},            // kibitz_button1           {600,540,60,20},                // kibitz_button1
    {600,442+YDELTA,60,20}             // kibitz_button2           {600,568,60,20}                 // kibitz_button2
};

#endif

// 1st cut, create table
#if 0
#define LOCATE_R( name, rec  )      release_printf( "TAG{%s,%d,%d,%d,%d},\n", #name, (rec).x, (rec).y, (rec).width, (rec).height )
#define LOCATE_P( name, pos )       release_printf( "TAG{%s,%d,%d},\n", #name, (pos).x, (pos).y )
#define LOCATE_XY( name, x, y )     release_printf( "TAG{%s,%d,%d},\n", #name, x, y )
#define LOCATE_2( name, pos, siz )  release_printf( "TAG{%s,%d,%d,%d,%d},\n", #name, (pos).x, (pos).y, (siz).x, (siz).y )
#define LOCATE_4( name, x, y, w, h) release_printf( "TAG{%s,%d,%d,%d,%d},\n", #name, x, y, w, h )

#else
static bool using_large_chess_board = false;
#define window_id_positions (using_large_chess_board?window_id_positions_54:window_id_positions_40)
// 2nd cut, get value from table
#define LOCATE_R( name, rec  )      rec.x = window_id_positions[ wid_ ## name ][0], rec.y = window_id_positions[ wid_ ## name ][1], rec.width = window_id_positions[ wid_ ## name ][2], rec.height  = window_id_positions[ wid_ ## name ][3]
#define LOCATE_P( name, pos )       pos.x = window_id_positions[ wid_ ## name ][0], pos.y = window_id_positions[ wid_ ## name ][1]
#define LOCATE_XY( name, x, y )     x     = window_id_positions[ wid_ ## name ][0], y = window_id_positions[ wid_ ## name ][1]
#define LOCATE_2( name, pos, siz )  pos.x = window_id_positions[ wid_ ## name ][0], pos.y = window_id_positions[ wid_ ## name ][1], siz.x = window_id_positions[ wid_ ## name ][2], siz.y = window_id_positions[ wid_ ## name ][3]
#define LOCATE_4( name, x, y, w, h) x     = window_id_positions[ wid_ ## name ][0], y = window_id_positions[ wid_ ## name ][1], w = window_id_positions[ wid_ ## name ][2], h  = window_id_positions[ wid_ ## name ][3]
#endif

//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(Canvas, wxPanel)
    EVT_SIZE( Canvas::OnSize )
    EVT_CHAR( Canvas::OnChar )
    EVT_LEFT_DOWN (Canvas::OnMouseLeftDown)
    EVT_MOTION (Canvas::OnMouseMove)
    EVT_PAINT (Canvas::OnPaint)
    EVT_MOUSE_CAPTURE_LOST(Canvas::OnMouseCaptureLost)
    EVT_MOUSEWHEEL(Canvas::OnMouseWheel)
    EVT_BUTTON(ID_BUTTON1,Canvas::OnButton1)
    EVT_BUTTON(ID_BUTTON2,Canvas::OnButton2)
    EVT_BUTTON(ID_BUTTON3,Canvas::OnButton3)
    EVT_BUTTON(ID_BUTTON4,Canvas::OnButton4)
    EVT_BUTTON(ID_KIBITZ_BUTTON1,Canvas::OnKibitzButton1)
    EVT_BUTTON(ID_KIBITZ_BUTTON2,Canvas::OnKibitzButton2)
    EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, Canvas::OnTabSelected)
//    EVT_ERASE_BACKGROUND(  Canvas::OnEraseBackground)
END_EVENT_TABLE()

IMPLEMENT_CLASS( wxStaticTextSub, wxStaticText )
BEGIN_EVENT_TABLE( wxStaticTextSub, wxStaticText )
    EVT_LEFT_DOWN (wxStaticTextSub::OnMouseLeftDown)
END_EVENT_TABLE()


// Destructor
Canvas::~Canvas()
{
/* No need to delete child windows, they are deleted by parent
    if( gb )
    {
        delete gb;
        gb = NULL;
    }
    if( lb )
    {
        delete lb;
        lb = NULL;
    }
    if( who_top )
    {
        delete who_top;
        who_top = NULL;
    }
    if( who_bottom )
    {
        delete who_bottom;
        who_bottom = NULL;
    }
    if( white_clock )
    {
        delete white_clock;
        white_clock = NULL;
    }
    if( black_clock )
    {
        delete black_clock;
        black_clock = NULL;
    }  */
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
    if( font_clock )
    {
        delete font_clock;
        font_clock = NULL;
    }
    if( box )
    {
        delete box;
        box = NULL;
    }
    if( status )
    {
        delete status;
        status = NULL;
    }
}

#define MARGIN 10
#define SMALL_MARGIN 4
#define FIELD 20


// Constructor
Canvas::Canvas
(
    wxWindow *parent, 
    wxWindowID id,
    const wxPoint &point,
    const wxSize &siz
)
    : wxPanel( parent, id, point, siz, wxSUNKEN_BORDER )
{
    resize_ready = false;
    popup = NULL;
    lb = NULL;
    gb = NULL;
    who_top = NULL;
    who_bottom = NULL;
    who_top = NULL;
    font1 = NULL;
    font2 = NULL;
    font3 = NULL;
    font_book = NULL;
    font_clock = NULL;
    white_clock = NULL;
    black_clock = NULL;
    box = NULL;
    status = NULL;
    view_flags_book_moves = true;

    // Create board
    wxRect rect = parent->GetRect();
    using_large_chess_board = (rect.height >= 680);
    LOCATE_R( gb, board_rect );
    
#ifdef THC_MAC
    wxPoint pt;
    wxSize  sz;
    pt.x = board_rect.x;
    pt.y = board_rect.y;
    sz.x = board_rect.width;
    sz.y = board_rect.height;
    wxSize  sz2;
    wxPoint pt2;
    pt2 = pt;
    //pt2.x = 0;
    pt2.x -= 9;
    pt2.y -= 30;
    sz2 = sz;
    sz2.x += 23;
    sz2.y += 42;
    //sz2.x += 4;
    int x,y,h,w;
    LOCATE_4( moves, x, y, w, h );
    sz2.x += w;

    notebook = new wxNotebook(this, wxID_ANY, pt2, sz2 );
    wxPanel *notebook_page1 = new wxPanel(notebook, wxID_ANY );
    notebook->AddPage(notebook_page1,"New Game",true);
#else
    wxPoint pt;
    wxSize  sz;
    pt.x = board_rect.x;
    pt.y = board_rect.y;
    sz.x = board_rect.width;
    sz.y = board_rect.height;
    wxSize  sz2;
    wxPoint pt2;
    pt2 = pt;
    //pt2.x = 0;
    //pt2.x -= 9;
    pt2.y -= 28;
    sz2 = sz;
    sz2.x += 7;
    sz2.y = 28;
    //sz2.x += 4;
    int x,y,h,w;
    LOCATE_4( moves, x, y, w, h );
    sz2.x += w;

    notebook = new wxNotebook(this, wxID_ANY, pt2, sz2 );
    wxPanel *notebook_page1 = new wxPanel(notebook, wxID_ANY );
    notebook->AddPage(notebook_page1,"New Game",true);
#endif

    gb = new GraphicBoard( this,
                          wxID_ANY,
                          pt,
                          sz, using_large_chess_board?54:40 );

    // Create labels
    who_top    = new wxStaticText(this,ID_WHO_TOP,   " ");
    who_bottom = new wxStaticText(this,ID_WHO_BOTTOM," ");
    font1      = new wxFont( 14, wxROMAN,  wxITALIC, wxBOLD,   false );
    font2      = new wxFont( 14, wxSWISS,  wxNORMAL, wxNORMAL, false );
    font3      = new wxFont( 12, wxSWISS, wxNORMAL, wxNORMAL, false );
    wxPoint pos;
    wxSize  size;
    who_top->SetFont( *font1 );
    LOCATE_2( who_top, pos, size );
    who_top->SetPosition( pos );
    who_top->SetSize( size );
    LOCATE_2( who_bottom, pos, size );
    who_bottom->SetFont( *font1 );
    who_bottom->SetPosition( pos );
    who_bottom->SetSize( size );

    font_book = new wxFont( 10, wxROMAN, wxNORMAL, wxBOLD,   false );
    wxRect r;
    LOCATE_R( book_moves, r );
    pt.x = r.x;
    pt.y = r.y;
    sz.x = r.width;
    sz.y = r.height;
    // book_moves = new CtrlBoxBookMoves( this,
    //                      wxID_ANY,
    //                      pt,
    //                      sz );
    extern CtrlBoxBookMoves *gbl_book_moves;
    book_moves = gbl_book_moves;
    book_moves->SetBackgroundColour( *wxGREEN );
    book_moves->SetBackgroundStyle( wxBG_STYLE_COLOUR );
    book_moves->SetForegroundColour( *wxBLACK );
    book_moves->SetFont( *font_book );
    book_moves->SetTxt("no book moves",false);
    
    font_clock = new wxFont( 14, wxROMAN, wxNORMAL, wxBOLD,   false );
    LOCATE_R( wclock, r );
    pt.x = r.x;
    pt.y = r.y;
    sz.x = r.width;
    sz.y = r.height;
    white_clock = new CtrlBox( this,
                          wxID_ANY,
                          pt,
                          sz );
    white_clock->hack_its_a_clock = true;
    white_clock->SetBackgroundColour( *wxWHITE );
    white_clock->SetBackgroundStyle( wxBG_STYLE_COLOUR );
    white_clock->SetForegroundColour( *wxBLACK );
    white_clock->SetFont( *font_clock );
    white_clock->SetTxt("***",false);
    LOCATE_R( bclock, r );
    pt.x = r.x;
    pt.y = r.y;
    sz.x = r.width;
    sz.y = r.height;
    black_clock = new CtrlBox( this,
                          wxID_ANY,
                          pt,
                          sz );
    black_clock->hack_its_a_clock = true;
    black_clock->SetBackgroundColour( *wxWHITE );
    black_clock->SetBackgroundStyle( wxBG_STYLE_COLOUR );
    black_clock->SetForegroundColour( *wxBLACK );
    black_clock->SetFont( *font_clock );
    black_clock->SetTxt("***",false);

    LOCATE_2( white_player, pos, size );
    white_player = new wxStaticTextSub(this,ID_WHITE_PLAYER,"",pos,size,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
    LOCATE_2( dash_player, pos, size );
    dash_player  = new wxStaticTextSub(this,ID_DASH_PLAYER,"",pos,size,wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
    LOCATE_2( black_player, pos, size );
    black_player = new wxStaticTextSub(this,ID_BLACK_PLAYER,"",pos,size,wxALIGN_LEFT|wxST_NO_AUTORESIZE);
    white_player->SetFont( *font2 );

    dash_player->SetFont( *font2 );
    dash_player->SetLabel( "" );

    black_player->SetFont( *font2 );

    // Create moves listbox
    LOCATE_4( moves, x, y, w, h );
    lb = new CtrlChessTxt( this, ID_LIST_CTRL, wxPoint(x,y), wxSize(w,h) ); // BORDER_COMMON|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_NO_HEADER );

    //wxListItem col1;
    //col1.SetColumn(0);//Text("Nbr");
    //lb->InsertColumn(0,col1);
    //lb->SetColumnWidth(0,34); // w/4+6);  // 116/4 + 6 = 29+6 = 35
    //wxListItem col2;
    //col2.SetColumn(1);//Text("Move");
    //lb->InsertColumn(1,"Move");
    //lb->InsertColumn(1,col2);
    //lb->SetColumnWidth(1,60);//w - w/4 -4-24);  //-4 else get ugly horizontal scroll bar
                                               // -20 more else same prob once vert scroll bar appears
                               //   116 -29 - 28 = 87 -28 = 59
    // Create Status box
    box = new wxStaticBox( this,ID_BOX,"");
    LOCATE_2( box, pos, size );
    box->SetPosition( pos );
    box->SetSize( size );
    
    box_width = size.x;
    status  = new wxStaticText(this,ID_STATUS,    " ");
    status->SetFont( *font2 );
    LOCATE_2( status, pos, size );
    status->SetPosition( pos );
    status->SetSize( size );

    // Create buttons
    LOCATE_XY( button, x, y );
    button1 = new wxButton( this, ID_BUTTON1, "New Game",       wxPoint(x,y), wxDefaultSize );
    button2 = new wxButton( this, ID_BUTTON2, "Setup Position", wxPoint(x,y), wxDefaultSize );
    button3 = new wxButton( this, ID_BUTTON3, "Play as White",  wxPoint(x,y), wxDefaultSize );
    button4 = new wxButton( this, ID_BUTTON4, "Play as Black",  wxPoint(x,y), wxDefaultSize );

    // Create Kibitz box
    wxPoint kpos;
    wxSize  ksiz;
/*  kibitz_box = new wxStaticBox( this,ID_KIBITZ_BOX,"");
    LOCATE_2( kibitz_box, kpos, ksiz );
    kibitz_box->SetPosition( kpos );
    kibitz_box->SetSize( ksiz ); */
    LOCATE_2( kibitz, kpos, ksiz );
    extern bool gbl_small_screen_detected;
    if( !gbl_small_screen_detected && ksiz.y==80 )
        ksiz.y=99;  // small board graphics on a big screen, show 4 kibitz lines not 3
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
    if( !gbl_small_screen_detected )
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
        int y_needed = (height+descent+external_leading)*(gbl_small_screen_detected?4:5) + 4;
        ksiz.y =  y_needed + 4;  // safety margin
        kibitz_ctrl->SetSize( ksiz );
        int calculated_y = kpos.y + ksiz.y + 4; // + 4 = safety margin
        int actual_y = client_siz.y;
        wxSize sz = parent->GetSize();
        sz.y += (calculated_y-actual_y);
        parent->SetSize(sz);
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
    db_primitive_build_default_database( objs.repository->database.m_file.c_str() );    // immediately after book compile

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
            wxPoint pos( objs.repository->nv.m_x, objs.repository->nv.m_y );
            wxSize  sz ( objs.repository->nv.m_w, objs.repository->nv.m_h );
            if( sz.x < parent_sz_base.x )
                sz.x = parent_sz_base.x;  // minimum startup width
            parent->SetPosition(parent_pos = pos);
            parent->SetSize    (parent_sz  = sz);
        }
    }
    objs.repository->nv.m_x = parent_pos.x;
    objs.repository->nv.m_y = parent_pos.y;
    objs.repository->nv.m_w = parent_sz.x;
    objs.repository->nv.m_h = parent_sz.y;
    //notebook->AdvanceSelection();
    //notebook->AdvanceSelection();
}

void Canvas::SetMinimumPlaySize()
{
    wxSize      sz1;
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
    sz3 = box->GetSize();
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

    int h1 = pos2.y-pos1.y;
    int h2 = sz1.y-sz2.y-h1;
    int min_y = (pos3.y - pos1.y +  // to the top of the status box
                  + sz3.y           // + height of the status box
                  + 10              // + a little gap
                  + h2);            // + height of status bar
    if( min_y > sz1.y )
    {
        sz1.y = min_y;
        GetParent()->SetSize( sz1 );
    }
}

void Canvas::SetMinimumKibitzSize()
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

void Canvas::OnMove()
{
    if( objs.repository )
    {
        wxPoint pos = GetParent()->GetPosition();
        objs.repository->nv.m_x = pos.x;
        objs.repository->nv.m_y = pos.y;
    }
}

void Canvas::OnSize( wxSizeEvent &event )
{
    if( resize_ready )
    {
        wxSize sz = GetParent()->GetSize();
        if( sz.x < parent_sz_base.x )
            sz.x = parent_sz_base.x;
        objs.repository->nv.m_w = sz.x;
        objs.repository->nv.m_h = sz.y;
        wxSize lb_sz = lb_sz_base;
        lb_sz.x += (sz.x-parent_sz_base.x);
        wxSize lb_sz_previous = lb->GetSize();
        lb->SetSize(lb_sz);
        wxSize notebook_sz = notebook->GetSize();
        notebook_sz.x += (lb_sz.x-lb_sz_previous.x);
        notebook->SetSize( notebook_sz );
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
}

void Canvas::AdjustPosition( bool have_players )
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
        r.y = y;
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

void Canvas::PositionButtons()
{
    int i;
    wxRect r;
    wxButton *button;
    int x, y, w, h;
    LOCATE_4( box, x, y, w, h );
    x += box_width; // right border

    // Calculate space required
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
        x -= (r.width+MARGIN);
    }

    // Hidden buttons first
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
        if( !button->IsShown() )
        {
            r = button->GetRect();
            r.x = x;
            x += (r.width+MARGIN);
            button->SetSize(r);
        }
    }

    // Then visible buttons
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
            r = button->GetRect();
            r.x = x;
            x += (r.width+MARGIN);
            button->SetSize(r);
        }
    }
}

void Canvas::SetSmallBox( bool is_small )
{
    wxRect r;
    LOCATE_R( box, r );
#ifdef THC_WINDOWS
    box_width = is_small ? ((r.width*48)/100) : r.width;
#else
    box_width = is_small ? ((r.width*505)/1000) : r.width;
#endif
    r.width = box_width;
    box->SetSize(r);
    box->SetLabel( is_small ? "Start" : "" );
}

void Canvas::SetPlayers( const char *white, const char *black )
{
    white_player->SetLabel( white );
    black_player->SetLabel( black );
    bool have_players=true;
    //if( *white=='\0' && *black=='\0' )
    //    have_players = false;
    dash_player->SetLabel( have_players ? "-" : " " );
    AdjustPosition( have_players );
}

bool Canvas::GetNormalOrientation()
{
    bool normal = gb->GetNormalOrientation();
    return normal;
}

void Canvas::SetNormalOrientation( bool normal )
{
    gb->SetNormalOrientation( normal );
    SetPosition();
}

void Canvas::SetPosition( thc::ChessPosition &pos )
{
    memcpy( &save_position, &pos, sizeof(save_position) );
    SetPosition();
}

void Canvas::SetPosition()
{
    bool normal = gb->GetNormalOrientation();
    gb->SetPosition( save_position.squares );
    gb->Draw();
    if( normal )
    {
        who_top->SetLabel( !save_position.white ? "B" : "" );
        who_bottom->SetLabel( save_position.white ? "W" : "" );
    }
    else
    {
        who_top->SetLabel( save_position.white ? "W" : "" );
        who_bottom->SetLabel( !save_position.white ? "B" : "" );
    }
}

void Canvas::BookUpdate( bool suppress )
{
    vector<thc::Move> bmoves;
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
        dbg_printf( "Book moves updated: %s\n", txt.c_str() );
        book_moves->Show(true);
        book_moves->SetTxt(txt);
    }
}

void Canvas::ClocksVisible()
{
    white_clock->Show( objs.repository->clock.m_white_visible );
    black_clock->Show( objs.repository->clock.m_black_visible );        
}

void Canvas::OnPaint (wxPaintEvent &)
{
    wxPaintDC dc(this);
    PrepareDC( dc );
}

void Canvas::ButtonCmd( int cmd )
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

void Canvas::OnTabSelected( wxBookCtrlEvent& event )
{
    if( resize_ready )
        objs.gl->OnTabSelected(event.GetSelection());
}

void Canvas::OnButton1( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button1_cmd );
}

void Canvas::OnButton2( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button2_cmd );
}

void Canvas::OnButton3( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button3_cmd );
}

void Canvas::OnButton4( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button4_cmd );
}

void Canvas::OnKibitzButton1( wxCommandEvent& WXUNUSED(event) )
{
    objs.gl->CmdKibitzCaptureOne();
}

void Canvas::OnKibitzButton2( wxCommandEvent& WXUNUSED(event) )
{
    objs.gl->CmdKibitzCaptureAll();
}

void wxStaticTextSub::OnMouseLeftDown( wxMouseEvent &WXUNUSED(event) )
{
    objs.gl->CmdPlayers();
}

void Canvas::OnMouseWheel( wxMouseEvent &event )
{
    lb->GetEventHandler()->ProcessEvent(event);
}

void Canvas::OnChar( wxKeyEvent &event )
{
    lb->GetEventHandler()->ProcessEvent(event);
}

void Canvas::OnMouseLeftDown( wxMouseEvent &event )
{
    wxPoint point = event.GetPosition();
    wxRect test_clocks;
    wxRect test_players;
    test_clocks.x       = white_clock->r.x;
    test_clocks.width   = black_clock->r.x+black_clock->r.width;
    test_clocks.y       = white_clock->r.y;
    test_clocks.height  = white_clock->r.height;
    test_players.x      = board_rect.x;
    test_players.width  = board_rect.width;
    test_players.y      = test_clocks.y;
    test_players.height = test_clocks.height;

    if( test_clocks.x<=point.x && point.x<(test_clocks.x+test_clocks.width) &&
        test_clocks.y<=point.y && point.y<(test_clocks.y+test_clocks.height)
      )
    {
        objs.gl->CmdClocks();
    }
    else if( test_players.x<=point.x && point.x<(test_players.x+test_players.width) &&
        test_players.y<=point.y && point.y<(test_players.y+test_players.height)
      )
    {
        objs.gl->CmdPlayers();
    }
}

void Canvas::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    //ReleaseMouse();
    //event.Skip(true);
    //event.StopPropagation();
}

void Canvas::OnMouseMove( wxMouseEvent& WXUNUSED(event) )
{
}

void Canvas::WhiteClock( const wxString &txt )
{
    white_clock->SetTxt(txt);
}

void Canvas::BlackClock( const wxString &txt )
{
    black_clock->SetTxt(txt);
}


void Canvas::Kibitz( int idx, const wxString &txt )
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

void Canvas::KibitzScroll( const wxString &txt )
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

void Canvas::RedrawClocks()
{
    white_clock->Refresh(false);
    white_clock->Update();
    black_clock->Refresh(false);
    black_clock->Update();
}

