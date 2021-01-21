/****************************************************************************
 * Run UCI chess engine, eg Stockfish - Unix version
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <stdio.h>
#include "Portability.h"

// Can be included in Windows builds if convenient because the whole thing is conditionally compiled
#ifdef THC_UNIX
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "UciInterface.h"
#include "DebugPrintf.h"
#include "Repository.h"
#include "Objects.h"
using namespace std;
using namespace thc;


/*

State transition notes:

UciInterface::Start()
    notes: busy waits for READY (can loop forever), is slow, needs okay
    INIT ->  WAIT_UCI  -> READY
    (tx:uci) (rx:uciokay)

UciInterface::Stop()
    notes: busy waits for process not running (protection against loop forever), is slow, needs okay
    ? -> SEND_QUIT_1 -> SEND_QUIT_2             -> WAIT_FINISHED
         (tx:stop)      (tx:isready,rx:readyok)    (tx:quit)
         [stop is optional - only after go infinite/go ponder or if start state is WAIT_EVALUATION]

UciInterface::StartThinking()
    notes: kicks off go command, two versions (with/without smoves), needs okay, for playing against engine
    ? -> SEND_PLAY_ENGINE1 -> SEND_PLAY_ENGINE2  -> SEND_PLAY_ENGINE3 -> SEND_PLAY_ENGINE4 -> WAIT_EVALUATION -> READY
         (tx:stop)            (tx:options)          (tx:position)        (:go)                (rx:bestmove)
         [stop is optional - only after go infinite/go ponder]

UciInterface::Kibitz()
    notes: kicks off go infinite command,  never sends smoves, needs okay
    ? -> SEND_KIBITZ1 -> SEND_KIBITZ2  -> SEND_KIBITZ3 -> SEND_KIBITZ4 -> KIBITZING
         (tx:stop)       (tx:MultiPV)     (tx:position)   (tx:go infinite)
         [stop is optional - only after go infinite/go ponder]

UciInterface::KibitzStop()
    notes: needs okay, does one Run() immediately
    ? -> SEND_KIBITZ_STOP -> READY
         (tx:stop)
         [stop is optional - only after go infinite]

WAIT_EVALUATION state
    notes: info -> kq_engine_to_move

KIBITZING state
    notes: info -> kq[multipv]

*/

// Doing this "JobObject" stuff is useful because it means that any processes started by
//  Tarrasch are terminated when Tarrasch terminates - avoids the possibility of zombie
//  engine processes still running once Tarrasch exits.
extern void JobBegin()
{
}

extern void JobEnd()
{
}


// Some smart string functions
const char *str_pattern( const char *str, const char *pattern, bool more=false );
const char *str_search( const char *str, const char *pattern, bool more=false );
const char *str_pattern_smart( const char *str, const char *pattern );

#define bzero(a) memset(a,0,sizeof(a)) //easier -- shortcut

static void ErrorMessage( const char *filename_uci_exe, char *str )  //display detailed error info
{
}

static int mac_fd_read;
static int mac_fd_write;

static fd_set mac_readfds;
fd_set *mac_fd = &mac_readfds;


unsigned long GetTickCount()
{
    static bool have_base;
    static timeval t1;
    timeval t2;
    unsigned long elapsed_time=0;
    if( !have_base )
    {
        gettimeofday(&t1, NULL);
        have_base = true;
    }
    else
    {
        gettimeofday(&t2, NULL);
        elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000;      // sec to ms
        elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000;   // us to ms
    }
    return elapsed_time;
}

void Sleep( int ms )
{
    fd_set fds;
    fd_set *p = &fds;
    FD_ZERO(p);
    struct timeval tv;
    tv.tv_sec  = ms/1000;
    tv.tv_usec = (ms%1000)*1000;
    select( 1, p, NULL, NULL, &tv );
}

int MacReadNonBlocking( int fd, char *buf, int max )
{
    int nbr_bytes = 0;
    fd_set fds;
    fd_set *p = &fds;
    FD_ZERO(p);
    FD_SET(fd,p);
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    select( fd+1, p, NULL, NULL, &tv );
    if( FD_ISSET(fd,p) )
    {
        nbr_bytes = read(fd,buf,max);
    }
    return nbr_bytes;
}

void MacWrite( int fd, const char *buf )
{
    write(fd,buf,strlen(buf));
}


