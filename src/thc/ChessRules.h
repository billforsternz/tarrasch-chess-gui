/****************************************************************************
 * Chess classes - Rules of chess
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSRULES_H
#define CHESSRULES_H
#include "ChessPosition.h"
#include "Move.h"
#include <vector>

// TripleHappyChess
namespace thc
{

// Class encapsulates state of game and operations available
class ChessRules: public ChessPosition
{
public:
    // Default constructor
    ChessRules() : ChessPosition() { Init(); }
    void Init()
    {
        history_idx    = 1;    // prevent bogus repition draws
        history[0].src = a8;   // (look backwards through history stops when src==dst)
        history[0].dst = a8;
        detail_idx =0;
    }

    // Copy constructor
    ChessRules( const ChessPosition& src ) : ChessPosition( src ) 
    {
        Init();   // even if src is eg ChessRules or ChessEngine don't
                  //   copy stuff for repitition, 50 move rule
    }

    // Assignment operator
    ChessRules& operator=( const ChessPosition& src )
    {
        *((ChessPosition *)this) = src;
        Init();   // even if src is eg ChessRules or ChessEngine don't
                  //   copy stuff for repitition, 50 move rule
        return *this;
    }

    // Initialise from Forsyth string
    bool Forsyth( const char *txt )
    {
        bool okay = ChessPosition::Forsyth(txt);
        if( okay )
            Init(); // clear stuff for repitition, 50 move rule
        return okay;
    }

    //  Test for legal position, sets reason to a mask of possibly multiple reasons
    bool IsLegal( ILLEGAL_REASON& reason );

    // Play a move
    void PlayMove( Move imove );

    // Check draw rules (50 move rule etc.)
    bool IsDraw( bool white_asks, DRAWTYPE &result );

    // Check insufficient material draw rule
    bool IsInsufficientDraw( bool white_asks, DRAWTYPE &result );

    // Evaluate a position, returns bool okay (not okay means illegal position)
    bool Evaluate( TERMINAL &score_terminal );

    // Is a square is attacked by enemy ?
    bool AttackedSquare( Square square, bool enemy_is_white );

    // Determine if an occupied square is attacked
    bool AttackedPiece( Square square );

    // Transform a position with W to move into an equivalent with B to move and vice-versa
    void Transform();

    // Transform a W move in a transformed position to a B one and vice-versa
    Move Transform( Move m );

    // Create a list of all legal moves in this position
    void GenLegalMoveList( std::vector<Move> &moves );

    //  Create a list of all legal moves in this position, with extra info
    void GenLegalMoveList(  std::vector<Move> &moves,
                            std::vector<bool> &check,
                            std::vector<bool> &mate,
                            std::vector<bool> &stalemate );

    // Create a list of all legal moves in this position
    void GenLegalMoveList( MOVELIST *list );

    // Create a list of all legal moves in this position, with extra info
    void GenLegalMoveList( MOVELIST *list, bool check[MAXMOVES],
                                           bool mate[MAXMOVES],
                                           bool stalemate[MAXMOVES] );

    // Make a move (with the potential to undo)
    void PushMove( Move& m );

    // Undo a move
    void PopMove( Move& m );
    
    // Test fundamental internal assumptions and operations
    void TestInternals();

// Private stuff
protected:

    // Generate a list of all possible moves in a position (including
    //  illegally "moving into check")
    void GenMoveList( MOVELIST *l );

    // Generate moves for pieces that move along multi-move rays (B,R,Q)
    void LongMoves( MOVELIST *l, Square square, const lte *ptr );

    // Generate moves for pieces that move along single-move rays (K,N,P)
    void ShortMoves( MOVELIST *l, Square square, const lte *ptr, SPECIAL special  );

    // Generate list of king moves
    void KingMoves( MOVELIST *l, Square square );

    // Generate list of white pawn moves
    void WhitePawnMoves( MOVELIST *l, Square square );

    // Generate list of black pawn moves
    void BlackPawnMoves( MOVELIST *l, Square square );

    // Evaluate a position, returns bool okay (not okay means illegal position)
    bool Evaluate( MOVELIST *list, TERMINAL &score_terminal );

    //### Data

    // Move history is a ring array
    Move history[256];                 // must be 256 ..
    unsigned char history_idx;          // .. so this loops around naturally
    
    // Detail stack is a ring array
    DETAIL detail_stack[256];           // must be 256 ..
    unsigned char detail_idx;           // .. so this loops around naturally
};

} //namespace thc

#endif //CHESSRULES_H
