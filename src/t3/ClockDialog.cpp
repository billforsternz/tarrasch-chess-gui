/****************************************************************************
 * Custom dialog - Clocks
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "Appdefs.h"
#include "DebugPrintf.h"
#include "ClockDialog.h"

// ClockDialog type definition
IMPLEMENT_CLASS( ClockDialog, wxDialog )

// ClockDialog event table definition
BEGIN_EVENT_TABLE( ClockDialog, wxDialog )
 // EVT_UPDATE_UI( ID_VOTE, ClockDialog::OnVoteUpdate )
    EVT_BUTTON( ID_CLOCK_RESET, ClockDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, ClockDialog::OnHelpClick )
    EVT_CHECKBOX( ID_FIXED_PERIOD_MODE,  ClockDialog::OnFixedPeriodMode )
END_EVENT_TABLE()

// ClockDialog constructors
ClockDialog::ClockDialog( ClockConfig &dat, wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    this->dat = dat;
    Init();
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void ClockDialog::Init()
{
}

// Dialog create
bool ClockDialog::Create( wxWindow* parent,
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

// Control creation for ClockDialog
void ClockDialog::CreateControls()
{    
#if 0
    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
#else
    // A top-level sizer
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(box_sizer);
#endif
    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT("This panel lets you control the on screen chess clocks"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxLEFT|wxTOP, 20 );

//    // Spacer
//    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);


    // An intermediate sizer
    wxBoxSizer* horiz_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Current white clock" );
    wxSizer     *small_sizer = new wxStaticBoxSizer(box, wxVERTICAL);
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, "&Current black clock" );
    wxSizer     *small_sizer2 = new wxStaticBoxSizer(box2, wxVERTICAL);

//WHITE
    // Label for time
    wxStaticText* time_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Time (minutes)  (seconds):"), wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer->Add(time_label, 0, wxALIGN_LEFT|wxALL, 5);

    // Two spin controls for time
    wxBoxSizer* time_sizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxSpinCtrl* time_ctrl = new wxSpinCtrl ( this, ID_WHITE_TIME,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 90 );
    time_sizer1->Add(time_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxSpinCtrl* secs_ctrl = new wxSpinCtrl ( this, ID_WHITE_SECS,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 59, 0 );
    time_sizer1->Add(secs_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    small_sizer->Add(time_sizer1, 1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 0);

    // Label for increment
    wxStaticText* increment_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Increment (seconds):"), wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer->Add(increment_label, 0, wxALIGN_LEFT|wxALL, 5);

    // A spin control for increment
    wxSpinCtrl* increment_ctrl = new wxSpinCtrl ( this, ID_WHITE_INCREMENT,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 30 );
    small_sizer->Add(increment_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* visible_box = new wxCheckBox( this, ID_WHITE_VISIBLE,
       wxT("&Visible"), wxDefaultPosition, wxDefaultSize, 0 );
    visible_box->SetValue( dat.m_white_visible );
    small_sizer->Add( visible_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* running_box = new wxCheckBox( this, ID_WHITE_RUNNING,
       wxT("&Running"), wxDefaultPosition, wxDefaultSize, 0 );
    running_box->SetValue( dat.m_white_running );
    small_sizer->Add( running_box, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    horiz_sizer->Add(small_sizer, 1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 10);
    //box_sizer->Add(small_sizer, 0, wxGROW | (wxALL/* & ~wxLEFT */), 10);