//---------------------------------------------------------------------------
UciInterface::UciInterface( const char *filename_uci_exe )
{
    this->filename_uci_exe = filename_uci_exe;
    engine_name[0] = '\0';

    nbr_cpus = 1;   // How do we calculate this in Unix?
    kq_engine_to_move.SetDepth(6);  // small number
    bestmove_received = false;
    ponder_received = false;
    last_command_was_go_infinite = false;
    ponder_found = false;
    ponder_first = true;
    send_ponderhit = false;
    send_stop = false;
    multipv_found = false;
    multipv_min = 1;
    multipv_max = 4;
    hash_found = false;
    hash_first = true;
    hash_min = 1;
    hash_max = 1024;
    max_cpu_cores_found = false;
    max_cpu_cores_first = true; 
    max_cpu_cores_min = 1;
    max_cpu_cores_max = nbr_cpus;
    max_cpu_cores_name[0] = '\0';
    custom1_first = true; 
    custom2_first = true; 
    custom3_first = true; 
    custom4_first = true; 

    okay = false;
    suspended = false;
    first = true;
    gbl_move  = NULL;
    gbl_score = 0;
    gbl_state = INIT;
    debug_trigger = false;


    pid_t nPid;
    int pipeto[2];      // pipe to feed the exec'ed program input
    int pipefrom[2];    // pipe to get the exec'ed program output
    
    if ( pipe( pipeto ) != 0 )
    {
        perror( "pipe() to" );
        _exit(255);
    }
    if ( pipe( pipefrom ) != 0 )
    {
        perror( "pipe() from" );
        _exit(255);
    }
    
    nPid = fork();
    if ( nPid < 0 )
    {
        perror( "fork() 1" );
        _exit(255);
    }
    
    //
    //  fork in the road A) used to fire up the engine
    //
    else if ( nPid == 0 )
    {
        // dup pipe read/write to stdin/stdout
        dup2( pipeto[0], STDIN_FILENO );
        dup2( pipefrom[1], STDOUT_FILENO  );

        // close unnecessary pipe descriptors for a clean environment
        close( pipeto[0] );
        close( pipeto[1] );
        close( pipefrom[0] );
        close( pipefrom[1] );
        #ifdef THC_MAC
        execlp( "/Users/billforster/Downloads/critter_1.6a_osx/critter-16a", "/Users/billforster/Downloads/critter_1.6a_osx/critter-16a", NULL ); //"/Users/billforster/Downloads/critter_1.6a_osx/critter-16a"
        #else
        execlp( objs.repository->engine.m_file.c_str(),
        objs.repository->engine.m_file.c_str(), NULL );
        #endif
        perror( "execlp()" );
        _exit(255);
    }
    
    //
    // fork in the road B) our program continues
    //
    else
    {
        
        // Close unused pipe ends. This is especially important for the
        //  pipefrom[1] write descriptor, otherwise readFromPipe will never
        //  get an EOF.
        close( pipeto[0] );
        close( pipefrom[1] );
        mac_fd_read  = pipefrom[0];
        mac_fd_write = pipeto[1];
        okay = true;
    }
}

void UciInterface::SuspendResume( bool resume )
{ 
}


bool UciInterface::Start()
{
    if( okay )
    {
        NewState( "Start()", INIT );
        unsigned long base_time = GetTickCount();	
        while( gbl_state != READY )
        {
            bool running = Run();
            if( !running )
                break;
            unsigned long now_time = GetTickCount();	
            unsigned long elapsed_time = now_time-base_time;
            if( elapsed_time > 20000 )   // 20 seconds
            {
                okay = false;
                break;
            }
            Sleep(0);
        }
    }
    return okay;
}

void UciInterface::StartThinking( bool ponder, thc::ChessPosition &pos, const char *forsyth, long wtime_ms, long btime_ms, long winc_ms, long binc_ms )
{
    pos_engine_to_move = pos;
    if( forsyth )
    {
        strcpy( gbl_forsyth, "fen " );
        strcpy( gbl_forsyth+4, forsyth );
    }
    else
        strcpy( gbl_forsyth, "startpos" );
    gbl_smoves = "";
    long wtime = wtime_ms;
    long btime = btime_ms;
    if( pos_engine_to_move.WhiteToPlay() )
    {
        if( wtime > 1000 )
            wtime -= 900;   // hurry engine along, but don't reduce time to less than 100ms
        else if( wtime > 100 )
            wtime = 100;
    }
    else
    {
        if( btime > 1000 )
            btime -= 900;
        else if( btime > 100 )
            btime = 100;
    }
    cprintf( "Condition times to avoid race when engine is almost out of time: wtime=%ld (wtime_ms=%ld), btime=%ld (btime_ms=%ld)\n", 
            wtime, wtime_ms, btime, btime_ms );
    sprintf( gbl_go, "go%s wtime %ld btime %ld winc %ld binc %ld",
                         (ponder?" ponder":""), wtime, btime, winc_ms, binc_ms );
    bestmove_received = false;
    ponder_received = false;
    gbl_score = 0;
    if( okay )
        NewState( "go (no smoves)", SEND_PLAY_ENGINE1 );
}

void UciInterface::StartThinking( bool ponder, thc::ChessPosition &pos, const char *forsyth, wxString &smoves, long wtime_ms, long btime_ms, long winc_ms, long binc_ms )
{
    pos_engine_to_move = pos;
    if( forsyth )
    {
        strcpy( gbl_forsyth, "fen " );
        strcpy( gbl_forsyth+4, forsyth );
    }
    else
        strcpy( gbl_forsyth, "startpos" );
    gbl_smoves = smoves;
    long wtime = wtime_ms;
    long btime = btime_ms;
    if( pos_engine_to_move.WhiteToPlay() )
    {
        if( wtime > 1000 )
            wtime -= 900;   // hurry engine along, but don't reduce time to less than 100ms
        else if( wtime > 100 )
            wtime = 100;
    }
    else
    {
        if( btime > 1000 )
            btime -= 900;
        else if( btime > 100 )
            btime = 100;
    }
    cprintf( "Condition times to avoid race when engine is almost out of time: wtime=%ld (wtime_ms=%ld), btime=%ld (btime_ms=%ld)\n", 
            wtime, wtime_ms, btime, btime_ms );
    sprintf( gbl_go, "go%s wtime %ld btime %ld winc %ld binc %ld",
                         (ponder?" ponder":""), wtime, btime, winc_ms, binc_ms );
    bestmove_received = false;
    ponder_received = false;
    send_ponderhit = false;
    send_stop = false;
    gbl_score = 0;
    if( okay )
        NewState( "go (smoves)", SEND_PLAY_ENGINE1 );
}

bool UciInterface::Ponderhit( thc::ChessPosition &pos )
{
    bool okay = false;
    release_printf( "Ponderhit\n" );
    if( gbl_state==SEND_PLAY_ENGINE1 ||
        gbl_state==SEND_PLAY_ENGINE2 ||
        gbl_state==SEND_PLAY_ENGINE3 ||
        gbl_state==SEND_PLAY_ENGINE4 ||
        gbl_state==SEND_ISREADY_P ||
        gbl_state==WAIT_READYOK_P ||
        gbl_state == WAIT_EVALUATION
      )
    {
        okay = true;
        pos_engine_to_move = pos;
        send_stop = false;
        send_ponderhit = true;
        release_printf( "Ponderhit inside\n" );
        Run();
    }
    return okay;
}

