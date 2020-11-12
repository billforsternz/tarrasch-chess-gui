/****************************************************************************
 * Custom dialog - list of games from database, file, session, or clipboard
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMES_DIALOG_H
#define GAMES_DIALOG_H
#include <unordered_set>

#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/accel.h"
#include "SuspendEngine.h"
#include "DialogDetect.h"
#include "GamesCache.h"
#include "GameDocument.h"
#include "Repository.h"
#include "CtrlChessBoard.h"
#include "Database.h"

// Control identifiers
enum
{
    ID_DB_CHECKBOX  = 10000,
    ID_DB_CHECKBOX2,
    ID_DB_RADIO ,
    ID_DB_COMBO ,
    ID_DB_SEARCH,
    ID_DB_UTILITY,
    ID_DB_TEXT   ,
    ID_DB_LISTBOX_GAMES,
    ID_DB_LISTBOX_STATS,
    ID_DB_LISTBOX_TRANSPO,
    ID_BUTTON_1,
    ID_BUTTON_2,
    ID_BUTTON_3,
    ID_BUTTON_4,
    ID_BUTTON_5,
    ID_PGN_DIALOG_FILE     ,
    ID_PGN_DIALOG_CURRENT_FILE     ,
    ID_PGN_DIALOG_CLIPBOARD,
    ID_PGN_DIALOG_SESSION  ,
    ID_GAMES_DIALOG_DATABASE   ,
    ID_BOARD2GAME          ,
    ID_TOURNAMENT_GAMES,
    ID_PGN_DIALOG_GAME_DETAILS,
    ID_REORDER           ,
    ID_ADD_TO_CLIPBOARD  ,
    ID_SAVE_ALL_TO_A_FILE,
    ID_SITE_EVENT,
    ID_PGN_DIALOG_GAME_PREFIX,
    ID_PGN_DIALOG_PUBLISH    ,
    ID_PGN_DIALOG_UTILITY1   ,
    ID_PGN_DIALOG_UTILITY2   ,
    ID_DIALOG_ECO
};

// Track the game presented on the mini board
struct MiniBoardGame
{
    thc::ChessPosition  updated_position;
    CompactGame         info;
    int                 focus_idx;
    int                 focus_offset;
};

// Used in moves column sort
struct MoveColCompareElement
{
    int idx;
    int transpo;
    const char *blob;
    uint32_t tie_break;
    uint32_t count;
};

// A GamesListCtrl is a list control within the GamesDialog, it displays a list of games to navigate through and pick from
class GamesDialog;
class GamesListCtrl: public wxListCtrl
{
    DECLARE_EVENT_TABLE()
public:
    GamesListCtrl( GamesDialog *parent, wxWindowID id, const wxPoint &pos, const wxSize &size );
    void OnChar( wxKeyEvent &event );
    
public:
    GamesDialog   *parent;
    int initial_focus_offset;
    CtrlChessBoard *mini_board;
    MiniBoardGame *track;
    
    // Focus changes to new item;
    void ReceiveFocus( int focus_idx );
    
    // Recipes to calculate move text
    std::string CalculateMoveTxt() const;
    std::string CalculateMoveTxt( std::string &previous_move, thc::Move &previous_move_bin  ) const;
    std::string CalculateMoveTxt( CompactGame &info, int offset ) const;
    std::string CalculateMoveTxt( std::string &previous_move, CompactGame &info, int focus_offset, thc::ChessPosition &updated_position, thc::Move &previous_move_bin  ) const;

protected:
    virtual wxString OnGetItemText( long item, long column) const;

private:
    std::map< uint32_t, int > browse_map;   // lookup offset into all games browsed in current session, by uint32_t game_id
};

// A helper, implements custom, non-volatile resizing of games dialogs
class GamesDialogResizer
{
    int window_x, window_y, window_w, window_h;
    int list_x, list_y, list_w, list_h;
    int line_x, line_y, line_w, line_h;
    std::vector<wxPoint>    panel_origins;
    std::vector<wxSize>     panel_sizes;
    std::vector<wxWindow *> panel_windows;
    std::vector<bool>       panel_stretch_widths;
    bool first_time=true;   //C++11 allows this construct which I didn't know about until recently
                            // no need for a constructor if you only need to do things like this
public:
    void RegisterPanelWindow( wxWindow *window, bool stretch_width=false );
    bool Layout( wxWindow *dialog, wxWindow *list, wxWindow *line );
    void AnchorOriginalPositions( wxWindow *dialog, wxWindow *list, wxWindow *line );
    void ReLayout( wxWindow *dialog, wxWindow *list, wxWindow *line );
    void Refresh( wxWindow *dialog, wxWindow *list, wxWindow *line );
};


// GamesDialog class declaration
class GamesDialog: public wxDialog
{    
    DECLARE_CLASS( GamesDialog )
    DECLARE_EVENT_TABLE()

protected:
    thc::ChessRules cr;
    thc::ChessRules cr_base;
    wxStaticText *title_ctrl;
    GamesDialogResizer gdr;
 
public:

    // Track the game presented on the mini board
    MiniBoardGame mini_board_game;
    MiniBoardGame *track;

    // Constructors
    GamesDialog
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

    // GamesDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );
    void OnSize(wxSizeEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnListSelected( wxListEvent &event );
    void OnListFocused( wxListEvent &event );
    void OnListColClick( wxListEvent &event );
    void OnTabSelected( wxBookCtrlEvent &event );
    void OnNextMove( wxCommandEvent &event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnTournamentGames( wxCommandEvent& event );
    void OnBoard2Game( wxCommandEvent& event );
    void OnRenumber( wxCommandEvent& event );
    void OnSiteEvent( wxCommandEvent& event );
    void OnSelectAll( wxCommandEvent& event );
    void OnEditGameDetails( wxCommandEvent& event );
    void OnEditGamePrefix( wxCommandEvent& event );
    void OnCopy( wxCommandEvent& event );
    void OnAddToClipboard( wxCommandEvent& event );
    void OnSaveAllToAFile( wxCommandEvent& event );
    void OnCutOrDelete( bool cut );
    void OnCut( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
    void OnPaste( wxCommandEvent& event );
    void OnSave( wxCommandEvent& event );
    void OnPublish( wxCommandEvent& event );
    void OnEco( wxCommandEvent& event );
    void OnHelpClick( wxCommandEvent& event );
    
    void OnSearch( wxCommandEvent& event );
    void OnUtility( wxCommandEvent& event );
    void OnButton1( wxCommandEvent& event );
    void OnButton2( wxCommandEvent& event );
    void OnButton3( wxCommandEvent& event );
    void OnButton4( wxCommandEvent& event );
    void OnButton5( wxCommandEvent& event );
    void OnRadio( wxCommandEvent& event );
    void OnSpin( wxCommandEvent& event );
    void OnComboBox( wxCommandEvent& event );
    void OnCheckBox( wxCommandEvent& event );
    void OnCheckBox2( wxCommandEvent& event );
    void OnListSelected( int idx );

    void Goto( int idx );
    void ReadItemWithSingleLineCache( int item, CompactGame &info );
    void ColumnSort( int compare_col, std::vector< smart_ptr<ListableGame> > &displayed_games );

    // Overrides - Gdv = Games Dialog Override
    virtual void GdvOnActivate();
    virtual void GdvEnumerateGames()    {}
    virtual wxSizer *GdvAddExtraControls( bool WXUNUSED(big_display) ) { return NULL; }
    virtual void GdvEnableControlsIfGamesFound( bool WXUNUSED(have_games) ) { return; }
    bool dirty;
    virtual bool GdvTestAndClearIsCacheDirty() { bool was=dirty; dirty=false; return was; }
    virtual void GdvReadItem( int item, CompactGame &info ) = 0;
    virtual void GdvOnCancel();
    virtual void GdvListColClick( int compare_col );
    virtual void GdvSaveAllToAFile();
    virtual void GdvHelpClick();
    virtual void GdvCheckBox( bool checked );
    virtual void GdvCheckBox2( bool checked );
    virtual void GdvSearch();
    virtual void GdvUtility();
    virtual void GdvButton1();
    virtual void GdvButton2();
    virtual void GdvButton3();
    virtual void GdvButton4();
    virtual void GdvButton5();
    virtual void GdvNextMove( int idx );
    virtual int  CalculateTranspo( const char *blob, int &transpo );
    virtual int  GetBasePositionIdx( CompactGame &WXUNUSED(pact), bool WXUNUSED(receiving_focus) ) { return 0; }

//  void OnClose( wxCloseEvent& event );
//  void SaveColumns();
    bool ShowModalOk( std::string title );
    
    // Return true if a game has been selected
    bool LoadGameFromPreview(  GameDocument &gd );
    void LoadGameForPreview( int idx, int focus_offset=0 );
 
    // Helpers
    void OnOk();
    void MoveColCompare( std::vector< smart_ptr<ListableGame> > &displayed_games );
    
    // GamesDialog member variables
public:
    wxStaticText *player_names;
    CompactGame   single_line_cache;
    int           single_line_cache_idx;
    int           compare_col;
    
protected:
    wxWindow* parent2;
    wxWindowID id;
    wxPoint pos;
    wxSize size;
    long style;
    DB_REQ db_req;

    GamesListCtrl  *list_ctrl;
    wxBoxSizer*  hsiz_panel;
    //wxBoxSizer *button_panel;
    wxFlexGridSizer* vsiz_panel_buttons;
    
    wxNotebook  *notebook;
    int          selected_game;
    void         CopyOrAdd( bool clear_clipboard );

    // Data members
    wxButton* ok_button;
    wxCheckBox *filter_ctrl;
    wxCheckBox *white_player_ctrl;
    wxRadioButton *radio_ctrl;
    wxComboBox *combo_ctrl;
    wxTextCtrl *text_ctrl;
    wxListBox *list_ctrl_stats;
    wxListBox *list_ctrl_transpo;
    CtrlChessBoard *mini_board;
    bool activated_at_least_once;

private:    //TODO - move more vars to private
    wxStaticLine* line_ctrl;
    bool db_search;
    int col_last_time;
    int col_consecutive;
    int focus_idx;
    bool sort_order_first;

public:
    bool transpo_activated;
    int nbr_games_in_list_ctrl;
    
protected:
    bool preview_game_set;
    GamesCache  *gc;
    GamesCache  *gc_clipboard;
    GameDocument preview_game;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
	DialogDetect    detect;		// similarly the presence of this var allows tracking of open dialogs
};


#endif    // GAMES_DIALOG_H
