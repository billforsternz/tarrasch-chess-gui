/****************************************************************************
 * Representation of a game's tree of variations
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <string>
#include <vector>
#include "thc.h"
#include "Bytecode.h"
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

// Get MovePlus at given offset
MovePlus GameTree::GetMovePlus( int offset_parm )
{
    MovePlus ret;
    return ret;
}

// Promote a variation at given offset
//  Return offset of promoted variation
int Promote( int offset_parm )
{
    return 0;
}

// Demote a variation at given offset
//  Return offset of promoted variation
int Demote( int offset_parm  )
{
    return 0;
}

// Delete the rest of a variation
void DeleteRestOfVariation( int offset_parm  )
{
}

// Delete variation
void DeleteVariation( int offset_parm  )
{
}