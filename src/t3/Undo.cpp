/****************************************************************************
 * Undo/Redo
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "Undo.h"
#include "Objects.h"
#include "GameLogic.h"

#define assert_todo_fix(x)

// Init
Undo::Undo( GameLogic *gl )
{
    this->gl = gl;
    no_front_pops_yet = true;
    state = NORMAL;
    it_saved = stack.begin();
}

Undo::Undo()
{
    this->gl = objs.gl;
    no_front_pops_yet = true;
    state = NORMAL;
    it_saved = stack.begin();
}

// Copy constructor needed begin it_saved needs special attention
Undo::Undo( const Undo& copy_from_me )
{
    state             = copy_from_me.state;
    no_front_pops_yet = copy_from_me.no_front_pops_yet;
    gl                = copy_from_me.gl;
    stack             = copy_from_me.stack;
#if 1 //FIXME later
    it_saved = stack.begin();
#else
    if( copy_from_me.it_saved < copy_from_me.stack.begin() )
        cprintf( "Gotcha 1\n");
    if( copy_from_me.it_saved > copy_from_me.stack.end() )
        cprintf( "Gotcha 2\n");
    it_saved = stack.begin() + (copy_from_me.it_saved - copy_from_me.stack.begin());
    cprintf( "it_saved offset=%d, stack.end() offset=%d\n", it_saved-stack.begin(), stack.end()-stack.begin() );
    assert_todo_fix( it_saved >= stack.begin() );
#endif
}

Undo & Undo::operator= (const Undo & copy_from_me )
{
    state             = copy_from_me.state;
    no_front_pops_yet = copy_from_me.no_front_pops_yet;
    gl                = copy_from_me.gl;
    stack             = copy_from_me.stack;
#if 1 //FIXME later
    it_saved = stack.begin();
#else
    if( copy_from_me.it_saved < copy_from_me.stack.begin() )
        cprintf( "* Gotcha 1\n");
    if( copy_from_me.it_saved > copy_from_me.stack.end() )
        cprintf( "* Gotcha 2\n");
    it_saved = stack.begin() + (copy_from_me.it_saved - copy_from_me.stack.begin());
    cprintf( "* it_saved offset=%d, stack.end() offset=%d\n", it_saved-stack.begin(), stack.end()-stack.begin() );
    assert_todo_fix( it_saved >= stack.begin() );
#endif
    return *this;
}

void Undo::Clear( GameDocument &gd, GAME_STATE game_state )
{
    stack.clear();
    it_saved = stack.begin();
    cprintf( "clear() stack_size()=%d\n", stack.size() );
    state = NORMAL;
    no_front_pops_yet = true;
    Save( 0, gd, game_state );
    gl->atom.NotUndoAble();
    assert_todo_fix( it_saved >= stack.begin() );
}

bool Undo::IsModified()
{
    bool modified = true;
    if( no_front_pops_yet )
    {
        if( stack.size() <= 1 )
            modified = false;
        else if( state==UNDOING && it_saved==stack.begin() )
            modified = false;
    }
    assert_todo_fix( it_saved >= stack.begin() );
    return modified;
}

void Undo::Save( long undo_previous_posn, GameDocument &gd, GAME_STATE game_state )
{
    RestorePoint rp;
    rp.tree = gd.tree;
    rp.previous_posn = undo_previous_posn;
    rp.posn = gd.GetInsertionPoint();
    rp.result = gd.result;
    rp.state = game_state;
    rp.ponder_move = gl->ponder_move;
    rp.human_is_white = gl->glc.human_is_white;
    rp.game_result = gl->glc.result;
    if( objs.canvas )
        rp.normal_orientation = objs.canvas->GetNormalOrientation();
    else
        rp.normal_orientation = true;
    if( state == UNDOING )
    {
        // remove the tail        }
        while( stack.size() )
        {
            if( it_saved+1 == stack.end() )
                break;
            stack.pop_back();
            cprintf( "pop_back() stack_size()=%d\n", stack.size() );
        }
    }
    stack.push_back(rp);
    cprintf( "push_back() stack_size()=%d\n", stack.size() );
    assert_todo_fix( it_saved >= stack.begin() );
    state = NORMAL;
}

GAME_STATE Undo::DoUndo( GameDocument &gd, bool takeback )
{
    GAME_STATE ret=MANUAL;
    int len = stack.size();
    if( len )
    {
        std::deque<RestorePoint>::iterator it;
        if( state == NORMAL )
            it = stack.end()-1;
        else if( state==UNDOING )
            it = it_saved;
        if( it > stack.begin() )
        {
            it->takeback = takeback;
            long posn = it->previous_posn;
            it--;
            it_saved = it;
            RestorePoint rp;
            rp = *it;
            ret = rp.state;
            gd.result = rp.result;
            gd.tree = rp.tree;
            gl->ponder_move = rp.ponder_move;
            gl->glc.human_is_white = rp.human_is_white;
            gl->glc.result = rp.game_result;
            objs.canvas->SetNormalOrientation( rp.normal_orientation );
            gd.Rebuild();
            gd.Redisplay( posn );
            state = UNDOING;
        }
    }
    assert_todo_fix( it_saved >= stack.begin() );
    return ret;
}

bool Undo::CanRedo()
{
    bool can_redo = false;
    int len = stack.size();
    if( len && state==UNDOING )
    {
        std::deque<RestorePoint>::iterator it = it_saved;
        if( it+1 != stack.end() )
            can_redo = true;
    }
    assert_todo_fix( it_saved >= stack.begin() );
    return can_redo;
}

GAME_STATE Undo::DoRedo( GameDocument &gd )
{
    GAME_STATE ret=MANUAL;
    int len = stack.size();
    if( len && state==UNDOING )
    {
        std::deque<RestorePoint>::iterator it = it_saved;
        if( it+1 == stack.end() )
            state = NORMAL;
        else
        {
            it++;
            it_saved = it;
            RestorePoint rp;
            rp = *it;
            #if 0   //put this back if you want redo after takeback to restart the game
                    // (not very good because if say engine plays move that will kill
                    //  the redo tail)
            ret = rp.takeback ? rp.state : MANUAL;
            #endif
            gd.tree = rp.tree;
            gd.result = rp.result;
            gl->ponder_move = rp.ponder_move;
            gl->glc.human_is_white = rp.human_is_white;
            gl->glc.result = rp.game_result;
            objs.canvas->SetNormalOrientation( rp.normal_orientation );
            gd.Rebuild();
            gd.Redisplay( rp.posn );
        }
    }
    assert_todo_fix( it_saved >= stack.begin() );
    return ret;
}


