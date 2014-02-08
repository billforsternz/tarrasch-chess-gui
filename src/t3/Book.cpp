/****************************************************************************
 * Facility to lookup book moves using game positions read from a .pgn
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "Book.h"
#include "Repository.h"
#include "Objects.h"
using namespace std;
using namespace thc;
#define nbrof(array) ( sizeof(array) / sizeof((array)[0]) )

//#define REGENERATE
#ifdef  REGENERATE
static FILE *file_regen;
#endif

// Misc
#define VERSION 4               // check we've got the right version
#define BOOK_MOVE_LIMIT 100     // book moves only up to here
#define MAGIC   0x43415041      // "CAPA"

// Constructor
Book::Book()
{
    fen    [0] = '\0';
    desc   [0] = '\0';
    desc2  [0] = '\0';
    name   [0] = '\0';
    variation[0] = '\0';
    date   [0] = '\0';
    white  [0] = '\0';
    black  [0] = '\0';
    result [0] = '\0';
    opening[0] = '\0';
    event  [0] = '\0';
    site   [0] = '\0';
    move_order_type[0] = '\0';
    fen_flag = false;
    nbr_games = 0;
    file_rep = NULL;
    file_inc = NULL;
    debug_log_file_txt = NULL;
    error_ptr = 0;
    debug_ptr = 0;
    stack_idx = 0;
    memset( stack_array, 0, sizeof(stack_array) );
}

const char *Book::ShowState( STATE state )
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


FILE *Book::debug_log_file()
{
    if( debug_log_file_txt == NULL )
        debug_log_file_txt = fopen( "debug_log_file.txt", "wt" );
    FILE *ret = debug_log_file_txt;
    if( ret == NULL )
        ret = stderr;
    return( ret );
}

// Load Book, Return bool error
bool Book::Load( wxString &error_msg, wxString &pgn_file )
{
    bool error=false;
    wxString compile_msg = "Digesting book";
    wxString pgn_compiled_file;
    pgn_compiled_file = pgn_file + "_compiled"; 
    wxFileName pcf(pgn_compiled_file);   
    wxFileName pf(pgn_file);
    bool compile = false;
    if( pf.FileExists() )
    {
        if( pcf.FileExists() )
        {
            if( pcf.GetModificationTime() < pf.GetModificationTime() )
                compile = true;
        }
        else
            compile = true;
    }
    if( compile )
        error = Compile( error_msg, compile_msg, pgn_file, pgn_compiled_file );
    if( !error )
    {
        if( pcf.FileExists() )
        {
            error = LoadCompiled( error_msg, pgn_compiled_file );
            if( error )
            {
                compile_msg = "Redigesting book";
                error = Compile( error_msg, compile_msg, pgn_file, pgn_compiled_file );
            }
        }
        else
        {
            error_msg = "Cannot find book file " + pgn_file;
            error = true;
        }
    }
    return error;
}

// Get predefined positions from book, Return bool error
bool Book::Predefined( wxArrayString &labels, wxArrayString &fens )
{
    bool error = true;
    labels = predefined_labels;
    fens   = predefined_fens;
    unsigned int nbr_labels = labels.GetCount();
    unsigned int nbr_fens   = fens.GetCount();
    if( nbr_labels==nbr_fens && nbr_labels>1 )
        error = false;
    return error;
}

 bool Book::TestResult( const char *buf )
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


void Book::debug_dump()
{
    int i;
    for( i=0; i<nbrof(debug_buf); i++ )
    {
        char       c = debug_buf[debug_ptr].c;
        STATE state  = debug_buf[debug_ptr].state;
        debug_ptr++;
        debug_ptr &= (nbrof(debug_buf)-1);
        if( c )
            fprintf( debug_log_file(), "[%s]%c", ShowState(state), c );
    }    
}

bool Book::Process( FILE *infile, wxProgressDialog &progress )
{
    bool aborted = false;
    char buf[BOOK_BUFLEN+10];
    static char comment_buf[10000];
    int ch, comment_ch=0, previous_ch=0, push_back=0, len=0, move_number=0;
    STATE state=INIT, old_state, save_state=INIT;
    static int nag_value;
    fseek(infile,0,SEEK_END);
    unsigned long file_len=ftell(infile);
    rewind(infile);

    // Loop through characters
    ch = fgetc(infile);
    int old_percent = -1;
    unsigned char modulo_256=0;
    while( ch != EOF )
    {
        if( modulo_256 == 0 )
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
                if( !progress.Update( percent>100 ? 100 : percent ) )
                {
                    aborted = true;
                    break;
                }
            }
            old_percent = percent;
        }
        modulo_256++;
        modulo_256 &= 0xff;
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
            state = save_state;
        }
        else if( ch == '$' && (
                                state!=HEADER     &&
                                state!=IN_COMMENT &&
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
            state = save_state;
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
                    if( len < BOOK_BUFLEN )
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
                        if( len < BOOK_BUFLEN )
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
                        if( len < BOOK_BUFLEN )
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
            //fprintf( debug_log_file(), "State change %s->%s\n", ShowState(old_state), ShowState(state) );
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
                                    old_state==IN_MOVE_BLACK
                                 )
              )
            {
                GameOver();
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
            ch = fgetc(infile);
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
                GameOver();
        }
    }
    FileOver();
    return aborted;
}       

void Book::Header( char *buf )
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
        "[FENreptor "
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
                if( i==1 || i==2 || i==3 || i>6 ) // white, black, result, desc, desc2, name, variation
                {
                    while( *s && *s != '\"' )
                        s++;
                }
                else
                {
                    while( *s && isascii(*s) && (isalnum(*s) || *s=='.' || *s==' ' || *s=='-' || *s=='/') )
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
                    case 4: dst = opening;   break;
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
                                        predefined_labels.Add(white);                  
                                        predefined_fens.Add(src);                  
                                    }
                                    chess_rules.Forsyth( src );
                                }
                            }
                            break;
                }
                if( dst )
                    strcpy( dst, src );
            }
        }
    }
}

void Book::GameBegin()
{
    date      [0] = '\0';
    white     [0] = '\0';
    black     [0] = '\0';
    result    [0] = '\0';
    opening   [0] = '\0';
    event     [0] = '\0';
    site      [0] = '\0';
    desc      [0] = '\0';
    desc2     [0] = '\0';
    name      [0] = '\0';
    variation [0] = '\0';
    move_order_type    [0] = '\0';
    fen                [0] = '\0';
    ChessRules temp;
    chess_rules = temp;    // init
    nbr_games++;
    if( (nbr_games%10) == 0 )
        DebugPrintf(( "%d games\n", nbr_games ));
}

Book::STATE Book::Push( Book::STATE in )
{
    STACK_ELEMENT *s, *n;
    s = &stack_array[stack_idx];
    stack_idx++;
    n = &stack_array[stack_idx];
    s->state     = in;
    s->position  = chess_rules;
    n->nbr_moves = s->nbr_moves;
    memcpy( n->big_move_array, s->big_move_array, s->nbr_moves*sizeof(Move) );
    return BETWEEN_MOVES;
}

Book::STATE Book::Pop()
{
    Book::STATE ret=ERROR_STATE;
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


void Book::GameOver()
{
    stack_idx = 0;
    ChessRules temp;
    chess_rules = temp;    // init
    stack_array[0].nbr_moves = 0;
    stack_array[0].position = chess_rules;
}

void Book::FileOver()
{
}


void Book::Error( const char *msg )
{
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

/* It will be good to get rid of this
void Book::FatalError( const char *msg )
{
    #ifdef _DEBUG
    fprintf( debug_log_file(), "Fatal error: %s\n", msg );
    exit(-1);
    #endif
} */

