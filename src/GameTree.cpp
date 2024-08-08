/****************************************************************************
 * Representation of a game's tree of variations
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include "thc.h"
#include "DebugPrintf.h"
#include "Bytecode.h"
#include "Stepper.h"
#include "GameTree.h"

//
// In branch "new-heart" I am starting out on an ambitious plan to transplant
//  a new heart into Tarrasch - based on bytecode. The byte code will be the
//  same one byte per move representation developed to implement the database
//  features (class CompressMoves) enhanced with techniques to represent
//  variations, annotations, comments (and more?). This should be more flexible
//  maintainable, understandable and improveable than the current MoveTree
//  recursive vector representation.
//
//  I am going to try to keep the code compiling and linking, but it won't
//  necessarily be functional for a while.
//

/*
    Design notes: How do we represent a Game Tree with bytecode ?

    Bytecode = move codes [0x08-0xff inclusive] + 7 extra codes
     1 variation start
     2 variation end
     3 comment start (comment is text Ansi or UTF-8 for further study)
     4 comment end
     5 meta data start (multi byte meta data)
     6 meta data end
     7 escape (single byte meta data)
    (0 is reserved and isn't used)

    Each of variation, comment and meta data is a multi-byte sequence.
    Escape introduces a two byte sequence (escape plus one code byte)
    The codes 0-7 are *sacrosanct*, no coded data including comment
    text, meta data and the escape code can use codes 0-7.

    Variations, comments and meta data allow arbitrary nesting, including
    (say) comments within variations OR variations within comments (all
    combinations, even if we don't use them because they're not useful)

    Nesting algorithm is as follows;

    000000111111122222222112222223333222111222211100000
          S      S      E  S     S  E  E   S  E  E       <- start / end
          1      2      1  2     3  2  1   2  1  0       <- level after start/end

    To skip over arbitrarily nested data simply increment a level count
    at each S and decrement it on each E. When the level count returns
    to its original value, then the nested data is over, and the
    original data stream continues. The sacrosanct property of the
    start/end codes (all three types) means you don't have to know or
    care how any nested data is coded to skip over it.

    Promotion, demotion, and navigation should all be easier to
    implement and reason about than in Tarrasch's original MoveTree
    recursive vector

    For example:

    Promote variation;

    Before: (ghijklmn descends from abcdef)

          abcdefghijklmnopqrst
    0000001111112222222211111100000000
                ^
                |
                Promote this variation

    After:  (ghijklmn continues from abcdef)

          abcdefopqrstghijklmn
    0000001111112222221111111100000000

    It's just a string swap!

*/

//
//   Utilities
//

// Create a readable view of a bytecode string
std::string dbg_outline( const std::string &bc );

// Extract the mainline from a bytecode string
std::string mainline( const std::string &bc );


// Get MovePlus at given offset TODO
MovePlus GameTree::GetMovePlus( int offset_parm )
{
    MovePlus ret;
    return ret;
}

// Promote a variation at given offset
//  Return offset of promoted variation
int GameTree::Promote( int offset_parm )
{
/*
    Promote variation;

    Before: (ghijklmn descends from abcdef)

          abcdefghijklmnopqrst
    0000001111112222222211111100000000
                ^
                |
                Promote this variation

    After:  (ghijklmn continues from abcdef)

          abcdefopqrstghijklmn
    0000001111112222221111111100000000

    It's just a string swap!

    Before: (ghi and jklmn are alternative variations)

          abcdef ghi  jklmn opqrst
    000000111111[222][22222]11111100000000
                      ^
                      |
                      Promote this variation

    After:

          abcdef ghi  opqrst jklmn
    000000111111[222][222222]1111100000000

    Still a string swap!

    */

    return 0;
}

// Demote a variation at given offset
//  Return offset of promoted variation
int GameTree::Demote( int offset_parm  )
{
    return 0;
}

// Delete the rest of a variation
void GameTree::DeleteRestOfVariation( int offset_parm  )
{
}

// Delete variation
void GameTree::DeleteVariation( int offset_parm  )
{
}


// Find parent of current variation, -1 if none
int GameTree::Parent( int offset_parm )
{
    return -1;
}

// Insert a move at current location
// Return true if move played okay
bool GameTree::InsertMove( GAME_MOVE game_move, bool allow_overwrite )
{
    //$ TODO support all GAME_MOVE fields, perhaps using meta data
    // bool move_played = false;
    Summary summary;
    GetSummary( summary );
    Bytecode bc(summary.pos);
    uint8_t c = bc.CompressMove(game_move.move);
    size_t len = summary.moves.size();
    if( summary.move_idx == (int)len )
    {
        bytecode.insert(summary.move_offset,1,(char)c);
        offset = summary.move_offset+1;
    }
    else
    {
        std::string new_variation;
        new_variation += (char)BC_VARIATION_START;
        new_variation += (char)c;
        new_variation += (char)BC_VARIATION_END;
        bytecode.insert(summary.move_offset,new_variation);
        offset = summary.move_offset+2;
    }
    return true;
}

