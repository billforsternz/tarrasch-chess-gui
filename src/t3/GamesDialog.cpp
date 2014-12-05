/****************************************************************************
 * Custom dialog - list of games from database, file, session, or clipboard
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/listctrl.h"
#include "wx/notebook.h"
#include "Portability.h"
#ifdef THC_MAC
#include <sys/time.h>               // for gettimeofday()
#endif
#ifdef THC_WINDOWS
#include <windows.h>                // for QueryPerformanceCounter()
#endif
#include "Appdefs.h"
#include "DebugPrintf.h"
#include "thc.h"
#include "GameDetailsDialog.h"
#include "GamePrefixDialog.h"
#include "GameLogic.h"
#include "Objects.h"
#include "PgnFiles.h"
#include "MiniBoard.h"
#include "DbDialog.h"
#include "GamesDialog.h"
#include "Database.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

// GamesDialog type definition
IMPLEMENT_CLASS( GamesDialog, wxDialog )

// GamesDialog event table definition
BEGIN_EVENT_TABLE( GamesDialog, wxDialog )
//  EVT_CLOSE( GamesDialog::OnClose )
    EVT_ACTIVATE(GamesDialog::OnActivate)
    EVT_BUTTON( wxID_OK,                GamesDialog::OnOkClick )
    EVT_BUTTON( ID_DB_UTILITY,          GamesDialog::OnUtility )
    EVT_BUTTON( ID_DB_RELOAD,           GamesDialog::OnReload )
    EVT_BUTTON( wxID_CANCEL,            GamesDialog::OnCancel )
    EVT_BUTTON( ID_SAVE_ALL_TO_A_FILE,  GamesDialog::OnSaveAllToAFile )
    EVT_BUTTON( ID_BUTTON_1,            GamesDialog::OnButton1 )
    EVT_BUTTON( ID_BUTTON_2,            GamesDialog::OnButton2 )
    EVT_BUTTON( ID_BUTTON_3,            GamesDialog::OnButton3 )
    EVT_BUTTON( ID_BUTTON_4,            GamesDialog::OnButton4 )

/*  EVT_BUTTON( ID_BOARD2GAME,          GamesDialog::OnBoard2Game )
    EVT_CHECKBOX( ID_REORDER,           GamesDialog::OnRenumber )
    EVT_BUTTON( ID_ADD_TO_CLIPBOARD,    GamesDialog::OnAddToClipboard )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_DETAILS,   GamesDialog::OnEditGameDetails )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_PREFIX,    GamesDialog::OnEditGamePrefix )
    EVT_BUTTON( ID_PGN_DIALOG_PUBLISH,  GamesDialog::OnPublish )
    EVT_BUTTON( wxID_COPY,              GamesDialog::OnCopy )
    EVT_BUTTON( wxID_CUT,               GamesDialog::OnCut )
    EVT_BUTTON( wxID_DELETE,            GamesDialog::OnDelete )
    EVT_BUTTON( wxID_PASTE,             GamesDialog::OnPaste )
    EVT_BUTTON( wxID_SAVE,              GamesDialog::OnSave ) */
    EVT_BUTTON( wxID_HELP,              GamesDialog::OnHelpClick )

    EVT_RADIOBUTTON( ID_DB_RADIO,       GamesDialog::OnRadio )
    EVT_CHECKBOX   ( ID_DB_CHECKBOX,    GamesDialog::OnCheckBox )
    EVT_COMBOBOX   ( ID_DB_COMBO,       GamesDialog::OnComboBox )
    EVT_LISTBOX(ID_DB_LISTBOX_STATS, GamesDialog::OnNextMove)

    //EVT_MENU( wxID_SELECTALL, GamesDialog::OnSelectAll )
    EVT_LIST_ITEM_FOCUSED(ID_PGN_LISTBOX, GamesDialog::OnListFocused)
    EVT_LIST_ITEM_ACTIVATED(ID_PGN_LISTBOX, GamesDialog::OnListSelected)
    EVT_LIST_COL_CLICK(ID_PGN_LISTBOX, GamesDialog::OnListColClick)
    EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, GamesDialog::OnTabSelected)
END_EVENT_TABLE()


