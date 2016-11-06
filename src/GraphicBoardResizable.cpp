/****************************************************************************
 * Control - The graphic chess board that is the centre-piece of the GUI
 *  Still working on making it a more-self contained and reusable control
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/rawbmp.h"
#include "wx/file.h"
#include "Portability.h"
#include "DebugPrintf.h"
#include "CompressedBitmaps.h"
#include "GraphicBoardResizable.h"
#include "GameLogic.h"
#include "thc.h"
#include "Objects.h"
#include "Repository.h"

// Initialise the graphic board
GraphicBoardResizable::GraphicBoardResizable
(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& point,
    const wxSize& size
)   : wxControl( parent, id, point, size, wxBORDER_NONE ) //wxBORDER_SIMPLE ) //wxBORDER_NONE /*BORDER_COMMON*/ )
            // for the moment we get a strange artifact effect on resize sometimes unless wxBORDER_NONE
{
    strcpy( cbb._position_ascii, "rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR" );
	cbb.current_size = size;
    int min = size.x<size.y ? size.x : size.y;
    int pix_ = min/8;
    Init( pix_ );
}

void GraphicBoardResizable::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if( cbb.my_chess_bmp.Ok() )
    {
        dc.DrawBitmap( cbb.my_chess_bmp, 0, 0, true );// board_rect.x, board_rect.y, true );
    }
}

// Setup a position	on the graphic board
void GraphicBoardResizable::Init( int pix_ )
{
	cbb.Init( pix_ );
}

// Setup a position	on the graphic board
void GraphicBoardResizable::SetChessPosition( char *position_ascii )
{
	cbb.SetChessPosition( position_ascii, cbb.normal_orientation );
}

// Draw the graphic board
void GraphicBoardResizable::Draw()
{
    Refresh(false);
    Update();
}

// Figure out which square is clicked on the board
void GraphicBoardResizable::HitTest( wxPoint hit, char &file, char &rank )
{
	unsigned long row = ( hit.y / (cbb.height/8) );
	unsigned long col = ( (hit.x*cbb.density) / (cbb.width_bytes/8) );
    //dbg_printf( "Hit test: x=%ld y=%ld\n", hit.x, hit.y );
	if( cbb.normal_orientation )
	{
		rank = '1'+ (int)(7-row);
		file = 'a'+ (int)col;
	}
	else
	{
		file = 'a'+ (int)(7-col);
		rank = '1'+ (int)row;
	}
}

// Figure out which square is at shift offset from this square
void GraphicBoardResizable::HitTestEx( char &file, char &rank, wxPoint shift )
{
	long row, y, col, x;
	if( cbb.normal_orientation )
	{
		row = 7 - (rank-'1');
		col = file - 'a';
	}
	else
	{
		row = rank-'1';
		col = 7 - (file-'a');
	}
    y = row*(cbb.height/8);
	x = (col*(cbb.width_bytes/8)) / cbb.density;
    //dbg_printf( "Ex, shift:     x=%ld y=%ld\n", shift.x, shift.y );
    //dbg_printf( "Ex, src raw:   x=%ld y=%ld\n", x, y );
    y += cbb.height/16;
	x += (cbb.width_bytes/16) / cbb.density;
    //dbg_printf( "Ex  src+half:  x=%ld y=%ld\n", x, y );
    y += shift.y;
	x += shift.x;
    //dbg_printf( "Ex  src+shift: x=%ld y=%ld\n", x, y );
    if( y < (int)(cbb.height/16) )
        y = cbb.height/16;
    else if( y > (int)((cbb.height/16) + 7*(cbb.height/8)) )
        y = ((cbb.height/16) + 7*(cbb.height/8));
	if( x < (int)((cbb.width_bytes/16)/cbb.density) )
	    x = (int)((cbb.width_bytes/16)/cbb.density);
	else if( x > (int)(( cbb.width_bytes/16 + 7*(cbb.width_bytes/8)) / cbb.density ) )
	    x = (int)( (cbb.width_bytes/16 + 7*(cbb.width_bytes/8)) / cbb.density );
    //dbg_printf( "Ex  src+shift constrained: x=%ld y=%ld\n", x, y );
    wxPoint adjusted;
    adjusted.x = x;
    adjusted.y = y;
    HitTest( adjusted, file, rank );
    //dbg_printf( "Ex result: file=%c rank=%c\n", file, rank );
}
/*
// Put a shifted, masked piece from box onto board
void GraphicBoardResizable::PutEx( char piece,	char dst_file, char dst_rank, wxPoint shift )
{
	cbb.PutEx( piece, dst_file, dst_rank, shift );
}
  */
