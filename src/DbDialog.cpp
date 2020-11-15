/****************************************************************************
 * Database data source for games dialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/listctrl.h"
#include "wx/notebook.h"
#include "Portability.h"
#include "Appdefs.h"
#include "DebugPrintf.h"
#include "thc.h"
#include "AutoTimer.h"
#include "GameDetailsDialog.h"
#include "GamePrefixDialog.h"
#include "GameLogic.h"
#include "Objects.h"
#include "Lang.h"
#include "PgnFiles.h"
#include "CtrlChessBoard.h"
#include "DbDialog.h"
#include "Database.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <list>    
#include <algorithm>

// DbDialog constructors
DbDialog::DbDialog
(
    wxWindow    *parent,
    thc::ChessRules *cr,
    GamesCache  *gc,
    GamesCache  *gc_clipboard,
    DB_REQ      db_req,
    PatternParameters *parm,
    wxWindowID  id,
    const wxPoint& pos,
    const wxSize& size
 ) : GamesDialog( parent, cr, gc, gc_clipboard, id, pos, size )
{
    this->db_req = db_req;
    if( db_req==REQ_PATTERN  && parm )
        this->pm.parm = *parm;
    activated_at_least_once = false;
    transpo_activated = false;
    white_player_search = true;
}

void DbDialog::GdvEnumerateGames()
{
   // int nbr_games = gc_db_displayed_games.gds.size();
   // for( int i=0; i<nbr_games; i++ )
   //     gc_db_displayed_games.gds[i]->game_id = i;
}

// Games Dialog Override - Add extra controls
wxSizer *DbDialog::GdvAddExtraControls( bool WXUNUSED(big_display) )
{
    // Stats list box
    //    wxSize sz4 = sz;
    //    sz.x /= 4;
    //    sz.y /= 3;
    wxSize sz4 = mini_board->GetSize();
    wxSize sz5 = sz4;
    cprintf( "mini_board size x=%d, y=%d\n",sz4.x, sz4.y );
    sz5.x = (sz4.x*5)/100;
    sz5.y = (sz4.y*10)/10;
    notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, sz5 );
    gdr.RegisterPanelWindow( notebook, true );

    //wxPanel *notebook_page1 = new wxPanel(notebook, wxID_ANY );
    hsiz_panel /*vsiz_panel_stats*/->Add( notebook, 0, wxALIGN_TOP|/*wxGROW|*/wxALL, 0 );

    // Text control for white entry
    sz5 = wxDefaultSize;
    sz5.x = (sz4.x*55)/100;
    //sz5.y = (sz4.y*2)/10;

    text_ctrl = 0;
    if( db_req == REQ_PLAYERS )
    {
        //text_ctrl->SetSize( sz3.x*2, sz3.y );      // temp temp
        text_ctrl = new wxTextCtrl ( this, ID_DB_TEXT, wxT(""), wxDefaultPosition, sz5, 0 );
        gdr.RegisterPanelWindow( text_ctrl );
        vsiz_panel_buttons->Add(text_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        wxSize sz2=text_ctrl->GetSize();
        //text_ctrl->SetSize((sz2.x*10)/32,sz2.y);      // temp temp
        //text_ctrl->SetSize((sz.x*7)/2,sz2.y);      // temp temp
        text_ctrl->SetValue("White Player");

        wxButton* search = new wxButton ( this, ID_DB_SEARCH, wxT("Search"),
                                         wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( search );
        vsiz_panel_buttons->Add(search, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        //wxSize sz3=reload->GetSize();
        //text_ctrl->SetSize( sz3.x*2, sz3.y );      // temp temp
    }
 
    
//    wxStaticText* spacer1 = new wxStaticText( this, wxID_ANY, wxT(""),
//                                     wxDefaultPosition, wxDefaultSize, 0 );
//    vsiz_panel_buttons->Add(spacer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

//    white_player_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX2,
//                                 wxT("&White player"), wxDefaultPosition, wxDefaultSize, 0 );
//    vsiz_panel_buttons->Add(white_player_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
//    white_player_ctrl->SetValue( true );

    wxButton* btn1 = new wxButton ( this, ID_BUTTON_1, wxT("Clear Clipboard"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    gdr.RegisterPanelWindow( btn1 );
    vsiz_panel_buttons->Add(btn1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* btn2 = new wxButton ( this, ID_BUTTON_2, wxT("Add to Clipboard"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    gdr.RegisterPanelWindow( btn2 );
    vsiz_panel_buttons->Add(btn2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    wxButton* btn3 = new wxButton ( this, ID_BUTTON_3, wxT("Add All Player's White Games"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    gdr.RegisterPanelWindow( btn3 );
    vsiz_panel_buttons->Add(btn3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    white_player_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX2,
                                   wxT("&White player"), wxDefaultPosition, wxDefaultSize, 0 );
    gdr.RegisterPanelWindow( white_player_ctrl );
    vsiz_panel_buttons->Add(white_player_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    white_player_ctrl->SetValue( true );

    wxButton* btn4 = new wxButton ( this, ID_BUTTON_4, wxT("Add All Player's Black Games"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    gdr.RegisterPanelWindow( btn4 );
    vsiz_panel_buttons->Add(btn4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    if( db_req == REQ_PLAYERS )	// Save all button assumes games are in the gc_db_displayed_games cache. Not true if REQ_PLAYERS
	{
		wxStaticText* spacer1 = new wxStaticText( this, wxID_ANY, wxT(""),
										 wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( spacer1 );
		vsiz_panel_buttons->Add(spacer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	}
	else
	{
		wxButton* save_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save all"),
			wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( save_all_to_a_file );
		vsiz_panel_buttons->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	}
    
    filter_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX, "&Clipboard as temp database", wxDefaultPosition, wxDefaultSize, 0 );
                             //       big_display?"&Clipboard as temporary database":"&Clipboard as temp database", wxDefaultPosition, wxDefaultSize, 0 );
    gdr.RegisterPanelWindow( filter_ctrl );
    vsiz_panel_buttons->Add(filter_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    filter_ctrl->SetValue( objs.gl->db_clipboard );
    wxStaticText* spacer1 = new wxStaticText( this, wxID_ANY, wxT(""),
                                        wxDefaultPosition, wxDefaultSize, 0 );
    gdr.RegisterPanelWindow( spacer1 );
    vsiz_panel_buttons->Add(spacer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    SetDialogHelp();
    return vsiz_panel_buttons;   
}

// Games Dialog Override - Disable features that make no sense without a game (enable them otherwise)
void DbDialog::GdvEnableControlsIfGamesFound( bool have_games )
{                                            
    FindWindow(wxID_OK)    ->Enable(have_games);     // Load Game
    FindWindow(ID_BUTTON_2)->Enable(have_games);     // Add to Clipboard
    FindWindow(ID_BUTTON_3)->Enable(have_games);     // Add All Player's White Games
    FindWindow(ID_BUTTON_4)->Enable(have_games);     // Add All Player's Black Games
    FindWindow(ID_BUTTON_5)->Enable(have_games);     // Use game
}

void DbDialog::SetDialogHelp()
{
    if( db_req == REQ_PLAYERS )
    {
        wxString white_player_help = "Enter a complete or partial player name here, then search for it.";
        wxString search_help = "Enter a complete or partial player name to the left, then search for it. If the player name is partial, multiple presses find all matching players.";
        FindWindow(ID_DB_TEXT)->SetHelpText(white_player_help);
        FindWindow(ID_DB_TEXT)->SetToolTip(white_player_help);
        FindWindow(ID_DB_SEARCH)->SetHelpText(search_help);
        FindWindow(ID_DB_SEARCH)->SetToolTip(search_help);
    }
}

// Games Dialog Override - One time activation
void DbDialog::GdvOnActivate()
{
    if( !activated_at_least_once )
    {
        activated_at_least_once = true;

        // Remove stats panel when that feature is unavailable
        if( db_req==REQ_PLAYERS || db_req==REQ_PATTERN )
        {
            notebook->Hide();
            if( db_req == REQ_PATTERN )
            {
                PatternSearch();
            }
        }

        // Else calculate stats
        else
        {
            StatsCalculate();                                 
        }
        Goto(0); // list_ctrl->SetFocus();
    }
}

// Games Dialog Override - Read a game into list control
void DbDialog::GdvReadItem( int item, CompactGame &pact )
{
    // Two mechanisms supported

    // Mechanism 1) Dialog presents selected games
    //  This mechanism kicks in after position/pattern/material balance searches
    bool in_memory = ReadGameFromSearchResults( item, pact );
    if( !in_memory )
    {

        // Mechanism 2) Fallback when no filtering - dialog presents all database games
        //  This mechanism kicks in after REQ_PLAYERS
        objs.db->GetRow( item, &pact );
        pact.transpo_nbr = 0;
    }
}

// Read game from search results - such games are stored in the displayed
//  games cache
bool DbDialog::ReadGameFromSearchResults( int item, CompactGame &pact )
{
    bool in_memory = false;
    pact.transpo_nbr = 0;
    int nbr_games = gc_db_displayed_games.gds.size();
    if( 0<=item && item<nbr_games )
    {
        in_memory = true;
        gc_db_displayed_games.gds[item]->GetCompactGame(pact);
        if( transpo_activated && transpositions.size() > 1 )
        {
            for( unsigned int j=0; j<transpositions.size(); j++ )
            {
                std::string &this_one = transpositions[j].blob;
                const char *p = this_one.c_str();
                size_t len = this_one.length();
                std::string str_blob( gc_db_displayed_games.gds[item]->CompressedMoves() );
                if( str_blob.length()>=len && 0 == memcmp(p,str_blob.c_str(),len) )
                {
                    pact.transpo_nbr = j+1;
                    break;
                }
            }
        }
    }
    return in_memory;
}

// Overidden function used to set initial position when navigating game
int DbDialog::GetBasePositionIdx( CompactGame &pact, bool receiving_focus )
{ 
    int idx = 0;
    if( pm.IsReady() && db_req == REQ_PATTERN )
    {
        if( receiving_focus )
        {
            CompressMoves press;
            std::string moves = press.Compress( pact.moves );
            unsigned short offset1;
            unsigned short offset2;
            bool reverse;
            bool found = objs.db->tiny_db.PatternSearchGameSlowPromotionAllowed( pm, reverse, moves, offset1, offset2 );
            if( found )
                idx = offset1;
        }
    }
    else
    {
        pact.FindPositionInGame( objs.db->GetPositionHash(), idx );
    }
    return idx;
}

// Overidden function used for smart move column compare
int DbDialog::CalculateTranspo( const char *blob, int &transpo )
{        
    transpo = 0;
    unsigned int sz2 = transpositions.size();
    for( unsigned int j=0; j<sz2; j++ )
    {
        PATH_TO_POSITION *ptp = &transpositions[j];
        unsigned int offset = ptp->blob.length();
        if( 0 == memcmp( ptp->blob.c_str(), blob, offset ) )
        {
            transpo = j+1;
            return offset;
        }
    }
    return 0;
}

// Games Dialog Override - List column clicked
void DbDialog::GdvListColClick( int compare_col_ )
{
    if( db_req == REQ_PLAYERS )
        return; // not supported
    ColumnSort( compare_col_, gc_db_displayed_games.gds );  
}

// Games Dialog Override - Search feature
void DbDialog::GdvSearch()
{
    if( text_ctrl )
    {
        wxString name = text_ctrl->GetValue();
        std::string sname(name.c_str());
    
        // Do a "find on page type feature"
        if( sname.length()>0 && db_req == REQ_PLAYERS )
        {
            std::string current = white_player_search ? track->info.r.white : track->info.r.black;
            int row = objs.db->FindPlayer( sname, current, track->focus_idx, white_player_search );
            cprintf( "row=%d\n", row );
            Goto(row);
        }
    }
}


// Games Dialog Override - Save all to a .pgn file
void DbDialog::GdvSaveAllToAFile()
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
        gc_db_displayed_games.FileSaveAllAsAFile( filename );
    }
}

// Games Dialog Override - Cancel
void DbDialog::GdvOnCancel()
{
}

// Games Dialog Override - Help
void DbDialog::GdvHelpClick()
{
    wxString helpText =
    db_req == REQ_PLAYERS ?
    "\nUse this panel to navigate the current database. Use Load Game to "
    "load a game into a new tab and Use Game to add the game as a variation "
    "in the current tab."
    "\n\n"
    "You can add games to the clipboard and set the clipboard as the "
    "temporary database so that only games from the clipboard appear "
    "and the drill down stats (available in other database views) apply to those games only."
    "\n\n"
    "One way to use this clipboard-as-temporary-database feature for preparation is "
    "to copy games from one player to the clipboard. To make this easy, buttons "
    "are provided to add a player's games to the clipboard."
    "\n\n"
    :
    "\nUse this panel to navigate the current database. Use Load Game to "
    "load a game into a new tab and Use Game to add the game as a variation "
    "in the current tab."
    "\n\n"
    "You can drill down by clicking on moves in the Next Move box."
    "\n\n"
    "You can add games to the clipboard and set the clipboard as the "
    "temporary database so that only games from the clipboard appear "
    "and the drill down stats apply to those games only."
    "\n\n"
    "One way to use this clipboard-as-temporary-database feature for preparation is "
    "to copy games from one player to the clipboard. To make this easy, buttons "
    "are provided to add a player's games to the clipboard. Use the Show all ordered "
    " by player menu to search for players."
    "\n\n"
    "You can sort on any column including the Moves column. The Moves column sort is statistical (most popular lines first) "
	"rather than alphabetical. Sort history is respected in tie breaks. For example, if you "
	"sort on Moves, then on Black, then on White; The games will be sorted by White "
	"player, but all opponents will be grouped together (because of the earlier sort on Black), "
	"and if there are multiple games between the same players, the games will be sorted according "
	"to the most common opening sequences between those two players (because of the earlier sort on Moves). "
	"The first column (column '#') is provided only to allow "
    "a sort on initial order (for restoring or reversing the initial order). To reverse "
	"sort click twice." 
    "\n\n";
    wxMessageBox(helpText,
    wxT("Database Dialog Help"),
    wxOK|wxICON_INFORMATION, NULL );
}

// ID_BUTTON_1 is Clear Clipboard
void DbDialog::GdvButton1()
{
    objs.gl->gc_clipboard.gds.clear();
    Goto( track->focus_idx );
}

// ID_BUTTON_2 is Add to Clipboard
void DbDialog::GdvButton2()
{
    CopyOrAdd( false );
}

// ID_BUTTON_3 is Add All Player's White Games
void DbDialog::GdvButton3()
{
    std::string player_name = white_player_search ? track->info.r.white : track->info.r.black;
    int nbr_loaded = objs.db->LoadPlayerGamesWithQuery( player_name, true, objs.gl->gc_clipboard.gds );
    if( nbr_loaded > 0 )
    {
        char buf[2000];
        sprintf( buf, "Added %d white games played by \"%s\" to clipboard", nbr_loaded, player_name.c_str() );
        wxMessageBox( buf, "Added player's white games to clipboard", wxOK, this );
    }
    Goto( track->focus_idx );
}

// ID_BUTTON_4 is Add All Player's Black Games
void DbDialog::GdvButton4()
{
    std::string player_name = white_player_search ? track->info.r.white : track->info.r.black;
    int nbr_loaded = objs.db->LoadPlayerGamesWithQuery( player_name, false, objs.gl->gc_clipboard.gds );
    if( nbr_loaded > 0 )
    {
        char buf[2000];
        sprintf( buf, "Added %d black games played by \"%s\" to clipboard", nbr_loaded, player_name.c_str() );
        wxMessageBox( buf, "Added player's black games to clipboard", wxOK, this );
    }
    Goto( track->focus_idx );
}

//  ID_BUTTON_5 is Use Game feature
void DbDialog::GdvButton5()
{
    objs.gl->gd.UseGame( cr_base, moves_from_base_position, track->info );
    TransferDataToWindow();
    AcceptAndClose();
}


// ID_DB_CHECKBOX2 is White player
void DbDialog::GdvCheckBox2( bool checked )
{
    white_player_search = checked;
    if( text_ctrl )
    {
        std::string s(text_ctrl->GetValue());
        if( !white_player_search && s=="White Player" )
        {
            text_ctrl->SetValue("Black Player");
        }
        else if( white_player_search && s=="Black Player" )
        {
            text_ctrl->SetValue("White Player");
        }
        Goto( track->focus_idx );
    }
}

// ID_DB_CHECKBOX is Clipboard as temp database
void DbDialog::GdvCheckBox( bool checked )
{
    objs.gl->db_clipboard = checked;

    // Clear back to the base position
    this->cr = cr_base;
    moves_from_base_position.clear();
    gc_db_displayed_games.gds.clear();

    // Select PatternSearch() or StatsCalculate()
    if( db_req == REQ_PATTERN )
    {
        PatternSearch();
        Goto(0); // list_ctrl->SetFocus();
    }

    // Comments for this block are confusing and outdated - sorry
    else if( db_req != REQ_PLAYERS )
    {
        if( objs.gl->db_clipboard )
        {
            nbr_games_in_list_ctrl = gc_clipboard->gds.size();
        }
        else    // TODO need special handling for initial position
                //  (not really - searching for initial position picks up every game - quick enough to work well)
        {
            nbr_games_in_list_ctrl = objs.db->SetDbPosition( db_req );
        
            // We have shown stats while db_clipboard was true. So to avoid going back to
            //  situation where stats must be requested, unconditionally load games into
            //  memory (Calculate Stats button loads games into memory)
            gc->gds.clear();
        }
        StatsCalculate();
        Goto(0); // list_ctrl->SetFocus();
    }
}

// Copy to clipboard if clear_clipboard is true
// Add to clipboard if clear_clipboard is false
//
//   (note that in DbDialog we only Add actually)
//
void DbDialog::CopyOrAdd( bool clear_clipboard )
{
    int idx_focus = -1;
    int nbr_copied = 0;
    if( list_ctrl )
    {
        CompactGame pact;
        size_t nbr = list_ctrl->GetItemCount();
        cprintf( "CopyOrAdd() Loop begin\n" );
#if 1
        int i = -1;
        i = list_ctrl->GetNextItem(i,
                                wxLIST_NEXT_ALL,
                                wxLIST_STATE_FOCUSED);
        if( i != -1 )
            idx_focus = i;
        i = -1;
        for(;;)
        {
            i = list_ctrl->GetNextItem(i,
                                    wxLIST_NEXT_ALL,
                                    wxLIST_STATE_SELECTED);
            if( i == -1 )
                break;
            else
            {
#else
        for( size_t i=0; i<nbr; i++ )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                idx_focus = i;
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
            {
#endif
                if( clear_clipboard )
                {
                    clear_clipboard = false;
                    gc_clipboard->gds.clear();
                }
                if( nbr == gc_db_displayed_games.gds.size() )
                    gc_clipboard->gds.push_back( gc_db_displayed_games.gds[i] ); // assumes smart_ptr is std::shared_ptr
                else
                {
                    CompactGame pact2;
                    GdvReadItem( i, pact2 );
                    ListableGameDb info( pact2.game_id, pact2 );
                    make_smart_ptr( ListableGameDb, new_info, info );
                    gc_clipboard->gds.push_back( std::move(new_info ) );
                }
                nbr_copied++;
            }
        }
        cprintf( "CopyOrAdd() Loop end\n" );
        if( nbr_copied==0 && idx_focus>=0 )
        {
            if( clear_clipboard )
            {
                clear_clipboard = false;
                gc_clipboard->gds.clear();
            }
            if( nbr == gc_db_displayed_games.gds.size() )
                gc_clipboard->gds.push_back( gc_db_displayed_games.gds[idx_focus] ); // assumes smart_ptr is std::shared_ptr
            else
            {
                CompactGame pact2;
                GdvReadItem( idx_focus, pact2 );
                ListableGameDb info( pact2.game_id, pact2 );
                make_smart_ptr( ListableGameDb, new_info, info );
                gc_clipboard->gds.push_back( std::move(new_info ) );
            }
            nbr_copied++;
        }
        Goto( 0<=idx_focus && static_cast<size_t>(idx_focus)<nbr ? idx_focus : 0 );
    }
    dbg_printf( "%d games copied\n", nbr_copied );
}

// Games Dialog Override - Next move listbox
//  One of the moves in the ID_DB_LISTBOX_STATS listbox is clicked
//  No real need for this to be overridable - it only occurs in this leaf class
void DbDialog::GdvNextMove( int idx )
{
    dirty = true;
    if( idx==0 && moves_from_base_position.size()>0 )
    {
        moves_from_base_position.pop_back();  // Undo last move
    }
    else
    {
        thc::Move this_one = moves_in_this_position[idx];
        moves_from_base_position.push_back(this_one);
    }
    
    thc::ChessRules cr_to_match = this->cr;
    for( size_t i=0; i<moves_from_base_position.size(); i++ )
    {
        thc::Move mv = moves_from_base_position[i];
        cr_to_match.PlayMove(mv);
    }
    title_ctrl->SetLabel( "Searching ...." );
    title_ctrl->Refresh();
    title_ctrl->Update();
    //title_ctrl->MacDoRedraw(0);
#ifdef THC_UNIX
    CallAfter( &DbDialog::StatsCalculate );
#else
    wxSafeYield();
    StatsCalculate();
#endif
}

// Sorting map<std::string,MOV_STATS> by MOVE_STATS instead of std::string requires this flipping procedure
template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_and_sort_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                   flip_pair<A,B>);
    return dst;
}

// Heart and soul of DbDialog() - do the search and calculate the stats
void DbDialog::StatsCalculate()
{
    wxString save_title = GetTitle();
    SetTitle("Searching...");

    int total_white_wins = 0;
    int total_black_wins = 0;
    int total_draws = 0;
    transpositions.clear();
    stats.clear();
    dirty = true;
    GamesCache temp;
    temp.gds.clear();
    cprintf( "Remove focus %d\n", track->focus_idx );
    list_ctrl->SetItemState( track->focus_idx, 0, wxLIST_STATE_FOCUSED );
    list_ctrl->SetItemState( track->focus_idx, 0, wxLIST_STATE_SELECTED );
    thc::ChessRules cr_to_match = this->cr;
    bool add_go_back = false;
    std::string go_back_string;
    for( size_t i=0; i<moves_from_base_position.size(); i++ )
    {
        thc::Move mv = moves_from_base_position[i];
        if( i+1 == moves_from_base_position.size() )
        {
            std::string s = mv.NaturalOut(&cr_to_match);
            LangOut(s);
            if( !cr_to_match.white )
                s = "..." + s;
            go_back_string = "Go back (undo " + s + ")";
            add_go_back = true;
        }
        cr_to_match.PlayMove(mv);
    }       

    // hash to match
    uint64_t gbl_hash = cr_to_match.Hash64Calculate();
    CompressMoves press_to_match(cr_to_match);
    objs.db->SetPositionHash( gbl_hash );
    objs.db->gbl_position = cr_to_match;
    MemoryPositionSearch partial;
    MemoryPositionSearch *mps = &partial;
    std::vector< smart_ptr<ListableGame> > *source = &gc->gds;
    bool do_partial_search = true;
    if( objs.gl->db_clipboard )
        source = &objs.gl->gc_clipboard.gds;
    else
    {
        mps = &objs.db->tiny_db;
        do_partial_search = false;
    }
    int game_count = 0;

    // The fast MemoryPositionSearch facility was developed to scan all the games in a tiny database,
    //  but once it was available it made sense to apply it to searching for positions in any game
    //  list, in particular games from a disk based (i.e. not tiny) database and the clipboard. These
    //  latter types of search we are calling 'partial' search because they aren't of an entire, (albeit
    //  tiny) in memory database
    if( do_partial_search )
    {

        // The promotion attribute is only set automatically for the tiny database games (at the moment)
        for( size_t i=0; i<source->size(); i++ )
            (*source)[i]->CalculatePromotionAttribute();
        ProgressBar progress2("Searching Clipboard", "Searching",false);
        game_count = mps->DoSearch(cr_to_match,&progress2,source);
    }
    else
    {
        bool search_needed = !mps->IsThisSearchPosition(cr_to_match);
        cprintf( "search_needed = %s\n", search_needed?"true":"false" );
        if( search_needed )
        {
            ProgressBar progress2("Searching Database", "Searching",false);
            //progress2.DrawNow();
            game_count = mps->DoSearch(cr_to_match,&progress2);
        }
    }
    
    std::vector< smart_ptr<ListableGame> >  &db_games    = mps->GetVectorSourceGames();
    std::vector<DoSearchFoundGame>          &found_games = mps->GetVectorGamesFound();
    size_t nbr_found_games = found_games.size();
    
    {
        ProgressBar progress("Calculating Stats","Calculating Stats",false);
        for( size_t i=0; i<nbr_found_games; i++ )
        {
            double permill = (static_cast<double>(i) * 1000.0) / static_cast<double>(nbr_found_games);
            progress.Permill( static_cast<int>(permill) );
            int idx                   = found_games[i].idx;
            unsigned short offset_first = found_games[i].offset_first;
            unsigned short offset_last  = found_games[i].offset_last;
            temp.gds.push_back(db_games[idx]);
            std::string blob = db_games[idx]->CompressedMoves();
        
            // Search for a match to this game
            bool new_transposition_found=false;
            bool found=false;
            int found_idx=0;
            for( size_t j=0; !found && j<transpositions.size(); j++ )
            {
                size_t len = transpositions[j].blob.size();
                if( len <= blob.size() )
                {
                    std::string s1 = blob.substr(0,len);
                    std::string s2 = transpositions[j].blob;
                    if( s1 == s2 )
                    {
                        found = true;
                        found_idx = j;
                    }
                }
            }
            
            // If none so far add the one from this game
            if( !found )
            {
                new_transposition_found = true;
                PATH_TO_POSITION ptp;
                ptp.blob = blob.substr(0,offset_first);
                transpositions.push_back(ptp);
                found_idx = transpositions.size()-1;
            }
            const char *result = db_games[idx]->Result();
            bool white_wins = (0==strcmp(result,"1-0"));
            if( white_wins )
                total_white_wins++;
            bool black_wins = (0==strcmp(result,"0-1"));
            if( black_wins )
                total_black_wins++;
            bool draw       = (0==strcmp(result,"1/2-1/2"));
            if( draw )
                total_draws++;
            PATH_TO_POSITION *p = &transpositions[found_idx];
            p->frequency++;
            if( offset_last < blob.length() ) // must be more moves
            {
                char compressed_move = blob[offset_last];
                std::map< char, MOVE_STATS >::iterator it;
                it = stats.find(compressed_move);
                if( it == stats.end() )
                {
                    MOVE_STATS empty;
                    empty.nbr_games = 0;
                    empty.nbr_white_wins = 0;
                    empty.nbr_black_wins = 0;
                    empty.nbr_draws = 0;
                    stats[compressed_move] = empty;
                    it = stats.find(compressed_move);
                }
                it->second.nbr_games++;
                if( white_wins )
                    it->second.nbr_white_wins++;
                else if( black_wins )
                    it->second.nbr_black_wins++;
                else if( draw )
                    it->second.nbr_draws++;
            }

            // For speed, have the most frequent transpositions first        
            if( new_transposition_found )
            {
                std::sort( transpositions.rbegin(), transpositions.rend() );
            }
        }

        // Play through the current game, and find the last instance of a user move in this position
        CompactGame pact;
        objs.gl->gd.GetCompactGame( pact );
        thc::Move user_move;
        user_move.Invalid();
        thc::ChessRules scan(pact.start_position);
        for( size_t i=0; i<pact.moves.size(); i++ )
        {
            if( scan == cr_to_match )
                user_move = pact.moves[i];
            scan.PlayMove( pact.moves[i] );
        }

        // Sort the stats according to number of games
        std::multimap< MOVE_STATS,  char > dst = flip_and_sort_map(stats);
        std::multimap< MOVE_STATS,  char >::reverse_iterator it;
        moves_in_this_position.clear();
        wxArrayString strings_stats;
        for( it=dst.rbegin(); it!=dst.rend(); it++ )
        {
            double percentage_score = 0.0;
            int nbr_games      = it->first.nbr_games;
            int nbr_white_wins = it->first.nbr_white_wins;
            int nbr_black_wins = it->first.nbr_black_wins;
            int nbr_draws      = it->first.nbr_draws;
            int draws_plus_no_result = nbr_games - nbr_white_wins - nbr_black_wins;
            if( nbr_games )
                percentage_score = ((1.0*nbr_white_wins + 0.5*draws_plus_no_result) * 100.0) / nbr_games;
            char compressed_move = it->second;
            CompressMoves press(cr_to_match);
            thc::Move mv = press.UncompressMove( compressed_move );
            if( add_go_back )
            {
                add_go_back = false;
                wxString wstr( go_back_string.c_str() );
                strings_stats.Add(wstr);
                moves_in_this_position.push_back(mv);
            }
            moves_in_this_position.push_back(mv);
            std::string s = mv.NaturalOut(&cr_to_match);
            LangOut(s);
            if( !cr_to_match.white )
                s = "..." + s;
            char buf[200];
            sprintf( buf, "%s%s: %d %s, white scores %.1f%% +%d -%d =%d",
                    mv==user_move ? ">" : " ",
                    s.c_str(),
                    nbr_games,
                    nbr_games==1 ? "game" : "games",
                    percentage_score,
                    nbr_white_wins, nbr_black_wins, nbr_draws );
            cprintf( "%s\n", buf );
            wxString wstr(buf);
            strings_stats.Add(wstr);
        }

        // Print the transpositions in order
        std::sort( transpositions.rbegin(), transpositions.rend() );
        cprintf( "%d transpositions\n", transpositions.size() );
        wxArrayString strings_transpos;
        for( unsigned int j=0; j<transpositions.size(); j++ )
        {
            PATH_TO_POSITION *p = &transpositions[j];
            size_t len = p->blob.length();
            CompressMoves press;
            std::vector<thc::Move> unpacked = press.Uncompress(p->blob);
            std::string txt;
            thc::ChessRules cr2;
            for( unsigned int k=0; k<len; k++ )
            {
                thc::Move mv = unpacked[k];
                if( cr2.white )
                {
                    char buf[100];
                    sprintf( buf, "%d.", cr2.full_move_count );
                    txt += buf;
                }
                std::string s = mv.NaturalOut(&cr2);
                LangOut(s);
                txt += s;
                txt += " ";
                cr2.PlayMove(mv);
            }
            char buf[2000];
            sprintf( buf, "T%d: %s: %d occurences", j+1, txt.c_str(), p->frequency );
            cprintf( "%s\n", buf );
            wxString wstr(buf);
            strings_transpos.Add(wstr);
        }

        char buf[1000];
        int total_games  = temp.gds.size();
        int total_draws_plus_no_result = total_games - total_white_wins - total_black_wins;
        double percent_score=0.0;
        if( total_games )
            percent_score= ((1.0*total_white_wins + 0.5*total_draws_plus_no_result) * 100.0) / total_games;
        sprintf( buf, "%s%d %s, white scores %.1f%% +%d -%d =%d",
                objs.gl->db_clipboard ? "Clipboard search: " : "",
                total_games,
                total_games==1 ? "game" : "games",
                percent_score,
                total_white_wins, total_black_wins, total_draws );
        cprintf( "Got here #5, %s\n", buf );
        title_ctrl->SetLabel( buf );
        if( !list_ctrl_stats )
        {
            //wxSize sz4 = mini_board->GetSize();
            //sz4.x = (sz4.x*50)/100;
            //sz4.y = (sz4.y*50)/100;
            
            list_ctrl_stats   = new wxListBox( notebook, ID_DB_LISTBOX_STATS, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL );
            list_ctrl_transpo = new wxListBox( notebook, ID_DB_LISTBOX_TRANSPO, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL );
            notebook->AddPage(list_ctrl_stats,"Next Move",true);
            notebook->AddPage(list_ctrl_transpo,"Transpositions",false);
        }
        
        list_ctrl_stats->Clear();
        list_ctrl_transpo->Clear();
        if( !strings_stats.IsEmpty() )
            list_ctrl_stats->InsertItems( strings_stats, 0 );
        if( !strings_transpos.IsEmpty() )
            list_ctrl_transpo->InsertItems( strings_transpos, 0 );
        gc_db_displayed_games = temp;
        nbr_games_in_list_ctrl = gc_db_displayed_games.gds.size();
        dirty = true;
        list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
        bool have_games = (nbr_games_in_list_ctrl>0);
        GdvEnableControlsIfGamesFound( have_games );
        if( nbr_games_in_list_ctrl>0 )
            list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
    }
    Goto(0);
    SetTitle(save_title);
}

// Search for patterns
void DbDialog::PatternSearch()
{
    gc_db_displayed_games.gds.clear();
    cprintf( "Remove focus %d\n", track->focus_idx );
    list_ctrl->SetItemState( track->focus_idx, 0, wxLIST_STATE_FOCUSED );
    list_ctrl->SetItemState( track->focus_idx, 0, wxLIST_STATE_SELECTED );

    // hash to match
    MemoryPositionSearch partial;
    MemoryPositionSearch *mps = &partial;
    std::vector< smart_ptr<ListableGame> > *source = &gc->gds;
    bool do_partial_search = true;
    if( objs.gl->db_clipboard )
        source = &objs.gl->gc_clipboard.gds;
    else
    {
        mps = &objs.db->tiny_db;
        do_partial_search = false;
    }
    int game_count = 0;

    // The fast MemoryPositionSearch facility was developed to scan all the games in a tiny database,
    //  but once it was available it made sense to apply it to searching for positions in any game
    //  list, in particular games from a disk based (i.e. not tiny) database and the clipboard. These
    //  latter types of search we are calling 'partial' search because they aren't of an entire, (albeit
    //  tiny) in memory database
    PATTERN_STATS stats_;
    if( do_partial_search )
    {

        // The promotion attribute is only set automatically for the tiny database games (at the moment)
        for( size_t i=0; i<source->size(); i++ )
            (*source)[i]->CalculatePromotionAttribute();
        ProgressBar progress2(objs.gl->db_clipboard ? "Searching" : "Searching", "Searching",false);
        game_count = mps->DoPatternSearch(pm,&progress2,stats_,source);
    }
    else
    {
        ProgressBar progress2("Searching", "Searching",false);
        game_count = mps->DoPatternSearch(pm,&progress2,stats_);
    }
    
    std::vector< smart_ptr<ListableGame> >  &db_games    = mps->GetVectorSourceGames();
    std::vector<DoSearchFoundGame>          &found_games = mps->GetVectorGamesFound();
    size_t nbr_found_games = found_games.size();
    
    for( size_t i=0; i<nbr_found_games; i++ )
    {
        int idx                     = found_games[i].idx;
        gc_db_displayed_games.gds.push_back(db_games[idx]);
    }
        
    nbr_games_in_list_ctrl = gc_db_displayed_games.gds.size();
    dirty = true;
    list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
    bool have_games = (nbr_games_in_list_ctrl>0);
    GdvEnableControlsIfGamesFound( have_games );
    if( nbr_games_in_list_ctrl>0 )
        list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
    char buf[1000];
    char base[1000];
    int total_games  = stats_.nbr_games;
    int total_draws_plus_no_result = total_games - stats_.white_wins - stats_.black_wins;
    double percent_score=0.0;
    if( total_games )
        percent_score= ((1.0*stats_.white_wins + 0.5*total_draws_plus_no_result) * 100.0) / total_games;
    sprintf( base, "%s%d %s, white scores %.1f%% +%d -%d =%d",
            objs.gl->db_clipboard ? "Clipboard search: " : "",
            total_games,
            total_games==1 ? "game" : "games",
            percent_score,
            stats_.white_wins, stats_.black_wins, stats_.draws );
    if( !pm.parm.include_reverse_colours )
    {
        sprintf( buf, "%s", base );
    }
    else
    {
        if( stats_.nbr_reversed_games == 0 )
            sprintf( buf, "%s (stats adjusted for reverse %s games, but none found)", base, gbl_spell_colour );
        else
            sprintf( buf, "%s (stats adjusted for %d reverse %s game%s)", base, stats_.nbr_reversed_games, gbl_spell_colour, stats_.nbr_reversed_games>1 ? "s": "" );
    }
    title_ctrl->SetLabel( buf );
    pm.NowReady();
    int top = list_ctrl->GetTopItem();
    int count = 1 + list_ctrl->GetCountPerPage();
    if( count > nbr_games_in_list_ctrl )
        count = nbr_games_in_list_ctrl;
    for( int i=0; i<count; i++ )
        list_ctrl->RefreshItem(top++);
    Goto(0);
}

