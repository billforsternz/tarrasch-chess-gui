/****************************************************************************
 * Provide a mechanism for locating games in one or more pgn files
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PGN_FILES_H
#define PGN_FILES_H
#include <stdio.h>
#include <string>
#include <map>
#include "GamesCache.h"

struct PgnFile
{
    PgnFile() { mode=closed; delete_on_exit=false;
                filelen=0; file_read=0; file_write=0; }
    enum {closed,creating,reading,modifying,copying} mode;
    bool delete_on_exit;
    std::string filename;
    std::string filename_temp;
    FILE   *file_read;
    FILE   *file_write;
    time_t  file_modification_time;
    long    filelen;
};

class PgnFiles
{
public:
    PgnFiles()  { files.clear(); next_handle=1; }
    ~PgnFiles();

    // Start reading a file and introduce it into the system
    FILE *OpenRead   ( std::string filename, int &handle );

    // Create a file and introduce it into the system
    FILE *OpenCreate ( std::string filename, int &handle );

    // Is a previously used file known and available (unmodified and ready to go)
    bool IsAvailable ( int handle );

    // Is a file already known and available (unmodified and ready to go)
    bool IsAvailable( std::string filename, int &handle );

    // Reopen a known file for reading
    FILE *ReopenRead ( int handle );

    // Reopen a known file for modification
    bool ReopenModify( int handle, FILE * &pgn_in, FILE * &pgn_out,  GamesCache *gc_clipboard );

    // Reopen a known file for copy
    bool ReopenCopy( int handle, std::string new_filename, FILE * &pgn_in, FILE * &pgn_out,  GamesCache *gc_clipboard );

    // Close all files
    void Close( GamesCache *gc_clipboard=NULL );

    // If a modified file is known, update length and time
    void UpdateKnownFile( std::string &filename, time_t filetime_before, long filelen_before, long delta );

private:
    bool IsAvailable( std::map<int,PgnFile>::iterator it );
    std::map<int,PgnFile> files;
    int next_handle;
};

#endif  // PGN_FILES_H
