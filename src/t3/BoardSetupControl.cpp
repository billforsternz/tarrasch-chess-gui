/****************************************************************************
 * Control - A custom control, allow user to setup a chess position
 *  Included as one of the controls in the PositionDialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/image.h"
#include "wx/file.h"
#include "Appdefs.h"
#include "thc.h"
#include "DebugPrintf.h"
#include "Canvas.h"
#include "Objects.h"
#include "BoardSetup.h"
#include "BoardSetupControl.h"
#include "bitmaps/board_setup_bitmap.xpm"
#include "bitmaps/custom_cursors.xpm"
using namespace std;
using namespace thc;

//static wxBitmap static_chess_bmp=wxBitmap( board_setup_bitmap_xpm );

BoardSetupControl::BoardSetupControl
(
    bool position_setup,    // either position setup (true) or database search (false)
    bool support_lockdown,
    wxWindow* parent,
    wxWindowID     id, //= wxID_ANY,
    const wxPoint &point //= wxDefaultPosition
)  : wxControl( parent, id, point, wxDefaultSize, wxBORDER_NONE )
/*
#define wxDOUBLE_BORDER         wxBORDER_DOUBLE
#define wxSUNKEN_BORDER         wxBORDER_SUNKEN
#define wxRAISED_BORDER         wxBORDER_RAISED
#define wxBORDER                wxBORDER_SIMPLE
#define wxSIMPLE_BORDER         wxBORDER_SIMPLE
#define wxSTATIC_BORDER         wxBORDER_STATIC
#define wxNO_BORDER             wxBORDER_NONE */

