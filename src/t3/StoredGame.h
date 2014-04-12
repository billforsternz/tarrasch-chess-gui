//
//  StoredGame.h
//  t3
//
//  Created by Bill Forster on 11/04/14.
//  Copyright (c) 2014 Bill Forster. All rights reserved.
//

#ifndef __t3__StoredGame__
#define __t3__StoredGame__

#include <memory>
#include <vector>
#include <stdio.h>
#include "Repository.h"
#include "GameLogic.h"
#include "GameDocument.h"

// For the moment this is purely experimental 


class StoredGame
{
public:
    virtual void FromStorage( GameDocument &gd ) = 0;
    virtual std::unique_ptr<StoredGame> ToStorage() = 0;
    virtual ~StoredGame() {};
};

class StoredGameFile : public StoredGame
{
private:
    GameDocumentBase base;

public:
    StoredGameFile( GameDocumentBase &base ) { this->base = base; }
    
    void FromStorage( GameDocument &read_from_file )
    {
        GameDocument temp = base;
        if( !temp.in_memory )
        {
            temp.in_memory = true;
            FILE *pgn_in = objs.gl->pf.ReopenRead( temp.pgn_handle );
            if( pgn_in )
            {
                fseek(pgn_in,temp.fposn2,SEEK_SET);
                long len = temp.fposn3-temp.fposn2;
                char *buf = new char [len];
                if( len == (long)fread(buf,1,len,pgn_in) )
                {
                    std::string s(buf,len);
                    thc::ChessRules cr;
                    int nbr_converted;
                    temp.PgnParse(true,nbr_converted,s,cr,NULL);
                }
                objs.gl->pf.Close( &objs.gl->gc_clipboard );
                delete[] buf;
            }
        }
        read_from_file = temp;
    }

    std::unique_ptr<StoredGame> ToStorage()
    {
        return std::unique_ptr<StoredGame>( new StoredGameFile(base) );
    }
};

class StoredGameDocument : public StoredGame
{
private:
    GameDocument gd;
public:
    StoredGameDocument( GameDocument &gd ) { this->gd = gd; }
    void FromStorage( GameDocument &already_in_memory )
    {
        already_in_memory = gd;
    }
    std::unique_ptr<StoredGame> ToStorage()
    {
        return std::unique_ptr<StoredGame>( new StoredGameDocument(gd) );
    }
};


#endif /* defined(__t3__StoredGame__) */
