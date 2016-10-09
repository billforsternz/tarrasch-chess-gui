/****************************************************************************
 * Implements the top level game logic
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <stdlib.h>
#include <time.h>
#include "wx/wx.h"
#include "wx/clipbrd.h"
#include "wx/listctrl.h"
#include "wx/filename.h"
#include "wx/utils.h"
#if !wxUSE_THREADS
    #error "Requires thread support!"
#endif // wxUSE_THREADS
#include "wx/thread.h"
#include "Appdefs.h"
#include "PanelContext.h"
#include "thc.h"
#include "Book.h"
#include "GameLogic.h"
#include "Lang.h"
#include "UciInterface.h"
#include "DebugPrintf.h"
#include "CtrlBoxBookMoves.h"
#include "PopupControl.h"
#include "Session.h"
#include "Log.h"
#include "Repository.h"
#include "PositionDialog.h"
#include "PatternDialog.h"
#include "GameDetailsDialog.h"
#include "GamePrefixDialog.h"
#include "CentralWorkSaver.h"
#include "ClockDialog.h"
#include "PlayerDialog.h"
#include "PgnDialog.h"
#include "ClipboardDialog.h"
#include "CreateDatabaseDialog.h"
#include "DbDialog.h"
#include "Objects.h"
#include "CompressMoves.h"
#include "Tabs.h"
#include "Database.h"
using namespace std;
using namespace thc;

// From PrivateChessDefs.h, avoids including whole file
#define FILE(sq)    ( (char) (  ((sq)&0x07) + 'a' ) )           // eg c5->'c'
#define RANK(sq)    ( (char) (  '8' - (((sq)>>3) & 0x07) ) )    // eg c5->'5'

// Stub - just put it anywhere that knows about vector
void ReportOnProgress
(
    bool ,//init,
    int  ,//multipv,
    vector<thc::Move> &, //pv,
    int  ,//   score_cp,
    int   //   depth
)
{
    dbg_printf( "Unexpected call to ReportOnProgress() in GUI rather than engine" );
}


// Initialise the game logic
GameLogic::GameLogic( PanelContext *canvas, CtrlChessTxt *lb )
    : atom(this), gd(this), undo(this)
{
    state    = RESET;
    db_clipboard = false;
    game_being_edited_tag = 0;
    engine_name[0] = '\0';
    under_our_program_control = false;
    kibitz_text_to_clear = false;
    this->canvas = canvas;
    this->lb = lb;
    this->tabs = objs.tabs;
    //human_is_white = false;
    thc::ChessPosition temp;
    gd.Init( temp );
	games_in_file_idx = -1;
    ShowNewDocument();
    if( objs.repository->book.m_enabled )
    {
        wxString error_msg;
        bool error = objs.book->Load( error_msg, objs.repository->book.m_file ); 
        if( error )
            wxMessageBox( error_msg, "Error loading book", wxOK|wxICON_ERROR );
        canvas->BookUpdate( false );
    }
}

bool GameLogic::OnExit()
{
    bool editing_log = objs.gl->EditingLog();
    bool okay = objs.cws->Exit();
    if( okay )
	{	// if exiting
		cprintf( "ChessApp::OnExit(): May wait for tiny database load here...\n" );
		extern wxMutex *KillWorkerThread();
		wxMutex *ptr_mutex_tiny_database = KillWorkerThread();
		wxMutexLocker lock(*ptr_mutex_tiny_database);
		cprintf( "ChessApp::OnExit() if we did wait, that wait is now over\n" );

		objs.log->SaveGame( &gd, editing_log );
	}
    return okay;
}

bool GameLogic::EditingLog()
{
    std::string log_filename(objs.repository->log.m_file.c_str());
    std::string pgn_filename(gc_pgn.pgn_filename.c_str());
    bool editing_log = (log_filename == pgn_filename);
    return editing_log;
}

void GameLogic::CmdFlip()
{
    Atomic begin;
    bool normal = canvas->GetNormalOrientation();
    normal = !normal;
    canvas->SetNormalOrientation( normal );
}

void GameLogic::CmdFileNew()
{
    Atomic begin;
    bool editing_log = objs.gl->EditingLog();
    if( objs.cws->FileNew() )
    {
		// #Workflow gd and gc_pgn cleared
        gc_pgn.gds.clear();
        gc_pgn.pgn_filename = "";
        objs.log->SaveGame( &gd, editing_log );
        objs.session->SaveGame( &gd );
        thc::ChessPosition temp;
        gd.Init( temp );
        ShowNewDocument();
    }
    atom.StatusUpdate();
}

void GameLogic::CmdNewGame()
{
    Atomic begin;
    bool editing_log = objs.gl->EditingLog();
    if( objs.cws->GameNew() )
    {
		// #Workflow) gd cleared, and so doesn't correspond to a file cache gd
        objs.log->SaveGame( &gd, editing_log );
        objs.session->SaveGame( &gd );
        thc::ChessPosition temp;
        gd.Init( temp );
        ShowNewDocument();
    }
    atom.StatusUpdate();
}

void GameLogic::CmdSetPosition()
{
    bool editing_log = objs.gl->EditingLog();
    Atomic begin;
    if( objs.cws->PositionNew() )
    {
        char after[128];
        PositionDialog dialog( objs.frame );
        if( wxID_OK == dialog.ShowModal() )
        {
            strcpy( after, dialog.fen.c_str() );
            thc::ChessPosition pos;
            bool okay = pos.Forsyth( after ); // "7K/8/7k/8/P7/8/8/8 b KQ a3 0 9" );
            if( okay )
            {
				// #Workflow) gd cleared, and so doesn't correspond to a file cache gd
                objs.log->SaveGame( &gd, editing_log );
                objs.session->SaveGame( &gd );
                gd.Init( pos );
                ShowNewDocument();
            }
        }
    }
    atom.StatusUpdate();
}

void GameLogic::ShowNewDocument()
{
    human_or_pondering = HUMAN;
    kibitz = false;
    CmdClearKibitz(true);
    initial_position = gd.start_position;
    canvas->lb->SetGameDocument(&gd);
    thc::ChessRules cr;
    string title;
    gd.Locate( atom.GetInsertionPoint(), cr ); //, title );
    gd.master_position = cr;
    chess_clock.SetDefault();
    canvas->RedrawClocks();
    GAME_RESULT result = RESULT_NONE;
    TERMINAL terminal_score;
//    DRAWTYPE draw_type1, draw_type2;
    bool gameover = false; 
    gd.master_position.Evaluate(terminal_score);
    gameover = (terminal_score==TERMINAL_WCHECKMATE || terminal_score==TERMINAL_BCHECKMATE ||
                terminal_score==TERMINAL_WSTALEMATE || terminal_score==TERMINAL_BSTALEMATE);
    if( gameover )
    {
        switch( terminal_score )
        {
            case TERMINAL_WCHECKMATE:  result = RESULT_WHITE_CHECKMATED;   break;
            case TERMINAL_BCHECKMATE:  result = RESULT_BLACK_CHECKMATED;   break;
            case TERMINAL_WSTALEMATE:  result = RESULT_DRAW_WHITE_STALEMATED;   break;
            case TERMINAL_BSTALEMATE:  result = RESULT_DRAW_BLACK_STALEMATED;   break;
        }
    }
/*  else if( gd.master_position.IsDraw(true,draw_type1) && gd.master_position.IsDraw(false,draw_type2) )
    {
        gameover = true; 
        result = RESULT_DRAW_INSUFFICIENT;
        if( draw_type1==DRAWTYPE_50MOVE || draw_type2==DRAWTYPE_50MOVE )
            result = RESULT_DRAW_50MOVE;
        else if( draw_type1==DRAWTYPE_REPITITION || draw_type2==DRAWTYPE_REPITITION )
            result = RESULT_DRAW_REPITITION;
    } */
    objs.repository->player.m_white = gd.r.white;
    objs.repository->player.m_black = gd.r.black;
    LabelPlayers(false,true);
    if( objs.uci_interface )
    {
        delete objs.uci_interface;  // Allow a different engine to be loaded (if engine option changed)
        objs.uci_interface = NULL;
    }
    engine_name[0] = '\0';
    glc.Set(result);
    NewState( gameover ? GAMEOVER : RESET );
    undo.Clear(gd,state);
    atom.Focus();
    atom.StatusUpdate();
}

void GameLogic::CmdPlayWhite()
{
    Atomic begin;
    bool okay = StartEngine();
    if( okay )
    {
        bool is_new_game_main = gd.AtEndOfMainLine();
        if( !objs.repository->general.m_no_auto_flip )
        {
            if( !objs.canvas->GetNormalOrientation() )
                objs.canvas->SetNormalOrientation(true);
        }
        glc.Begin( true );
        thc::ChessRules cr;
        string move_txt;
        GAME_MOVE *game_move = gd.GetSummaryMove( cr, move_txt );
        chess_clock.NewHumanEngineGame( true, cr.white );
        chess_clock.GetTimes( human_millisecs_time_start, engine_millisecs_time_start );
        if( game_move )
        {
            if( game_move->flag_ingame )
            {
                int white_millisecs_time = game_move->human_millisecs_time;
                int black_millisecs_time = game_move->engine_millisecs_time;
                chess_clock.SetTimes( white_millisecs_time, black_millisecs_time );
            }
        }
		bool show_title=true;
        if( cr.white )
        {
            if( move_txt == "" )
                reply_to = "Make your move";
            else
                reply_to.sprintf( "Reply to %s", move_txt.c_str() );
            NewState( HUMAN );
        }
        else
        {
			show_title = false;
            NewState( StartThinking(NULL) );
        }
        objs.canvas->RedrawClocks();
        SetGroomedPosition(show_title);
        LabelPlayers( true, is_new_game_main );
    }
}

void GameLogic::CmdPlayBlack()
{
    Atomic begin;
    bool okay = StartEngine();
    if( okay )
    {
        bool is_new_game_main = gd.AtEndOfMainLine();
        if( !objs.repository->general.m_no_auto_flip )
        {
            if( objs.canvas->GetNormalOrientation() )
                objs.canvas->SetNormalOrientation(false);
        }
        glc.Begin( false );
        thc::ChessRules cr;
        string move_txt;
        GAME_MOVE *game_move = gd.GetSummaryMove( cr, move_txt );
        chess_clock.NewHumanEngineGame( false, cr.white );
        chess_clock.GetTimes( engine_millisecs_time_start, human_millisecs_time_start );
        if( game_move )
        {
            if( game_move->flag_ingame )
            {
                int white_millisecs_time = game_move->engine_millisecs_time;
                int black_millisecs_time = game_move->human_millisecs_time;
                chess_clock.SetTimes( white_millisecs_time, black_millisecs_time );
            }
        }
		bool show_title=true;
        if( !cr.white )
        {
            if( move_txt == "" )
                reply_to = "Make your move";
            else
                reply_to.sprintf( "Reply to %s", move_txt.c_str() );
            NewState( HUMAN );
        }
        else
		{
			show_title = false;
            NewState( StartThinking(NULL) );
		}
        objs.canvas->RedrawClocks();
        SetGroomedPosition( show_title );
        LabelPlayers( true, is_new_game_main );
    }
}

bool GameLogic::EngineChanged()
{
    bool ingame=false;
    bool restart_kibitz=kibitz;
    if( kibitz )
    {
        kibitz = false;
        CmdClearKibitz();
    }
    if( objs.uci_interface )
    {
        delete objs.uci_interface;
        objs.uci_interface = NULL;
    }
    if( state==HUMAN || state==PONDERING )
    {
        state = HUMAN;
        human_or_pondering = HUMAN;
        bool okay = StartEngine();
        if( !okay )
            NewState(MANUAL);
        else
        {
            ingame = true;
            if( glc.human_is_white )
                objs.repository->player.m_black = engine_name;
            else
                objs.repository->player.m_white = engine_name;
            LabelPlayers();
        }
    }
    if( restart_kibitz )
        CmdKibitz();
    return ingame;
}

void GameLogic::CmdTabNew()
{
    Atomic begin;
    GameDocument temp = gd;
    PutBackDocument();
    {
        GameDocument blank;
        tabs->TabNew(blank);
        ShowNewDocument();
        atom.NotUndoAble();  // don't save an undo position
    }
    objs.session->SaveGame(&temp);      // ...modify session only after loading old game
}

void GameLogic::OnTabSelected( int idx )
{
    cprintf( "OnTabSelected(%d)\n",idx);
    Atomic begin;
    if( tabs->TabSelected(idx) )
    {
        Undo temp = undo;
        ShowNewDocument();   // clears undo
        undo = temp;
        atom.NotUndoAble();  // don't save an undo position
    }
    undo.ShowStackSize( "OnTabSelected()" );
}


#ifdef AUI_NOTEBOOK
void GameLogic::OnTabClose( int idx )
{
    cprintf( "OnTabClose(%d)\n",idx);
    Atomic begin;
    tabs->TabDelete(idx,true);    // true means system deletes the notebook page
/*    int current_idx = tabs->GetCurrentIdx();
    if( tabs->TabSelected( current_idx ) )
    {
        Undo temp = undo;
        ShowNewDocument();   // clears undo
        undo = temp;
        atom.NotUndoAble();  // don't save an undo position
    } */
    undo.ShowStackSize( "OnTabClose()" );
}


void GameLogic::CmdTabClose()
{
    cprintf( "CmdTabClose()\n");
    Atomic begin;
    int current_idx = tabs->GetCurrentIdx();
    tabs->TabDelete(current_idx,false);    // false means system doesn't delete the notebook page
/*    current_idx = tabs->GetCurrentIdx();
    if( tabs->TabSelected( current_idx ) )
    {
        Undo temp = undo;
        ShowNewDocument();   // clears undo
        undo = temp;
        atom.NotUndoAble();  // don't save an undo position
    } */
    undo.ShowStackSize( "CmdTabClose()" );
}

#else
void GameLogic::CmdTabClose()
{
    Atomic begin;
	int nbr_tabs = tabs->GetNbrTabs();
	int current_idx = tabs->GetCurrentIdx();
	if( nbr_tabs>1 && current_idx<nbr_tabs )
	{
		bool editing_log = objs.gl->EditingLog();
		if( objs.cws->FileNew() )
		{
			objs.log->SaveGame( &gd, editing_log );
			objs.session->SaveGame( &gd );
			tabs->TabSelected( tabs->TabDelete() );
			Undo temp = undo;
			ShowNewDocument();  // clears undo
			undo = temp;
			atom.NotUndoAble(); // don't save an undo position
			undo.ShowStackSize( "CmdTabClose()" );
		}
	}
    atom.StatusUpdate();
}
#endif

void GameLogic::CmdTabInclude()
{
    Atomic begin;
    objs.cws->AddTabToFile();
    atom.StatusUpdate();
}

void GameLogic::CmdSwapSides()
{
    Atomic begin;
    if( state==HUMAN || state==PONDERING )
    {
        if( !objs.repository->general.m_no_auto_flip )
        {
            bool normal = objs.canvas->GetNormalOrientation();
            objs.canvas->SetNormalOrientation(!normal);
        }
        SetGroomedPosition();
        thc::ChessRules cr;
        string last_move_txt;
        GAME_MOVE *last_move = gd.GetSummary( cr, last_move_txt );
        glc.Swap();
        wxString temp                   = objs.repository->player.m_black;
        objs.repository->player.m_black = objs.repository->player.m_white;
        objs.repository->player.m_white = temp;
        chess_clock.Swap( glc.human_is_white );
        LabelPlayers();
        if( last_move )
        {
            long pos = atom.GetInsertionPoint();
            std::string txt;
            if( last_move->comment=="" )
                txt = "Swap sides, now ";
            else
                txt = ". Swap sides, now ";
            txt += objs.repository->player.m_white;
            txt += "-";
            txt += objs.repository->player.m_black;
            last_move->comment += txt;
            gd.Rebuild();
            gd.Display(pos);
        }
        NewState( StartThinking(NULL) );
    }
}

