/****************************************************************************
 * Ancilliary game details, players, event etc.
 * IDEA, but not used yet
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef GAME_DETAILS_H
#define GAME_DETAILS_H
#include <string>
#include <vector>
#include "ChessRules.h"
#include "MoveTree.h"
#include "GameView.h"
#include "GameLifecycle.h"

class GameDetails
{
public:
    GameDetails();
    ~GameDetails();

    // Data
    std::string white;          // "White"
    std::string black;          // "Black"
    std::string event;          // "Event"
    std::string site;           // "Site"
    std::string date;           // "Date"
    std::string round;          // "Round"
    std::string result;         // "Result"
    std::string eco;            // "ECO"
    std::string white_elo;      // "WhiteElo"
    std::string black_elo;      // "BlackElo"
    thc::ChessPosition start_position;  // the start position
    unsigned long fposn;        // offset where moves are in .pgn file
    std::string moves_txt;      // "1.e4 e5 2.Nf3.."
};

#endif //GAME_DETAILS_H
