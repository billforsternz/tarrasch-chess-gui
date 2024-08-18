/****************************************************************************
 * A complete view of a game's variations, built from a MoveTree
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "thc.h"
#include "GameLogic.h"
#include "Lang.h"
#include "DebugPrintf.h"
#include "Objects.h"
#include "Repository.h"
#include "Bytecode.h"
#include "Stepper.h"
#include "DebugPrintf.h"
#include "Util.h"
#include "GameView.h"

using namespace std;
using namespace thc;

//$ Note that result and start_position are part of game tree, refactor later
#if 0
void GameView::Build_bc( std::string &result_, GameTree &tree_bc_, thc::ChessPosition &start_position_ )
{
    start_position = start_position_;
    tree_bc = tree_bc_;
    result = result_;
    Bytecode bc(start_position);
    bc.GameViewOut( tree_bc.bytecode, result, expansion );
}
#endif

//$ Note that result and start_position are part of game tree, refactor later
//$ Get rid of Bytecode::GameViewOut() later
void GameView::Build_bc( std::string &result_, GameTree &tree_bc_, thc::ChessPosition &start_position_ )
{

    this->result = result_;
    final_position_txt = "Initial position";
    final_position.Init();
    this->tree_bc = tree_bc_;
    expansion.clear();
    level = 0;
    offset = 0;
    newline = true;
    this->start_position = start_position_;
    cr = start_position;
    std::string &bc = tree_bc.bytecode;

    bool move_nbr_needed = true;
    std::string space;
    Stepper it(start_position,bc);
    for(; !it.End(); it++)
    {
        switch( it.ct )
        {
            case ct_move:
            {
                // Handle moves
                GameViewElement gve;
                bool move0 = (it.stk->variation_move_count == 0 );
                if( move0 )
                {
                    gve.type    = MOVE0;
                    gve.level   = level;
                    gve.offset1 = offset;
                    gve.offset2 = offset;
                    gve.offset_bc = it.idx;
                    gve.mv      = it.stk->mv;
                    expansion.push_back(gve);
                }

                // Body of move
                bool need_extra_space=true;
                std::string intro;
                if( move0 || newline || comment )
                    intro = util::sprintf( "%d%s", it.cr.full_move_count, it.cr.white?".":"..." );
                else
                {
                    if( it.cr.white )
                        intro = util::sprintf( " %d.", it.cr.full_move_count );
                    else
                    {
                        intro = " ";
                        need_extra_space = false;
                    }
                }
                newline = false;
                comment = false;
                string move_body = it.san_move;
                LangOut(move_body);
                string fragment  = intro + move_body;
                string file_view = need_extra_space ? (intro + " " + move_body) : fragment;
                gve.type    = MOVE;
                gve.level   = level;
                gve.offset1 = offset;
                offset += (fragment.length());
                gve.offset2 = offset;
                gve.str     = fragment;
                gve.str_for_file_move_only = file_view;
                gve.offset_bc = it.idx;
                expansion.push_back(gve);
                break;                                                                                                        
            }

            case ct_variation_start:        
            {
                level++;
                GameViewElement gve;

                // If not root variation, add "\n\t\t...\t(" prefix
                if( level > 1)
                {
                    gve.type    = NEWLINE;
                    newline     = true;
                    gve.level   = level;
                    gve.offset1 = offset;
                    offset++;   // "\n"
                    gve.offset2 = offset;
                    gve.offset_bc = it.idx;
                    expansion.push_back(gve);
                }

                gve.type   = START_OF_VARIATION;
                gve.offset1 = offset;
                offset++;   // "("
                gve.offset2 = offset;
                gve.offset_bc = it.idx;
                expansion.push_back(gve);
                break;
            }
            case ct_variation_end:        
            {
                level--;
                // If not root variation, add ")" or ")\n\t\t...\t" suffix
                if( level > 0 )
                {
                    bool another_variation_follows = (it.idx+1<it.len && it.bc[it.idx+1]==BC_VARIATION_START);
                    GameViewElement gve;
                    gve.type    = END_OF_VARIATION;
                    gve.level   = level+1;
                    gve.offset1 = offset;
                    offset++;       // ")"
                    gve.offset2 = offset;
                    gve.offset_bc = it.idx;
                    expansion.push_back(gve);
                    if( !another_variation_follows )
                    {
                        gve.type    = NEWLINE;
                        gve.level   = level;
                        gve.offset1 = offset;
                        offset++;       // "\n"
                        gve.offset2 = offset;
                        newline = true;
                        gve.offset_bc = it.idx;
                        expansion.push_back(gve);
                    }
                }
                break;
            }
            case ct_comment_end:        
            {
                GameViewElement gve;
                gve.type    = COMMENT;
                gve.level   = level;
                gve.offset1 = offset;
                std::string comment_ = it.comment_txt;
                offset += comment_.length();
                gve.offset2 = offset;
                gve.offset_bc = it.idx;
                expansion.push_back(gve);
                break;
            }
            case ct_meta_start:        
            {
                break;
            }
            case ct_meta_end:        
            {
                break;
            }
            case ct_escape_code:
            {
                // NAG TODO
                break;
            }
        }


#if 0 // NAG TODO
        gve.nag_value1 = 0;
        if( node->game_move.nag_value1 &&
            node->game_move.nag_value1 < (sizeof(nag_array1)/sizeof(nag_array1[0]))
          )
        {
            gve.nag_value1 = node->game_move.nag_value1;
            fragment.append( nag_array1[node->game_move.nag_value1] );
            char buf2[50];
            sprintf(buf2," $%d",node->game_move.nag_value1);
            file_view.append(buf2);
        }
        gve.nag_value2 = 0;
        if( node->game_move.nag_value2 &&
            node->game_move.nag_value2 < (sizeof(nag_array2)/sizeof(nag_array2[0]))
          )
        {
            gve.nag_value2 = node->game_move.nag_value2;
            fragment.append( nag_array2[node->game_move.nag_value2] );
            char buf2[50];
            sprintf(buf2," $%d",node->game_move.nag_value2);
            file_view.append(buf2);
        }
#endif

#if 0 // FINAL POSITION TODO
        if( last_move )
        {
            if( level == 1 )
            {
                final_position = it.cr;
                final_position.PlayMove( node->game_move.move );
                final_position_node = node;
                sprintf( buf, "Final position after %d%s", it.cr.full_move_count, it.cr.white?".":"..." );
                final_position_txt  = buf + move_body;
            }
        }
#endif
    }

    // After all root variations done add END_OF_GAME
    GameViewElement gve;
    gve.type    = END_OF_GAME;
    gve.level   = level+1;
    gve.offset1 = offset;

    // Add space before result if not following comment and not following newline
    //  (it would be END_OF_VARIATION rather than NEWLINE, except that the last
    //   variation's END_OF_VARIATION is followed by NEWLINE)
    int sz=expansion.size();
    bool add_space = (sz==0 ? false : (expansion[sz-1].type!=NEWLINE && expansion[sz-1].type!=COMMENT) );
    std::string temp = (result==""?"*":result); // in file case we make sure we DO write "*"
    gve.str_for_file_move_only = (add_space ? " " + temp : temp);
    gve.str = "";
    size_t len = result.length();
    if( len > 1 )   // not "*", in screen case we make sure we DON'T write "*"
        gve.str = (add_space ? " " + result : result);
    offset += gve.str.length();
    gve.offset2 = offset;
    gve.offset_bc = it.idx;
    expansion.push_back(gve);

    // Save a copy of the language as it was when we built the view
    memcpy( language_lookup, LangGet(), sizeof(language_lookup) );
}


static const char *nag_array1[] =
{   // no leading space on these so Nf3+! not Nf3+ !
    "",
    "!",     // $1
    "?",     // $2
    "!!",    // $3
    "??",    // $4
    "!?",    // $5
    "?!",    // $6
    "",      // $7
    "",      // $8
    "??"     // $9
};

static const char *nag_array2[] =
{   // leading space on these so Nf3+! +- not Nf3+!+-
    //  apart from anything else this helps us PgnParse() our comments properly
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
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
};

/*
static const char *pgn_result_array_screen[] =
{
    "",              // PGN_RESULT_NULL, special value means no associated pgn result
    "",              // PGN_RESULT_LINE (note no "*")
    " 1/2-1/2",      // PGN_RESULT_DRAW
    " 1-0",          // PGN_RESULT_WHITE_WIN
    " 0-1",          // PGN_RESULT_BLACK_WIN
};

static const char *pgn_result_array_file[] =
{
    "",              // PGN_RESULT_NULL, special value means no associated pgn result
    " *",            // PGN_RESULT_LINE (note "*")
    " 1/2-1/2",      // PGN_RESULT_DRAW
    " 1-0",          // PGN_RESULT_WHITE_WIN
    " 0-1",          // PGN_RESULT_BLACK_WIN
}; */


