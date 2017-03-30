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
    PackedGameBinDb pack;

public:
    ListableGameBinDb() {}
    ListableGameBinDb( int cb_idx, uint32_t game_id, std::string binary_game )
        : pack( cb_idx, binary_game )
    {
        this->game_id = game_id;
        SetAttributes();
    }

    ListableGameBinDb( 
        uint8_t cb_idx,
        uint32_t game_id,
        std::string event,
        std::string site,
        std::string white,
        std::string black,
        uint32_t    date,
        uint16_t    round,
        uint8_t     result,
        uint16_t    eco,
        uint16_t    white_elo,
        uint16_t    black_elo,
        std::string compressed_moves
    )  : pack (
        cb_idx,
        event,
        site,
        white,
        black,
        date,
        round,
        result,
        eco,
        white_elo,
        black_elo,
        compressed_moves
    )
    {
        this->game_id = game_id;
        SetAttributes( compressed_moves.c_str(), compressed_moves.length() );
    }

    virtual void GetCompactGame( CompactGame &pact )
    {
        pack.Unpack(pact);
        pact.game_id = game_id;
    }
    
	virtual void ConvertToGameDocument(GameDocument &gd)
    {
        CompactGame pact;
        GetCompactGame( pact );
        pact.Upscale(gd);
        gd.game_id = game_id;
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
	virtual int WhiteBin()			{ return pack.WhiteBin(); }
    virtual int BlackBin()          { return pack.BlackBin(); }
    virtual int EventBin()          { return pack.EventBin(); }
    virtual int SiteBin()           { return pack.SiteBin(); }
    virtual int ResultBin()         { return pack.ResultBin(); }
    virtual int RoundBin()          { return pack.RoundBin(); }
    virtual int DateBin()           { return pack.DateBin(); }
    virtual int EcoBin()            { return pack.EcoBin(); }
    virtual int WhiteEloBin()       { return pack.WhiteEloBin(); }
    virtual int BlackEloBin()       { return pack.BlackEloBin(); }
    virtual bool UsesControlBlock( uint8_t &control_block_idx ) { control_block_idx=pack.GetControlBlockIdx(); return true; }
};


#endif  // LISTABLE_GAME_BIN_DB_H
