/****************************************************************************
 * Custom dialog - Clipboard browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
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
 const wxSize& size,
 long style
 ) : PgnDialog( parent, gc, gc_clipboard, id, pos, size, style )
{
}

wxSizer *ClipboardDialog::GdvAddExtraControls()
{
    wxSizer *vsiz_panel_button1 = PgnDialog::GdvAddExtraControls();
    filter_ctrl = new wxCheckBox( this, ID_DB_CHECKBOX,
                                 wxT("&Clipboard as temp database"), wxDefaultPosition, wxDefaultSize, 0 );
    vsiz_panel_button1->Add(filter_ctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    filter_ctrl->SetValue( objs.gl->db_clipboard );
    return vsiz_panel_button1;
}

void ClipboardDialog::GdvCheckBox( bool checked )
{
    objs.gl->db_clipboard = checked;
}

void ClipboardDialog::GdvEnumerateGames()
{
}

