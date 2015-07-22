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
    EVT_BUTTON( ID_DB_RELOAD,           GamesDialog::OnSearch )
    EVT_BUTTON( wxID_CANCEL,            GamesDialog::OnCancel )
    EVT_BUTTON( ID_SAVE_ALL_TO_A_FILE,  GamesDialog::OnSaveAllToAFile )
    EVT_BUTTON( ID_BUTTON_1,            GamesDialog::OnButton1 )
    EVT_BUTTON( ID_BUTTON_2,            GamesDialog::OnButton2 )
    EVT_BUTTON( ID_BUTTON_3,            GamesDialog::OnButton3 )
    EVT_BUTTON( ID_BUTTON_4,            GamesDialog::OnButton4 )

    EVT_BUTTON( ID_BOARD2GAME,          GamesDialog::OnBoard2Game )
//    EVT_CHECKBOX( ID_REORDER,           GamesDialog::OnRenumber )
    EVT_BUTTON( ID_ADD_TO_CLIPBOARD,    GamesDialog::OnAddToClipboard )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_DETAILS,   GamesDialog::OnEditGameDetails )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_PREFIX,    GamesDialog::OnEditGamePrefix )
    EVT_BUTTON( ID_PGN_DIALOG_PUBLISH,  GamesDialog::OnPublish )
    EVT_BUTTON( wxID_COPY,              GamesDialog::OnCopy )
    EVT_BUTTON( wxID_CUT,               GamesDialog::OnCut )
    EVT_BUTTON( wxID_DELETE,            GamesDialog::OnDelete )
    EVT_BUTTON( wxID_PASTE,             GamesDialog::OnPaste )
    EVT_BUTTON( wxID_SAVE,              GamesDialog::OnSave )
    EVT_BUTTON( wxID_HELP,              GamesDialog::OnHelpClick )

    EVT_RADIOBUTTON( ID_DB_RADIO,       GamesDialog::OnRadio )
    EVT_CHECKBOX   ( ID_DB_CHECKBOX,    GamesDialog::OnCheckBox )
    EVT_CHECKBOX   ( ID_DB_CHECKBOX2,   GamesDialog::OnCheckBox2 )
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
        //if( focus_idx==0 ) 
        //    cprintf( "** ReceiveFocus(0) calling ReadItemWithSingleLineCache\n" );
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
	thc::ChessRules cr=track->info.GetStartPosition();
    for( size_t i=0; i<track->focus_moves.size(); i++ )
    {
        thc::Move mv = track->focus_moves[i];
        if( i>=track->focus_offset || i+1==track->focus_offset )
        {
            bool prev_move = (i+1 == track->focus_offset);
            bool first_move = (i == track->focus_offset);
            std::string s = mv.NaturalOut(&cr);
            if( cr.white || prev_move || first_move )
            {
                if( first_move )
                {
                    position_updated = true;
                    track->updated_position = cr;
                }
                char buf[100];
                sprintf( buf, "%lu%s", cr.full_move_count, cr.white?".":"..." );
                s = std::string(buf) + s;
            }
            if( prev_move )
                previous_move = s;
            else
            {
                move_txt += s;
                if( i+1 == track->focus_moves.size() )
                {
                    move_txt += " ";
                    move_txt += track->info.r.result;
                }
                else if( i < track->focus_moves.size()-5 && move_txt.length()>100 )
                {
                    move_txt += "...";  // very long lines get over truncated by the list control (sad but true)
                    break;
                }
                else
                {
                    move_txt += " ";
                }
            }
        }
        cr.PlayMove(mv);
    }
    if( !position_updated )
    {
        track->updated_position = cr;
        move_txt = track->info.r.result;
    }
    return move_txt;
}
    
