//
// Some TDD (test driven development) for Tarrasch
//
//   (better late than never)
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include "thc.h"
#include "util.h"
#include "GameTree.h"
#include "Bytecode.h"
#include "Stepper.h"

int  game_tree_test();
bool it_txt_view(void *link, const std::string& bc, size_t offset, Codepoint &cpt);
std::string stepper_txt_view(const std::string& bc);
bool it_simple_expansion_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt);
void simple_expansion_test(const std::string& bc, std::string &s );
bool it_find_first_variation( const std::string& bc, int &offset );
bool find_first_variation( const std::string &bc, int &offset );

int core_printf( const char *fmt, ... )
{
    int ret=0;
    va_list args;
    va_start( args, fmt );
    char buf[1000];
    char *p = buf;
    vsnprintf( p, sizeof(buf)-2-(p-buf), fmt, args );
    fputs(buf,stdout);
    va_end(args);
    return ret;
}

int main()
{
    extern void compress_temp_lookup_gen_function();
    compress_temp_lookup_gen_function();

    thc::ChessRules cr;

    // Test internals, for porting to new environments etc
    bool ok = cr.TestInternals();
    printf( "TestInternals() returns %s\n", ok?"Success":"Fail" );


    game_tree_test();
    return 0;
}

std::string white_space_normalise( const std::string &in )
{
    std::string out;
    bool was_white = false;
    for( char c: in )
    {
        bool is_white = (c==' '||c=='\t'||c=='\n'||c=='\r');
        if( !is_white )
        {
            if( was_white )
                out += ' ';
            out += c;
        }
        was_white = is_white;
    }
    return out;
}

