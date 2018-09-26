/****************************************************************************
 * Custom dialog - General options
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/msgdlg.h"
#include "wx/statline.h"
#include "wx/clrpicker.h"
#include "Appdefs.h"
#include "DebugPrintf.h"
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
    EVT_BUTTON   ( ID_RESTORE_LIGHT, GeneralDialog::OnRestoreLight )
    EVT_BUTTON   ( ID_RESTORE_DARK , GeneralDialog::OnRestoreDark  )
    EVT_BUTTON   ( ID_RESTORE_HIGHLIGHT_LIGHT, GeneralDialog::OnRestoreHighlightLight )
    EVT_BUTTON   ( ID_RESTORE_HIGHLIGHT_DARK , GeneralDialog::OnRestoreHighlightDark  )
    EVT_BUTTON   ( ID_RESTORE_HIGHLIGHT_LINE, GeneralDialog::OnRestoreHighlightLine )
    EVT_COMBOBOX ( ID_NOTATION_LANGUAGE,  GeneralDialog::OnNotationLanguage )
	EVT_COLOURPICKER_CHANGED( wxID_ANY, GeneralDialog::OnColourPicker )
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

    // Colour Pickers
    wxBoxSizer* colour_sizer1  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* colour_label1 = new wxStaticText ( this, wxID_STATIC,
        gbl_spelling_us ? "Light Square Color" : "Light Square Colour", wxDefaultPosition, wxDefaultSize, 0 );
	wxColour light(dat.m_light_colour_r,dat.m_light_colour_g,dat.m_light_colour_b);
    light_picker = new wxColourPickerCtrl( this, wxID_ANY, light );
    colour_sizer1->Add(colour_label1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    colour_sizer1->Add(10, 5, 1, wxALL, 0);
    colour_sizer1->Add(light_picker,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxButton *restore_light = new wxButton ( this, ID_RESTORE_LIGHT, "Restore Default",
        wxDefaultPosition, wxDefaultSize, 0 );
    colour_sizer1->Add(restore_light, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	box_sizer->Add(colour_sizer1, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* colour_sizer2  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* colour_label2 = new wxStaticText ( this, wxID_STATIC,
        gbl_spelling_us ? "Dark Square Color" : "Dark Square Colour", wxDefaultPosition, wxDefaultSize, 0 );
	wxColour dark(dat.m_dark_colour_r,dat.m_dark_colour_g,dat.m_dark_colour_b);
    dark_picker = new wxColourPickerCtrl( this, wxID_ANY, dark );
    colour_sizer2->Add(colour_label2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    colour_sizer2->Add(10, 5, 1, wxALL, 0);
    colour_sizer2->Add(dark_picker,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxButton *restore_dark = new wxButton ( this, ID_RESTORE_DARK, "Restore Default",
        wxDefaultPosition, wxDefaultSize, 0 );
    colour_sizer2->Add(restore_dark, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    box_sizer->Add(colour_sizer2, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* colour_sizer3  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* colour_label3 = new wxStaticText ( this, wxID_STATIC,
        gbl_spelling_us ? "Highlighted Square Light Color" : "Highlighted Square Light Colour", wxDefaultPosition, wxDefaultSize, 0 );
	wxColour highlight_light(dat.m_highlight_light_colour_r,dat.m_highlight_light_colour_g,dat.m_highlight_light_colour_b);
    highlight_light_picker = new wxColourPickerCtrl( this, wxID_ANY, highlight_light );
    colour_sizer3->Add(colour_label3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    colour_sizer3->Add(10, 5, 1, wxALL, 0);
    colour_sizer3->Add(highlight_light_picker,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxButton *restore_highlight_light = new wxButton ( this, ID_RESTORE_HIGHLIGHT_LIGHT, "Restore Default",
        wxDefaultPosition, wxDefaultSize, 0 );
    colour_sizer3->Add(restore_highlight_light, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    box_sizer->Add(colour_sizer3, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* colour_sizer4  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* colour_label4 = new wxStaticText ( this, wxID_STATIC,
        gbl_spelling_us ? "Highlighted Square Dark Color" : "Highlighted Square Dark Colour", wxDefaultPosition, wxDefaultSize, 0 );
	wxColour highlight_dark(dat.m_highlight_dark_colour_r,dat.m_highlight_dark_colour_g,dat.m_highlight_dark_colour_b);
    highlight_dark_picker = new wxColourPickerCtrl( this, wxID_ANY, highlight_dark );
    colour_sizer4->Add(colour_label4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    colour_sizer4->Add(10, 5, 1, wxALL, 0);
    colour_sizer4->Add(highlight_dark_picker,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxButton *restore_highlight_dark = new wxButton ( this, ID_RESTORE_HIGHLIGHT_DARK, "Restore Default",
        wxDefaultPosition, wxDefaultSize, 0 );
    colour_sizer4->Add(restore_highlight_dark, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    box_sizer->Add(colour_sizer4, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* colour_sizer5  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* colour_label5 = new wxStaticText ( this, wxID_STATIC,
        gbl_spelling_us ? "Highlighted Square Border Color" : "Highlighted Square Border Colour", wxDefaultPosition, wxDefaultSize, 0 );
	wxColour highlight_line(dat.m_highlight_line_colour_r,dat.m_highlight_line_colour_g,dat.m_highlight_line_colour_b);
    highlight_line_picker = new wxColourPickerCtrl( this, wxID_ANY, highlight_line );
    colour_sizer5->Add(colour_label5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    colour_sizer5->Add(10, 5, 1, wxALL, 0);
    colour_sizer5->Add(highlight_line_picker,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    wxButton *restore_highlight_line = new wxButton ( this, ID_RESTORE_HIGHLIGHT_LINE, "Restore Default",
        wxDefaultPosition, wxDefaultSize, 0 );
    colour_sizer5->Add(restore_highlight_line, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    box_sizer->Add(colour_sizer5, 0, wxALIGN_LEFT|wxALL, 5);

    // Suppress highlight of last move
    wxCheckBox* suppress_highlight = new wxCheckBox( this, ID_SUPPRESS_HIGHLIGHT,
       "Don't highlight the most recent move on the chess board", wxDefaultPosition, wxDefaultSize, 0 );
    suppress_highlight->SetValue( dat.m_suppress_highlight );
    box_sizer->Add( suppress_highlight, 0,
        wxALL, 5);

    // Use line highlighting rather than colour highlighting
    wxCheckBox* line_highlight = new wxCheckBox( this, ID_LINE_HIGHLIGHT,
       gbl_spelling_us ?
		"Use borders rather than colors for square highlights":
		"Use borders rather than colours for square highlights", wxDefaultPosition, wxDefaultSize, 0 );
    line_highlight->SetValue( dat.m_line_highlight );
    box_sizer->Add( line_highlight, 0,
        wxALL, 5);

    // Show heading above board
    wxCheckBox* heading_above_board_box = new wxCheckBox( this, ID_HEADING_ABOVE_BOARD,
       wxT("Show heading above board (instead of in frame)"), wxDefaultPosition, wxDefaultSize, 0 );
    heading_above_board_box->SetValue( dat.m_heading_above_board );
    box_sizer->Add( heading_above_board_box, 0,
        wxALL, 5);

    // Font size
	wxBoxSizer* font_size_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* font_size_label = new wxStaticText(this, wxID_STATIC,
		"Font size in points (default is 9)", wxDefaultPosition, wxDefaultSize, 0);
	wxSpinCtrl *font_size_ctrl = new wxSpinCtrl(this, ID_LARGE_FONT,
		wxEmptyString, wxDefaultPosition, wxSize(50, wxDefaultCoord), //wxDefaultSize, 
		wxSP_ARROW_KEYS, 4, 100, dat.m_font_size );
	font_size_sizer->Add(font_size_ctrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
	font_size_sizer->Add(10, 0, 0, wxALL, 0);
	font_size_sizer->Add(font_size_label, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
    box_sizer->Add( font_size_sizer, 0,
        wxALL, 5);

    // Don't use italics for subvariations
    wxCheckBox* no_italics_box = new wxCheckBox( this, ID_NO_ITALICS,
       wxT("Don't use italics for subvariations"), wxDefaultPosition, wxDefaultSize, 0 );
    no_italics_box->SetValue( dat.m_no_italics );
    box_sizer->Add( no_italics_box, 0,
        wxALL, 5);

    // Go straight to game for single game pgns
    wxCheckBox* straight_to_game_box = new wxCheckBox( this, ID_STRAIGHT_TO_GAME,
       wxT("Go straight to game for single game .pgn files"), wxDefaultPosition, wxDefaultSize, 0 );
    straight_to_game_box->SetValue( dat.m_straight_to_game );
    box_sizer->Add( straight_to_game_box, 0,
        wxALL, 5);

    // Go straight to game for all pgns
    wxCheckBox* straight_to_first_game_box = new wxCheckBox( this, ID_STRAIGHT_TO_FIRST_GAME,
       wxT("Go straight to first game for all .pgn files"), wxDefaultPosition, wxDefaultSize, 0 );
    straight_to_first_game_box->SetValue( dat.m_straight_to_first_game );
    box_sizer->Add( straight_to_first_game_box, 0,
        wxALL, 5);

    // No auto flip at start of game or on swap sides
    wxCheckBox* no_auto_flip = new wxCheckBox( this, ID_NO_AUTO_FLIP,
       wxT("Don't flip board to put human at bottom"), wxDefaultPosition, wxDefaultSize, 0 );
    no_auto_flip->SetValue( dat.m_no_auto_flip );
    box_sizer->Add( no_auto_flip, 0,
        wxALL, 5);

    // Emit bell sound when engine moves
    wxCheckBox* emit_bell_sound_when_engine_moves = new wxCheckBox( this, ID_EMIT_BELL,
       wxT("Emit bell sound when engine moves"), wxDefaultPosition, wxDefaultSize, 0 );
    emit_bell_sound_when_engine_moves->SetValue( dat.m_bell );
    box_sizer->Add( emit_bell_sound_when_engine_moves, 0,
        wxALL, 5);

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
    FindWindow(ID_LARGE_FONT)->SetValidator(
        wxGenericValidator(& dat.m_font_size));
    FindWindow(ID_HEADING_ABOVE_BOARD)->SetValidator(
        wxGenericValidator(& dat.m_heading_above_board));
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
    FindWindow(ID_SUPPRESS_HIGHLIGHT)->SetValidator(
        wxGenericValidator(& dat.m_suppress_highlight));
    FindWindow(ID_LINE_HIGHLIGHT)->SetValidator(
        wxGenericValidator(& dat.m_line_highlight));
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
    wxString help4 = "Set this to move board heading into frame (particularly useful for small screen setups)";
    FindWindow(ID_HEADING_ABOVE_BOARD)->SetHelpText(help4);
    FindWindow(ID_HEADING_ABOVE_BOARD)->SetToolTip(help4);
    wxString help5 = "Use this to change the font size in the moves window";
    FindWindow(ID_LARGE_FONT)->SetHelpText(help5);
    FindWindow(ID_LARGE_FONT)->SetToolTip(help5);
    wxString help6 = "Set this if you don't want the board to automatically flip around when you start a game as black";
    FindWindow(ID_NO_AUTO_FLIP)->SetHelpText(help6);
    FindWindow(ID_NO_AUTO_FLIP)->SetToolTip(help6);
    wxString help7 = "Set this if you want the engine to emit a bell sound when it moves in human versus engine games";
    FindWindow(ID_EMIT_BELL)->SetHelpText(help7);
    FindWindow(ID_EMIT_BELL)->SetToolTip(help7);
    wxString help8 = "Set this if you don't want the last move to be highlighted on the chess board";
    FindWindow(ID_SUPPRESS_HIGHLIGHT)->SetHelpText(help8);
    FindWindow(ID_SUPPRESS_HIGHLIGHT)->SetToolTip(help8);
    wxString help9 = gbl_spelling_us ? "Set this if you prefer squares highlighted with a line rather than by color" : "Set this if you prefer squares highlighted with a line rather than by colour";
    FindWindow(ID_LINE_HIGHLIGHT)->SetHelpText(help9);
    FindWindow(ID_LINE_HIGHLIGHT)->SetToolTip(help9);
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
	wxColour light = light_picker->GetColour();
	dat.m_light_colour_r = light.Red();
	dat.m_light_colour_g = light.Green();
	dat.m_light_colour_b = light.Blue();
	wxColour dark  = dark_picker->GetColour();
	dat.m_dark_colour_r = dark.Red();
	dat.m_dark_colour_g = dark.Green();
	dat.m_dark_colour_b = dark.Blue();
	wxColour highlight_light = highlight_light_picker->GetColour();
	dat.m_highlight_light_colour_r = highlight_light.Red();
	dat.m_highlight_light_colour_g = highlight_light.Green();
	dat.m_highlight_light_colour_b = highlight_light.Blue();
	wxColour highlight_dark  = highlight_dark_picker->GetColour();
	dat.m_highlight_dark_colour_r = highlight_dark.Red();
	dat.m_highlight_dark_colour_g = highlight_dark.Green();
	dat.m_highlight_dark_colour_b = highlight_dark.Blue();
	wxColour highlight_line  = highlight_line_picker->GetColour();
	dat.m_highlight_line_colour_r = highlight_line.Red();
	dat.m_highlight_line_colour_g = highlight_line.Green();
	dat.m_highlight_line_colour_b = highlight_line.Blue();
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

// Note that the following colour constants violate DRY (Don't Repeat Yourself) - search project
//  for "violate DRY" to find out why
void GeneralDialog::OnRestoreLight( wxCommandEvent& WXUNUSED(event) )
{
	wxColour light(255,226,179);
	light_picker->SetColour( light );
}

void GeneralDialog::OnRestoreDark( wxCommandEvent& WXUNUSED(event) )
{
	wxColour dark(220,162,116);
	dark_picker->SetColour( dark );
}

void GeneralDialog::OnRestoreHighlightLight( wxCommandEvent& WXUNUSED(event) )
{
	wxColour highlight_light(250,240,160);
	highlight_light_picker->SetColour( highlight_light );
}

void GeneralDialog::OnRestoreHighlightDark( wxCommandEvent& WXUNUSED(event) )
{
	wxColour highlight_dark(196,160,130);
	highlight_dark_picker->SetColour( highlight_dark );
}

void GeneralDialog::OnRestoreHighlightLine( wxCommandEvent& WXUNUSED(event) )
{
	wxColour highlight_line(112,146,190);
	highlight_line_picker->SetColour( highlight_line );
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

void GeneralDialog::OnColourPicker( wxColourPickerEvent& WXUNUSED(event) )
{
	cprintf( "GeneralDialog::OnColourPicker()\n" );
}