void GameLogic::KibitzStart()
{
    wxString intro = "Kibitzing starting up";
    canvas->Kibitz( 0, intro );
    canvas->kibitz_ctrl->Show(true );
    canvas->Update();
    bool okay = true;
    if( state!=THINKING && state!=PONDERING )
    {
        okay = StartEngine();
        if( !okay )
            canvas->Kibitz( 0, "" );
        else
        {
            char buf[128];
            strcpy( buf, gd.master_position.ForsythPublish().c_str() );
            thc::ChessPosition pos = gd.master_position;
            objs.uci_interface->Kibitz( pos, buf );
        }
    }
    if( okay )
    {
        kibitz = true;
        kibitz_pos = gd.master_position;
        objs.uci_interface->SuspendResume(true);
        KibitzClearDisplay( true );
    }
    canvas->SetKibitzButtons( kibitz );
}

void GameLogic::KibitzStop()
{
    wxString intro;
    if( state==THINKING || state==PONDERING )
        intro.sprintf( "Analysis by %s [stopped]", engine_name );
    else
        intro.sprintf( "Kibitzing by %s [stopped]", engine_name );
    canvas->Kibitz( 0, intro );
    kibitz = false;
    if( state!=THINKING && state!=PONDERING )
        objs.uci_interface->KibitzStop();
    canvas->SetKibitzButtons( kibitz );
}

void GameLogic::KibitzTabChanged( bool kibitz_selected )
{
    if( kibitz_selected )
    {
        if( !kibitz )
            KibitzStart();
    }
    else
    {
        if( kibitz )
            KibitzStop();
    }
}

void GameLogic::CmdKibitz()
{
    Atomic begin;
    wxString dummy;
    if( CmdUpdateKibitz(dummy) )
    {
        canvas->SetKibitzPage();
        if( !kibitz )
            KibitzStart();
        else
            KibitzStop();
    }
}

void GameLogic::CmdClearKibitz( bool WXUNUSED(hide_window) )
{
    Atomic begin;
    if( !kibitz )
        canvas->Kibitz( 0, "" );
    KibitzClearDisplay();
    KibitzClearMultiPV();
    kibitz_text_to_clear = false;
    canvas->SetKibitzButtons( kibitz );
}

void GameLogic::CmdMoveNow()
{
    Atomic begin;
    if( state==THINKING && objs.uci_interface )
        objs.uci_interface->MoveNow();
    else if( state == FAKE_BOOK_DELAY )
        fake_book_delay = 0;
}

void GameLogic::CmdExamineGame()
{
    Atomic begin;
    NewState( MANUAL ); // same as simply clicking in game
}


void GameLogic::CmdFileOpen()
{
    Atomic begin;
    if( objs.cws->FileOpen() )
    {
        wxFileDialog fd( objs.frame, "Select .pgn file", "", "", "*.pgn", wxFD_FILE_MUST_EXIST );//|wxFD_CHANGE_DIR );
        wxString dir = objs.repository->nv.m_doc_dir;
        fd.SetDirectory(dir);
        if( wxID_OK == fd.ShowModal() )
        {
            wxString dir2;
            wxFileName::SplitPath( fd.GetPath(), &dir2, NULL, NULL );
            objs.repository->nv.m_doc_dir = dir2;
            wxString wx_filename = fd.GetPath();
            std::string filename( wx_filename.c_str() );
            CmdFileOpenInner( filename );
        }
    }
    atom.StatusUpdate();
}

void GameLogic::CmdFileOpenLog()
{
    Atomic begin;
    string filename( objs.repository->log.m_file.c_str() );
    if( filename == "" )
    {
        wxMessageBox( "No log file specified, use options menu to solve this problem", "Cannot read log file", wxOK|wxICON_ERROR );
    }
    else if( !::wxFileExists(filename) )
    {
        wxString msg;
        msg.sprintf( "Cannot read from file %s", filename.c_str() );
        wxMessageBox( msg, "Cannot read log file", wxOK|wxICON_ERROR );
    }
    else
    {
        if( objs.cws->FileOpen() )
            CmdFileOpenInner( filename );
    }
    atom.StatusUpdate();
}


void GameLogic::CmdFileOpenInner( std::string &filename )
{
    Atomic begin;
    bool is_empty = gd.IsEmpty();
    bool editing_log = objs.gl->EditingLog();
    if( !gc_pgn.Load(filename) )
        wxMessageBox( "Cannot read file", "Error", wxOK|wxICON_ERROR );
    else
    {
        if(
            ( gc_pgn.gds.size()==1 && objs.repository->general.m_straight_to_game )  ||
            ( gc_pgn.gds.size()>0  && objs.repository->general.m_straight_to_first_game )
          )
        {
			objs.log->SaveGame(&gd, editing_log);
			objs.session->SaveGame(&gd);

			// Upgrade first element of games cache to a GameDocument, if necessary
			GameDocument *gd_file = gc_pgn.gds[0]->IsGameDocument();
			GameDocument new_gd;
			if (gd_file)
				new_gd = *gd_file;
			else
			{
				gc_pgn.gds[0]->ConvertToGameDocument(new_gd);
				make_smart_ptr(GameDocument, new_smart_ptr, new_gd);
				gc_pgn.gds[0] = std::move(new_smart_ptr);
			}

			// #Workflow)  Open file go straight to game, game opens in new tab
			// Set edit correspondence between new_gd and game in file cache
            SetGameBeingEdited( new_gd, *gc_pgn.gds[0] );
            if( is_empty )
                gd = new_gd;
            else
                tabs->TabNew(new_gd);
            ShowNewDocument();
        }
		else
        {
            wxPoint pt(0,0);
            wxSize sz = objs.frame->GetSize();
            sz.x = (sz.x*9)/10;
            sz.y = (sz.y*9)/10;
            PgnDialog dialog( objs.frame, &gc_pgn, &gc_clipboard, ID_PGN_DIALOG_FILE, pt, sz );   // GamesDialog instance
            std::string title = "File: " + filename;
            if( !dialog.ShowModalOk(title) )
			{
#define CANCEL_PGN_DIALOG_CANCELS_FILE	// if this is defined, open file -> pgn dialog -> cancel doesn't load the file
#ifdef CANCEL_PGN_DIALOG_CANCELS_FILE
				gc_pgn.gds.clear();
				gc_pgn.pgn_filename = "";
#endif
			}
			else
            {
                objs.log->SaveGame(&gd,editing_log);
                objs.session->SaveGame(&gd);
                GameDocument new_gd;

				// #Workflow)  Open file and select new game from list, game opens in new tab
				// [edit correspondence between new_gd and game in file cache is created inside LoadGame()]
                if( dialog.LoadGame(this,new_gd) )
                {
                    if( is_empty )
                        gd = new_gd;
                    else
                        tabs->TabNew(new_gd);
                    ShowNewDocument();
                }
            }
        }
    }
    atom.StatusUpdate();
}


bool GameLogic::CmdUpdateNextGame()
{
    int nbr = gc_pgn.gds.size();
    return( nbr>1 && games_in_file_idx!=-1 && 0<=games_in_file_idx+1 && games_in_file_idx+1<nbr );
}

bool GameLogic::CmdUpdatePreviousGame()
{
    int nbr = gc_pgn.gds.size();
    return( nbr>1 && games_in_file_idx!=-1 && 0<=games_in_file_idx-1 && games_in_file_idx-1<nbr );
}

void GameLogic::NextGamePreviousGame( int idx )
{
    Atomic begin;

	// If the next game is already present in a tab, switch to it
//#define CHANGE_TABS_ON_NEXT_PREVIOUS_GAME
#ifdef CHANGE_TABS_ON_NEXT_PREVIOUS_GAME
	bool switch_tabs = false;
	int tab_idx = 0;
#endif

	// Upgrade next/previous element of games cache to a GameDocument, if necessary
	GameDocument *gd_file = gc_pgn.gds[idx]->IsGameDocument();
	GameDocument new_gd;

	// If not necessary, check to see whether game is already in a tab
	if( gd_file )
	{
		new_gd = *gd_file;
#ifdef CHANGE_TABS_ON_NEXT_PREVIOUS_GAME
        GameDocument *pd;
        Undo *pu;
		int handle = tabs->Iterate(0,pd,pu);
        while( pd && pu )
        {
            if( gd_file->game_being_edited>0 && gd_file->game_being_edited==pd->game_being_edited )
			{
				if( tab_idx != tabs->GetCurrentIdx() )
					switch_tabs = true;
            }
			if( !switch_tabs )
				tab_idx++;
			tabs->Iterate(handle,pd,pu);
        }
#endif
	}
	else
	{
		gc_pgn.gds[idx]->ConvertToGameDocument(new_gd);
		make_smart_ptr(GameDocument, new_smart_ptr, new_gd);
		gc_pgn.gds[idx] = std::move(new_smart_ptr);
	}

#ifdef CHANGE_TABS_ON_NEXT_PREVIOUS_GAME
	if( switch_tabs )
	{
		canvas->notebook->ChangeSelection(tab_idx);
		if( tabs->TabSelected(tab_idx) )
		{
			Undo temp = undo;
			ShowNewDocument();   // clears undo
			undo = temp;
			atom.NotUndoAble();  // don't save an undo position
		}
	}
	else
#endif
	{
		bool editing_log = objs.gl->EditingLog();
		PutBackDocument();
		objs.log->SaveGame(&gd, editing_log);
		objs.session->SaveGame(&gd);

		// #Workflow)
		// Set new edit correspondence, gd to next/previous game in file
		gd = new_gd;
		SetGameBeingEdited( gd, *gc_pgn.gds[idx] );
		ShowNewDocument();
	}
    atom.StatusUpdate();
}

void GameLogic::CmdNextGame()
{
    if( CmdUpdateNextGame() )
    {
        int idx = games_in_file_idx+1;
        NextGamePreviousGame(idx);
        StatusUpdate(idx);
    }
}

void GameLogic::CmdPreviousGame()
{
    if( CmdUpdatePreviousGame() )
    {
        int idx = games_in_file_idx-1;
        NextGamePreviousGame(idx);
        StatusUpdate(idx);
    }
}

// Remove space from beginning and end
void trim( std::string &s )
{
    int len = s.length();
    int sublen=len;
    int idx=0;
    for( int i=0; i<len; i++ )
    {
        char c = s[i];
        if( c != ' ' )
            break;
        else
        {
            sublen--;
            idx++;
        }
    }
    for( int i=len-1; sublen && i>=0; i-- )
    {
        char c = s[i];
        if( c == ' ' )
            sublen--;
        else
            break;
    }
    s = s.substr(idx,sublen);
}

// #Workflow) Before moving on to a new tab or game we put the current game back in the file cache if it belongs
//  there (i.e. if we are editing a file game). The current working file can have multiple edited games sitting
//  at once in the cache.
//
//  Don't do this for games from other caches - eg session, clipboard. We avoid losing edits to those games by
//  loading games into new tabs except when we are editing a file game.
void GameLogic::PutBackDocument()
{
	cprintf( "PutBackDocument 0\n" );
    for( size_t i=0; i<gc_pgn.gds.size(); i++ )
    {
        smart_ptr<ListableGame> smp = gc_pgn.gds[i];
        if( smp->GetGameBeingEdited() == gd.game_being_edited && (gd.game_being_edited!=0) )
        {
            gd.FleshOutDate();
            gd.FleshOutMoves();

			// #Workflow) The version of the file we put back into the cache includes the modified flag, which
			//  captures current modifications (from undo) but also historic modifications - for example if we
			//  keep returning to the game with next/previous buttons without doing more edits. Hence |= (OR=)
			//  operator below.
			GameDocument new_gd=gd;
			new_gd.modified |= undo.IsModified();
            make_smart_ptr( GameDocument, new_smart_ptr, new_gd );

			// #Workflow) Set edited game relationship remains
            gc_pgn.gds[i] = std::move(new_smart_ptr);
			SetGameBeingEdited( gd, *gc_pgn.gds[i] );
            return;
        }
    }
}

void GameLogic::CmdGamesCurrent()
{
    Atomic begin;
    bool editing_log = objs.gl->EditingLog();
    bool ok=CmdUpdateGamesCurrent();
    if( ok )
    {
        if( !gc_pgn.IsLoaded() )
        {
            if( !gc_pgn.Reload() )
            {
                ok = false;
                wxMessageBox( "Cannot read any games", "Error reloading file", wxOK|wxICON_ERROR );
            }
        }
    }
    if( ok )
    {
        wxPoint pt(0,0);
        wxSize sz = objs.frame->GetSize();
        sz.x = (sz.x*9)/10;
        sz.y = (sz.y*9)/10;
        gc_pgn.Debug( "Before loading current file games dialog" );
        PgnDialog dialog( objs.frame, &gc_pgn, &gc_clipboard, ID_PGN_DIALOG_FILE, pt, sz );   // GamesDialog instance
        if( dialog.ShowModalOk("Current file") )
        {
            objs.log->SaveGame(&gd,editing_log);
            objs.session->SaveGame(&gd);
            PutBackDocument();
			bool game_in_file =	gc_pgn.TestGameInCache(gd);

			// #Workflow)  Open file and select new game from list, game opens in same tab, unless not editing a file game
			//  (Don't want to overwrite a non file game [say an edited database game] - we could lose work unprompted if we do that)
			// [edit correspondence between new_gd and game in file cache is created inside LoadGame()]
            GameDocument new_gd;
            if( dialog.LoadGame(this,new_gd) )
            {
				if( game_in_file || gd.IsEmpty()  )
                    gd = new_gd;
                else
                    tabs->TabNew(new_gd);
                ShowNewDocument();
            }
        }
    }
    atom.StatusUpdate();
}

void GameLogic::CmdGamesDatabase()
{
    thc::ChessRules cr;
    thc::ChessRules start_position;
    std::string title_txt;
    gd.GetSummary( cr, title_txt );
    DB_REQ db_req;
    if( cr == start_position )
        db_req = REQ_SHOW_ALL;
    else
        db_req = REQ_POSITION;
    CmdDatabase( cr, db_req );
}

void GameLogic::CmdDatabaseSearch()
{
    thc::ChessRules cr;
    thc::ChessRules start_position;
    std::string title_txt;
    gd.GetSummary( cr, title_txt );
    if( cr == start_position )
        CmdDatabase( cr, REQ_SHOW_ALL );
    else
        CmdDatabase( cr, REQ_POSITION );
}

void GameLogic::CmdDatabasePattern()
{
    thc::ChessRules cr;
    std::string title_txt;
    gd.GetSummary( cr, title_txt );
    pp_persist.OneTimeInit(false,cr);
    PatternParameters parm = pp_persist;
    bool do_search = false;
    {   // scope controls when dialog closes
        PatternDialog dialog( &parm, objs.frame, "Search database for games where pieces reached this position", ID_PATTERN_DIALOG );
        if( wxID_OK == dialog.ShowModal() )
        {
            pp_persist = parm;
            cr = parm.cp;
            do_search = true;
        }
    }
    if( do_search )
    {
        CmdDatabase( cr, REQ_PATTERN, &parm );
    }
}

void GameLogic::CmdDatabaseMaterial()
{
    thc::ChessRules cr;
    std::string title_txt;
    gd.GetSummary( cr, title_txt );
    pp_persist_material_balance.OneTimeInit(true,cr);
    PatternParameters parm = pp_persist_material_balance;
    bool do_search = false;
    {
        PatternDialog dialog( &parm, objs.frame, "Search database for games which reached this material balance", ID_MATERIAL_BALANCE_DIALOG );
        if( wxID_OK == dialog.ShowModal() )
        {
            pp_persist_material_balance = parm;
            cr = parm.cp;
            do_search = true;
        }
    }
    if( do_search )
    {
        CmdDatabase( cr, REQ_PATTERN, &parm );
    }
}

void GameLogic::CmdDatabaseShowAll()
{
    thc::ChessRules cr;
    std::string title_txt;
    CmdDatabase( cr, REQ_SHOW_ALL );
}

void GameLogic::CmdDatabasePlayers()
{
    thc::ChessRules cr;
    std::string title_txt;
    CmdDatabase( cr, REQ_PLAYERS );
}

