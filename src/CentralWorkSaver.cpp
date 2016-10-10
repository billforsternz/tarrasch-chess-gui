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
#include "wx/wx.h"
#include "Appdefs.h"
#include "DebugPrintf.h"
#include "Session.h"
#include "Log.h"
#include "Repository.h"
#include "Objects.h"
#include "GameDetailsDialog.h"
#include "GameLogic.h"
#include "Tabs.h"
#include "CentralWorkSaver.h"

/*
Some use cases;
    enter moves save
    enter moves save as existing
    enter moves save as new
    open file load edit save
    open file load edit save as existing
    open file load edit save as new
    open file load edit save as same
    pick game from session edit save
    pick game from session edit save as existing
    pick game from session edit save as new
    pick game from session edit save as same
    cancel at any time in any of the above
    any of the above after any of the above
*/

void CentralWorkSaver::SetTitle()
{
    if( gc->pgn_filename != "" )
        objs.frame->SetTitle( gc->pgn_filename.c_str() );
}

bool CentralWorkSaver::TestFileExists()
{
    return( gc->pgn_filename != "" );
}

bool CentralWorkSaver::TestFileModified()
{
    bool modified=false;
    if( gc->pgn_filename != "" )
    {
        modified = gc->file_irrevocably_modified;
        for( unsigned int i=0; !modified && i<gc->gds.size(); i++ )
        {
			// Is any game sitting modified in the cache ?
            ListableGame *ptr = gc->gds[i].get();
            if( ptr && ptr->IsModified() )
            {
                modified = true;
                break;
            }

			// Is the file game sitting modified in a tab ?
			uint32_t game_being_edited = ptr->GetGameBeingEdited();
			if( game_being_edited )
			{
				Tabs *tabs = objs.gl->tabs;
				GameDocument *pd;
				Undo *pu;
				int handle = tabs->Iterate(0,pd,pu);
				while( pd && pu )
				{
					if( game_being_edited == pd->game_being_edited )
					{
						if( pd->IsModified() || pu->IsModified() )
							modified = true;
						break;
					}
					tabs->Iterate(handle,pd,pu);
				}
			}
        }
    }
    return modified;
}

bool CentralWorkSaver::TestGameModified()
{
    return gd->modified || gd->game_prefix_edited || gd->game_details_edited || undo->IsModified();
}

bool CentralWorkSaver::TestGameInFile()
{
    return gc->TestGameInCache(*gd);
}

// Is any modified game in a non current tab, but not in the file ?
bool CentralWorkSaver::TestModifiedOrphanTabs( int &nbr_orphans )
{
	nbr_orphans = 0;
	Tabs *tabs = objs.gl->tabs;
	bool exclude_current_tab=true;
	GameDocument *pd;
	Undo *pu;
	int handle = tabs->Iterate(0,pd,pu,exclude_current_tab);
	while( pd && pu )
	{
		bool modified = pd->IsModified() || pu->IsModified();
		if( modified && !gc->TestGameInCache(*pd) )
			nbr_orphans++;
		tabs->Iterate(handle,pd,pu);
	}
	return nbr_orphans>0;
}

void CentralWorkSaver::AddOrphansToFile()
{
	Tabs *tabs = objs.gl->tabs;
	GameDocument *pd;
	Undo *pu;
	bool exclude_current_tab=true;
	int handle = tabs->Iterate(0,pd,pu,exclude_current_tab);
	while( pd && pu )
	{
		bool modified = pd->IsModified() || pu->IsModified();
		if( modified && !gc->TestGameInCache(*pd) )
		{
			// Copy and paste a subset of AddGameToFile()
			pd->pgn_handle = 0;
			pd->game_id = GameIdAllocateBottom(1);
			make_smart_ptr( GameDocument, new_smart_ptr, *pd );
			gc->gds.push_back( std::move(new_smart_ptr) );
		}
		tabs->Iterate(handle,pd,pu);
	}
}

void CentralWorkSaver::AddTabToFile()
{
    //if( !TestGameInFile() )
    {
        if( gd->r.white == "" ) // if( !game_details_edited )
        {
            GameDetailsDialog dialog(objs.frame);
            if( dialog.Run(*gd) )
                objs.gl->GameRedisplayPlayersResult();
        }
        AddGameToFile();
    }
}

