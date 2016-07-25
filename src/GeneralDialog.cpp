/****************************************************************************
 * Custom dialog - General options
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/msgdlg.h"
#include "wx/statline.h"
#include "Appdefs.h"
#include "Lang.h"
#include "GeneralDialog.h"
using namespace std;
using namespace thc;

// GeneralDialog type definition
IMPLEMENT_CLASS( GeneralDialog, wxDialog )

// GeneralDialog event table definition
BEGIN_EVENT_TABLE( GeneralDialog, wxDialog )
    EVT_BUTTON   ( wxID_HELP, GeneralDialog::OnHelpClick )
    EVT_BUTTON   ( wxID_OK, GeneralDialog::OnOkClick )
    EVT_COMBOBOX ( ID_NOTATION_LANGUAGE,  GeneralDialog::OnNotationLanguage )
END_EVENT_TABLE()

// GeneralDialog constructor
GeneralDialog::GeneralDialog
(
    wxWindow* parent, GeneralConfig *dat_ptr,
    wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size, long style
)
{
    Init( dat_ptr );
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void GeneralDialog::Init( GeneralConfig *dat_ptr )
{
    dat = *dat_ptr;
/*  dat.m_notation_language  = "KQRNB (English)";
    dat.m_no_italics         = false;
    dat.m_straight_to_game   = false; */
}


