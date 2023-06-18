/****************************************************************************
 * Full game representation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "Appdefs.h"
#include "PanelContext.h"
#include "Repository.h"
#include "Objects.h"
#include "DebugPrintf.h"
#include "thc.h"
#include "GameLogic.h"
#include "Lang.h"
#include "Bytecode.h"
#include "GameDocument.h"

GameDocument::GameDocument( GameLogic *gl )
    : gv(gl)
{
    this->gl = gl;
    thc::ChessPosition initial_position;
    Init(initial_position);
}

GameDocument::GameDocument()
    : gv(objs.gl)
{
    this->gl = objs.gl;
    thc::ChessPosition initial_position;
    Init(initial_position);
}

void GameDocument::Init( const thc::ChessPosition &start_position_ )
{
    non_zero_start_pos = 0;
    modified = false;
    game_details_edited = false;
    game_prefix_edited  = false;
    game_being_edited   = 0;
    pgn_handle = 0;
    sort_idx   = 0;
    r.white     = "";
    r.black     = "";
    r.event     = "";
    r.site      = "";
    r.date      = "";
    r.round     = "";
    r.result    = "";
    r.eco       = "";
    r.white_elo = "";
    r.black_elo = "";
    this->start_position = start_position_;
    r.fen       = this->start_position.ForsythPublish();
    if( r.fen == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" )
        r.fen = "";
    fposn1 = 0;
    fposn2 = 0;
    fposn3 = 0;
    moves_txt = "";
    prefix_txt = "";
    this->master_position = start_position_;
    tree_bc.Init( this->start_position );
    Rebuild();
}

void GameDocument::FleshOutDate()
{
    if( r.date=="" )
    {
        time_t timer = time(NULL);
        struct tm *ptime = localtime( &timer );
        int yyyy = ptime->tm_year+1900;
        int mm   = ptime->tm_mon+1;
        int dd   = ptime->tm_mday;
        char buf[20];
        sprintf( buf, "%04d.%02d.%02d", yyyy, mm, dd );
        r.date = buf;
    }
}

void GameDocument::FleshOutMoves()
{
    std::string str;
    ToFileTxtGameBody( str );
    size_t idx = str.find_first_of("\r\n");
    if( idx != std::string::npos )
        str = str.substr(0,idx);
    moves_txt = str;
    LangLine(moves_txt,NULL,LangGet());    // File line is always English, convert to current language
}

int NagAlternative( const char *s )
{
    int nag = 0;
    if( *s == '!' )
    {
        if( 0 == strcmp(s,"!") )        nag=1;  // $1
        else if( 0 == strcmp(s,"!!") )  nag=3;  // $3
        else if( 0 == strcmp(s,"!?") )  nag=5;  // $5
    }
    else if( *s == '?' )
    {
        if( 0 == strcmp(s,"?") )        nag=2;  // $2
        else if( 0 == strcmp(s,"??") )  nag=4;  // $4
        else if( 0 == strcmp(s,"?!") )  nag=6;  // $6
    }
    else if( *s == '=' )
    {
        if( 0 == strcmp(s,"=") )        nag=10;  // $10
        else if( 0 == strcmp(s,"=+") )  nag=15;  // $15
    }
    else if( *s == '+' )
    {
        if( 0 == strcmp(s,"+=") )       nag=14;  // $14
        else if( 0 == strcmp(s,"+/-") ) nag=16;  // $16
        else if( 0 == strcmp(s,"+-")  ) nag=18;  // $18
    }
    else if( *s == '-' )
    {
        if( 0 == strcmp(s,"-/+") )      nag=17;  // $17
        else if( 0 == strcmp(s,"-+") )  nag=19;  // $19
    }
    return nag;
}


bool PgnTestResult( const char *s )
{
    bool is_pgn_result = false;
    if( 0 == strcmp(s,"1-0") )
        is_pgn_result = true;
    else if( 0 == strcmp(s,"0-1") )
        is_pgn_result = true;
    else if( 0 == strcmp(s,"1/2-1/2") )
        is_pgn_result = true;
    else if( 0 == strcmp(s,"*") )
        is_pgn_result = true;
    return is_pgn_result;
}

/*
bool PgnTestResult( const char *s, PGN_RESULT &pgn_result )
{
    bool is_pgn_result = false;
    pgn_result = PGN_RESULT_NULL;
    if( 0 == strcmp(s,"1-0") )
    {
        pgn_result = PGN_RESULT_WHITE_WIN;
        is_pgn_result = true;
    }
    else if( 0 == strcmp(s,"0-1") )
    {
        pgn_result = PGN_RESULT_BLACK_WIN;
        is_pgn_result = true;
    }
    else if( 0 == strcmp(s,"1/2-1/2") )
    {
        pgn_result = PGN_RESULT_DRAW;
        is_pgn_result = true;
    }
    else if( 0 == strcmp(s,"*") )
    {
        pgn_result = PGN_RESULT_NULL;
        is_pgn_result = true;
    }
    return is_pgn_result;
}

*/

std::string RemoveLineEnds( std::string &s )
{
    std::string t;
    int len=s.length();
    bool in_white=false;
    for( int i=0; i<len; i++ )
    {
        char c = s[i];
        if( c=='\n' || c=='\r' )
        {
            if( !in_white )
                t += ' ';
            in_white = true;
        }
        else
        {
            t += c;
            in_white = (c==' ');
        }
    }
    return t;
}

std::string GameDocument::Description()
{
    std::string white = this->r.white;
    std::string black = this->r.black;
    size_t comma = white.find(',');
    if( comma != std::string::npos )
        white = white.substr( 0, comma );
    comma = black.find(',');
    if( comma != std::string::npos )
        black = black.substr( 0, comma );
    int move_cnt = tree_bc.GetMainVariation().size();
    std::string label = white;
    if( r.white_elo != "" )
    {
        label += " (";
        label += r.white_elo;
        label += ")";
    }
    label += " - ";
    label += black;
    if( r.black_elo != "" )
    {
        label += " (";
        label += r.black_elo;
        label += ")";
    }
    if( r.site != "" )
    {
        label += ", ";
        label += r.site;
    }
    else if( r.event != "" )
    {
        label += ", ";
        label += r.event;
    }
    if( r.date.length() >= 4 )
    {
        label += " ";
        label += r.date.substr(0,4);
    }
    bool result_or_moves = false;
    if( r.result != "*" )
    {
        result_or_moves = true;
        label += ", ";
        label += r.result;
        if( move_cnt > 0 )
            label += " in";
    }
    if( move_cnt > 0 )
    {
        if( !result_or_moves )
            label += ", ";
        char buf[100];
        sprintf( buf, " %d moves", (move_cnt+1)/2 );
        label += std::string(buf);
    }
    return label;
}

