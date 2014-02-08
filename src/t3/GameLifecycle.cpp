/****************************************************************************
 * Manage game life cycle - i.e. detect end of game, so new game can start
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "wx/wx.h"
#include "GameLifecycle.h"

bool GameLifecycle::CanHumanClaimDraw( GAME_RESULT &result )
{
    result = RESULT_DRAW_AGREED;
    return true;
}

// returns true if game is over
bool GameLifecycle::TestResult( wxString &txt )
{
    bool game_over=true;
    switch( this->result )
    {
        case RESULT_NONE: game_over = false;
            txt = "";
            break;
        case RESULT_WHITE_CHECKMATED:
            txt = "White checkmated";
            break;
        case RESULT_BLACK_CHECKMATED:
            txt = "Black checkmated";
            break;
        case RESULT_WHITE_RESIGNS:
            txt = "White resigns";
            break;
        case RESULT_BLACK_RESIGNS:
            txt = "Black resigns";
            break;
        case RESULT_WHITE_LOSE_TIME:
            txt = "White loses on time";
            break;
        case RESULT_BLACK_LOSE_TIME:
            txt = "Black loses on time";
            break;
        case RESULT_DRAW_WHITE_STALEMATED:
            txt = "Draw: White is stalemated";
            break;
        case RESULT_DRAW_BLACK_STALEMATED:
            txt = "Draw: Black is stalemated";
            break;
        case RESULT_DRAW_AGREED:
            txt = "Draw agreed";
            break;
        case RESULT_DRAW_50MOVE:
            txt = "Draw: 50 move rule";
            break;
        case RESULT_DRAW_INSUFFICIENT:
            txt = "Draw: Insufficient material";
            break;
        case RESULT_DRAW_REPITITION:
            txt = "Draw: Threefold repetition";
            break;
    }
    return game_over;
}
