/****************************************************************************
 * A ListableGame that was originally created for the list in the PgnDialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LISTABLE_GAME_PGN_H
#define LISTABLE_GAME_PGN_H
#include "GameDocument.h"
#include "CompactGame.h"
#include "PackedGame.h"
#include "CompressMoves.h"


void ReadGameFromPgn( int pgn_handle, long fposn, GameDocument &gd );
void *ReadGameFromPgnInLoop( int pgn_handle, long fposn, CompactGame &pact, void *context, bool end=true );

class ListableGamePgn : public ListableGame
{
private:
    int  pgn_handle;
    long fposn;
    PackedGame pack;
public:
    ListableGamePgn( int pgn_handle, long fposn ) { this->pgn_handle=pgn_handle, this->fposn = fposn; game_being_edited=0;  }
    virtual long GetFposn() { return fposn; }
    virtual void SetFposn( long posn ) { fposn=posn; }
    virtual bool GetPgnHandle( int &pgn_handle_ ) { pgn_handle_=this->pgn_handle; return true; }
    virtual void *LoadIntoMemory( void *context, bool end )
    {
        if( pack.Empty() )
        {
            CompactGame pact;
            context = ReadGameFromPgnInLoop( pgn_handle, fposn, pact, context, end );
            pack.Pack(pact);
        }
        return context;
    }

    virtual void GetCompactGame( CompactGame &pact )
    {
        if( pack.Empty() )
            LoadIntoMemory( NULL, true );
        pack.Unpack(pact);
        pact.game_id = game_id;
    }

    // For editing the roster
    virtual void SetRoster( Roster &r )
    {
        CompactGame pact;
        if( pack.Empty() )
            LoadIntoMemory( NULL, true );
        pack.Unpack(pact);
        pact.r = r;
        pack.Pack(pact);
    }
    
	virtual void ConvertToGameDocument(GameDocument &gd)
	{
		ReadGameFromPgn(pgn_handle, fposn, gd);
	}


    virtual Roster &RefRoster()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.r;
    }
    virtual std::vector<thc::Move> &RefMoves()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.moves;
    }
    virtual thc::ChessPosition &RefStartPosition()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.start_position;
    }

    // For now at least, the following are used for fast sorting on column headings
    //  (only available after LoadInMemory() called - games are loaded from file
    //   when user clicks on a column heading
    virtual const char *White()     { return pack.White();    }
    virtual const char *Black()     { return pack.Black();    }
    virtual const char *Event()     { return pack.Event();    }
    virtual const char *Site()      { return pack.Site();     }
    virtual const char *Result()    { return pack.Result();   }
    virtual const char *Round()     { return pack.Round() ;   }
    virtual const char *Date()      { return pack.Date();     }
    virtual const char *Eco()       { return pack.Eco();      }
    virtual const char *WhiteElo()  { return pack.WhiteElo(); }
    virtual const char *BlackElo()  { return pack.BlackElo(); }
    virtual const char *Fen()       { return pack.Fen();      }
    virtual const char *CompressedMoves() {return pack.Blob();  }

    virtual void SetGameBeingEdited( uint32_t game_being_edited_ ) { this->game_being_edited = game_being_edited_; }
    virtual uint32_t GetGameBeingEdited() { return game_being_edited; }
private:
    uint32_t game_being_edited;
};


#endif    // LISTABLE_GAME_PGN_H