int game_tree_test()
{
    GameTree gt;
    std::vector<thc::Move> moves;
    thc::ChessPosition cp;

    // Load from PGN text
    std::string txt("1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6 {The exchange variation} ( {Alternatively} 4. Ba4 Nf6 5. O-O {is the main line} ) 4... dxc6");
    gt.PgnParse( txt );

    // Goto first variation
    int offset1, offset2;
    bool found = it_find_first_variation( gt.bytecode, offset1 );
    find_first_variation( gt.bytecode, offset2 );
    if( offset1 != offset2 )
        printf( "find_first_variation() mismatch\r\n" );

    // Promote a variation at current offset
    bool ok = gt.Promote();
    printf( "GameTree::Promote() returns ok=%s\n", ok?"true":"false" );

    // Demote a variation at current offset
    ok = gt.Demote();
    printf( "GameTree::Demote() returns ok=%s\n", ok?"true":"false" );

    // Bytecode type is intended to be increasingly the heart of Tarrasch
    Bytecode press;
    std::string txt2("1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6 {The exchange variation} ( {Alternatively} 4. Ba4 Nf6 5. O-O {is the main line} ) 4... dxc6");
    std::string bc = press.PgnParse(txt2);
    thc::ChessRules cr;
    std::string t = press.cr.ToDebugStr();
    printf( "%s", t.c_str() );
    std::string expected ="\n"
        "White to move\n"
        "r.bqkbnr\n"
        ".pp..ppp\n"
        "p.p.....\n"
        "....p...\n"
        "....P...\n"
        ".....N..\n"
        "PPPP.PPP\n"
        "RNBQK..R\n";
    ok = (t == expected);
    printf( "Bytecode test #1: %s\n", ok?"pass":"fail" );

    std::string stepper_simple_pgn_out;
    press.Reset();
    simple_expansion_test(bc, stepper_simple_pgn_out );
    printf( "stepper_simple_pgn_out uses Stepper:\n%s\n", stepper_simple_pgn_out.c_str() );

    std::string complex_example =
/*[Event "Waitakere Trust Open"]
[Site "Waitakere"]
[Date "2007.06.02"]
[Round "1.9"]
[White "Krstev, Mario"]
[Black "Forster, Bill"]
[Result "0-1"]
[WhiteElo "1718"]
[BlackElo "1880"]
[ECO "E20"] */

// Tarrasch original
#if 0
"1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. f3 d5 5. a3 Bxc3+ 6. bxc3 dxc4 7. e4 e5 $2 8.\n"
"Bxc4 Be6 9. Bxe6 fxe6 10. Qb3 O-O 11. Qxe6+ Kh8 12. Qxe5 {Objectively the\n"
"opening has been a catastrophe and Black is quite lost, but he does have a lead\n"
"in development...} 12... Nc6 13. Qg3 $2 {Diagram #Just one mistake is\n"
"sufficient for a turnaround. This is not the best square and Black has a lot of\n"
"fun for the rest of the game. Both White's proud extra centre pawns look\n"
"securely defended but Black gives up both knights for them to tear the position\n"
"open.} 13... Nxd4 $1 14. cxd4 Qxd4 15. Ra2 Qc4 {Fork!} 16. Ne2 Qxa2 17. Qxc7 $2\n"
"{Inviting catastrophe}  (17. Qe5 {returns the Queen from exile and White can\n"
"try to grovel an exchange down} ) 17... Rac8 18. Qxb7 {Diagram #} 18... Nxe4 $1\n"
"19. fxe4 Rxc1+ $1 20. Nxc1 Qxg2 $1 {Leaving White completely helpless} 21. Nd3\n"
"Qxh1+ 22. Kd2 Qxh2+ 23. Kc3 Qb8 24. Qe7 Re8 25. Qh4 Qc7+ 26. Kd2 Qa5+ 27. Ke2\n"
"Qxa3 28. e5 Qa6 29. Kd2 Qh6+ $1 {A nice practical decision. White does not want\n"
"to be accused of premature resignation. For anxious players like me in such\n"
"circumstances it's a good idea to permanently prevent any possible queen plus\n"
"knight shenanigans. A knight hates rook pawns, and here it faces three of them!\n"
"Even so as a nervous finisher I wanted to give up my rook for the knight and\n"
"eliminate the possibility of forks as well, but White never gave me a chance.}\n"
"30. Qxh6 gxh6 31. Ke3 Kg7 32. Ke4 Kg6 33. Nc5 h5 34. Kf4 h4 35. e6 Kf6 36. Kg4\n"
"Rxe6 37. Kxh4 Ke5 38. Kg4 Rg6+ 39. Kf3 Kd4 40. Nb3+ Kc3 41. Nc5 a5 42. Ke2 Kc4\n"
"43. Na4 Kb4 44. Nb2 Rg2+ 0-1\n";

// Proposed improvement, less unnecessary space
#else
"1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. f3 d5 5. a3 Bxc3+ 6. bxc3 dxc4 7. e4 e5 $2 8.\n"
"Bxc4 Be6 9. Bxe6 fxe6 10. Qb3 O-O 11. Qxe6+ Kh8 12. Qxe5 {Objectively the\n"
"opening has been a catastrophe and Black is quite lost, but he does have a lead\n"
"in development...} 12... Nc6 13. Qg3 $2 {Diagram #Just one mistake is\n"
"sufficient for a turnaround. This is not the best square and Black has a lot of\n"
"fun for the rest of the game. Both White's proud extra centre pawns look\n"
"securely defended but Black gives up both knights for them to tear the position\n"
"open.} 13... Nxd4 $1 14. cxd4 Qxd4 15. Ra2 Qc4 {Fork!} 16. Ne2 Qxa2 17. Qxc7 $2\n"
"{Inviting catastrophe} (17. Qe5 {returns the Queen from exile and White can try\n"
"to grovel an exchange down}) 17... Rac8 18. Qxb7 {Diagram #} 18... Nxe4 $1 19.\n"
"fxe4 Rxc1+ $1 20. Nxc1 Qxg2 $1 {Leaving White completely helpless} 21. Nd3\n"
"Qxh1+ 22. Kd2 Qxh2+ 23. Kc3 Qb8 24. Qe7 Re8 25. Qh4 Qc7+ 26. Kd2 Qa5+ 27. Ke2\n"
"Qxa3 28. e5 Qa6 29. Kd2 Qh6+ $1 {A nice practical decision. White does not want\n"
"to be accused of premature resignation. For anxious players like me in such\n"
"circumstances it's a good idea to permanently prevent any possible queen plus\n"
"knight shenanigans. A knight hates rook pawns, and here it faces three of them!\n"
"Even so as a nervous finisher I wanted to give up my rook for the knight and\n"
"eliminate the possibility of forks as well, but White never gave me a chance.}\n"
"30. Qxh6 gxh6 31. Ke3 Kg7 32. Ke4 Kg6 33. Nc5 h5 34. Kf4 h4 35. e6 Kf6 36. Kg4\n"
"Rxe6 37. Kxh4 Ke5 38. Kg4 Rg6+ 39. Kf3 Kd4 40. Nb3+ Kc3 41. Nc5 a5 42. Ke2 Kc4\n"
"43. Na4 Kb4 44. Nb2 Rg2+ 0-1\n";
#endif

    press.Reset();
    bc = press.PgnParse(complex_example);
    std::string outline_out = press.OutlineOut( bc, "0-1" );
    printf( "Complex outline out:\n%s\n", outline_out.c_str() );
    press.Reset();
    std::string rough_out;
    simple_expansion_test(bc, rough_out );
    printf( "Game 2 (complex) stepper PGN dump:\n%s\n", rough_out.c_str() );
    press.Reset();
    std::string txt_out = press.PgnOut( bc, "0-1" );
    std::string txt_out2 = white_space_normalise(txt_out);
    printf( "Refined dump:\n%s\n", txt_out2.c_str() );
    std::string txt_out3 = white_space_normalise(complex_example);
    printf( "Should match input:\n%s\n", txt_out3.c_str() );
    ok = (txt_out2 == txt_out3);
    printf( "Bytecode test #2: %s\n", ok?"pass":"fail" );
    printf( "PgnOut():\n%s\n", txt_out.c_str() );
    printf( "should match in:\n%s\n", complex_example.c_str() );
    ok = (txt_out == complex_example);
    printf( "Bytecode test #3:\n%s\n", ok?"pass":"fail" );
    std::string stepper_pgn_out;
    simple_expansion_test(bc, stepper_pgn_out );
    stepper_pgn_out += " 0-1\n";
    extern void white_space_word_wrap( std::string &s );
    white_space_word_wrap( stepper_pgn_out );
    printf( "Stepper PGN out:\n%s\n", stepper_pgn_out.c_str() );
    ok = (txt_out == stepper_pgn_out);
    printf( "Bytecode test #4:\n%s\n", ok?"pass":"fail" );
    gt.bytecode = bc;
    std::string txt_view = stepper_txt_view(gt.bytecode);
    std::ofstream fout("test_out.txt");
    printf( "txt_view test output in test_out.txt\n" );
    fout.write( txt_view.c_str(), txt_view.length() );
    std::string txt_view2;
    press.Reset();
    press.IterateOver( gt.bytecode, &txt_view2, it_txt_view );
    std::ofstream fout2("test_out2.txt");
    printf( "txt_view2 test output in test_out2.txt\n" );
    fout2.write( txt_view2.c_str(), txt_view2.length() );
    ok = (txt_view == txt_view2);
    printf( "Bytecode test #5: %s\n", ok?"pass":"fail" );
#if 0

    // Delete the rest of variation at current offset
    ok = gt.DeleteRestOfVariation();
    printf( "GameTree::DeleteRestOfVariation() returns ok=%s\n", ok?"true":"false" );

    // Delete variation at current offset
    ok = gt.DeleteVariation();
    printf( "GameTree::DeleteRestOfVariation() returns ok=%s\n", ok?"true":"false" );

    // Find parent of variation, -1 if none
    int gt.Parent( int offset );

    // Get MovePlus at given offset
    MovePlus gt.GetMovePlus() { return GetMovePlus(offset); }
    MovePlus gt.GetMovePlus( int offset_parm );

    // Promote a variation at given offset
    //  Return offset of promoted variation
    int gt.Promote( int offset_parm );

    // Demote a variation at given offset
    //  Return offset of promoted variation
    int gt.Demote( int offset_parm  );

    // Delete the rest of a variation
    void gt.DeleteRestOfVariation( int offset_parm  );

    // Delete variation
    void gt.DeleteVariation( int offset_parm  );

    // Get main variation
    std::vector<thc::Move> gt.GetMainVariation(); 

    // Set non zero start position
    void gt.SetNonZeroStartPosition( int main_line_idx );

    // Is empty except for one comment ?
    bool gt.IsJustAComment();

    // Is empty ?
    bool gt.IsEmpty();

    // Insert a move at current location
    // Return true if move played okay
    bool gt.InsertMove( GAME_MOVE game_move, bool allow_overwrite );

    // Is the current move the last in the main line (or is the main line empty)
    bool gt.AtEndOfMainLine();

    // Is the current move in the main line?
    bool gt.AreWeInMain();
#endif
    return 0;
}


