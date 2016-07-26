/****************************************************************************
 *  Packed storage of a binary block
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BINARY_BLOCK_H
#define BINARY_BLOCK_H

class BinaryBlock
{
    uint8_t buf[128];
    int offset[20];
    int shift[20];
    int mask[20];
    int idx;
    int bit_offset;  
    int frozen_size;  
public:
    BinaryBlock() { idx=0; bit_offset=0; }
    char *GetPtr() { return reinterpret_cast<char *>(&buf[0]); }
    void Next( int nbits )
    {
        offset[idx] = bit_offset/8;
        shift[idx] = bit_offset%8;
        switch(nbits)
        {
            case 1:  mask[idx] = 0x01;        break;
            case 2:  mask[idx] = 0x03;        break;
            case 3:  mask[idx] = 0x07;        break;
            case 4:  mask[idx] = 0x0f;        break;
            case 5:  mask[idx] = 0x1f;        break;
            case 6:  mask[idx] = 0x3f;        break;
            case 7:  mask[idx] = 0x7f;        break;
            case 8:  mask[idx] = 0xff;        break;
            case 9:  mask[idx] = 0x1ff;       break;
            case 10: mask[idx] = 0x3ff;       break;
            case 11: mask[idx] = 0x7ff;       break;
            case 12: mask[idx] = 0xfff;       break;
            case 13: mask[idx] = 0x1fff;      break;
            case 14: mask[idx] = 0x3fff;      break;
            case 15: mask[idx] = 0x7fff;      break;
            case 16: mask[idx] = 0xffff;      break;
            case 17: mask[idx] = 0x1ffff;     break;
            case 18: mask[idx] = 0x3ffff;     break;
            case 19: mask[idx] = 0x7ffff;     break;
            case 20: mask[idx] = 0xfffff;     break;
            case 21: mask[idx] = 0x1fffff;    break;
            case 22: mask[idx] = 0x3fffff;    break;
            case 23: mask[idx] = 0x7fffff;    break;
            case 24: mask[idx] = 0xffffff;    break;
        }
        idx++;
        bit_offset += nbits;        
    }
    int Size()
    {
        return bit_offset/8 + ((bit_offset%8==0)?0:1);  // round up
    }
    void Freeze()
    {
        frozen_size = Size();
    }
    int FrozenSize()
    {
        return frozen_size;
    }
    void Write( int idx_, uint32_t dat )
    {
        uint32_t *p = reinterpret_cast<uint32_t *>(&buf[offset[idx_]]);
        uint32_t raw = *p;
        uint32_t msk = mask[idx_];
        msk = msk << shift[idx_];
        dat = dat << shift[idx_];
        raw = raw & (~msk);
        raw = raw | dat;
        *p = raw;
    }    
    uint32_t Read( int idx_ )
    {
        uint32_t *p = reinterpret_cast<uint32_t *>(&buf[offset[idx_]]);
        uint32_t raw = *p;
        raw = raw >> (shift[idx_]);
        uint32_t dat = raw & mask[idx_];
        return dat;
    }    
    uint32_t Read( int idx_, const char *location )
    {
        const uint32_t *p = reinterpret_cast<const uint32_t *>(&location[offset[idx_]]);
        uint32_t raw = *p;
        raw = raw >> (shift[idx_]);
        uint32_t dat = raw & mask[idx_];
        return dat;
    }    
};

#endif // BINARY_BLOCK_H