bool GameDocument::PgnParse( bool use_semi, int &nbr_converted, const std::string str, thc::ChessRules &cr, VARIATION *pvar, bool use_current_language, int imove )
{
    std::string bytecode;

    // Main state machine has these states
    enum PSTATE
    {
        BETWEEN_MOVES,
        IN_COMMENT,
        IN_DOLLAR,
        IN_NUMBER,
        IN_MISC,
        IN_STAR,
        IN_MOVE
    };

    PSTATE       state;         // the current state machine state
    int offset = 0;

    // Allow stacking of the key state variables
    const int MAX_DEPTH=20;
    struct STACK_ELEMENT
    {
        PSTATE     state;
        thc::ChessRules cr;
        int variation_move_count;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    int stk_idx = 0;
    STACK_ELEMENT *stk = &stk_array[stk_idx];

    // Misc
    #define Error(x)
    std::string move_str;
    std::string comment_str;
    std::string buffered_comment;
    std::string prefix;
    char comment_ch='\0', previous_ch='\0';
    int nag_value=0;
    PSTATE old_state, save_state=BETWEEN_MOVES;
    char ch, push_back='\0';
    unsigned int str_idx=0;
    nbr_converted = 0;

    state = BETWEEN_MOVES;      // state machine
    old_state = BETWEEN_MOVES;

    //
    //  Later - allow PGN parsing to start at arbitrary offset - legacy code did some weird stack scanning
    //

    // Start loop
    bool okay=true;
    bool eof=false;
    ch = ' ';   // dummy to start loop
    while( okay && !eof )
    {
        push_back = '\0';
        old_state = state;
        if( ch == '\x96' )  // ANSI code for en dash, sometimes used instead of '-'
            ch = '-';
        bool in_number = isascii(ch) && isdigit(ch);
        bool in_move   = isascii(ch) && isalnum(ch);
        bool in_misc   = (ch=='.'||ch=='?'||ch=='!'||ch=='+'||ch=='-'||ch=='='||ch=='/');
        bool in_star   = (ch=='*');
        switch( state )
        {

            // In a comment
            case IN_COMMENT:
            {

                // End of the comment ?
                if(  (comment_ch=='{' && ch=='}') ||
                     (comment_ch==';' && ch!=';' && ch!='\n' && ch!='\r' && (previous_ch=='\n'||previous_ch=='\r') )
                  )
                {
                    if( comment_ch == ';' )
                        push_back = ch;
                    else
                    {
                        // New policy from V2.03c
                        //  Only create '{' comments (most chess software doesn't understand ';' comments)
                        //  If  "}" appears in comment transform to "|>"    (change *is* restored when reading .pgn)
                        //  If  "|>" appears in comment transform to "| >"  (change is *not* restored when reading .pgn)
                        std::string ReplaceAll( const std::string &in, const std::string &from, const std::string &to );
                        comment_str = ReplaceAll( comment_str, "|>", "}" );
                    }
                    size_t len = bytecode.length();
                    if( len > 0 && bytecode[len-1] == BC_COMMENT_END )
                    {
                        bytecode.pop_back();    // remove COMMENT_END and extend existing comment
                        bytecode.push_back( ' ' );
                    }
                    else
                        bytecode.push_back( BC_COMMENT_START );
                    bytecode += comment_str;
                    bytecode.push_back( BC_COMMENT_END );
                    state = save_state;
                }

                // Else continue comment
                else
                {
                    bool skip = (comment_ch==';' && ch==';' && (previous_ch=='\n'||previous_ch=='\r') );
                    if( !skip )
                    {
                        if( ch == '\n' )
                            comment_str += ' ';
                        else if( ch != '\r' )
                            comment_str += (char)ch;
                    }
                }
                break;
            }

            // Creating NAG value
            case IN_DOLLAR:
            {
                if( isascii(ch) && isdigit(ch) )
                    nag_value = nag_value*10+(ch-'0');
                else
                {
                    /*
                    const char *nag_array[] =
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
                    };  */
                    if( 1<=nag_value && nag_value<=21 )
                    {
                        bytecode.push_back( BC_ESCAPE );
                        bytecode.push_back( 8 + nag_value );
                    }
                    push_back = ch;
                    state = save_state;
                }
                break;
            }

            // Waiting for something to happen
            case BETWEEN_MOVES:
            {

                // NAG token ?
                if( ch == '$' )
                {
                    save_state = state;
                    state = IN_DOLLAR;
                    nag_value = 0;
                }

                // Start {comment} style comment ?
                else if( ch == '{' )
                {
                    save_state = state;
                    comment_ch = '{';
                    state = IN_COMMENT;
                    comment_str = "";
                }

                // Start ;comment style comment ?
                else if( use_semi && ch == ';' )
                {
                    save_state = state;
                    comment_ch = ';';
                    state = IN_COMMENT;
                    comment_str = "";
                }

                // Start new variation ?
                else if( ch == '(' )
                {

                    // Push current state onto a stack
                    stk->cr    = cr;
                    stk->state = state;
                    if( stk_idx+1 >= MAX_DEPTH )
                    {
                        Error("Too deep");
                        okay = false;
                    }
                    else
                    {
                        stk_idx++;
                        stk = &stk_array[stk_idx];
                        stk->variation_move_count = 0;
                        thc::ChessPosition cp;
                        std::vector<thc::Move> var;
                        bc_locate( bytecode, offset, cp, var );
                        if( var.size() == 0 )
                        {
                            Error("Cannot branch from empty variation");
                            okay = false;
                        }
                        else
                        {
                            bytecode.push_back( BC_VARIATION_START );
                        }
                    }
                }

                // End variation ?
                else if( ch == ')' )
                {

                    // Pop old state off stack
                    if( stk_idx == 0 )
                    {
                        Error("Mismatched )");
                        okay = false;
                    }
                    else
                    {
                        stk_idx--;
                        stk   = &stk_array[stk_idx];
                        cr    = stk->cr;
                        state = stk->state;
                        bytecode.push_back( BC_VARIATION_END );

                        //
                        // Todo if variation we added was empty, remove it
                        //
                    }
                }

                // Else go into token if possible
                else
                {
                    if( in_number || in_misc || in_star || in_move )
                    {
                        move_str = "";
                        push_back = ch;
                        if( in_number )
                            state = IN_NUMBER;
                        else if( in_move )
                            state = IN_MOVE;
                        else if( in_misc )
                            state = IN_MISC;
                        else if( in_star )
                            state = IN_STAR;
                    }
                }
                break;
            }

            // Collect a token
            case IN_NUMBER:
            case IN_MISC:
            case IN_STAR:
            case IN_MOVE:
            {
                bool keep_buffering=false;
                if( state == IN_NUMBER )
                    keep_buffering = in_number || ch=='-' || ch=='/';   // eg 0-1 or 1/2-1/2
                else if( state == IN_MISC )
                    keep_buffering = in_misc;
                else if( state == IN_STAR )
                    keep_buffering = in_star;
                else if( state == IN_MOVE )
                {
                    if( ch=='=' || ch=='+' || ch=='#' || ch=='-' )  // can also be in moves
                        keep_buffering = true;
                    else
                        keep_buffering = in_move;
                }
                if( keep_buffering )
                    move_str += (char)ch;
                else
                {
                    push_back = ch;
                    bool was_in_move = (state==IN_MOVE);
                    bool do_nothing_move = false;

                    // Support some popular variants
                    if( state == IN_NUMBER )
                    {
                        if( move_str == "0-0" )
                        {
                            move_str = "O-O";
                            was_in_move = true;
                        }
                        else if( move_str=="0-0-0" )
                        {
                            move_str = "O-O-O";
                            was_in_move = true;
                        }
                    }
                    else if( state == IN_MISC )
                    {
                        if( move_str == "--" )     // do nothing or pass move
                        {
                            do_nothing_move = true;
                            was_in_move = true;
                        }

                        // Support '?', '!!' etc as text rather than only as NAG codes
                        int nag_value2 = NagAlternative(move_str.c_str());
                        if( 1<=nag_value2 && nag_value2<=21 && stk->variation_move_count>0 )
                        {
                            bytecode.push_back( BC_ESCAPE );
                            bytecode.push_back( 8 + nag_value2 );
                            nbr_converted++;    // so just a lone += for example counts
                        }
                    }
                    state = BETWEEN_MOVES;
                    if( PgnTestResult(move_str.c_str()) )
                    {
                        eof = true;     // just stop (a bit simplistic)
                    }
                    else if( was_in_move )
                    {
                        bool adding_from_middle_to_end_bug = false;
                        int sz = (*pvar).size();
                        if( imove!=-1 && imove!=sz-1 )
                            adding_from_middle_to_end_bug = true;
                        if( adding_from_middle_to_end_bug )
                            okay = false;
                        else
                        {
                            std::string temp = move_str;
                            if( use_current_language )
                                LangToEnglish(temp);
                            thc::Move mv;
                            if( !do_nothing_move )
                            {
                                okay = mv.NaturalIn(&cr,temp.c_str());
                            }
                            else
                            {   // Nasty little hack - support "--" = do nothing, create a move from one empty square
                                //  to same empty square, capturing empty - chess engine will "play" that okay
                                okay = false;
                                for( int i=63; i>=0; i-- )  // start search at h1 to avoiding a8a8 which is Invalid move
                                {   // found an empty square yet ?
                                    if( cr.squares[i]==' ' || cr.squares[i]=='.' )  // plan to change empty from ' ' to '.', so be prepared
                                    {   // C++ can't cast into the bitfield, so do this, aaaaaaaargh
                                        static thc::Square lookup[] = { thc::a8,thc::b8,thc::c8,thc::d8,thc::e8,thc::f8,thc::g8,thc::h8,
                                                                        thc::a7,thc::b7,thc::c7,thc::d7,thc::e7,thc::f7,thc::g7,thc::h7,
                                                                        thc::a6,thc::b6,thc::c6,thc::d6,thc::e6,thc::f6,thc::g6,thc::h6,
                                                                        thc::a5,thc::b5,thc::c5,thc::d5,thc::e5,thc::f5,thc::g5,thc::h5,
                                                                        thc::a4,thc::b4,thc::c4,thc::d4,thc::e4,thc::f4,thc::g4,thc::h4,
                                                                        thc::a3,thc::b3,thc::c3,thc::d3,thc::e3,thc::f3,thc::g3,thc::h3,
                                                                        thc::a2,thc::b2,thc::c2,thc::d2,thc::e2,thc::f2,thc::g2,thc::h2,
                                                                        thc::a1,thc::b1,thc::c1,thc::d1,thc::e1,thc::f1,thc::g1,thc::h1
                                                                      };
                                        mv.src =
                                        mv.dst = lookup[i];
                                        mv.capture = cr.squares[i];  // empty
                                        mv.special = thc::NOT_SPECIAL;
                                        okay = true;
                                        break;
                                    }
                                }
                            }
                            if( !okay )
                            {
                                Error("Illegal move");
                                //prefix += move_str;
                                //prefix += push_back;
                            }
                            else
                            {
                                prefix = "";
                                nbr_converted++;
                                cr.PushMove(mv);
                                bc_insert(bytecode,offset,cr,mv);
                                stk->variation_move_count++;
                            }
                        }
                    }
                }
                break;
            }
        }

        // State changes
        if( state != old_state )
        {
            //fprintf( debug_log_file(), "State change %s->%s\n", ShowState(old_state), ShowState(state) );
        }

        // Error handling, append the rest of the string as a comment
        if( !okay )
        {
            std::string comment;
            if( str_idx < str.length() )
            {
                std::string temp( str.begin()+str_idx, str.end() );
                int len = temp.length();
                if( len>0 && temp[len-1]==')' )
                    temp = temp.substr(0,len-1);
                comment += temp;
            }
            comment = RemoveLineEnds(comment);
            if( comment != "" )
            {
                size_t len = bytecode.length();
                if( len > 0 && bytecode[len-1] == BC_COMMENT_END )
                {
                    bytecode.pop_back();    // remove COMMENT_END and extend existing comment
                    bytecode.push_back( ' ' );
                }
                else
                    bytecode.push_back( BC_COMMENT_START );
                bytecode += comment;
                bytecode.push_back( BC_COMMENT_END );
            }
        }

        // Next character
        else if( !eof )
        {
            if( push_back )
                ch = push_back;
            else
            {
                previous_ch = ch;
                if( str_idx < str.length() )
                {
                    ch = str[str_idx];
                    prefix += ch;
                }
                else
                {
                    ch  = ' ';  // add trailing padding to terminate last token
                    if( str_idx > str.length()+2 )
                        eof = true;   // after a trailing ' ' or so, stop
                }
                str_idx++;
            }
        }
    }
    Rebuild();
    return okay;
}


// Load a GameDocument from a list of moves
void GameDocument::LoadFromMoveList( std::vector<thc::Move> &moves, int move_idx )
{
    tree_bc.Init( moves );
    Rebuild();
    SetNonZeroStartPosition( move_idx );
}

// Set a non zero start position
void GameDocument::SetNonZeroStartPosition( int main_line_idx )
{
    tree_bc.SetNonZeroStartPosition( main_line_idx );
}


// Return ptr to move played if any
bool GameDocument::MakeMove( GAME_MOVE game_move, bool allow_overwrite )
{
    bool ok = tree_bc.InsertMove( game_move, allow_overwrite );
    if( ok )
    {
        Rebuild();
        gl->atom.Undo();
        unsigned long pos2 = gv.GetMoveOffset( tree_bc.offset );
        gl->atom.Display( pos2 );
    }
    return ok;
}

// Are we at the end of the main line ?
bool GameDocument::AtEndOfMainLine()
{
    return tree_bc.AtEndOfMainLine();
}
bool GameDocument::AreWeInMain()
{
    return tree_bc.AreWeInMain();
}

// Where are we in the document
unsigned long GameDocument::GetInsertionPoint()
{
    return gl->atom.GetInsertionPoint();
}

// Where are we in the document
void GameDocument::SetInsertionPoint(unsigned long pos)
{
    // This sets the physical insertion point in a control - is that really what we want ?
    //  29/9/2018 we checked it out, it is subtle but it is a little more than that - it
    //  also a peer to gets insertion point above and this might be used to locate the users
    //  cursor position and set the the game board position and title. In fact we only found
    //  this function called from one point - to set the position in a different tab after
    //  a tab delete - and subsequently ShowNewDocument picked up and located the position
    //  properly.
    gl->atom.SetInsertionPoint(pos);
}

void GameDocument::RedisplayRequest()
{
    unsigned long pos = gv.GetMoveOffset( tree_bc.offset );
    gl->atom.Redisplay( pos );
}

void GameDocument::Redisplay( unsigned long pos )
{
    std::string title;
    bool at_move0;
    bool found = Locate(pos,master_position,title,at_move0);
    if( !found )
    {
        pos = 0;
        Locate(pos,master_position,title,at_move0);
    }
    if( objs.canvas )
    {
        Display( pos );
        objs.canvas->SetChessPosition( master_position );
        objs.canvas->SetBoardTitle(title.c_str());
    }
}


// KibitzCapture() needs to be completely rewritten, largely shifted to GameTree

int GameDocument::KibitzCaptureStart( const char *engine_name, const char *txt, std::vector<thc::Move> &var,
        bool &use_repeat_one_move,
        GAME_MOVE &repeat_one_move     // eg variation = e4,c5 new_variation = Nf3,Nc6 etc.
                                       //  must make new_variation = c5,Nf3,Nc6 etc.
    )
{
#if 1
    return 0;
#else
    use_repeat_one_move = false;
    GameTree *insertion_point = NULL;
    unsigned long pos = GetInsertionPoint();
    unsigned long pos_restore = pos;
    thc::ChessRules cr = start_position;
    std::string title;
    bool at_move0=true;

    GameTree *found = Locate( pos, cr, title, at_move0 );
    int ivar=0;
    int imove=0;
    GameTree *parent;
    if( found && found!=&tree_bc )
        parent = tree_bc.Parent( found, cr, ivar, imove );
    else
    {
        parent = &tree_bc;
        at_move0 = true;
    }
    VARIATION &variation2 = parent->variations[ivar];
    for( int i=0; i<imove; i++ )
        cr.PlayMove( variation2[i].game_move.move );
    if( !at_move0 )
        cr.PlayMove( variation2[imove].game_move.move );
    if( cr == master_position )
    {
        VARIATION &variation = parent->variations[ivar];
        int sz = variation.size();
        if( at_move0 && parent!= &tree_bc )
            ;
        else if( !at_move0 && sz==imove+1 )
        {
            use_repeat_one_move=true;
            repeat_one_move = variation[sz-1].game_move;
        }

        // Create new variation
        VARIATION new_variation;
        for( size_t i=0; i<var.size(); i++ )
        {
            GameTree node;
            node.game_move.move = var[i];
            if( i == 0 )
            {
                std::string tmp = txt;
                size_t idx = tmp.find_first_of(')');
                if( idx != std::string::npos )
                    tmp = tmp.substr(0,idx+1);
                std::string s(engine_name);
                s += " ";
                s += tmp;
                node.game_move.pre_comment = s;
                if( use_repeat_one_move )
                {
                    GameTree mt;
                    mt.game_move = repeat_one_move;
                    new_variation.push_back(mt);
                }
            }
            new_variation.push_back(node);
        }

        // New variation for parent
        sz = variation.size();
        if( at_move0 )
        {
            if( parent == &tree_bc )
            {
                if( parent->variations[0].size() == 0 )
                {
                    parent->variations[0] = new_variation;
                    insertion_point = &parent->variations[0][0];
                }
                else
                {
                    insertion_point = &parent->variations[0][0];
                    insertion_point->variations.push_back( new_variation );
                }
            }
            else
            {
                insertion_point = parent;
                insertion_point->variations.push_back( new_variation );
            }
        }

        // New variation
        else if( sz > imove+1 )
        {
            GameTree &existing_node = variation[imove+1];
            insertion_point = &existing_node;
            insertion_point->variations.push_back( new_variation );
        }

        // Need our repeat move
        else if( use_repeat_one_move )
        {
            GameTree &existing_node = variation[variation.size()-1];
            insertion_point = &existing_node;
            insertion_point->variations.push_back( new_variation );
        }

        // Rebuild
        Rebuild();
        gl->atom.Undo();
        pos = pos_restore;
        gl->atom.Redisplay( pos );
    }
    return insertion_point;
#endif
}

/* void GameDocument::KibitzCapture( GameTree *node, const char *txt, std::vector<thc::Move> &var,
            bool use_repeat_one_move,
            GAME_MOVE &repeat_one_move    // eg variation = e4,c5 new_variation = Nf3,Nc6 etc.
            )                             //  must make new_variation = c5,Nf3,Nc6 etc. */
void GameDocument::KibitzCapture( int offset, const char *txt, std::vector<thc::Move> &var,
                    bool use_repeat_one_move,
                    GAME_MOVE &repeat_one_move       // eg variation = e4,c5 new_variation = Nf3,Nc6 etc.
                                                    //  must make new_variation = c5,Nf3,Nc6 etc.
)
{
#if 0
/*
    unsigned long pos = GetInsertionPoint();
    unsigned long pos_restore = pos;
    thc::ChessRules cr = start_position;
    std::string title;
    bool at_move0=true;
    GameTree *found = Locate( pos, cr, title, at_move0 );
    int ivar=0;
    int imove=0;
    GameTree *parent;
    if( found && found!=&tree_bc )
        parent = tree_bc.Parent( found, cr, ivar, imove );
    else
    {
        parent = &tree_bc;
        at_move0 = true;
    }
    if( parent )
    {
        // New variation for parent
        VARIATION &variation = parent->variations[ivar];
        if( at_move0 && parent!= &tree_bc )
            parent->variations.push_back( new_variation );

        // New variation
        else if( variation.size() > imove+1 )
        {
            GameTree &existing_node = variation[imove+1];
            existing_node.variations.push_back( new_variation );
        }
    }
*/

    if( node )
    {

        VARIATION new_variation;
        if( use_repeat_one_move )
        {
            GameTree mt;
            mt.game_move =  repeat_one_move;
            new_variation.push_back(mt);
        }
        for( size_t i=0; i<var.size(); i++ )
        {
            GameTree node2;
            node2.game_move.move = var[i];
            if( i == 0 )
            {
                std::string tmp = txt;
                size_t idx = tmp.find_first_of(')');
                if( idx != std::string::npos )
                    tmp = tmp.substr(0,idx+1);
                node2.game_move.pre_comment = tmp;
            }
            new_variation.push_back(node2);
        }

        unsigned long pos = GetInsertionPoint();
        node->variations.push_back( new_variation );

        // Rebuild
        Rebuild();
        gl->atom.Undo();
        gl->atom.Redisplay( pos );
    }
#endif
}


void GameDocument::Promote()
{
    if( tree_bc.Promote() )
    {
        Rebuild();
        gl->atom.Undo();
        RedisplayRequest();
    }
}

void GameDocument::Demote()
{
    if( tree_bc.Demote() )
    {
        Rebuild();
        gl->atom.Undo();
        RedisplayRequest();
    }
}

void GameDocument::PromoteRestToVariation()
{
    unsigned long pos = GetInsertionPoint();
    unsigned long offset_within_comment;
    bool in_comment = gv.GetOffsetWithinComment( pos, offset_within_comment );
    if( in_comment )
        PromoteToVariation(offset_within_comment);
}



bool GameDocument::PromotePaste( std::string &str )
{
    unsigned long pos = GetInsertionPoint();
    std::string title;
    GameTree save=tree_bc;
    bool changes = false;
    if( pos==0 && tree_bc.IsEmpty() )
    {
        GameTree candidate_a;
        GameTree candidate_b;
        int nbr_converted_a = 0;
        int nbr_converted_b = 0;

        // Scenario a, use current language
        thc::ChessRules cr;
        VARIATION variation;
        PgnParse( false, nbr_converted_a, str, cr, &variation, true );
        if( nbr_converted_a > 0 )
            candidate_a = tree_bc;

        // Scenario b, use English
        tree_bc = save;
        Rebuild();
        {
            thc::ChessRules cr2;
            VARIATION variation2;
            Rebuild();
            PgnParse( false, nbr_converted_b, str, cr2, &variation2, false );
        }
        if( nbr_converted_b > 0 )
            candidate_b = tree_bc;

        // Select one or the scenarios (or none if nothing worked)
        if( nbr_converted_a==0  && nbr_converted_b==0 )
        {
            tree_bc = save;
        }
        else if( nbr_converted_a >= nbr_converted_b &&
                 nbr_converted_a >= 2 )
        {
            tree_bc = candidate_a;
            changes = true;
        }
        else if( nbr_converted_b >= nbr_converted_a &&
                 nbr_converted_b >= 2 )
        {
            tree_bc = candidate_b;
            changes = true;
        }
    }

    // Rebuild
    Rebuild();
    if( changes )
        gl->atom.Undo();
    gl->atom.Redisplay( pos );
    return changes;
}

void GameDocument::PromoteToVariation( unsigned long offset_within_comment )
{
#if 0
    unsigned long pos = GetInsertionPoint();
    unsigned long pos_restore = pos;
    thc::ChessRules cr;
    std::string title;
    bool at_move0;
    GameTree save=tree_bc;
    bool no_changes=false;

    // First cope with an empty document with a lone comment
    if( !HaveMoves() && tree_bc.game_move.comment.length()>offset_within_comment )
    {
        GameTree candidate_a;
        GameTree candidate_b;
        unsigned long pos_restore_a = pos_restore;
        unsigned long pos_restore_b = pos_restore;
        int nbr_converted_a = 0;
        int nbr_converted_b = 0;

        // Scenario a, use current language
        thc::ChessRules cr2 = *tree_bc.root;
        std::string str = tree_bc.game_move.comment.substr(offset_within_comment);
        tree_bc.game_move.comment = tree_bc.game_move.comment.substr(0,offset_within_comment);
        VARIATION &variation2 = tree_bc.variations[0];
        Rebuild();
        PgnParse( false, nbr_converted_a, str, cr2, &variation2, true );
        if( nbr_converted_a > 0 )
        {
            candidate_a = tree_bc;
            pos_restore_a = gv.GetMoveOffset( gbl_plast_move ); // where to end up pointing
        }

        // Scenario b, use English
        tree_bc = save;
        Rebuild();
        {
            thc::ChessRules cr3 = *tree_bc.root;
            std::string str3 = tree_bc.game_move.comment.substr(offset_within_comment);
            tree_bc.game_move.comment = tree_bc.game_move.comment.substr(0,offset_within_comment);
            VARIATION &variation3 = tree_bc.variations[0];
            Rebuild();
            PgnParse( false, nbr_converted_b, str3, cr3, &variation3, false );
        }
        if( nbr_converted_b > 0 )
        {
            candidate_b = tree_bc;
            pos_restore_b = gv.GetMoveOffset( gbl_plast_move ); // where to end up pointing
        }

        // Select one or the scenarios (or none if nothing worked)
        if( nbr_converted_a==0  && nbr_converted_b==0 )
        {
            tree_bc = save;
            no_changes = true;
        }
        else if( nbr_converted_a >= nbr_converted_b )
        {
            tree_bc = candidate_a;
            pos_restore = pos_restore_a;
        }
        else if( nbr_converted_b >= nbr_converted_a )
        {
            tree_bc = candidate_b;
            pos_restore = pos_restore_b;
        }
    }
    else
    {
        GameTree candidate_scenario_1a;
        GameTree candidate_scenario_1b;
        GameTree candidate_scenario_2a;
        GameTree candidate_scenario_2b;
        unsigned long pos_restore_scenario1 = pos_restore;
        unsigned long pos_restore_scenario2 = pos_restore;
        int nbr_converted_scenario_1a=0;
        int nbr_converted_scenario_1b=0;
        int nbr_converted_scenario_2a=0;
        int nbr_converted_scenario_2b=0;

        // Try scenario 1;
        //   Comment extends from current move, eg 1.e4 e5 2.Nf3 {2...Nc6 3.Bb5}
        //   OR is a well formed variation already, eg 1.e4 e5 2.Nf3 {(2.Nc3 Nf6 3.f4)}
        //   (Scenario 1a, use current language)
        GameTree *found = Locate( pos, cr, title, at_move0 );
        if( found && found->game_move.comment.length()>offset_within_comment  )
        {
            pos_restore_scenario1 = gv.GetMoveOffset( found ); // where to end up pointing
            thc::ChessRules cr2;
            int ivar;
            int imove;
            GameTree *parent = tree_bc.Parent( found, cr2, ivar, imove );
            if( parent )
            {
                VARIATION &variation = parent->variations[ivar];
                std::string str = found->game_move.comment.substr(offset_within_comment);
                found->game_move.comment = found->game_move.comment.substr(0,offset_within_comment);
                Rebuild();
                PgnParse( false, nbr_converted_scenario_1a, str, cr, &variation, true, imove );
                if( nbr_converted_scenario_1a > 0 )
                    candidate_scenario_1a = tree_bc;
            }
        }

        // Scenario 1b, use English
        tree_bc = save;
        Rebuild();
        found = Locate( pos, cr, title, at_move0 );
        if( found && found->game_move.comment.length()>offset_within_comment  )
        {
            thc::ChessRules cr2;
            int ivar;
            int imove;
            GameTree *parent = tree_bc.Parent( found, cr2, ivar, imove );
            if( parent )
            {
                VARIATION &variation = parent->variations[ivar];
                std::string str = found->game_move.comment.substr(offset_within_comment);
                found->game_move.comment = found->game_move.comment.substr(0,offset_within_comment);
                Rebuild();
                PgnParse( false, nbr_converted_scenario_1b, str, cr, &variation, false, imove );
                if( nbr_converted_scenario_1b > 0 )
                    candidate_scenario_1b = tree_bc;
            }
        }

        // Try scenario 2;
        //   Comment works as a variation, but is not yet parenthesised,
        //   eg 1.e4 e5 2.Nf3 {2.Nc3 Nf6 3.f4}
        //   (Scenario 2a, use current language)
        tree_bc = save;
        Rebuild();
        found = Locate( pos, cr, title, at_move0 );
        if( found && found->game_move.comment.length()>offset_within_comment && found->game_move.comment[offset_within_comment]!='(' )
        {
            pos_restore_scenario2 = gv.GetMoveOffset( found ); // where to end up pointing
            thc::ChessRules cr2;
            int ivar;
            int imove;
            GameTree *parent = tree_bc.Parent( found, cr2, ivar, imove );
            if( parent )
            {
                VARIATION &variation = parent->variations[ivar];
                std::string str;
                str = "(";
                str += found->game_move.comment.substr(offset_within_comment);
                str += ")";
                found->game_move.comment = found->game_move.comment.substr(0,offset_within_comment);
                Rebuild();
                PgnParse( false, nbr_converted_scenario_2a, str, cr, &variation, true, imove );
                if( nbr_converted_scenario_2a > 0 )
                    candidate_scenario_2a = tree_bc;
            }
        }

        // Scenario 2b, use English
        tree_bc = save;
        Rebuild();
        found = Locate( pos, cr, title, at_move0 );
        if( found && found->game_move.comment.length()>offset_within_comment && found->game_move.comment[offset_within_comment]!='(' )
        {
            thc::ChessRules cr2;
            int ivar;
            int imove;
            GameTree *parent = tree_bc.Parent( found, cr2, ivar, imove );
            if( parent )
            {
                VARIATION &variation = parent->variations[ivar];
                std::string str;
                str = "(";
                str += found->game_move.comment.substr(offset_within_comment);
                str += ")";
                found->game_move.comment = found->game_move.comment.substr(0,offset_within_comment);
                Rebuild();
                PgnParse( false, nbr_converted_scenario_2b, str, cr, &variation, false, imove );
                if( nbr_converted_scenario_2b > 0 )
                    candidate_scenario_2b = tree_bc;
            }
        }

        // Select one or the scenarios (or none if nothing worked)
        if( nbr_converted_scenario_1a==0  && nbr_converted_scenario_1b==0 &&
            nbr_converted_scenario_2a==0  && nbr_converted_scenario_2b==0 )
        {
            tree_bc = save;
            no_changes = true;
        }

        // Current language trumps English, less importantly scenario 1 trumps scenario 2
        else if( nbr_converted_scenario_1a>=nbr_converted_scenario_1b &&
                 nbr_converted_scenario_1a>=nbr_converted_scenario_2a &&
                 nbr_converted_scenario_1a>=nbr_converted_scenario_2b
               )
        {
            tree_bc = candidate_scenario_1a;
            pos_restore = pos_restore_scenario1;
        }
        else if( nbr_converted_scenario_2a>=nbr_converted_scenario_2b &&
                 nbr_converted_scenario_2a>=nbr_converted_scenario_1a &&
                 nbr_converted_scenario_2a>=nbr_converted_scenario_1b
               )
        {
            tree_bc = candidate_scenario_2a;
            pos_restore = pos_restore_scenario2;
        }
        else if( nbr_converted_scenario_1b>=nbr_converted_scenario_1a &&
                 nbr_converted_scenario_1b>=nbr_converted_scenario_2a &&
                 nbr_converted_scenario_1b>=nbr_converted_scenario_2b
               )
        {
            tree_bc = candidate_scenario_1b;
            pos_restore = pos_restore_scenario1;
        }
        else if( nbr_converted_scenario_2b>=nbr_converted_scenario_2a &&
                 nbr_converted_scenario_2b>=nbr_converted_scenario_1a &&
                 nbr_converted_scenario_2b>=nbr_converted_scenario_1b
               )
        {
            tree_bc = candidate_scenario_2b;
            pos_restore = pos_restore_scenario2;
        }
    }

    // Rebuild
    Rebuild();
    if( !no_changes )
        gl->atom.Undo();
    pos = pos_restore;
    gl->atom.Redisplay( pos );
#endif
}

void GameDocument::DemoteToComment()
{
#if 0
    unsigned long pos = GetInsertionPoint();
    thc::ChessRules cr;
    std::string title;
    bool at_move0;
    GameTree *found = Locate( pos, cr, title, at_move0 );
    if( found )
    {
        thc::ChessRules cr2;
        int ivar;
        int imove;
        GameTree *parent = tree_bc.Parent( found, cr2, ivar, imove );
        if( parent )
        {
            VARIATION &variation = parent->variations[ivar];
            int sz = variation.size();
            if( imove+1 < sz+at_move0?1:0 )
            {
                std::string str;
                if( at_move0 )
                {
                    pos = gv.GetMoveOffset(parent);
                    GameTree *move = &variation[0];
                    int begin = gv.GetInternalOffset(move);
                    int end   = gv.GetInternalOffsetEndOfVariation(begin);
                    gv.ToCommentString(str,begin,end);
                    std::string existing_comment = variation[0].game_move.pre_comment;
                    if( parent == &tree_bc )
                        variation.clear();
                    else
                        tree_bc.DeleteVariation(found);
                    if( existing_comment == "" )
                        parent->game_move.comment = str;
                    else
                    {
                        std::string str2;
                        str2 = "{";
                        str2 += existing_comment;
                        str2 += "} ";
                        str2 += str;
                        parent->game_move.comment = str2;
                    }
                }
                else
                {
                    pos = gv.GetMoveOffset( found );
                    GameTree *next_move = &variation[imove+1];
                    int begin = gv.GetInternalOffset(next_move);
                    int end   = gv.GetInternalOffsetEndOfVariation(begin);
                    gv.ToCommentString(str,begin,end);
                    tree_bc.DeleteRestOfVariation(found);
                    if( found->game_move.comment == "" )
                        found->game_move.comment = str;
                    else
                    {
                        std::string str2;
                        str2 = "{";
                        str2 += found->game_move.comment;
                        str2 += "} ";
                        str2 += str;
                        found->game_move.comment = str2;
                    }
                }
                Rebuild();
                gl->atom.Undo();
                gl->atom.Redisplay( pos );
            }
        }
    }
#endif
}

void GameDocument::UseGame( const thc::ChessPosition &cp, const std::vector<thc::Move> &moves_from_base_position, CompactGame &pact )
{
#if 0
    unsigned long pos = GetInsertionPoint();
    unsigned long pos_restore = pos;
    thc::ChessRules cr;
    std::string title;
    bool at_move0=false;
    GameTree save=tree_bc;

    GameTree *found = Locate( pos, cr, title, at_move0 );
    bool have_moves = HaveMoves();
    if( !have_moves || (found && cr==cp) )
    {
        cprintf( "Step 1 okay\n" );
        for( size_t i=0; i<moves_from_base_position.size(); i++ )
        {
            thc::Move mv = moves_from_base_position[i];
            cr.PlayMove(mv);
        }
        thc::ChessRules cr2 = pact.GetStartPosition();
        int offset_of_rest_of_game=0;
        bool position_found = false;
        if( cr2 == cr )
            position_found = true;
        else
        {
            for( size_t i=0; i<pact.moves.size(); i++ )
            {
                thc::Move mv = pact.moves[i];
                cr2.PlayMove(mv);
                if( cr2 == cr )
                {
                    offset_of_rest_of_game = i+1;
                    position_found = true;  // don't break, go to later position if repetitions
                }
            }
        }
        if( position_found )
        {
            cprintf( "Full move count initially=%d, %d further moves, then pick up from offset %d\n",
                cp.full_move_count,
                moves_from_base_position.size(),
                offset_of_rest_of_game );

            // Game description
            std::string description = pact.Description();

            // combined moves to end of game variation
            std::vector<thc::Move> rest_of_game( pact.moves.begin()+offset_of_rest_of_game, pact.moves.end() );
            std::vector<thc::Move> combined = moves_from_base_position;
            combined.insert( combined.end(), rest_of_game.begin(), rest_of_game.end() );  // combined += rest_of_game;

            // find the variation we are in
            thc::ChessRules cr3;
            int ivar;
            int imove;
            GameTree *parent = tree_bc.Parent( found, cr3, ivar, imove );
            if( !have_moves )
            {
                VARIATION &variation = tree_bc.variations[0];
                for( size_t j=0; j<combined.size(); j++ )
                {
                    GameTree m;
                    if( j == 0 )
                    {
                        int len = tree_bc.game_move.comment.size();
                        if( len > 0 )
                        {
                            m.game_move.pre_comment = tree_bc.game_move.comment + " " + description;
                            tree_bc.game_move.comment.clear();
                        }
                        else
                        {
                            m.game_move.pre_comment = description;
                        }
                    }
                    thc::Move mv = combined[j];
                    m.game_move.move = mv;
                    variation.push_back(m);
                }
            }
            else if( parent )
            {
                cprintf( "Parent found, ivar=%d, imove=%d\n", ivar, imove );
                VARIATION &variation = parent->variations[ivar];
                int nbr_common_moves=0;
                for( size_t j=imove+(at_move0?0:1), k=0; j<variation.size() && k<combined.size(); j++, k++ )
                {
                    GameTree m = variation[j];
                    if( m.game_move.move == combined[k] )
                        nbr_common_moves++;
                    else
                        break;
                }
                int offset_last_common_move = imove + nbr_common_moves;
                int variation_size = variation.size();
                bool append_not_branch = offset_last_common_move >= (variation_size-1);
                if( append_not_branch )
                {
                    cprintf( "Append, nbr_common_moves=%d, combined.size()=%d\n", nbr_common_moves, combined.size() );
                    GameTree &branch_point = *(variation.begin() + offset_last_common_move);
                    branch_point.game_move.comment += description;
                    branch_point.game_move.comment += " continued";
                    for( size_t j=nbr_common_moves; j<combined.size(); j++ )
                    {
                        GameTree m;
                        thc::Move mv = combined[j];
                        m.game_move.move = mv;
                        variation.push_back(m);
                    }
                }
                else
                {
                    cprintf( "Branch, nbr_common_moves=%d, combined.size()=%d\n", nbr_common_moves, combined.size() );
                    GameTree &branch_point = *(variation.begin() + offset_last_common_move + (at_move0?0:1));
                    std::vector<GameTree> sub_variation;
                    bool first=true;
                    for( size_t j=nbr_common_moves; j<combined.size(); j++ )
                    {
                        GameTree m;
                        m.game_move.move = combined[j];
                        if( first )
                        {
                            m.game_move.pre_comment = description;
                            first = false;
                        }
                        sub_variation.push_back(m);
                    }
                    branch_point.variations.push_back( sub_variation );
                }
            }
        }
    }

    // Rebuild
    Rebuild();
    gl->atom.Undo();
    pos = pos_restore;
    gl->atom.Redisplay( pos );
#endif
}

void GameDocument::DeleteRestOfVariation()
{
#if 0
    unsigned long pos = GetInsertionPoint();
    thc::ChessRules cr;
    std::string title;
    bool at_move0;
    GameTree *found = Locate( pos, cr, title, at_move0 );
    if( found )
    {
        thc::ChessRules cr2;
        int ivar;
        int imove;
        GameTree *parent = tree_bc.Parent( found, cr2, ivar, imove );
        if( parent )
        {
            VARIATION &variation = parent->variations[ivar];
            int sz = variation.size();
            if( imove+1 < sz+at_move0?1:0 )
            {
                std::string str;
                if( at_move0 )
                {
                    tree_bc.DeleteVariation(found);
                    pos = gv.GetMoveOffset(parent);
                }
                else
                {
                    tree_bc.DeleteRestOfVariation(found);
                    pos = gv.GetMoveOffset( found );
                }
                if( tree_bc.variations.size() == 0 ) // deleted main variation ?
                {
                    VARIATION empty_variation;
                    tree_bc.variations.push_back( empty_variation );
                }
                Rebuild();
                gl->atom.Undo();
                gl->atom.Redisplay( pos );
            }
        }
    }
#endif
}

void GameDocument::DeleteVariation()
{
#if 0
    unsigned long pos = GetInsertionPoint();
    thc::ChessRules cr;
    std::string title;
    bool at_move0;
    GameTree *found = Locate( pos, cr, title, at_move0 );
    if( found )
    {
        thc::ChessRules cr2;
        int ivar;
        int imove;
        GameTree *parent = tree_bc.Parent( found, cr2, ivar, imove );
        if( parent )
        {
            std::string str;
            tree_bc.DeleteVariation(found);
            pos = gv.GetMoveOffset(parent);
            if( tree_bc.variations.size() == 0 ) // deleted main variation ?
            {
                VARIATION empty_variation;
                tree_bc.variations.push_back( empty_variation );
            }
            Rebuild();
            gl->atom.Undo();
            gl->atom.Redisplay( pos );
        }
    }
#endif
}

// A start position, a vector of moves, leading to a final position
void GameDocument::GetSummary( thc::ChessPosition &start_position_, std::vector<GAME_MOVE> &game_moves, thc::ChessPosition &end_pos )
{
#if 0
    unsigned long pos = GetInsertionPoint();
    start_position_ = this->start_position;
    end_pos = start_position_;
    std::string title;
    bool at_move0=false;
    thc::ChessRules cr;

    // Locate the node in the tree_bc corresponding to this position, also get the position
    //  on the board, a text title, and the at_move0 flag (true if pointing before the node)
    GameTree *found = Locate( pos, cr, title, at_move0 );
    int ivar=0;
    int imove=0;
    std::vector<GAME_MOVE> temp;
    if( found )
    {
        bool leaf = true;   // start at the leaf and iterate back
        GameTree *parent = tree_bc.Parent( found, cr, ivar, imove );
        while( parent )
        {
            VARIATION &variation = parent->variations[ivar];
            if( leaf )
            {
                for( int i=0; i<imove; i++ )
                    cr.PlayMove( variation[i].game_move.move );
                if( !at_move0 )
                    cr.PlayMove( variation[imove].game_move.move );
                end_pos = cr;
            }

            // Add moves in reverse order
            if( !at_move0 )
            {
                for( int i=(leaf?imove:imove-1); i>=0; i-- )
                {
                    GAME_MOVE game_move;
                    game_move = variation[i].game_move;
                    temp.push_back( game_move );
                }
            }
            leaf = false;
            at_move0 = false;
            parent = tree_bc.Parent( parent, cr, ivar, imove );
        }
    }
    game_moves.clear();
    int nbr = temp.size();
    for( int i=nbr-1; i>=0; i-- )
    {
        GAME_MOVE game_move = temp[i];
        game_moves.push_back( game_move );
    }
#endif
}

// The current position, title text for the last move played eg "Position after 23...Nxd5"
//  Return ptr to the last move played,  NULL if no last move OR if nbr_half_moves_lag
GAME_MOVE *GameDocument::GetSummaryTitle( thc::ChessRules &cr, std::string &title_txt, int nbr_half_moves_lag )
{
    GAME_MOVE *last_move = NULL;
#if 0
    if( nbr_half_moves_lag )
    {
        thc::ChessPosition end;
        std::vector<GAME_MOVE> game_moves;
        static GAME_MOVE ret;   // Return pointer to a static GAME_MOVE not a dynamic element in a local vector!
                                // Fixes Github issue: Several functions can access memory through invalid pointers #13
                                //  Thanks Github user metiscus for identifying this!
        GetSummary( cr, game_moves, end );
        int nbr = game_moves.size();
        if( nbr == 0 )
            title_txt = "Initial position";
        else
        {
            if( nbr > nbr_half_moves_lag )
                nbr -= nbr_half_moves_lag;
            else
                nbr = 0;
            char buf[80];
            if( nbr == 0 )
            {
                std::string s = game_moves[0].move.NaturalOut(&cr);
                LangOut(s);
                sprintf( buf, "Position before %d%s%s",
                    cr.full_move_count,
                    cr.white?".":"...",
                    s.c_str() );
            }
            else
            {
                for( int i=0; i<nbr-1; i++ )
                    cr.PlayMove( game_moves[i].move );
                ret = game_moves[nbr-1];
                last_move = &ret;
                std::string s = last_move->move.NaturalOut(&cr);
                LangOut(s);
                sprintf( buf, "Position after %d%s%s",
                        cr.full_move_count,
                        cr.white?".":"...",
                        s.c_str() );
                cr.PlayMove( last_move->move );  //play the move
            }
            title_txt = buf;
        }
    }
    else
    {
        unsigned long pos = GetInsertionPoint();
        cr = this->start_position;
        bool at_move0=false;
        GameTree *found = Locate( pos, cr, title_txt, at_move0 );
        if( found )
        {
            if( !at_move0 )
                last_move = &found->game_move;
            else
                last_move = gv.LocateAtMoveZeroGetLastMove();
        }
    }
#endif
    return( last_move );
}

//  We should define and use some simple recipes like this
GameTree *GameDocument::GetSummary()
{
    GameTree *found = NULL;
#if 0
    unsigned long pos = GetInsertionPoint();
    thc::ChessRules cr;
    found = Locate( pos, cr );
#endif
    return( found );
}

// The current position, move text for the last move played eg "23...Nxd5"
//  Return ptr to the last move played,  NULL if no last move OR if nbr_half_moves_lag
GAME_MOVE *GameDocument::GetSummaryMove( thc::ChessRules &cr, std::string &move_txt )
{
#if 0
    GameTree *found = NULL;
    unsigned long pos = GetInsertionPoint();
    cr = this->start_position;
    bool at_move0=false;
    move_txt = "";
    std::string title_txt;
    found = Locate( pos, cr, title_txt, at_move0 );
    if( title_txt.substr(0,15) == "Position after " )
        move_txt = title_txt.substr(15);
    return( found && !found->root ? &found->game_move : NULL );
#endif
    return 0;
}


bool GameDocument::HaveMoves()
{
#if 0
    return( tree_bc.variations.size()>0 && tree_bc.variations[0].size()>0 );
#endif
    return false;
}

bool GameDocument::IsAtEnd()
{
    unsigned long pos = GetInsertionPoint();
    return( gv.IsAtEnd(pos) );
}

void GameDocument::ToFileTxtGameDetails( std::string &str )
{
    #ifdef _WINDOWS
    #define EOL "\r\n"
    #else
    #define EOL "\n"
    #endif
    std::string str1;
    str1 += "[Event \"";
    str1 += (r.event=="" ? "?" : r.event);
    str1 += "\"]" EOL;
    str1 += "[Site \"";
    str1 += (r.site=="" ? "?" : r.site);
    str1 += "\"]" EOL;
    str1 += "[Date \"";
    str1 += (r.date=="" ? "????.??.??" : r.date);
    str1 += "\"]" EOL;
    str1 += "[Round \"";
    str1 += (r.round=="" ? "?": r.round);
    str1 += "\"]" EOL;
    str1 += "[White \"";
    str1 += (r.white=="" ? "?": r.white);
    str1 += "\"]" EOL;
    str1 += "[Black \"";
    str1 += (r.black=="" ? "?": r.black);
    str1 += "\"]" EOL;
    str1 += "[Result \"";
    str1 += (r.result=="" ? "*" : r.result);
    str1 += "\"]" EOL;
    if( r.white_elo != "" )
    {
        str1 += "[WhiteElo \"";
        str1 += r.white_elo;
        str1 += "\"]" EOL;
    }
    if( r.black_elo != "" )
    {
        str1 += "[BlackElo \"";
        str1 += r.black_elo;
        str1 += "\"]" EOL;
    }
    if( r.eco != "" )
    {
        str1 += "[ECO \"";
        str1 += r.eco;
        str1 += "\"]" EOL;
    }
    thc::ChessPosition tmp;
    bool needs_fen = (tmp!=start_position);
    if( needs_fen )
    {
        str1 += "[FEN \"";
        str1 += start_position.ForsythPublish();
        str1 += "\"]" EOL;
    }
    for( std::pair<std::string,std::string> key_value: extra_tags )
    {
        str1 += "[";
        str1 += key_value.first;    // key
        str1 += " \"";
        str1 += key_value.second;    // value
        str1 += "\"]" EOL;
    }
    str1 += EOL;
    str = str1;
}

void GameDocument::ToFileTxtGameBody( std::string &str )
{
    std::string str1;
    gv.ToString( str1 );
    str1 += "\n";
    str1 += "\n";
    #ifndef _WINDOWS
    str = str1;
    #else
    std::string str2;
    int len = str1.length();
    for( int i=0; i<len; i++ )
    {
        if( str1[i] == '\n' )
            str2 += "\r\n";  // Windows filesystem in binary mode, so '\n' -> '\r','\n'
        else
            str2 += str1[i];
    }
    str = str2;
    #endif
}


void GameDocument::ToPublishTxtGameBody( std::string &str, int &diagram_base, int &mv_base, int &neg_base, int publish_options )
{
    std::string str1;
    gv.ToPublishString( str1, diagram_base, mv_base, neg_base, publish_options );
    str1 += "\n";
    str1 += "\n";
    #ifndef _WINDOWS
    str = str1;
    #else
    std::string str2;
    int len = str1.length();
    for( int i=0; i<len; i++ )
    {
        if( str1[i] == '\n' )
            str2 += "\r\n";  // Windows filesystem in binary mode, so '\n' -> '\r','\n'
        else
            str2 += str1[i];
    }
    str = str2;
    #endif
}


// Is other GameDocument different to this one ?
bool GameDocument::IsDiff( GameDocument &other )
{
    std::string s1;
    std::string s2;
    gv.ToString( s1 );
    other.gv.ToString( s2 );
    bool diff = (s1!=s2);
    if( !diff )   // if main part not different ...
    {
        ToFileTxtGameDetails( s1 ); // ... test header
        other.ToFileTxtGameDetails( s2 );
        diff = (s1!=s2);
    }
    return diff;
}

/*
void GameDocument::Temp()
{
    // Allow stacking of the key state variables
    const int MAX_DEPTH=20;
    struct STACK_ELEMENT
    {
        VARIATION  *pvar;
        thc::ChessRules cr;
        std::vector<VARIATION>::iterator v;
        std::vector<VARIATION>::iterator vend;
        std::vector<GameTree>::iterator m;
        std::vector<GameTree>::iterator mend;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    int stk_idx = 0;
    STACK_ELEMENT *stk = &stk_array[stk_idx];


    stk->v      = tree_bc.variations.begin();
    stk->vend   = tree_bc.variations.end();
    stk->m      = stk->v->begin();
    stk->mend   = stk->v->end();
    stk->cr     = *tree_bc.root;
    bool done=false;
    bool error=false;
    while( !done && !error )
    {
        for( ; stk->v != stk->vend; stk->v++ )
        {
            while( stk->m != stk->mend )
            {
                stk->cr.PushMove( stk->m->game_move.move );
                if( stk->m->variations.size() == 0 )
                    stk->m++;
                else if( stk_idx+1 >= MAX_DEPTH )
                {
                    done = true;
                    error = true;
                }
                else
                {
                    std::vector<GameTree>::iterator m = stk->m;
                    stk = &stk_array[++stk_idx];
                    stk->v      = m->variations.begin();
                    stk->vend   = m->variations.end();
                    stk->m      = stk->v->begin();
                    stk->mend   = stk->v->end();
                    stk->cr     = stk_array[stk_idx-1].cr;
                    stk->cr.PopMove( m->game_move.move );
                }
            }
        }
        if( done || stk_idx==0 )
            break;
        else
            stk = &stk_array[--stk_idx];
    }
}
  */

