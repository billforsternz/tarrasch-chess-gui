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
        }
    }
 };

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

public:
    // Constructor
    PatternMatch();

    // What to search for
    PatternParameters search_criteria;

    // Set up for search
    void Prime( const thc::ChessPosition *rover )
    {
        PrimePattern( rover );
        PrimeMaterialBalance();
    }

    // Test for pattern
    bool Test( MpsSide *ws, MpsSide *bs )
    {
        if( search_criteria.material_balance )
            return TestMaterialBalance( ws, bs );
        else
            return TestPattern();
    }

private:

    // Prime
    void PrimePattern( const thc::ChessPosition *rover );
    void PrimeMaterialBalance();

    // Test against criteria
    bool TestPattern();
    bool TestMaterialBalance( MpsSide *ws, MpsSide *bs );

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

};

#endif    // PATTERN_MATCH_H
