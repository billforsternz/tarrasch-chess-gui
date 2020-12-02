/****************************************************************************
 * Triple Happy Chess library = thc library
 *  This is thc rendered as a single thc.h header + thc.cpp source file to
 *  avoid the complications of libraries - Inspired by sqlite.c
 *
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

/*
    thc.h The basic idea is to concatenate the following into one .h file;

        ChessDefs.h
        Move.h
        ChessPositionRaw.h
        ChessPosition.h
        ChessRules.h
        ChessEvaluation.h

 */

#include <stddef.h>
#include <string>
#include <vector>
/****************************************************************************
 * Chessdefs.h Chess classes - Common definitions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSDEFS_H
#define CHESSDEFS_H

// Simple definition to aid platform portability (only remains of former Portability.h)
int strcmp_ignore( const char *s, const char *t ); // return 0 if case insensitive match

// Fast test for is square white or black. Intend to move this to namespace thc when convenient...
inline bool is_dark( int sq )
{
    bool dark = (!(sq&8) &&  (sq&1))    // eg (a8,b8,c8...h8) && (b8|d8|f8|h8) odd rank + odd file
             || ( (sq&8) && !(sq&1));   // eg (a7,b7,c7...h7) && (a7|c7|e7|g7) even rank + even file
    return dark;
}

// TripleHappyChess
namespace thc
{

// Use the most natural square convention possible; Define Square to
//  correspond to a conventionally oriented chess diagram; Top left corner
//  (square a8) is 0, bottom right corner (square h1) is 63.
// Note that instead of defining a special piece type, we use the built in
//  char type, with 'N'=white knight, 'b'=black bishop etc. and ' '=an
//  empty square.
enum Square
{
    a8=0,
        b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
    SQUARE_INVALID
};

// thc::Square utilities
inline char get_file( Square sq )
    { return static_cast<char> (  (static_cast<int>(sq)&0x07) + 'a' ); }           // eg c5->'c'
inline char get_rank( Square sq )
    { return static_cast<char> (  '8' - ((static_cast<int>(sq)>>3) & 0x07) ); }    // eg c5->'5'
inline Square make_square( char file, char rank )
    { return static_cast<Square> ( ('8'-(rank))*8 + ((file)-'a') );  }            // eg ('c','5') -> c5

// Special (i.e. not ordinary) move types
enum SPECIAL
{
    NOT_SPECIAL = 0,
    SPECIAL_KING_MOVE,     // special only because it changes wking_square, bking_square
    SPECIAL_WK_CASTLING,
    SPECIAL_BK_CASTLING,
    SPECIAL_WQ_CASTLING,
    SPECIAL_BQ_CASTLING,
    SPECIAL_PROMOTION_QUEEN,
    SPECIAL_PROMOTION_ROOK,
    SPECIAL_PROMOTION_BISHOP,
    SPECIAL_PROMOTION_KNIGHT,
    SPECIAL_WPAWN_2SQUARES,
    SPECIAL_BPAWN_2SQUARES,
    SPECIAL_WEN_PASSANT,
    SPECIAL_BEN_PASSANT,
};

// Results of a test for legal position, note that they are powers
//  of 2, allowing a mask of reasons
enum ILLEGAL_REASON
{
    IR_NULL=0, IR_PAWN_POSITION=1, //pawns on 1st or 8th rank
    IR_NOT_ONE_KING_EACH=2, IR_CAN_TAKE_KING=4,
    IR_WHITE_TOO_MANY_PIECES=8, IR_WHITE_TOO_MANY_PAWNS=16,
    IR_BLACK_TOO_MANY_PIECES=32, IR_BLACK_TOO_MANY_PAWNS=64
};

// Types of draw checked by IsDraw()
enum DRAWTYPE
{
    NOT_DRAW,
    DRAWTYPE_50MOVE,
    DRAWTYPE_INSUFFICIENT,      // draw if superior side wants it
                                //  since inferior side has insufficent
                                //  mating material
    DRAWTYPE_INSUFFICIENT_AUTO, // don't wait to be asked, eg draw
                                //  immediately if bare kings
    DRAWTYPE_REPITITION,
};

// Stalemate or checkmate game terminations
enum TERMINAL
{
    NOT_TERMINAL = 0,
    TERMINAL_WCHECKMATE = -1,   // White is checkmated
    TERMINAL_WSTALEMATE = -2,   // White is stalemated
    TERMINAL_BCHECKMATE = 1,    // Black is checkmated
    TERMINAL_BSTALEMATE = 2     // Black is stalemated
};

// Calculate an upper limit to the length of a list of moves
#define MAXMOVES (27 + 2*13 + 2*14 + 2*8 + 8 + 8*4  +  3*27)
                //[Q   2*B    2*R    2*N   K   8*P] +  [3*Q]
                //             ^                         ^
                //[calculated practical maximum   ] + [margin]

// We have developed an algorithm to compress any legal chess position,
//  including who to move, castling allowed flags and enpassant_target
//  into 24 bytes
union CompressedPosition
{
    unsigned char storage[24];
    unsigned int ints[ 24 / sizeof(unsigned int) ];
};

// Types we'd really rather have in PrivateChessDefs.h, but not possible
//  at the moment, so we reluctantly expose them to users of the chess
//  classes.
typedef unsigned char lte;   // lte = lookup table element
typedef int32_t DETAIL;

} //namespace thc

#endif // CHESSDEFS_H
/****************************************************************************
 * Move.h Chess classes - Move
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef MOVE_H
#define MOVE_H

// TripleHappyChess
namespace thc
{
class ChessRules;

// Our representation of a chess move
//
// Note this is really an old school C struct, designed for speed
// There is no contructor on purpose, we don't want unnecessary
// contruction of an array of Moves in a MOVELIST when we are running
// the fast move generator.
// The default assignment operator (bitwise copy) is ideal.
// We define bitwise == and != operators
// At one time we had 4 (count em, 4) move representations, but this
// was always the primary and main representation. The others were
// FMOVE (16 bit moves), NMOVE (natural string representation, eg
// "Nf3") and TMOVE (terse string representation eg "g1f3"). When I
// realised I could streamline an IMOVE (the old name for Move =
// internal move) to only 32 bits, I realised I could live without
// FMOVEs and that sparked a large simplification exercise.
//
class Move
{
public:
    // Move is a lightweight type, it is accomodated in only 32 bits
    Square  src       : 8;
    Square  dst       : 8;
    SPECIAL special   : 8;
    int     capture   : 8;      // ' ' (empty) if move not a capture
                                // for some reason Visual C++ 2005 (at least)
                                // blows sizeof(Move) out to 64 bits if
                                // capture is defined as char instead of int

    bool operator ==(const Move &other) const
    {
        return( *((int32_t *)this) == *((int32_t *)(&other)) );
    }

    bool operator !=(const Move &other) const
    {
        return( *((int32_t *)this) != *((int32_t *)(&other)) );
    }

    // Use these sparingly when you need to specifically mark
    //  a move as not yet set up (defined when we got rid of
    //  16 bit FMOVEs, we could always set and test 0 with those)
    void Invalid()  { src=a8; dst=a8; }
    bool Valid()    { return src!=a8 || dst!=a8; }

    // Read natural string move eg "Nf3"
    //  return bool okay
    bool NaturalIn( ChessRules *cr, const char *natural_in );

    // Read natural string move eg "Nf3"
    //  return bool okay
    // Fast alternative for known good input
    bool NaturalInFast( ChessRules *cr, const char *natural_in );

    // Read terse string move eg "g1f3"
    //  return bool okay
    bool TerseIn( ChessRules *cr, const char *tmove );

    // Convert to natural string
    //  eg "Nf3"
    std::string NaturalOut( ChessRules *cr );

    // Convert to terse string eg "e7e8q"
    std::string TerseOut();
};

// List of moves
struct MOVELIST
{
    int count;  // number of moves
    Move moves[MAXMOVES];
};

} //namespace thc

#endif //MOVE_H
/****************************************************************************
 * ChessPositionRaw.h Chess classes - A raw chess position, could be used as a C style POD
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSPOSITIONRAW_H
#define CHESSPOSITIONRAW_H


// TripleHappyChess
namespace thc
{

// ChessPositionRaw - A complete representation of the position on the
//  board. Corresponds broadly to fields of Forsyth representation
struct ChessPositionRaw
{
    // Who to play
    bool white;

    // Pieces on board in readable form; Forsyth like but without compression,
    //  eg "rnbqkbnr"
    //     "pppppppp"
    //     "        "
    //     "        "
    //     "        "
    //     "        "
    //     "PPPPPPPP"
    //     "RNBQKBNR"
    //  (represents starting position)
    char squares[64 +1]; // +1 allows a trailing '\0'
    // note indexed according to Square convention, a8=0 etc.

    // Half moves since pawn move or capture (for 50 move rule)
    //  eg after 1.e4 it's 0
    int  half_move_clock;

    // Full move count. Initially 1 and increments after black moves
    //  eg after 1.e4 it's 1
    //  eg after 1... d6 it's 2
    int  full_move_count;

    // The following are deemed "details", and must be stored at the
    //  end of the structure. Search for DETAIL for, ahem, details.
    //  For performance reasons we want the details to be able to fit
    //  into 32 bits.
    Square enpassant_target : 8;
    Square wking_square     : 8;
    Square bking_square     : 8;
    unsigned int  wking     : 1;    // Castling still allowed flags
    unsigned int  wqueen    : 1;    //  unfortunately if the castling
    unsigned int  bking     : 1;    //  flags are declared as bool,
    unsigned int  bqueen    : 1;    //  with Visual C++ at least,
                                    //  the details blow out and use
                                    //  another 32 bits (??!!)
    // Note that for say white king side castling to be allowed in
    //  in the same sense as the Forsyth representation, not only
    //  must wking be true, but the  white king and king rook must
    //  be present and in position, see the wking_allowed() etc
    //  methods in class ChessPosition, these are used for the ChessPosition
    //  == operator.
};

} //namespace thc

#endif //CHESSPOSITIONRAW_H
/****************************************************************************
 * ChessPosition.h Chess classes - Representation of the position on the board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSPOSITION_H
#define CHESSPOSITION_H

// TripleHappyChess
namespace thc
{
    class Move;

// ChessPosition - A complete representation of the position on the
//  board.
class ChessPosition : public ChessPositionRaw
{
public:

    // Default constructor
    ChessPosition()  { Init(); }
    virtual ~ChessPosition()  {}    // destructor not actually needed now as
                                    //  we don't allocate resources in ctor.
    void Init()
    {
        white = true;
        strcpy_s( squares, sizeof(squares),
           "rnbqkbnr"
           "pppppppp"
           "        "
           "        "
           "        "
           "        "
           "PPPPPPPP"
           "RNBQKBNR" );
        enpassant_target = SQUARE_INVALID;
        wking  = true;
        wqueen = true;
        bking  = true;
        bqueen = true;
        wking_square = e1;
        bking_square = e8;
        half_move_clock = 0;
        full_move_count = 1;
    }

    // Copy constructor and Assignment operator. Defining them this way
    //  generates simple bitwise memory copy, which is exactly what we
    //  want and is better practice than the old memcpy() versions (which
    //  copy the vtable ptr as well - we don't want that). Thanks to Github
    //  user metiscus for the pull request that fixed this.
    ChessPosition( const ChessPosition& src ) = default;
    ChessPosition& operator=( const ChessPosition& src ) = default;

    // Equality operator
    bool operator ==( const ChessPosition &other ) const
    {
        return( white == other.white                        &&
                0 == memcmp( &squares, &other.squares, 64 ) &&
                groomed_enpassant_target() == other.groomed_enpassant_target()  &&
                wking_allowed()  == other.wking_allowed()   &&
                wqueen_allowed() == other.wqueen_allowed()  &&
                bking_allowed()  == other.bking_allowed()   &&
                bqueen_allowed() == other.bqueen_allowed()
             );
    }

    // < Operator
    // Why do we want this elaborate operator? Well if we ever use the std
    //  sort algorithm on ChessPosition objects we need a < operator with
    //  the following properties (a and b are two ChessPositions);
    // 1) if a==b is true then a<b is false
    // 2) if a<b  is true then b<a is false
    // To achieve this we need a < operator that takes into account all
    // the factors as the == operator. If you don't bother and use any old
    // deterministic function (what does one position being less than
    // another even mean?) then you'll get subtle errors in your sorts
    // (the voice of bitter experience speaks).
    bool operator <( const ChessPosition &other ) const
    {
        bool temp = (white==other.white);
        if( !temp )
            return white;
        int itemp = memcmp( &squares, &other.squares, 64 );
        if( itemp != 0 )
            return( itemp < 0 );
        temp = (groomed_enpassant_target() == other.groomed_enpassant_target());
        if( !temp )
            return( (int)groomed_enpassant_target() < (int)other.groomed_enpassant_target() );
        temp = (wking_allowed()  == other.wking_allowed()  );
        if( !temp )
            return wking_allowed();
        temp = (wqueen_allowed() == other.wqueen_allowed() );
        if( !temp )
            return wqueen_allowed();
        temp = (bking_allowed()  == other.bking_allowed()  );
        if( !temp )
            return bking_allowed();
        temp = (bqueen_allowed() == other.bqueen_allowed() );
        if( !temp )
            return bqueen_allowed();
        return false;   // ==
    }

    // Inequality operator
    bool operator !=( const ChessPosition &other ) const
    {
        bool same = (*this == other);
        return !same;
    }

    // Groomed enpassant target is enpassant target qualified by the possibility to
    //  take enpassant. For example any double square pawn push creates an
    //  enpassant target, but a groomed enpassant target will still be SQUARE_INVALID
    //  unless there is an opposition pawn in position to make the capture
    Square groomed_enpassant_target() const
    {
        Square ret = SQUARE_INVALID;
        if( white && a6<=enpassant_target && enpassant_target<=h6 )
        {
            bool zap=true;  // zap unless there is a 'P' in place
            int idx = enpassant_target+8; //idx = SOUTH(enpassant_target)
            if( enpassant_target>a6 && squares[idx-1]=='P' )
                zap = false;    // eg a5xb6 ep, through g5xh6 ep
            if( enpassant_target<h6 && squares[idx+1]=='P' )
                zap = false;    // eg b5xa6 ep, through h5xg6 ep
            if( !zap )
                ret = enpassant_target;
        }
        else if( !white && a3<=enpassant_target && enpassant_target<=h3 )
        {
            bool zap=true;  // zap unless there is a 'p' in place
            int idx = enpassant_target-8; //idx = NORTH(enpassant_target)
            if( enpassant_target>a3 && squares[idx-1]=='p' )
                zap = false;    // eg a4xb3 ep, through g4xh3 ep
            if( enpassant_target<h3 && squares[idx+1]=='p' )
                zap = false;    // eg b4xa3 ep, through h4xg3 ep
            if( !zap )
                ret = enpassant_target;
        }
        return ret;
    }

    // Castling allowed ?
    bool wking_allowed()  const { return wking  && squares[e1]=='K' && squares[h1]=='R'; }
    bool wqueen_allowed() const { return wqueen && squares[e1]=='K' && squares[a1]=='R'; }
    bool bking_allowed()  const { return bking  && squares[e8]=='k' && squares[h8]=='r'; }
    bool bqueen_allowed() const { return bqueen && squares[e8]=='k' && squares[a8]=='r'; }

    // Return true if Positions are the same (including counts)
    bool CmpStrict( const ChessPosition &other ) const;

    // For debug
    std::string ToDebugStr( const char *label = 0 );

    // Set up position on board from Forsyth string with extensions
    //  return bool okay
    virtual bool Forsyth( const char *txt );

    // Publish chess position and supplementary info in forsyth notation
    std::string ForsythPublish();

    // Compress a ChessPosition into 24 bytes, return 16 bit hash
    unsigned short Compress( CompressedPosition &dst ) const;

    // Decompress chess position
    void Decompress( const CompressedPosition &src );

    // Calculate a hash value for position (not same as CompressPosition algorithm hash)
    uint32_t HashCalculate();

    // Incremental hash value update
    uint32_t HashUpdate( uint32_t hash_in, Move move );

    // Calculate a hash value for position (64 bit version)
    uint64_t Hash64Calculate();

    // Incremental hash value update (64 bit version)
    uint64_t Hash64Update( uint64_t hash_in, Move move );

    // Whos turn is it anyway
    inline bool WhiteToPlay() const { return white; }
    void Toggle() { white = !white; }
};

} //namespace thc

#endif //CHESSPOSITION_H
/****************************************************************************
 * ChessRules.h Chess classes - Rules of chess
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSRULES_H
#define CHESSRULES_H

// TripleHappyChess
namespace thc
{

// Class encapsulates state of game and operations available
class ChessRules: public ChessPosition
{
public:
    // Default constructor
    ChessRules() : ChessPosition() { Init(); }
    void Init()    // TODO == ChessRules::Init() should call ChessPosition::Init() right ????!!!!
                   // Thoughts: Maybe - but can't do this casually. For example we would need to
                   // change the code that converts ChessPosition to ChessRules below, both the
                   // copy constructor and assignment operator use ChessRules::Init() at a time
                   // when it would be disastrous to set the initial position (because
                   // we have carefully copied a position into the ChessRules object)
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

    // Test internals, for porting to new environments etc
    bool TestInternals( int (*log)(const char *,...) = NULL );

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

    // Get number of times position has been repeated
    int GetRepetitionCount();

    // Check insufficient material draw rule
    bool IsInsufficientDraw( bool white_asks, DRAWTYPE &result );

    // Evaluate a position, returns bool okay (not okay means illegal position)
    bool Evaluate();
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
/****************************************************************************
 * ChessEvaluation.h Chess classes - Simple chess AI, leaf scoring function for position
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2020, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSEVALUATION_H
#define CHESSEVALUATION_H

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
