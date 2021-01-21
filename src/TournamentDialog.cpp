/****************************************************************************
* Custom dialog - Tournament round for PGN Skeleton
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <time.h>
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "Appdefs.h"
#include "Objects.h"
#include "GameLogic.h"
#include "TournamentDialog.h"

// TournamentDialog type definition
IMPLEMENT_CLASS( TournamentDialog, wxDialog )

// TournamentDialog event table definition
BEGIN_EVENT_TABLE( TournamentDialog, wxDialog )
    EVT_BUTTON( ID_TOURNAMENT_RESET, TournamentDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, TournamentDialog::OnHelpClick )
    EVT_BUTTON( wxID_OK, TournamentDialog::OnOkClick )
END_EVENT_TABLE()

// From my utility library, should probably link it in properly
static void rtrim( std::string &s );
static std::string sprintf( const char *fmt, ... );

// TournamentDialog constructors
TournamentDialog::TournamentDialog()
{
    Init();
}

TournamentDialog::TournamentDialog( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void TournamentDialog::Init()
{
    event       = "";               // Event
    site        = "";               // Site
    date        = "";               // Date
    round       = "";               // Round
}

// Create dialog
bool TournamentDialog::Create( wxWindow* parent,
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

// Control creation for TournamentDialog
void TournamentDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, SMALL);

//    // A friendly message
//    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
//        wxT("Add Tournament Games"), wxDefaultPosition, wxDefaultSize, 0 );
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
    #define WIDTH 110
    wxSize sz=wxSize(2*WIDTH,wxDefaultCoord);

    // Label for event
    wxStaticText* event_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Event "), wxDefaultPosition, wxDefaultSize, 0 );
    HV1->Add(event_label, 0, wxALIGN_LEFT|wxALL, SMALL);

    // Text control for event
    event_ctrl = new wxTextCtrl ( this, ID_TOURNAMENT_EVENT,           wxT(""), wxDefaultPosition, sz, 0 );
    HV1->Add(event_ctrl, 0, wxGROW|wxALL, SMALL);

    // Label for site
    wxStaticText* site_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Site "), wxDefaultPosition, wxDefaultSize, 0 );
    HV1->Add(site_label, 0, wxALIGN_LEFT|wxALL, SMALL);

    // Text control for site
    site_ctrl = new wxTextCtrl ( this, ID_TOURNAMENT_SITE,            wxT(""), wxDefaultPosition, sz, 0 );
    HV1->Add(site_ctrl, 0, wxGROW|wxALL, SMALL);

    // Label for date
    wxStaticText* date_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Date "), wxDefaultPosition, wxDefaultSize, 0 );
    HV1->Add(date_label, 0, wxALIGN_LEFT|wxALL, SMALL);

    // Text control for date
    date_ctrl = new wxTextCtrl ( this, ID_TOURNAMENT_DATE,            wxT(""), wxDefaultPosition, sz, 0 );
    HV1->Add(date_ctrl, 0, wxGROW|wxALL, SMALL);

    // Label for round
    wxStaticText* round_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Base Round "), wxDefaultPosition, wxDefaultSize, 0 );
    HV1->Add(round_label, 0, wxALIGN_LEFT|wxALL, SMALL);

    // Text control for round
    round_ctrl = new wxTextCtrl ( this, ID_TOURNAMENT_ROUND,           wxT(""), wxDefaultPosition, sz, 0 );
    HV1->Add(round_ctrl, 0, wxGROW|wxALL, SMALL);

    //H1->Add(V2, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);
    box_sizer->Add(HV1, 1, wxGROW | (wxALL/* & ~wxLEFT */), LARGE);

    // A horizontal box sizer to contain the user text
    wxBoxSizer* user_text_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Text control for pairings_txt
    wxSize sz_multi = sz;
    sz_multi.x = 4*WIDTH;
    sz_multi.y = 2*WIDTH;
    pairings_txt =
          // Don't change this without looking for all instances of 2499!
          "Paste Players and Ratings here, using the format shown below;\n"
          "Federer, Roger 2499   Woods, Tiger 2499\n"
          "Messi, Lionel 2499   Ronaldo, Christiano 2499\n"
          "Tarrasch expects two player names with ratings per line. It will remove extra spaces and punctuation. "
          "It expects ratings to immediately follow names, but other non-alpha characters can appear between "
          "or before fields, so the following Swiss Perfect format for example is okay;\n"
          "1 Federer, Roger    2499 [6.5]     :     Woods, Tiger      2499 [5.5]\n"
          "If your pairing program produces something that doesn't work, email me at billforsternz@gmail.com with "
          "a sample, and if you ask nicely I'll adapt Tarrasch for you.\n\n"
          "This help information will be ignored if it is present, so you can paste above, paste below or Ctrl-A to select all and paste over the top.\n";
    pairings_ctrl = new wxTextCtrl ( this, ID_TOURNAMENT_PAIRINGS, "",
        wxDefaultPosition, sz_multi, wxTE_MULTILINE );
    user_text_sizer->Add(pairings_ctrl, 0, wxGROW|wxALL, SMALL);

    box_sizer->Add(user_text_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);

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
void TournamentDialog::SetDialogValidators()
{
    FindWindow(ID_TOURNAMENT_EVENT)->SetValidator(
        wxTextValidator(wxFILTER_NONE, &event));
    FindWindow(ID_TOURNAMENT_SITE)->SetValidator(
        wxTextValidator(wxFILTER_NONE, &site));
    FindWindow(ID_TOURNAMENT_DATE)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &date));
    FindWindow(ID_TOURNAMENT_ROUND)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &round));
    FindWindow(ID_TOURNAMENT_PAIRINGS)->SetValidator(
        wxTextValidator(wxFILTER_NONE, &pairings_txt));
}

