/****************************************************************************
 * Run UCI chess engine, eg Rybka
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef RYBKA_H
#define RYBKA_H
#include "Portability.h"
#include "kibitzq.h"
#include "Appdefs.h"
#include "thc.h"
#include "wx/wx.h"

class Rybka
{
private:
    bool first;
    bool okay;
    enum RYBKA_STATE
    {
        INIT,
        WAIT_UCIOKAY,
        READY,
        SEND_PLAY_ENGINE1,
        SEND_PLAY_ENGINE2,
        SEND_PLAY_ENGINE3,
        SEND_PLAY_ENGINE4,
        WAIT_EVALUATION,
        SEND_QUIT_1,
        SEND_QUIT_2,
        WAIT_FINISHED,
        SEND_KIBITZ1,
        SEND_KIBITZ2,
        SEND_KIBITZ3,
        SEND_KIBITZ4,
        KIBITZING,
        SEND_KIBITZ_STOP,
        SEND_ISREADY_K,
        SEND_ISREADY_P,
        SEND_ISREADY_Q,
        WAIT_READYOK_K,
        WAIT_READYOK_P,
        WAIT_READYOK_Q,
        SEND_FORCE_STOP
    };
    RYBKA_STATE gbl_state;
    wxString gbl_smoves;
    char  gbl_forsyth[124];
    char  gbl_go[100];
    thc::Move  gbl_bestmove;
    thc::Move  gbl_ponder;
    Kibitzq kq[ NBR_KIBITZ_LINES ];
    Kibitzq kq_engine_to_move;
    const char *gbl_move;
    int gbl_score;
    const char *filename_uci_exe;
#ifdef WINDOWS_FIX_LATER
    STARTUPINFOA si;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    HANDLE newstdin,newstdout,read_stdout,write_stdin;  //pipe handles
#endif

public:
    bool debug_trigger;
    Rybka( const char *filename_uci_exe );
    ~Rybka();
    bool Start();  // return true if okay
    bool IsSuspended() { return suspended; }
    void SuspendResume( bool resume );
    void StartThinking( bool ponder, thc::ChessPosition &pos, const char *forsyth, long wtime_ms, long btime_ms, long winc_ms, long binc_ms );
    void StartThinking( bool ponder, thc::ChessPosition &pos, const char *forsyth, wxString &smoves, long wtime_ms, long btime_ms, long winc_ms, long binc_ms );
    bool Ponderhit( thc::ChessPosition &pos );
    void MoveNow();
    void ForceStop();
    const char *EngineName() { return engine_name[0] ? engine_name : NULL; }
    void Kibitz( thc::ChessPosition &pos, const char *forsyth );
    void KibitzStop();
    bool KibitzPeek( bool run, int multi_idx, bool &cleared, char buf[], unsigned int max_strlen );
    bool KibitzPeekEngineToMove( bool run, bool &cleared, char buf[], unsigned int max_strlen );
    thc::Move CheckBestMove( thc::Move &ponder );
    void Stop();
    bool Run(); // return true if running
private:
    void NewState( const char *comment, RYBKA_STATE new_state );
    void line_out( const char *s );
    void user_hook_out( const char *s );
    const char *user_hook_in();         // Input to Rybka
    bool WaitingForUciok( const char *s );
    void OptionIn( const char *s );
    char engine_name[80];
    int  stepper;
    bool ponder_found;
    bool ponder_first;
    bool ponder_sent;
    bool multipv_found;
    int  multipv_min;
    int  multipv_max;
    bool hash_found;
    bool hash_first;
    int  hash_min;
    int  hash_max;
    int  hash_sent;
    bool custom1_first;
    bool custom2_first;
    bool custom3_first;
    bool custom4_first;
    bool max_cpu_cores_found;
    bool max_cpu_cores_first;
    int  max_cpu_cores_min;
    int  max_cpu_cores_max;
    int  max_cpu_cores_sent;
    char max_cpu_cores_name[80];
    int nbr_cpus;
    bool suspended;
    bool last_command_was_go_infinite;
    bool bestmove_received;
    bool ponder_received;
    bool send_ponderhit;
    bool send_stop;
    thc::ChessPosition pos_engine_to_move;
    thc::ChessPosition pos_kibitz;
    RYBKA_STATE readyok_next_state;
    unsigned long readyok_basetime;
};

#endif // RYBKA_H
