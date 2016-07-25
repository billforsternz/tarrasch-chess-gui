/****************************************************************************
 * Custom dialog - Game Details
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAME_DETAILS_DIALOG_H
#define GAME_DETAILS_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "MoveTree.h"   // for PGN_RESULT
#include "SuspendEngine.h"
#include "GameDocument.h"

// Control identifiers
enum
{
    ID_GAME_DETAILS_DIALOG  = 10000,
    ID_GAME_DETAILS_RESET   = 10001,
    ID_WHITE_PLAYER_NAME    = 10002,
    ID_BLACK_PLAYER_NAME    = 10003,
    ID_EVENT                = 10004,
    ID_SITE                 = 10005,
    ID_DATE                 = 10006,
    ID_ROUND                = 10007,
    ID_BOARD_NBR            = 10008,
    ID_RESULT               = 10009,
    ID_ECO                  = 10010,
    ID_WHITE_ELO            = 10011,
    ID_BLACK_ELO            = 10012
};

// GameDetailsDialog class declaration
class GameDetailsDialog: public wxDialog
{    
    DECLARE_CLASS( GameDetailsDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    GameDetailsDialog( );
    GameDetailsDialog( wxWindow* parent,
      wxWindowID id = ID_GAME_DETAILS_DIALOG,
      const wxString& caption = wxT("Game details"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Run dialog
    bool Run( GameDocument &gd );

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_GAME_DETAILS_DIALOG,
      const wxString& caption = wxT("Game details"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // GameDetailsDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    // GameDetailsDialog member variables
    wxTextCtrl* white_ctrl;
    wxTextCtrl* black_ctrl;
    wxTextCtrl* event_ctrl;
    wxTextCtrl* site_ctrl;
    wxTextCtrl* date_ctrl;
    wxTextCtrl* round_ctrl;
    wxTextCtrl* board_nbr_ctrl;
    wxComboBox* result_ctrl;
    wxTextCtrl* white_elo_ctrl;
    wxTextCtrl* eco_ctrl;
    wxTextCtrl* black_elo_ctrl;
    static wxString remember_event;
    static wxString remember_site;

    // Data members
    wxString    white;          // White
    wxString    black;          // Black
    wxString    event;          // Event
    wxString    site;           // Site
    wxString    date;           // Date
    wxString    round;          // Round
    wxString    board_nbr;      // Board
    wxString    result;         // Result
    wxString    eco;            // ECO code
    wxString    white_elo;      // WhiteElo
    wxString    black_elo;      // BlackElo
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
};

#endif    // GAME_DETAILS_DIALOG_H
