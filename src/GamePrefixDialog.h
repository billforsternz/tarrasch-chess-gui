/****************************************************************************
 * Custom dialog - Game Prefix
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAME_PREFIX_DIALOG_H
#define GAME_PREFIX_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "MoveTree.h"   // for PGN_RESULT
#include "SuspendEngine.h"
#include "GameDocument.h"

// Control identifiers
enum
{
    ID_GAME_PREFIX_DIALOG  = 10000,
    ID_GAME_PREFIX_RESET   = 10001,
    ID_GAME_PREFIX_TXT     = 10002
};

// GamePrefixDialog class declaration
class GamePrefixDialog: public wxDialog
{    
    DECLARE_CLASS( GamePrefixDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    GamePrefixDialog( );
    GamePrefixDialog( wxWindow* parent,
      wxWindowID id = ID_GAME_PREFIX_DIALOG,
      const wxString& caption = wxT("Game prefix"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Run dialog
    bool Run( GameDocument &gd );

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_GAME_PREFIX_DIALOG,
      const wxString& caption = wxT("Game prefix"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    // GamePrefixDialog member variables
    wxTextCtrl* prefix_ctrl;

    // Data members
    wxString        prefix_txt;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
};

#endif    // GAME_PREFIX_DIALOG_H