// When saving a game, either add it to file ...(see below)
void CentralWorkSaver::AddGameToFile()
{
    gd->pgn_handle = 0;
    objs.session->SaveGame(gd);     // must do this...
    bool editing_log = objs.gl->EditingLog();
    objs.log->SaveGame(gd,editing_log);         // ...and this now because we need to set...
    gd->modified = false;           // ...modified=false, which could mean the game
                                    // not getting to log or session later (not satisfactory I know - too many flags)
    int nbr = gc->gds.size();
    gd->game_id = GameIdAllocateBottom(1);
    make_smart_ptr( GameDocument, new_smart_ptr, *gd );  // smart_ptr event: document->cache
    gc->gds.push_back( std::move(new_smart_ptr) );
    nbr = gc->gds.size();

	// #Workflow
	// Set edit correspondence between currently edited document and game added to end of file
    objs.gl->SetGameBeingEdited(*gd, *gc->gds[nbr-1] );
}

// ...(see above) Or put it back where it came from
void CentralWorkSaver::PutBackDocument()
{
    objs.gl->PutBackDocument();
}


// Prompt user whether to save game, allow game detail editing, returns wxYES or wxNO or wxCANCEL
int CentralWorkSaver::SaveGamePrompt( bool prompt, FILE_MODE fm, bool save_as )
{
    int answer = wxYES;

    // Save game
    if( prompt )
    {
        wxString msg;
        if( save_as || fm==FILE_NEW_GAME_NEW )
        {
            msg = save_orphans?"Save game and other modified tabs?":"Save game ?";
        }
        else
        {
            msg = save_orphans?"Save game and other modified tabs to ":"Save game to ";
            msg += gc->pgn_filename;
            msg += " ?";
        }
        answer = wxMessageBox( msg, "Yes to save, No to discard",  wxYES_NO|wxCANCEL, objs.frame );
    }
    if( answer == wxYES )
    {
        if( gd->r.white == "" ) // if( !game_details_edited )
        {
            GameDetailsDialog dialog(objs.frame);
            if( dialog.Run(*gd) )
                objs.gl->GameRedisplayPlayersResult();
            else
                answer = wxCANCEL;
        }
    }
    if( answer == wxCANCEL )
        any_cancel = true;
    return answer;
}

