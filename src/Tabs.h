/****************************************************************************
 * Implements separate games on tabbed pages
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef TABS_H
#define TABS_H
#include "GameLogic.h"
#include "Undo.h"
#include <vector>

struct TabElement
{
    GameDocument gd;
    Undo undo;
    unsigned long pos;
    TabElement(GameLogic *gl) : undo(gl) { pos=0; }
};

class Tabs
{
private:
    std::vector<TabElement> v;
    int current_idx;
    int nbr_tabs;
    int iter_doc;
    int iter_undo;
    
public:
    GameLogic *gl;
    Tabs()
    {
        current_idx=0;
        nbr_tabs=0;
        iter_doc = 0;
        iter_undo = 0;
    }
    int  GetCurrentIdx() { return current_idx; }
    int  GetNbrTabs() { return nbr_tabs; }
    void TabNew( GameDocument &new_gd );
    bool TabSelected( int idx );
#ifdef AUI_NOTEBOOK
    void TabDelete( int idx, bool system_will_delete_notebook_page );
#else
    int  TabDelete();
#endif
    void SetInfile( bool UNUSED(infile) ) {}	// later delete
    void SetTitle( GameDocument &gd );
    GameDocument *Begin();
    Undo         *BeginUndo();
    GameDocument *Next();
    Undo         *NextUndo();
	GameDocument *Current() { return &v[current_idx].gd; }
	Undo		 *CurrentUndo() { return &v[current_idx].undo; }
};


#endif // TABS_H
