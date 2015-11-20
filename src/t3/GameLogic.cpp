/****************************************************************************
 * Implements the top level game logic
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <time.h>
#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/filename.h"
#include "Appdefs.h"
#include "Canvas.h"
#include "thc.h"
#include "GraphicBoard.h"
#include "Book.h"
#include "GameLogic.h"
#include "Lang.h"
#include "Rybka.h"
#include "DebugPrintf.h"
#include "CtrlBoxBookMoves.h"
#include "PopupControl.h"
#include "Session.h"
#include "Log.h"
#include "Repository.h"
#include "PositionDialog.h"
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
    vector<Move> &, //pv,
    int  ,//   score_cp,
    int   //   depth
)
{
    dbg_printf( "Unexpected call to ReportOnProgress() in GUI rather than engine" );
}


// Initialise the game logic
GameLogic::GameLogic( Canvas *canvas, CtrlChessTxt *lb )
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
    ChessPosition temp;
    gd.Init( temp );
    file_game_idx = -1;
    tabs->SetInfile(false);
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
    if( okay ) // if exiting
        objs.log->SaveGame( &gd, editing_log );
    return okay;
}

bool GameLogic::EditingLog()
{
    std::string log_filename(objs.repository->log.m_file.c_str());
    std::string pgn_filename(gc.pgn_filename.c_str());
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
        gc.gds.clear();
        gc.pgn_filename = "";
        objs.log->SaveGame( &gd, editing_log );
        objs.session->SaveGame( &gd );
        IndicateNoCurrentDocument();
        ChessPosition temp;
        gd.Init( temp );
        this->file_game_idx = -1;
        tabs->SetInfile(false);
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
        objs.log->SaveGame( &gd, editing_log );
        objs.session->SaveGame( &gd );
        IndicateNoCurrentDocument();
        ChessPosition temp;
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
            ChessPosition pos;
            bool okay = pos.Forsyth( after ); // "7K/8/7k/8/P7/8/8/8 b KQ a3 0 9" );
            if( okay )
            {
                objs.log->SaveGame( &gd, editing_log );
                objs.session->SaveGame( &gd );
                IndicateNoCurrentDocument();
                gd.Init( pos );
                tabs->SetInfile(false);
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
    ChessRules cr;
    string title;
    gd.Locate( atom.GetInsertionPoint(), cr ); //, title );
    gd.master_position = cr;
//    objs.frame->SetTitle(title.c_str());
    chess_clock.SetDefault();
    canvas->RedrawClocks();
    GAME_RESULT result = RESULT_NONE;
    TERMINAL terminal_score;
    DRAWTYPE draw_type1, draw_type2;
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
    else if( gd.master_position.IsDraw(true,draw_type1) && gd.master_position.IsDraw(false,draw_type2) )
    {
        gameover = true; 
        result = RESULT_DRAW_INSUFFICIENT;
        if( draw_type1==DRAWTYPE_50MOVE || draw_type2==DRAWTYPE_50MOVE )
            result = RESULT_DRAW_50MOVE;
        else if( draw_type1==DRAWTYPE_REPITITION || draw_type2==DRAWTYPE_REPITITION )
            result = RESULT_DRAW_REPITITION;
    }
    objs.repository->player.m_white = gd.r.white;
    objs.repository->player.m_black = gd.r.black;
    LabelPlayers(false,true);
    if( objs.rybka )
    {
        delete objs.rybka;  // Allow a different engine to be loaded (if engine option changed)
        objs.rybka = NULL;
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
        objs.canvas->SetMinimumPlaySize();
        glc.Begin( true );
        ChessRules cr;
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
        if( cr.white )
        {
            if( move_txt == "" )
                reply_to = "Make your move";
            else
                reply_to.sprintf( "Reply to %s", move_txt.c_str() );
            NewState( HUMAN );
        }
        else
            NewState( StartThinking(NULL) );
        objs.canvas->RedrawClocks();
        SetGroomedPosition();
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
        objs.canvas->SetMinimumPlaySize();
        glc.Begin( false );
        ChessRules cr;
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
        if( !cr.white )
        {
            if( move_txt == "" )
                reply_to = "Make your move";
            else
                reply_to.sprintf( "Reply to %s", move_txt.c_str() );
            NewState( HUMAN );
        }
        else
            NewState( StartThinking(NULL) );
        objs.canvas->RedrawClocks();
        SetGroomedPosition();
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
    if( objs.rybka )
    {
        delete objs.rybka;
        objs.rybka = NULL;
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
        tabs->SetInfile(false);
        ShowNewDocument();
        atom.NotUndoAble();  // don't save an undo position
    }
    objs.session->SaveGame(&temp);      // ...modify session only after loading old game
}

void GameLogic::OnTabSelected( int idx )
{
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


void GameLogic::CmdTabClose()
{
    Atomic begin;
    tabs->TabSelected( tabs->TabDelete() );
    Undo temp = undo;
    ShowNewDocument();  // clears undo
    undo = temp;
    atom.NotUndoAble(); // don't save an undo position
    undo.ShowStackSize( "CmdTabClose()" );
}

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
        ChessRules cr;
        string last_move_txt;
        GAME_MOVE *last_move = gd.GetSummary( cr, last_move_txt );
        glc.Swap();
        wxString temp                   = objs.repository->player.m_black;
        objs.repository->player.m_black = objs.repository->player.m_white;
        objs.repository->player.m_white = temp;
        chess_clock.Swap();
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

void GameLogic::CmdKibitz()
{
    Atomic begin;
    wxString txt;
    if( CmdUpdateKibitz(txt) )
    {
        if( !kibitz )
        {
            objs.canvas->SetMinimumKibitzSize();
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
                    ChessPosition pos = gd.master_position;
                    objs.rybka->Kibitz( pos, buf );
                }
            }
            if( okay )
            {
                kibitz = true;
                kibitz_pos = gd.master_position;
                objs.rybka->SuspendResume(true);
                KibitzClearDisplay( true );
            }
        }
        else
        {
            wxString intro;
            if( state==THINKING || state==PONDERING )
                intro.sprintf( "Analysis by %s [stopped]", engine_name );
            else
                intro.sprintf( "Kibitzing by %s [stopped]", engine_name );
            canvas->Kibitz( 0, intro );
            kibitz = false;
            if( state!=THINKING && state!=PONDERING )
                objs.rybka->KibitzStop();
            //objs.rybka->SuspendResume(false);
        }
        if( kibitz )
        {
            canvas->kibitz_ctrl->Show(true );
            canvas->kibitz_button1->Show(true);
            canvas->kibitz_button2->Show(true);
        }
    }
}

void GameLogic::CmdClearKibitz( bool hide_window )
{
    Atomic begin;
    if( !kibitz )
        canvas->Kibitz( 0, "" );
    KibitzClearDisplay();
    KibitzClearMultiPV();
    kibitz_text_to_clear = false;
    if( hide_window )
    {
        canvas->kibitz_ctrl->Show(false);
        canvas->kibitz_button1->Show(false);
        canvas->kibitz_button2->Show(false);
    }
}

void GameLogic::CmdMoveNow()
{
    Atomic begin;
    if( state==THINKING && objs.rybka )
        objs.rybka->MoveNow();
    else if( state == FAKE_BOOK_DELAY )
        fake_book_delay = 0;
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
            wxString dir;
            wxFileName::SplitPath( fd.GetPath(), &dir, NULL, NULL );
            objs.repository->nv.m_doc_dir = dir;
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
    if( !gc.Load(filename) )
        wxMessageBox( "Cannot read file", "Error", wxOK|wxICON_ERROR );
    else
    {
        bool have_game = false;
        if( gc.gds.size()==1 && objs.repository->general.m_straight_to_game )
        {
            GameDocument *gd_file = gc.gds[0]->GetGameDocumentPtr();
            bool have_game = gd_file && gd_file->in_memory;
            if( !have_game && gd_file )
            {
                FILE *pgn_in = pf.ReopenRead( gd_file->pgn_handle );
                if( pgn_in )
                {
                    long fposn2 = gd_file->fposn2;
                    long end    = gd_file->fposn3;
                    fseek(pgn_in,fposn2,SEEK_SET);
                    long len = end-fposn2;
                    char *buf = new char [len];
                    if( len == (long)fread(buf,1,len,pgn_in) )
                    {
                        std::string s(buf,len);
                        thc::ChessRules cr;
                        int nbr_converted;
                        gd = *gd_file;
                        gd.PgnParse(true,nbr_converted,s,cr,NULL);
                        make_smart_ptr( GameDocument,new_smart_ptr,gd);
                        gc.gds[0] = std::move(new_smart_ptr);
                        have_game = true;
                    }
                    pf.Close( &gc_clipboard );
                    delete[] buf;
                }
            }
            if( have_game )
            {
                objs.log->SaveGame(&gd,editing_log);
                objs.session->SaveGame(&gd);
                IndicateNoCurrentDocument();
                gd_file->game_being_edited = ++game_being_edited_tag;
                gd_file->GetGameDocumentFromFile(gd);
                gd_file->selected = true;
                this->file_game_idx = 0;    // game 0
                tabs->SetInfile(true);
                if( !is_empty )
                    tabs->TabNew(gd);
                ShowNewDocument();
            }
        }
        if( !have_game )
        {
            wxPoint pt(0,0);
            wxSize sz = objs.frame->GetSize();
            sz.x = (sz.x*9)/10;
            sz.y = (sz.y*9)/10;
            PgnDialog dialog( objs.frame, &gc, &gc_clipboard, ID_PGN_DIALOG_FILE, pt, sz );   // GamesDialog instance
            std::string title = "File: " + filename;
            if( dialog.ShowModalOk(title) )
            {
                objs.log->SaveGame(&gd,editing_log);
                objs.session->SaveGame(&gd);
                if( dialog.LoadGame(this,gd,this->file_game_idx) )
                {
                    if( !is_empty )
                        tabs->TabNew(gd);
                    tabs->SetInfile(true);
                    ShowNewDocument();
                }
            }
        }
    }
    atom.StatusUpdate();
}


bool GameLogic::CmdUpdateNextGame()
{
    if( false )
    {
        return objs.db->TestNextRow();
    }
    else
    {
        size_t nbr = gc.gds.size();
        return( gc.gds.size()>1 && file_game_idx!=-1 && 0<=file_game_idx+1 && file_game_idx+1<nbr );
    }
}

bool GameLogic::CmdUpdatePreviousGame()
{
    if( false )
    {
        return objs.db->TestPrevRow();
    }
    else
    {
        size_t nbr = gc.gds.size();
        return( gc.gds.size()>1 && file_game_idx!=-1 && 0<=file_game_idx-1 && file_game_idx-1<nbr );
    }
}

void GameLogic::NextGamePreviousGame( int idx )
{
    Atomic begin;
    bool editing_log = objs.gl->EditingLog();
    bool have_game = false;
    GameDocument *ptr = gc.gds[idx]->GetGameDocumentPtr();
    if( ptr )
    {
        GameDocument gd_file = *ptr;
        have_game = gd_file.in_memory;
        if( !have_game )
        {
            FILE *pgn_in = pf.ReopenRead( gd_file.pgn_handle );
            if( pgn_in )
            {
                long fposn2 = gd_file.fposn2;
                long end    = gd_file.fposn3;
                fseek(pgn_in,fposn2,SEEK_SET);
                long len = end-fposn2;
                char *buf = new char [len];
                if( len == (long)fread(buf,1,len,pgn_in) )
                {
                    std::string s(buf,len);
                    thc::ChessRules cr;
                    int nbr_converted;
                    gd_file.PgnParse(true,nbr_converted,s,cr,NULL);
                    make_smart_ptr( GameDocument,new_smart_ptr,gd_file);
                    gc.gds[idx] = std::move(new_smart_ptr);
                    have_game = true;
                }
                pf.Close( &gc_clipboard );
                delete[] buf;
            }
        }
        if( have_game )
        {
            PutBackDocument();
            objs.log->SaveGame(&gd,editing_log);
            objs.session->SaveGame(&gd);
            IndicateNoCurrentDocument();
            gd_file.game_being_edited = ++game_being_edited_tag;
            gd = gd_file;
            smart_ptr<MagicBase> smp = gc.gds[idx];
            smp->SetSelected(true);
            this->file_game_idx = idx;
            tabs->SetInfile(true);
            ShowNewDocument();
        }
    }
    atom.StatusUpdate();
}

void GameLogic::CmdNextGame()
{
    if( CmdUpdateNextGame() )
    {
        if( false )
        {
            Atomic begin;
            bool editing_log = objs.gl->EditingLog();
            static CompactGame pact;
            int idx = objs.db->GetCurrent();
            objs.db->GetRow(idx+1,&pact);
            GameDocument gd_temp;
            std::vector<thc::Move> moves;
            gd_temp.r = pact.r;
            gd_temp.LoadFromMoveList(pact.moves);
            PutBackDocument();
            objs.log->SaveGame(&gd,editing_log);
            objs.session->SaveGame(&gd);
            IndicateNoCurrentDocument();
            gd = gd_temp;
            tabs->SetInfile(false);
            ShowNewDocument();
            atom.StatusUpdate();
        }
        else
        {
            int idx = file_game_idx+1;
            NextGamePreviousGame(idx);
            StatusUpdate(idx);
        }
    }
}

void GameLogic::CmdPreviousGame()
{
    if( CmdUpdatePreviousGame() )
    {
        if( false )
        {
            Atomic begin;
            bool editing_log = objs.gl->EditingLog();
            static CompactGame pact;
            int idx = objs.db->GetCurrent();
            objs.db->GetRow( idx-1, &pact );
            GameDocument gd_temp;
            std::vector<thc::Move> moves;
            gd_temp.r = pact.r;
            gd_temp.LoadFromMoveList(pact.moves);
            PutBackDocument();
            objs.log->SaveGame(&gd,editing_log);
            objs.session->SaveGame(&gd);
            IndicateNoCurrentDocument();
            gd = gd_temp;
            tabs->SetInfile(false);
            ShowNewDocument();
            atom.StatusUpdate();
        }
        else
        {
            int idx = file_game_idx-1;
            NextGamePreviousGame(idx);
            StatusUpdate(idx);
        }
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

// After editing, put document back to file cache or clipboard cache (not session cache,
//  edited document is added to end of session instead)
void GameLogic::PutBackDocument()
{
    for( int i=0; i<gc.gds.size(); i++ )
    {
        smart_ptr<MagicBase> smp = gc.gds[i];
        if( smp->GetGameBeingEdited() == gd.game_being_edited )
        {
            gd.FleshOutDate();
            gd.FleshOutMoves();
            GameDocument new_doc = gd;
            new_doc.modified = gd.modified || undo.IsModified();
            make_smart_ptr( GameDocument, new_smart_ptr, new_doc);
            gc.gds[i] = std::move(new_smart_ptr);
            return;
        }
    }
    for( int i=0; i<gc_clipboard.gds.size(); i++ )
    {
        smart_ptr<MagicBase> smp = gc_clipboard.gds[i];
        if( smp->GetGameBeingEdited() == gd.game_being_edited )
        {
            gd.FleshOutDate();
            gd.FleshOutMoves();
            GameDocument new_doc = gd;
            new_doc.modified = gd.modified || undo.IsModified();
            make_smart_ptr( GameDocument, new_smart_ptr, new_doc );
            gc_clipboard.gds[i] = std::move(new_smart_ptr);
            return;
        }
    }
}

void GameLogic::IndicateNoCurrentDocument()
{
    for( int i=0; i<gc.gds.size(); i++ )
    {
        smart_ptr<MagicBase> smp = gc.gds[i];
        if( smp->GetGameBeingEdited() == gd.game_being_edited )
             smp->SetGameBeingEdited(0);
        smp->SetSelected(false);
    }
    for( int i=0; i<gc_clipboard.gds.size(); i++ )
    {
        smart_ptr<MagicBase> smp = gc_clipboard.gds[i];
        if( smp->GetGameBeingEdited() == gd.game_being_edited )
             smp->SetGameBeingEdited(0);
        smp->SetSelected(false);
    }
}

void GameLogic::CmdGamesCurrent()
{
    Atomic begin;
    bool editing_log = objs.gl->EditingLog();
    bool ok=CmdUpdateGamesCurrent();
    if( ok )
    {
        if( !gc.IsSynced() )
        {
            if( !gc.Reload() )
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
        gc.Debug( "Before loading current file games dialog" );
        PgnDialog dialog( objs.frame, &gc, &gc_clipboard, ID_PGN_DIALOG_FILE, pt, sz );   // GamesDialog instance
        if( dialog.ShowModalOk("Current file") )
        {
            objs.log->SaveGame(&gd,editing_log);
            objs.session->SaveGame(&gd);
            PutBackDocument();
            if( dialog.LoadGame(this,gd,this->file_game_idx) )
            {
                tabs->SetInfile(true);
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
    CmdDatabase( cr, REQ_POSITION );
}

void GameLogic::CmdDatabaseShowAll()
{
    thc::ChessRules cr;
    thc::ChessRules start_position;
    std::string title_txt;
    gd.GetSummary( cr, title_txt );
    CmdDatabase( cr, REQ_SHOW_ALL );
}

void GameLogic::CmdDatabasePlayers()
{
    thc::ChessRules cr;
    thc::ChessRules start_position;
    std::string title_txt;
    gd.GetSummary( cr, title_txt );
    CmdDatabase( cr, REQ_PLAYERS );
}

void GameLogic::CmdDatabase( thc::ChessRules &cr, DB_REQ db_req )
{
    std::string error_msg;
    bool operational = objs.db->IsOperational(error_msg);
    if( !operational )
    {
        wxMessageBox(
            "The database is not currently running. To correct this select a database\n"
            "using the database menu. If you don't have a database, the database menu\n"
            "also offers a command to create a database\n", "Database problem", wxOK|wxICON_ERROR
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
        DbDialog dialog( objs.frame, &cr, &gc_database, &gc_clipboard, db_req, ID_PGN_DIALOG_DATABASE, pt, sz );   // GamesDialog instance
        if( dialog.ShowModalOk("Database games") )
        {
            objs.log->SaveGame(&gd,editing_log);
            //objs.session->SaveGame(&gd);        //careful...
            GameDocument temp = gd;
            GameDocument new_gd;
            PutBackDocument();
            if( dialog.LoadGame(new_gd) )
            {
                tabs->TabNew(new_gd);
                tabs->SetInfile(false);
                ShowNewDocument();
            }
            objs.session->SaveGame(&temp);      // ...modify session only after loading old game
        }
        atom.StatusUpdate();
    }
}

void GameLogic::CmdDatabaseSelect()
{
    wxFileDialog fd( objs.frame, "Select current database", "", "", "*.tarrasch_db", wxFD_FILE_MUST_EXIST );
    wxString path( objs.repository->database.m_file );
    fd.SetPath(path);
    if( wxID_OK == fd.ShowModal() )
    {
        wxString s = fd.GetPath();
        wxString previous = objs.repository->database.m_file;
        objs.repository->database.m_file = s;
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

void GameLogic::CmdDatabaseCreate()
{
    Atomic begin;
    wxPoint pt(0,0);
    wxSize sz = objs.frame->GetSize();
    sz.x = (sz.x*9)/10;
    sz.y = (sz.y*9)/10;
    CreateDatabaseDialog dialog( objs.frame, ID_CREATE_DB_DIALOG, true ); // create_mode = true
    dialog.ShowModal();
    atom.StatusUpdate();
}

void GameLogic::CmdDatabaseAppend()
{
    Atomic begin;
    wxPoint pt(0,0);
    wxSize sz = objs.frame->GetSize();
    sz.x = (sz.x*9)/10;
    sz.y = (sz.y*9)/10;
    CreateDatabaseDialog dialog( objs.frame, ID_CREATE_DB_DIALOG, false ); // create_mode = false
    dialog.ShowModal();
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
            if( dialog.LoadGame(this,gd,this->file_game_idx) )
            {
                tabs->SetInfile(this->file_game_idx >= 0);
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
            if( dialog.LoadGame(this,gd,this->file_game_idx) )
            {
                tabs->SetInfile(this->file_game_idx >= 0);
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
            if( objs.rybka )
                objs.rybka->ForceStop();
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
            ChessRules cr;
            std::string move_txt;
            gd.GetSummaryMove( cr, move_txt );
            if( !found || move_txt=="" )
                reply_to = "Make your move";
            else    
            {
                reply_to.sprintf( "Reply to %s", move_txt.c_str() );
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
                vector<Move> moves;
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
            if( objs.rybka )
                objs.rybka->ForceStop();
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
            ChessRules cr;
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
    ClockDialog dialog( objs.frame );
    objs.gl->chess_clock.Clocks2Repository();
    dialog.dat = objs.repository->clock;
    chess_clock.PauseBegin();
    int ret = dialog.ShowModal();
    chess_clock.PauseEnd();
    if( wxID_OK == ret )
    {
        if( dialog.dat.m_white_time==0 && dialog.dat.m_white_secs==0 )
            dialog.dat.m_white_secs = 1;
        if( dialog.dat.m_black_time==0 && dialog.dat.m_black_secs==0 )
            dialog.dat.m_black_secs = 1;
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
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
    return enabled;
}

bool GameLogic::CmdUpdateTabInclude()
{
    bool enabled = (state==MANUAL || state==RESET || state==HUMAN || state==PONDERING || state==GAMEOVER);
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
        enabled = gc.IsLoaded();
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
    if( objs.rybka == NULL )
    {
        objs.rybka = new Rybka( objs.repository->engine.m_file.c_str());
        okay = objs.rybka->Start();
        memset( engine_name, 0, sizeof(engine_name) );
        if( okay )
        {
            const char *name = objs.rybka->EngineName();
            strncpy( engine_name, name?name:"Computer", sizeof(engine_name)-1 );
        }
        else
        {
            delete objs.rybka;
            objs.rybka = NULL;
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
    if( set_document_player_names )
    {
        gd.r.white = objs.repository->player.m_white;
        gd.r.black = objs.repository->player.m_black;
        canvas->SetPlayers( white.c_str(), black.c_str()  );
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
        if( argv1 != "" )
        {
            std::string filename( argv1.c_str() );
            CmdFileOpenInner( filename );
        }
        StatusWarning();
        SetFocusOnList();
        objs.canvas->notebook->AdvanceSelection();
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
        expired = chess_clock.Run( white );
    }
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
                break;
            }
        }
    }
    wxString txt;
    chess_clock.white.GetDisplay( txt, true );
    canvas->WhiteClock( txt );
    chess_clock.black.GetDisplay( txt, true );
    canvas->BlackClock( txt );
    if( state==POPUP || state==POPUP_MANUAL )
    {
        Move move;
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
            objs.rybka->Run();
        else
        {
            bool run=true;
            for(;;)
            {
                char buf[256];
                bool cleared;
                bool have_data = objs.rybka->KibitzPeekEngineToMove( run, cleared, buf, sizeof(buf)-1 );
                run = false;
                if( cleared )
                    KibitzClearDisplay(true);
                if( !have_data )
                    break;
                dbg_printf( "Rybka kibitz engine to move; txt=%s\n", buf );
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
                bool have_data = objs.rybka->KibitzPeekEngineToMove( run, cleared, buf, sizeof(buf)-1 );
                run = false;
                if( cleared )
                    KibitzClearDisplay( true );
                if( !have_data )
                    break;
                dbg_printf( "Rybka kibitz engine to move; txt=%s\n", buf );
                KibitzUpdateEngineToMove( false, buf );
            }
        }
        Move ponder;
        Move bestmove = objs.rybka->CheckBestMove( ponder );
        if( bestmove.Valid() )
        {
            release_printf( "Engine returns. bestmove is %s, ponder is %s\n",
                 bestmove.TerseOut().c_str(), ponder.TerseOut().c_str() );
            ChessRules cr = gd.master_position;
            cr.PlayMove( bestmove );
            std::string nmove = bestmove.NaturalOut( &gd.master_position );
            LangOut(nmove);
            reply_to.sprintf( "Reply to %d%s%s", gd.master_position.full_move_count,
                                                 gd.master_position.WhiteToPlay()?".":"...", nmove.c_str() );
            GAME_RESULT result;
            bool gameover = MakeMove( bestmove, result );
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
                    vector<Move> moves;
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
                        std::string nmove = ponder.NaturalOut(&gd.master_position);
                        LangOut(nmove);
                        ponder_nmove_txt.sprintf( "%d%s%s", ponder_full_move_count, ponder_white_to_play?".":"...",nmove.c_str() );
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
                bool have_data = objs.rybka->KibitzPeek( run, idx, cleared, buf, sizeof(buf)-1 );
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
                                hav_dat = objs.rybka->KibitzPeek( false, j, cleared, buf, sizeof(buf)-1 );
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
                    dbg_printf( "Rybka kibitz; idx=%d, txt=%s\n", idx, buf );
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
        vector<Move> moves;
        vector<Move> menu;
        ChessEvaluation chess_evaluation = gd.master_position;
        chess_evaluation.GenLegalMoveListSorted( moves );
        for( unsigned int i=0; i<moves.size(); i++ )
        {
            Move move=moves[i];
            if( FILE(move.src)==src_file && RANK(move.src)==src_rank &&
                FILE(move.dst)==file     && RANK(move.dst)==rank )
            {
                menu.push_back( move );
            }
        }
        vector<Move> book_moves;
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
            Move move = menu[0];
            if( menu.size() == 1 )
            {

                // If there are no book moves play it
                if( !have_book_moves || !objs.repository->book.m_suggest )
                    play_it_now = true;

                // If it's a book move play it
                vector<Move>::iterator it;
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
    else
        SetGroomedPosition( false );
}

void GameLogic::MouseUp()
{
    Atomic begin(false);
    if( state == SLIDING )
        NewState( human_or_pondering );
    else if( state == SLIDING_MANUAL )
        NewState( MANUAL );
    else
        SetGroomedPosition( false );
}

/****************************************************************************
 * The user hovers over the book moves rectangle
 ****************************************************************************/
