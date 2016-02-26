/****************************************************************************
 * Database data source for games dialog
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
#include "Appdefs.h"
#include "DebugPrintf.h"
#include "thc.h"
#include "AutoTimer.h"
#include "GameDetailsDialog.h"
#include "GamePrefixDialog.h"
#include "GameLogic.h"
#include "Objects.h"
#include "PgnFiles.h"
#include "MiniBoard.h"
#include "DbDialog.h"
#include "Database.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <list>    
#include <algorithm>

using namespace std;


wxSizer *DbDialog::GdvAddExtraControls()
{
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

    // Text control for white entry
    sz5 = wxDefaultSize;
    sz5.x = (sz4.x*55)/100;
    //sz5.y = (sz4.y*2)/10;

    text_ctrl = 0;
    if( db_req == REQ_PLAYERS )
    {
        //text_ctrl->SetSize( sz3.x*2, sz3.y );      // temp temp
        text_ctrl = new wxTextCtrl ( this, ID_DB_TEXT, wxT(""), wxDefaultPosition, sz5, 0 );
        vsiz_panel_button1->Add(text_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        wxSize sz2=text_ctrl->GetSize();
        //text_ctrl->SetSize((sz2.x*10)/32,sz2.y);      // temp temp
        //text_ctrl->SetSize((sz.x*7)/2,sz2.y);      // temp temp
        text_ctrl->SetValue("White Player");

        wxButton* search = new wxButton ( this, ID_DB_SEARCH, wxT("Search"),
                                         wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(search, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        //wxSize sz3=reload->GetSize();
        //text_ctrl->SetSize( sz3.x*2, sz3.y );      // temp temp
    }
 
    
//    wxStaticText* spacer1 = new wxStaticText( this, wxID_ANY, wxT(""),
//                                     wxDefaultPosition, wxDefaultSize, 0 );
//    vsiz_panel_button1->Add(spacer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

//    white_player_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX2,
//                                 wxT("&White player"), wxDefaultPosition, wxDefaultSize, 0 );
//    vsiz_panel_button1->Add(white_player_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
//    white_player_ctrl->SetValue( true );

    wxButton* btn1 = new wxButton ( this, ID_BUTTON_1, wxT("Clear Clipboard"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* btn2 = new wxButton ( this, ID_BUTTON_2, wxT("Add to Clipboard"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    wxButton* btn3 = new wxButton ( this, ID_BUTTON_3, wxT("Add All Player's White Games"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    white_player_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX2,
                                   wxT("&White player"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(white_player_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    white_player_ctrl->SetValue( true );

    wxButton* btn4 = new wxButton ( this, ID_BUTTON_4, wxT("Add All Player's Black Games"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* save_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save all"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    filter_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX,
                                 wxT("&Clipboard as temp database"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(filter_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    filter_ctrl->SetValue( objs.gl->db_clipboard );
    wxStaticText* spacer1 = new wxStaticText( this, wxID_ANY, wxT(""),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(spacer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    /*radio_ctrl = new wxRadioButton( this,  ID_DB_RADIO,
     wxT("&Radio"), wxDefaultPosition, wxDefaultSize,  wxRB_GROUP );
     vsiz_panel_buttons->Add(radio_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
     radio_ctrl->SetValue( false ); */
 
#if 0
    wxString combo_array[9];
    combo_array[0] = "Equals";
    combo_array[1] = "Starts with";
    combo_array[2] = "Ends with";
    combo_ctrl = new wxComboBox ( this, ID_DB_COMBO,
                                 "None", wxDefaultPosition,
                                 wxSize(50, wxDefaultCoord), 3, combo_array, wxCB_READONLY );
    vsiz_panel_buttons->Add(combo_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxString combo;
    combo = "None";
    combo_ctrl->SetValue(combo);
    wxSize sz3=combo_ctrl->GetSize();
    combo_ctrl->SetSize((sz3.x*118)/32,sz3.y);      // temp temp
#endif
    return vsiz_panel_button1;   
}

