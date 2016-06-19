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
    in_a_row = 0;

    // Calculate number of positions to test each time (note that 3 is actually
    //  used to indicate 2 tests - 
    //     reflect and not reverse = 2
    //     reverse and not reflect = 3 (but also represents 2 tests)
    if( parm.include_reflections )
        reflect_and_reverse = parm.include_reverse_colours ? 4 : 2;
    else
        reflect_and_reverse = parm.include_reverse_colours ? 3 : 1;

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

    PatternParameters parm_r;
    target_n.parm = parm;
    target_m.parm = parm;
    parm_r = parm;
    parm_r.white_to_move  = !parm.white_to_move;
    parm_r.more_pieces_wq = parm.more_pieces_bq;
    parm_r.more_pieces_wr = parm.more_pieces_br;
    parm_r.more_pieces_wb = parm.more_pieces_bb;
    parm_r.more_pieces_wn = parm.more_pieces_bn;
    parm_r.more_pieces_wp = parm.more_pieces_bp;
    parm_r.more_pieces_bq = parm.more_pieces_wq;
    parm_r.more_pieces_br = parm.more_pieces_wr;
    parm_r.more_pieces_bb = parm.more_pieces_wb;
    parm_r.more_pieces_bn = parm.more_pieces_wn;
    parm_r.more_pieces_bp = parm.more_pieces_wp;
    parm_r.lockdown_wk    = parm.lockdown_bk;
    parm_r.lockdown_wq    = parm.lockdown_bq;
    parm_r.lockdown_wr    = parm.lockdown_br;
    parm_r.lockdown_wb    = parm.lockdown_bb;
    parm_r.lockdown_wn    = parm.lockdown_bn;
    parm_r.lockdown_wp    = parm.lockdown_bp;
    parm_r.lockdown_bk    = parm.lockdown_wk;
    parm_r.lockdown_bq    = parm.lockdown_wq;
    parm_r.lockdown_br    = parm.lockdown_wr;
    parm_r.lockdown_bb    = parm.lockdown_wb;
    parm_r.lockdown_bn    = parm.lockdown_wn;
    parm_r.lockdown_bp    = parm.lockdown_wp;
    target_r.parm  = parm_r;
    target_rm.parm = parm_r;

    // Normal = _n
    target_n.cp.white = parm.cp.white;
    for( int i=0; i<64; i++ )
    {
        char c = parm.cp.squares[i];
        if( c=='B' )
            c = is_dark(i) ? 'D' : 'B';
        else if( c=='b' )
            c = is_dark(i) ? 'd' : 'b';
        target_n.cp.squares[i] = c;
    }
    std::string s = target_n.cp.ToDebugStr();
    cprintf( "Normal %s\n", s.c_str() );

    // Mirror = _m
    target_m.cp.white = parm.cp.white;
    for( int i=0; i<64; i++ )
    {
        char c = parm.cp.squares[i];
        int j = reflect[i];
        if( c=='B' )
            c = is_dark(j) ? 'D' : 'B';
        else if( c=='b' )
            c = is_dark(j) ? 'd' : 'b';
        target_m.cp.squares[j] = c;
    }
    s = target_m.cp.ToDebugStr();
    cprintf( "Mirror %s\n", s.c_str() );

    // Reverse = _r
    target_r.cp.white = !parm.cp.white;
    for( int i=0; i<64; i++ )
    {
        char c = parm.cp.squares[i];
        if( isalpha(c) && isupper(c) )
            c = tolower(c);
        else if( isalpha(c) && islower(c) )
            c = toupper(c);
        int j = reverse[i];
        if( c=='B' )
            c = is_dark(j) ? 'D' : 'B';
        else if( c=='b' )
            c = is_dark(j) ? 'd' : 'b';
        target_r.cp.squares[j] = c;
    }
    s = target_r.cp.ToDebugStr();
    cprintf( "Colours reversed %s\n", s.c_str() );

    // Mirrored and reversed = _mr
    target_rm.cp.white = !parm.cp.white;
    for( int i=0; i<64; i++ )
    {
        char c = target_r.cp.squares[i];
        int j = reflect[i];
        if( c=='B' || c=='D' )
            c = is_dark(j) ? 'D' : 'B';
        else if( c=='b' || c=='d' )
            c = is_dark(j) ? 'd' : 'b';
        target_rm.cp.squares[j] = c;
    }
    s = target_rm.cp.ToDebugStr();
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
        PatternMatchTarget *target;
        switch(i)
        {
            case 0: target = &target_n;   break;
            case 1: target = &target_m;   break;
            case 2: target = &target_r;   break;
            case 3: target = &target_rm;  break;
        }

        // Initialise material balance structures for both sides
        InitSide( &target->side_w, true, target->cp.squares );
        InitSide( &target->side_b, false, target->cp.squares );

        // Set up rank pointers to the target position
        target->rank8_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[0]);
        target->rank7_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[8]);
        target->rank6_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[16]);
        target->rank5_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[24]);
        target->rank4_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[32]);
        target->rank3_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[40]);
        target->rank2_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[48]);
        target->rank1_target_ptr = reinterpret_cast<const uint64_t *>(&target->cp.squares[56]);

        // Set up the pattern mask
        for( int j=0, idx=0; j<8; j++ )
        {
            char *mask;
            switch( j )
            {
                case 0: mask = reinterpret_cast<char *>(&target->rank8_mask);  break;
                case 1: mask = reinterpret_cast<char *>(&target->rank7_mask);  break;
                case 2: mask = reinterpret_cast<char *>(&target->rank6_mask);  break;
                case 3: mask = reinterpret_cast<char *>(&target->rank5_mask);  break;
                case 4: mask = reinterpret_cast<char *>(&target->rank4_mask);  break;
                case 5: mask = reinterpret_cast<char *>(&target->rank3_mask);  break;
                case 6: mask = reinterpret_cast<char *>(&target->rank2_mask);  break;
                case 7: mask = reinterpret_cast<char *>(&target->rank1_mask);  break;
            }
            for( int k=0; k<8; k++ )
            {
                if( target->parm.material_balance )
                {
                    mask[k] = 0;
                    char c = target->cp.squares[idx];
                    switch( c )
                    {
                        default:  c = '\0';                                 break;
                        case 'K': c = target->parm.lockdown_wk ? c : '\0';  break;
                        case 'Q': c = target->parm.lockdown_wq ? c : '\0';  break;
                        case 'R': c = target->parm.lockdown_wr ? c : '\0';  break;
                        case 'D':
                        case 'B': c = target->parm.lockdown_wb ? c : '\0';  break;
                        case 'N': c = target->parm.lockdown_wn ? c : '\0';  break;
                        case 'P': c = target->parm.lockdown_wp ? c : '\0';  break;
                        case 'k': c = target->parm.lockdown_bk ? c : '\0';  break;
                        case 'q': c = target->parm.lockdown_bq ? c : '\0';  break;
                        case 'r': c = target->parm.lockdown_br ? c : '\0';  break;
                        case 'd':
                        case 'b': c = target->parm.lockdown_bb ? c : '\0';  break;
                        case 'n': c = target->parm.lockdown_bn ? c : '\0';  break;
                        case 'p': c = target->parm.lockdown_bp ? c : '\0';  break;
                    }
                    target->cp.squares[idx] = c;
                    idx++;
                    if( c != '\0' )
                        mask[k] = '\x7f';
                }
                else
                {
                    if( !target->parm.allow_more_pieces )
                    {
                        mask[k] = '\x7f';   // so effectively (*rank & *mask) == *target
                                            //  will mean *rank == *target i.e. rover==target
                                            //  with no extra pieces allowed
                    }
                    else
                    {
                        mask[k] = 0;
                        char c = target->cp.squares[idx];
                        bool empty = (c =='.' || c==' ');
                        if( empty )
                            target->cp.squares[idx] = 0;
                        idx++;
                        if( !empty )
                            mask[k] = '\x7f';
                    }
                }
            }
        }
    }
}