void UciInterface::MoveNow()
{
    release_printf( "MoveNow\n" );
    if( gbl_state==SEND_PLAY_ENGINE1 ||
        gbl_state==SEND_PLAY_ENGINE2 ||
        gbl_state==SEND_PLAY_ENGINE3 ||
        gbl_state==SEND_PLAY_ENGINE4 ||
        gbl_state==SEND_ISREADY_P ||
        gbl_state==WAIT_READYOK_P ||
        gbl_state == WAIT_EVALUATION
      )
    {
        send_ponderhit = false;
        send_stop = true;
        last_command_was_go_infinite = true;
        release_printf( "MoveNow inside\n" );
        Run();
    }
}

void UciInterface::ForceStop()
{
    release_printf( "ForceStop\n" );
    if( gbl_state==SEND_PLAY_ENGINE1 ||
        gbl_state==SEND_PLAY_ENGINE2 ||
        gbl_state==SEND_PLAY_ENGINE3 ||
        gbl_state==SEND_PLAY_ENGINE4 ||
        gbl_state==SEND_ISREADY_P ||
        gbl_state==WAIT_READYOK_P ||
        gbl_state == WAIT_EVALUATION
      )
    {
        send_ponderhit = false;
        send_stop = false;
        last_command_was_go_infinite = false;
        if( gbl_state==SEND_ISREADY_P || gbl_state==WAIT_READYOK_P )
        {
            release_printf( "ForceStop 1\n" );
            readyok_next_state = READY;
        }
        else if( gbl_state == WAIT_EVALUATION )
        {
            release_printf( "ForceStop 2\n" );
            gbl_state = SEND_FORCE_STOP;
        }
        else
        {
            release_printf( "ForceStop 3\n" );
            gbl_state = READY;
        }
        Run();
    }
}

void UciInterface::KibitzStop()
{
    if( okay )
    {
        NewState( "KibitzStop()", SEND_KIBITZ_STOP );
        Run();
    }
}

void UciInterface::Kibitz( thc::ChessPosition &pos, const char *forsyth )
{
    bool skip=false;
    if( gbl_state==SEND_KIBITZ1 ||
        gbl_state==SEND_KIBITZ2 ||
        gbl_state==SEND_KIBITZ3 ||
        gbl_state==SEND_KIBITZ4 ||
        gbl_state==SEND_ISREADY_K ||
        gbl_state==WAIT_READYOK_K ||
        gbl_state==KIBITZING
      )
    {
        if( pos == pos_kibitz )
            skip = true;    // do nothing, this is a repeat kibitz
    }
    if( !skip )
    {
        // Don't ask chess engines to analyse illegal positions or checkmates/stalemates
        thc::ChessRules cr = pos;
        thc::TERMINAL terminal;
        bool ok = cr.Evaluate( terminal );
        if( ok )
        {
            bool game_over = (terminal==TERMINAL_WCHECKMATE || terminal==TERMINAL_WSTALEMATE || terminal==TERMINAL_BCHECKMATE || terminal==TERMINAL_BSTALEMATE );
            if( !game_over )
            {
                pos_kibitz = pos;
                strcpy( gbl_forsyth, "fen " );
                strcpy( gbl_forsyth+4, forsyth );
                sprintf( gbl_go, "go infinite" );
                gbl_score = 0;
                if( okay )
                    NewState( "Kibitz()", SEND_KIBITZ1 );
            }
        }
    }
}

bool UciInterface::KibitzPeek( bool run, int multi_idx, bool &cleared, char buf[], unsigned int max_strlen )
{
    if( multi_idx < 0 )
        multi_idx = 0;
    else if( multi_idx >= nbrof(kq) )
        multi_idx = nbrof(kq)-1;
    cleared = kq[multi_idx].TestCleared();
    bool have_data = false;
    if( run )
        Run();
    have_data = kq[multi_idx].Get( buf, max_strlen );
    return have_data;    
}

bool UciInterface::KibitzPeekEngineToMove( bool run, bool &cleared, char buf[], unsigned int max_strlen )
{
    cleared = kq_engine_to_move.TestCleared();
    bool have_data = false;
//    if( gbl_state != READY )
    if( run )
        Run();
    have_data = kq_engine_to_move.Get( buf, max_strlen );
    return have_data;    
}

thc::Move UciInterface::CheckBestMove( thc::Move &ponder )
{
    thc::Move move;
    move.Invalid();
    ponder.Invalid();
    Run();
    if( bestmove_received )
    {
        move = gbl_bestmove;
        bestmove_received = false;
        if( ponder_received )
        {
            ponder = gbl_ponder;
            ponder_received = false;
        }
    }
    return move;
}

void UciInterface::Stop()
{
    debug_trigger = true;
    if( !okay )
        dbg_printf( "Stop: not okay\n" );
    else
    {
        dbg_printf( "Stop: okay\n" );
        if( gbl_state == WAIT_EVALUATION )
            last_command_was_go_infinite = true;    // usually only send stop if go infinite, but here if waiting
                                                    //  for engine to move, also need to send stop
        NewState( "Stop()", SEND_QUIT_1 );
        unsigned long base_time = GetTickCount();	
        unsigned long elapsed_time_changed=0;
        bool first=true;
        for( unsigned long i=0; true /*i<10000*/; i++ )
        {
            bool running = Run();
            unsigned long now_time = GetTickCount();	
            unsigned long elapsed_time = now_time-base_time;
            if( first || elapsed_time!=elapsed_time_changed )
                release_printf( "Waiting for engine to die: running=%s, elapsed_time=%lu, loops=%lu\n", running?"true":"false", elapsed_time, i );
            elapsed_time_changed = elapsed_time;
            first = false;
            if( !running )
                break;
            if( elapsed_time > 20000 )   // 20 seconds
                break;
            Sleep(0);  //problem in V2.01 sleep() and Sleep() used!!
        }
    }
}

