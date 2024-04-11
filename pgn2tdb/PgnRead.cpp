/****************************************************************************
 * Facility to lookup book moves using game positions read from a .pgn
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <random>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "thc.h"
#include "BinDb.h"
#include "PgnRead.h"
#include "DebugPrintf.h"


using namespace std;
using namespace thc;
#define nbrof(array) ( sizeof(array) / sizeof((array)[0]) )

// Constructor
PgnRead::PgnRead( char callback_code, ProgressBar *pb )
{
    this->callback_code = callback_code;
    this->pb = pb;
    round   [0] = '\0';
    white_elo[0] = '\0';
    black_elo[0] = '\0';
    fen    [0] = '\0';
    desc   [0] = '\0';
    desc2  [0] = '\0';
    name   [0] = '\0';
    variation[0] = '\0';
    date   [0] = '\0';
    white  [0] = '\0';
    black  [0] = '\0';
    result [0] = '\0';
    eco[0] = '\0';
    event  [0] = '\0';
    site   [0] = '\0';
    move_order_type[0] = '\0';
    fen_flag = false;
    first_move = false;
    first_move_offset = 0;
    nbr_games = 0;
    file_rep = NULL;
    file_inc = NULL;
    debug_log_file_txt = NULL;
    error_ptr = 0;
    debug_ptr = 0;
    stack_idx = 0;
    memset( stack_array, 0, sizeof(stack_array) );
}

const char *PgnRead::ShowState( STATE state )
{
    const char *s="?";
    switch( state )
    {
        case INIT:
            s = "INIT"; break;
        case PREFIX:
            s = "PREFIX";   break;
        case HEADER:
            s = "HEADER";   break;
        case IN_COMMENT:
            s = "IN_COMMENT";   break;
        case BETWEEN_MOVES:
            s = "BETWEEN_MOVES";    break;
        case MOVE_NUMBER:
            s = "MOVE_NUMBER";  break;
        case POST_MOVE_NUMBER:
            s = "POST_MOVE_NUMBER"; break;
        case POST_MOVE_NUMBER_HAVE_PERIOD:
            s = "POST_MOVE_NUMBER_HAVE_PERIOD"; break;
        case POST_MOVE_NUMBER_BLACK:
            s = "POST_MOVE_NUMBER_BLACK";   break;
        case PRE_MOVE_WHITE:
            s = "PRE_MOVE_WHITE";   break;
        case PRE_MOVE_BLACK:
            s = "PRE_MOVE_BLACK";   break;
        case IN_MOVE_WHITE:
            s = "IN_MOVE_WHITE";    break;
        case IN_MOVE_BLACK:
            s = "IN_MOVE_BLACK";    break;
        case ERROR_STATE:
            s = "ERROR_STATE"; break;
        case IN_DOLLAR:
            s = "IN_DOLLAR";    break;
    }
    return s;
}


FILE *PgnRead::debug_log_file()
{
    #ifndef _DEBUG
    return stderr;
    #else
    if( debug_log_file_txt == NULL )
        debug_log_file_txt = fopen( "debug_log_file.txt", "wt" );
    FILE *ret = debug_log_file_txt;
    if( ret == NULL )
        ret = stderr;
    return( ret );
    #endif
}

bool PgnRead::TestResult( const char *buf )
{
    bool is_result = false;
    if( 0 == strcmp(buf,result) )
        is_result = true;
    else if( 0 == strcmp(buf,"1-0") )
        is_result = true;
    else if( 0 == strcmp(buf,"0-1") )
        is_result = true;
    else if( 0 == strcmp(buf,"1/2-1/2") )
        is_result = true;
    return is_result;
}


void PgnRead::debug_dump()
{
    #ifdef _DEBUG
    int i;
    for( i=0; i<nbrof(debug_buf); i++ )
    {
        char       c = debug_buf[debug_ptr].c;
        STATE state  = debug_buf[debug_ptr].state;
        debug_ptr++;
        debug_ptr &= (nbrof(debug_buf)-1);
        if( c )
            //fprintf( debug_log_file(), "[%s]%c", ShowState(state), c );
            cprintf( "[%s]%c", ShowState(state), c );
    }
    #endif
}


#define LINE_STATE_MACHINE_IMPLEMENTATION
#ifdef LINE_STATE_MACHINE_IMPLEMENTATION
// Returns true if aborted
bool PgnRead::Process( FILE *infile )
{
    std::string prefix_txt;
    std::string moves;
    int typ;
    char buf[2048];
    extern bool PgnStateMachine( FILE *pgn_file, int &typ, char *buf, int buflen );
    bool done = PgnStateMachine( NULL, typ,  buf, sizeof(buf) );
    GameBegin();
    while( !done )
    {
        done = PgnStateMachine( infile, typ,  buf, sizeof(buf) );
        switch( typ )
        {
            default:
            {
                break;
            }
            case 'T':
            case 't':
            {
                Header( buf );
                break;
            }
            case 'P':
            case 'p':
            {
                prefix_txt += std::string(buf);
                //prefix_txt += '\n';
                break;
            }
            case 'M':
            case 'm':
            {
                moves += std::string(buf);
                moves += '\n';
                break;
            }
            case 'G':
            {
                GameParse(moves);
                moves.clear();
                prefix_txt.clear();
                if( GameOver() )
                {
                    return true;
                }
                GameBegin();
                break;
            }
        }
    }
    FileOver();
    return false;
}


void PgnRead::GameParse( std::string &str )
{
    char buf[FIELD_BUFLEN+10];
    static char comment_buf[10000];
    int ch, comment_ch=0, previous_ch=0, push_back=0, len=0, move_number=0;
    STATE state=MOVE_NUMBER, old_state, save_state=MOVE_NUMBER;
    static int nag_value;
    int input_len = str.length();
    int idx = 0;
    if( idx < input_len )
        ch = str[idx++];
    else
        ch = EOF;
    while( ch != EOF )
    {
        if( push_back == '\0' )
        {
            error_buf[error_ptr++] = (char)ch;
            error_ptr &= (sizeof(error_buf)-1);
        }
        push_back = '\0';

        debug_buf[debug_ptr].c     = (char)ch;
        debug_buf[debug_ptr].state = state;
        debug_ptr++;
        debug_ptr &= (nbrof(debug_buf)-1);

        // State machine
        old_state = state;
        if( ch == '{' && (
                                state!=IN_COMMENT &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                            )
            )
        {
            save_state = state;
            comment_ch = '{';
            state = IN_COMMENT;
        }
        else if( ch == ';' && (
                                state!=IN_COMMENT &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                                )
                )
        {
            save_state = state;
            comment_ch = ';';
            state = IN_COMMENT;
        }
        else if( state==IN_COMMENT &&
                    (
                        (comment_ch=='{' && ch=='}') ||
                        (comment_ch==';' && ch!=';' && previous_ch=='\n')
                    )
                )
        {
            if( comment_ch == ';' )
                push_back = ch;
            comment_buf[len] = '\0';

            // Added the following to fix an ugly bug we were going IN_DOLLAR->IN_COMMENT then back to IN_DOLLAR
            //  and never escaping from IN_DOLLAR because save_state was IN_DOLLAR. Yuk. Maybe we should adopt
            //  the main tarrasch pgn parser practice of only going to IN_COMMENT and IN_DOLLAR from BETWEEN_MOVES
            state = ((save_state==IN_DOLLAR||save_state==MOVE_NUMBER)?BETWEEN_MOVES:save_state);
        }
        else if( ch == '$' && (
                                state!=IN_COMMENT &&
                                state!=IN_DOLLAR &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                            )
            )
        {
            save_state = state;
            state = IN_DOLLAR;
            nag_value = 0;
        }
        else if( state==IN_DOLLAR && isascii(ch) && isdigit(ch) )
        {
            nag_value = nag_value*10+(ch-'0');
        }
        else if( state==IN_DOLLAR && isascii(ch) && !isdigit(ch) )
        {
        /*  const char *nag_array[] =
            {
                "",
                " !",     // $1
                " ?",     // $2
                " !!",    // $3
                " ??",    // $4
                " !?",    // $5
                " ?!",    // $6
                "",       // $7
                "",       // $8
                " ??",    // $9
                " =",     // $10
                " =",     // $11
                " =",     // $12
                "",       // $13
                " +=",    // $14
                " =+",    // $15
                " +/-",   // $16
                " -/+",   // $17
                " +-",    // $18
                " -+",    // $19
                " +-",    // $20
                " -+"     // $21
            };   */
            push_back = ch;

            // Added the following to fix an ugly bug we were going IN_DOLLAR->IN_COMMENT then back to IN_DOLLAR
            //  and never escaping from IN_DOLLAR because save_state was IN_DOLLAR. Yuk. Maybe we should adopt
            //  the main tarrasch pgn parser practice of only going to IN_COMMENT and IN_DOLLAR from BETWEEN_MOVES
            state = (save_state==IN_COMMENT?BETWEEN_MOVES:save_state);
        }
        else if( ch == '(' && (
                                state!=IN_COMMENT    &&
                                state!=ERROR_STATE   &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                                )
                )
            state = Push(state);
        else if( ch == ')' && (
                                state!=IN_COMMENT &&
                                state!=ERROR_STATE      &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                                )
                )
        {
            state = Pop();
        }
        else
        {

            switch( state )
            {
                default: break;
                case IN_COMMENT:
                {
                    if( len < sizeof(comment_buf)-3 )
                    {
                        if( comment_ch == '{' )
                            comment_buf[len++] = (char)ch;
                        else
                        {
                            if( ch!=';' || previous_ch!='\n' )
                                comment_buf[len++] = (char)ch;
                        }
                    }
                    break;
                }

                case MOVE_NUMBER:
                {
                    if( ch=='.' || ch==' ' || ch=='\t' || ch=='\n' )
                    {
                        buf[len] = '\0';
                        len = 0;
                        if( TestResult(buf) )
                            state = NORMAL_EXIT;
                        else if( (move_number=atoi(buf)) > 0 )
                        {
                            state = POST_MOVE_NUMBER;
                            if( ch == '.' )
                                push_back = ch;
                        }
                        else
                        {
                            Error( "Bad move number" );
                            cprintf( "debugging 1: %s\n", str.c_str( ));
                            state = ERROR_STATE;
                        }
                    }
                    else
                    {
                        if( len < FIELD_BUFLEN )
                            buf[len++] = (char)ch;
                        else
                        {
                            Error( "Internal buffer overflow" );
                            state = ERROR_STATE;
                        }
                    }
                    break;
                }

                case POST_MOVE_NUMBER:
                {
                    if( ch == '.' )
                        state = POST_MOVE_NUMBER_HAVE_PERIOD;
                    else if( ch!=' ' && ch!='\t' && ch!='\n' )
                    {
                        Error( "Bad move number" );
                        cprintf( "debugging 1: %s\n", str.c_str( ));
                        state = ERROR_STATE;
                    }
                    break;
                }

                case POST_MOVE_NUMBER_HAVE_PERIOD:
                {
                    if( ch == '.' )
                        state = POST_MOVE_NUMBER_BLACK;
                    else
                    {
                        push_back = ch;
                        state = PRE_MOVE_WHITE;
                    }
                    break;
                }

                case POST_MOVE_NUMBER_BLACK:
                {
                    if( ch != '.' )
                    {
                        push_back = ch;
                        state = PRE_MOVE_BLACK;
                    }
                    break;
                }

                case PRE_MOVE_WHITE:
                case PRE_MOVE_BLACK:
                {
                    if( isascii(ch) && isdigit(ch) )
                    {
                        push_back = ch;
                        state = MOVE_NUMBER;
                    }
                    else if( ch!=' ' && ch!='\t' && ch!='\n' )
                    {
                        push_back = ch;
                        state = (state==PRE_MOVE_WHITE?IN_MOVE_WHITE:IN_MOVE_BLACK);
                    }
                    break;
                }

                case IN_MOVE_WHITE:
                case IN_MOVE_BLACK:
                {
                    if( ch==' ' || ch=='\t' || ch=='\n' || ch=='(' || ch==')' || ch=='{' || ch=='}' || ch=='$' )
                    {
                        buf[len] = '\0';
                        len = 0;
                        if( TestResult(buf) )
                            state = PREFIX;
                        else if( !DoMove(state==IN_MOVE_WHITE,move_number,buf) )
                        {
                            cprintf( "debugging 3: %s\n", str.c_str( ));
                            state = ERROR_STATE;
                        }
                        else
                            state = (state==IN_MOVE_WHITE?PRE_MOVE_BLACK:BETWEEN_MOVES);
                        if( ch=='(' || ch==')' || ch=='{' || ch=='}' || ch=='?' )
                            push_back = ch;
                    }
                    else
                    {
                        if( len < FIELD_BUFLEN )
                            buf[len++] = (char)ch;
                        else
                        {
                            Error( "Internal buffer overflow" );
                            state = ERROR_STATE;
                        }
                    }
                    break;
                }

                case BETWEEN_MOVES:
                {
                    if( isascii(ch) && isdigit(ch) )
                    {
                        push_back = ch;
                        state = MOVE_NUMBER;
                    }
                    break;
                }
            }
        }

        if( state==NORMAL_EXIT || state==ERROR_STATE )
            return;

        // State changes
        if( state != old_state )
        {
            if( state==IN_COMMENT || state==MOVE_NUMBER || state==IN_MOVE_WHITE || state==IN_MOVE_BLACK )
                len=0;
        }

        // Next character
        previous_ch = ch;
        if( push_back )
            ch = push_back;
        else
        {
            do {
                if( idx < input_len )
                    ch = str[idx++];
                else
                    ch = EOF;
            } while ( ch == '\r' );
        }
    }
}

