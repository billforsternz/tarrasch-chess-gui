/****************************************************************************
 * Undo/Redo
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "Undo.h"
#include "Objects.h"
#include "GameLogic.h"

#define assert_todo_fix(x)
#define MAX_DEPTH 1000 // Until I did this I worried that the undo stack would consume all of memory eventually

// Init
Undo::Undo( GameLogic *gl )
{
    this->gl = gl;
    no_front_pops_yet = true;
    state = NORMAL;
    it_saved = 0; //stack.begin();
}

Undo::Undo()
{
    this->gl = objs.gl;
    no_front_pops_yet = true;
    state = NORMAL;
    it_saved = 0; //stack.begin();
}

// Copy constructor needed begin it_saved needs special attention
Undo::Undo( const Undo& copy_from_me )
{
    state             = copy_from_me.state;
    no_front_pops_yet = copy_from_me.no_front_pops_yet;
    gl                = copy_from_me.gl;
    stack             = copy_from_me.stack;
    it_saved = copy_from_me.it_saved;
}

Undo & Undo::operator= (const Undo & copy_from_me )
{
    state             = copy_from_me.state;
    no_front_pops_yet = copy_from_me.no_front_pops_yet;
    gl                = copy_from_me.gl;
    stack             = copy_from_me.stack;
    it_saved          = copy_from_me.it_saved;
    return *this;
}

void Undo::Clear( GameDocument &gd, GAME_STATE game_state )
{
    stack.clear();
    it_saved = 0; //stack.begin();
    cprintf( "clear() stack_size()=%d\n", stack.size() );
    state = NORMAL;
    no_front_pops_yet = true;
    Save( 0, gd, game_state );
    gl->atom.NotUndoAble();
    //assert_todo_fix( it_saved >= stack.begin() );
}

bool Undo::CanUndo()
{
    bool can_undo = true;
    if( stack.size() <= 1 )     // need at least one restore point to undo back to
        can_undo = false;
    else if( state==UNDOING && it_saved==0 /*stack.begin()*/ )  // in UNDOING state it_saved "points" at the element last restored
                                                                //  when it gets down to zero we have just restored the oldest restore
                                                                //  point in the stack
        can_undo = false;
    return can_undo;
}

bool Undo::IsModified()
{
    bool modified = true;
    if( no_front_pops_yet )
    {
        if( stack.size() <= 1 )
            modified = false;
        else if( state==UNDOING && it_saved==0 /*stack.begin()*/ )
            modified = false;
    }
    //assert_todo_fix( it_saved >= stack.begin() );
    return modified;
}

void Undo::Save( long undo_previous_posn, GameDocument &gd, GAME_STATE game_state )
{
    RestorePoint rp;
    rp.tree = gd.tree_bc;
    rp.previous_posn = undo_previous_posn;
    rp.posn = gd.GetInsertionPoint();
    rp.result = gd.r.result;
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
            if( stack.begin()+it_saved+1 == stack.end() )
                break;
            stack.pop_back();
            cprintf( "pop_back() stack_size()=%d\n", stack.size() );
        }
    }
    stack.push_back(rp);
    cprintf( "push_back() stack_size()=%d, stack.end()-stack.begin()=%d\n", stack.size(), stack.end()-stack.begin() );
    if( stack.size() > MAX_DEPTH )
    {
        stack.pop_front();
        no_front_pops_yet = false;
        cprintf( "After popping from front, stack_size()=%d, stack.end()-stack.begin()=%d\n", stack.size(), stack.end()-stack.begin() );
    }
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
            it = stack.begin() + it_saved;
        if( it > stack.begin() )
        {
            it->takeback = takeback;
            long posn = it->previous_posn;
            it--;
            it_saved = it-stack.begin();
            cprintf( "In DoUndo, it_saved=%d, stack_size()=%d, stack.end()-stack.begin()=%d\n", it_saved, stack.size(), stack.end()-stack.begin() );
            RestorePoint rp;
            rp = *it;
            ret = rp.state;
            gd.r.result = rp.result;
            gd.tree_bc = rp.tree;
            gl->ponder_move = rp.ponder_move;
            gl->glc.human_is_white = rp.human_is_white;
            gl->glc.result = rp.game_result;
            objs.canvas->SetNormalOrientation( rp.normal_orientation );
            gd.Rebuild();
            gd.Redisplay( posn );
            state = UNDOING;
        }
    }
    //assert_todo_fix( it_saved >= stack.begin() );
    return ret;
}

bool Undo::CanRedo()
{
    bool can_redo = false;
    int len = stack.size();
    if( len && state==UNDOING )
    {
        int /*std::deque<RestorePoint>::iterator*/ it = it_saved;
        if( stack.begin()+(it+1) != stack.end() )
            can_redo = true;
    }
    //assert_todo_fix( it_saved >= stack.begin() );
    return can_redo;
}

GAME_STATE Undo::DoRedo( GameDocument &gd )
{
    GAME_STATE ret=MANUAL;
    int len = stack.size();
    if( len && state==UNDOING )
    {
        int /*std::deque<RestorePoint>::iterator*/ it = it_saved;
        if( stack.begin()+(it+1) == stack.end() )
            state = NORMAL;
        else
        {
            it++;
            it_saved = it;
            RestorePoint rp;
            rp = *(stack.begin() + it);
            #if 0   //put this back if you want redo after takeback to restart the game
                    // (not very good because if say engine plays move that will kill
                    //  the redo tail)
            ret = rp.takeback ? rp.state : MANUAL;
            #endif
            gd.tree_bc = rp.tree;
            gd.r.result = rp.result;
            gl->ponder_move = rp.ponder_move;
            gl->glc.human_is_white = rp.human_is_white;
            gl->glc.result = rp.game_result;
            objs.canvas->SetNormalOrientation( rp.normal_orientation );
            gd.Rebuild();
            gd.Redisplay( rp.posn );
        }
    }
    //assert_todo_fix( it_saved >= stack.begin() );
    return ret;
}


