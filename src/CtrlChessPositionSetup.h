/****************************************************************************
 * Control - A custom control, allow user to setup a chess position
 *  Included as one of the controls in the PositionDialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CTRL_CHESS_POSITION_SETUP_H
#define CTRL_CHESS_POSITION_SETUP_H
#include "wx/wx.h"
#include "ChessBoardBitmap.h"

class CtrlChessPositionSetup : public wxControl
{
public:
    CtrlChessPositionSetup(
                  bool position_setup,    // either position setup (true) or database search (false)
                  bool support_lockdown,
                  wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& point = wxDefaultPosition,
				  const wxSize& size = wxDefaultSize );
    void Set( const thc::ChessPosition &cp_, const bool *lockdown_=0 );
    thc::ChessPosition cp;
    bool         lockdown[64];
    void ClearCustomCursor();
    // Figure out whether a piece or square is pointed to
    bool HitTest( wxPoint &point, char &piece, char &file, char &rank );

private:
    wxTimer      m_timer;
    wxBitmap     chess_bmp;
	ChessBoardBitmap cbb;
    char         cursor;
    char         wait_cursor;
    int          wait_offset;
    unsigned int movements;
    bool         long_clearing_click;
    bool         support_lockdown;
    bool         position_setup;
    enum State
    {
        UP_IDLE,
        DOWN_IDLE,
        UP_CURSOR_BOARD,
        UP_CURSOR_SIDE,
        UP_CURSOR_SIDE_WAIT,
        DOWN_CURSOR_WAIT,
        DOWN_CURSOR_IDLE,
        DOWN_CURSOR_DRAG
    } state;
    void SetState( const char *action, State new_state );
    void UpdateBoard();
    void OnTimeout( wxTimerEvent& event );
    void OnMouseMove( wxMouseEvent& event );
    void OnMouseLeftUp( wxMouseEvent &event );
    void OnMouseLeftDown( wxMouseEvent &event );
    void OnMouseRightDown( wxMouseEvent &event );
    void OnPaint(wxPaintEvent& WXUNUSED(evt));
    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt));
    void OnSize(wxSizeEvent& WXUNUSED(evt));
    void OnMouseCaptureLost( wxMouseCaptureLostEvent &event );
    void SetCustomCursor( char piece );
private:
    DECLARE_EVENT_TABLE()
};

#endif // CTRL_CHESS_POSITION_SETUP_H