//BLACK
    // Label for time
    wxStaticText* time_label2 = new wxStaticText ( this, wxID_STATIC,
        wxT("&Time (minutes)  (seconds):"), wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer2->Add(time_label2, 0, wxALIGN_LEFT|wxALL, 5);

    // Two spin controls for time
    wxBoxSizer* time_sizer2 = new wxBoxSizer(wxHORIZONTAL);
    wxSpinCtrl* time_ctrl2 = new wxSpinCtrl ( this, ID_BLACK_TIME,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 90 );
    time_sizer2->Add(time_ctrl2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxSpinCtrl* secs_ctrl2 = new wxSpinCtrl ( this, ID_BLACK_SECS,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 59, 0 );
    time_sizer2->Add(secs_ctrl2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    small_sizer2->Add(time_sizer2, 1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 0);

    // Label for increment
    wxStaticText* increment_label2 = new wxStaticText ( this, wxID_STATIC,
        wxT("&Increment (seconds):"), wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer2->Add(increment_label2, 0, wxALIGN_LEFT|wxALL, 5);

    // A spin control for increment
    wxSpinCtrl* increment_ctrl2 = new wxSpinCtrl ( this, ID_BLACK_INCREMENT,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 30 );
    small_sizer2->Add(increment_ctrl2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* visible_box2 = new wxCheckBox( this, ID_BLACK_VISIBLE,
       wxT("&Visible"), wxDefaultPosition, wxDefaultSize, 0 );
    visible_box2->SetValue( dat.m_black_visible );
    small_sizer2->Add( visible_box2, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* running_box2 = new wxCheckBox( this, ID_BLACK_RUNNING,
       wxT("&Running"), wxDefaultPosition, wxDefaultSize, 0 );
    running_box2->SetValue( dat.m_black_running );
    small_sizer2->Add( running_box2, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    horiz_sizer->Add(small_sizer2, 1, wxALIGN_RIGHT|wxGROW | (wxALL/* & ~wxLEFT */), 10);
    //box_sizer->Add(small_sizer2, 1, wxGROW | (wxALL/* & ~wxLEFT */), 10);
    box_sizer->Add(horiz_sizer, 1, wxGROW | (wxALL/* & ~wxLEFT */), 10);

    // An intermediate sizer
    wxBoxSizer* horiz_sizer2 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBox *box3 = new wxStaticBox(this, wxID_ANY, "&New game: human" );
    wxSizer     *small_sizer3 = new wxStaticBoxSizer(box3, wxVERTICAL);
    wxStaticBox *box4 = new wxStaticBox(this, wxID_ANY, "&New game: engine" );
    wxSizer     *small_sizer4 = new wxStaticBoxSizer(box4, wxVERTICAL);

//HUMAN
    // Label for time
    wxStaticText* time_label3 = new wxStaticText ( this, wxID_STATIC,
        wxT("&Time (minutes):"), wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer3->Add(time_label3, 0, wxALIGN_LEFT|wxALL, 5);

    // A spin control for time
    wxSpinCtrl* time_ctrl3 = new wxSpinCtrl ( this, ID_HUMAN_TIME,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 1, 120, 90 );
    small_sizer3->Add(time_ctrl3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Label for increment
    wxStaticText* increment_label3 = new wxStaticText ( this, wxID_STATIC,
        wxT("&Increment (seconds):"), wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer3->Add(increment_label3, 0, wxALIGN_LEFT|wxALL, 5);

    // A spin control for increment
    wxSpinCtrl* increment_ctrl3 = new wxSpinCtrl ( this, ID_HUMAN_INCREMENT,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 30 );
    small_sizer3->Add(increment_ctrl3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    human_visible_checkbox = new wxCheckBox( this, ID_HUMAN_VISIBLE,
       wxT("&Visible"), wxDefaultPosition, wxDefaultSize, 0 );
    human_visible_checkbox->SetValue( dat.m_human_visible );
    small_sizer3->Add( human_visible_checkbox, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    human_running_checkbox = new wxCheckBox( this, ID_HUMAN_RUNNING,
       wxT("&Running"), wxDefaultPosition, wxDefaultSize, 0 );
    human_running_checkbox->SetValue( dat.m_human_running );
    small_sizer3->Add( human_running_checkbox, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    horiz_sizer2->Add(small_sizer3, 1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 10);
    //box_sizer->Add(small_sizer, 0, wxGROW | (wxALL/* & ~wxLEFT */), 10);

//ENGINE
    // Label for time
    wxStaticText* time_label4 = new wxStaticText ( this, wxID_STATIC,
        wxT("&Time (minutes):"), wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer4->Add(time_label4, 0, wxALIGN_LEFT|wxALL, 5);

    // A spin control for time
    time_ctrl4 = new wxSpinCtrl ( this, ID_ENGINE_TIME,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 90 );
    small_sizer4->Add(time_ctrl4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Label for increment
    cprintf( "dat.m_fixed_period_mode = %s\n", dat.m_fixed_period_mode ? "true" : "false" );
    increment_label4 = new wxStaticText ( this, wxID_STATIC,
        dat.m_fixed_period_mode ? "Time (seconds):" : "Increment (seconds):", wxDefaultPosition, wxDefaultSize, 0 );
    small_sizer4->Add(increment_label4, 0, wxALIGN_LEFT|wxALL, 5);

    // A spin control for increment
    increment_ctrl4 = new wxSpinCtrl ( this, ID_ENGINE_INCREMENT,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 30 );
    small_sizer4->Add(increment_ctrl4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    engine_visible_checkbox = new wxCheckBox( this, ID_ENGINE_VISIBLE,
       wxT("&Visible"), wxDefaultPosition, wxDefaultSize, 0 );
    engine_visible_checkbox->SetValue( dat.m_engine_visible );
    small_sizer4->Add( engine_visible_checkbox, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    engine_running_checkbox = new wxCheckBox( this, ID_ENGINE_RUNNING,
       wxT("&Running"), wxDefaultPosition, wxDefaultSize, 0 );
    engine_running_checkbox->SetValue( dat.m_engine_running );
    small_sizer4->Add( engine_running_checkbox, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    horiz_sizer2->Add(small_sizer4, 1, wxALIGN_RIGHT|wxGROW | (wxALL/* & ~wxLEFT */), 10);
    //box_sizer->Add(small_sizer2, 1, wxGROW | (wxALL/* & ~wxLEFT */), 10);
    box_sizer->Add(horiz_sizer2, 1, wxGROW | (wxALL/* & ~wxLEFT */), 10);

    // An intermediate sizer
    wxBoxSizer* horiz_sizer3 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBox *box5 = new wxStaticBox(this, wxID_ANY, "Fixed period mode" );
    wxSizer     *small_sizer5 = new wxStaticBoxSizer(box5, wxVERTICAL);

    fixed_period_mode = new wxCheckBox( this, ID_FIXED_PERIOD_MODE,
       wxT("Engine gets fixed time per move, human not timed"), wxDefaultPosition, wxDefaultSize, 0 );
    fixed_period_mode->SetValue( dat.m_fixed_period_mode );
    small_sizer5->Add( fixed_period_mode, 0,
        /*wxALIGN_CENTER_VERTICAL|*/wxALL, 5);
    horiz_sizer3->Add(small_sizer5, 1, wxALIGN_RIGHT/*|wxGROW */| (wxALL/* & ~wxLEFT */), 10);
    box_sizer->Add(horiz_sizer3, 0, wxGROW |  (wxALL/* & ~wxLEFT */), 10);

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
/*
    // The Reset button
    wxButton* reset = new wxButton( this, ID_CLOCK_RESET, wxT("&Reset"),
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
void ClockDialog::SetDialogValidators()
{
/*  wxArrayString time_chars;
    time_chars.Add( "0" );
    time_chars.Add( "1" );
    time_chars.Add( "2" );
    time_chars.Add( "3" );
    time_chars.Add( "4" );
    time_chars.Add( "5" );
    time_chars.Add( "6" );
    time_chars.Add( "7" );
    time_chars.Add( "8" );
    time_chars.Add( "9" );
    time_chars.Add( "." );
    wxTextValidator val( wxFILTER_INCLUDE_CHAR_LIST, &dat.m_time );
    val.SetIncludes( time_chars );
    FindWindow(ID_WHITE_TIME)->SetValidator( val );
    wxArrayString inc_chars;
    inc_chars.Add( "0" );
    inc_chars.Add( "1" );
    inc_chars.Add( "2" );
    inc_chars.Add( "3" );
    inc_chars.Add( "4" );
    inc_chars.Add( "5" );
    inc_chars.Add( "6" );
    inc_chars.Add( "7" );
    inc_chars.Add( "8" );
    inc_chars.Add( "9" );
    wxTextValidator val2( wxFILTER_INCLUDE_CHAR_LIST, &dat.m_increment );
    val2.SetIncludes( inc_chars );
    FindWindow(ID_WHITE_INCREMENT)->SetValidator( val2 ); */

    FindWindow(ID_WHITE_TIME)->SetValidator(
        wxGenericValidator(& dat.m_white_time));
    FindWindow(ID_WHITE_SECS)->SetValidator(
        wxGenericValidator(& dat.m_white_secs));
    FindWindow(ID_WHITE_INCREMENT)->SetValidator(
        wxGenericValidator(& dat.m_white_increment));
    FindWindow(ID_WHITE_VISIBLE)->SetValidator(
        wxGenericValidator(& dat.m_white_visible));
    FindWindow(ID_WHITE_RUNNING)->SetValidator(
        wxGenericValidator(& dat.m_white_running));
    FindWindow(ID_BLACK_TIME)->SetValidator(
        wxGenericValidator(& dat.m_black_time));
    FindWindow(ID_BLACK_SECS)->SetValidator(
        wxGenericValidator(& dat.m_black_secs));
    FindWindow(ID_BLACK_INCREMENT)->SetValidator(
        wxGenericValidator(& dat.m_black_increment));
    FindWindow(ID_BLACK_VISIBLE)->SetValidator(
        wxGenericValidator(& dat.m_black_visible));
    FindWindow(ID_BLACK_RUNNING)->SetValidator(
        wxGenericValidator(& dat.m_black_running));
    FindWindow(ID_HUMAN_TIME)->SetValidator(
        wxGenericValidator(& dat.m_human_time));
    FindWindow(ID_HUMAN_INCREMENT)->SetValidator(
        wxGenericValidator(& dat.m_human_increment));
    FindWindow(ID_HUMAN_VISIBLE)->SetValidator(
        wxGenericValidator(& dat.m_human_visible));
    FindWindow(ID_HUMAN_RUNNING)->SetValidator(
        wxGenericValidator(& dat.m_human_running));
    FindWindow(ID_ENGINE_TIME)->SetValidator(
        wxGenericValidator(& dat.m_engine_minutes));
    FindWindow(ID_ENGINE_INCREMENT)->SetValidator(
        wxGenericValidator(& dat.m_engine_seconds));
    FindWindow(ID_ENGINE_VISIBLE)->SetValidator(
        wxGenericValidator(& dat.m_engine_visible));
    FindWindow(ID_ENGINE_RUNNING)->SetValidator(
        wxGenericValidator(& dat.m_engine_running));
    FindWindow(ID_FIXED_PERIOD_MODE)->SetValidator(
        wxGenericValidator(& dat.m_fixed_period_mode));
}

// Sets the help text for the dialog controls
void ClockDialog::SetDialogHelp()
{
    wxString time_help      = wxT("Enter the game time in minutes.");
    wxString increment_help = wxT("Enter the increment in seconds.");
    wxString visible_help   = wxT("Check this if the clock is visible.");
    wxString running_help   = wxT("Check this if the clock is running.");
    wxString fixed_period_mode_help   = wxT("Check this to make the engine move after a fixed time interval. Adjust time interval using engine time(mins) and increment(secs)");

    FindWindow(ID_WHITE_TIME)->SetHelpText(time_help);
    FindWindow(ID_WHITE_TIME)->SetToolTip(time_help);

    FindWindow(ID_WHITE_INCREMENT)->SetHelpText(increment_help);
    FindWindow(ID_WHITE_INCREMENT)->SetToolTip(increment_help);

    FindWindow(ID_WHITE_VISIBLE)->SetHelpText(visible_help);
    FindWindow(ID_WHITE_VISIBLE)->SetToolTip(visible_help);

    FindWindow(ID_WHITE_RUNNING)->SetHelpText(running_help);
    FindWindow(ID_WHITE_RUNNING)->SetToolTip(running_help);

    FindWindow(ID_BLACK_TIME)->SetHelpText(time_help);
    FindWindow(ID_BLACK_TIME)->SetToolTip(time_help);

    FindWindow(ID_BLACK_INCREMENT)->SetHelpText(increment_help);
    FindWindow(ID_BLACK_INCREMENT)->SetToolTip(increment_help);

    FindWindow(ID_BLACK_VISIBLE)->SetHelpText(visible_help);
    FindWindow(ID_BLACK_VISIBLE)->SetToolTip(visible_help);

    FindWindow(ID_BLACK_RUNNING)->SetHelpText(running_help);
    FindWindow(ID_BLACK_RUNNING)->SetToolTip(running_help);

    FindWindow(ID_HUMAN_TIME)->SetHelpText(time_help);
    FindWindow(ID_HUMAN_TIME)->SetToolTip(time_help);

    FindWindow(ID_HUMAN_INCREMENT)->SetHelpText(increment_help);
    FindWindow(ID_HUMAN_INCREMENT)->SetToolTip(increment_help);

    FindWindow(ID_HUMAN_VISIBLE)->SetHelpText(visible_help);
    FindWindow(ID_HUMAN_VISIBLE)->SetToolTip(visible_help);

    FindWindow(ID_HUMAN_RUNNING)->SetHelpText(running_help);
    FindWindow(ID_HUMAN_RUNNING)->SetToolTip(running_help);

    FindWindow(ID_ENGINE_TIME)->SetHelpText(time_help);
    FindWindow(ID_ENGINE_TIME)->SetToolTip(time_help);

    FindWindow(ID_ENGINE_INCREMENT)->SetHelpText(increment_help);
    FindWindow(ID_ENGINE_INCREMENT)->SetToolTip(increment_help);

    FindWindow(ID_ENGINE_VISIBLE)->SetHelpText(visible_help);
    FindWindow(ID_ENGINE_VISIBLE)->SetToolTip(visible_help);

    FindWindow(ID_ENGINE_RUNNING)->SetHelpText(running_help);
    FindWindow(ID_ENGINE_RUNNING)->SetToolTip(running_help);

    FindWindow(ID_FIXED_PERIOD_MODE)->SetHelpText(fixed_period_mode_help);
    FindWindow(ID_FIXED_PERIOD_MODE)->SetToolTip(fixed_period_mode_help);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLOCK_RESET
void ClockDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void ClockDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Specify how you want the on screen clocks to work.")
      wxT("\n\n")
      wxT("You can make instantaneously effective changes to ")
      wxT("the clock settings. One application of that feature ")
      wxT("is to start the clocks running in the opening ")
      wxT("position and use the program as a combined ")
      wxT("electronic board and clock for a normal chess game ")
      wxT("with a friend.")
      wxT("\n\n")
      wxT("You can also change how the clocks are setup ")
      wxT("each time a new human versus engine game starts. A ")
      wxT("fixed period mode option is now available. This specifies a mode where the human is ")
      wxT("not timed and the engine receives a fixed amount of time per ")
      wxT("move. Specify the number of seconds the engine gets in this ")
      wxT("mode with the engine time (minutes, probably zero is best!) and engine increment (seconds) parameters.")
      wxT("\n\n")
      wxT("A shortcut to this dialog is available; simply ")
      wxT("click on the clocks.\n");

    wxMessageBox(helpText,
      wxT("Chess Clock Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

void ClockDialog::OnFixedPeriodMode( wxCommandEvent& WXUNUSED(event) )
{
    bool fpm = fixed_period_mode->GetValue();
    dat.m_engine_minutes = fpm ? dat.m_engine_fixed_minutes : dat.m_engine_time;
    dat.m_engine_seconds = fpm ? dat.m_engine_fixed_seconds : dat.m_engine_increment;
    time_ctrl4->SetValue(dat.m_engine_minutes);
    increment_ctrl4->SetValue(dat.m_engine_seconds);
    if( fpm )
    {
        increment_label4->SetLabel( "Time (seconds):" );
        human_running_checkbox->SetValue( false );
        human_visible_checkbox->SetValue( false );
        engine_running_checkbox->SetValue( true );
        engine_visible_checkbox->SetValue( true );
    }
    else
    {
        increment_label4->SetLabel( "Increment (seconds):" );
        human_running_checkbox->SetValue( dat.m_human_running );
        human_visible_checkbox->SetValue( dat.m_human_visible );
        engine_running_checkbox->SetValue( dat.m_engine_running );
        engine_visible_checkbox->SetValue( dat.m_engine_visible );
    }
}