#endif


#ifdef ORIGINAL_IMPLEMENTATION
// Returns true if aborted
bool PgnRead::Process( FILE *infile )
{
    bool aborted = false;
    char buf[FIELD_BUFLEN+10];
    static char comment_buf[10000];
    int ch, comment_ch=0, previous_ch=0, push_back=0, len=0, move_number=0;
    STATE state=INIT, old_state, save_state=INIT;
    static int nag_value;
    //fseek(infile,0,SEEK_END);
    //unsigned long file_len=ftell(infile);
    //rewind(infile);

    // Loop through characters
    ch = fgetc(infile);
    //int old_percent = -1;
    //unsigned char modulo_256=0;
    while( ch != EOF )
    {
    /*  if( modulo_256 == 0 )
        {
            unsigned long file_offset=ftell(infile);
            int percent;
            if( file_len == 0 )
                percent = 100;
            else if( file_len < 10000000 )
                percent = (int)( (file_offset*100L) / file_len );
            else // if( file_len > 10000000 )
                percent = (int)( file_offset / (file_len/100L) );
            if( percent != old_percent )
            {
                cprintf( "%d %% complete\r", percent>100 ? 100 : percent );
            }
            old_percent = percent;
        }
        modulo_256++;
        modulo_256 &= 0xff; */
        if( push_back == '\0' )
        {
            error_buf[error_ptr++] = (char)ch;
            error_ptr &= (sizeof(error_buf)-1);
        }
        push_back = '\0';

        debug_buf[debug_ptr].c     = (char)ch;
        debug_buf[debug_ptr].state = state;
        debug_ptr++;
        debug_ptr &= (nbrof(debug_buf)-1);

        // State machine
        old_state = state;
        if( ch == '{' && (
                                state!=HEADER     &&
                                state!=IN_COMMENT &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                         )
          )
        {
            save_state = state;
            comment_ch = '{';
            state = IN_COMMENT;
        }
        else if( ch == ';' && (
                                state!=HEADER     &&
                                state!=IN_COMMENT &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                              )
               )
        {
            save_state = state;
            comment_ch = ';';
            state = IN_COMMENT;
        }
        else if( state==IN_COMMENT &&
                    (
                        (comment_ch=='{' && ch=='}') ||
                        (comment_ch==';' && ch!=';' && previous_ch=='\n')
                    )
               )
        {
            if( comment_ch == ';' )
                push_back = ch;
            comment_buf[len] = '\0';

            // Added the following to fix an ugly bug we were going IN_DOLLAR->IN_COMMENT then back to IN_DOLLAR
            //  and never escaping from IN_DOLLAR because save_state was IN_DOLLAR. Yuk. Maybe we should adopt
            //  the main tarrasch pgn parser practice of only going to IN_COMMENT and IN_DOLLAR from BETWEEN_MOVES
            state = (save_state==IN_DOLLAR?BETWEEN_MOVES:save_state);
        }
        else if( ch == '$' && (
                                state!=HEADER     &&
                                state!=IN_COMMENT &&
                                state!=IN_DOLLAR &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                         )
          )
        {
            save_state = state;
            state = IN_DOLLAR;
            nag_value = 0;
        }
        else if( state==IN_DOLLAR && isascii(ch) && isdigit(ch) )
        {
            nag_value = nag_value*10+(ch-'0');
        }
        else if( state==IN_DOLLAR && isascii(ch) && !isdigit(ch) )
        {
        /*  const char *nag_array[] =
            {
                "",
                " !",     // $1
                " ?",     // $2
                " !!",    // $3
                " ??",    // $4
                " !?",    // $5
                " ?!",    // $6
                "",       // $7
                "",       // $8
                " ??",    // $9
                " =",     // $10
                " =",     // $11
                " =",     // $12
                "",       // $13
                " +=",    // $14
                " =+",    // $15
                " +/-",   // $16
                " -/+",   // $17
                " +-",    // $18
                " -+",    // $19
                " +-",    // $20
                " -+"     // $21
            };   */
            push_back = ch;

            // Added the following to fix an ugly bug we were going IN_DOLLAR->IN_COMMENT then back to IN_DOLLAR
            //  and never escaping from IN_DOLLAR because save_state was IN_DOLLAR. Yuk. Maybe we should adopt
            //  the main tarrasch pgn parser practice of only going to IN_COMMENT and IN_DOLLAR from BETWEEN_MOVES
            state = (save_state==IN_COMMENT?BETWEEN_MOVES:save_state);
        }
        else if( ch == '(' && (
                                state!=HEADER     &&
                                state!=IN_COMMENT &&
                                state!=ERROR_STATE      &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                              )
               )
            state = Push(state);
        else if( ch == ')' && (
                                state!=HEADER     &&
                                state!=IN_COMMENT &&
                                state!=ERROR_STATE      &&
                                state!=IN_MOVE_BLACK &&
                                state!=IN_MOVE_WHITE
                              )
               )
        {
            state = Pop();
        }
        else
        {

            switch( state )
            {
                default: break;
                case IN_COMMENT:
                {
                    if( len < sizeof(comment_buf)-3 )
                    {
                        if( comment_ch == '{' )
                            comment_buf[len++] = (char)ch;
                        else
                        {
                            if( ch!=';' || previous_ch!='\n' )
                                comment_buf[len++] = (char)ch;
                        }
                    }
                    break;
                }

                case INIT:
                {
                    push_back = ch;
                    state = PREFIX;
                    break;
                }

                case PREFIX:
                {
                    if( ch == '[' )
                    {
                        state = HEADER;
                        push_back = ch;
                    }
                    else if( isascii(ch) && isdigit(ch) )
                    {
                        push_back = ch;
                        state = MOVE_NUMBER;
                    }
                    else if( ch == '*' )
                    {
                        push_back = ch;
                        state = MOVE_NUMBER;
                    }
                    break;
                }

                case HEADER:
                {
                    if( len < FIELD_BUFLEN )
                        buf[len++] = (char)ch;
                    if( ch == ']' )
                        state = PREFIX;
                    break;
                }

                case MOVE_NUMBER:
                {
                    if( ch=='.' || ch==' ' || ch=='\t' || ch=='\n' )
                    {
                        buf[len] = '\0';
                        len = 0;
                        if( TestResult(buf) )
                            state = PREFIX;
                        else if( (move_number=atoi(buf)) > 0 )
                        {
                            state = POST_MOVE_NUMBER;
                            if( ch == '.' )
                                push_back = ch;
                        }
                        else
                        {
                            Error( "Bad move number" );
                            state = ERROR_STATE;
                        }
                    }
                    else
                    {
                        if( len < FIELD_BUFLEN )
                            buf[len++] = (char)ch;
                        else
                        {
                            Error( "Internal buffer overflow" );
                            state = ERROR_STATE;
                        }
                    }
                    break;
                }

                case POST_MOVE_NUMBER:
                {
                    if( ch == '.' )
                        state = POST_MOVE_NUMBER_HAVE_PERIOD;
                    else if( ch!=' ' && ch!='\t' && ch!='\n' )
                    {
                        Error( "Bad move number" );
                        state = ERROR_STATE;
                    }
                    break;
                }

                case POST_MOVE_NUMBER_HAVE_PERIOD:
                {
                    if( ch == '.' )
                        state = POST_MOVE_NUMBER_BLACK;
                    else
                    {
                        push_back = ch;
                        state = PRE_MOVE_WHITE;
                    }
                    break;
                }

                case POST_MOVE_NUMBER_BLACK:
                {
                    if( ch != '.' )
                    {
                        push_back = ch;
                        state = PRE_MOVE_BLACK;
                    }
                    break;
                }

                case ERROR_STATE:
                {
                    if( ch == '[' )
                    {
                        push_back = ch;
                        state = PREFIX;
                    }
                    break;
                }

                case PRE_MOVE_WHITE:
                case PRE_MOVE_BLACK:
                {
                    if( ch == '[' )
                    {
                        push_back = ch;
                        state = PREFIX;
                    }
                    else if( isascii(ch) && isdigit(ch) )
                    {
                        push_back = ch;
                        state = MOVE_NUMBER;
                    }
                    else if( ch!=' ' && ch!='\t' && ch!='\n' )
                    {
                        push_back = ch;
                        state = (state==PRE_MOVE_WHITE?IN_MOVE_WHITE:IN_MOVE_BLACK);
                    }
                    break;
                }

                case IN_MOVE_WHITE:
                case IN_MOVE_BLACK:
                {
                    if( ch == '[' )
                    {
                        push_back = ch;
                        state = PREFIX;
                    }
                    else if( ch==' ' || ch=='\t' || ch=='\n' || ch=='(' || ch==')' || ch=='{' || ch=='}' || ch=='$' )
                    {
                        buf[len] = '\0';
                        len = 0;
                        if( TestResult(buf) )
                            state = PREFIX;
                        else if( !DoMove(state==IN_MOVE_WHITE,move_number,buf) )
                            state = ERROR_STATE;
                        else
                            state = (state==IN_MOVE_WHITE?PRE_MOVE_BLACK:BETWEEN_MOVES);
                        if( ch=='(' || ch==')' || ch=='{' || ch=='}' || ch=='?' )
                            push_back = ch;
                    }
                    else
                    {
                        if( len < FIELD_BUFLEN )
                            buf[len++] = (char)ch;
                        else
                        {
                            Error( "Internal buffer overflow" );
                            state = ERROR_STATE;
                        }
                    }
                    break;
                }

                case BETWEEN_MOVES:
                {
                    if( ch == '[' )
                    {
                        push_back = ch;
                        state = PREFIX;
                    }
                    else if( isascii(ch) && isdigit(ch) )
                    {
                        push_back = ch;
                        state = MOVE_NUMBER;
                    }
                    break;
                }
            }
        }

        // State changes
        if( state != old_state )
        {
            if( old_state == HEADER )
            {
                buf[len++] = '\0';
                Header( buf );
            }
            if( state==HEADER || state==IN_COMMENT || state==MOVE_NUMBER || state==IN_MOVE_WHITE || state==IN_MOVE_BLACK )
                len=0;
            if( state==PREFIX && (
                                    //old_state==IN_COMMENT ||
                                    old_state==BETWEEN_MOVES ||
                                    old_state==MOVE_NUMBER ||
                                    old_state==POST_MOVE_NUMBER ||
                                    old_state==POST_MOVE_NUMBER_HAVE_PERIOD ||
                                    old_state==POST_MOVE_NUMBER_BLACK ||
                                    old_state==PRE_MOVE_WHITE ||
                                    old_state==PRE_MOVE_BLACK ||
                                    old_state==IN_MOVE_WHITE  ||
                                    old_state==IN_MOVE_BLACK  ||
                                    old_state==ERROR_STATE
                                 )
              )
            {
                if( GameOver() )
                {
                    return true;
                }
            }
            if( state==PREFIX && old_state!=HEADER && old_state!=IN_COMMENT )
            {
                GameBegin();
            }
        }

        // Next character
        previous_ch = ch;
        if( push_back )
            ch = push_back;
        else
        {
            do {
                ch = fgetc(infile);
            } while ( ch == '\r' );
            if( ch==EOF &&  (
                                state==MOVE_NUMBER ||
                                state==POST_MOVE_NUMBER ||
                                state==POST_MOVE_NUMBER_HAVE_PERIOD ||
                                state==POST_MOVE_NUMBER_BLACK ||
                                state==PRE_MOVE_WHITE ||
                                state==PRE_MOVE_BLACK ||
                                state==BETWEEN_MOVES
                            )
              )
            {
                if( GameOver() )
                    return true;
            }
        }
    }
    FileOver();
    return aborted;
}
#endif

