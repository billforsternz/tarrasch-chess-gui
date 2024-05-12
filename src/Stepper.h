/****************************************************************************
 *  Iterate through a chess document
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef STEPPER_H
#define STEPPER_H
#include <algorithm>
#include <vector>
#include <string>
#include "thc.h"
#include "Bytecode.h"

class Stepper : public Bytecode
{
public:
    Stepper( const std::string& _bc ) : bc(_bc)
    {
        len = static_cast<int>(bc.length());
    }
    void Next();
    const std::string   bc;
    int            depth=0;         // nesting depth
    uint8_t        raw;             // raw code
    codepoint_type ct;              // type of code
    bool           is_move = false; // true if it's a ct_move
    thc::Move      mv;              // if is_move
    std::string    san_move;        // if is_move
    size_t         comment_offset;  // if it's any of ct_comment _start, _end or _txt
    std::string    comment_txt;     //  as above

    // after a comment or variation, the next move text should have a number (even for Black)
    bool           move_nbr_needed = false;
    bool           move_nbr_needed_pending = true; // persists until move_nbr_needed set for
    int     idx = 0;
    int     len;
    bool    end = false;
    bool    found = false;
    enum
    {
        IN_MOVES,
        AFTER_MOVE,
        IN_COMMENT,
        IN_META,
        AFTER_ESCAPE
    } state = IN_MOVES;

    // Allow stacking of the key state variables
    struct STACK_ELEMENT
    {
        thc::ChessRules cr;
        thc::Move       mv;
        int             variation_move_count=0;
    };
    STACK_ELEMENT   stk_array[MAX_DEPTH+1];
    STACK_ELEMENT   *stk = stk_array;
    int             stk_idx = 0;
};

#endif // STEPPER_H
