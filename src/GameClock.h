/****************************************************************************
 * On screen chess clock
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAME_CLOCK_H
#define GAME_CLOCK_H
#include "GameClockHalf.h"

class GameClock
{
public:
    GameClock();
    ~GameClock();
    void SetDefault();
    void Repository2Clocks();
    void Clocks2Repository();
    void NewHumanEngineGame( bool human_is_white, bool white_to_move );
    void GameStart( bool white_to_move );
    void Swap( bool human_is_white );
    void SetStaticTimes( int white_millisecs_time, int black_millisecs_time );
    void SetStaticTimes( int white_millisecs_time, bool white_visible, int black_millisecs_time, bool black_visible );
    void SetTimes( int white_millisecs_time, int black_millisecs_time );
    void GetTimes( int &white_millisecs_time, int &black_millisecs_time );
    bool Run( bool white_to_move );
    void Press( bool white_move );
    void GameOver();
    int  MillisecsToNextSecond();
    void PauseBegin();
    void PauseEnd();

//private:
    GameClockHalf white;
    GameClockHalf black;
    bool fixed_period_mode;

private:
    bool white_was_ticking;
    bool black_was_ticking;
};

#endif //GAME_CLOCK_H
