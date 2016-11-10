/****************************************************************************
 * Control - A custom control, allow user to setup a chess position
 *  Included as one of the controls in the PositionDialog
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
#include "PanelContext.h"
#include "Objects.h"
#include "ChessBoardBitmap.h"
#include "CtrlChessPositionSetup.h"

CtrlChessPositionSetup::CtrlChessPositionSetup
(
    bool position_setup,    // either position setup (true) or database search (false)
    bool support_lockdown,
    wxWindow* parent,
    wxWindowID     id, //= wxID_ANY,
    const wxPoint &point, //= wxDefaultPosition
    const wxSize  &size   //= wxDefaultSize
)  : wxControl( parent, id, point, size, wxBORDER_NONE )
{
    #define TIMER_ID 2000
    m_timer.SetOwner(this,TIMER_ID);
    state = UP_IDLE;
    cursor = 0;
    wait_cursor = 0;
    movements = 0;
    long_clearing_click = false;
    this->support_lockdown = support_lockdown;
    this->position_setup = position_setup;
    memset( lockdown, 0, sizeof(lockdown) );
    wxClientDC dc(parent);
    bool normal_orientation = objs.canvas->GetNormalOrientation();
	wxSize sz_bmp(400,320); //(364,294);
	cbb.CreateAsBoardSetup( sz_bmp, normal_orientation, cp );
    SetSize( sz_bmp );
    SetCustomCursor( normal_orientation?'P':'p' );
	state = UP_CURSOR_SIDE;
}

void CtrlChessPositionSetup::SetState( const char *action, State new_state )
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
    cprintf( "Transition: %s: %s->%s\n", action, before, after );
    state = new_state;
    if( timer_running_before && !timer_running_after )
        m_timer.Stop();
}


// Later - learn how to do this by sending an event to parent instead
extern void PositionSetupVeryUglyTemporaryCallback();
extern void DatabaseSearchVeryUglyTemporaryCallback( int offset );
void CtrlChessPositionSetup::UpdateBoard()
{
	cbb.SetChessPosition(cp,lockdown);
    Refresh(false);
    Update();
    if( position_setup )
        PositionSetupVeryUglyTemporaryCallback();
    else if( support_lockdown )
        DatabaseSearchVeryUglyTemporaryCallback(-1);
}

void CtrlChessPositionSetup::Set( const thc::ChessPosition &cp_, const bool *lockdown_ )
{
    this->cp = cp_;
    if( lockdown_ )
        memcpy(this->lockdown,lockdown_,64);
	cbb.SetChessPosition(cp,lockdown);
    Refresh(false);
    Update();
}

void CtrlChessPositionSetup::OnMouseMove( wxMouseEvent& event )
{
    movements++;
    wxPoint point = event.GetPosition();
    char piece='\0', file='\0', rank='\0';
    bool hit = HitTest( point, piece, file, rank );
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

void CtrlChessPositionSetup::OnTimeout( wxTimerEvent& WXUNUSED(event) )
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

void CtrlChessPositionSetup::OnMouseLeftUp( wxMouseEvent& event )
{
    dbg_printf( "Up\n" );
    wxPoint point = event.GetPosition();
    char piece='\0', file='\0', rank='\0';
    bool hit = HitTest( point, piece, file, rank );
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

void CtrlChessPositionSetup::OnMouseLeftDown( wxMouseEvent& event )
{
    dbg_printf( "Down\n" );
    long_clearing_click = false;
    wxPoint point = event.GetPosition();
    char piece='\0', file='\0', rank='\0';
    bool hit = HitTest( point, piece, file, rank );
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


void CtrlChessPositionSetup::OnMouseRightDown( wxMouseEvent& event )
{
    wxPoint point = event.GetPosition();
    if( support_lockdown )
    {
        char piece='\0', file='\0', rank='\0';
        bool hit = HitTest( point, piece, file, rank );
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

void CtrlChessPositionSetup::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if( cbb.GetBoardSetupBmp()->Ok() )
    {
        dc.DrawBitmap( *cbb.GetBoardSetupBmp(), 0, 0, true );
    }
}

void CtrlChessPositionSetup::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

void CtrlChessPositionSetup::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    Refresh();
}

void CtrlChessPositionSetup::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    //event.Skip(true);
    //event.StopPropagation();
}

void CtrlChessPositionSetup::SetCustomCursor( char piece )
{
    wxImage img;
	cbb.GetCustomCursor( piece, img );
    img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X,cbb.dim_pix/2);
    img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y,cbb.dim_pix/2);
    wxCursor temp(img);
    SetCursor( temp );    
}

void CtrlChessPositionSetup::ClearCustomCursor()
{
    SetState( "ClearCustomCursor", UP_IDLE );
    this->cursor = 0;
    wxCursor temp(wxCURSOR_ARROW);
    SetCursor( temp );    
}

// Figure out whether a piece or square is pointed to
bool CtrlChessPositionSetup::HitTest( const wxPoint &point, char &piece, char &file, char &rank )
{
	bool hit = cbb.BoardSetupHitTest( point, piece, file, rank );
	return hit;
}


BEGIN_EVENT_TABLE(CtrlChessPositionSetup, wxControl)
    EVT_PAINT(CtrlChessPositionSetup::OnPaint)
    EVT_SIZE(CtrlChessPositionSetup::OnSize)
    EVT_MOUSE_CAPTURE_LOST(CtrlChessPositionSetup::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(CtrlChessPositionSetup::OnMouseEvent)
    EVT_LEFT_UP (CtrlChessPositionSetup::OnMouseLeftUp)
    EVT_LEFT_DOWN (CtrlChessPositionSetup::OnMouseLeftDown)
    EVT_RIGHT_DOWN (CtrlChessPositionSetup::OnMouseRightDown)
    EVT_MOTION (CtrlChessPositionSetup::OnMouseMove)
    EVT_ERASE_BACKGROUND(CtrlChessPositionSetup::OnEraseBackground)
    EVT_TIMER( TIMER_ID, CtrlChessPositionSetup::OnTimeout)
END_EVENT_TABLE()

