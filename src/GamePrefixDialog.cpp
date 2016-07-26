/****************************************************************************
 * Custom dialog - Game Prefix
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <time.h>
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "Appdefs.h"
#include "GamePrefixDialog.h"

// GamePrefixDialog type definition
IMPLEMENT_CLASS( GamePrefixDialog, wxDialog )

// GamePrefixDialog event table definition
BEGIN_EVENT_TABLE( GamePrefixDialog, wxDialog )
    EVT_BUTTON( ID_GAME_PREFIX_RESET, GamePrefixDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, GamePrefixDialog::OnHelpClick )
    EVT_BUTTON( wxID_OK, GamePrefixDialog::OnOkClick )
END_EVENT_TABLE()

// GamePrefixDialog constructors
GamePrefixDialog::GamePrefixDialog()
{
    Init();
}

GamePrefixDialog::GamePrefixDialog( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void GamePrefixDialog::Init()
{
    prefix_txt = "";
}

// Create dialog
bool GamePrefixDialog::Create( wxWindow* parent,
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

#define SMALL 5
#define LARGE 2

// Control creation for GamePrefixDialog
void GamePrefixDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, SMALL);

//    // A friendly message
//    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
//        wxT("Game Prefix."), wxDefaultPosition, wxDefaultSize, 0 );
//    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, SMALL);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, SMALL);

/*
   +-H1-------------+
   |+-V1--+  +-V2--+|
   ||+---+|  |+---+||
   |||   ||  ||   |||
   ||+---+|  |+---+||
   ||     |  |     ||
   ||+---+|  |+---+||
   |||   ||  ||   |||
   ||+---+|  |+---+||
   |+-----+  +-----+|
   +----------------+

   +-H2-------------+
   |+-----+  +-----+|
   ||     |  |     ||
   |+-----+  +-----+|
   +----------------+

          .....

   +-HN-------------+
   |+-----+  +-----+|
   ||     |  |     ||
   |+-----+  +-----+|
   +----------------+
*/

    //wxBoxSizer* H1 = new wxBoxSizer(wxHORIZONTAL);
    //wxBoxSizer* V1 = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *HV1 = new wxFlexGridSizer( 10, 2, 0, 0 );
    #define WIDTH 600
    #define HEIGHT 400
    wxSize sz=wxSize(WIDTH,HEIGHT);

    // Label for prefix_txt
 /*   wxStaticText* prefix_label = new wxStaticText ( this, wxID_STATIC,
        wxT("prefix "), wxDefaultPosition, wxDefaultSize, 0 );
    HV1->Add(prefix_label, 0, wxALIGN_LEFT|wxALL, SMALL); */

    // Text control for prefix_txt
    prefix_ctrl = new wxTextCtrl ( this, ID_GAME_PREFIX_TXT,  wxT(""), wxDefaultPosition, sz, wxTE_MULTILINE );
    HV1->Add(prefix_ctrl, 0, wxGROW|wxALL, SMALL);

    //H1->Add(V2, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);
    box_sizer->Add(HV1, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, SMALL);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);

    // The OK button
    wxButton* ok = new wxButton ( this, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, SMALL);

    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
        wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, SMALL);

    // The Help button
    wxButton* help = new wxButton( this, wxID_HELP, wxT("&Help"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, SMALL);
}

// Set the validators for the dialog controls
void GamePrefixDialog::SetDialogValidators()
{
    FindWindow(ID_GAME_PREFIX_TXT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &prefix_txt));
}

// Sets the help text for the dialog controls
void GamePrefixDialog::SetDialogHelp()
{
    wxString prefix_help    = wxT("The text that prefixes this game.");

    FindWindow(ID_GAME_PREFIX_TXT)->SetHelpText(prefix_help);
    FindWindow(ID_GAME_PREFIX_TXT)->SetToolTip(prefix_help);
}


// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYER_RESET
void GamePrefixDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void GamePrefixDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{

    wxString helpText =
      wxT("Use this panel to edit text that prefixes the\n")
      wxT("game on the .pgn file.\n");

    wxMessageBox(helpText,
      wxT("Game Prefix Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GamePrefixDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    AcceptAndClose();
}

bool GamePrefixDialog::Run( GameDocument &gd )
{
    bool ok=false;
    prefix_txt = gd.prefix_txt;
    prefix_ctrl->SetValue(prefix_txt);
    prefix_ctrl->SetInsertionPoint(0);
    if( wxID_OK == ShowModal() )
    {
        ok = true;
        gd.game_prefix_edited = true;
        gd.prefix_txt = prefix_txt;
    }
    return ok;
}
