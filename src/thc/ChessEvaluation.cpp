/****************************************************************************
 * Chess classes - Simple chess AI, leaf scoring function for position
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <algorithm>
#include "DebugPrintf.h"
#include "ChessEvaluation.h"
#include "PrivateChessDefs.h"
using namespace std;
using namespace thc;

//-- preferences
#define USE_CHASE_PAWNS
//#define USE_WEAKER_CENTRAL
#define USE_STRONG_KING
#define USE_IN_THE_SQUARE
#define USE_LIQUIDATION

// Do we check for mate at a leaf node, and if so how do we do it ?
#define CHECK_FOR_LEAF_MATE
// #define CHECK_FOR_LEAF_MATE_USE_EVALUATE // not the fastest way

// Lookup table for quick calculation of material value of any piece
static int either_colour_material[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,500,  0,  0, 30,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
   10, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,500,  0,  0, 30,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
   10, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

/****************************************************************************
 * Calculate material that side to play can win directly
 *  Fast white to move version
 ****************************************************************************/
int ChessEvaluation::EnpriseWhite()
{    

    int best_so_far=0;  // amount of material that can be safely captured

    // Locals
    unsigned char defenders_buf[32];
    unsigned char *defenders;
    unsigned char attackers_buf[32];
    unsigned char *attackers;
    unsigned char *reorder_base, *base;
    unsigned char target;
    unsigned char attacker;
    Square square;
    Square attack_square;
    const lte *ptr;
    lte nbr_rays, nbr_squares;

    // For all squares
    for( square=a8; square<=h1; ++square )
    {

        // If piece on square is black, it's a potential target
        target = squares[square];
        if( IsBlack(target) )
        {
            attackers = attackers_buf;

            // It could be attacked by up to 2 white pawns
            ptr = pawn_attacks_black_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'P' )
                    *attackers++ = attacker;
            }

            // It could be attacked by up to 8 white knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {    
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'N' )
                    *attackers++ = attacker;
            }

            // From here on we may need to reorder the attackers
            reorder_base = base = attackers;

            // Move along each queen ray from the square being evaluated to
            //  each possible attacking square (looking for white attackers
            //  of a black piece)
            ptr = attacks_black_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {    
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    attack_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    attacker = squares[attack_square];

                    // If the square is occupied by an attacking (white) piece, and
                    //  it matches a piece that attacks down that ray we have found
                    //  an attacker
                    if( IsWhite(attacker) && (to_mask[attacker]&mask) )
                    {   
                        if( attacker != 'P' ) // we've already done pawn
                            *attackers++ = attacker;
                        if( attacker == 'K' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }    

                    // Any other piece or a defender, must move to end of ray
                    else if( !IsEmptySquare(attacker) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  attackers I'm afraid
                // If this ray generated attackers, and there were attackers
                //  before this ray
                if( attackers>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base==('K') )
                        swap = true;
                    else if( *reorder_base==('Q') && *base!=('K') )
                        swap = true;
                    else if( *reorder_base==('R') && *base==('B') )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    attackers
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != attackers )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != attackers )
                            *dst++ = *src++;
                    }
                }
                base = attackers;
            }

            // Any attackers ?
            if( attackers == attackers_buf )
                continue;  // No - continue to next square

            // Locals
            unsigned char defender;
            Square defend_square;

            // The target itself counts as a defender
            defenders = defenders_buf;
            *defenders++ = target;

            // It could be defended by up to 2 black pawns
            ptr = pawn_attacks_white_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'p' )
                    *defenders++ = defender;
            }

            // It could be defended by up to 8 black knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {    
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'n' )
                    *defenders++ = defender;
            }

            // From here on we may need to reorder the defenders
            reorder_base = base = defenders;

            // Move along each queen ray from the square being evaluated to
            //  each possible defending square (looking for black defenders
            //  of a black piece)
            ptr = attacks_white_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {    
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    defend_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    defender = squares[defend_square];

                    // If the square is occupied by an defending (black) piece, and
                    //  it matches a piece that defends down that ray we have found
                    //  a defender
                    if( IsBlack(defender) && (to_mask[defender]&mask) )
                    {   
                        if( defender != 'p' ) // we've already done pawn
                            *defenders++ = defender;
                        if( defender == 'k' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }    

                    // Any other piece or an attacker, must move to end of ray
                    else if( !IsEmptySquare(defender) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  defenders I'm afraid

                // If this ray generated defenders, and there were defenders
                //  before this ray
                if( defenders>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base=='k' )
                        swap = true;
                    else if( *reorder_base=='q' && *base!='k' )
                        swap = true;
                    else if( *reorder_base=='r' && *base=='b' )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    defenders
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != defenders )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != defenders )
                            *dst++ = *src++;
                    }
                }
                base = defenders;
            }

            // Figure out the net effect of the capturing sequence
            const unsigned char *a=attackers_buf;
            const unsigned char *d=defenders_buf;
            #define POS_INFINITY  1000000000
            #define NEG_INFINITY -1000000000
            int min = POS_INFINITY;
            int max = NEG_INFINITY;
            int net=0;  // net gain

            // While there are attackers and defenders
            while( a<attackers && d<defenders )
            {

                // Attacker captures
                net += either_colour_material[*d++];

                // Defender can elect to stop here
                if( net < min )
                    min = net;
        
                // Can defender recapture ?
                if( d == defenders )
                {
                    if( net > max )
                        max = net;
                    break;  // no
                }

                // Defender recaptures
                net -= either_colour_material[*a++];

                // Attacker can elect to stop here
                if( net > max )
                    max = net;
            }

            // Result is the lowest of best attacker can do and
            //  best defender can do
            int score = min<=max ? min : max;
            if( score > best_so_far )
                best_so_far = score;
        }   // end if black target
    } // end square loop

    // After looking at every square, return the best result
    return best_so_far;
}


/****************************************************************************
 * Calculate material that side to play can win directly
 *  Fast black to move version
 ****************************************************************************/
