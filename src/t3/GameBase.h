/****************************************************************************
 * Game representation base class
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef GAME_BASE_H
#define GAME_BASE_H
#include <string>
#include <vector>
#include <memory>
#include "thc.h"
#include "CompressMoves.h"
#include "CompactGame.h"

#define smart_ptr std::shared_ptr //std::unique_ptr
#define make_smart_ptr(T,to,from) smart_ptr<T> to; to.reset(new T(from))

class MagicBase
{
public:
    MagicBase() { transpo_nbr=0; }
    virtual ~MagicBase() {}
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
    
    // High performance
    virtual Roster                  &RefRoster()          { static Roster r; return r; }
    virtual std::vector<thc::Move>  &RefMoves()           { static std::vector<thc::Move> moves; return moves; }
    virtual std::string             &RefCompressedMoves() { static std::string moves; return moves; }
    virtual thc::ChessPosition      &RefStartPosition()   { static thc::ChessPosition cp; return cp; }
    
    // Easy to use
    virtual void GetCompactGame( CompactGame &pact )
    {
        pact.r     = RefRoster();
        pact.moves = RefMoves();
        pact.start_position = RefStartPosition();
        pact.transpo_nbr = transpo_nbr;
    }
    
    int transpo_nbr;
};

        
#endif //GAME_BASE_H
