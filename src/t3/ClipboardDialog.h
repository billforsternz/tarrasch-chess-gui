/****************************************************************************
 * Custom dialog - Clipboard browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CLIPBOARD_DIALOG_H
#define CLIPBOARD_DIALOG_H
#include "wx/wx.h"
#include "GameDocument.h"
#include "PgnDialog.h"

// ClipboardDialog class declaration
class ClipboardDialog: public PgnDialog
{
public:
    
    // Constructors
    ClipboardDialog
    (
     wxWindow    *parent,
     GamesCache  *gc,
     GamesCache  *gc_clipboard,
     wxWindowID  id,
     const wxPoint& pos = wxDefaultPosition,
     const wxSize& size = wxDefaultSize,
     long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
     );
    
    // Overrides
    virtual wxSizer *GdvAddExtraControls();
    virtual void GdvCheckBox( bool checked );

};


#endif    // CLIPBOARD_DIALOG_H
