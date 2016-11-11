/****************************************************************************
 * Custom dialog - list of games from database, file, session, or clipboard
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
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
#include "CtrlChessBoard.h"
#include "DbDialog.h"
#include "Lang.h"
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
    EVT_RADIOBUTTON( ID_SITE_EVENT,     GamesDialog::OnSiteEvent )
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
    EVT_BUTTON( ID_DIALOG_ECO,          GamesDialog::OnEco )
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

static bool gbl_right_arrow_pressed;

GamesListCtrl::GamesListCtrl( GamesDialog *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
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
        int offset = parent->GetBasePositionIdx( track->info, true );
        if( browse_map.count(track->info.game_id) > 0 )
            offset = browse_map[track->info.game_id];
        initial_focus_offset = track->focus_offset = offset;
        if( mini_board )
        {
            std::string previous_move;
			thc::Move previous_move_bin;
            CalculateMoveTxt( previous_move, previous_move_bin );
			if( previous_move_bin.Valid() )
			{
				mini_board->SetHighlight1(previous_move_bin.src);
				mini_board->SetHighlight2(previous_move_bin.dst);
			}
			else
			{
				mini_board->ClearHighlight1();
				mini_board->ClearHighlight2();
			}
            mini_board->SetChessPosition( track->updated_position );
            std::string desc = track->info.Description();
            if( previous_move.length() > 0 )
            {
                desc += ", after ";
                desc += previous_move;
            }
            if( !gbl_right_arrow_pressed )
            {
                desc += "  (use right arrow key to browse)";
            }
            parent->player_names->SetLabel(wxString(desc.c_str()));
        }
    }
}

std::string GamesListCtrl::CalculateMoveTxt() const
{
    std::string previous_move_not_needed;
    thc::Move previous_move_bin_not_needed;
    return CalculateMoveTxt(previous_move_not_needed,track->info,track->focus_offset,track->updated_position,previous_move_bin_not_needed);
}

std::string GamesListCtrl::CalculateMoveTxt( std::string &previous_move, thc::Move &previous_move_bin ) const
{
    return CalculateMoveTxt(previous_move,track->info,track->focus_offset,track->updated_position,previous_move_bin);
}

std::string GamesListCtrl::CalculateMoveTxt( CompactGame &info, int offset ) const
{
    std::string previous_move_not_needed;
    thc::Move previous_move_bin_not_needed;
    thc::ChessPosition updated_position_not_needed;
    return CalculateMoveTxt(previous_move_not_needed,info, offset,updated_position_not_needed,previous_move_bin_not_needed);
}

std::string GamesListCtrl::CalculateMoveTxt( std::string &previous_move, CompactGame &info, int focus_offset, thc::ChessPosition &updated_position, thc::Move &previous_move_bin ) const
{
    bool position_updated = false;
	previous_move_bin.Invalid();
    std::string move_txt;
	thc::ChessRules cr=info.GetStartPosition();
    int nbr_moves = info.moves.size();
    for( int i=0; i<nbr_moves; i++ )
    {
        thc::Move mv = info.moves[i];
        if( i>=focus_offset || i+1==focus_offset )
        {
            bool prev_move = (i+1 == focus_offset);
            bool first_move = (i == focus_offset);
            std::string s = mv.NaturalOut(&cr);
            LangOut(s);
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
			{
                previous_move = s;
				previous_move_bin = mv;
			}
            else
            {
                move_txt += s;
                if( i+1 == nbr_moves )
                {
                    move_txt += " ";
                    move_txt += info.r.result;
                }
                else if( i < nbr_moves-5 && move_txt.length()>100 )
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
        case 6: txt =   objs.repository->nv.m_event_not_site ? info.r.event.c_str() : info.r.site.c_str();          break;
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
                int idx = parent->GetBasePositionIdx( info, false );
                if( browse_map.count(info.game_id) > 0 )
                    idx = browse_map.at(info.game_id);  // browse_map[info.game_id] doesn't compile because [] operator is not const if it is on lhs!
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
    int nbr_moves = track->info.moves.size();
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
            if( track->focus_offset < nbr_moves )
            {
                track->focus_offset++;
                update = true;
                gbl_right_arrow_pressed = true;
            }
            break;
        default:
            event.Skip();
    }
    if( update )
    {
        RefreshItem(track->focus_idx);
		if( track->info.game_id != 0 )	// only map games that have a real game_id
	        browse_map[track->info.game_id] = track->focus_offset;
        if( mini_board )
        {
            std::string previous_move;
			thc::Move previous_move_bin;
            CalculateMoveTxt( previous_move, previous_move_bin );
			if( previous_move_bin.Valid() )
			{
				mini_board->SetHighlight1(previous_move_bin.src);
				mini_board->SetHighlight2(previous_move_bin.dst);
			}
			else
			{
				mini_board->ClearHighlight1();
				mini_board->ClearHighlight2();
			}
            mini_board->SetChessPosition( track->updated_position );
            std::string desc = track->info.Description();
            if( previous_move.length() > 0 )
            {
                desc += ", after ";
                desc += previous_move;
            }
            if( !gbl_right_arrow_pressed )
            {
                desc += "  (use arrow keys to browse)";
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
    col_last_time = -1;
    col_consecutive = 0;
    sort_order_first = true;
    focus_idx = 0;

    if( cr )
    {
        this->cr = *cr;
        this->cr_base = *cr;
    }
    single_line_cache_idx = -1;
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
    preview_game_set = false;
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
   wxWindowID id_, const wxString& caption,
   const wxPoint& pos_, const wxSize& size_, long style_ )
{
    bool okay=true;

    // We have to set extra styles before creating the dialog
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS/*|wxDIALOG_EX_CONTEXTHELP*/ );
    if( !wxDialog::Create( parent, id_, caption, pos_, size_, style_ ) )
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
        nbr_games_in_list_ctrl = objs.db->SetDbPosition( db_req );
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
    list_ctrl  = new GamesListCtrl( this, ID_PGN_LISTBOX, wxDefaultPosition, sz/*wxDefaultSize*/);
    list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
    if( nbr_games_in_list_ctrl > 0 )
    {
        list_ctrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }

    list_ctrl->InsertColumn( 0, "#"  ); // id==ID_PGN_DIALOG_FILE?"#":" "  );
    list_ctrl->InsertColumn( 1, "White"    );
    list_ctrl->InsertColumn( 2, "Elo"      );
    list_ctrl->InsertColumn( 3, "Black"    );
    list_ctrl->InsertColumn( 4, "Elo"      );
    list_ctrl->InsertColumn( 5, "Date"     );
    list_ctrl->InsertColumn( 6, "Site/Event"     );
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
        objs.repository->nv.m_col0 = cols[0] =  3*x/190;     // "#"
        objs.repository->nv.m_col1 = cols[1] =  16*x/142;    // "White" 
        objs.repository->nv.m_col2 = cols[2] =   5*x/142;    // "Elo W"
        objs.repository->nv.m_col3 = cols[3] =  16*x/142;    // "Black" 
        objs.repository->nv.m_col4 = cols[4] =   5*x/142;    // "Elo B" 
        objs.repository->nv.m_col5 = cols[5] =   9*x/142;    // "Date"  
        objs.repository->nv.m_col6 = cols[6] =  13*x/142;    // "Site/Event"  
        objs.repository->nv.m_col7 = cols[7] =   6*x/142;    // "Round" 
        objs.repository->nv.m_col8 = cols[8] =   7*x/142;    // "Result"
        objs.repository->nv.m_col9 = cols[9] =   5*x/142;    // "ECO"   
        objs.repository->nv.m_col10= cols[10]=  54*x/142-2;  // "Moves"
        objs.repository->nv.m_col11 = cols[11] = 5*x/142;    // "Ply"   
    }
    cprintf( "cols[0] = %d\n", cols[0] );
    cprintf( "cols[1] = %d\n", cols[1] );
    cprintf( "cols[2] = %d\n", cols[2] );
    cprintf( "cols[3] = %d\n", cols[3] );
    cprintf( "cols[4] = %d\n", cols[4] );
    cprintf( "cols[5] = %d\n", cols[5] );
    cprintf( "cols[6] = %d\n", cols[6] );
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
    list_ctrl->SetColumnWidth( 6, cols[6] );    // "Site/Event"  
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 7, cols[7] );    // "Round"
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 8, cols[8] );    // "Result"
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth( 9, cols[9] );    // "ECO"   
    gc->col_flags.push_back(col_flag);
    list_ctrl->SetColumnWidth(10, cols[11] );   // "Ply"
    gc->col_flags.push_back(col_flag);                       // Note that Ply and Moves are inverted here for compatibility with T2 (on T2 there is no Ply so nv.m_col10 is Moves)
    list_ctrl->SetColumnWidth(11, cols[10] );   // "Moves"
    gc->col_flags.push_back(col_flag);
    box_sizer->Add(list_ctrl, 0, wxGROW|wxALL, 5);

    // A dividing line before the details
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // Create a panel beneath the list control, containing everything else
    hsiz_panel = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(hsiz_panel, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 10);
	bool interactive = false;
	bool normal_orientation = objs.canvas->GetNormalOrientation();
    mini_board = new CtrlChessBoard(interactive,normal_orientation,this);	// non interactive CtrlChessBoard
    list_ctrl->mini_board = mini_board;
    track = &mini_board_game;
    track->updated_position = cr;
    track->focus_idx = -1;
    track->focus_offset = 0;
    list_ctrl->track = track;
    mini_board->SetChessPosition( cr );

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
        wxButton* save_as_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save as"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(save_as_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
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

    // Overridden by specialised classes
    GdvEnumerateGames();

    // Select site/or event
    wxRadioButton *site_button = new wxRadioButton( this, ID_SITE_EVENT,
       wxT("&Site"), wxDefaultPosition, wxDefaultSize,  wxRB_GROUP );
    wxRadioButton *event_button = new wxRadioButton( this, ID_SITE_EVENT,
       wxT("&Event"), wxDefaultPosition, wxDefaultSize, 0 );
    site_button->SetValue( !objs.repository->nv.m_event_not_site );
    event_button->SetValue( objs.repository->nv.m_event_not_site );
    vsiz_panel_button1->Add(site_button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vsiz_panel_button1->Add(event_button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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
    int compare_col_ = event.GetColumn();
    GdvListColClick( compare_col_ );
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void GamesDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    OnOk();
}

void GamesDialog::OnActivate(wxActivateEvent& WXUNUSED(event))
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
        list_ctrl->SetFocus();
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

// LoadGameForPreview() LoadGameFromPreview() is a two step alternative to single LoadGame(), loads main variation
//  only so suitable only for database cache which holds compact games not full games
void GamesDialog::LoadGameForPreview( int idx, int focus_offset )
{
    static CompactGame info;
    ReadItemWithSingleLineCache( idx, info );
    GameDocument gd;
    gd.r = info.r;
	gd.game_id = info.game_id;
    gd.LoadFromMoveList( info.moves, focus_offset );
    preview_game = gd;
    preview_game_set = true;
}

void GamesDialog::OnListSelected( wxListEvent &event )
{
    if( list_ctrl )
    {
        int idx = event.m_itemIndex;
        cprintf( "GamesDialog::OnListSelected(%d)\n", idx );
        list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
        LoadGameForPreview( idx, track->focus_offset );
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
        LoadGameForPreview( track->focus_idx, track->focus_offset );
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
void GamesDialog::GdvCheckBox( bool WXUNUSED(checked) )
{
}

void GamesDialog::OnCheckBox2( wxCommandEvent& event )
{
    bool checked = event.IsChecked();
    GdvCheckBox2( checked );
}

// overide
void GamesDialog::GdvCheckBox2( bool WXUNUSED(checked) )
{
}

void GamesDialog::OnRadio( wxCommandEvent& WXUNUSED(event))
{
}

void GamesDialog::OnSpin( wxCommandEvent& WXUNUSED(event))
{
}

void GamesDialog::OnComboBox( wxCommandEvent& WXUNUSED(event))
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
void GamesDialog::GdvNextMove( int WXUNUSED(idx) )
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
    objs.repository->nv.m_col6  = list_ctrl->GetColumnWidth( 6 );    // "Site/Event"
    cprintf( "Site width = %d\n",   objs.repository->nv.m_col6 );
    objs.repository->nv.m_col7  = list_ctrl->GetColumnWidth( 7 );    // "Round" 
    objs.repository->nv.m_col8  = list_ctrl->GetColumnWidth( 8 );    // "Result"
    objs.repository->nv.m_col9  = list_ctrl->GetColumnWidth( 9 );    // "ECO"   
    objs.repository->nv.m_col10 = list_ctrl->GetColumnWidth(11 );    // "Moves"
            // Note that Ply and Moves are inverted here for compatibility with T2 (on T2 there is no Ply so nv.m_col10 is Moves)
    objs.repository->nv.m_col11 = list_ctrl->GetColumnWidth(10 );    // "Ply"
    return ok;
}

// LoadGameForPreview() LoadGameFromPreview() is a two step alternative to single LoadGame(). Loads the main variation
//  only so suitable only for database cache which holds compact games not full games
bool GamesDialog::LoadGameFromPreview( GameDocument &gd )
{
    if( preview_game_set )
        gd = preview_game;
    return preview_game_set;
}

void GamesDialog::OnListSelected( int idx )
{
    cprintf( "DbDialog::OnListSelected(%d)\n", idx );
    list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
    LoadGameForPreview( idx, track->focus_offset );
}

void GamesDialog::OnEditGameDetails( wxCommandEvent& WXUNUSED(event) )
{
    int idx = track->focus_idx;
    if( idx != -1 )
    {
		GameDocument temp;
		gc->gds[idx]->ConvertToGameDocument(temp);
        GameDetailsDialog dialog( this );
        if( dialog.Run( temp, true ) )
        {
            list_ctrl->SetItem( idx, 1, temp.r.white );
            list_ctrl->SetItem( idx, 2, temp.r.white_elo );
            list_ctrl->SetItem( idx, 3, temp.r.black );
            list_ctrl->SetItem( idx, 4, temp.r.black_elo );
            list_ctrl->SetItem( idx, 5, temp.r.date );
            list_ctrl->SetItem( idx, 6, objs.repository->nv.m_event_not_site ? temp.r.event : temp.r.site );
            list_ctrl->SetItem( idx, 7, temp.r.round );
            list_ctrl->SetItem( idx, 8, temp.r.result );
            list_ctrl->SetItem( idx, 9, temp.r.eco );
			make_smart_ptr(GameDocument, new_smart_ptr, temp);
			gc->gds[idx] = std::move(new_smart_ptr);
            objs.gl->GameRedisplayPlayersResult();
            Goto(idx);
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
        Goto( 0<=idx_focus && idx_focus<sz ? idx_focus : 0 );
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
            for( size_t j=0; j<games_to_cut.size(); j++ )
            {
                int idx = games_to_cut[j];
                idx -= j;					//@ suspicious
                gc->gds.erase(iter+idx);
				iter = gc->gds.begin();
            }
            sz-=nbr_cut;
            list_ctrl->SetItemCount(sz);
            Goto( 0<=idx_focus && idx_focus<sz ? idx_focus : 0 );
            list_ctrl->RefreshItems(0,sz-1);
			char buf[80];
			sprintf(buf,"%d games",sz);
		    title_ctrl->SetLabel( buf );
        }
    }
    dbg_printf( "%d games %s\n", nbr_cut, cut?"cut":"deleted" );
}

void GamesDialog::OnSiteEvent( wxCommandEvent& WXUNUSED(event) )
{
    int gds_nbr = list_ctrl->GetItemCount();
    int top = list_ctrl->GetTopItem();
    int end = top + list_ctrl->GetCountPerPage();
    if( top > gds_nbr-1 )
        top = gds_nbr-1;
    if( end > gds_nbr )
        end = gds_nbr;
    objs.repository->nv.m_event_not_site = !objs.repository->nv.m_event_not_site;
    for( int i=top; i<end; i++ )    
    {
        CompactGame info;
        GdvReadItem( i, info );
        std::string field;
        if( objs.repository->nv.m_event_not_site )
            field = info.r.event;
        else
            field = info.r.site;
        list_ctrl->SetItem( i, 6, field.c_str() );
    }
    list_ctrl->RefreshItems( top, end-1 );
    //Goto( 0<=track->focus_idx && track->focus_idx<gds_nbr ? track->focus_idx : 0 );
    list_ctrl->SetFocus();
}

void GamesDialog::OnBoard2Game( wxCommandEvent& WXUNUSED(event) )
{
    dirty = true;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz && 0<=focus_idx && focus_idx<(sz>=1?sz:1) )
    {
        int insert_idx = focus_idx;
        cprintf( "insert_idx=%d\n", insert_idx );
        std::vector< smart_ptr<ListableGame> >::iterator iter = gc->gds.begin() + insert_idx;
        GameDocument gd = objs.gl->gd;
		bool ok = true;
        if( gd.r.white == "" )
		{
			GameDetailsDialog dialog( this );
			ok = dialog.Run(gd);
		}
		if( ok )
        {
			gd.modified |= objs.gl->undo.IsModified();
            gc->file_irrevocably_modified = true;
            gd.game_id = GameIdAllocateBottom(1);
            make_smart_ptr( GameDocument, new_doc, gd );

			// #Workflow Paste game into working file, create edit relationship if not already editing same game
			bool already_in = gc->TestGameInCache(gd);
			if( gc==&objs.gl->gc_pgn && !already_in )
				objs.gl->SetGameBeingEdited(gd,*new_doc);
            gc->gds.insert( iter, std::move(new_doc) );
            sz++;
            list_ctrl->SetItemCount( sz );
            //int ret = list_ctrl->InsertItem( insert_idx, item );
            //cprintf( "ret=%d\n", ret );
            list_ctrl->SetItemState( insert_idx, 0, wxLIST_STATE_FOCUSED );
            list_ctrl->SetItemState( insert_idx, 0, wxLIST_STATE_SELECTED );
            Goto( insert_idx );
            list_ctrl->RefreshItems(0,sz-1);
			char buf[80];
			sprintf(buf,"%d games",sz);
            objs.gl->gd = gd;
            objs.gl->GameRedisplayPlayersResult();
		    title_ctrl->SetLabel( buf );
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
        wxString dir2;
        wxFileName::SplitPath( fd.GetPath(), &dir2, NULL, NULL );
        objs.repository->nv.m_doc_dir = dir2;
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
	dirty = true;
    int idx_focus=focus_idx;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz && 0<=idx_focus && idx_focus<(sz>=1?sz:1) )
    {
        int sz2 = gc_clipboard->gds.size();
        std::vector< smart_ptr<ListableGame> >::iterator iter = gc->gds.begin() + idx_focus;
        for( int i=sz2-1; i>=0; i-- )
        {                                 
			gc_clipboard->gds[i]->SetGameBeingEdited(0);	// #Workflow, don't initially have edit relationships with pasted games
            gc->gds.insert( iter, gc_clipboard->gds[i] );
			iter = gc->gds.begin() + idx_focus;
			gc->file_irrevocably_modified = true;
        }
        sz += sz2;
        list_ctrl->SetItemCount(sz);
        Goto(idx_focus);
        list_ctrl->RefreshItems(0,sz-1);
		char buf[80];
  		sprintf(buf,"%d games",sz);
		title_ctrl->SetLabel( buf );
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

void GamesDialog::OnEco( wxCommandEvent& WXUNUSED(event) )
{
    int idx_focus=focus_idx;
    int sz=gc->gds.size();
    gc->Eco( gc_clipboard );
    Goto(idx_focus);
    list_ctrl->RefreshItems(0,sz-1);
}

#if 0
static void print_vector_int( const char *desc, std::vector<int> &v )
{ 
    char buf[500];
    char *p = buf;
    for( size_t i=0; i<v.size(); i++ )
    {
        sprintf( p, "%d ", v[i] );
        p = strchr(p,'\0');
        if( p>buf+400 && i+1<v.size() )
        {
            sprintf( p, "...(%d more)", v.size()-i-1 );
            break;
        }
    }
    cprintf( "%s [%s]\n", desc, buf );
}
#endif

static const int NBR_COLUMNS=12;
static int sort_order[NBR_COLUMNS];
static bool sort_forward[NBR_COLUMNS];
static GamesDialog *backdoor;
static float NLOGN_FACTOR=7.5;
static const uint32_t NBR_STEPS=1000;   // if this isn't 1000, then sort_scan() won't return permill
static bool predicate_transpo_activated;
static uint64_t predicate_count;
static uint64_t predicate_nbr_expected;
std::vector< smart_ptr<ListableGame> > *predicate_displayed_games;
//static std::vector< smart_ptr<ListableGame> >::iterator predicate_begin;
//static std::vector< smart_ptr<ListableGame> >::iterator predicate_end;
static bool (*predicate_func)( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 );
static uint32_t predicate_dist;
static bool no_recurs;
static uint32_t predicate_step;
static int permill_initial;
static int permill_max_so_far;
static ProgressBar *predicate_pb; 
//std::vector<int> sort_scan_vector;

static void dump( const uint8_t *ptr )
{
    for( int i=0; i< sizeof(smart_ptr<ListableGame>); i++  )
    {
        cprintf( "%02x", *ptr++ );
        if( (i+1)%4 == 0 )
            cprintf( " " );
    }
}
static int sort_scan()
{
    if( predicate_step == 0 )
        return 500;

    int permill=1;  // start at 1 since there are only 999 steps in loop
                    // so permill returned will be between 1 and 1000 inclusive
    uint64_t restore_value = predicate_count;
    for( uint32_t i=1; i<NBR_STEPS; i++ )
    {
        uint32_t step = i*predicate_step;      // first value of step is predicate_step
        uint32_t prev = step - predicate_step; // first value of prev is 0
        std::vector< smart_ptr<ListableGame> >::iterator it1 = predicate_displayed_games->begin()+prev; //predicate_displayed_games.begin()+prev;
        std::vector< smart_ptr<ListableGame> >::iterator it2 = predicate_displayed_games->begin()+step;
        bool ok = (predicate_displayed_games->begin()<=it1 && it1<predicate_displayed_games->end() ) && (predicate_displayed_games->begin()<=it2 && it2<predicate_displayed_games->end() );
        if( !ok )
        {
            cprintf( "danger danger pull up step=%u, dist=%u\n", step, predicate_dist );
            return permill;
        }
        no_recurs = true;
        bool a = ( (permill_initial==375) && ((uint32_t)predicate_count==0x01b00000) && i>150 );
        bool b = ( (permill_initial==375) && ((uint32_t)predicate_count==0x00010000) && i>980 );
        if( a || b )
        {
            const uint8_t *ptr1 = (uint8_t *)&predicate_displayed_games[prev];
            const uint8_t *ptr2 = (uint8_t *)&predicate_displayed_games[step];
            dump(ptr1);
            dump(ptr2);
            cprintf( " predicate_count=0x%08x, i in=%d", (uint32_t)predicate_count, i );
            smart_ptr<ListableGame> &g2 = *it2;
            cprintf( " [%d]",  g2->WhiteEloBin() );
            cprintf( " - [%d]\n",  g2->BlackEloBin() );
        }
        bool lower = (*predicate_func)( *it1, *it2 );
        if( (permill_initial==375) && ((uint32_t)predicate_count==0x01b00000) && i>150 )
            cprintf( "i out=%d\n", i );
        no_recurs = false;
        //bool lower = (*predicate_func)( *(predicate_displayed_games.begin()+prev), *(predicate_displayed_games.begin()+step) );
        if( lower )
            permill++;
    }
    predicate_count = restore_value; // sort_scan() shouldn't affect value of predicate_count
    //sort_scan_vector.push_back(permill);
    return permill;
}

static void sort_before( std::vector< smart_ptr<ListableGame> >::iterator begin,
                   std::vector< smart_ptr<ListableGame> >::iterator end,
                   bool (*predicate)( const smart_ptr<ListableGame> &e1, const smart_ptr<ListableGame> &e2 ),
                   ProgressBar *pb 
                 )
{
    //sort_scan_vector.clear();
    predicate_count = 0;
    predicate_pb = pb;
    predicate_func = predicate;
    unsigned int dist = std::distance( begin, end );
    predicate_dist = dist;

    // The following formula is based on experiment - std::sort() called the predicate function approx
    //  this many times for random input - hopefully this is approx worse case since in our experiments
    //  as far as we could tell it was actually called less if the input was patterned - including
    //  already sorted and already reverse sorted patterns (NLOGN_FACTOR = 7.5 initially is now
    //  an adjustable variable)
    predicate_nbr_expected = static_cast<uint64_t>( NLOGN_FACTOR * dist * log10(static_cast<float>(dist)) );
    //predicate_begin = begin;
    //predicate_end   = end;
    predicate_step  = dist/NBR_STEPS;
    permill_initial = sort_scan();
    permill_max_so_far = 0;
    cprintf( "Sorting order:" );
    for( int i=0; i<NBR_COLUMNS; i++ )
    {
        if( sort_order[i] == -1 )
        {
            cprintf( " -1" );
            break;
        }
        cprintf( " %d(%c)", sort_order[i], sort_forward[i]?'f':'b' );
    }
    cprintf( "\n" );
    cprintf( "Sorting: nbr_to_sort=%u, nbr_expected=%u, initial permill=%u\n", (unsigned int)dist,  (unsigned int)predicate_nbr_expected, (unsigned int)permill_initial );
}

static void sort_before_mc( std::vector< MoveColCompareElement >::iterator begin,
                   std::vector< MoveColCompareElement >::iterator end,
                   ProgressBar *pb 
                 )
{
    predicate_count = 0;
    predicate_pb = pb;
    unsigned int dist = std::distance( begin, end );
    predicate_dist = dist;

    // The following formula is based on experiment - std::sort() called the predicate function approx
    //  this many times for random input - hopefully this is approx worse case since in our experiments
    //  as far as we could tell it was actually called less if the input was patterned - including
    //  already sorted and already reverse sorted patterns
    predicate_nbr_expected = static_cast<uint64_t>( NLOGN_FACTOR * dist * log10(static_cast<float>(dist)) );
    //predicate_begin = begin;
    //predicate_end   = end;
    permill_initial = 0;
    permill_max_so_far = 0;
    cprintf( "Sorting: nbr_to_sort=%u, nbr_expected=%u, initial permill=%u\n", (unsigned int)dist,  (unsigned int)predicate_nbr_expected, (unsigned int)permill_initial );
}

static void sort_after()
{
    cprintf( "Sorting: nbr_expected=%u, nbr_actual=%u\n", (unsigned int)predicate_nbr_expected, (unsigned int)predicate_count );

    // Do a certain amount of adjustment based on prior experience
    float actual_factor = ((float)predicate_count) / ((float)predicate_nbr_expected);
    if( actual_factor > 1.0 )
        NLOGN_FACTOR = 7.5;  // return to default
    if( actual_factor < 0.9 )
        NLOGN_FACTOR = 0.9 * NLOGN_FACTOR;
    if( NLOGN_FACTOR > 7.5 ) // constrain to within range of 0.6*DEFAULT to DEFAULT
        NLOGN_FACTOR = 7.5;
    if( NLOGN_FACTOR < 4.0 )
        NLOGN_FACTOR = 4.0;
}

static void sort_progress_probe()
{
    if( predicate_pb )
    {
        int permill;
        // #define SCAN_BASED - Warning the SCAN_BASED approach to sorting seems flawed/dangerous. Extensive
        //  efforts to debug crashes during the sort only produced the conclusion that std::sort() doesn't
        //  necessarily guarantee that the container it is sorting is valid while it is sorting it.
        #ifdef SCAN_BASED
        if( (1000-permill_initial) > 100 ) // scan based approach is useless if input initially nearly sorted
        {
            permill = sort_scan();
            if( permill > permill_max_so_far )
                permill_max_so_far = permill;
            else
                permill = permill_max_so_far;
            predicate_pb->Perfraction( permill-permill_initial, (1000-permill_initial) );
        }
        else
        #endif
        {
            if( predicate_count>=predicate_nbr_expected || predicate_nbr_expected==0 )
                permill = 1000;
            else if( predicate_nbr_expected > 1000000 )
                permill = predicate_count / (predicate_nbr_expected/1000);
            else
                permill = (predicate_count*1000) / predicate_nbr_expected;
            predicate_pb->Permill( permill );
        }
    }
}

static bool master_predicate( const smart_ptr<ListableGame> &g1, const smart_ptr<ListableGame> &g2 )
{
    predicate_count++;
    if( (predicate_count & 0xffff) == 0 )
        sort_progress_probe();
    bool lt=false; bool eq=true;  // Note that both of these cannot, ever, both be true
    for( int i=0; eq && i<NBR_COLUMNS; i++ )  // tie break loop
    {
        int col = sort_order[i];
        if( col == -1 )
            //break;  // No more tie breaks, eq must be true, so return lt which will be false.
            return( g1->game_id < g2->game_id );  // Use game_id as ultimate tie-breaker
        bool forward = sort_forward[i];
        bool use_bin=false;
        bool use_rev_bin=false;
        uint32_t bin1=0;
        uint32_t bin2=0;
        const char *parm1=NULL;
        const char *parm2=NULL;
        switch( col )
        {
            case 0:
            {
                use_bin = true;
                bin1 = g1->game_id;
                bin2 = g2->game_id;
                break;
            }
            case 1:
            {
                parm1 = g1->White();
                parm2 = g2->White();
                break;
            }
            case 2:
            {
                use_rev_bin = true;
                bin1 = g1->WhiteEloBin();
                bin2 = g2->WhiteEloBin();
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
                use_rev_bin = true;
                bin1 = g1->BlackEloBin();
                bin2 = g2->BlackEloBin();
                break;
            }
            case 5:
            {
                use_rev_bin = true;
                bin1 = g1->DateBin();
                bin2 = g2->DateBin();
                break;
            }
            case 6:
            {
                parm1 = objs.repository->nv.m_event_not_site ? g1->Event() : g1->Site();
                parm2 = objs.repository->nv.m_event_not_site ? g2->Event() : g2->Site();
                break;
            }
            case 7:
            {
                use_bin = true;
                bin1 = g1->RoundBin();
                bin2 = g2->RoundBin();
                break;
            }
            case 8:
            {
                static int xform[] = {3,0,1,2};     // transform order to 1-0, 0-1, 1/2-1/2, *
                use_bin = true;
                bin1 = xform[g1->ResultBin() & 3];
                bin2 = xform[g2->ResultBin() & 3];
                break;
            }
            case 9:
            {
                use_bin = true;
                bin1 = g1->EcoBin();
                bin2 = g2->EcoBin();
                break;
            }
            case 10: // Ply
            {
                use_bin = true;
                bin1 = strlen(g1->CompressedMoves());
                bin2 = strlen(g2->CompressedMoves());
                break;
            }
            // case 11: // moves - not present, use the _mc version
        }
        if( use_bin )
        {
            lt = forward ? (bin1 < bin2) : (bin2 < bin1);
            eq = (bin1 == bin2);
        }
        else if( use_rev_bin )  // use this if we want big numbers first when you first
        {                       //  click on the column, eg elo
            lt = forward ? (bin2 < bin1) : (bin1 < bin2);
            eq = (bin1 == bin2);
        }
        else
        {
            int negative_if_parm1_lt_parm2 = strcmp(parm1,parm2);
            lt = forward ? (negative_if_parm1_lt_parm2 < 0) : (negative_if_parm1_lt_parm2 > 0);
            eq = (negative_if_parm1_lt_parm2 == 0);
        }
    }
    return lt;
}


// Use the suffix _mc to indicate special arrangements necessary for move column sorting
static std::vector< smart_ptr<ListableGame> >::iterator base_mc;

static bool master_predicate_mc( const MoveColCompareElement &e1, const MoveColCompareElement &e2 )
{
    predicate_count++;
    if( (predicate_count & 0xffff) == 0 )
        sort_progress_probe();
    smart_ptr<ListableGame> &g1 = *(base_mc+e1.idx);        
    smart_ptr<ListableGame> &g2 = *(base_mc+e2.idx);        
    bool lt=false; bool eq=true;  // Note that both of these cannot, ever, both be true
    for( int i=0; eq && i<NBR_COLUMNS; i++ )  // tie break loop
    {
        int col = sort_order[i];
        if( col == -1 )
            //break;  // No more tie breaks, eq must be true, so return lt which will be false.
            return( g1->game_id < g2->game_id );  // Use game_id as ultimate tie-breaker
        bool forward = sort_forward[i];
        bool use_bin=false;
        bool use_rev_bin=false;
        int bin1=0;
        int bin2=0;
        const char *parm1=NULL;
        const char *parm2=NULL;
        switch( col )
        {
            case 1:
            {
                parm1 = g1->White();
                parm2 = g2->White();
                break;
            }
            case 2:
            {
                use_rev_bin = true;
                bin1 = g1->WhiteEloBin();
                bin2 = g2->WhiteEloBin();
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
                use_rev_bin = true;
                bin1 = g1->BlackEloBin();
                bin2 = g2->BlackEloBin();
                break;
            }
            case 5:
            {
                use_rev_bin = true;
                bin1 = g1->DateBin();
                bin2 = g2->DateBin();
                break;
            }
            case 6:
            {
                parm1 = objs.repository->nv.m_event_not_site ? g1->Event() : g1->Site();
                parm2 = objs.repository->nv.m_event_not_site ? g2->Event() : g2->Site();
                break;
            }
            case 7:
            {
                use_bin = true;
                bin1 = g1->RoundBin();
                bin2 = g2->RoundBin();
                break;
            }
            case 8:
            {
                static int xform[] = {3,0,1,2};     // transform order to 1-0, 0-1, 1/2-1/2, *
                use_bin = true;
                bin1 = xform[g1->ResultBin()];
                bin2 = xform[g2->ResultBin()];
                break;
            }
            case 9:
            {
                use_bin = true;
                bin1 = g1->EcoBin();
                bin2 = g2->EcoBin();
                break;
            }
            case 10: // Ply
            {
                use_bin = true;
                bin1 = strlen(g1->CompressedMoves());
                bin2 = strlen(g2->CompressedMoves());
                break;
            }
            case 11: // moves, stage 1
            {
                if( !predicate_transpo_activated )
                {
                    parm1 = e1.blob;
                    parm2 = e2.blob;
                }
                else
                {
                    if( e1.transpo == e2.transpo )
                    {
                        parm1 = e1.blob;
                        parm2 = e2.blob;
                    }
                    else
                    {
                        use_bin = true;
                        bin1 = e1.transpo;
                        bin2 = e2.transpo;
                    }
                }
                break;
            }
        }
        if( use_bin )
        {
            lt = forward ? (bin1 < bin2) : (bin2 < bin1);
            eq = (bin1 == bin2);
        }
        else if( use_rev_bin )  // use this if we want big numbers first when you first
        {                       //  click on the column, eg elo
            lt = forward ? (bin2 < bin1) : (bin1 < bin2);
            eq = (bin1 == bin2);
        }
        else
        {
            int negative_if_parm1_lt_parm2 = strcmp(parm1,parm2);
            lt = forward ? (negative_if_parm1_lt_parm2 < 0) : (negative_if_parm1_lt_parm2 > 0);
            eq = (negative_if_parm1_lt_parm2 == 0);
        }
    }
    return lt;
}

static bool moves_column_forward;
static bool compare_counts( const MoveColCompareElement &e1, const MoveColCompareElement &e2 )
{
    bool lt = false;    // lt = less than conventionally, and since the default sort order is smaller first,
                        //   it can be read as "true if e1 comes first"
    if( true ) //!predicate_transpo_activated )
    {
        if( e1.count == e2.count )
            lt = (e1.tie_break < e2.tie_break);
        else
            lt = moves_column_forward ? (e2.count < e1.count) : (e1.count < e2.count);
    }
    else
    {
        if( e1.transpo == e2.transpo )
        {
            if( e1.count == e2.count )
                lt = (e1.tie_break < e2.tie_break);
            else
                lt = moves_column_forward ? (e2.count < e1.count) : (e1.count < e2.count);
        }
        else
        {
            lt = moves_column_forward ? (e1.transpo < e2.transpo) : (e2.transpo < e1.transpo);
        }
    }
    return lt;
}

// Overridable - base classes may calculate transpo
int GamesDialog::CalculateTranspo( const char *UNUSED(blob), int &transpo )
{        
    transpo = 0;
    return 0;
}

#if 0
static void DebugRange( const char *desc, std::vector< MoveColCompareElement >::iterator begin, std::vector< MoveColCompareElement >::iterator end )
{
    cprintf( "%s\n", desc );
    for( std::vector< MoveColCompareElement >::iterator it=begin; it!=end; it++ )
    {
        std::vector< smart_ptr<ListableGame> >::iterator g = base_mc + it->idx;
        cprintf( "count=%d tie_break=%d T%c %s-%s, [%02x %02x %02x]\n", it->count, it->tie_break, '0'+it->transpo, (*g)->White(), (*g)->Black(),
            (unsigned char) it->blob[0], (unsigned char) it->blob[1], (unsigned char) it->blob[2] );
    }
}
#endif

// #define RECURSE
static int do_column_count;
static bool do_column( std::vector< MoveColCompareElement >::iterator begin, std::vector< MoveColCompareElement >::iterator end )
{
    bool done=true;
    std::vector< MoveColCompareElement >::iterator rover=begin;
    int max_clump_count = 2; 
    if( do_column_count<5 || (do_column_count<50 && do_column_count%10==0) ||  do_column_count%100==0 )   
        cprintf( "%d> do_column() in\n", do_column_count );
    ++do_column_count;

    // Loop through column
    while( rover < end )
    {

        // Identify multiple possible ranges
        //  range = series of elements with same count 
        if( rover->count < 2 )
        {
            rover++;
        }
        else
        {
            std::vector< MoveColCompareElement >::iterator range = rover;
            done = false;  // still work to do
            if( range->count > static_cast<unsigned int>(end-range) )
                range->count = end-range;   // shouldn't happen
            rover += range->count;

            // Have range begin=range, end=rover
            //  Within range identify multiple possible clumps
            //  clump = series of elements with same move
            std::vector< MoveColCompareElement >::iterator range_begin = range;
            while( range != rover )  // across whole range
            {

                std::vector< MoveColCompareElement >::iterator clump = range;
                char prev = '\0';
                int clump_count=0;
                while( range != rover )  
                {
                    char c = *range->blob;
                    if( c == '\0' )
                    {
                        range++;
                        break;  // range->blob never advances once we reach '\0' = end of string
                    }
                    if( prev == '\0' )
                        prev = c;
                    else if( c != prev )
                        break;   // don't advance range or range blob, start of new clump identified
                    clump_count++;
                    range->blob++;
                    range++;
                }

                // Clump identified - every element in clump gets count = nbr of elements in clump
                while( clump != range )
                {
                    clump->count = clump_count;
                    clump++;
                }
                if( clump_count > max_clump_count )
                    max_clump_count = clump_count;    
            }

            // Sort the whole range
            //DebugRange( "Range in", range_begin, rover );
            std::sort( range_begin, rover, compare_counts );
            //DebugRange( "Range out", range_begin, rover );

            #ifdef  RECURSE
            do_column( range_begin, rover );
            #endif
        }
    }
    if( max_clump_count == 2 )
        done = true;
    return done;
}    


void GamesDialog::MoveColCompare( std::vector< smart_ptr<ListableGame> > &displayed_games  )
{
    size_t sz = displayed_games.size();
    if( sz < 2 )
        return;     // no sorting possible

    // Second phase is required to sort on move frequency - if moves col
    //  is primary sort column apply second phase across whole array
    //  otherwise apply it in fragments
    bool whole=false;   // set true if whole array
    int fragments=0;    // set non-zero if multiple fragments
    for( int i=0; i<NBR_COLUMNS; i++ )
    {
        int col = sort_order[i];
        if( col == -1 )
            break;
        if( col == 11 )
        {
            moves_column_forward = sort_forward[i];
            if( i==0 )
                whole = true;
            else
                fragments = i;
            break;
        }
    }

    // If transpo activated, use transpo part of Moves column as part of fragment identification
    bool primary=whole; // set true if moves column is primary sort column
                        //  (not quite same as whole because of transpo)
    if( transpo_activated )
    {
        whole = false;
        fragments++;
    }    

    // Do the whole sort using an intermediate representation
    AutoTimer at("MoveColCompare()");
    DebugPrintfTime dpt;
    cprintf( "Copy to intermediate representation in\n" );
    std::vector< MoveColCompareElement> inter;     // intermediate representation
    //std::vector< MoveColCompareElement *> inter_ptr;            // sort ptrs, it's faster
    
    // Copy to the intermediate representation
    int idx=0;
    base_mc = displayed_games.begin();
    for( std::vector< smart_ptr<ListableGame> >::iterator it=displayed_games.begin(); it!=displayed_games.end(); idx++, it++ )
    {
        MoveColCompareElement e;
        e.idx = idx;
        e.tie_break = 0;
        e.count = 0;
        const char *blob = (*it)->CompressedMoves();
        e.blob = blob + CalculateTranspo( blob, e.transpo );
        inter.push_back( e );
    }
    //for( int i=0; i<sz; i++ )
    //    inter.push_back( &inter_values[i] );
    cprintf( "Copy to intermediate representation out\n" );
    //DebugRange( "Show Intermediate representation", inter.begin(), inter.end() );

    // Step 1, do a conventional string sort on the moves of the master column
    {
        ProgressBar pb(primary?"Column Sort: Moves column requires two stages":"Column Sort: Moves column included in sort", "Requires two stages, stage 1", false );
        sort_before_mc( inter.begin(),
                        inter.end(),
                        &pb 
                );
        cprintf( "Conventional sort in\n" );
        std::sort( inter.begin(), inter.end(), master_predicate_mc );
        cprintf( "Conventional sort out\n" );
        sort_after();
    }

    // Establish tie break after conventional sort
    idx = 0;
    for( std::vector< MoveColCompareElement >::iterator it=inter.begin(); it!=inter.end(); idx++, it++ )
    {
        it->tie_break = idx;
    }
    //DebugRange( "Tie break added", inter.begin(), inter.end() );

    // If whole, do second phase over entire array
    if( whole )
    {
        std::vector< MoveColCompareElement>::iterator it=inter.begin();
        it->count = sz;     // one range of size sz
    }

    // If fragments, find multiple fragments and do second phase over each one
    else if( fragments )
    {
        bool event_not_site=objs.repository->nv.m_event_not_site;
        bool in_fragment=false;
        std::vector< MoveColCompareElement>::iterator start=inter.begin();
        for( std::vector< MoveColCompareElement>::iterator it=inter.begin(); (it+1)!=inter.end(); it++ )
        {
            it->count = 0;  // by default no fragment
            ListableGame &g1 = *displayed_games[it->idx];
            ListableGame &g2 = *displayed_games[(it+1)->idx];
            bool same = true;
            for( int j=0; same && j<fragments; j++ )
            {
                int col = sort_order[j];
                switch( col )
                {
                    case 0:  same = (g1.game_id == g2.game_id);                break;
                    case 1:  same = (0 == strcmp(g1.White(),g2.White()));      break;
                    case 2:  same = (g1.WhiteEloBin() == g2.WhiteEloBin());    break; 
                    case 3:  same = (0 == strcmp(g1.Black(),g2.Black()));      break;
                    case 4:  same = (g1.BlackEloBin() == g2.BlackEloBin());    break;
                    case 5:  same = (g1.DateBin() == g2.DateBin());            break;
                    case 6:  same = (0 == event_not_site ? (0==strcmp(g1.Event(),g2.Event())) : (0==strcmp(g1.Site(),g2.Site())) );  break;
                    case 7:  same = (g1.RoundBin() == g2.RoundBin());          break;
                    case 8:  same = (g1.ResultBin() == g2.ResultBin());        break;
                    case 9:  same = (g1.EcoBin() == g2.EcoBin());              break;
                    case 10: same = (strlen(g1.CompressedMoves()) == strlen(g2.CompressedMoves()) ); break;
                    case 11: same = (it->transpo == (it+1)->transpo);          break;
                }
            }
            bool last = ((it+2) == inter.end());
            if( last )
                (it+1)->count = 0;
            if( in_fragment && !same )   // eg "abcddddefg"
            {                            //           ^ it points to last d
                in_fragment = false;
                int count = (it-start)+1;
                start->count = count;
            }
            else if( in_fragment && last )  // eg "abcddddd"
            {                               //           ^ it points to second to last d
                int count = (it-start)+2;
                start->count = count;
            }
            else if( !in_fragment && same )  // eg "abcdeeee"
            {                                //         ^ it points to first e
                in_fragment = true;
                start = it;
                if( last )
                    start->count = 2;
            }
        }
    }

    // Step 3 sort each column in turn using counts
    cprintf( "Subsort on each column of moves in\n" );
    do_column_count=0;
    #ifdef  RECURSE
    do_column( inter.begin(), inter.end() );
    #else
    {
        ProgressBar pb(primary?"Column Sort: Moves column requires two stages":"Column Sort: Moves column included in sort", "Requires two stages, stage 2", false);//!primary );
        bool done = false;
        int permill = 0;
        for(;;) //int col=0; col<120; col++ )    // after 120 columns (i.e. plys) we surely are only tiebreaking very unlikely duplicates 
        {
            //DebugRange( "Before do_column()", inter.begin(), inter.end() );
            done = do_column( inter.begin(), inter.end() );
            if( done )
                break;
            int delta = 50;
            if( permill < 400 )
                delta = 50;
            else if( permill < 650 )
                delta = 40;
            else if( permill < 800 )
                delta = 30;
            else if( permill < 880 )
                delta = 20;
            else if( permill < 940 )
                delta = 10;
            else if( permill < 970 )
                delta = 5;
            else if( permill < 990 )
                delta = 2;
            else if( permill < 999 )
                delta = 1;
            else if( permill >= 1000 )
                delta = 0;
            /*bool abort = */ pb.Permill( permill += delta );
            /* The following feature was not very useful/obvious in practice
            if( abort )
            {
                int answer = wxMessageBox("Temporarily remove moves column from sort stack?","Moves column sort cancelled", wxYES_NO);
                if( answer == wxYES )
                {
                    bool triggered = false;
                    for( int i=0; i<NBR_COLUMNS-1; i++ )
                    {
                        if( !triggered )
                        {
                            int col = sort_order[i];
                            if( col == 11 )
                                triggered = true;
                        }
                        if( triggered )
                        {
                            sort_order[i] = sort_order[i+1];
                            sort_forward[i] = sort_forward[i+1];
                        }
                    }
                }
                break;
            } */
        }
    }
    #endif
    cprintf( "Subsort on each column of moves out\n" );
    
    // Step 4 build sorted version of games list
    cprintf( "Rebuild displayed games vector in\n" );
    std::vector< smart_ptr<ListableGame> > temp;
    for( size_t i=0; i<sz; i++ )
    {
        MoveColCompareElement &e = inter[i];
        temp.push_back( displayed_games[e.idx] );
    }
    cprintf( "Rebuild displayed games vector out\n" );
    
    // Step 5 replace original games list
    cprintf( "Replace displayed games vector in\n" );
    displayed_games = temp;
    cprintf( "Replace displayed games vector out\n" );
}

// This is basically how move column sorting works now
// ---------------------------------------------------
// while work to do
//    loop through next column (in this context column === ply)
//        for each range in column
//              count move groups
//              sort

// The following explains the ideas involved a little more
// -------------------------------------------------------

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
     G)  1.d4 d5 2.c4 c5 3.e3

     // At this point we establish the tie-break order - basically A->B->C->D->E->F->G

     // col 1 range is whole array
     // col 1 A) count the moves
     // Calculate the counts in column 1 like this
         col=1 col=2 col=3 col=4 col=5
     A)  7 d4
     B)  7
     C)  7
     D)  7    col=1 count 7 '1.d4's at offset 0 into each game
     E)  7
     F)  7
     G)  7
     // col 1 B) sort based on these counts (unchanged due to the tie-break order)
    
     // col 2 range is whole array (because col 1 count is uniform)
     // col 2 A) count the moves
         col=1 col=2 col=3 col=4 col=5
     A)  7     3 Nf6
     B)  7     3
     C)  7     3
     D)  7     4 d5    col=2 count 3 '1...Nf6's and 4 '1...d5's
     E)  7     4
     F)  7     4
     G)  7     4

     // col 2 B) sort based on these counts - the tie-break system again avoids disruptive
     //  chaos (D,E,F,G move ahead of A,B,C with no internal reordering) and gives us this
         col=1 col=2 col=3 col=4 col=5
     D)  7     4
     E)  7     4
     F)  7     4
     G)  7     4
     A)  7     3
     B)  7     3
     C)  7     3
     
     // col 3 two ranges (because two groups of col 2 counts)
     // col 3 range 1 A) count the moves
     // col 3 range 1 B) sort based on these counts
         col=1 col=2 col=3 col=4 col=5
     D)  7     4     4 c4
     E)  7     4     4    col 3 count 4 2.c4 s
     F)  7     4     4     then sort (sort doesn't change order as 4s all rank equally)
     G)  7     4     4
     A)  7     3     
     B)  7     3     
     C)  7     3     

     // col 3 range 2 A) count the moves
     // col 3 range 2 B) sort based on these counts
              col=1 col=2 col=3 col=4 col=5
     D)  7     4     
     E)  7     4     
     F)  7     4     
     G)  7     4     
     A)  7     3     3 c4
     B)  7     3     3    col 3 count 3 2.c4 s
     C)  7     3     3     then sort (sort doesn't change order as 3s all rank equally)
     
     // col 4 two ranges (because two groups of col 3 counts)
     // col 4 range 1 A) count the moves
     // col 4 range 1 B) sort based on these counts (no changes as 3 e6s were ahead of 1 c5 anyway)
         col=1 col=2 col=3 col=4 col=5
     D)  7     4     4     3 e6
     E)  7     4     4     3
     F)  7     4     4     3
     G)  7     4     4     1 c5
     A)  7     3     3     
     B)  7     3     3     
     C)  7     3     3     

     // col 4 range 2 A) count the moves
     // col 4 range 2 B) sort based on these counts (no changes)
         col=1 col=2 col=3 col=4 col=5
     D)  7     4     4     
     E)  7     4     4     
     F)  7     4     4     
     G)  7     4     4     
     A)  7     3     3     3 e6
     B)  7     3     3     3
     C)  7     3     3     3

     // col 5 three ranges (because three groups of col 4 counts)
     // col 5 range 1 A) count the moves
         col=1 col=2 col=3 col=4 col=5
     D)  7     4     4     3     1 Nc3
     E)  7     4     4     3     2 Nf3
     F)  7     4     4     3     2
     G)  7     4     4     1     
     A)  7     3     3     3     
     B)  7     3     3     3     
     C)  7     3     3     3     

     // col 5 range 1 B) sort based on these counts (E and F move ahead of D)
         col=1 col=2 col=3 col=4 col=5
     E)  7     4     4     3     2 Nf3
     F)  7     4     4     3     2
     D)  7     4     4     3     1 Nc3
     G)  7     4     4     1     
     A)  7     3     3     3     
     B)  7     3     3     3     
     C)  7     3     3     3     

     // col 5 range 2 A) count the moves
     // col 5 range 2 B) sort based on these counts (no change)
         col=1 col=2 col=3 col=4 col=5
     E)  7     4     4     3      
     F)  7     4     4     3      
     D)  7     4     4     3      
     G)  7     4     4     1     1 e3
     A)  7     3     3     3      
     B)  7     3     3     3      
     C)  7     3     3     3

     // col 5 range 3 A) count the moves
         col=1 col=2 col=3 col=4 col=5
     E)  7     4     4     3      
     F)  7     4     4     3      
     D)  7     4     4     3      
     G)  7     4     4     1      
     A)  7     3     3     3     1 Nc3 
     B)  7     3     3     3     2 Nf3
     C)  7     3     3     3     2

     // col 5 range 3 B) sort (B and C move ahead of A)
         col=1 col=2 col=3 col=4 col=5
     E)  7     4     4     3      
     F)  7     4     4     3      
     D)  7     4     4     3      
     G)  7     4     4     1      
     B)  7     3     3     3     2 Nf3
     C)  7     3     3     3     2
     A)  7     3     3     3     1 Nc3 
     
     // So final ordering is according to line popularity
     E)  1.d4 d5 2.c4 e6 3.Nf3
     F)  1.d4 d5 2.c4 e6 3.Nf3
     D)  1.d4 d5 2.c4 e6 3.Nc3
     G)  1.d4 d5 2.c4 c5 3.e3
     B)  1.d4 Nf6 2.c4 e6 3.Nf3
     C)  1.d4 Nf6 2.c4 e6 3.Nf3
     A)  1.d4 Nf6 2.c4 e6 3.Nc3

     There are more ranges in each column, as each range either remains the same size
     or is divided up as we move right. So we keep counting moves and sorting in smaller
     ranges. All line swaps take place within these smaller ranges (so the changes become
     less dramatic as the algorithm converges).

     Note that even if (by some fluke) all moves at column/ply 40 (say) were identical,
     this couldn't reverse this trend, the ranges and counts would not increase relative
     to column 39, (they would actually remain the same).

