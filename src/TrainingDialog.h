/****************************************************************************
 * Custom dialog - Training
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef TRAINING_DIALOG_H
#define TRAINING_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "SuspendEngine.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_TRAINING_DIALOG              = 10000,
    ID_TRAINING_RESET               = 10001,
    ID_NBR_HALF_MOVES_BEHIND        = 10002,
    ID_PEEK_AT_COMPLETE_POSITION    = 10003,
    ID_BLINDFOLD_HIDE_WHITE_PAWNS   = 10004,
    ID_BLINDFOLD_HIDE_WHITE_PIECES  = 10005,
    ID_BLINDFOLD_HIDE_BLACK_PAWNS   = 10006,
    ID_BLINDFOLD_HIDE_BLACK_PIECES  = 10007
//  ID_BLINDFOLD_HIDE_BOARD         = 10008
};

// TrainingDialog class declaration
class TrainingDialog: public wxDialog
{    
    DECLARE_CLASS( TrainingDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    TrainingDialog( );
    TrainingDialog( wxWindow* parent,
      wxWindowID id = ID_TRAINING_DIALOG,
      const wxString& caption = wxT("Training settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_TRAINING_DIALOG,
      const wxString& caption = wxT("Training settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // TrainingDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // TrainingDialog member variables

    // Data members
    TrainingConfig   dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
};

#endif    // TRAINING_DIALOG_H
