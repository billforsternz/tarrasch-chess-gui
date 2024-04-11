/****************************************************************************
 * Manage game life cycle - i.e. detect end of game, so new game can start
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMELIFECYCLE_H
#define GAMELIFECYCLE_H
#include "thc.h"

enum GAME_RESULT
{
    RESULT_NONE = 0,
    RESULT_WHITE_CHECKMATED,
    RESULT_BLACK_CHECKMATED,
    RESULT_WHITE_RESIGNS,
    RESULT_BLACK_RESIGNS,
    RESULT_WHITE_LOSE_TIME,
    RESULT_BLACK_LOSE_TIME,
    RESULT_DRAW_WHITE_STALEMATED,
    RESULT_DRAW_BLACK_STALEMATED,
    RESULT_DRAW_AGREED,
    RESULT_DRAW_50MOVE,
    RESULT_DRAW_INSUFFICIENT,
    RESULT_DRAW_REPITITION
};

class GameLifecycle
{
public:
    GameLifecycle()                     {this->result = RESULT_NONE; this->human_is_white = true;}
    void Begin( bool human_is_white_ )  {this->result = RESULT_NONE; this->human_is_white = human_is_white_;}
    void Swap()                         {this->human_is_white = !this->human_is_white;}
    void Set( GAME_RESULT result_ )     {this->result = result_;}
    GAME_RESULT Get()                   {return this->result;}
    bool CanHumanClaimDraw( GAME_RESULT &result );
    // bool TestResult( wxString &txt );    // returns true if game is over
    bool human_is_white;
    GAME_RESULT result;
};

#endif // GAMELIFECYCLE

