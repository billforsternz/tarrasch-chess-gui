/****************************************************************************
 * Chess clock (for one player only, so half of the complete chess clock)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <stdlib.h>
#include <time.h>
#include "wx/wx.h"
#include "Portability.h"
#include "GameClockHalf.h"

// Constructor
GameClockHalf::GameClockHalf()
{
    ticking = false;
}

// Destructor
GameClockHalf::~GameClockHalf()
{
}


// Set the clock parameters
void GameClockHalf::SetClock( int time, int increment, bool running_, bool visible_ )
{
    bool temp = ticking;
    if( temp )
        Stop(false);
    this->millisecs_time      = time      * 1000;
    this->millisecs_increment = increment * 1000;
    this->running             = running_;
    this->visible             = visible_;
    if( temp )
        Start();
}

// Set the clock parameters
void GameClockHalf::SetClock( int millisecs_time_ )
{
    bool temp = ticking;
    if( temp )
        Stop(false);
    this->millisecs_time = millisecs_time_;
    if( temp )
        Start();
}

// Get the clock parameters
void GameClockHalf::GetClock( int &time, int &increment, bool &running_, bool &visible_ )
{
    bool temp = ticking;
    if( temp )
        Stop(false);
    time      = this->millisecs_time/1000;
    increment = this->millisecs_increment/1000;
    running_  = this->running;             
    visible_  = this->visible;
    if( temp )
        Start();
}

// Get the current displayed time
bool GameClockHalf::GetDisplay( wxString &txt, bool expire_show_neg  )   // returns bool expired
{
    bool expired=false;
    if( millisecs_time == 0 )
    {
        expired = true;
        if( expire_show_neg )
            txt = "- 0";
        else
            txt = "0.00";
    }
    else
    {
        int secs = millisecs_time / 1000;
        int mins = secs / 60;
        int remainder_secs = secs % 60;
        txt.Printf( "%d.%02d", mins, remainder_secs );        
    }
    return expired;
}

// Start the clock
void GameClockHalf::Start()
{
    if( running )
    {
        base_sys = GetTickCount();
        base_remaining = (unsigned long)millisecs_time;
        ticking = true;
    }
}

// Stop the clock
bool GameClockHalf::Stop( bool add_increment )
{
    bool was_ticking = ticking;
    if( running && ticking && add_increment )
        base_remaining += millisecs_increment;
    if( ticking )
    {
        Run();
        ticking = false;
    }
    return was_ticking;
}

// Give processing time
bool GameClockHalf::Run()
{
    bool expired=false;
    if( ticking )
    {
        unsigned long now = GetTickCount();
        unsigned long ms_expired = now-base_sys;
        if( ms_expired > base_remaining )
        {
            expired = true;
            millisecs_time = 0;
        }
        else
            millisecs_time = base_remaining-ms_expired;
    }
    return expired;
}


// How many millisecs until next second tick ?
int GameClockHalf::MillisecsToNextSecond()
{
    int millisecs=1000;
    if( ticking )
        millisecs = (millisecs_time % 1000);
    return millisecs;
}