wxString wxVirtualListCtrl::OnGetItemText( long item, long column) const
{
    DB_GAME_INFO_FEN info;
    std::string move_txt;
    const char *txt;
    //if( item==0 && column==10 ) 
    //    cprintf( "** OnGetItemText(0) calling ReadItemWithSingleLineCache\n" );
    parent->ReadItemWithSingleLineCache( item, info );
    switch( column )
    {
        default: txt =  "";                         break;
        case 1: txt =   info.r.white.c_str();         break;
        case 2: txt =   info.r.white_elo.c_str();     break;
        case 3: txt =   info.r.black.c_str();         break;
        case 4: txt =   info.r.black_elo.c_str();     break;
        case 5: txt =   info.r.date.c_str();          break;
        case 6: txt =   info.r.site.c_str();          break;
        //case 7: txt = info.r.round.c_str();         break;
        case 8: txt =   info.r.result.c_str();        break;
        //case 9: txt = info.r.eco.c_str();           break;
        case 10:
        {
            char buf[1000];
            buf[0] = '\0';
            if( info.transpo_nbr > 0 )
                sprintf(buf,"(T%d) ", info.transpo_nbr );
            if( item == track->focus_idx )
            {
                //if( item == 0 ) 
                //    cprintf( " ** OnGetItemText(0) focus hit, nbr moves=%d\n", track->focus_moves.size() );
                move_txt = CalculateMoveTxt();
                if( track->focus_offset == initial_focus_offset )
                    move_txt = buf + move_txt;
            }
            else
            {
                //if( item == 0 ) 
                //    cprintf( " ** OnGetItemText(0) no focus hit\n" );
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
    this->parent2 = parent;
    this->id = id;
    this->pos = pos;
    this->size = size;
    this->style = style;
    this->gc = gc;
    this->gc_clipboard = gc_clipboard;
    init_position_specified = (cr!=NULL);
    if( init_position_specified )
        this->cr = *cr;
    single_line_cache_idx = -1;
    file_game_idx = -1;
    nbr_games_in_list_ctrl = 0;
    compare_col = 0;
    dirty = true;
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
    orig_nbr_games_in_list_ctrl = nbr_games_in_list_ctrl;

    title_ctrl = new wxStaticText( this, wxID_STATIC,
        buf, wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(title_ctrl, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    int disp_width, disp_height;
    wxDisplaySize(&disp_width, &disp_height);
    wxSize sz;
    cprintf( "disp_width=%d, disp_height=%d\n", disp_width, disp_height );
    if( disp_width > 1366 )
        disp_width = 1366;
    if( disp_height > 768 )
        disp_height = 768;
    sz.x = (disp_width*90)/100;
    sz.y = (disp_height*36)/100;
    list_ctrl  = new wxVirtualListCtrl( this, ID_PGN_LISTBOX, wxDefaultPosition, sz/*wxDefaultSize*/,wxLC_REPORT|wxLC_VIRTUAL );
    list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
    if( nbr_games_in_list_ctrl > 0 )
    {
        list_ctrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }

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
    #if 0 //temp todo
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
    #endif
    {
        int x   = (sz.x*98)/100;
        objs.repository->nv.m_col0 = cols[0] =   4*x/142;    // "Game #"
        objs.repository->nv.m_col1 = cols[1] =  16*x/142;    // "White" 
        objs.repository->nv.m_col2 = cols[2] =   6*x/142;    // "Elo W"
        objs.repository->nv.m_col3 = cols[3] =  16*x/142;    // "Black" 
        objs.repository->nv.m_col4 = cols[4] =   6*x/142;    // "Elo B" 
        objs.repository->nv.m_col5 = cols[5] =  10*x/142;    // "Date"  
        objs.repository->nv.m_col6 = cols[6] =  14*x/142;    // "Site"  
        objs.repository->nv.m_col7 = cols[7] =   3*x/142;    // "Round" 
        objs.repository->nv.m_col8 = cols[8] =   8*x/142;    // "Result"
        objs.repository->nv.m_col9 = cols[9] =   3*x/142;    // "ECO"   
        objs.repository->nv.m_col10= cols[10]=  56*x/142;    // "Moves"
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
    track->focus_idx = -1;
    track->focus_offset = 0;
    list_ctrl->track = track;
    mini_board->SetPosition( cr.squares );

    hsiz_panel->Add( mini_board, 1, wxALIGN_LEFT|wxTOP|wxRIGHT|wxBOTTOM|wxFIXED_MINSIZE, 5 );

    wxBoxSizer *button_panel = new wxBoxSizer(wxVERTICAL);
    hsiz_panel->Add(button_panel, 0, wxALIGN_TOP|wxALL, 10);

    int row1, col1, row2, col2;
    GetButtonGridDimensions( row1, col1, row2, col2 );
    if( row1>0 && col1>0 )
        vsiz_panel_button1 = new wxFlexGridSizer(row1,col1,0,0);
    else
        vsiz_panel_button1 = NULL;
    if( vsiz_panel_button1 )
        button_panel->Add(vsiz_panel_button1, 0, wxALIGN_TOP|wxALL, 0);
    if( row2>0 && col2>0 )
        vsiz_panel_buttons = new wxFlexGridSizer(row2,col2,0,0);
    else
        vsiz_panel_buttons = NULL;
    if( vsiz_panel_buttons )
        button_panel->Add(vsiz_panel_buttons, 0, wxALIGN_TOP|wxALL, 0);

    // Load / Ok / Game->Board
    ok_button = new wxButton ( this, wxID_OK, wxT("Load Game"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(ok_button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
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
    
    // Overridden by specialised classes
    AddExtraControls();
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


// TODO - Fix the mass copy paste from DbDialog.cpp !

static GamesDialog *backdoor;
static bool compare( const smart_ptr<MagicBase> &g1, const smart_ptr<MagicBase> &g2 )
{
    bool lt=false;
    DB_GAME_INFO *p1 = g1->GetCompactGamePtr();
    if( !p1 )
    {
        static GameDocument gd1;
        g1->GetGameDocument(gd1);
        static DB_GAME_INFO db1;
        db1.Downscale(gd1);
        p1 = &db1;
    }
    DB_GAME_INFO *p2 = g2->GetCompactGamePtr();
    if( !p2 )
    {
        static GameDocument gd2;
        g2->GetGameDocument(gd2);
        static DB_GAME_INFO db2;
        db2.Downscale(gd2);
        p2 = &db2;
    }
    switch( backdoor->compare_col )
    {
        case 1: lt = p1->r.white < p2->r.white;
                break;
        case 2:
        {       
                int elo_1 = atoi( p1->r.white_elo.c_str() );
                int elo_2 = atoi( p2->r.white_elo.c_str() );
                lt = elo_1 < elo_2;
                break;
        }
        case 3: lt = p1->r.black < p2->r.black;
                break;
        case 4:
        {
                int elo_1 = atoi( p1->r.black_elo.c_str() );
                int elo_2 = atoi( p2->r.black_elo.c_str() );
                lt = elo_1 < elo_2;
                break;
        }
        case 5: lt = p1->r.date < p2->r.date;
                break;
        case 6: lt = p1->r.site < p2->r.site;
                break;
        case 8: lt = p1->r.result < p2->r.result;
                break;
    }
    return lt;
}
        
static bool rev_compare( const smart_ptr<MagicBase> &g1, const smart_ptr<MagicBase> &g2 )
{
    bool lt=true;
    DB_GAME_INFO *p1 = g1->GetCompactGamePtr();
    if( !p1 )
    {
        static GameDocument gd1;
        g1->GetGameDocument(gd1);
        static DB_GAME_INFO db1;
        db1.Downscale(gd1);
        p1 = &db1;
    }
    DB_GAME_INFO *p2 = g2->GetCompactGamePtr();
    if( !p2 )
    {
        static GameDocument gd2;
        g2->GetGameDocument(gd2);
        static DB_GAME_INFO db2;
        db2.Downscale(gd2);
        p2 = &db2;
    }
    switch( backdoor->compare_col )
    {
        case 1: lt = p1->r.white > p2->r.white;           break;
        case 2:
        {       int elo_1 = atoi( p1->r.white_elo.c_str() );
                int elo_2 = atoi( p2->r.white_elo.c_str() );
                lt = elo_1 > elo_2;
                break;
        }
        case 3: lt = p1->r.black > p2->r.black;           break;
        case 4:
        {       int elo_1 = atoi( p1->r.black_elo.c_str() );
                int elo_2 = atoi( p2->r.black_elo.c_str() );
                lt = elo_1 > elo_2;
                break;
        }
        case 5: lt = p1->r.date > p2->r.date;             break;
        case 6: lt = p1->r.site > p2->r.site;             break;
        case 8: lt = p1->r.result > p2->r.result;         break;
    }
    return lt;
}

struct TempElement
{
    int idx;
    int transpo;
    std::string blob;
    std::vector<int> counts;
};

static bool compare_blob( const TempElement &e1, const TempElement &e2 )
{
    bool lt = false;
    if( e1.transpo == e2.transpo )
        lt = (e1.blob < e2.blob);
    else
        lt = (e1.transpo > e2.transpo);     // smaller transpo nbr should come first
    return lt;
}

static bool compare_counts( const TempElement &e1, const TempElement &e2 )
{
    bool lt = false;
    if( e1.transpo != e2.transpo )
        lt = (e1.transpo > e2.transpo);     // smaller transpo nbr should come first
    else
    {
        unsigned int len = e1.blob.length();
        if( e2.blob.length() < len )
            len = e2.blob.length();
        for( unsigned int i=0; i<len; i++ )
        {
            if( e1.counts[i] != e2.counts[i] )
            {
                lt = (e1.counts[i] < e2.counts[i]);
                return lt;
            }
        }
        lt = (e1.blob.length() < e2.blob.length());
    }
    return lt;
}

#if 0
void GamesDialog::SmartCompare()
{
    std::vector<TempElement> inter;     // intermediate representation
    
    // Step 1, do a conventional string sort, beginning at our offset into the blob
    unsigned int sz = gc->gds.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        DB_GAME_INFO &g = gc->gds[i];
        TempElement e;
        unsigned int sz2 = transpositions.size();
        for( unsigned int j=0; j<sz2; j++ )
        {
            PATH_TO_POSITION *ptp = &transpositions[j];
            unsigned int offset = ptp->blob.length();
            if( 0 == memcmp( ptp->blob.c_str(), g.str_blob.c_str(), offset ) )
            {
                e.idx = i;
                e.blob = g.str_blob.substr(offset);
                e.counts.resize( e.blob.length() );
                e.transpo = transpo_activated ? j+1 : 0;
                inter.push_back(e);
                break;
            }
        }
    }
    std::sort( inter.begin(), inter.end(), compare_blob );
    
    // Step 2, work out the nbr of moves in clumps of moves
    sz = inter.size();
    bool at_least_one = true;
    for( unsigned int j=0; at_least_one; j++ )
    {
        at_least_one = false;  // stop when we've passed end of all strings
        char current='\0';
        unsigned int start=0;
        bool run_in_progress=false;
        for( unsigned int i=0; i<sz; i++ )
        {
            TempElement &e = inter[i];
            
            // A short game stops runs
            if( j >= e.blob.length() )
            {
                if( run_in_progress )
                {
                    run_in_progress = false;
                    int count = i-start;
                    for( int k=start; k<i; k++ )
                    {
                        TempElement &f = inter[k];
                        f.counts[j] = count;
                    }
                }
                continue;
            }
            at_least_one = true;
            char c = e.blob[j];
            
            // First time, get something to start a run
            if( !run_in_progress )
            {
                run_in_progress = true;
                current = c;
                start = i;
            }
            else
            {
                
                // Run can be over because of character change
                if( c != current )
                {
                    int count = i-start;
                    for( int k=start; k<i; k++ )
                    {
                        TempElement &f = inter[k];
                        f.counts[j] = count;
                    }
                    current = c;
                    start = i;
                }
                
                // And/Or because we reach bottom
                if( i+1 == sz )
                {
                    int count = sz - start;
                    for( int k=start; k<sz; k++ )
                    {
                        TempElement &f = inter[k];
                        f.counts[j] = count;
                    }
                }
            }
        }
    }

    // Step 3 sort again using the counts
    std::sort( inter.begin(), inter.end(), compare_counts );
    
    // Step 4 build sorted version of games list
    std::vector<DB_GAME_INFO> temp;
    sz = inter.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        TempElement &e = inter[i];
        temp.push_back( gc->gds[e.idx] );
    }

    // Step 5 replace original games list
    gc->gds = temp;
}
#endif

// override
void GamesDialog::OnListColClick( int compare_col )
{
    if( gc->gds.size() > 0 )
    {
        static int last_time;
        static int consecutive;
        if( compare_col == last_time )
            consecutive++;
        else
            consecutive=0;
        this->compare_col = compare_col;
        backdoor = this;
        if( compare_col == 10 )
            ;//SmartCompare();
        else
            std::sort( gc->gds.begin(), gc->gds.end(), (consecutive%2==0)?rev_compare:compare );
        nbr_games_in_list_ctrl = gc->gds.size();
        list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
        list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
        int top = list_ctrl->GetTopItem();
        int count = 1 + list_ctrl->GetCountPerPage();
        if( count > nbr_games_in_list_ctrl )
            count = nbr_games_in_list_ctrl;
        for( int i=0; i<count; i++ )
            list_ctrl->RefreshItem(top++);
        Goto(0);
        last_time = compare_col;
    }
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
    if( !activated_at_least_once )
    {
        activated_at_least_once = true;
        Goto(0); // list_ctrl->SetFocus();
    }
}

void GamesDialog::Goto( int idx )
{
    int old = track->focus_idx;
    if( old != idx && old>=0 )
    {
        list_ctrl->SetItemState( old, 0, wxLIST_STATE_SELECTED );
        list_ctrl->SetItemState( old, 0, wxLIST_STATE_FOCUSED );
    }
    list_ctrl->SetItemState( idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
    
    dirty = true;
    list_ctrl->RefreshItem( idx );
    list_ctrl->ReceiveFocus( idx );
    list_ctrl->SetItemState( idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
    list_ctrl->SetFocus();
    list_ctrl->EnsureVisible(idx);
}

void GamesDialog::ReadItemWithSingleLineCache( int item, DB_GAME_INFO_FEN &info )
{
    //if( item==0 )
    //    cprintf( "** In ReadItemWithSingleLineCache(0)\n" );        
    if( !TestAndClearIsCacheDirty() && (item==single_line_cache_idx) )
    {
        //if( item==0 )
        //    cprintf( " single line cache\n" );        
        info = single_line_cache;
    }
    else
    {
        //if( item==0 )
        //    cprintf( " not single line cache\n" );        
        ReadItem( item, info );
        single_line_cache_idx = item;
        single_line_cache = info;
    }
    //if( item==0 )
    //    cprintf( " nbr moves=%d\n", info.str_blob.size() );        
}

void GamesDialog::LoadGame( int idx, int focus_offset )
{
    static DB_GAME_INFO_FEN info;
    ReadItemWithSingleLineCache( idx, info );
    GameDocument gd;
    std::vector<thc::Move> moves;
    gd.r.white = info.r.white;
    gd.r.black = info.r.black;
    gd.r.result = info.r.result;
    int len = info.str_blob.length();
    const char *blob = info.str_blob.c_str();
    CompressMoves press;
    bool have_start_position = info.HaveStartPosition();
    if( have_start_position )
        press.Init( info.GetStartPosition() );
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

void GamesDialog::OnCheckBox( wxCommandEvent& event )
{
    bool checked = event.IsChecked();
    OnCheckBox( checked );
}

// overide
void GamesDialog::OnCheckBox( bool checked )
{
}

void GamesDialog::OnCheckBox2( wxCommandEvent& event )
{
    bool checked = event.IsChecked();
    OnCheckBox2( checked );
}

// overide
void GamesDialog::OnCheckBox2( bool checked )
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


void GamesDialog::OnUtility( wxCommandEvent& WXUNUSED(event) )
{
    OnUtility();
}

// overide
void GamesDialog::OnUtility()
{
}

void GamesDialog::OnSearch( wxCommandEvent& WXUNUSED(event) )
{
    OnSearch();
}

// overide
void GamesDialog::OnSearch()
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
    Init();
    Create( parent2, id, "Title FIXME", pos, size, style );
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



void GamesDialog::OnEditGameDetails( wxCommandEvent& WXUNUSED(event) )
{
 /*   int idx;
    int focus_idx = GetFocusGame(idx);
    if( focus_idx != -1  )
    {
        GameDetailsDialog dialog( this );
        GameDocument temp = *GetCachedDocument(focus_idx);
        if( dialog.Run( temp ) )
        {
            GameDocument temp = *GetCachedDocument(focus_idx);
            objs.gl->GameRedisplayPlayersResult();
            list_ctrl->SetItem( idx, 1, temp.white );
            list_ctrl->SetItem( idx, 2, temp.white_elo );
            list_ctrl->SetItem( idx, 3, temp.black );
            list_ctrl->SetItem( idx, 4, temp.black_elo );
            list_ctrl->SetItem( idx, 5, temp.date );
            list_ctrl->SetItem( idx, 6, temp.site );
            list_ctrl->SetItem( idx, 7, temp.round );
            list_ctrl->SetItem( idx, 8, temp.result );
            list_ctrl->SetItem( idx, 9, temp.eco );
        }
    }  */
}

void GamesDialog::OnEditGamePrefix( wxCommandEvent& WXUNUSED(event) )
{
/*    int idx;
    int focus_idx = GetFocusGame(idx);
    if( focus_idx != -1  )
    {
        GamePrefixDialog dialog( this );
        GameDocument temp = gc->gds[focus_idx]->GetGameDocument();
        if( dialog.Run( temp ) )
        {
            std::string s = CalculateMovesColumn(temp);
            list_ctrl->SetItem( idx,10,s);
        }
    } */
}

void GamesDialog::OnAddToClipboard( wxCommandEvent& WXUNUSED(event) )
{
    CopyOrAdd( false );
}

void GamesDialog::OnCopy( wxCommandEvent& WXUNUSED(event) )
{
    CopyOrAdd( true );
}

void GamesDialog::CopyOrAdd( bool clear_clipboard )
{
/*    int nbr_copied=0, idx_focus=-1;
    if( list_ctrl )
    {
        int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                idx_focus = i;
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
            {
                if( clear_clipboard )
                {
                    clear_clipboard = false;
                    gc_clipboard->gds.clear();
                }
                GameDocument gd = gc->gds[i]->GetGameDocument();
                make_smart_ptr( HoldDocument, new_doc, gd );
                gc_clipboard->gds.push_back(std::move(new_doc));
                nbr_copied++;
            }
        }
        if( nbr_copied==0 && idx_focus>=0 )
        {
            if( clear_clipboard )
            {
                clear_clipboard = false;
                gc_clipboard->gds.clear();
            }
            GameDocument gd = gc->gds[idx_focus]->GetGameDocument();
            make_smart_ptr( HoldDocument, new_doc, gd );
            gc_clipboard->gds.push_back(std::move(new_doc));
            nbr_copied++;
        }
    }
    dbg_printf( "%d games copied\n", nbr_copied ); */
}

void GamesDialog::OnBoard2Game( wxCommandEvent& WXUNUSED(event) )
{
    /*int idx_focus=0;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                idx_focus = i;
        }
        std::vector< smart_ptr<MagicBase> >::iterator iter = gc->gds.begin() + idx_focus;
        GameDocument gd = objs.gl->gd;
        gd.modified = true;
        GameDetailsDialog dialog( this );
        if( dialog.Run(gd) )
        {
            objs.gl->GameRedisplayPlayersResult();
            gd.game_nbr = 0;
            gd.modified = true;
            gc->file_irrevocably_modified = true;
            make_smart_ptr( HoldDocument, new_doc, gd );
            gc->gds.insert( iter, std::move(new_doc) );
            wxListItem item;              
            list_ctrl->InsertItem( idx_focus, item );
            list_ctrl->SetItem( idx_focus, 0, "" );                     // game_nbr
            list_ctrl->SetItem( idx_focus, 1, gd.white );
            list_ctrl->SetItem( idx_focus, 2, gd.white_elo );
            list_ctrl->SetItem( idx_focus, 3, gd.black );
            list_ctrl->SetItem( idx_focus, 4, gd.black_elo );
            list_ctrl->SetItem( idx_focus, 5, gd.date );
            list_ctrl->SetItem( idx_focus, 6, gd.site );
            list_ctrl->SetItem( idx_focus, 7, gd.round );
            list_ctrl->SetItem( idx_focus, 8, gd.result );
            list_ctrl->SetItem( idx_focus, 9, gd.eco );
            list_ctrl->SetItem( idx_focus, 10, gd.moves_txt );
        }
    } */
}


#if 0
void GamesDialog::OnSaveAllToAFile()
{
/*    wxFileDialog fd( objs.frame, "Save all listed games to a new .pgn file", "", "", "*.pgn", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
    wxString dir = objs.repository->nv.m_doc_dir;
    fd.SetDirectory(dir);
    int answer = fd.ShowModal();
    if( answer == wxID_OK )
    {
        wxString dir;
        wxFileName::SplitPath( fd.GetPath(), &dir, NULL, NULL );
        objs.repository->nv.m_doc_dir = dir;
        wxString wx_filename = fd.GetPath();
        std::string filename( wx_filename.c_str() );
        gc->FileSaveAllAsAFile( filename );
    } */
}

#endif

void GamesDialog::OnCut( wxCommandEvent& WXUNUSED(event) )
{
/*    bool clear_clipboard = true;
    int nbr_cut=0, idx_focus=-1;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        std::vector< smart_ptr<MagicBase> >::iterator iter = gc->gds.begin();
        std::vector< smart_ptr<MagicBase> >::iterator iter_focus;
        for( int i=0; iter!=gc->gds.end(); )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
            {
                idx_focus = i;
                iter_focus = iter;
            }
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
            {
                if( clear_clipboard )
                {
                    clear_clipboard = false;
                    gc_clipboard->gds.clear();
                }
                MagicBase &mb = **iter;
                GameDocument gd = mb.GetGameDocument();
                make_smart_ptr( HoldDocument, new_doc, gd );
                gc_clipboard->gds.push_back(std::move(new_doc));
                list_ctrl->DeleteItem(i);
                iter = gc->gds.erase(iter);
                gc->file_irrevocably_modified = true;
                nbr_cut++;
            }
            else
            {
                ++iter;
                ++i;
            }
        }

        if( nbr_cut==0 && idx_focus>=0 )
        {
            gc_clipboard->gds.clear();
            MagicBase &mb = **iter_focus;
            GameDocument gd = mb.GetGameDocument();
            // This is required because for some reason it doesn't work if you don't use the intermediate reference, i.e.:
            //   GameDocument gd = **iter_focus.GetGameDocument();   // doesn't work
            make_smart_ptr( HoldDocument,new_doc,gd);
            gc_clipboard->gds.push_back(std::move(new_doc));
            list_ctrl->DeleteItem(idx_focus);
            iter = gc->gds.erase(iter_focus);
            gc->file_irrevocably_modified = true;
            nbr_cut++;
        }
    }
    dbg_printf( "%d games cut\n", nbr_cut ); */
}

void GamesDialog::OnDelete( wxCommandEvent& WXUNUSED(event) )
{
/*    int nbr_deleted=0, idx_focus=-1;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        std::vector< smart_ptr<MagicBase> >::iterator iter = gc->gds.begin();
        std::vector< smart_ptr<MagicBase> >::iterator iter_focus;
        for( int i=0; iter!=gc->gds.end(); )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
            {
                idx_focus = i;
                iter_focus = iter;
            }
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
            {
                list_ctrl->DeleteItem(i);
                iter = gc->gds.erase(iter);
                gc->file_irrevocably_modified = true;
                nbr_deleted++;
            }
            else
            {
                ++iter;
                ++i;
            }
        }

        if( nbr_deleted==0 && idx_focus>=0 )
        {
            list_ctrl->DeleteItem(idx_focus);
            iter = gc->gds.erase(iter_focus);
            gc->file_irrevocably_modified = true;
            nbr_deleted++;
        }
    }
    dbg_printf( "%d games deleted\n", nbr_deleted ); */
}

void GamesDialog::OnPaste( wxCommandEvent& WXUNUSED(event) )
{
    /*int idx_focus=0;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                idx_focus = i;
        }
        sz = gc_clipboard->gds.size();
        for( int i=sz-1; i>=0; i-- )    
        {                                 
            std::vector< smart_ptr<MagicBase> >::iterator iter = gc->gds.begin() + idx_focus;
            GameDocument gd;
            gc_clipboard->gds[i]->GetGameDocument(gd);
            gd.game_nbr = 0;
            gd.modified = true;
            make_smart_ptr( HoldDocument,new_doc,gd);
            gc->gds.insert( iter, std::move(new_doc) );
            gc->file_irrevocably_modified = true;
            wxListItem item;              
            list_ctrl->InsertItem( idx_focus, item );
            list_ctrl->SetItem( idx_focus, 0, "" );                     // game_nbr
            GameDocument *ptr = gc_clipboard->gds[i]->GetGameDocumentPtr();
            if( ptr )
            {
                list_ctrl->SetItem( idx_focus, 1, ptr->white );
                list_ctrl->SetItem( idx_focus, 2, ptr->white_elo );
                list_ctrl->SetItem( idx_focus, 3, ptr->black );
                list_ctrl->SetItem( idx_focus, 4, ptr->black_elo );
                list_ctrl->SetItem( idx_focus, 5, ptr->date );
                list_ctrl->SetItem( idx_focus, 6, ptr->site );
                list_ctrl->SetItem( idx_focus, 7, ptr->round );
                list_ctrl->SetItem( idx_focus, 8, ptr->result );
                list_ctrl->SetItem( idx_focus, 9, ptr->eco );
                list_ctrl->SetItem( idx_focus,10, ptr->moves_txt );
            }
        }
    } */
}

void GamesDialog::OnSave( wxCommandEvent& WXUNUSED(event) )
{
   // gc->FileSave( gc_clipboard );
}

void GamesDialog::OnPublish( wxCommandEvent& WXUNUSED(event) )
{
    gc->Publish( gc_clipboard );
}
