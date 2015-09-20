/****************************************************************************
 * Custom dialog - Clipboard browser
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
#include "ClipboardDialog.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

#if 1

// ClipboardDialog constructors
ClipboardDialog::ClipboardDialog
(
 wxWindow    *parent,
 GamesCache  *gc,
 GamesCache  *gc_clipboard,
 wxWindowID  id,
 const wxPoint& pos,
 const wxSize& size,
 long style
 ) : PgnDialog( parent, gc, gc_clipboard, id, pos, size, style )
{
}

wxSizer *ClipboardDialog::AddExtraControls()
{
    wxSizer *vsiz_panel_button1 = PgnDialog::AddExtraControls();
    filter_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX,
                                 wxT("&Clipboard as temp database"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(filter_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    filter_ctrl->SetValue( objs.gl->db_clipboard );
    return vsiz_panel_button1;
}

void ClipboardDialog::OnCheckBox( bool checked )
{
    objs.gl->db_clipboard = checked;
}

#else
wxSizer *ClipboardDialog::AddExtraControls()
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
}


void ClipboardDialog::GetCachedDocumentRaw( int idx, GameDocument &gd )
{
    smart_ptr<MagicBase> &mb = gc->gds[idx];
    gd = *mb->GetGameDocumentPtr();
}

GameDocument * ClipboardDialog::GetCachedDocument( int idx )
{
    if( 0 != local_cache.count(idx) )
    {
        cprintf( "ClipboardDialog cache hit, idx %d\n", idx );
    }
    else
    {
        GameDocument gd;
        GetCachedDocumentRaw( idx, gd );
        local_cache[idx]      = gd;
        stack.push_back(idx);
        if( stack.size() < 100 )
        {
            cprintf( "ClipboardDialog add idx %d to cache, no removal\n", idx );
        }
        else
        {
            int front = *stack.begin();
            cprintf( "ClipboardDialog add idx %d and remove idx %d from cache\n", idx, front );
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
}

void ClipboardDialog::ReadItem( int item, CompactGame &info )
{
    GameDocument *ptr = GetCachedDocument(item);
    info.Downscale( *ptr );
}


void ClipboardDialog::OnSaveAllToAFile() {}
void ClipboardDialog::OnHelpClick() {}
void ClipboardDialog::OnSearch() {}
void ClipboardDialog::OnUtility() {}
void ClipboardDialog::OnCancel() {}
void ClipboardDialog::OnNextMove( int idx ) {}



// ClipboardDialog constructors
ClipboardDialog::ClipboardDialog
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

void ClipboardDialog::OnListColClick( int compare_col )
{
    local_cache.clear();
    stack.clear();
    GamesDialog::OnListColClick( compare_col );
}


bool ClipboardDialog::LoadGame( GameLogic *gl, GameDocument& gd, int &file_game_idx )
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
void ClipboardDialog::SyncCacheOrderBefore()
{
/*    int gds_nbr = gc->gds.size();
    for( int i=0; i<gds_nbr; i++ )    
        list_ctrl->SetItemData( i, i );  */
}

// ... Afterward call this to reestablish sync
void ClipboardDialog::SyncCacheOrderAfter()
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

#endif

