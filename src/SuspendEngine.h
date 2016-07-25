/****************************************************************************
 * Objects of this type will automatically suspend and resume the UCI engine
 *  as they come into and out of scope.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef SUSPEND_ENGINE_H
#define SUSPEND_ENGINE_H
#include "Objects.h"
#include "Rybka.h"

class SuspendEngine
{
public:

    // Constructor - suspend if appropriate
    SuspendEngine()
    {
        suspended=false;
        if( objs.rybka && !objs.rybka->IsSuspended() )
        {
            objs.rybka->SuspendResume(false);
            suspended = true;
        }
    }

    // Destructor - resume if we suspended
    ~SuspendEngine()
    {
        if( objs.rybka && suspended )
            objs.rybka->SuspendResume(true);
    }

private:
    bool suspended;
};

#endif // SUSPEND_ENGINE_H