#if 0
    #define gv_printf(...) cprintf ( __VA_ARGS__ ), cprintf("\n")
#else
    #define gv_printf(...)
#endif

void GameView::Display( unsigned long pos )
{
    wxRichTextCtrl *ctrl = objs.canvas->lb;
    if( ctrl )
    {
#ifndef THC_UNIX
        ctrl->Freeze();
        gv_printf( "ctrl->Freeze();" );
#endif
        ctrl->BeginSuppressUndo();
        gv_printf( "ctrl->BeginSuppressUndo();" );

        // don't try and restore scroll position when at top of document - clear
        //  will take us to the top naturally
        if( pos == 0 )
            ctrl->Clear();

        // restore scroll position
        else
        {
            int x, y;
            ctrl->GetScrollHelper()->GetViewStart( &x, &y );
            ctrl->Clear();
            ctrl->GetScrollHelper()->Scroll(x,y);
        }
        gv_printf( "ctrl->Clear();" );
        ctrl->EndAllStyles();
        gv_printf( "ctrl->EndAllStyles();" );
        ctrl->BeginParagraphSpacing(0, 10);
        gv_printf( "ctrl->BeginParagraphSpacing(0, 10);" );
        bool italic=false;
        bool bold=true;
        ctrl->BeginBold();
        gv_printf( "ctrl->BeginBold();" );
        ctrl->BeginLeftIndent(0);
        gv_printf( "ctrl->BeginLeftIndent(0);" );
        int nbr = expansion.size();
        for( int i=0; i<nbr; i++ )
        {
            const char *c_str = NULL;
            GameViewElement &gve = expansion[i];
            switch( gve.type )
            {
                case COMMENT:
                {
                    c_str = gve.str.c_str();
                    ctrl->BeginTextColour(wxColour(0, 0, 255));
                    gv_printf( "ctrl->BeginTextColour(wxColour(0, 0, 255));" );
                    if( i )
                    {
                        ctrl->WriteText( " " );
                        gv_printf( "ctrl->WriteText( " " );" );
                    }
                    ctrl->WriteText( c_str );
                    gv_printf( "ctrl->WriteText( \"%s\" );", c_str );
                    ctrl->WriteText( " " );
                    gv_printf( "ctrl->WriteText( " " );" );
                    ctrl->EndTextColour();
                    gv_printf( "ctrl->EndTextColour();" );
                    break;
                }
                case MOVE0:
                {
                    break;
                }
                case MOVE:
                {
                    ctrl->WriteText( gve.str.c_str() );
                    gv_printf( "ctrl->WriteText( \"%s\" );", gve.str.c_str() );
                    break;
                }
                case START_OF_VARIATION:
                {
                    ctrl->WriteText( "(" );
                    gv_printf( "ctrl->WriteText( \"(\" );" );
                    break;
                }
                case END_OF_VARIATION:
                {
                    ctrl->WriteText( ")" );
                    gv_printf( "ctrl->WriteText( \")\" );" );
                    break;
                }
                case END_OF_GAME:
                {
                    if( gve.str.length() )
                    {
                        ctrl->WriteText( gve.str.c_str() );
                        gv_printf( "ctrl->WriteText( \"%s\" );", gve.str.c_str() );
                    }
                    break;
                }
                case NEWLINE:
                {
                    if( bold )
                    {
                        ctrl->EndBold();
                        gv_printf( "ctrl->EndBold();" );
                        bold = false;
                    }
                    if( italic )
                    {
                        ctrl->EndItalic();
                        gv_printf( "ctrl->EndItalic();" );
                        italic = false;
                    }
                    ctrl->Newline();
                    gv_printf( "ctrl->Newline();" );
                    ctrl->EndLeftIndent();
                    gv_printf( "ctrl->EndLeftIndent();" );
                    if( gve.level > 0 )
                    {
                        ctrl->BeginLeftIndent(20*(gve.level-1));
                        gv_printf( "ctrl->BeginLeftIndent( %d );", 20*(gve.level-1) );
                    }
                    else
                    {
                        ctrl->BeginLeftIndent(0);
                        gv_printf( "ctrl->BeginLeftIndent(0);" );
                    }
                    if( gve.level == 1 )
                    {
                        bold = true;
                        ctrl->BeginBold();
                        gv_printf( "ctrl->BeginBold();" );
                    }
                    else if( gve.level > 2 && !objs.repository->general.m_no_italics )
                    {
                        italic = true;
                        ctrl->BeginItalic();
                        gv_printf( "ctrl->BeginItalic();" );
                    }
                    break;
                }
            }
        }
        if( bold )
        {
            ctrl->EndBold();
            gv_printf( "ctrl->EndBold();" );
        }
        if( italic )
        {
            ctrl->EndItalic();
            gv_printf( "ctrl->EndItalic();" );
        }
        ctrl->SetInsertionPoint(pos);
        gv_printf( "ctrl->SetInsertionPoint(%lu);", pos );
        ctrl->EndSuppressUndo();
        gv_printf( "ctrl->EndSuppressUndo();" );
#ifndef THC_UNIX
        ctrl->Thaw();
        gv_printf( "ctrl->Thaw();" );
#endif
        ctrl->Update();
        gv_printf( "ctrl->Update();" );
        ctrl->ShowPosition(pos);
        gv_printf( "ctrl->ShowPosition(%lu);", pos );
    }
}

