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
#include "Database.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <list>    
#include <algorithm>

using namespace std;


void DbDialog::AddExtraControls()
{
    // Text control for white entry
    wxSize sz4 = mini_board->GetSize();
    wxSize sz5 = wxDefaultSize;
    sz5.x = (sz4.x*55)/100;
    //sz5.y = (sz4.y*2)/10;

    //text_ctrl->SetSize( sz3.x*2, sz3.y );      // temp temp
    text_ctrl = new wxTextCtrl ( this, ID_DB_TEXT, wxT(""), wxDefaultPosition, sz5, 0 );
    vsiz_panel_button1->Add(text_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxSize sz2=text_ctrl->GetSize();
    //text_ctrl->SetSize((sz2.x*10)/32,sz2.y);      // temp temp
    //text_ctrl->SetSize((sz.x*7)/2,sz2.y);      // temp temp
    text_ctrl->SetValue("White Player");

    wxButton* reload = new wxButton ( this, ID_DB_RELOAD, wxT("Search"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(reload, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //wxSize sz3=reload->GetSize();
    //text_ctrl->SetSize( sz3.x*2, sz3.y );      // temp temp
 
    
    wxButton* btn1 = new wxButton ( this, ID_BUTTON_1, wxT("Clear Clipboard"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_buttons->Add(btn1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* btn2 = new wxButton ( this, ID_BUTTON_2, wxT("Add to Clipboard"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_buttons->Add(btn2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    wxButton* btn3 = new wxButton ( this, ID_BUTTON_3, wxT("Add All Player's White Games"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_buttons->Add(btn3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    wxButton* btn4 = new wxButton ( this, ID_BUTTON_4, wxT("Add All Player's Black Games"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_buttons->Add(btn4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    filter_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX,
                                 wxT("&Clipboard as temp database"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_buttons->Add(filter_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    filter_ctrl->SetValue( false );
    
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
   
}

void DbDialog::OnActivate()
{
    if( !activated_at_least_once )
    {
        activated_at_least_once = true;
        //cprintf( "GamesDialog::OnActivate\n");
        //wxPoint pos = notebook->GetPosition();
        //list_ctrl_stats->Hide();
        //list_ctrl_transpo->Hide();
        
        wxPoint pos = ok_button->GetPosition();
        wxSize  sz  = ok_button->GetSize();
        pos.x += 3*(sz.x);
        ok_button->SetPosition( pos );
        ok_button->Update();
        ok_button->Refresh();

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
        
        Goto(0); // list_ctrl->SetFocus();
    }
}



// Read game information from games or database
void DbDialog::ReadItem( int item, DB_GAME_INFO &info )
{
    bool in_memory = ReadItemFromMemory( item, info );
    if( !in_memory )
    {
        objs.db->GetRow( &info, item );
        //cprintf( "ListCtrl::ReadItem(%d) READ %s FROM DATABASE\n", item, info.Description().c_str() );
        info.transpo_nbr = 0;
    }
}

bool DbDialog::ReadItemFromMemory( int item, DB_GAME_INFO &info )
{
    bool in_memory = false;
    info.transpo_nbr = 0;
    int nbr_games = games.size();
    if( 0<=item && item<nbr_games )
    {
        in_memory = true;
        info = games[nbr_games-1-item];
        info.transpo_nbr = 0;
        //cprintf( "ReadItemFromMemory(%d), white=%s\n", item, info.white.c_str() );
        //if( info.move_txt.length() == 0 )
        {
            //info.db_calculate_move_txt( objs.db->gbl_hash );
            if( transpo_activated && transpositions.size() > 1 )
            {
                for( unsigned int j=0; j<transpositions.size(); j++ )
                {
                    std::string &this_one = transpositions[j].blob;
                    const char *p = this_one.c_str();
                    size_t len = this_one.length();
                    if( info.str_blob.length()>=len && 0 == memcmp(p,info.str_blob.c_str(),len) )
                    {
                        info.transpo_nbr = j+1;
                        break;
                    }
                }
            }
        }
    }
    return in_memory;
}

static bool DebugIsTarget( const DB_GAME_INFO &info )
{
    bool match = (info.str_blob == "\xc3\xa3\x93\xa0\xb3\xc1\x10\xb3\xc1\x54\x81\xd1\x80\x51\xa1\x54\x5b\xd0\x10\x15\x66\x26\xd3\x02\x01\x24\x6d\x25\x6c\x7c\x1d\x6b\x23\x81\x2c\x3a\x34\x45\xe1\x73\x23\xf1\x20\x64\x31\x7a\x3e\x61\xf3\x0f\x35\x91\x06\x81\xf1\xe3\xf2\x08\x47\x47\xe1\x2c\x06\x64\xd1\x73\x0e\x81\x4a\xc2\x48\x6e\x77");
    return match;
}

static void DebugDumpBlob( const char *msg, const DB_GAME_INFO &info )
{
    char buf[2000];
    char *p = buf;
    for( int i=0; i<info.str_blob.length(); i++ )
    {
        char c = info.str_blob[i];
        unsigned u = (unsigned)c;
        u &= 0xff;
        if( i == 0 )
            sprintf( p, "%02x", u );
        else
            sprintf( p, " %02x", u );
        p = strchr(p,'\0');
    }
    cprintf( "%s [%s]\n", msg, buf );
}


// DbDialog constructors
DbDialog::DbDialog
(
    wxWindow    *parent,
    thc::ChessRules *cr,
    GamesCache  *gc,
    GamesCache  *gc_clipboard,
    wxWindowID  id,
    const wxPoint& pos,
    const wxSize& size,
    long style
 ) : GamesDialog( parent, cr, gc, gc_clipboard, id, pos, size )
{
    activated_at_least_once = false;
    transpo_activated = false;
    clipboard_db = false;
}


static DbDialog *backdoor;
static bool compare( const DB_GAME_INFO &g1, const DB_GAME_INFO &g2 )
{
    bool lt=false;
    bool is_target1 = DebugIsTarget( g1 );
    bool is_target2 = DebugIsTarget( g2 );
    switch( backdoor->compare_col )
    {
        case 1: lt = g1.white < g2.white;           break;
        case 3: lt = g1.black < g2.black;           break;
        default:
        {
            /* if( is_target1 )
            {
                DebugDumpBlob("compare(g1,g2) g1 is match, g2 is", g2 );
            }
            if( is_target2 )
            {
                DebugDumpBlob("compare(g1,g2) g2 is match, g1 is", g1 );
            } */
            unsigned int sz = backdoor->transpositions.size();
            for( unsigned int i=0; i<sz; i++ )
            {
                PATH_TO_POSITION *ptp1 = &backdoor->transpositions[i];
                unsigned int offset1 = ptp1->blob.length();
                if( 0 == memcmp( ptp1->blob.c_str(), g1.str_blob.c_str(), offset1 ) )
                {
                    for( unsigned int j=0; j<sz; j++ )
                    {
                        PATH_TO_POSITION *ptp2 = &backdoor->transpositions[j];
                        unsigned int offset2 = ptp2->blob.length();
                        if( 0 == memcmp( ptp2->blob.c_str(), g2.str_blob.c_str(), offset2 ) )
                        {
                            if( backdoor->transpo_activated && i!=j )
                            {
                                lt = i<j;
                            }
                            else
                            {
                                // lt = g1.str_blob.substr(offset1) < g2.str_blob.substr(offset2);
                                unsigned int len1 = g1.str_blob.length();
                                unsigned int len2 = g2.str_blob.length();
                                const char *p = g1.str_blob.c_str() + offset1;
                                const char *q = g2.str_blob.c_str() + offset2;
                                int idx = 0;
                                while( *p == *q )
                                {
                                    idx++;
                                    if( offset1+idx >= len1 || offset2+idx >= len2 )
                                        break;
                                    p++;
                                    q++;
                                }
                                if( offset1+idx>=len1 && offset2+idx<len2 )
                                    lt = true;
                                else if( offset1+idx<len1 && offset2+idx<len2 )
                                {
                                    unsigned int c = *p;
                                    unsigned int d = *q;
                                    unsigned int hi = c&0xf0;
                                    unsigned int lo = c&0x0f;
                                    if( hi == 0x00 )
                                    {
                                        if( 1<=lo && lo<=4 )
                                            hi = 0x30; // O-O or O-O-O
                                        else
                                            hi = 0x10; // K
                                    }
                                    else if( hi==0x10 || hi==0x20 )
                                    {
                                        if( lo < 8 )
                                            hi = 0x20; // N
                                        else
                                            hi = 0x40; // Q (shadow)
                                    }
                                    else if( hi==0x30 || hi==0x40 )
                                    {
                                        hi = 0x60;     // R
                                    }
                                    else if( hi==0x50 || hi==0x60 )
                                    {
                                        hi = 0x00;     // B
                                    }
                                    else if( hi == 0x70 )
                                    {
                                        hi = 0x50; // Q
                                    }
                                    c = hi|lo;
                                    hi = d&0xf0;
                                    lo = d&0x0f;
                                    if( hi == 0x00 )
                                    {
                                        if( 1<=lo && lo<=4 )
                                            hi = 0x30; // O-O or O-O-O
                                        else
                                            hi = 0x10; // K
                                    }
                                    else if( hi==0x10 || hi==0x20 )
                                    {
                                        if( lo < 8 )
                                            hi = 0x20; // N
                                        else
                                            hi = 0x40; // Q (shadow)
                                    }
                                    else if( hi==0x30 || hi==0x40 )
                                    {
                                        hi = 0x60;     // R
                                    }
                                    else if( hi==0x50 || hi==0x60 )
                                    {
                                        hi = 0x00;     // B
                                    }
                                    else if( hi == 0x70 )
                                    {
                                        hi = 0x50; // Q
                                    }
                                    d = hi|lo;
                                    lt = c<d;
                                }
                            }
                            break;
                        }
                    }
                }
            }
            break;
        }
    }
    if( is_target1 || is_target2 )
    {
        cprintf( "compare() returns lt is %s\n", lt?"true":"false" );
    }
    return lt;
}

struct TempElement
{
    int idx;
    std::string blob;
    std::vector<int> counts;
};

static bool compare_blob( const TempElement &e1, const TempElement &e2 )
{
    bool lt = e1.blob < e2.blob;
    return lt;
}

static bool compare_counts( const TempElement &e1, const TempElement &e2 )
{
    bool lt = false;
    unsigned int len = e1.blob.length();
    if( e2.blob.length() < len )
        len = e2.blob.length();
    for( unsigned int i=0; i<len; i++ )
    {
        if( e1.counts[i] != e2.counts[i] )
        {
            lt = (e1.counts[i] < e2.counts[i]);
            return lt;;
        }
    }
    lt = (e1.blob.length() < e2.blob.length());
    return lt;
}

void DbDialog::SmartCompare()
{
    std::vector<TempElement> inter;     // intermediate representation
    
    // Step 1, do a conventional string sort, beginning at our offset into the blob
    unsigned int sz = games.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        DB_GAME_INFO &g = games[i];
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
        temp.push_back( games[e.idx] );
    }

    // Step 5 replace original games list
    games = temp;
}

void DbDialog::OnListColClick( int compare_col )
{
    if( games.size() > 0 )
    {
        this->compare_col = compare_col;
        backdoor = this;
        if( compare_col==1 || compare_col==3 )
            std::sort( games.begin(), games.end(), compare );
        else
            SmartCompare();
        nbr_games_in_list_ctrl = games.size();
        list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
        list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
        list_ctrl->SetItemState(0, wxLIST_STATE_SELECTED+wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED+wxLIST_STATE_FOCUSED);
        int top = list_ctrl->GetTopItem();
        int count = 1 + list_ctrl->GetCountPerPage();
        if( count > nbr_games_in_list_ctrl )
            count = nbr_games_in_list_ctrl;
        for( int i=0; i<count; i++ )
            list_ctrl->RefreshItem(top++);
        Goto(0);
    }
}

void DbDialog::OnSaveAllToAFile()
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


void DbDialog::OnCancel()
{
    gc->PrepareForResumePreviousWindow( list_ctrl->GetTopItem() );
    int sz=gc->gds.size();
    for( int i=0; i<sz; i++ )
    {
        GameDocument *ptr = gc->gds[i]->GetGameDocumentPtr();
        if( ptr )
        {
            ptr->selected =  ( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED)) ? true : false;
            ptr->focus    =  ( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED)  ) ? true : false;
        }
    }
}

void DbDialog::OnHelpClick()
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

class AutoTimer
{
public:
    void Begin();
    double End();
    AutoTimer( const char *desc )
    {
        this->desc = desc;
        Begin();
    }
    ~AutoTimer()
    {
        double elapsed = End();;
        cprintf( "%s: time elapsed (ms) = %f\n", desc, elapsed );
    }
private:
#ifdef THC_WINDOWS
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
#endif
#ifdef THC_MAC
    timeval t1, t2;
#endif
    const char *desc;
};


void AutoTimer::Begin()
{
#ifdef THC_WINDOWS
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    
    // start timer
    QueryPerformanceCounter(&t1);
#endif
#ifdef THC_MAC
    // start timer
    gettimeofday(&t1, NULL);
#endif
}

double AutoTimer::End()
{
    double elapsed_time;
#ifdef THC_WINDOWS
    // stop timer
    QueryPerformanceCounter(&t2);

    // compute the elapsed time in millisec
    elapsed_time = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
#endif
#ifdef THC_MAC
    // stop timer
    gettimeofday(&t2, NULL);
    
    // compute the elapsed time in millisec
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
#endif
    return elapsed_time;
}

void DbDialog::OnUtility()
{
    LoadGamesIntoMemory();
    StatsCalculate();
}

void DbDialog::OnButton1()
{
    objs.gl->gc_clipboard.gds.clear();
    Goto( track->focus_idx );
}

void DbDialog::OnButton2()
{
}

void DbDialog::OnButton3()
{
    std::string player_name = track->info.white;
    objs.db->LoadGamesWithQuery( player_name, true, objs.gl->gc_clipboard.gds );
    Goto( track->focus_idx );
}

void DbDialog::OnButton4()
{
    std::string player_name = track->info.white;
    objs.db->LoadGamesWithQuery( player_name, false, objs.gl->gc_clipboard.gds );
    Goto( track->focus_idx );
}

void DbDialog::OnCheckBox( bool checked )
{
    // Clear the base position
    thc::ChessRules cr;
    this->cr = cr;
    moves_from_base_position.clear();
        
    // No need to look for more games in database in base position
    clipboard_db = checked;
        
    if( clipboard_db || games.size()>0 )
        StatsCalculate();
    else
    {
        nbr_games_in_list_ctrl = orig_nbr_games_in_list_ctrl;
        dirty = true;
        list_ctrl->SetItemCount(nbr_games_in_list_ctrl);
        list_ctrl->RefreshItems( 0, nbr_games_in_list_ctrl-1 );
    }
    Goto(0);
}


void DbDialog::LoadGamesIntoMemory()
{
    AutoTimer at("Load games into memory");
    
    // Load all the matching games from the database
    objs.db->LoadAllGames( cache, nbr_games_in_list_ctrl );
    moves_from_base_position.clear();
    
    // No need to look for more games in database in base position
    drill_down_set.clear();
    uint64_t base_hash = this->cr.Hash64Calculate();
    drill_down_set.insert(base_hash);
    games_set.clear();
    for( unsigned int i=0; i<cache.size(); i++ )
    {
        DB_GAME_INFO *info = cache[i]->GetCompactGamePtr();
        if( info )
            games_set.insert( info->game_id );
    }
}

void DbDialog::StatsCalculate()
{
    int total_white_wins = 0;
    int total_black_wins = 0;
    int total_draws = 0;
    transpositions.clear();
    stats.clear();
    dirty = true;
    games.clear();
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
    objs.db->gbl_hash = gbl_hash;
    objs.db->gbl_position = cr_to_match;
    int maxlen = 1000000;   // absurdly large until a match found

    // Only if we haven't seen this position, look for and load extra games (due to transposition) from database
    if( clipboard_db || drill_down_set.count(gbl_hash) > 0 )
    {
        cprintf( "Already seen this position\n" );
    }
    else
    {
        drill_down_set.insert(gbl_hash);
        objs.db->LoadGamesWithQuery( gbl_hash, cache, games_set );
    }
    
    // For each cached game
    std::vector< smart_ptr<MagicBase> > &source = (clipboard_db ? objs.gl->gc_clipboard.gds : cache );
    for( unsigned int i=0; i<source.size(); i++ )
    {
        DB_GAME_INFO *info = source[i]->GetCompactGamePtr();
        if( info )
        {
    
            // Search for a match to this game
            bool new_transposition_found=false;
            bool found=false;
            int found_idx=0;
            for( unsigned int j=0; !found && j<transpositions.size(); j++ )
            {
                std::string &this_one = transpositions[j].blob;
                const char *p = this_one.c_str();
                size_t len = this_one.length();
                if( info->str_blob.length()>=len && 0 == memcmp(p,info->str_blob.c_str(),len) )
                {
                    found = true;
                    found_idx = j;
                }
            }
        
            // If none so far add the one from this game
            if( !found )
            {
                PATH_TO_POSITION ptp;
                size_t len = info->str_blob.length();
                const char *blob = (const char*)info->str_blob.c_str();
                uint64_t hash = ptp.press.cr.Hash64Calculate();
                int nbr=0;
                found = (hash==gbl_hash && ptp.press.cr==cr_to_match );
                while( !found && nbr<len && nbr<maxlen )
                {
                    thc::ChessRules cr_hash = ptp.press.cr;
                    thc::Move mv;
                    int nbr_used = ptp.press.decompress_move( blob, mv );
                    if( nbr_used == 0 )
                        break;
                    blob += nbr_used;
                    nbr += nbr_used;
                    hash = cr_hash.Hash64Update( hash, mv );
                    if( hash == gbl_hash && ptp.press.cr==cr_to_match )
                        found = true;
                }
                if( found )
                {
                    maxlen = nbr+8;
                    new_transposition_found = true;
                    ptp.blob =info->str_blob.substr(0,nbr);
                    transpositions.push_back(ptp);
                    found_idx = transpositions.size()-1;
                }
            }

            if( found )
            {
                bool white_wins = (info->result=="1-0");
                if( white_wins )
                    total_white_wins++;
                bool black_wins = (info->result=="0-1");
                if( black_wins )
                    total_black_wins++;
                bool draw       = (info->result=="1/2-1/2");
                if( draw )
                    total_draws++;

                games.push_back(*info);
                PATH_TO_POSITION *p = &transpositions[found_idx];
                p->frequency++;
                size_t len = p->blob.length();
                if( len < info->str_blob.length() ) // must be more moves
                {
                    const char *compress_move_ptr = info->str_blob.c_str()+len;
                    thc::Move mv;
                    p->press.decompress_move_stay( compress_move_ptr, mv );
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
            }
        
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
        const char *blob = p->blob.c_str();
        size_t len = p->blob.length();
        std::string txt;
        CompressMoves press;
        int nbr = 0;
        int count = 0;
        while( nbr < len )
        {
            thc::ChessRules cr_before = press.cr;
            thc::Move mv;
            int nbr_used = press.decompress_move( blob, mv );
            if( nbr_used == 0 )
                break;
            if( count%2 == 0 )
            {
                char buf[100];
                sprintf( buf, "%d.", count/2+1 );
                txt += buf;
            }
            txt += mv.NaturalOut(&cr_before);
            txt += " ";
            blob += nbr_used;
            nbr += nbr_used;
            count++;
        }
        char buf[200];
        sprintf( buf, "T%d: %s: %d occurences", j+1, txt.c_str(), p->frequency );
        cprintf( "%s\n", buf );
        wxString wstr(buf);
        strings_transpos.Add(wstr);
    }

    nbr_games_in_list_ctrl = games.size();
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
void DbDialog::OnNextMove( int idx )
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




