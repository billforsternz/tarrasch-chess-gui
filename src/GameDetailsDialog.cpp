/****************************************************************************
 * Custom dialog - Game Details
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <time.h>
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "Appdefs.h"
#include "Eco.h"
#include "Objects.h"
#include "GameLogic.h"
#include "GameDetailsDialog.h"

// GameDetailsDialog type definition
IMPLEMENT_CLASS( GameDetailsDialog, wxDialog )

// GameDetailsDialog event table definition
BEGIN_EVENT_TABLE( GameDetailsDialog, wxDialog )
    EVT_BUTTON( ID_GAME_DETAILS_RESET, GameDetailsDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, GameDetailsDialog::OnHelpClick )
    EVT_CHILD_FOCUS( GameDetailsDialog::OnChildFocus )
    EVT_BUTTON( wxID_OK, GameDetailsDialog::OnOkClick )
END_EVENT_TABLE()

// GameDetailsDialog constructors
GameDetailsDialog::GameDetailsDialog()
{
    Init();
}

GameDetailsDialog::GameDetailsDialog( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void GameDetailsDialog::Init()
{
    white       = "";               // White
    black       = "";               // Black
    event       = "";               // Event
    site        = "";               // Site
    date        = "";               // Date
    round       = "";               // Round
    board_nbr   = "";               // Board
    result      = "";               // Result
    white_elo   = "";               // WhiteElo
    black_elo   = "";               // BlackElo
    previous_child_window = NULL;
}

// Create dialog
bool GameDetailsDialog::Create( wxWindow* parent,
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

// Control creation for GameDetailsDialog
void GameDetailsDialog::CreateControls()
{

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);

    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, SMALL);

//    // A friendly message
//    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
//        wxT("Game Details."), wxDefaultPosition, wxDefaultSize, 0 );
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
    #define WIDTH 200
    wxSize sz=wxSize(WIDTH,wxDefaultCoord);

        // Label for white
        wxStaticText* white_label = new wxStaticText ( this, wxID_STATIC,
            wxT("White "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(white_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for white
        white_ctrl = new wxTextCtrl ( this, ID_WHITE_PLAYER_NAME,  wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(white_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for black
        wxStaticText* black_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Black "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(black_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for black
        black_ctrl = new wxTextCtrl ( this, ID_BLACK_PLAYER_NAME,   wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(black_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for event
        wxStaticText* event_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Event "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(event_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for event
        event_ctrl = new wxTextCtrl ( this, ID_EVENT,           wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(event_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for site
        wxStaticText* site_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Site "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(site_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for site
        site_ctrl = new wxTextCtrl ( this, ID_SITE,            wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(site_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for date
        wxStaticText* date_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Date "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(date_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for date
        date_ctrl = new wxTextCtrl ( this, ID_DATE,            wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(date_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for round
        wxStaticText* round_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Round "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(round_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for round
        round_ctrl = new wxTextCtrl ( this, ID_ROUND,           wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(round_ctrl, 0, wxGROW|wxALL, SMALL);
/*
        // Label for board nbr
        wxStaticText* board_nbr_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Board number "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(board_nbr_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for board nbr
        board_nbr_ctrl = new wxTextCtrl ( this, ID_BOARD_NBR,       wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(board_nbr_ctrl, 0, wxGROW|wxALL, SMALL);
  */
        // Label for result
        wxStaticText* result_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Result "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(result_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Combo box for result
        wxString choices[4];
        choices[0] = "1-0";
        choices[1] = "0-1";
        choices[2] = "1/2-1/2";
        choices[3] = "(no result)";
        result_ctrl = new wxComboBox ( this, ID_RESULT,  wxT(""), wxDefaultPosition, sz, 4, choices, wxCB_READONLY );
        HV1->Add(result_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for eco
        wxStaticText* eco_label = new wxStaticText ( this, wxID_STATIC,
            wxT("ECO code "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(eco_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for eco
        eco_ctrl = new wxTextCtrl ( this, ID_ECO,       wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(eco_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for WhiteElo
        #if 1
        wxStaticText* white_elo_label = new wxStaticText ( this, wxID_STATIC,
            wxT("White Elo "), wxDefaultPosition, wxDefaultSize, 0 );
        #else
        wxCheckBox* white_elo_label = new wxCheckBox( this, wxID_STATIC,
           wxT("White Elo "), wxDefaultPosition, wxDefaultSize, 0 );
        white_elo_label->SetValue( true );
        #endif
        HV1->Add(white_elo_label, 0, wxALIGN_LEFT|wxALL, SMALL);
        //small_sizer2->Add( visible_box2, 0,
        //    wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Text control for WhiteElo
        #if 1
        white_elo_ctrl = new wxTextCtrl ( this, ID_WHITE_ELO,       wxT(""), wxDefaultPosition, sz, 0 );
        // A spin control for WhiteElo
        #else
        wxSpinCtrl* white_elo_ctrl = new wxSpinCtrl ( this, ID_WHITE_ELO,
            wxEmptyString, wxDefaultPosition, wxSize(60, -1),
            wxSP_ARROW_KEYS, 0, 4000, 2000 );
        #endif
        HV1->Add(white_elo_ctrl, 0, wxGROW|wxALL, SMALL);

        // Label for BlackElo
        wxStaticText* black_elo_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Black Elo "), wxDefaultPosition, wxDefaultSize, 0 );
        HV1->Add(black_elo_label, 0, wxALIGN_LEFT|wxALL, SMALL);

        // Text control for BlackElo
        black_elo_ctrl = new wxTextCtrl ( this, ID_BLACK_ELO,       wxT(""), wxDefaultPosition, sz, 0 );
        HV1->Add(black_elo_ctrl, 0, wxGROW|wxALL, SMALL);

    //H1->Add(V1, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);
    //wxBoxSizer* V2 = new wxBoxSizer(wxVERTICAL);

    //H1->Add(V2, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);
    box_sizer->Add(HV1, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, SMALL);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
/*
    // The Reset button
    wxButton* reset = new wxButton( this, ID_PLAYER_RESET, wxT("&Reset"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(reset, 0, wxALIGN_CENTER_VERTICAL|wxALL, SMALL);
*/
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
void GameDetailsDialog::SetDialogValidators()
{
    FindWindow(ID_WHITE_PLAYER_NAME)->SetValidator(
        wxTextValidator(wxFILTER_NONE, &white));
    FindWindow(ID_BLACK_PLAYER_NAME)->SetValidator(
        wxTextValidator(wxFILTER_NONE, &black));
    FindWindow(ID_EVENT)->SetValidator(
        wxTextValidator(wxFILTER_NONE, &event));
    FindWindow(ID_SITE)->SetValidator(
        wxTextValidator(wxFILTER_NONE, &site));
    FindWindow(ID_DATE)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &date));
    FindWindow(ID_ROUND)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &round));
//    FindWindow(ID_BOARD_NBR)->SetValidator(
//        wxTextValidator(wxFILTER_ASCII, &board_nbr));
    FindWindow(ID_RESULT)->SetValidator(
        wxGenericValidator(&result));
    FindWindow(ID_ECO)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &eco));
    FindWindow(ID_WHITE_ELO)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &white_elo));
    FindWindow(ID_BLACK_ELO)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &black_elo));
}

