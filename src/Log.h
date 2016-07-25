/****************************************************************************
 * Log everything to a .pgn file
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LOG_H
#define LOG_H
#include "GameDocument.h"

class Log
{
public:

	// Init
	Log();

public:
    void SaveGame( GameDocument *gd, bool editing_log );
    void Gameover() {}
private:
    std::string head;
    std::string body;
};

#endif // LOG_H
