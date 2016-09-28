/****************************************************************************
 * A ListableGame that was originally created for the list in the DbDialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef LISTABLE_GAME_DB_H
#define LISTABLE_GAME_DB_H
#include "GameDocument.h"
#include "CompactGame.h"
#include "PackedGame.h"
#include "CompressMoves.h"

class ListableGameDb : public ListableGame
{
private:
    uint32_t game_id;
    PackedGame pack;

public:
    ListableGameDb( uint32_t game_id, Roster &r, std::string str_blob )
    {
        this->game_id = game_id;
        pack.Pack( r, str_blob );
    }

    ListableGameDb( uint32_t game_id, CompactGame &pact )
    {
        this->game_id = game_id;
        pack.Pack( pact );
    }

    ListableGameDb
    (
        int id,
        const char *white, int len1,
        const char *black, int len2,
        const char *event, int len3,
        const char *site, int len4,
        const char *result, int len5,
        const char *round, int len6,
        const char *date, int len7,
        const char *eco, int len8,
        const char *white_elo, int len9,
        const char *black_elo, int len10,
        const char *fen, int len11,
        const char *moves_blob, int len12
    ) : game_id(id),
        pack
        (
            white, len1,
            black, len2,
            event, len3,
            site, len4,
            result, len5,
            round, len6,
            date, len7,
            eco, len8,
            white_elo, len9,
            black_elo, len10,
            fen, len11,
            moves_blob, len12
        )
    {
        SetAttributes(moves_blob,len12);
    }

    virtual void GetCompactGame( CompactGame &pact )
    {
        pack.Unpack(pact);
        pact.game_id = game_id;
    }
    
	virtual void ConvertToGameDocument(GameDocument &gd)
	{
		CompactGame pact;
		GetCompactGame(pact);
		pact.Upscale(gd);
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
    virtual thc::ChessPosition &RefStartPosition()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.start_position;
    }
    
    // For now at least, the following are used for fast sorting on column headings
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
};


#endif  // LISTABLE_GAME_DB_H