std::string ReplaceAll( const std::string &in, const std::string &from, const std::string &to )
{
    std::string out = in;
    size_t pos = out.find(from);
    while( pos != std::string::npos )
    {
        out.replace( pos, from.length(), to );
        pos = out.find( from, pos+to.length() );
    }
    return out;
}

void GameView::ToString( std::string &str )
{
    int nbr = expansion.size();
    ToString( str, 0, nbr );
}

void GameView::ToString( std::string &str, int begin, int end )
{
    str = "";
    int col = 0;
    for( int i=begin; i<end; i++ )
    {
        std::string frag="";
        GameViewElement &gve = expansion[i];
        bool make_comment = false;
        switch( gve.type )
        {
            case COMMENT:
            {

                // New policy with V2.03c
                //  Only create '{' comments (most chess software doesn't understand ';' comments)
                //  If  "}" appears in comment transform to "|>"    (change *is* restored when reading .pgn)
                //  If  "|>" appears in comment transform to "| >"  (change is *not* restored when reading .pgn)
                frag = gve.str;
                std::string groomed = ReplaceAll(frag,"|>", "| >");
                groomed = ReplaceAll(groomed,"}","|>");
                if( i == begin )
                    frag = "{" + groomed + "} ";
                else
                    frag = " {" + groomed + "} ";
                break;
            }
            case MOVE0:
            {
                break;
            }
            case MOVE:
            {
                frag = gve.str_for_file_move_only;
                LangToEnglish(frag,language_lookup);
                break;
            }
            case START_OF_VARIATION:
            {
                frag = " (";
                break;
            }
            case END_OF_VARIATION:
            {
                frag =  ") ";
                break;
            }
            case END_OF_GAME:
            {
                frag = gve.str_for_file_move_only;
                break;
            }
        }

        // Append with line wrap
        size_t idx_f = frag.find(' ');
        bool atomic = (idx_f==string::npos);  // atomic if no space
        bool splitting = !atomic;           // don't split if atomic
        while( splitting )
        {
            int len = frag.length();
            if( col+len < 80 )
                break;  // don't need to split
            int idx = len-1;

            // Split the fragment in two if possible
            splitting = false;
            while( idx > 0 )
            {
                if( col+idx<80 && frag[idx]==' ' )
                {
                    string part1 = frag.substr(0,idx);
                    string part2 = frag.substr(idx+1);

                    // In column 0, left trim
                    if( col == 0 )
                    {
                        size_t idx2 = part1.find_first_not_of(' ');
                        if( idx2!=string::npos && idx2>0 )
                            part1 = part1.substr(idx2);
                    }
                    str += part1;
                    str += "\n";
                    if( make_comment )
                        str += ";";
                    col = 0;
                    frag = part2;
                    splitting = true;   // split again if possible
                    break;
                }
                idx--;
            }

            // Maybe couldn't split because first split was beyond col 80
            if( !splitting )
            {
                idx = frag.find(' ');
                if( 0<=idx && idx<78 )
                {
                    splitting = true;
                    str += "\n";
                    if( make_comment )
                        str += ";";
                    col = 0;
                }
            }
        }

        // In column 0, left trim
        if( col == 0 )
        {
            size_t idx2 = frag.find_first_not_of(' ');
            if( idx2!=string::npos && idx2>0 )
                frag = frag.substr(idx2);
        }
        int len = frag.length();
        if( col+len < 80 )
        {
            str += frag;
            col += len;
        }
        else
        {
            str += "\n";
            if( make_comment )
                str += ";";
            str += frag;
            col = len;
        }
        if( make_comment )
            str += "\n";
    }
}

