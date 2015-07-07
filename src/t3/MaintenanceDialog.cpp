/****************************************************************************
 * Custom dialog - Maintenance Dialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/filename.h"
#include "wx/filepicker.h"
#include "DebugPrintf.h"
#include "Portability.h"
#include "Appdefs.h"
#include "DbPrimitives.h"
#include "DbMaintenance.h"
#include "MaintenanceDialog.h"

// MaintenanceDialog type definition
IMPLEMENT_CLASS( MaintenanceDialog, wxDialog )

// MaintenanceDialog event table definition
BEGIN_EVENT_TABLE( MaintenanceDialog, wxDialog )
// EVT_UPDATE_UI( ID_TEMP_VOTE, MaintenanceDialog::OnVoteUpdate )
// EVT_BUTTON( ID_TEMP_ENGINE_BROWSE, MaintenanceDialog::OnBrowseClick )

EVT_BUTTON( ID_MAINTENANCE_CMD_1, MaintenanceDialog::OnMaintenanceSpeed )
EVT_BUTTON( ID_MAINTENANCE_CMD_2, MaintenanceDialog::OnMaintenanceCompress )
EVT_BUTTON( ID_MAINTENANCE_CMD_3, MaintenanceDialog::OnMaintenanceDecompress )
EVT_BUTTON( ID_MAINTENANCE_CMD_4, MaintenanceDialog::OnMaintenanceVerify )
EVT_BUTTON( ID_MAINTENANCE_CMD_5, MaintenanceDialog::OnMaintenanceCreate )
EVT_BUTTON( ID_MAINTENANCE_CMD_6, MaintenanceDialog::OnMaintenanceIndexes )

EVT_BUTTON( wxID_HELP, MaintenanceDialog::OnHelpClick )
EVT_FILEPICKER_CHANGED( ID_TEMP_ENGINE_PICKER, MaintenanceDialog::OnFilePicked )
END_EVENT_TABLE()

// MaintenanceDialog constructors
MaintenanceDialog::MaintenanceDialog()
{
    Init();
}

MaintenanceDialog::MaintenanceDialog(
                           const EngineConfig &dat,
                           wxWindow* parent,
                           wxWindowID id, const wxString& caption,
                           const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    this->dat = dat;
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void MaintenanceDialog::Init()
{
    dat.m_file = "";
}

// Dialog create
bool MaintenanceDialog::Create( wxWindow* parent,
                          wxWindowID id, const wxString& caption,
                          const wxPoint& pos, const wxSize& size, long style )
{
    bool okay=true;
    
    // Get nbr of CPUs
#ifdef WINDOWS_FIX_LATER
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    nbr_cpus = sysinfo.dwNumberOfProcessors;
#else
    nbr_cpus = 1;
#endif
    
    // We have to set extra styles before creating the dialog
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP );
    if( !wxDialog::Create( parent, id, caption, pos, size, style ) )
        okay = false;
    else
    {
        
        CreateControls();
        SetDialogHelp();
        SetDialogValidators();
        
        // This fits the dialog to the minimum size dictated by the sizers
        GetSizer()->Fit(this);
        
        // This ensures that the dialog cannot be sized smaller than the minimum size
        GetSizer()->SetSizeHints(this);
        
        // Centre the dialog on the parent or (if none) screen
        Centre();
    }
    return okay;
}

// Control creation for MaintenanceDialog
void MaintenanceDialog::CreateControls()
{
    
    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    
    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
           "This panel is a placeholder for a proper database management facility.\n"
           "At the moment the only functionality offered is some database\n"
           "test and rebuild functions. Be careful with these, experts only !\n"
           "For now the files involved are mainly hardwired at compile time - to\n"
           "change them, change the source code and recompile! See files\n"
           "DbPrimitives.h and DbMaintenance.cpp.\n\n"
            #define DB_APPEND_ONLY
            #ifdef DB_APPEND_ONLY
           "Before appending to the database, make a copy of the production database;\n"
            DB_FILE "\n"
           "to a maintenance database;\n"
            DB_MAINTENANCE_FILE "\n"
           "Then use the append from .pgn button, for each .pgn you wish to add\n"
           "(a file picker GUI control does let you select that .pgn, at the\n"
           "moment avoid .pgn files with overlapping games).\n"
           "Finally manually replace the production database;\n"
            DB_FILE "\n"
           "With the newly created maintenance database and restart Tarrasch.\n"
            #endif
           , wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);
    
    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    
    // Label for file
    wxStaticText* file_label = new wxStaticText ( this, wxID_STATIC,
                                                 wxT("&Choose the next .pgn file with games to append to the database:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(file_label, 0, wxALIGN_LEFT|wxALL, 5);
    
    // File picker control
    wxString   database_file = DB_MAINTENANCE_FILE;
    wxFileName fn( database_file );
    wxString   pgn_file= DB_MAINTENANCE_PGN_FILE;
    wxFileName fn2( pgn_file );
    pgn_filename = fn2.GetFullPath();
    
    wxFilePickerCtrl *picker = new wxFilePickerCtrl( this, ID_TEMP_ENGINE_PICKER, pgn_filename, wxT("Select .pgn for append operation below"),
                                                    "*.pgn", wxDefaultPosition, wxDefaultSize,
                                                    wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );
    box_sizer->Add(picker, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);

/*
 
 I've left this stuff lying around in case it provides useful examples for future maintenance features
 
    // Ponder enabled
    wxCheckBox* ponder_box = new wxCheckBox( this, ID_TEMP_PONDER,
                                            wxT("&Ponder"), wxDefaultPosition, wxDefaultSize, 0 );
    ponder_box->SetValue( dat.m_ponder );
    box_sizer->Add( ponder_box, 0,
                   wxALIGN_CENTER_VERTICAL|wxALL, 5);
     // Label for the hash
     wxStaticText* hash_label = new wxStaticText ( this, wxID_STATIC,
     wxT("&Hash:"), wxDefaultPosition, wxDefaultSize, 0 );
     box_sizer->Add(hash_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
     
     // A spin control for the hash
     wxSpinCtrl* hash_spin = new wxSpinCtrl ( this, ID_TEMP_HASH,
     wxEmptyString, wxDefaultPosition, wxSize(60, -1),
     wxSP_ARROW_KEYS, 1, 4096, 64 );
     box_sizer->Add(hash_spin, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // Label for the hash
    wxStaticText* hash_label = new wxStaticText ( this, wxID_STATIC,
                                                 wxT("&Hash size:"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // A spin control for the hash
    wxSpinCtrl* hash_spin = new wxSpinCtrl ( this, ID_TEMP_HASH,
                                            wxEmptyString, wxDefaultPosition, wxSize(60, -1),
                                            wxSP_ARROW_KEYS, 1, 4096, 64 );
    wxBoxSizer* hash_horiz  = new wxBoxSizer(wxHORIZONTAL);
    hash_horiz->Add( hash_label,  0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    hash_horiz->Add( hash_spin,  0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    box_sizer->Add( hash_horiz, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5);
    
     // Label for max cpu cores
     wxStaticText* max_cpu_cores_label = new wxStaticText ( this, wxID_STATIC,
     wxT("&Max CPU cores:"), wxDefaultPosition, wxDefaultSize, 0 );
     box_sizer->Add(max_cpu_cores_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
     
     // A spin control for max cpu cores
     wxSpinCtrl* max_cpu_cores_spin = new wxSpinCtrl ( this, ID_TEMP_MAX_CPU_CORES,
     wxEmptyString, wxDefaultPosition, wxSize(60, -1),
     wxSP_ARROW_KEYS, 1, nbr_cpus, 1 );
     box_sizer->Add(max_cpu_cores_spin, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // Label for max cpu cores
    wxStaticText* max_cpu_cores_label = new wxStaticText ( this, wxID_STATIC,
                                                          wxT("&Max CPU cores:"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // A spin control for max cpu cores
    wxSpinCtrl* max_cpu_cores_spin = new wxSpinCtrl ( this, ID_TEMP_MAX_CPU_CORES,
                                                     wxEmptyString, wxDefaultPosition, wxSize(60, -1),
                                                     wxSP_ARROW_KEYS, 1, nbr_cpus, 1 );
    wxBoxSizer* max_cpu_cores_horiz  = new wxBoxSizer(wxHORIZONTAL);
    max_cpu_cores_horiz->Add( max_cpu_cores_label,  0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    max_cpu_cores_horiz->Add( max_cpu_cores_spin,  0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    box_sizer->Add( max_cpu_cores_horiz, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5);
    
    // Text controls for custom parameter 1
    wxTextCtrl *custom1a_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM1A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom1b_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM1B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    
    // Label for custom parameter 1
    wxStaticText* custom1_label = new wxStaticText ( this, wxID_STATIC,
                                                    wxT("Custom parameter 1 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom1_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom1_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom1_horiz->Add( custom1a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom1_horiz->Add( custom1b_ctrl,  1, wxALIGN_RIGHT|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom1_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 5 );
    
    // Text controls for custom parameter 2
    wxTextCtrl *custom2a_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM2A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom2b_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM2B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    
    // Label for custom parameter 2
    wxStaticText* custom2_label = new wxStaticText ( this, wxID_STATIC,
                                                    wxT("Custom parameter 2 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom2_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom2_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom2_horiz->Add( custom2a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom2_horiz->Add( custom2b_ctrl,  1, wxALIGN_RIGHT|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom2_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 5 );
    
    // Text controls for custom parameter 3
    wxTextCtrl *custom3a_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM3A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom3b_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM3B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    
    // Label for custom parameter 3
    wxStaticText* custom3_label = new wxStaticText ( this, wxID_STATIC,
                                                    wxT("Custom parameter 3 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom3_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom3_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom3_horiz->Add( custom3a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom3_horiz->Add( custom3b_ctrl,  1, wxALIGN_RIGHT|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom3_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 5 );
    
    // Text controls for custom parameter 4
    wxTextCtrl *custom4a_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM4A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom4b_ctrl = new wxTextCtrl ( this, ID_TEMP_CUSTOM4B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    
    // Label for custom parameter 4
    wxStaticText* custom4_label = new wxStaticText ( this, wxID_STATIC,
                                                    wxT("Custom parameter 4 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom4_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom4_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom4_horiz->Add( custom4a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom4_horiz->Add( custom4b_ctrl,  1, wxALIGN_RIGHT|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom4_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 5 );
*/
    
    // A dividing line before the database buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
                                           wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);
    
    // Temporary primitive database management functions
    wxBoxSizer* db_vert  = new wxBoxSizer(wxVERTICAL);
    box_sizer->Add( db_vert, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5);
    wxButton* button_cmd_1 = new wxButton( this, ID_MAINTENANCE_CMD_1, wxT("&Database speed test"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
    db_vert->Add( button_cmd_1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* button_cmd_2 = new wxButton( this, ID_MAINTENANCE_CMD_2, wxT("&Test compress file .pgn -> .qgn"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
    db_vert->Add( button_cmd_2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* button_cmd_3 = new wxButton( this, ID_MAINTENANCE_CMD_3, wxT("&Test decompress file .qgn -> .pgn"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
    db_vert->Add( button_cmd_3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* button_cmd_4 = new wxButton( this, ID_MAINTENANCE_CMD_4, wxT("&Test move compression algorithm"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
    db_vert->Add( button_cmd_4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* button_cmd_5 = new wxButton( this, ID_MAINTENANCE_CMD_5, wxT("&DANGER append to database from .pgn"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
    db_vert->Add( button_cmd_5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* button_cmd_6 = new wxButton( this, ID_MAINTENANCE_CMD_6, wxT("&DANGER database create indexes"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
    db_vert->Add( button_cmd_6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    
    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line2 = new wxStaticLine ( this, wxID_STATIC,
                                           wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line2, 0, wxGROW|wxALL, 5);
    
    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
  
   
    
    // The OK button
    wxButton* ok = new wxButton ( this, wxID_OK, wxT("&OK"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
                                     wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // The Help button
    wxButton* help = new wxButton( this, wxID_HELP, wxT("&Help"),
                                  wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

// Set the validators for the dialog controls
void MaintenanceDialog::SetDialogValidators()
{
    /*
    FindWindow(ID_TEMP_PONDER)->SetValidator(
                                        wxGenericValidator(& dat.m_ponder));
    FindWindow(ID_TEMP_HASH)->SetValidator(
                                      wxGenericValidator(& dat.m_hash));
    FindWindow(ID_TEMP_MAX_CPU_CORES)->SetValidator(
                                               wxGenericValidator(& dat.m_max_cpu_cores));
    FindWindow(ID_TEMP_CUSTOM1A)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom1a));
    FindWindow(ID_TEMP_CUSTOM1B)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom1b));
    FindWindow(ID_TEMP_CUSTOM2A)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom2a));
    FindWindow(ID_TEMP_CUSTOM2B)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom2b));
    FindWindow(ID_TEMP_CUSTOM3A)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom3a));
    FindWindow(ID_TEMP_CUSTOM3B)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom3b));
    FindWindow(ID_TEMP_CUSTOM4A)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom4a));
    FindWindow(ID_TEMP_CUSTOM4B)->SetValidator(
                                          wxTextValidator(wxFILTER_ASCII, &dat.m_custom4b));
     */
}

// Sets the help text for the dialog controls
void MaintenanceDialog::SetDialogHelp()
{
    /*
    wxString file_help = wxT("The UCI engine to use (a .exe file, eg Rybka v2.3.2a.mp.w32.exe, or komodo3-64.exe).");
    FindWindow(ID_TEMP_ENGINE_PICKER)->SetHelpText(file_help);
    FindWindow(ID_TEMP_ENGINE_PICKER)->SetToolTip(file_help);
    wxString ponder_help = wxT("Allow the engine to think on human's time (if capable).");
    FindWindow(ID_TEMP_PONDER)->SetHelpText(ponder_help);
    FindWindow(ID_TEMP_PONDER)->SetToolTip(ponder_help);
    wxString hash_help = wxT("The maximum size of hash table the engine is allowed (in megabytes).");
    FindWindow(ID_TEMP_HASH)->SetHelpText(hash_help);
    FindWindow(ID_TEMP_HASH)->SetToolTip(hash_help);
    wxString max_cpu_cores_help = wxT("The number of CPU cores the engine can use.");
    FindWindow(ID_TEMP_MAX_CPU_CORES)->SetHelpText(max_cpu_cores_help);
    FindWindow(ID_TEMP_MAX_CPU_CORES)->SetToolTip(max_cpu_cores_help);
    wxString custom_1a_help = wxT("Optional extra parameter, specify name here, eg UCI_Elo");
    FindWindow(ID_TEMP_CUSTOM1A)->SetHelpText(custom_1a_help);
    FindWindow(ID_TEMP_CUSTOM1A)->SetToolTip(custom_1a_help);
    wxString custom_1b_help = wxT("Optional extra parameter, specify value here, eg 1600");
    FindWindow(ID_TEMP_CUSTOM1B)->SetHelpText(custom_1b_help);
    FindWindow(ID_TEMP_CUSTOM1B)->SetToolTip(custom_1b_help);
    wxString custom_2a_help = wxT("Optional extra parameter, specify name here, eg UCI_LimitStrength");
    FindWindow(ID_TEMP_CUSTOM2A)->SetHelpText(custom_2a_help);
    FindWindow(ID_TEMP_CUSTOM2A)->SetToolTip(custom_2a_help);
    wxString custom_2b_help = wxT("Optional extra parameter, specify value here, eg true");
    FindWindow(ID_TEMP_CUSTOM2B)->SetHelpText(custom_2b_help);
    FindWindow(ID_TEMP_CUSTOM2B)->SetToolTip(custom_2b_help);
    wxString custom_a_help = wxT("Optional extra parameter, specify name here");
    FindWindow(ID_TEMP_CUSTOM3A)->SetHelpText(custom_a_help);
    FindWindow(ID_TEMP_CUSTOM3A)->SetToolTip(custom_a_help);
    FindWindow(ID_TEMP_CUSTOM4A)->SetHelpText(custom_a_help);
    FindWindow(ID_TEMP_CUSTOM4A)->SetToolTip(custom_a_help);
    wxString custom_b_help = wxT("Optional extra parameter, specify value here");
    FindWindow(ID_TEMP_CUSTOM3B)->SetHelpText(custom_b_help);
    FindWindow(ID_TEMP_CUSTOM3B)->SetToolTip(custom_b_help);
    FindWindow(ID_TEMP_CUSTOM4B)->SetHelpText(custom_b_help);
    FindWindow(ID_TEMP_CUSTOM4B)->SetToolTip(custom_b_help);
     */
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_1
void MaintenanceDialog::OnMaintenanceSpeed( wxCommandEvent& WXUNUSED(event) )
{
    db_maintenance_speed_tests();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_2
void MaintenanceDialog::OnMaintenanceCompress( wxCommandEvent& WXUNUSED(event) )
{
    db_maintenance_compress_pgn();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_3
void MaintenanceDialog::OnMaintenanceDecompress( wxCommandEvent& WXUNUSED(event) )
{
    db_maintenance_decompress_pgn();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_4
void MaintenanceDialog::OnMaintenanceVerify( wxCommandEvent& WXUNUSED(event) )
{
    db_maintenance_verify_compression();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_5
void MaintenanceDialog::OnMaintenanceCreate( wxCommandEvent& WXUNUSED(event) )
{
    #ifdef DB_APPEND_ONLY
    db_maintenance_create_or_append_to_database( pgn_filename.c_str() );
    const char *str_filename = pgn_filename.c_str();
    cprintf(                     "Appending file: %s\n", str_filename );
    #else
    #ifdef THC_MAC
    cprintf(                     "Appending file: /Users/billforster/Documents/Tarrasch/giant-base-part1-rebuilt.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/billforster/Documents/Tarrasch/giant-base-part1-rebuilt.pgn" );
    cprintf(                     "Appending file: /Users/billforster/Documents/Tarrasch/giant-base-part2-rebuilt.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/billforster/Documents/Tarrasch/giant-base-part2-rebuilt.pgn" );
    cprintf(                     "Appending file: /Users/billforster/Documents/Tarrasch/twic_minimal_overlap.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/billforster/Documents/Tarrasch/twic_minimal_overlap.pgn" );
    cprintf(                     "Appending file: /Users/billforster/Documents/Tarrasch/twic-948-1010.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/billforster/Documents/Tarrasch/twic-948-1010.pgn" );
    cprintf(                     "Appending file: /Users/billforster/Documents/Tarrasch/twic-1011-1048.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/billforster/Documents/Tarrasch/twic-1011-1048.pgn" );
    #else
    cprintf(                     "Appending file: /Users/Bill/Documents/T3Database/giant1.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/Bill/Documents/T3Database/giant1.pgn" );
    cprintf(                     "Appending file: /Users/Bill/Documents/T3Database/giant2.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/Bill/Documents/T3Database/giant2.pgn" );
    cprintf(                     "Appending file: /Users/Bill/Documents/T3Database/twic_minimal_overlap.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/Bill/Documents/T3Database/twic_minimal_overlap.pgn" );
    cprintf(                     "Appending file: /Users/Bill/Documents/T3Database/twic-948-1010.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/Bill/Documents/T3Database/twic-948-1010.pgn" );
    cprintf(                     "Appending file: /Users/Bill/Documents/T3Database/twic-1011-1050.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/Bill/Documents/T3Database/twic-1011-1050.pgn" );
    cprintf(                     "Appending file: /Users/Bill/Documents/T3Database/twic-1051-1077.pgn\n" );
    db_maintenance_create_or_append_to_database( "/Users/Bill/Documents/T3Database/twic-1051-1077.pgn" );
    #endif
    cprintf( "Appending files complete\n" );
    db_maintenance_create_indexes();    // temp
    cprintf( "TEMP - created indexes automatically\n" );
    #endif
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MAINTENANCE_CMD_6
void MaintenanceDialog::OnMaintenanceIndexes( wxCommandEvent& WXUNUSED(event) )
{
    db_maintenance_create_indexes();
}

// ID_TEMP_ENGINE_PICKER
void MaintenanceDialog::OnFilePicked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    pgn_filename = file;
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void MaintenanceDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
     wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */
    
    wxString helpText =
    wxT("Add help text later.\n"); 
    wxMessageBox(helpText,
                 wxT("Maintenance dialog help"),
                 wxOK|wxICON_INFORMATION, this);
}
