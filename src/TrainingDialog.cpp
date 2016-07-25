/****************************************************************************
 * Custom dialog - Training
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "Appdefs.h"
#include "TrainingDialog.h"

// TrainingDialog type definition
IMPLEMENT_CLASS( TrainingDialog, wxDialog )

// TrainingDialog event table definition
BEGIN_EVENT_TABLE( TrainingDialog, wxDialog )
 // EVT_UPDATE_UI( ID_VOTE, TrainingDialog::OnVoteUpdate )
    EVT_BUTTON( ID_TRAINING_RESET, TrainingDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, TrainingDialog::OnHelpClick )
END_EVENT_TABLE()

// TrainingDialog constructors
TrainingDialog::TrainingDialog()
{
    Init();
}

TrainingDialog::TrainingDialog( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void TrainingDialog::Init()
{
    dat.m_nbr_half_moves_behind         = 0;
    dat.m_peek_at_complete_position     = false;
    dat.m_blindfold_hide_white_pawns    = false;
    dat.m_blindfold_hide_white_pieces   = false;
    dat.m_blindfold_hide_black_pawns    = false;
    dat.m_blindfold_hide_black_pieces   = false;
}

// Dialog create
bool TrainingDialog::Create( wxWindow* parent,
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

// Control creation for TrainingDialog
void TrainingDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT("This panel lets you setup training options."), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Label for the nbr half moves behind
    wxStaticText* nbr_half_moves_behind_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Board position lags play by this many half moves:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(nbr_half_moves_behind_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // A spin control for the nbr half moves behind
    wxSpinCtrl* nbr_half_moves_behind_spin = new wxSpinCtrl ( this, ID_NBR_HALF_MOVES_BEHIND,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 25 );
    box_sizer->Add(nbr_half_moves_behind_spin, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Peek at complete position
    wxCheckBox* peek_at_complete_position_box = new wxCheckBox( this, ID_PEEK_AT_COMPLETE_POSITION,
       wxT("&Peek at complete position"), wxDefaultPosition, wxDefaultSize, 0 );
    peek_at_complete_position_box->SetValue( dat.m_peek_at_complete_position );
    box_sizer->Add( peek_at_complete_position_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Blindfold hide white pawns
    wxCheckBox* blindfold_hide_white_pawns_box = new wxCheckBox( this, ID_BLINDFOLD_HIDE_WHITE_PAWNS,
       wxT("&Blindfold: hide white pawns"), wxDefaultPosition, wxDefaultSize, 0 );
    blindfold_hide_white_pawns_box->SetValue( dat.m_blindfold_hide_white_pawns );
    box_sizer->Add( blindfold_hide_white_pawns_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Blindfold hide white pieces
    wxCheckBox* blindfold_hide_white_pieces_box = new wxCheckBox( this, ID_BLINDFOLD_HIDE_WHITE_PIECES,
       wxT("&Blindfold: hide white pieces"), wxDefaultPosition, wxDefaultSize, 0 );
    blindfold_hide_white_pieces_box->SetValue( dat.m_blindfold_hide_white_pieces );
    box_sizer->Add( blindfold_hide_white_pieces_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Blindfold hide black pawns
    wxCheckBox* blindfold_hide_black_pawns_box = new wxCheckBox( this, ID_BLINDFOLD_HIDE_BLACK_PAWNS,
       wxT("&Blindfold: hide black pawns"), wxDefaultPosition, wxDefaultSize, 0 );
    blindfold_hide_black_pawns_box->SetValue( dat.m_blindfold_hide_black_pawns );
    box_sizer->Add( blindfold_hide_black_pawns_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Blindfold hide black pieces
    wxCheckBox* blindfold_hide_black_pieces_box = new wxCheckBox( this, ID_BLINDFOLD_HIDE_BLACK_PIECES,
       wxT("&Blindfold: hide black pieces"), wxDefaultPosition, wxDefaultSize, 0 );
    blindfold_hide_black_pieces_box->SetValue( dat.m_blindfold_hide_black_pawns );
    box_sizer->Add( blindfold_hide_black_pieces_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Blindfold hide board
 /* wxCheckBox* blindfold_hide_board_box = new wxCheckBox( this, ID_BLINDFOLD_HIDE_BOARD,
       wxT("&Blindfold: hide board"), wxDefaultPosition, wxDefaultSize, 0 );
    blindfold_hide_board_box->SetValue( dat.m_blindfold_hide_board );
    box_sizer->Add( blindfold_hide_board_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5); */

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
 /*
    // The Reset button
    wxButton* reset = new wxButton( this, ID_TRAINING_RESET, wxT("&Reset"),
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
void TrainingDialog::SetDialogValidators()
{
    FindWindow(ID_NBR_HALF_MOVES_BEHIND)->SetValidator(
        wxGenericValidator(& dat.m_nbr_half_moves_behind));
    FindWindow(ID_PEEK_AT_COMPLETE_POSITION)->SetValidator(
        wxGenericValidator(& dat.m_peek_at_complete_position));
    FindWindow(ID_BLINDFOLD_HIDE_WHITE_PAWNS)->SetValidator(
        wxGenericValidator(& dat.m_blindfold_hide_white_pawns));
    FindWindow(ID_BLINDFOLD_HIDE_WHITE_PIECES)->SetValidator(
        wxGenericValidator(& dat.m_blindfold_hide_white_pieces));
    FindWindow(ID_BLINDFOLD_HIDE_BLACK_PAWNS)->SetValidator(
        wxGenericValidator(& dat.m_blindfold_hide_black_pawns));
    FindWindow(ID_BLINDFOLD_HIDE_BLACK_PIECES)->SetValidator(
        wxGenericValidator(& dat.m_blindfold_hide_black_pieces));
/*  FindWindow(ID_BLINDFOLD_HIDE_BOARD)->SetValidator(
        wxGenericValidator(& dat.m_blindfold_hide_board)); */
}