void GameLogic::CmdDatabase( thc::ChessRules &cr, DB_REQ db_req, PatternParameters *parm )
{
    static int count;
    int temp = ++count;
    cprintf( "CmdDatabase(): May wait for tiny database load here (%d)...\n", temp );
    extern wxMutex *WaitForWorkerThread();
    wxMutex *ptr_mutex_tiny_database = WaitForWorkerThread();
    wxMutexLocker lock(*ptr_mutex_tiny_database);
    {
        cprintf( "...CmdDatabase() if we did wait, that wait is now over (%d)\n", temp );
        std::string error_msg;
        bool operational = objs.db->IsOperational(error_msg);
        if( !operational )
        {
            wxMessageBox(
                "The database is not currently running. To correct this select a database "
                "using the database menu. If you don't have a database, the database menu "
                "also offers a command to create a database.", "Database problem", wxOK|wxICON_ERROR
            );
        }
        else
        {
            Atomic begin;
            bool editing_log = objs.gl->EditingLog();
            std::vector<GameDocument> games;
            wxPoint pt(0,0);
            wxSize sz = objs.frame->GetSize();
            sz.x = (sz.x*9)/10;
            sz.y = (sz.y*9)/10;
            DbDialog dialog( objs.frame, &cr, &gc_database, &gc_clipboard, db_req, parm, ID_GAMES_DIALOG_DATABASE, pt, sz );   // GamesDialog instance
            if( dialog.ShowModalOk("Database games") )
            {
                objs.log->SaveGame(&gd,editing_log);
                //objs.session->SaveGame(&gd);        //careful...
                GameDocument temp2 = gd;
                GameDocument new_gd;
                PutBackDocument();

				// #Workflow)  Pull in game from database, opens in new tab
				// No edit correspondence to games in game caches except for pgn cache, i.e. current working file
				if( dialog.LoadGameFromPreview(new_gd) )
                {
					if( gd.IsEmpty() )
                        gd = new_gd;
                    else
                        tabs->TabNew(new_gd);
                    ShowNewDocument();
                }
                objs.session->SaveGame(&temp2);      // ...modify session only after loading old game
            }
            atom.StatusUpdate();
        }
    }
}

void GameLogic::CmdDatabaseSelect()
{
    wxFileDialog fd( objs.frame, "Select current database", "", "", "*.tdb", wxFD_FILE_MUST_EXIST );
    wxString path( objs.repository->database.m_file );
    fd.SetPath(path);
    if( wxID_OK == fd.ShowModal() )
    {
        static int count;
        int temp = ++count;
        cprintf( "CmdDatabaseSelect(): May wait for tiny database load here (%d)...\n", temp );
        extern wxMutex *WaitForWorkerThread();
        wxMutex *ptr_mutex_tiny_database = WaitForWorkerThread();
        wxMutexLocker lock(*ptr_mutex_tiny_database);
        {
            cprintf( "...CmdDatabaseSelect() if we did wait, that wait is now over (%d)\n", temp );
            wxString s = fd.GetPath();
            wxString previous = objs.repository->database.m_file;
            const char *filename = s.c_str();
            cprintf( "File is %s\n", filename );
            objs.db->Reopen(filename);
            std::string error_msg;
            bool operational = objs.db->IsOperational(error_msg);
            if( operational )
                objs.repository->database.m_file = s;
            else
            {
                objs.db->Reopen(previous);
                wxMessageBox( error_msg.c_str(), "Database selection failed", wxOK|wxICON_ERROR );
            }
        }
    }
}

void GameLogic::CmdDatabaseCreate()
{
    Atomic begin;
    wxString db_name;
    bool ok=false;
    {
        CreateDatabaseDialog dialog( objs.frame, ID_CREATE_DB_DIALOG, true ); // create_mode = true
        dialog.ShowModal();
        ok = dialog.db_created_ok;
        wxString s(dialog.db_name.c_str());
        db_name = s;
    }
    if( ok )
    {
        int answer = wxMessageBox( "Would you like to use the new database now?", "Press Yes to set the new database as the current database",  wxYES_NO|wxCANCEL );
        bool set_current = (answer == wxYES);
        if( set_current )
        {
            static int count;
            int temp = ++count;
            cprintf( "CmdDatabaseCreate(): May wait for tiny database load here (%d)...\n", temp );
            extern wxMutex s_mutex_tiny_database;
            wxMutexLocker lock(s_mutex_tiny_database);
            {
                cprintf( "...CmdDatabaseCreate() if we did wait, that wait is now over (%d)\n", temp );
                wxString previous = objs.repository->database.m_file;
                const char *filename = db_name.c_str();
                cprintf( "File is %s\n", filename );
                objs.db->Reopen(filename);
                std::string error_msg;
                bool operational = objs.db->IsOperational(error_msg);
                if( operational )
                    objs.repository->database.m_file = db_name;
                else
                {
                    objs.db->Reopen(previous);
                    wxMessageBox( error_msg.c_str(), "Database selection failed", wxOK|wxICON_ERROR );
                }
            }
        }
    }
    atom.StatusUpdate();
}

void GameLogic::CmdDatabaseAppend()
{
    Atomic begin;
    bool ok=false;
    wxString db_name;
    {
        CreateDatabaseDialog dialog( objs.frame, ID_CREATE_DB_DIALOG, false ); // create_mode = false
        dialog.ShowModal();
        ok = dialog.db_created_ok;
        wxString s(dialog.db_name.c_str());
        db_name = s;
    }
    if( ok )
    {
        int answer = wxMessageBox( "Would you like to use the new database now?", "Press Yes to set the new database as the current database",  wxYES_NO|wxCANCEL );
        bool set_current = (answer == wxYES);
        if( set_current )
        {
            static int count;
            int temp = ++count;
            cprintf( "CmdDatabaseCreate(): May wait for tiny database load here (%d)...\n", temp );
            extern wxMutex s_mutex_tiny_database;
            wxMutexLocker lock(s_mutex_tiny_database);
            {
                cprintf( "...CmdDatabaseCreate() if we did wait, that wait is now over (%d)\n", temp );
                wxString previous = objs.repository->database.m_file;
                const char *filename = db_name.c_str();
                cprintf( "File is %s\n", filename );
                objs.db->Reopen(filename);
                std::string error_msg;
                bool operational = objs.db->IsOperational(error_msg);
                if( operational )
                    objs.repository->database.m_file = db_name;
                else
                {
                    objs.db->Reopen(previous);
                    wxMessageBox( error_msg.c_str(), "Database selection failed", wxOK|wxICON_ERROR );
                }
            }
        }
    }
    atom.StatusUpdate();
}


void GameLogic::CmdGamesSession()
{
    Atomic begin;
    bool editing_log = objs.gl->EditingLog();
    bool ok=CmdUpdateGamesSession();
    if( ok )
    {
        wxPoint pt(0,0);
        wxSize sz = objs.frame->GetSize();
        sz.x = (sz.x*9)/10;
        sz.y = (sz.y*9)/10;
        gc_session.Debug( "Before loading session games dialog" );
        PgnDialog dialog( objs.frame, &gc_session, &gc_clipboard, ID_PGN_DIALOG_SESSION, pt, sz );   // GamesDialog instance
        if( dialog.ShowModalOk("Games in this session") )
        {
            objs.log->SaveGame(&gd,editing_log);
            //objs.session->SaveGame(&gd);        //careful...
            GameDocument temp = gd;
            PutBackDocument();

			// #Workflow)  Select game from session list, game opens in new tab
			// [No edit correspondence between new_gd and game in session cache created inside LoadGame()
			//   since it's the session cache not the file cache]
            GameDocument new_gd;
            if( dialog.LoadGame(this,new_gd) )
            {
                if( gd.IsEmpty() )
                    gd = new_gd;
                else
                    tabs->TabNew(new_gd);
                ShowNewDocument();
            }
            objs.session->SaveGame(&temp);      // ...modify session only after loading old game
        }
    }
    atom.StatusUpdate();
}

void GameLogic::CmdGamesClipboard()
{
    Atomic begin;
    bool editing_log = objs.gl->EditingLog();
    bool ok=CmdUpdateGamesClipboard();
    if( ok )
    {
        wxPoint pt(0,0);
        wxSize sz = objs.frame->GetSize();
        sz.x = (sz.x*9)/10;
        sz.y = (sz.y*9)/10;
        ClipboardDialog dialog( objs.frame, &gc_clipboard, &gc_clipboard, ID_PGN_DIALOG_CLIPBOARD, pt, sz );   // GamesDialog instance
        if( dialog.ShowModalOk("Clipboard") )
        {
            objs.log->SaveGame(&gd,editing_log);
            objs.session->SaveGame(&gd);
            PutBackDocument();

			// #Workflow)  Select game from clipboard list, game opens in new tab
			// [No edit correspondence between new_gd and game in clipboard cache created inside LoadGame()
			//   since it's the clipboard cache not the file cache]
            GameDocument new_gd;
            if( dialog.LoadGame(this,new_gd) )
            {
                if( gd.IsEmpty() )
                    gd = new_gd;
                else
                    tabs->TabNew(new_gd);
                ShowNewDocument();
            }
        }
    }
    atom.StatusUpdate();
}

void GameLogic::CmdFileSave()
{
    Atomic begin;
    objs.cws->FileSave();
    atom.StatusUpdate();
}

void GameLogic::CmdFileSaveAs()
{
    Atomic begin;
    objs.cws->FileSaveAs();
    atom.StatusUpdate();
}

void GameLogic::CmdFileSaveGameAs()
{
    Atomic begin;
    objs.cws->FileSaveGameAs();
    atom.StatusUpdate();
}

void GameLogic::CmdEditUndo()
{
    if( CmdUpdateEditUndo() )
    {
        if( state!=MANUAL && state!=RESET )
            CmdTakeback();
        else
        {
            Atomic begin;
            undo.DoUndo(gd);
            FullUndo( MANUAL );
            atom.NotUndoAble();
            atom.StatusUpdate();
        }
    }
}

void GameLogic::CmdEditRedo()
{
    if( CmdUpdateEditRedo() )
    {
        Atomic begin;
        GAME_STATE game_state = undo.DoRedo(gd);
        FullUndo( game_state );
        atom.NotUndoAble();
        atom.StatusUpdate();
    }
}

void GameLogic::CmdTakeback()
{
    Atomic begin;
    GAME_STATE game_state = undo.DoUndo(gd,true);
    FullUndo( game_state );
    atom.NotUndoAble();
    atom.StatusUpdate();
}

void GameLogic::FullUndo( GAME_STATE game_state )
{
    unsigned long pos = atom.GetInsertionPoint();
    std::string move_txt;
    bool at_move0;
    thc::ChessRules cr;
    MoveTree *found = gd.Locate( pos, cr, move_txt, at_move0 );
    switch( game_state )
    {
        case RESET:
        case MANUAL:            // Human but moving manually rather than playing against engine
        case SLIDING:           // Sliding
        case SLIDING_MANUAL:
        case POPUP:
        case POPUP_MANUAL:
        case GAMEOVER:
        {
            bool game_over = (game_state==GAMEOVER);
            SetManual(found,at_move0);
            if( game_over )
                NewState(GAMEOVER);
            break;
        }
        case PONDERING:         // Human move, but the engine is thinking
        case HUMAN:		        // Human move
        {
            if( objs.uci_interface )
                objs.uci_interface->ForceStop();
            int white_millisecs_time;
            int black_millisecs_time;
            if( glc.human_is_white )
            {
                white_millisecs_time = human_millisecs_time_start;
                black_millisecs_time = engine_millisecs_time_start;
            }
            else
            {
                white_millisecs_time = engine_millisecs_time_start;
                black_millisecs_time = human_millisecs_time_start;
            }
            thc::ChessRules cr2;
            std::string move_txt2;
            gd.GetSummaryMove( cr2, move_txt2 );
            if( !found || move_txt2=="" )
                reply_to = "Make your move";
            else    
            {
                reply_to.sprintf( "Reply to %s", move_txt2.c_str() );
                if( found && found->game_move.flag_ingame )
                {
                    if( glc.human_is_white )
                    {
                        white_millisecs_time = found->game_move.human_millisecs_time;
                        black_millisecs_time = found->game_move.engine_millisecs_time;
                    }
                    else
                    {
                        white_millisecs_time = found->game_move.engine_millisecs_time;
                        black_millisecs_time = found->game_move.human_millisecs_time;
                    }
                }
            }
            chess_clock.SetTimes( white_millisecs_time, black_millisecs_time );
            SetGroomedPosition();
            bool pondering = (game_state==PONDERING);
            if( pondering )
            {
                int ponder_full_move_count = gd.master_position.full_move_count;
                bool ponder_white_to_play = gd.master_position.WhiteToPlay();
                std::string nmove = ponder_move.NaturalOut(&gd.master_position);
                LangOut(nmove);
                ponder_nmove_txt.sprintf( "%d%s%s", ponder_full_move_count, ponder_white_to_play?".":"...",nmove.c_str() );

                // Check that the move to ponder is legal
                bool legal=false;
                vector<thc::Move> moves;
                gd.master_position.GenLegalMoveList( moves );
                for( unsigned int i=0; i<moves.size(); i++ )
                {
                    if( ponder_move == moves[i] )
                    {
                        legal = true;
                        break;
                    }
                }
                if( legal )
                    pondering = StartPondering( ponder_move );
                else
                    release_printf( "pondering failed\n" );
            }
            chess_clock.GameStart( gd.master_position.WhiteToPlay() );
            NewState( pondering ? PONDERING : HUMAN );
            break;
        }

        case THINKING:
        case FAKE_BOOK_DELAY:
        {
            if( objs.uci_interface )
                objs.uci_interface->ForceStop();
            int white_millisecs_time;
            int black_millisecs_time;
            if( glc.human_is_white )
            {
                white_millisecs_time = human_millisecs_time_start;
                black_millisecs_time = engine_millisecs_time_start;
            }
            else
            {
                white_millisecs_time = engine_millisecs_time_start;
                black_millisecs_time = human_millisecs_time_start;
            }
            if( found && found->game_move.flag_ingame )
            {
                if( glc.human_is_white )
                {
                    white_millisecs_time = found->game_move.human_millisecs_time;
                    black_millisecs_time = found->game_move.engine_millisecs_time;
                }
                else
                {
                    white_millisecs_time = found->game_move.engine_millisecs_time;
                    black_millisecs_time = found->game_move.human_millisecs_time;
                }
            }
            chess_clock.SetTimes( white_millisecs_time, black_millisecs_time );
            chess_clock.GameStart( gd.master_position.WhiteToPlay() );
            objs.canvas->RedrawClocks();
            SetGroomedPosition();
            NewState( StartThinking(NULL) );
            break;
        }
    }
}

bool GameLogic::CmdUpdateEditUndo()
{
    bool possible = (undo.IsModified() &&
                        (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==THINKING || state==FAKE_BOOK_DELAY || state==GAMEOVER)
                    );
    return( possible );
}

bool GameLogic::CmdUpdateEditRedo()
{
    bool possible = (undo.CanRedo() &&
                        (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==THINKING || state==FAKE_BOOK_DELAY || state==GAMEOVER)
                    );
    return( possible );
}

bool GameLogic::CmdUpdateTakeback()
{
    bool possible = (undo.IsModified() && 
                        (state==HUMAN || state==PONDERING || state==THINKING || state==FAKE_BOOK_DELAY || state==GAMEOVER)
                    );
    return( possible );
}