*/    

void GamesDialog::ColumnSort( int compare_col_, std::vector< smart_ptr<ListableGame> > &displayed_games )
{
	dirty = true;
    uint32_t sz = displayed_games.size();
    uint32_t game_id_restore = GAME_ID_SENTINEL;    // no game should match this
    if( sz > 1 )
    {
        if( 0<=track->focus_idx && track->focus_idx<static_cast<int>(sz) )
            game_id_restore = displayed_games[ track->focus_idx]->game_id;
        backdoor = this;

        // If we sort on the same column as last time....    
        if( compare_col_ == col_last_time )
        {

            // Simply reverse last order
            sort_forward[0] = !sort_forward[0];
        }
        else
        {

            // Otherwise push the  previous sort critera down
            for( int i=NBR_COLUMNS-1; i>=1; i-- )
            {
                sort_order[i]   = sort_order[i-1];
                sort_forward[i] = sort_forward[i-1];
            }

            // Sort the selected column, forward first 
            sort_order[0]   = compare_col_;
            sort_forward[0] = true;

            // The first time, mark the next spot in the stack as the end
            if( sort_order_first )
            {
                cprintf( "sort_order_first\n" );
                sort_order_first = false;
                sort_order[1] = -1;
            }

            // Not the first time, look for a repeat instance of the current
            //  column and stop there. Note that every column can occur once
            //  in the stack (that's the worst case) in which case we will
            //  stop at the end of the stack, not at the magic -1 marker
            else
            {
                for( int i=1; i<NBR_COLUMNS; i++ )
                {
                    if( sort_order[i] == compare_col_ )
                    {
                        sort_order[i] = -1; // mark end
                        break;
                    }
                }
            }
        }

        // Special handling for column 0
        //  If already completely forward sorted, we definitely want to reverse sort (for example we start with a column 0 sort
        //   on an otherwise undisturbed database dialog which will start forward sorted)
        //  Similarly if already completely reverse sorted, we definitely want to forward sort
        if( compare_col_ == 0 )
        {
            bool already_forward=true;
            bool already_reverse=false;
            uint32_t prev=0;
            for( uint32_t i=0; already_forward && i<displayed_games.size(); i++ )
            {
                uint32_t gid = displayed_games[i]->game_id;
                if( gid < prev )
                    already_forward = false;
                prev = gid;
            }
            if( !already_forward )
            {
                already_reverse=true;
                prev=0xffffffff;
                for( uint32_t i=0; already_reverse && i<displayed_games.size(); i++ )
                {
                    uint32_t gid = displayed_games[i]->game_id;
                    if( gid > prev )
                        already_reverse = false;
                    prev = gid;
                }
            }
            if( already_forward )
            {
                cprintf( "Already forward sorted, so reversing\n" );
                sort_forward[0] = false;
            }
            else if( already_reverse )
            {
                cprintf( "Already reverse sorted, so forwarding\n" );
                sort_forward[0] = true;
            }
            // else don't meddle
        }
        
        // We need a more elaborate algorithm if the moves column is part of the sort
        bool use_move_col_algorithm = false;
        for( int i=0; i<NBR_COLUMNS; i++ )
        {
            if( sort_order[i] == -1 )
                break;
            if( sort_order[i] == 11 )
                use_move_col_algorithm = true;
        }

        // Complicated version if move column involved
        predicate_transpo_activated = transpo_activated;
        if( use_move_col_algorithm )
        {
            MoveColCompare( displayed_games );
        }

        // Simple version if move column not involved
        else
        {
            //AutoTimer at("Move column not included");
            //DebugPrintfTime dpt;
            ProgressBar pb("Column sort" , "Sorting...", false );
            predicate_displayed_games = &displayed_games;
            sort_before( displayed_games.begin(),
                   displayed_games.end(),
                   master_predicate,
                   &pb 
                 );
            std::sort( displayed_games.begin(), displayed_games.end(), master_predicate );
            sort_after();
        }
        nbr_games_in_list_ctrl = displayed_games.size();
        list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
        list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
        int top = list_ctrl->GetTopItem();
        int count = 1 + list_ctrl->GetCountPerPage();
        if( count > nbr_games_in_list_ctrl )
            count = nbr_games_in_list_ctrl;
        for( int i=0; i<count; i++ )
            list_ctrl->RefreshItem(top++);

        // Put cursor on same game as before sort
        uint32_t cursor=0;
        for( uint32_t i=0; i<sz; i++ )
        {
            if( displayed_games[i]->game_id == game_id_restore )
            {
                cursor = i;
                break;
            }
        }
        Goto(cursor);
        col_last_time = compare_col_;
    }
}

void GamesDialog::GdvListColClick( int compare_col_ )
{
    ColumnSort( compare_col_, gc->gds );
}


