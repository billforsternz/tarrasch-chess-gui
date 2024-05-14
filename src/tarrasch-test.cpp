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
bool it_summary_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt);
bool it_simple_expansion_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt);
void simple_expansion_test(const std::string& bc, std::string &s );
bool it_find_first_variation( void *offset_of_first_variation, const std::string& bc, size_t offset, Codepoint &cpt );
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
    thc::ChessRules cr;

    // Test internals, for porting to new environments etc
    bool ok = cr.TestInternals();
    printf( "TestInternals() returns %s\n", ok?"Success":"Fail" );

    game_tree_test();
    return 0;
}

int game_tree_test()
{
    GameTree gt;
    std::vector<thc::Move> moves;
    thc::ChessPosition cp;

    // Load from PGN text
    std::string txt2("1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6 {The exchange variation} ( {Alternatively} 4. Ba4 Nf6 5. O-O {is the main line} ) 4... dxc6");
    gt.PgnParse( txt2 );

    // Goto first variation
    int offset1, offset2;
    gt.IterateOver( &offset1, it_find_first_variation );
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
    Bytecode bc;
    std::string txt_in("1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6 {The exchange variation} ( {Alternatively} 4. Ba4 Nf6 5. O-O {is the main line} ) 4... dxc6");
    std::string bytecode = bc.PgnParse(txt_in);
    thc::ChessRules cr;
    std::string t = bc.cr.ToDebugStr();
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

    std::string rough_out, rough_out2;
    Bytecode bc2;   // bc.Init() doesn't work, need a new Bytecode, fix this!
    bc2.IterateOver( bytecode, &rough_out, it_simple_expansion_test );
    printf( "Rough dump: %s\n", rough_out.c_str() );
    simple_expansion_test(bytecode, rough_out2 );
    printf( "Rough dump 2: %s\n", rough_out2.c_str() );


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

"1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. f3 d5 5. a3 Bxc3+ 6. bxc3 dxc4 7. e4 e5 $2 8.\r\n"
"Bxc4 Be6 9. Bxe6 fxe6 10. Qb3 O-O 11. Qxe6+ Kh8 12. Qxe5 {Objectively the\r\n"
"opening has been a catastrophe and Black is quite lost, but he does have a lead\r\n"
"in development...} 12... Nc6 13. Qg3 $2 {Diagram #Just one mistake is\r\n"
"sufficient for a turnaround. This is not the best square and Black has a lot of\r\n"
"fun for the rest of the game. Both White's proud extra centre pawns look\r\n"
"securely defended but Black gives up both knights for them to tear the position\r\n"
"open.} 13... Nxd4 $1 14. cxd4 Qxd4 15. Ra2 Qc4 {Fork!} 16. Ne2 Qxa2 17. Qxc7 $2\r\n"
"{Inviting catastrophe}  (17. Qe5 {returns the Queen from exile and White can\r\n"
"try to grovel an exchange down} ) 17... Rac8 18. Qxb7 {Diagram #} 18... Nxe4 $1\r\n"
"19. fxe4 Rxc1+ $1 20. Nxc1 Qxg2 $1 {Leaving White completely helpless} 21. Nd3\r\n"
"Qxh1+ 22. Kd2 Qxh2+ 23. Kc3 Qb8 24. Qe7 Re8 25. Qh4 Qc7+ 26. Kd2 Qa5+ 27. Ke2\r\n"
"Qxa3 28. e5 Qa6 29. Kd2 Qh6+ $1 {A nice practical decision. White does not want\r\n"
"to be accused of premature resignation. For anxious players like me in such\r\n"
"circumstances it's a good idea to permanently prevent any possible queen plus\r\n"
"knight shenanigans. A knight hates rook pawns, and here it faces three of them!\r\n"
"Even so as a nervous finisher I wanted to give up my rook for the knight and\r\n"
"eliminate the possibility of forks as well, but White never gave me a chance.}\r\n"
"30. Qxh6 gxh6 31. Ke3 Kg7 32. Ke4 Kg6 33. Nc5 h5 34. Kf4 h4 35. e6 Kf6 36. Kg4\r\n"
"Rxe6 37. Kxh4 Ke5 38. Kg4 Rg6+ 39. Kf3 Kd4 40. Nb3+ Kc3 41. Nc5 a5 42. Ke2 Kc4\r\n"
"43. Na4 Kb4 44. Nb2 Rg2+ 0-1\r\n";

    Bytecode bc2a;
    bytecode = bc2a.PgnParse(complex_example);
    Bytecode bc2b;
    std::string outline_out = bc2b.OutlineOut( bytecode, "0-1" );
    printf( "Complex outline out:\n%s\n", outline_out.c_str() );
    std::string rough_out3;
    Bytecode bc2c;
    std::string pgn_out3 = bc2c.PgnOut( bytecode, "0-1" );
    printf( "Complex pgn out: %s\n", pgn_out3.c_str() );
    Bytecode bc2d;
    bc2d.IterateOver( bytecode, &rough_out3, it_simple_expansion_test );
    printf( "Complex dump 1: %s\n", rough_out3.c_str() );
    simple_expansion_test(bytecode, rough_out3 );
    printf( "Complex dump 2: %s\n", rough_out3.c_str() );

    Bytecode bc3;
    std::string txt_out = bc3.PgnOut( bytecode, "*" );
    printf( "Refined dump: %s\n", txt_out.c_str() );
    ok = ((txt_in+" *\n") == txt_out);
    printf( "Bytecode test #2: %s\n", ok?"pass":"fail" );
    Bytecode bc4;
    gt.bytecode = bytecode;
    std::string summary_output;
    bc4.IterateOver( gt.bytecode, &summary_output, it_summary_test );
    std::ofstream fout("test_out.txt");
    printf( "GetSummary test output in test_out.txt\n" );
    fout.write( summary_output.c_str(), summary_output.length() );
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

bool it_summary_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt)
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
        it.Next();
        if( it.is_move )
        {
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
    }
    return;
}

bool it_find_first_variation( void *offset_of_first_variation, const std::string& bc, size_t offset, Codepoint &cpt )
{
    if( cpt.depth > 0 )
    {
        *(static_cast<int *>(offset_of_first_variation)) = offset;
        return true;
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
    