// Sets the help text for the dialog controls
void GameDetailsDialog::SetDialogHelp()
{
    wxString white_help    = wxT("White's name.");
    wxString black_help    = wxT("Black's name.");

    FindWindow(ID_WHITE_PLAYER_NAME)->SetHelpText(white_help);
    FindWindow(ID_WHITE_PLAYER_NAME)->SetToolTip(white_help);

    FindWindow(ID_BLACK_PLAYER_NAME)->SetHelpText(black_help);
    FindWindow(ID_BLACK_PLAYER_NAME)->SetToolTip(black_help);
}

/*
// wxEVT_UPDATE_UI event handler for ID_CHECKBOX
void GameDetailsDialog::OnVoteUpdate( wxUpdateUIEvent& event )
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
void GameDetailsDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void GameDetailsDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to enter or edit supplementary details that\n")
      wxT("you want associated with this game as stored on file.\n");

    wxMessageBox(helpText,
      wxT("Game Details Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

void GameDetailsDialog::OnChildFocus( wxChildFocusEvent& evt )
{
    wxWindow *window = evt.GetWindow();
    bool leaving_white = (window!=previous_child_window && previous_child_window==white_ctrl);
    bool leaving_black = (window!=previous_child_window && previous_child_window==black_ctrl);
    previous_child_window = window;
    cprintf( "OnChildFocus() in: leaving_white=%s leaving_black=%s\n", leaving_white?"true":"false", leaving_black?"true":"false" );
    if( leaving_white )
    {
        std::string w = std::string(white_ctrl->GetValue());
        std::string we = std::string(white_elo_ctrl->GetValue());
        if( we=="" && lookup_elo.count(w)>0 )
            white_elo_ctrl->SetValue(lookup_elo[w].c_str());
    }
    if( leaving_black )
    {
        std::string b = std::string(black_ctrl->GetValue());
        std::string be = std::string(black_elo_ctrl->GetValue());
        if( be=="" && lookup_elo.count(b)>0 )
            black_elo_ctrl->SetValue(lookup_elo[b].c_str());
    }
    cprintf( "OnChildFocus() out\n" );
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GameDetailsDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    // Validate date
    wxString temp = date_ctrl->GetValue();
    temp.Trim(true);
    temp.Trim(false);
    const char *s = temp.c_str();
    bool ok=false;
    wxString err_msg = "Illegal date: valid format is yyyy.mm.dd, eg 1999.12.31";
    if( temp=="?" || strlen(s)==0 )
        ok = true;
    else if( strlen(s) == 4 )
    {
        int yyyy = atoi(&s[0]);
        bool yyyy_unknown = (s[0]=='?' && s[1]=='?' && s[2]=='?' && s[3]=='?');
        ok = yyyy_unknown || (1000<=yyyy && yyyy<=2999);
    }
    else if( 8<=strlen(s) && strlen(s)<=10 )
    {
        if( s[4]=='.' )
        {
            ok = true;
            int dd_offset = 8;
            if( s[6]=='.' && s[7]!='.' && strlen(s)<10 )
                dd_offset = 7;  // yyyy.m.d or yyyy.m.dd (len=8 or 9)
            else if( s[6]!='.' && s[7]=='.' && strlen(s)>8 )
                dd_offset = 8;  // yyyy.mm.d or yyyy.mm.dd (len=9 or 10)
            else
                ok = false;
            if( ok )
            {
                int yyyy = atoi(&s[0]);
                int mm   = atoi(&s[5]);
                int dd   = atoi(&s[dd_offset]);
                bool yyyy_unknown = (s[0]=='?' && s[1]=='?' && s[2]=='?' && s[3]=='?');
                bool mm_unknown   = (s[5]=='?' && (s[6]=='?'||s[6]=='.') );
                bool dd_unknown   = (s[dd_offset]=='?' && (s[dd_offset+1]=='?'||s[dd_offset+1]=='\0') );
                bool yyyy_ok = yyyy_unknown || (1000<=yyyy && yyyy<=2999);
                bool mm_ok   = mm_unknown   || (1<=mm && mm<=12);
                bool dd_ok   = dd_unknown   || (1<=dd && dd<=31);
                ok = (yyyy_ok && mm_ok && dd_ok);
                if( ok )
                {
                    if( mm == 2 )
                    {
                        bool leap=false;
                        if( yyyy%400 == 0 )
                            leap = true;            //  eg 1600,2000,2400 are leap (also yyyy_unknown as then yyyy==0)
                        else if( yyyy%100 == 0 )
                            leap = false;           //  eg 1700,1800,1900,2100 aren't leap
                        else if( yyyy%4 == 0 )
                            leap = true;            //  eg 1996,2004,2008,2012 are leap
                        ok = (dd <= (leap?29:28));  //  (also dd_unknown is okay as then dd==0)
                    }
                    else if( mm==4 || mm==6 || mm==9 || mm==11 )
                    {
                        ok = dd<=30;  // (also dd_unknown is okay as then dd==0)
                    }
                    if( !ok )
                        err_msg = "Illegal date: there aren't that many days in that month";
                }
            }
        }
    }

    // Validate round
    if( ok )
    {
        temp = round_ctrl->GetValue();
        temp.Trim(true);
        temp.Trim(false);
        if( temp!="?" && temp!='-' )  // both of these explicitly allowed
        {
            s = temp.c_str();
            if( *s )
            {
                int r = atoi(s);
                if( r <= 0 )
                    ok = false;
                while( ok && *s )
                {  // allow digits and '.' characters, '.' must be followed by a digit
                    if( *s!='.' && !isdigit(*s) )
                        ok = false;
                    if( *s=='.' && !isdigit(*(s+1)) )
                        ok = false;
                    s++;
                }
                if( !ok )
                    err_msg = "Illegal round";
            }
        }
    }
/*
    // Validate board number
    if( ok )
    {
        temp = board_nbr_ctrl->GetValue();
        temp.Trim(true);
        temp.Trim(false);
        s = temp.c_str();
        if( temp!="?" && *s )
        {
            int r = atoi(s);
            if( r <= 0 )
            {
                ok = false;
                err_msg = "Illegal board number";
            }
        }
    }
  */
    // Validate result

    // Validate ECO
    if( ok )
    {
        temp = eco_ctrl->GetValue();
        temp.Trim(true);
        temp.Trim(false);
        if( temp != "?" )
        {
            s = temp.c_str();
            ok = (strlen(s)==0);
            // Allow "" (empty) or "A00" (alpha,digit,digit)
            // But also allow say "A000" or "AA00" (possible future formats?)
            if( 3<=strlen(s) && strlen(s)<=4 )
            {
                ok = true;
                for( int i=0; ok && *s; i++ )
                {
                    int oki=0;
                    if( i==0 )
                        oki = isalpha(*s);
                    else if( i==1 )
                    {
                        if( strlen(s) == 4 )
                            oki = (isalpha(*s) || isdigit(*s));
                        else  // if( strlen(s) == 3 )
                            oki = isdigit(*s);
                    }
                    else
                        oki = isdigit(*s);
                    s++;
                    ok = (oki!=0);
                }
            }
            if( !ok )
                err_msg = "Illegal ECO code";
        }
    }

    // Validate white elo
    if( ok )
    {
        temp = white_elo_ctrl->GetValue();
        temp.Trim(true);
        temp.Trim(false);
        s = temp.c_str();
        if( temp!="?" && *s )
        {
            int r = atoi(s);
            if( r<=0 || r>10000 )
            {
                ok = false;
                err_msg = "Illegal white elo";
            }
        }
    }


    // Validate black elo
    if( ok )
    {
        temp = black_elo_ctrl->GetValue();
        temp.Trim(true);
        temp.Trim(false);
        s = temp.c_str();
        if( temp!="?" && *s )
        {
            int r = atoi(s);
            if( r<=0 || r>10000 )
            {
                ok = false;
                err_msg = "Illegal black elo";
            }
        }
    }
    if( ok )
        AcceptAndClose();
    else
        wxMessageBox( err_msg, "Error in game details", wxOK|wxICON_ERROR );
}

