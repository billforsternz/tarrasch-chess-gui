/****************************************************************************
 * Like ListableGameDb, but read from the (new, no sql) BinDb database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LISTABLE_GAME_BIN_DB_H
#define LISTABLE_GAME_BIN_DB_H
#include "GameDocument.h"
#include "CompactGame.h"
#include "CompressMoves.h"
#include "PackedGameBinDb.h"

class ListableGameBinDb : public ListableGame
{
private:
    int game_id;
    PackedGameBinDb pack;

public:
    ListableGameBinDb( int game_id, std::string binary_game )
        : pack(binary_game)
    {
        this->game_id = game_id;
    }

    virtual int GetGameId()  { return game_id; }

    virtual void GetCompactGame( CompactGame &pact )
    {
        pack.Unpack(pact);
    }
    
	virtual GameDocument  *GetGameDocumentPtr()
    {
        static GameDocument  the_game;
        CompactGame pact;
        GetCompactGame( pact );
        pact.Upscale(the_game);
        return &the_game;
    }

    virtual bool HaveStartPosition() { return false; }
    
    virtual Roster &RefRoster()
    {
        static Roster r;
        pack.Unpack(r);
        return r;
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

    virtual std::vector<thc::Move> &RefMoves()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.moves;
    }
    virtual std::string &RefCompressedMoves()
    {
        static std::string blob;
        blob.clear();
        pack.Unpack(blob);
        return blob;
    }
    virtual thc::ChessPosition &RefStartPosition()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.start_position;
    }
    
    // For now at least, the following are used for fast sorting on column headings
	virtual int WhiteBin()			{ return pack.WhiteBin(); }
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
    
    virtual bool IsDbGameOnly() { return true; }   // a horrible kludge
    virtual bool IsInMemory()   { return true; }
};


#endif  // LISTABLE_GAME_BIN_DB_H