void GameLogic::BookHover( wxPoint& WXUNUSED(point) )
{
    if( state==RESET || state==MANUAL || state==HUMAN || state==PONDERING )
    {
        vector<Move>  book_moves;
        bool have_book_moves = objs.book->Lookup( gd.master_position, book_moves );
        if( have_book_moves )
        {
            if( objs.rybka )
                objs.rybka->SuspendResume(false);   // suspend
            vector<Move> menu; // empty
            wxPoint pt   = objs.canvas->book_moves->GetPosition();
            wxRect  rect = objs.canvas->book_moves->GetRect();
            pt.y = -4;  // just above client area
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
    if( objs.rybka && state!=THINKING )
        objs.rybka->SuspendResume( false );  // suspend if not already
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
                if( slide_piece != 'P' )
                    sprintf( slide_buf, "Sliding %c%c%c", slide_piece, file, rank );
                else
                    sprintf( slide_buf, "Sliding %c%c", file, rank );
                NewState( (state==HUMAN||state==PONDERING) ? SLIDING : SLIDING_MANUAL );
            }
            else
            {
                vector<Move> moves, menu;
                ChessEvaluation chess_evaluation = gd.master_position;
                chess_evaluation.GenLegalMoveListSorted( moves );
                for( unsigned int i=0; i<moves.size(); i++ )
                {
                    Move move = moves[i];
                    if( FILE(move.dst)==file && RANK(move.dst)==rank )
                        menu.push_back( move );
                }
                vector<Move> book_moves;
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
    switch( state )
    {
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
    const char *stat = "";
    wxString wx_stat;
    switch( state )
    {
        case RESET:
        case MANUAL:
                        stat = NULL;
                        b1 = NULL; //"New Game";
                        b2 = "Setup Position";  b2_cmd = ID_CMD_SET_POSITION;
                        b3 = "Play White";      b3_cmd = ID_CMD_PLAY_WHITE;
                        b4 = "Play Black";      b4_cmd = ID_CMD_PLAY_BLACK;
                        break;

        case GAMEOVER:
                        red = true;
                        chess_clock.GameOver();
                        glc.TestResult( wx_stat );
                        stat = wx_stat.c_str();
                        b1 = "New Game";        b1_cmd = ID_CMD_NEW_GAME;
                        b2 = "Setup Position";  b2_cmd = ID_CMD_SET_POSITION;
                        b3 = NULL; // "Play as White";
                        b4 = NULL; // "Play as Black";
                        objs.log->Gameover();// initial_position, game_moves, glc.Get() );
                        break;

        case HUMAN:
        case PONDERING:
                        stat = reply_to.c_str();
                        b1 = "New Game";        b1_cmd = ID_CMD_NEW_GAME;
                        b2 = "Setup Position";  b2_cmd = ID_CMD_SET_POSITION;
                        b3 = "Swap sides";      b3_cmd = ID_CMD_SWAP_SIDES;
                        b4 = NULL; //"Play as Black";
                        break;

        case SLIDING:
        case SLIDING_MANUAL:
                        stat = slide_buf;
                        b1 = NULL; //"New Game";
                        b2 = NULL; //"Setup Position";
                        b3 = NULL; //"Play as White";
                        b4 = NULL; //"Play as Black";
                        break;

        case FAKE_BOOK_DELAY:
        case THINKING:
                        red = true;
                        stat = "Engine thinking...";
                        b1 = "New Game";    b1_cmd = ID_CMD_NEW_GAME;
                        b2 = NULL; //"Setup Position";
                        b3 = NULL; //"Play as White";
                        b4 = NULL; //"Play as Black";
                        break;

        case POPUP:
        case POPUP_MANUAL:
                        stat = "Select move";
                        b1 = NULL; //"New Game";
                        b2 = NULL; //"Setup Position";
                        b3 = NULL; //"Play as White";
                        b4 = NULL; //"Play as Black";
                        break;
    }
    bool white = gd.master_position.WhiteToPlay();
    if( canvas->gb->GetNormalOrientation() )
    {
        canvas->who_top->SetLabel   ( white ? "" : "B" );
        canvas->who_bottom->SetLabel( white ? "W" : "" );
    }
    else
    {
        canvas->who_top->SetLabel   ( white ? "W" : "" );
        canvas->who_bottom->SetLabel( white ? "" : "B" );
    }
    if( red )
        canvas->status->SetForegroundColour( *wxRED );
    else
        canvas->status->SetForegroundColour( *wxBLACK );
    canvas->status->SetLabel( stat?stat:"" );
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
        canvas->kibitz_ctrl->Show(false);
        canvas->kibitz_button1->Show(false);
        canvas->kibitz_button2->Show(false);
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
    canvas->SetSmallBox(stat?false:true);
    canvas->PositionButtons();
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
            ChessPosition pos = gd.master_position;
            char buf[128];
            strcpy( buf, pos.ForsythPublish().c_str() );
            kibitz_pos = pos;
            objs.rybka->Kibitz( pos, buf );
        }
    }
    if( objs.rybka && (state==MANUAL||state==HUMAN||state==PONDERING||state==THINKING||(kibitz&&state==GAMEOVER) ) )
        objs.rybka->SuspendResume( true );  // resume if suspended
    atom.StatusUpdate();
    atom.Focus();
}

void GameLogic::StatusWarning()
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
    if( objs.frame )
    {
        bool file_loaded =  gc.pgn_filename != "";
        bool refresh=false;
        std::string str;
        str = "File: ";
        if( !file_loaded )
            str += "(none)";
        else
            str += gc.pgn_filename;
        if( str != status_field1 )
        {
            status_field1 = str;
            refresh=true;
        }

        int nbr_modified=0;
        str = "";
        if( file_loaded )
        {
            for( int i=0; i<gc.gds.size(); i++ )
            {
                MagicBase *ptr = gc.gds[i].get();
                uint32_t game_being_edited = ptr->GetGameBeingEdited();
                if( ptr && ptr->IsModified() )
                {
                    nbr_modified++;
                }
                else if( ptr && game_being_edited )
                {
                    GameDocument *pd = tabs->Begin();
                    Undo *pu = tabs->BeginUndo();
                    while( pd && pu )
                    {
                        if( game_being_edited == pd->game_being_edited )
                        {
                            bool doc_modified = (pd->game_details_edited || pd->game_prefix_edited || pd->modified || pu->IsModified());
                            if( doc_modified )
                                nbr_modified++;
                            break;
                        }
                        pd = tabs->Next();
                        pu = tabs->NextUndo();
                    }
                }
            }
            bool doc_modified = (nbr_modified > 0);
            if( doc_modified )
                str = "*";
            char buf[80];
            if( idx >= 0 )
                sprintf( buf, doc_modified?"* Game %d of %ld":"Game %d of %ld", idx+1, gc.gds.size() );
            else if( nbr_modified )
            {
                sprintf( buf, doc_modified?"* File games: %ld (%d modified)":"File games: %ld (%d modified)",
                     gc.gds.size(), nbr_modified );
            }
            else
            {
                sprintf( buf, doc_modified?"* File games: %ld":"File games: %ld",
                     gc.gds.size() );
            }
            if( !tabs->GetInfile() )
                sprintf( buf, " ( this tab not in file ) " );
            str = buf;
        }
        if( str != status_field2 )
        {
            status_field2 = str;
            refresh=true;
        }
        if( refresh )
        {
            objs.frame->SetStatusText( status_field1.c_str(), 1 );
            objs.frame->SetStatusText( status_field2.c_str(), 2 );
        }
    }
}

