/****************************************************************************
 * Log everything to a .pgn file
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LOG_H
#define LOG_H
#include <string>
#include "GameDocument.h"

class Log
{
public:

	// Init
	Log();

public:
    void SaveGame( GameDocument *gd, bool editing_log );
    void EmergencySaveGame( GameDocument *gd, bool first, bool last, std::string &filename_used );
    void Gameover() {}

private:
    std::string head;
    std::string body;
    FILE *emergency_file;
    time_t emergency_filetime_before;
    long emergency_filelen_before, emergency_filelen_delta;
};

#endif // LOG_H