void GameLogic::SetManual( MoveTree *mt, bool at_move0, bool from_mouse_move )
{
    //if( mt == NULL )
    //    mt = gd.GetSummary();
    if( mt ) //&& (mt->game_move.flag_ingame) )
    {
        if( at_move0 )
        {
            int human_millisecs_time  = mt->game_move.human_millisecs_time;
            bool white_clock_visible  = mt->game_move.white_clock_visible;
            int engine_millisecs_time = mt->game_move.engine_millisecs_time;
            bool black_clock_visible  = mt->game_move.black_clock_visible;
            thc::ChessRules cr;
            int ivar, imove;
            human_millisecs_time  = human_millisecs_time_start;
            engine_millisecs_time = engine_millisecs_time_start;
            bool using_default_time=true;

            // If possible, get time for previous move in parent variation
            MoveTree *parent = gd.tree.Parent( mt, cr, ivar, imove );
            if( parent )
            {
                MoveTree *grand_parent = gd.tree.Parent( parent, cr, ivar, imove ); 
                if( grand_parent )
                {
                    VARIATION &var = grand_parent->variations[ivar];
                    if( imove>0 && var[imove-1].game_move.flag_ingame )
                    {
                        human_millisecs_time  = var[imove-1].game_move.human_millisecs_time;
                        engine_millisecs_time = var[imove-1].game_move.engine_millisecs_time;
                        using_default_time = false;
                    }
                }
            }
            if( using_default_time && !mt->game_move.flag_ingame )
            {
                white_clock_visible = false;
                black_clock_visible = false;
            }
            if( glc.human_is_white )
            {
                chess_clock.SetStaticTimes( human_millisecs_time,
                                            white_clock_visible,
                                            engine_millisecs_time,
                                            black_clock_visible );
            }
            else
            {
                chess_clock.SetStaticTimes( engine_millisecs_time,
                                            white_clock_visible,
                                            human_millisecs_time,
                                            black_clock_visible );
            }
        }
        else
        {
            if( mt->game_move.human_is_white )
            {
                chess_clock.SetStaticTimes( mt->game_move.human_millisecs_time,
                                            mt->game_move.white_clock_visible,
                                            mt->game_move.engine_millisecs_time,
                                            mt->game_move.black_clock_visible );
            }
            else
            {
                chess_clock.SetStaticTimes( mt->game_move.engine_millisecs_time,
                                            mt->game_move.white_clock_visible,
                                            mt->game_move.human_millisecs_time,
                                            mt->game_move.black_clock_visible );
            }
        }
        canvas->RedrawClocks();
    }
    chess_clock.GameOver();
    NewState( MANUAL, from_mouse_move );
    atom.Focus();
}


void GameLogic::CmdDraw()
{
    Atomic begin;
    if( CmdUpdateDraw() )
    {
        GAME_RESULT result = RESULT_DRAW_AGREED;
        if( state==HUMAN || state==PONDERING )
        {
            DRAWTYPE draw_type;
            if( gd.master_position.IsDraw(glc.human_is_white,draw_type) )
            {
                switch( draw_type )
                {
                    case DRAWTYPE_INSUFFICIENT:    
                    case DRAWTYPE_INSUFFICIENT_AUTO:  result = RESULT_DRAW_INSUFFICIENT;   break;
                    case DRAWTYPE_50MOVE:             result = RESULT_DRAW_50MOVE;         break;
                    case DRAWTYPE_REPITITION:         result = RESULT_DRAW_REPITITION;     break;
                }
            }
        }
        glc.Set( result );
        NewState( GAMEOVER );
        gd.r.result = "1/2-1/2";
        gd.Rebuild();
        unsigned long pos = gd.gv.FindEnd();
        atom.Redisplay( pos );
        atom.Undo();
        atom.StatusUpdate();
    }
}

void GameLogic::CmdWhiteResigns()
{
    Atomic begin;
    if( state==MANUAL || state==RESET ||
            ((state==HUMAN||state==PONDERING) && glc.human_is_white)
      )
    {
        glc.Set( RESULT_WHITE_RESIGNS );
        NewState( GAMEOVER );
        if( gd.AreWeInMain() )
        {
            gd.r.result = "0-1";
            gd.Rebuild();
            unsigned long pos = gd.gv.FindEnd();
            atom.Redisplay( pos );
        }
        atom.Undo();
    }
    atom.StatusUpdate();
}
    
void GameLogic::CmdBlackResigns()
{
    Atomic begin;
    if( state==MANUAL || state==RESET ||
            ((state==HUMAN||state==PONDERING) && !glc.human_is_white)
      )
    {
        glc.Set( RESULT_BLACK_RESIGNS );
        NewState( GAMEOVER );
        if( gd.AreWeInMain() )
        {
            gd.r.result = "1-0";
            gd.Rebuild();
            unsigned long pos = gd.gv.FindEnd();
            atom.Redisplay( pos );
        }
        atom.Undo();
    }
    atom.StatusUpdate();
}

void GameLogic::CmdClocks()
{
    Atomic begin;
    objs.gl->chess_clock.Clocks2Repository();
    ClockConfig cfg = objs.repository->clock;
    cfg.m_engine_minutes = cfg.m_fixed_period_mode ? cfg.m_engine_fixed_minutes : cfg.m_engine_time;
    cfg.m_engine_seconds = cfg.m_fixed_period_mode ? cfg.m_engine_fixed_seconds : cfg.m_engine_increment;
    bool adjust_engine_time = false;
    if( cfg.m_fixed_period_mode && (state==HUMAN||state==PONDERING) )
    {
        if( glc.human_is_white )
        {
            adjust_engine_time = (cfg.m_black_time==cfg.m_engine_fixed_minutes &&
                                  cfg.m_black_secs==cfg.m_engine_fixed_seconds);
        }
        else
        {
            adjust_engine_time = (cfg.m_white_time==cfg.m_engine_fixed_minutes &&
                                  cfg.m_white_secs==cfg.m_engine_fixed_seconds);
        }
    }
    ClockDialog dialog( cfg, objs.frame );
    chess_clock.PauseBegin();
    int ret = dialog.ShowModal();
    chess_clock.PauseEnd();
    if( wxID_OK == ret )
    {
        if( dialog.dat.m_white_time==0 && dialog.dat.m_white_secs==0 )
            dialog.dat.m_white_secs = 1;
        if( dialog.dat.m_black_time==0 && dialog.dat.m_black_secs==0 )
            dialog.dat.m_black_secs = 1;
        if( dialog.dat.m_fixed_period_mode )
        {
            dialog.dat.m_engine_fixed_minutes = dialog.dat.m_engine_minutes;
            dialog.dat.m_engine_fixed_seconds = dialog.dat.m_engine_seconds;
            if( adjust_engine_time )
            {
                if( glc.human_is_white )
                {
                    dialog.dat.m_black_time = dialog.dat.m_engine_fixed_minutes;
                    dialog.dat.m_black_secs = dialog.dat.m_engine_fixed_seconds;
                }
                else
                {
                    dialog.dat.m_white_time = dialog.dat.m_engine_fixed_minutes;
                    dialog.dat.m_white_secs = dialog.dat.m_engine_fixed_seconds;
                }
            }
        }
        else
        {
            dialog.dat.m_engine_time          = dialog.dat.m_engine_minutes;
            dialog.dat.m_engine_increment     = dialog.dat.m_engine_seconds;
        }
        objs.repository->clock = dialog.dat;
        objs.gl->chess_clock.Repository2Clocks();
        objs.gl->chess_clock.GameStart( gd.master_position.WhiteToPlay() );
        objs.canvas->RedrawClocks();
        dbg_printf( "WHITE: time=%d, increment=%d, visible=%s, running=%s\n",
                dialog.dat.m_white_time,
                dialog.dat.m_white_increment,
                dialog.dat.m_white_visible?"yes":"no",
                dialog.dat.m_white_running?"yes":"no" );
    }
    canvas->ClocksVisible();
}

void GameLogic::CmdPlayers()
{
    Atomic begin;
    PlayerDialog dialog( objs.frame );
    dialog.dat = objs.repository->player;
    if( wxID_OK == dialog.ShowModal() )
    {
        objs.repository->player = dialog.dat;
        dbg_printf( "human=%s, computer=%s, white=%s, black=%s\n",
                     dialog.dat.m_human.c_str(),
                     dialog.dat.m_computer.c_str(),
                     dialog.dat.m_white.c_str(),
                     dialog.dat.m_black.c_str() );
        objs.gl->LabelPlayers(false,true);
    }
}

void GameLogic::CmdEditGameDetails()
{
    GameDetailsDialog dialog( objs.frame );
    bool okay = dialog.Run(gd);
    if( okay )
        GameRedisplayPlayersResult();        
    StatusUpdate();
}

void GameLogic::CmdEditGamePrefix()
{
    GamePrefixDialog dialog( objs.frame );
    dialog.Run(gd);
    StatusUpdate();
}

void GameLogic::CmdEditCopyGamePGNToClipboard()
{
    if( gd.IsEmpty() )
        return;
    gd.FleshOutDate();
    gd.FleshOutMoves();
    std::string head;
    gd.ToFileTxtGameDetails(head);
    std::string body;
    gd.ToFileTxtGameBody(body);
    if( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData( new wxTextDataObject(head + body) );
        wxTheClipboard->Close();
    }
}

// If players or result (possibly) changed, redisplay it
void GameLogic::GameRedisplayPlayersResult()
{
    objs.repository->player.m_white = gd.r.white;
    objs.repository->player.m_black = gd.r.black;
    LabelPlayers(false,true);
    long pos = lb->GetInsertionPoint();
    gd.Rebuild();
    gd.Redisplay(pos);
}

void GameLogic::CmdEditPromote()
{
    Atomic begin;
    gd.Promote();
}

void GameLogic::CmdEditDemote()
{
    Atomic begin;
    gd.Demote();
}

void GameLogic::CmdEditPromoteToVariation()
{
    Atomic begin;
    gd.PromoteToVariation();
}

void GameLogic::CmdEditPromoteRestToVariation()
{
    Atomic begin;
    gd.PromoteRestToVariation();
}

void GameLogic::CmdEditDemoteToComment()
{
    Atomic begin;
    gd.DemoteToComment();
}

void GameLogic::CmdEditDeleteRestOfVariation()
{
    Atomic begin;
    gd.DeleteRestOfVariation();
}

void GameLogic::CmdEditDeleteVariation()
{
    Atomic begin;
    gd.DeleteVariation();
}

bool GameLogic::CmdUpdateWhiteResigns()
{
    bool enabled = (state==MANUAL || state==RESET || 
                        ((state==HUMAN||state==PONDERING) && glc.human_is_white)
                    );
    return enabled;
}

bool GameLogic::CmdUpdateBlackResigns()
{
    bool enabled = (state==MANUAL || state==RESET || 
                        ((state==HUMAN||state==PONDERING) && !glc.human_is_white)
                    );
    return enabled;
}


bool GameLogic::CmdUpdateMoveNow ()
{
    return( state==THINKING || state==FAKE_BOOK_DELAY );
}

bool GameLogic::CmdUpdateTabNew()
{
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    return enabled;
}