#define LOAD_INTO_MEMORY_THRESHOLD 2000
#define QUERY_LOAD_INTO_MEMORY_THRESHOLD 10000

void DbDialog::GdvOnActivate()
{
    if( !activated_at_least_once )
    {
        activated_at_least_once = true;
        wxPoint pos = ok_button->GetPosition();
        wxSize  sz  = ok_button->GetSize();
        pos.x += 3*(sz.x);
        ok_button->SetPosition( pos );
        ok_button->Update();
        ok_button->Refresh();

        if( db_req == REQ_PLAYERS )
        {
            notebook->Hide();
        }
        else
        {
            wxPoint pos2 = notebook->GetPosition();
            wxSize  sz2  = notebook->GetSize();
            wxSize  sz3  = ok_button->GetSize();
            pos2.x += (sz2.x/2);        
            pos2.y += (sz2.y/2);        
            pos2.x -= (sz3.x/2);        
            pos2.y -= (sz3.y/2);        
            //pos_button.x += (sz_panel.x/2 - sz_button.x/2);
            //pos_button.y += (sz_panel.y/2 - sz_button.y/2);
            utility = new wxButton ( this, ID_DB_UTILITY, wxT("Calculate Stats"),
                                    pos2, wxDefaultSize, 0 );
            utility->Raise();
            wxSize sz_panel  = notebook->GetSize();
            wxSize sz_button = utility->GetSize();
            wxPoint pos_button = utility->GetPosition();
            //utility->SetPosition( pos_button );
            if( objs.gl->db_clipboard )
            {
                StatsCalculate();
            }
            else if( objs.db->IsTinyDb() || nbr_games_in_list_ctrl <= LOAD_INTO_MEMORY_THRESHOLD )
            {
                bool ok = LoadGamesIntoMemory();
                if( ok )
                    StatsCalculate();
            }
        }
        Goto(0); // list_ctrl->SetFocus();
    }
}



// Read game information from games or database
void DbDialog::GdvReadItem( int item, CompactGame &pact )
{
    bool in_memory = ReadItemFromMemory( item, pact );
    if( !in_memory )
    {
        objs.db->GetRow( item, &pact );
        pact.transpo_nbr = 0;
    }
}

bool DbDialog::ReadItemFromMemory( int item, CompactGame &pact )
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
                std::string str_blob = gc_db_displayed_games.gds[item]->RefCompressedMoves();
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

// DbDialog constructors
DbDialog::DbDialog
(
    wxWindow    *parent,
    thc::ChessRules *cr,
    GamesCache  *gc,
    GamesCache  *gc_clipboard,
    DB_REQ      db_req,
    wxWindowID  id,
    const wxPoint& pos,
    const wxSize& size,
    long style
 ) : GamesDialog( parent, cr, gc, gc_clipboard, id, pos, size )
{
    this->db_req = db_req;
    activated_at_least_once = false;
    transpo_activated = false;
    white_player_search = true;
    utility = NULL;
}


// Overidden function used for smart move column compare
bool DbDialog::MoveColCompareReadGame( MoveColCompareElement &e, int idx, const char *blob  )
{        
    std::string str_blob = std::string(blob);
    unsigned int sz2 = transpositions.size();
    for( unsigned int j=0; j<sz2; j++ )
    {
        PATH_TO_POSITION *ptp = &transpositions[j];
        unsigned int offset = ptp->blob.length();
        if( 0 == memcmp( ptp->blob.c_str(), str_blob.c_str(), offset ) )
        {
            e.idx  = idx;
            e.blob = str_blob.substr(offset);   // rest of game, from start position
            e.counts.resize( e.blob.length() );
            e.transpo = transpo_activated ? j+1 : 0;
            return true;
        }
    }
    return false;
}