void GameView::ToPublishString( std::string &str, int &diagram_base, int &mv_base, int &neg_base, int publish_options  )
{
    int nbr = expansion.size();
    ToPublishString( str, diagram_base, mv_base, neg_base, publish_options, 0, nbr );
}

void GameView::ToPublishDiagram( std::string &str, thc::ChessPosition &cp )
{
    for( int i=0; i<64; i++ )
    {
        str += "<img src='png/";

        char square_colour = (i&1)?'w':'b';
        if( i<8 || (16<=i && i<24) || (32<=i && i<40) || (48<=i && i<56) )
            square_colour = (i&1)?'b':'w';
        char piece = cp.squares[i];
        char colour = square_colour;
        bool empty=true;
        if( isalpha(piece) )
        {
            empty = false;
            colour = isupper(piece) ? 'w' : 'b';
            if( isupper(piece) )
                piece = tolower(piece);
        }
        if( !empty )
        {
            str += colour;
            str += piece;
        }
        str += square_colour;
        str += ".png' alt=''>";
        if( (i&7) == 7 )
            str += "<br>\n";
    }
}

int ReplaceAllInplace( std::string &str, const char *in, const char *out )
{
    int first_found=-1;
    for(;;)
    {
        size_t pos = str.find(in);
        if( pos == std::string::npos )
            break;
        else
        {
            if( first_found == -1 )
                first_found = pos;
            str.replace(pos,strlen(in),out);
        }
    }
    return first_found;
}

bool GameView::ShouldDiagGoHere( int here, int end, int indent )
{
    bool diag_coming_up = false;
    bool diag_here = false;
    int move_count = 0;
    int comment_count = 0;
    for( int i=here+1; !diag_here && !diag_coming_up && i<end; i++ )
    {
        GameViewElement &gve = expansion[i];
        switch( gve.type )
        {
            case COMMENT:
            {
                comment_count++;
                std::string comment_txt = gve.str;
                size_t found, found2;
                found = comment_txt.find("#Diagram");
                found2 = comment_txt.find("Diagram #");
                if( found != std::string::npos || found2 != std::string::npos )
                    diag_coming_up = true;
                else if( indent==0 && move_count==0 )
                {
                    diag_here = true; // comment without #Diagram is ideal for an autogenerated diagram
                                      // (comment with #Diagram isn't because then we'll get two adjacent identical diagrams)
                }
                break;
            }
            case MOVE:
            {
                move_count++;
                break;
            }
            case START_OF_VARIATION:
            {
                if( indent==0 )
                {
                    diag_coming_up = true;
                    if( move_count == 0 )
                        diag_here = true;
                }
                indent++;
                break;
            }
            case END_OF_VARIATION:
            {
                indent--;
                break;
            }
        }
        if( comment_count>2 || move_count>5 )
            break;  // give up, a diagram isn't coming soon enough
    }
    if( !diag_here )
        diag_here = !diag_coming_up;
    return diag_here;
}