int ChessEvaluation::EnpriseBlack()
{    
    int best_so_far=0;  // amount of material that can be safely captured
    
    // Locals
    unsigned char defenders_buf[32];
    unsigned char *defenders;
    unsigned char attackers_buf[32];
    unsigned char *attackers;
    unsigned char *reorder_base, *base;
    unsigned char target;
    unsigned char attacker;
    Square square;
    Square attack_square;
    const lte *ptr;
    lte nbr_rays, nbr_squares;

    // For all squares
    for( square=a8; square<=h1; ++square )
    {

        // If piece on square is white, it's a potential target
        target = squares[square];
        if( IsWhite(target) )
        {
            attackers = attackers_buf;

            // It could be attacked by up to 2 black pawns
            ptr = pawn_attacks_white_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'p' )
                    *attackers++ = attacker;
            }

            // It could be attacked by up to 8 black knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {    
                attack_square = (Square)*ptr++;
                if( (attacker = squares[attack_square]) == 'n' )
                    *attackers++ = attacker;
            }

            // From here on we may need to reorder the attackers
            reorder_base = base = attackers;

            // Move along each queen ray from the square being evaluated to
            //  each possible attacking square (looking for black attackers
            //  of a white piece)
            ptr = attacks_white_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {    
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    attack_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    attacker = squares[attack_square];

                    // If the square is occupied by an attacking (black) piece, and
                    //  it matches a piece that attacks down that ray we have found
                    //  an attacker
                    if( IsBlack(attacker) && (to_mask[attacker]&mask) )
                    {   
                        if( attacker != 'p' ) // we've already done pawn
                            *attackers++ = attacker;
                        if( attacker == 'k' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }    

                    // Any other piece or a defender, must move to end of ray
                    else if( !IsEmptySquare(attacker) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  attackers I'm afraid
                // If this ray generated attackers, and there were attackers
                //  before this ray
                if( attackers>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base==('k') )
                        swap = true;
                    else if( *reorder_base==('q') && *base!=('k') )
                        swap = true;
                    else if( *reorder_base==('r') && *base==('b') )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    attackers
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != attackers )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != attackers )
                            *dst++ = *src++;
                    }
                }
                base = attackers;
            }

            // Any attackers ?
            if( attackers == attackers_buf )
                continue;  // No - continue to next square

            // Locals
            unsigned char defender;
            Square defend_square;

            // The target itself counts as a defender
            defenders = defenders_buf;
            *defenders++ = target;

            // It could be defended by up to 2 white pawns
            ptr = pawn_attacks_black_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'P' )
                    *defenders++ = defender;
            }

            // It could be defended by up to 8 white knights
            ptr = knight_lookup[square];
            nbr_squares = *ptr++;
            while( nbr_squares-- )
            {    
                defend_square = (Square)*ptr++;
                if( (defender = squares[defend_square]) == 'N' )
                    *defenders++ = defender;
            }

            // From here on we may need to reorder the defenders
            reorder_base = base = defenders;

            // Move along each queen ray from the square being evaluated to
            //  each possible defending square (looking for white defenders
            //  of a white piece)
            ptr = attacks_black_lookup[square];
            nbr_rays = *ptr++;
            while( nbr_rays-- )
            {    
                nbr_squares = *ptr++;
                while( nbr_squares-- )
                {
                    defend_square = (Square)*ptr++;
                    lte mask      = *ptr++;
                    defender = squares[defend_square];

                    // If the square is occupied by an defending (white) piece, and
                    //  it matches a piece that defends down that ray we have found
                    //  a defender
                    if( IsWhite(defender) && (to_mask[defender]&mask) )
                    {   
                        if( defender != 'P' ) // we've already done pawn
                            *defenders++ = defender;
                        if( defender == 'K' ) // don't look beyond a king
                        {
                            ptr += (nbr_squares+nbr_squares);
                            nbr_squares = 0;
                        }
                    }    

                    // Any other piece or an attacker, must move to end of ray
                    else if( !IsEmptySquare(defender) )
                    {
                        ptr += (nbr_squares+nbr_squares);
                        nbr_squares = 0;
                    }
                }

                // Do a limited amount of reordering at end of rays
                //  This will not optimally reorder an absurdly large number of
                //  defenders I'm afraid

                // If this ray generated defenders, and there were defenders
                //  before this ray
                if( defenders>base && base>reorder_base)
                {
                    bool swap=false;
                    if( *reorder_base==('K') )
                        swap = true;
                    else if( *reorder_base==('Q') && *base!=('K') )
                        swap = true;
                    else if( *reorder_base==('R') && *base==('B') )
                        swap = true;
                    if( swap )
                    {
                        unsigned char temp[32];
                        unsigned char *src, *dst;

                        // AAAABBBBBBBB
                        // ^   ^       ^
                        // |   |       |
                        // |   base    defenders
                        // reorder_base
                        // stage 1, AAAA -> temp
                        src = reorder_base;
                        dst = temp;
                        while( src != base )
                            *dst++ = *src++;

                        // stage 2, BBBBBBBB -> reorder_base
                        src = base;
                        dst = reorder_base;
                        while( src != defenders )
                            *dst++ = *src++;

                        // stage 3, replace AAAA after BBBBBBBB
                        src = temp;
                        while( dst != defenders )
                            *dst++ = *src++;
                    }
                }
                base = defenders;
            }

            // Figure out the net effect of the capturing sequence
            const unsigned char *a=attackers_buf;
            const unsigned char *d=defenders_buf;
            int min = POS_INFINITY;
            int max = NEG_INFINITY;
            int net=0;  // net gain

            // While there are attackers and defenders
            while( a<attackers && d<defenders )
            {

                // Attacker captures
                net += either_colour_material[*d++];

                // Defender can elect to stop here
                if( net < min )
                    min = net;
        
                // Can defender recapture ?
                if( d == defenders )
                {
                    if( net > max )
                        max = net;
                    break;  // no
                }

                // Defender recaptures
                net -= either_colour_material[*a++];

                // Attacker can elect to stop here
                if( net > max )
                    max = net;
            }

            // Result is the lowest of best attacker can do and
            //  best defender can do
            int score = min<=max ? min : max;
            if( score > best_so_far )
                best_so_far = score;
        }   // end if white target
    } // end square loop

    // After looking at every square, return the best result
    return best_so_far;
}