void func()
{
    GameTree gt;
    //GameTree( std::string &bytecode_ ) { Init(bytecode_); }
    //GameTree( thc::ChessPosition &start_position ) { Init(start_position); }
    //GameTree( thc::ChessPosition &start_position, std::string &bytecode_ ) { Init(start_position,bytecode_); }
    thc::ChessPosition start_position;
    std::string bytecode;
    gt.Init( start_position );
    gt.Init( start_position, bytecode );
    gt.Init( bytecode );

    // Load from a start position plus a list of moves
    std::vector<thc::Move> moves;
    //gt.Init( start_position, moves );
    gt.Init( moves );

    // Promote a variation at current offset
    bool ok = gt.Promote();

    // Demote a variation at current offset
    ok = gt.Demote();

    // Find parent of variation, -1 if none
    int offset = 0;
    int offset2 = gt.Parent( offset );

    // Get MovePlus at given offset
    MovePlus mp  = gt.GetMovePlus();
    MovePlus mp2 = gt.GetMovePlus( offset2 );

    // Promote a variation at given offset
    //  Return offset of promoted variation
    int offset3 = gt.Promote( offset2 );

    // Demote a variation at given offset
    //  Return offset of promoted variation
    offset3 = gt.Demote( offset2 );

    // Delete the rest of a variation
    gt.DeleteRestOfVariation( offset2 );

    // Delete variation
    gt.DeleteVariation( offset2  );

#if 0
    // Get main variation
    std::vector<thc::Move> GetMainVariation(); 

    // Set non zero start position
    void SetNonZeroStartPosition( int main_line_idx );

    // Is empty except for one comment ?
    bool IsJustAComment();

    // Is empty ?
    bool IsEmpty();

    // Insert a move at current location
    // Return true if move played okay
    bool InsertMove( GAME_MOVE game_move, bool allow_overwrite );

    // Is the current move the last in the main line (or is the main line empty)
    bool AtEndOfMainLine();

    // Is the current move in the main line?
    bool AreWeInMain();
#endif
}

