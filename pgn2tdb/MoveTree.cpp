/****************************************************************************
 * Representation of a game's tree of variations
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "MoveTree.h"
#include "thc.h"
using namespace std;
using namespace thc;


// Find a child node's parent
#if 1
MoveTree *MoveTree::Parent( MoveTree *child, thc::ChessRules &cr_out, int &ivar, int &imove )
{
    MoveTree *found=NULL;
    if( child != this )
    {
        int level=-1;
        bool first = true;
        thc::ChessRules cr;
        if( root )
            cr = *root;
        found = ParentCrawler( level, first, child, cr, cr_out, ivar, imove );
    }
    return found;
}
#endif

MoveTree *MoveTree::ParentCrawler( int& level, bool& first, MoveTree *child, thc::ChessRules &cr, thc::ChessRules &cr_out, int &ivar, int &imove )
{
    MoveTree *found=NULL;
    level++;
    if( child == this )
        found = this;
    else
    {
        thc::ChessRules cr_before_move = cr;
        bool at_root = (level==0);
        if( !at_root )
            cr.PlayMove(game_move.move);
        int nbr_vars=variations.size();
        if( nbr_vars )
        {
            for( int i=0; !found && i<nbr_vars; i++ )
            {
                thc::ChessRules cr_temp = cr_before_move;
                vector<MoveTree> &var = variations[i];
                int j, nbr_moves=var.size();
                for( j=0; j<nbr_moves; j++ )
                {
                    found = var[j].ParentCrawler( level, first, child, cr_temp, cr_out, ivar, imove );
                    if( found )
                    {
                        if( first )
                        {
                            first  = false;
                            found  = this;
                            cr_out = cr_before_move;
                            ivar   = i;
                            imove  = j;
                        }
                        break;
                    }
                }
            }
        }
    }
    level--;
    return found;
}

#if 0
// Find a child node's parent
MoveTree *MoveTree::Parent( MoveTree *child, thc::ChessRules &cr_out, int &ivar, int &imove )
{
    std::vector<VARIATION_STACK_ELEMENT> stack;
    Find( child, stack );
    thc::ChessRules cr;
    if( root )
        cr = *root;
    int i=0;
    VARIATION &var = variations[0];
    child = this;
    ivar  = 0;
    imove = 0;
    for( i=0; i<stack.size(); i++ )
    {
        bool final = (i+1 >=  stack.size());
        if( final )
        {
            for( int j=0; j<var.
        }
        var = *stack[i].v;
        int ibranch = stack[i].imove;
        if( i+1 < stack.size() )
        {
            for( int j=0; j<ibranch; j++ )
                cr.PlayMove( var[j].game_move );
        }
        else
        {

        }
    }
}
#endif

// Find a target node in the tree under here, build a stack of variations leading to the node
//   This is a much faster, simpler more general mechanism that the Parent() and ParentCrawler()
//   code - Intention is to transition to Find() instead of Parent() whenever possible
bool MoveTree::Find( MoveTree *target, std::vector<VARIATION_STACK_ELEMENT> &stack )
{
    bool found = false;
    stack.clear();
    int level=0;
    found = FindCrawler( level, target, stack );
    found = (found && stack.size()>0);
    return found;
}


bool MoveTree::FindCrawler( int& level, const MoveTree *target, std::vector<VARIATION_STACK_ELEMENT> &stack )
{
    if( target == this )
        return true;
    else
    {
        int nbr_vars = variations.size();
        if( nbr_vars )
        {
            level++;
            VARIATION_STACK_ELEMENT n;
            stack.push_back(n);
            for( int i=0; i<nbr_vars; i++ )
            {
                vector<MoveTree> &var = variations[i];
                stack[level-1].v = &variations[i];
                int nbr_moves=var.size();
                for( int j=0; j<nbr_moves; j++ )
                {
                    stack[level-1].imove = j;
                    bool found = var[j].FindCrawler( level, target, stack );
                    if( found )
                        return true; // once found we back out very quickly, with no stack pop_back()s
                }
            }
            stack.pop_back();
            level--;
        }
    }
    return false;
}

// Calculate a ChessRules object with history leading to a position
//   Seek() and SeekCrawler() aren't very useful since they fail once the game (in any variation) reaches
//   256 half moves - remove later
bool MoveTree::Seek( const MoveTree *target, thc::ChessRules &cr )
{
    bool found=false;
    int level=-1;
    if( root )
    {
        cr = *root;
        found = SeekCrawler( level, target, cr, found );
    }
    return found;
}

bool MoveTree::SeekCrawler( int& level, const MoveTree *target, thc::ChessRules &cr, bool done )
{
    level++;
    if( !done )
    {
        done = (target==this);
        if( !done )
        {
            if( level>0 )
                cr.PlayMove(game_move.move);
            int nbr_vars=variations.size();
            if( nbr_vars )
            {
                for( int i=0; !done && i<nbr_vars; i++ )
                {
                    vector<MoveTree> &var = variations[i];
                    int nbr_moves=var.size();
                    for( int j=0; !done && j<nbr_moves; j++ )
                        done = var[j].SeekCrawler( level, target, cr, done );
                    if( !done )
                        for( int j=nbr_moves-1; j>=0; j-- )
                            cr.PopMove(var[j].game_move.move);
                }
            }
        }
    }
    level--;
    return done;
}



// Promote the entire variation containing a child node
//  Return ptr to child node in its new position in the promoted variation
MoveTree *MoveTree::Promote( MoveTree *child )
{
    thc::ChessRules cr;
    int ivar;
    int imove;
    MoveTree *promoted_move = NULL;

    // Given a child node, find its parent
    MoveTree *parent = Parent( child, cr, ivar, imove );
    if( parent )
    {

        // Find the variation that contains the parent (i.e. the grandparent),
        //  that variation, from the parent to the end, will be demoted
        int ivar2, imove2;
        MoveTree *grand_parent = Parent( parent, cr, ivar2, imove2 );
        if( grand_parent )
        {

//
// G0   [G1]   G2                                   [G1]=grand_parent
//       P     P     P                              0
//       P0    P1    P2   [P3]   P4    P5           1=ivar2 3=imove2  [P3]=parent
//                         C     C     C               0
//                         C     C                     1
//                         C                           2
//                         C0    C1   [C2]   C3        3=ivar 2=imove  [C2]=child
//                         B0    B1
//                         C     C                     4
//       P     P
//

            // Make a copy of the variation to be promoted                           // to_be_promoted
            VARIATION to_be_promoted = parent->variations[ivar];                     //  C0 C1 [C2] C3
                                                                                     //  B0 B1
            vector<VARIATION>::iterator vpos = parent->variations.begin() + ivar;    //  -> C0 C1 [C2] C3
                                                                                     //     B0 B1
            // Remove the variation from its current location
            parent->variations.erase(vpos);
//
// G0   [G1]   G2                                   [G1]=grand_parent
//       P     P     P                              0
//       P0    P1    P2   [P3]   P4    P5           1=ivar2 3=imove2  [P3]=parent
//                         C     C     C               0
//                         C     C                     1
//                         C                           2
//                         C     C                     4
//       P     P
//
// to_be_promoted = C0 C1 [C2] C3   imove=2
//                  B0 B1


            // Point at the parent within the demoted variation                    // to_be_demoted
            VARIATION to_be_demoted = grand_parent->variations[ivar2];             // P0 P1 P2 [P3] P4 P5
            vector<MoveTree>::iterator mpos = to_be_demoted.begin() + imove2;      //  ->[P3]

            // Erase moves up to but not including parent                          // to_be_demoted
            to_be_demoted.erase( to_be_demoted.begin(), mpos );                    // [P3] P4 P5
            to_be_demoted[0].variations.clear();                                   // don't end up creating a second
                                                                                   // copy of these child variations

            // Add the demoted variation to the parent
            vpos = parent->variations.begin() + ivar;
            parent->variations.insert(vpos,to_be_demoted);
//
// G0   [G1]   G2                                   [G1]=grand_parent
//       P     P     P                              0
//       P0    P1    P2   [P3]   P4    P5           1=ivar2 3=imove2  [P3]=parent
//                         C     C     C               0
//                         C     C                     1
//                         C                           2
//                         P3    P4    P5              3=ivar
//                         C     C                     4
//       P     P
//
// to_be_promoted = C0 C1 [C2] C3  imove=2
//                  B0 B1



            // Point at the parent within the grandparent variation
            mpos = grand_parent->variations[ivar2].begin() + imove2;            // ->P3

            // Copy the promoted move (just the move not the variations) [* note below]
            mpos->game_move = to_be_promoted[0].game_move;
//
// G0   [G1]   G2                                   [G1]=grand_parent
//       P     P     P                              0
//       P0    P1    P2    C0    P4    P5           1=ivar2 3=imove2
//                         C     C     C               0
//                         C     C                     1
//                         C                           2
//                         P3    P4    P5              3=ivar
//                         C     C                     4
//       P     P
//
// to_be_promoted = C0 C1 [C2] C3  imove=2
//                  B0 B1


            // Append the promoted move's variations, if any
            for( size_t i=0; i<to_be_promoted[0].variations.size(); i++ )
                mpos->variations.push_back( to_be_promoted[0].variations[i] );
//
// G0   [G1]   G2                                   [G1]=grand_parent
//       P     P     P                              0
//       P0    P1    P2    C0    P4    P5           1=ivar2 3=imove2
//                         C     C     C               0
//                         C     C                     1
//                         C                           2
//                         P3    P4    P5              3=ivar
//                         C     C                     4
//                         B0    B1
//       P     P
//
// to_be_promoted = C0 C1 [C2] C3  imove=2
//                  B0 B1


            // If not at end erase the rest of the grandparent line
            if( mpos != grand_parent->variations[ivar2].end() )
            {
                mpos++;
                grand_parent->variations[ivar2].erase( mpos, grand_parent->variations[ivar2].end() );
            }
//
// G0   [G1]   G2                                   [G1]=grand_parent
//       P     P     P                              0
//       P0    P1    P2    C0                       1=ivar2 3=imove2
//                         C     C     C               0
//                         C     C                     1
//                         C                           2
//                         P3    P4    P5              3=ivar
//                         C     C                     4
//                         B0    B1
//       P     P
//
// to_be_promoted = C0 C1 [C2] C3  imove=2
//                  B0 B1


            // Graft on the promoted tree, skipping the first MoveTree which
            //  we copied (move only) above [* note above]
            for( size_t i=1; i<to_be_promoted.size(); i++ )
            {
                MoveTree mt = to_be_promoted[i];
                grand_parent->variations[ivar2].push_back(mt);
            }
//
// G0   [G1]   G2                                   [G1]=grand_parent
//       P     P     P                              0
//       P0    P1    P2    C0    C1   [C2]   C3     1=ivar2 3=imove2  imove=2
//                         C     C     C               0
//                         C     C                     1
//                         C                           2
//                         P3    P4    P5              3=ivar
//                         C     C                     4
//                         B0    B1
//       P     P
//
// to_be_promoted = C0 C1 [C2] C3  imove=2
//                  B0 B1


            // The move we started with is now in one of the grandparent's
            //  variations rather than the parent's variations.
            promoted_move = &grand_parent->variations[ivar2][imove+imove2];   // imove=2 + imove2=3 =5 ->[C2]
        }
    }
    return promoted_move;
}


// Demote the entire variation containing a child node
//  Return ptr to child node in its new position in the demoted variation
MoveTree *MoveTree::Demote( MoveTree *child )
{
    thc::ChessRules cr;
    int ivar;
    int imove;
    MoveTree *demoted_move = NULL;

    // Given a child node, find its parent
    MoveTree *parent = Parent( child, cr, ivar, imove );
    if( parent )
    {

        // Search back thru this variation, looking for a subvariation
        for( int i=imove; i>=0; i-- )
        {
            if( parent->variations[ivar][i].variations.size() )
            {

                // Point at the move with (at least one) subvariation
                MoveTree *node = &parent->variations[ivar][i];

                // Point at the first move in the first subvariation
                MoveTree *promote_this = &node->variations[0][0];

                // Promote that subvariation, at the expense of the variation we started
                //  out with. The variation we started out with will be added as the
                //  last subvariation, so find the original move in that subvariation
                promote_this = Promote(promote_this);
                if( promote_this )
                {
                    if( promote_this->variations.size() )
                        demoted_move = &promote_this->variations[0][imove-i];
                }
            }
        }
    }
    return demoted_move;
}

// Delete the rest of a variation
void MoveTree::DeleteRestOfVariation( MoveTree *child )
{
    thc::ChessRules cr;
    int ivar;
    int imove;

    // Given a child node, find its parent
    MoveTree *parent = Parent( child, cr, ivar, imove );
    if( parent )
    {
        VARIATION &variation = parent->variations[ivar];

        // If the next move is part of the variation, delete from there
        int sz = variation.size();
        if( imove+1 < sz )
        {
            vector<MoveTree>::iterator pos = variation.begin() + imove+1;
            variation.erase( pos, variation.end() );
        }
    }
}

// Delete variation
void MoveTree::DeleteVariation( MoveTree *child )
{
    thc::ChessRules cr;
    int ivar;
    int imove;

    // Given a child node, find its parent
    MoveTree *parent = Parent( child, cr, ivar, imove );
    if( parent )
    {

        // Remove the variation from its current location
        vector<VARIATION>::iterator pos = parent->variations.begin() + ivar;
        parent->variations.erase(pos);
    }
}


