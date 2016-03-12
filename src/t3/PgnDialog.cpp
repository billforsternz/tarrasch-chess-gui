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


wxSizer *PgnDialog::GdvAddExtraControls()
{
    if( id == ID_PGN_DIALOG_FILE )
    {
        // Edit game details
        wxButton* edit_game_details = new wxButton ( this, ID_PGN_DIALOG_GAME_DETAILS, wxT("Edit Game Details"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(edit_game_details, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Edit game prefix
        //wxButton* edit_game_prefix = new wxButton ( this, ID_PGN_DIALOG_GAME_PREFIX, wxT("Edit Game Prefix"),
        //    wxDefaultPosition, wxDefaultSize, 0 );
        //vsiz_panel_button1->Add(edit_game_prefix, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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

        // Publish
        wxButton* publish = new wxButton ( this, ID_PGN_DIALOG_PUBLISH, wxT("Publish"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(publish, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // ECO codes
        wxButton* eco_codes = new wxButton ( this, ID_DIALOG_ECO, wxT("ECO codes"),
            wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(eco_codes, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }
    else if( id == ID_PGN_DIALOG_CLIPBOARD )
    {
        // Edit game details
        wxButton* edit_game_details = new wxButton ( this, ID_PGN_DIALOG_GAME_DETAILS, wxT("Edit Game Details"),
                                                    wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(edit_game_details, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
        // Paste game / Board->Game
        wxButton* board2game = new wxButton ( this, ID_BOARD2GAME, wxT("Paste current game"),
                                             wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(board2game, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
        // Delete
        wxButton* delete_ = new wxButton ( this, wxID_DELETE, wxT("Delete"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(delete_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
    }
    else if( id == ID_PGN_DIALOG_SESSION )
    {
        // Add to clipboard
        wxButton* add = new wxButton ( this, ID_ADD_TO_CLIPBOARD, wxT("Add to clipboard"),
                                      wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
        // Copy
        wxButton* copy = new wxButton ( this, wxID_COPY, wxT("Copy"),
                                       wxDefaultPosition, wxDefaultSize, 0 );
        vsiz_panel_button1->Add(copy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
    }
    return vsiz_panel_button1;
}


void PgnDialog::GetCachedDocumentRaw( int idx, GameDocument &gd )
{
    smart_ptr<ListableGame> &mb = gc->gds[idx];
    gd = *mb->GetGameDocumentPtr();
}

GameDocument * PgnDialog::GetCachedDocument( int idx )
{
    static GameDocument gd;
    CompactGame pact;
    smart_ptr<ListableGame> &mb = gc->gds[idx];
    mb->GetCompactGame(pact);
    pact.Upscale( gd );
    return &gd;
}

void PgnDialog::GdvReadItem( int item, CompactGame &info )
{
    smart_ptr<ListableGame> &mb = gc->gds[item];
    mb->GetCompactGame( info );
}

void PgnDialog::GdvHelpClick()
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
    "Use this panel to preview the games in a .pgn file."
    "\n\n"
    "Two special features are provided at the moment. ECO calculates ECO codes. Note "
    "that existing ECO codes are overwritten. Since different chess programs may differ "
    "in how they calculate ECO codes, this may modify existing codes."
    "\n\n"
    "The Publish button is used to create interactive web content. This is still under "
    "development and should be treated as an extra-for-experts. There is a file "
    "named web.zip in your TarraschDb installation directory with supplementary files "
    "and instructions for the keen and well motivated."
    "\n\n";
    wxMessageBox(helpText,
    wxT("Database Dialog Help"),
    wxOK|wxICON_INFORMATION, NULL );
}

void PgnDialog::GdvSearch() {}
void PgnDialog::GdvUtility() {}
void PgnDialog::GdvOnCancel() {}
void PgnDialog::GdvNextMove( int idx ) {}

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


void PgnDialog::GdvListColClick( int compare_col )
{
    local_cache.clear();
    stack.clear();

    // Load all games into memory
    int nbr = gc->gds.size();
    int old_percent = -1;
    void *context=0;
    bool end=false;
    for( int i=0; !end && i<nbr; i++ )
    {
        
        // If all the games are already in memory, the whole loop will operate here
        if( !context )
            context = gc->gds[i]->LoadIntoMemory( context, i+1 >= nbr );

        // If we need to load games, put up a progress dialog box
        else
        {
            wxProgressDialog progress( "Loading games", "Loading games", 100, NULL,
                                      wxPD_APP_MODAL+
                                      wxPD_AUTO_HIDE+
                                      wxPD_ELAPSED_TIME+
                                      wxPD_CAN_ABORT+
                                      wxPD_ESTIMATED_TIME );
            for( ; !end && i<nbr; i++ )
            {
                end = (i+1 >= nbr);
                int percent = (i*100) / (nbr?nbr:1);
                if( percent < 1 )
                    percent = 1;
                if( percent != old_percent )
                {
                    old_percent = percent;
                    if( !progress.Update( percent>100 ? 100 : percent ) )
                        end = true;
                }
                context = gc->gds[i]->LoadIntoMemory( context, end );   // never exit without an end=true call
                                                                        //  prevents resource leaks
            }
        }
    }
    GamesDialog::GdvListColClick( compare_col );
}

bool PgnDialog::LoadGame( GameLogic *gl, GameDocument& gd, int &file_game_idx )
{
    int selected_game = track->focus_idx;
    if( selected_game != -1 )
    {
        uint32_t temp = ++gl->game_being_edited_tag;
        GameDocument *ptr = gc->gds[selected_game]->GetGameDocumentPtr();
        if( ptr )
        {
            gc->gds[selected_game]->SetGameBeingEdited( temp );
            gd = *ptr;
            gd.SetGameBeingEdited( temp );
            gd.SetNonZeroStartPosition(track->focus_offset);
            gd.selected = false;
            file_game_idx = selected_game; //this->file_game_idx;    // update this only if loading game from current file
        }
    }
    return selected_game != -1;
}
