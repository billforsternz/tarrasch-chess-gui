/****************************************************************************
 * Implements separate games on tabbed pages
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "Tabs.h"
#include "GameLogic.h"

void Tabs::TabNew( GameDocument &new_gd )
{
    unsigned long pos=0;
    TabElement e(gl);
    v.push_back(e); // create a new slot, not used until we switch away from the new tab
    nbr_tabs++;
    if( nbr_tabs > 1 ) // nbr_tabs==0 only on start up, no need to save anything then
    {
        pos = gl->gd.GetInsertionPoint();
        v[current_idx].gd  = gl->gd;
        v[current_idx].pos = pos;
        v[current_idx].undo = gl->undo;
        gl->gd = new_gd;
        gl->undo = e.undo; // blank
        current_idx = nbr_tabs-1;
        wxPanel *notebook_page1 = new wxPanel(objs.canvas->notebook, wxID_ANY );
        objs.canvas->notebook->AddPage(notebook_page1,"New Game",true);
    }
    cprintf( "Set: tab idx=%d, pos=%ld\n", current_idx, pos );
}


bool Tabs::TabSelected( int idx )
{
    bool okay=false;
    if( idx < nbr_tabs )
    {
        okay = true;
        unsigned long pos = gl->gd.GetInsertionPoint();
        v[current_idx].gd  = gl->gd;
        v[current_idx].pos = pos;
        v[current_idx].undo = gl->undo;
        cprintf( "Set: tab idx=%d, pos=%ld\n", current_idx, pos );
        gl->gd = v[idx].gd;
        gl->undo = v[idx].undo;
        pos = v[idx].pos;
        gl->gd.SetInsertionPoint(pos);
        gl->gd.non_zero_start_pos = pos;
        current_idx = idx;
        cprintf( "Get: tab idx=%d, pos=%ld\n", idx, pos );
    }
    return okay;
}

GameDocument *Tabs::Begin()
{
    iter_doc = 0;
    return Next();
}

Undo *Tabs::BeginUndo()
{
    iter_undo = 0;
    return NextUndo();
}

GameDocument *Tabs::Next()
{
    GameDocument *p = NULL;
    if( 0<=iter_doc && iter_doc<v.size() )
    {
        p = iter_doc==current_idx ? &gl->gd : &v[iter_doc].gd;
    }
    iter_doc++;
    return p;
}

Undo *Tabs::NextUndo()
{
    Undo *p = NULL;
    if( 0<=iter_undo && iter_undo<v.size() )
    {
        p = iter_undo==current_idx ? &gl->undo : &v[iter_undo].undo;
    }
    iter_undo++;
    return p;
}



void Tabs::SetTitle( GameDocument &gd )
{
    const int TAB_TITLE_LENGTH=20;
    std::string s("New Game");
    if( gd.white.length() > 0 )
    {
        s = gd.white + "-" + gd.black;
    }
    else
    {
        gd.gv.ToString( s );
        if( s.length() < 4 )
        {
            s = "New Game";
        }
        else if( s.length() > TAB_TITLE_LENGTH )
        {
            s = s.substr(0,TAB_TITLE_LENGTH) + "...";
        }
    }
    wxString ws(s.c_str());
    if( objs.canvas->resize_ready )
        objs.canvas->notebook->SetPageText( current_idx, ws );
}

int Tabs::TabDelete()
{
    if( nbr_tabs>1 && current_idx<nbr_tabs )
    {
        v.erase( v.begin() + current_idx );
        objs.canvas->notebook->DeletePage( current_idx );
        nbr_tabs--;
        if( current_idx == nbr_tabs )
            current_idx--;
        int idx = current_idx;
        gl->undo = v[idx].undo;
        gl->gd = v[idx].gd;
        unsigned long pos = v[idx].pos;
        gl->gd.SetInsertionPoint(pos);
        gl->gd.non_zero_start_pos = pos;
    }
    return( current_idx );
}
