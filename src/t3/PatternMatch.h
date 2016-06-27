/****************************************************************************
 * Test position for pattern
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2016, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PATTERN_MATCH_H
#define PATTERN_MATCH_H
#include "thc.h"
#include "MemoryPositionSearchSide.h"

struct PatternParameters
{
    bool initialised;

    // Type of search (pattern or material balance)
    bool material_balance;

    // Input positon
    thc::ChessPosition cp;

    // Lockdown flags
    bool lockdown[64];

    // common
    bool include_reflections;
    bool include_reverse_colours;

    // pattern
    bool either_to_move;
    bool white_to_move;
    bool allow_more_pieces;

    // material balance
    bool pawns_must_be_on_same_files;
    bool bishops_must_be_same_colour;
    int  number_of_ply;
    bool more_pieces_wq;
    bool more_pieces_wr;
    bool more_pieces_wb;
    bool more_pieces_wn;
    bool more_pieces_wp;
    bool more_pieces_bq;
    bool more_pieces_br;
    bool more_pieces_bb;
    bool more_pieces_bn;
    bool more_pieces_bp;

    PatternParameters() { initialised = false; }
    void OneTimeInit( bool material_balance, thc::ChessPosition &cp )
    {
        if( !initialised )
        {
            initialised = true;
            this->cp = cp;
            this->material_balance = material_balance;
            include_reflections = false;
            include_reverse_colours = false;
            allow_more_pieces = material_balance?false:true;
            either_to_move = false;
            white_to_move = true;
            pawns_must_be_on_same_files = false;
            bishops_must_be_same_colour = false;
            number_of_ply = 1;
            more_pieces_wq = false;
            more_pieces_wr = false;
            more_pieces_wb = false;
            more_pieces_wn = false;
            more_pieces_wp = false;
            more_pieces_bq = false;
            more_pieces_br = false;
            more_pieces_bb = false;
            more_pieces_bn = false;
            more_pieces_bp = false;
            memset(lockdown,0,sizeof(lockdown));
        }
    }
 };

// Mask information for matching to a particular fixed target position
struct PatternMatchTarget
{
    const uint64_t *rank8_target_ptr;
    uint64_t        rank8_mask;
    const uint64_t *rank7_target_ptr;
    uint64_t        rank7_mask;
    const uint64_t *rank6_target_ptr;
    uint64_t        rank6_mask;
    const uint64_t *rank5_target_ptr;
    uint64_t        rank5_mask;
    const uint64_t *rank4_target_ptr;
    uint64_t        rank4_mask;
    const uint64_t *rank3_target_ptr;
    uint64_t        rank3_mask;
    const uint64_t *rank2_target_ptr;
    uint64_t        rank2_mask;
    const uint64_t *rank1_target_ptr;
    uint64_t        rank1_mask;
    thc::ChessPosition cp;
    MpsSide         side_w;
    MpsSide         side_b;
    PatternParameters parm;
};

// PatternDialog class declaration
class PatternMatch
{    

public:
    // Constructor
    PatternMatch();

    // What to search for
    PatternParameters parm;

    // Set up for search
    void Prime( const thc::ChessPosition *rover )
    {
        PrimePattern( rover );
        PrimeMaterialBalance();
    }

    // Start of game
    void NewGame() { in_a_row=0; }

    // Test for pattern
    bool Test( MpsSide *ws, MpsSide *bs, bool white, const char *squares_rover, bool may_need_to_rebuild_side )
    {
        if( parm.material_balance )
            return TestMaterialBalance( ws, bs, squares_rover, may_need_to_rebuild_side );
        else
            return TestPattern( white, squares_rover );
    }

private:

    // Prime
    void PrimePattern( const thc::ChessPosition *rover );
    void PrimeMaterialBalance();
    void InitSide( MpsSide *side, bool white, const char *squares_rover );

    // Test against criteria
    bool TestPattern( bool white, const char *squares_rover );
    bool TestMaterialBalance( MpsSide *ws, MpsSide *bs, const char *squares_rover, bool may_need_to_rebuild_side );
    bool TestMaterialBalanceInner( MpsSide *ws, MpsSide *bs, const char *squares_rover, bool may_need_to_rebuild_side );

    // Working positions
    PatternMatchTarget target_n;    // normal
    PatternMatchTarget target_m;    // mirror image
    PatternMatchTarget target_r;    // reverse colours
    PatternMatchTarget target_rm;   // reverse colours mirror image

    const uint64_t *rank8_ptr;
    const uint64_t *rank7_ptr;
    const uint64_t *rank6_ptr;
    const uint64_t *rank5_ptr;
    const uint64_t *rank4_ptr;
    const uint64_t *rank3_ptr;
    const uint64_t *rank2_ptr;
    const uint64_t *rank1_ptr;

    // Reflection and Reversal loop controller
    int reflect_and_reverse;
    int in_a_row;
};

#endif    // PATTERN_MATCH_H
