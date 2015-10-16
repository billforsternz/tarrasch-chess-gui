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
#include "MiniBoard.h"
#include "PgnFiles.h"
#include "PgnDialog.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

#if 0
// PgnDialog event table definition
BEGIN_EVENT_TABLE( PgnDialog, wxDialog )
//  EVT_CLOSE( PgnDialog::OnClose )
    EVT_BUTTON( wxID_OK,                PgnDialog::OnOkClick )
    EVT_BUTTON( wxID_CANCEL,            PgnDialog::GdvOnCancel )
    EVT_BUTTON( ID_BOARD2GAME,          PgnDialog::OnBoard2Game )
    EVT_CHECKBOX( ID_REORDER,           PgnDialog::OnRenumber )
    EVT_BUTTON( ID_ADD_TO_CLIPBOARD,    PgnDialog::OnAddToClipboard )
    EVT_BUTTON( ID_SAVE_ALL_TO_A_FILE,  PgnDialog::OnSaveAllToAFile )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_DETAILS,   PgnDialog::OnEditGameDetails )
    EVT_BUTTON( ID_PGN_DIALOG_GAME_PREFIX,    PgnDialog::OnEditGamePrefix )
    EVT_BUTTON( ID_PGN_DIALOG_PUBLISH,  PgnDialog::OnPublish )
    EVT_BUTTON( ID_PGN_DIALOG_UTILITY1,  PgnDialog::OnUtility1 )
    EVT_BUTTON( ID_PGN_DIALOG_UTILITY2,  PgnDialog::OnUtility2 )
    EVT_BUTTON( wxID_COPY,              PgnDialog::OnCopy )
    EVT_BUTTON( wxID_CUT,               PgnDialog::OnCut )
    EVT_BUTTON( wxID_DELETE,            PgnDialog::OnDelete )
    EVT_BUTTON( wxID_PASTE,             PgnDialog::OnPaste )
    EVT_BUTTON( wxID_SAVE,              PgnDialog::OnSave )
    EVT_BUTTON( wxID_HELP,              PgnDialog::OnHelpClick )
    EVT_MENU( wxID_SELECTALL, PgnDialog::OnSelectAll )
    EVT_LIST_ITEM_FOCUSED(ID_PGN_LISTBOX, PgnDialog::OnListFocused)
    EVT_LIST_ITEM_ACTIVATED(ID_PGN_LISTBOX, PgnDialog::OnListSelected)
    EVT_LIST_COL_CLICK(ID_PGN_LISTBOX, PgnDialog::OnListColClick)
END_EVENT_TABLE()
#endif

