/****************************************************************************
 * Custom dialog - Log
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LOG_DIALOG_H
#define LOG_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/filepicker.h"
#include "SuspendEngine.h"
#include "DialogDetect.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_LOG_DIALOG         = 10000,
    ID_LOG_RESET          = 10001,
    ID_LOG_ENABLED        = 10002,
    ID_LOG_PICKER         = 10003
};

// LogDialog class declaration
class LogDialog: public wxDialog
{    
    DECLARE_CLASS( LogDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    LogDialog();
    LogDialog(
      const LogConfig &dat,
      wxWindow* parent,
      wxWindowID id = ID_LOG_DIALOG,
      const wxString& caption = wxT("Log file settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_LOG_DIALOG,
      const wxString& caption = wxT("Log file settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // LogDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // ID_LOG_PICKER
    void OnFilePicked( wxFileDirPickerEvent& event );
    
    // LogDialog member variables

    // Data members
    LogConfig   dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
	DialogDetect    detect;		// similarly the presence of this var allows tracking of open dialogs
};

#endif    // LOG_DIALOG_H
