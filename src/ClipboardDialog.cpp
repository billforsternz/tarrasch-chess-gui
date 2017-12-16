/****************************************************************************
 * Custom dialog - Clipboard browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "GameLogic.h"
#include "ClipboardDialog.h"

// ClipboardDialog constructors
ClipboardDialog::ClipboardDialog
(
 wxWindow    *parent,
 GamesCache  *gc,
 GamesCache  *gc_clipboard,
 wxWindowID  id,
 const wxPoint& pos,
 const wxSize& size
 ) : PgnDialog( parent, gc, gc_clipboard, id, pos, size)
{
}

wxSizer *ClipboardDialog::GdvAddExtraControls( bool big_display )
{
    wxSizer *vsiz_panel_buttons_ = PgnDialog::GdvAddExtraControls(big_display);
    filter_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX,
                                 wxT("&Clipboard as temporary database"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_buttons_->Add(filter_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    filter_ctrl->SetValue( objs.gl->db_clipboard );
    return vsiz_panel_buttons_;
}

void ClipboardDialog::GdvCheckBox( bool checked )
{
    objs.gl->db_clipboard = checked;
}

void ClipboardDialog::GdvEnumerateGames()
{
}