bool UciInterface::Run()
{
    bool running;
    char buf[1024+2];        //i/o buffer
    //GetExitCodeProcess(pi.hProcess,&exit);      //check the process is running
    running = true; //(exit==STILL_ACTIVE);
    if( running )
    {
        int nbr_bytes = MacReadNonBlocking(mac_fd_read,buf,sizeof(buf)-2);
        while( nbr_bytes > 0 )
        {
            release_printf( "Read nbr_bytes=%d\n", nbr_bytes );
            buf[nbr_bytes] = '\0';
            user_hook_out(buf);
            nbr_bytes = MacReadNonBlocking(mac_fd_read,buf,sizeof(buf)-2);
        }
        const char *user_ptr = user_hook_in();
        if( user_ptr )      //check for user input.
        {
            MacWrite(mac_fd_write,user_ptr);
            MacWrite(mac_fd_write,"\n");
        }
    }
    return running;
}

UciInterface::~UciInterface()
{
}

#define DEPTH 8
#define DEPTH_STR "8"

// Send the commands to engine at the correct time
const char *UciInterface::user_hook_in()
{
    static char buf[1024];
    const char *s=NULL;
    switch( gbl_state )
    {
        default: break;
        case INIT:
        {
            s = "uci";
            NewState( "user_hook_in()", WAIT_UCIOKAY );
            engine_name[0] = '\0';
            max_cpu_cores_name[0] = '\0';
            ponder_found = false;
            multipv_found = false;
            multipv_min = 1;
            multipv_max = 4;
            hash_found = false;
            hash_min = 1;
            hash_max = 1024;
            max_cpu_cores_found = false;
            max_cpu_cores_min = 1;
            max_cpu_cores_max = nbr_cpus;
            break;
        }
        case SEND_FORCE_STOP:
        {
            s = "stop";
            NewState( "user_hook_in()", SEND_ISREADY_P );
            readyok_next_state = READY;
            break;              // break and send "stop"
        }
        case SEND_ISREADY_P:    // send isready before play
        {
            s = "isready";
            NewState( "user_hook_in()", WAIT_READYOK_P );
            readyok_basetime = GetTickCount();
            break;
        }
        case SEND_ISREADY_K:    // send isready before kibitz
        {
            s = "isready";
            NewState( "user_hook_in()", WAIT_READYOK_K );
            readyok_basetime = GetTickCount();
            break;
        }
        case SEND_ISREADY_Q:    // send isready before quit
        {
            s = "isready";
            NewState( "user_hook_in()", WAIT_READYOK_Q );
            readyok_basetime = GetTickCount();
            break;
        }
        case WAIT_READYOK_P:
        case WAIT_READYOK_K:
        case WAIT_READYOK_Q:
        {
            s = NULL;
            unsigned long now_time = GetTickCount();
            if( now_time-readyok_basetime > 10000 )     // 10 seconds
                NewState( "timeout", readyok_next_state );
            break;
        }
        case SEND_PLAY_ENGINE1:
        {
            stepper = 0;
            if( last_command_was_go_infinite )
            {
                s = "stop";
                NewState( "user_hook_in()", SEND_ISREADY_P );
                readyok_next_state = SEND_PLAY_ENGINE2;
                break;  // break and send "stop"
            }
            // else fall through and send something more relevant
            NewState( "user_hook_in()", SEND_PLAY_ENGINE2 );
        }
        case SEND_PLAY_ENGINE2:
        {
            static char option_buf[300];
            bool found_something_to_send = false;
            while( !found_something_to_send && stepper<=7 )
            {
                EngineConfig *rep = &objs.repository->engine;
                switch( stepper )
                {
                    case 0:
                    {
                        if( multipv_found )
                        {
                            found_something_to_send = true;
                            sprintf( option_buf, "setoption name MultiPV value 1" );
                        }
                        break;
                    }
                    case 1:
                    {
                        if( ponder_found )
                        {
                            if( ponder_first || ponder_sent!=rep->m_ponder )
                            {
                                ponder_first = false;
                                ponder_sent  = rep->m_ponder;
                                found_something_to_send = true;
                                sprintf( option_buf, "setoption name Ponder value %s", rep->m_ponder?"true":"false" );
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        if( hash_found )
                        { 
                            if( hash_first || hash_sent!=rep->m_hash )
                            {
                                hash_first = false;
                                hash_sent  = rep->m_hash;
                                found_something_to_send = true;
                                int hash = rep->m_hash;
                                if( hash < hash_min )
                                    hash = hash_min;
                                else if( hash > hash_max )
                                    hash = hash_max;
                                sprintf( option_buf, "setoption name Hash value %d", hash );
                            }
                        }
                        break;
                    }
                    case 3:
                    {
                        if( max_cpu_cores_found )
                        {
                            if( max_cpu_cores_first || max_cpu_cores_sent!=rep->m_max_cpu_cores )
                            {
                                max_cpu_cores_first = false;
                                max_cpu_cores_sent  = rep->m_max_cpu_cores;
                                found_something_to_send = true;
                                int max_cpu_cores = rep->m_max_cpu_cores;
                                if( max_cpu_cores < max_cpu_cores_min )
                                    max_cpu_cores = max_cpu_cores_min;
                                else if( max_cpu_cores > max_cpu_cores_max )
                                    max_cpu_cores = max_cpu_cores_max;
                                sprintf( option_buf, "setoption name %s value %d", max_cpu_cores_name, max_cpu_cores );
                            }
                        }
                        break;
                    }
                    case 4:
                    {
                        if( custom1_first )
                        {
                            if( rep->m_custom1a.Len()>0 && rep->m_custom1a.Len()<=100 &&
                                rep->m_custom1b.Len()>0 && rep->m_custom1b.Len()<=100 )
                            {
                                custom1_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom1a.c_str();
                                const char *s2 = rep->m_custom1b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2  );
                            }
                        }
                        break;
                    }
                    case 5:
                    {
                        if( custom2_first )
                        {
                            if( rep->m_custom2a.Len()>0 && rep->m_custom2a.Len()<=100 &&
                                rep->m_custom2b.Len()>0 && rep->m_custom2b.Len()<=100 )
                            {
                                custom2_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom2a.c_str();
                                const char *s2 = rep->m_custom2b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2 );
                            }
                        }
                        break;
                    }
                    case 6:
                    {
                        if( custom3_first )
                        {
                            if( rep->m_custom3a.Len()>0 && rep->m_custom3a.Len()<=100 &&
                                rep->m_custom3b.Len()>0 && rep->m_custom3b.Len()<=100 )
                            {
                                custom3_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom3a.c_str();
                                const char *s2 = rep->m_custom3b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2 );
                            }
                        }
                        break;
                    }
                    case 7:
                    {
                        if( custom4_first )
                        {
                            if( rep->m_custom4a.Len()>0 && rep->m_custom4a.Len()<=100 &&
                                rep->m_custom4b.Len()>0 && rep->m_custom4b.Len()<=100 )
                            {
                                custom4_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom4a.c_str();
                                const char *s2 = rep->m_custom4b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2 );
                            }
                        }
                        break;
                    }
                }   // end switch
                stepper++;
            } // end while

            if( found_something_to_send )
                s = option_buf;    
            else
            {
                s = NULL;
                readyok_next_state = SEND_PLAY_ENGINE3;
                NewState( "user_hook_in()", SEND_ISREADY_P );
            }
            break;
        }
        case SEND_PLAY_ENGINE3:
        {
            if( gbl_smoves.Len() == 0 )
                sprintf( buf, "position %s", gbl_forsyth );
            else
            {
                const char *temp = gbl_smoves.c_str();
                sprintf( buf, "position %s moves%s", gbl_forsyth, temp );
            }
            s = buf;
            NewState( "user_hook_in()", SEND_PLAY_ENGINE4 );
            break;
        }
        case SEND_PLAY_ENGINE4:
        {
            s = gbl_go;
            //s = "go depth " DEPTH_STR;
            kq_engine_to_move.Clear();
            for( unsigned int i=0; i<nbrof(kq); i++ )
                kq[i].Clear();  // must clear all together - so that when one clear detected, all can be flushed
            bestmove_received = false;
            ponder_received = false;
            NewState( "user_hook_in()", WAIT_EVALUATION );
            break;
        }
        case WAIT_EVALUATION:
        {
            if( send_ponderhit )
            {
                send_ponderhit = false;
                s = "ponderhit";
            }
            else if( send_stop )
            {
                send_stop = false;
                s = "stop";
            }
            break;
        }
        case SEND_KIBITZ1:
        {
            stepper = 0;
            if( last_command_was_go_infinite )
            {
                s = "stop";
                NewState( "user_hook_in()", SEND_ISREADY_K );
                readyok_next_state = SEND_KIBITZ2;
                break;  // break and send "stop"
            }
            // else fall through and send something more relevant
            NewState( "user_hook_in()", SEND_KIBITZ2 );
        }
        case SEND_KIBITZ2:
        {
            static char option_buf[300];
            bool found_something_to_send = false;
            while( !found_something_to_send && stepper<=6 )
            {
                EngineConfig *rep = &objs.repository->engine;
                switch( stepper )
                {
                    case 0:
                    {
                        if( multipv_found )
                        {
                            found_something_to_send = true;
                            if( NBR_KIBITZ_LINES <= 2 )
                                sprintf( option_buf, "setoption name MultiPV value 2" );
                            else if( NBR_KIBITZ_LINES == 3 )
                                sprintf( option_buf, "setoption name MultiPV value 3" );
                            else
                                sprintf( option_buf, "setoption name MultiPV value 4" );
                        }
                        break;
                    }
                    case 1:
                    {
                        if( hash_found )
                        { 
                            if( hash_first || hash_sent!=rep->m_hash )
                            {
                                hash_first = false;
                                hash_sent  = rep->m_hash;
                                found_something_to_send = true;
                                int hash = rep->m_hash;
                                if( hash < hash_min )
                                    hash = hash_min;
                                else if( hash > hash_max )
                                    hash = hash_max;
                                sprintf( option_buf, "setoption name Hash value %d", hash );
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        if( max_cpu_cores_found )
                        {
                            if( max_cpu_cores_first || max_cpu_cores_sent!=rep->m_max_cpu_cores )
                            {
                                max_cpu_cores_first = false;
                                max_cpu_cores_sent  = rep->m_max_cpu_cores;
                                found_something_to_send = true;
                                int max_cpu_cores = rep->m_max_cpu_cores;
                                if( max_cpu_cores < max_cpu_cores_min )
                                    max_cpu_cores = max_cpu_cores_min;
                                else if( max_cpu_cores > max_cpu_cores_max )
                                    max_cpu_cores = max_cpu_cores_max;
                                sprintf( option_buf, "setoption name %s value %d", max_cpu_cores_name, max_cpu_cores );
                            }
                        }
                        break;
                    }
                    case 3:
                    {
                        if( custom1_first )
                        {
                            if( rep->m_custom1a.Len()>0 && rep->m_custom1a.Len()<=100 &&
                                rep->m_custom1b.Len()>0 && rep->m_custom1b.Len()<=100 )
                            {
                                custom1_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom1a.c_str();
                                const char *s2 = rep->m_custom1b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2 );
                            }
                        }
                        break;
                    }
                    case 4:
                    {
                        if( custom2_first )
                        {
                            if( rep->m_custom2a.Len()>0 && rep->m_custom2a.Len()<=100 &&
                                rep->m_custom2b.Len()>0 && rep->m_custom2b.Len()<=100 )
                            {
                                custom2_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom2a.c_str();
                                const char *s2 = rep->m_custom2b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2 );
                            }
                        }
                        break;
                    }
                    case 5:
                    {
                        if( custom3_first )
                        {
                            if( rep->m_custom3a.Len()>0 && rep->m_custom3a.Len()<=100 &&
                                rep->m_custom3b.Len()>0 && rep->m_custom3b.Len()<=100 )
                            {
                                custom3_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom3a.c_str();
                                const char *s2 = rep->m_custom3b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2 );
                            }
                        }
                        break;
                    }
                    case 6:
                    {
                        if( custom4_first )
                        {
                            if( rep->m_custom4a.Len()>0 && rep->m_custom4a.Len()<=100 &&
                                rep->m_custom4b.Len()>0 && rep->m_custom4b.Len()<=100 )
                            {
                                custom4_first = false;
                                found_something_to_send = true;
                                const char *s1 = rep->m_custom4a.c_str();
                                const char *s2 = rep->m_custom4b.c_str();
                                sprintf( option_buf, "setoption name %s value %s", s1, s2 );
                            }
                        }
                        break;
                    }
                }   // end switch
                stepper++;
            } // end while

            if( found_something_to_send )
                s = option_buf;    
            else
            {
                s = NULL;
                readyok_next_state = SEND_KIBITZ3;
                NewState( "user_hook_in()", SEND_ISREADY_K );
            }
            break;
        }
        case SEND_KIBITZ3:
        {
            sprintf( buf, "position %s", gbl_forsyth );
            s = buf;
            NewState( "user_hook_in()", SEND_KIBITZ4 );
            break;
        }
        case SEND_KIBITZ4:
        {
            s = gbl_go;
            kq_engine_to_move.Clear();
            for( unsigned int i=0; i<nbrof(kq); i++ )
                kq[i].Clear();  // must clear all together - so that when one clear detected, all can be flushed
            NewState( "user_hook_in()", KIBITZING );
            break;
        }
        case SEND_QUIT_1:
        {
            s = NULL;
            if( last_command_was_go_infinite )
                s = "stop";
            NewState( "user_hook_in()", SEND_ISREADY_Q );
            readyok_next_state = SEND_QUIT_2;
            break;
        }
        case SEND_QUIT_2:
        {
            s = "quit";
            NewState( "user_hook_in()", WAIT_FINISHED );
            break;
        }
        case SEND_KIBITZ_STOP:
        {
            s = NULL;
            if( last_command_was_go_infinite )
            {
                s = "stop";
                NewState( "user_hook_in()", SEND_ISREADY_P );
                readyok_next_state = READY;
            }
            else
                NewState( "user_hook_in()", READY );
            break;
        }
    }
    if( s )
    {
        release_printf( "in: %s\n", s );
        last_command_was_go_infinite = (
                                            0 == memcmp(s,"go infinite",11) ||
                                            0 == memcmp(s,"go ponder",9)
                                       );
    }
    return s;
}

