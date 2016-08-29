/****************************************************************************
 * Top level UI panel for providing chess board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PANEL_BOARD_H
#define PANEL_BOARD_H
#include "wx/wx.h"
#include "GraphicBoardResizable.h"

class PanelBoard: public wxPanel
{
public:
    PanelBoard( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    void OnSize( wxSizeEvent &evt );
    GraphicBoardResizable *gb;
/*    ~PanelBoard();
    void SetPlayers( const char *white, const char *black );
    bool GetNormalOrientation();
    void SetNormalOrientation( bool normal );
    void SetPosition( thc::ChessPosition &pos );
    void SetPosition();
    void AdjustPosition( bool have_players );
    void ClocksVisible();
    void WhiteClock( const wxString &txt );
    void BlackClock( const wxString &txt );
    void RedrawClocks();
    void OnMove(); */
    DECLARE_EVENT_TABLE()
};

#endif // PANEL_BOARD_H