void PgnRead::Header( char *buf )
{
/* Examples
[Event "Wch U20"]
[Site "Sharjah"]
[Date "1984.08.04"]
[White "Piket, Jeroen"]
[Black "Anand, Viswanathan"]
[Result "0-1"]
[ECO "A48"]
[EventDate "1984.??.??"] */

    char *src, *dst, *s;
    int i;
    const char *array[]=
    {
        "[Date ",
        "[White ",     // warning! must be offset 1
        "[Black ",
        "[Result ",    // warning! must be offset 3
        "[ECO ",
        "[Site ",
        "[Event ",
        "[Desc ",      // warning! must be offset 6
        "[Desc2 ",
        "[Name ",
        "[Variation ",
        "[MoveOrder ",
        "[FEN ",
        "[FENreptor ",
        "[Round ",
        "[WhiteElo ",
        "[BlackElo "
    };

    for( i=0; i<nbrof(array); i++ )
    {
        if( 0 == memcmp(array[i],buf,strlen(array[i])) )
        {
            s = buf + strlen(array[i]);
            while( *s )
            {
                if( *s == '\"' )
                    break;
                else
                    s++;
            }
            if( *s == '\"' )
            {
                s++;
                src = s;
                if( i==1 || i==2 || i==3 || i==5 || i==6 || i>6 ) // white, black, result, site, event, name, variation
                {
                    while( *s && *s != '\"' )
                        s++;
                }
                else
                {
                    while( *s && isascii(*s) && (isalnum(*s) || *s=='.' || *s==' ' || *s=='-' || *s=='\'' || *s=='/') )
                    //while( *s && isascii(*s) && (isalnum(*s) || *s=='.' || *s==' ' || *s=='-' || *s=='/') )
                        s++;
                }
                *s = '\0';
                dst = NULL;
                switch( i )
                {
                    case 0: dst = date;      break;
                    case 1: dst = white;     break;
                    case 2: dst = black;     break;
                    case 3: dst = result;    break;
                    case 4: dst = eco;   break;
                    case 5: dst = site;      break;
                    case 6: dst = event;     break;
                    case 7: dst = desc;      break;
                    case 8: dst = desc2;     break;
                    case 9: dst = name;      break;
                    case 10:dst = variation; break;
                    case 11:dst = move_order_type;      break;
                    case 12:  // FEN
                    case 13:  // FENreptor
                            {
                                if( i==13 || fen[0]=='\0' )
                                {
                                    dst = fen;
                                    fen_flag = true;
                                    if( *src && *white )
                                    {
                                        ;//predefined_labels.Add(white);
                                        ;//predefined_fens.Add(src);
                                    }
                                    chess_rules.Forsyth( src );
                                }
                                break;
                            }
                    case 14:dst = round;        break;
                    case 15:dst = white_elo;    break;
                    case 16:dst = black_elo;    break;
                }
                if( dst )
                    strcpy( dst, src );
            }
        }
    }
}