void GameLogic::DoPopup( wxPoint &point, vector<Move> &target_moves,
                                         vector<Move> &book_moves,
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
        Move move = book_moves[i];

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
        Move move = target_moves[i];

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
            Move move = book_moves[i];

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
    canvas->popup = new PopupControl( objs.frame,strs,terses,book,popup_mode,hover,ID_POPUP,point );
}

bool GameLogic::CheckPopup( Move &move )
{
    bool done = false;
    move.Invalid();
    if( canvas->popup )
    {
        done = canvas->popup->done;
        if( done )
        {
            ChessRules temp = pre_popup_position;
            bool okay = move.TerseIn(&temp,canvas->popup->terse_move);
            if( !okay )
                move.Invalid();
            delete canvas->popup;
            canvas->popup = NULL;
        }
    }
    return done;
}

bool GameLogic::MakeMove( Move move, GAME_RESULT &result )
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
    chess_clock.Press( white );
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
        chess_clock.Press( !white );    // temp hack, undo previous press (almost)
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
    ChessPosition view_pos = gd.master_position;
    title.sprintf( "Initial position%s", qualifier );
    if( objs.canvas )
    {
        ChessRules cr;
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
        title = "Tarrasch Chess GUI";
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
    canvas->SetPosition( view_pos );
    if( objs.frame && show_title )
        objs.frame->SetTitle( title );
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
GAME_STATE GameLogic::StartThinking( const Move *human_move )
{
    GAME_STATE ret;
    bool ponderhit=false;
    if( human_or_pondering==PONDERING && human_move && *human_move==ponder_move )
    {
        ChessPosition pos = gd.master_position;
        ponderhit = objs.rybka->Ponderhit( pos );
    }
    if( ponderhit )
        ret = THINKING;
    else
    {
        // Use a move list if possible so engine can see repititions
        #define MAX_NBR_MOVES_IN_MOVELIST 100
        ChessPosition std_startpos;
        ChessPosition move_list_startpos;
        ChessPosition pos;
        std::vector<GAME_MOVE> game_moves;
        gd.GetSummary( move_list_startpos, game_moves, pos );

        // Use a move list if possible so engine can see repititions
        int n=game_moves.size();
        bool okay_to_use_move_list = true;
        wxString smoves;
        ChessRules cr;
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
            Move move  = game_moves[i].move;
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
        Move book_move;
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

        // If book move
        if( book_move.Valid() )
        {
            move_after_delay = book_move;
            unsigned long time = (gd.master_position.WhiteToPlay() ? wtime_ms : btime_ms ); 
            fake_book_delay = 1000;     // 1 second by default
            if( time < 4000 )
                fake_book_delay = 200;  // 200mS if less than 4 secs left
            else if( time < 10000 )
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
                objs.rybka->StartThinking( false, pos, use_startpos?NULL:forsyth, smoves, wtime_ms, btime_ms, winc_ms, binc_ms );   // ms,ms,inc_ms,inc_ms
            }
            else
            {
                bool use_startpos = pos.CmpStrict(std_startpos);
                if( !use_startpos )
                    strcpy( forsyth, gd.master_position.ForsythPublish().c_str() );
                objs.rybka->StartThinking( false, pos, use_startpos?NULL:forsyth, wtime_ms, btime_ms, winc_ms, binc_ms );   // ms,ms,inc_ms,inc_ms
            }
            ret = THINKING;
        }
    }
    return ret;
}