static const char * compressPosition( const char *pos )
{
    const char *abbrevChars = "cCausehgmvftdAUSEHGMVFTD";  // 12 so far, kqrbnp(6) reserved, ijlowxyz(8) available, 12+6+8=26
    const char* abbrevArray[] =
    {
        " rk ",     //c
        " RK ",     //C
        "rnbq",     //a
        "r bq",     //u
        "rn q",     //s
        "r  q",     //e
        "kbnr",     //h
        "kb r",     //g
        "k nr",     //m
        "k  r",     //v
        "pppp",     //f
        "ppp",      //t
        "pp",       //d
        "RNBQ",     //A
        "R BQ",     //U
        "RN Q",     //S
        "R  Q",     //E
        "KBNR",     //H
        "KB R",     //G
        "K NR",     //M
        "K  R",     //V
        "PPPP",     //F
        "PPP",      //T
        "PP"        //D
    };

    static char buf[65];
    char *dst = buf;
    int spacesOutstanding = 0;
    bool buffering;
    int abbrevIdx;
    const char *base = pos;
    while( pos-base < 64 )
    {
        buffering = false;
        abbrevIdx = -1;
        char c = *pos++;
        if( c == ' ' )
        {
            if( 0 == memcmp(pos,"rk ",3) )
                abbrevIdx = 0;
            else if( 0 == memcmp(pos,"RK ",3) )
                abbrevIdx = 1;
            else
            {
                buffering = true;
                spacesOutstanding++;
            }
        }
        else if( c=='p' || c=='P' || c=='r' || c=='k' || c=='R' || c=='K' )
        {
            for( int j=2; abbrevIdx==-1 && j<nbrof(abbrevArray); j++ )
            {
                const char *s = abbrevArray[j];
                if( 0 == memcmp(pos-1,s,strlen(s)) )
                    abbrevIdx = j;
            }
        }
        if( !buffering )
        {
            if( spacesOutstanding )
            {
                sprintf( dst, "%d", spacesOutstanding );
                spacesOutstanding = 0;
                while( *dst )
                    dst++;
            }
            if( abbrevIdx == -1 )
                *dst++ = c;
            else
            {
                *dst++ = abbrevChars[abbrevIdx];
                pos--;   // point at abbreviated sequence
                pos += strlen(abbrevArray[abbrevIdx]);
            }
        }
    }
    *dst = '\0';
    return buf;
}

static std::string removeExtraLineFeeds( const std::string &in )
{
    std::string str = in;
    for(;;)
    {
        size_t pos = str.find("\n\n");
        if( pos == std::string::npos )
            break;
        str.replace( pos, 2, "\n" );
    }
    return str;
}



void GameView::ToPublishString( std::string &str, int &diagram_base, int &mv_base, int &neg_base, int publish_options, int begin, int end )
{

    // TODO - remove this feature altogether - deprecated, replaced by WingedSpider
}

void GameView::ToCommentString( std::string &str )
{
    int nbr = expansion.size();
    ToCommentString( str, 0, nbr );
}

void GameView::ToCommentString( std::string &str, int begin, int end )
{
    str = "";
    for( int i=begin; i<end; i++ )
    {
        std::string frag="";
        GameViewElement &gve = expansion[i];
        switch( gve.type )
        {
            case COMMENT:
            {
                frag = gve.str;
                if( i == begin )
                    frag = "{" + frag + "} ";
                else
                    frag = " {" + frag + "} ";
                break;
            }
            case MOVE0:
            {
                break;
            }
            case MOVE:
            {
                frag = gve.str;
                break;
            }
            case START_OF_VARIATION:
            {
                frag = " (";
                break;
            }
            case END_OF_VARIATION:
            {
                frag =  ") ";
                break;
            }
            case END_OF_GAME:
            {
                frag = gve.str;
                break;
            }
        }
        if( str.length()>0 && frag!="" && frag[0]!=' ' )
        {
            if( str[str.length()-1] != ' ' )
                str += " ";
        }
        str += frag;
    }
}

