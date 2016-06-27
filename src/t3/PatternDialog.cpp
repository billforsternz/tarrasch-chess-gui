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
END_EVENT_TABLE()

// Constructor
static PatternDialog *singleton;   // for DatabaseSearchVeryUglyTemporaryCallback();
PatternDialog::PatternDialog
(
    PatternParameters *parm, wxWindow* parent,
    const wxString& caption,
    wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style
)
{
    singleton = this;
    bsc = NULL;
    this->parm = parm;
    support_lockdown = (id==ID_MATERIAL_BALANCE_DIALOG);
    offset_persist = 0;

    // pattern
    b_either = !parm->material_balance;
    b_white  = !parm->material_balance;
    b_black  = !parm->material_balance;
    b_allow_more = !parm->material_balance;

    // material balance
    b_piece_map = parm->material_balance;
    b_pawns   = parm->material_balance;
    b_bishops = parm->material_balance;
    b_ply     = parm->material_balance;
    Create(parent, caption, id, pos, size, style);
}

// Dialog create
bool PatternDialog::Create( wxWindow* parent,
  const wxString& caption,
  wxWindowID id,
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

    // The board setup bitmap
    bsc = new BoardSetupControl(false,support_lockdown,this);
    bsc->Set( parm->cp, parm->lockdown );

    // Intermediate sizers
    wxBoxSizer* horiz_board = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* horiz_extra = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, "&Refine search" );
    wxSizer     *castling_box = new wxStaticBoxSizer(box, wxVERTICAL);
    wxSizer     *who_box = NULL;
    if( b_white )
    {
        wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, "&Who to move" );
        who_box = new wxStaticBoxSizer(box2, wxVERTICAL);
    }
    //wxStaticBox *box3 = new wxStaticBox(this, wxID_ANY, "&Other details" );
    //wxSizer     *details_box = new wxStaticBoxSizer(box3, wxVERTICAL);
    inc_reflection = new wxCheckBox( this, ID_PATTERN_INC_REFLECTION,
       wxT("&Include reflections"), wxDefaultPosition, wxDefaultSize, 0 );
    inc_reflection->SetValue( parm->include_reflections );
    inc_reverse = new wxCheckBox( this, ID_PATTERN_INC_REVERSE,
       wxT("&Include reversed colours"), wxDefaultPosition, wxDefaultSize, 0 );
    inc_reverse->SetValue( parm->include_reverse_colours );
    if( b_allow_more )
    {
        allow_more = new wxCheckBox( this, ID_PATTERN_ALLOW_MORE,
           wxT("&Allow more pieces"), wxDefaultPosition, wxDefaultSize, 0 );
        allow_more->SetValue( parm->allow_more_pieces );
    }
    if( b_pawns )
    {
        pawns_same_files = new wxCheckBox( this, ID_PATTERN_PAWNS_SAME_FILES,
           wxT("&Pawns must be on same files"), wxDefaultPosition, wxDefaultSize, 0 );
        pawns_same_files->SetValue( parm->pawns_must_be_on_same_files );
    }
    if( b_bishops )
    {
        bishops_same_colour = new wxCheckBox( this, ID_PATTERN_BISHOPS_SAME_COLOUR,
           wxT("&Bishops must be same colour"), wxDefaultPosition, wxDefaultSize, 0 );
        bishops_same_colour->SetValue( parm->bishops_must_be_same_colour );
    }
    if( b_white )
    {
        white_to_move = new wxRadioButton( this, ID_PATTERN_WHITE_TO_MOVE,
           wxT("&White to move"), wxDefaultPosition, wxDefaultSize,  wxRB_GROUP );
        white_to_move->SetValue( parm->either_to_move ? false : parm->white_to_move );
        black_to_move = new wxRadioButton( this, ID_PATTERN_BLACK_TO_MOVE,
           wxT("&Black to move"), wxDefaultPosition, wxDefaultSize, 0 );
        black_to_move->SetValue( parm->either_to_move ? false : !parm->white_to_move );
        either_to_move = new wxRadioButton( this, ID_PATTERN_EITHER_TO_MOVE,
           wxT("&Either to move"), wxDefaultPosition, wxDefaultSize, 0 );
        either_to_move->SetValue( parm->either_to_move );
    }

    // Move count
    wxBoxSizer* move_count_sizer = NULL;;
    if( b_ply )
    {
        move_count_sizer  = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* move_count_label = new wxStaticText ( this, wxID_STATIC,
            wxT("Balance must persist for at\nleast this many half moves"), wxDefaultPosition, wxDefaultSize, 0 );
        move_count_ctrl = new wxSpinCtrl ( this, ID_PATTERN_MOVE_COUNT,
            wxEmptyString, wxDefaultPosition, wxSize(50, wxDefaultCoord), //wxDefaultSize, 
            wxSP_ARROW_KEYS, parm->number_of_ply, 500, 1 );
        move_count_sizer->Add(move_count_label, 0, wxBOTTOM|wxALIGN_CENTER_VERTICAL, 10);
        move_count_sizer->Add( 10, 5, 1, wxALL, 0);
        move_count_sizer->Add(move_count_ctrl,  0, wxBOTTOM|wxALIGN_CENTER_VERTICAL, 10);
    }
   
    castling_box->Add( inc_reflection, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    castling_box->Add( inc_reverse, 0,
        wxALIGN_CENTER_VERTICAL|wxALL, 5);
    if( b_allow_more )
    {
        castling_box->Add( allow_more, 0,
            wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    if( b_pawns )
    {
        castling_box->Add( pawns_same_files, 0,
            wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    if( b_bishops )
    {
        castling_box->Add( bishops_same_colour, 0,
            wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    if( b_ply )
    {
        castling_box->Add(move_count_sizer, 
         wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
         //wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    if( b_white )
    { 
        who_box->Add( white_to_move, 0,
            wxALIGN_CENTER_VERTICAL|wxALL, 5);
        who_box->Add( black_to_move, 0,
            wxALIGN_CENTER_VERTICAL|wxALL, 5);
        who_box->Add( either_to_move, 0,
            wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

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

    //box_sizer->Add( buttons_box, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    horiz_board->Add( bsc, 1, wxALIGN_LEFT|wxALL|wxFIXED_MINSIZE, 5 );
    horiz_board->Add( buttons_box, 0, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);


    if( b_piece_map )
    {
        wxStaticBox *lockdown = new wxStaticBox(this, wxID_ANY, "&Optionally right click to lockdown squares" );
        wxSizer     *lockdown_vert  = new wxStaticBoxSizer(lockdown, wxHORIZONTAL );
        lockdown_text = new wxStaticText(this, wxID_ANY, "Currently there are no locked down squares" );   
        wxStaticBox *more_pieces = new wxStaticBox(this, wxID_ANY, "&Optionally allow some extra material" );
        wxSizer     *more_pieces_vert  = new wxStaticBoxSizer(more_pieces, wxVERTICAL );   
        wxStaticBox *more_pieces1 = new wxStaticBox(this, wxID_ANY, "&White" );
        wxStaticBox *more_pieces2 = new wxStaticBox(this, wxID_ANY, "&Black" );
        wxSizer     *more_pieces_horiz1 = new wxStaticBoxSizer(more_pieces1,wxHORIZONTAL);
        wxSizer     *more_pieces_horiz2 = new wxStaticBoxSizer(more_pieces2,wxHORIZONTAL);
        more_pieces_vert->Add( more_pieces_horiz1,  0, wxALIGN_LEFT |wxRIGHT, 80 /*spacer*/  );
        more_pieces_vert->Add( more_pieces_horiz2,  0, wxALIGN_LEFT |wxLEFT|wxRIGHT|wxTOP, 0 );
        more_pieces_wq = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_WQ,
           "Q", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_wq->SetValue( parm->more_pieces_wq );
        more_pieces_wr = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_WR,
           "R", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_wr->SetValue( parm->more_pieces_wr );
        more_pieces_wb = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_WB,
           "B", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_wb->SetValue( parm->more_pieces_wb );
        more_pieces_wn = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_WN,
           "N", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_wn->SetValue( parm->more_pieces_wn );
        more_pieces_wp = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_WP,
           "P", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_wp->SetValue( parm->more_pieces_wp );
        more_pieces_horiz1->Add( more_pieces_wq, 0,
            wxALIGN_CENTER_VERTICAL|wxLEFT, 15);
        more_pieces_horiz1->Add( more_pieces_wr, 0,
            wxALIGN_CENTER_VERTICAL,0);
        more_pieces_horiz1->Add( more_pieces_wb, 0,
            wxALIGN_CENTER_VERTICAL,0);
        more_pieces_horiz1->Add( more_pieces_wn, 0,
            wxALIGN_CENTER_VERTICAL,0);
        more_pieces_horiz1->Add( more_pieces_wp, 0,
            wxALIGN_CENTER_VERTICAL,0);
        more_pieces_bq = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_BQ,
           "Q", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_bq->SetValue( parm->more_pieces_bq );
        more_pieces_br = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_BR,
           "R", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_br->SetValue( parm->more_pieces_br );
        more_pieces_bb = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_BB,
           "B", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_bb->SetValue( parm->more_pieces_bb );
        more_pieces_bn = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_BN,
           "N", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_bn->SetValue( parm->more_pieces_bn );
        more_pieces_bp = new wxCheckBox( this, ID_PATTERN_MORE_PIECES_BP,
           "P", wxDefaultPosition, wxDefaultSize, 0 );
        more_pieces_bp->SetValue( parm->more_pieces_bp );
        more_pieces_horiz2->Add( more_pieces_bq, 0,
            wxALIGN_CENTER_VERTICAL|wxLEFT, 15);
        more_pieces_horiz2->Add( more_pieces_br, 0,
            wxALIGN_CENTER_VERTICAL,0);
        more_pieces_horiz2->Add( more_pieces_bb, 0,
            wxALIGN_CENTER_VERTICAL,0);
        more_pieces_horiz2->Add( more_pieces_bn, 0,
            wxALIGN_CENTER_VERTICAL,0);
        more_pieces_horiz2->Add( more_pieces_bp, 0,
            wxALIGN_CENTER_VERTICAL,0);
        lockdown_vert->Add( lockdown_text,  0, wxALIGN_LEFT |wxLEFT|wxTOP, 12 );
        wxBoxSizer* vert_extra  = new wxBoxSizer(wxVERTICAL);
        vert_extra->Add(more_pieces_vert,  1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
        vert_extra->Add(lockdown_vert,  1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
        horiz_extra->Add( vert_extra,  0, wxALIGN_LEFT |wxLEFT|wxRIGHT|wxTOP, 0 );
    }
    if( b_white )
        horiz_extra->Add(who_box,       1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxTOP  */), 5);
    horiz_extra->Add(castling_box,  1, wxALIGN_LEFT|wxGROW | (wxALL/* & ~wxLEFT */), 5);
    box_sizer->Add( horiz_board,  0, wxALIGN_LEFT |wxALL, 5 );
    box_sizer->Add( horiz_extra,  0, wxALIGN_LEFT |wxLEFT|wxRIGHT|wxTOP, 5 );
    if( b_piece_map )
        ModifyLockdown( -1 );
    clear->SetFocus();
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
    if( parm->material_balance )
        help3 = "Set to allow extra pieces - only useful for patterns with extra promoted pieces - eg two queens";
    if( b_allow_more )
    {
        FindWindow(ID_PATTERN_ALLOW_MORE)->SetHelpText(help3);
        FindWindow(ID_PATTERN_ALLOW_MORE)->SetToolTip(help3);
    }
    if( b_pawns )
    {
        wxString help4 = "Set to specify that games only match if pawns are on the same files as the search pattern";
        FindWindow(ID_PATTERN_PAWNS_SAME_FILES)->SetHelpText(help4);
        FindWindow(ID_PATTERN_PAWNS_SAME_FILES)->SetToolTip(help4);
    }
    if( b_bishops )
    {
        wxString help5 = "Set this if you want light squared and dark squared bishops to be distinct pieces for search purposes";
        FindWindow(ID_PATTERN_BISHOPS_SAME_COLOUR)->SetHelpText(help5);
        FindWindow(ID_PATTERN_BISHOPS_SAME_COLOUR)->SetToolTip(help5);
    }
    if( b_white )
    {
        wxString white_help = "Set to find positions with white to move";
        FindWindow(ID_PATTERN_WHITE_TO_MOVE)->SetHelpText(white_help);
        FindWindow(ID_PATTERN_WHITE_TO_MOVE)->SetToolTip(white_help);
        wxString black_help = "Set to find positions with black to move";
        FindWindow(ID_PATTERN_BLACK_TO_MOVE)->SetHelpText(black_help);
        FindWindow(ID_PATTERN_BLACK_TO_MOVE)->SetToolTip(black_help);
        wxString either_help = "Set if you don't care which side is to move";
        FindWindow(ID_PATTERN_EITHER_TO_MOVE)->SetHelpText(either_help);
        FindWindow(ID_PATTERN_EITHER_TO_MOVE)->SetToolTip(either_help);
    }
    if( b_ply )
    {
        wxString move_count_help = "Games only match if the specified material balance persists for at least this many half moves";
        FindWindow(ID_PATTERN_MOVE_COUNT)->SetHelpText(move_count_help);
        FindWindow(ID_PATTERN_MOVE_COUNT)->SetToolTip(move_count_help);
    }
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_RESET
void PatternDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessPosition tmp;
    parm->cp = tmp;
    bsc->Set( parm->cp );
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_CURRENT
void PatternDialog::OnCurrentClick( wxCommandEvent& WXUNUSED(event) )
{
    parm->cp = objs.gl->gd.master_position;
    bsc->Set( parm->cp );
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_CLEAR
void PatternDialog::OnClearClick( wxCommandEvent& WXUNUSED(event) )
{
    thc::ChessPosition tmp;
    parm->cp = tmp;
    memset( parm->cp.squares, ' ', 64 );
    bsc->Set( parm->cp );
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
        parm->cp = bsc->cp;
        memcpy( parm->lockdown, bsc->lockdown, sizeof(parm->lockdown) );
        parm->include_reverse_colours       = inc_reverse->GetValue();
        parm->include_reflections           = inc_reflection->GetValue();
        if( b_piece_map )
        {
            parm->more_pieces_wq = more_pieces_wq->GetValue();
            parm->more_pieces_wr = more_pieces_wr->GetValue();
            parm->more_pieces_wb = more_pieces_wb->GetValue();
            parm->more_pieces_wn = more_pieces_wn->GetValue();
            parm->more_pieces_wp = more_pieces_wp->GetValue();
            parm->more_pieces_bq = more_pieces_bq->GetValue();
            parm->more_pieces_br = more_pieces_br->GetValue();
            parm->more_pieces_bb = more_pieces_bb->GetValue();
            parm->more_pieces_bn = more_pieces_bn->GetValue();
            parm->more_pieces_bp = more_pieces_bp->GetValue();
        }
        if( b_allow_more )
            parm->allow_more_pieces             = allow_more->GetValue();
        if( b_pawns )
            parm->pawns_must_be_on_same_files   = pawns_same_files->GetValue();
        if( b_bishops )
            parm->bishops_must_be_same_colour   = bishops_same_colour->GetValue();
        if( b_ply )
            parm->number_of_ply                 = move_count_ctrl->GetValue();    
        if( b_white )
        {
            parm->white_to_move                 = white_to_move->GetValue();
            parm->either_to_move                = either_to_move->GetValue();
        }
        TransferDataToWindow();    
    }
    if( !err )
        AcceptAndClose();
}

// Later - learn how to do this by sending an event to parent instead
void DatabaseSearchVeryUglyTemporaryCallback( int offset )
{
    PatternDialog *ptr = singleton;
    if( ptr )
        ptr->ModifyLockdown( offset );
}

void PatternDialog::ModifyLockdown( int offset )
{
    int count=0;
    if( offset == -1 )
        offset = offset_persist;
    for( int i=0; i<64; i++ )
    {
        if( bsc->lockdown[i] )
            count++;
    }
    char buf[1000];
    if( count == 0 )
        sprintf( buf, "There are no locked down squares" );
    else
    {
        if( offset<0 || offset>63 || !bsc->lockdown[offset] )
        {
            for( int i=0; i<64; i++ )
            {
                if( bsc->lockdown[i] )
                {
                    offset = i;
                }
            }
        }
        int file = offset&7;
        int rank = 7 - ((offset>>3)&7);
        char c = bsc->cp.squares[offset];
        const char *s="empty";
        switch( c )
        {
            case 'K': s = "a white king";     break;
            case 'Q': s = "a white queen";    break;
            case 'R': s = "a white rook";     break;
            case 'B': s = "a white bishop";   break;
            case 'N': s = "a white knight";   break;
            case 'P': s = "a white pawn";     break;
            case 'k': s = "a black king";     break;
            case 'q': s = "a black queen";    break;
            case 'r': s = "a black rook";     break;
            case 'b': s = "a black bishop";   break;
            case 'n': s = "a black knight";   break;
            case 'p': s = "a black pawn";     break;
        } 
        char buf2[200];
        if( count-1 > 0 )
            sprintf( buf2, " (and %d other%s...)", count-1, (count-1)>1?"s":"" );
        else
            sprintf( buf2, "" );
        sprintf( buf, "There %s %d locked down square%s\n %c%c must be %s%s", count>1?"are":"is", count, count>1?"s":"", 'a'+file, '1'+rank, s, buf2 );
    }
    offset_persist = offset;
    lockdown_text->SetLabel( wxString(buf) );   
}


