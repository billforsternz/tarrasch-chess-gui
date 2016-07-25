/******************************************************************************
 * Access text version of board bitmap
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ******************************************************************************/
#ifndef BOARD_BITMAP_H
#define BOARD_BITMAP_H

class BoardBitmap
{
public:
    virtual const char **GetXpm()            = 0;
    virtual const char *GetWhiteKingMask()   = 0;
    virtual const char *GetWhiteQueenMask()  = 0;
    virtual const char *GetWhiteRookMask()   = 0;
    virtual const char *GetWhiteBishopMask() = 0;
    virtual const char *GetWhiteKnightMask() = 0;
    virtual const char *GetWhitePawnMask()   = 0;
    virtual const char *GetBlackKingMask()   = 0;
    virtual const char *GetBlackQueenMask()  = 0;
    virtual const char *GetBlackRookMask()   = 0;
    virtual const char *GetBlackBishopMask() = 0;
    virtual const char *GetBlackKnightMask() = 0;
    virtual const char *GetBlackPawnMask()   = 0;
};

#endif //BOARD_BITMAP_H
