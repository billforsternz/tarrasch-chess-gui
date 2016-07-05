/****************************************************************************
 * Custom dialog - Clocks
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CLOCK_DIALOG_H
#define CLOCK_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "SuspendEngine.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_CLOCK_DIALOG         = 10000,
    ID_CLOCK_RESET          = 10001,
    ID_HUMAN_VISIBLE        = 10002,
    ID_HUMAN_RUNNING        = 10003,
    ID_HUMAN_TIME           = 10004,
    ID_HUMAN_INCREMENT      = 10005,
    ID_ENGINE_VISIBLE       = 10006,
    ID_ENGINE_RUNNING       = 10007,
    ID_ENGINE_TIME          = 10008,
    ID_ENGINE_INCREMENT     = 10009,
    ID_WHITE_VISIBLE        = 10010,
    ID_WHITE_RUNNING        = 10011,
    ID_WHITE_TIME           = 10012,
    ID_WHITE_SECS           = 10013,
    ID_WHITE_INCREMENT      = 10014,
    ID_BLACK_VISIBLE        = 10015,
    ID_BLACK_RUNNING        = 10016,
    ID_BLACK_TIME           = 10017,
    ID_BLACK_SECS           = 10018,
    ID_BLACK_INCREMENT      = 10019,
    ID_FIXED_PERIOD_MODE    = 10020
};

// ClockDialog class declaration
class ClockDialog: public wxDialog
{    
    DECLARE_CLASS( ClockDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    ClockDialog( );
    ClockDialog( wxWindow* parent,
      wxWindowID id = ID_CLOCK_DIALOG,
      const wxString& caption = wxT("Clock Settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_CLOCK_DIALOG,
      const wxString& caption = wxT("Clock Settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // ClockDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOCK_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );
    void OnFixedPeriodMode( wxCommandEvent& event );

    // Data members
    ClockConfig dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog

private:
    wxCheckBox* human_visible_checkbox;
    wxCheckBox* human_running_checkbox;
    wxCheckBox* engine_visible_checkbox;
    wxCheckBox* engine_running_checkbox;
    wxCheckBox* fixed_period_mode;
};

#endif    // CLOCK_DIALOG_H
