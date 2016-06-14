/****************************************************************************
 * Custom dialog - Set up pattern on board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/statline.h"
#include "wx/msgdlg.h"
#include "Appdefs.h"
#include "Objects.h"
#include "GameLogic.h"
#include "thc.h"
#include "BoardSetupControl.h"
#include "Book.h"
#include "Repository.h"
#include "PatternDialog.h"
using namespace std;
using namespace thc;

// PatternDialog type definition
IMPLEMENT_CLASS( PatternDialog, wxDialog )

// PatternDialog event table definition
BEGIN_EVENT_TABLE( PatternDialog, wxDialog )
    EVT_BUTTON( ID_PATTERN_CLEAR,PatternDialog::OnClearClick )
    EVT_BUTTON( ID_PATTERN_CURRENT, PatternDialog::OnCurrentClick )
    EVT_BUTTON( ID_PATTERN_RESET, PatternDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, PatternDialog::OnHelpClick )
    EVT_BUTTON( wxID_OK, PatternDialog::OnOkClick )
    EVT_SPINCTRL( ID_PATTERN_MOVE_COUNT, PatternDialog::OnSpin )
    EVT_RADIOBUTTON( ID_PATTERN_WHITE_TO_MOVE,      PatternDialog::OnRadio )
    EVT_RADIOBUTTON( ID_PATTERN_BLACK_TO_MOVE,      PatternDialog::OnRadio )
    EVT_RADIOBUTTON( ID_PATTERN_EITHER_TO_MOVE,     PatternDialog::OnRadio )
    EVT_CHECKBOX   ( ID_PATTERN_PAWNS_SAME_FILES,   PatternDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_PATTERN_BISHOPS_SAME_COLOUR,PatternDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_PATTERN_ALLOW_MORE,         PatternDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_PATTERN_INC_REVERSE,        PatternDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_PATTERN_INC_REFLECTION,     PatternDialog::OnCheckBox )
END_EVENT_TABLE()


// PatternDialog constructors
static PatternDialog *singleton;   // for VeryUglyTemporaryCallback();
PatternDialog::PatternDialog()
{
    singleton = this;
    bsc = NULL;
    Init();
}

PatternDialog::PatternDialog
(
    wxWindow* parent,
    wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size, long style
)
{
    singleton = this;
    bsc = NULL;
    memset( &parm, 0, sizeof(parm) );
    parm.number_of_ply = 1;
    parm.either_to_move = false;
    Init();
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void PatternDialog::Init()
{
    singleton = this;
    thc::ChessPosition tmp;
    parm.cp = tmp;                        // init pattern
    memset( parm.cp.squares, ' ', 64 );   // .. but with blank board
    parm.cp.wking  = false;               // .. and no castling
    parm.cp.wqueen = false;
    parm.cp.bking  = false;
    parm.cp.bqueen = false;
    Pos2Fen( parm.cp, fen );
}


void PatternDialog::Pos2Fen( const thc::ChessPosition& pos, wxString& fen )
{
    const char *src=pos.squares;
    char buf[128];
    char *dst=buf;
    int count=0;
    for( int i=0; i<64; i++ )
    {
        if( *src==' ' )
            count++;
        else
        {
            if( count )
            {
                *dst++ = '0'+count;
                count = 0;
            }
            *dst++ = *src;
        }
        if( i==7 || i==15 || i==23 || i==31 ||
            i==39 || i==47 || i==55 )
        {
            if( count )
                *dst++ = '0'+count;
            *dst++ = '/';
            count = 0;
        }
        src++;
    }
    if( count )
        *dst++ = '0'+count;
    *dst = '\0';

    // Validate en-passant
    bool ep=false;

    // White captures enpassant from a6 to h6
    if( pos.white && a6<=pos.enpassant_target && pos.enpassant_target<=h6 )
    {
        int idx = pos.enpassant_target+8;   // SOUTH
        if( pos.squares[idx] == 'p' )
        {
            #if 1
            ep = true;
            #else
            if( pos.enpassant_target==a6 && pos.squares[idx+1]=='P' )
                ep = true;
            else if( pos.enpassant_target==h6 && pos.squares[idx-1]=='P' )
                ep = true;
            else if( pos.squares[idx-1]=='P' || pos.squares[idx+1]=='P' )
                ep = true;
            #endif
        }
    }        

    // Black captures enpassant from a3 to h3
    if( !pos.white && a3<=pos.enpassant_target && pos.enpassant_target<=h3 )
    {
        int idx = pos.enpassant_target-8;    //NORTH
        if( pos.squares[idx] == 'P' )
        {
            #if 1
            ep = true;
            #else
            if( pos.enpassant_target==a3 && pos.squares[idx+1]=='p' )
                ep = true;
            else if( pos.enpassant_target==h3 && pos.squares[idx-1]=='p' )
                ep = true;
            else if( pos.squares[idx-1]=='p' || pos.squares[idx+1]=='p' )
                ep = true;
            #endif
        }
    }        
    char castling[5];
    char *pc = castling;
    if( pos.wking_allowed() )
        *pc++ = 'K';
    if( pos.wqueen_allowed() )
        *pc++ = 'Q';
    if( pos.bking_allowed() )
        *pc++ = 'k';
    if( pos.bqueen_allowed() )
        *pc++ = 'q';
    if( pc == castling )
        *pc++ = '-';
    *pc = '\0';
    char ep_buf[3];
    ep_buf[0] = '-';
    ep_buf[1] = '\0';
    if( ep && pos.white && a6<=pos.enpassant_target && pos.enpassant_target<=h6 )
    {
        ep_buf[0] = 'a' + (pos.enpassant_target-a6);
        ep_buf[1] = '6';
        ep_buf[2] = '\0';
    }
    else if( ep && !pos.white && a3<=pos.enpassant_target && pos.enpassant_target<=h3 )
    {
        ep_buf[0] = 'a' + (pos.enpassant_target-a3);
        ep_buf[1] = '3';
        ep_buf[2] = '\0';
    }
    fen.sprintf( "%s %c %s %s %d %d", 
            buf,
            pos.white?'w':'b',
            castling,
            ep_buf,
            pos.half_move_clock,
            pos.full_move_count );
}

// Control update
void PatternDialog::WriteToControls()
{
    bsc->SetPosition( parm.cp.squares );
    inc_reflection->SetValue( parm.cp.wking_allowed() );
    inc_reverse->SetValue( parm.cp.wqueen_allowed() );
    allow_more->SetValue( parm.cp.bking_allowed() );
    pawns_same_files->SetValue( parm.cp.bqueen_allowed() );
    white_to_move->SetValue( parm.cp.white );
    black_to_move->SetValue( !parm.cp.white );
/*    wxString en_passant;
    if( parm.cp.white && a6<=parm.cp.enpassant_target && parm.cp.enpassant_target<=h6 )
        en_passant.sprintf( "%c", 'a' + (parm.cp.enpassant_target-a6) );
    else if( !parm.cp.white && a3<=parm.cp.enpassant_target && parm.cp.enpassant_target<=h3 )
        en_passant.sprintf( "%c", 'a' + (parm.cp.enpassant_target-a3) );
    else
        en_passant = "None";
    en_passant_ctrl->SetValue(en_passant);
    fen_ctrl->SetValue(fen);
    half_count_ctrl->SetValue(parm.cp.half_move_clock); */
    move_count_ctrl->SetValue(parm.cp.full_move_count);
}

