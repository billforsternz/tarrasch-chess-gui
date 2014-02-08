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
    bool infile;
    TabElement(GameLogic *gl) : undo(gl) { pos=0; infile=false; }
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
    void TabNew( GameDocument &new_gd );
    bool TabSelected( int idx );
    int  TabDelete();
    void SetInfile( bool infile ) { if( current_idx<nbr_tabs ) v[current_idx].infile = infile; }
    bool GetInfile() { return current_idx<nbr_tabs ? v[current_idx].infile : false; }
    void SetTitle( GameDocument &gd );
    GameDocument *Begin();
    Undo         *BeginUndo();
    GameDocument *Next();
    Undo         *NextUndo();
};


#endif // TABS_H