// Test against criteria
bool PatternMatch::TestPattern( bool white, const char *squares_rover )
{
    bool match=false;
    for( int i=0; !match && i<reflect_and_reverse; i++ )
    {
        PatternMatchTarget *target;
        switch(i)
        {
            case 0: target = &target_n;   break;
            case 1: target = &target_m;   break;
            case 2: target = &target_r;   break;
            case 3: target = &target_rm;  break;
        }
        if( reflect_and_reverse==3 && i==1 )    // reflect_and_reverse==3 means do i==0 and i==2
            continue;
        bool match = target->parm.either_to_move || (white==target->parm.white_to_move);
        if( match )
        {
            match = 
            (*reinterpret_cast<const uint64_t *>(&squares_rover[40]) & target->rank3_mask) == *target->rank3_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[32]) & target->rank4_mask) == *target->rank4_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[24]) & target->rank5_mask) == *target->rank5_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[16]) & target->rank6_mask) == *target->rank6_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[8] ) & target->rank7_mask) == *target->rank7_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[0] ) & target->rank8_mask) == *target->rank8_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[56]) & target->rank1_mask) == *target->rank1_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[48]) & target->rank2_mask) == *target->rank2_target_ptr;
        }
    }
    return match;    
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
}

void PatternMatch::InitSide( MpsSide *side, bool white, const char *squares_rover )
{
    memset(side,0,sizeof(*side));
    side->white = white;
    if( white )
    {
        for( int i=0; i<64; i++ )
        {
            thc::Square sq = traverse_order[i];
            char c = squares_rover[sq];
            switch(c)
            {
                case 'P':   if(side->nbr_pawns<8)
                                side->pawns[ side->nbr_pawns++ ] = (int)sq;
                            break;
                case 'R':   side->nbr_rooks++;              break;
                case 'N':   side->nbr_knights++;            break;
                case 'D':   side->nbr_dark_bishops++;       break;
                case 'B':   if( !is_dark((int)sq) )
                                side->nbr_light_bishops++;
                            else
                                side->nbr_dark_bishops++;   break;
                case 'Q':   side->nbr_queens++;             break;
            }
        }    
    }
    else
    {
        for( int i=0; i<64; i++ )
        {
            thc::Square sq = traverse_order[i];
            char c = squares_rover[sq];
            switch(c)
            {
                case 'p':   if(side->nbr_pawns<8)
                                side->pawns[ side->nbr_pawns++ ] = (int)sq;
                            break;
                case 'r':   side->nbr_rooks++;              break;
                case 'n':   side->nbr_knights++;            break;
                case 'd':   side->nbr_dark_bishops++;       break;
                case 'b':   if( !is_dark((int)sq) )
                                side->nbr_light_bishops++;
                            else
                                side->nbr_dark_bishops++;   break;
                case 'q':   side->nbr_queens++;             break;
            }
        }    
    }
}