unsigned long GameView::NavigationKey( unsigned long pos, NAVIGATION_KEY nk )
{
    //Debug();
    unsigned save_pos=pos;
    bool forward=true;              // down, right
    bool skip_to_mainline=true;     // right, left
    if( nk==NK_UP || nk==NK_LEFT || nk==NK_HOME || nk==NK_PGUP )
        forward = false;
    if( nk==NK_LEFT || nk==NK_RIGHT )
        skip_to_mainline = false;
    int nbr = expansion.size();
    for( int repeat=0; repeat < (nk==NK_PGUP||nk==NK_PGDOWN ? 4 : 1); repeat++ )
    {
        bool found=false;
        int level_original=0, level_=0;
        int start = forward ? 0   : nbr-1;
        int end   = forward ? nbr : -1;
        int iter  = forward ? +1  : -1;
        bool justfound=true;    // this is a truly horrible hack
        for( int i=start; i!=end; i+=iter )
        {
            GameViewElement &gve = expansion[i];
            if( !found )
            {
                if( gve.offset1<=pos && pos<=gve.offset2 )
                {
                    found = true;
                    level_original = level_ = gve.level;

                    // This is a specific "band-aid" type fix to Github.com issue "Moving backwards in main line not working #11"
                    if( iter==-1 && gve.type==NEWLINE && i-1!=end && expansion[i-1].type==MOVE && expansion[i-1].offset1<=pos && pos<=expansion[i-1].offset2)
                    {
                        /*
                            The minimal bug example was this document;
                            1.e4 e5
                              (1...c5)
                              (1...e6)
                            2.Nf3
                              (2...Nc3)
                            2...Nc6

                            Putting the cursor on 2...Nc6 and pressing up arrow twice did *not* get to 1...e5 (the cursor stuck
                            at 2.Nf3). Here is the structure revealed by a call to GameView::Debug();
                            MOVE0 level=1, offset1=0, offset2=0, str=
                            MOVE level=1, offset1=0, offset2=4, str=1.e4
                            MOVE level=1, offset1=4, offset2=7, str= e5
                            NEWLINE level=2, offset1=7, offset2=8, str=
                            START_OF_VARIATION level=2, offset1=8, offset2=9, str=
                            MOVE0 level=2, offset1=9, offset2=9, str=
                            MOVE level=2, offset1=9, offset2=15, str=1...c5
                            END_OF_VARIATION level=2, offset1=15, offset2=16, str=
                            NEWLINE level=2, offset1=16, offset2=17, str=
                            START_OF_VARIATION level=2, offset1=17, offset2=18, str=
                            MOVE0 level=2, offset1=18, offset2=18, str=
                            MOVE level=2, offset1=18, offset2=24, str=1...e6
                            END_OF_VARIATION level=2, offset1=24, offset2=25, str=
                            NEWLINE level=1, offset1=25, offset2=26, str=
                            MOVE level=1, offset1=26, offset2=31, str=2.Nf3
                            NEWLINE level=2, offset1=31, offset2=32, str=
                            START_OF_VARIATION level=2, offset1=32, offset2=33, str=
                            MOVE0 level=2, offset1=33, offset2=33, str=
                            MOVE level=2, offset1=33, offset2=38, str=2.Nc3
                            END_OF_VARIATION level=2, offset1=38, offset2=39, str=
                            NEWLINE level=1, offset1=39, offset2=40, str=
                            MOVE level=1, offset1=40, offset2=47, str=2...Nc6
                            END_OF_GAME level=1, offset1=47, offset2=47, str=

                            Analysis revealed that the first up arrow press set the cursor to pos=31 at the end of
                            the line with 2.Nf3. Then when the up arrow key was pressed again the backwards search
                            from the end would hit the NEWLINE level=2 instead of the MOVE level=1. These 2 lines;
                            MOVE level=1, offset1=26, offset2=31, str=2.Nf3
                            NEWLINE level=2, offset1=31, offset2=32, str=

                            The fix is this extra if clause which targets this very specific situation and iterates
                            one more time to the MOVE level=1 line instead of the NEWLINE level=2 line.
                        */
                        i--;
                        level_original = level_ = expansion[i].level;
                    }
                }
            }
            else
            {
                if( nk==NK_HOME )
                {
                    if( gve.type==END_OF_VARIATION )
                        justfound = false;
                    if( justfound && gve.type==MOVE && gve.level!=level_original )
                        level_original = gve.level;  // adjust, we might have had wrong
                                                     //  level because of NEWLINE
                    if( gve.level==level_original && gve.type==MOVE0 )
                        save_pos = gve.offset2;
                    if( i+iter == end )
                    {
                        pos = save_pos;
                        break;
                    }
                    else if( gve.level==level_original && gve.type==START_OF_VARIATION )
                    {
                        pos = save_pos;
                        break;
                    }
                }
                else if( nk==NK_END )
                {
                    if( gve.level==level_original && gve.type==MOVE )
                        save_pos = gve.offset2;
                    if( i+iter == end )
                    {
                        pos = save_pos;
                        break;
                    }
                    else if( gve.level==level_original && gve.type==END_OF_VARIATION )
                    {
                        pos = save_pos;
                        break;
                    }
                }
                else
                {
                    if( gve.type  == (forward?START_OF_VARIATION:END_OF_VARIATION) &&
                        gve.level == level_original
                    )
                        level_--;
                    if( gve.type==END_OF_GAME && forward )
                    {
                        pos = gve.offset2;
                        break;
                    }
                    if( gve.type==MOVE0 && gve.offset1!=pos )
                    {
                        if( !skip_to_mainline || gve.level<=level_ )
                        {
                            pos = gve.offset1;
                            break;
                        }
                    }
                    if( gve.type==MOVE && gve.offset2!=pos )
                    {
                        if( !skip_to_mainline || gve.level<=level_ )
                        {
                            pos = gve.offset2;
                            break;
                        }
                    }
                }
            }
        }
    }
    return pos;
}

int GameView::GetInternalOffsetEndOfVariation( int start )
{
    int nbr = expansion.size();
    GameViewElement &gve = expansion[start];
    int level_ = gve.level;
    int end = nbr;
    for( int i=start; i<nbr; i++ )
    {
        GameViewElement &gve2 = expansion[i];
        if( gve2.type==END_OF_VARIATION && gve2.level==level_ )
        {
            end = i;
            break;
        }
        if( gve2.type == END_OF_GAME )
        {
            end = i;
            break;
        }
    }
    return end;
}

bool GameView::Locate( unsigned long pos, thc::ChessRules &cr_, string &title, bool &at_move0 )
{
    std::vector<thc::Move> var;

    // TODO properly match this !
    bc_locate( tree_bc.bytecode, pos, start_position, var );
    title = "TODO - describe location";
    return false;
}

void GameView::Debug()
{
    int nbr = expansion.size();
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement gve = expansion[i];
        const char *s="???";
        switch( gve.type )
        {
            case COMMENT:               s="COMMENT";                break;
            case MOVE0:                 s="MOVE0";                  break;
            case MOVE:                  s="MOVE";                   break;
            case START_OF_VARIATION:    s="START_OF_VARIATION";     break;
            case END_OF_VARIATION:      s="END_OF_VARIATION";       break;
            case END_OF_GAME:           s="END_OF_GAME";            break;
            case NEWLINE:               s="NEWLINE";                break;
        }
        for( int j=0; j<gve.level; j++ )
            cprintf(" ");
        cprintf( "(%d) %s offset1=%d, offset2=%d, str=%s",
            gve.level,
            s,
            gve.offset1,
            gve.offset2,
            gve.str.c_str() );
        if( gve.type==MOVE0 || gve.type==MOVE )
        {
            cprintf( ", txt=%s, mv=%s", gve.str.c_str(), gve.mv.TerseOut().c_str() );
        }
        if( gve.type == COMMENT )
        {
            cprintf( ", comment=%s", gve.str.c_str() );
        }
        cprintf("\n");
    }
}