wxSizer *PgnDialog::GdvAddExtraControls()
{
    // Edit game details
    wxButton* edit_game_details = new wxButton ( this, ID_PGN_DIALOG_GAME_DETAILS, wxT("Edit Game Details"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(edit_game_details, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Edit game prefix
    wxButton* edit_game_prefix = new wxButton ( this, ID_PGN_DIALOG_GAME_PREFIX, wxT("Edit Game Prefix"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(edit_game_prefix, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Paste game / Board->Game
    wxButton* board2game = new wxButton ( this, ID_BOARD2GAME, wxT("Paste current game"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(board2game, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Delete
    wxButton* delete_ = new wxButton ( this, wxID_DELETE, wxT("Delete"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(delete_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Cut
    wxButton* cut = new wxButton ( this, wxID_CUT, wxT("Cut"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(cut, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Add to clipboard
    wxButton* add = new wxButton ( this, ID_ADD_TO_CLIPBOARD, wxT("Add to clipboard"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Copy
    wxButton* copy = new wxButton ( this, wxID_COPY, wxT("Copy"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(copy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Paste
    wxButton* paste = new wxButton ( this, wxID_PASTE, wxT("Paste"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(paste, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Save all games to a file
    wxButton* save_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save all to a file"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Write to file
    wxButton* write_to_file = new wxButton ( this, wxID_SAVE, wxT("Save file"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(write_to_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Publish
    wxButton* publish = new wxButton ( this, ID_PGN_DIALOG_PUBLISH, wxT("Publish"),
        wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(publish, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    return vsiz_panel_button1;
}


void PgnDialog::GetCachedDocumentRaw( int idx, GameDocument &gd )
{
    smart_ptr<MagicBase> &mb = gc->gds[idx];
    gd = *mb->GetGameDocumentPtr();
}

GameDocument * PgnDialog::GetCachedDocument( int idx )
{
#if 1
    static GameDocument gd;
    CompactGame pact;
    smart_ptr<MagicBase> &mb = gc->gds[idx];
    mb->GetCompactGame(pact);
    pact.Upscale( gd );
    return &gd;
#else
    if( 0 != local_cache.count(idx) )
    {
        cprintf( "PgnDialog cache hit, idx %d\n", idx );
    }
    else
    {
        GameDocument gd;
        GetCachedDocumentRaw( idx, gd );
        local_cache[idx]      = gd;
        stack.push_back(idx);
        if( stack.size() < 100 )
        {
            cprintf( "PgnDialog add idx %d to cache, no removal\n", idx );
        }
        else
        {
            int front = *stack.begin();
            cprintf( "PgnDialog add idx %d and remove idx %d from cache\n", idx, front );
            stack.pop_front();
            local_cache.erase(front);
        }
        
        // Take the opportunity to put games that are likely to be needed soon in the local_cache
        for( int i=1; i<10 && idx+i<gc->gds.size(); i++ )
        {
            if( 0 == local_cache.count(idx+i) )
            {
                GetCachedDocumentRaw( idx+i, gd );
                local_cache[idx+i] = gd;
                stack.push_back(idx+i);
                if( stack.size() >= 100 )
                {
                    int front = *stack.begin();
                    stack.pop_front();
                    local_cache.erase(front);
                }
            }
        }
    }
    return &local_cache.at(idx);
#endif
}

void PgnDialog::GdvReadItem( int item, CompactGame &info )
{
    GameDocument *ptr = GetCachedDocument(item);
    info.Downscale( *ptr );
}


void PgnDialog::GdvSaveAllToAFile() {}
void PgnDialog::GdvHelpClick() {}
void PgnDialog::GdvSearch() {}
void PgnDialog::GdvUtility() {}
void PgnDialog::GdvOnCancel() {}
void PgnDialog::OnNextMove( int idx ) {}



// PgnDialog constructors
PgnDialog::PgnDialog
 (
  wxWindow    *parent,
  GamesCache  *gc,
  GamesCache  *gc_clipboard,
  wxWindowID  id,
  const wxPoint& pos,
  const wxSize& size,
  long style
  ) : GamesDialog( parent, NULL, gc, gc_clipboard, id, pos, size )
{
}


static smart_ptr<MagicBase> *ptr_gds;
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
    GameDocument *summary1 = ptr_gds[item1]->GetGameDocumentPtr();
    GameDocument *summary2 = ptr_gds[item2]->GetGameDocumentPtr();
    if( summary1==NULL || summary2==NULL )
        return 0;
    switch( col )
    {
        case 0: iflag = true;
                i1=summary1->game_nbr;
                i2=summary2->game_nbr;     break;
        case 1: s1=summary1->r.white;
                s2=summary2->r.white;        break;
        case 2: iflag = true;
                i1=atoi(summary1->r.white_elo.c_str());
                i2=atoi(summary2->r.white_elo.c_str());
                                                break;
        case 3: s1=summary1->r.black;
                s2=summary2->r.black;        break;
        case 4: iflag = true;
                i1=atoi(summary1->r.black_elo.c_str());
                i2=atoi(summary2->r.black_elo.c_str());
                                                break;
        case 5: s1=summary1->r.date;
                s2=summary2->r.date;         break;
        case 6: s1=summary1->r.site;
                s2=summary2->r.site;         break;
//      case 7: fflag = true;
//              f1=atof(ptr_item1.round.c_str());
//              f2=atof(ptr_item2.round.c_str());
//                                              break;
        case 7: s1=summary1->r.round;
                s2=summary2->r.round;        break;
        case 8: s1=summary1->r.result;
                s2=summary2->r.result;       break;
        case 9: s1=summary1->r.eco;
                s2=summary2->r.eco;          break;
        case 10:s1=summary1->prefix_txt.length() ? summary1->prefix_txt : summary1->moves_txt;
                s2=summary2->prefix_txt.length() ? summary2->prefix_txt : summary2->moves_txt;  break;
        case 11:iflag = true;
                i1=summary1->sort_idx;
                i2=summary2->sort_idx;     break;
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

/*    int top_item;
    bool resuming = gc->IsResumingPreviousWindow(top_item);
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
            GameDocument *ptr = gc->gds[i]->GetGameDocumentPtr();
            if( ptr->focus )
            {
                list_ctrl->SetItemState( i, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
                focus_found = true;
                focus_idx = i;
            }
        }
        for( int i=0; i<sz; i++ )
        {
            GameDocument *ptr = gc->gds[i]->GetGameDocumentPtr();
            if( ptr->selected )
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
                GameDocument *ptr = gc->gds[i]->GetGameDocumentPtr();
                if( ptr && ptr->game_being_edited == objs.gl->gd.game_being_edited )
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

    // Edit game details
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* edit_game_details = new wxButton ( this, ID_PGN_DIALOG_GAME_DETAILS, wxT("Edit Game Details"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(edit_game_details, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Edit game prefix
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* edit_game_prefix = new wxButton ( this, ID_PGN_DIALOG_GAME_PREFIX, wxT("Edit Game Prefix"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(edit_game_prefix, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Paste game / Board->Game
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD )
    {
        wxButton* board2game = new wxButton ( this, ID_BOARD2GAME, wxT("Paste current game"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(board2game, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Delete
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD )
    {
        wxButton* delete_ = new wxButton ( this, wxID_DELETE, wxT("Delete"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(delete_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Cut
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* cut = new wxButton ( this, wxID_CUT, wxT("Cut"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(cut, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Add to clipboard
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* add = new wxButton ( this, ID_ADD_TO_CLIPBOARD, wxT("Add to clipboard"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Copy
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* copy = new wxButton ( this, wxID_COPY, wxT("Copy"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(copy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Paste
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* paste = new wxButton ( this, wxID_PASTE, wxT("Paste"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(paste, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Save all games to a file
    if( id==ID_PGN_DIALOG_CLIPBOARD||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* save_all_to_a_file = new wxButton ( this, ID_SAVE_ALL_TO_A_FILE, wxT("Save all to a file"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(save_all_to_a_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Renumber
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxCheckBox* reorder = new wxCheckBox ( this, ID_REORDER, wxT("Renumber"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(reorder, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Write to file
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* write_to_file = new wxButton ( this, wxID_SAVE, wxT("Save file"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(write_to_file, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Publish
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* publish = new wxButton ( this, ID_PGN_DIALOG_PUBLISH, wxT("Publish"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(publish, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // Utility1
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* utility1 = new wxButton ( this, ID_PGN_DIALOG_UTILITY1, wxT("Add all to database"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(utility1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    
    // Utility2
    if( id==ID_PGN_DIALOG_FILE )
    {
        wxButton* utility2 = new wxButton ( this, ID_PGN_DIALOG_UTILITY2, wxT("Utility2"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(utility2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    
    // The Cancel button
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* cancel = new wxButton ( this, wxID_CANCEL,
            wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    // The Help button
    if( id==ID_PGN_DIALOG_FILE||id==ID_PGN_DIALOG_CLIPBOARD||id==ID_PGN_DIALOG_SESSION )
    {
        wxButton* help = new wxButton( this, wxID_HELP, wxT("Help"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_buttons->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    } */
    

void PgnDialog::GdvListColClick( int compare_col )
{
    local_cache.clear();
    stack.clear();
    GamesDialog::GdvListColClick( compare_col );
}

/*
    ptr_gds = &gc->gds[0];
    ptr_col_flags = &gc->col_flags[compare_col];
    gc->Debug( "Before sort" );
    SyncCacheOrderBefore();
    list_ctrl->SortItems(sort_callback,compare_col);
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
        GameDocument *ptr = gc->gds[i]->GetGameDocumentPtr();
        if( ptr && ptr->game_being_edited==objs.gl->gd.game_being_edited && gc==&objs.gl->gc )
            objs.gl->file_game_idx = i;
    }
    list_ctrl->EnsureVisible( idx );
    *ptr_col_flags = !*ptr_col_flags;
} */


bool PgnDialog::LoadGame( GameLogic *gl, GameDocument& gd, int &file_game_idx )
{
    int selected_game = track->focus_idx;
    if( selected_game != -1 )
    {
        // gl->IndicateNoCurrentDocument();
        uint32_t temp = ++gl->game_being_edited_tag;
        GameDocument *ptr2 = gc->gds[selected_game]->GetGameDocumentPtr();
        if( ptr2 )
            ptr2->game_being_edited = temp;
        GameDocument *ptr = GetCachedDocument(selected_game);
        gd = *ptr;
        gd.SetNonZeroStartPosition(track->focus_offset);
        gd.game_being_edited = temp;
        gd.selected = false;
        ptr->selected = true;
        //if( &gl->gc == gc )
        file_game_idx = selected_game; //this->file_game_idx;    // update this only if loading game from current file
    }
    return selected_game != -1;
}


// We keep the game cache array of game documents synced to the
//  list box presentation of those games. Before changing the
//  order of the list box items call this ...
void PgnDialog::SyncCacheOrderBefore()
{
/*    int gds_nbr = gc->gds.size();
    for( int i=0; i<gds_nbr; i++ )    
        list_ctrl->SetItemData( i, i );  */
}

// ... Afterward call this to reestablish sync
void PgnDialog::SyncCacheOrderAfter()
{
/*    int gds_nbr = gc->gds.size();
    for( int i=0; i<gds_nbr; i++ )    
    {
        int idx = list_ctrl->GetItemData(i);
        GameDocument *ptr = gc->gds[idx]->GetGameDocumentPtr();
        ptr->sort_idx = i;
    }
    sort( gc->gds.begin(), gc->gds.end() );
    for( int i=0; i<gds_nbr; i++ )    
        list_ctrl->SetItemData( i, i ); */
}



// TODO WAKE ALL THIS STUFF UP LATER

#if 0
int PgnDialog::GetFocusGame( int &idx )
{
    int focus_game = -1;
    if( list_ctrl )
    {
        int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
            {
                focus_game = i;
                break;
            }
        }

        // If no item currently in focus, take the top item in window
        if( focus_game==-1 && sz>0 )
        {
            idx = list_ctrl->GetTopItem();
            if( idx >= sz )
                idx = 0;
            list_ctrl->SetItemState( idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );
            focus_game = idx;
        }
    }
    return focus_game;
}

void PgnDialog::DeselectOthers( int selected_game )
{
    if( list_ctrl && selected_game!=-1 )
    {
        int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            GameDocument *ptr = gc->gds[i]->GetGameDocumentPtr();
            if( i != selected_game )
            {
                if( ptr->selected || (wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED)) )
                {
                    ptr->selected = false;
                    list_ctrl->SetItemState( i, 0, wxLIST_STATE_SELECTED );
                }
            }
        }
    }
    gc->Debug( "After DeselectOthers()" );
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
void PgnDialog::OnOk()
{
    test();
    selected_game = -1;
    if( list_ctrl )
    {
        gc->PrepareForResumePreviousWindow( list_ctrl->GetTopItem() );
     /* int sz=gc->gds.size();
        for( int i=0; i<sz; i++ )
        {
            GameDocument *ptr = gc->gds[i]->GetGameDocumentPtr();
            if( wxLIST_STATE_SELECTED & list_ctrl->GetItemState(i,wxLIST_STATE_SELECTED) )
                ptr->selected = true;
            if( wxLIST_STATE_FOCUSED & list_ctrl->GetItemState(i,wxLIST_STATE_FOCUSED) )
                ptr->focus = true;
            else
                ptr->focus = false;
        } */
        selected_game = GetFocusGame(file_game_idx);
        if( selected_game != -1  )
        {
            // DeselectOthers(selected_game);
            GameDocument doc = *GetCachedDocument(selected_game);
            doc.in_memory = true;
            doc.selected = true;
            make_smart_ptr( GameDocument,sptr,doc );
            gc->gds[selected_game] = std::move(sptr);
        }
    }
    TransferDataToWindow();    
    AcceptAndClose();
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
            make_smart_ptr( GameDocument, new_doc, gd );
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
        {
            GameDocument *ptr = GetCachedDocument(i);
            if( ptr )
                game_nbr = ptr->game_nbr;
        }
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
    }
}

void PgnDialog::OnEditGamePrefix( wxCommandEvent& WXUNUSED(event) )
{
    int idx;
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
    }
}

void PgnDialog::GdvSaveAllToAFile()
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
                GameDocument gd = gc->gds[i]->GetGameDocument();
                make_smart_ptr( GameDocument, new_doc, gd );
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
            make_smart_ptr( GameDocument, new_doc, gd );
            gc_clipboard->gds.push_back(std::move(new_doc));
            nbr_copied++;
        }
    }
    dbg_printf( "%d games copied\n", nbr_copied );
}

void PgnDialog::OnCut( wxCommandEvent& WXUNUSED(event) )
{
    bool clear_clipboard = true;
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
                make_smart_ptr( GameDocument, new_doc, gd );
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
            make_smart_ptr( GameDocument,new_doc,gd);
            gc_clipboard->gds.push_back(std::move(new_doc));
            list_ctrl->DeleteItem(idx_focus);
            iter = gc->gds.erase(iter_focus);
            gc->file_irrevocably_modified = true;
            nbr_cut++;
        }
    }
    dbg_printf( "%d games cut\n", nbr_cut );
}

void PgnDialog::OnDelete( wxCommandEvent& WXUNUSED(event) )
{
    int nbr_deleted=0, idx_focus=-1;
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
    dbg_printf( "%d games deleted\n", nbr_deleted );
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
            std::vector< smart_ptr<MagicBase> >::iterator iter = gc->gds.begin() + idx_focus;
            GameDocument gd;
            gc_clipboard->gds[i]->GetGameDocument(gd);
            gd.game_nbr = 0;
            gd.modified = true;
            make_smart_ptr( GameDocument,new_doc,gd);
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

#endif


