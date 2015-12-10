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
            ListableGame *ptr = gc->gds[i].get();
            if( ptr && ptr->IsModified() )
            {
                modified = true;
                break;
            }
        }
    }
    return modified;
}

bool CentralWorkSaver::TestGameModified()
{
    return gd->game_prefix_edited || gd->game_details_edited || undo->IsModified();
}

bool CentralWorkSaver::TestGameInFile()
{
    bool in_file=false;
    for( unsigned int i=0; i<gc->gds.size(); i++ )
    {
        ListableGame *ptr = gc->gds[i].get();
        if( ptr && ptr->GetGameBeingEdited()==gd->game_being_edited && gd->game_being_edited )
        {
            in_file = true;
            break;
        }
    }
    return in_file;
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
    objs.gl->IndicateNoCurrentDocument();
    gd->in_memory = true;
    uint32_t temp = ++objs.gl->game_being_edited_tag;
    gd->SetGameBeingEdited(temp);
    gd->modified = false;           // ...modified=false, which could mean the game
                                    // not getting to log or session later (not satisfactory I know - too many flags)
    int nbr = gc->gds.size();
    gd->game_nbr = nbr + 1;
    make_smart_ptr( GameDocument, new_smart_ptr, *gd );  // smart_ptr event: document->cache
    gc->gds.push_back( std::move(new_smart_ptr) );
    objs.gl->file_game_idx = gc->gds.size()-1;
    objs.gl->tabs->SetInfile(true);
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
            msg = "Save game ?";
        }
        else
        {
            msg = "Save game to ";
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
        wxString msg = "Save changes to ";
        msg += gc->pgn_filename;
        msg += " ?";
        int answer = wxMessageBox( msg, "Confirm",  wxYES_NO|wxCANCEL, objs.frame );
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
                    gc->FileSave( gc_clipboard );
                    gd->modified = false;
                    gd->game_prefix_edited = false;
                    gd->game_details_edited = false;
                    undo->Clear(*gd);
                }
                else
                {
                    ok = false;
                    wxFileDialog fd( objs.frame, "Create new .pgn file, or append games to existing .pgn file", "", "", "*.pgn", wxFD_SAVE ); //|wxFD_CHANGE_DIR );
                    wxString dir = objs.repository->nv.m_doc_dir;
                    fd.SetDirectory(dir);
                    int answer = fd.ShowModal();
                    if( answer == wxID_CANCEL )
                        any_cancel = true;
                    if( answer == wxID_OK)
                    {
                        wxString dir;
                        wxFileName::SplitPath( fd.GetPath(), &dir, NULL, NULL );
                        objs.repository->nv.m_doc_dir = dir;
                        wxString wx_filename = fd.GetPath();
                        std::string filename( wx_filename.c_str() );
                        if( !::wxFileExists(wx_filename ) )
                        {
                            if( fm == FILE_EXISTS_GAME_NEW )
                                AddGameToFile();
                            else if( fm == FILE_EXISTS_GAME_MODIFIED )
                                PutBackDocument();
                            gc->FileSaveAs( filename, gc_clipboard );
                            gd->modified = false;
                            gd->game_prefix_edited = false;
                            gd->game_details_edited = false;
                            undo->Clear(*gd);
                            ok = true;
                        }

                        // Else if it's an existing file, load it and add file, then this game to
                        //  end of a new file game list
                        else
                        {
                            if( fm == FILE_EXISTS_GAME_NEW )
                                AddGameToFile();
                            else if( fm == FILE_EXISTS_GAME_MODIFIED )
                                PutBackDocument();
                            gc->Debug( "Games that are about to be added to an existing file" );
                            std::vector< smart_ptr<GameDocument> > gds_temp; // FIXME! = gc->gds;   // copy existing file of games to temp
                            if( !gc->Load(filename) )
                            {
                                wxString msg="Cannot append to existing file ";
                                msg += wx_filename;
                                wxMessageBox( msg, "Error reading file", wxOK|wxICON_ERROR );
                                //FIXME FIX ME  gc->gds = gds_temp;
                            }
                            else
                            {
                                gc->Debug( "The existing file" );
                                int existing_nbr = gc->gds.size();
                                
                                // Append copied temp games to newly loaded file
                                for( int i=0; i<gds_temp.size(); i++ )
                                {
                                    //? gc_vecGet( i, GameDocument, doc ); 
                                    GameDocument doc; //? = * std::dynamic_pointer_cast<GameDocument> (gds_temp[i]);
                                    doc.game_nbr = existing_nbr + i;
                                    if( doc.game_being_edited )
                                    {
                                        gd->game_nbr = doc.game_nbr+1;  // todo: why +1 ?
                                        objs.gl->file_game_idx = doc.game_nbr;
                                    }
                                    //? gc->Put( GameDocument, doc ); 
                                    make_smart_ptr( GameDocument, new_smart_ptr, doc );
                                    gc->gds.push_back( std::move(new_smart_ptr) );
                                }
                                gc->FileSave( gc_clipboard );
                                gc->KillResumePreviousWindow();
                                gd->modified = false;
                                gd->game_prefix_edited = false;
                                gd->game_details_edited = false;
                                undo->Clear(*gd);
                                ok = true;
                            }
                            gc->Debug( "After adding games to an existing file" );
                        }
                    }
                }
                break;
            }

            case FILE_NEW_GAME_NEW:
            {
                ok = false;
                wxFileDialog fd( objs.frame, "Save either to a new or existing .pgn file", "", "", "*.pgn", wxFD_SAVE ); //|wxFD_CHANGE_DIR );
                wxString dir = objs.repository->nv.m_doc_dir;
                fd.SetDirectory(dir);
                int answer = fd.ShowModal();
                if( answer == wxID_CANCEL )
                    any_cancel = true;
                if( answer == wxID_OK)
                {
                    wxString dir;
                    wxFileName::SplitPath( fd.GetPath(), &dir, NULL, NULL );
                    objs.repository->nv.m_doc_dir = dir;
                    gd->game_nbr = 0;
                    wxString wx_filename = fd.GetPath();
                    std::string filename( wx_filename.c_str() );

                    // If it's a new file, create with single game
                    if( !::wxFileExists( wx_filename ) )
                    {
                        AddGameToFile();
                        gc->FileCreate( filename, *gd );
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
                        if( !gc->Load(filename) )
                        {
                            wxString msg="Cannot append to existing file ";
                            msg += wx_filename;
                            wxMessageBox( msg, "Error reading file", wxOK|wxICON_ERROR );
                        }
                        else
                        {
                            AddGameToFile();
                            gc->FileSave( gc_clipboard );
                            gd->modified = false;
                            gd->game_prefix_edited = false;
                            gd->game_details_edited = false;
                            undo->Clear(*gd);
                            ok = true;
                        }
                    }
                }
                break;
            }
        }
    }
}