{
    #define TIMER_ID 2000
    m_timer.SetOwner(this,TIMER_ID);
    state = UP_IDLE;
    cursor = 0;
    wait_cursor = 0;
    bs = NULL;
    movements = 0;
    long_clearing_click = false;
    this->support_lockdown = support_lockdown;
    this->position_setup = position_setup;
    memset( lockdown, 0, sizeof(lockdown) );
    wxClientDC dc(parent);
//    dc.SetFont(*wxNORMAL_FONT);
//    wxSize  size( max_w, n*max_h );
//    wxPoint pos = point;
//    SetPosition( pos );
//    SetSize    ( size );
    dbg_printf( "*BoardSetupControl::CaptureMouse*\n" );
    //CaptureMouse();

    // try to find the directory with our images
    #if 0
    wxString dir;
    if ( wxFile::Exists(wxT("./position_setup.bmp")) )
        dir = wxT("./");
    else if ( wxFile::Exists(wxT("../position_setup.bmp")) )
        dir = wxT("../");
    else
        wxLogWarning(wxT("Can't find image files in either '.' or '..'!"));

    wxImage image;
    if ( !image.LoadFile( dir + _T("position_setup.bmp"), wxBITMAP_TYPE_BMP ) )
        wxLogError(wxT("Can't load BMP image"));
    else
    {
        chess_bmp = wxBitmap( image );
        image.Destroy();
    #else
    if(true)
    {
        //chess_bmp = static_chess_bmp; //wxBitmap( board_setup_bitmap_xpm );
        chess_bmp = wxBitmap( board_setup_bitmap_xpm );
    #endif
        wxSize  size( chess_bmp.GetWidth(), chess_bmp.GetHeight() );
        SetSize( size );
        bool normal_orientation = objs.canvas->GetNormalOrientation();
        cprintf( "Normal orientation = %s\n", normal_orientation?"true":"false" );
        bs = new BoardSetup( &chess_bmp, this, 46, 11, normal_orientation );  // Note XBORDER, YBORDER now passed as parameters
        SetCustomCursor( normal_orientation?'P':'p' );
    }
}

BoardSetupControl::~BoardSetupControl()
{
    if( bs )
        delete bs;
    //ReleaseMouse();
}

void BoardSetupControl::SetState( const char *action, State new_state )
{
    bool timer_running = false;
    bool timer_running_before=false;
    bool timer_running_after =false;
    const char *before=NULL;
    const char *after=NULL;
    const char *s=NULL;
    State x = state;
    for( int i=0; i<2; i++ )
    {
        switch(x)
        {
            case UP_IDLE:               s="UP_IDLE";
                                        timer_running = false;      break;
            case DOWN_IDLE:             s="DOWN_IDLE";
                                        timer_running = false;      break;
            case UP_CURSOR_BOARD:       s="UP_CURSOR_BOARD";
                                        timer_running = false;      break;
            case UP_CURSOR_SIDE:        s="UP_CURSOR_SIDE";
                                        timer_running = false;      break;
            case UP_CURSOR_SIDE_WAIT:   s="UP_CURSOR_SIDE_WAIT";
                                        timer_running = true;       break;
            case DOWN_CURSOR_WAIT:      s="DOWN_CURSOR_WAIT";
                                        timer_running = true;       break;
            case DOWN_CURSOR_IDLE:      s="DOWN_CURSOR_IDLE";
                                        timer_running = false;      break;
            case DOWN_CURSOR_DRAG:      s="DOWN_CURSOR_DRAG";
                                        timer_running = false;      break;
        }
        x = new_state;
        if( i == 0 )
        {
            before = s;
            timer_running_before = timer_running;
        }
        else
        {
            after = s;
            timer_running_after = timer_running;
        }
    }
    dbg_printf( "Transition: %s: %s->%s\n", action, before, after );
    state = new_state;
    if( timer_running_before && !timer_running_after )
        m_timer.Stop();
}


// Later - learn how to do this by sending an event to parent instead
extern void PositionSetupVeryUglyTemporaryCallback();
extern void DatabaseSearchVeryUglyTemporaryCallback( int offset );
void BoardSetupControl::UpdateBoard()
{
    bs->Set(cp,lockdown);
    bs->Draw();
    if( position_setup )
        PositionSetupVeryUglyTemporaryCallback();
    else if( support_lockdown )
        DatabaseSearchVeryUglyTemporaryCallback(-1);
}

void BoardSetupControl::OnMouseMove( wxMouseEvent& event )
{
    movements++;
    wxPoint point = event.GetPosition();
    char piece='\0', file='\0', rank='\0';
    bool hit = bs->HitTest( point, piece, file, rank );
/*  if( !hit )
        dbg_printf( "no hit; x=%d, y=%d\n", point.x, point.y );
    else
    {
        dbg_printf( "hit; x=%d, y=%d -> piece=%c, file=%c, rank=%c\n",
                point.x, point.y,
                piece?piece:'?',
                file?file:'?',
                rank?rank:'?' );
    } */
    switch( state )
    {
        default:
            break;
        case UP_CURSOR_BOARD:
        case UP_IDLE:
        {
            if( hit && piece )
            {
                cursor = piece;
                SetCustomCursor( cursor );
                SetState( "move", UP_CURSOR_SIDE );
            }
            break;
        }
        case UP_CURSOR_SIDE:
        {
            if( hit && piece && cursor!=piece )
            {
                //cursor = piece;
                //SetCustomCursor( cursor );
                m_timer.Start( 200, true );
                wait_cursor = piece;
                SetState( "move", UP_CURSOR_SIDE_WAIT );
            }
            else if( hit && !piece )
                SetState( "move", UP_CURSOR_BOARD );
            break;
        }
        case UP_CURSOR_SIDE_WAIT:
        {
            if( hit && piece )
            {
                if( cursor == piece )
                    SetState( "undo wait", UP_CURSOR_SIDE );
                else if( wait_cursor != piece )
                {
                    wait_cursor = piece;
                    m_timer.Start( 200, true );
                    SetState( "change wait_cursor", UP_CURSOR_SIDE_WAIT );
                }
            }
            else if( hit && !piece )
                SetState( "move", UP_CURSOR_BOARD );
            break;
        }
    }
}

/* State machine notes:
3 main possibilities; click curs on 1)empty square or 2)different piece or 3)same piece as curs
1) curs + empty = curs
2) curs + piece = curs  (quick click)
   curs + piece = piece (slow click)
3) curs + curs  = empty (quick click)
   curs + curs  = curs  (slow click)
Slow click mechanism is;
a) After timeout, curs changes to curs(piece) (if different piece)
b) square = empty
c) square = curs on release (may be different square)
*/

void BoardSetupControl::OnTimeout( wxTimerEvent& WXUNUSED(event) )
{
    dbg_printf( "Timeout\n" );
    long_clearing_click = false;
    if( state == DOWN_CURSOR_WAIT )
    {
        SetState( "timeout", DOWN_CURSOR_DRAG );
        if( cp.squares[wait_offset] == cursor )
        {
            long_clearing_click = true;
            movements = 0;
        }
        else
        {
            cursor = cp.squares[wait_offset];
            SetCustomCursor( cursor );
        }
        cp.squares[wait_offset] = ' ';
        UpdateBoard();
    }
    else if( state == UP_CURSOR_SIDE_WAIT )
    {
        cursor = wait_cursor;
        SetCustomCursor( cursor );
        SetState( "timeout", UP_CURSOR_SIDE );
    }
}

void BoardSetupControl::OnMouseLeftUp( wxMouseEvent& event )
{
    dbg_printf( "Up\n" );
    wxPoint point = event.GetPosition();
    char piece='\0', file='\0', rank='\0';
    bool hit = bs->HitTest( point, piece, file, rank );
    if( state==UP_CURSOR_BOARD || state==UP_CURSOR_SIDE || state==UP_CURSOR_SIDE_WAIT )
        OnMouseLeftDown(event);  // This is very important. If you double click
                                 //  we want events as follows; down up down up
                                 //  but without this we get; down up up
    switch( state )
    {
        default:
        case UP_IDLE:
        case UP_CURSOR_BOARD:
        case UP_CURSOR_SIDE:
        case UP_CURSOR_SIDE_WAIT:
            break;
        case DOWN_IDLE:
        {
            SetState( "up", UP_IDLE );
            break;
        }
        case DOWN_CURSOR_IDLE:
        {
            SetState( "up", UP_CURSOR_BOARD );
            break;
        }

        // Quick click, released before timeout
        case DOWN_CURSOR_WAIT:
        {
            SetState( "up", UP_CURSOR_BOARD );
            if( hit && !piece )
            {
                int col = file-'a';     // file='a' -> col=0
                int row = 7-(rank-'1'); // rank='8' -> row=0
                int offset = row*8 + col;
                if( offset != wait_offset )
                {
                    // Quick drag
                    cp.squares[offset] = cursor;
                    cp.squares[wait_offset] = ' ';
                }
                else
                {
                    // Quick click
                    if( cursor == cp.squares[wait_offset] )
                    {
                        cp.squares[wait_offset] = ' ';
                        dbg_printf( "Quick click clear\n" );
                    }
                    else
                    {
                        cp.squares[wait_offset] = cursor;
                        dbg_printf( "Quick click set\n" );
                    }
                }
            }
            else
                cp.squares[wait_offset] = ' '; // clear waiting square
            UpdateBoard();
            dbg_printf( "Quick click end\n" );
            break;
        }
        case DOWN_CURSOR_DRAG:
        {
            SetState( "up", UP_CURSOR_BOARD );
            if( hit && !piece )
            {
                int col = file-'a';     // file='a' -> col=0
                int row = 7-(rank-'1'); // rank='8' -> row=0
                int offset = row*8 + col;
                if( long_clearing_click && offset==wait_offset && movements<5 )
                    ; // don't drop piece onto newly empty square if we haven't
                      //  moved the cursor after clearing it  
                else
                {
                    cp.squares[offset] = cursor;
                    UpdateBoard();
                }
            }
            break;
        }
    }
    long_clearing_click = false;
}

void BoardSetupControl::OnMouseLeftDown( wxMouseEvent& event )
{
    dbg_printf( "Down\n" );
    long_clearing_click = false;
    wxPoint point = event.GetPosition();
    char piece='\0', file='\0', rank='\0';
    bool hit = bs->HitTest( point, piece, file, rank );
    if( !hit )
        dbg_printf( "no hit; x=%d, y=%d\n", point.x, point.y );
    else
    {
        dbg_printf( "hit; x=%d, y=%d -> piece=%c, file=%c, rank=%c\n",
                point.x, point.y,
                piece?piece:'?',
                file?file:'?',
                rank?rank:'?' );
    }
    switch( state )
    {
        default:
        case DOWN_IDLE:
            break;
        case UP_IDLE:
        {
            SetState( "down", DOWN_IDLE );
            if( hit && piece )
            {
                cursor = piece;
                SetCustomCursor( cursor );
                SetState( "down", DOWN_CURSOR_IDLE );
            }
            break;
        }
        case DOWN_CURSOR_WAIT:
        case DOWN_CURSOR_IDLE:
        case DOWN_CURSOR_DRAG:
//      {
//          SetState( "down", DOWN_CURSOR_IDLE );
//          break;
//      }
        case UP_CURSOR_BOARD:
        case UP_CURSOR_SIDE:
        case UP_CURSOR_SIDE_WAIT:
        {
            SetState( "down", DOWN_CURSOR_IDLE );
            if( hit )
            {
                if( piece )
                {
                    cursor = piece;
                    SetCustomCursor( cursor );
                }
                else
                {
                    int col = file-'a';     // file='a' -> col=0
                    int row = 7-(rank-'1'); // rank='8' -> row=0
                    int offset = row*8 + col;
                    if( cp.squares[offset] == ' ' )
                    {
                        SetState( "down", DOWN_CURSOR_IDLE );
                        cp.squares[offset] = cursor;
                        UpdateBoard();
                    }
                    else
                    {
                        SetState( "down", DOWN_CURSOR_WAIT );
                        wait_offset = offset;
                        m_timer.Start( 200, true );
                    }
                }
            }
            break;
        }
    }
}


void BoardSetupControl::OnMouseRightDown( wxMouseEvent& event )
{
    wxPoint point = event.GetPosition();
    if( support_lockdown )
    {
        char piece='\0', file='\0', rank='\0';
        bool hit = bs->HitTest( point, piece, file, rank );
        if( hit )
        {
            cprintf( "hit; x=%d, y=%d -> piece=%c, file=%c, rank=%c\n",
                    point.x, point.y,
                    piece?piece:'?',
                    file?file:'?',
                    rank?rank:'?' );
            int col = file-'a';     // file='a' -> col=0
            int row = 7-(rank-'1'); // rank='8' -> row=0
            int offset = row*8 + col;
            lockdown[offset] = !lockdown[offset];
            cprintf( "Square %c%c is %s\n", file, rank, lockdown[offset]?"locked down":"not locked down" );
            UpdateBoard();
            DatabaseSearchVeryUglyTemporaryCallback( offset );
        }
    }
}


void BoardSetupControl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
//    wxSize size = GetClientSize();
//  dc.SetFont(*wxNORMAL_FONT);
//    dc.SetBrush(*wxWHITE_BRUSH);
//    dc.SetPen(*wxLIGHT_GREY_PEN);
//    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
//    dc.DrawRectangle( 0, y, size.x, y+height );
//    dc.DrawLine(0, y+height, size.x, y+height );
    if( chess_bmp.Ok() )
    {
        //bool dont_redraw = (captured&&suppress_redraw);
        //if( dont_redraw )
        //    dbg_printf( "OMG " );
        dc.DrawBitmap( chess_bmp, 0, 0, true );
    }
}