bool PatternMatch::TestMaterialBalance( MpsSide *ws, MpsSide *bs, const char *squares_rover, bool may_need_to_rebuild_side )
{
    bool match = TestMaterialBalanceInner(ws,bs,squares_rover,may_need_to_rebuild_side);
    if( !match )
        in_a_row = 0;
    else
    {
        in_a_row++;
        if( in_a_row < parm.number_of_ply )
            match = false;
    }
    return match;
}

bool PatternMatch::TestMaterialBalanceInner( MpsSide *ws, MpsSide *bs, const char *squares_rover, bool may_need_to_rebuild_side )
{
    bool match=false;
    if( may_need_to_rebuild_side && !ws->fast_mode  )
        InitSide( ws, true, squares_rover );
    if( may_need_to_rebuild_side && !bs->fast_mode  )
        InitSide( bs, false, squares_rover );
    for( int test=0; !match && test<reflect_and_reverse; test++ )
    {
        PatternMatchTarget *target;
        switch(test)
        {
            case 0: target = &target_n;   break;
            case 1: target = &target_m;   break;
            case 2: target = &target_r;   break;
            case 3: target = &target_rm;  break;
        }
        if( reflect_and_reverse==3 && test==1 )    // reflect_and_reverse==3 means do i==0 and i==2
            continue;
        if( !parm.lockdown_any )
            match = true;
        else
        {
            match = 
            (*reinterpret_cast<const uint64_t *>(&squares_rover[40]) & target->rank3_mask) == *target->rank3_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[32]) & target->rank4_mask) == *target->rank4_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[24]) & target->rank5_mask) == *target->rank5_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[16]) & target->rank6_mask) == *target->rank6_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[8] ) & target->rank7_mask) == *target->rank7_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[0] ) & target->rank8_mask) == *target->rank8_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[56]) & target->rank1_mask) == *target->rank1_target_ptr &&
            (*reinterpret_cast<const uint64_t *>(&squares_rover[48]) & target->rank2_mask) == *target->rank2_target_ptr;
        }
        if( match )
        {
            match =
            ( 
                (target->parm.more_pieces_wp ?
                ws->nbr_pawns   >= target->side_w.nbr_pawns  :
                ws->nbr_pawns   == target->side_w.nbr_pawns)   &&
                (target->parm.more_pieces_wr ?
                ws->nbr_rooks   >= target->side_w.nbr_rooks  :
                ws->nbr_rooks   == target->side_w.nbr_rooks)   &&
                (target->parm.more_pieces_wn ?
                ws->nbr_knights >= target->side_w.nbr_knights :
                ws->nbr_knights == target->side_w.nbr_knights) &&
                (target->parm.more_pieces_wq ?
                ws->nbr_queens  >= target->side_w.nbr_queens  :
                ws->nbr_queens  == target->side_w.nbr_queens)  &&
                (target->parm.more_pieces_bp ?
                bs->nbr_pawns   >= target->side_b.nbr_pawns  :
                bs->nbr_pawns   == target->side_b.nbr_pawns)   &&
                (target->parm.more_pieces_br ?
                bs->nbr_rooks   >= target->side_b.nbr_rooks  :
                bs->nbr_rooks   == target->side_b.nbr_rooks)   &&
                (target->parm.more_pieces_bn ?
                bs->nbr_knights >= target->side_b.nbr_knights  :
                bs->nbr_knights == target->side_b.nbr_knights) &&
                (target->parm.more_pieces_bq ?
                bs->nbr_queens  >= target->side_b.nbr_queens  :
                bs->nbr_queens  == target->side_b.nbr_queens)
            );
        }
        if( match )
        {
            if( target->parm.bishops_must_be_same_colour )
            {
                match =
                (
                    (target->parm.more_pieces_wb ?
                    ws->nbr_light_bishops >= target->side_w.nbr_light_bishops :
                    ws->nbr_light_bishops == target->side_w.nbr_light_bishops) &&
                    (target->parm.more_pieces_wb ?
                    ws->nbr_dark_bishops  >= target->side_w.nbr_dark_bishops :
                    ws->nbr_dark_bishops  == target->side_w.nbr_dark_bishops)  &&
                    (target->parm.more_pieces_bb ?
                    bs->nbr_light_bishops >= target->side_b.nbr_light_bishops :
                    bs->nbr_light_bishops == target->side_b.nbr_light_bishops) &&
                    (target->parm.more_pieces_bb ?
                    bs->nbr_dark_bishops  >= target->side_b.nbr_dark_bishops :
                    bs->nbr_dark_bishops  == target->side_b.nbr_dark_bishops)
                );
            }
            else
            {
                match =
                ( 
                    (target->parm.more_pieces_wb ?
                        (ws->nbr_light_bishops +
                            ws->nbr_dark_bishops)
                                                >=
                                            (target->side_w.nbr_light_bishops +
                                                target->side_w.nbr_dark_bishops)  :
                        (ws->nbr_light_bishops +
                            ws->nbr_dark_bishops)
                                                ==
                                            (target->side_w.nbr_light_bishops +
                                                target->side_w.nbr_dark_bishops)
                    )                                                             &&
                    (target->parm.more_pieces_bb ?
                        (bs->nbr_light_bishops +
                            bs->nbr_dark_bishops)
                                                >=
                                            (target->side_b.nbr_light_bishops +
                                                target->side_b.nbr_dark_bishops)  :
                        (bs->nbr_light_bishops +
                            bs->nbr_dark_bishops)
                                                ==
                                            (target->side_b.nbr_light_bishops +
                                                target->side_b.nbr_dark_bishops)
                    )
                );
            }
        }

        if( match && target->parm.pawns_must_be_on_same_files )
        {
            if( !target->parm.more_pieces_wp )
            {
                // Each rover pawn must be on same file as each target pawn
                for( int i=0; i<target->side_w.nbr_pawns; i++ )
                {
                    if( (ws->pawns[i]&7) != (target->side_w.pawns[i]&7) )
                        match = false;
                }
            }
            else
            {
                // Must find a rover pawn on same file as each and every target pawn
                for( int i=0; match && i<target->side_w.nbr_pawns; i++ )
                {
                    match = false;
                    for( int k=0; !match && k<ws->nbr_pawns; k++ )
                    {
                        if( (ws->pawns[k]&7) == (target->side_w.pawns[i]&7) )
                            match = true;
                    }
                }
            }
            if( match && !target->parm.more_pieces_bp )
            {
                // Each rover pawn must be on same file as each target pawn
                for( int i=0; i<target->side_b.nbr_pawns; i++ )
                {
                    if( (bs->pawns[i]&7) != (target->side_b.pawns[i]&7) )
                        match = false;
                }
            }
            else
            {
                // Must find a rover pawn on same file as each and every target pawn
                for( int i=0; match && i<target->side_b.nbr_pawns; i++ )
                {
                    match = false;
                    for( int k=0; !match && k<bs->nbr_pawns; k++ )
                    {
                        if( (bs->pawns[k]&7) == (target->side_b.pawns[i]&7) )
                            match = true;
                    }
                }
            }
        }
    }
    return match;
}