bool DbDialog::LoadGamesPrompted( std::string prompt )
{
    bool ok=true;
    bool in_memory = (gc_db_displayed_games.gds.size() > 0);
    if( !in_memory )
    {
        if( nbr_games_in_list_ctrl >= QUERY_LOAD_INTO_MEMORY_THRESHOLD )
        {
            wxString msg( prompt.c_str() );
            int answer = wxMessageBox( msg, "Press Yes to load games",  wxYES_NO|wxCANCEL, this );
            bool load_games = (answer == wxYES);
            if( !load_games )
                return false;
        }
        ok = LoadGamesIntoMemory();
        if( ok )
            StatsCalculate();
    }
    return ok;
}

void DbDialog::GdvListColClick( int compare_col )
{
    if( LoadGamesPrompted
        ("This column sort requires loading a large number of games into memory, is that okay?")
      )
    {
        ColumnSort( compare_col, gc_db_displayed_games.gds );  
    }
}

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


void DbDialog::GdvSaveAllToAFile()
{
    if( LoadGamesPrompted
          ("Saving these games to a file requires loading a large number of games into memory, is that okay?")
      )
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
            gc_db_displayed_games.FileSaveAllAsAFile( filename );
        }
    }
}


void DbDialog::GdvOnCancel()
{
    gc->PrepareForResumePreviousWindow( list_ctrl->GetTopItem() );
    int sz=gc->gds.size();
 /* for( int i=0; i<sz; i++ )
    {
        smart_ptr<ListableGame> smp = gc->gds[i];
        bool selected = (wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED)) ? true : false;
        smp->SetSelected(selected);
        bool focused  =  (wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED)  ) ? true : false;
        smp->SetFocused(focused);
    } */
}

void DbDialog::GdvHelpClick()
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
    wxT("Use this panel to load games from the database\n\n");
    wxMessageBox(helpText,
    wxT("Database Dialog Help"),
    wxOK|wxICON_INFORMATION, NULL );
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

void DbDialog::GdvUtility()
{
    bool ok = LoadGamesIntoMemory();
    if( ok )
        StatsCalculate();
}

void DbDialog::GdvButton1()
{
    objs.gl->gc_clipboard.gds.clear();
    Goto( track->focus_idx );
}

void DbDialog::GdvButton2()
{
    CopyOrAdd( false );
}

void DbDialog::GdvButton3()
{
    std::string player_name = white_player_search ? track->info.r.white : track->info.r.black;
    int nbr_loaded = objs.db->LoadGamesWithQuery( player_name, true, objs.gl->gc_clipboard.gds );
    if( nbr_loaded > 0 )
    {
        char buf[2000];
        sprintf( buf, "Added %d white games played by \"%s\" to clipboard", nbr_loaded, player_name.c_str() );
        wxMessageBox( buf, "Added player's white games to clipboard", wxOK, this );
    }
    Goto( track->focus_idx );
}

void DbDialog::GdvButton4()
{
    std::string player_name = white_player_search ? track->info.r.white : track->info.r.black;
    int nbr_loaded = objs.db->LoadGamesWithQuery( player_name, false, objs.gl->gc_clipboard.gds );
    if( nbr_loaded > 0 )
    {
        char buf[2000];
        sprintf( buf, "Added %d black games played by \"%s\" to clipboard", nbr_loaded, player_name.c_str() );
        wxMessageBox( buf, "Added player's black games to clipboard", wxOK, this );
    }
    Goto( track->focus_idx );
}

// UseGame feature
void DbDialog::GdvButton5()
{
    objs.gl->gd.UseGame( cr_base, moves_from_base_position, track->info );
    TransferDataToWindow();
    AcceptAndClose();
}

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

void DbDialog::GdvCheckBox( bool checked )
{
    objs.gl->db_clipboard = checked;

    // Clear back to the base position
    this->cr = cr_base;
    moves_from_base_position.clear();
    gc_db_displayed_games.gds.clear();
    drill_down_set.clear();
    if( objs.gl->db_clipboard )
    {
        nbr_games_in_list_ctrl = gc_clipboard->gds.size();
    }
    else   // TODO need special handling for initial position
    {
        nbr_games_in_list_ctrl = objs.db->SetDbPosition( db_req, cr );
        
        // We have shown stats while db_clipboard was true. So to avoid going back to
        //  situation where stats must be requested, unconditionally load games into
        //  memory (Calculate Stats button loads games into memory)
        gc->gds.clear();
        LoadGamesIntoMemory();
    }
    StatsCalculate();
}


