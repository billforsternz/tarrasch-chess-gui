/****************************************************************************
 * Implements the top level game logic
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include "Appdefs.h"
#include "Book.h"
#include "PopupControl.h"
#include "GameDocument.h"
#include "thc.h"
#include "GameClock.h"
#include "GameLifecycle.h"
#include "Tabs.h"
#include "PgnFiles.h"
#include "GamesCache.h"
#include "Undo.h"
#include "Atom.h"
#include "kibitzq.h"
#include "PanelContext.h"
#include "GameState.h"
#include "Database.h"

class GameLogic
{
public:

	// Initialise, shutdown
	GameLogic( PanelContext *canvas, CtrlChessTxt *lb );
    void Init();

    // The user hovers over the book moves rectangle
    void BookHover( wxPoint &point );

	// React to a square being clicked
	bool MouseDown( char file, char rank, wxPoint &point );
	void MouseUp( char file, char rank, wxPoint &point );
	void MouseUp();

    // Get blindfold mode for sliding
    bool ShowSlidingPieceOnly();

    // Misc
    bool StartEngine();
    void LabelPlayers( bool start_game=false, bool set_document_player_names=false );
    void SetGroomedPosition( bool show_title=true );
    void SetManual( MoveTree *mt, bool at_move0, bool from_mouse_move=false );
    bool IsManual() {return state==MANUAL;}
    void ShowNewDocument();
    void PutBackDocument();
    void IndicateNoCurrentDocument();
    void GameRedisplayPlayersResult();
    bool InHumanEngineGame( bool &human_is_white );

	// Idle routine
	void OnIdle();
    bool OnIdleNeeded();
    int MillisecsToNextSecond();

    // Command handlers
    void OnTabSelected( int idx );
    void OnTabClose( int idx );
    bool OnExit();
    void CmdFlip();
    void CmdSetPosition();
    void CmdNewGame();
    void CmdTakeback();
    void CmdEditUndo();
    void CmdEditRedo();
    void CmdMoveNow();
    void CmdExamineGame();
    void CmdFileNew();
    void CmdFileOpen();
    void CmdFileOpenLog();
    void CmdFileOpenInner( std::string &filename );
    void CmdFileSave();
    void CmdFileSaveAs();
    void CmdFileSaveGameAs();
    void CmdFileSaveCommon(bool save_as);
    void CmdGamesCurrent();
    void CmdGamesClipboard();
    void CmdGamesSession();
    void CmdGamesDatabase();
    void CmdDatabaseSearch();
    void CmdDatabaseShowAll();
    void CmdDatabasePlayers();
    void CmdDatabaseSelect();
    void CmdDatabaseCreate();
    void CmdDatabaseAppend();
    void CmdDatabasePattern();
    void CmdDatabaseMaterial();

    void CmdDatabase( thc::ChessRules &cr, DB_REQ db_req, PatternParameters *parm=NULL );
    void CmdNextGame();
    void CmdPreviousGame();
    void NextGamePreviousGame( int idx );

    void CmdDraw();
    void CmdWhiteResigns();
    void CmdBlackResigns();
    void CmdPlayWhite();
    void CmdPlayBlack();
    void CmdSwapSides();
    void CmdTabNew();
    void CmdTabClose();
    void CmdTabInclude();
    void KibitzStart();
    void KibitzStop();
    void KibitzTabChanged( bool kibitz_selected );
    void CmdKibitz();
    void CmdClearKibitz( bool hide_window=false );
    void CmdPlayers();
    void CmdClocks();
    void CmdEditGameDetails();
    void CmdEditCopyGamePGNToClipboard();
    void CmdEditGamePrefix();
    void CmdEditPromote();
    void CmdEditDemote();
    void CmdEditPromoteToVariation();
    void CmdEditPromoteRestToVariation();
    void CmdEditDemoteToComment();
    void CmdEditDeleteRestOfVariation();
    void CmdKibitzCaptureOne();
    void CmdKibitzCaptureAll();

    // Update handlers (check whether the menu item really is applicable)
    bool CmdUpdateWhiteResigns();
    bool CmdUpdateBlackResigns();
    bool CmdUpdateTakeback();
    bool CmdUpdateEditUndo();
    bool CmdUpdateEditRedo();
    bool CmdUpdateMoveNow  ();
    bool CmdUpdateFileOpen ();
    bool CmdUpdateFileOpenLog ();
    bool CmdUpdateGamesCurrent ();
    bool CmdUpdateGamesClipboard ();
    bool CmdUpdateGamesSession ();
    bool CmdUpdateNextGame();
    bool CmdUpdatePreviousGame();
    bool CmdUpdateDraw     ();
    bool CmdUpdatePlayWhite();
    bool CmdUpdatePlayBlack();
    bool CmdUpdateSwapSides();
    bool CmdUpdateTabNew();
    bool CmdUpdateTabClose();
    bool CmdUpdateTabInclude();
    bool CmdUpdateKibitz( wxString &txt );
    bool CmdUpdateClearKibitz();
    bool UpdateOptions();
    void SetFocusOnList() { if(canvas) canvas->SetFocusOnList(); }
    void Refresh() { SetGroomedPosition(); NewState(state); }
    bool EngineChanged(); // return true if in-game
    void StatusUpdate( int idx=-1 );
    void StatusWarning();
    bool EditingLog();

private:

    // Do a full undo operation (restore gameplay for non-MANUAL states)
    void FullUndo( GAME_STATE game_state );

    // Popup create/destroy
    void DoPopup( wxPoint &point, std::vector<thc::Move> &target_moves,
                                  std::vector<thc::Move> &book_moves,
                                  POPUP_MODE popup_mode,
                                  wxRect hover );
    bool CheckPopup( thc::Move &move );

    // Make move on board, returns true if gameover
    bool MakeMove( thc::Move move, GAME_RESULT &result );

    // Start engine thinking if after human plays human_move (if not NULL)
    GAME_STATE StartThinking( const thc::Move *human_move );

    // Start engine pondering engine's own proposed ponder move
    bool StartPondering( thc::Move ponder );

    // Update kibitz while human thinking
    void KibitzUpdate( int idx, const char *txt );

    // Update kibitz while engine thinking
    void KibitzUpdateEngineToMove( bool ponder, const char *txt );

	// Set new state
	void NewState( GAME_STATE new_state, bool from_mouse_move=false );
    wxString ponder_nmove_txt;
    GAME_STATE human_or_pondering;  // most recent value of state if state==HUMAN || state==PONDERING
    char src_file;
    char src_rank;
    char slide_buf[80];
    PanelContext *canvas;
    thc::ChessPosition pre_popup_position;
    unsigned long t1_start_delay;
    unsigned long fake_book_delay;
    thc::Move move_after_delay;
    bool under_our_program_control;
    char engine_name[40];
    thc::ChessPosition      initial_position;
    PatternParameters       pp_persist;
    PatternParameters       pp_persist_material_balance;
    wxString reply_to;
    int     human_millisecs_time_start;
    int     engine_millisecs_time_start;
    bool    kibitz;
    bool    kibitz_text_to_clear;
    std::string status_field1;
    std::string status_field2;
    thc::ChessPosition      kibitz_pos;
    thc::Move               kibitz_move[NBR_KIBITZ_LINES];
    std::vector<thc::Move>  kibitz_var[NBR_KIBITZ_LINES];
    int                     kibitz_depth[NBR_KIBITZ_LINES];
    int                     kibitz_rank[NBR_KIBITZ_LINES];
    int                     kibitz_sorted[NBR_KIBITZ_LINES];
    wxString                kibitz_pv[NBR_KIBITZ_LINES];
    void KibitzClearDisplay( bool intro=false );
    void KibitzIntro();
    void KibitzClearMultiPV();

    // public data
public:
    GAME_STATE state;
    CtrlChessTxt *lb;
    Tabs *tabs;
    int file_game_idx;
    uint32_t game_being_edited_tag;
    thc::Move ponder_move;
    GameLifecycle glc;
    GameDocument gd;
    GamesCache gc_pgn;
    GamesCache gc_clipboard;
    GamesCache gc_session;
    GamesCache gc_database;
    PgnFiles pf;
    GameClock chess_clock;
    bool db_clipboard;
    Undo undo;
    Atom atom;
};

#endif // GAMELOGIC_H


