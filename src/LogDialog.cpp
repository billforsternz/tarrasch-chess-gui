/****************************************************************************
 * Custom dialog - Log
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "Appdefs.h"
#include "LogDialog.h"

// LogDialog type definition
IMPLEMENT_CLASS( LogDialog, wxDialog )

// LogDialog event table definition
BEGIN_EVENT_TABLE( LogDialog, wxDialog )
 // EVT_UPDATE_UI( ID_VOTE, LogDialog::OnVoteUpdate )
    EVT_BUTTON( ID_LOG_RESET, LogDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, LogDialog::OnHelpClick )
    EVT_FILEPICKER_CHANGED( ID_LOG_PICKER, LogDialog::OnFilePicked )
END_EVENT_TABLE()

// LogDialog constructors
LogDialog::LogDialog()
{
    Init();
}

LogDialog::LogDialog(
  const LogConfig &dat,
  wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    this->dat = dat;
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void LogDialog::Init()
{
    dat.m_file = "";
    dat.m_enabled = false;
}

// Dialog create
bool LogDialog::Create( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    bool okay=true;

    // We have to set extra styles before creating the dialog
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS/*|wxDIALOG_EX_CONTEXTHELP*/ );
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

// Control creation for LogDialog
void LogDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT("This panel lets you setup the .pgn log file."), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Label for file
    wxStaticText* file_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Log file "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(file_label, 0, wxALIGN_LEFT|wxALL, 5);

    // File picker control
    wxString path = dat.m_file;
    wxFilePickerCtrl *picker = new wxFilePickerCtrl( this, ID_LOG_PICKER, path, wxT("Select log file"),
        "*.pgn", wxDefaultPosition, wxDefaultSize, 
        wxFLP_USE_TEXTCTRL|wxFLP_OPEN );    
    box_sizer->Add(picker, 1, wxALIGN_LEFT|wxEXPAND|wxALL, 5);

    // File enabled
    wxCheckBox* enabled_box = new wxCheckBox( this, ID_LOG_ENABLED,
       wxT("&Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
    enabled_box->SetValue( dat.m_enabled );
    box_sizer->Add( enabled_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
/*
    // The Reset button
    wxButton* reset = new wxButton( this, ID_LOG_RESET, wxT("&Reset"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(reset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
 */
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
void LogDialog::SetDialogValidators()
{
    FindWindow(ID_LOG_ENABLED)->SetValidator(
        wxGenericValidator(& dat.m_enabled));
}

// Sets the help text for the dialog controls
void LogDialog::SetDialogHelp()
{
    wxString file_help               = wxT("The .pgn file that is created to save your activity.");
    wxString enabled_help            = wxT("Clear this box to disable logging.");

    FindWindow(ID_LOG_PICKER)->SetHelpText(file_help);
    FindWindow(ID_LOG_PICKER)->SetToolTip(file_help);

    FindWindow(ID_LOG_ENABLED)->SetHelpText(enabled_help);
    FindWindow(ID_LOG_ENABLED)->SetToolTip(enabled_help);
}

/*
// wxEVT_UPDATE_UI event handler for ID_CHECKBOX
void LogDialog::OnVoteUpdate( wxUpdateUIEvent& event )
{
    wxSpinCtrl* ageCtrl = (wxSpinCtrl*) FindWindow(ID_AGE);
    if (ageCtrl->GetValue() < 18)
    {
        event.Enable(false);
        event.Check(false);
    }
    else
        event.Enable(true);
}
*/

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOG_RESET
void LogDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// ID_LOG_PICKER
void LogDialog::OnFilePicked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    dat.m_file = file;
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void LogDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to select a .pgn file to save a record\n")
      wxT("of activity. You can subsequently access the log using\n")
      wxT("the File - Open log file menu command\n" );

    wxMessageBox(helpText,
      wxT("Log file dialog help"),
      wxOK|wxICON_INFORMATION, this);
}