// Dialog create
bool GeneralDialog::Create( wxWindow* parent,
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

// Control creation for GeneralDialog
void GeneralDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
  
    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT("This panel lets you setup some general options."), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

/*  // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5); */
  
    // Notation language
    wxBoxSizer* notation_language_sizer  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* notation_language_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Language used for notation (KQRNB):"), wxDefaultPosition, wxDefaultSize, 0 );

    labels.Clear();
    const char **pknown_lang = LangGetKnownArray();
    const char *user_defined = *pknown_lang++;  // user defined string comes first ...
    while( *pknown_lang )
        labels.Add( *pknown_lang++ );
    labels.Add( user_defined );     // ... but appears last
    combo_label = dat.m_notation_language;
    notation_language_ctrl = new wxComboBox( this, ID_NOTATION_LANGUAGE,
        combo_label, wxDefaultPosition,
        wxSize(200, wxDefaultCoord), labels, wxCB_DROPDOWN );

    notation_language_ctrl->SetValue(combo_label);
    notation_language_sizer->Add(notation_language_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    notation_language_sizer->Add(10, 5, 1, wxALL, 0);
    notation_language_sizer->Add(notation_language_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    box_sizer->Add(notation_language_sizer, 0, wxALIGN_LEFT|wxALL, 5);

    // Use small board graphics
    wxCheckBox* use_small_board_box = new wxCheckBox( this, ID_SMALL_BOARD,
       wxT("Use small chess board (requires restart)"), wxDefaultPosition, wxDefaultSize, 0 );
    use_small_board_box->SetValue( dat.m_small_board );
    box_sizer->Add( use_small_board_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Use large font for chess text
    wxCheckBox* use_large_font_box = new wxCheckBox( this, ID_LARGE_FONT,
       wxT("Use larger font in move window"), wxDefaultPosition, wxDefaultSize, 0 );
    use_large_font_box->SetValue( dat.m_large_font );
    box_sizer->Add( use_large_font_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Don't use italics for subvariations
    wxCheckBox* no_italics_box = new wxCheckBox( this, ID_NO_ITALICS,
       wxT("Don't use italics for subvariations"), wxDefaultPosition, wxDefaultSize, 0 );
    no_italics_box->SetValue( dat.m_no_italics );
    box_sizer->Add( no_italics_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Go straight to game for single game pgns
    wxCheckBox* straight_to_game_box = new wxCheckBox( this, ID_STRAIGHT_TO_GAME,
       wxT("Go straight to game for single game .pgn files"), wxDefaultPosition, wxDefaultSize, 0 );
    straight_to_game_box->SetValue( dat.m_straight_to_game );
    box_sizer->Add( straight_to_game_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Go straight to game for all pgns
    wxCheckBox* straight_to_first_game_box = new wxCheckBox( this, ID_STRAIGHT_TO_FIRST_GAME,
       wxT("Go straight to first game for all .pgn files"), wxDefaultPosition, wxDefaultSize, 0 );
    straight_to_first_game_box->SetValue( dat.m_straight_to_first_game );
    box_sizer->Add( straight_to_first_game_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // No auto flip at start of game or on swap sides
    wxCheckBox* no_auto_flip = new wxCheckBox( this, ID_NO_AUTO_FLIP,
       wxT("Don't flip board to put human at bottom"), wxDefaultPosition, wxDefaultSize, 0 );
    no_auto_flip->SetValue( dat.m_no_auto_flip );
    box_sizer->Add( no_auto_flip, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Emit bell sound when engine moves
    wxCheckBox* emit_bell_sound_when_engine_moves = new wxCheckBox( this, ID_EMIT_BELL,
       wxT("Emit bell sound when engine moves"), wxDefaultPosition, wxDefaultSize, 0 );
    emit_bell_sound_when_engine_moves->SetValue( dat.m_bell );
    box_sizer->Add( emit_bell_sound_when_engine_moves, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);

    // The OK button
    ok_button = new wxButton ( this, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(ok_button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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
void GeneralDialog::SetDialogValidators()
{
    //FindWindow(ID_NOTATION_LANGUAGE)->SetValidator(
    //    wxTextValidator(wxFILTER_ASCII, &dat.m_notation_language));
    FindWindow(ID_LARGE_FONT)->SetValidator(
        wxGenericValidator(& dat.m_large_font));
    FindWindow(ID_SMALL_BOARD)->SetValidator(
        wxGenericValidator(& dat.m_small_board));
    FindWindow(ID_NO_ITALICS)->SetValidator(
        wxGenericValidator(& dat.m_no_italics));
    FindWindow(ID_STRAIGHT_TO_GAME)->SetValidator(
        wxGenericValidator(& dat.m_straight_to_game));
    FindWindow(ID_STRAIGHT_TO_FIRST_GAME)->SetValidator(
        wxGenericValidator(& dat.m_straight_to_first_game));
    FindWindow(ID_EMIT_BELL)->SetValidator(
        wxGenericValidator(& dat.m_bell));
    FindWindow(ID_NO_AUTO_FLIP)->SetValidator(
        wxGenericValidator(& dat.m_no_auto_flip));
}

// Sets the help text for the dialog controls
void GeneralDialog::SetDialogHelp()
{
    wxString help1  = wxT("Set the algebraic notation characters used for king, queen, rook, knight and bishop respectively");
    FindWindow(ID_NOTATION_LANGUAGE)->SetHelpText(help1);
    FindWindow(ID_NOTATION_LANGUAGE)->SetToolTip(help1);
    wxString help2 = "Set this if you don't want subvariations to show as italic";
    FindWindow(ID_NO_ITALICS)->SetHelpText(help2);
    FindWindow(ID_NO_ITALICS)->SetToolTip(help2);
    wxString help3 = "Set this to skip the game selection dialog when a newly opened file has only one game";
    FindWindow(ID_STRAIGHT_TO_GAME)->SetHelpText(help3);
    FindWindow(ID_STRAIGHT_TO_GAME)->SetToolTip(help3);
    wxString help3b = "Set this to skip the game selection dialog and go straight to the first game";
    FindWindow(ID_STRAIGHT_TO_FIRST_GAME)->SetHelpText(help3b);
    FindWindow(ID_STRAIGHT_TO_FIRST_GAME)->SetToolTip(help3b);
    wxString help4 = "Set this to use small board graphics on a large screen (takes effect at next restart)";
    FindWindow(ID_SMALL_BOARD)->SetHelpText(help4);
    FindWindow(ID_SMALL_BOARD)->SetToolTip(help4);
    wxString help5 = "Set this to use a larger font in the moves window";
    FindWindow(ID_LARGE_FONT)->SetHelpText(help5);
    FindWindow(ID_LARGE_FONT)->SetToolTip(help5);
    wxString help6 = "Set this if you don't want the board to automatically flip around when you start a game as black";
    FindWindow(ID_NO_AUTO_FLIP)->SetHelpText(help6);
    FindWindow(ID_NO_AUTO_FLIP)->SetToolTip(help6);
    wxString help7 = "Set this if you want the engine to emit a bell sound when it moves in human versus engine games";
    FindWindow(ID_EMIT_BELL)->SetHelpText(help7);
    FindWindow(ID_EMIT_BELL)->SetToolTip(help7);
}


// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void GeneralDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString helpText =
      wxT("Use this panel to specify some general options, most importantly\n")
      wxT("the algebraic notation characters to use. Select from a major\n")
      wxT("European language, or define a custom string if your language\n")
      wxT("is not listed\n");

    wxMessageBox(helpText,
      wxT("General Settings Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}


// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GeneralDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString txt = this->notation_language_ctrl->GetValue();
    if( LangValidateString( txt ) )
    {
        dat.m_notation_language = txt;
        LangSet(txt);
        AcceptAndClose();
    }
    else
    {
        const char **pknown_lang = LangGetKnownArray();
        const char *user_defined = *pknown_lang++;  // user defined string comes first
        if( txt == user_defined )
            wxMessageBox( "Replace the '?' characters with your chosen characters for king, queen, rook, knight and bishop respectively", "Illegal notation string", wxOK|wxICON_ERROR );
        else
            wxMessageBox( "You must enter or select a string starting with your chosen characters for king, queen, rook, knight and bishop respectively", "Illegal notation string", wxOK|wxICON_ERROR );
    }
}

void GeneralDialog::OnNotationLanguage( wxCommandEvent& WXUNUSED(event) )
{
/*  wxString txt = this->notation_language_ctrl->GetValue();
    if( LangValidateString( txt ) )
        notation_language_ctrl->SetValue(txt);
    else
    {
        if( txt !=  "????? (User defined)" )
            wxMessageBox( "You must enter or select a string starting with the desired characters for king, queen, rook, knight and bishop respectively", "Illegal notation string", wxOK|wxICON_ERROR );
        notation_language_ctrl->SetValue(dat.m_notation_language);
    }
    ok_button->SetFocus(); */
}

