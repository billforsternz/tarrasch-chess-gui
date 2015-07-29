/****************************************************************************
 * Games Cache - Abstracted interface to  a list of games
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAMES_CACHE_H
#define GAMES_CACHE_H
#include "GameDocument.h"
#include "CompressMoves.h"
#include <time.h> // time_t

class DB_GAME_INFO;

class CompactGame
{
public:
    Roster r;
    thc::ChessPosition start_position;
    std::vector< thc::Move > moves;
    
    // temp stuff hopefully
    int game_id;
    int transpo_nbr;
    
    std::string Description();
    void Upscale( GameDocument &gd );       // to GameDocument
    void Downscale( GameDocument &gd );     // from GameDocument
    bool HaveStartPosition() {
        bool is_initial_position = ( 0 == strcmp(start_position.squares,"rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR")
                                    ) && start_position.white;
        return !is_initial_position; }
    thc::ChessPosition &GetStartPosition() { return start_position; }
    
    // Return index into vector where start position found
    bool FindPositionInGame( uint64_t hash_to_match, int &idx )
    {
        thc::ChessRules cr = start_position;
        size_t len = moves.size();
        uint64_t hash = cr.Hash64Calculate();
        bool found = (hash==hash_to_match);
        idx = 0;
        for( size_t i=0; !found && i<len; i++  )
        {
            thc::Move mv = moves[i];
            hash = cr.Hash64Update( hash, mv );
            if( hash == hash_to_match )
            {
                found = true;
                idx = static_cast<int>(i+1);
                break;
            }
            cr.PlayMove(mv);
        }
        return found;
    }
    
};

class MagicBase
{
public:
    virtual ~MagicBase() {}
    virtual void GetGameDocument( GameDocument &gd )    {
        cprintf("FIXME DANGER WILL ROBINSON 1\n"); }
    virtual GameDocument GetGameDocument()              { GameDocument doc;
        cprintf("FIXME DANGER WILL ROBINSON 2\n"); return doc; }
    virtual GameDocument *GetGameDocumentPtr()  {
        cprintf("FIXME DANGER WILL ROBINSON 3\n");  return NULL; }
    virtual DB_GAME_INFO *GetCompactGamePtr() {
        cprintf("FIXME DANGER WILL ROBINSON 4\n");  return NULL; }      // FIXME USE THIS AS MUCH AS POSSIBLE FOR GOOD PERFORMANCE
    virtual bool IsInMemory()        { return false; }
    virtual bool IsModified()        { return false; }
    virtual uint32_t GetGameBeingEdited() { return 0; }
    virtual long GetFposn() { return 0; }

    // High performance
    virtual Roster &RefRoster() { static Roster r; return r; }
    virtual std::vector<thc::Move> &RefMoves() { static std::vector<thc::Move> moves; return moves; }
    virtual thc::ChessPosition &RefStartPosition() { static thc::ChessPosition cp; return cp; }
    
    // Easy to use
    virtual void GetCompactGame( CompactGame &pact )
    {
        pact.r     = RefRoster();
        pact.moves = RefMoves();
        pact.start_position = RefStartPosition();
        pact.transpo_nbr = 0;
    }
};

class HoldDocument : public MagicBase
{
public:
    HoldDocument( GameDocument &doc ) { the_game = doc; }
    GameDocument        the_game;
	virtual void GetGameDocument(GameDocument &gd)				{ the_game.GetGameDocument(gd); }
	virtual GameDocument        GetGameDocument()               { return the_game; }
	virtual GameDocument        *GetGameDocumentPtr()           { return &the_game; }
    virtual bool IsInMemory()        { return true; }
    virtual bool IsModified()        { return the_game.IsModified(); }
    virtual uint32_t GetGameBeingEdited() { return the_game.game_being_edited; }
    virtual long GetFposn() { return the_game.fposn0; }
    virtual Roster &RefRoster() { return the_game.r; }
    virtual std::vector<thc::Move> &RefMoves()
    {
        static std::vector<thc::Move> moves;
        moves.clear();
        std::vector<MoveTree> &variation = the_game.tree.variations[0];
        for( int i=0; i<variation.size(); i++ )
        {
            thc::Move mv = variation[i].game_move.move;
            moves.push_back(mv);
        }
        return moves;
    }
    virtual thc::ChessPosition &RefStartPosition() { return the_game.start_position; }
};

class DB_GAME_INFO : public MagicBase
{
public:
    virtual void GetGameDocument( GameDocument &gd )    { Upscale(gd); }
    virtual GameDocument GetGameDocument()              { GameDocument doc;  Upscale(doc);
        cprintf("FIXME DANGER WILL ROBINSON 5\n"); return doc; }
    virtual DB_GAME_INFO *GetCompactGamePtr() { return this; }
    int game_id;
    Roster r;
    std::string str_blob;
    int transpo_nbr;
    
    std::string Description();
    void Upscale( GameDocument &gd );       // to GameDocument
    virtual void Downscale( GameDocument &gd );     // from GameDocument
    virtual bool HaveStartPosition() { return false; }
    virtual thc::ChessPosition &GetStartPosition() { cprintf("FIXME (MAYBE) DANGER WILL ROBINSON 6\n"); static thc::ChessPosition start; return start; }
    virtual Roster &RefRoster() { return r; }
    virtual std::vector<thc::Move> &RefMoves()
    {
        static std::vector<thc::Move> moves;
        moves.clear();
        CompressMoves press;
        int len = str_blob.size();
        const char *blob = str_blob.c_str();
        for( int i=0; i<len; i++ )
        {
            thc::Move mv;
            int nbr = press.decompress_move( blob, mv );
            blob++;
            if( nbr == 0 )
                break;
            moves.push_back(mv);
        }
        return moves;
    }
    virtual thc::ChessPosition &RefStartPosition() { static thc::ChessPosition start; return start;  }
};

class DB_GAME_INFO_FEN : public DB_GAME_INFO
{
public:
	thc::ChessPosition start_position;
    virtual bool HaveStartPosition() {
        bool is_initial_position = ( 0 == strcmp(start_position.squares,"rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR")
                                    ) && start_position.white;
        return !is_initial_position; }
    virtual thc::ChessPosition &GetStartPosition() { return start_position; }
    virtual void Downscale( GameDocument &gd );     // from GameDocument
    virtual std::vector<thc::Move> &RefMoves()
    {
        static std::vector<thc::Move> moves;
        moves.clear();
        CompressMoves press;
        press.Init( start_position );
        int len = str_blob.size();
        const char *blob = str_blob.c_str();
        for( int i=0; i<len; i++ )
        {
            thc::Move mv;
            int nbr = press.decompress_move( blob, mv );
            blob++;
            if( nbr == 0 )
                break;
            moves.push_back(mv);
        }
        return moves;
    }
    virtual thc::ChessPosition &RefStartPosition() { return start_position;  }
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
    virtual long GetFposn() { return fposn; }
    virtual Roster &RefRoster()
    {
        static GameDocument  the_game;
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        return the_game.r;
    }
    virtual std::vector<thc::Move> &RefMoves()
    {
        static GameDocument  the_game;
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        static std::vector<thc::Move> moves;
        moves.clear();
        std::vector<MoveTree> &variation = the_game.tree.variations[0];
        for( int i=0; i<variation.size(); i++ )
        {
            thc::Move mv = variation[i].game_move.move;
            moves.push_back(mv);
        }
        return moves;
    }
    virtual thc::ChessPosition &RefStartPosition()
    {
        static GameDocument  the_game;
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        return the_game.start_position;
    }
    
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
    //  add info to a GameDocument, optionally clearing it first
public:
    bool Tagline( GameDocument &gd,  const char *s );
};

#endif    // GAMES_CACHE_H
