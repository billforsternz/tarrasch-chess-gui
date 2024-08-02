/****************************************************************************
 * Games Cache - Abstracted interface to  a list of games
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMES_CACHE_H
#define GAMES_CACHE_H

#include <string>
#include <vector>
#include <map>
#include "GameDocument.h"

class GamesCache
{
public:
    std::vector< smart_ptr<ListableGame> >  gds;
    std::vector<int>           col_flags;
    std::string                pgn_filename;
    bool file_irrevocably_modified;

    GamesCache() { state=PREFIX; loaded=false;
                    file_irrevocably_modified=false; pgn_handle=0; }
    void Debug( const char *intro_message );
    bool Load( const std::string &filename,  const std::string &asset_filename );
    //bool Reload() { return Load(pgn_filename); }
    bool Load( FILE *pgn_file, FILE *asset_file );
    void FileCreate( std::string &filename );
    void FileSave( GamesCache *gc_clipboard );
    void FileSaveAs( std::string &filename, GamesCache *gc_clipboard );
    //void FileSaveGameAs( std::string &filename, GamesCache *gc_clipboard );
    void FileSaveAllAsAFile( std::string &filename );
    void FileSaveInner( FILE *pgn_out );
    void Publish( const std::string &html_out_file,
                  const std::string &header,
                  const std::string &footer,
                  std::map<char,std::string> &macros,
                  const std::vector<std::pair<std::string,std::string>> &menu, int menu_idx );
    void Eco( GamesCache *gc_clipboard );
    bool IsLoaded();
    bool TestGameInCache( const GameDocument &gd );


    // Helpers
    int  pgn_handle;
private:
    enum {PREFIX,HEADER,INGAME} state;
    bool loaded;

    // Check whether text s is a valid header, return true if it is,
    //  add info to a GameDocument, optionally clearing it first
public:
    bool Tagline( GameDocument &gd,  const char *s );
};

#endif    // GAMES_CACHE_H
