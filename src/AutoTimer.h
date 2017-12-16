/****************************************************************************
 * Simple way of adding timing instrumentation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef AUTO_TIMER_H
#define AUTO_TIMER_H

#include "Portability.h"
#ifdef THC_UNIX
#include <sys/time.h>               // for gettimeofday()
#endif
#ifdef THC_WINDOWS
#include <windows.h>                // for QueryPerformanceCounter()
#endif
#include "DebugPrintf.h"

class AutoTimer
{
public:
    static int instance_cnt;
    static AutoTimer *instance_ptr;
    void Begin()
    {
        #ifdef THC_WINDOWS
        // get ticks per second
        QueryPerformanceFrequency(&frequency);
    
        // start timer
        QueryPerformanceCounter(&t1);
        #endif
        #ifdef THC_UNIX
        // start timer
        gettimeofday(&t1, NULL);
        #endif
    }

    double Elapsed()
    {
        double elapsed_time;
        #ifdef THC_WINDOWS
        // stop timer
        QueryPerformanceCounter(&t2);

        // compute the elapsed time in millisec
        elapsed_time = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        #endif
        #ifdef THC_UNIX
        // stop timer
        gettimeofday(&t2, NULL);
    
        // compute the elapsed time in millisec
        elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
        elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
        #endif
        return elapsed_time;
    }
    AutoTimer( const char *desc )
    {
        if( instance_cnt == 0 )
            instance_ptr = this;
        instance_cnt++;
        this->desc = desc;
        Begin();
    }
    ~AutoTimer()
    {
        instance_cnt--;
        if( instance_cnt == 0 )
            instance_ptr = NULL;
        double elapsed = Elapsed();
        if( desc )
            cprintf( "%s: time elapsed (ms) = %f\n", desc, elapsed );
    }
private:
#ifdef THC_WINDOWS
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
#endif
#ifdef THC_UNIX
    timeval t1, t2;
#endif
    const char *desc;
};

#endif //AUTO_TIMER__H