// Save file, return ok (false if save canceled)
void CentralWorkSaver::SaveFile( bool prompt, FILE_MODE fm, bool save_as )
{
    bool ok=true;
    if( prompt )
    {
        wxString msg = "Save modified games to ";
        msg += gc->pgn_filename;
        msg += " ?";
        int answer = wxMessageBox( msg, "'Yes' to save, 'No' to discard (be careful with 'No')",  wxYES_NO|wxCANCEL, objs.frame );

		// If discard, save orphans to log
        if( answer==wxNO && save_orphans )
		{
			Tabs *tabs = objs.gl->tabs;
			GameDocument *pd;
			Undo *pu;
			bool exclude_current_tab=true;
			int handle = tabs->Iterate(0,pd,pu,exclude_current_tab);
			while( pd && pu )
			{
				bool modified = pd->IsModified() || pu->IsModified();
				if( modified && !gc->TestGameInCache(*pd) )
				{
					bool editing_log = objs.gl->EditingLog();
					objs.log->SaveGame(pd,editing_log);
				}
				tabs->Iterate(handle,pd,pu);
			}
		}
        if( answer == wxCANCEL )
            any_cancel = true;
        if( answer != wxYES )
            ok = false;
    }
    if( ok )
    {
        switch( fm )
        {
            case FILE_EXISTS_GAME_UNCHANGED:
            case FILE_EXISTS_GAME_MODIFIED:
            case FILE_EXISTS_GAME_NEW:
            {
                if( !save_as )
                {
                    if( fm == FILE_EXISTS_GAME_NEW )
                        AddGameToFile();
                    else if( fm == FILE_EXISTS_GAME_MODIFIED )
                        PutBackDocument();
					if( save_orphans )
						AddOrphansToFile();
                    gc->FileSave( gc_clipboard );
                    gd->modified = false;
                    gd->game_prefix_edited = false;
                    gd->game_details_edited = false;
                    undo->Clear(*gd);
                }
                else
                {
                    ok = false;
                    wxFileDialog fd( objs.frame, "Select .pgn file to create, replace or append to", "", "", "*.pgn", wxFD_SAVE ); //|wxFD_CHANGE_DIR );
                    wxString dir = objs.repository->nv.m_doc_dir;
                    fd.SetDirectory(dir);
                    int answer = fd.ShowModal();
                    if( answer == wxID_CANCEL )
                        any_cancel = true;
                    if( answer == wxID_OK)
                    {
                        wxString dir2;
                        wxFileName::SplitPath( fd.GetPath(), &dir2, NULL, NULL );
                        objs.repository->nv.m_doc_dir = dir2;
                        wxString wx_filename = fd.GetPath();
                        std::string filename( (const char*)wx_filename.c_str() );
                        if( !::wxFileExists(wx_filename ) )
                        {
                            if( fm == FILE_EXISTS_GAME_NEW )
                                AddGameToFile();
                            else if( fm == FILE_EXISTS_GAME_MODIFIED )
                                PutBackDocument();
							if( save_orphans )
								AddOrphansToFile();
                            gc->FileSaveAs( filename, gc_clipboard );
		                    gd->modified = false;
                            gd->game_prefix_edited = false;
                            gd->game_details_edited = false;
                            undo->Clear(*gd);
                            ok = true;
                        }

                        // Else if it's an existing file, optionally load it ahead of existing pgn cache
                        else
                        {
                            ok = true;
                            bool append=false;
                            int answer2 = wxMessageBox( "Append games to existing file ?", "'Yes' to append, 'No' to replace (be careful with 'No')",  wxYES_NO|wxCANCEL, objs.frame );
                            if( answer2 == wxYES )
                                append = true;
                            else if( answer2 == wxCANCEL )
                                ok = false;
                            if( ok )
                            {
                                if( fm == FILE_EXISTS_GAME_NEW )
                                    AddGameToFile();
                                else if( fm == FILE_EXISTS_GAME_MODIFIED )
                                    PutBackDocument();
								if( save_orphans )
									AddOrphansToFile();
                            }
                            if( ok && append )
                            {
                                std::vector< smart_ptr<ListableGame> > gds_temp = gc->gds;   // copy existing file of games to temp
                                if( !gc->Load(filename) )
                                {
                                    wxString msg="Cannot append to existing file ";
                                    msg += wx_filename;
                                    wxMessageBox( msg, "Error reading file", wxOK|wxICON_ERROR );
                                    gc->gds = gds_temp;
                                    ok = false;
                                }
                                else
                                {
                                    // Append copied temp games to newly loaded file
                                    for( size_t i=0; i<gds_temp.size(); i++ )
                                    {
                                        smart_ptr<ListableGame> smp = gds_temp[i];
                                        gc->gds.push_back( smp );
                                    }
                                }
                            }
                            if( ok )
                            {
                                if( append )
                                    gc->FileSave( gc_clipboard );
                                else
                                    gc->FileSaveAs( filename, gc_clipboard );
			                    gd->modified = false;
                                gd->game_prefix_edited = false;
                                gd->game_details_edited = false;
                                undo->Clear(*gd);
                            }
                        }
                    }
                }
                break;
            }

            case FILE_NEW_GAME_NEW:
            {
                ok = false;
                wxFileDialog fd( objs.frame, "Select .pgn file to create, replace or append to", "", "", "*.pgn", wxFD_SAVE ); //|wxFD_CHANGE_DIR );
                wxString dir = objs.repository->nv.m_doc_dir;
                fd.SetDirectory(dir);
                int answer = fd.ShowModal();
                if( answer == wxID_CANCEL )
                    any_cancel = true;
                if( answer == wxID_OK)
                {
                    wxString dir2;
                    wxFileName::SplitPath( fd.GetPath(), &dir2, NULL, NULL );
                    objs.repository->nv.m_doc_dir = dir2;
                    wxString wx_filename = fd.GetPath();
                    std::string filename( (const char *)wx_filename.c_str() );

                    // If it's a new file, create with single game
                    if( !::wxFileExists( wx_filename ) )
                    {
                        AddGameToFile();
						if( save_orphans )
							AddOrphansToFile();
                        gc->FileCreate( filename );
	                    gd->modified = false;
                        gd->game_prefix_edited = false;
                        gd->game_details_edited = false;
                        undo->Clear(*gd);
                        ok = true;
                    }

                    // Else if it's an existing file, load it and add this game to
                    //  end of a new file game list
                    else
                    {
                        ok = true;
                        bool append=false;
                        int answer2 = wxMessageBox( "Append to existing file ?", "'Yes' to append, 'No' to overwrite (be careful with 'No')",  wxYES_NO|wxCANCEL, objs.frame );
                        if( answer2 == wxYES )
                            append = true;
                        else if( answer2 == wxCANCEL )
                            ok = false;
                        if( ok && append )
                        {
                            if( !gc->Load(filename) )
                            {
                                wxString msg="Cannot append to existing file ";
                                msg += wx_filename;
                                wxMessageBox( msg, "Error reading file", wxOK|wxICON_ERROR );
                                ok = false;
                            }
                        }
                        if( ok )
                        {
                            AddGameToFile();
							if( save_orphans )
								AddOrphansToFile();
                            if( append )
                                gc->FileSave( gc_clipboard );
                            else
		                        gc->FileCreate( filename );
		                    gd->modified = false;
                            gd->game_prefix_edited = false;
                            gd->game_details_edited = false;
                            undo->Clear(*gd);
                        }
                    }
                }
                break;
            }
        }
    }
}