wxString GameDetailsDialog::remember_event;
wxString GameDetailsDialog::remember_site;
bool GameDetailsDialog::Run( GameDocument &gd, bool push_changes_to_tabs )
{
    bool ok=false;
    Roster before = gd.r;
    white     = gd.r.white;          // "White"
    black     = gd.r.black;          // "Black"
    event     = gd.r.event;          // "Event"
    site      = gd.r.site;           // "Site"
    date      = gd.r.date;           // "Date"
    round     = gd.r.round;          // "Round"
    result    = gd.r.result;         // "Result"
    eco       = gd.r.eco;            // "ECO"
    white_elo = gd.r.white_elo;      // "WhiteElo"
    black_elo = gd.r.black_elo;      // "BlackElo"
    if( result!="1-0" && result!="0-1" && result!="1/2-1/2" )
    {
        result = "(no result)";
        before.result="*";      // don't set game_details_edited unnecessarily
    }
    bool needs_calc_elo = (eco=="" || eco=="A00" || (eco.length()>0 && eco[0]=='?'));
    if( needs_calc_elo )
    {
        CompactGame pact;
        gd.GetCompactGame(pact);
        eco = eco_calculate( pact.moves );
        before.eco = eco;       // don't set game_details_edited unnecessarily
    }
    if( date=="" )
    {
        time_t rawtime;
        struct tm * timeinfo;
        time( &rawtime );
        timeinfo = localtime( &rawtime );
        char buf[20];
        sprintf( buf, "%04d.%02d.%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday );
        date = buf;
        before.date = date;    // don't set game_details_edited unnecessarily
    }
    if( event=="" )
    {
        event = remember_event;
        before.event = event; // don't set game_details_edited unnecessarily
    }
    if( site=="" )
    {
        site = remember_site;
        before.site = site;   // don't set game_details_edited unnecessarily
    }

    // Auto fill out elo field if possible
    GamesCache *gc = &objs.gl->gc_pgn;
    if( gc->pgn_filename != "" )
    {
        int nbr = gc->gds.size();
        if( nbr > 1000 )   // don't read huge pgn files in there entirety in order to auto-fill in elo field
            nbr=1000;
        void *context=NULL;
        cprintf( "Loading games into memory\n" );
        for( int i=0; i<nbr; i++ )
            context = gc->gds[i]->LoadIntoMemory( context, i+1 >= nbr );
        cprintf( "Building map begin\n" );
        lookup_elo.clear();
        for( int i=0; i<nbr; i++ )
        {
            const char *player = gc->gds[i]->White();
            const char *elo    = gc->gds[i]->WhiteElo();
            if( player && *player && elo && *elo )
                lookup_elo[std::string(player)] = std::string(elo);
            player = gc->gds[i]->Black();
            elo    = gc->gds[i]->BlackElo();
            if( player && *player && elo && *elo )
                lookup_elo[std::string(player)] = std::string(elo);
        }
        cprintf( "Building map end\n" );
    }
    if( wxID_OK == ShowModal() )
    {
        ok = true;
        result.Trim(true);
        result.Trim(false);
        white.Trim(true);
        white.Trim(false);
        black.Trim(true);
        black.Trim(false);
        event.Trim(true);
        event.Trim(false);
        site.Trim(true);
        site.Trim(false);
        date.Trim(true);
        date.Trim(false);
        round.Trim(true);
        round.Trim(false);
        eco.Trim(true);
        eco.Trim(false);
        white_elo.Trim(true);
        white_elo.Trim(false);
        black_elo.Trim(true);
        black_elo.Trim(false);
        if( result!="1-0" && result!="0-1" && result!="1/2-1/2" )
            result = "*";
        remember_event = event;
        remember_site = site;
        gd.r.white      = white;        // "White"
        gd.r.black      = black;        // "Black"
        gd.r.event      = event;        // "Event"
        gd.r.site       = site;         // "Site"
        const char *s = date.c_str();
        if( 8<=strlen(s) && strlen(s)<=9 )
        {
            if( s[4]=='.' )
            {
                bool date_ok = true;
                int dd_offset = 8;
                if( s[6]=='.' && s[7]!='.' )
                    dd_offset = 7;  // yyyy.m.d or yyyy.m.dd (len=8 or 9)
                else if( s[6]!='.' && s[7]=='.' && strlen(s)>8 )
                    dd_offset = 8;  // yyyy.mm.d (len=9)
                else
                    date_ok = false;
                if( date_ok )
                {
                    int yyyy = atoi(&s[0]);
                    int mm   = atoi(&s[5]);
                    int dd   = atoi(&s[dd_offset]);
                    char buf[40];
                    sprintf( buf, "%04d.%02d.%02d", yyyy, mm, dd );
                    if( yyyy==0 )
                        memcpy(buf,"????",4);
                    if( mm==0 )
                        memcpy(buf+5,"??",2);
                    if( dd==0 )
                        memcpy(buf+8,"??",2);
                    date = buf;
                }
            }
        }
        gd.r.date       = date;         // "Date"
        gd.r.round      = round;        // "Round"
        gd.r.result     = result;       // "Result"
        gd.r.eco        = eco;          // "ECO"
        gd.r.white_elo  = white_elo;    // "WhiteElo"
        gd.r.black_elo  = black_elo;    // "BlackElo"
        bool same = gd.r.white == before.white &&
                    gd.r.black == before.black &&
                    gd.r.event == before.event &&
                    gd.r.site == before.site &&
                    gd.r.result == before.result &&
                    gd.r.round == before.round &&
                    gd.r.date == before.date &&
                    gd.r.eco == before.eco &&
                    gd.r.white_elo == before.white_elo &&
                    gd.r.black_elo == before.black_elo &&
                    gd.r.fen == before.fen;
        gd.game_details_edited |= !same;

        // Push the changes out to the document immediately
        if( push_changes_to_tabs && gd.game_details_edited )
        {
            GameDocument *pd;
            Undo *pu;
            int handle = objs.tabs->Iterate(0,pd,pu);
            while( pd && pu )
            {
                if( gd.game_being_edited!=0 && (gd.game_being_edited == pd->game_being_edited)  )
                {
                    pd->r = gd.r;
                    pd->game_details_edited = gd.game_details_edited;
                    break;
                }
                objs.tabs->Iterate(handle,pd,pu);
            }
        }
    }
    return ok;
}
