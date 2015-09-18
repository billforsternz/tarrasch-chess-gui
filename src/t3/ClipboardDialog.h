/****************************************************************************
 * Custom dialog - Clipboard browser
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CLIPBOARD_DIALOG_H
#define CLIPBOARD_DIALOG_H
#include <map>
#include <list>
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/accel.h"
#include "SuspendEngine.h"
#include "GamesCache.h"
#include "GamesDialog.h"
#include "PgnDialog.h"
#include "GameDocument.h"
#include "Repository.h"


#if 1
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
    
};

#else
class wxVirtualClipboardListCtrl;


// ClipboardDialog class declaration
class ClipboardDialog: public GamesDialog
{    
public:
    GameDocument *GetCachedDocument( int idx );
    void GetCachedDocumentRaw( int idx, GameDocument &gd );
    std::map<int,GameDocument>     local_cache;
    std::list<int>                 stack;

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
    virtual void AddExtraControls();
    virtual void GetButtonGridDimensions( int &row1, int &col1, int &row2, int &col2 ) { row1=8; col1=2; row2=0; col2=0; }
    virtual void ReadItem( int item, CompactGame &info );
    virtual void OnListColClick( int compare_col );
    virtual void OnSaveAllToAFile();
    virtual void OnHelpClick();
    virtual void OnSearch();
    virtual void OnUtility();
    virtual void OnCancel();
    virtual void OnNextMove( int idx );


    // Return true if a game has been selected
    bool LoadGame( GameLogic *gl, GameDocument& gd, int &file_game_idx );
    void SyncCacheOrderBefore();
    void SyncCacheOrderAfter();


    // ClipboardDialog member variables
public:

private:

    // Data members
    int file_game_idx;
};
#endif

#endif    // CLIPBOARD_DIALOG_H