void PgnRead::GameBegin()
{

    //cprintf( "GameBegin() %d\n", nbr_games );
    date      [0] = '\0';
    white     [0] = '\0';
    black     [0] = '\0';
    result    [0] = '\0';
    eco   [0] = '\0';
    event     [0] = '\0';
    site      [0] = '\0';
    desc      [0] = '\0';
    desc2     [0] = '\0';
    name      [0] = '\0';
    variation [0] = '\0';
    move_order_type    [0] = '\0';
    fen                [0] = '\0';
    round[0]     = '\0';
    white_elo[0] = '\0';
    black_elo[0] = '\0';
    fen_flag = false;
    first_move = true;
    first_move_offset = 0;
    thc::ChessRules temp;
    chess_rules = temp;    // init
    hash = 0x0f6e2dc7837aa12f; //  0x837AA12F; //2205851951; //chess_rules.HashCalculate();
    nbr_games++;
    if(
       ((nbr_games%100000) == 0 ) /* ||
                                  (
                                  (nbr_games < 100) &&
                                  ((nbr_games%10) == 0 )
                                  ) */
       )
        cprintf( "%d games\n", nbr_games );
}

bool PgnRead::GameOver()
{
    bool aborted = false;
    if( pb )
    {
        aborted = pb->ProgressFile();
    }
    if( !aborted )
    {
        STACK_ELEMENT *s;
        s = &stack_array[0];
        const char *pfen = (fen_flag && fen[0]) ? fen : NULL;
        aborted = hook_gameover( callback_code, pfen, event, site, date, round, white, black, result, white_elo, black_elo, eco, s->nbr_moves, s->big_move_array, s->big_hash_array  );
        stack_idx = 0;
        thc::ChessRules temp;
        chess_rules = temp;    // init
        stack_array[0].nbr_moves = 0;
        stack_array[0].position = chess_rules;
    }
    return aborted;
}



