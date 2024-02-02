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

int  game_tree_test();
void summary_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt);
void simple_expansion_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt);

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

    // Load from a start position plus a list of moves
    // gt.Init( cp, moves );
    gt.Init( moves );

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

    std::string rough_out;
    Bytecode bc2;   // bc.Init() doesn't work, need a new Bytecode, fix this!
    bc2.IterateOver( bytecode, &rough_out, simple_expansion_test );
    printf( "Rough dump: %s\n", rough_out.c_str() );
    Bytecode bc3;
    std::string txt_out = bc3.PgnOut( bytecode, "*" );
    printf( "Refined dump: %s\n", txt_out.c_str() );
    ok = ((txt_in+" *\n") == txt_out);
    printf( "Bytecode test #2: %s\n", ok?"pass":"fail" );
    Bytecode bc4;
    gt.bytecode = bytecode;
    std::string summary_output;
    bc4.IterateOver( gt.bytecode, &summary_output, summary_test );
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

void summary_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt)
{
    // Truncate byte by byte comment analysis
    if( cpt.ct == ct_comment_txt && cpt.comment_offset>1  )
        return;
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
}

void simple_expansion_test(void *link, const std::string& bc, size_t offset, Codepoint &cpt)
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
}
