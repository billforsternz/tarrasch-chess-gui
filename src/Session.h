/****************************************************************************
 * Keep a record of all games in a session
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef SESSION_H
#define SESSION_H
#include "GameDocument.h"

class Session
{
public:

    // Init
    Session();

public:
    void SaveGame( GameDocument *gd );
    void Gameover() {}
};

#endif // SESSION_H