// Save prompt is set by caller to indicate the user has not chosen a save or save as command and
//  so needs to be prompted to save (eg user has gone File New, prompt him to save existing work)
void CentralWorkSaver::Save( bool prompt, bool save_as, bool open_file )
{
    bool file_exists   = TestFileExists();
    bool game_modified = TestGameModified();
    bool file_modified = TestFileModified();
    bool game_in_file  = TestGameInFile();
    if( !game_modified )
    {
        if( !prompt ) 
        {
            // if we are being asked to save, then save
            SaveFile(prompt,file_exists?FILE_EXISTS_GAME_UNCHANGED:FILE_NEW_GAME_NEW,save_as);
        }
        else
        {
            // else only prompt if file modified
            if( file_exists && file_modified )
                SaveFile(prompt,FILE_EXISTS_GAME_UNCHANGED,save_as);
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
    Save(true,false);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileNew()
{
    any_cancel = false;
    Save(true,false);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileOpen()
{
    any_cancel = false;
    Save(true,false,true);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileSave()
{
    any_cancel = false;
    Save(false,false);
    bool okay = !any_cancel;
    return okay; 
}

bool CentralWorkSaver::FileSaveAs()
{
    any_cancel = false;
    Save(false,true);
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
            wxString dir;
            wxFileName::SplitPath( fd.GetPath(), &dir, NULL, NULL );
            objs.repository->nv.m_doc_dir = dir;
            wx_filename = fd.GetPath();
            
            // If file exists, append or replace
            if( ::wxFileExists(wx_filename ) )
            {
                int answer = wxMessageBox( "Append game to file ?", "Yes to append, no to replace",  wxYES_NO|wxCANCEL, objs.frame );
                if( answer == wxYES )
                    append = true;
                else if( answer == wxCANCEL )
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
        file = fopen( wx_filename.c_str(), append ? "ab" : "wb" );
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
