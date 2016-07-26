/****************************************************************************
 * Custom dialog - Set up position on board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
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
#include "PositionDialog.h"
using namespace std;
using namespace thc;

// PositionDialog type definition
IMPLEMENT_CLASS( PositionDialog, wxDialog )

// PositionDialog event table definition
BEGIN_EVENT_TABLE( PositionDialog, wxDialog )
    EVT_BUTTON( ID_POSITION_960,PositionDialog::OnChess960Click )
    EVT_BUTTON( ID_POSITION_CLEAR,PositionDialog::OnClearClick )
    EVT_BUTTON( ID_POSITION_CURRENT, PositionDialog::OnCurrentClick )
    EVT_BUTTON( ID_POSITION_RESET, PositionDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, PositionDialog::OnHelpClick )
    EVT_BUTTON( wxID_OK, PositionDialog::OnOkClick )
    EVT_BUTTON( ID_APPLY,         PositionDialog::OnApplyClick )
    EVT_SPINCTRL( ID_MOVE_COUNT, PositionDialog::OnSpin )
    EVT_SPINCTRL( ID_HALF_COUNT, PositionDialog::OnSpin )
    EVT_RADIOBUTTON( ID_WHITE_TO_MOVE, PositionDialog::OnRadio )
    EVT_RADIOBUTTON( ID_BLACK_TO_MOVE, PositionDialog::OnRadio )
    EVT_CHECKBOX   ( ID_WHITE_OO,      PositionDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_WHITE_OOO,     PositionDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_BLACK_OO,      PositionDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_BLACK_OOO,     PositionDialog::OnCheckBox )
    EVT_COMBOBOX   ( ID_EN_PASSANT,    PositionDialog::OnComboBox )
    EVT_COMBOBOX   ( ID_PREDEFINED,    PositionDialog::OnPredefined )
END_EVENT_TABLE()

// PositionDialog constructors
static PositionDialog *singleton;   // for PositionSetupVeryUglyTemporaryCallback();
PositionDialog::PositionDialog()
{
    singleton = this;
    bsc = NULL;
    Init();
}

PositionDialog::PositionDialog
(
    wxWindow* parent,
    wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size, long style
)
{
    singleton = this;
    bsc = NULL;
    Init();
    Create(parent, id, caption, pos, size, style);
}

PositionDialog::~PositionDialog()
{
    singleton = NULL;
}

// Initialisation
void PositionDialog::Init()
{
    singleton = this;
/*  // if we want to start with a clear board - this is how we do it
    thc::ChessPosition tmp;
    m_pos = tmp;                        // init position
    memset( m_pos.squares, ' ', 64 );   // .. but with blank board
    m_pos.wking  = false;               // .. and no castling
    m_pos.wqueen = false;
    m_pos.bking  = false;
    m_pos.bqueen = false; */
    m_pos = objs.gl->gd.master_position;
    Pos2Fen( m_pos, fen );
}


void PositionDialog::Pos2Fen( const thc::ChessPosition& pos, wxString& fen_ )
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
    fen_.sprintf( "%s %c %s %s %d %d", 
            buf,
            pos.white?'w':'b',
            castling,
            ep_buf,
            pos.half_move_clock,
            pos.full_move_count );
}

// Control update
void PositionDialog::WriteToControls()
{
    bsc->Set( m_pos );
    white_oo->SetValue( m_pos.wking_allowed() );
    white_ooo->SetValue( m_pos.wqueen_allowed() );
    black_oo->SetValue( m_pos.bking_allowed() );
    black_ooo->SetValue( m_pos.bqueen_allowed() );
    white_to_move->SetValue( m_pos.white );
    black_to_move->SetValue( !m_pos.white );
    wxString en_passant;
    if( m_pos.white && a6<=m_pos.enpassant_target && m_pos.enpassant_target<=h6 )
        en_passant.sprintf( "%c", 'a' + (m_pos.enpassant_target-a6) );
    else if( !m_pos.white && a3<=m_pos.enpassant_target && m_pos.enpassant_target<=h3 )
        en_passant.sprintf( "%c", 'a' + (m_pos.enpassant_target-a3) );
    else
        en_passant = "None";
    en_passant_ctrl->SetValue(en_passant);
    half_count_ctrl->SetValue(m_pos.half_move_clock);
    move_count_ctrl->SetValue(m_pos.full_move_count);
    fen_ctrl->SetValue(fen);
}