wxVirtualListCtrl::wxVirtualListCtrl( GamesDialog *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
    : wxListCtrl( (wxWindow *)parent, id, pos, size, wxLC_REPORT|wxLC_VIRTUAL )
{
    this->parent = parent;
    mini_board = 0;
}

// Focus changes to new item;
void wxVirtualListCtrl::ReceiveFocus( int focus_idx )
{
    //cprintf( "ListCtrl::ReceiveFocus(%d)\n", focus_idx );
    if( focus_idx >= 0 )
    {
        track->focus_idx = focus_idx;
        parent->ReadItemWithSingleLineCache( focus_idx, track->info );

        initial_focus_offset = track->focus_offset = track->info.db_calculate_move_vector( track->focus_moves, objs.db->gbl_hash );
        if( mini_board )
        {
            std::string previous_move;
            CalculateMoveTxt( previous_move );
            mini_board->SetPosition( track->updated_position.squares );
            std::string desc = track->info.Description();
            if( previous_move.length() > 0 )
            {
                desc += ", after ";
                desc += previous_move;
            }
            parent->player_names->SetLabel(wxString(desc.c_str()));
        }
    }
}

std::string wxVirtualListCtrl::CalculateMoveTxt() const
{
    std::string previous_move_not_needed;
    return CalculateMoveTxt(previous_move_not_needed);
}

std::string wxVirtualListCtrl::CalculateMoveTxt( std::string &previous_move ) const
{
    bool position_updated = false;
    std::string move_txt;
    bool truncated = false;
    thc::ChessRules cr;
    for( size_t i=0; i<track->focus_moves.size(); i++ )
    {
        thc::Move mv = track->focus_moves[i];
        if( i>=track->focus_offset || i+1==track->focus_offset )
        {
            bool prev_move = (i+1 == track->focus_offset);
            bool first_move = (i == track->focus_offset);
            std::string s = mv.NaturalOut(&cr);
            if( i%2 == 0 || prev_move || first_move )
            {
                if( first_move )
                {
                    position_updated = true;
                    track->updated_position = cr;
                }
                char buf[100];
                sprintf( buf, "%lu%s", i/2+1, i%2==0?".":"..." );
                s = std::string(buf) + s;
            }
            if( prev_move )
                previous_move = s;
            else
            {
                move_txt += s;
                move_txt += " ";
                if( i+1 == track->focus_moves.size() )
                    move_txt += track->info.result;
                else if( i < track->focus_moves.size()-5 && move_txt.length()>100 )
                {
                    truncated = true;
                    move_txt += "...";  // very long lines get over truncated by the list control (sad but true)
                    break;
                }
            }
        }
        cr.PlayMove(mv);
    }
    if( !truncated )
        move_txt += track->info.result;
    if( !position_updated )
    {
        track->updated_position = cr;
        move_txt = track->info.result;
    }
    return move_txt;
}
    
wxString wxVirtualListCtrl::OnGetItemText( long item, long column) const
{
    DB_GAME_INFO info;
    std::string move_txt;
    const char *txt;
    parent->ReadItemWithSingleLineCache( item, info );
    switch( column )
    {
        default: txt =  "";                         break;
        case 1: txt =   info.white.c_str();         break;
        case 2: txt =   info.white_elo.c_str();     break;
        case 3: txt =   info.black.c_str();         break;
        case 4: txt =   info.black_elo.c_str();     break;
        case 5: txt =   info.date.c_str();          break;
        case 6: txt =   info.site.c_str();          break;
        //case 7: txt = info.round.c_str();         break;
        case 8: txt =   info.result.c_str();        break;
        //case 9: txt = info.eco.c_str();           break;
        case 10:
        {
            char buf[1000];
            buf[0] = '\0';
            if( info.transpo_nbr > 0 )
                sprintf(buf,"(T%d) ", info.transpo_nbr );
            if( item == track->focus_idx )
            {
                move_txt = CalculateMoveTxt();
                if( track->focus_offset == initial_focus_offset )
                    move_txt = buf + move_txt;
            }
            else
            {
                move_txt = info.db_calculate_move_txt(objs.db->gbl_hash);
                move_txt = buf + move_txt;
            }
            txt = move_txt.c_str();
            //if( item == 0 )
            //    cprintf( "item 0, column 10: %s\n", txt );
            break;
        }
    }
    wxString ws(txt);
    return ws;
}


// GamesDialog event table definition
BEGIN_EVENT_TABLE( wxVirtualListCtrl, wxListCtrl )
    EVT_CHAR(wxVirtualListCtrl::OnChar)
END_EVENT_TABLE()

void wxVirtualListCtrl::OnChar( wxKeyEvent &event )
{
    bool update = false;
    switch ( event.GetKeyCode() )
    {
        case WXK_LEFT:
            if( track->focus_offset > 0 )
            {
                track->focus_offset--;
                update = true;
            }
            break;
        case WXK_RIGHT:
            if( track->focus_offset < track->focus_moves.size() )
            {
                track->focus_offset++;
                update = true;
            }
            break;
        default:
            event.Skip();
    }
    if( update )
    {
        RefreshItem(track->focus_idx);
        if( mini_board )
        {
            std::string previous_move;
            CalculateMoveTxt( previous_move );
            mini_board->SetPosition( track->updated_position.squares );
            std::string desc = track->info.Description();
            if( previous_move.length() > 0 )
            {
                desc += ", after ";
                desc += previous_move;
            }
            parent->player_names->SetLabel(wxString(desc.c_str()));
        }
        
    }
}

// GamesDialog constructor
GamesDialog::GamesDialog
(
    wxWindow* parent,
    thc::ChessRules *cr,
    GamesCache  *gc,
    GamesCache  *gc_clipboard,
    wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style
)
{
    init_position_specified = (cr!=NULL);
    if( init_position_specified )
        this->cr = *cr;
    this->id = id;
    this->gc = gc;
    this->gc_clipboard = gc_clipboard;
    single_line_cache_idx = -1;
    file_game_idx = -1;
    nbr_games_in_list_ctrl = 0;
    Init();
    Create( parent, id, "Title FIXME", pos, size, style );
}


// Pre window creation initialisation
void GamesDialog::Init()
{
    list_ctrl_stats = NULL;
    list_ctrl = NULL;
    selected_game = -1;
    db_game_set = false;
    activated_at_least_once = false;
    transpo_activated = false;
    wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_CTRL,  (int) 'X',     wxID_CUT);
    entries[1].Set(wxACCEL_CTRL,  (int) 'C',     wxID_COPY);
    entries[2].Set(wxACCEL_CTRL,  (int) 'V',     wxID_PASTE);
    entries[3].Set(wxACCEL_CTRL,  (int) 'A',     wxID_SELECTALL);
    entries[4].Set(wxACCEL_NORMAL,  WXK_DELETE,  wxID_DELETE);
    wxAcceleratorTable accel(5, entries);
    SetAcceleratorTable(accel);
}

