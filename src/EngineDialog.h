/****************************************************************************
 * Custom dialog - Select UCI engine
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef ENGINE_DIALOG_H
#define ENGINE_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/filepicker.h"
#include "SuspendEngine.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_ENGINE_DIALOG   = 10000,
    ID_ENGINE_RESET    = 10001,
    ID_ENGINE_PICKER   = 10002,
    ID_PONDER          = 10003,
    ID_HASH            = 10004,
    ID_MAX_CPU_CORES   = 10005,
    ID_CUSTOM1A        = 10006,
    ID_CUSTOM1B        = 10007,
    ID_CUSTOM2A        = 10008,
    ID_CUSTOM2B        = 10009,
    ID_CUSTOM3A        = 10010,
    ID_CUSTOM3B        = 10011,
    ID_CUSTOM4A        = 10012,
    ID_CUSTOM4B        = 10013, 
    ID_CUSTOM5A        = 10014,
    ID_CUSTOM5B        = 10015, 
    ID_CUSTOM6A        = 10016,
    ID_CUSTOM6B        = 10017, 
	ID_NORMAL_PRIORITY = 10018,
	ID_LOW_PRIORITY    = 10019,
	ID_IDLE_PRIORITY   = 10020
};

// EngineDialog class declaration
class EngineDialog: public wxDialog
{    
    DECLARE_CLASS( EngineDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    EngineDialog( );
    EngineDialog(
      const EngineConfig &dat,
      wxWindow* parent,
      wxWindowID id = ID_ENGINE_DIALOG,
      const wxString& caption = wxT("Engine settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX  );

    // Member initialisation
    void Init();

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_ENGINE_DIALOG,   
      const wxString& caption = wxT("Engine settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // EngineDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ENGINE_BROWSE
    //void OnBrowseClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // ID_ENGINE_PICKER
    void OnFilePicked( wxFileDirPickerEvent& event );

    // EngineDialog member variables

    // Data members
    EngineConfig   dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
    int nbr_cpus;
};

#endif    // ENGINE_DIALOG_H
