/****************************************************************************
 * Test position for pattern
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "thc.h"
#include "DebugPrintf.h"
#include "PatternMatch.h"

// Constructor
PatternMatch::PatternMatch()
{
}

// Prepare for series of calls to Test()
void PatternMatch::PrimePattern( const thc::ChessPosition *rover )
{

    static int reflect[] =
    {
        7,6,5,4,3,2,1,0,
        15,14,13,12,11,10,9,8,
        23,22,21,20,19,18,17,16,
        31,30,29,28,27,26,25,24,
        39,38,37,36,35,34,33,32,
        47,46,45,44,43,42,41,40,
        55,54,53,52,51,50,49,48,
        63,62,61,60,59,58,57,56
    };

    static int reverse[] =
    {
        56,57,58,59,60,61,62,63,
        48,49,50,51,52,53,54,55,
        40,41,42,43,44,45,46,47,
        32,33,34,35,36,37,38,39,
        24,25,26,27,28,29,30,31,
        16,17,18,19,20,21,22,23,
        8,9,10,11,12,13,14,15,
        0,1,2,3,4,5,6,7
    };

    for( int i=0; i<64; i++ )
    {
        char c = search_criteria.cp.squares[i];
        if( c=='B' )
            c = is_dark(i) ? 'D' : 'B';
        else if( c=='b' )
            c = is_dark(i) ? 'd' : 'b';
        pmm_n.cp.squares[i] = c;
    }
    std::string s = pmm_n.cp.ToDebugStr();
    cprintf( "Normal %s\n", s.c_str() );
    for( int i=0; i<64; i++ )
    {
        char c = search_criteria.cp.squares[i];
        int j = reflect[i];
        if( c=='B' )
            c = is_dark(j) ? 'D' : 'B';
        else if( c=='b' )
            c = is_dark(j) ? 'd' : 'b';
        pmm_m.cp.squares[j] = c;
    }
    s = pmm_m.cp.ToDebugStr();
    cprintf( "Mirror %s\n", s.c_str() );
    for( int i=0; i<64; i++ )
    {
        char c = search_criteria.cp.squares[i];
        if( isalpha(c) && isupper(c) )
            c = tolower(c);
        else if( isalpha(c) && islower(c) )
            c = toupper(c);
        int j = reverse[i];
        if( c=='B' )
            c = is_dark(j) ? 'D' : 'B';
        else if( c=='b' )
            c = is_dark(j) ? 'd' : 'b';
        pmm_r.cp.squares[j] = c;
    }
    s = pmm_r.cp.ToDebugStr();
    cprintf( "Colours reversed %s\n", s.c_str() );
    for( int i=0; i<64; i++ )
    {
        char c = pmm_r.cp.squares[i];
        int j = reflect[i];
        if( c=='B' || c=='D' )
            c = is_dark(j) ? 'D' : 'B';
        else if( c=='b' || c=='d' )
            c = is_dark(j) ? 'd' : 'b';
        pmm_rm.cp.squares[j] = c;
    }
    s = pmm_rm.cp.ToDebugStr();
    cprintf( "Colours reversed and mirror %s\n", s.c_str() );

    // Set up rank pointers to the position to be tested
    rank8_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[0]);
    rank7_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[8]);
    rank6_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[16]);
    rank5_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[24]);
    rank4_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[32]);
    rank3_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[40]);
    rank2_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[48]);
    rank1_ptr = reinterpret_cast<const uint64_t *>(&rover->squares[56]);

    for( int i=0; i<4; i++ )
    {
        PatternMatchMask *pmm;
        switch(i)
        {
            case 0: pmm = &pmm_n;   break;
            case 1: pmm = &pmm_m;   break;
            case 2: pmm = &pmm_r;   break;
            case 3: pmm = &pmm_rm;  break;
        }

        // Set up rank pointers to the target position
        pmm->rank8_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[0]);
        pmm->rank7_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[8]);
        pmm->rank6_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[16]);
        pmm->rank5_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[24]);
        pmm->rank4_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[32]);
        pmm->rank3_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[40]);
        pmm->rank2_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[48]);
        pmm->rank1_target_ptr = reinterpret_cast<const uint64_t *>(&pmm->cp.squares[56]);

        // Set up the pattern mask
        for( int j=0, idx=0; j<8; j++ )
        {
            char *mask;
            switch( j )
            {
                case 0: mask = reinterpret_cast<char *>(&pmm->rank8_mask);  break;
                case 1: mask = reinterpret_cast<char *>(&pmm->rank7_mask);  break;
                case 2: mask = reinterpret_cast<char *>(&pmm->rank6_mask);  break;
                case 3: mask = reinterpret_cast<char *>(&pmm->rank5_mask);  break;
                case 4: mask = reinterpret_cast<char *>(&pmm->rank4_mask);  break;
                case 5: mask = reinterpret_cast<char *>(&pmm->rank3_mask);  break;
                case 6: mask = reinterpret_cast<char *>(&pmm->rank2_mask);  break;
                case 7: mask = reinterpret_cast<char *>(&pmm->rank1_mask);  break;
            }
            for( int k=0; k<8; k++ )
            {
                mask[k] = 0;
                char c = pmm->cp.squares[idx];
                bool empty = (c =='.' || c==' ');
                if( empty )
                    pmm->cp.squares[idx] = 0;
                idx++;
                if( !empty )
                    mask[k] = '\x7f';
            }
        }
    }
}

// Test against criteria
bool PatternMatch::TestPattern()
{
    int max;
    if( search_criteria.include_reflections )
        max = search_criteria.include_reverse_colours ? 4 : 2;
    else
        max = search_criteria.include_reverse_colours ? 3 : 1;
    for( int i=0; i<max; i++ )
    {
        PatternMatchMask *pmm;
        switch(i)
        {
            case 0: pmm = &pmm_n;   break;
            case 1: pmm = &pmm_m;   break;
            case 2: pmm = &pmm_r;   break;
            case 3: pmm = &pmm_rm;  break;
        }
        if( max==3 && i==1 )    // max==3 means do i==0 and i==2
            continue;
        bool match = 
        //(msi.cr.white == target_white) && 
        (*rank3_ptr & pmm->rank3_mask) == *pmm->rank3_target_ptr &&
        (*rank4_ptr & pmm->rank4_mask) == *pmm->rank4_target_ptr &&
        (*rank5_ptr & pmm->rank5_mask) == *pmm->rank5_target_ptr &&
        (*rank6_ptr & pmm->rank6_mask) == *pmm->rank6_target_ptr &&
        (*rank7_ptr & pmm->rank7_mask) == *pmm->rank7_target_ptr &&
        (*rank8_ptr & pmm->rank8_mask) == *pmm->rank8_target_ptr &&
        (*rank1_ptr & pmm->rank1_mask) == *pmm->rank1_target_ptr &&
        (*rank2_ptr & pmm->rank2_mask) == *pmm->rank2_target_ptr;
        if( match )
            return true;
    }
    return false;    
}


// Pawns for each side are assigned logical numbers from 0 to nbr_pawns-1
//  The ordering of the numbers is determined by consulting this table...
static int pawn_ordering[64] =
{
    7, 15, 23, 31, 39, 47, 55, 63,
    6, 14, 22, 30, 38, 46, 54, 62,
    5, 13, 21, 29, 37, 45, 53, 61,
    4, 12, 20, 28, 36, 44, 52, 60,
    3, 11, 19, 27, 35, 43, 51, 59,
    2, 10, 18, 26, 34, 42, 50, 58,
    1,  9, 17, 25, 33, 41, 49, 57,
    0,  8, 16, 24, 32, 40, 48, 56
};

// ...to initially assign logical pawn numbers according to pawn_ordering[]
//  above, traverse the squares in this order and assign 0,1,2... etc as each
//  pawn is found
static thc::Square traverse_order[64] =
{
    thc::a1, thc::a2, thc::a3, thc::a4, thc::a5, thc::a6, thc::a7, thc::a8,
    thc::b1, thc::b2, thc::b3, thc::b4, thc::b5, thc::b6, thc::b7, thc::b8,
    thc::c1, thc::c2, thc::c3, thc::c4, thc::c5, thc::c6, thc::c7, thc::c8,
    thc::d1, thc::d2, thc::d3, thc::d4, thc::d5, thc::d6, thc::d7, thc::d8,
    thc::e1, thc::e2, thc::e3, thc::e4, thc::e5, thc::e6, thc::e7, thc::e8,
    thc::f1, thc::f2, thc::f3, thc::f4, thc::f5, thc::f6, thc::f7, thc::f8,
    thc::g1, thc::g2, thc::g3, thc::g4, thc::g5, thc::g6, thc::g7, thc::g8,
    thc::h1, thc::h2, thc::h3, thc::h4, thc::h5, thc::h6, thc::h7, thc::h8
};


void PatternMatch::PrimeMaterialBalance()
{
    memset(&white,0,sizeof(white));
    memset(&black,0,sizeof(black));
    for( int i=0; i<64; i++ )
    {
        thc::Square sq = traverse_order[i];
        char c = search_criteria.cp.squares[sq];
        switch(c)
        {
            case 'P':   if(white.nbr_pawns<8)
                            white.pawns[ white.nbr_pawns++ ] = (int)sq;
                        break;
            case 'R':   white.nbr_rooks++;      break;
            case 'N':   white.nbr_knights++;    break;
            case 'B':   if( !is_dark((int)sq) )
                            white.nbr_light_bishops++;
                        else
                            white.nbr_dark_bishops++;    break;
            case 'Q':   white.nbr_queens++;     break;
            case 'p':   if(black.nbr_pawns<8)
                            black.pawns[ black.nbr_pawns++ ] = (int)sq;
                        break;
            case 'r':   black.nbr_rooks++;      break;
            case 'n':   black.nbr_knights++;    break;
            case 'b':   if( !is_dark((int)sq) )
                            black.nbr_light_bishops++;
                        else
                            black.nbr_dark_bishops++;    break;
            case 'q':   black.nbr_queens++;     break;
        }
    }    
}

bool PatternMatch::TestMaterialBalance( MpsSide *ws, MpsSide *bs )
{
    bool match =
    ( 
        ws->nbr_pawns   == white.nbr_pawns  &&
        ws->nbr_rooks   == white.nbr_rooks  &&
        ws->nbr_knights == white.nbr_knights &&
        ws->nbr_light_bishops == white.nbr_light_bishops &&
        ws->nbr_dark_bishops == white.nbr_dark_bishops &&
        ws->nbr_queens  == white.nbr_queens  &&
        bs->nbr_pawns   == black.nbr_pawns  &&
        bs->nbr_rooks   == black.nbr_rooks  &&
        bs->nbr_knights == black.nbr_knights &&
        bs->nbr_light_bishops == black.nbr_light_bishops &&
        bs->nbr_dark_bishops == black.nbr_dark_bishops &&
        bs->nbr_queens  == black.nbr_queens
    );
    if( match && search_criteria.pawns_must_be_on_same_files )
    {
        for( int i=0; i<white.nbr_pawns; i++ )
        {
            if( (ws->pawns[i]&7) != (white.pawns[i]&7) )
                match = false;
        }
        for( int i=0; i<black.nbr_pawns; i++ )
        {
            if( (bs->pawns[i]&7) != (black.pawns[i]&7) )
                match = false;
        }
    }
    if( !match && search_criteria.include_reverse_colours )
    {
        match =
        ( 
            bs->nbr_pawns   == white.nbr_pawns  &&
            bs->nbr_rooks   == white.nbr_rooks  &&
            bs->nbr_knights == white.nbr_knights &&
            bs->nbr_light_bishops == white.nbr_light_bishops &&
            bs->nbr_dark_bishops == white.nbr_dark_bishops &&
            bs->nbr_queens  == white.nbr_queens  &&
            ws->nbr_pawns   == black.nbr_pawns  &&
            ws->nbr_rooks   == black.nbr_rooks  &&
            ws->nbr_knights == black.nbr_knights &&
            ws->nbr_light_bishops == black.nbr_light_bishops &&
            ws->nbr_dark_bishops == black.nbr_dark_bishops &&
            ws->nbr_queens  == black.nbr_queens
        );
        if( match && search_criteria.pawns_must_be_on_same_files )
        {
            for( int i=0; i<white.nbr_pawns; i++ )
            {
                if( (bs->pawns[i]&7) != (white.pawns[white.nbr_pawns-i-1]&7) )
                    return false;
            }
            for( int i=0; i<black.nbr_pawns; i++ )
            {
                if( (ws->pawns[i]&7) != (black.pawns[black.nbr_pawns-i-1]&7) )
                    return false;
            }
        }
    }
    return match;
}