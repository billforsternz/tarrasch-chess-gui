/****************************************************************************
 * Chess classes - Representation of the position on the board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CHESSPOSITION_H
#define CHESSPOSITION_H
#include <string>
#include <stddef.h>
#include "ChessPositionRaw.h"

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
        strcpy( squares,
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

    // Copy constructor
    ChessPosition( const ChessPosition& src )
    {
        memcpy( (void*)this, (void*)&src, sizeof(ChessPosition) );
    }

    // Assignment operator
    ChessPosition& operator=( const ChessPosition& src )
    {
        memcpy( (void*)this, (void*)&src, sizeof(ChessPosition) );
        return( *this );
    }

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
