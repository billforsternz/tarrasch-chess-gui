/****************************************************************************
 * Pack a complete game into a single string
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "CompressMoves.h"
#include "PackedGame.h"

#define HDR_LEN_SINGLE 12    // '1' + single byte offsets of 11 fields
#define HDR_LEN_DOUBLE 23    // '2' + double byte offsets of 11 fields

PackedGame::PackedGame
(
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
)
{
    char buf[2048];
    bool complete=false;
    char *ptr=buf;
    for( int attempt=0; !complete && attempt<2; attempt++ )
    {
        ptr = buf;
        complete = true; // normally only one attempt required
        if( attempt == 0 )
        {
            strcpy( ptr, "112345678901" );
            ptr += 12;
        }
        else
        {
            strcpy( ptr, "2x1x2x3x4x5x6x7x8x9x0x1" );
            ptr += 23;
        }
        const char *base = ptr;
        unsigned int offset_idx=1;
        if( len1 > 128 )
            len1 = 128;
        memcpy( ptr, white, len1 );
        ptr += len1;
        *ptr++ = '\0';
        for( int i=2; i<=12; i++ )
        {
            int offset = (ptr-base);
            switch(i)
            {
                case 2:
                {
                    if( len2 > 128 )
                        len2 = 128;
                    memcpy( ptr, black, len2 );
                    ptr += len2;
                    *ptr++ = '\0';
                    break;
                }
                case 3:
                {
                    if( len3 > 128 )
                        len3 = 128;
                    memcpy( ptr, event, len3 );
                    ptr += len3;
                    *ptr++ = '\0';
                    break;
                }
                case 4:
                {
                    if( len4 > 128 )
                        len4 = 128;
                    memcpy( ptr, site, len4 );
                    ptr += len4;
                    *ptr++ = '\0';
                    break;
                }
                case 5:
                {
                    if( len5 > 8 )
                        len5 = 8;
                    memcpy( ptr, result, len5 );
                    ptr += len5;
                    *ptr++ = '\0';
                    break;
                }
                case 6:
                {
                    if( len6 > 32 )
                        len6 = 32;
                    memcpy( ptr, round, len6 );
                    ptr += len6;
                    *ptr++ = '\0';
                    break;
                }
                case 7:
                {
                    if( len7 > 16 )
                        len7 = 16;
                    memcpy( ptr, date, len7 );
                    ptr += len7;
                    *ptr++ = '\0';
                    break;
                }
                case 8:
                {
                    if( len8 > 8 )
                        len8 = 8;
                    memcpy( ptr, eco, len8 );
                    ptr += len8;
                    *ptr++ = '\0';
                    break;
                }
                case 9:
                {
                    if( len9 > 8 )
                        len9 = 8;
                    memcpy( ptr, white_elo, len9 );
                    ptr += len9;
                    *ptr++ = '\0';
                    break;
                }
                case 10:
                {
                    if( len10 > 8 )
                        len10 = 8;
                    memcpy( ptr, black_elo, len10 );
                    ptr += len10;
                    *ptr++ = '\0';
                    break;
                }
                case 11:
                {
                    if( len11 > 128 )
                        len11 = 128;
                    memcpy( ptr, fen, len11 );
                    ptr += len11;
                    *ptr++ = '\0';
                    break;
                }
                case 12:
                {
                    if( ptr+len12 > &buf[sizeof(buf)-2] )
                        len12 = &buf[sizeof(buf)-2] - ptr;
                    memcpy( ptr, moves_blob, len12 );
                    ptr += len12;
                    *ptr++ = '\0';
                    break;
                }
            }
            if( attempt == 0 )
            {
                if( offset > 255 )
                {
                    complete = false; // move on to second attempt
                    break;
                }
                unsigned char c = static_cast<unsigned char>(offset);
                buf[offset_idx++] = c;
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
                buf[offset_idx++] = c;
                buf[offset_idx++] = d;
            }
        }
    }
    fields = std::string(buf,ptr);
}

void PackedGame::Pack( Roster &r, std::string &blob )
{
    bool complete=false;
    for( int attempt=0; !complete && attempt<2; attempt++ )
    {
        complete = true; // normally only one attempt required
        fields = (attempt==0 ? "112345678901" : "2x1x2x3x4x5x6x7x8x9x0x1");
        unsigned int offset_idx=1;
        unsigned int offset=0;
        fields += r.white;
        offset += r.white.length();
        fields += '\0';
        offset++;
        for( int i=1; i<12; i++ )
        {
            std::string s;
            switch(i)
            {
                //case 0: s = r.white;        break;
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
    int begin_idx = single ? 1+(i-1) : 1+2*(i-1);
    bool bad=false;
    unsigned char c, d;
    unsigned int end_offset=0;
    unsigned int begin_offset=0;
    if( single )
    {
        c = fields[begin_idx++];
        begin_offset = static_cast<unsigned int>(c);
        end_offset = fields.length()-HDR_LEN_SINGLE;
    }
    else
    {
        c = fields[begin_idx++];
        d = fields[begin_idx++];
        if( c==255 && d==255 )
            bad = true;
        else
            begin_offset = static_cast<unsigned int>(c) + 256*static_cast<unsigned int>(d);
        end_offset = fields.length()-HDR_LEN_DOUBLE;
    }
    blob = bad ? "" : fields.substr( (single?HDR_LEN_SINGLE:HDR_LEN_DOUBLE)+begin_offset, end_offset-begin_offset-1 );
}

void PackedGame::Unpack( Roster &r )
{
    bool single = (fields[0] == '1');
    for( int i=0; i<11; i++ )
    {
        int begin_idx = single ? 1+(i-1) : 1+2*(i-1);
        int end_idx   = single ? 1+i : 1+2*i;
        bool bad=false;
        unsigned char c, d;
        unsigned int end_offset=0;
        unsigned int begin_offset=0;
        if( single )
        {
            if( i != 0 )
            {
                c = fields[begin_idx++];
                begin_offset = static_cast<unsigned int>(c);
            }
            c = fields[end_idx++];
            end_offset = static_cast<unsigned int>(c);
        }
        else
        {
            if( i != 0 )
            {
                c = fields[begin_idx++];
                d = fields[begin_idx++];
                if( c==255 && d==255 )
                    bad = true;
                else
                    begin_offset = static_cast<unsigned int>(c) + 256*static_cast<unsigned int>(d);
            }
            c = fields[end_idx++];
            d = fields[end_idx++];
            if( c==255 && d==255 )
                bad = true;
            else
                end_offset = static_cast<unsigned int>(c) + 256*static_cast<unsigned int>(d);
        }
        std::string s = bad ? "" : fields.substr( (single?HDR_LEN_SINGLE:HDR_LEN_DOUBLE)+begin_offset, end_offset-begin_offset-1 );
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

const char *PackedGame::Black()
{
    return GetField(1);
}

const char *PackedGame::Event()
{
    return GetField(2);
}

const char *PackedGame::Site()
{
    return GetField(3);
}

const char *PackedGame::Result()
{
    return GetField(4);
}

const char *PackedGame::Round()
{
    return GetField(5);
}

const char *PackedGame::Date()
{
    return GetField(6);
}

const char *PackedGame::Eco()
{
    return GetField(7);
}

const char *PackedGame::WhiteElo()
{
    return GetField(8);
}

const char *PackedGame::BlackElo()
{
    return GetField(9);
}

const char *PackedGame::Fen()
{
    return GetField(10);
}

const char *PackedGame::Blob()
{
    return GetField(11);
}

const char *PackedGame::White()
{
    if( fields.length() == 0 )
        return( "?" );
    bool single = (fields[0] == '1');
    return &fields[single?HDR_LEN_SINGLE:HDR_LEN_DOUBLE];
}

const char *PackedGame::GetField( int field_nbr )
{
    if( fields.length() == 0 )
        return( "?" );
    bool single = (fields[0] == '1');
    unsigned int begin_offset=0;
    bool bad=false;
    if( field_nbr>0 )
    {
        int begin_idx = (single ? 1+(field_nbr-1) : 1+2*(field_nbr-1));
        unsigned char c, d;
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
    }
    return bad ? "" : &fields[ (single?HDR_LEN_SINGLE:HDR_LEN_DOUBLE)+begin_offset ];
}
