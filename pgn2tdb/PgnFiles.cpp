/****************************************************************************
 * Provide a mechanism for locating games in one or more pgn files
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "GamesCache.h"
#include "PgnFiles.h"

// Skip over the Unicode BOM at the start of some text files
static void UnicodeBomSkip( FILE *in )
{
    unsigned char buf[4];
    int len = fread( buf, 1, 3, in );
    if( len==3 && 0==memcmp(buf,"\xef\xbb\xbf",3)  )
        ;  // File starts with UNICODE BOM, I think ChessBase does this sometimes, so skip over it
    else
        fseek(in,0,SEEK_SET); //rewind to start
}

// Start reading a file and introduce it into the system
FILE *PgnFiles::OpenRead( std::string filename, int &handle )
{
    FILE *pgn_file = NULL;
    bool available = IsAvailable( filename, handle );
    if( available )
    {
        std::map<int,PgnFile>::iterator it = files.find(handle);
        if( it != files.end() )
        {
            pgn_file = fopen( it->second.filename.c_str(), "rb" );
            if( pgn_file )
            {
                it->second.file_read = pgn_file;
                it->second.mode = PgnFile::reading;
            }
        }
    }
    else
    {
        handle = next_handle++;
        PgnFile pf;
        pf.filename = filename;
        //wxString wx_filename = filename.c_str();
        pf.file_modification_time = 0; //::wxFileModificationTime(wx_filename);
        pgn_file = fopen( pf.filename.c_str(), "rb" );
        if( pgn_file )
        {
            pf.file_read = pgn_file;
            fseek(pgn_file,0,SEEK_END);
            pf.filelen = ftell(pgn_file);
            fseek(pgn_file,0,SEEK_SET);
            pf.mode = PgnFile::reading;
            std::pair<int,PgnFile> elem(handle,pf);
            files.insert(elem);
        }
    }
    if( pgn_file )
        UnicodeBomSkip( pgn_file );
    return pgn_file;
}

// If a modified file is known, update length and time
void PgnFiles::UpdateKnownFile( std::string &filename, time_t filetime_before, long filelen_before, long delta )
{
    std::map<int,PgnFile>::iterator it;
    for( it=files.begin(); it!=files.end(); ++it )
    {
        PgnFile pf = it->second;
        if( pf.filename == filename )
        {
            if( pf.file_modification_time == filetime_before &&
                pf.filelen                == filelen_before
              )
            {
                it->second.filelen += delta;
                it->second.file_modification_time = 0; //::wxFileModificationTime(filename);
            }
            break;
        }
    }
}

// Create a file and introduce it into the system
FILE *PgnFiles::OpenCreate( std::string filename, int &handle )
{
    FILE *pgn_file = NULL;
    bool available = IsAvailable( filename, handle );
    if( available )
    {
        std::map<int,PgnFile>::iterator it = files.find(handle);
        if( it != files.end() )
            files.erase(it);
    }
    handle = next_handle++;
    pgn_file = fopen( filename.c_str(), "wb" );
    if( pgn_file )
    {
        PgnFile pf;
        pf.filename = filename;
        pf.file_write  = pgn_file;
        pf.mode = PgnFile::creating;
        std::pair<int,PgnFile> elem(handle,pf);
        files.insert(elem);
    }
    return pgn_file;
}

// Is a previously used file known and available (unmodified and ready to go)
bool PgnFiles::IsAvailable( int handle )
{
    bool available=false;
    std::map<int,PgnFile>::iterator it;
    it = files.find(handle);
    if( it != files.end() )
        available = IsAvailable(it);
    return available;
}

// Is a file already known and available (unmodified and ready to go)
bool PgnFiles::IsAvailable( std::string filename, int &handle )
{
    bool available=false;
    std::map<int,PgnFile>::iterator it;
    for( it=files.begin(); it!=files.end(); ++it )
    {
        PgnFile pf = it->second;
        if( pf.filename == filename )
        {
            handle = it->first;
            if( IsAvailable(handle) )
                available = true;
            break;
        }
    }
    return available;
}

// Is a previously used file known and available (unmodified and ready to go)
bool PgnFiles::IsAvailable( std::map<int,PgnFile>::iterator it )
{
    bool available=false;
    PgnFile pf = it->second;
    //wxString wx_filename = pf.filename.c_str();
    //time_t filetime = ::wxFileModificationTime(wx_filename);
    //if( ((long)filetime != -1L) && filetime==pf.file_modification_time )
    {
        FILE *pgn_file = fopen( pf.filename.c_str(), "rb" );
        if( pgn_file )
        {
            fseek(pgn_file,0,SEEK_END);
            long len = ftell(pgn_file);
            fseek(pgn_file,0,SEEK_SET);
            fclose(pgn_file);
            available = (len==pf.filelen);
        }
    }
    return available;
}


// Reopen a known file for reading
FILE *PgnFiles::ReopenRead( int handle )
{
    FILE *pgn_file = NULL;
    std::map<int,PgnFile>::iterator it = files.find(handle);
    if( it != files.end() )
    {
        if( it->second.mode==PgnFile::reading || it->second.mode==PgnFile::modifying || it->second.mode==PgnFile::copying )
            pgn_file = it->second.file_read;
        else
        {
            if( IsAvailable(it) )
            {
                pgn_file = fopen( it->second.filename.c_str(), "rb" );
                if( pgn_file )
                {
                    UnicodeBomSkip( pgn_file );
                    it->second.file_read  = pgn_file;
                    it->second.mode = PgnFile::reading;
                }
            }
        }
    }
    return pgn_file;
}

// Reopen a known file for modification
bool PgnFiles::ReopenModify( int handle, FILE * &pgn_in, FILE * &pgn_out, GamesCache *gc_clipboard  )
{
    bool ok = IsAvailable( handle );
    if( ok )
    {
        ok = false;

        // Avoid orphaning clipboard games, step1
        if( gc_clipboard )
        {
            int sz=gc_clipboard->gds.size();
            for( int i=0; i<sz; i++ )
                gc_clipboard->gds[i]->saved = false;
        }

        std::map<int,PgnFile>::iterator it = files.find(handle);
        if( it != files.end() )
        {
            pgn_in = fopen( it->second.filename.c_str(), "rb" );
            if( pgn_in )
            {
                UnicodeBomSkip( pgn_in );
                it->second.file_read  = pgn_in;
                //wxString wx_filename_in  = it->second.filename.c_str();
                //wxString wx_filename_out = wxFileName::CreateTempFileName(wx_filename_in);
                it->second.filename_temp = it->second.filename + ".temp"; // wx_filename_out.c_str();
                pgn_out = fopen( it->second.filename_temp.c_str(), "wb" );
                if( !pgn_out )
                    fclose( pgn_in );
                else
                {
                    ok = true;
                    it->second.file_write = pgn_out;
                    it->second.mode = PgnFile::modifying;
                }
            }
        }
    }
    return ok;
}

// Reopen a known file for copy
bool PgnFiles::ReopenCopy( int handle, std::string new_filename, FILE * &pgn_in, FILE * &pgn_out, GamesCache *gc_clipboard )
{
    bool ok = IsAvailable( handle );
    if( ok )
    {
        ok = false;

        // Avoid orphaning clipboard games, step1
        if( gc_clipboard )
        {
            int sz=gc_clipboard->gds.size();
            for( int i=0; i<sz; i++ )
                gc_clipboard->gds[i]->saved = false;
        }


        std::map<int,PgnFile>::iterator it = files.find(handle);
        if( it != files.end() )
        {
            pgn_in = fopen( it->second.filename.c_str(), "rb" );
            if( pgn_in )
            {
                UnicodeBomSkip( pgn_in );
                it->second.file_read  = pgn_in;
                pgn_out = fopen( new_filename.c_str(), "wb" );
                if( !pgn_out )
                    fclose( pgn_in );
                else
                {
                    ok = true;
                    it->second.filename_temp = new_filename;
                    it->second.file_write = pgn_out;
                    it->second.mode = PgnFile::copying;
                }
            }
        }
    }
    return ok;
}

// Close all files
void PgnFiles::Close( GamesCache *gc_clipboard )
{
    bool still_needed=false;
    int handle_replace=0;
    PgnFile pf;
    std::map<int,PgnFile>::iterator it;
    for( it=files.begin(); it!=files.end(); ++it )
    {
        bool reading   = (it->second.mode == PgnFile::reading);
        bool creating  = (it->second.mode == PgnFile::creating);
        bool modifying = (it->second.mode == PgnFile::modifying);
        bool copying   = (it->second.mode == PgnFile::copying);
        it->second.mode = PgnFile::closed;
        if( reading )
            fclose(it->second.file_read);
        if( creating )
        {
            it->second.filelen = ftell(it->second.file_write);
            fclose(it->second.file_write);
            //wxString wx_filename = it->second.filename.c_str();
            it->second.file_modification_time = 0; //::wxFileModificationTime(wx_filename);
        }
        if( modifying || copying )
        {
            fclose(it->second.file_read);

            // Avoid orphaning clipboard games step 2
            int sz = (gc_clipboard ? gc_clipboard->gds.size() : 0);
            for( int i=0; !still_needed && i<sz; i++ )
            {
                int pgn_handle;
                bool is_pgn = gc_clipboard->gds[i]->GetPgnHandle( pgn_handle );
                if( !gc_clipboard->gds[i]->saved && is_pgn && pgn_handle == it->first )
                {
                    still_needed   = true;
                    handle_replace = it->first;
                    pf             = it->second;    // note this can only happen once per close()
                    if( modifying )
                    {
                        pf.filename = it->second.filename_temp.c_str();
                        pf.delete_on_exit = true;
                    }
                }
            }
            it->second.filelen = ftell(it->second.file_write);
            fclose(it->second.file_write);
            if( copying )
            {
                it->second.filename  = it->second.filename_temp;        // new filename
                //wxString wx_filename = it->second.filename.c_str();
                it->second.file_modification_time = 0; //::wxFileModificationTime(wx_filename);
            }
            else // if( modifying )
            {
            #if 0
                wxString wx_filename_temp  = it->second.filename_temp.c_str();
                wxString wx_filename       = it->second.filename.c_str();
                if( still_needed )
                {

                    // Avoid orphaning clipboard games step 3
                    // We want to swap filenames of the file and temp file, eg
                    //  before
                    //     wx_filename = "file.pgn" (original file)
                    //     wx_filename_temp = "fileDC46.tmp" (modified file)
                    //  after
                    //     "fileDC46.pgn" (original file) was "file.pgn", keep this because it contains clipboard games
                    //     "file.pgn" (modified file) was "fileDC46.tmp"
                    // Use standard swap(a,b) algorithm, i.e. rename a->temp, rename b->a, rename temp->b
                    wxString wx_filename_temp2 = wxFileName::CreateTempFileName(wx_filename);
                    ::wxRenameFile( wx_filename_temp,  wx_filename_temp2, true );
                    ::wxRenameFile( wx_filename,       wx_filename_temp,  true );
                    ::wxRenameFile( wx_filename_temp2, wx_filename,       true );
                }
                else
                    ::wxRenameFile( wx_filename_temp, wx_filename, true );  // old file deleted
                it->second.file_modification_time = ::wxFileModificationTime(wx_filename);
            #endif
            }
        }
    }

    // Avoid orphaning clipboard games step 4
    // If needed, then replace handle for current file whereever it
    //  appears in the clipboard. The new handle in the clipboard points
    //  at a new element in the map; this new element represents the
    //  previous (i.e. unmodified) version of the current file, which
    //  is still present as a temporary file.
    if( gc_clipboard && still_needed )
    {
        int handle = next_handle++;
        int sz=gc_clipboard->gds.size();
        for( int i=0; i<sz; i++ )
        {
            int pgn_handle;
            bool is_pgn = gc_clipboard->gds[i]->GetPgnHandle( pgn_handle );
            if( !gc_clipboard->gds[i]->saved && is_pgn && pgn_handle == handle_replace )
                gc_clipboard->gds[i]->SetPgnHandle( handle );
        }
        std::pair<int,PgnFile> elem(handle,pf);
        files.insert(elem);
    }
}

// Delete temp files on exit
PgnFiles::~PgnFiles()
{
    std::map<int,PgnFile>::iterator it;
    for( it=files.begin(); it!=files.end(); ++it )
    {
        if( it->second.delete_on_exit )
        {
            //wxString wx_filename = it->second.filename.c_str();
            //::wxRemoveFile( wx_filename );
        }
    }
    files.clear();
}

