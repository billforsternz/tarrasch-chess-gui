/****************************************************************************
 * Central Work Saver  This object tracks the top level state, e.g
 *                     whether we are editing a file or not, for the purpose
 *                     of saving our work.
 *                     Both prompted saves (eg when user opens a new file) and
 *                     unprompted saves (on File Save or SaveAs) are
 *                     implemented
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef TOP_LEVEL_CONTEXT_H
#define TOP_LEVEL_CONTEXT_H
#include "Undo.h"
#include "GameDocument.h"
#include "GamesCache.h"

class CentralWorkSaver
{    
public:
	CentralWorkSaver() { any_cancel=false; save_orphans=false; just_closing_tab=false; }
    void Init( Undo *undo_, GameDocument *gd_, GamesCache *gc_, GamesCache *gc_clipboard_ ) {this->undo=undo_, this->gd=gd_; this->gc=gc_; this->gc_clipboard=gc_clipboard_;}
    void SetTitle();
    bool FileNew();
    bool FileOpen();
    bool FileSave();
    bool FileSaveAs();
    bool FileSaveGameAs();
    bool TabClose();
    bool GameNew();
    bool PositionNew();
    void AddTabToFile();
    bool Exit();

private:
    bool TestFileExists();
    bool TestFileModified();
    bool TestGameModified();
    bool TestGameInFile();
	bool TestModifiedOrphanTabs( int &nbr_orphans );
    void AddGameToFile();
    void PutBackDocument();
	void AddOrphansToFile();

    enum FILE_MODE
    {
        FILE_EXISTS_GAME_MODIFIED,
        FILE_EXISTS_GAME_UNCHANGED,
        FILE_EXISTS_GAME_NEW,
        FILE_NEW_GAME_NEW,
    };

    void Save( bool prompt, bool save_as, bool open_file );

    // Prompt user whether to save game, allow game detail editing, returns wxYES or wxNO or wxCANCEL
    int SaveGamePrompt( bool prompt, FILE_MODE fm, bool save_as );

    // Save file
    void SaveFile( bool prompt, FILE_MODE fm, bool save_as );

    Undo         *undo;
    GameDocument *gd;
    GamesCache   *gc;
    GamesCache   *gc_clipboard;
    bool         any_cancel;
	bool		 save_orphans;
	bool		 just_closing_tab;
};

#endif    // TOP_LEVEL_CONTEXT_H
