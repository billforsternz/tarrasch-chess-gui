/****************************************************************************
 * Custom dialog - Player names
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PLAYER_DIALOG_H
#define PLAYER_DIALOG_H
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "SuspendEngine.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_PLAYER_DIALOG        = 10000,
    ID_PLAYER_RESET         = 10001,
    ID_HUMAN                = 10002,
    ID_COMPUTER             = 10003,
    ID_WHITE                = 10004,
    ID_BLACK                = 10005
};

// PlayerDialog class declaration
class PlayerDialog: public wxDialog
{    
    DECLARE_CLASS( PlayerDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    PlayerDialog( );
    PlayerDialog( wxWindow* parent,
      wxWindowID id = ID_PLAYER_DIALOG,
      const wxString& caption = wxT("Player names"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_PLAYER_DIALOG,
      const wxString& caption = wxT("Player names"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // PlayerDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // PlayerDialog member variables

    // Data members
    PlayerConfig   dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
};

#endif    // PLAYER_DIALOG_H