PgnRead::STATE PgnRead::Push( PgnRead::STATE in )
{
    STACK_ELEMENT *s, *n;
    s = &stack_array[stack_idx];
    stack_idx++;
    n = &stack_array[stack_idx];
    s->state     = in;
    s->position  = chess_rules;
    n->nbr_moves = s->nbr_moves;
    memcpy( n->big_move_array, s->big_move_array, s->nbr_moves*sizeof(thc::Move) );
    return BETWEEN_MOVES;
}

PgnRead::STATE PgnRead::Pop()
{
    PgnRead::STATE ret=ERROR_STATE;
    STACK_ELEMENT *s = &stack_array[0];
    if( stack_idx == 0 )
        Error( "Too many pops" );
    else
    {
        s = &stack_array[--stack_idx];
        ret = s->state;
    }
    chess_rules = s->position;
    return ret;
}


void PgnRead::FileOver()
{
    cprintf( "Finished %d total games\n", nbr_games-1 );
}


void PgnRead::Error( const char *msg )
{
    cprintf( "PgnRead::Error(%s)\n", msg );
    #ifdef _DEBUG
    int i;
    char c;
    fprintf( debug_log_file(), "Error: %s\n", msg );
    if( *desc )
        fprintf( debug_log_file(), "%s\n", desc );
    else
        fprintf( debug_log_file(), "%s-%s %s %s %s\n", white, black, date, site, event );
    for( i=0; i<sizeof(error_buf); i++ )
    {
        c = error_buf[error_ptr++];
        error_ptr &= (sizeof(error_buf)-1);
        if( c )
            fprintf( debug_log_file(), "%c", c );
    }
    fprintf( debug_log_file(), "\n" );
    for( i=0; i<nbrof(debug_buf); i++ )
    {
        c = debug_buf[debug_ptr].c;
        STATE state  = debug_buf[debug_ptr].state;
        debug_ptr++;
        debug_ptr &= (nbrof(debug_buf)-1);
        if( c )
            fprintf( debug_log_file(), "[%s]%c", ShowState(state), c );
    }
    #endif
}


