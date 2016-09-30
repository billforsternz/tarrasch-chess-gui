#if 0	// No longer needed - delete soon

/****************************************************************************
 * Text version of board bitmap
 *  40 pixel squares version
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BOARD_BITMAP_40_H
#define BOARD_BITMAP_40_H
#include "BoardBitmap.h"

class BoardBitmap40 : public BoardBitmap
{
public:
    const char **GetXpm();
    const char *GetWhiteKingMask()   ;
    const char *GetWhiteQueenMask()  ;
    const char *GetWhiteRookMask()   ;
    const char *GetWhiteBishopMask() ;
    const char *GetWhiteKnightMask() ;
    const char *GetWhitePawnMask()   ;
    const char *GetBlackKingMask()   ;
    const char *GetBlackQueenMask()  ;
    const char *GetBlackRookMask()   ;
    const char *GetBlackBishopMask() ;
    const char *GetBlackKnightMask() ;
    const char *GetBlackPawnMask()   ;
};

#endif // BOARD_BITMAP_40_H

#endif
