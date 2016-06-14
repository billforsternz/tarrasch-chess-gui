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

// Mask information for matching to a particular fixed target position
struct PatternMatchMask
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
};

// PatternDialog class declaration
class PatternMatch
{    

private:
    // Working positions
    PatternMatchMask pmm_n;    // normal
    PatternMatchMask pmm_m;    // mirror image
    PatternMatchMask pmm_r;    // reverse colours
    PatternMatchMask pmm_rm;   // reverse colours mirror image

    const uint64_t *rank8_ptr;
    const uint64_t *rank7_ptr;
    const uint64_t *rank6_ptr;
    const uint64_t *rank5_ptr;
    const uint64_t *rank4_ptr;
    const uint64_t *rank3_ptr;
    const uint64_t *rank2_ptr;
    const uint64_t *rank1_ptr;

    MpsSide white;
    MpsSide black;

public:
    // Constructor
    PatternMatch();

    // Prepare for series of calls to Test()
    void Prime( const thc::ChessPosition *p );

    // Test against criteria
    bool Test();

    // Prepare for series of calls to TestMaterialBalance()
    void PrimeMaterialBalance();

    // Test against criteria
    bool TestMaterialBalance( MpsSide *ws, MpsSide *bs );

    // Input positon
    thc::ChessPosition cp;
     
    bool material_balance;

    // common
    bool include_reflections;
    bool include_reverse_colours;
    bool allow_more_pieces;

    // pattern
    bool either_to_move;
    bool white_to_move;

    // material balance
    bool pawns_must_be_on_same_files;
    bool bishops_must_be_same_colour;
    int  number_of_ply;
};

#endif    // PATTERN_MATCH_H
