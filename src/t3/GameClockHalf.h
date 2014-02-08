/****************************************************************************
 * Chess clock (for one player only, so half of the complete chess clock)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAME_CLOCK_HALF_H
#define GAME_CLOCK_HALF_H

class GameClockHalf
{
public:
    GameClockHalf();
    ~GameClockHalf();

    // Set the clock parameters
    void SetClock( int time, int increment, bool running, bool visible );
    void SetClock( int millisecs_time );

    // Get the clock parameters
    void GetClock( int &time, int &increment, bool &running, bool &visible );

    // Get/Set  ticking
    bool GetTicking()         { return ticking; }
    void SetTicking( bool ticking ) { this->ticking = ticking; }

    // Get the current displayed time
    bool GetDisplay( wxString &txt, bool expire_show_neg );   // returns bool expired

    // Start and stop the clock
    void Start();
    bool Stop( bool add_increment );  // returns bool was_ticking

    // Give processing time
    bool Run();
    int MillisecsToNextSecond();

    // Data
public:
    int           millisecs_time;
private:
    int           millisecs_increment;
    bool          running;
    bool          visible;
    bool          ticking;
    unsigned long base_sys;
    unsigned long base_remaining;
};

#endif // GAME_CLOCK_HALF_H