// Sets the help text for the dialog controls
void TrainingDialog::SetDialogHelp()
{
    wxString nbr_half_moves_behind_help       = wxT("Non zero values train the user to visualise future positions.");
    wxString peek_at_complete_position_help   = wxT("Set this box to peek at the complete current position when making moves.");
    wxString blindfold_hide_white_pawns_help  = wxT("Set this box to hide white pawns.");
    wxString blindfold_hide_white_pieces_help = wxT("Set this box to hide white pieces.");
    wxString blindfold_hide_black_pawns_help  = wxT("Set this box to hide black pawns.");
    wxString blindfold_hide_black_pieces_help = wxT("Set this box to hide black pieces.");
//  wxString blindfold_hide_board_help        = wxT("Set this box to hide the squares of the board.");

    FindWindow(ID_NBR_HALF_MOVES_BEHIND)     ->SetHelpText(nbr_half_moves_behind_help);
    FindWindow(ID_NBR_HALF_MOVES_BEHIND)     ->SetToolTip(nbr_half_moves_behind_help);

    FindWindow(ID_PEEK_AT_COMPLETE_POSITION) ->SetHelpText(peek_at_complete_position_help);
    FindWindow(ID_PEEK_AT_COMPLETE_POSITION) ->SetToolTip(peek_at_complete_position_help);

    FindWindow(ID_BLINDFOLD_HIDE_WHITE_PAWNS)->SetHelpText(blindfold_hide_white_pawns_help);
    FindWindow(ID_BLINDFOLD_HIDE_WHITE_PAWNS)->SetToolTip(blindfold_hide_white_pawns_help);

    FindWindow(ID_BLINDFOLD_HIDE_WHITE_PIECES)->SetHelpText(blindfold_hide_white_pieces_help);
    FindWindow(ID_BLINDFOLD_HIDE_WHITE_PIECES)->SetToolTip(blindfold_hide_white_pieces_help);

    FindWindow(ID_BLINDFOLD_HIDE_BLACK_PAWNS)->SetHelpText(blindfold_hide_black_pawns_help);
    FindWindow(ID_BLINDFOLD_HIDE_BLACK_PAWNS)->SetToolTip(blindfold_hide_black_pawns_help);

    FindWindow(ID_BLINDFOLD_HIDE_BLACK_PIECES)->SetHelpText(blindfold_hide_black_pieces_help);
    FindWindow(ID_BLINDFOLD_HIDE_BLACK_PIECES)->SetToolTip(blindfold_hide_black_pieces_help);

//    FindWindow(ID_BLINDFOLD_HIDE_BOARD)->SetHelpText(blindfold_hide_board_help);
//    FindWindow(ID_BLINDFOLD_HIDE_BOARD)->SetToolTip(blindfold_hide_board_help);
}

/*
// wxEVT_UPDATE_UI event handler for ID_CHECKBOX
void TrainingDialog::OnVoteUpdate( wxUpdateUIEvent& event )
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

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRAINING_RESET
void TrainingDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void TrainingDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to make life harder for yourself! The\n")
      wxT("payoff to this kind of training is improved ability to\n")
      wxT("visualise positions.\n\n")
      wxT("The tools available include a lag between the play and\n")
      wxT("the displayed board position and also the ability to\n")
      wxT("hide some or all of the pieces and pawns on the board.\n");

    wxMessageBox(helpText,
      wxT("Training Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}
