//
//  GameViewElement.h
//
//  GameView uses a vector of elements to model the view of a game
//  Each element is kind of like a token of the PGN view, expanded
//   and decoded for programmer convenience
//

#ifndef GAME_VIEW_ELEMENT_H
#define GAME_VIEW_ELEMENT_H
#include <string>

enum GAME_VIEW_ELEMENT_TYPE
{
    COMMENT,
    MOVE0,
    MOVE,
    START_OF_VARIATION,
    END_OF_VARIATION,
    NEWLINE,
    END_OF_GAME
};

struct GameViewElement
{
    GAME_VIEW_ELEMENT_TYPE  type;
    int                     level;
    unsigned long           offset1;
    unsigned long           offset2;
    thc::Move               mv;
    int                     offset_bc;
    std::string             str;
    std::string             str_for_file_move_only;
    char                    nag_value1;
    char                    nag_value2;
    bool                    published;
};

#endif // GAME_VIEW_ELEMENT_H