void GraphicBoardResizable::OnMouseLeftDown( wxMouseEvent &event )
{
    wxPoint point = event.GetPosition();
    wxPoint click = point;
    wxPoint pos = GetPosition();
    click.x += pos.x;
    click.y += pos.y;
    char file, rank;
    HitTest( point, file, rank );
    if( 'a'<=file && file<='h' && '1'<=rank && rank<='8' )
    {
        cbb.pickup_file  = file;
        cbb.pickup_rank  = rank;
        cbb.pickup_point = point;
        dbg_printf( "TODO check file=%c rank=%c\n", file, rank );
        if( objs.gl->MouseDown(file,rank,click) )
        {
            cbb.pickup_file  = file;
            cbb.pickup_rank  = rank;
            cbb.pickup_point = point;
            cbb.sliding = true;
            cbb.slide_pos = objs.gl->gd.master_position;
            OnMouseMove(event);     // Draw the position in case peeking allowed
        }
    }
}

void GraphicBoardResizable::OnMouseMove( wxMouseEvent &event )
{
    wxPoint point = event.GetPosition();
    if( cbb.sliding )
    {
        dbg_printf( "In board\n" );
        point.x -= cbb.pickup_point.x;
        point.y -= cbb.pickup_point.y;
        bool blank_other_squares = objs.gl->ShowSlidingPieceOnly();
        cbb.SetPositionEx( cbb.slide_pos, blank_other_squares, cbb.pickup_file, cbb.pickup_rank, point );
        Draw();
    }
}

void GraphicBoardResizable::OnMouseLeftUp( wxMouseEvent &event )
{
    cbb.sliding = false;
    wxPoint point = event.GetPosition();
    wxPoint click = point;
    wxPoint pos = GetPosition();
    click.x += pos.x;
    click.y += pos.y;
    point.x -= cbb.pickup_point.x;
    point.y -= cbb.pickup_point.y;
	char file=cbb.pickup_file;
    char rank=cbb.pickup_rank;
  	HitTestEx( file, rank, point );
    if( 'a'<=file && file<='h' && '1'<=rank && rank<='8' )
    {
        dbg_printf( "UP file=%c rank=%c\n", file, rank );
        objs.gl->MouseUp(file,rank,click);
    }
    else
    {
        dbg_printf( "UP out of range\n" );
        objs.gl->MouseUp();
    }
}


void GraphicBoardResizable::OnMouseCaptureLost( wxMouseCaptureLostEvent& WXUNUSED(event) )
{
    //event.Skip(true);
    //event.StopPropagation();
}



void GraphicBoardResizable::SetBoardSize( wxSize &size )
{
    cprintf( "Child: resize x=%d, y=%d\n", size.x, size.y );
    char temp[ sizeof(cbb._position_ascii) + 1 ];
    strcpy( temp, cbb._position_ascii );
    SetSize(size);
	cbb.current_size = size;
    int min = size.x<size.y ? size.x : size.y;
    int pix_ = min/8;
    Init( pix_ );
    SetChessPosition( temp );
    Refresh();
    Update();
}


BEGIN_EVENT_TABLE(GraphicBoardResizable, wxControl)
    EVT_PAINT(GraphicBoardResizable::OnPaint)
    EVT_MOUSE_CAPTURE_LOST(GraphicBoardResizable::OnMouseCaptureLost)
//    EVT_MOUSE_EVENTS(GraphicBoardResizable::OnMouseEvent)
    EVT_LEFT_UP (GraphicBoardResizable::OnMouseLeftUp)
    EVT_LEFT_DOWN (GraphicBoardResizable::OnMouseLeftDown)
    EVT_MOTION (GraphicBoardResizable::OnMouseMove)
//    EVT_ERASE_BACKGROUND(GraphicBoardResizable::OnEraseBackground)
//    EVT_TIMER( TIMER_ID, GraphicBoardResizable::OnTimeout)
END_EVENT_TABLE()


