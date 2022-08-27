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
    bool in_memory;
public:
    ListableGamePgn( int pgn_handle, long fposn ) { this->pgn_handle=pgn_handle, this->fposn = fposn; in_memory=false;  }
    virtual long GetFposn() { return fposn; }
    virtual void SetFposn( long posn ) { fposn=posn; }
    virtual bool GetPgnHandle( int &pgn_handle_ ) { pgn_handle_=this->pgn_handle; return true; }
    virtual void SetPgnHandle( int pgn_handle_ )  { this->pgn_handle = pgn_handle_; }
    virtual void *LoadIntoMemory( void *context, bool end )
    {
        if( pack.Empty() )
        {
            CompactGame pact;
            context = ReadGameFromPgnInLoop( pgn_handle, fposn, pact, context, end );
            pack.Pack(pact);
        }
        in_memory = true;
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
        gd.game_id = game_id;
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
    virtual const char *White()     { if(!in_memory) LoadIntoMemory(NULL,true); return pack.White();    }
    virtual const char *Black()     { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Black();    }
    virtual const char *Event()     { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Event();    }
    virtual const char *Site()      { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Site();     }
    virtual const char *Result()    { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Result();   }
    virtual const char *Round()     { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Round() ;   }
    virtual const char *Date()      { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Date();     }
    virtual const char *Eco()       { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Eco();      }
    virtual const char *WhiteElo()  { if(!in_memory) LoadIntoMemory(NULL,true); return pack.WhiteElo(); }
    virtual const char *BlackElo()  { if(!in_memory) LoadIntoMemory(NULL,true); return pack.BlackElo(); }
    virtual const char *Fen()       { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Fen();      }
    virtual const char *CompressedMoves() { if(!in_memory) LoadIntoMemory(NULL,true); return pack.Blob();  }
};


#endif    // LISTABLE_GAME_PGN_H