bool PgnRead::DoMove( bool white_, int move_number, char *buf )
{
    //FILE *debug=debug_log_file();
    //fprintf( debug, "** DoMove( white=%s, move_number=%d, buf=%s)\n", white_?"true":"false", move_number, buf );
    STACK_ELEMENT *n;
    n = &stack_array[stack_idx];
    thc::Move terse;
    char buf2[FIELD_BUFLEN+10];
    thc::Move move;
    int nbr_moves = (move_number-1)*2;
    if( !white_ )
        ++nbr_moves;
    bool okay = false;
    //bool dbg_trigger=false;
    if( first_move )
    {
        first_move = false;
        first_move_offset = nbr_moves;
    }
    nbr_moves -= first_move_offset;

#if 1
    if( nbr_moves > n->nbr_moves )
    {
        Error( white_ ? "White move synchronisation"
                      : "Black move synchronisation" );
    }
    else
    {
        okay = true;
        if( nbr_moves < n->nbr_moves )
        {
            //fprintf( debug, "** nbr_moves=%d < n->nbr_moves=%d\n", nbr_moves, n->nbr_moves );
            //dbg_trigger=true;
            //fprintf( debug_log_file(), "\nnbr_moves=%d, node_idx=%d\n", nbr_moves, node_idx );
            const char *fen2 = fen_flag ? this->fen : NULL;
            if( fen2 && *fen2 )
                chess_rules.Forsyth( fen2 );
            else
            {
                thc::ChessRules temp;
                chess_rules = temp;    // init
            }
            for( int i=0; i<nbr_moves; i++ )
            {
                //fprintf( debug_log_file(), "%s (0x%04x)\n", n->readable[i], n->terse[i] );
                terse = n->big_move_array[i];
                chess_rules.PlayMove( terse );
            }
            n->nbr_moves = nbr_moves;
        }
    }
#else
    okay = true;
    if( nbr_moves != n->nbr_moves )
    {
        if( nbr_moves < n->nbr_moves )
            n->nbr_moves = nbr_moves;
        else
            nbr_moves = n->nbr_moves;
        //fprintf( debug, "** nbr_moves=%d < n->nbr_moves=%d\n", nbr_moves, n->nbr_moves );
        //dbg_trigger=true;
        //fprintf( debug_log_file(), "\nnbr_moves=%d, node_idx=%d\n", nbr_moves, node_idx );
        const char *fen = fen_flag ? this->fen : NULL;
        if( fen && *fen )
            chess_rules.Forsyth( fen );
        else
        {
            thc::ChessRules temp;
            chess_rules = temp;    // init
        }
        for( int i=0; i<nbr_moves; i++ )
        {
            //fprintf( debug_log_file(), "%s (0x%04x)\n", n->readable[i], n->terse[i] );
            terse = n->big_move_array[i];
            chess_rules.PlayMove( terse );
        }
    }
#endif

    if( okay )
    {
        strcpy( buf2, buf );
        char *s=strchr(buf2,'\0')-1;
        while( s >= buf2 )
        {
            if( *s=='!' || *s=='?' )
                *s-- = '\0';
            else
                break;
        }
        okay = move.NaturalInFast( &chess_rules, buf2 );
        if( !okay )
        {
            Error( white_ ? "Cannot convert white move"
                          : "Cannot convert black move" );
        }
        else
        {
            //std::string smove = move.NaturalOut( &chess_rules );
            //thc::ChessPosition old_position = chess_rules;
            hash = chess_rules.Hash64Update(hash, move );
            //db_hash(hash);
            chess_rules.PlayMove( move );
         /* uint32_t check = chess_rules.HashCalculate();
            if( hash != check )
            {
                cprintf( "Incremental hash fail\n");
                cprintf( "Move: %s\n", smove.c_str() );
                cprintf( "Old position = %s\n", old_position.ToDebugStr().c_str() );
                cprintf( "New position = %s\n", chess_rules.ToDebugStr().c_str() );
                cprintf( "hash = 0x%08x\n", hash );
                cprintf( "check = 0x%08x\n", check );
                cprintf( "bugger\n" );
            }
            bool hit = ( hash == 0x990779e0 );
            mega_array[ hash&0xfffff ]++;
            if( hit )
            {
                cprintf( "hit: hash=0x%08x, mega_array[hash]=%d\n", hash, mega_array[hash] );
            } */
            n->big_move_array[n->nbr_moves] = move;
            n->big_hash_array[n->nbr_moves++] = hash;
            if( n->nbr_moves == nbrof(n->big_move_array)-3 )
            {
                Error( "Game too big" );
                okay = false;
            }
            //if( nbr_games == 1 )
            //{
            //   cprintf( "%s\n", chess_rules.ToDebugStr().c_str() );
            //}
        }
    }
    return okay;
}



