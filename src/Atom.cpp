/****************************************************************************
 * Atom implements a mechanism whereby each user action is atomic in terms of
 * display update and undo/redo
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "GameDocument.h"
#include "GameLogic.h"
#include "Undo.h"
#include "Atom.h"

Atomic::Atomic( bool set_focus )
{
    if( stack_count++ == 0 ) 
        gl->atom.Begin(set_focus);
}

Atomic::~Atomic()
{
    if( --stack_count == 0 ) 
        gl->atom.End();
}

int Atomic::stack_count = 0;
GameLogic * Atomic::gl = 0;
Atom::Atom( GameLogic *gl )
{
    Atomic::gl = gl;
    this->gl = gl;
    running = false;
}

void Atom::SetInsertionPoint( long pos_ )
{
    if( running )
    {
        insertion_point=true;
        this->pos = pos_;
    }
    else
        gl->lb->SetInsertionPoint(pos_);
}

unsigned long Atom::GetInsertionPoint()
{
    if( running && (insertion_point||display||redisplay) )
        return pos;
    else
        return gl->lb->GetInsertionPoint();
}

void Atom::Display( long pos_ )
{
    if( running )
    {
        display=true;
        this->pos=pos_;
    }
    else
        gl->gd.Display(pos_);
}

void Atom::Redisplay( long pos_ )
{
    if( running )
    {
        redisplay=true;
        this->pos=pos_;
    }
    else
        gl->gd.Redisplay(pos_);
}

void Atom::Begin( bool set_focus_ )
{
    this->set_focus = set_focus_;
    insertion_point=false;
    display=false;
    redisplay=false;
    undo=false;
    status_update=false;
    running=true;
    undo_previous_posn = gl->lb->GetInsertionPoint();
}

void Atom::End()
{
    if( redisplay )
        gl->gd.Redisplay(pos);
    else if( display )
        gl->gd.Display(pos);
    else if( insertion_point )
    {
        gl->lb->SetInsertionPoint(pos);
        gl->lb->ShowPosition(pos);
        gl->lb->Refresh();
    }
    if( undo )
        gl->undo.Save(undo_previous_posn,gl->gd,gl->state);
    if( undo||status_update )
        gl->StatusUpdate();
    if( set_focus )
        gl->SetFocusOnList();
    set_focus=false;
    insertion_point=false;
    display=false;
    redisplay=false;
    undo=false;
    status_update=false;
    running=false;
}