// Window creation
bool GamesDialog::Create( wxWindow* parent,
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


// Control creation for GamesDialog
void GamesDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    char buf[200];
    if( !init_position_specified )
    {
        // FIXME FIXME FIXME
        CompressMoves press;
        uint64_t hash = press.cr.Hash64Calculate();
        objs.db->gbl_hash = hash;
        
        nbr_games_in_list_ctrl = gc->gds.size();
        sprintf(buf,"%d games in file",nbr_games_in_list_ctrl);
    }
    else
    {
        nbr_games_in_list_ctrl = objs.db->SetPosition( cr ); //gc->gds.size();
        sprintf(buf,"List of %d matching games from the database",nbr_games_in_list_ctrl);
    }
    title_ctrl = new wxStaticText( this, wxID_STATIC,
        buf, wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(title_ctrl, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    int disp_width, disp_height;
    wxDisplaySize(&disp_width, &disp_height);
    wxSize sz;
    if( disp_width > 1366 )
        disp_width = 1366;
    sz.x = (disp_width*4)/5;
    sz.y = (disp_height*2)/5;
    list_ctrl  = new wxVirtualListCtrl( this, ID_PGN_LISTBOX, wxDefaultPosition, sz/*wxDefaultSize*/,wxLC_REPORT|wxLC_VIRTUAL );
    list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
    if( nbr_games_in_list_ctrl > 0 )
        list_ctrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    list_ctrl->InsertColumn( 0, id==ID_PGN_DIALOG_FILE?"#":" "  );
    list_ctrl->InsertColumn( 1, "White"    );
    list_ctrl->InsertColumn( 2, "Elo W"    );
    list_ctrl->InsertColumn( 3, "Black"    );
    list_ctrl->InsertColumn( 4, "Elo B"    );
    list_ctrl->InsertColumn( 5, "Date"     );
    list_ctrl->InsertColumn( 6, "Site"     );
    list_ctrl->InsertColumn( 7, "Round"    );
    list_ctrl->InsertColumn( 8, "Result"   );
    list_ctrl->InsertColumn( 9, "ECO"      );
    list_ctrl->InsertColumn(10, "Moves"    );
    int col_flag=0;
    int cols[11];

    // Only use the non volatile column widths if they validate okay
    if( objs.repository->nv.m_col0 > 0 &&
        objs.repository->nv.m_col1 > 0 &&
        objs.repository->nv.m_col2 > 0 &&
        objs.repository->nv.m_col3 > 0 &&
        objs.repository->nv.m_col4 > 0 &&
        objs.repository->nv.m_col5 > 0 &&
        objs.repository->nv.m_col6 > 0 &&
        objs.repository->nv.m_col7 > 0 &&
        objs.repository->nv.m_col8 > 0 &&
        objs.repository->nv.m_col9 > 0 &&
        objs.repository->nv.m_col10 > 0
      )
    {
        cols[0] = objs.repository->nv.m_col0;
        cols[1] = objs.repository->nv.m_col1;
        cols[2] = objs.repository->nv.m_col2;
        cols[3] = objs.repository->nv.m_col3;
        cols[4] = objs.repository->nv.m_col4;
        cols[5] = objs.repository->nv.m_col5;
        cols[6] = objs.repository->nv.m_col6;
        cols[7] = objs.repository->nv.m_col7;
        cols[8] = objs.repository->nv.m_col8;
        cols[9] = objs.repository->nv.m_col9;
        cols[10]= objs.repository->nv.m_col10;
    }
    else // else set some sensible defaults
    {
        int x   = (sz.x*98)/100;
        objs.repository->nv.m_col0 = cols[0] =   4*x/97;    // "Game #"
        objs.repository->nv.m_col1 = cols[1] =  14*x/97;    // "White" 
        objs.repository->nv.m_col2 = cols[2] =   6*x/97;    // "Elo W"
        objs.repository->nv.m_col3 = cols[3] =  14*x/97;    // "Black" 
        objs.repository->nv.m_col4 = cols[4] =   6*x/97;    // "Elo B" 
        objs.repository->nv.m_col5 = cols[5] =  10*x/97;    // "Date"  
        objs.repository->nv.m_col6 = cols[6] =   9*x/97;    // "Site"  
        objs.repository->nv.m_col7 = cols[7] =   7*x/97;    // "Round" 
        objs.repository->nv.m_col8 = cols[8] =   8*x/97;    // "Result"
        objs.repository->nv.m_col9 = cols[9] =   5*x/97;    // "ECO"   
        objs.repository->nv.m_col10= cols[10]=  14*x/97;    // "Moves"
    }
 /*   if(true) //temp temp temp white, black, result, moves only
    {
        int x   = (sz.x*98)/100;
        objs.repository->nv.m_col0 = cols[0] =   2*x/97;    // "Game #"
        objs.repository->nv.m_col1 = cols[1] =  14*x/97;    // "White"
        objs.repository->nv.m_col2 = cols[2] =   2*x/97;    // "Elo W"
        objs.repository->nv.m_col3 = cols[3] =  14*x/97;    // "Black"
        objs.repository->nv.m_col4 = cols[4] =   2*x/97;    // "Elo B"
        objs.repository->nv.m_col5 = cols[5] =   2*x/97;    // "Date"
        objs.repository->nv.m_col6 = cols[6] =   2*x/97;    // "Site"
        objs.repository->nv.m_col7 = cols[7] =   2*x/97;    // "Round"
        objs.repository->nv.m_col8 = cols[8] =   8*x/97;    // "Result"
        objs.repository->nv.m_col9 = cols[9] =   2*x/97;    // "ECO"
        objs.repository->nv.m_col10= cols[10]=  45*x/97;    // "Moves"
    }  */
    list_ctrl->SetColumnWidth( 0, cols[0] );    // "Game #"
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 1, cols[1] );    // "White" 
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 2, cols[2] );    // "Elo W" 
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 3, cols[3] );    // "Black" 
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 4, cols[4] );    // "Elo B" 
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 5, cols[5] );    // "Date"  
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 6, cols[6] );    // "Site"  
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 7, cols[7] );    // "Round"
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 8, cols[8] );    // "Result"
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 9, cols[9] );    // "ECO"   
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth(10, cols[10] );   // "Moves"
    gc->col_flags.push_back(col_flag);
    //int top_item;
    //bool resuming = data_src->gc->IsResumingPreviousWindow(top_item);
    box_sizer->Add(list_ctrl, 0, wxGROW|wxALL, 5);

    // A dividing line before the details
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // Create a panel beneath the list control, containing everything else
    hsiz_panel = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(hsiz_panel, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 10);

    mini_board = new MiniBoard(this);
    list_ctrl->mini_board = mini_board;
    track = &mini_board_game;
    track->updated_position = cr;
    track->focus_idx = 0;
    track->focus_offset = 0;
    list_ctrl->track = track;
    mini_board->SetPosition( cr.squares );

    hsiz_panel->Add( mini_board, 1, wxALIGN_LEFT|wxTOP|wxRIGHT|wxBOTTOM|wxFIXED_MINSIZE, 5 );

    wxBoxSizer *button_panel = new wxBoxSizer(wxVERTICAL);
    hsiz_panel->Add(button_panel, 0, wxALIGN_TOP|wxALL, 10);

    vsiz_panel_button1 = new wxFlexGridSizer(3,2,0,0);
    button_panel->Add(vsiz_panel_button1, 0, wxALIGN_TOP|wxALL, 0);
    vsiz_panel_buttons = new wxFlexGridSizer(5,1,0,0);
    button_panel->Add(vsiz_panel_buttons, 0, wxALIGN_TOP|wxALL, 0);

    // Load / Ok / Game->Board
    wxButton* ok = new wxButton ( this, wxID_OK, wxT("Load Game"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // Save all games to a file
    wxButton* save_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save all"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
        wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Help button
    wxButton* help = new wxButton( this, wxID_HELP, wxT("Help"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    player_names = new wxStaticText( this, wxID_ANY, "White - Black",
                                    wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL );
    box_sizer->Add(player_names, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 10);
    
    // Stats list box
    //    wxSize sz4 = sz;
    //    sz.x /= 4;
    //    sz.y /= 3;
    wxSize sz4 = mini_board->GetSize();
    wxSize sz5 = sz4;
    sz5.x = (sz4.x*18)/10;
    sz5.y = (sz4.y*10)/10;
    notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, /*wxDefaultSize*/ sz5 );
    //wxPanel *notebook_page1 = new wxPanel(notebook, wxID_ANY );
    hsiz_panel /*vsiz_panel_stats*/->Add( notebook, 0, wxALIGN_TOP|wxGROW|wxALL, 0 );

}


// Set the validators for the dialog controls
void GamesDialog::SetDialogValidators()
{
/*    FindWindow(ID_HUMAN)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_human));
//    FindWindow(ID_COMPUTER)->SetValidator(
//        wxTextValidator(wxFILTER_ASCII, &dat.m_computer));
    FindWindow(ID_WHITE)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_white));
    FindWindow(ID_BLACK)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_black));
*/
}

// Sets the help text for the dialog controls
void GamesDialog::SetDialogHelp()
{
/*
    wxString human_help    = wxT("The person who usually uses this program to play against a chess engine.");
//    wxString computer_help = wxT("An optional friendly name for the chess engine.");
    wxString white_help    = wxT("White's name.");
    wxString black_help    = wxT("Black's name.");

    FindWindow(ID_HUMAN)->SetHelpText(human_help);
    FindWindow(ID_HUMAN)->SetToolTip(human_help);

//    FindWindow(ID_COMPUTER)->SetHelpText(computer_help);
//    FindWindow(ID_COMPUTER)->SetToolTip(computer_help);

    FindWindow(ID_WHITE)->SetHelpText(white_help);
    FindWindow(ID_WHITE)->SetToolTip(white_help);

    FindWindow(ID_BLACK)->SetHelpText(black_help);
    FindWindow(ID_BLACK)->SetToolTip(black_help);
*/
}


void GamesDialog::OnListColClick( wxListEvent &event )
{
    int compare_col = event.GetColumn();
    OnListColClick( compare_col );
}

// override
void GamesDialog::OnListColClick( int compare_col )
{
}


// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GamesDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    OnOk();
}

void GamesDialog::OnActivate(wxActivateEvent& event)
{
    OnActivate();
}

// override
void GamesDialog::OnActivate()
{
}

void GamesDialog::ReadItemWithSingleLineCache( int item, DB_GAME_INFO &info )
{
    if( !TestAndClearIsCacheDirty() && (item==single_line_cache_idx) )
    {
        info = single_line_cache;
    }
    else
    {
        ReadItem( item, info );
        single_line_cache_idx = item;
        single_line_cache = info;
    }
}

void GamesDialog::LoadGame( int idx, int focus_offset )
{
    static DB_GAME_INFO info;
    ReadItemWithSingleLineCache( idx, info );
    GameDocument gd;
    std::vector<thc::Move> moves;
    gd.white = info.white;
    gd.black = info.black;
    gd.result = info.result;
    int len = info.str_blob.length();
    const char *blob = info.str_blob.c_str();
    CompressMoves press;
    for( int nbr=0; nbr<len;  )
    {
        thc::Move mv;
        int nbr_used = press.decompress_move( blob, mv );
        if( nbr_used == 0 )
            break;
        moves.push_back(mv);
        blob += nbr_used;
        nbr += nbr_used;
    }
    gd.LoadFromMoveList( moves, focus_offset );
    db_game = gd;
    db_game_set = true;
}


void GamesDialog::OnListSelected( wxListEvent &event )
{
    if( list_ctrl )
    {
        int idx = event.m_itemIndex;
        cprintf( "GamesDialog::OnListSelected(%d)\n", idx );
        list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
        LoadGame( idx, track->focus_offset );
        TransferDataToWindow();
        AcceptAndClose();
    }
}

void GamesDialog::OnListFocused( wxListEvent &event )
{
    if( list_ctrl )
    {
        int prev = track->focus_idx;
        int idx = event.m_itemIndex;
        cprintf( "GamesDialog::OnListFocused() Prev idx=%d, New idx=%d\n", prev, idx );
        list_ctrl->ReceiveFocus( idx );
        list_ctrl->RefreshItem(prev);
    }
}



// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GamesDialog::OnOk()
{
    if( list_ctrl )
    {
        LoadGame( track->focus_idx, track->focus_offset );
        TransferDataToWindow();
        AcceptAndClose();
    }
}

void GamesDialog::OnSaveAllToAFile( wxCommandEvent& WXUNUSED(event) )
{
    OnSaveAllToAFile();
}

// override
void GamesDialog::OnSaveAllToAFile()
{
}

void GamesDialog::OnCancel( wxCommandEvent& WXUNUSED(event) )
{
    OnCancel();
    EndDialog( wxID_CANCEL );
}

// override
void GamesDialog::OnCancel()
{
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void GamesDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    OnHelpClick();
}

// overide
void GamesDialog::OnHelpClick()
{
}

void GamesDialog::OnRadio( wxCommandEvent& event )
{
}

void GamesDialog::OnSpin( wxCommandEvent& event )
{
}

void GamesDialog::OnComboBox( wxCommandEvent& event )
{
}

void GamesDialog::OnCheckBox( wxCommandEvent& event )
{
}


void GamesDialog::OnReload( wxCommandEvent& WXUNUSED(event) )
{
    wxString name = text_ctrl->GetValue();
    std::string sname(name.c_str());
    thc::ChessPosition start_cp;
    
    // Temp - do a "find on page type feature"
    if( sname.length()>0 && cr==start_cp )
    {
        int row = objs.db->FindRow( sname );
        list_ctrl->EnsureVisible( row );   // get vaguely close
        list_ctrl->SetItemState( row, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
        list_ctrl->SetItemState( row, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
        list_ctrl->SetFocus();
    }
    else
    {
        nbr_games_in_list_ctrl = objs.db->SetPosition( cr, sname );
        char buf[200];
        sprintf(buf,"List of %d matching games from the database",nbr_games_in_list_ctrl);
        title_ctrl->SetLabel( buf );
        cprintf( "Reloading, %d games\n", nbr_games_in_list_ctrl);
        list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
        list_ctrl->RefreshItems(0,nbr_games_in_list_ctrl-1);
    }
}

void GamesDialog::OnUtility( wxCommandEvent& WXUNUSED(event) )
{
    OnUtility();
}

// overide
void GamesDialog::OnUtility()
{
}

void GamesDialog::OnButton1( wxCommandEvent& WXUNUSED(event) )
{
    OnButton1();
}

// overide
void GamesDialog::OnButton1()
{
}

void GamesDialog::OnButton2( wxCommandEvent& WXUNUSED(event) )
{
    OnButton2();
}

// overide
void GamesDialog::OnButton2()
{
}

void GamesDialog::OnButton3( wxCommandEvent& WXUNUSED(event) )
{
    OnButton3();
}

// overide
void GamesDialog::OnButton3()
{
}

void GamesDialog::OnButton4( wxCommandEvent& WXUNUSED(event) )
{
    OnButton4();
}

// overide
void GamesDialog::OnButton4()
{
}

// Move Stats or Transpostitions selected
void GamesDialog::OnTabSelected( wxBookCtrlEvent& event )
{
    transpo_activated = (1==event.GetSelection());
    int top = list_ctrl->GetTopItem();
    int count = 1 + list_ctrl->GetCountPerPage();
    if( count > nbr_games_in_list_ctrl )
        count = nbr_games_in_list_ctrl;
    for( int i=0; i<count; i++ )
        list_ctrl->RefreshItem(top++);
}


// One of the moves in move stats is clicked
void GamesDialog::OnNextMove( wxCommandEvent &event )
{
    int idx = event.GetSelection();
    OnNextMove(idx);
}


// overide
void GamesDialog::OnNextMove( int idx )
{
}
   
    
bool GamesDialog::ShowModalOk()
{
    bool ok = (wxID_OK == ShowModal());
    objs.repository->nv.m_col0  = list_ctrl->GetColumnWidth( 0 );    // "Game #"
    objs.repository->nv.m_col1  = list_ctrl->GetColumnWidth( 1 );    // "White"
    objs.repository->nv.m_col2  = list_ctrl->GetColumnWidth( 2 );    // "Elo W"
    objs.repository->nv.m_col3  = list_ctrl->GetColumnWidth( 3 );    // "Black"
    objs.repository->nv.m_col4  = list_ctrl->GetColumnWidth( 4 );    // "Elo B" 
    objs.repository->nv.m_col5  = list_ctrl->GetColumnWidth( 5 );    // "Date"  
    objs.repository->nv.m_col6  = list_ctrl->GetColumnWidth( 6 );    // "Site"  
    objs.repository->nv.m_col7  = list_ctrl->GetColumnWidth( 7 );    // "Round" 
    objs.repository->nv.m_col8  = list_ctrl->GetColumnWidth( 8 );    // "Result"
    objs.repository->nv.m_col9  = list_ctrl->GetColumnWidth( 9 );    // "ECO"   
    objs.repository->nv.m_col10 = list_ctrl->GetColumnWidth(10 );    // "Moves"
    return ok;
}

bool GamesDialog::LoadGame( GameDocument &gd )
{
    if( db_game_set )
    {
        gd = db_game;
    }
    return db_game_set;
}


void GamesDialog::OnListSelected( int idx )
{
    cprintf( "DbDialog::OnListSelected(%d)\n", idx );
    list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
    LoadGame( idx, track->focus_offset );
}