// Sets the help text for the dialog controls
void TournamentDialog::SetDialogHelp()
{
    wxString the_help =
      wxT("Use this panel to generate empty skeleton games for a tournament round.\n"
          "Don't worry, if Roger Federer is playing Tiger Woods at your tournament\n"
          "their game won't be skipped unless they are both rated 2499!\n"
      );

    FindWindow(ID_TOURNAMENT_PAIRINGS)->SetHelpText(the_help);
    FindWindow(ID_TOURNAMENT_PAIRINGS)->SetToolTip(the_help);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYER_RESET
void TournamentDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void TournamentDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString helpText =
      wxT("Use this panel to generate empty skeleton games for a tournament round.\n"
          "Don't worry, if Roger Federer is playing Tiger Woods at your tournament\n"
          "their game won't be skipped unless they are both rated 2499!\n"
      );

    wxMessageBox(helpText,
      wxT("Tournament Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void TournamentDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
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
                err_msg = "Illegal base round field";
        }
    }
    if( ok )
        AcceptAndClose();
    else
        wxMessageBox( err_msg, "Error in game template", wxOK|wxICON_ERROR );
}

// From my utility library, should probably link it in properly
static void rtrim( std::string &s )
{
    size_t final_char_offset = s.find_last_not_of(" \n\r\t");
    if( final_char_offset == std::string::npos )
        s.clear();
    else
        s.erase(final_char_offset+1);
}


// From my utility library, should probably link it in properly
static std::string sprintf( const char *fmt, ... )
{
    int size = strlen(fmt) * 3;   // guess at size
    std::string str;
    va_list ap;
    for(;;)
    {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt, ap);
        va_end(ap);
        if( n>-1 && n<size )    // are we done yet?
        {
            str.resize(n);
            break;
        }
        if( n > size )  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 4;      // Guess at a larger size
    }
    return str;
}