// Dialog create
bool PositionDialog::Create( wxWindow* parent,
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

// Control creation for PositionDialog
void PositionDialog::CreateControls()
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
        wxT(" Enter position graphically or using FEN notation"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
*/
    // Predefined positions
    wxBoxSizer* predefined_positions_sizer  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* predefined_positions_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Training position:"), wxDefaultPosition, wxDefaultSize, 0 );
    //#define NBR_OF_PREDEFINED 3
    //wxString pos_array[NBR_OF_PREDEFINED];
    //pos_array[0] = "";
    //pos_array[1] = "Mate bare king with king and queen";
    //pos_array[2] = "Mate bare king with king and rook";

    // Get predefined positions from book, Return bool error
    int nbr = 0;
    bool error = true;
    if( objs.repository->book.m_enabled )
    {
        error = objs.book->Predefined( labels, fens );
        nbr = labels.GetCount();
    }
    if( !error && nbr>1 )
    {
        labels[0] =  "-- Cancel --";
        combo_label.sprintf( "-- Load one of %d training positions from the book --", nbr-1 );
    }
    else
    {
        labels.Clear();
        fens.Clear();
        labels.Add( "-- Cancel --" );
        fens.  Add( "" );
        labels.Add( "Beginner: Mating K+Q versus bare K" );
        fens.  Add( "8|8|8|4k3|8|8|8|6QK w - - 0 1" );

        labels.Add( "Beginner: Mating K+R versus bare K" );
        fens.  Add( "8|8|8|4k3|8|8|8|6RK w - - 0 1" );

        labels.Add( "Beginner: Winning with extra piece, endgame" );
        fens.  Add( "2b3k1|ppp2ppp|8|8|8|2N5|PPP2PPP|5BK1 w - - 0 1" );

        labels.Add( "Beginner: Winning with extra rook, middlegame" );
        fens.  Add( "2bqk2r|2ppbppp|p4n2|1p2p3|4P3|1PP2N2|1P1P1PPP|RNBQR1K1 w k - 0 1" );

        labels.Add( "Intermediate: Winning with extra piece, middlegame" );
        fens.  Add( "rnb1kb1r|pp1ppppp|2p2n2|6q1|3P4|2N1P3|PPP2PPP|R2QKBNR w KQkq - 0 1" );

        labels.Add( "Intermediate: Lucena Position" );
        fens.  Add( "6K1|4k1P1|8|8|8|8|5R2|6r1 w - - 0 1" );

        labels.Add( "Intermediate: Philidor Position" );
        fens.  Add( "2R5|8|8|8|4pk2|8|r7|4K3 w - - 0 1" );

        labels.Add( "Intermediate: K+Q versus K+P (R pawn on 7th); draw" );
        fens.  Add( "KQ6|8|8|8|8|8|7p|6k1 w - - 0 1" );

        labels.Add( "Intermediate: K+Q versus K+P (B pawn on 7th); draw" );
        fens.  Add( "KQ6|8|8|8|8|8|5p2|6k1 w - - 0 1" );

        labels.Add( "Intermediate: K+Q versus K+P (N pawn on 7th); win" );
        fens.  Add( "KQ6|8|8|8|8|8|6p1|7k w - - 0 1" );

        labels.Add( "Advanced: Converting extra pawn, middlegame" );
        fens.  Add( "r2r2k1|pppnppbp|1n4p1|1NQPP3|5q2|4B2P|PP2BP2|3RK2R w K - 0 1" );

        labels.Add( "Advanced: Converting extra pawn, endgame" );
        fens.  Add( "8|pppr1kpp|8|8|8|5P2|PPP1RKPP|8 w - - 0 1" );
        combo_label.sprintf( "-- Load one of %d predefined positions --", (int)labels.GetCount()-1 );
    }

    predefined_positions_ctrl = new wxComboBox( this, ID_PREDEFINED,
        combo_label, wxDefaultPosition,
        wxSize(wxDefaultCoord, wxDefaultCoord), labels, wxCB_DROPDOWN ); //wxCB_READONLY );
    predefined_positions_ctrl->SetValue(combo_label);
    predefined_positions_sizer->Add(predefined_positions_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    predefined_positions_sizer->Add(10, 5, 1, wxALL, 0);
    predefined_positions_sizer->Add(predefined_positions_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    box_sizer->Add(predefined_positions_sizer, 0, wxALIGN_LEFT|wxALL, 5);

    // The board setup bitmap
    bsc = new BoardSetupControl(true,false,this);
    bsc->Set( m_pos );

    // Intermediate sizers
    wxBoxSizer* horiz_board = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* horiz_extra = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Castling available ?" );
    wxSizer     *castling_box = new wxStaticBoxSizer(box, wxVERTICAL);
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, "&Who to move" );
    wxSizer     *who_box = new wxStaticBoxSizer(box2, wxVERTICAL);
    wxStaticBox *box3 = new wxStaticBox(this, wxID_ANY, "&Other details" );
    wxSizer     *details_box = new wxStaticBoxSizer(box3, wxVERTICAL);

    white_oo = new wxCheckBox( this, ID_WHITE_OO,
       wxT("&White O-O"), wxDefaultPosition, wxDefaultSize, 0 );
    white_oo->SetValue( m_pos.wking_allowed() );
    white_ooo = new wxCheckBox( this, ID_WHITE_OOO,
       wxT("&White O-O-O"), wxDefaultPosition, wxDefaultSize, 0 );
    white_ooo->SetValue( m_pos.wqueen_allowed() );
    black_oo = new wxCheckBox( this, ID_BLACK_OO,
       wxT("&Black O-O"), wxDefaultPosition, wxDefaultSize, 0 );
    black_oo->SetValue( m_pos.bking_allowed() );
    black_ooo = new wxCheckBox( this, ID_BLACK_OOO,
       wxT("&Black O-O-O"), wxDefaultPosition, wxDefaultSize, 0 );
    black_ooo->SetValue( m_pos.bqueen_allowed() );

    white_to_move = new wxRadioButton( this, ID_WHITE_TO_MOVE,
       wxT("&White to move"), wxDefaultPosition, wxDefaultSize,  wxRB_GROUP );
    black_to_move = new wxRadioButton( this, ID_BLACK_TO_MOVE,
       wxT("&Black to move"), wxDefaultPosition, wxDefaultSize, 0 );
    white_to_move->SetValue( m_pos.white );
    black_to_move->SetValue( !m_pos.white );
  
    // Move count
    wxBoxSizer* move_count_sizer  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* move_count_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Move count"), wxDefaultPosition, wxDefaultSize, 0 );
    move_count_ctrl = new wxSpinCtrl ( this, ID_MOVE_COUNT,
        wxEmptyString, wxDefaultPosition, wxSize(50, wxDefaultCoord), //wxDefaultSize, 
        wxSP_ARROW_KEYS, m_pos.full_move_count, 500, 1 );
    move_count_sizer->Add(move_count_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    move_count_sizer->Add(92, 5, 1, wxALL, 0);
    move_count_sizer->Add(move_count_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
/*  itemSizer2->Add(new wxStaticText(this, wxID_ANY, _("&Window:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    itemSizer2->Add(5, 5, 1, wxALL, 0);
    itemSizer2->Add(choice2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5); */
  
    // Half moves since pawn move or capture
    wxBoxSizer* half_count_sizer  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* half_count_label = new wxStaticText ( this, wxID_STATIC,
        wxT("50 move rule count"), wxDefaultPosition, wxDefaultSize, 0 );
    half_count_ctrl = new wxSpinCtrl ( this, ID_HALF_COUNT,
        wxEmptyString, wxDefaultPosition, wxSize(50, wxDefaultCoord), //wxDefaultSize, 
        wxSP_ARROW_KEYS, m_pos.half_move_clock, 100, 0 );
    half_count_sizer->Add(half_count_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    half_count_sizer->Add(57, 5, 1, wxALL, 0);
    half_count_sizer->Add(half_count_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
  
    // En passant target file
    wxBoxSizer* en_passant_sizer  = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* en_passant_label = new wxStaticText ( this, wxID_STATIC,
        wxT("En passant target file"), wxDefaultPosition, wxDefaultSize, 0 );
    wxString file_array[9];
    file_array[0] = "None";
    file_array[1] = "a";
    file_array[2] = "b";
    file_array[3] = "c";
    file_array[4] = "d";
    file_array[5] = "e";
    file_array[6] = "f";
    file_array[7] = "g";
    file_array[8] = "h";
    en_passant_ctrl = new wxComboBox ( this, ID_EN_PASSANT,
        "None", wxDefaultPosition,
        wxSize(50, wxDefaultCoord), 9, file_array, wxCB_READONLY );
    wxString en_passant;
    if( m_pos.white && a6<=m_pos.enpassant_target && m_pos.enpassant_target<=h6 )
        en_passant.sprintf( "%c", 'a' + (m_pos.enpassant_target-a6) );
    else if( !m_pos.white && a3<=m_pos.enpassant_target && m_pos.enpassant_target<=h3 )
        en_passant.sprintf( "%c", 'a' + (m_pos.enpassant_target-a3) );
    else
        en_passant = "None";
    en_passant_ctrl->SetValue(en_passant);
    en_passant_sizer->Add(en_passant_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    en_passant_sizer->Add(44, 5, 1, wxALL, 0);
    en_passant_sizer->Add(en_passant_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
   
    castling_box->Add( white_oo, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( white_ooo, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( black_oo, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( black_ooo, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    who_box->Add( white_to_move, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    who_box->Add( black_to_move, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);

    details_box->Add(move_count_sizer, 
        wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
        //wxALIGN_CENTER_VERTICAL|wxALL, 5);
    details_box->Add(half_count_sizer,
        wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
        //wxALIGN_CENTER_VERTICAL|wxALL, 5);
    details_box->Add(en_passant_sizer,
        wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
        //wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // A vertical box sizer to contain Reset, Clear, OK, Cancel and Help
    wxBoxSizer* buttons_box = new wxBoxSizer(wxVERTICAL);

    // The Initial button
    wxButton* reset = new wxButton( this, ID_POSITION_RESET, wxT("&Initial"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( reset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Current button
    wxButton* current = new wxButton( this, ID_POSITION_CURRENT, wxT("&Current"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( current, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Clear button
    wxButton* clear = new wxButton( this, ID_POSITION_CLEAR, wxT("&Clear"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( clear, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Chess960 button
    wxButton* chess_960 = new wxButton( this, ID_POSITION_960, wxT("Shuffle"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( chess_960, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    buttons_box->AddSpacer(50);

    // The OK button
    ok_button = new wxButton ( this, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( ok_button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
        wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Help button
    wxButton* help = new wxButton( this, wxID_HELP, wxT("&Help"),
        wxDefaultPosition, wxDefaultSize, 0 );
    buttons_box->Add( help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //box_sizer->Add( buttons_box, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    horiz_board->Add( bsc, 1, wxALIGN_LEFT|wxALL|wxFIXED_MINSIZE, 5 );
    horiz_board->Add( buttons_box, 0, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);

    horiz_extra->Add(who_box,       1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxTOP  */), 5);
    horiz_extra->Add(castling_box,  1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
    horiz_extra->Add(details_box,   0, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);

    box_sizer->Add( horiz_board,  0, wxALIGN_LEFT |wxALL, 5 );
    box_sizer->Add( horiz_extra,  0, wxALIGN_LEFT |wxLEFT|wxRIGHT|wxTOP, 5 );

    // The Apply button
    wxButton* apply = new wxButton( this, ID_APPLY, wxT("&Apply"),
        wxDefaultPosition, wxDefaultSize, 0 );

    // Text control for fen entry
    fen_ctrl = new wxTextCtrl ( this, ID_FEN, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxSize sz=fen_ctrl->GetSize();
    fen_ctrl->SetSize((sz.x*118)/32,sz.y);      // temp temp
    //fen_ctrl->SetSize((sz.x*7)/2,sz.y);      // temp temp
    fen_ctrl->SetValue(fen);

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
    wxStaticBox *box4 = new wxStaticBox(this, wxID_ANY, "&FEN (Forsyth-Edwards Notation)" );
    wxSizer     *fen_box = new wxStaticBoxSizer(box4, /*wxGROW|*/wxHORIZONTAL);
/*  fen_box->Add( fen_ctrl,  0, wxALIGN_LEFT|wxALL|wxFIXED_MINSIZE, 5);
    fen_box->Add( apply,     0, wxALIGN_RIGHT|wxALL, 5 );
    box_sizer->Add( fen_box, 1, wxALIGN_LEFT|wxGROW|wxALL, 5 ); */
    fen_box->Add( fen_ctrl,  0, wxALIGN_CENTER_VERTICAL|wxALL|wxFIXED_MINSIZE, 2);
    fen_box->AddSpacer(5);
    fen_box->Add( apply,     0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );
    box_sizer->Add( fen_box, 0, wxALIGN_LEFT|wxGROW|wxALL, 10 );

/*  move_count_sizer->Add(move_count_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    move_count_sizer->Add(95, 5, 1, wxALL, 0);
    move_count_sizer->Add(move_count_ctrl,  0, wxALL|wxALIGN_CENTER_VERTICAL, 5); */

/*    wxSize sz=box4->GetSize();
    box4->SetSize(sz.x,sz.y/2);
    wxPoint pt = apply->GetPosition();
    pt.x += 150;
    apply->SetPosition(pt); */

/*
    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);
*/
    clear->SetFocus();
}

// Set the validators for the dialog controls
void PositionDialog::SetDialogValidators()
{
    FindWindow(ID_FEN)           -> SetValidator( wxTextValidator(wxFILTER_ASCII, &fen));
}

// Sets the help text for the dialog controls
void PositionDialog::SetDialogHelp()
{
    wxString fen_help  = wxT("A FEN format position specification");
    FindWindow(ID_FEN)->SetHelpText(fen_help);
    FindWindow(ID_FEN)->SetToolTip(fen_help);
    wxString initial_help = "Set the standard chess initial position";
    FindWindow(ID_POSITION_RESET)->SetHelpText(initial_help);
    FindWindow(ID_POSITION_RESET)->SetToolTip(initial_help);
    wxString clear_help = "Clear the board";
    FindWindow(ID_POSITION_CLEAR)->SetHelpText(clear_help);
    FindWindow(ID_POSITION_CLEAR)->SetToolTip(clear_help);
    wxString chess_960_help = "Create shuffle chess position (note: \"Shuffle\" not \"Chess960\" since Tarrasch doesn't understand Chess960 castling)";
    FindWindow(ID_POSITION_960)->SetHelpText(chess_960_help);
    FindWindow(ID_POSITION_960)->SetToolTip(chess_960_help);
    wxString current_help = "Copy from the current main board position";
    FindWindow(ID_POSITION_CURRENT)->SetHelpText(current_help);
    FindWindow(ID_POSITION_CURRENT)->SetToolTip(current_help);
    wxString apply_help = "Apply the fen string (useful after hand editing)";
    FindWindow(ID_APPLY)->SetHelpText(apply_help);
    FindWindow(ID_APPLY)->SetToolTip(apply_help);
    wxString woo_help = "Set if white kingside castling possible";
    FindWindow(ID_WHITE_OO)->SetHelpText(woo_help);
    FindWindow(ID_WHITE_OO)->SetToolTip(woo_help);
    wxString wooo_help = "Set if white queenside castling possible";
    FindWindow(ID_WHITE_OOO)->SetHelpText(wooo_help);
    FindWindow(ID_WHITE_OOO)->SetToolTip(wooo_help);
    wxString boo_help = "Set if black kingside castling possible";
    FindWindow(ID_BLACK_OO)->SetHelpText(boo_help);
    FindWindow(ID_BLACK_OO)->SetToolTip(boo_help);
    wxString booo_help = "Set if black queenside castling possible";
    FindWindow(ID_BLACK_OOO)->SetHelpText(booo_help);
    FindWindow(ID_BLACK_OOO)->SetToolTip(booo_help);
    wxString white_help = "Set if white to move";
    FindWindow(ID_WHITE_TO_MOVE)->SetHelpText(white_help);
    FindWindow(ID_WHITE_TO_MOVE)->SetToolTip(white_help);
    wxString black_help = "Set if black to move";
    FindWindow(ID_BLACK_TO_MOVE)->SetHelpText(black_help);
    FindWindow(ID_BLACK_TO_MOVE)->SetToolTip(black_help);
    wxString move_count_help = "Numeric label for a move made in this position. E.g. if next move will be 23... Nd5, set this to 23";
    FindWindow(ID_MOVE_COUNT)->SetHelpText(move_count_help);
    FindWindow(ID_MOVE_COUNT)->SetToolTip(move_count_help);
    wxString half_clock_help = "Half moves made since last capture or pawn move, a draw can be claimed when the count gets to 100";
    FindWindow(ID_HALF_COUNT)->SetHelpText(half_clock_help);
    FindWindow(ID_HALF_COUNT)->SetToolTip(half_clock_help);
    wxString ep_help = "Set this if the last move makes an enpassant capture possible next move";
    FindWindow(ID_EN_PASSANT)->SetHelpText(ep_help);
    FindWindow(ID_EN_PASSANT)->SetToolTip(ep_help);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_RESET
void PositionDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessPosition tmp;
    m_pos = tmp;
    Pos2Fen( m_pos, fen );
    WriteToControls();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_CURRENT
void PositionDialog::OnCurrentClick( wxCommandEvent& WXUNUSED(event) )
{
    m_pos = objs.gl->gd.master_position;
    Pos2Fen( m_pos, fen );
    WriteToControls();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_CLEAR
void PositionDialog::OnClearClick( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessPosition tmp;
    m_pos = tmp;
    memset( m_pos.squares, ' ', 64 );
    m_pos.wking  = false;
    m_pos.wqueen = false;
    m_pos.bking  = false;
    m_pos.bqueen = false;
    Pos2Fen( m_pos, fen );
    WriteToControls();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_960
void PositionDialog::OnChess960Click( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessPosition tmp;
    memset( tmp.squares, ' ', 8 );

    // Light squared bishop, one of 4 squares
    int x = rand();
    int idx = (x%4)*2;                  // 0,2,4 or 6     
    tmp.squares[idx]       = 'b';
    tmp.squares[idx + 7*8] = 'B';

    // Dark squared bishop, one of 4 squares
    x = rand();
    idx = 1 + (x%4)*2;                  // 1,3,5 or 7
    tmp.squares[idx]       = 'b';
    tmp.squares[idx + 7*8] = 'B';

    // Queen, one of 6 remaining squares
    x = rand();                         
    idx = (x%6);
    for( int i=0; i<8; i++ )
    {
        if( tmp.squares[i] == ' ' )
        {
            if( idx )
                idx--;
            else
            {
                tmp.squares[i]       = 'q';
                tmp.squares[i + 7*8] = 'Q';
                break;
            }
        }
    }
            
    // 1st knight, one of 5 remaining squares
    x = rand();                         
    idx = (x%5);
    for( int i=0; i<8; i++ )
    {
        if( tmp.squares[i] == ' ' )
        {
            if( idx )
                idx--;
            else
            {
                tmp.squares[i]       = 'n';
                tmp.squares[i + 7*8] = 'N';
                break;
            }
        }
    }
            
    // 2nd knight, one of 4 remaining squares
    x = rand();                         
    idx = (x%4);
    for( int i=0; i<8; i++ )
    {
        if( tmp.squares[i] == ' ' )
        {
            if( idx )
                idx--;
            else
            {
                tmp.squares[i]       = 'n';
                tmp.squares[i + 7*8] = 'N';
                break;
            }
        }
    }
            
    // Remaining squares - fill with 'r', 'k', 'r'
    idx = 0;
    for( int i=0; i<8; i++ )
    {
        if( tmp.squares[i] == ' ' )
        {
            switch( idx++ )
            {
                default:
                case 0:
                case 2:
                {
                    tmp.squares[i]       = 'r';
                    tmp.squares[i + 7*8] = 'R';
                    break;
                }
                case 1:
                {
                    tmp.squares[i]       = 'k';
                    tmp.squares[i + 7*8] = 'K';
                    break;
                }
            }
        }
    }

    m_pos = tmp;
    m_pos.wking  = false;
    m_pos.wqueen = false;
    m_pos.bking  = false;
    m_pos.bqueen = false;
    Pos2Fen( m_pos, fen );
    WriteToControls();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_APPLY
void PositionDialog::OnApplyClick( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessRules cr;
    bool legal=true;
    ILLEGAL_REASON reason;
    fen = fen_ctrl->GetValue();
    if( !cr.Forsyth( fen.c_str()) )
    {
        legal = false;
        wxMessageDialog md(this,"FEN string is not properly formed", "Error", wxOK|wxICON_ERROR);
        md.ShowModal();
    }
    else
    {
        legal = cr.IsLegal(reason);
        if( !legal )
            IllegalPositionDialog( "FEN Specifies Illegal Position", cr,reason);
    }
    if( legal )
    {
        m_pos = cr;
        Pos2Fen( m_pos, fen );
        WriteToControls();
    }
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void PositionDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to specify a new position. ")
      wxT("Note that the orientation of the board is the same as the main ")
      wxT("program board. So it is now possible to set up a position from ")
      wxT("Black's point of view (flip the main board first).");
    wxMessageBox(helpText,
      wxT("Position Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

void PositionDialog::IllegalPositionDialog( const char *caption, thc::ChessRules &cr, ILLEGAL_REASON reason )
{
    wxString msg = "Reason(s) the position is illegal;        \n";
    if( reason & IR_PAWN_POSITION )
        msg += " Pawn(s) are on the first or last rank\n";
    if( reason & IR_NOT_ONE_KING_EACH )
        msg += " Both sides should have one king\n";
    if( reason & IR_CAN_TAKE_KING )
    {
        if( cr.WhiteToPlay() )
            msg += " White to play can capture Black's king\n";
        else
            msg += " Black to play can capture White's king\n";
    }
    if( reason & IR_WHITE_TOO_MANY_PIECES )
        msg += " White has too many pieces\n";
    if( reason & IR_BLACK_TOO_MANY_PIECES )
        msg += " Black has too many pieces\n";
    if( reason & IR_WHITE_TOO_MANY_PAWNS )
        msg += " White has too many pawns\n";
    if( reason & IR_BLACK_TOO_MANY_PAWNS )
        msg += " Black has too many pawns\n";
    wxMessageDialog md(this,msg,caption,wxOK|wxICON_ERROR);
    md.ShowModal();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void PositionDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    bool err=false;
    if( bsc )
    {
        thc::ChessRules cr = m_pos;
        ILLEGAL_REASON reason;
        if( !cr.IsLegal(reason) )
        {
            err = true;
            IllegalPositionDialog("Illegal Position", cr,reason);
        }
        else
        {
            char forsyth[128];
            strcpy( forsyth, cr.ForsythPublish().c_str() );
            fen = forsyth;
            TransferDataToWindow();    
        }
    }
    if( !err )
        AcceptAndClose();
}

void PositionDialog::OnSpin( wxSpinEvent& WXUNUSED(event) )
{
    ModifyFen();
}

void PositionDialog::OnCheckBox( wxCommandEvent& WXUNUSED(event) )
{
    ModifyFen();
}

void PositionDialog::OnRadio( wxCommandEvent& WXUNUSED(event) )
{
    ModifyFen();
}

void PositionDialog::OnComboBox( wxCommandEvent& WXUNUSED(event) )
{
    ModifyFen();
}

void PositionDialog::OnPredefined( wxCommandEvent& WXUNUSED(event) )
{
    bool found=false;
    wxString txt = this->predefined_positions_ctrl->GetValue();
    // char piece=' ';
    for( unsigned int i=1; !found && i<labels.GetCount(); i++ )
    {
        if( txt == labels[i] )
        {
            thc::ChessRules cr;
            wxString sfen = fens[i];
            if( cr.Forsyth(sfen.c_str()) )
            {
                found = true;
                this->fen = sfen;
                m_pos = cr;
                bsc->ClearCustomCursor();
                WriteToControls();
            }
        }
    }
    if( !found )
        predefined_positions_ctrl->SetValue(combo_label);
    ok_button->SetFocus();
}

// Later - learn how to do this by sending an event to parent instead
void PositionSetupVeryUglyTemporaryCallback()
{
    PositionDialog *ptr = singleton;
    if( ptr )
        ptr->ModifyFen();
}

static bool Castling( const thc::ChessPosition &cp, char KkQq )
{
    bool ret=false;
    switch( KkQq )
    {
        default:  ret = false;
        case 'q': ret = (cp.squares[0]=='r' &&  cp.squares[4]=='k');   break;
        case 'k': ret = (cp.squares[7]=='r' &&  cp.squares[4]=='k');   break;
        case 'Q': ret = (cp.squares[56]=='R' && cp.squares[60]=='K');  break;
        case 'K': ret = (cp.squares[63]=='R' && cp.squares[60]=='K');  break;
    }
    return ret;
}
void PositionDialog::ModifyFen()
{
    if( Castling(bsc->cp,'q') && !Castling(m_pos,'q') )
        this->black_ooo->SetValue(true);
    else if( !Castling(bsc->cp,'q') && Castling(m_pos,'q') )
        this->black_ooo->SetValue(false);
    if( Castling(bsc->cp,'k') && !Castling(m_pos,'k') )
        this->black_oo->SetValue(true);
    else if( !Castling(bsc->cp,'k') && Castling(m_pos,'k') )
        this->black_oo->SetValue(false);
    if( Castling(bsc->cp,'Q') && !Castling(m_pos,'Q') )
        this->white_ooo->SetValue(true);
    else if( !Castling(bsc->cp,'Q') && Castling(m_pos,'Q') )
        this->white_ooo->SetValue(false);
    if( Castling(bsc->cp,'K') && !Castling(m_pos,'K') )
        this->white_oo->SetValue(true);
    else if( !Castling(bsc->cp,'K') && Castling(m_pos,'K') )
        this->white_oo->SetValue(false);
    m_pos = bsc->cp;
    if( !Castling(m_pos,'q') )
        this->black_ooo->SetValue(false);
    if( !Castling(m_pos,'k') )
        this->black_oo->SetValue(false);
    if( !Castling(m_pos,'Q') )
        this->white_ooo->SetValue(false);
    if( !Castling(m_pos,'K') )
        this->white_oo->SetValue(false);
    m_pos.wqueen = this->white_ooo->GetValue();
    m_pos.wking  = this->white_oo->GetValue();
    m_pos.bqueen = this->black_ooo->GetValue();
    m_pos.bking  = this->black_oo->GetValue();

    m_pos.full_move_count = move_count_ctrl->GetValue();    
    m_pos.half_move_clock = half_count_ctrl->GetValue();    
    m_pos.white  = !black_to_move->GetValue();
    m_pos.wqueen = this->white_ooo->GetValue();
    m_pos.wking  = this->white_oo->GetValue();
    m_pos.bqueen = this->black_ooo->GetValue();
    m_pos.bking  = this->black_oo->GetValue();
    wxString ep  = this->en_passant_ctrl->GetValue();
    m_pos.enpassant_target = SQUARE_INVALID;
    if( ep!="None" && 'a'<=ep[0] && ep[0]<='h' )
        m_pos.enpassant_target = (Square)( (m_pos.white?a6:a3) + (ep[0]-'a') );
    Pos2Fen( m_pos, fen );
    fen_ctrl->SetValue( fen );
}