void BoardSetupControl::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

void BoardSetupControl::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    Refresh();
}
void BoardSetupControl::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    //event.Skip(true);
    //event.StopPropagation();
}

void BoardSetupControl::SetCustomCursor( char piece )
{
    const char * const *ptr;
    switch( piece )
    {
        default:
        case 'P':   ptr = white_pawn_xpm;    break;
        case 'N':   ptr = white_knight_xpm;  break;
        case 'B':   ptr = white_bishop_xpm;  break;
        case 'R':   ptr = white_rook_xpm;    break;
        case 'Q':   ptr = white_queen_xpm;   break;
        case 'K':   ptr = white_king_xpm;    break;
        case 'p':   ptr = black_pawn_xpm;    break;
        case 'n':   ptr = black_knight_xpm;  break;
        case 'b':   ptr = black_bishop_xpm;  break;
        case 'r':   ptr = black_rook_xpm;    break;
        case 'q':   ptr = black_queen_xpm;   break;
        case 'k':   ptr = black_king_xpm;    break;
    }
    wxImage image( ptr );
    image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X,17);
    image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y,17);
    wxCursor cursor(image);
    SetCursor( cursor );    
}

void BoardSetupControl::ClearCustomCursor()
{
    SetState( "ClearCustomCursor", UP_IDLE );
    this->cursor = 0;
    wxCursor cursor(wxCURSOR_ARROW);
    SetCursor( cursor );    
}


BEGIN_EVENT_TABLE(BoardSetupControl, wxControl)
    EVT_PAINT(BoardSetupControl::OnPaint)
    EVT_SIZE(BoardSetupControl::OnSize)
    EVT_MOUSE_CAPTURE_LOST(BoardSetupControl::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(BoardSetupControl::OnMouseEvent)
    EVT_LEFT_UP (BoardSetupControl::OnMouseLeftUp)
    EVT_LEFT_DOWN (BoardSetupControl::OnMouseLeftDown)
    EVT_RIGHT_DOWN (BoardSetupControl::OnMouseRightDown)
    EVT_MOTION (BoardSetupControl::OnMouseMove)
    EVT_ERASE_BACKGROUND(BoardSetupControl::OnEraseBackground)
    EVT_TIMER( TIMER_ID, BoardSetupControl::OnTimeout)
END_EVENT_TABLE()