bool DbDialog::LoadGamesIntoMemory()
{
    AutoTimer at("Load games into memory");
    
    // Load all the matching games from the database
    bool ok = objs.db->LoadAllGames( gc->gds, nbr_games_in_list_ctrl );
    if( ok )
    {
        moves_from_base_position.clear();
    
        // No need to look for more games in database in base position
        drill_down_set.clear();
        uint64_t base_hash = this->cr.Hash64Calculate();
        drill_down_set.insert(base_hash);
        games_set.clear();
        for( unsigned int i=0; i<gc->gds.size(); i++ )
        {
            int game_id = gc->gds[i]->GetGameId();
            if( game_id )
                games_set.insert( game_id );
        }
    }
    return ok;
}

void DbDialog::CopyOrAdd( bool clear_clipboard )
{
    int idx_focus = -1;
    int nbr_copied = 0;
    if( list_ctrl )
    {
        CompactGame pact;
        int nbr = list_ctrl->GetItemCount();
        for( int i=0; i<nbr; i++ )
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
                if( nbr == gc_db_displayed_games.gds.size() )
                    gc_clipboard->gds.push_back( gc_db_displayed_games.gds[i] ); // assumes smart_ptr is std::shared_ptr
                else
                {
                    CompactGame pact;
                    GdvReadItem( i, pact );
                    ListableGameDb info( pact.game_id, pact );
                    make_smart_ptr( ListableGameDb, new_info, info );
                    gc_clipboard->gds.push_back( std::move(new_info ) );
                }
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
            if( nbr == gc_db_displayed_games.gds.size() )
                gc_clipboard->gds.push_back( gc_db_displayed_games.gds[idx_focus] ); // assumes smart_ptr is std::shared_ptr
            else
            {
                CompactGame pact;
                GdvReadItem( idx_focus, pact );
                ListableGameDb info( pact.game_id, pact );
                make_smart_ptr( ListableGameDb, new_info, info );
                gc_clipboard->gds.push_back( std::move(new_info ) );
            }
            nbr_copied++;
        }
        Goto( 0<=idx_focus && idx_focus<nbr ? idx_focus : 0 );
    }
    dbg_printf( "%d games copied\n", nbr_copied );
}