// Collect output from engine and break into lines
void UciInterface::user_hook_out( const char *s )
{
    static char buf[1024];
    static int idx;
    while( *s )
    {
        if( *s=='\n' || *s=='\r' || idx>=sizeof(buf)-2 )
        {
            buf[idx] = '\0';
            if( buf[0] )
                line_out(buf);
            idx = 0;
        }
        else
            buf[idx++] = *s;
        s++;
    }
}

// Interpret the output lines from UCI engine, print the relevant lines
//  and advance the state machine when appropriate
void UciInterface::line_out( const char *s )
{
    const char *p, *temp;
    release_printf( "out: %s\n", s );
    switch( gbl_state )
    {
        case WAIT_READYOK_P:
        case WAIT_READYOK_K:
        case WAIT_READYOK_Q:
        {
            if( str_pattern(s,"readyok",false) )
                NewState( "readyok received", readyok_next_state );
            break;
        }
        case WAIT_UCIOKAY:
        {
            bool uciok = WaitingForUciok( s );
            if( uciok )
                NewState( "line_out()", READY );
            else
            {
                const char *parm;
                parm = str_pattern(s,"option name",true);
                if( parm )
                    OptionIn(parm);
            }
            break;
        }
        case WAIT_EVALUATION:
        {
            if( 0 == memcmp(s,"info ",5) && strstr(s," pv ") )
            {
                kq_engine_to_move.Put(s+4);
                dbg_printf( "Kibitz engine to move info:%s\n", s+4 );
            }
            p = strstr(s,temp="bestmove ");
            if( p )
            {
                p += strlen(temp);
                thc::Move move;
                thc::ChessRules cr=pos_engine_to_move;
                bool legal = move.TerseIn(&cr,p);
                if( !legal )
                    release_printf( "**!! False bestmove %s detected !!**\n", p );
                else
                {
                    gbl_bestmove = move;
                    bestmove_received = true;
                    NewState( "line_out()", READY );
                    p = strstr(s,temp="ponder ");
                    if( p && ponder_sent )
                    {
                        cr.PlayMove(move);
                        p += strlen(temp);
                        thc::Move ponder;
                        bool okay = ponder.TerseIn(&cr,p);
                        if( okay )
                        {
                            gbl_ponder = ponder;
                            ponder_received = true;
                        }
                    }
                }
            }
            break;
        }
        case KIBITZING:
        {
            if( strstr(s," multipv 3 ") )
                dbg_printf( "Break here");
            if( 0==memcmp(s,"info ",5) && strstr(s," pv ") )
            {
                int idx = 0; // first attempt at V2 used 0, changed to -1 for when
                             //  attempting fix for Komodo special edition,
                             //  back to 0 June 2012, allows Kibitzing with Strelka
                             //  which doesn't have multipv
                if( strstr(s," multipv 1 ") )
                    idx = 0;
                else if( strstr(s," multipv 2 ") )
                    idx = 1;
                else if( strstr(s," multipv 3 ") )
                    idx = 2;
                else if( strstr(s," multipv 4 ") )
                    idx = 3;
                if( idx >= 0 )
                {
                    kq[idx].Put(s+4);
                    dbg_printf( "Kibitz info(%d):%s\n", idx, s+4 );
                }
            }
            break;
        }
    }
}

