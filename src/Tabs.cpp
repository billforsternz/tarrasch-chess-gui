/****************************************************************************
 * Implements separate games on tabbed pages
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/richtext/richtextctrl.h"
#include "GameLogic.h"
#include "Objects.h"
#include "Tabs.h"

BEGIN_EVENT_TABLE(NotebookPage,wxTextCtrl)
	EVT_SET_FOCUS(NotebookPage::RedirectFocus)
END_EVENT_TABLE()

void NotebookPage::RedirectFocus(wxFocusEvent& WXUNUSED(event))
{
	objs.gl->SetFocusOnList();
}

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
        wxRichTextCtrl *ctrl = objs.canvas->lb;
        int x=0, y=0;
        if( ctrl )
            ctrl->GetScrollHelper()->GetViewStart( &x, &y );
        v[current_idx].normal_orientation = objs.canvas->GetNormalOrientation();
        v[current_idx].scroll_x = x;
        v[current_idx].scroll_y = y;
        v[current_idx].undo = gl->undo;
        cprintf( "Creating new tab, current_idx=%d, [0]undo=%s [0]redo=%s, old undo=%s old redo=%s\n", current_idx,
                v[0].undo.IsModified()?"yes":"no", v[0].undo.CanRedo()?"yes":"no",
                gl->undo.IsModified()?"yes":"no", gl->undo.CanRedo()?"yes":"no" );
        gl->gd = new_gd;
        gl->undo = e.undo; // blank
        current_idx = nbr_tabs-1;
        NotebookPage *notebook_page1 = new NotebookPage(objs.canvas->notebook, wxID_ANY,wxDefaultPosition, wxDefaultSize);
        //wxWindow *notebook_page1 = new wxWindow(objs.canvas->notebook, wxID_ANY );
        objs.canvas->notebook->AddPage(notebook_page1,"New Game",true);
    }
    cprintf( "New tab idx=%d, pos=%ld\n", current_idx, pos );
}


bool Tabs::TabSelected( int idx )
{
    bool okay=false;
    if( idx < nbr_tabs )
    {
        okay = true;

        // If reverting to an old tab (as opposed to switching to a new one) save
        //  the current insertion point, doc and undo and restore the saved
        //  insertion point doc and undo
        unsigned long pos;
        if( idx != current_idx )
        {
            pos = gl->gd.GetInsertionPoint();
            v[current_idx].gd  = gl->gd;
            v[current_idx].pos = pos;
            v[current_idx].undo = gl->undo;
            wxRichTextCtrl *ctrl = objs.canvas->lb;
            int x=0, y=0;
            if( ctrl )
                ctrl->GetScrollHelper()->GetViewStart( &x, &y );
            v[current_idx].normal_orientation = objs.canvas->GetNormalOrientation();
            v[current_idx].scroll_x = x;
            v[current_idx].scroll_y = y;
            gl->gd = v[idx].gd;
            gl->undo = v[idx].undo;
            pos = v[idx].pos;
            gl->gd.non_zero_start_pos = pos;
            objs.canvas->SetNormalOrientation(v[idx].normal_orientation);
            if( ctrl )
                ctrl->GetScrollHelper()->Scroll( v[idx].scroll_x, v[idx].scroll_y );
            current_idx = idx;
            cprintf( "Set difference tab idx=%d, [0]undo=%s [0]redo=%s, new undo=%s new redo=%s\n", idx,
                        v[0].undo.IsModified()?"yes":"no", v[0].undo.CanRedo()?"yes":"no",
                        gl->undo.IsModified()?"yes":"no", gl->undo.CanRedo()?"yes":"no" );
        }
    }
    return okay;
}

// Walk through the tabs. Initiate (and set exclude_foreground parameter to indicate whether
//  you want to exclude the current tab) with handle=0, then use return value as handle for
//  subsequent calls until gd (and undo) return as NULL. The handle mechanism allows
//  nesting, i.e. more than one Iterate() series can occur at once.
int Tabs::Iterate( int handle, GameDocument *&gd, Undo *&undo, bool exclude_foreground )
{
	if( handle == 0 )
	{
		handle = next_handle++;
		if( next_handle >= sizeof(iter_buf)/(sizeof(iter_buf[0])) )
			next_handle = 1;
		iter_buf[handle] = 0;
		iter_exclude_foreground[handle] = exclude_foreground;
	}
	int iter = iter_buf[handle];
	if( iter_exclude_foreground[handle] && iter==current_idx )
		iter++;
    int sz=v.size();
    if( 0<=iter && iter<sz )
    {
        gd   = iter==current_idx ? &gl->gd : &v[iter].gd;
        undo = iter==current_idx ? &gl->undo : &v[iter].undo;
		pos_iterate = (iter==current_idx ? gl->gd.GetInsertionPoint() : v[iter].pos);
		iter_buf[handle] = ++iter;
    }
	else
	{
        gd   = NULL;
        undo = NULL;
	}
	return handle;
}


void Tabs::SetTitle( GameDocument &gd )
{
    const int TAB_TITLE_LENGTH=20;
    std::string s("New Game");
    if( gd.r.white.length() > 0 )
    {
        s = gd.r.white + "-" + gd.r.black;
    }
    else
    {
        gd.gv.ToString( s );
		if( s.length()>1 && s[s.length()-1] == '*' )
			s.pop_back();	// remove '*' at end
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
    {
        cprintf( "Tabs::SetTitle(%d,%s)\n", current_idx, s.c_str() );
        objs.canvas->notebook->SetPageText( current_idx, ws );
    }
}

#ifdef AUI_NOTEBOOK
void Tabs::TabDelete( int idx, bool system_will_delete_notebook_page )
{
    if( nbr_tabs>1 && idx<nbr_tabs )
    {
        int idx_to_delete = idx;
        v.erase( v.begin() + idx );
        for( size_t i=0; i<v.size(); i++ )
        {
            GameDocument gd =v[i].gd;
            cprintf( "Tab %d: %s-%s\n", i, gd.White(), gd.Black() );
        }
        nbr_tabs--;
        if( current_idx == nbr_tabs )
            current_idx--;
        if( idx == nbr_tabs )
            idx--;
        if( !system_will_delete_notebook_page )
            objs.canvas->notebook->DeletePage( idx_to_delete );
    }
}

#endif
int Tabs::TabDelete()
{
    if( nbr_tabs>1 && current_idx<nbr_tabs )
    {
        int temp = current_idx;
        objs.canvas->notebook->DeletePage( current_idx );   // changes current_idx
        v.erase( v.begin() + temp );
        nbr_tabs--;
        if( current_idx == nbr_tabs )
            current_idx--;
        int idx = current_idx;
        gl->undo = v[idx].undo;
        gl->gd = v[idx].gd;
        unsigned long pos = v[idx].pos;
        gl->gd.SetInsertionPoint(pos);
        gl->gd.non_zero_start_pos = pos;
        objs.canvas->SetNormalOrientation(v[idx].normal_orientation);
        wxRichTextCtrl *ctrl = objs.canvas->lb;
        if( ctrl )
            ctrl->GetScrollHelper()->Scroll( v[idx].scroll_x, v[idx].scroll_y );
    }
    return( current_idx );
}