// Dialog create
bool PatternDialog::Create( wxWindow* parent,
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

// Control creation for PatternDialog
void PatternDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
/*
    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT(" Enter pattern graphically or using FEN notation"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
*/

    // The board setup bitmap
    bsc = new BoardSetupControl(this);
    bsc->SetPosition( parm.cp.squares );
//    box_sizer->Add( bsc, 0, wxALIGN_LEFT|wxALL|wxFIXED_MINSIZE, 0 );

///

    // Intermediate sizers
    wxBoxSizer* horiz_board = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* horiz_extra = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Refine search" );
    wxSizer     *castling_box = new wxStaticBoxSizer(box, wxVERTICAL);
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, "&Who to move" );
    wxSizer     *who_box = new wxStaticBoxSizer(box2, wxVERTICAL);
    //wxStaticBox *box3 = new wxStaticBox(this, wxID_ANY, "&Other details" );
    //wxSizer     *details_box = new wxStaticBoxSizer(box3, wxVERTICAL);

    inc_reflection = new wxCheckBox( this, ID_PATTERN_INC_REFLECTION,
       wxT("&Include reflections"), wxDefaultPosition, wxDefaultSize, 0 );
    inc_reflection->SetValue( true );
    inc_reverse = new wxCheckBox( this, ID_PATTERN_INC_REVERSE,
       wxT("&Include reversed colours"), wxDefaultPosition, wxDefaultSize, 0 );
    inc_reverse->SetValue( parm.cp.wqueen_allowed() );
    allow_more = new wxCheckBox( this, ID_PATTERN_ALLOW_MORE,
       wxT("&Allow more pieces"), wxDefaultPosition, wxDefaultSize, 0 );
    allow_more->SetValue( parm.cp.bking_allowed() );
    pawns_same_files = new wxCheckBox( this, ID_PATTERN_PAWNS_SAME_FILES,
       wxT("&Pawns must be on same files"), wxDefaultPosition, wxDefaultSize, 0 );
    pawns_same_files->SetValue( parm.cp.bqueen_allowed() );
    bishops_same_colour = new wxCheckBox( this, ID_PATTERN_BISHOPS_SAME_COLOUR,
       wxT("&Bishops must be same colour"), wxDefaultPosition, wxDefaultSize, 0 );
    bishops_same_colour->SetValue( parm.cp.bqueen_allowed() );

    white_to_move = new wxRadioButton( this, ID_PATTERN_WHITE_TO_MOVE,
       wxT("&White to move"), wxDefaultPosition, wxDefaultSize,  wxRB_GROUP );
    white_to_move->SetValue( parm.either_to_move ? false : parm.white_to_move );
    black_to_move = new wxRadioButton( this, ID_PATTERN_BLACK_TO_MOVE,
       wxT("&Black to move"), wxDefaultPosition, wxDefaultSize, 0 );
    black_to_move->SetValue( parm.either_to_move ? false : !parm.white_to_move );
    either_to_move = new wxRadioButton( this, ID_PATTERN_EITHER_TO_MOVE,
       wxT("&Either to move"), wxDefaultPosition, wxDefaultSize, 0 );
    either_to_move->SetValue( parm.either_to_move );
 
    // Move count
    wxBoxSizer* move_count_sizer  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* move_count_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Move count"), wxDefaultPosition, wxDefaultSize, 0 );
    move_count_ctrl = new wxSpinCtrl ( this, ID_PATTERN_MOVE_COUNT,
        wxEmptyString, wxDefaultPosition, wxSize(50, wxDefaultCoord), //wxDefaultSize, 
        wxSP_ARROW_KEYS, parm.number_of_ply, 500, 1 );
    move_count_sizer->Add(move_count_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    move_count_sizer->Add(92, 5, 1, wxALL, 0);
    move_count_sizer->Add(move_count_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
   
    castling_box->Add( inc_reflection, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( inc_reverse, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( allow_more, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( pawns_same_files, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( bishops_same_colour, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add(move_count_sizer, 
         wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
         //wxALIGN_CENTER_VERTICAL|wxALL, 5);
 
    who_box->Add( white_to_move, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    who_box->Add( black_to_move, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    who_box->Add( either_to_move, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

 // details_box->Add(move_count_sizer, 
 //     wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
 //     //wxALIGN_CENTER_VERTICAL|wxALL, 5);
 // details_box->Add(half_count_sizer,
 //     wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
 //     //wxALIGN_CENTER_VERTICAL|wxALL, 5);
 // details_box->Add(en_passant_sizer,
 //     wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
 //     //wxALIGN_CENTER_VERTICAL|wxALL, 5);   

    // A vertical box sizer to contain Reset, Clear, OK, Cancel and Help
    wxBoxSizer* buttons_box = new wxBoxSizer(wxVERTICAL);

    // The Initial button
    wxButton* reset = new wxButton( this, ID_PATTERN_RESET, wxT("&Initial"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( reset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Current button
    wxButton* current = new wxButton( this, ID_PATTERN_CURRENT, wxT("&Current"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( current, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Clear button
    wxButton* clear = new wxButton( this, ID_PATTERN_CLEAR, wxT("&Clear"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( clear, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The OK button
    ok_button = new wxButton ( this, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( ok_button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
        wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Help button
/*  wxButton* help = new wxButton( this, wxID_HELP, wxT("&Help"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5); */

    //box_sizer->Add( buttons_box, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    horiz_board->Add( bsc, 1, wxALIGN_LEFT|wxALL|wxFIXED_MINSIZE, 5 );
    horiz_board->Add( buttons_box, 0, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);

    horiz_extra->Add(who_box,       1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxTOP  */), 5);
    horiz_extra->Add(castling_box,  1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
//    horiz_extra->Add(details_box,   0, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);

    box_sizer->Add( horiz_board,  0, wxALIGN_LEFT |wxALL, 5 );
    box_sizer->Add( horiz_extra,  0, wxALIGN_LEFT |wxLEFT|wxRIGHT|wxTOP, 5 );

/*    // The Apply button
    wxButton* apply = new wxButton( this, ID_APPLY, wxT("&Apply"),
        wxDefaultPosition, wxDefaultSize, 0 );

    // Text control for fen entry
    fen_ctrl = new wxTextCtrl ( this, ID_FEN, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxSize sz=fen_ctrl->GetSize();
    fen_ctrl->SetSize((sz.x*118)/32,sz.y);      // temp temp
    //fen_ctrl->SetSize((sz.x*7)/2,sz.y);      // temp temp
    fen_ctrl->SetValue(fen); */

/*
    // Label for fen entry
    wxStaticText* fen_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Forsyth"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(fen_label, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* horiz_fen  = new wxBoxSizer(wxHORIZONTAL);
    horiz_fen->Add( fen_ctrl,  1, wxALIGN_LEFT|wxGROW|wxALL, 5);
    horiz_fen->Add( apply,     0, wxALIGN_RIGHT|wxALL, 5);
    box_sizer->Add( horiz_fen, 0, wxALIGN_LEFT|wxALL, 5 );
*/
//    wxStaticBox *box4 = new wxStaticBox(this, wxID_ANY, "&FEN (Forsyth-Edwards Notation)" );
//    wxSizer     *fen_box = new wxStaticBoxSizer(box4, /*wxGROW|*/wxHORIZONTAL);
/*  fen_box->Add( fen_ctrl,  0, wxALIGN_LEFT|wxALL|wxFIXED_MINSIZE, 5);
    fen_box->Add( apply,     0, wxALIGN_RIGHT|wxALL, 5 );
    box_sizer->Add( fen_box, 1, wxALIGN_LEFT|wxGROW|wxALL, 5 ); */
/*    fen_box->Add( fen_ctrl,  0, wxALIGN_CENTER_VERTICAL|wxALL|wxFIXED_MINSIZE, 2);
    fen_box->AddSpacer(5);
    fen_box->Add( apply,     0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
    box_sizer->Add( fen_box, 0, wxALIGN_LEFT|wxGROW|wxALL, 10 ); */

/*  move_count_sizer->Add(move_count_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    move_count_sizer->Add(95, 5, 1, wxALL, 0);
    move_count_sizer->Add(move_count_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5); */

/*    wxSize sz=box4->GetSize();
    box4->SetSize(sz.x,sz.y/2);
    wxPoint pt = apply->GetPattern();
    pt.x += 150;
    apply->SetPosition(pt); */

/*
    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);
*/
    current->SetFocus();
}

// Set the validators for the dialog controls
void PatternDialog::SetDialogValidators()
{
//    FindWindow(ID_FEN)           -> SetValidator( wxTextValidator(wxFILTER_ASCII, &fen));
}

// Sets the help text for the dialog controls
void PatternDialog::SetDialogHelp()
{
    wxString initial_help = "Set the standard chess initial pattern";
    FindWindow(ID_PATTERN_RESET)->SetHelpText(initial_help);
    FindWindow(ID_PATTERN_RESET)->SetToolTip(initial_help);
    wxString clear_help = "Clear the board";
    FindWindow(ID_PATTERN_CLEAR)->SetHelpText(clear_help);
    FindWindow(ID_PATTERN_CLEAR)->SetToolTip(clear_help);
    wxString current_help = "Copy from the current main board pattern";
    FindWindow(ID_PATTERN_CURRENT)->SetHelpText(current_help);
    FindWindow(ID_PATTERN_CURRENT)->SetToolTip(current_help);
    wxString help1 = "Set to include mirror images of this pattern in search";
    FindWindow(ID_PATTERN_INC_REFLECTION)->SetHelpText(help1);
    FindWindow(ID_PATTERN_INC_REFLECTION)->SetToolTip(help1);
    wxString help2 = "Set to include reversed colours form of this pattern in search";
    FindWindow(ID_PATTERN_INC_REVERSE)->SetHelpText(help2);
    FindWindow(ID_PATTERN_INC_REVERSE)->SetToolTip(help2);
    wxString help3 = "Set to allow extra pieces - unless set pattern search is effectively position search";
    if( parm.material_balance )
        help3 = "Set to allow extra pieces - only useful for patterns with extra promoted pieces - eg two queens";
    FindWindow(ID_PATTERN_ALLOW_MORE)->SetHelpText(help3);
    FindWindow(ID_PATTERN_ALLOW_MORE)->SetToolTip(help3);
    wxString help4 = "Set to specify that games only match if pawns are on the same files as the search pattern";
    FindWindow(ID_PATTERN_PAWNS_SAME_FILES)->SetHelpText(help4);
    FindWindow(ID_PATTERN_PAWNS_SAME_FILES)->SetToolTip(help4);
    wxString help5 = "Set this if you want light squared and dark squared bishops to be distinct pieces for search purposes";
    FindWindow(ID_PATTERN_BISHOPS_SAME_COLOUR)->SetHelpText(help5);
    FindWindow(ID_PATTERN_BISHOPS_SAME_COLOUR)->SetToolTip(help5);
    wxString white_help = "Set to find positions with white to move";
    FindWindow(ID_PATTERN_WHITE_TO_MOVE)->SetHelpText(white_help);
    FindWindow(ID_PATTERN_WHITE_TO_MOVE)->SetToolTip(white_help);
    wxString black_help = "Set to find positions with black to move";
    FindWindow(ID_PATTERN_BLACK_TO_MOVE)->SetHelpText(black_help);
    FindWindow(ID_PATTERN_BLACK_TO_MOVE)->SetToolTip(black_help);
    wxString either_help = "Set if you don't care which side is to move";
    FindWindow(ID_PATTERN_EITHER_TO_MOVE)->SetHelpText(either_help);
    FindWindow(ID_PATTERN_EITHER_TO_MOVE)->SetToolTip(either_help);
    wxString move_count_help = "Games only match if the specified material balance persists for at least this many half moves";
    FindWindow(ID_PATTERN_MOVE_COUNT)->SetHelpText(move_count_help);
    FindWindow(ID_PATTERN_MOVE_COUNT)->SetToolTip(move_count_help);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_RESET
void PatternDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessPosition tmp;
    parm.cp = tmp;
    Pos2Fen( parm.cp, fen );
    WriteToControls();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_CURRENT
void PatternDialog::OnCurrentClick( wxCommandEvent& WXUNUSED(event) )
{
    parm.cp = objs.gl->gd.master_position;
    Pos2Fen( parm.cp, fen );
    WriteToControls();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_CLEAR
void PatternDialog::OnClearClick( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessPosition tmp;
    parm.cp = tmp;
    memset( parm.cp.squares, ' ', 64 );
    parm.cp.wking  = false;
    parm.cp.wqueen = false;
    parm.cp.bking  = false;
    parm.cp.bqueen = false;
    Pos2Fen( parm.cp, fen );
    WriteToControls();
}


// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void PatternDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to specify a new pattern\n");

    wxMessageBox(helpText,
      wxT("Pattern Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void PatternDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    bool err=false;
    if( bsc )
    {
        strcpy( parm.cp.squares, bsc->squares );
        thc::ChessRules cr = parm.cp;
        char forsyth[128];
        strcpy( forsyth, cr.ForsythPublish().c_str() );
        fen = forsyth;
        TransferDataToWindow();    
    }
    if( !err )
        AcceptAndClose();
}

void PatternDialog::OnSpin( wxSpinEvent& WXUNUSED(event) )
{
}

void PatternDialog::OnCheckBox( wxCommandEvent& WXUNUSED(event) )
{
}

void PatternDialog::OnRadio( wxCommandEvent& WXUNUSED(event) )
{
}

static bool Castling( char squares[64], char KkQq )
{
    bool ret=false;
    switch( KkQq )
    {
        default:  ret = false;
        case 'q': ret = (squares[0]=='r' &&  squares[4]=='k');   break;
        case 'k': ret = (squares[7]=='r' &&  squares[4]=='k');   break;
        case 'Q': ret = (squares[56]=='R' && squares[60]=='K');  break;
        case 'K': ret = (squares[63]=='R' && squares[60]=='K');  break;
    }
    return ret;
}

/*void PatternDialog::ModifyFen()
{
    if( Castling(bsc->squares,'q') && !Castling(parm.cp.squares,'q') )
        this->pawns_same_files->SetValue(true);
    else if( !Castling(bsc->squares,'q') && Castling(parm.cp.squares,'q') )
        this->pawns_same_files->SetValue(false);
    if( Castling(bsc->squares,'k') && !Castling(parm.cp.squares,'k') )
        this->allow_more->SetValue(true);
    else if( !Castling(bsc->squares,'k') && Castling(parm.cp.squares,'k') )
        this->allow_more->SetValue(false);
    if( Castling(bsc->squares,'Q') && !Castling(parm.cp.squares,'Q') )
        this->inc_reverse->SetValue(true);
    else if( !Castling(bsc->squares,'Q') && Castling(parm.cp.squares,'Q') )
        this->inc_reverse->SetValue(false);
    if( Castling(bsc->squares,'K') && !Castling(parm.cp.squares,'K') )
        this->inc_reflection->SetValue(true);
    else if( !Castling(bsc->squares,'K') && Castling(parm.cp.squares,'K') )
        this->inc_reflection->SetValue(false);
    strcpy( parm.cp.squares, bsc->squares );
    if( !Castling(parm.cp.squares,'q') )
        this->pawns_same_files->SetValue(false);
    if( !Castling(parm.cp.squares,'k') )
        this->allow_more->SetValue(false);
    if( !Castling(parm.cp.squares,'Q') )
        this->inc_reverse->SetValue(false);
    if( !Castling(parm.cp.squares,'K') )
        this->inc_reflection->SetValue(false);
    parm.cp.wqueen = this->inc_reverse->GetValue();
    parm.cp.wking  = this->inc_reflection->GetValue();
    parm.cp.bqueen = this->pawns_same_files->GetValue();
    parm.cp.bking  = this->allow_more->GetValue();

    parm.cp.full_move_count = move_count_ctrl->GetValue();    
    parm.cp.half_move_clock = half_count_ctrl->GetValue();    
    parm.cp.white  = !black_to_move->GetValue();
    parm.cp.wqueen = this->inc_reverse->GetValue();
    parm.cp.wking  = this->inc_reflection->GetValue();
    parm.cp.bqueen = this->pawns_same_files->GetValue();
    parm.cp.bking  = this->allow_more->GetValue();
    wxString ep  = this->en_passant_ctrl->GetValue();
    parm.cp.enpassant_target = SQUARE_INVALID;
    if( ep!="None" && 'a'<=ep[0] && ep[0]<='h' )
        parm.cp.enpassant_target = (Square)( (parm.cp.white?a6:a3) + (ep[0]-'a') );
    Pos2Fen( parm.cp, fen );
    fen_ctrl->SetValue( fen );
} */
