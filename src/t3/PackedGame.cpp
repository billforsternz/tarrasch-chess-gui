/****************************************************************************
 * Pack a complete game into a single string
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "CompressMoves.h"
#include "PackedGame.h"

void PackedGame::Pack( Roster &r, std::string &blob )
{
    fields.clear();
    for( int i=0; i<12; i++ )
    {
        std::string s;
        bool bad=false;
        switch(i)
        {
            case 0: s = r.white;        break;
            case 1: s = r.black;        break;
            case 2: s = r.event;        break;
            case 3: s = r.site;         break;
            case 4: s = r.result;       break;
            case 5: s = r.round;        break;
            case 6: s = r.date;         break;
            case 7: s = r.eco;          break;
            case 8: s = r.white_elo;    break;
            case 9: s = r.black_elo;    break;
            case 10: s = r.fen;         break;
            case 11: s = blob;          break;
        }
        unsigned int len = s.length();
        if( len < 255 )
        {
            unsigned char c = static_cast<unsigned char>(len);
            fields.push_back(c);
        }
        else if( len < 65535 )
        {
            unsigned char c = 255;
            fields.push_back(c);
            c = static_cast<unsigned char>(len&0xff);
            fields.push_back(c);
            c = static_cast<unsigned char>((len>>8)&0xff);
            fields.push_back(c);
        }
        else
        {
            bad = true;
            unsigned char c = 255;
            fields.push_back(c);
            fields.push_back(c);
            fields.push_back(c);
        }
        if( !bad )
            fields += s;
    }
}

void PackedGame::Pack( CompactGame &pact )
{
    CompressMoves press( pact.GetStartPosition() );
    std::string blob = press.Compress( pact.moves );
    Pack( pact.r, blob );
}

void PackedGame::Unpack( CompactGame &pact )
{
    std::string blob;
    Unpack( pact.r, blob );
    CompressMoves press( pact.GetStartPosition() );
    pact.moves = press.Uncompress( blob );
}

void PackedGame::Unpack( Roster &r, std::string &blob )
{
    int idx=0;
    for( int i=0; i<12; i++ )
    {
        std::string s;
        bool bad=false;
        unsigned char c = fields[idx++];
        unsigned int len = 0;
        if( c < 255 )
        {
            len = static_cast<unsigned int>(c);
        }
        else
        {
            unsigned char d = fields[idx++];
            unsigned char e = fields[idx++];
            if( d==255 && e==255 )
                bad = true;
            else
                len = static_cast<unsigned int>(d) + 256*static_cast<unsigned int>(e);
        }
        if( !bad )
        {
            std::string s = fields.substr(idx,len);
            switch(i)
            {
                case 0: r.white = s;        break;
                case 1: r.black = s;        break;
                case 2: r.event = s;        break;
                case 3: r.site = s;         break;
                case 4: r.result = s;       break;
                case 5: r.round = s;        break;
                case 6: r.date = s;         break;
                case 7: r.eco = s;          break;
                case 8: r.white_elo = s;    break;
                case 9: r.black_elo = s;    break;
                case 10: r.fen = s;         break;
                case 11: blob = s;          break;
            }
            idx += len;
        }
    }
}