// Save the user's games in a file
//  prompt
//   Set to indicate the user has not chosen a save or save as command and so needs to be prompted to
//   save (eg user has gone File New, prompt him to save existing work)
//  save_as
//   Set to indicate user wants to "save as", i.e. save to a new file
//  open_file
//   Set to indicate save (may be) needed because user is opening file
void CentralWorkSaver::Save( bool prompt, bool save_as, bool open_file )
{
    bool file_exists   = TestFileExists();
    bool game_modified = TestGameModified();
    bool file_modified = TestFileModified();
    bool game_in_file  = TestGameInFile();
	if( just_closing_tab )  // if set limit how much this can save
	{
		if( !game_modified )
			return;
		file_modified = false;
	}
    if( !game_modified )
    {
        if( !prompt ) 
        {
            // if user is asking to save, then save
            SaveFile(prompt,file_exists?(game_in_file?FILE_EXISTS_GAME_UNCHANGED:FILE_EXISTS_GAME_NEW):FILE_NEW_GAME_NEW,save_as);
        }
        else
        {
            // Prompt user to save if file modified
            if( file_exists && file_modified )
                SaveFile(prompt,FILE_EXISTS_GAME_UNCHANGED,save_as);

			// Or to save orphans
			else if( save_orphans )
                SaveFile(prompt,file_exists?(game_in_file?FILE_EXISTS_GAME_UNCHANGED:FILE_EXISTS_GAME_NEW):FILE_NEW_GAME_NEW,save_as);
        }
    }
    else
    {
        if( !file_exists )
        {
            // file doesn't exist, game is modified
            if( !open_file )    // if opening a file, new tab created, old game can live alone in old tab
            {
                int answer = SaveGamePrompt(prompt,FILE_NEW_GAME_NEW,true);
                if( answer == wxYES )
                    SaveFile(false,FILE_NEW_GAME_NEW,true);
            }
        }
        else
        {
            // file exists, game is modified
            FILE_MODE fm = game_in_file ? FILE_EXISTS_GAME_MODIFIED : FILE_EXISTS_GAME_NEW;
            int answer = SaveGamePrompt(prompt,fm,save_as);
            if( answer != wxCANCEL )
            {
                bool file_prompt = false;   // normally prompt only on game
                if( answer == wxNO )
                {
                    file_prompt = prompt;   // but user doesn't want to save game, may need
                                            //  to ask if they want to save file
                    fm = FILE_EXISTS_GAME_UNCHANGED;
                }
                if( file_modified || answer==wxYES )
                    SaveFile( file_prompt, fm, save_as );
            }
        }
    }
}

