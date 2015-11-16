/****************************************************************************
 * A Game in a GameCache - From a .pgn file
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PGN_DOCUMENT_H
#define PGN_DOCUMENT_H
#include "GameDocument.h"
#include "CompactGame.h"
#include "PackedGame.h"
#include "CompressMoves.h"


void ReadGameFromPgn( int pgn_handle, long fposn, GameDocument &gd );
void *ReadGameFromPgnInLoop( int pgn_handle, long fposn, CompactGame &pact, void *context, bool end=true );

class PgnDocument : public MagicBase
{
private:
    int  pgn_handle;
    long fposn;
    PackedGame pack;
public:
    PgnDocument( int pgn_handle, long fposn ) { this->pgn_handle=pgn_handle, this->fposn = fposn;  }

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
        if( !pack.Empty() )
            pack.Unpack(pact);
        else
        {
            ReadGameFromPgnInLoop( pgn_handle, fposn, pact, NULL, true );
            pack.Pack(pact);
        }
    }
    
	virtual GameDocument  *GetGameDocumentPtr()
    {
        static GameDocument  the_game;
        cprintf("FIXME DANGER WILL ROBINSON (ptr to static 1)\n");
        ReadGameFromPgn( pgn_handle, fposn, the_game );
        return &the_game;
    }
    virtual long GetFposn() { return fposn; }
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
    virtual std::string &RefCompressedMoves()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        CompressMoves press( pact.start_position );
        static std::string blob;
        blob.clear();
        blob = press.Compress( pact.moves );
        return blob;
    }
    virtual thc::ChessPosition &RefStartPosition()
    {
        static CompactGame pact;
        GetCompactGame( pact );
        return pact.start_position;
    }
    virtual const char *White() { return pack.White(); }
    virtual const char *Black() { return pack.Black(); }
    
};


#endif    // PGN_DOCUMENT_H