// Start engine pondering engine's own proposed ponder move
bool GameLogic::StartPondering( Move ponder )
{
    bool pondering=false;
    ChessPosition std_startpos;
    ChessPosition move_list_startpos;
    ChessPosition pos;
    std::vector<GAME_MOVE> game_moves;
    gd.GetSummary( move_list_startpos, game_moves, pos );

    // Use a move list if possible so engine can see repititions
    int n=game_moves.size();
    bool okay_to_use_move_list = true;
    wxString smoves;
    ChessRules cr;
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
        Move move  = game_moves[i].move;
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
        objs.rybka->StartThinking( true, pos, use_startpos?NULL:forsyth, smoves, wtime_ms, btime_ms, winc_ms, binc_ms );   // ms,ms,inc_ms,inc_ms
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
    Move candidate_move;
    candidate_move.Invalid();
    int depth=0, score_cp=0, rank_score_cp=0;
    ChessRules cr = gd.master_position;
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
    std::vector<Move> var;
    if( s )
    {
        if( mate )
            pv.sprintf( "#%d (depth %d)", mate, depth );
        else
            pv.sprintf( "%1.2f (depth %d)", ((double)score_cp)/100.0, depth );
        const char *txt = s+strlen(temp);
        while( *txt == ' ' )
            txt++;
        #define MAX_NBR_KIBITZ_MOVES 13 //14
        for( unsigned int i=0; /*i<MAX_NBR_KIBITZ_MOVES*/; i++ )
        {
            Move move;
            std::string nmove;
            bool have_move = move.TerseIn( &cr, txt );
            if( !have_move )
                break;
            else
            {
                txt += 4;
                if( isascii(*txt) && isalpha(*txt) )   // eg 'Q','R' etc.
                    txt++;
                while( *txt == ' ' )
                    txt++;
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
                if( *txt == '\0' )
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
    ChessRules cr = gd.master_position;
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
        const char *txt = s+strlen(temp);
        while( *txt == ' ' )
            txt++;
        for( unsigned int i=0; /*i<MAX_NBR_KIBITZ_MOVES*/; i++ )
        {
            Move move;
            std::string nmove;
            bool have_move=false;
            if( ponder && i==0 )
            {
                have_move = true;
                move = ponder_move;
            }
            else
            {
                have_move = move.TerseIn( &cr, txt );
                txt += 4;
                if( isalpha(*txt) )   // eg 'Q','R' etc.
                    txt++;
                while( *txt == ' ' )
                    txt++;
            }
            if( !have_move )
                break;
            else
            {
                wxString mv;
                bool white=cr.WhiteToPlay();
                nmove = move.NaturalOut(&cr);
                LangOut(nmove);
                if( white || i==0 )
                    mv.sprintf( " %d%s%s", cr.full_move_count, white?".":"...",nmove.c_str() );
                else
                    mv.sprintf( " %s", nmove.c_str() );
                cr.PlayMove( move );
                pv += mv;
                if( *txt == '\0' )
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