bool it_txt_view(void *link, const std::string& bc, size_t offset, Codepoint &cpt)
{
    // Truncate byte by byte comment analysis
    if( cpt.ct == ct_comment_txt && cpt.comment_offset>1  )
        return false;
    std::string *ps = (std::string *)link;
    std::string &s = *ps;
    GameTree gt;
    gt.bytecode = bc;
    gt.offset   = offset;
    Summary summary;
    gt.GetSummary(summary);
    thc::ChessRules cr = summary.start_position;
    s += codepoint_type_txt(cpt.ct);
    s += " ";
    if( cpt.is_move )
    {
        s += util::sprintf( "%u", cpt.cr->full_move_count );
        s += cpt.cr->white? ". " : "... ";
        s += cpt.san_move;
    }
    else if( cpt.ct == ct_comment_txt )
    {
        s += util::sprintf( "'%c'", (uint8_t)cpt.raw );
    }
    else if( cpt.ct == ct_comment_end )
    {
        s += util::sprintf( "\"%s\"", cpt.comment_txt.c_str() );
    }
    else
    {
        s += util::sprintf( "Raw code: %02x", (uint8_t)cpt.raw );
    }
    s += util::sprintf( " depth=%u", cpt.depth );
    s += " [";
    size_t i=0;
    if( i == (summary.move_idx - summary.variation_idx) )
        s +=  "variation_start_ptr-> ";
    if( i == summary.move_idx )
        s +=  "ptr-> ";
    for( i=0; i<summary.moves.size(); i++ )
    {
        thc::Move mv = summary.moves[i];
        if( cr.white || i==0 )
        {
            s += util::sprintf( "%d", cr.full_move_count );
            if( cr.white )
                s += ".";
            else
                s += "...";
        }
        s += util::sprintf( "%s", mv.NaturalOut(&cr).c_str() );
        if( i+1 == (summary.move_idx - summary.variation_idx) )
            s += " variation_ptr->";
        if( i+1 == summary.move_idx )
            s += " ptr->";
        if( i+1 < summary.moves.size() )
            s += " ";
        cr.PlayMove(mv);
    }
    s += "]\n";
    s += util::sprintf( "description: %s\n",         summary.description.c_str()    );
    s += util::sprintf( "pre_comment: %s\n",         summary.pre_comment.c_str()    );
    s += util::sprintf( "comment:     %s\n",         summary.comment.c_str()        );
    s += util::sprintf( "empty:       %s\n",         summary.empty ? "true" : "false" );
    s += util::sprintf( "at_move0:    %s\n",         summary.at_move0 ? "true" : "false" );
    s += util::sprintf( "at_end_of_variation: %s\n", summary.at_end_of_variation ? "true" : "false" );
    s += util::sprintf( "in_comment:  %s\n",         summary.in_comment ? "true" : "false" );
    s += util::sprintf( "move_offset: %d\n",         summary.move_offset            );
    s += util::sprintf( "depth:       %d\n",         summary.depth                  );
    s += util::sprintf( "nag1:        %d\n",         summary.nag1                   );
    s += util::sprintf( "nag2:        %d\n",         summary.nag2                   );
    s += "\n";
    return false;
}