unsigned long GameView::FindMove0()
{
    int nbr = expansion.size();
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement gve = expansion[i];
        if( gve.type == MOVE0 )
            return gve.offset1;
    }
    return 0;
}

unsigned long GameView::FindEnd()
{
    unsigned long offs=0;
    int nbr = expansion.size();
    if( nbr > 0 )
    {
        GameViewElement gve = expansion[nbr-1];
        offs = gve.offset2;
    }
    return offs;
}

bool GameView::IsAtEnd( unsigned long pos )
{
    bool at_end=true;
    int nbr = expansion.size();
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement gve = expansion[i];
        // bool in_range   = (gve.offset1<=pos && pos<=gve.offset2);
        bool its_a_move = (gve.type==MOVE0 || gve.type==MOVE);
        bool gone_past  = (pos<gve.offset1);
        //bool not_yet    = (pos>gve.offset2);
        if( gone_past && its_a_move )
        {
            at_end = false;
            break;
        }
    }
    return at_end;
}

#if 0
unsigned long GameView::GetMoveOffset( MoveTree *node )
{
    unsigned long pos=0;
    int nbr = expansion.size();
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement gve = expansion[i];
        if( gve.type==MOVE && gve.node==node )
        {
            pos = gve.offset2;
            break;
        }
    }
    return pos;
}
#endif

int GameView::GetMoveOffset( int bc_offset )
{
    int nbr = expansion.size();
    int ret = 0;
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement &gve = expansion[i];
        if( gve.offset_bc > bc_offset )
            return (ret == 0 ? gve.offset2 : ret);  // return the offset of the last matching element
        if( gve.offset_bc >= bc_offset )
            ret = gve.offset2;
    }
    return ret;
}

bool GameView::GetOffsetWithinComment( unsigned long pos, unsigned long &pos_within_comment )
{
    bool found = false;
    int nbr = expansion.size();
    for( int i=0; !found && i<nbr; i++ )
    {
        GameViewElement &gve = expansion[i];
        if( gve.type==COMMENT )
        {
            if( gve.offset1==0 && gve.offset1<=pos && pos<gve.offset2 )
            {
                // Special case - no inserted space before an initial comment, so no
                //  need to subtract one from gve.offset1 to get pos_within_comment
                found = true;
                pos_within_comment = pos-gve.offset1;
            }
            else if( gve.offset1<pos && pos<gve.offset2 )
            {
                found = true;
                pos_within_comment = pos-gve.offset1-1;
            }
        }
    }
    return found;
}

bool GameView::CommentEdit( wxRichTextCtrl *UNUSED(ctrl), std::string &txt_to_insert, long keycode, bool *pass_thru_edit_ptr )
{
    bool pass_thru_edit = false;
    bool used = false;
    bool ascii = false;
    unsigned long pos = gl->atom.GetInsertionPoint();
    unsigned long orig_pos = pos;
    int nbr = expansion.size();
    if( 0x20<=keycode && keycode<=0xff && keycode!=0x7f /*delete*/ )
    {
        char c = (char)keycode;
        txt_to_insert = c;
        ascii = true;
    }
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement &gve = expansion[i];
        if( gve.offset1<=pos && pos<=gve.offset2 )
        {
            if( gve.type==COMMENT )
            {
                unsigned long home = gve.offset1+1; // 1 after leading ' '
                unsigned long end  = gve.offset2-1; // 1 before trailing ' '
                if( home == 1 )
                    home = 0;   // no leading space at offset 0
                if( home<=pos && pos<=end )
                {
                    int offset_within_comment = pos - home;
                    switch( keycode )
                    {
                        case WXK_BACK:
                        {
                            if( pos > home ) // can't backspace from home
                            {
                                bool empty;
                                gve.str.erase( offset_within_comment-1, 1 );
                                empty = (gve.str.length()==0);
                                if( home == 0 )
                                    pos--;
                                else
                                    pos -= (empty?2:1);
                                comment_edited = true;
                                used = true;
                                pass_thru_edit = (end-home > 1);     // only pass thru when substantial amount of comment remains
                            }
                            break;
                        }
                        case WXK_DELETE:
                        {
                            if( pos < end ) // can't delete at end
                            {
                                bool empty;
                                gve.str.erase( offset_within_comment, 1 );
                                empty = (gve.str.length()==0);
                                comment_edited = true;
                                used = true;
                                pass_thru_edit = (end-home > 1);     // only pass thru when substantial amount of comment remains
                            }
                            break;
                        }
                        case WXK_LEFT:
                        {
                            if( pos > home )
                            {
                                pos--;
                                used = true;
                            }
                            break;
                        }
                        case WXK_RIGHT:
                        {
                            if( pos < end )
                            {
                                pos++;
                                used = true;
                            }
                            break;
                        }
                        default:
                        {
                            if( txt_to_insert.length() )
                            {
                                gve.str.insert( offset_within_comment, txt_to_insert );
                                pos += txt_to_insert.length();
                                comment_edited = true;
                                pass_thru_edit = ascii;
                                used = true;
                            }
                        }
                    }
                }
            }
            break;
        }
    }
    if( !used &&  txt_to_insert.length() )
    {
        for( int i=0; i<nbr; i++ )
        {
            GameViewElement &gve = expansion[i];
            if( gve.offset1<=pos && pos<=gve.offset2 )
            {
                bool create_lone_comment = (gve.type==END_OF_GAME&&pos==0);
                if( gve.type==MOVE || gve.type==MOVE0 || create_lone_comment )
                {
                    comment_edited = true;
                    if( gve.type == MOVE0 )
                        gve.str += txt_to_insert;
                    else
                        gve.str += txt_to_insert;
                    if( create_lone_comment )
                    {
                        used = true;
                        pos += txt_to_insert.length();
                    }
                    else if( gve.type == MOVE0 )
                    {
                        used = true;
                        pos += (txt_to_insert.length() + (pos==0?0:1));
                    }
                    else
                    {
                        used = true;
                        pos = gve.offset2 + gve.str.length() + 1;
                    }
                    break;
                }
            }
        }
    }
    if( comment_edited )
    {
        gl->gd.Rebuild();
        bool will_be_passed_thru = pass_thru_edit_ptr && pass_thru_edit;
        if( !will_be_passed_thru )
            gl->atom.Display( pos );
        gl->atom.Undo();
    }
    else if( orig_pos != pos )
    {
        gl->atom.SetInsertionPoint(pos);
    }
    if( pass_thru_edit_ptr )
        *pass_thru_edit_ptr = pass_thru_edit;
    return used;
}