// Load from a start position plus a list of moves
void GameTree::Init( const thc::ChessPosition &start_position_, const std::vector<thc::Move> &moves )
{
    Bytecode press;
    start_position = start_position_;
    press.Init(start_position);
    bytecode = press.Compress(moves);
}

void GameTree::Init( const std::vector<thc::Move> &moves )
{
    Bytecode press;
    start_position.Init();
    press.Init();
    bytecode = press.Compress(moves);
}


bool GameTree::Promote(void)
{
    return false;
}

bool  GameTree::Demote(void)
{
    return false;
}

std::vector<thc::Move>  GameTree::GetMainVariation(void)
{
    std::vector<thc::Move> dummy;
    return dummy;
}

void GameTree::SetNonZeroStartPosition(int)
{
}

bool  GameTree::IsEmpty(void)
{
    return false;
}

bool  GameTree::AtEndOfMainLine(void)
{
    return false;
}

bool  GameTree::AreWeInMain(void)
{
    return false;
}

/*
// For GetSummary(), find the move corresponding to the current GameTree offset and get details as follows;
struct Summary
{
    thc::ChessPosition start_position;      // If we start at this position
    int move_idx;                           // and play this many moves
    std::vector<thc::Move> moves;           // of this sequence, we will get to the current position (there may be more moves if the variation continues)
    std::string description;                // eg "Position after 23. f4"
    std::string pre_comment;                // comment before 23. f4 if it is the start of a variation, of follows a variation
    std::string comment;                    // comment after 23. f4
    bool empty;                             // There aren't any moves
    bool at_move0;                          // Before any moves in this variation
    bool at_end_of_variation;               // After last move in this variation
    bool in_comment;                        // In a comment
    int  move_offset;                       // May be different to GameTree offset (eg if in_comment is true)
    int  depth;                             // depth = 0 is main line
    int  variation_offset;                  // If depth > 1 will not necessarily equal moves.length()
    int  nag1;                              // annotation type 1 (!, !!, ? etc)
    int  nag2;                              // annotation type 2 (+-, += etc)
};

  */
    // Get a summary of the current situation
void GameTree::GetSummary( Summary &summary )
{
    //$ TODO, replace with Stepper based version
#if 0
    static int dbg_count = 0;
    printf( "GetSummary() %d: In\n", ++dbg_count );
    printf( "Outline = %s\n", dbg_outline(bytecode).c_str() ); 
    std::string mainline_outline = dbg_outline(mainline(bytecode));
    printf( "Mainline outline  = %s\n", mainline_outline.c_str() );
    if( dbg_count == 16 )
        ; //printf( "Debug\n" );
#endif
#define MAX_DEPTH 30
    int most_recent_offset[MAX_DEPTH+1];
    most_recent_offset[0] = 0;
    bool escape = false;
    size_t len = bytecode.length();
    std::string moves;
    int last_move_offset = 0;
    struct STACK_ELEMENT
    {
        int variation_move_count = 0;
        int most_recent_offset = 0;
    };
    STACK_ELEMENT stk_array[MAX_DEPTH+1];
    STACK_ELEMENT *stk = stk_array;
    int stk_idx = 0;

    // Pass 1, calculate where variations start at each depth
    size_t idx=0;
    for( idx=0; idx<(size_t)offset && idx<len; idx++ )
    {
        if( escape )
        {
            escape = false;
            continue;
        }
        uint8_t c = (uint8_t)bytecode[idx];
        if( c == BC_ESCAPE )
        {
            escape = true;
        }
        else if( c == BC_VARIATION_START )
        {
            if( stk_idx < MAX_DEPTH )
            {
                stk_idx++;
                stk = &stk_array[stk_idx];
                stk->most_recent_offset = idx;
            }
        }
        else if( c == BC_VARIATION_END )
        {
            if( stk_idx > 0 )
            {
                stk_idx--;
                stk = &stk_array[stk_idx];
            }
        }
    }
    int target_depth = stk_idx;

    // Pass 2, build sequence of moves from start of game to offset
    bool collect = true;
    bool just_finish_variation = false;
    stk_idx = 0;
    int in_meta=0, in_comment=0;
    escape = false;
    for( idx=0; idx<len+1; idx++ )  // note len+1 in case offset == len, see below *
    {
        if( (int)idx == offset )
        {
            summary.move_idx             = moves.length();
            summary.variation_move_count = stk->variation_move_count;
            summary.at_move0             = (stk->variation_move_count == 0);
            summary.move_offset          = last_move_offset;
            just_finish_variation        = true; // keep going, until the end of the variation
        }
        if( idx >= len )            // * if idx<len stop
            break;
        if( escape )
        {
            escape = false;
            continue;
        }
        uint8_t c = (uint8_t)bytecode[idx];
        if( c == BC_ESCAPE )
        {
            escape = true;
        }
        else if( c == BC_VARIATION_START )
        {
            if( stk_idx < MAX_DEPTH )
            {
                stk_idx++;
                stk = &stk_array[stk_idx];
                stk->variation_move_count = 0;
                collect = (stk_idx<=target_depth && ((int)idx)>=stk->most_recent_offset );
            }
        }
        else if( c == BC_VARIATION_END )
        {
            if( stk_idx > 0 )
            {
                stk_idx--;
                stk = &stk_array[stk_idx];
                collect = (stk_idx<=target_depth && ((int)idx)>=stk->most_recent_offset );
                if( just_finish_variation && stk_idx<target_depth )
                    break;
            }
        }
        else if( c == BC_COMMENT_START )
        {
            in_comment++;
        }
        else if( c == BC_COMMENT_END )
        {
            in_comment--;
        }
        else if( c == BC_META_START )
        {
            in_meta++;
        }
        else if( c == BC_META_END )
        {
            in_meta--;
        }
        else if( c>=BC_MOVE_CODES && in_comment==0 && in_meta==0 && collect )
        {
            last_move_offset = idx;
            moves += c;
            stk->variation_move_count++;
        }
    }
#if 0
    std::string variation_outline = dbg_outline(moves);
    printf( "Variation outline = %s\n", variation_outline.c_str() ); 
    if( mainline_outline != variation_outline )
        printf( "Debug 2\n" );
#endif
    Bytecode bc;
    summary.start_position = start_position;
    thc::ChessRules cr = start_position;
    summary.moves = bc.Uncompress( start_position, moves );
    for( size_t i=0; i<(int)summary.move_idx; i++ )
        cr.PlayMove(summary.moves[i]);
    summary.pos = cr;
}

