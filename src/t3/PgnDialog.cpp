/****************************************************************************
 * Custom dialog - Pgn browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/listctrl.h"
#include "Portability.h"
#include "Appdefs.h"
#include "DebugPrintf.h"
#include "thc.h"
#include "GameDetailsDialog.h"
#include "GamePrefixDialog.h"
#include "GameLogic.h"
#include "Objects.h"
#include "PgnFiles.h"
#include "PgnDialog.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

// PgnDialog type definition
IMPLEMENT_CLASS( PgnDialog, wxDialog )

// PgnDialog event table definition
BEGIN_EVENT_TABLE( PgnDialog, wxDialog )
//  EVT_CLOSE( PgnDialog::OnClose )
    EVT_BUTTON( wxID_OK,                PgnDialog::OnOkClick )
    EVT_BUTTON( wxID_CANCEL,            PgnDialog::OnCancel )
    EVT_BUTTON( ID_BOARD2GAME,          PgnDialog::OnBoard2Game )
    EVT_CHECKBOX( ID_REORDER,           PgnDialog::OnRenumber )
    EVT_BUTTON( ID_ADD_TO_CLIPBOARD,    PgnDialog::OnAddToClipboard )
    EVT_BUTTON( ID_SAVE_ALL_TO_A_FILE,  PgnDialog::OnSaveAllToAFile )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_DETAILS,   PgnDialog::OnEditGameDetails )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_PREFIX,    PgnDialog::OnEditGamePrefix )
    EVT_BUTTON( ID_PGN_DIALOG_PUBLISH,  PgnDialog::OnPublish )
    EVT_BUTTON( wxID_COPY,              PgnDialog::OnCopy )
    EVT_BUTTON( wxID_CUT,               PgnDialog::OnCut )
    EVT_BUTTON( wxID_DELETE,            PgnDialog::OnDelete )
    EVT_BUTTON( wxID_PASTE,             PgnDialog::OnPaste )
    EVT_BUTTON( wxID_SAVE,              PgnDialog::OnSave )
    EVT_BUTTON( wxID_HELP,              PgnDialog::OnHelpClick )
    EVT_MENU( wxID_SELECTALL, PgnDialog::OnSelectAll )
    EVT_LIST_ITEM_ACTIVATED(ID_PGN_LISTBOX, PgnDialog::OnListSelected)
    EVT_LIST_COL_CLICK(ID_PGN_LISTBOX, PgnDialog::OnListColClick)
END_EVENT_TABLE()

// PgnDialog constructors
PgnDialog::PgnDialog
(
    wxWindow* parent,
    GamesCache  *gc,
    GamesCache  *gc_clipboard,
    wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style
)
{
    this->gc = gc;
    this->gc_clipboard = gc_clipboard;
    this->id = id;
    file_game_idx = -1;
    Init();
    Create( parent, id, gc->pgn_filename, pos, size, style );
}

// Pre window creation initialisation
void PgnDialog::Init()
{
    list_ctrl = NULL;
    selected_game = NULL;
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
bool PgnDialog::Create( wxWindow* parent,
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

static smart_ptr<GameDocument> *ptr_gds;
static int          *ptr_col_flags;
int wxCALLBACK sort_callback( long item1, long item2, long col )
{
    string s1;
    string s2;
    int i1=0;
    int i2=0;
    int f1=0;
    int f2=0;
    bool iflag=false;
    bool fflag=false;
    switch( col )
    {
        case 0: iflag = true;
                i1=ptr_gds[item1]->game_nbr;
                i2=ptr_gds[item2]->game_nbr;     break;
        case 1: s1=ptr_gds[item1]->white;
                s2=ptr_gds[item2]->white;        break;
        case 2: iflag = true;
                i1=atoi(ptr_gds[item1]->white_elo.c_str());
                i2=atoi(ptr_gds[item2]->white_elo.c_str());
                                                break;
        case 3: s1=ptr_gds[item1]->black;
                s2=ptr_gds[item2]->black;        break;
        case 4: iflag = true;
                i1=atoi(ptr_gds[item1]->black_elo.c_str());
                i2=atoi(ptr_gds[item2]->black_elo.c_str());
                                                break;
        case 5: s1=ptr_gds[item1]->date;
                s2=ptr_gds[item2]->date;         break;
        case 6: s1=ptr_gds[item1]->site;
                s2=ptr_gds[item2]->site;         break;
//      case 7: fflag = true;
//              f1=atof(ptr_gds[item1].round.c_str());
//              f2=atof(ptr_gds[item2].round.c_str());
//                                              break;
        case 7: s1=ptr_gds[item1]->round;
                s2=ptr_gds[item2]->round;        break;
        case 8: s1=ptr_gds[item1]->result;
                s2=ptr_gds[item2]->result;       break;
        case 9: s1=ptr_gds[item1]->eco;
                s2=ptr_gds[item2]->eco;          break;
        case 10:s1=ptr_gds[item1]->prefix_txt.length() ? ptr_gds[item1]->prefix_txt : ptr_gds[item1]->moves_txt;
                s2=ptr_gds[item2]->prefix_txt.length() ? ptr_gds[item2]->prefix_txt : ptr_gds[item2]->moves_txt;  break;
        case 11:iflag = true;
                i1=ptr_gds[item1]->sort_idx;
                i2=ptr_gds[item2]->sort_idx;     break;
    }
    int dir=0;
    if( ptr_col_flags )
        dir = *ptr_col_flags;
    int ret=0;
    if( iflag )
        ret = (dir ? i1<i2 : i2<i1);
    else if( fflag )
        ret = (dir ? f1<f2 : f2<f1);
    else
        ret = (dir ? s1<s2 : s2<s1);
    if( col == 1 )
        cprintf( "sort_callback; s1=%s, s2=%s, result=%s\n", s1.c_str(),  s2.c_str(), ret?"true":"false" );
    return ret;
}

// Control creation for PgnDialog
void PgnDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    const char *list_txt = "List of games in the current file";
    if( id == ID_PGN_DIALOG_CLIPBOARD )
        list_txt = "List of games currently in the clipboard";
    else if( id == ID_PGN_DIALOG_SESSION )
        list_txt = "List of games from this session (use File - Open log file to see older games)";
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        list_txt, wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    int disp_width, disp_height;
    wxDisplaySize(&disp_width, &disp_height);
    wxSize sz;
    if( disp_width > 1366 )
        disp_width = 1366;
    sz.x = (disp_width*4)/5;
    sz.y = (disp_height*1)/2;
    list_ctrl  = new wxListCtrl( this, ID_PGN_LISTBOX, wxDefaultPosition, sz/*wxDefaultSize*/,wxLC_REPORT );
    int gds_nbr = gc->gds.size();
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
    int top_item;
    bool resuming = gc->IsResumingPreviousWindow(top_item);
    for( int i=0; i<gds_nbr; i++ )    
    {                                 
        wxListItem item;              
        list_ctrl->InsertItem( i, item );
        list_ctrl->SetItemData( i, i );
        char buf[20];
        int game_nbr;
        if( resuming )
        {
            if( gc->renumber )
                game_nbr = i+1;
            else
                game_nbr = gc->gds[i]->game_nbr;
        }
        else 
        {
            game_nbr = i+1;
            gc->gds[i]->game_nbr = game_nbr;
        }
        buf[0] = '\0';
        if( id != ID_PGN_DIALOG_FILE )
        {
            //if( modified )
            //    strcpy( buf, "*" );
        }
        else
        {
            GameDocument *pd = objs.tabs->Begin();
            Undo *pu = objs.tabs->BeginUndo();
            while( pd && pu )
            {
                bool modified = gc->gds[i]->modified;
                if( gc->gds[i]->game_being_edited == pd->game_being_edited )
                    modified = modified || pu->IsModified();
                if( modified )
                    strcpy( buf,"*" );
                if( game_nbr )
                {
    #ifdef WINDOWS_FIX_LATER
                    itoa( game_nbr, buf, 10 );
    #else
                    sprintf( buf, "%d", game_nbr );
    #endif
                    if( modified )
                        strcat( buf, " *" );
                }
                pd = objs.tabs->Next();
                pu = objs.tabs->NextUndo();
            }
        }
        list_ctrl->SetItem( i, 0, buf );
        list_ctrl->SetItem( i, 1, gc->gds[i]->white );
        list_ctrl->SetItem( i, 2, gc->gds[i]->white_elo );
        list_ctrl->SetItem( i, 3, gc->gds[i]->black );
        list_ctrl->SetItem( i, 4, gc->gds[i]->black_elo );
        list_ctrl->SetItem( i, 5, gc->gds[i]->date );
        list_ctrl->SetItem( i, 6, gc->gds[i]->site );
        list_ctrl->SetItem( i, 7, gc->gds[i]->round );
        list_ctrl->SetItem( i, 8, gc->gds[i]->result=="*"?"":gc->gds[i]->result );
        list_ctrl->SetItem( i, 9, gc->gds[i]->eco );
        std::string s = CalculateMovesColumn(*gc->gds[i]);
        list_ctrl->SetItem( i, 10, s );
    }
    bool selections_made = false;
    if( resuming )
    {
        ptr_gds = &gc->gds[0];
        ptr_col_flags = NULL;
        list_ctrl->SortItems(sort_callback,11); // sort_idx
        int sz=gc->gds.size();
        bool focus_found = false;
        int focus_idx;
        for( int i=0; !focus_found && i<sz; i++ )
        {
            if( gc->gds[i]->focus )
            {
                list_ctrl->SetItemState( i, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
                focus_found = true;
                focus_idx = i;
            }
        }
        for( int i=0; i<sz; i++ )
        {
            if( gc->gds[i]->selected )
            {
                selections_made = true;
                list_ctrl->SetItemState( i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
                if( !focus_found )
                {
                    list_ctrl->SetItemState( i, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
                    focus_found = true;
                    focus_idx = i;
                }
            }
        }

        // This is an ugly way of performing list_ctrl->SetTopItem() (which for some reason isn't
        //  provided by wxWidgets)
        list_ctrl->EnsureVisible( top_item );   // get vaguely close
        int temp_top_item = list_ctrl->GetTopItem();
        bool error=false;
        while( !error && temp_top_item!=top_item )   // loop until we make it
        {
            int dir = (temp_top_item > top_item ? -1 : 1);   // scroll up or down

            // scroll 1,2.4,8... pixels until we move at least one game
            for( int step=1;; step++ )
            {
                list_ctrl->ScrollList( 0, dir*step );
                int top_item2 = list_ctrl->GetTopItem();
                if( temp_top_item != top_item2 )
                    break;
                if( step == 100 )
                {
                    error = true;
                    break;
                } 
            }
            temp_top_item = list_ctrl->GetTopItem();
        }
    }
    if( !selections_made )
    {
        if( gds_nbr )
        {
            int idx = 0;
            bool current_found = false;
            for( int i=0; i<gds_nbr; i++ )
            {
                if( gc->gds[i]->game_being_edited == objs.gl->gd.game_being_edited )
                {
                    idx = i;
                    current_found = true;
                    break;
                }
            }
            list_ctrl->EnsureVisible( idx );
            list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
            list_ctrl->SetItemState( idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            if( current_found )
            {
                for( int i=0; i<gds_nbr; i++ )
                {
                    if( i != idx )
                        list_ctrl->SetItemState( i, wxLIST_STATE_FOCUSED, 0 );
                }
            }
        }
    }
    box_sizer->Add(list_ctrl, 0, wxGROW|wxALL, 5);

    // A dividing line before the buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A pair of horizontal box sizers to contain buttons
    wxBoxSizer* button_box1 = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(button_box1, 0, wxALIGN_LEFT|wxALL, 10);
    wxBoxSizer* button_box2 = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(button_box2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    // Load / Ok / Game->Board
    wxButton* ok = new wxButton ( this, wxID_OK, wxT("Load"),
        wxDefaultPosition, wxDefaultSize, 0 );
    button_box1->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Edit game details
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* edit_game_details = new wxButton ( this, ID_PGN_DIALOG_GAME_DETAILS, wxT("Edit Game Details"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(edit_game_details, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Edit game prefix
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* edit_game_prefix = new wxButton ( this, ID_PGN_DIALOG_GAME_PREFIX, wxT("Edit Game Prefix"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(edit_game_prefix, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Paste game / Board->Game
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD )
    {
        wxButton* board2game = new wxButton ( this, ID_BOARD2GAME, wxT("Paste current game"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(board2game, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Delete
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD )
    {
        wxButton* delete_ = new wxButton ( this, wxID_DELETE, wxT("Delete"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(delete_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Cut
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* cut = new wxButton ( this, wxID_CUT, wxT("Cut"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(cut, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Add to clipboard
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* add = new wxButton ( this, ID_ADD_TO_CLIPBOARD, wxT("Add to clipboard"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Copy
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* copy = new wxButton ( this, wxID_COPY, wxT("Copy"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(copy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Paste
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* paste = new wxButton ( this, wxID_PASTE, wxT("Paste"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(paste, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Save all games to a file
    if( id==ID_PGN_DIALOG_CLIPBOARD||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* save_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save all to a file"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box1->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Renumber
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxCheckBox* reorder = new wxCheckBox ( this, ID_REORDER, wxT("Renumber"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box2->Add(reorder, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Write to file
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* write_to_file = new wxButton ( this, wxID_SAVE, wxT("Save file"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box2->Add(write_to_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Write to file
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* publish = new wxButton ( this, ID_PGN_DIALOG_PUBLISH, wxT("Publish"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box2->Add(publish, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // The Cancel button
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* cancel = new wxButton ( this, wxID_CANCEL,
            wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
        button_box2->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // The Help button
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* help = new wxButton( this, wxID_HELP, wxT("Help"),
            wxDefaultPosition, wxDefaultSize, 0 );
        button_box2->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);    
    }
}


std::string PgnDialog::CalculateMovesColumn( GameDocument &gd )
{
    std::string sp = gd.prefix_txt;
    std::string sm = gd.moves_txt;
    std::string s  = sm;
    int len = sm.length();
    if( len>=1 && sm[len-1] == '*' )
    {
        sm = sm.substr(0,len-1);
        s = sm;
    }
    len = sp.length();
    int start_idx = 0;
    int end_idx = 0;
    for( ; start_idx<len; start_idx++ )
    {
        if( sp[start_idx]!='\n' && sp[start_idx]!='\r' )
            break;
    }
    len -= start_idx;
    end_idx = start_idx;
    for( ; end_idx<len; end_idx++ )
    {
        if( sp[end_idx]=='\n' || sp[end_idx]=='\r' )
            break;
    }
    len = end_idx-start_idx;
    if( len > 0 )
    {
        bool truncate=false;
        if( len > 12 )
        {
            truncate=true;
            len = 12;
        }
        s = "(" + sp.substr(start_idx,start_idx+len-1) + (truncate?"...)":")") + sm;
    }
    return s;
}

// Set the validators for the dialog controls
void PgnDialog::SetDialogValidators()
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
void PgnDialog::SetDialogHelp()
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

void PgnDialog::OnListSelected( wxListEvent &event )
{
    DebugPrintf(( "idx=%d\n", event.m_itemIndex ));
    if( list_ctrl )
        list_ctrl->SetItemState( event.m_itemIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
    OnOk();
}

void PgnDialog::OnListColClick( wxListEvent &event )
{
    int col = event.GetColumn();
    ptr_gds = &gc->gds[0];
    ptr_col_flags = &gc->col_flags[col];
    gc->Debug( "Before sort" );
    SyncCacheOrderBefore();
    list_ctrl->SortItems(sort_callback,col);
    SyncCacheOrderAfter();
    gc->Debug( "After sort" );
    int idx=0;
    for( int i=0; i<gc->gds.size(); i++ )
    {
        if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
        {
            idx = i;
            break;
        }
        else if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
        {
            idx = i;
            break;
        }
    }
    for( int i=0; i<gc->gds.size(); i++ )
    {
        if( gc->gds[i]->game_being_edited==objs.gl->gd.game_being_edited && gc==&objs.gl->gc )
            objs.gl->file_game_idx = i;
    }
    list_ctrl->EnsureVisible( idx );
    *ptr_col_flags = !*ptr_col_flags;
}

// We keep the game cache array of game documents synced to the
//  list box presentation of those games. Before changing the
//  order of the list box items call this ...
void PgnDialog::SyncCacheOrderBefore()
{
    int gds_nbr = gc->gds.size();
    for( int i=0; i<gds_nbr; i++ )    
        list_ctrl->SetItemData( i, i );
}

// ... Afterward call this to reestablish sync
void PgnDialog::SyncCacheOrderAfter()
{
    int gds_nbr = gc->gds.size();
    for( int i=0; i<gds_nbr; i++ )    
    {
        int idx = list_ctrl->GetItemData(i);
        gc->gds[idx]->sort_idx = i;
    }
    sort( gc->gds.begin(), gc->gds.end() );
    for( int i=0; i<gds_nbr; i++ )    
        list_ctrl->SetItemData( i, i );
}

GameDocument *PgnDialog::GetFocusGame( int &idx )
{
    GameDocument *gd=NULL;
    if( list_ctrl )
    {
        int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
            {
                idx = i;
                gd = gc->gds[idx].get();
                break;
            }
        }

        // If no item currently in focus, take the top item in window
        if( gd==NULL && sz>0 )
        {
            idx = list_ctrl->GetTopItem();
            if( idx >= sz )
                idx = 0;
            list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
            gd = gc->gds[idx].get();
        }
    }
    return gd;
}

void PgnDialog::DeselectOthers( GameDocument *selected_game )
{
    GameDocument *gd=NULL;
    if( list_ctrl && selected_game )
    {
        int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            gd = gc->gds[i].get();
            if( gd != selected_game )
            {
                if( gd->selected || (wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED)) )
                {
                    gd->selected = false;
                    list_ctrl->SetItemState( i, 0, wxLIST_STATE_SELECTED );
                }
            }
        }
    }
    gc->Debug( "After DeselectOthers()" );
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void PgnDialog::OnOkClick( wxCommandEvent& WXUNUSED(event) )
{
    OnOk();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void PgnDialog::OnOk()
{
    selected_game = NULL;
    if( list_ctrl )
    {
        gc->PrepareResumePreviousWindow( list_ctrl->GetTopItem() );
        int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
                gc->gds[i]->selected = true;
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                gc->gds[i]->focus = true;
            else
                gc->gds[i]->focus = false;
        }
        GameDocument *gd = GetFocusGame(file_game_idx);
        if( gd )
        {
            DeselectOthers(gd);
            if( gd->in_memory )
                selected_game = gd;
            else
            {
                FILE *pgn_in = objs.gl->pf.ReopenRead( gd->pgn_handle );
                if( pgn_in )
                {
                    long fposn2 = gd->fposn2;
                    long end    = gd->fposn3;
                    fseek(pgn_in,fposn2,SEEK_SET);
                    long len = end-fposn2;
                    char *buf = new char [len];
                    if( len == (long)fread(buf,1,len,pgn_in) )
                    {
                        std::string s(buf,len);
                        thc::ChessRules cr;
                        int nbr_converted;
                        gd->PgnParse(true,nbr_converted,s,cr,NULL);
                        selected_game = gd;                    
                    }
                    objs.gl->pf.Close( gc_clipboard );
                    delete[] buf;
                }
            }
        }
    }
    TransferDataToWindow();    
    AcceptAndClose();
}

bool PgnDialog::LoadGame( GameLogic *gl, GameDocument& gd, int &file_game_idx )
{
    if( selected_game )
    {
        gl->IndicateNoCurrentDocument();
        selected_game->game_being_edited = ++objs.gl->game_being_edited_tag;
        gd = *selected_game;
        gd.selected = false;
        selected_game->selected = true;
        if( &gl->gc == gc )
            file_game_idx = this->file_game_idx;    // update this only if loading game from current file
    }
    return selected_game?true:false;
}

void PgnDialog::OnBoard2Game( wxCommandEvent& WXUNUSED(event) )
{
    int idx_focus=0;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                idx_focus = i;
        }
        std::vector< smart_ptr<GameDocument> >::iterator iter = gc->gds.begin() + idx_focus;
        GameDocument gd = objs.gl->gd;
        gd.modified = true;
        GameDetailsDialog dialog( this );
        if( dialog.Run(gd) )
        {
            objs.gl->GameRedisplayPlayersResult();
            gd.game_nbr = 0;
            gd.modified = true;
            gc->file_irrevocably_modified = true;
            make_smart_ptr( GameDocument, new_doc, gd );
            gc->gds.insert( iter, new_doc );
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
    }
}

void PgnDialog::OnRenumber( wxCommandEvent& WXUNUSED(event) )
{
    gc->renumber = !gc->renumber;
    int gds_nbr = gc->gds.size();
    for( int i=0; i<gds_nbr; i++ )    
    {                                 
        int game_nbr = i+1;
        if( !gc->renumber )
            game_nbr = gc->gds[i]->game_nbr;
        char buf[20];
#ifdef WINDOWS_FIX_LATER
        itoa( game_nbr, buf, 10 );
#else
        sprintf( buf, "%d", game_nbr );
#endif
        list_ctrl->SetItem( i, 0, buf );
    }
}

void PgnDialog::OnSelectAll( wxCommandEvent& WXUNUSED(event) )
{
    int gds_nbr = gc->gds.size();
    for( int i=0; i<gds_nbr; i++ )    
        list_ctrl->SetItemState( i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
}

void PgnDialog::OnEditGameDetails( wxCommandEvent& WXUNUSED(event) )
{
    int idx;
    GameDocument *gd = GetFocusGame(idx);
    if( gd )
    {
        GameDetailsDialog dialog( this );
        if( dialog.Run( *gd ) )
        {
            objs.gl->GameRedisplayPlayersResult();
            list_ctrl->SetItem( idx, 1, gc->gds[idx]->white );
            list_ctrl->SetItem( idx, 2, gc->gds[idx]->white_elo );
            list_ctrl->SetItem( idx, 3, gc->gds[idx]->black );
            list_ctrl->SetItem( idx, 4, gc->gds[idx]->black_elo );
            list_ctrl->SetItem( idx, 5, gc->gds[idx]->date );
            list_ctrl->SetItem( idx, 6, gc->gds[idx]->site );
            list_ctrl->SetItem( idx, 7, gc->gds[idx]->round );
            list_ctrl->SetItem( idx, 8, gc->gds[idx]->result );
            list_ctrl->SetItem( idx, 9, gc->gds[idx]->eco );
        }
    }
}

void PgnDialog::OnEditGamePrefix( wxCommandEvent& WXUNUSED(event) )
{
    int idx;
    GameDocument *gd = GetFocusGame(idx);
    if( gd )
    {
        GamePrefixDialog dialog( this );
        if( dialog.Run( *gd ) )
        {
            std::string s = CalculateMovesColumn(*gd);
            list_ctrl->SetItem( idx,10,s);
        }
    }
}

void PgnDialog::OnSaveAllToAFile( wxCommandEvent& WXUNUSED(event) )
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

void PgnDialog::OnAddToClipboard( wxCommandEvent& WXUNUSED(event) )
{
    CopyOrAdd( false );
}

void PgnDialog::OnCopy( wxCommandEvent& WXUNUSED(event) )
{
    CopyOrAdd( true );
}

void PgnDialog::CopyOrAdd( bool clear_clipboard )
{
    int nbr_copied=0, idx_focus=-1;
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
                GameDocument gd = *gc->gds[i];
                make_smart_ptr( GameDocument, new_doc, gd );
                gc_clipboard->gds.push_back(new_doc);
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
            GameDocument gd = *gc->gds[idx_focus];
            make_smart_ptr( GameDocument, new_doc, gd );
            gc_clipboard->gds.push_back(new_doc);
            nbr_copied++;
        }
    }
    DebugPrintf(( "%d games copied\n", nbr_copied ));
}

void PgnDialog::OnCut( wxCommandEvent& WXUNUSED(event) )
{
    bool clear_clipboard = true;
    int nbr_cut=0, idx_focus=-1;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        std::vector< smart_ptr<GameDocument> >::iterator iter = gc->gds.begin();
        std::vector< smart_ptr<GameDocument> >::iterator iter_focus;
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
                GameDocument gd = **iter;
                make_smart_ptr( GameDocument, new_doc, gd );
                gc_clipboard->gds.push_back(new_doc);
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
            GameDocument gd = **iter_focus;
            make_smart_ptr(GameDocument,new_doc,gd);
            gc_clipboard->gds.push_back(new_doc);
            list_ctrl->DeleteItem(idx_focus);
            iter = gc->gds.erase(iter_focus);
            gc->file_irrevocably_modified = true;
            nbr_cut++;
        }
    }
    DebugPrintf(( "%d games cut\n", nbr_cut ));
}

void PgnDialog::OnDelete( wxCommandEvent& WXUNUSED(event) )
{
    int nbr_deleted=0, idx_focus=-1;
    int sz=gc->gds.size();
    if( list_ctrl && list_ctrl->GetItemCount()==sz )
    {
        std::vector< smart_ptr<GameDocument> >::iterator iter = gc->gds.begin();
        std::vector< smart_ptr<GameDocument> >::iterator iter_focus;
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
    DebugPrintf(( "%d games deleted\n", nbr_deleted ));
}

void PgnDialog::OnPaste( wxCommandEvent& WXUNUSED(event) )
{
    int idx_focus=0;
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
            std::vector< smart_ptr<GameDocument> >::iterator iter = gc->gds.begin() + idx_focus;
            GameDocument gd = *gc_clipboard->gds[i];
            gd.game_nbr = 0;
            gd.modified = true;
            make_smart_ptr(GameDocument,new_doc,gd);
            gc->gds.insert( iter, new_doc );
            gc->file_irrevocably_modified = true;
            wxListItem item;              
            list_ctrl->InsertItem( idx_focus, item );
            list_ctrl->SetItem( idx_focus, 0, "" );                     // game_nbr
            list_ctrl->SetItem( idx_focus, 1, gc_clipboard->gds[i]->white );
            list_ctrl->SetItem( idx_focus, 2, gc_clipboard->gds[i]->white_elo );
            list_ctrl->SetItem( idx_focus, 3, gc_clipboard->gds[i]->black );
            list_ctrl->SetItem( idx_focus, 4, gc_clipboard->gds[i]->black_elo );
            list_ctrl->SetItem( idx_focus, 5, gc_clipboard->gds[i]->date );
            list_ctrl->SetItem( idx_focus, 6, gc_clipboard->gds[i]->site );
            list_ctrl->SetItem( idx_focus, 7, gc_clipboard->gds[i]->round );
            list_ctrl->SetItem( idx_focus, 8, gc_clipboard->gds[i]->result );
            list_ctrl->SetItem( idx_focus, 9, gc_clipboard->gds[i]->eco );
            list_ctrl->SetItem( idx_focus,10, gc_clipboard->gds[i]->moves_txt );
        }
    }
}

void PgnDialog::OnSave( wxCommandEvent& WXUNUSED(event) )
{
    gc->FileSave( gc_clipboard );
}

void PgnDialog::OnPublish( wxCommandEvent& WXUNUSED(event) )
{
    gc->Publish( gc_clipboard );
}

void PgnDialog::OnCancel( wxCommandEvent& WXUNUSED(event) )
{
    if( list_ctrl )
    {
        gc->PrepareResumePreviousWindow( list_ctrl->GetTopItem() );
        int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            #if 0
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
                gc->gds[i]->selected = true;
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                gc->gds[i]->focus = true;
            #else
            gc->gds[i]->selected =  ( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED)) ? true : false;
            gc->gds[i]->focus    =  ( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED)  ) ? true : false;
            #endif
        }
    }
    gc->Debug( "After on cancel" );
    EndDialog( wxID_CANCEL );
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void PgnDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to load games from a .pgn file\n")
      wxT("and save games to a .pgn file\n\n");
    if( id == ID_PGN_DIALOG_CLIPBOARD )
    {
        helpText =
          wxT("Use this panel to view and load games saved\n")
          wxT("in the game clipboard\n\n");
    }
    else if( id == ID_PGN_DIALOG_SESSION )
    {
        helpText =
          wxT("Use this panel to view and load games from\n")
          wxT("earlier in the session\n\n");
    }
    wxMessageBox(helpText,
      wxT("Pgn Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

bool PgnDialog::ShowModalOk()
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