bool GameLogic::CmdUpdateTabClose()
{
    bool enabled = (tabs->GetNbrTabs()>1) && (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    return enabled;
}

bool GameLogic::CmdUpdateTabInclude()
{
    bool enabled = (gc_pgn.pgn_filename != "") && (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    return enabled;
}

bool GameLogic::CmdUpdateFileOpen()
{
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    return enabled;
}

bool GameLogic::CmdUpdateFileOpenLog()
{
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    return enabled;
}

bool GameLogic::CmdUpdateGamesCurrent()
{
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    if( enabled )
        enabled = gc_pgn.IsLoaded();
    return enabled;
}

bool GameLogic::CmdUpdateGamesClipboard()
{
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    //if( enabled )
    //    enabled = gc_clipboard.IsLoaded();
    return enabled;
}

bool GameLogic::CmdUpdateGamesSession()
{
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    return enabled;
}

bool GameLogic::CmdUpdateDraw     ()
{
    bool enabled = (state==MANUAL || state==RESET);
    if( state==HUMAN || state==PONDERING )
    {
        DRAWTYPE draw_type;
        if( gd.master_position.IsDraw(glc.human_is_white,draw_type) )
            enabled = true;
    }
    return enabled;
}

bool GameLogic::CmdUpdatePlayWhite()
{
    return( state==RESET || state==MANUAL );
}

bool GameLogic::CmdUpdatePlayBlack()
{
    return( state==RESET || state==MANUAL );
}

bool GameLogic::CmdUpdateSwapSides()
{
    return( state==HUMAN || state==PONDERING );
}

bool GameLogic::CmdUpdateKibitz( wxString &txt )
{
    txt = ( kibitz ? "Stop Kibitzer\tCtrl+K" : "Start Kibitzer\tCtrl+K" );
    return( state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER || state==THINKING );
}

bool GameLogic::CmdUpdateClearKibitz()
{
    return( !kibitz && kibitz_text_to_clear );
}

bool GameLogic::UpdateOptions()
{
    return( state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER );
}

bool GameLogic::StartEngine()
{
    bool okay=true;
    if( objs.uci_interface == NULL )
    {
        objs.uci_interface = new UciInterface( objs.repository->engine.m_file.c_str());
        okay = objs.uci_interface->Start();
        memset( engine_name, 0, sizeof(engine_name) );
        if( okay )
        {
            const char *name = objs.uci_interface->EngineName();
            strncpy( engine_name, name?name:"Computer", sizeof(engine_name)-1 );
        }
        else
        {
            delete objs.uci_interface;
            objs.uci_interface = NULL;
        }
    }
    return okay;
}

void GameLogic::LabelPlayers( bool start_game, bool set_document_player_names )
{
    wxString white = objs.repository->player.m_white;
    wxString black = objs.repository->player.m_black;
    if( start_game )
    {
        if( glc.human_is_white )    
        {
            white = objs.repository->player.m_human;
            black = engine_name;
        }
        else
        {
            white = engine_name;
            black = objs.repository->player.m_human;
        }
    }
    objs.repository->player.m_white = white;
    objs.repository->player.m_black = black;
    canvas->SetPlayers( white.c_str(), black.c_str()  );
    if( set_document_player_names )
    {
        gd.r.white = objs.repository->player.m_white;
        gd.r.black = objs.repository->player.m_black;
    }
}

void GameLogic::OnIdle()
{
    Atomic begin(false);
    static bool not_first_time;
    if( !not_first_time )
    {
        not_first_time = true;
        extern wxString argv1;
        StatusInit();
        if( argv1 != "" )
        {
            std::string filename( argv1.c_str() );
            CmdFileOpenInner( filename );
        }
        SetFocusOnList();
        objs.canvas->notebook->AdvanceSelection();
        objs.canvas->lb->SetFocus();
    }
    bool expired = false;
    bool white = gd.master_position.WhiteToPlay();
    switch( state )
    {
        case RESET:
    //  case GAMEOVER:
    //  case POPUP:
    //  case POPUP_MANUAL:
        case FAKE_BOOK_DELAY:
        case MANUAL:        
        case SLIDING_MANUAL:
        case HUMAN:		    
        case PONDERING:		    
        case SLIDING:       
        case THINKING:
        expired = chess_clock.Run( white );     // Here expired means time_remaining < 0 (see below)
    }
    bool show_white_expired = false;
    bool show_black_expired = false;
    bool fixed_expired = false;
    if( expired )
    {
        switch( state )
        {
        /*  case RESET:
            case GAMEOVER:
            case POPUP:
            case POPUP_MANUAL:
            case FAKE_BOOK_DELAY:
            case MANUAL:        
            case SLIDING_MANUAL: */
            case HUMAN:		    
            case PONDERING:		    
            case SLIDING:       
            case THINKING:
            {
                bool computer_to_play = (state==PONDERING||state==THINKING);
                if( computer_to_play && chess_clock.fixed_period_mode )
                {
                    fixed_expired = true;
                    CmdMoveNow();
                }
                else
                {
                    if( white )
                        show_white_expired = true;
                    else
                        show_black_expired = true;
                    glc.Set( white ? RESULT_WHITE_LOSE_TIME : RESULT_BLACK_LOSE_TIME );
                    NewState( GAMEOVER );
                    if( gd.AreWeInMain() )
                    {
                        gd.r.result = (white ? "0-1" : "1-0");
                        gd.Rebuild();
                        unsigned long pos = gd.gv.FindEnd();
                        atom.Redisplay( pos );
                    }
                    atom.Undo();
                    atom.StatusUpdate();
                }
                break;
            }
        }
    }
    wxString txt;
    expired = chess_clock.white.GetDisplay( txt, show_white_expired );  // Here expired means time_remaining <= 0 (see above)
                                                                        // so expired can be true here but false above
    bool white_is_computer_to_play_fixed = (white && chess_clock.fixed_period_mode && (state==PONDERING||state==THINKING));
    if( expired && white_is_computer_to_play_fixed && !fixed_expired )
    {
        fixed_expired = true;
        CmdMoveNow();
    }
    canvas->WhiteClock( txt );
    expired = chess_clock.black.GetDisplay( txt, show_black_expired );
    bool black_is_computer_to_play_fixed = (!white && chess_clock.fixed_period_mode && (state==PONDERING||state==THINKING));
    if( expired && black_is_computer_to_play_fixed && !fixed_expired )
    {
        fixed_expired = true;
        CmdMoveNow();
    }
    canvas->BlackClock( txt );
    if( state==POPUP || state==POPUP_MANUAL )
    {
        thc::Move move;
        bool done = CheckPopup( move );
        if( done )
        {
            gd.master_position = pre_popup_position;
            if( !move.Valid() )
                NewState( state==POPUP ? human_or_pondering : MANUAL );
            else
            {
                GAME_RESULT result;
                bool gameover = MakeMove( move, result );
                glc.Set( result );
                if( gameover )
                {
                    NewState( GAMEOVER );
                    if( gd.AreWeInMain() )
                    {
                        switch( result )
                        {
                            case RESULT_BLACK_CHECKMATED:
                            case RESULT_BLACK_RESIGNS:
                            case RESULT_BLACK_LOSE_TIME:
                                gd.r.result = "1-0";
                                break;
                            case RESULT_WHITE_CHECKMATED:
                            case RESULT_WHITE_RESIGNS:
                            case RESULT_WHITE_LOSE_TIME:
                                gd.r.result = "0-1";
                                break;
                            case RESULT_DRAW_WHITE_STALEMATED:
                            case RESULT_DRAW_BLACK_STALEMATED:
                            case RESULT_DRAW_AGREED:
                            case RESULT_DRAW_50MOVE:
                            case RESULT_DRAW_INSUFFICIENT:
                            case RESULT_DRAW_REPITITION:
                                gd.r.result = "1/2-1/2";
                                break;
                        }
                        gd.Rebuild();
                        unsigned long pos = gd.gv.FindEnd();
                        atom.Redisplay( pos );
                    }
                }
                else if( state == POPUP_MANUAL )
                    NewState( MANUAL );
                else
                    NewState( StartThinking(&move) );
                atom.Undo();
                atom.StatusUpdate();
            }
        }
    }
    else if( state == PONDERING )
    {
        if( !kibitz )
            objs.uci_interface->Run();
        else
        {
            bool run=true;
            for(;;)
            {
                char buf[256];
                bool cleared;
                bool have_data = objs.uci_interface->KibitzPeekEngineToMove( run, cleared, buf, sizeof(buf)-1 );
                run = false;
                if( cleared )
                    KibitzClearDisplay(true);
                if( !have_data )
                    break;
                dbg_printf( "UciInterface kibitz engine to move; txt=%s\n", buf );
                KibitzUpdateEngineToMove( true, buf );
            }
        }
    }
    else if( state == THINKING )
	{
        if( kibitz )
        {
            bool run=true;
            for(;;)
            {
                char buf[256];
                bool cleared;
                bool have_data = objs.uci_interface->KibitzPeekEngineToMove( run, cleared, buf, sizeof(buf)-1 );
                run = false;
                if( cleared )
                    KibitzClearDisplay( true );
                if( !have_data )
                    break;
                dbg_printf( "UciInterface kibitz engine to move; txt=%s\n", buf );
                KibitzUpdateEngineToMove( false, buf );
            }
        }
        thc::Move ponder;
        thc::Move bestmove = objs.uci_interface->CheckBestMove( ponder );
        if( bestmove.Valid() )
        {
            release_printf( "Engine returns. bestmove is %s, ponder is %s\n",
                 bestmove.TerseOut().c_str(), ponder.TerseOut().c_str() );
            thc::ChessRules cr = gd.master_position;
            cr.PlayMove( bestmove );
            std::string nmove = bestmove.NaturalOut( &gd.master_position );
            LangOut(nmove);
            reply_to.sprintf( "Reply to %d%s%s", gd.master_position.full_move_count,
                                                 gd.master_position.WhiteToPlay()?".":"...", nmove.c_str() );
            GAME_RESULT result;
            bool gameover = MakeMove( bestmove, result );
            if( objs.repository->general.m_bell )
                wxBell();
            glc.Set( result );
            if( gameover )
            {
                NewState( GAMEOVER );
                if( gd.AreWeInMain() )
                {
                    switch( result )
                    {
                        case RESULT_BLACK_CHECKMATED:
                        case RESULT_BLACK_RESIGNS:
                        case RESULT_BLACK_LOSE_TIME:
                            gd.r.result = "1-0";
                            break;
                        case RESULT_WHITE_CHECKMATED:
                        case RESULT_WHITE_RESIGNS:
                        case RESULT_WHITE_LOSE_TIME:
                            gd.r.result = "0-1";
                            break;
                        case RESULT_DRAW_WHITE_STALEMATED:
                        case RESULT_DRAW_BLACK_STALEMATED:
                        case RESULT_DRAW_AGREED:
                        case RESULT_DRAW_50MOVE:
                        case RESULT_DRAW_INSUFFICIENT:
                        case RESULT_DRAW_REPITITION:
                            gd.r.result = "1/2-1/2";
                            break;
                    }
                    gd.Rebuild();
                    unsigned long pos = gd.gv.FindEnd();
                    atom.Redisplay( pos );
                }
            }
            else
            {
                bool pondering = false;
                if( ponder.Valid() && objs.repository->engine.m_ponder )
                {

                    // Check that the move to ponder is legal
                    bool legal=false;
                    vector<thc::Move> moves;
                    gd.master_position.GenLegalMoveList( moves );
                    for( unsigned int i=0; i<moves.size(); i++ )
                    {
                        if( ponder == moves[i] )
                        {
                            legal = true;
                            break;
                        }
                    }
                    if( legal )
                        pondering = StartPondering( ponder );
                    if( pondering )
                    {
                        ponder_move = ponder;
                        int ponder_full_move_count = gd.master_position.full_move_count;
                        bool ponder_white_to_play = gd.master_position.WhiteToPlay();
                        std::string nmove2 = ponder.NaturalOut(&gd.master_position);
                        LangOut(nmove2);
                        ponder_nmove_txt.sprintf( "%d%s%s", ponder_full_move_count, ponder_white_to_play?".":"...",nmove2.c_str() );
                    }
                    else
                        release_printf( "pondering failed\n" );
                }
                NewState( pondering ? PONDERING : HUMAN );
            }
            atom.Undo();
            atom.StatusUpdate();
        }
    }
    else if( kibitz && (state==MANUAL||state==RESET||state==HUMAN||state==GAMEOVER) )
	{
        bool run=true;
        //release_printf( "Entering main kibitz display\n" );
        //int cleared_events=0, have_data_events=0;
        for( int idx=0; idx<4; idx++ )
        {
            for(;;)
            {
                char buf[256];
                bool cleared;
                bool have_data = objs.uci_interface->KibitzPeek( run, idx, cleared, buf, sizeof(buf)-1 );
                run = false;    // don't run multiple times
                if( cleared )   // do a complete clear, once
                {   
                    KibitzClearDisplay();
                    KibitzClearMultiPV();
                    for( int j=0; j<4; j++ ) 
                    {
                        if( j != idx )  // if one slot is cleared, all slots are cleared,
                                        //  so flush the other slots (read up until cleared)
                        {
                            do
                            {
                                bool hav_dat;
                                hav_dat = objs.uci_interface->KibitzPeek( false, j, cleared, buf, sizeof(buf)-1 );
                                if( !hav_dat )
                                    break;
                            } while(!cleared);
                        }
                    }
                }
                if( !have_data )
                    break;
                else
                {
                    dbg_printf( "UciInterface kibitz; idx=%d, txt=%s\n", idx, buf );
                    KibitzUpdate( idx, buf );
                }
            }
        }
        //release_printf( "Exiting main kibitz display %d cleared events, %d have_data events\n",
        //                    cleared_events, have_data_events );
    }
    else if( state == FAKE_BOOK_DELAY )
	{
        unsigned long t2 = GetTickCount();
        if( t2-t1_start_delay > fake_book_delay )  // 1 second by default, less if less time
        {
            std::string nmove;
            nmove = move_after_delay.NaturalOut( &gd.master_position );
            LangOut(nmove);
            reply_to.sprintf( "Reply to %d%s%s (book)", gd.master_position.full_move_count,
                                                 gd.master_position.WhiteToPlay()?".":"...", nmove.c_str() );
            GAME_RESULT result;
            bool gameover = MakeMove( move_after_delay, result );
            if( objs.repository->general.m_bell )
                wxBell();
            glc.Set( result );
            NewState( gameover ? GAMEOVER : HUMAN );
            if( gameover )
            {
                if( gd.AreWeInMain() )
                {
                    switch( result )
                    {
                        case RESULT_BLACK_CHECKMATED:
                        case RESULT_BLACK_RESIGNS:
                        case RESULT_BLACK_LOSE_TIME:
                            gd.r.result = "1-0";
                            break;
                        case RESULT_WHITE_CHECKMATED:
                        case RESULT_WHITE_RESIGNS:
                        case RESULT_WHITE_LOSE_TIME:
                            gd.r.result = "0-1";
                            break;
                        case RESULT_DRAW_WHITE_STALEMATED:
                        case RESULT_DRAW_BLACK_STALEMATED:
                        case RESULT_DRAW_AGREED:
                        case RESULT_DRAW_50MOVE:
                        case RESULT_DRAW_INSUFFICIENT:
                        case RESULT_DRAW_REPITITION:
                            gd.r.result = "1/2-1/2";
                            break;
                    }
                    gd.Rebuild();
                    unsigned long pos = gd.gv.FindEnd();
                    atom.Redisplay( pos );
                }
            }
            atom.Undo();
            atom.StatusUpdate();
        }
    }
}

int GameLogic::MillisecsToNextSecond()
{
    return chess_clock.MillisecsToNextSecond();
}

bool GameLogic::OnIdleNeeded()
{
    bool needed = false;
    switch( state )
    {
        default:
        case RESET:
        case MANUAL:        
        case GAMEOVER:
            needed = false;
            break;
        case FAKE_BOOK_DELAY:
        case SLIDING_MANUAL:
        case HUMAN:		    
        case PONDERING:		    
        case SLIDING:       
        case THINKING:
        case POPUP:
        case POPUP_MANUAL:
            needed = true;
            break;
    }
    ClockConfig *rep = &objs.repository->clock;
    if( rep->m_white_visible && rep->m_white_running )
        needed = true;
    if( rep->m_black_visible && rep->m_black_running )
        needed = true;
    return needed;
}

/****************************************************************************
 * React to mouse release
 ****************************************************************************/
void GameLogic::MouseUp( char file, char rank, wxPoint &point )
{
    Atomic begin(false);

    // Sliding piece into place ?
    if( state==SLIDING || state==SLIDING_MANUAL )
	{

        // It's possible there's more than one move (promotion)
        vector<thc::Move> moves;
        vector<thc::Move> menu;
        ChessEvaluation chess_evaluation = gd.master_position;
        chess_evaluation.GenLegalMoveListSorted( moves );
        for( unsigned int i=0; i<moves.size(); i++ )
        {
            thc::Move move=moves[i];
            if( FILE(move.src)==src_file && RANK(move.src)==src_rank &&
                FILE(move.dst)==file     && RANK(move.dst)==rank )
            {
                menu.push_back( move );
            }
        }
        vector<thc::Move> book_moves;
        bool have_book_moves = objs.book->Lookup( gd.master_position, book_moves );

        // If 0 moves go to HUMAN or MANUAL
        if( menu.size() == 0 )
        {
            if( state == SLIDING_MANUAL )
		        NewState( MANUAL );
            else
            {
                SetGroomedPosition( false );   // redraw original position
		        NewState( human_or_pondering );
            }
        }

        // If 1 move possible play it now
        // If >1 move menu it
        else
        {
            bool play_it_now=false;
            thc::Move move = menu[0];
            if( menu.size() == 1 )
            {

                // If there are no book moves play it
                if( !have_book_moves || !objs.repository->book.m_suggest )
                    play_it_now = true;

                // If it's a book move play it
                vector<thc::Move>::iterator it;
                for( it = book_moves.begin(); it != book_moves.end(); it++ )
                {
                    if( move == *it )
                    {
                        play_it_now = true;
                        break;
                    }
                }
            }

            // Play it now ?
            if( play_it_now )
            {
                GAME_RESULT result;
                bool gameover = MakeMove( move, result );
                if( gameover )
                {
                    glc.Set( result );
                    NewState( GAMEOVER );
                    if( gd.AreWeInMain() )
                    {
                        switch( result )
                        {
                            case RESULT_BLACK_CHECKMATED:
                            case RESULT_BLACK_RESIGNS:
                            case RESULT_BLACK_LOSE_TIME:
                                gd.r.result = "1-0";
                                break;
                            case RESULT_WHITE_CHECKMATED:
                            case RESULT_WHITE_RESIGNS:
                            case RESULT_WHITE_LOSE_TIME:
                                gd.r.result = "0-1";
                                break;
                            case RESULT_DRAW_WHITE_STALEMATED:
                            case RESULT_DRAW_BLACK_STALEMATED:
                            case RESULT_DRAW_AGREED:
                            case RESULT_DRAW_50MOVE:
                            case RESULT_DRAW_INSUFFICIENT:
                            case RESULT_DRAW_REPITITION:
                                gd.r.result = "1/2-1/2";
                                break;
                        }
                        gd.Rebuild();
                        unsigned long pos = gd.gv.FindEnd();
                        atom.Redisplay( pos );
                    }
                }
                else if( state == SLIDING_MANUAL )
                    NewState( MANUAL );
                else
                    NewState( StartThinking(&move) );
                atom.Undo();
                atom.StatusUpdate();
            }

            // Else menu it
            else
            {
                wxRect dummy;
                DoPopup( point, menu, book_moves, SLIDE_TO_DESTINATION, dummy );    
                NewState( state==SLIDING ? POPUP : POPUP_MANUAL );
            }
        }
    }

    // Else not sliding piece, just set position
    //else
    //    SetGroomedPosition( false );
}

void GameLogic::MouseUp()
{
    Atomic begin(false);
    if( state == SLIDING )
        NewState( human_or_pondering );
    else if( state == SLIDING_MANUAL )
        NewState( MANUAL );
    //else
    //    SetGroomedPosition( false );
}

/****************************************************************************
 * The user hovers over the book moves rectangle
 ****************************************************************************/
void GameLogic::BookHover( wxPoint& WXUNUSED(point) )
{
    if( state==RESET || state==MANUAL || state==HUMAN || state==PONDERING )
    {
        vector<thc::Move>  book_moves;
        bool have_book_moves = objs.book->Lookup( gd.master_position, book_moves );
        if( have_book_moves )
        {
            if( objs.uci_interface )
                objs.uci_interface->SuspendResume(false);   // suspend
            vector<thc::Move> menu; // empty
            wxPoint pt_base = objs.canvas->book_moves->GetParent()->GetPosition();
            wxPoint pt   = objs.canvas->book_moves->GetPosition();
            wxRect  rect = objs.canvas->book_moves->GetRect();
            pt.x += pt_base.x;
            pt.y += pt_base.y;
            pt.y += rect.height;
            DoPopup( pt, menu, book_moves, BOOK_HOVER, rect );
            NewState( (state==HUMAN||state==PONDERING) ? POPUP : POPUP_MANUAL );
        }
    }
}


/****************************************************************************
 * React to a square being clicked
 ****************************************************************************/
bool GameLogic::MouseDown( char file, char rank, wxPoint &point )     // return true to slide piece
{
    Atomic begin(false);
    bool slide = false;
    char slide_piece;
    if( objs.uci_interface && state!=THINKING )
        objs.uci_interface->SuspendResume( false );  // suspend if not already
	switch( state )
	{
        case RESET: state = MANUAL;
		// Human move stage 1
		case MANUAL:
		case HUMAN:
		case PONDERING:
		{
			src_file   = file;
			src_rank   = rank;
            slide_piece = gd.master_position.squares[ ('8'-rank)*8+(file-'a') ];
            bool its_me;
            if( gd.master_position.white )
                its_me = (isascii(slide_piece) && isupper(slide_piece));
            else
                its_me = (isascii(slide_piece) && islower(slide_piece));
            if( its_me )
            {
                slide = true;
                slide_piece = toupper(slide_piece);
                NewState( (state==HUMAN||state==PONDERING) ? SLIDING : SLIDING_MANUAL );
            }
            else
            {
                vector<thc::Move> moves, menu;
                ChessEvaluation chess_evaluation = gd.master_position;
                chess_evaluation.GenLegalMoveListSorted( moves );
                for( unsigned int i=0; i<moves.size(); i++ )
                {
                    thc::Move move = moves[i];
                    if( FILE(move.dst)==file && RANK(move.dst)==rank )
                        menu.push_back( move );
                }
                vector<thc::Move> book_moves;
                bool unique_and_in_book = false;
                bool have_book_moves = objs.book->Lookup( gd.master_position, book_moves );
                if( !objs.repository->book.m_suggest )
                    have_book_moves = false;    // pretend we don't
                if( have_book_moves && menu.size()==1 )
                {
                    for( unsigned int i=0; i<book_moves.size(); i++ )
                    {
                        if( menu[0] == book_moves[i] )
                        {
                            unique_and_in_book = true;
                            break;
                        }
                    }
                }
                if( (have_book_moves&&!unique_and_in_book) || menu.size()>1 )
                {
                    wxRect dummy;
                    DoPopup( point, menu, book_moves, CLICK_ON_DESTINATION, dummy );    
                    NewState( (state==HUMAN||state==PONDERING) ? POPUP : POPUP_MANUAL );
                }
                else if( menu.size() == 1 )
                {
                    GAME_RESULT result;
                    bool gameover = MakeMove( menu[0], result );
                    glc.Set( result );
                    if( gameover )
                    {
                        NewState( GAMEOVER );
                        if( gd.AreWeInMain() )
                        {
                            switch( result )
                            {
                                case RESULT_BLACK_CHECKMATED:
                                case RESULT_BLACK_RESIGNS:
                                case RESULT_BLACK_LOSE_TIME:
                                    gd.r.result = "1-0";
                                    break;
                                case RESULT_WHITE_CHECKMATED:
                                case RESULT_WHITE_RESIGNS:
                                case RESULT_WHITE_LOSE_TIME:
                                    gd.r.result = "0-1";
                                    break;
                                case RESULT_DRAW_WHITE_STALEMATED:
                                case RESULT_DRAW_BLACK_STALEMATED:
                                case RESULT_DRAW_AGREED:
                                case RESULT_DRAW_50MOVE:
                                case RESULT_DRAW_INSUFFICIENT:
                                case RESULT_DRAW_REPITITION:
                                    gd.r.result = "1/2-1/2";
                                    break;
                            }
                            gd.Rebuild();
                            unsigned long pos = gd.gv.FindEnd();
                            atom.Redisplay( pos );
                        }
                    }
                    else if( state==HUMAN || state==PONDERING )
                        NewState( StartThinking(&menu[0]) );
                    else
                        NewState( MANUAL );
                    atom.Undo();
                    atom.StatusUpdate();
                }
            }
            break;
        }

		// Sliding
		case SLIDING:
		{
            NewState(HUMAN);   // shouldn't happen
            break;
        }

		// Sliding
		case SLIDING_MANUAL:
		{
            NewState(MANUAL);   // shouldn't happen
            break;
        }

		// Popup
		case POPUP:
		{
            NewState(HUMAN);   // shouldn't happen
            break;
        }

		// Popup
		case POPUP_MANUAL:
		{
            NewState(MANUAL);   // shouldn't happen
            break;
        }
	}
    return( slide );
}

/****************************************************************************
 * Set a new state
 ****************************************************************************/
void GameLogic::NewState( GAME_STATE new_state, bool from_mouse_move )
{
    bool red = false;
    //GAME_STATE old_state=state;
    state = new_state;
    if( new_state==HUMAN || new_state==PONDERING )
        human_or_pondering = new_state; // now we have something to come back to if state
                                        //  later changes temporarily to say POPUP
    bool show=false;
    bool suggestions = true;
    switch( state )
    {
        case HUMAN:
        case RESET:
        case GAMEOVER:
        case MANUAL:  show=true;
    }
    if( show )
        SetGroomedPosition();
    int b1_cmd=0;
    int b2_cmd=0;
    int b3_cmd=0;
    int b4_cmd=0;
    const char *b1 = NULL;
    const char *b2 = NULL;
    const char *b3 = NULL;
    const char *b4 = NULL;
    const char *title = NULL;
    wxString wx_stat;
    switch( state )
    {
        case RESET:
        case MANUAL:
        case SLIDING_MANUAL:
        case POPUP_MANUAL:
        {
                        TERMINAL terminal_score;
                        gd.master_position.Evaluate(terminal_score);
                        bool gameover = (terminal_score==TERMINAL_WCHECKMATE || terminal_score==TERMINAL_BCHECKMATE ||
                                         terminal_score==TERMINAL_WSTALEMATE || terminal_score==TERMINAL_BSTALEMATE);
                        if( gameover )
                        {
                            b1 = "New Game";                    b1_cmd = ID_CMD_NEW_GAME;
                        }
                        else
                        {
                            b1 = "Play white against engine";        b1_cmd = ID_CMD_PLAY_WHITE;
                            b2 = "Play black against engine";        b2_cmd = ID_CMD_PLAY_BLACK;
                            b3 = "Setup a position";                    b3_cmd = ID_CMD_SET_POSITION;
                            b4 = "Search database for this position";   b4_cmd = ID_DATABASE_SEARCH;
                        }
                        break;
        }
        case GAMEOVER:
                        chess_clock.GameOver();
                        //red = true;
                        //glc.TestResult( wx_stat );
                        //title = wx_stat.c_str();
                        b1 = "New Game";                    b1_cmd = ID_CMD_NEW_GAME;
                        b2 = "Examine game";                b2_cmd = ID_CMD_EXAMINE_GAME;
                        objs.log->Gameover();// initial_position, game_moves, glc.Get() );
                        break;

        case SLIDING:
        case HUMAN:
        case PONDERING:
        case POPUP:
                        canvas->box->SetLabel(reply_to.c_str());
                        suggestions = false;
                        b1 = "New Game";                    b1_cmd = ID_CMD_NEW_GAME;
                        b2 = "Swap sides";                  b2_cmd = ID_CMD_SWAP_SIDES;
                        b3 = "Examine game";                b3_cmd = ID_CMD_EXAMINE_GAME;
                        break;

/*      case SLIDING:
        case SLIDING_MANUAL:
                        //title = slide_buf;
                        break;
        case POPUP:
        case POPUP_MANUAL:
                        title = "Select move";
                        break;  */

        case FAKE_BOOK_DELAY:
        case THINKING:
                        red = true;
                        title = "Engine thinking...";
                        b1 = "New Game";                    b1_cmd = ID_CMD_NEW_GAME;
                        b2 = "Force immediate move";        b2_cmd = ID_CMD_MOVENOW;
                        b3 = "Examine game";                b3_cmd = ID_CMD_EXAMINE_GAME;
                        break;

    }
    canvas->SetChessPosition( gd.master_position );
    if( suggestions )
        canvas->box->SetLabel( b1 && b2 && b3 && b4 ? "Enter moves, comments and variations freely - or ..." : "Suggestions" );
    if( title && !show )
        canvas->SetBoardTitle( title, red );
    if( !b1 )
        canvas->button1->Show( false );        
    else
    {
        canvas->button1->SetLabel( b1 );
        canvas->button1->Show( true );        
        canvas->button1_cmd = b1_cmd;
    }
    if( !b2 )
        canvas->button2->Show( false );        
    else
    {
        canvas->button2->SetLabel( b2 );
        canvas->button2->Show( true );        
        canvas->button2_cmd = b2_cmd;
    }
    if( !b3 )
        canvas->button3->Show( false );        
    else
    {
        canvas->button3->SetLabel( b3 );
        canvas->button3->Show( true );        
        canvas->button3_cmd = b3_cmd;
    }
    if( !b4 )
        canvas->button4->Show( false );        
    else
    {
        canvas->button4->SetLabel( b4 );
        canvas->button4->Show( true );        
        canvas->button4_cmd = b4_cmd;
    }
    if( kibitz )
    {
        canvas->kibitz_ctrl->Show(true );
        canvas->kibitz_button1->Show(true);
        canvas->kibitz_button2->Show(true);
    }
    #if 1 // auto_kibitz_clear
    else if( !from_mouse_move )
    {
        kibitz_text_to_clear = false;
        canvas->ClearStaleKibitz();
    }
    #endif
    #if 0 // manual_kibitz_clear
    else if( !kibitz_text_to_clear  )
    {
        canvas->kibitz_ctrl->Show(false);
        canvas->kibitz_button1->Show(false);
        canvas->kibitz_button2->Show(false);
    }
    #endif
    canvas->PositionSuggestionButtons();
    canvas->button1->Refresh();
    canvas->button2->Refresh();
    canvas->button3->Refresh();
    canvas->button4->Refresh();
    canvas->BookUpdate( state==THINKING || state==FAKE_BOOK_DELAY );
    wxString txt;
    chess_clock.white.GetDisplay( txt, true );
    canvas->WhiteClock( txt );
    chess_clock.black.GetDisplay( txt, true );
    canvas->BlackClock( txt );
    canvas->ClocksVisible();
    if( kibitz && (state==MANUAL||state==HUMAN||state==PONDERING||state==THINKING||state==GAMEOVER) )
    {
        KibitzIntro();
        if( state==MANUAL || state==HUMAN || state==GAMEOVER )
        {
            thc::ChessPosition pos = gd.master_position;
            char buf[128];
            strcpy( buf, pos.ForsythPublish().c_str() );
            kibitz_pos = pos;
            objs.uci_interface->Kibitz( pos, buf );
        }
    }
    if( objs.uci_interface && (state==MANUAL||state==HUMAN||state==PONDERING||state==THINKING||(kibitz&&state==GAMEOVER) ) )
        objs.uci_interface->SuspendResume( true );  // resume if suspended
    atom.StatusUpdate();
    atom.Focus();
}

void GameLogic::StatusInit()
{
    if( objs.frame )
    {
        status_field1 = "File: (none)";
        status_field2 = "";
        objs.frame->SetStatusText( status_field1.c_str(), 1 );
        objs.frame->SetStatusText( status_field2.c_str(), 2 );
    }
}

void GameLogic::StatusUpdate( int idx )
{
	games_in_file_idx = -1;
	int tabs_current_idx = tabs->GetCurrentIdx();
	bool tab_in_file=false;
	if( objs.frame )
    {
        bool file_loaded =  gc_pgn.pgn_filename != "";
	    bool tab_modified = false;
        int  nbr_modified=0;
        bool refresh=false;
		std::string str;
        if( !file_loaded )
		{
	        tab_modified = undo.IsModified();
            if( tab_modified )
				str = "*";
		}
        else
        {
            size_t nbr = gc_pgn.gds.size();
#ifdef _DEBUG	// performance sensitive - could be looping through a big file as user types - every character
			char *dbg = NULL;
			char dbg_buf[200];
            if( nbr < 20 )
				dbg = dbg_buf;
#endif
            for( size_t i=0; i<nbr; i++ )
            {
                ListableGame *ptr = gc_pgn.gds[i].get();
				#ifdef _DEBUG
				if( dbg )
				{
					sprintf( dbg, "> %s-%s", ptr->White(), ptr->Black() );
					dbg = dbg+strlen(dbg);
				}
				#endif
				if( !ptr->IsGameDocument() )
				{
				#ifdef _DEBUG
					if( dbg )
					{
						sprintf( dbg, ": Not Game Document" );
						dbg = dbg+strlen(dbg);
					}
				#endif
				}
				else
                {
                    uint32_t game_being_edited = ptr->GetGameBeingEdited();
                    bool game_modified = ptr->IsModified();
					#ifdef _DEBUG
					if( dbg )
					{
						sprintf( dbg, ": %u", game_being_edited );
						dbg = dbg+strlen(dbg);
					}
					#endif

					// Loop through tabs looking for this game
                    GameDocument *pd;
                    Undo *pu;
                    int tab_idx=0;
                    int handle = tabs->Iterate(0,pd,pu);
                    while( pd && pu )
                    {
                        if( game_being_edited>0 && game_being_edited==pd->game_being_edited )
                        {
							// Woo hoo, found the game in a tab 
							// It might be sitting in the tab modified
                            game_modified |= (pd->IsModified() || pu->IsModified());

							// It might be the current tab
							if( tab_idx == tabs_current_idx )
							{
								tab_in_file = true;
								tab_modified = game_modified;
								idx = i;
							}
							#ifdef _DEBUG
							if( dbg )
							{
	  							sprintf( dbg, ": Edit match, %s tab %s", tab_in_file?"Current":"Not current", game_modified?"modified":"not modified" );
								dbg = dbg+strlen(dbg);
							}
							#endif
                            break;
                        }
						else
						{
							#ifdef _DEBUG
							if( dbg )
							{
	  							sprintf( dbg, ": %u", pd->game_being_edited );
								dbg = dbg+strlen(dbg);
							}
							#endif
						}
                        tabs->Iterate(handle,pd,pu);
                        tab_idx++;
                    }
                    if( game_modified )
                        nbr_modified++;
                }
				#ifdef _DEBUG
				if( dbg )
				{
					cprintf( "%s\n", dbg_buf );
					dbg = dbg_buf;
				}
				#endif
            }
            str = (nbr_modified > 0) ? "*" : "";
            char buf[200];
            if( tab_in_file )
			{
				games_in_file_idx = idx;
				if( nbr_modified )
				{
					sprintf( buf, tab_modified?"* Game %d of %ld (%d modified)":"Game %d of %ld (%d modified)",
							idx+1, gc_pgn.gds.size(), nbr_modified );
				}
				else
				{
					sprintf( buf, tab_modified?"* Game %d of %ld":"Game %d of %ld",
							idx+1, gc_pgn.gds.size() );
				}
			}
			else
			{
	            tab_modified = gd.IsModified() || undo.IsModified();
                sprintf( buf, " %s( this tab not in file ) ", tab_modified?"*":"" );
			}
            str = buf;
        }
        if( str != status_field2 )
        {
            status_field2 = str;
            refresh=true;
        }
		bool file_modified = gc_pgn.file_irrevocably_modified || (nbr_modified>0);
        str = file_modified ? "* File: " :  "File: ";
        if( !file_loaded )
            str += "(none)";
        else
            str += gc_pgn.pgn_filename;
        if( str != status_field1 )
        {
            status_field1 = str;
            refresh=true;
        }
        if( refresh )
        {
            objs.frame->SetStatusText( status_field1.c_str(), 1 );
            objs.frame->SetStatusText( status_field2.c_str(), 2 );
        }
    }
}

void GameLogic::DoPopup( wxPoint &point, vector<thc::Move> &target_moves,
                                         vector<thc::Move> &book_moves,
                                         POPUP_MODE popup_mode,
                                         wxRect hover )
{
    wxArrayString strs;
    wxArrayString terses;
    wxArrayString book;

    // Start with cancel
    if( popup_mode != BOOK_HOVER )
    {
        strs.Add( "Cancel" );
        terses.Add( "" );
        book.Add( "" );
    }

    // 1st book moves that are target moves
    // 2nd target moves that aren't book moves
    // 3rd book moves that aren't target moves
    bool found_1 = false;
    bool found_2 = false;

    // 1) Book moves that are target moves
    // Loop through book moves
    for( unsigned int i=0; i<book_moves.size(); i++ )
    {
        thc::Move move = book_moves[i];

        // Is book move a target move ?
        bool found = false;
        for( unsigned int j=0; !found && j<target_moves.size(); j++ )
        {
            if( move == target_moves[j] )
                found = true;
        }

        // Add book and target move info to strs[], terses[] and book[]
        if( found )
        {
            found_1 = true;
            std::string natural_move = move.NaturalOut( &gd.master_position );
            LangOut(natural_move);
            std::string tmove        = move.TerseOut();
            wxString s1 = tmove.c_str();
            terses.Add( tmove );
            wxString s2 = natural_move.c_str();
            strs.Add( s2 );
            book.Add( "B" );
        }
    }

    // 2) Target moves that aren't book moves
    // Loop through target moves
    for( unsigned int i=0; i<target_moves.size(); i++ )
    {
        thc::Move move = target_moves[i];

        // Is target move a book move ?
        bool found = false;
        for( unsigned int j=0; !found && j<book_moves.size(); j++ )
        {
            if( move == book_moves[j] )
                found = true;
        }

        // Add target move info to strs[] terses[] and book[]
        if( !found )
        {
            found_2 = true;
            std::string natural_move = move.NaturalOut( &gd.master_position );
            LangOut(natural_move);
            std::string tmove        = move.TerseOut();
            wxString s1 = tmove.c_str();
            terses.Add( tmove );
            wxString s2 = natural_move.c_str();
            strs.Add( s2 );
            book.Add( "" );
        }
    }

    // 3) Book moves that aren't target moves
    if( !found_1 )  // if we didn't do 1
    {
        // Loop through book moves
        for( unsigned int i=0; i<book_moves.size(); i++ )
        {
            thc::Move move = book_moves[i];

            // Is book move a target move ?
            bool found = false;
            for( unsigned int j=0; !found && j<target_moves.size(); j++ )
            {
                if( move == target_moves[j] )
                    found = true;
            }

            // Only if we haven't had target move, add book move info to strs[],
            //   terses[] and book[]
            if( !found )
            {
                std::string natural_move = move.NaturalOut( &gd.master_position );
                LangOut(natural_move);
                std::string tmove        = move.TerseOut();
                wxString s1 = tmove.c_str();
                terses.Add( tmove );
                wxString s2 = natural_move.c_str();
                strs.Add( s2 );
                book.Add( "B" );
            }
        }
    }

    // Show the position after playing the move if SLIDE_TO_DESTINATION
    //  but be prepared to undo that move
    pre_popup_position = gd.master_position;
    if( popup_mode == SLIDE_TO_DESTINATION )
        gd.master_position.PlayMove(target_moves[0]);
    if( canvas->popup )
        delete canvas->popup;
    //dbg_printf( "objs.frame is a %s window\n", objs.frame->IsTopLevel()?"top level":"child" );

    // After introducing new windowing arrangement with an intermediate PanelBoard holding the graphical
    //  board we, need different parent windows depending on whether we are clicking inside or outside
    //  the PanelBoard
    if( popup_mode == BOOK_HOVER )
        canvas->popup = new PopupControl( objs.frame, strs,terses,book,popup_mode,hover,ID_POPUP,point );
    else
        canvas->popup = new PopupControl( objs.canvas->pb, strs,terses,book,popup_mode,hover,ID_POPUP,point );
}

bool GameLogic::CheckPopup( thc::Move &move )
{
    bool done = false;
    move.Invalid();
    if( canvas->popup )
    {
        done = canvas->popup->done;
        if( done )
        {
            thc::ChessRules temp = pre_popup_position;
            bool okay = move.TerseIn(&temp,canvas->popup->terse_move);
            if( !okay )
                move.Invalid();
            delete canvas->popup;
            canvas->popup = NULL;
        }
    }
    return done;
}

bool GameLogic::InHumanEngineGame( bool &human_is_white )
{
    bool ingame=false;
    human_is_white = glc.human_is_white;
    switch( state )
    {
        case RESET:                             break;
        case GAMEOVER:                          break;
        case PONDERING:         ingame = true;  break;
        case HUMAN:             ingame = true;  break;
        case MANUAL:                            break;
        case SLIDING:           ingame = true;  break;
        case SLIDING_MANUAL:                    break;
        case THINKING:          ingame = true;  break;
        case POPUP:             ingame = true;  break;
        case POPUP_MANUAL:                      break;
        case FAKE_BOOK_DELAY:   ingame = true;  break;
    }
    return ingame;
}

bool GameLogic::MakeMove( thc::Move move, GAME_RESULT &result )
{
    bool ingame=false;
    switch( state )
    {
        case RESET:                             break;
        case GAMEOVER:                          break;
        case PONDERING:         ingame = true;  break;
        case HUMAN:             ingame = true;  break;
        case MANUAL:                            break;
        case SLIDING:           ingame = true;  break;
        case SLIDING_MANUAL:                    break;
        case THINKING:          ingame = true;  break;
        case POPUP:             ingame = true;  break;
        case POPUP_MANUAL:                      break;
        case FAKE_BOOK_DELAY:   ingame = true;  break;
    }
    result=RESULT_NONE;
    std::string nmove;
    bool white = gd.master_position.white;
    GAME_MOVE game_move;
    game_move.move = move;
    game_move.human_is_white = glc.human_is_white;
    int  white_time;
    int  black_time;
    bool white_visible;
    bool black_visible;
    int  increment;
    bool running;
    chess_clock.Press( white, ingame, glc.human_is_white );
    chess_clock.white.GetClock( white_time, increment, running, white_visible );
    chess_clock.black.GetClock( black_time, increment, running, black_visible );
    game_move.white_clock_visible = white_visible;
    game_move.black_clock_visible = black_visible;
    if( glc.human_is_white )
    {
        game_move.human_millisecs_time = chess_clock.white.millisecs_time;
        game_move.engine_millisecs_time = chess_clock.black.millisecs_time;
    }
    else
    {
        game_move.engine_millisecs_time = chess_clock.white.millisecs_time;
        game_move.human_millisecs_time = chess_clock.black.millisecs_time;
    }
    game_move.flag_ingame = ingame;
    game_move.nag_value1 = 0;
    game_move.nag_value2 = 0;
    bool allow_overwrite = !ingame;
    bool gameover = false; 
    if( !gd.MakeMove( game_move, allow_overwrite) )
        chess_clock.Press( !white, ingame, glc.human_is_white );    // temp hack, undo previous press (almost)
    else
    {
        gd.master_position.PlayMove(move);
        TERMINAL terminal_score;
        DRAWTYPE draw_type1, draw_type2;
        gd.master_position.Evaluate(terminal_score);
        gameover = (terminal_score==TERMINAL_WCHECKMATE || terminal_score==TERMINAL_BCHECKMATE ||
                    terminal_score==TERMINAL_WSTALEMATE || terminal_score==TERMINAL_BSTALEMATE);
        if( gameover )
        {
            switch( terminal_score )
            {
                case TERMINAL_WCHECKMATE:  result = RESULT_WHITE_CHECKMATED;   break;
                case TERMINAL_BCHECKMATE:  result = RESULT_BLACK_CHECKMATED;   break;
                case TERMINAL_WSTALEMATE:  result = RESULT_DRAW_WHITE_STALEMATED;   break;
                case TERMINAL_BSTALEMATE:  result = RESULT_DRAW_BLACK_STALEMATED;   break;
            }
        }
        else if( ingame && gd.master_position.IsDraw(true,draw_type1) && gd.master_position.IsDraw(false,draw_type2) )
        {
            gameover = true; 
            result = RESULT_DRAW_INSUFFICIENT;
            if( draw_type1==DRAWTYPE_50MOVE || draw_type2==DRAWTYPE_50MOVE )
                result = RESULT_DRAW_50MOVE;
            else if( draw_type1==DRAWTYPE_REPITITION || draw_type2==DRAWTYPE_REPITITION )
                result = RESULT_DRAW_REPITITION;
        }
    }
    SetGroomedPosition();
    return gameover;
}

void GameLogic::SetGroomedPosition( bool show_title )
{
    bool ingame=false;
    switch( state )
    {
        case RESET:                             break;
        case GAMEOVER:                          break;
        case PONDERING:         ingame = true;  break;
        case HUMAN:             ingame = true;  break;
        case MANUAL:                            break;
        case SLIDING:           ingame = true;  break;
        case SLIDING_MANUAL:                    break;
        case THINKING:          ingame = true;  break;
        case POPUP:             ingame = true;  break;
        case POPUP_MANUAL:                      break;
        case FAKE_BOOK_DELAY:   ingame = true;  break;
    }
    bool full_blindfold = (objs.repository->training.m_blindfold_hide_black_pawns  &&
                           objs.repository->training.m_blindfold_hide_black_pieces &&
                           objs.repository->training.m_blindfold_hide_white_pawns  &&
                           objs.repository->training.m_blindfold_hide_white_pieces);
    bool partial_blindfold = (objs.repository->training.m_blindfold_hide_black_pawns  ||
                           objs.repository->training.m_blindfold_hide_black_pieces ||
                           objs.repository->training.m_blindfold_hide_white_pawns  ||
                           objs.repository->training.m_blindfold_hide_white_pieces);
    int lag_nbr = objs.repository->training.m_nbr_half_moves_behind;
    if( !ingame )
    {
        lag_nbr = 0;
        full_blindfold = false;
        partial_blindfold = false;
    }
    wxString title;
    const char *qualifier="";
    if( full_blindfold )
        qualifier = " (blindfold)";
    else if( partial_blindfold )
        qualifier = " (partial blindfold)";
    thc::ChessPosition view_pos = gd.master_position;
    title.sprintf( "Initial position%s", qualifier );
    if( objs.canvas )
    {
        thc::ChessRules cr;
        string move_txt;
        if( lag_nbr == 0 )
        {
            GAME_MOVE *game_move = gd.GetSummary( cr, move_txt, 0 );
            if( game_move )
            {
                title.sprintf( "%s%s",
                     move_txt.c_str(), qualifier );
            }
        }
        else
        {
            gd.GetSummary( cr, move_txt, lag_nbr );
            title.sprintf( "%s (%d half moves behind)%s",
                            move_txt.c_str(), lag_nbr, qualifier );
            view_pos = cr;
        }
    }
    if( state == RESET )
        title = "Initial Position";
    else if( ingame )
    {
        for( int i=0; i<4; i++ )
        {
            char min=0;
            char max=0;
            switch( i )
            {
                case 0:
                {
                    if( objs.repository->training.m_blindfold_hide_black_pawns )
                    {
                        min='p';
                        max='p';
                    }
                    break;
                }
                case 1:
                {
                    if( objs.repository->training.m_blindfold_hide_black_pieces )
                    {
                        min='a';
                        max='z';
                    }
                    break;
                }
                case 2:
                {
                    if( objs.repository->training.m_blindfold_hide_white_pawns )
                    {
                        min='P';
                        max='P';
                    }
                    break;
                }
                case 3:
                {
                    if( objs.repository->training.m_blindfold_hide_white_pieces )
                    {
                        min='A';
                        max='Z';
                    }
                    break;
                }
            }
            for( int j=0; j<64; j++ )
            {
                char c = view_pos.squares[j];
                if( min<=c && c<=max )
                {
                    if( c=='p' || c=='P' )
                        c = (min!=max?c:' ');   //if min!=max, blank pieces not pawns
                    else
                        c = ' ';
                    view_pos.squares[j] = c;
                }
            }
        }
    }
    canvas->SetChessPosition( view_pos );
	if( show_title )
		canvas->SetBoardTitle( title );
    if( canvas->resize_ready && tabs && show_title )
        tabs->SetTitle( gd );
 }

// Get blindfold mode for sliding
bool GameLogic::ShowSlidingPieceOnly()
{
    bool show_sliding_piece_only = false;
    if( state == SLIDING )  // SLIDING_MANUAL doesn't count
    {
        bool blindfold = (objs.repository->training.m_blindfold_hide_black_pawns  ||
                          objs.repository->training.m_blindfold_hide_black_pieces ||
                          objs.repository->training.m_blindfold_hide_white_pawns  ||
                          objs.repository->training.m_blindfold_hide_white_pieces);
        int lag_nbr = objs.repository->training.m_nbr_half_moves_behind;
        if( blindfold || lag_nbr!=0 )
            show_sliding_piece_only = !objs.repository->training.m_peek_at_complete_position;
    }
    return show_sliding_piece_only;    
}


// Start engine thinking after human plays human_move (if not NULL)
GAME_STATE GameLogic::StartThinking( const thc::Move *human_move )
{
    GAME_STATE ret;
    bool ponderhit=false;
    if( human_or_pondering==PONDERING && human_move && *human_move==ponder_move )
    {
        thc::ChessPosition pos = gd.master_position;
        ponderhit = objs.uci_interface->Ponderhit( pos );
    }
    if( ponderhit )
        ret = THINKING;
    else
    {
        // Use a move list if possible so engine can see repititions
        #define MAX_NBR_MOVES_IN_MOVELIST 100
        thc::ChessPosition std_startpos;
        thc::ChessPosition move_list_startpos;
        thc::ChessPosition pos;
        std::vector<GAME_MOVE> game_moves;
        gd.GetSummary( move_list_startpos, game_moves, pos );

        // Use a move list if possible so engine can see repititions
        int n=game_moves.size();
        bool okay_to_use_move_list = true;
        wxString smoves;
        thc::ChessRules cr;
        cr = move_list_startpos;
        bool saving_moves = false;
        for( int i=0; okay_to_use_move_list && i<n; i++ )
        {
            if( !saving_moves )
            {
                int moves_to_go = n-i;
                if( moves_to_go <= MAX_NBR_MOVES_IN_MOVELIST )
                {
                    move_list_startpos = cr;
                    saving_moves = true;
                }    
            }
            thc::Move move  = game_moves[i].move;
            if( saving_moves )
            {
                smoves += " ";
                smoves += move.TerseOut().c_str();
            }
            okay_to_use_move_list = true;
            cr.PlayMove( move );
            if( saving_moves && move.src==move.dst ) // If NULL move encountered, start again after NULL move played
            {                                        //  [Minimum V2.02 bug fix note: surrounding code is ugly and should
                                                     //    be improved, eg okay_to_move_list does nothing in the loop ]
                move_list_startpos = cr;
                smoves = "";
            }
        }
        if( okay_to_use_move_list )
        {
            if( pos != cr )
                okay_to_use_move_list = false;
        }

        // See if the position is in the book
        vector<BookMove> fast_moves;
        bool have_fast_moves = objs.book->Lookup( pos, fast_moves );
        if( have_fast_moves )
        {
            int nbr_pieces = 0;   // ignore "always play move limit" if endgame
            for( int i=0; i<64; i++ )
            {
                if( pos.squares[i] != ' ' )
                    nbr_pieces++;
            }

            // If we are beyond the "always play book move limit"
            if( nbr_pieces>6 && gd.master_position.full_move_count>objs.repository->book.m_limit_moves )
            {

                // Calc a random percentage, say 81
                int x = rand();
                int percent = x%100;

                // Then if we play a book move say 80% of the time, don't
                //  play a book move if percent = say 81            
                if( percent > objs.repository->book.m_post_limit_percent )
                    have_fast_moves = false;
            }
        }

        // Get book move if we are going to play it
        thc::Move book_move;
        book_move.Invalid();
        if( have_fast_moves )
        {
            vector<BookMove> candidates;
            unsigned int min = 4000000000;
            for( unsigned int i=0; i<fast_moves.size(); i++ )
            {
                BookMove bm = fast_moves[i];
                unsigned int nbr_times = *bm.play_position_count;
                if( nbr_times < min )
                    min = nbr_times;
            }
            for( unsigned int i=0; i<fast_moves.size(); i++ )
            {
                BookMove bm = fast_moves[i];
                unsigned int nbr_times = *bm.play_position_count;
                if( nbr_times == min )
                    candidates.push_back( bm );
            }
            unsigned int nbr = 0;
            for( unsigned int i=0; i<candidates.size(); i++ )
                nbr += candidates[i].count;
            unsigned int idx=0;
            if( nbr > 1 )
            {
                unsigned int x = (unsigned int)rand();
                idx = x%nbr;
                nbr = 0;
                for( unsigned int i=0; i<candidates.size(); i++ )
                {
                    nbr += candidates[i].count;
                    if( idx < nbr )
                    {
                        idx = i;
                        break;
                    }
                }
            }
            BookMove bm = candidates[idx];
            (*bm.play_position_count)++;
            book_move = bm.move;
        }

        // Get times
        long wtime_ms, btime_ms, winc_ms, binc_ms;
        int time, increment;
        bool running, visible;
        int white_millisecs_time;
        int black_millisecs_time;
        chess_clock.GetTimes( white_millisecs_time, black_millisecs_time );
        wtime_ms = white_millisecs_time;
        btime_ms = black_millisecs_time;
        chess_clock.white.GetClock( time, increment, running, visible );
        winc_ms = increment*1000;
        chess_clock.black.GetClock( time, increment, running, visible );
        binc_ms = increment*1000;
        if( chess_clock.fixed_period_mode )
        {
            btime_ms  = 10000000;
            wtime_ms  = 10000000;
            binc_ms   = 100000;
            winc_ms   = 100000;
        }

        // If book move
        if( book_move.Valid() )
        {
            move_after_delay = book_move;
            unsigned long time2 = (gd.master_position.WhiteToPlay() ? wtime_ms : btime_ms ); 
            fake_book_delay = 1000;     // 1 second by default
            if( time2 < 4000 )
                fake_book_delay = 200;  // 200mS if less than 4 secs left
            else if( time2 < 10000 )
                fake_book_delay = 500;  // 500mS if less than 10 secs left
            t1_start_delay = GetTickCount();
            ret = FAKE_BOOK_DELAY;
        }
        else
        {
            char forsyth[120];
            if( okay_to_use_move_list )
            {
                bool use_startpos = move_list_startpos.CmpStrict(std_startpos);
                if( !use_startpos )
                {
                    cr = move_list_startpos;
                    strcpy( forsyth, cr.ForsythPublish().c_str() );
                }
                objs.uci_interface->StartThinking( false, pos, use_startpos?NULL:forsyth, smoves, wtime_ms, btime_ms, winc_ms, binc_ms );   // ms,ms,inc_ms,inc_ms
            }
            else
            {
                bool use_startpos = pos.CmpStrict(std_startpos);
                if( !use_startpos )
                    strcpy( forsyth, gd.master_position.ForsythPublish().c_str() );
                objs.uci_interface->StartThinking( false, pos, use_startpos?NULL:forsyth, wtime_ms, btime_ms, winc_ms, binc_ms );   // ms,ms,inc_ms,inc_ms
            }
            ret = THINKING;
        }
    }
    return ret;
}

// Start engine pondering engine's own proposed ponder move
bool GameLogic::StartPondering( thc::Move ponder )
{
    bool pondering=false;
    thc::ChessPosition std_startpos;
    thc::ChessPosition move_list_startpos;
    thc::ChessPosition pos;
    std::vector<GAME_MOVE> game_moves;
    gd.GetSummary( move_list_startpos, game_moves, pos );

    // Use a move list if possible so engine can see repititions
    int n=game_moves.size();
    bool okay_to_use_move_list = true;
    wxString smoves;
    thc::ChessRules cr;
    cr = move_list_startpos;
    bool saving_moves = false;
    for( int i=0; okay_to_use_move_list && i<n; i++ )
    {
        if( !saving_moves )
        {
            int moves_to_go = n-i;
            if( moves_to_go <= MAX_NBR_MOVES_IN_MOVELIST )
            {
                move_list_startpos = cr;
                saving_moves = true;
            }    
        }
        thc::Move move  = game_moves[i].move;
        if( saving_moves )
        {
            smoves += " ";
            smoves += move.TerseOut().c_str();
        }
        okay_to_use_move_list = true;
        cr.PlayMove( move );
        if( saving_moves && move.src==move.dst ) // If NULL move encountered, start again after NULL move played
        {                                        //  [Minimum V2.02 bug fix note: surrounding code is ugly and should
                                                 //    be improved, eg okay_to_move_list does nothing in the loop ]
            move_list_startpos = cr;
            smoves = "";
        }
    }
    if( okay_to_use_move_list )
    {
        if( pos != cr )
            okay_to_use_move_list = false;
        else
        {
            cr.PlayMove( ponder );
            smoves += " ";
            smoves += ponder.TerseOut().c_str();
        }
    }

    if( okay_to_use_move_list )
    {
        // Get times
        long wtime_ms, btime_ms, winc_ms, binc_ms;
        int time, increment;
        bool running, visible;
        int white_millisecs_time;
        int black_millisecs_time;
        chess_clock.GetTimes( white_millisecs_time, black_millisecs_time );
        wtime_ms = white_millisecs_time;
        btime_ms = black_millisecs_time;
        chess_clock.white.GetClock( time, increment, running, visible );
        winc_ms = increment*1000;
        chess_clock.black.GetClock( time, increment, running, visible );
        binc_ms = increment*1000;

        char forsyth[120];
        bool use_startpos = move_list_startpos.CmpStrict(std_startpos);
        if( !use_startpos )
        {
            cr = move_list_startpos;
            strcpy( forsyth, cr.ForsythPublish().c_str() );
        }
        objs.uci_interface->StartThinking( true, pos, use_startpos?NULL:forsyth, smoves, wtime_ms, btime_ms, winc_ms, binc_ms );   // ms,ms,inc_ms,inc_ms
        pondering = true;
    }
    return pondering;
}

// Sort PV's according to their score
struct PV
{
    int score_cp;
    int idx;
    bool operator < (const PV& pv) const { return score_cp<pv.score_cp; }
    bool operator > (const PV& pv) const { return score_cp > pv.score_cp; }
    bool operator == (const PV& pv) const { return score_cp == pv.score_cp; }
};

void GameLogic::CmdKibitzCaptureAll()
{
    Atomic begin;
    if( /*kibitz &&*/ kibitz_pos==gd.master_position &&
        (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER || state==THINKING)
      )
    {
        // Rank the available lines according to score
        vector<PV> sortable;
        for( unsigned int i=0; i<nbrof(kibitz_rank); i++ )
        {
            PV x;
            x.score_cp = kibitz_rank[i];
            x.idx  = i;
            sortable.push_back(x);
        }
        sort( sortable.rbegin(), sortable.rend() );

        // Write lines in ranked order
        bool first=true;
        MoveTree *node=NULL;
        bool use_repeat_one_move=false;
        GAME_MOVE repeat_one_move;
        for( unsigned int i=0; i<nbrof(kibitz_sorted); i++ )
        {
            PV x;
            x = sortable[i];
            if( kibitz_move[x.idx].Valid() )
            {
                if( first )
                {
                    first = false;
                    node = gd.KibitzCaptureStart( engine_name, kibitz_pv[x.idx].c_str(), kibitz_var[x.idx],
                        use_repeat_one_move, repeat_one_move );
                    if( !node )
                        break;
                }
                else
                {
                    gd.KibitzCapture( node, kibitz_pv[x.idx].c_str(), kibitz_var[x.idx],
                        use_repeat_one_move, repeat_one_move );
                }
            }
        }
    }
}

void GameLogic::CmdKibitzCaptureOne()
{
    Atomic begin;
    if( /*kibitz &&*/ kibitz_pos==gd.master_position &&
        (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER || state==THINKING)
      )
    {
        // Rank the available lines according to score
        vector<PV> sortable;
        for( unsigned int i=0; i<nbrof(kibitz_rank); i++ )
        {
            PV x;
            x.score_cp = kibitz_rank[i];
            x.idx  = i;
            sortable.push_back(x);
        }
        sort( sortable.rbegin(), sortable.rend() );

        // Write lines in ranked order
        bool first=true;
        MoveTree *node=NULL;
        bool use_repeat_one_move=false;
        GAME_MOVE repeat_one_move;
        for( unsigned int i=0; i<1 /*i<nbrof(kibitz_sorted)*/; i++ )
        {
            PV x;
            x = sortable[i];
            if( kibitz_move[x.idx].Valid() )
            {
                if( first )
                {
                    first = false;
                    node = gd.KibitzCaptureStart( engine_name, kibitz_pv[x.idx].c_str(), kibitz_var[x.idx],
                        use_repeat_one_move, repeat_one_move );
                    if( !node )
                        break;
                }
                else
                {
                    gd.KibitzCapture( node, kibitz_pv[x.idx].c_str(), kibitz_var[x.idx],
                        use_repeat_one_move, repeat_one_move );
                }
            }
        }
    }
}

void GameLogic::KibitzUpdate( int idx, const char *txt )
{
    wxString pv;
    bool have_moves=false;
    thc::Move candidate_move;
    candidate_move.Invalid();
    int depth=0, score_cp=0, rank_score_cp=0;
    thc::ChessRules cr = gd.master_position;
    const char *s, *temp;
    s = strstr(txt,temp=" depth ");
    if( s )
    {
        s += strlen(temp);
        while( *s == ' ' )
            s++;
        depth = atoi(s);
    }
    s = strstr(txt,temp=" score cp ");
    if( s )
    {
        s += strlen(temp);
        while( *s == ' ' )
            s++;
        rank_score_cp = atoi(s);
    }
    s = strstr(txt,temp=" mate ");
    int mate = 0;
    if( s )
    {
        s += strlen(temp);
        while( *s == ' ' )
            s++;
        mate = atoi(s);    // eg mate=2, mate=-2
        if( mate > 0 )
            rank_score_cp = 100000 - mate;  // eg rank_score_cp = 99998
        else if( mate < 0 )
            rank_score_cp = -100000 - mate;  // eg rank_score_cp = -99998
    }
    if( cr.WhiteToPlay() )
        score_cp = rank_score_cp;
    else
        score_cp = 0-rank_score_cp;
    s = strstr(txt,temp=" pv ");
    std::vector<thc::Move> var;
    if( s )
    {
        if( mate )
            pv.sprintf( "#%d (depth %d)", mate, depth );
        else
            pv.sprintf( "%1.2f (depth %d)", ((double)score_cp)/100.0, depth );
        const char *txt2 = s+strlen(temp);
        while( *txt2 == ' ' )
            txt2++;
        #define MAX_NBR_KIBITZ_MOVES 13 //14
        for( unsigned int i=0; /*i<MAX_NBR_KIBITZ_MOVES*/; i++ )
        {
            thc::Move move;
            std::string nmove;
            bool have_move = move.TerseIn( &cr, txt2 );
            if( !have_move )
                break;
            else
            {
                txt2 += 4;
                if( isascii(*txt2) && isalpha(*txt2) )   // eg 'Q','R' etc.
                    txt2++;
                while( *txt2 == ' ' )
                    txt2++;
                var.push_back( move );
                wxString mv;
                bool white=cr.WhiteToPlay();
                nmove = move.NaturalOut(&cr);
                LangOut(nmove);
                if( white || i==0 )
                    mv.sprintf( " %d%s%s", cr.full_move_count, white?".":"...",nmove.c_str() );
                else
                    mv.sprintf( " %s", nmove.c_str() );
                if( !have_moves )
                {
                    candidate_move = move;
                    have_moves = true;
                }
                cr.PlayMove( move );
                pv += mv;
                if( *txt2 == '\0' )
                    break;
            }
        }
    }
    if( !have_moves )
        candidate_move.Invalid();
    kibitz_pv   [idx] = pv;
    kibitz_depth[idx] = depth;
    kibitz_rank [idx] = rank_score_cp;
    kibitz_move [idx] = candidate_move;
    kibitz_var  [idx] = var;

    // Suppress low depth duplicates (unfortunately O(N^2) but okay as N is small)
    for( unsigned int i=0; i<nbrof(kibitz_move); i++ )
    {
        for( unsigned int j=0; j<i; j++ )
        {
            if( kibitz_move[j] == kibitz_move[i] )
            {
                if( kibitz_depth[i] < kibitz_depth[j] )
                    kibitz_move[i].Invalid();
                else
                    kibitz_move[j].Invalid();
            }
        }
    }

    // Rank the available lines according to score
    vector<PV> sortable;
    for( unsigned int i=0; i<nbrof(kibitz_rank); i++ )
    {
        PV x;
        x.score_cp = kibitz_rank[i];
        x.idx  = i;
        sortable.push_back(x);
    }
    sort( sortable.rbegin(), sortable.rend() );

    // Write lines in ranked order
    int line=1;
    for( unsigned int i=0; i<nbrof(kibitz_sorted); i++ )
    {
        PV x;
        x = sortable[i];
        if( kibitz_move[x.idx].Valid() )
            canvas->Kibitz( line++, kibitz_pv[x.idx] );
    }
    while( line <= nbrof(kibitz_sorted) )
        canvas->Kibitz( line++, "" );
    kibitz_text_to_clear = true;
}


void GameLogic::KibitzUpdateEngineToMove( bool ponder, const char *txt )
{
    wxString pv;
    int depth=0, score_cp=0, rank_score_cp=0;
    thc::ChessRules cr = gd.master_position;
    const char *s, *temp;
    s = strstr(txt,temp=" depth ");
    if( s )
    {
        s += strlen(temp);
        while( *s == ' ' )
            s++;
        depth = atoi(s);
    }
    s = strstr(txt,temp=" score cp ");
    if( s )
    {
        s += strlen(temp);
        while( *s == ' ' )
            s++;
        rank_score_cp = atoi(s);
    }
    s = strstr(txt,temp=" mate ");
    int mate = 0;
    if( s )
    {
        s += strlen(temp);
        while( *s == ' ' )
            s++;
        mate = atoi(s);    // eg mate=2, mate=-2
        if( mate > 0 )
            rank_score_cp = 100000 - mate;  // eg rank_score_cp = 99998
        else if( mate < 0 )
            rank_score_cp = -100000 - mate;  // eg rank_score_cp = -99998
    }
    bool white = cr.WhiteToPlay();
    if( ponder )
        white = !white;  // extra move prepended
    if( white )
        score_cp = rank_score_cp;
    else
        score_cp = 0-rank_score_cp;
    s = strstr(txt,temp=" pv ");
    if( s )
    {
        if( mate )
            pv.sprintf( "#%d (depth %d)", mate, depth );
        else
            pv.sprintf( "%1.2f (depth %d)", ((double)score_cp)/100.0, depth );
        const char *txt2 = s+strlen(temp);
        while( *txt2 == ' ' )
            txt2++;
        for( unsigned int i=0; /*i<MAX_NBR_KIBITZ_MOVES*/; i++ )
        {
            thc::Move move;
            std::string nmove;
            bool have_move=false;
            if( ponder && i==0 )
            {
                have_move = true;
                move = ponder_move;
            }
            else
            {
                have_move = move.TerseIn( &cr, txt2 );
                txt2 += 4;
                if( isalpha(*txt2) )   // eg 'Q','R' etc.
                    txt2++;
                while( *txt2 == ' ' )
                    txt2++;
            }
            if( !have_move )
                break;
            else
            {
                wxString mv;
                bool white2=cr.WhiteToPlay();
                nmove = move.NaturalOut(&cr);
                LangOut(nmove);
                if( white2 || i==0 )
                    mv.sprintf( " %d%s%s", cr.full_move_count, white2?".":"...",nmove.c_str() );
                else
                    mv.sprintf( " %s", nmove.c_str() );
                cr.PlayMove( move );
                pv += mv;
                if( *txt2 == '\0' )
                    break;
            }
        }
    }
    canvas->KibitzScroll( pv );
}

void GameLogic::KibitzClearMultiPV()
{
    for( int i=0; i<nbrof(kibitz_sorted); i++ )
    {
        kibitz_sorted[i] = i;
        kibitz_rank  [i] = -1000000;
        kibitz_move  [i].Invalid();
        kibitz_depth [i] = 0;
    }
}

void GameLogic::KibitzClearDisplay( bool intro )
{
    if( intro )
        KibitzIntro();
    for( int i=0; i<nbrof(kibitz_sorted); i++ )
        canvas->Kibitz(i+1, "" );
}

void GameLogic::KibitzIntro()
{
    wxString txt;
    if( state == PONDERING )
        txt.sprintf( "Analysis by %s [pondering %s]", engine_name, ponder_nmove_txt.c_str() );
    else if( state == THINKING )
        txt.sprintf( "Analysis by %s [running]", engine_name );
    else
        txt.sprintf( "Kibitzing by %s [running]", engine_name );
    canvas->Kibitz( 0, txt );
}


