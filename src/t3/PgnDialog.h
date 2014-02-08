/****************************************************************************
 * Custom dialog - Pgn browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PGN_DIALOG_H
#define PGN_DIALOG_H
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/accel.h"
#include "SuspendEngine.h"
#include "GamesCache.h"
#include "GameDocument.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_PGN_DIALOG_FILE      = 10000,
    ID_PGN_DIALOG_CLIPBOARD = 10001,
    ID_PGN_DIALOG_SESSION   = 10002,
    ID_BOARD2GAME           = 10003,
    ID_PGN_DIALOG_GAME_DETAILS  = 10004,
    ID_REORDER              = 10005,
    ID_ADD_TO_CLIPBOARD     = 10006,
    ID_SAVE_ALL_TO_A_FILE   = 10007,
    ID_PGN_DIALOG_GAME_PREFIX    = 10008,
    ID_PGN_DIALOG_PUBLISH    = 10009,
    ID_PGN_DIALOG_DATABASE   = 10010
};

// PgnDialog class declaration
class PgnDialog: public wxDialog
{    
    DECLARE_CLASS( PgnDialog )
    DECLARE_EVENT_TABLE()

private:
    GamesCache *gc;
    GamesCache *gc_clipboard;

public:

    // Constructors
    PgnDialog
    (
        wxWindow    *parent,
        GamesCache  *gc,
        GamesCache  *gc_clipboard,
        wxWindowID  id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );

    // Member initialisation
    void Init();

    // Creation
    bool Create
    (
        wxWindow        *parent,
        wxWindowID      id,
        const wxString& caption = wxT("Pgn browser"),
        const wxPoint&  pos = wxDefaultPosition,
        const wxSize&   size = wxDefaultSize,
        long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // PgnDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    void OnListSelected( wxListEvent &event );
    void OnListColClick( wxListEvent &event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    void OnBoard2Game( wxCommandEvent& event );
    void OnRenumber( wxCommandEvent& event );
    void OnSelectAll( wxCommandEvent& event );
    void OnEditGameDetails( wxCommandEvent& event );
    void OnEditGamePrefix( wxCommandEvent& event );
    void OnCopy( wxCommandEvent& event );
    void OnAddToClipboard( wxCommandEvent& event );
    void OnSaveAllToAFile( wxCommandEvent& event );
    void OnCut( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
    void OnPaste( wxCommandEvent& event );
    void OnSave( wxCommandEvent& event );
    void OnPublish( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnHelpClick( wxCommandEvent& event );
//  void OnClose( wxCloseEvent& event );
//  void SaveColumns();
    bool ShowModalOk();


    // Return true if a game has been selected
    bool LoadGame( GameLogic *gl, GameDocument& gd, int &file_game_idx );

    // Helpers
    GameDocument *GetFocusGame( int &idx );
    void DeselectOthers( GameDocument *selected_game );
    void OnOk();

    // PgnDialog member variables
private:
    wxListCtrl  *list_ctrl;
    GameDocument *selected_game;
    void         SyncCacheOrderBefore();
    void         SyncCacheOrderAfter();
    void         CopyOrAdd( bool clear_clipboard );
    std::string  CalculateMovesColumn( GameDocument &gd );

    // Data members
    wxWindowID  id;
    int file_game_idx;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
};

#endif    // PGN_DIALOG_H