std::string stepper_txt_view(const std::string& bc)
{
    std::string s;
    Stepper it(bc);
    while( !it.end )
    {
        GameTree gt;
        gt.bytecode = bc;
        gt.offset   = it.idx;
        Summary summary;
        gt.GetSummary(summary);
        thc::ChessRules cr = summary.start_position;
        s += codepoint_type_txt(it.ct);
        s += " ";
        if( it.is_move )
        {
            s += util::sprintf( "%u", it.cr.full_move_count );
            s += it.cr.white? ". " : "... ";
            s += it.san_move;
        }
        else if( it.ct == ct_comment_txt )
        {
            s += util::sprintf( "'%c'", (uint8_t)it.raw );
        }
        else if( it.ct == ct_comment_end )
        {
            s += util::sprintf( "\"%s\"", it.comment_txt.c_str() );
        }
        else
        {
            s += util::sprintf( "Raw code: %02x", (uint8_t)it.raw );
        }
        s += util::sprintf( " depth=%u", it.depth );
        s += " [";
        size_t i=0;
        if( i == (summary.move_idx - summary.variation_idx) )
            s +=  "variation_start_ptr-> ";
        if( i == summary.move_idx )
            s +=  "ptr-> ";
        for( i=0; i<summary.moves.size(); i++ )
        {
            thc::Move mv = summary.moves[i];
            if( cr.white || i==0 )
            {
                s += util::sprintf( "%d", cr.full_move_count );
                if( cr.white )
                    s += ".";
                else
                    s += "...";
            }
            s += util::sprintf( "%s", mv.NaturalOut(&cr).c_str() );
            if( i+1 == (summary.move_idx - summary.variation_idx) )
                s += " variation_ptr->";
            if( i+1 == summary.move_idx )
                s += " ptr->";
            if( i+1 < summary.moves.size() )
                s += " ";
            cr.PlayMove(mv);
        }
        s += "]\n";
        s += util::sprintf( "description: %s\n",         summary.description.c_str()    );
        s += util::sprintf( "pre_comment: %s\n",         summary.pre_comment.c_str()    );
        s += util::sprintf( "comment:     %s\n",         summary.comment.c_str()        );
        s += util::sprintf( "empty:       %s\n",         summary.empty ? "true" : "false" );
        s += util::sprintf( "at_move0:    %s\n",         summary.at_move0 ? "true" : "false" );
        s += util::sprintf( "at_end_of_variation: %s\n", summary.at_end_of_variation ? "true" : "false" );
        s += util::sprintf( "in_comment:  %s\n",         summary.in_comment ? "true" : "false" );
        s += util::sprintf( "move_offset: %d\n",         summary.move_offset            );
        s += util::sprintf( "depth:       %d\n",         summary.depth                  );
        s += util::sprintf( "nag1:        %d\n",         summary.nag1                   );
        s += util::sprintf( "nag2:        %d\n",         summary.nag2                   );
        s += "\n";
        it.Next();
    }
    return s;
}