extern void pgn_read_hook( const char *fen, const char *white, const char *black, const char *event, const char *site, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo, const char *eco, const char *round,
                                    int nbr_moves, thc::Move *moves, uint64_t *hashes  );


bool hook_gameover( char callback_code, const char *fen, const char *event, const char *site, const char *date, const char *round,
                  const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                  int nbr_moves, thc::Move *moves, uint64_t *hashes )
{
    bool aborted = false;
    static int counter;
    if( (++counter % 100000) == 0 )
        cprintf( "%d games processed so far\n", counter );
    if( fen && callback_code!='R' )
        return false;
    switch( callback_code )
    {
        // Great players database
        case 'G': // players_database( event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves, hashes );  break;

        // Binary database append
        case 'B': bin_db_append( fen, event, site, date, round, white, black, result, white_elo, black_elo, eco, nbr_moves, moves );  break;

        // Read one game
        case 'R': pgn_read_hook( fen, white, black, event, site, result, date, white_elo, black_elo, eco, round, nbr_moves, moves, hashes ); return true;

        // Verify
        case 'V':
        {
#if 0
            int is_interesting = verify_compression_algorithm( nbr_moves, moves );
            extern int nbr_games_with_two_queens;
            extern int nbr_games_with_promotions;
            extern int nbr_games_with_slow_mode;
            if( is_interesting & 4096 )
            {
                nbr_games_with_two_queens++;
                is_interesting &= (~4096);
            }
            if( is_interesting&512 )
            {
                nbr_games_with_promotions++;
                is_interesting &= (~512);
            }
            if( is_interesting )
            {
                char buf[200];
                buf[0] = '\0';
                if( is_interesting&0x1f)
                    nbr_games_with_slow_mode++;
                if(is_interesting&1)
                    strcat(buf,"Dark Bishop ");
                if(is_interesting&2)
                    strcat(buf,"Light Bishop ");
                if(is_interesting&4)
                    strcat(buf,"Knight ");
                if(is_interesting&8)
                    strcat(buf,"Rook ");
                if(is_interesting&16)
                    strcat(buf,"Queen ");
                if(is_interesting&32)
                    strcat(buf,"Unusual instance of 3 pawn swaps ");
                if(is_interesting&64)
                    strcat(buf,"New record nbr slow moves - queen ");
                if(is_interesting&128)
                    strcat(buf,"New record nbr slow moves - not queen ");
                if(is_interesting&256)
                    strcat(buf,"Verification error ");
                if(is_interesting&1024)
                    strcat(buf,"Ten or more slow moves - queen ");
                if(is_interesting&2048)
                    strcat(buf,"Ten or more slow moves - not queen ");
                cprintf( "Adding game with issues; %s\n", buf );
                FILE *f = fopen( (is_interesting&16) ? "TwoOrMoreQueens.pgn" : "TooManyKnightsEtc.pgn" , "at" );
                if( f )
                {
                    fprintf( f, "[Event \"%s\"]\n", event );
                    fprintf( f, "[Site \"%s - %s\"]\n", buf, site );
                    fprintf( f, "[Date \"%s\"]\n", date );
                    fprintf( f, "[Round \"%s\"]\n", round );
                    fprintf( f, "[White \"%s\"]\n", white );
                    fprintf( f, "[Black \"%s\"]\n", black );
                    fprintf( f, "[Result \"%s\"]\n", result );
                    fprintf( f, "\n" );
                    thc::ChessRules cr;
                    bool end=true;
                    for( int i=0; i<nbr_moves; i++ )
                    {
                        bool start = ((i%10) == 0);
                        if( !start )
                            fprintf( f, " " );
                        if( cr.white )
                            fprintf( f, "%d. ", cr.full_move_count );
                        thc::Move mv = moves[i];
                        std::string s = mv.NaturalOut(&cr);
                        fprintf( f, "%s", s.c_str() );
                        end = (((i+1)%10) == 0);
                        if( end )
                            fprintf( f, "\n" );
                        cr.PlayMove(mv);
                    }
                    if( !end )
                        fprintf( f, " " );
                    fprintf( f, "%s\n\n", result );
                    fclose(f);
                }
            }
#endif
            break;
        }
    }
    return aborted;
}
