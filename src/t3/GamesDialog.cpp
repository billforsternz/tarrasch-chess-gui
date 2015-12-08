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
    EVT_BUTTON( ID_DB_SEARCH,           GamesDialog::OnSearch )
    EVT_BUTTON( wxID_CANCEL,            GamesDialog::OnCancel )
    EVT_BUTTON( ID_SAVE_ALL_TO_A_FILE,  GamesDialog::OnSaveAllToAFile )
    EVT_BUTTON( ID_BUTTON_1,            GamesDialog::OnButton1 )
    EVT_BUTTON( ID_BUTTON_2,            GamesDialog::OnButton2 )
    EVT_BUTTON( ID_BUTTON_3,            GamesDialog::OnButton3 )
    EVT_BUTTON( ID_BUTTON_4,            GamesDialog::OnButton4 )
    EVT_BUTTON( ID_BUTTON_5,            GamesDialog::OnButton5 )

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


GamesListCtrl::GamesListCtrl( GamesDialog *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
    : wxListCtrl( (wxWindow *)parent, id, pos, size, wxLC_REPORT|wxLC_VIRTUAL )
{
    this->parent = parent;
    mini_board = 0;
}

// Focus changes to new item;
void GamesListCtrl::ReceiveFocus( int focus_idx )
{
    if( focus_idx >= 0 )
    {
        track->focus_idx = focus_idx;
        parent->ReadItemWithSingleLineCache( focus_idx, track->info );

        int offset = parent->GetBasePositionIdx( track->info );
        initial_focus_offset = track->focus_offset = offset;
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

std::string GamesListCtrl::CalculateMoveTxt() const
{
    std::string previous_move_not_needed;
    return CalculateMoveTxt(previous_move_not_needed,track->info,track->focus_offset,track->updated_position);
}

std::string GamesListCtrl::CalculateMoveTxt( std::string &previous_move ) const
{
    return CalculateMoveTxt(previous_move,track->info,track->focus_offset,track->updated_position);
}

std::string GamesListCtrl::CalculateMoveTxt( CompactGame &info, int offset ) const
{
    std::string previous_move_not_needed;
    thc::ChessPosition updated_position_not_needed;
    return CalculateMoveTxt(previous_move_not_needed,info,offset,updated_position_not_needed);
}

std::string GamesListCtrl::CalculateMoveTxt( std::string &previous_move, CompactGame &info, int focus_offset, thc::ChessPosition &updated_position ) const
{
    bool position_updated = false;
    std::string move_txt;
	thc::ChessRules cr=info.GetStartPosition();
    for( size_t i=0; i<info.moves.size(); i++ )
    {
        thc::Move mv = info.moves[i];
        if( i>=focus_offset || i+1==focus_offset )
        {
            bool prev_move = (i+1 == focus_offset);
            bool first_move = (i == focus_offset);
            std::string s = mv.NaturalOut(&cr);
            if( cr.white || prev_move || first_move )
            {
                if( first_move )
                {
                    position_updated = true;
                    updated_position = cr;
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
                if( i+1 == info.moves.size() )
                {
                    move_txt += " ";
                    move_txt += info.r.result;
                }
                else if( i < info.moves.size()-5 && move_txt.length()>100 )
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
        updated_position = cr;
        move_txt = info.r.result;
    }
    return move_txt;
}
    
wxString GamesListCtrl::OnGetItemText( long item, long column) const
{
    char buf[1000];
    CompactGame info;
    std::string move_txt;
    const char *txt;
    parent->ReadItemWithSingleLineCache( item, info );
    switch( column )
    {
        default: txt =  "";                           break;
        case 1: txt =   info.r.white.c_str();         break;
        case 2: txt =   info.r.white_elo.c_str();     break;
        case 3: txt =   info.r.black.c_str();         break;
        case 4: txt =   info.r.black_elo.c_str();     break;
        case 5: txt =   info.r.date.c_str();          break;
        case 6: txt =   info.r.site.c_str();          break;
        case 7: txt =   info.r.round.c_str();         break;
        case 8: txt =   info.r.result.c_str();        break;
        case 9: txt =   info.r.eco.c_str();           break;
        case 10: sprintf( buf,"%d", info.moves.size() );
                 txt =  buf;                          break;
        case 11:
        {
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
                int idx = parent->GetBasePositionIdx( info );
                move_txt = CalculateMoveTxt( info, idx );
                move_txt = buf + move_txt;
            }
            txt = move_txt.c_str();
            break;
        }
    }
    wxString ws(txt);
    return ws;
}


// GamesDialog event table definition
BEGIN_EVENT_TABLE( GamesListCtrl, wxListCtrl )
    EVT_CHAR(GamesListCtrl::OnChar)
END_EVENT_TABLE()

void GamesListCtrl::OnChar( wxKeyEvent &event )
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
            if( track->focus_offset < track->info.moves.size() )
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
    db_search = (cr!=NULL);
    col_last_time = 0;
    col_consecutive = 0;
    focus_idx = 0;

    if( cr )
    {
        this->cr = *cr;
        this->cr_base = *cr;
    }
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
    if( !db_search )
    {
        nbr_games_in_list_ctrl = gc->gds.size();
        sprintf(buf,"%d games",nbr_games_in_list_ctrl);
    }
    else if( objs.gl->db_clipboard && db_req==REQ_POSITION )
    {
        nbr_games_in_list_ctrl = gc_clipboard->gds.size();
        sprintf(buf,"Using clipboard as database" );
    }
    else
    {
        nbr_games_in_list_ctrl = objs.db->SetDbPosition( db_req, cr );
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
    cprintf( "disp_width=%d, disp_height=%d\n", disp_width, disp_height );
    if( disp_width > 1366 )
        disp_width = 1366;
    if( disp_height > 768 )
        disp_height = 768;
    sz.x = (disp_width*90)/100;
    sz.y = (disp_height*36)/100;
    list_ctrl  = new GamesListCtrl( this, ID_PGN_LISTBOX, wxDefaultPosition, sz/*wxDefaultSize*/,wxLC_REPORT|wxLC_VIRTUAL );
    list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
    if( nbr_games_in_list_ctrl > 0 )
    {
        list_ctrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }

    list_ctrl->InsertColumn( 0, " "  ); // id==ID_PGN_DIALOG_FILE?"#":" "  );
    list_ctrl->InsertColumn( 1, "White"    );
    list_ctrl->InsertColumn( 2, "Elo"      );
    list_ctrl->InsertColumn( 3, "Black"    );
    list_ctrl->InsertColumn( 4, "Elo"      );
    list_ctrl->InsertColumn( 5, "Date"     );
    list_ctrl->InsertColumn( 6, "Site"     );
    list_ctrl->InsertColumn( 7, "Round"    );
    list_ctrl->InsertColumn( 8, "Result"   );
    list_ctrl->InsertColumn( 9, "ECO"      );
    list_ctrl->InsertColumn(10, "Ply"      );
    list_ctrl->InsertColumn(11, "Moves"    );
    int col_flag=0;
    int cols[20];

    // Only use the non volatile column widths if they validate okay (factory
    //  default is -1 (for all columns), which forces a recalc)
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
        objs.repository->nv.m_col10 > 0 &&
        objs.repository->nv.m_col11 > 0
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
        cols[11]= objs.repository->nv.m_col11;
    }
    else // else set some sensible defaults
    {
        int x   = (sz.x*98)/100;
        objs.repository->nv.m_col0 = cols[0] =   1*x/142;    // "Game #"
        objs.repository->nv.m_col1 = cols[1] =  16*x/142;    // "White" 
        objs.repository->nv.m_col2 = cols[2] =   5*x/142;    // "Elo W"
        objs.repository->nv.m_col3 = cols[3] =  16*x/142;    // "Black" 
        objs.repository->nv.m_col4 = cols[4] =   5*x/142;    // "Elo B" 
        objs.repository->nv.m_col5 = cols[5] =   9*x/142;    // "Date"  
        objs.repository->nv.m_col6 = cols[6] =  13*x/142;    // "Site"  
        objs.repository->nv.m_col7 = cols[7] =   6*x/142;    // "Round" 
        objs.repository->nv.m_col8 = cols[8] =   7*x/142;    // "Result"
        objs.repository->nv.m_col9 = cols[9] =   5*x/142;    // "ECO"   
        objs.repository->nv.m_col10 = cols[10] = 5*x/142;    // "Ply"   
        objs.repository->nv.m_col11= cols[11]=  54*x/142;    // "Moves"
    }
    cprintf( "cols[0] = %d\n", cols[0] );
    cprintf( "cols[1] = %d\n", cols[1] );
    cprintf( "cols[2] = %d\n", cols[2] );
    cprintf( "cols[3] = %d\n", cols[3] );
    cprintf( "cols[4] = %d\n", cols[4] );
    cprintf( "cols[5] = %d\n", cols[5] );
    cprintf( "Site cols[6] = %d\n", cols[6] );
    cprintf( "cols[7] = %d\n", cols[7] );
    cprintf( "cols[8] = %d\n", cols[8] );
    cprintf( "cols[9] = %d\n", cols[9] );
    cprintf( "cols[10] = %d\n", cols[10] );
    cprintf( "cols[11] = %d\n", cols[11] );
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
    list_ctrl->SetColumnWidth(10, cols[10] );   // "Ply"
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth(11, cols[11] );   // "Moves"
    gc->col_flags.push_back(col_flag);
    //int top_item;
    //bool resuming = gc->IsResumingPreviousWindow(top_item);
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
    GdvGetButtonGridDimensions( row1, col1, row2, col2 );
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
    if( id == ID_PGN_DIALOG_FILE )
    {
        wxButton* save_all_to_a_file = new wxButton ( this, wxID_SAVE, wxT("Save"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    else if( id == ID_GAMES_DIALOG_DATABASE )
    {
        wxButton* btn5 = new wxButton ( this, ID_BUTTON_5, wxT("Use Game"),
                                       wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(btn5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    else
    {
        wxButton* save_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save all"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

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
    GdvAddExtraControls();
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
    GdvListColClick( compare_col );
}




// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GamesDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    OnOk();
}

void GamesDialog::OnActivate(wxActivateEvent& event)
{
    GdvOnActivate();
}

// override
void GamesDialog::GdvOnActivate()
{
    if( !activated_at_least_once )
    {
        activated_at_least_once = true;
        Goto(0); // list_ctrl->SetFocus();
    }
}

void GamesDialog::Goto( int idx )
{
    if( list_ctrl )
    {
        int sz = list_ctrl->GetItemCount();
        if(  0<=idx && idx<sz )
        {

            // Move focus to a new location
            dirty = true;
            int old = focus_idx;
            list_ctrl->RefreshItem( idx );
            list_ctrl->ReceiveFocus( idx );
            list_ctrl->SetItemState( idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
            list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
            if( 0<=old && old<sz && old!=idx )
            {
                list_ctrl->SetItemState( old, 0, wxLIST_STATE_FOCUSED );
                list_ctrl->SetItemState( old, 0, wxLIST_STATE_SELECTED );
            }
            list_ctrl->SetFocus();
            int span = list_ctrl->GetCountPerPage() - 4;    // so that on average maybe 2 lines before idx are also visible
            list_ctrl->EnsureVisible(idx+10<sz?idx+span:sz-1);
            list_ctrl->EnsureVisible(idx);
        }
    }
}

void GamesDialog::ReadItemWithSingleLineCache( int item, CompactGame &info )
{
    if( !GdvTestAndClearIsCacheDirty() && (item==single_line_cache_idx) )
        info = single_line_cache;
    else
    {
        GdvReadItem( item, info );
        // cprintf( "GdvReadItem(%d) = %s-%s\n", item, info.r.white.c_str(), info.r.black.c_str() );
        single_line_cache_idx = item;
        single_line_cache = info;
    }
}

void GamesDialog::LoadGameOne( int idx, int focus_offset )
{
    static CompactGame info;
    ReadItemWithSingleLineCache( idx, info );
    GameDocument gd;
    gd.r = info.r;
    gd.LoadFromMoveList( info.moves, focus_offset );
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
        LoadGameOne( idx, track->focus_offset );
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
        focus_idx = idx;
        cprintf( "****  GamesDialog::OnListFocused() Prev idx=%d, New idx=%d\n", prev, idx );
        list_ctrl->ReceiveFocus( idx );
        list_ctrl->RefreshItem(prev);
    }
}



// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GamesDialog::OnOk()
{
    if( list_ctrl )
    {
        LoadGameOne( track->focus_idx, track->focus_offset );
        TransferDataToWindow();
        AcceptAndClose();
    }
}

void GamesDialog::OnSaveAllToAFile( wxCommandEvent& WXUNUSED(event) )
{
    GdvSaveAllToAFile();
}

void GamesDialog::OnCancel( wxCommandEvent& WXUNUSED(event) )
{
    GdvOnCancel();
    EndDialog( wxID_CANCEL );
}

// override
void GamesDialog::GdvOnCancel()
{
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void GamesDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    GdvHelpClick();
}

// overide
void GamesDialog::GdvHelpClick()
{
}

void GamesDialog::OnCheckBox( wxCommandEvent& event )
{
    bool checked = event.IsChecked();
    GdvCheckBox( checked );
}

// overide
void GamesDialog::GdvCheckBox( bool checked )
{
}

void GamesDialog::OnCheckBox2( wxCommandEvent& event )
{
    bool checked = event.IsChecked();
    GdvCheckBox2( checked );
}

// overide
void GamesDialog::GdvCheckBox2( bool checked )
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
    GdvUtility();
}

// overide
void GamesDialog::GdvUtility()
{
}

void GamesDialog::OnSearch( wxCommandEvent& WXUNUSED(event) )
{
    GdvSearch();
}

// overide
void GamesDialog::GdvSearch()
{
}

void GamesDialog::OnButton1( wxCommandEvent& WXUNUSED(event) )
{
    GdvButton1();
}

// overide
void GamesDialog::GdvButton1()
{
}

void GamesDialog::OnButton2( wxCommandEvent& WXUNUSED(event) )
{
    GdvButton2();
}

// overide
void GamesDialog::GdvButton2()
{
    GdvButton2();
}

void GamesDialog::OnButton3( wxCommandEvent& WXUNUSED(event) )
{
    GdvButton3();
}

// overide
void GamesDialog::GdvButton3()
{
}

void GamesDialog::OnButton4( wxCommandEvent& WXUNUSED(event) )
{
    GdvButton4();
}

// overide
void GamesDialog::GdvButton4()
{
}

void GamesDialog::OnButton5( wxCommandEvent& WXUNUSED(event) )
{
    GdvButton5();
}

// overide
void GamesDialog::GdvButton5()
{
}

// Move Stats or Transpositions selected
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
    GdvNextMove(idx);
}


// overide
void GamesDialog::GdvNextMove( int idx )
{
}
   
    
bool GamesDialog::ShowModalOk( std::string title )
{
    Init();
    Create( parent2, id, wxString(title.c_str()), pos, size, style );
    bool ok = (wxID_OK == ShowModal());
    objs.repository->nv.m_col0  = list_ctrl->GetColumnWidth( 0 );    // "Game #"
    objs.repository->nv.m_col1  = list_ctrl->GetColumnWidth( 1 );    // "White"
    objs.repository->nv.m_col2  = list_ctrl->GetColumnWidth( 2 );    // "Elo W"
    objs.repository->nv.m_col3  = list_ctrl->GetColumnWidth( 3 );    // "Black"
    objs.repository->nv.m_col4  = list_ctrl->GetColumnWidth( 4 );    // "Elo B" 
    objs.repository->nv.m_col5  = list_ctrl->GetColumnWidth( 5 );    // "Date"  
    objs.repository->nv.m_col6  = list_ctrl->GetColumnWidth( 6 );    // "Site"
    cprintf( "Site width = %d\n",   objs.repository->nv.m_col6 );
    objs.repository->nv.m_col7  = list_ctrl->GetColumnWidth( 7 );    // "Round" 
    objs.repository->nv.m_col8  = list_ctrl->GetColumnWidth( 8 );    // "Result"
    objs.repository->nv.m_col9  = list_ctrl->GetColumnWidth( 9 );    // "ECO"   
    objs.repository->nv.m_col10 = list_ctrl->GetColumnWidth(10 );    // "Ply"
    objs.repository->nv.m_col11 = list_ctrl->GetColumnWidth(11 );    // "Moves"
    return ok;
}

// This is used to load games from the database
bool GamesDialog::LoadGameTwo( GameDocument &gd )
{
    if( db_game_set )
        gd = db_game;
    return db_game_set;
}


void GamesDialog::OnListSelected( int idx )
{
    cprintf( "DbDialog::OnListSelected(%d)\n", idx );
    list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
    LoadGameOne( idx, track->focus_offset );
}

void GamesDialog::OnEditGameDetails( wxCommandEvent& WXUNUSED(event) )
{
    int idx = track->focus_idx;
    if( idx != -1 )
    {
        GameDocument *ptr = gc->gds[idx]->GetGameDocumentPtr();
        if( ptr )
        {
            GameDetailsDialog dialog( this );
            GameDocument temp = *ptr;
            if( dialog.Run( temp ) )
            {
                list_ctrl->SetItem( idx, 1, temp.r.white );
                list_ctrl->SetItem( idx, 2, temp.r.white_elo );
                list_ctrl->SetItem( idx, 3, temp.r.black );
                list_ctrl->SetItem( idx, 4, temp.r.black_elo );
                list_ctrl->SetItem( idx, 5, temp.r.date );
                list_ctrl->SetItem( idx, 6, temp.r.site );
                list_ctrl->SetItem( idx, 7, temp.r.round );
                list_ctrl->SetItem( idx, 8, temp.r.result );
                list_ctrl->SetItem( idx, 9, temp.r.eco );
                temp.modified = true;
                make_smart_ptr( GameDocument, new_smart_ptr, temp);
                gc->gds[idx] = std::move(new_smart_ptr);
                objs.gl->GameRedisplayPlayersResult();
                Goto(idx);
            }
        }
    }
}


void GamesDialog::OnEditGamePrefix( wxCommandEvent& WXUNUSED(event) )
{
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
    int idx_focus = -1;
    int nbr_copied = 0;
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
                gc_clipboard->gds.push_back( gc->gds[i] ); // assumes smart_ptr is std::shared_ptr
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
            gc_clipboard->gds.push_back( gc->gds[idx_focus] ); // assumes smart_ptr is std::shared_ptr
            nbr_copied++;
        }
    }
    dbg_printf( "%d games copied\n", nbr_copied );
}

void GamesDialog::OnCutOrDelete( bool cut )
{
    dirty = true;
    int nbr_cut=0, idx_focus=focus_idx;
    int sz=gc->gds.size();
    std::vector< smart_ptr<ListableGame> >::iterator iter = gc->gds.begin();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        std::vector<int> games_to_cut;
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
            {
                list_ctrl->SetItemState( i, 0, wxLIST_STATE_SELECTED );
                if( cut )
                {
                    if( nbr_cut == 0 )
                        gc_clipboard->gds.clear();
                    gc_clipboard->gds.push_back(std::move(gc->gds[i]));
                }
                games_to_cut.push_back(i);
                nbr_cut++;
            }
        }
        
        if( nbr_cut==0 && 0<=idx_focus && idx_focus<sz )
        {
            if( cut )
            {
                gc_clipboard->gds.clear();
                gc_clipboard->gds.push_back(std::move(gc->gds[idx_focus]));
            }
            games_to_cut.push_back(idx_focus);
            nbr_cut++;
        }
        if( nbr_cut > 0 )
        {
            gc->file_irrevocably_modified = true;
            for( int j=0; j<games_to_cut.size(); j++ )
            {
                int idx = games_to_cut[j];
                idx -= j;
                gc->gds.erase(iter+idx);
            }
            sz-=nbr_cut;
            list_ctrl->SetItemCount(sz);
            Goto( 0<=idx_focus && idx_focus<sz ? idx_focus : 0 );
            list_ctrl->RefreshItems(0,sz-1);
        }
    }
    dbg_printf( "%d games %s\n", nbr_cut, cut?"cut":"deleted" );
}

void GamesDialog::OnBoard2Game( wxCommandEvent& WXUNUSED(event) )
{
    dirty = true;
    int idx_focus=0;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz && 0<=focus_idx && focus_idx<sz )
    {
        int insert_idx = focus_idx;
        cprintf( "insert_idex=%d\n", insert_idx );
        std::vector< smart_ptr<ListableGame> >::iterator iter = gc->gds.begin() + insert_idx;
        GameDocument gd = objs.gl->gd;
        gd.modified = true;
        GameDetailsDialog dialog( this );
        if( dialog.Run(gd) )
        {
            uint32_t temp = ++objs.gl->game_being_edited_tag;
            gd.SetGameBeingEdited( temp );
            objs.gl->gd = gd;
            objs.gl->GameRedisplayPlayersResult();
            gd.game_nbr = 0;
            gd.modified = true;
            gc->file_irrevocably_modified = true;
            make_smart_ptr( GameDocument, new_doc, gd );
            gc->gds.insert( iter, std::move(new_doc) );
            sz++;
            list_ctrl->SetItemCount( sz );
            //int ret = list_ctrl->InsertItem( insert_idx, item );
            //cprintf( "ret=%d\n", ret );
            list_ctrl->SetItemState( insert_idx, 0, wxLIST_STATE_FOCUSED );
            list_ctrl->SetItemState( insert_idx, 0, wxLIST_STATE_SELECTED );
            Goto( insert_idx );
            list_ctrl->RefreshItems(0,sz-1);
        }
    } 
}

// override
void GamesDialog::GdvSaveAllToAFile()
{
    wxFileDialog fd( objs.frame, "Save all listed games to a new .pgn file", "", "", "*.pgn", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
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
    }
}

void GamesDialog::OnCut( wxCommandEvent& WXUNUSED(event) )
{
    OnCutOrDelete( true );
}

void GamesDialog::OnDelete( wxCommandEvent& WXUNUSED(event) )
{
    OnCutOrDelete( false );
}

void GamesDialog::OnPaste( wxCommandEvent& WXUNUSED(event) )
{
    int idx_focus=focus_idx;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz && 0<=idx_focus && idx_focus<sz )
    {
        int sz2 = gc_clipboard->gds.size();
        std::vector< smart_ptr<ListableGame> >::iterator iter = gc->gds.begin() + idx_focus;
        for( int i=sz2-1; i>=0; i-- )
        {                                 
            gc->gds.insert( iter, gc_clipboard->gds[i] );
            gc->file_irrevocably_modified = true;
        }
        sz += sz2;
        list_ctrl->SetItemCount(sz);
        Goto(idx_focus);
        list_ctrl->RefreshItems(0,sz-1);
    }
}

void GamesDialog::OnSave( wxCommandEvent& WXUNUSED(event) )
{
    gc->FileSave( gc_clipboard );
}

void GamesDialog::OnPublish( wxCommandEvent& WXUNUSED(event) )
{
    gc->Publish( gc_clipboard );
}


static GamesDialog *backdoor;

static bool compare( const smart_ptr<ListableGame> g1, const smart_ptr<ListableGame> g2 )
{
    bool lt=false;
    bool use_atoi=false;
    const char *parm1;
    const char *parm2;
    switch( backdoor->compare_col )
    {
        case 1:
        {
            parm1 = g1->White();
            parm2 = g2->White();
            break;
        }
        case 2:
        {
            parm1 = g1->WhiteElo();
            parm2 = g2->WhiteElo();
            use_atoi = true;
            break;
        }
        case 3:
        {
            parm1 = g1->Black();
            parm2 = g2->Black();
            break;
        }
        case 4:
        {
            parm1 = g1->BlackElo();
            parm2 = g2->BlackElo();
            use_atoi = true;
            break;
        }
        case 5:
        {
            parm1 = g1->Date();
            parm2 = g2->Date();
            break;
        }
        case 6:
        {
            parm1 = g1->Site();
            parm2 = g2->Site();
            break;
        }
        case 7:
        {
            parm1 = g1->Round();
            parm2 = g2->Round();
            break;
        }
        case 8:
        {
            parm1 = g1->Result();
            parm2 = g2->Result();
            break;
        }
        case 9:
        {
            parm1 = g1->Eco();
            parm2 = g2->Eco();
            break;
        }
        case 10: // Ply
        {
            int int1 = g1->RefCompressedMoves().length();
            int int2 = g2->RefCompressedMoves().length();
            lt = int1 < int2;
            return lt;
        }
    }
    if( use_atoi )
    {
        int int1 = atoi(parm1);
        int int2 = atoi(parm2);
        lt = int1 < int2;
    }
    else
    {
        int negative_if_lt0 = strcmp(parm1,parm2);
        lt = (negative_if_lt0 < 0);
    }
    return lt;
}

static bool rev_compare( const smart_ptr<ListableGame> g1, const smart_ptr<ListableGame> g2 )
{
    bool lt=false;
    bool use_atoi=false;
    const char *parm1;
    const char *parm2;
    switch( backdoor->compare_col )
    {
        case 1:
        {
            parm1 = g1->White();
            parm2 = g2->White();
            break;
        }
        case 2:
        {
            parm1 = g1->WhiteElo();
            parm2 = g2->WhiteElo();
            use_atoi = true;
            break;
        }
        case 3:
        {
            parm1 = g1->Black();
            parm2 = g2->Black();
            break;
        }
        case 4:
        {
            parm1 = g1->BlackElo();
            parm2 = g2->BlackElo();
            use_atoi = true;
            break;
        }
        case 5:
        {
            parm1 = g1->Date();
            parm2 = g2->Date();
            break;
        }
        case 6:
        {
            parm1 = g1->Site();
            parm2 = g2->Site();
            break;
        }
        case 8:
        {
            parm1 = g1->Result();
            parm2 = g2->Result();
            break;
        }
        case 9:
        {
            parm1 = g1->Eco();
            parm2 = g2->Eco();
            break;
        }
        case 10:     //Ply
        {
            int int1 = g1->RefCompressedMoves().length();
            int int2 = g2->RefCompressedMoves().length();
            lt = int2 < int1;
            return lt;
        }
    }
    if( use_atoi )
    {
        int int1 = atoi(parm1);
        int int2 = atoi(parm2);
        lt = int2 < int1;
    }
    else
    {
        int negative_if_lt0 = strcmp(parm2,parm1);
        lt = (negative_if_lt0 < 0);
    }
    return lt;
}

static bool compare_blob( const MoveColCompareElement &e1, const MoveColCompareElement &e2 )
{
    bool lt = false;    // lt = less than conventionally, and since the default sort order is smaller first,
                        //   it can be read as "true if e1 comes first"
    if( e1.transpo == e2.transpo )
        lt = (e1.blob < e2.blob);
    else
        lt = (e1.transpo < e2.transpo);     // smaller transpo nbr should come first
    return lt;
}

static bool compare_counts( const MoveColCompareElement &e1, const MoveColCompareElement &e2 )
{
    bool lt = false;    // lt = less than conventionally, and since the default sort order is smaller first,
                        //   it can be read as "true if e1 comes first"
    if( e1.transpo != e2.transpo )
        lt = (e1.transpo < e2.transpo);     // smaller transpo nbr should come first
    else
    {
        unsigned int len = e1.blob.length();
        if( e2.blob.length() < len )
            len = e2.blob.length();
        for( unsigned int i=0; i<len; i++ )
        {
            if( e1.counts[i] != e2.counts[i] )
            {
                lt = (e1.counts[i] > e2.counts[i]);     // larger count comes first
                return lt;
            }
        }
        lt = (e1.blob.length() > e2.blob.length());     // longer length comes first
    }
    return lt;
}


/*
   Tracking down the issue of "false friends" in move column sorting was tricky. I first
   noticed the problem in the position after 1.e4 c5 2.b3 d6 with the giant123 database.
   The three 7...g6 games below were incorrectly appearing before the four 7...e6 games.
   The solution to the problem was the false_friend_detector bool array, removing the
   few lines of code where that is created, initialised, set and tested will bring the
   problem back.

    1.e4 c5 2.b3 bug hunt> Found (idx=14, e6), Sobeck,G-Vlasak,Lu
    1.e4 c5 2.b3 bug hunt> Found (idx=283, g6), Shanava,K-Magalashvili,Da
    1.e4 c5 2.b3 bug hunt> Found (idx=384, g6), Boskovic, Malina-Rakic, Marija
    1.e4 c5 2.b3 bug hunt> Found (idx=390, e6), Stuchly, Radim-Kubos, Rostislav
    1.e4 c5 2.b3 bug hunt> Found (idx=658, e6), Galko, Agnes-Palinkas, Balazs
    1.e4 c5 2.b3 bug hunt> Found (idx=708, g6), Miljanic, Boro-Florean, Andrei
    1.e4 c5 2.b3 bug hunt> Found (idx=1139, e6), Hagenbeck Huebert, Hartmut-Kaeser,Udo
*/

//#define FALSE_FRIEND_BUG_HUNT
#ifdef FALSE_FRIEND_BUG_HUNT
static void make_test_game( std::string &blob, int argc, const char *argv[] )
{
    thc::ChessRules cr;
    thc::Move mv;
    std::vector<thc::Move> mvs;
    for( int i=0; i<argc; i++ )
    {
        mv.NaturalInFast( &cr, argv[i] );
        mvs.push_back(mv);
        cr.PlayMove(mv);
    }
    CompressMoves press;
    blob = press.Compress(mvs);
}
#endif

#ifdef FALSE_FRIEND_BUG_HUNT
static void diagnose_bug( std::string msg, std::vector<MoveColCompareElement> &inter )
{
    cprintf( "%s\n", msg.c_str() );
    unsigned int sz = inter.size();
    int nbr=0;
    for( int i=0; i<sz; i++ )
    {
        MoveColCompareElement &e = inter[i];
        bool g6 = (e.idx==283 || e.idx==384 || e.idx==708); 
        bool e6 = (e.idx==14  || e.idx==390 || e.idx==658 || e.idx==1139);
        if( e6 || g6 )
        {
            if( nbr==0 && i-1>=0 )
            {
                MoveColCompareElement &f = inter[i-1];
                cprintf( "At %d,   : ", i-1 ); 
                for( int j=0; j<10; j++ )
                    cprintf( "%s%02x %d%s", j==0?"{":"", (j<f.counts.size() ? 0xff&f.blob[j] : 0), (j<f.counts.size() ? f.counts[j] : -1), j+1<10?",":"}\n" );
            }

            cprintf( "At %d, %s: ", i, e6?"e6":"g6" ); 
            for( int j=0; j<10; j++ )
                cprintf( "%s%02x %d%s", j==0?"{":"", (j<e.counts.size() ? 0xff&e.blob[j] : 0), (j<e.counts.size() ? e.counts[j] : -1), j+1<10?",":"}\n" );

            if( nbr==6 && i+1<inter.size() )
            {
                MoveColCompareElement &f = inter[i+1];
                cprintf( "At %d,   : ", i+1  ); 
                for( int j=0; j<10; j++ )
                    cprintf( "%s%02x %d%s", j==0?"{":"", (j<f.counts.size() ? 0xff&f.blob[j] : 0), (j<f.counts.size() ? f.counts[j] : -1), j+1<10?",":"}\n" );
            }
            nbr++;
        }
    }
}
#endif

// Overridable - base classes may calculate transpo etc
bool GamesDialog::MoveColCompareReadGame( MoveColCompareElement &e, int idx, const char *blob )
{        
    e.idx = idx;
    e.blob = std::string( blob );
    e.transpo = 0;
    e.counts.resize( e.blob.length() );
    return true;
}

void GamesDialog::MoveColCompare( std::vector< smart_ptr<ListableGame> > &gds )
{
    // Temp testing
    #ifdef FALSE_FRIEND_BUG_HUNT
    std::string blob1, blob2;
    const char *moves1[] = { "e4", "c5", "b3", "d6", "Bb2", "Nf6", "Bb5+", "Bd7", "Bxd7+", "Qxd7", "d3", "Nc6", "Ne2", "g6" };
    make_test_game( blob1, 14, moves1 );
    const char *moves2[] = { "e4", "c5", "b3", "d6", "Bb2", "Nf6", "Bb5+", "Bd7", "Bxd7+", "Qxd7", "d3", "Nc6", "Ne2", "e6" };
    make_test_game( blob2, 14, moves2 );
    #endif

    std::vector<MoveColCompareElement> inter;     // intermediate representation
    
    // Step 1, do a conventional std::string sort
    unsigned int sz = gds.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        #ifdef FALSE_FRIEND_BUG_HUNT
        std::string blob = blob1;
        for( int j=0; j<2; j++ )
        {
            if( strlen(gds[i]->CompressedMoves())>=14 && 0==memcmp( blob.c_str(), gds[i]->CompressedMoves(), 14 )  )
            {
                cprintf( "1.e4 c5 2.b3 bug hunt> Found (idx=%d, %s), %s-%s\n", i, j==0?"g6":"e6", gds[i]->White(), gds[i]->Black() );
            }
            blob = blob2;
        }
        #endif
        MoveColCompareElement e;
        if( MoveColCompareReadGame(e,i,gds[i]->CompressedMoves())  )
            inter.push_back(e);
    }
    std::sort( inter.begin(), inter.end(), compare_blob );
    #ifdef FALSE_FRIEND_BUG_HUNT
    diagnose_bug( "After step 1", inter );
    #endif

    // Step 2, work out the nbr of moves in clumps of moves
    /*
     // Imagine that the compressed one byte codes sort in the same order as multi-char ascii move
     //  representations (they don't but the key thing is that they are deterministic - the actual order
     //  doesn't matter)
     A)  1.d4 Nf6 2.c4 e6 3.Nc3
     B)  1.d4 Nf6 2.c4 e6 3.Nf3
     C)  1.d4 Nf6 2.c4 e6 3.Nf3
     D)  1.d4 d5 2.c4 e6 3.Nc3
     E)  1.d4 d5 2.c4 e6 3.Nf3
     F)  1.d4 d5 2.c4 e6 3.Nf3
     G)  1.d4 d5 2.c4 c5 3.d5
     
     // Calculate the counts like this
              j=0 j=1 j=2 j=3 j=4
     i=0  A)  7
     i=1  B)  7
     i=2  C)  7
     i=3  D)  7      j=0 count 7 '1.d4's at offset 0 into each game
     i=4  E)  7
     i=5  F)  7
     i=6  G)  7
     
              j=0 j=1 j=2 j=3 j=4
     i=0  A)  7   3
     i=1  B)  7   3
     i=2  C)  7   3
     i=3  D)  7   4    j=1 count 3 '1...Nf6's and 4 '1...d5's
     i=4  E)  7   4
     i=5  F)  7   4
     i=6  G)  7   4
     
              j=0 j=1 j=2 j=3 j=4
     i=0  A)  7   3   7   6   1
     i=1  B)  7   3   7   6   2
     i=2  C)  7   3   7   6   2     etc.
     i=3  D)  7   4   7   6   1
     i=4  E)  7   4   7   6   2
     i=5  F)  7   4   7   6   2
     i=6  G)  7   4   7   1   1
     
     //  Now re-sort based on these counts, bigger counts come first
     E)  7   4   7   6   2
     F)  7   4   7   6   2
     D)  7   4   7   6   1
     G)  7   4   7   1   1
     B)  7   3   7   6   2
     C)  7   3   7   6   2
     A)  7   3   7   6   1
     
     // So final ordering is according to line popularity
     E)  1.d4 d5 2.c4 e6 3.Nf3
     F)  1.d4 d5 2.c4 e6 3.Nf3
     D)  1.d4 d5 2.c4 e6 3.Nc3
     G)  1.d4 d5 2.c4 c5 3.d5
     B)  1.d4 Nf6 2.c4 e6 3.Nf3
     C)  1.d4 Nf6 2.c4 e6 3.Nf3
     A)  1.d4 Nf6 2.c4 e6 3.Nc3

     Actually, experience shows we need a refinement. Look at the refined
     initial count calculation below. The asterisked moves are "false friends",
     they should break the vertical run from above even though they are the
     same move as the move above them.

          j=0     j=1    j=2      j=3    j=4
     A)   1.d4,7  Nf6,3  2.c4,3   e6,3   3.Nc3,1
     B)   1.d4,7  Nf6,3  2.c4,3   e6,3   3.Nf3,2
     C)   1.d4,7  Nf6,3  2.c4,3   e6,3   3.Nf3,2
     D)   1.d4,7  d5,4   2.c4*,4  e6*,3  3.Nc3,1
     E)   1.d4,7  d5,4   2.c4*,4  e6*,3  3.Nf3,2
     F)   1.d4,7  d5,4   2.c4*,4  e6*,3  3.Nf3,2
     G)   1.d4,7  d5,4   2.c4*,4  c5,1   3.d5,1

     Our false friend detector is very simple. Our count calculation
     algorithm does complete columns, so j=0 then j=1, then j=2, etc. Keep an
     array of flags, initially clear, one flag per row.

     For each column, 1) set the flag for any row where a vertical run is broken
     Also, 2) if we find the row flag was previously set, break the vertical run 
     there even if the move is unchanged

     At stage j=0, no flags previously set, no vertical runs broken.
     At stage j=1, 1) set the flag for row D.
     At stage j=2, 2) break the vertical run since the flag is set for row D.
     At stage j=3, 2) break the vertical run since the flag is set for row D.
               and 1) set the flag for row G
     At stage j=4, 1) set the flag for row B and E, 2) vertical runs would be
              broken at rows D) and G) although the move changes so this is moot

*/    
    
    sz = inter.size();
    std::vector<bool> false_friend_detector(inter.size());
    std::fill( false_friend_detector.begin(), false_friend_detector.end(), false );
    bool at_least_one = true;
    for( unsigned int j=0; at_least_one; j++ )
    {
        at_least_one = false;  // stop when we've passed end of all strings
        char current='\0';
        char current_prev_move='\0';
        unsigned int start=0;
        bool run_in_progress=false;
        for( unsigned int i=0; i<sz; i++ )
        {
            MoveColCompareElement &e = inter[i];
            
            // A short game stops runs
            if( j >= e.blob.length() )
            {
                if( run_in_progress )
                {
                    false_friend_detector[i] = true;
                    run_in_progress = false;
                    int count = i-start;
                    for( int k=start; k<i; k++ )
                    {
                        MoveColCompareElement &f = inter[k];
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
                //  or if false friend detector fires
                if( c!=current || false_friend_detector[i] )
                {
                    int count = i-start;
                    for( int k=start; k<i; k++ )
                    {
                        MoveColCompareElement &f = inter[k];
                        f.counts[j] = count;
                    }
                    false_friend_detector[i] = true;
                    current = c;  // start a new run
                    start = i;
                }
                
                // And/Or because we reach bottom
                if( i+1 == sz )
                {
                    int count = sz - start;
                    for( int k=start; k<sz; k++ )
                    {
                        MoveColCompareElement &f = inter[k];
                        f.counts[j] = count;
                    }
                }
            }
        }
    }
    #ifdef FALSE_FRIEND_BUG_HUNT
    diagnose_bug( "After step 2", inter );
    #endif
    
    // Step 3 sort again using the counts
    std::sort( inter.begin(), inter.end(), compare_counts );
    #ifdef FALSE_FRIEND_BUG_HUNT
    diagnose_bug( "After step 3", inter );
    #endif
    
    // Step 4 build sorted version of games list
    std::vector< smart_ptr<ListableGame> > temp;
    sz = inter.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        MoveColCompareElement &e = inter[i];
        temp.push_back( gds[e.idx] );
    }
    
    // Step 5 replace original games list
    gds = temp;
}

void GamesDialog::ColumnSort( int compare_col, std::vector< smart_ptr<ListableGame> > &displayed_games )
{
    if( displayed_games.size() > 0 )
    {
        if( compare_col == col_last_time )
            col_consecutive++;
        else
            col_consecutive=0;
        this->compare_col = compare_col;
        backdoor = this;
        if( compare_col == 11 )
        {
            if( col_consecutive%2 == 0 )
                MoveColCompare(displayed_games);
            else
                std::reverse( displayed_games.begin(), displayed_games.end() );
        }
        else
            std::sort( displayed_games.begin(), displayed_games.end(), (col_consecutive%2==0)?compare:rev_compare );
        nbr_games_in_list_ctrl = displayed_games.size();
        list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
        list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
        int top = list_ctrl->GetTopItem();
        int count = 1 + list_ctrl->GetCountPerPage();
        if( count > nbr_games_in_list_ctrl )
            count = nbr_games_in_list_ctrl;
        for( int i=0; i<count; i++ )
            list_ctrl->RefreshItem(top++);
        Goto(0);
        col_last_time = compare_col;
    }
}

void GamesDialog::GdvListColClick( int compare_col )
{
    ColumnSort( compare_col, gc->gds );
}

