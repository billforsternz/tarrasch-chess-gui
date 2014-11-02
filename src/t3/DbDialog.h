/****************************************************************************
 * Custom dialog - Database browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DB_DIALOG_H
#define DB_DIALOG_H
#include <unordered_set>
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/accel.h"
#include "SuspendEngine.h"
#include "GamesCache.h"
#include "GameDocument.h"
#include "Repository.h"
#include "MiniBoard.h"
#include "CompressMoves.h"
#include "Database.h"
#include "PgnDialog.h"

// Control identifiers
enum
{
    ID_DB_CHECKBOX      = 10000,
    ID_DB_RADIO         = 10001,
    ID_DB_COMBO         = 10002,
    ID_DB_RELOAD        = 10003,
    ID_DB_UTILITY       = 10004,
    ID_DB_TEXT          = 10005,
    ID_DB_LISTBOX_GAMES = 10006,
    ID_DB_LISTBOX_STATS = 10007,
    ID_DB_LISTBOX_TRANSPO = 10008
};

class wxVirtualListCtrl;

// Each move in a given position has stats associated with it
struct MOVE_STATS
{
    int nbr_games;
    int nbr_white_wins;
    int nbr_black_wins;
    int nbr_draws;
    
    // Sort according to number of games
    bool operator < (const MOVE_STATS& ms)  const { return nbr_games < ms.nbr_games; }
    bool operator > (const MOVE_STATS& ms)  const { return nbr_games > ms.nbr_games; }
    bool operator == (const MOVE_STATS& ms) const { return nbr_games == ms.nbr_games; }
};

// Individual path to a given position
struct PATH_TO_POSITION
{
    PATH_TO_POSITION() { frequency=0; }
    int frequency;
    std::string blob;
    CompressMoves press;    // Each of the different blobs has its own decompressor - necessary for situations where
    //  for example the knights have swapped places - position is same but compressor state is not
    
    // Sort according to frequency
    bool operator < (const PATH_TO_POSITION& ptp)  const { return frequency < ptp.frequency; }
    bool operator > (const PATH_TO_POSITION& ptp)  const { return frequency > ptp.frequency; }
    bool operator == (const PATH_TO_POSITION& ptp) const { return frequency == ptp.frequency; }
};


// DbDialog class declaration
class DbDialog: public wxDialog
{    
    DECLARE_CLASS( DbDialog )
    DECLARE_EVENT_TABLE()

private:
    GamesCache *gc;
    GamesCache *gc_clipboard;
    thc::ChessRules cr;
    wxStaticText *title_ctrl;
    
    // Map each move in the position to move stats
    std::map< uint32_t, MOVE_STATS > stats;
    
public:
    // We calculate a vector of all blobs in the games that leading to the search position
    std::vector< PATH_TO_POSITION > transpositions;
    int compare_col;

    // Track the chess position on the mini board
    thc::ChessPosition track_updated_position;
    DB_GAME_INFO track_info;
    std::vector< thc::Move > track_focus_moves;

    // Constructors
    DbDialog
    (
        wxWindow    *parent,
        thc::ChessRules *cr,
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

    // DbDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );
    void OnActivate(wxActivateEvent& event);
    void OnListSelected( wxListEvent &event );
    void OnListFocused( wxListEvent &event );
    void OnListColClick( wxListEvent &event );
    void OnTabSelected( wxBookCtrlEvent &event );
    void OnNextMove( wxCommandEvent &event );

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
    
    void OnReload( wxCommandEvent& event );
    void OnUtility( wxCommandEvent& event );
    void OnRadio( wxCommandEvent& event );
    void OnSpin( wxCommandEvent& event );
    void OnComboBox( wxCommandEvent& event );
    void OnCheckBox( wxCommandEvent& event );
    
    bool ReadItemFromMemory( int item ); //const
    bool ReadItemFromMemory( int item, DB_GAME_INFO &info );

    void StatsCalculate();
    
//  void OnClose( wxCloseEvent& event );
//  void SaveColumns();
    bool ShowModalOk();

    void LoadGame( int idx, int focus_offset=0 );
    
    // Return true if a game has been selected
    bool LoadGame( GameLogic *gl, GameDocument& gd, int &file_game_idx );

    // Helpers
    GameDocument *GetFocusGame( int &idx );
    void DeselectOthers();
    void OnOk();
    
    // DbDialog member variables
public:
    wxStaticText *player_names;
private:
    wxVirtualListCtrl  *list_ctrl;
    wxNotebook *notebook;
    int          selected_game;
    void         SyncCacheOrderBefore();
    void         SyncCacheOrderAfter();
    void         CopyOrAdd( bool clear_clipboard );
    std::string  CalculateMovesColumn( GameDocument &gd );

    // Data members
    wxCheckBox *filter_ctrl;
    wxRadioButton *radio_ctrl;
    wxComboBox *combo_ctrl;
    wxTextCtrl *text_ctrl;
    wxListBox *list_ctrl_stats;
    wxListBox *list_ctrl_transpo;
    wxButton *utility;
    MiniBoard *mini_board;
    bool activated_at_least_once;
    int nbr_games_in_list_ctrl;

public:
    bool transpo_activated;
private:
    
    wxWindowID  id;
    int file_game_idx;
    bool db_game_set;
    std::vector<DB_GAME_INFO> cache;    // games from database
    std::unordered_set<int>   games_set;    // game_ids for all games in memory
    std::unordered_set<uint64_t> drill_down_set;  // positions already encountered drilling down
    std::vector<thc::Move> moves_in_this_position;
    std::vector<thc::Move> moves_from_base_position;
    GameDocument db_game;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
public:
    std::vector<DB_GAME_INFO> games;    // games being displayed
};

#endif    // DB_DIALOG_H