//=========== EVALUATION ===============================================

static int king_ending_bonus_static[] =
{
    #if 1
    /*  0x00-0x07 a8-h8 */ -25,-25,-25,-25,-25,-25,-25,-25,    
    /*  0x00-0x0f a7-h7 */ -25,  0,  0,  0,  0,  0,  0,-25,    
    /*  0x10-0x17 a6-h6 */ -25,  0, 25, 25, 25, 25,  0,-25,    
    /*  0x10-0x1f a5-h5 */ -25,  0, 25, 50, 50, 25,  0,-25,    
    /*  0x20-0x27 a4-h4 */ -25,  0, 25, 50, 50, 25,  0,-25,    
    /*  0x20-0x2f a3-h3 */ -25,  0, 25, 25, 25, 25,  0,-25,    
    /*  0x30-0x37 a2-h2 */ -25,  0,  0,  0,  0,  0,  0,-25,    
    /*  0x30-0x3f a1-h1 */ -25,-25,-25,-25,-25,-25,-25,-25     
    #else
    /*  0x00-0x07 a8-h8 */ -10,-10,-10,-10,-10,-10,-10,-10,    
    /*  0x00-0x0f a7-h7 */ -10,  0,  0,  0,  0,  0,  0,-10,    
    /*  0x10-0x17 a6-h6 */ -10,  0, 10, 10, 10, 10,  0,-10,    
    /*  0x10-0x1f a5-h5 */ -10,  0, 10, 20, 20, 10,  0,-10,    
    /*  0x20-0x27 a4-h4 */ -10,  0, 10, 20, 20, 10,  0,-10,    
    /*  0x20-0x2f a3-h3 */ -10,  0, 10, 10, 10, 10,  0,-10,    
    /*  0x30-0x37 a2-h2 */ -10,  0,  0,  0,  0,  0,  0,-10,    
    /*  0x30-0x3f a1-h1 */ -10,-10,-10,-10,-10,-10,-10,-10     
    #endif
};

static int king_ending_bonus_dynamic_white[0x80];
static int king_ending_bonus_dynamic_black[0x80];