bool UciInterface::WaitingForUciok( const char *s )
{
    bool uciokay = false;
    if( strstr(s,"id name ") )
    {
        memset( engine_name, 0, sizeof(engine_name) );
        strncpy( engine_name, strstr(s,"id name ")+8, sizeof(engine_name)-1 );
    }
    if( str_pattern(s,"uciok",false) )
        uciokay = true;
    return uciokay;
}

void UciInterface::OptionIn( const char *s )
{
    const char *parm;

    // MultiPV
    parm = str_pattern(s,"MultiPV",true);
    if( parm && str_search(parm,"type spin",true) )
    {
        const char *parm2;
        multipv_found = true;
        multipv_min = 1;
        multipv_max = 4;
        parm2 = str_search(parm,"min",true);
        if( parm2 )
        {
            multipv_min = atoi(parm2);
            if( multipv_min < 1 )
                multipv_min = 1;
            if( multipv_min > 1 )
                multipv_min = 1; // 1 is the only acceptable value!
        }
        parm2 = str_search(parm,"max",true);
        if( parm2 )
        {
            multipv_max = atoi(parm2);
            if( multipv_max > 4 )
                multipv_max = 4;
            if( multipv_max < multipv_min )
                multipv_max = multipv_min + 1;
        }
    }

    // Hash
    parm = str_pattern(s,"Hash",true);
    if( parm && str_search(parm,"type spin",true) )
    {
        const char *parm2;
        hash_found = true;
        hash_min = 1;
        hash_max = 1024;
        parm2 = str_search(parm,"min",true);
        if( parm2 )
        {
            hash_min = atoi(parm2);
            if( hash_min < 0 )
                hash_min = 0;
            if( hash_min > 64 )
                hash_min = 64;
        }
        parm2 = str_search(parm,"max",true);
        if( parm2 )
        {
            hash_max = atoi(parm2);
            if( hash_max <= hash_min )
                hash_max = hash_min + 16;
        }
    }

    // Ponder
    parm = str_pattern(s,"Ponder",true);
    if( parm && str_search(parm,"type check",true) )
        ponder_found = true;

    // max_cpu_cores
    parm = str_pattern_smart(s,"nbr|number|max|maximum*processors|cpus|cores");
    if( parm && str_search(parm,"type spin",true) )
    {
        memset( max_cpu_cores_name, 0, sizeof(max_cpu_cores_name) );
        int len = parm-s;
        if( len >= sizeof(max_cpu_cores_name) ) 
            len = sizeof(max_cpu_cores_name)-1; 
        memcpy( max_cpu_cores_name, s, len );
        const char *parm2;
        max_cpu_cores_found = true;
        max_cpu_cores_min = 1;
        max_cpu_cores_max = nbr_cpus;
        parm2 = str_search(parm,"min",true);
        if( parm2 )
        {
            max_cpu_cores_min = atoi(parm2);
            if( max_cpu_cores_min < 1 )
                max_cpu_cores_min = 1;
            if( max_cpu_cores_min > nbr_cpus )
                max_cpu_cores_min = nbr_cpus;  
        }
        parm2 = str_search(parm,"max",true);
        if( parm2 )
        {
            max_cpu_cores_max = atoi(parm2);
            if( max_cpu_cores_max <= max_cpu_cores_min )
                max_cpu_cores_max = max_cpu_cores_min;
        }
    }
}