void DbDialog::StatsCalculate()
{
    int total_white_wins = 0;
    int total_black_wins = 0;
    int total_draws = 0;
    transpositions.clear();
    stats.clear();
    dirty = true;
    gc_db_displayed_games.gds.clear();
    cprintf( "Remove focus %d\n", track->focus_idx );
    list_ctrl->SetItemState( track->focus_idx, 0, wxLIST_STATE_FOCUSED );
    list_ctrl->SetItemState( track->focus_idx, 0, wxLIST_STATE_SELECTED );
    thc::ChessRules cr_to_match = this->cr;
    bool add_go_back = false;
    std::string go_back_string;
    for( int i=0; i<moves_from_base_position.size(); i++ )
    {
        thc::Move mv = moves_from_base_position[i];
        if( i+1 == moves_from_base_position.size() )
        {
            std::string s = mv.NaturalOut(&cr_to_match);
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
    int maxlen = 1000000;   // absurdly large until a match found

    // Only if we haven't seen this position, look for and load extra games (due to transposition) from database
    if( objs.gl->db_clipboard || drill_down_set.count(gbl_hash) > 0 )
    {
        cprintf( "Already seen this position\n" );
    }
    else
    {
        drill_down_set.insert(gbl_hash);
        objs.db->LoadGamesWithQuery( cr_to_match, gbl_hash, gc->gds, games_set );
    }
    
    // For each cached game
    std::vector< smart_ptr<ListableGame> > &source = (objs.gl->db_clipboard ? objs.gl->gc_clipboard.gds : gc->gds );
    for( unsigned int i=0; i<source.size(); i++ )
    {
        Roster r         = source[i]->RefRoster();
        std::string blob = source[i]->RefCompressedMoves();
    
        // Search for a match to this game
        bool new_transposition_found=false;
        bool found=false;
        int found_idx=0;
        for( unsigned int j=0; !found && j<transpositions.size(); j++ )
        {
            size_t len = transpositions[j].blob.size();
            std::string s1 = blob.substr(0,len);
            std::string s2 = transpositions[j].blob;
            if( s1 == s2 )
            {
                found = true;
                found_idx = j;
            }
        }
        
        // If none so far add the one from this game
        if( !found )
        {
            PATH_TO_POSITION ptp;
            CompressMoves press;
            size_t len = blob.length();
            const char *b = (const char*)blob.c_str();
            uint64_t hash = press.cr.Hash64Calculate();
            found = (hash==gbl_hash && press.cr==cr_to_match );
            int offset=0;
            while( !found && offset<len && offset<maxlen )
            {
                thc::ChessRules cr_hash = press.cr;
                thc::Move mv = press.UncompressMove( *b++ );
                hash = cr_hash.Hash64Update( hash, mv );
                offset++;
                if( hash == gbl_hash && press.cr==cr_to_match )
                    found = true;
            }
            if( found )
            {
                new_transposition_found = true;
                ptp.blob = blob.substr(0,offset);
                transpositions.push_back(ptp);
                found_idx = transpositions.size()-1;
                maxlen = offset+16; // stops unbounded searching through unrelated game
            }
        }
        if( found )
        {
            bool white_wins = (r.result=="1-0");
            if( white_wins )
                total_white_wins++;
            bool black_wins = (r.result=="0-1");
            if( black_wins )
                total_black_wins++;
            bool draw       = (r.result=="1/2-1/2");
            if( draw )
                total_draws++;
            ListableGameDb temp(0,r,blob);
            make_smart_ptr( ListableGameDb, smptr, temp );
            gc_db_displayed_games.gds.push_back(smptr);
            PATH_TO_POSITION *p = &transpositions[found_idx];
            p->frequency++;
            size_t len = p->blob.length();
            if( len < blob.length() ) // must be more moves
            {
                const char *next_compressed_move = blob.c_str()+len;
                CompressMoves press = press_to_match;
                thc::Move mv = press.UncompressMove( *next_compressed_move );
                uint32_t imv = 0;
                assert( sizeof(imv) == sizeof(mv) );
                memcpy( &imv, &mv, sizeof(mv) ); // FIXME
                std::map< uint32_t, MOVE_STATS >::iterator it;
                it = stats.find(imv);
                if( it == stats.end() )
                {
                    MOVE_STATS empty;
                    empty.nbr_games = 0;
                    empty.nbr_white_wins = 0;
                    empty.nbr_black_wins = 0;
                    empty.nbr_draws = 0;
                    stats[imv] = empty;
                    it = stats.find(imv);
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
    }
    
    // Sort the stats according to number of games
    std::multimap< MOVE_STATS,  uint32_t > dst = flip_and_sort_map(stats);
    std::multimap< MOVE_STATS,  uint32_t >::reverse_iterator it;
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
        uint32_t imv=it->second;
        thc::Move mv;
        memcpy( &mv, &imv, sizeof(mv) ); // FIXME
        if( add_go_back )
        {
            add_go_back = false;
            wxString wstr( go_back_string.c_str() );
            strings_stats.Add(wstr);
            moves_in_this_position.push_back(mv);
        }
        moves_in_this_position.push_back(mv);
        std::string s = mv.NaturalOut(&cr_to_match);
        if( !cr_to_match.white )
            s = "..." + s;
        char buf[200];
        sprintf( buf, "%s: %d games, white scores %.1f%% +%d -%d =%d",
                s.c_str(),
                nbr_games, percentage_score,
                nbr_white_wins, nbr_black_wins, nbr_draws );
        cprintf( "%s\n", buf );
        wxString wstr(buf);
        strings_stats.Add(wstr);
    }

    // Print the transpositions in order
    cprintf( "%d transpositions\n", transpositions.size() );
    wxArrayString strings_transpos;
    for( unsigned int j=0; j<transpositions.size(); j++ )
    {
        PATH_TO_POSITION *p = &transpositions[j];
        size_t len = p->blob.length();
        CompressMoves press;
        std::vector<thc::Move> unpacked = press.Uncompress(p->blob);
        std::string txt;
        thc::ChessRules cr;
        for( int k=0; k<len; k++ )
        {
            thc::Move mv = unpacked[k];
            if( cr.white )
            {
                char buf[100];
                sprintf( buf, "%d.", cr.full_move_count );
                txt += buf;
            }
            txt += mv.NaturalOut(&cr);
            txt += " ";
            cr.PlayMove(mv);
        }
        char buf[2000];
        sprintf( buf, "T%d: %s: %d occurences", j+1, txt.c_str(), p->frequency );
        cprintf( "%s\n", buf );
        wxString wstr(buf);
        strings_transpos.Add(wstr);
    }

    nbr_games_in_list_ctrl = gc_db_displayed_games.gds.size();
    dirty = true;
    list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
    list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
    char buf[1000];
    int total_games  = nbr_games_in_list_ctrl;
    int total_draws_plus_no_result = total_games - total_white_wins - total_black_wins;
    double percent_score=0.0;
    if( total_games )
        percent_score= ((1.0*total_white_wins + 0.5*total_draws_plus_no_result) * 100.0) / total_games;
    sprintf( buf, "%d games, white scores %.1f%% +%d -%d =%d",
            total_games, percent_score,
            total_white_wins, total_black_wins, total_draws );
    cprintf( "Got here #5\n" );
    title_ctrl->SetLabel( buf );

    int top = list_ctrl->GetTopItem();
    int count = 1 + list_ctrl->GetCountPerPage();
    if( count > nbr_games_in_list_ctrl )
        count = nbr_games_in_list_ctrl;
    for( int i=0; i<count; i++ )
        list_ctrl->RefreshItem(top++);
    list_ctrl->SetFocus();

    if( !list_ctrl_stats )
    {
        if( utility )
            utility->Hide();
        wxSize sz4 = mini_board->GetSize();
        sz4.x = (sz4.x*13)/10;
        sz4.y = (sz4.y*10)/10;
        
        list_ctrl_stats   = new wxListBox( notebook, ID_DB_LISTBOX_STATS, wxDefaultPosition, sz4, 0, NULL, wxLB_HSCROLL );
        list_ctrl_transpo = new wxListBox( notebook, ID_DB_LISTBOX_TRANSPO, wxDefaultPosition, sz4, 0, NULL, wxLB_HSCROLL );
        notebook->AddPage(list_ctrl_stats,"Next Move",true);
        notebook->AddPage(list_ctrl_transpo,"Transpositions",false);
    }
    
    list_ctrl_stats->Clear();
    list_ctrl_transpo->Clear();
    list_ctrl_stats->InsertItems( strings_stats, 0 );
    list_ctrl_transpo->InsertItems( strings_transpos, 0 );
    Goto(0);
}

// One of the moves in move stats is clicked
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
    for( int i=0; i<moves_from_base_position.size(); i++ )
    {
        thc::Move mv = moves_from_base_position[i];
        cr_to_match.PlayMove(mv);
    }
    uint64_t hash = cr_to_match.Hash64Calculate();
    if( drill_down_set.count(hash) > 0 )
    {
        cprintf( "Already seen this position\n" );
    }
    else
    {
        title_ctrl->SetLabel( "Searching for extra games through transposition...." );
        title_ctrl->Refresh();
        title_ctrl->Update();
        //title_ctrl->MacDoRedraw(0);
    }

    wxSafeYield();
#ifdef THC_MAC
    CallAfter( &DbDialog::StatsCalculate );
#else
    StatsCalculate();
#endif
}

