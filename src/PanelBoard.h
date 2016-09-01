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
    ~PanelBoard();
    void SetPlayers( const char *white, const char *black );
    bool GetNormalOrientation();
    void SetNormalOrientation( bool normal );
    void SetChessPosition( thc::ChessPosition &pos );
    void SetChessPosition();
    void ClocksVisible();                  
    void WhiteClock( const wxString &txt );
    void BlackClock( const wxString &txt );
    void RedrawClocks();                   
    void SetBoardTitle( const char *txt, bool highlight=false );

private:
    wxFont       *font1;
    wxFont       *font2;
    wxFont       *font3;
    wxFont       *font4;
    void OnSize( wxSizeEvent &evt );
    GraphicBoardResizable *gb;
    wxStaticText    *who_top;
    wxStaticText    *who_bottom;
    wxStaticText    *board_title;
    wxStaticText    *name_top;
    wxStaticText    *name_bottom;
    wxStaticText    *time_top;
    wxStaticText    *time_bottom;
    wxStaticText    *coord1;
    wxStaticText    *coord2;
    wxStaticText    *coord3;
    wxStaticText    *coord4;
    wxStaticText    *coord5;
    wxStaticText    *coord6;
    wxStaticText    *coord7;
    wxStaticText    *coord8;
    wxStaticText    *coorda;
    wxStaticText    *coordb;
    wxStaticText    *coordc;
    wxStaticText    *coordd;
    wxStaticText    *coorde;
    wxStaticText    *coordf;
    wxStaticText    *coordg;
    wxStaticText    *coordh;
    thc::ChessPosition   save_position;
    std::string  white_player;
    std::string  black_player;
    std::string  white_clock_txt;
    std::string  black_clock_txt;
    std::string  time_top_txt;
    std::string  time_bottom_txt;
    bool         white_clock_visible;
    bool         black_clock_visible;
    DECLARE_EVENT_TABLE()
};

#endif // PANEL_BOARD_H