// Case insensitive pattern match
// Return NULL if no match
// Return ptr into string beyond matched part of string
// Only if more is true, can the string be longer than the matching part
// So if more is false, and NULL is not returned, the returned ptr always points at trailing '\0'
const char *str_pattern( const char *str, const char *pattern, bool more )
{
    bool match=true;
    int c, d;
    while( match && *str && *pattern )
    {
        c = *str++;
        d = *pattern++;
        if( c != d )
        {
            match = false;
            if( isascii(c) && isascii(d) && toupper(c)==toupper(d) )
                match = true;
            else if( c=='\t' && d==' ' )
                match = true;
        }
        if( match && (c==' '||c=='\t') )
        {
            while( *str == ' ' )
                str++;
        }
    }
    if( !match )
        str = NULL;
    else if( *pattern )
        str = NULL;
    else if( !more && *str )
        str = NULL;
    if( more && str )
    {
        while( *str == ' ' )
            str++;
    }
    return str;
}

// Case insensitive pattern match - search for start of pattern
// Return NULL if no match
// Return ptr into string beyond matched part of string
// Only if more is true, can the string be longer than the matching part
// So if more is false, and NULL is not returned, the returned ptr always points at trailing '\0'
const char *str_search( const char *str, const char *pattern, bool more )
{
    bool found = false;
    while( *str && !found )
    {
        const char *src = str;
        const char *dst = pattern;
        bool match = true;
        while( *src && *dst && match )
        {
            int c = *src++;
            int d = *dst++;
            if( c != d )
            {
                match = false;
                if( isascii(c) && isascii(d) && toupper(c)==toupper(d) )
                    match = true;
                else if( c=='\t' && d==' ' )
                    match = true;
            }
        }
        if( match )
            found = true;
        else
            str++;  // continue search
    }
    const char *result = NULL;
    if( found )
        result = str_pattern( str, pattern, more );
    return result;
}

