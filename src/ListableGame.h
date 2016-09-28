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
#include "BinaryConversions.h"

#define smart_ptr std::shared_ptr //std::unique_ptr
#define make_smart_ptr(T,to,from) smart_ptr<T> to; to.reset(new T(from))

#define GAME_ID_SENTINEL 0xffffffff // otherwise unused
uint32_t GameIdAllocateTop( uint32_t count );
uint32_t GameIdAllocateBottom( uint32_t count );
#define UNUSED(x)

class GameDocument;
class ListableGame
{
public:
    ListableGame() { /*transpo_nbr=0;*/ game_attributes=0; game_id=0; }
    virtual ~ListableGame() {}
	virtual GameDocument *IsGameDocument()  { return NULL; }		// return ptr to this if and only if this is type GameDocument
	virtual void ConvertToGameDocument(GameDocument &UNUSED(gd)) {}
	virtual long GetFposn() { return 0; }
    virtual void SetFposn( long UNUSED(posn) ) {}
    virtual bool GetPgnHandle( int &UNUSED(pgn_handle) ) { return false; }
    virtual void SetPgnHandle( int UNUSED(pgn_handle) )  {}
    virtual bool IsModified()        { return false; }
    virtual void SetGameBeingEdited( uint32_t UNUSED(game_being_edited) ) {}
    virtual uint32_t GetGameBeingEdited() { return 0; }
    virtual void *LoadIntoMemory( void *UNUSED(context), bool UNUSED(end) )  {return 0;}
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
	virtual int         WhiteBin() {return 0;}
    virtual int         BlackBin() {return 0;}
    virtual int         EventBin() {return 0;}
    virtual int         SiteBin() {return 0;}
    virtual int         ResultBin() {return Result2Bin(Result());}
    virtual int         RoundBin() {return Round2Bin(Round());}
    virtual int         DateBin() {return Date2Bin(Date());}
    virtual int         EcoBin() {return Eco2Bin(Eco());}
    virtual int         WhiteEloBin() {return Elo2Bin(WhiteElo());}
    virtual int         BlackEloBin() {return Elo2Bin(BlackElo());}
    
    // High performance
    virtual Roster                  &RefRoster()          { static Roster r; return r; }
    virtual std::vector<thc::Move>  &RefMoves()           { static std::vector<thc::Move> moves; return moves; }
    virtual thc::ChessPosition      &RefStartPosition()   { static thc::ChessPosition cp; return cp; }

    // For editing the roster
    virtual void SetRoster( Roster &UNUSED(r) ) {}
    
    // Easy to use
    virtual void GetCompactGame( CompactGame &pact )
    {
        pact.r     = RefRoster();
        pact.moves = RefMoves();
        pact.start_position = RefStartPosition();
        pact.transpo_nbr = 0;
        pact.game_id = game_id;
    }
    
public:
    uint32_t game_id;
    uint8_t  game_attributes;            // at the moment this is effectively bool game_has_promotion;
    virtual void SetAttributes( const char *blob, int len )
    {
        game_attributes = static_cast<uint8_t>(false);
        while( len-- )
        {
            if( (*blob++&0x8c) > 0x80 )     // binary 1pppttxx is a pawn move, ppp selects the pawn,
            {                               //  tt selects the major move type, xx selects the minor
                                            //  move type. Only tt=00 is a non-promoting pawn move
                game_attributes = static_cast<uint8_t>(true);
                break;
            }
        }
    }
    virtual void SetAttributes()
    {
        const char *blob = CompressedMoves();
        game_attributes = static_cast<uint8_t>(false);
        while( *blob )
        {
            if( (*blob++&0x8c) > 0x80 )
            {
                game_attributes = static_cast<uint8_t>(true);
                break;
            }
        }
    }
 
};

        
#endif //LISTABLE_GAME_H
