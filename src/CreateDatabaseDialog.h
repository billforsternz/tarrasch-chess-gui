/****************************************************************************
 * Custom dialog - Create (or append to) database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CREATE_DATABASE_DIALOG_H
#define CREATE_DATABASE_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/filepicker.h"
#include "SuspendEngine.h"
#include "DialogDetect.h"
//#include "Repository.h"

// Control identifiers
enum
{
    ID_CREATE_DB_DIALOG          = 10000,
    ID_CREATE_DB_CREATE          = 10001,
    ID_CREATE_DB_APPEND          = 10002,
    ID_CREATE_DB_PICKER_DB       = 10003,
    ID_CREATE_DB_PICKER1         = 10004,
    ID_CREATE_DB_PICKER2         = 10005,
    ID_CREATE_DB_PICKER3         = 10006,
    ID_CREATE_DB_PICKER4         = 10007,
    ID_CREATE_DB_PICKER5         = 10008,
    ID_CREATE_DB_PICKER6         = 10009,
    ID_CREATE_ELO_CUTOFF         = 10010,
    ID_CREATE_RESTRICTED         = 10011
};

// CreateDatabaseDialog class declaration
class CreateDatabaseDialog: public wxDialog
{
    DECLARE_CLASS( CreateDatabaseDialog )
    DECLARE_EVENT_TABLE()
    
public:
    
    // Constructor
    CreateDatabaseDialog(
                 wxWindow* parent,
                 wxWindowID id = ID_CREATE_DB_DIALOG,
                 bool create_mode = true,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX  );
    
    // Creation
    bool Create( wxWindow* parent,
                wxWindowID id = ID_CREATE_DB_DIALOG,
                const wxString& caption = wxT("Create Database"),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );
    
    // Creates the controls and sizers
    void CreateControls();
    
    // Sets the validators for the dialog controls
    void SetDialogValidators();
    
    // Sets the help text for the dialog controls
    void SetDialogHelp();
    
    // CreateDatabaseDialog event handler declarations
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOk( wxCommandEvent& event );  // selects one of the following
    void OnCreateDatabase();
    void OnAppendDatabase();

    // Help button
    void OnHelpClick( wxCommandEvent& event ) ;
    
    // File pickers
    void OnDbFilePicked( wxFileDirPickerEvent& event );
    void OnPgnFile1Picked( wxFileDirPickerEvent& event );
    void OnPgnFile2Picked( wxFileDirPickerEvent& event );
    void OnPgnFile3Picked( wxFileDirPickerEvent& event );
    void OnPgnFile4Picked( wxFileDirPickerEvent& event );
    void OnPgnFile5Picked( wxFileDirPickerEvent& event );
    void OnPgnFile6Picked( wxFileDirPickerEvent& event );
    void OnCreateTinyDb( wxCommandEvent& event );
    
    // CreateDatabaseDialog member variables
    
    // Data members
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
	DialogDetect    detect;		// similarly the presence of this var allows tracking of open dialogs
    wxString db_filename;
    wxString pgn_filename1;
    wxString pgn_filename2;
    wxString pgn_filename3;
    wxString pgn_filename4;
    wxString pgn_filename5;
    wxString pgn_filename6;
    bool create_mode;   // or append
    bool db_created_ok;
    std::string db_name;
    bool create_tiny_db;
    wxSpinCtrl    *elo_cutoff_spin;
    wxSpinCtrl    *before_year;
    wxRadioButton *ignore;
    wxRadioButton *at_least_one;
    wxRadioButton *both;
    wxRadioButton *fail;
    wxRadioButton *pass;
    wxRadioButton *pass_before;
    wxCheckBox *restricted_box;
};

#endif    // CREATE_DATABASE_DIALOG_H
