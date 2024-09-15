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

static void move_entry_testing();
int  game_tree_test();
void stepper_gen_pgn_unjustified(const std::string& bc, std::string &s, const std::string& result );
bool find_first_variation( const std::string &bc, int &offset );

// Find start of first variation using Stepper
bool stepper_find_first_variation( const std::string& bc, int &offset );

// Find start of first variation
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

static int total_passes;
static int total_fails;

static void results_count( bool ok )
{
    if( ok )
        total_passes++;
    else
        total_fails++;
}

static void results_print()
{
    printf( "Total passes %d\n", total_passes );
    printf( "Total fails %d\n", total_fails );
}

int main()
{
    extern void compress_temp_lookup_gen_function();
    compress_temp_lookup_gen_function();
    thc::ChessRules cr;

    // Test internals, for porting to new environments etc
    bool ok = cr.TestInternals();
    results_count( ok );
    printf( "TestInternals() returns %s\n", ok?"Success":"Fail" );
    game_tree_test();
    move_entry_testing();
    results_print();
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

 /*
   Small (simple) PGN testing
  */

    // Load from PGN text
    std::string txt("1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6 {The exchange variation} ( {Alternatively} 4. Ba4 Nf6 5. O-O {is the main line} ) 4... dxc6");
    gt.PgnParse( txt );

    // Goto first variation
    int offset1, offset2;
    bool found1 = stepper_find_first_variation( gt.bytecode, offset1 );
    bool found2 = find_first_variation( gt.bytecode, offset2 );
    printf( "found1=%s, found2=%s, offset1=%u, offset2=%u\n",
             found1?"true":"false", found2?"true":"false", offset1, offset2 );
    bool ok = (found1==found2 && offset1==offset2);
    printf( "find_first_variation() %s\n", ok?"pass":"fail" );
    results_count(ok);

    // Promote a variation at current offset
    ok = gt.Promote();
    printf( "GameTree::Promote() returns ok=%s\n", ok?"true":"false" );
    //results_count(ok);

    // Demote a variation at current offset
    ok = gt.Demote();
    printf( "GameTree::Demote() returns ok=%s\n", ok?"true":"false" );
    //results_count(ok);

    // Bytecode type is intended to be increasingly the heart of Tarrasch
    Bytecode press;
    std::string simple_in("1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Bxc6 {The exchange variation} ({Alternatively} 4. Ba4 Nf6 5. O-O {is the main line}) 4... dxc6");
    std::string bc = press.PgnParse(simple_in);
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
    results_count(ok);

    // Optimised PGN directly from Bytecode
    press.Reset();
    std::string pgn_direct = press.PgnOut( bc, "*" );
    printf("direct:\n%s\n",pgn_direct.c_str());

    // PGN from Stepper
    press.Reset();
    std::string pgn_stepper;
    stepper_gen_pgn_unjustified( bc, pgn_stepper, "*" );
    printf("stepper:\n%s\n",pgn_stepper.c_str());

    // Compare
    std::string s0 = white_space_normalise(simple_in+" *");
    std::string s1 = white_space_normalise(pgn_direct);
    std::string s2 = white_space_normalise(pgn_stepper);
    printf( "s0:\n%s\n", s0.c_str() );
    printf( "s1:\n%s\n", s1.c_str() );
    bool ok_simple_pgn_test = (s0==s1);
    bool ok_simple_direct_vs_stepper_test = (s1==s2);
    printf( "Simple pgn test %s\n", ok_simple_pgn_test?"pass":"fail" );
    printf( "Simple direct vs stepper pgn test %s\n", ok_simple_direct_vs_stepper_test?"pass":"fail" );
    results_count( ok_simple_pgn_test );
    results_count( ok_simple_direct_vs_stepper_test );

 /*
   Large (complicated) PGN testing
  */

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

    // Demo OutlineOut(), basically a debug device
    press.Reset();
    bc = press.PgnParse(complex_example);
    std::string outline_out = press.OutlineOut( bc, "0-1" );
    printf( "Complex outline out:\n%s\n", outline_out.c_str() );

    // Optimised PGN directly from Bytecode
    press.Reset();
    pgn_direct = press.PgnOut( bc, "0-1" );
    printf("direct:\n%s\n",pgn_direct.c_str());

    // PGN from Stepper
    press.Reset();
    stepper_gen_pgn_unjustified( bc, pgn_stepper, "0-1" );
    printf("stepper:\n%s\n",pgn_stepper.c_str());

    // Compare
    s0 = white_space_normalise(complex_example);
    s1 = white_space_normalise(pgn_direct);
    s2 = white_space_normalise(pgn_stepper);
    printf( "s0:\n%s\n", s0.c_str() );
    printf( "s1:\n%s\n", s1.c_str() );
    bool ok_complex_pgn_test = (s0==s1);
    bool ok_complex_direct_vs_stepper_test = (s1==s2);
    printf( "Complex pgn test %s\n", ok_complex_pgn_test?"pass":"fail" );
    printf( "Complex direct vs stepper pgn test %s\n", ok_complex_direct_vs_stepper_test?"pass":"fail" );
    results_count( ok_complex_pgn_test );
    results_count( ok_complex_direct_vs_stepper_test );

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

void stepper_gen_pgn_unjustified(const std::string& bc, std::string &s, const std::string& result )
{
    std::string space;
    bool move_nbr_needed = true;
    s.clear();
    for( Stepper it(bc); !it.End(); it.Next() )
    {
        switch( it.ct )
        {
            case ct_move:
            {
                s += space;
                if( move_nbr_needed )
                    s += util::sprintf( "%u%s ", it.cr.full_move_count, it.cr.white ? "." : "..." );
                s += it.san_move;
                space= " ";
                move_nbr_needed = !it.cr.white;
                break;
            }
            case ct_comment_end:
            {
                s += space;
                s += util::sprintf( "{%s}", it.comment_txt.c_str() );
                space= " ";
                move_nbr_needed = true;
                break;
            }
            case ct_variation_start:
            {
                s += space;
                s += "(";
                space= "";
                move_nbr_needed = true;
                break;
            }
            case ct_variation_end:
            {
                s += ")";
                space= " ";
                move_nbr_needed = true;
                break;
            }
            case ct_escape_code:
            {
                s += space;
                s += util::sprintf( "$%u", it.raw-8 );
                space= " ";
                break;
            }
        }
    }
    s += space;
    s += result;
}

// Find start of first variation using Stepper
bool stepper_find_first_variation( const std::string& bc, int &offset )
{
    offset = 0;
    for( Stepper it(bc); !it.End(); it.Next() )
    {
        if( it.depth > 0 )
        {
            offset = it.idx;
            return true;
        }
    }
    return false;
}

// Find start of first variation
bool find_first_variation( const std::string &bc, int &offset )
{
    offset = 0;
    for( char c: bc )
    {
        offset++;
        if( c == BC_VARIATION_START )
            return true;
    }
    return false;
}


static void move_entry_testing()
{
    GameTree gt;

    // Load from PGN text
    std::string txt("1. e4 e5 2. Nf3 Nc6");
    gt.PgnParse( txt );
    printf( "Moves=%s, Bytecode as hex:", txt.c_str() );
    int len = (int)gt.bytecode.length();
    for( int i=0; i<len; i++ )
    {
        printf( " %02x", (uint8_t)gt.bytecode[i] );
    }
    printf( "\n" );
    int original_offset = gt.offset;
    printf( "Summary at all possible offsets\n");
    Summary summary;
    for( int offset=0; offset<len+1; offset++ )
    {
        gt.offset = offset;
        printf( "offset=%d%s\n", offset, offset==original_offset ? " (Original Offset) ":"" );
        printf( "--------\n");
        gt.GetSummary(summary);
        std::string s = summary.ToDebugStr();
        printf( "%s\n", s.c_str() );
    }
}

