/****************************************************************************
 * Base class defines interface to a game that can be listed in a GamesDialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LISTABLE_GAME_H
#define LISTABLE_GAME_H
#include <string>
#include <vector>
#include <memory>
#include "thc.h"
#include "CompressMoves.h"
#include "CompactGame.h"

#define smart_ptr std::shared_ptr //std::unique_ptr
#define make_smart_ptr(T,to,from) smart_ptr<T> to; to.reset(new T(from))

class ListableGame
{
public:
    ListableGame() { transpo_nbr=0; }
    virtual ~ListableGame() {}
    virtual int GetGameId() { return 0; }
    virtual GameDocument *GetGameDocumentPtr()  {
        cprintf("FIXME DANGER WILL ROBINSON 3\n");  return NULL; }
    virtual bool GetPgnHandle( int &pgn_handle ) { return false; }
    virtual void SetPgnHandle( int pgn_handle )  {}
    virtual bool IsInMemory()        { return false; }
    virtual bool IsModified()        { return false; }
    virtual void SetSelected( bool selected ) {}
    virtual void SetFocused( bool focused ) {}
    virtual bool GetSelected() { return false; }
    virtual bool GetFocused() { return false; }
    virtual void SetGameBeingEdited( uint32_t game_being_edited ) {}
    virtual uint32_t GetGameBeingEdited() { return 0; }
    virtual long GetFposn() { return 0; }
    virtual void *LoadIntoMemory( void *context, bool end )  {return 0;}
    virtual const char *White() {return "";}
    virtual const char *Black() {return "";}
    virtual const char *Event() {return "";}
    virtual const char *Site() {return "";}
    virtual const char *Result() {return "";}
    virtual const char *Round() {return "";}
    virtual const char *Date() {return "";}
    virtual const char *Eco() {return "";}
    virtual const char *WhiteElo() {return "";}
    virtual const char *BlackElo() {return "";}
    virtual const char *Fen() {return "";}
    virtual const char *CompressedMoves() {return "";}
    
    // High performance
    virtual Roster                  &RefRoster()          { static Roster r; return r; }
    virtual std::vector<thc::Move>  &RefMoves()           { static std::vector<thc::Move> moves; return moves; }
    virtual std::string             &RefCompressedMoves() { static std::string moves; return moves; }
    virtual thc::ChessPosition      &RefStartPosition()   { static thc::ChessPosition cp; return cp; }

    // For editing the roster
    //virtual void SetRoster( Roster &r ) {}
    
    // Easy to use
    virtual void GetCompactGame( CompactGame &pact )
    {
        pact.r     = RefRoster();
        pact.moves = RefMoves();
        pact.start_position = RefStartPosition();
        pact.transpo_nbr = transpo_nbr;
    }
    
private:
    int transpo_nbr;    // it would be nice to move this into ListableGameDb.h
};

        
#endif //LISTABLE_GAME_H