bool it_simple_expansion_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt)
{
    std::string s;
    if( cpt.is_move )
    {
        if( cpt.cr->white || cpt.move_nbr_needed )
            s += util::sprintf( "%u%s ", cpt.cr->full_move_count, cpt.cr->white ? "." : "..." );
        s += cpt.san_move;
    }
    else if( cpt.ct == ct_comment_end )
    {
        s += util::sprintf( "{%s}", cpt.comment_txt.c_str() );
    }
    else if( cpt.ct == ct_variation_start )
    {
        s += "(";
    }
    else if( cpt.ct == ct_variation_end )
    {
        s += ")";
    }
    if( s.length() > 0 )
    {
        std::string *ps = (std::string *)link;
        std::string &t = *ps;
        if( t.length() > 0 )
            t += " ";
        t += s;
    }
    return false;
}

void simple_expansion_test(const std::string& bc, std::string &s )
{
    s.clear();
    Stepper it(bc);
    while( !it.end )
    {
        if( it.is_move )
        {
            if( it.stk->variation_move_count > 1 )
                s += " ";
            if( it.move_nbr_needed )
                s += util::sprintf( "%u%s ", it.cr.full_move_count, it.cr.white ? "." : "..." );
            s += it.san_move;
        }
        else if( it.ct == ct_comment_end )
        {
            s += util::sprintf( " {%s}", it.comment_txt.c_str() );
        }
        else if( it.ct == ct_variation_start )
        {
            s += " (";
        }
        else if( it.ct == ct_variation_end )
        {
            s += ")";
        }
        else if( it.ct == ct_escape_code )
        {
            s += util::sprintf( " $%u", it.raw-8 );
        }
        it.Next();
    }
    return;
}

bool it_find_first_variation( const std::string& bc, int &offset )
{
    offset = 0;
    Stepper it(bc);
    while( !it.end )
    {
        if( it.depth > 0 )
        {
            offset = it.idx-1;
            return true;
        }
        it.Next();
    }
    return false;
}

// Find start of first variation using Stepper
bool find_first_variation( const std::string &bc, int &offset )
{
    offset = 0;
    Stepper2 it(bc);
    while( !it.end )
    {
        if( it.depth > 0 )
        {
            offset = it.idx-1;
            return true;
        }
        it.Next();
    }
    return false;
}
