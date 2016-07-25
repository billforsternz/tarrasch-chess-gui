/****************************************************************************
 * Custom dialog - Opening book
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/filepicker.h"
#include "Appdefs.h"
#include "BookDialog.h"

// BookDialog type definition
IMPLEMENT_CLASS( BookDialog, wxDialog )

// BookDialog event table definition
BEGIN_EVENT_TABLE( BookDialog, wxDialog )
 // EVT_UPDATE_UI( ID_VOTE, BookDialog::OnVoteUpdate )
    EVT_BUTTON( ID_BOOK_RESET, BookDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, BookDialog::OnHelpClick )
    EVT_FILEPICKER_CHANGED( ID_BOOK_PICKER, BookDialog::OnFilePicked )
END_EVENT_TABLE()

// BookDialog constructors
BookDialog::BookDialog()
{
    Init();
}

BookDialog::BookDialog(
  const BookConfig &dat,
  wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    this->dat = dat;
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void BookDialog::Init()
{
}

// Dialog create
bool BookDialog::Create( wxWindow* parent,
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

// Control creation for BookDialog
void BookDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT("This panel lets you setup the opening book."), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Label for file
    wxStaticText* file_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&File "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(file_label, 0, wxALIGN_LEFT|wxALL, 5);

    // File picker control
    wxString path = dat.m_file;
    wxFilePickerCtrl *picker = new wxFilePickerCtrl( this, ID_BOOK_PICKER, path, wxT("Select .pgn file for book"),
        "*.pgn", wxDefaultPosition, wxDefaultSize, 
        wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST );    
    box_sizer->Add(picker, 1, wxALIGN_LEFT|wxEXPAND|wxALL, 5);

    // File enabled
    wxCheckBox* enabled_box = new wxCheckBox( this, ID_ENABLED,
       wxT("&Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
    enabled_box->SetValue( dat.m_enabled );
    box_sizer->Add( enabled_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Label for the moves limit
    wxStaticText* limit_moves_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&If available always play book moves up until move:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(limit_moves_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // A spin control for the moves limit
    wxSpinCtrl* limit_moves_spin = new wxSpinCtrl ( this, ID_LIMIT_MOVES,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 25 );
    box_sizer->Add(limit_moves_spin, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Label for the post limit percentage
    wxStaticText* post_limit_percent_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Then if available play book moves this percentage of the time:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(post_limit_percent_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // A spin control for the post limit percentage
    wxSpinCtrl* post_limit_percent_spin = new wxSpinCtrl ( this, ID_POST_LIMIT_PERCENT,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 100, 50 );
    box_sizer->Add(post_limit_percent_spin, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Suggest book moves
    wxCheckBox* suggest_box = new wxCheckBox( this, ID_SUGGEST,
       wxT("&Suggest book moves"), wxDefaultPosition, wxDefaultSize, 0 );
    suggest_box->SetValue( dat.m_suggest );
    box_sizer->Add( suggest_box, 0,
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
    wxButton* reset = new wxButton( this, ID_BOOK_RESET, wxT("&Reset"),
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
void BookDialog::SetDialogValidators()
{
    FindWindow(ID_ENABLED)->SetValidator(
        wxGenericValidator(& dat.m_enabled));
    FindWindow(ID_SUGGEST)->SetValidator(
        wxGenericValidator(& dat.m_suggest));
    FindWindow(ID_LIMIT_MOVES)->SetValidator(
        wxGenericValidator(& dat.m_limit_moves));
    FindWindow(ID_POST_LIMIT_PERCENT)->SetValidator(
        wxGenericValidator(& dat.m_post_limit_percent));
}

// Sets the help text for the dialog controls
void BookDialog::SetDialogHelp()
{
    wxString file_help              = wxT("The .pgn file that is used for opening book moves.");
    wxString enabled_help            = wxT("Clear this box to disable the book.");
    wxString limit_moves_help        = wxT("Initially always play a book move if available.");
    wxString post_limit_percent_help = wxT("Once limit reached, a book move is only selected a proportion of the time.");

    FindWindow(ID_BOOK_PICKER)->SetHelpText(file_help);
    FindWindow(ID_BOOK_PICKER)->SetToolTip(file_help);

    FindWindow(ID_ENABLED)->SetHelpText(enabled_help);
    FindWindow(ID_ENABLED)->SetToolTip(enabled_help);

    FindWindow(ID_LIMIT_MOVES)->SetHelpText(limit_moves_help);
    FindWindow(ID_LIMIT_MOVES)->SetToolTip(limit_moves_help);

    FindWindow(ID_POST_LIMIT_PERCENT)->SetHelpText(post_limit_percent_help);
    FindWindow(ID_POST_LIMIT_PERCENT)->SetToolTip(post_limit_percent_help);
}

/*
// wxEVT_UPDATE_UI event handler for ID_CHECKBOX
void BookDialog::OnVoteUpdate( wxUpdateUIEvent& event )
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

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BOOK_RESET
void BookDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// ID_BOOK_PICKER
void BookDialog::OnFilePicked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    dat.m_file = file;
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void BookDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to select a .pgn file to use as the\n")
      wxT("opening book and to setup how the book is used.\n\n")
      wxT("The last two parameters let you control how\n")
      wxT("early or late the engine will start thinking\n")
      wxT("for itself rather than using book moves.\n\n")
      wxT("Note that the book may contain endgame training\n")
      wxT("exercises (refer to \"training positions\" when\n")
      wxT("setting up a position) and if you start a game\n")
      wxT("from one of those positions the engine will always\n")
      wxT("use a book move if one is available.\n");

    wxMessageBox(helpText,
      wxT("Book Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}
