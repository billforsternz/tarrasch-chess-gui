/****************************************************************************
 * Custom dialog - Maintain database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef MAINTENANCE_DIALOG_H
#define MAINTENANCE_DIALOG_H

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/filepicker.h"
#include "SuspendEngine.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_TEMP_ENGINE_DIALOG   = 10000,
    ID_MAINTENANCE_CMD_1    = 10001,
    ID_MAINTENANCE_CMD_2    = 10002,
    ID_MAINTENANCE_CMD_3    = 10003,
    ID_MAINTENANCE_CMD_4    = 10004,
    ID_MAINTENANCE_CMD_5    = 10005,
    ID_MAINTENANCE_CMD_6    = 10006,
    ID_MAINTENANCE_CMD_7    = 10007,
    ID_TEMP_ENGINE_PICKER   = 10008,
    ID_TEMP_PONDER          = 10009,
    ID_TEMP_HASH            = 10010,
    ID_TEMP_MAX_CPU_CORES   = 10011,
    ID_TEMP_CUSTOM1A        = 10012,
    ID_TEMP_CUSTOM1B        = 10013,
    ID_TEMP_CUSTOM2A        = 10014,
    ID_TEMP_CUSTOM2B        = 10015,
    ID_TEMP_CUSTOM3A        = 10016,
    ID_TEMP_CUSTOM3B        = 10017,
    ID_TEMP_CUSTOM4A        = 10018,
    ID_TEMP_CUSTOM4B        = 10019
};

// MaintenanceDialog class declaration
class MaintenanceDialog: public wxDialog
{
    DECLARE_CLASS( MaintenanceDialog )
    DECLARE_EVENT_TABLE()
    
public:
    
    // Constructors
    MaintenanceDialog( );
    MaintenanceDialog(
                 const EngineConfig &dat,
                 wxWindow* parent,
                 wxWindowID id = ID_TEMP_ENGINE_DIALOG,
                 const wxString& caption = wxT("Maintenance settings"),
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX  );
    
    // Member initialisation
    void Init();
    
    // Creation
    bool Create( wxWindow* parent,
                wxWindowID id = ID_TEMP_ENGINE_DIALOG,
                const wxString& caption = wxT("Maintenance settings"),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );
    
    // Creates the controls and sizers
    void CreateControls();
    
    // Sets the validators for the dialog controls
    void SetDialogValidators();
    
    // Sets the help text for the dialog controls
    void SetDialogHelp();
    
    // MaintenanceDialog event handler declarations
    
    // wxEVT_UPDATE_UI event handler for ID_TEMP_???
    //void On???Update( wxUpdateUIEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TEMP_ENGINE_BROWSE
    //void OnBrowseClick( wxCommandEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_1
    void OnMaintenanceSpeed( wxCommandEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_2
    void OnMaintenanceCompress( wxCommandEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_3
    void OnMaintenanceDecompress( wxCommandEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_4
    void OnMaintenanceVerify( wxCommandEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_5
    void OnMaintenanceCreate( wxCommandEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_6
    void OnMaintenanceExtraIndexes( wxCommandEvent& event );
    
    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );
    
    // ID_TEMP_ENGINE_PICKER
    void OnFilePicked( wxFileDirPickerEvent& event );
    
    // MaintenanceDialog member variables
    
    // Data members
    EngineConfig   dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
    wxString pgn_filename;
    int nbr_cpus;
};

#endif    // MAINTENANCE_DIALOG_H
