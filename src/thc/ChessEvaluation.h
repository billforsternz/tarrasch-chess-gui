/****************************************************************************
 * Chess classes - Simple chess AI, leaf scoring function for position
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSEVALUATION_H
#define CHESSEVALUATION_H
#include "ChessRules.h"
#include "Move.h"

// TripleHappyChess
namespace thc
{

class ChessEvaluation: public ChessRules
{
public:
    // Default contructor
    ChessEvaluation() : ChessRules() 
    {
    }

    // Copy constructor
    ChessEvaluation( const ChessPosition& src ) : ChessRules( src ) 
    {
    }

    // Assignment operator
    ChessEvaluation& operator=( const ChessPosition& src )
    {
        *((ChessRules *)this) = src;
        return *this;
    }


    // Use leaf evaluator to generate a sorted move list
    void GenLegalMoveListSorted( MOVELIST *list );
    void GenLegalMoveListSorted( std::vector<Move> &moves );

    // Evaluate a position, leaf node (useful for playing programs)
    void EvaluateLeaf( int &material, int &positional );

// internal stuff
protected:

    // Always some planning before calculating a move
    void Planning();

    // Calculate material that side to play can win directly
    int Enprise();
    int EnpriseWhite();   // fast white to move version
    int EnpriseBlack();   // fast black to move version

// misc
private:
    bool white_is_better;
    bool black_is_better;
    int  planning_score_white_pieces;
    int  planning_score_black_pieces;
    int  planning_white_piece_pawn_percent;
    int  planning_black_piece_pawn_percent;
};

} //namespace thc

#endif //CHESSEVALUATION_H
