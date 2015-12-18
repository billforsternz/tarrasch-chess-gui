/****************************************************************************
 * Undo/Redo
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef UNDO_H
#define UNDO_H
#include <deque>
#include "GameState.h"
#include "GameDocument.h"
#include "GameLifecycle.h"
#include "thc.h"
#include "DebugPrintf.h"

struct RestorePoint
{
    MoveTree    tree;                     // the moves
    long        previous_posn;
    long        posn;
    std::string result;
    GAME_STATE  state;
    thc::Move   ponder_move;
    bool        human_is_white;
    GAME_RESULT game_result;
    bool        normal_orientation;
    bool        takeback;
};

class GameLogic;
class Undo
{
public:

	// Init
	Undo( GameLogic *gl );
    Undo();
    void Clear( GameDocument &gd, GAME_STATE game_state=MANUAL );

    // Copy constructor needed because it_saved needs special attention
    Undo( const Undo& copy_from_me );
    Undo & operator= (const Undo & copy_from_me );
    
public:
    void Save( long undo_previous_posn, GameDocument &gd, GAME_STATE game_state=MANUAL );
    GAME_STATE DoUndo( GameDocument &gd, bool takeback=false );
    GAME_STATE DoRedo( GameDocument &gd );
    bool IsModified();
    bool CanRedo();
    void ShowStackSize( const char *desc ) { cprintf( "%s Stack size = %d\n", desc, stack.size() ); }

private:
    enum { NORMAL, UNDOING } state;
    bool no_front_pops_yet;
public:
    std::deque<RestorePoint> stack;
    int /* std::deque<RestorePoint>::iterator */ it_saved;
    GameLogic *gl;
};

#endif // UNDO_H
