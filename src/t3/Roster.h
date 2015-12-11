/****************************************************************************
 * Roster = Meta data for game (inspired by the pgn header's "7 tag roster")
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef ROSTER_H
#define ROSTER_H
#include <string>

class Roster
{
public:
    std::string white;
    std::string black;
    std::string event;
    std::string site;
    std::string result;
    std::string round;
    std::string date;
    std::string eco;
    std::string white_elo;
    std::string black_elo;
    std::string fen;
};

#endif //ROSTER_H
