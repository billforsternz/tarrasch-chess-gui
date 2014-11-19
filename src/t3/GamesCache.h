/****************************************************************************
 * Games Cache - Abstracted interface to  a list of games
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMES_CACHE_H
#define GAMES_CACHE_H
#include "GameDocument.h"
#include <time.h> // time_t


class MagicBase
{
public:
    ~MagicBase() {}
    virtual void GetGameDocument( GameDocument &gd )    { cprintf("FIXME DANGER WILL ROBINSON 1\n"); }
    virtual GameDocument GetGameDocument()              { GameDocument doc;     cprintf("FIXME DANGER WILL ROBINSON 2\n"); return doc; }
    virtual GameSummary  GetGameSummary()               { GameSummary  summary; cprintf("FIXME DANGER WILL ROBINSON 3\n"); return summary; }
    virtual GameDocumentBase *GetGameDocumentBasePtr()  { return NULL; }
    virtual GameDocument     *GetGameDocumentPtr()      { return NULL; }
    virtual std::string white()     { return ""; }
    virtual std::string white_elo() { return ""; }
    virtual std::string black()     { return ""; }
    virtual std::string black_elo() { return ""; }
    virtual std::string date()      { return ""; }
    virtual std::string site()      { return ""; }
    virtual std::string round()     { return ""; }
    virtual std::string result()    { return ""; }
    virtual std::string eco()       { return ""; }
    virtual bool IsInMemory()        { return false; }
    virtual bool IsModified()        { return false; }
    virtual uint32_t GetGameBeingEdited() { return 0; }
    virtual long GetFposn() { return 0; }
};

class HoldGameSummary : public MagicBase
{
public:
    GameSummary         the_game;
	virtual GameSummary         GetGameSummary()                { return the_game; }
	virtual std::string white()     { return the_game.white; }
	virtual std::string white_elo() { return the_game.white_elo; }
	virtual std::string black()     { return the_game.black; }
	virtual std::string black_elo() { return the_game.black_elo; }
	virtual std::string date()      { return the_game.date; }
	virtual std::string site()      { return the_game.site; }
	//virtual std::string round()     { return the_game.round; }
    virtual std::string result()    { return the_game.result; }
    //virtual std::string eco()       { return the_game.eco; }
};

class HoldDocumentBase : public MagicBase
{
public:
    HoldDocumentBase( GameDocumentBase &doc ) { the_game = doc; }
    GameDocumentBase    the_game;
	virtual void GetGameDocument(GameDocument &gd)				{ the_game.GetGameDocument(gd); }
	virtual GameDocument        GetGameDocument()               { GameDocument gd; the_game.GetGameDocument(gd); return gd; }
	virtual GameDocumentBase    *GetGameDocumentBasePtr()       { return &the_game; }
	virtual std::string white()     { return the_game.white; }
	virtual std::string white_elo() { return the_game.white_elo; }
	virtual std::string black()     { return the_game.black; }
	virtual std::string black_elo() { return the_game.black_elo; }
	virtual std::string date()      { return the_game.date; }
	virtual std::string site()      { return the_game.site; }
	virtual std::string round()     { return the_game.round; }
	virtual std::string result()    { return the_game.result; }
    virtual std::string eco()       { return the_game.eco; }
    virtual bool IsInMemory()        { return true; }
    virtual bool IsModified()        { return the_game.IsModified(); }
    virtual uint32_t GetGameBeingEdited() { return the_game.game_being_edited; }
    virtual long GetFposn() { return the_game.fposn0; }
};

class HoldDocument : public MagicBase
{
public:
    HoldDocument( GameDocument &doc ) { the_game = doc; }
    GameDocument        the_game;
	virtual void GetGameDocument(GameDocument &gd)				{ the_game.GetGameDocument(gd); }
	virtual GameDocument        GetGameDocument()               { return the_game; }
	virtual GameDocument        *GetGameDocumentPtr()           { return &the_game; }
	virtual GameDocumentBase    *GetGameDocumentBasePtr()       { return static_cast<GameDocumentBase *>(&the_game); }
	virtual std::string white()     { return the_game.white; }
	virtual std::string white_elo() { return the_game.white_elo; }
	virtual std::string black()     { return the_game.black; }
	virtual std::string black_elo() { return the_game.black_elo; }
	virtual std::string date()      { return the_game.date; }
	virtual std::string site()      { return the_game.site; }
	virtual std::string round()     { return the_game.round; }
	virtual std::string result()    { return the_game.result; }
	virtual std::string eco()       { return the_game.eco; }
    virtual bool IsInMemory()        { return true; }
    virtual bool IsModified()        { return the_game.IsModified(); }
    virtual uint32_t GetGameBeingEdited() { return the_game.game_being_edited; }
    virtual long GetFposn() { return the_game.fposn0; }
};

void ReadGameFromPgn( int pgn_handle, long fposn, GameDocument &gd );

class PgnDocument : public MagicBase
{
private:
    int  pgn_handle;
    long fposn;
public:
    PgnDocument( int pgn_handle, long fposn ) { this->pgn_handle=pgn_handle, this->fposn = fposn; }
	virtual void GetGameDocument(GameDocument &gd)
    {
        ReadGameFromPgn( pgn_handle, fposn, gd );
    }
	virtual GameDocument        GetGameDocument()
    {
        GameDocument  the_game;
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        return the_game;
    }
	virtual GameDocument        *GetGameDocumentPtr()
    {
        static GameDocument  the_game;
        cprintf("FIXME DANGER WILL ROBINSON (ptr to static 1)\n");
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        return &the_game;
    }
	virtual GameDocumentBase    *GetGameDocumentBasePtr()
    {
        static GameDocument  the_game;
        cprintf("FIXME DANGER WILL ROBINSON (ptr to static 2)\n");
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        return static_cast<GameDocumentBase *>(&the_game);
    }
    virtual long GetFposn() { return fposn; }
};


class GamesCache
{    
public:
    std::vector< smart_ptr<MagicBase> >  gds;
    std::vector<int>           col_flags;
    std::string                pgn_filename;
    int game_nbr;
    bool renumber;
    bool file_irrevocably_modified;

    GamesCache() { state=PREFIX; renumber=false; resume_previous_window=false; loaded=false; top_item=0;
                    file_irrevocably_modified=false; }
    void Debug( const char *intro_message );
    bool Load( std::string &filename );
    bool Reload() { return Load(pgn_filename); }
    bool Load( FILE *pgn_file );
    bool FileCreate( std::string &filename, GameDocument &gd );
    void FileSave( GamesCache *gc_clipboard );
    void FileSaveAs( std::string &filename, GamesCache *gc_clipboard );
    void FileSaveGameAs( std::string &filename, GamesCache *gc_clipboard );
    void FileSaveAllAsAFile( std::string &filename );
    void FileSaveInner( GamesCache *gc_clipboard, FILE *pgn_in, FILE *pgn_out );
    void Publish( GamesCache *gc_clipboard );
    bool IsLoaded();
    bool IsSynced();
    void KillResumePreviousWindow()
    {
        resume_previous_window=false;
    }
    void PrepareForResumePreviousWindow( int top_item )
    {
        resume_previous_window=true; this->top_item = top_item;
    }
    bool IsResumingPreviousWindow( int &top_item )
    {
        top_item=this->top_item; return resume_previous_window; 
    }

    // Helpers
    int  pgn_handle;
private:
    enum {PREFIX,HEADER,INGAME} state;
    bool resume_previous_window;
    int  top_item;
    bool loaded;

    // Check whether text s is a valid header, return true if it is,
    //  add info to a GameDocumentBase, optionally clearing it first
public:
    bool Tagline( GameDocumentBase &gd,  const char *s );
};

#endif    // GAMES_CACHE_H
