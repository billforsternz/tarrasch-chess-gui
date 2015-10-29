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
    bool complete=false;
    for( int attempt=0; !complete && attempt<2; attempt++ )
    {
        complete = true; // normally only one attempt required
        fields = (attempt==0 ? "1123456789012" : "2x1x2x3x4x5x6x7x8x9x0x1x2");
        unsigned int offset_idx=1;
        unsigned int offset=0;
        for( int i=0; i<12; i++ )
        {
            std::string s;
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
            if( attempt == 0 )
            {
                if( offset > 255 )
                {
                    complete = false; // move on to second attempt
                    break;
                }
                unsigned char c = static_cast<unsigned char>(offset);
                fields[offset_idx++] = c;
            }
            else
            {
                unsigned char c = static_cast<unsigned char>(offset&0xff);
                unsigned char d = static_cast<unsigned char>((offset>>8)&0xff);
                if( offset > 65535 )
                {
                    c = 255;
                    d = 255;
                }
                fields[offset_idx++] = c;
                fields[offset_idx++] = d;
            }
            fields += s;
            offset += s.length();
            fields += '\0';
            offset++;
        }
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
    Unpack(blob);
    Unpack(r);
}

void PackedGame::Unpack( std::string &blob )
{
    int i=11;
    bool single = (fields[0] == '1');
    int begin_idx = single ? 1+i : 1+2*i;
    bool bad=false;
    unsigned char c, d;
    unsigned int end_offset;
    unsigned int begin_offset;
    if( single )
    {
        c = fields[begin_idx++];
        begin_offset = static_cast<unsigned int>(c);
        end_offset = fields.length()-13;
    }
    else
    {
        c = fields[begin_idx++];
        d = fields[begin_idx++];
        if( c==255 && d==255 )
            bad = true;
        else
            begin_offset = static_cast<unsigned int>(c) + 256*static_cast<unsigned int>(d);
        end_offset = fields.length()-25;
    }
    blob = bad ? "" : fields.substr( (single?13:25)+begin_offset, end_offset-begin_offset-1 );
}

void PackedGame::Unpack( Roster &r )
{
    bool single = (fields[0] == '1');
    for( int i=0; i<11; i++ )
    {
        int begin_idx = single ? 1+i : 1+2*i;
        int end_idx   = single ? 1+(i+1) : 1+2*(i+1);
        bool bad=false;
        unsigned char c, d;
        unsigned int end_offset;
        unsigned int begin_offset;
        if( single )
        {
            c = fields[begin_idx++];
            begin_offset = static_cast<unsigned int>(c);
            c = fields[end_idx++];
            end_offset = static_cast<unsigned int>(c);
        }
        else
        {
            c = fields[begin_idx++];
            d = fields[begin_idx++];
            if( c==255 && d==255 )
                bad = true;
            else
                begin_offset = static_cast<unsigned int>(c) + 256*static_cast<unsigned int>(d);
            c = fields[end_idx++];
            d = fields[end_idx++];
            if( c==255 && d==255 )
                bad = true;
            else
                end_offset = static_cast<unsigned int>(c) + 256*static_cast<unsigned int>(d);
        }
        std::string s = bad ? "" : fields.substr( (single?13:25)+begin_offset, end_offset-begin_offset-1 );
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
        }
    }
}


const char *PackedGame::White()
{
    bool single = (fields[0] == '1');
    return &fields[single?13:25];
}

const char *PackedGame::Black()
{
    bool single = (fields[0] == '1');
    int i=1;
    int begin_idx = single ? 1+i : 1+2*i;
    bool bad=false;
    unsigned char c, d;
    unsigned int begin_offset;
    if( single )
    {
        c = fields[begin_idx++];
        begin_offset = static_cast<unsigned int>(c);
    }
    else
    {
        c = fields[begin_idx++];
        d = fields[begin_idx++];
        if( c==255 && d==255 )
            bad = true;
        else
            begin_offset = static_cast<unsigned int>(c) + 256*static_cast<unsigned int>(d);
    }
    return bad ? "" : &fields[ (single?13:25)+begin_offset ];
}
