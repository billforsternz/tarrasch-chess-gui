/****************************************************************************
 * Custom dialog - Player names
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "Appdefs.h"
#include "PlayerDialog.h"

// PlayerDialog type definition
IMPLEMENT_CLASS( PlayerDialog, wxDialog )

// PlayerDialog event table definition
BEGIN_EVENT_TABLE( PlayerDialog, wxDialog )
 // EVT_UPDATE_UI( ID_VOTE, PlayerDialog::OnVoteUpdate )
    EVT_BUTTON( ID_PLAYER_RESET, PlayerDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, PlayerDialog::OnHelpClick )
END_EVENT_TABLE()

// PlayerDialog constructors
PlayerDialog::PlayerDialog()
{
    Init();
}

PlayerDialog::PlayerDialog( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void PlayerDialog::Init()
{
    dat.m_human    = "";
//    dat.m_computer = "";
    dat.m_white    = "";
    dat.m_black    = "";
}

// Dialog create
bool PlayerDialog::Create( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    bool okay=true;

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

// Control creation for PlayerDialog
void PlayerDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT("This panel lets you set player names."), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Label for white
    wxStaticText* white_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Current white player "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(white_label, 0, wxALIGN_LEFT|wxALL, 5);

    // Text control for white
    wxTextCtrl* white_ctrl = new wxTextCtrl ( this, ID_WHITE,           wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(white_ctrl, 0, wxGROW|wxALL, 5);

    // Label for black
    wxStaticText* black_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Current black player "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(black_label, 0, wxALIGN_LEFT|wxALL, 5);

    // Text control for black
    wxTextCtrl* black_ctrl = new wxTextCtrl ( this, ID_BLACK,           wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(black_ctrl, 0, wxGROW|wxALL, 5);

    // Label for human
    wxStaticText* human_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Usual name of human player in human vs engine games "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(human_label, 0, wxALIGN_LEFT|wxALL, 5);

    // Text control for human
    wxTextCtrl* human_ctrl = new wxTextCtrl ( this, ID_HUMAN, wxT("Bill"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(human_ctrl, 0, wxGROW|wxALL, 5);

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
/*
    // The Reset button
    wxButton* reset = new wxButton( this, ID_PLAYER_RESET, wxT("&Reset"),
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
void PlayerDialog::SetDialogValidators()
{
    FindWindow(ID_HUMAN)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_human));
//    FindWindow(ID_COMPUTER)->SetValidator(
//        wxTextValidator(wxFILTER_ASCII, &dat.m_computer));
    FindWindow(ID_WHITE)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_white));
    FindWindow(ID_BLACK)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_black));
}

// Sets the help text for the dialog controls
void PlayerDialog::SetDialogHelp()
{
    wxString human_help    = wxT("The person who usually uses this program to play against a chess engine.");
//    wxString computer_help = wxT("An optional friendly name for the chess engine.");
    wxString white_help    = wxT("White's name.");
    wxString black_help    = wxT("Black's name.");

    FindWindow(ID_HUMAN)->SetHelpText(human_help);
    FindWindow(ID_HUMAN)->SetToolTip(human_help);

//    FindWindow(ID_COMPUTER)->SetHelpText(computer_help);
//    FindWindow(ID_COMPUTER)->SetToolTip(computer_help);

    FindWindow(ID_WHITE)->SetHelpText(white_help);
    FindWindow(ID_WHITE)->SetToolTip(white_help);

    FindWindow(ID_BLACK)->SetHelpText(black_help);
    FindWindow(ID_BLACK)->SetToolTip(black_help);
}

/*
// wxEVT_UPDATE_UI event handler for ID_CHECKBOX
void PlayerDialog::OnVoteUpdate( wxUpdateUIEvent& event )
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

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYER_RESET
void PlayerDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void PlayerDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to name the players involved in the\n")
      wxT("onscreen games.\n\n")
      wxT("A shortcut to this dialog is available; simply\n")
      wxT("click on a player name.\n");

    wxMessageBox(helpText,
      wxT("Player Name Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}
