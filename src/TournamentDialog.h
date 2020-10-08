/****************************************************************************
 * Custom dialog - Tournament round for PGN Skeleton
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef TOURNAMENT_DIALOG_H
#define TOURNAMENT_DIALOG_H
#include <map>
#include <string>
#include "wx/statline.h"
#include "MoveTree.h"   // for PGN_RESULT
#include "SuspendEngine.h"
#include "DialogDetect.h"
#include "GameDocument.h"
#include "GamesCache.h"

// Control identifiers
enum
{
    ID_TOURNAMENT_DIALOG    = 10000,
    ID_TOURNAMENT_RESET     = 10001,
    ID_TOURNAMENT_EVENT     = 10002,
    ID_TOURNAMENT_SITE      = 10003,
    ID_TOURNAMENT_DATE      = 10004,
    ID_TOURNAMENT_ROUND     = 10005,
    ID_TOURNAMENT_PAIRINGS  = 10006
};

// TournamentDialog class declaration
class TournamentDialog: public wxDialog
{    
    DECLARE_CLASS( TournamentDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    TournamentDialog( );
    TournamentDialog( wxWindow* parent,
      wxWindowID id = ID_TOURNAMENT_DIALOG,
      const wxString& caption = wxT("Create Skeleton Games for a Tournament Round"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Run dialog
    bool Run( CompactGame &proto, std::vector<CompactGame> &skeleton_games );

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_TOURNAMENT_DIALOG,
      const wxString& caption = wxT("Create Skeleton Games for a Tournament Round"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // TournamentDialog event handler declarations

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    // TournamentDialog member variables
    wxTextCtrl* event_ctrl;
    wxTextCtrl* site_ctrl;
    wxTextCtrl* date_ctrl;
    wxTextCtrl* round_ctrl;
    wxTextCtrl* pairings_ctrl;
    static wxString remember_event;
    static wxString remember_site;

    // Data members
    wxString    event;          // Event
    wxString    site;           // Site
    wxString    date;           // Date
    wxString    round;          // Round
    wxString    pairings_txt;   // White player rating Black player rating
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
	DialogDetect    detect;		// similarly the presence of this var allows tracking of open dialogs
};

#endif    // TOURNAMENT_DIALOG_H