bool GameView::IsInComment( wxRichTextCtrl *UNUSED(ctrl) )
{
    bool pos_in_comment=false;
    unsigned long pos;
    pos = gl->atom.GetInsertionPoint();
    int nbr = expansion.size();
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement &gve = expansion[i];
        if( gve.type==COMMENT )
        {
            if( gve.offset1<pos && pos<gve.offset2 )
            {
                pos_in_comment = true;
                break;
            }
        }
    }
    return pos_in_comment;
}

bool GameView::IsSelectionInComment( wxRichTextCtrl *ctrl )
{
    bool selection_in_comment=false;
    unsigned long pos1, pos2;
    ctrl->GetSelection( reinterpret_cast<long *>(&pos1), reinterpret_cast<long *>(&pos2) );
    if( pos2 > pos1 )
    {
        int nbr = expansion.size();
        for( int i=0; i<nbr; i++ )
        {
            GameViewElement &gve = expansion[i];
            if( gve.type==COMMENT )
            {
                if( gve.offset1<=pos1 && pos2<gve.offset2 )
                {
                    selection_in_comment = true;
                    break;
                }
            }
        }
    }
    return selection_in_comment;
}

void GameView::DeleteSelection( wxRichTextCtrl *ctrl )
{
    unsigned long pos = ctrl->GetInsertionPoint();
    //unsigned long loc;
    unsigned long pos1, pos2;
    ctrl->GetSelection(reinterpret_cast<long *>(&pos1), reinterpret_cast<long *>(&pos2));
    ctrl->SetSelection(pos,pos);
    int nbr = expansion.size();
    int found=0;
    for( int i=0; i<nbr; i++ )
    {
        GameViewElement &gve = expansion[i];
        if( gve.offset1<=pos && pos<=gve.offset2 && gve.offset1<=pos1 && pos2<=gve.offset2 )
        {
            found = i;
            if( gve.type==COMMENT )
            {
                unsigned long home = gve.offset1+1; // 1 after leading ' '
                unsigned long end  = gve.offset2-1; // 1 before trailing ' '
                if( home == 1 )
                    home = 0;   // no leading space at offset 0
                if( home<=pos && pos<=end && home<=pos1 && pos2<=end  )
                {

                    // First delete, possibly the whole comment
                    int offset_within_comment = pos1 - home;
                    bool empty;
                    gve.str.erase( offset_within_comment, pos2-pos1 );
                    empty = (gve.str.length()==0);
                    if( home == 0 )
                        pos = empty?home:pos1, empty?end+1:pos2;
                    else
                        pos = empty?home-1:pos1, empty?end+1:pos2;
                    gl->gd.Rebuild();
                    gl->atom.Redisplay( pos );
                    gl->atom.Undo();
                }
            }
            break;
        }
    }
//  ctrl->SetInsertionPoint(pos);   // todo/later remove this and get caller's to use returned pos
//  return pos;
/*
    if( offset_adjust )
    {
        nbr = expansion.size();
        for( int i=nbr-1; i>=found; i-- )
        {
            GameViewElement &gve = expansion[i];
            if( i != found )
            {
                gve.offset1 += offset_adjust;
                gve.offset2 += offset_adjust;
            }
            else
            {
                gve.offset2 += offset_adjust;
                if( gve.type==COMMENT && gve.offset1==gve.offset2 )
                {
                    if( gve.str.length() == 0 )
                    {
                        vector<GameViewElement>::iterator it = expansion.begin() + i;
                        expansion.erase(it);
                    }
                }
                else if( gve.type==PRE_COMMENT && gve.offset1==gve.offset2 )
                {
                    if( gve.node->game_move.pre_comment.length() == 0 )
                    {
                        vector<GameViewElement>::iterator it = expansion.begin() + i;
                        expansion.erase(it);
                    }
                }
            }
        }
    } */
}


