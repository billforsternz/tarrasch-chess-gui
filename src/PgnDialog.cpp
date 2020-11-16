/****************************************************************************
 * Custom dialog - Pgn browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
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
#include "CtrlChessBoard.h"
#include "PgnFiles.h"
#include "PgnDialog.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

void PgnDialog::GdvEnumerateGames()
{
}


wxSizer *PgnDialog::GdvAddExtraControls( bool WXUNUSED(big_display) )
{
    if( id==ID_PGN_DIALOG_FILE || id==ID_PGN_DIALOG_CURRENT_FILE )
    {
        // Edit game details
        wxButton* edit_game_details = new wxButton ( this, ID_PGN_DIALOG_GAME_DETAILS, wxT("Edit Game Details"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( edit_game_details );
        vsiz_panel_buttons->Add(edit_game_details, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Edit game prefix
        //wxButton* edit_game_prefix = new wxButton ( this, ID_PGN_DIALOG_GAME_PREFIX, wxT("Edit Game Prefix"),
        //    wxDefaultPosition, wxDefaultSize, 0 );
        //vsiz_panel_buttons->Add(edit_game_prefix, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Paste game / Board->Game
        wxButton* board_to_game = new wxButton ( this, ID_BOARD2GAME, wxT("Paste current game"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( board_to_game );
        vsiz_panel_buttons->Add(board_to_game, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Delete
        wxButton* delete_ = new wxButton ( this, wxID_DELETE, wxT("Delete"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( delete_ );
        vsiz_panel_buttons->Add(delete_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Cut
        wxButton* cut = new wxButton ( this, wxID_CUT, wxT("Cut"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( cut );
        vsiz_panel_buttons->Add(cut, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Add to clipboard
        wxButton* add = new wxButton ( this, ID_ADD_TO_CLIPBOARD, wxT("Add to clipboard"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( add );
        vsiz_panel_buttons->Add(add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Copy
        wxButton* copy = new wxButton ( this, wxID_COPY, wxT("Copy"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( copy );
        vsiz_panel_buttons->Add(copy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Paste
        wxButton* paste = new wxButton ( this, wxID_PASTE, wxT("Paste"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( paste );
        vsiz_panel_buttons->Add(paste, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Publish
        wxButton* publish = new wxButton ( this, ID_PGN_DIALOG_PUBLISH, wxT("Publish"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( publish );
        vsiz_panel_buttons->Add(publish, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // ECO codes
        wxButton* eco_codes = new wxButton ( this, ID_DIALOG_ECO, wxT("ECO codes"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( eco_codes );
        vsiz_panel_buttons->Add(eco_codes, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // New pairings
        wxButton* tournament_games = new wxButton ( this, ID_TOURNAMENT_GAMES, wxT("New Pairings"),
            wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( tournament_games );
        vsiz_panel_buttons->Add(tournament_games, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

        // Spacer
        vsiz_panel_buttons->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    }
    else if( id == ID_PGN_DIALOG_CLIPBOARD )
    {
        // Edit game details
        wxButton* edit_game_details = new wxButton ( this, ID_PGN_DIALOG_GAME_DETAILS, wxT("Edit Game Details"),
                                                    wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( edit_game_details );
        vsiz_panel_buttons->Add(edit_game_details, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
        // Paste game / Board->Game
        wxButton* board2game = new wxButton ( this, ID_BOARD2GAME, wxT("Paste current game"),
                                             wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( board2game );
        vsiz_panel_buttons->Add(board2game, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
        // Delete
        wxButton* delete_ = new wxButton ( this, wxID_DELETE, wxT("Delete"),
                                          wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( delete_ );
        vsiz_panel_buttons->Add(delete_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
    }
    else if( id == ID_PGN_DIALOG_SESSION )
    {
        // Add to clipboard
        wxButton* add = new wxButton ( this, ID_ADD_TO_CLIPBOARD, wxT("Add to clipboard"),
                                      wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( add );
        vsiz_panel_buttons->Add(add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
        // Copy
        wxButton* copy = new wxButton ( this, wxID_COPY, wxT("Copy"),
                                       wxDefaultPosition, wxDefaultSize, 0 );
        gdr.RegisterPanelWindow( copy );
        vsiz_panel_buttons->Add(copy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        
    }
    return vsiz_panel_buttons;
}

void PgnDialog::GdvEnableControlsIfGamesFound( bool have_games )
{
    FindWindow(wxID_OK)->Enable(have_games);                            // Load Game
    if( id==ID_PGN_DIALOG_FILE || id==ID_PGN_DIALOG_CURRENT_FILE )
    {
        // Don't selectively disable the save buttons, we want to be able to save zero length PGN files
        //FindWindow(wxID_SAVE)->Enable(have_games);                      // Save
        //FindWindow(ID_SAVE_ALL_TO_A_FILE)->Enable(have_games);          // Save all
        FindWindow(ID_PGN_DIALOG_GAME_DETAILS)->Enable(have_games);     // Edit Game Details
        FindWindow(wxID_COPY)->Enable(have_games);                      // Copy
        FindWindow(wxID_CUT)->Enable(have_games);                       // Cut
        FindWindow(wxID_DELETE)->Enable(have_games);                    // Delete
        FindWindow(ID_ADD_TO_CLIPBOARD)->Enable(have_games);            // Add to clipboard
        //FindWindow(ID_PGN_DIALOG_PUBLISH)->Enable(have_games);          // Publish
        FindWindow(ID_DIALOG_ECO)->Enable(have_games);                  // ECO codes
    }
    else if( id == ID_PGN_DIALOG_CLIPBOARD )
    {
        FindWindow(ID_PGN_DIALOG_GAME_DETAILS)->Enable(have_games);     // Edit Game Details
        FindWindow(wxID_DELETE)->Enable(have_games);                    // Delete
        //FindWindow(ID_SAVE_ALL_TO_A_FILE)->Enable(have_games);          // Save all
    }
    else if( id == ID_PGN_DIALOG_SESSION )
    {
        FindWindow(ID_ADD_TO_CLIPBOARD)->Enable(have_games);            // Add to clipboard
        FindWindow(wxID_COPY)->Enable(have_games);                      // Copy
        //FindWindow(ID_SAVE_ALL_TO_A_FILE)->Enable(have_games);          // Save all
    }
}


void PgnDialog::GetCachedDocumentRaw( int idx, GameDocument &gd )
{
    smart_ptr<ListableGame> &mb = gc->gds[idx];
    mb->ConvertToGameDocument(gd);
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
    if( 0<=item && static_cast<unsigned int>(item)<gc->gds.size() )
    {
        smart_ptr<ListableGame> &mb = gc->gds[item];
        mb->GetCompactGame( info );
    }
}

void PgnDialog::GdvHelpClick()
{
    wxString helpText;
    if( id==ID_PGN_DIALOG_FILE || id==ID_PGN_DIALOG_CURRENT_FILE )
        helpText=
    "\nUse this panel to preview the games in a .pgn file."
    "\n\n"
    "You can sort on any column. The first column (column '#') is provided only to allow "
    "a sort on initial order (for restoring or reversing the initial order). To reverse "
	"sort click twice. The moves column sort is statistical (most popular lines first) "
	"rather than alphabetical. Sort history is respected in tie breaks. For example, if you "
	"sort on Moves, then on Black, then on White; The games will be sorted by White "
	"player, but all opponents will be grouped together (because of the earlier sort on Black), "
	"and if there are multiple games between the same players, the games will be sorted according "
	"to the most common opening sequences between those two players (because of the earlier sort on Moves)."
    "\n\n"
    "Two special features are provided at the moment. ECO calculates ECO codes for games "
    "which do not have them yet."
    "\n\n"
    "The Publish button is used to create interactive web content. This is still under "
    "development and should be treated as an extra-for-experts. There is a file "
    "named web.zip in your Tarrasch installation directory with supplementary files "
    "and instructions for the keen and well motivated.";

    else if( id == ID_PGN_DIALOG_CLIPBOARD )
        helpText=
    "\nUse this panel to look at games in the clipboard. Games can be copied to the clipboard "
    "from any of the Current file, Session or Database games dialogs, all available from the "
    "Games menu."
    "\n\n"
    "You can sort on any column. The first column (column '#') is provided only to allow "
    "a sort on initial order (for restoring or reversing the initial order). To reverse "
	"sort click twice. The moves column sort is statistical (most popular lines first) "
	"rather than alphabetical. Sort history is respected in tie breaks. For example, if you "
	"sort on Moves, then on Black, then on White; The games will be sorted by White "
	"player, but all opponents will be grouped together (because of the earlier sort on Black), "
	"and if there are multiple games between the same players, the games will be sorted according "
	"to the most common opening sequences between those two players (because of the earlier sort on Moves).";

    else if( id == ID_PGN_DIALOG_SESSION )
        helpText=
    "\nUse this panel to look at games you have worked with in this Tarrasch Chess GUI session."
    "\n\n"
    "You can sort on any column. The first column (column '#') is provided only to allow "
    "a sort on initial order (for restoring or reversing the initial order). To reverse "
	"sort click twice. The moves column sort is statistical (most popular lines first) "
	"rather than alphabetical. Sort history is respected in tie breaks. For example, if you "
	"sort on Moves, then on Black, then on White; The games will be sorted by White "
	"player, but all opponents will be grouped together (because of the earlier sort on Black), "
	"and if there are multiple games between the same players, the games will be sorted according "
	"to the most common opening sequences between those two players (because of the earlier sort on Moves).";
    wxMessageBox(helpText,
    wxT("Games Dialog Help"),
    wxOK|wxICON_INFORMATION, NULL );
}

void PgnDialog::GdvSearch() {}
void PgnDialog::GdvUtility() {}
void PgnDialog::GdvOnCancel() {}
void PgnDialog::GdvNextMove( int UNUSED(idx) ) {}

// PgnDialog constructors
PgnDialog::PgnDialog
 (
  wxWindow    *parent,
  GamesCache  *gc,
  GamesCache  *gc_clipboard,
  wxWindowID  id,
  const wxPoint& pos,
  const wxSize& size
  ) : GamesDialog( parent, NULL, gc, gc_clipboard, id, pos, size )
{
}


void PgnDialog::GdvListColClick( int compare_col_ )
{
    local_cache.clear();
    stack.clear();
    bool cancelled = false;

    // Load all games into memory
    int nbr = gc->gds.size();
    int old_percent = -1;
    void *context=0;
    bool end = false;
    if( compare_col_ == 0 )
        end = true; // we don't need to load into memory for column 0
    for( int i=0; !end && i<nbr; i++ )
    {
        
        // If all the games are already in memory, the whole loop will operate here
        if( !context )
            context = gc->gds[i]->LoadIntoMemory( context, i+1 >= nbr );

        // If we need to load games, put up a progress dialog box
        else
        {
            wxProgressDialog progress( "Loading games", "Loading games", 100, this,
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
                    {
                        end = true;
                        cancelled = true;
                    }
                }
                context = gc->gds[i]->LoadIntoMemory( context, end );   // never exit without an end=true call
                                                                        //  prevents resource leaks
            }
        }
    }
    if( !cancelled )
        GamesDialog::GdvListColClick( compare_col_ );
}

bool PgnDialog::LoadGame( GameLogic *gl, GameDocument& gd )
{
    int selected_game_ = track->focus_idx;
    if( selected_game_ != -1 )
    {
		GameDocument *gd_file = gc->gds[selected_game_]->IsGameDocument();
		if( gd_file )
			gd = *gd_file;
		else
		{
			gc->gds[selected_game_]->ConvertToGameDocument(gd);
			make_smart_ptr(GameDocument, new_smart_ptr, gd);
			gc->gds[selected_game_] = std::move(new_smart_ptr);
		}

		// #Workflow)  Game pulled out of game dialog
		// if it's the working file dialog establish edit relationship
		if( gc == &gl->gc_pgn )
			gl->SetGameBeingEdited( gd, *gc->gds[selected_game_] );
        gd.SetNonZeroStartPosition(track->focus_offset);
    }
    return selected_game_ != -1;
}

int PgnDialog::GetSelectedGame( int *offset )
{
	*offset = track->focus_offset;
    return track->focus_idx;
}