static void parse_players_txt( const char *txt, std::vector<CompactGame> &games );
bool TournamentDialog::Run( CompactGame &proto, std::vector<CompactGame> &games )
{
    games.clear();
    event     = proto.r.event;          // "Event"
    site      = proto.r.site;           // "Site"
    std::string base = proto.r.round;   // "Round"
    if( base=="" || base == "?" )
        round = "1";
    else
    {
        size_t offset = round.find_last_of(".");
        if( offset != std::string::npos )
            base = round.substr(0,offset);
        int ibase = atoi( base.c_str() );
        if( ibase < 1 )
            round = "1";
        else
            round = sprintf("%d",ibase + 1);
    }
    time_t rawtime;
    struct tm * timeinfo;
    time( &rawtime );
    timeinfo = localtime( &rawtime );
    char buf[40];
    sprintf( buf, "%04d.%02d.%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday );
    date = buf;
    if( wxID_OK != ShowModal() )
        return false;
    event.Trim(true);
    event.Trim(false);
    site.Trim(true);
    site.Trim(false);
    date.Trim(true);
    date.Trim(false);
    round.Trim(true);
    round.Trim(false);
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
    parse_players_txt( pairings_txt.c_str(), games );
    size_t offset = round.find_last_of(".");
    base=round;
    int board=1;
    if( offset != std::string::npos )
    {
        base = round.substr(0,offset);
        board = atoi( round.substr(offset+1).c_str() );
        if( board < 1 )
            board = 1;
    }
    for( CompactGame &pact: games )
    {
        pact.r.event = event;
        pact.r.site  = site;
        pact.r.date  = date;
        pact.r.round  = base + "." + sprintf("%d",board++);
    }
    return games.size() > 1;
}

static void parse_players_txt( const char *txt, std::vector<CompactGame> &games )
{

    // 2 Dive, Russell         2414 [3.5]     :     Ker, Anthony        2437 [3.5]
    enum {init,in_name,in_rating} state=init;
    std::string name, rating, first, first_rating, between, second, second_rating;
    char previous=' ';
    for(;;)
    {
        char c = *txt++;
        switch( state )
        {
            case init:
            {
                if( isascii(c) && isalpha(c) )
                {
                    state = in_name;
                    name = "";
                    name += c;
                }
                else
                {
                    if( first_rating!="" && second=="" )
                        between += c;
                }
                break;
            }
            case in_name:
            {
                std::string s;
                s += c;
                if( isascii(c) && isdigit(c) && (previous==' '||previous=='\t') )
                {
                    rtrim(name);
                    state = in_rating;
                    rating = "";
                    rating += c;
                }
                else if( isascii(c) && std::string::npos==s.find_first_of("!#$%^&*(){}[]") )
                    name += c;
                else if( c=='.' || c==',' || c==' ' )
                    name += c;
                break;
            }
            case in_rating:
            {
                if( isascii(c) && !isdigit(c) )
                {
                    state = init;
                    if( first == "" )
                    {
                        first = name;
                        first_rating = rating;
                    }
                    else
                    {
                        second = name;
                        second_rating = rating;
                    }
                }
                else
                    rating += c;
                break;
            }
        }
        if( c=='\0' || c=='\n' )
        {
            bool skip = false;
            if( first=="Federer, Roger" && first_rating=="2499" &&
                second=="Woods, Tiger" && second_rating=="2499" )
                skip = true;
            else if( first=="Messi, Lionel" && first_rating=="2499" &&
                second=="Ronaldo, Christiano" && second_rating=="2499" )
                skip = true;
            if( !skip && first!="" && second!="" )
            {
                CompactGame pact;
                pact.r.white = first;
                pact.r.black = second;
                pact.r.white = first;
                int r = atoi(first_rating.c_str());
                if( r < 0 )
                    first_rating = "0";
                if( r > 9999 )
                    first_rating = "9999";
                r = atoi(second_rating.c_str());
                if( r < 0 )
                    second_rating = "0";
                if( r > 9999 )
                    second_rating = "9999";
                pact.r.white_elo = first_rating;
                pact.r.black_elo = second_rating;
                pact.r.result = "*";
                if( std::string::npos != between.find("1:0") ||  std::string::npos != between.find("1-0") )
                    pact.r.result = "1-0";
                else if( std::string::npos != between.find("0:1") || std::string::npos != between.find("0-1") )
                    pact.r.result = "0-1";
                else if( std::string::npos != between.find("\xbd") || std::string::npos != between.find("0.5")
                    || std::string::npos != between.find("1/2") )
                    pact.r.result = "1/2-1/2";
                games.push_back(pact);
            }
            if( c=='\0' )
                break;
            else
            {
                state = init;
                first = "";
                second = "";
                first_rating = "";
                second_rating = "";
                between = "";
            }
        }
    }
}