// Case insensitive pattern match - smart version
// Return NULL if no match
// Return ptr into string beyond matched part of string
// special pattern characters
//   '|'  alternative token
//   '*'  any number of tokens including zero
//   '-'  don't skip tokens
// example;
//   parm = str_pattern_smart(s,"nbr|number|max|maximum*processors|cpus|cores");
const char *str_pattern_smart( const char *str, const char *pattern )
{
/*
    skip = false
    loop
        read token      // token = maximum
                        // pattern -> nbr|number|maximum|max  (skip = false)
                        // pattern -> cpus|processors|cores   (skip = true)
        save pattern
        loop
            read pattern   nbr,number,maximum,max
            if match found=true
        if not found and not skip fail
        if not found
            restore pattern
        else
            if pattern ended in '*'
                skip = true
            else
                skip = false
*/
    char buf_str[128];
    char buf_pattern[128];
    int c='\0', d;
    bool ultimate_success = false;
    bool skip=false;
    for(;;)
    {

        // Success ?
        if( *pattern=='\0' )
        {
            ultimate_success = true;
            break;
        }    

        // Failure ?
        else if( *str == '\0' )
            break;

        // Read next token from string
        while( *str && isascii(*str) && !isalnum(*str) )
            str++;
        char *dst = buf_str;
        for( int i=0; i<sizeof(buf_str)-2; i++ )
        {
            c = *str;
            *dst++ = *str++;
            d = *str;
            if( d=='\0' || d==' ' || d=='\t' )
                break;
            if( isascii(c) && isascii(d) )
            {
                if( isupper(c) && !isalpha(d) ) // eg MAX_
                    break;                            
                if( islower(c) && !islower(d) ) // eg MaxN
                    break;                            
            }
        }
        *dst = '\0';

        // Loop reading pattern tokens
        const char *save_pattern = pattern;
        bool found_token_in_patterns=false;
        for(;;)
        {
            char *dst = buf_pattern;
            for( int i=0; i<sizeof(buf_pattern)-2; i++ )
            {
                *dst++ = *pattern++;
                c = *pattern;
                if( c=='*' || c=='-' || c=='|' || c=='\0' )
                {
                    if( c != '\0' )
                        pattern++;
                    break;
                }
            }
            *dst = '\0';
            bool match;
            match = (0 == strcmp_ignore(buf_pattern,buf_str));
            if( match )
                found_token_in_patterns = true;
            if( c != '|' )
                break;
        }

        if( found_token_in_patterns )
            skip = (c=='*');    // continue to next token, next pattern
        else
        {
            if( skip )
                pattern = save_pattern;  // try again with a new token
            else
                break;  // fail
        }
    }
    return( ultimate_success ? str : NULL );
}

void UciInterface::NewState( const char *comment, UCI_INTERFACE_STATE new_state )
{
    UCI_INTERFACE_STATE state;
    const char *s1="", *s2="", *s;
    state = gbl_state;
    for( int i=0; i<2; i++ )
    {
        switch( state )
        {
            default:                s = "??";
                                    break;
            case INIT:              s = "INIT";
                                    break;
            case WAIT_UCIOKAY:      s = "WAIT_UCIOKAY";
                                    break;
            case READY:             s = "READY";
                                    break;
            case SEND_PLAY_ENGINE1: s = "SEND_PLAY_ENGINE1";
                                    break;
            case SEND_PLAY_ENGINE2: s = "SEND_PLAY_ENGINE2";
                                    break;
            case SEND_PLAY_ENGINE3: s = "SEND_PLAY_ENGINE3";
                                    break;
            case SEND_PLAY_ENGINE4: s = "SEND_PLAY_ENGINE4";
                                    break;
            case WAIT_EVALUATION:   s = "WAIT_EVALUATION";
                                    break;
            case SEND_QUIT_1:       s = "SEND_QUIT_1";
                                    break;
            case SEND_QUIT_2:       s = "SEND_QUIT_2";
                                    break;
            case WAIT_FINISHED:     s = "WAIT_FINISHED";
                                    break;
            case SEND_KIBITZ1:      s = "SEND_KIBITZ1";
                                    break;
            case SEND_KIBITZ2:      s = "SEND_KIBITZ2";
                                    break;
            case SEND_KIBITZ3:      s = "SEND_KIBITZ3";
                                    break;
            case SEND_KIBITZ4:      s = "SEND_KIBITZ4";
                                    break;
            case KIBITZING:         s = "KIBITZING";
                                    break;
            case SEND_KIBITZ_STOP:  s = "SEND_KIBITZ_STOP";
                                    break;
            case SEND_ISREADY_K:    s = "SEND_ISREADY_K";
                                    break;
            case SEND_ISREADY_P:    s = "SEND_ISREADY_P";
                                    break;
            case SEND_ISREADY_Q:    s = "SEND_ISREADY_Q";
                                    break;
            case WAIT_READYOK_K:    s = "WAIT_READYOK_K";
                                    break;
            case WAIT_READYOK_P:    s = "WAIT_READYOK_P";
                                    break;
            case WAIT_READYOK_Q:    s = "WAIT_READYOK_Q";
                                    break;
            case SEND_FORCE_STOP:   s = "SEND_FORCE_STOP";
                                    break;
        }
        if( i == 1 )
            s2 = s;
        else
        {
            state = new_state;
            s1 = s;
        }
    }
    gbl_state = new_state;
    release_printf( "State change %s -> %s (%s)\n", s1, s2, comment );
}

#endif // THC_UNIX

