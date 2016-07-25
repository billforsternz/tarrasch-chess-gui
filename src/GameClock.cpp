/****************************************************************************
 * On screen chess clock
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "Repository.h"
#include "Objects.h"
#include "GameClockHalf.h"
#include "GameClock.h"

GameClock::GameClock()
{
    // Repository2Clocks(); // don't really want to restore this stuff
    SetDefault();   // set something sensible instead
}

void GameClock::SetDefault()
{
    fixed_period_mode = false;
    Repository2Clocks();
    white.SetClock( 5*60, 0, false, false );
    black.SetClock( 5*60, 0, false, false );
    Clocks2Repository();
}

GameClock::~GameClock()
{
}

void GameClock::Repository2Clocks()
{
    ClockConfig *rep = &objs.repository->clock;
    fixed_period_mode = rep->m_fixed_period_mode;
    int secs = rep->m_white_time*60 + rep->m_white_secs;
    if( secs <= 0 )
        secs = 1;
    white.SetClock( secs, rep->m_white_increment,
                          rep->m_white_running,
                          rep->m_white_visible );
    secs = rep->m_black_time*60 + rep->m_black_secs;
    if( secs <= 0 )
        secs = 1;
    black.SetClock( secs, rep->m_black_increment,
                          rep->m_black_running,
                          rep->m_black_visible );
}

void GameClock::Clocks2Repository()
{
    ClockConfig *rep = &objs.repository->clock;
    int secs;
    white.GetClock( secs, rep->m_white_increment,
                          rep->m_white_running,
                          rep->m_white_visible );
    rep->m_white_time = secs/60;
    rep->m_white_secs = secs%60;
    black.GetClock( secs, rep->m_black_increment,
                          rep->m_black_running,
                          rep->m_black_visible );
    rep->m_black_time = secs/60;
    rep->m_black_secs = secs%60;
}

void GameClock::NewHumanEngineGame( bool human_is_white, bool white_to_move )
{
    ClockConfig *rep = &objs.repository->clock;
    fixed_period_mode = rep->m_fixed_period_mode;
    if( human_is_white )
    {
        rep->m_white_time      = rep->m_human_time;
        rep->m_white_increment = rep->m_human_increment;
        rep->m_white_visible   = rep->m_human_visible;
        rep->m_white_running   = rep->m_human_running;
        rep->m_black_time      = fixed_period_mode ? rep->m_engine_fixed_minutes : rep->m_engine_time;
        rep->m_black_secs      = fixed_period_mode ? rep->m_engine_fixed_seconds : 0;
        rep->m_black_increment = fixed_period_mode ? 0 : rep->m_engine_increment;
        rep->m_black_visible   = rep->m_engine_visible;
        rep->m_black_running   = rep->m_engine_running;
    }
    else
    {
        rep->m_black_time      = rep->m_human_time;
        rep->m_black_increment = rep->m_human_increment;
        rep->m_black_visible   = rep->m_human_visible;
        rep->m_black_running   = rep->m_human_running;
        rep->m_white_time      = fixed_period_mode ? rep->m_engine_fixed_minutes : rep->m_engine_time;
        rep->m_white_secs      = fixed_period_mode ? rep->m_engine_fixed_seconds : 0;
        rep->m_white_increment = fixed_period_mode ? 0 : rep->m_engine_increment;
        rep->m_white_visible   = rep->m_engine_visible;
        rep->m_white_running   = rep->m_engine_running;
    }
    Repository2Clocks();
    GameStart( white_to_move );
}

void GameClock::SetStaticTimes( int white_millisecs_time, bool white_visible, int black_millisecs_time, bool black_visible )
{
    int time;
    int increment;
    bool running;
    bool visible;
    white.GetClock( time, increment, running, visible );
    objs.repository->clock.m_white_visible = white_visible;
    white.SetClock( time, increment, false, white_visible );
    white.SetClock( white_millisecs_time );
    black.GetClock( time, increment, running, visible );
    objs.repository->clock.m_black_visible = black_visible;
    black.SetClock( time, increment, false, black_visible );
    black.SetClock( black_millisecs_time );
}

void GameClock::SetStaticTimes( int white_millisecs_time, int black_millisecs_time )
{
    int time;
    int increment;
    bool running;
    bool visible;
    white.GetClock( time, increment, running, visible );
    white.SetClock( time, increment, false, visible );
    white.SetClock( white_millisecs_time );
    black.GetClock( time, increment, running, visible );
    black.SetClock( time, increment, false, visible );
    black.SetClock( black_millisecs_time );
}

void GameClock::SetTimes( int white_millisecs_time, int black_millisecs_time )
{
    white.SetClock( white_millisecs_time );
    black.SetClock( black_millisecs_time );
}

void GameClock::GetTimes( int &white_millisecs_time, int &black_millisecs_time )
{
    white_millisecs_time = white.millisecs_time;
    black_millisecs_time = black.millisecs_time;
}

void GameClock::Swap( bool human_is_white, bool white_to_move )
{
    ClockConfig *rep = &objs.repository->clock;
    int  temp1      = rep->m_white_time;
    int  temp2      = rep->m_white_secs;
    int  temp3      = rep->m_white_increment;
    bool temp4      = rep->m_white_visible;
    bool temp5      = rep->m_white_running;
    rep->m_white_time      = rep->m_black_time;
    rep->m_white_secs      = rep->m_black_secs;
    rep->m_white_increment = rep->m_black_increment;
    rep->m_white_visible   = rep->m_black_visible;
    rep->m_white_running   = rep->m_black_running;
    rep->m_black_time      = temp1;
    rep->m_black_secs      = temp2;
    rep->m_black_increment = temp3;
    rep->m_black_visible   = temp4;
    rep->m_black_running   = temp5;

    int time_w, time_b;
    int increment_w, increment_b;
    bool running_w, running_b;
    bool visible_w, visible_b;
    white.GetClock( time_w, increment_w, running_w, visible_w );
    black.GetClock( time_b, increment_b, running_b, visible_b );
    white.SetClock( time_b, increment_b, running_b, visible_b );
    black.SetClock( time_w, increment_w, running_w, visible_w );

    // Needs a special kick in fixed_period_mode since the computer clock expired (to generate move - not to end game)
    if( fixed_period_mode )
    {
        if( human_is_white )
            black.Start();
        else
            white.Start();
    }
}

void GameClock::GameStart( bool white_to_move )
{
    if( white_to_move )
        white.Start();
    else
        black.Start();
}

bool GameClock::Run( bool white_to_move )
{
    bool expired = false;
    if( black.GetTicking() && white.GetTicking() )  // this shouldn't happen
    {
        if( white_to_move )
            black.SetTicking( false );
        else
            white.SetTicking( false );
    }
    if( white_to_move ) 
        expired = white.Run();
    else
        expired = black.Run();
    return expired;
}

void GameClock::Press( bool white_move )
{
    if( white_move )
    {
        white.Stop(true);
        black.Start();
    }
    else
    {
        black.Stop(true);
        white.Start();
    }
    if( fixed_period_mode )
        Repository2Clocks();
}

void GameClock::GameOver()
{
    white.Stop(false);
    black.Stop(false);
}

// How many millisecs until next second tick ?
int GameClock::MillisecsToNextSecond()
{
    int min_millisecs = 1000;
    int millisecs = white.MillisecsToNextSecond();
    if( millisecs < min_millisecs )
        min_millisecs = millisecs;
    millisecs = black.MillisecsToNextSecond();
    if( millisecs < min_millisecs )
        min_millisecs = millisecs;
    return min_millisecs;
}

void GameClock::PauseBegin()
{
    white_was_ticking = white.Stop(false);
    black_was_ticking = black.Stop(false);
}

void GameClock::PauseEnd()
{
    if( white_was_ticking )
        white.Start();
    if( black_was_ticking )
        black.Start();
}