bool Book::DoMove( bool white, int move_number, char *buf )
{
    #ifdef REGENERATE
    if( move_number==1 && white )
    {
        fprintf( file_regen, "*\n\n[White \"Anon\"]\n" );
        fprintf( file_regen, "[Black \"Anon\"]\n" );
        fprintf( file_regen, "[Result \"*\"]\n" );
    }
    if( move_number < 21 )
    {
        if( white )
            fprintf( file_regen, "%d.", move_number );
        fprintf( file_regen, "%s ", buf );
    }
    #endif
    //FILE *debug=debug_log_file();
    //fprintf( debug, "** DoMove( white=%s, move_number=%d, buf=%s)\n", white?"true":"false", move_number, buf );
    STACK_ELEMENT *n;
    n = &stack_array[stack_idx];
    Move terse;
    char buf2[BOOK_BUFLEN+10];
    Move move;
    int nbr_moves = (move_number-1)*2;
    if( !white )
        ++nbr_moves;
    bool okay = false;
    //bool dbg_trigger=false;
    if( nbr_moves > n->nbr_moves )
    {
        Error( white ? "White move synchronisation"
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
            const char *fen = fen_flag ? this->fen : NULL;
            if( fen && *fen )
                chess_rules.Forsyth( fen );
            else
            {
                ChessRules temp;
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
        okay = move.NaturalIn( &chess_rules, buf2 );
        if( !okay )
        {
            Error( white ? "Cannot convert white terse move"
                         : "Cannot convert black terse move" );
        }
        else
        {
            chess_rules.PlayMove( move );
            if( move_number<=BOOK_MOVE_LIMIT )
            {
                BookPosition bp;
                unsigned short hash = chess_rules.Compress( bp.cpos );
                bp.count = 0;
                bp.play_position_count = 0;
                #if 0
                { // temp - test Decompress() function
                    ChessPosition pos;
                    pos.Decompress( bp.cpos );
                    bool match;
                    match = (chess_rules == pos);
                    if( !match )
                        DebugPrintf(( "**** No match %s-%s\n", chess_rules.squares, pos.squares ));
                }
                #endif
                hash &= BOOK_HASH_MSK;
                bool found = false;
                vector<BookPosition>::iterator it;
                if( bucket[hash].size() )
                {
                    for( it = bucket[hash].begin(); it != bucket[hash].end(); it++ )
                    {
                        if( 0 == memcmp(&bp.cpos,&it->cpos,sizeof(bp.cpos)) )
                        {
                            found = true;
                            it->count++;
                            break;
                        }
                    }
                }
                if( !found )
                    bucket[hash].push_back( bp );
            }
            if( !okay )
            {
                Error( white ? "Cannot play white terse move"
                             : "Cannot play black terse move" );
            }
            else
            {
                n->big_move_array[n->nbr_moves++] = move;
                if( n->nbr_moves == nbrof(n->big_move_array)-3 )
                {
                    Error( "Game too big" );
                    okay = false;
                }
            }
        }
    }
    return okay;
}

// Compile book. Returns bool error
bool Book::Compile( wxString &error_msg, wxString &compile_msg, wxString &pgn_file, wxString &pgn_compiled_file )
{
    wxProgressDialog progress( "One time only book digestion", compile_msg, 100, NULL,
                                     wxPD_APP_MODAL+
                                     wxPD_AUTO_HIDE+
                                     wxPD_ELAPSED_TIME+
                                     wxPD_CAN_ABORT+
                                     wxPD_ESTIMATED_TIME );
    bool error = false;
    const char *pgn_in = pgn_file.c_str();
    const char *pgn_compiled_out = pgn_compiled_file.c_str();
    FILE *outfile = NULL;
    FILE *infile  = fopen( pgn_in, "rt" );
    #ifdef REGENERATE
    file_regen = fopen( "file_regen.pgn", "wt" );
    #endif
    DebugPrintf(( compile_msg ));
    if( infile == NULL )
    {
        error_msg.Printf( "Cannot open %s for reading", pgn_in );
        error = true;
    }
    if( !error )
    {
        outfile  = fopen( pgn_compiled_out, "wb" );
        if( outfile == NULL )
        {
            error_msg.Printf( "Cannot open %s for writing", pgn_compiled_out );
            error = true;
        }
    }
    if( !error )
    {
        unsigned short us;
        unsigned int   ui;
        for( unsigned int i=0; i<BOOK_HASH_NBR; i++ )
            bucket[i].clear();
        predefined_labels.clear();
        predefined_fens.clear();
        predefined_labels.Add(" ");  // a blank line for combo box
        predefined_fens.Add(" ");
        bool aborted = Process( infile, progress );
        if( aborted )
        {
            error_msg.Printf( "Digestion cancelled by user - no book moves available" );
            error = true;
            for( unsigned int i=0; i<BOOK_HASH_NBR; i++ )
                bucket[i].clear();
        }
        else
        {
            ui = MAGIC;
            fwrite( &ui, sizeof(ui), 1, outfile );
            ui = VERSION;
            fwrite( &ui, sizeof(ui), 1, outfile );
            ui = BOOK_HASH_NBR;
            fwrite( &ui, sizeof(ui), 1, outfile );
            unsigned int nbr_labels = predefined_labels.GetCount();
            unsigned int nbr_fens = predefined_fens.GetCount();
            if( nbr_labels != nbr_fens )
            {
                predefined_labels.clear();
                predefined_fens.clear();
                predefined_labels.Add(" ");  // a blank line for combo box
                predefined_fens.Add(" ");
                nbr_labels = nbr_fens = 1;
            }
            fwrite( &nbr_labels, sizeof(nbr_labels), 1, outfile );
            for( unsigned int i=0; i<nbr_labels; i++ )
            {
                wxString label = predefined_labels[i];
                ui = label.Len();
                fwrite( &ui, sizeof(ui), 1, outfile );
                fwrite( label.c_str(), label.Len(), 1, outfile );
                wxString fen = predefined_fens[i];
                ui = fen.Len();
                fwrite( &ui, sizeof(ui), 1, outfile );
                fwrite( fen.c_str(), fen.Len(), 1, outfile );
            }
            ChessRules cr2;
            for( int i=0; i<BOOK_HASH_NBR; i++ )
            {
                if( bucket[i].size() )
                {
                    us = (unsigned short)i;
                    fwrite( &us, sizeof(us), 1, outfile );
                    ui = bucket[i].size();
                    fwrite( &ui, sizeof(ui), 1, outfile );
                    //bool multi = (bucket[i].size()>1);
                    //DebugPrintf(( "bucket[%04x] has %d position%s", i, bucket[i].size(), multi?"s":"" ));
                    vector<BookPosition>::iterator it;
                    for( it = bucket[i].begin(); it != bucket[i].end(); it++ )
                    {
                        BookPosition bp = *it;
                        BookPositionInFile bpif;
                        bpif.cpos  = bp.cpos;
                        unsigned long count = bp.count;
                        // count = 4294967295; testing

                        // Compress a 32 bit count into a 16 bit count, sacrificing
                        //  accuracy for large values
                        // Examples;
                        //  9999  --compress--> 9999  --decompress--> 9999
                        //  10000 --compress--> 10000 --decompress--> 10000
                        //  10001 --compress--> 11000 --decompress--> 10000
                        //  10009 --compress--> 11000 --decompress--> 10000
                        //  10010 --compress--> 11001 --decompress--> 10010
                        //  10019 --compress--> 11001 --decompress--> 10010
                        //  10020 --compress--> 11002 --decompress--> 10020
                        //  4,294,967,295 --compress--> 64294 --decompress--> 4,294,000,000
                        //  Characteristics of scheme;
                        //  - There are unused coding holes 10,000->10,999 20,000->20,999 etc.
                        //  - Scheme preserves 4 ms digits of accuracy
                        if( count > 1000000000 )        // (4,294,967,295->64294) [2^32-1 is largest unsigned int we cope with]
                        {
                            count = (count/1000000);    // 4000,000,000->4000
                            count += 60000;             // 4000,000,000->64000
                        }
                        else if( count > 100000000 )    // (1000,000,000->60,000)
                        {
                            count = (count/100000);     // 900,000,000->9000
                            count += 50000;             // 900,000,000->59000
                        }
                        else if( count > 10000000 )
                        {
                            count = (count/10000);      // 90,000,000->9000
                            count += 40000;             // 90,000,000->49000
                        }
                        else if( count > 1000000 )
                        {
                            count = (count/1000);       // 9,000,000->9000
                            count += 30000;             // 9,000,000->39000
                        }
                        else if( count > 100000 )
                        {
                            count = (count/100);        // 900,000->9000
                            count += 20000;             // 900,000->29000
                        }
                        else if( count > 10000 )
                        {
                            count = (count/10);         // 90,000->9000
                            count += 10000;             // 90,000->19000
                        }
                        // else 9000->9000 (no loss of accuracy from 0-10000)
                        bpif.count  = (unsigned short)count;
                        fwrite( &bpif, sizeof(BookPositionInFile), 1, outfile );
                    }
                }
            }

            // Terminate with 0,0
            us = (unsigned short)0;
            fwrite( &us, sizeof(us), 1, outfile );
            ui = (unsigned int)0;
            fwrite( &ui, sizeof(ui), 1, outfile );
        }
    }
    if( infile )
        fclose( infile );
    if( outfile )
        fclose( outfile );
    #ifdef REGENERATE
    if( file_regen )
        fclose( file_regen );
    #endif
	return error;
}

// Load compiled book. Returns bool error
bool Book::LoadCompiled( wxString &error_msg, wxString &pgn_compiled_file )
{
    bool error = false;
    unsigned int   ui;
    for( unsigned int i=0; i<BOOK_HASH_NBR; i++ )
        bucket[i].clear();
    predefined_labels.clear();
    predefined_fens.clear();
    const char *pgn_compiled_in = pgn_compiled_file.c_str();
    FILE *infile  = fopen( pgn_compiled_in, "rb" );
    if( infile == NULL )
    {
        error_msg.Printf( "Cannot open %s for reading", pgn_compiled_in );
        error = true;
    }
    if( !error )
    {
        fread( &ui, sizeof(ui), 1, infile );
        if( ui != MAGIC )
        {
            error_msg.Printf( "File %s is not a book file", pgn_compiled_in );
            error = true;
        }
    }
    if( !error )
    {
        fread( &ui, sizeof(ui), 1, infile );
        if( ui != VERSION )
        {
            error_msg.Printf( "File %s uses book format version %u, not supported by this program",
                                 pgn_compiled_in, ui );
            error = true;
        }
    }
    if( !error )
    {
        fread( &ui, sizeof(ui), 1, infile );
        if( ui != BOOK_HASH_NBR )
        {
            error_msg.Printf( "File %s uses book format hash number %u, not supported by this program",
                                 pgn_compiled_in, ui );
            error = true;
        }
    }
    if( !error )
    {
        fread( &ui, sizeof(ui), 1, infile );
        unsigned int nbr=ui;
        for( unsigned int i=0; !error && i<nbr; i++ )
        {
            char buf[1024];
            fread( &ui, sizeof(ui), 1, infile );
            if( ui==0 || ui>sizeof(buf)-3 )
            {
                error_msg.Printf( "File %s has an illegal predefined position label",
                                     pgn_compiled_in );
                error = true;
            }
            else
            {
                fread( buf, ui, 1, infile );
                buf[ui] = '\0';
                predefined_labels.Add(buf);
            }
            fread( &ui, sizeof(ui), 1, infile );
            if( ui==0 || ui>sizeof(buf)-3 )
            {
                error_msg.Printf( "File %s has an illegal predefined position fen",
                                     pgn_compiled_in );
                error = true;
            }
            else
            {
                fread( buf, ui, 1, infile );
                buf[ui] = '\0';
                predefined_fens.Add(buf);
            }
        }
    }
    if( !error )
    {
        ChessRules cr2;
        for( ;; )
        {
            // Read a hash idx and a non-zero number of BookPositions for that hash idx
            unsigned short hash;
            fread( &hash, sizeof(hash), 1, infile );
            unsigned int ui;
            fread( &ui, sizeof(ui), 1, infile );
            if( ui == 0 )
                break;  // zero len vector terminates

            // Loop adding BookPositions
            for( unsigned int i=0; i<ui; i++ )
            {
                BookPosition       bp;  
                BookPositionInFile bpif;
                fread( &bpif, sizeof(BookPositionInFile), 1, infile );
                bp.cpos  = bpif.cpos;
                bp.play_position_count = 0;
                unsigned long count = bpif.count;

                // Reverse the 32->16 bit compression
                // Compress a 32 bit count into a 16 bit count, sacrificing
                //  accuracy for large values
                if( count > 64294)
                    count = 64294;  // largest number algorithm copes with
                if( count > 60000 )             // [64294->4294,000,000 (almost 2^32)]
                {
                    count -= 60000;             // 64000->4000
                    count = (count*1000000);    // 4000->4000,000,000
                }
                else if( count > 50000 )        // (60,000->1000,000,000)
                {
                    count -= 50000;             // 59000->9000
                    count = (count*100000);     // 9000->900,000,000
                }
                else if( count > 40000 )        // (50,000->100,000,000)
                {
                    count -= 40000;             // 49000->9000
                    count = (count*10000);      // 9000->90,000,000
                }
                else if( count > 30000 )        // (40,000->10,000,000)
                {
                    count -= 30000;             // 39000->9000
                    count = (count*1000);       // 9000->9,000,000
                }
                else if( count > 20000 )        // (30,000->1,000,000)
                {
                    count -= 20000;             // 29000->9000
                    count = (count*100);        // 9000->900,000
                }
                else if( count > 10000 )        // (20,000->100,000)
                {
                    count -= 10000;             // 19000->9000
                    count = (count*10);         // 9000->90,000
                }
                // else 9000->9000 (no loss of accuracy from 0-10000)
                bp.count  = count;
                bucket[hash].push_back( bp );
            }
        }
    }
    if( infile )
        fclose( infile );
	return error;
}

// Lookup full move list, return true if any moves found
bool Book::Lookup( ChessPosition &pos, vector<BookMove> &bmoves )
{
    bool found = false;
    //if( pos.squares[c4]=='P' && pos.squares[f3]=='N' && pos.squares[d5]=='p' )
    if( objs.repository->book.m_enabled )
    {
        bmoves.clear();
        vector<Move> moves;
        ChessRules cr = pos;
        cr.GenLegalMoveList( moves );
        for( unsigned int i=0; i<moves.size(); i++ )
        {
            Move move = moves[i];
            cr = pos;
            cr.PlayMove( move );
            CompressedPosition cpos;
            unsigned short hash = BOOK_HASH_MSK & cr.Compress( cpos );
            vector<BookPosition>::iterator it;
            if( bucket[hash].size() )
            {
                for( it = bucket[hash].begin(); it != bucket[hash].end(); it++ )
                {
                    if( 0 == memcmp(&cpos,&it->cpos,sizeof(cpos)) )
                    {
                        found = true;
                        BookMove bm;
                        bm.move = move;
                        bm.count = it->count;
                        bm.play_position_count = &it->play_position_count;
                        bmoves.push_back(bm);
                        break;
                    }
                }
            }
        }
        if( found )
            sort( bmoves.rbegin(), bmoves.rend() );
    }
    return found;
}

// Lookup simple move list, return true if any moves found
bool Book::Lookup( ChessPosition &pos, vector<Move> &moves )
{
    moves.clear();
    vector<BookMove> bms;
    bool found = Lookup( pos, bms );
    if( found )
    {
        for( unsigned int i=0; i<bms.size(); i++ )
            moves.push_back( bms[i].move );
    }
    return found;
}

