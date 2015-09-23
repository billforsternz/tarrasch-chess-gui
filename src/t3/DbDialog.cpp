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


wxSizer *DbDialog::AddExtraControls()
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
 
    
    wxStaticText* spacer1 = new wxStaticText( this, wxID_ANY, wxT(""),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(spacer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    white_player_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX2,
                                 wxT("&White player"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(white_player_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    white_player_ctrl->SetValue( true );

    wxButton* btn1 = new wxButton ( this, ID_BUTTON_1, wxT("Clear Clipboard"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* btn2 = new wxButton ( this, ID_BUTTON_2, wxT("Add to Clipboard"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    wxButton* btn3 = new wxButton ( this, ID_BUTTON_3, wxT("Add All Player's White Games"),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* spacer2 = new wxStaticText( this, wxID_ANY, wxT(""),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(spacer2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    wxButton* btn4 = new wxButton ( this, ID_BUTTON_4, wxT("Add All Player's Black Games"),
                                   wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(btn4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* spacer3 = new wxStaticText( this, wxID_ANY, wxT(""),
                                     wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(spacer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    filter_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX,
                                 wxT("&Clipboard as temp database"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(filter_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    filter_ctrl->SetValue( objs.gl->db_clipboard );
    
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
        if( objs.gl->db_clipboard )
            StatsCalculate();        
        Goto(0); // list_ctrl->SetFocus();
    }
}



// Read game information from games or database
void DbDialog::ReadItem( int item, CompactGame &pact )
{
    DB_GAME_INFO info;
    bool in_memory = ReadItemFromMemory( item, info );
    if( !in_memory )
    {
        objs.db->GetRow( &info, item );
        info.transpo_nbr = 0;
    }
    info.GetCompactGame( pact );
}

bool DbDialog::ReadItemFromMemory( int item, DB_GAME_INFO &info )
{
    bool in_memory = false;
    info.transpo_nbr = 0;
    int nbr_games = displayed_games.size();
    if( 0<=item && item<nbr_games )
    {
        in_memory = true;
        info = *displayed_games[nbr_games-1-item]->GetDbGameInfoPtr();
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
    white_player_search = true;
}


static DbDialog *backdoor;
static bool compare( const smart_ptr<DB_GAME_INFO> g1, const smart_ptr<DB_GAME_INFO> g2 )
{
    bool lt=false;
    switch( backdoor->compare_col )
    {
        case 1: lt = g1->r.white < g2->r.white;
                break;
        case 2:
        {       
                int elo_1 = atoi( g1->r.white_elo.c_str() );
                int elo_2 = atoi( g2->r.white_elo.c_str() );
                lt = elo_1 < elo_2;
                break;
        }
        case 3: lt = g1->r.black < g2->r.black;
                break;
        case 4:
        {
                int elo_1 = atoi( g1->r.black_elo.c_str() );
                int elo_2 = atoi( g2->r.black_elo.c_str() );
                lt = elo_1 < elo_2;
                break;
        }
        case 5: lt = g1->r.date < g2->r.date;
                break;
        case 6: lt = g1->r.site < g2->r.site;
                break;
        case 8: lt = g1->r.result < g2->r.result;
                break;
    }
    return lt;
}
        

static bool rev_compare( const smart_ptr<DB_GAME_INFO> g1, const smart_ptr<DB_GAME_INFO> g2 )
{
    bool lt=true;
    switch( backdoor->compare_col )
    {
        case 1: lt = g1->r.white > g2->r.white;           break;
        case 2:
        {       int elo_1 = atoi( g1->r.white_elo.c_str() );
                int elo_2 = atoi( g2->r.white_elo.c_str() );
                lt = elo_1 > elo_2;
                break;
        }
        case 3: lt = g1->r.black > g2->r.black;           break;
        case 4:
        {       int elo_1 = atoi( g1->r.black_elo.c_str() );
                int elo_2 = atoi( g2->r.black_elo.c_str() );
                lt = elo_1 > elo_2;
                break;
        }
        case 5: lt = g1->r.date > g2->r.date;             break;
        case 6: lt = g1->r.site > g2->r.site;             break;
        case 8: lt = g1->r.result > g2->r.result;         break;
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

void DbDialog::SmartCompare()
{
    std::vector<TempElement> inter;     // intermediate representation
    
    // Step 1, do a conventional string sort, beginning at our offset into the blob
    unsigned int sz = displayed_games.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        DB_GAME_INFO &g = *displayed_games[i]->GetDbGameInfoPtr();
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
    std::vector< smart_ptr<DB_GAME_INFO> > temp;
    sz = inter.size();
    for( unsigned int i=0; i<sz; i++ )
    {
        TempElement &e = inter[i];
        temp.push_back( displayed_games[e.idx] );
    }

    // Step 5 replace original games list
    displayed_games = temp;
}

void DbDialog::OnListColClick( int compare_col )
{
    if( displayed_games.size() > 0 )
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
            SmartCompare();
        else
            std::sort( displayed_games.begin(), displayed_games.end(), (consecutive%2==0)?rev_compare:compare );
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
        last_time = compare_col;
    }
}

void DbDialog::OnSearch()
{
    wxString name = text_ctrl->GetValue();
    std::string sname(name.c_str());
    thc::ChessPosition start_cp;
    
    // Temp - do a "find on page type feature"
    if( sname.length()>0 && cr==start_cp )
    {
        std::string current = white_player_search ? track->info.r.white : track->info.r.black;
        int row = objs.db->FindPlayer( sname, current, track->focus_idx, white_player_search );
        Goto(row); /*
        list_ctrl->EnsureVisible( row );   // get vaguely close
        list_ctrl->SetItemState( row, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
        list_ctrl->SetItemState( row, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
        list_ctrl->SetFocus( ); */
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
        if( nbr_games_in_list_ctrl > 0 )
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
 /* for( int i=0; i<sz; i++ )
    {
        smart_ptr<MagicBase> smp = gc->gds[i];
        bool selected = (wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED)) ? true : false;
        smp->SetSelected(selected);
        bool focused  =  (wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED)  ) ? true : false;
        smp->SetFocused(focused);
    } */
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
    CopyOrAdd( false );
}

void DbDialog::OnButton3()
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

void DbDialog::OnButton4()
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

void DbDialog::OnCheckBox2( bool checked )
{
    white_player_search = checked;
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

void DbDialog::OnCheckBox( bool checked )
{
    // Clear the base position
    thc::ChessRules cr;
    this->cr = cr;
    moves_from_base_position.clear();
        
    // No need to look for more games in database in base position
    objs.gl->db_clipboard = checked;
        
    if( objs.gl->db_clipboard || displayed_games.size()>0 )
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
    objs.db->LoadAllGames( gc->gds, nbr_games_in_list_ctrl );
    moves_from_base_position.clear();
    
    // No need to look for more games in database in base position
    drill_down_set.clear();
    uint64_t base_hash = this->cr.Hash64Calculate();
    drill_down_set.insert(base_hash);
    games_set.clear();
    for( unsigned int i=0; i<gc->gds.size(); i++ )
    {
        DB_GAME_INFO *p = gc->gds[i]->GetDbGameInfoPtr();
        if( p )
            games_set.insert( p->game_id );
    }
}

void DbDialog::CopyOrAdd( bool clear_clipboard )
{
    int idx_focus = -1;
    int nbr_copied = 0;
    if( list_ctrl )
    {
        int sz=displayed_games.size();
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
                gc_clipboard->gds.push_back( displayed_games[sz-1-i] ); // assumes smart_ptr is std::shared_ptr
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
            gc_clipboard->gds.push_back( displayed_games[sz-1-idx_focus] ); // assumes smart_ptr is std::shared_ptr
            nbr_copied++;
        }
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
    displayed_games.clear();
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
    objs.db->gbl_hash = gbl_hash;
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
        objs.db->LoadGamesWithQuery( gbl_hash, gc->gds, games_set );
    }
    
    // For each cached game
    std::vector< smart_ptr<MagicBase> > &source = (objs.gl->db_clipboard ? objs.gl->gc_clipboard.gds : gc->gds );
    for( unsigned int i=0; i<source.size(); i++ )
    {
        Roster r         = source[i]->RefRoster();
        std::string blob = source[i]->RefCompressedMoves();
        if( blob.size() > 0 )
        {
    
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
                DB_GAME_INFO temp;
                temp.r = r;
                temp.str_blob = blob;
                make_smart_ptr( DB_GAME_INFO, smptr, temp );
                displayed_games.push_back(smptr);
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
        char buf[200];
        sprintf( buf, "T%d: %s: %d occurences", j+1, txt.c_str(), p->frequency );
        cprintf( "%s\n", buf );
        wxString wstr(buf);
        strings_transpos.Add(wstr);
    }

    nbr_games_in_list_ctrl = displayed_games.size();
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