// Lookup table for quick calculation of material value of white piece
static int white_material[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,500,  0,  0, 30,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
   10, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

// Lookup table for quick calculation of material value of black piece
static int black_material[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0,-31,  0,  0,  0,  0,  0,  0,  0,  0,-500, 0,  0,-30,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
  -10,-90,-50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

// Lookup table for quick calculation of material value of white piece (not pawn or king)
static int white_pieces[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
    0, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

// Lookup table for quick calculation of material value of black piece (not pawn or king)
static int black_pieces[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x00-0x0f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x10-0x1f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x20-0x2f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x30-0x3f
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x40-0x4f    'B'=0x42, 'K'=0x4b, 'N'=0x4e
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0x50-0x5f    'P'=0x50, 'Q'=0x51, 'R'=0x52
    0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30,  0,   // 0x60-0x6f    'b'=0x62, 'k'=0x6b, 'n'=0x6e
    0, 90, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    // 0x70-0x7f    'p'=0x70, 'q'=0x71, 'r'=0x72
};

/****************************************************************************
 * Do some planning before making a move
 *   (needs a lot of improvement)
 ****************************************************************************/
void ChessEvaluation::Planning()
{    
    Square square, weaker_king, bonus_square;
    char piece;
    int score_black_material = 0;
    int score_white_material = 0;
    const int MATERIAL_ENDING  = (500 + ((8*10+4*30+2*50+90)*1)/3);
    int *bonus_ptr;

    // Get material for both sides
    int score_black_pieces = 0;
    int score_white_pieces = 0;
    for( square=a8; square<=h1; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
    }
    int score_white_pawns = score_white_material - 500 // -500 is king
                          - score_white_pieces;
    planning_score_white_pieces = score_white_pieces;
    planning_white_piece_pawn_percent = 1000;
    if( score_white_pawns )
    {
        planning_white_piece_pawn_percent = (100*score_white_pieces) /
                                                score_white_pawns;
        if( planning_white_piece_pawn_percent > 1000 )
            planning_white_piece_pawn_percent = 1000;
    }
    int score_black_pawns = (0-score_black_material) - 500 // -500 is king
                          - score_black_pieces;
    planning_score_black_pieces = score_black_pieces;
    planning_black_piece_pawn_percent = 1000;
    if( score_black_pawns )
    {
        planning_black_piece_pawn_percent = (100*score_black_pieces) /
                                                score_black_pawns;
        if( planning_black_piece_pawn_percent > 1000 )
            planning_black_piece_pawn_percent = 1000;
    }

    // Reset dynamic king position arrays
    memset( king_ending_bonus_dynamic_white,
            0,
            sizeof(king_ending_bonus_dynamic_white) );
    memset( king_ending_bonus_dynamic_black,
            0,
            sizeof(king_ending_bonus_dynamic_black) );

    // Are we in an ending ?
    bool ending = (score_white_material<MATERIAL_ENDING && score_black_material>0-MATERIAL_ENDING);

    // Adjust material for enprise
    if( white )
        score_white_material += EnpriseWhite();
    else
        score_black_material -= EnpriseBlack();
    int sum = score_white_material+score_black_material;

    // Find stronger side
    white_is_better = false;
    black_is_better = false;
    if( sum > 0 )
        white_is_better = true;
    else if( sum < 0 )
        black_is_better = true;

    // Are we in an ending ?
    if( ending )
    { 

        // Reset dynamic king position arrays
        memcpy( king_ending_bonus_dynamic_white,
                king_ending_bonus_static,
                sizeof(king_ending_bonus_dynamic_white) );
        memcpy( king_ending_bonus_dynamic_black,
                king_ending_bonus_static,
                sizeof(king_ending_bonus_dynamic_black) );

        // Encourage kings to go where the pawns are
        #ifdef USE_CHASE_PAWNS
        for( Square square=a8; square<=h1; ++square )
        {
            if( squares[square] == 'P' )
                king_ending_bonus_dynamic_black[square] = king_ending_bonus_static[e5];
            if( squares[square] == 'p' )
                king_ending_bonus_dynamic_white[square] = king_ending_bonus_static[e5];
        }
        #endif

        // Reward stronger side putting his K an extended knight's
        //  square away from weaker side's K, if it's toward centre
        if( white_is_better || black_is_better )
        {
            if( white_is_better )
            {
                bonus_ptr   = king_ending_bonus_dynamic_white;
                weaker_king = (Square)bking_square;
            }
            else
            {
                bonus_ptr   = king_ending_bonus_dynamic_black;
                weaker_king = (Square)wking_square;
            }

            const lte *ptr = good_king_position_lookup[weaker_king];
            lte nbr_squares = *ptr++;
            while( nbr_squares-- )
            {    
                bonus_square = (Square)*ptr++;
                if( king_ending_bonus_static[bonus_square] >
                    king_ending_bonus_static[weaker_king]
                  )
                {
                    #ifdef USE_WEAKER_CENTRAL
                    bonus_ptr[bonus_square] += king_ending_bonus_static[f3];
                    #else
                    bonus_ptr[bonus_square] += king_ending_bonus_static[e5];
                    #endif
                }
            }
        }
    }
}

/****************************************************************************
 * Evaluate a position, leaf node
 *
 *	 (this makes a rather ineffectual effort to score positional features
 *    needs a lot of improvement)
 ****************************************************************************/
void ChessEvaluation::EvaluateLeaf( int &material, int &positional )
{    
	//DIAG_evaluate_leaf_count++;	
    Square square;
    char   piece;
    int file;
    int bonus = 0;
    int score_black_material = 0;
    int score_white_material = 0;
    int black_connected=0;
    int white_connected=0;

    int white_king_safety_bonus          =0;
    int white_king_central_bonus         =0;
    int white_queen_central_bonus        =0;
    int white_queen_developed_bonus      =0;
    int white_queen78_bonus              =0;
    int white_undeveloped_minor_bonus    =0;

    int black_king_safety_bonus          =0;
    int black_king_central_bonus         =0;
    int black_queen_central_bonus        =0;
    int black_queen_developed_bonus      =0;
    int black_queen78_bonus              =0;
    int black_undeveloped_minor_bonus    =0;
            

#define BONUS_WHITE_SWAP_PIECE          60
#define BONUS_BLACK_SWAP_PIECE          -60
#define BONUS_BLACK_CONNECTED_ROOKS     -10
#define BONUS_BLACK_BLOCKED_BISHOP      10
#define BLACK_UNDEVELOPED_MINOR_BONUS   3
#define BONUS_BLACK_KNIGHT_CENTRAL0     -8
#define BONUS_BLACK_KNIGHT_CENTRAL1     -9
#define BONUS_BLACK_KNIGHT_CENTRAL2     -10
#define BONUS_BLACK_KNIGHT_CENTRAL3     -12
#define BONUS_BLACK_KING_SAFETY         -10
#define BONUS_BLACK_KING_CENTRAL0       -8
#define BONUS_BLACK_KING_CENTRAL1       -9
#define BONUS_BLACK_KING_CENTRAL2       -10
#define BONUS_BLACK_KING_CENTRAL3       -12
#define BONUS_BLACK_QUEEN_CENTRAL       -10
#define BONUS_BLACK_QUEEN_DEVELOPED     -10
#define BONUS_BLACK_QUEEN78             -5 
#define BONUS_BLACK_ROOK7               -5 
#define BONUS_BLACK_PAWN5               -20     // boosted because now must be passed
#define BONUS_BLACK_PAWN6               -30     // boosted because now must be passed
#define BONUS_BLACK_PAWN7               -40     // boosted because now must be passed
#define BONUS_BLACK_PAWN_CENTRAL        -5

#define BONUS_WHITE_CONNECTED_ROOKS      10
#define BONUS_WHITE_BLOCKED_BISHOP       -10
#define WHITE_UNDEVELOPED_MINOR_BONUS    -3
#define BONUS_WHITE_KNIGHT_CENTRAL0      8
#define BONUS_WHITE_KNIGHT_CENTRAL1      9
#define BONUS_WHITE_KNIGHT_CENTRAL2      10
#define BONUS_WHITE_KNIGHT_CENTRAL3      12
#define BONUS_WHITE_KING_SAFETY          10
#define BONUS_WHITE_KING_CENTRAL0        8
#define BONUS_WHITE_KING_CENTRAL1        9
#define BONUS_WHITE_KING_CENTRAL2        10
#define BONUS_WHITE_KING_CENTRAL3        12
#define BONUS_WHITE_QUEEN_CENTRAL        10
#define BONUS_WHITE_QUEEN_DEVELOPED      10
#define BONUS_WHITE_QUEEN78              5 
#define BONUS_WHITE_ROOK7                5 
#define BONUS_WHITE_PAWN5                20     // boosted because now must be passed
#define BONUS_WHITE_PAWN6                30     // boosted because now must be passed
#define BONUS_WHITE_PAWN7                40     // boosted because now must be passed
#define BONUS_WHITE_PAWN_CENTRAL         5
#define BONUS_STRONG_KING                50

const int MATERIAL_OPENING = (500 + ((8*10+4*30+2*50+90)*2)/3);
const int MATERIAL_MIDDLE  = (500 + ((8*10+4*30+2*50+90)*1)/3);

    
    Square black_king_square = SQUARE_INVALID;
    Square white_king_square = SQUARE_INVALID;
    Square black_pawns_buf[16];
    Square white_pawns_buf[16];
    Square black_passers_buf[16];
    Square white_passers_buf[16];
    Square *black_passers =  black_passers_buf;
    Square *white_passers =  white_passers_buf;
    Square *black_pawns   =  black_pawns_buf;
    Square *white_pawns   =  white_pawns_buf;
    int score_black_pieces = 0;
    int score_white_pieces = 0;

    // a8->h8    
    for( square=a8; square<=h8; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                break;
            }

            case 'r':
            {
                black_connected++;
                if( black_connected == 2 )
                    bonus += BONUS_BLACK_CONNECTED_ROOKS;
                break;
            }

            case 'n':
            {
                black_connected=2;
                black_undeveloped_minor_bonus++;
                break;
            }

            case 'b':
            {
                black_connected=2;
                black_undeveloped_minor_bonus++;
                if( a8==square && IsBlack(squares[b7]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else if( h8==square && IsBlack(squares[g7]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else
                {
                    if( IsBlack(squares[SE(square)]) &&
                        IsBlack(squares[SW(square)])
                      )
                    {
                        bonus += BONUS_BLACK_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'q':
            {
                black_connected=2;
                break;
            }

            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                black_connected=2;
                file = IFILE(square);
                if( file<2 || file>5 )
                    black_king_safety_bonus = BONUS_BLACK_KING_SAFETY;
                break;
            }

            case 'Q':
            {
                white_queen78_bonus = BONUS_WHITE_QUEEN78;
                break;
            }
        }
    }

    // a7->h7    
    unsigned int next_passer_mask = 0;
    unsigned int passer_mask = 0;
    unsigned int three_files = 0x1c0;   // 1 1100 0000
    for( square=a7; square<=h7; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                break;
            }

            case 'b':
            {
                if( a7==square && IsBlack(squares[b6]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else if( h7==square && IsBlack(squares[g6]) )
                    bonus += BONUS_BLACK_BLOCKED_BISHOP;
                else
                {
                    if( IsBlack(squares[SE(square)]) &&
                        IsBlack(squares[SW(square)])
                      )
                    {
                        bonus += BONUS_BLACK_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'q':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_queen_developed_bonus = BONUS_BLACK_QUEEN_DEVELOPED;
                break;
            }

            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( file<2 || file>5 )
                    black_king_safety_bonus = BONUS_BLACK_KING_SAFETY;
                break;
            }

            case 'R':
            {
                bonus += BONUS_WHITE_ROOK7;
                break;
            }

            case 'Q':
            {
                white_queen78_bonus = BONUS_WHITE_QUEEN78;
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                *white_passers++ = square;
                bonus += BONUS_WHITE_PAWN7;
                #ifdef USE_STRONG_KING
                Square ahead = NORTH(square);
                if( squares[ahead]=='K' && king_ending_bonus_dynamic_white[ahead]==0 )
                    bonus += BONUS_STRONG_KING;
                #endif
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                passer_mask |= three_files;
                break;
            }
        }
        three_files >>= 1;
    }

    // a6->h6
    unsigned int file_mask = 0x80;  // 0 1000 0000
    three_files = 0x1c0;            // 1 1100 0000
    for( square=a6; square<=h6; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL0;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL0;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL3;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL3;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'P':
            {
                *white_pawns++ = square;
                if( !(passer_mask&file_mask) )
                {
                    *white_passers++ = square;
                    bonus += BONUS_WHITE_PAWN6;
                    #ifdef USE_STRONG_KING
                    Square ahead = NORTH(square);
                    if( squares[ahead]=='K' && king_ending_bonus_dynamic_white[ahead]==0 )
                        bonus += BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                next_passer_mask |= three_files;
                break;
            }
        }
        file_mask   >>= 1;
        three_files >>= 1;
    }
    passer_mask |= next_passer_mask;
    
    // a5->h5;
    file_mask   = 0x80;             // 0 1000 0000
//  three_files = 0x1c0;            // 1 1100 0000
    for( square=a5; square<=h5; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL1;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL1;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL2;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL2;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'P':
            {
                *white_pawns++ = square;
                file = IFILE(square);
                if( file==3 || file==4 )
                    bonus += BONUS_WHITE_PAWN_CENTRAL;
                if( !(passer_mask&file_mask) )
                {
                    *white_passers++ = square;
                    bonus += BONUS_WHITE_PAWN5;
                    #ifdef USE_STRONG_KING
                    Square ahead = NORTH(square);
                    if( squares[ahead]=='K' && king_ending_bonus_dynamic_white[ahead]==0 )
                        bonus += BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_PAWN_CENTRAL;
                break;
            }
        }
        file_mask   >>= 1;
        // three_files >>= 1;
    }

    // a2->h2
    next_passer_mask = 0;
    passer_mask = 0;
    three_files = 0x1c0;   // 1 1100 0000
    for( square=a2; square<=h2; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                break;
            }

            case 'B':
            {
                if( a2==square && IsWhite(squares[b3]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else if( h2==square && IsWhite(squares[g3]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else
                {
                    if( IsWhite(squares[NW(square)]) &&
                        IsWhite(squares[NE(square)])
                      )
                    {
                        bonus += BONUS_WHITE_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'Q':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_queen_developed_bonus = BONUS_WHITE_QUEEN_DEVELOPED;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( file<2 || file>5 )
                    white_king_safety_bonus = BONUS_WHITE_KING_SAFETY;
                break;
            }

            case 'r':
            {
                bonus += BONUS_BLACK_ROOK7;
                break;
            }

            case 'q':
            {
                black_queen78_bonus = BONUS_BLACK_QUEEN78;
                break;
            }

            case 'p':
            {
                *black_pawns++   = square;
                *black_passers++ = square;
                bonus += BONUS_BLACK_PAWN7;
                #ifdef USE_STRONG_KING
                Square ahead = SOUTH(square);
                if( squares[ahead]=='k' && king_ending_bonus_dynamic_black[ahead]==0 )
                    bonus -= BONUS_STRONG_KING;
                #endif
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                passer_mask |= three_files;
                break;
            }
        }
        three_files >>= 1;
    }

    // a3->h3
    file_mask = 0x80;       // 0 1000 0000
    three_files = 0x1c0;    // 1 1100 0000
    for( square=a3; square<=h3; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL3;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL3;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL0;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL0;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                if( !(passer_mask&file_mask) )
                {
                    *black_passers++ = square;
                    bonus += BONUS_BLACK_PAWN6;
                    #ifdef USE_STRONG_KING
                    Square ahead = SOUTH(square);
                    if( squares[ahead]=='k' && king_ending_bonus_dynamic_black[ahead]==0 )
                        bonus -= BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                next_passer_mask |= three_files;
                break;
            }
        }
        file_mask   >>= 1;
        three_files >>= 1;
    }
    passer_mask |= next_passer_mask;

    // a4->h4
    file_mask   = 0x80;             // 0 1000 0000
//  three_files = 0x1c0;            // 1 1100 0000
    for( square=a4; square<=h4; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    black_king_central_bonus = BONUS_BLACK_KING_CENTRAL2;
                break;
            }

            case 'n':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_BLACK_KNIGHT_CENTRAL2;
                break;
            }

            case 'q':
            {
                black_queen_central_bonus = BONUS_BLACK_QUEEN_CENTRAL;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    white_king_central_bonus = BONUS_WHITE_KING_CENTRAL1;
                break;
            }

            case 'N':
            {
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_KNIGHT_CENTRAL1;
                break;
            }

            case 'Q':
            {
                white_queen_central_bonus = BONUS_WHITE_QUEEN_CENTRAL;
                break;
            }

            case 'p':
            {
                *black_pawns++ = square;
                file = IFILE(square);
                if( file==3 || file==4 )
                    bonus += BONUS_BLACK_PAWN_CENTRAL;
                if( !(passer_mask&file_mask) )
                {
                    *black_passers++ = square;
                    bonus += BONUS_BLACK_PAWN5;
                    #ifdef USE_STRONG_KING
                    Square ahead = SOUTH(square);
                    if( squares[ahead]=='k' && king_ending_bonus_dynamic_black[ahead]==0 )
                        bonus -= BONUS_STRONG_KING;
                    #endif
                }
                break;
            }

            case 'P':
            {
                *white_pawns++   = square;
                file = IFILE(square);
                if( 2<=file && file<=5 )
                    bonus += BONUS_WHITE_PAWN_CENTRAL;
                break;
            }
        }
        file_mask   >>= 1;
        // three_files >>= 1;
    }

    // a1->h1
    for( square=a1; square<=h1; ++square )
    {
        piece = squares[square];
        score_black_material += black_material[ piece ];
        score_white_material += white_material[ piece ];
        score_black_pieces   += black_pieces[ piece ];
        score_white_pieces   += white_pieces[ piece ];
        switch( piece )
        {
            case 'k':
            {
                black_king_square = square;
                bonus -= king_ending_bonus_dynamic_black[square];
                break;
            }

            case 'R':
            {
                white_connected++;
                if( white_connected == 2 )
                    bonus += BONUS_WHITE_CONNECTED_ROOKS;
                break;
            }

            case 'N':
            {
                white_connected=2;
                white_undeveloped_minor_bonus++;
                break;
            }

            case 'B':
            {
                white_connected=2;
                white_undeveloped_minor_bonus++;
                if( a1==square && IsWhite(squares[b2]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else if( h1==square && IsWhite(squares[g2]) )
                    bonus += BONUS_WHITE_BLOCKED_BISHOP;
                else
                {
                    if( IsWhite(squares[NW(square)]) &&
                        IsWhite(squares[NE(square)])
                      )
                    {
                        bonus += BONUS_WHITE_BLOCKED_BISHOP;
                    }
                }
                break;
            }

            case 'Q':
            {
                white_connected=2;
                break;
            }

            case 'K':
            {
                white_king_square = square;
                bonus += king_ending_bonus_dynamic_white[square];
                white_connected=2;
                file = IFILE(square);
                if( file<2 || file>5 )
                    white_king_safety_bonus = BONUS_WHITE_KING_SAFETY;
                break;
            }

            case 'q':
            {
                black_queen78_bonus = BONUS_BLACK_QUEEN78;
                break;
            }
        }
    }

    if( score_white_material > MATERIAL_OPENING )
    {
        bonus += white_king_safety_bonus;
        bonus += white_queen_developed_bonus;
        bonus += white_undeveloped_minor_bonus*WHITE_UNDEVELOPED_MINOR_BONUS;
    }
    else if( score_white_material > MATERIAL_MIDDLE )
    {
        bonus += white_king_safety_bonus;
        bonus += white_queen_central_bonus;
    }
    else
    {
        // bonus += white_king_central_bonus;  
        bonus += white_queen78_bonus;
    }

    if( score_black_material < -MATERIAL_OPENING )
    {
        bonus += black_king_safety_bonus;
        bonus += black_queen_developed_bonus;
        bonus += black_undeveloped_minor_bonus*BLACK_UNDEVELOPED_MINOR_BONUS;
    }
    else if( score_black_material < -MATERIAL_MIDDLE )
    {
        bonus += black_king_safety_bonus;
        bonus += black_queen_central_bonus;
    }
    else
    {
        // bonus += black_king_central_bonus;    
        bonus += black_queen78_bonus;
    }

    material   = score_white_material + score_black_material;
    if( white )
    {
#ifdef CHECK_FOR_LEAF_MATE
        bool mate=false;
        if( AttackedPiece((Square)wking_square) )
 #ifdef CHECK_FOR_LEAF_MATE_USE_EVALUATE
        {
            TERMINAL terminal_score;
            bool okay = Evaluate(terminal_score);
            if( okay && terminal_score==TERMINAL_WCHECKMATE )   // white mated ?
                mate = true;
        }
 #else
        {

            mate = true;
            MOVELIST list;
            GenMoveList( &list );
            for( int i=0; mate && i<list.count; i++ )
            {
                PushMove( list.moves[i] );
                if( !AttackedPiece((Square)wking_square) )
                    mate = false;
                PopMove( list.moves[i] );
            }
        }
 #endif
        if( mate )
            material = -500;    // white is mated
        else
            material += EnpriseWhite();
#else
        material += EnpriseWhite();
#endif
    }
    else
    {
#ifdef CHECK_FOR_LEAF_MATE
        bool mate=false;
        if( AttackedPiece((Square)bking_square) )
 #ifdef CHECK_FOR_LEAF_MATE_USE_EVALUATE
        {
            TERMINAL terminal_score;
            bool okay = Evaluate(terminal_score);
            if( okay && terminal_score==TERMINAL_BCHECKMATE )   // black mated ?
                mate = true;
        }
 #else
        {
            mate = true;
            MOVELIST list;
            GenMoveList( &list );
            for( int i=0; mate && i<list.count; i++ )
            {
                PushMove( list.moves[i] );
                if( !AttackedPiece((Square)bking_square) )
                    mate = false;
                PopMove( list.moves[i] );
            }
        }
 #endif
        if( mate )
            material = 500;    // black is mated
        else
            material -= EnpriseBlack();
#else
        material -= EnpriseBlack();
#endif
    }
    positional = bonus;

    // Reward stronger side with a bonus for swapping pieces not pawns
    if( material>0 && planning_white_piece_pawn_percent ) // if white ahead
                                                          //   and a figure to compare to  
    {
        int score_white_pawns = score_white_material - 500 // -500 is king
                                - score_white_pieces;
        int piece_pawn_percent = 1000;
        if( score_white_pawns )
        {
            piece_pawn_percent = (100*score_white_pieces) /
                                      score_white_pawns;
            if( piece_pawn_percent > 1000 )
                piece_pawn_percent = 1000;
        }
        // start of game
        //  piece_pawn_percent = 100* Q+2R+2B+2N/8P = 100 * (190+120)/80
        //                     = 400 (approx)
        // typical endings
        //  piece_pawn_percent = 100* R+B/5P = 100 * (80)/50
        //                     = 160
        //  piece_pawn_percent = 100* R/5P = 100 * 50/50
        //                     = 100  after swapping a bishop
        //  piece_pawn_percent = 100* R+B/P = 100 * (80)/10
        //                     = 800
        //  piece_pawn_percent = 100* R/P = 100 * 50/10
        //                     = 500  after swapping a bishop
        //
        //  Lower numbers are better for the stronger side, calculate
        //  an adjustment as follows;
        //   up to +0.8 pawns for improved ratio for white as stronger side
        //   up to -0.8 pawns for worse ratio for white as stronger side
        int piece_pawn_ratio_adjustment = 8 - (8*piece_pawn_percent)/planning_white_piece_pawn_percent;
        if( piece_pawn_ratio_adjustment < -8 )
            piece_pawn_ratio_adjustment = -8;
        //   eg planning_white_piece_pawn_percent = 160
        //      now            piece_pawn_percent = 160
        //      adjustment = 0
        //   eg planning_white_piece_pawn_percent = 160
        //      now            piece_pawn_percent = 100
        //      adjustment = +3 (i.e. +0.3 pawns)
        //   eg planning_white_piece_pawn_percent = 800
        //      now            piece_pawn_percent = 500
        //      adjustment = +3 (i.e. +0.3 pawns)
        //   eg planning_white_piece_pawn_percent = 100
        //      now            piece_pawn_percent = 160
        //      adjustment = -4 (i.e. -0.4 pawns)
        //   eg planning_white_piece_pawn_percent = 500
        //      now            piece_pawn_percent = 800
        //      adjustment = -4 (i.e. -0.4 pawns)

        // If white is better, positive adjustment increases +ve material advantage
        material += piece_pawn_ratio_adjustment;
    }
    else if( material<0 && planning_black_piece_pawn_percent ) // if black ahead
                                                               //   and a figure to compare to  
    {
        int score_black_pawns = (0-score_black_material) - 500 // -500 is king
                                - score_black_pieces;
        int piece_pawn_percent = 1000;
        if( score_black_pawns )
        {
            piece_pawn_percent = (100*score_black_pieces) /
                                      score_black_pawns;
            if( piece_pawn_percent > 1000 )
                piece_pawn_percent = 1000;
        }
        int piece_pawn_ratio_adjustment = 8 - (8*piece_pawn_percent)/planning_black_piece_pawn_percent;
        if( piece_pawn_ratio_adjustment < -8 )
            piece_pawn_ratio_adjustment = -8;

        // If black is better, positive adjustment increases -ve material advantage
        material -= piece_pawn_ratio_adjustment;
    }

    // Check whether white king is in square of black passers
    if( score_white_pieces==0 )
    {
        bool black_will_queen = false;
        #ifdef USE_IN_THE_SQUARE
        while( black_passers>black_passers_buf
                && white_king_square != SQUARE_INVALID
                && !black_will_queen )
        {
            --black_passers;
            Square square = *black_passers;
            int pfile = IFILE(square);
            int prank = IRANK(square);
            int kfile = IFILE(white_king_square);
            int krank = IRANK(white_king_square);

            // Calculations assume it is black to move; so if it is white
            //  shift pawn one more square away from queening square
            if( white )
                prank++;

            // Will queen if eg Pa3, Ka4
            if( prank < krank )
                black_will_queen = true;
            else
            {

                // Will queen if eg Pa3, Kd3
                if( kfile > pfile )
                    black_will_queen = (kfile-pfile > prank); // eg d-a=3 > 2 

                // Will queen if eg Ph3, Ke3
                else if( kfile < pfile )
                    black_will_queen = (pfile-kfile > prank); // eg h-e=3 > 2
            }
        }

        // If white has a bare king, a pawn protecting a pawn always wins and a pair of pawns
        //  separated by 5 or 6 files always wins (even if white to move captures one)
        //  Note at the planning stage white had material - so are encouraging liquidation to
        //  easily winning pawn endings
        #ifdef USE_LIQUIDATION
        while( score_white_material==500 && planning_score_white_pieces && black_pawns>black_pawns_buf
                 && !black_will_queen )
        {
            int nbr_separating_files = (white?5:4);
            --black_pawns;
            Square square = *black_pawns;
            int pfile1 = IFILE(square);
            int prank1 = IRANK(square);
            Square *p = black_pawns;
            while( p > black_pawns_buf )
            {
                p--;
                Square square = *p;
                int pfile2 = IFILE(square);
                int prank2 = IRANK(square);
                if( (prank2==prank1+1 || prank2+1==prank1) &&
                    (pfile2==pfile1+1 || pfile2+1==pfile1)
                  )
                {
                    black_will_queen = true;  // pawn protects pawn
                }
                else if( pfile2>pfile1+nbr_separating_files || pfile1>pfile2+nbr_separating_files )
                {
                    black_will_queen = true;  // pawns separated by 5 or more empty files
                }
            }
        }
        #endif
        if( black_will_queen )
            material -= 65; // almost as good as a black queen (if it's too close we might not actually queen!)
    }

    // Check whether black king is in square of white passers
    if( score_black_pieces==0 )
    {
        bool white_will_queen = false;
        #ifdef USE_IN_THE_SQUARE
        while( white_passers>white_passers_buf
                && black_king_square != SQUARE_INVALID
                && !white_will_queen )
        {
            --white_passers;
            Square square = *white_passers;
            int pfile = IFILE(square);
            int prank = IRANK(square);
            int kfile = IFILE(black_king_square);
            int krank = IRANK(black_king_square);

            // Calculations assume it is white to move; so if it is black
            //  shift pawn one more square away from queening square
            if( !white )
                prank--;

            // Will queen if eg Pa6, Ka5
            if( prank > krank )
                white_will_queen = true;
            else
            {

                // Will queen if eg Pa6, Kd6
                if( kfile > pfile )
                    white_will_queen = (kfile-pfile > 7-prank); // eg d-a=3 > 7-5=2 

                // Will queen if eg Ph3, Ke3
                else if( kfile < pfile )
                    white_will_queen = (pfile-kfile > 7-prank); // eg h-e=3 > 7-5=2
            }
        }
        #endif

        // If black has a bare king, a pawn protecting a pawn always wins and a pair of pawns
        //  separated by 5 or 6 files always wins (even if black to move captures one)
        //  Note at the planning stage black had material - so are encouraging liquidation to
        //  easily winning pawn endings
        #ifdef USE_LIQUIDATION
        while( score_black_material==-500 && planning_score_black_pieces && white_pawns>white_pawns_buf
                 && !white_will_queen )
        {
            int nbr_separating_files = (!white?5:4);
            --white_pawns;
            Square square = *white_pawns;
            int pfile1 = IFILE(square);
            int prank1 = IRANK(square);
            Square *p = white_pawns;
            while( p > white_pawns_buf )
            {
                p--;
                Square square = *p;
                int pfile2 = IFILE(square);
                int prank2 = IRANK(square);
                if( (prank2==prank1+1 || prank2+1==prank1) &&
                    (pfile2==pfile1+1 || pfile2+1==pfile1)
                  )
                {
                    white_will_queen = true;  // pawn protects pawn
                }
                else if( pfile2>pfile1+nbr_separating_files || pfile1>pfile2+nbr_separating_files )
                {
                    white_will_queen = true;  // pawns separated by 5 or more empty files
                }
            }
        }
        #endif
        if( white_will_queen )
            material += 65; // almost as good as a white queen (if it's too close we might not actually queen!)
    }
    #endif
//	int score_test = material*4 /*balance=4*/ + positional;
//  int score_test_cp = (score_test*10)/4;
//	if( score_test_cp > 4000 )
//      printf( "too much" );   // Problem fen "k7/8/PPK5/8/8/8/8/8 w - - 0 1"
}


/****************************************************************************
 * Create a list of all legal moves (sorted strongest first, public version)
 ****************************************************************************/
void ChessEvaluation::GenLegalMoveListSorted( vector<Move> &moves )
{
    MOVELIST movelist;
    GenLegalMoveListSorted( &movelist );
    for( int i=0; i<movelist.count; i++ )
        moves.push_back( movelist.moves[i] );
}

/****************************************************************************
 * Create a list of all legal moves (sorted strongest first)
 ****************************************************************************/
// Sort moves according to their score
struct MOVE_IDX
{
    int score;
    int idx;
    bool operator <  (const MOVE_IDX& arg) const { return score <  arg.score; }
    bool operator >  (const MOVE_IDX& arg) const { return score >  arg.score; }
    bool operator == (const MOVE_IDX& arg) const { return score == arg.score; }
};
void ChessEvaluation::GenLegalMoveListSorted( MOVELIST *list )
{
    int i, j;
    bool okay;
    TERMINAL terminal_score;
    MOVELIST list2;
    vector<MOVE_IDX> sortable;

    // Call this before calls to EvaluateLeaf()
    Planning();

    // Generate all moves, including illegal (eg put king in check) moves
    GenMoveList( &list2 );

    // Loop copying the proven good ones
    for( i=j=0; i<list2.count; i++ )
    {
        PushMove( list2.moves[i] );
        okay = Evaluate(terminal_score);
        if( okay )
        {
            int score=0;
            if( terminal_score == TERMINAL_WCHECKMATE )  // White is checkmated
                score = -1000000;
            else if( terminal_score == TERMINAL_BCHECKMATE )  // Black is checkmated
                score = 1000000;
            else if( terminal_score==TERMINAL_WSTALEMATE ||
                     terminal_score==TERMINAL_BSTALEMATE )
                score = 0;
            else
            {
			    int material, positional;
			    EvaluateLeaf(material,positional);
			    score = material*4 /*balance=4*/ + positional;
            }
            MOVE_IDX x;
            x.score = score;
            x.idx   = i;
            sortable.push_back(x);
        }
        PopMove( list2.moves[i] );
    }
    if( white )
        sort( sortable.rbegin(), sortable.rend() );
    else
        sort( sortable.begin(), sortable.end() );
    for( i=0; i<(int)sortable.size(); i++ )
    {
        MOVE_IDX x = sortable[i];
        list->moves[i] = list2.moves[x.idx];
    }
    list->count  = i;
}

