/****************************************************************************
 * Chess classes - Simple chess AI, add search to ChessEvaluation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSENGINE_H
#define CHESSENGINE_H
#include "thc.h"

// TripleHappyChess
namespace thc
{

class ChessEngine: public ChessEvaluation
{
public:

    // Default contructor
    ChessEngine() : ChessEvaluation() 
    {
    }

    // Copy constructor
    ChessEngine( const ChessPosition& src ) : ChessEvaluation( src ) 
    {
    }

    // Assignment operator
    ChessEngine& operator=( const ChessPosition& src )
    {
        *((ChessEvaluation *)this) = src;
        return *this;
    }

    // Calculate a new move, returns bool have_move, sets score in units of balance*decipawns (white positive)
    bool CalculateNextMove( bool &only_move, int &score, Move &move, int balance, int depth );

    // A version for Multi-PV mode, called repeatedly, removes best move from
    //  movelist each time
    bool CalculateNextMove( int &score, Move &move, int balance, int depth, bool first );

    // Public interface to version for repitition avoidance
    bool CalculateNextMove( bool new_game, std::vector<Move> &pv, Move &bestmove, int &score_cp,
                            unsigned long ms_time,
                            unsigned long ms_budget,
                            int balance,
                            int &depth );

    // Retrieve PV (primary variation?), call after CalculateNextMove()
    void GetPV( std::vector<Move> &pv );

    // Run test(s)
    void Test();

private:
    //###################################
    //#
    //#  Internal stuff
    //#
    //###################################

    // Internal version for repitition avoidance
    bool CalculateNextMove( MOVELIST &ml, bool &only_move, int &score, int &besti, int balance, int depth );

    // Has this position occurred before ?
    bool IsRepitition();

    // Do an careful (i.e. expensive) sort on the movelist
    void CarefulSort( MOVELIST &ml );

    // Recursive scoring function
    int Score( MOVELIST &ml, int &besti );
    int ScoreBlackToMove( MOVELIST &ml, int &besti, int white_mobility );
    int ScoreWhiteToMove( MOVELIST &ml, int &besti, int black_mobility );

    // Internal test suite
    void TestInternals();
    void TestGame();
    void TestPosition();
    void TestEnprise();
};

} //namespace thc

#endif //CHESSENGINE_H