// Create a readable view of a bytecode string
std::string dbg_outline( const std::string &bc )
{
    std::string s;
    bool escape = false;
    int  in_comment=0, in_meta=0, comment_len=0;
    for( unsigned char c: bc )
    {
        if( escape )
        {
            s += '-';
        }
        else if( c == BC_ESCAPE )
        {
            s += '!';
        }
        else if( c == BC_VARIATION_START )
        {
            s += '(';
        }
        else if( c == BC_VARIATION_END )
        {
            s += ')';
        }
        else if( c == BC_COMMENT_START )
        {
            s += '"';
            comment_len = 0;
            in_comment++;
        }
        else if( c == BC_COMMENT_END )
        {
            s += '"';
            in_comment--;
        }
        else if( c == BC_META_START )
        {
            s += 'M';
            in_meta++;
        }
        else if( c == BC_META_END )
        {
            s += 'm';
            in_meta--;
        }
        else if( c>=BC_MOVE_CODES && in_comment==0 && in_meta==0 )
        {
            switch( (c>>4) & 0x0f )
            {
                default: s += 'P';   break;  // 99.9% of time will be a pawn
                case 0:  s += 'K';   break;
                case 1:  s += 'N';   break;
                case 2:
                case 3:  s += 'R';   break;
                case 4:
                case 5:  s += 'B';   break;
                case 6:
                case 7:  s += 'Q';   break;
            }
        }
        else if( in_comment > 0 )
        {
            comment_len++;
            if( comment_len<5 )
                s += c;
            else if( comment_len==5 )
                s += "...";
            else
                comment_len--;
        }
        else if( in_meta > 0 )
        {
            s += '-';
        }
    }
    return s;
}

// Extract the mainline from a bytecode string
std::string mainline( const std::string &bc )
{
    std::string s;
    bool escape = false;
    int  depth=0, in_comment=0, in_meta=0;
    for( unsigned char c: bc )
    {
        if( escape )
        {
            escape = false;
        }
        else if( c == BC_ESCAPE )
        {
            escape = true;
        }
        else if( c == BC_VARIATION_START )
        {
            depth++;
        }
        else if( c == BC_VARIATION_END )
        {
            depth--;
        }
        else if( c == BC_COMMENT_START )
        {
            in_comment++;
        }
        else if( c == BC_COMMENT_END )
        {
            in_comment--;
        }
        else if( c == BC_META_START )
        {
            in_meta++;
        }
        else if( c == BC_META_END )
        {
            in_meta--;
        }
        else if( c>=BC_MOVE_CODES && depth==0 && in_comment==0 && in_meta==0 )
        {
            s += c;
        }
    }
    return s;
}