bool CentralWorkSaver::Exit()
{
    any_cancel = false;
	int nbr_orphans;
	save_orphans = TestModifiedOrphanTabs( nbr_orphans );
	just_closing_tab = false;
	Save(true/*prompt*/,false/*save_as*/,false/*open_file*/);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::TabClose()
{
    any_cancel = false;
	save_orphans = false;
	just_closing_tab = true;
    Save(true/*prompt*/,false/*save_as*/,false/*open_file*/);
    bool okay = !any_cancel;
    return okay; 
}


bool CentralWorkSaver::FileNew()
{
    any_cancel = false;
	save_orphans = false;
	just_closing_tab = false;
    Save(true/*prompt*/,false/*save_as*/,false/*open_file*/);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileOpen()
{
    any_cancel = false;
	save_orphans = false;
	just_closing_tab = false;
    Save(true/*prompt*/,false/*save_as*/,true/*open_file*/);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileSave()
{
    any_cancel = false;
	save_orphans = false;
	just_closing_tab = false;
    Save(false/*prompt*/,false/*save_as*/,false/*open_file*/);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileSaveAs()
{
    any_cancel = false;
	save_orphans = false;
	just_closing_tab = false;
    Save(false/*prompt*/,true/*save_as*/,false/*open_file*/);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileSaveGameAs()
{
    bool ok=true;
    bool append=false;
    wxString wx_filename;
    GameDetailsDialog dialog(objs.frame);
    if( dialog.Run(*gd) )
        objs.gl->GameRedisplayPlayersResult();
    else
        ok = false;
    if( ok )
    {
        wxFileDialog fd( objs.frame, "Select .pgn file to create, replace or append to", "", "", "*.pgn", wxFD_SAVE ); //|wxFD_CHANGE_DIR );
        wxString dir = objs.repository->nv.m_doc_dir;
        fd.SetDirectory(dir);
        int answer = fd.ShowModal();
        ok = (answer==wxID_OK);
        if( ok )
        {
            wxString dir2;
            wxFileName::SplitPath( fd.GetPath(), &dir2, NULL, NULL );
            objs.repository->nv.m_doc_dir = dir2;
            wx_filename = fd.GetPath();
            
            // If file exists, append or replace
            if( ::wxFileExists(wx_filename ) )
            {
                int answer2 = wxMessageBox( "Append game to file ?", "'Yes' to append, 'No' to replace, be careful with 'No'",  wxYES_NO|wxCANCEL, objs.frame );
                if( answer2 == wxYES )
                    append = true;
                else if( answer2 == wxCANCEL )
                    ok = false;
            }
        }
    }
    if( ok )
    {
        gd->FleshOutDate();
        gd->FleshOutMoves();
        std::string head;
        gd->ToFileTxtGameDetails( head );
        std::string body;
        gd->ToFileTxtGameBody( body );
        FILE *file = NULL;
        file = fopen( (const char *)wx_filename.c_str(), append ? "ab" : "wb" );
        if( file )
        {
            fseek(file,0,SEEK_END);
            if( append )
            {
                std::string eol =
#ifdef THC_WINDOWS
                "\r\n";
#else
                "\n";
#endif
                fwrite( eol.c_str(), 1, eol.length(), file );
            }
            fwrite( head.c_str(), 1, head.length(), file );
            fwrite( body.c_str(), 1, body.length(), file );
            fclose( file );
            if( !TestGameInFile() )
            {
                gd->modified = false;
                gd->game_prefix_edited = false;
                gd->game_details_edited = false;
                undo->Clear(*gd);
            }
        }
    }
    return ok;
}


// Don't trouble the user with prompts unless he is working with a file
bool CentralWorkSaver::PositionNew() { return GameNew(); }
bool CentralWorkSaver::GameNew()
{
    any_cancel = false;
	save_orphans = false;
    bool file_exists   = TestFileExists();
    bool game_modified = TestGameModified();
    bool game_in_file  = TestGameInFile();
    if( file_exists && game_modified )
    {
        if( game_in_file )
        {
            int answer = SaveGamePrompt(true,FILE_EXISTS_GAME_MODIFIED,false);
            if( answer == wxYES )
                SaveFile(false,FILE_EXISTS_GAME_MODIFIED,false);
        }
        else
        {
            int answer = SaveGamePrompt(true,FILE_EXISTS_GAME_NEW,false);
            if( answer == wxYES )
                SaveFile(false,FILE_EXISTS_GAME_NEW,false);
        }
    }
    bool okay = !any_cancel;
    return okay; 
}
