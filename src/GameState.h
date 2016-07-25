/****************************************************************************
 * Main game state
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMESTATE_H
#define GAMESTATE_H

// Main game state
typedef enum 
{
    RESET,
    GAMEOVER,
    PONDERING,      // Human move, but the engine is thinking
    HUMAN,		    // Human move
    MANUAL,         // Human but moving manually rather than playing against engine
    SLIDING,        // Sliding
    SLIDING_MANUAL,
    THINKING,
    POPUP,
    POPUP_MANUAL,
    FAKE_BOOK_DELAY
} GAME_STATE;

#endif // GAMESTATE_H
