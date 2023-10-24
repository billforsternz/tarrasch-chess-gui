/****************************************************************************
 *  Chess Bytecode - chess moves can be encoded in one byte, with a few spare
 *    codes available to allow representation of variations, annotations etc.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BYTECODE_H
#define BYTECODE_H
#include <algorithm>
#include <vector>
#include <string>
#include "thc.h"
#include "GameViewElement.h"


// Non move byte code values (0 reserved)
#define BC_VARIATION_START  1 // variation start
#define BC_VARIATION_END    2 // variation end
#define BC_COMMENT_START    3 // comment start (comment is text Ansi or UTF-8 for further study)
#define BC_COMMENT_END      4 // comment end
#define BC_META_START       5 // meta data start (multi byte meta data)
#define BC_META_END         6 // meta data end
#define BC_ESCAPE           7 // escape (single byte meta data)
#define BC_MOVE_CODES       8 // move codes start here

//
// In branch "new-heart" I am starting out on an ambitious plan to transplant
//  a new heart into Tarrasch - based on bytecode. The byte code will be the
//  same one byte per move representation developed to implement the database
//  features (class CompressMoves) enhanced with techniques to represent
//  variations, annotations, comments (and more?). This should be more flexible
//  maintainable, understandable and improveable than the current MoveTree
//  recursive vector representation.
//
//  I am going to try to keep the code compiling and linking, but it won't
//  necessarily be functional for a while.
//

// For the time being we have Bytecode and Army instead of the former
//  CompressMoves and Side. The plan is to unify once we've transplanted
//  Bytecode into Tarrasch as its new heart (new way of representing chess
//  documents)
struct Army
{
    bool white;
    bool fast_mode;
    int rooks[2];       // locations of each dynamic piece
    int knights[2];     //
    int queens[2];      //
    int pawns[8];
    int bishop_dark;
    int bishop_light;
    int king;
    int nbr_pawns;      // 0-8
    int nbr_rooks;      // 0,1 or 2
    int nbr_knights;    // 0,1 or 2
    int nbr_queens;     // 0,1 or 2
    int nbr_light_bishops;  // 0 or 1
    int nbr_dark_bishops;   // 0 or 1
};

class Bytecode
{
public:
    Bytecode()
    {
        sides[0].white=true;
        sides[0].fast_mode=false;
        sides[1].white=false;
        sides[1].fast_mode=false;
        is_interesting = 0;
        nbr_slow_moves = 0;
    }
    Bytecode( thc::ChessPosition &cp )
    {
        sides[0].white=true;
        sides[0].fast_mode=false;
        sides[1].white=false;
        sides[1].fast_mode=false;
        is_interesting = 0;
        nbr_slow_moves = 0;
        Init(cp);
    }
    bool TryFastMode( Army *side );
    std::string Compress( const std::vector<thc::Move> &moves_in );
    std::string Compress( thc::ChessPosition &cp, const std::vector<thc::Move> &moves_in );
    std::vector<thc::Move> Uncompress( std::string &moves_in );
    std::vector<thc::Move> Uncompress( thc::ChessPosition &cp, std::string &moves_in );
    std::string PgnOut( const std::string& bc_moves_in, const std::string& result );
    void GameViewOut( const std::string& bc_moves_in, const std::string& result, std::vector<GameViewElement> &expansion_out );
    std::string RoughDump( const std::string& moves_in );
    char      CompressMove( thc::Move mv );
    thc::Move UncompressMove( char c );
    Bytecode( const Bytecode& copy_from_me ) { cr=copy_from_me.cr; sides[0]=copy_from_me.sides[0]; sides[1]=copy_from_me.sides[1]; }
    Bytecode & operator= (const Bytecode & copy_from_me ) { cr=copy_from_me.cr; sides[0]=copy_from_me.sides[0]; sides[1]=copy_from_me.sides[1]; return *this; }
    void Init() { TryFastMode( &sides[0]); TryFastMode( &sides[1]); }
    void Init( const thc::ChessPosition &cp ) { cr = cp; Init(); }
    std::string PgnParse( thc::ChessRules &cr2, const std::string str, bool use_semi, int &nbr_converted, bool use_current_language );
    std::string PgnParse( const std::string str );
public:
    thc::ChessRules cr;
    int is_interesting;
    int nbr_slow_moves;
private:
    Army sides[2];
    char CompressSlowMode( thc::Move mv );
    char CompressFastMode( thc::Move mv, Army *side, Army *other );
    thc::Move UncompressSlowMode( char code );
    thc::Move UncompressFastMode( char code, Army *side, Army *other );
    thc::Move UncompressFastMode( char code, Army *side, Army *other, std::string &san_move );
};

// Find the position and moves leading to this offset in the bytecode
void bc_locate( std::string &bc, int offset, thc::ChessPosition &start, std::vector<thc::Move> var );

// Strip out everything except the main line
void bc_mainline( const std::string &bc_in,  std::string &bc_out );

// Insert into game
void bc_insert_str( std::string &bc_in, int offset, const std::string str );
void bc_insert_nag( std::string &bc_in, int offset, int nag );

// Add a move of bytecode
void bc_insert( std::string &bc, int offset, thc::ChessRules &cr, thc::Move mv );

// Skip to matching end
size_t bc_skipover( const std::string &bc, size_t offset );

// Interpret bytecode starting at offset as a comment
std::string bc_comment( std::string &bc, size_t offset );

// Find the index of a move within its variation
//  eg 1. e4 e5 2. Nf3 (2. Nc3 Nf6 3.Bc4 Nxe4)  // idx of 2.Nc3 is 0, idx of 3...Nxe4 is 3)
int bc_variation_idx( const std::string &bc, size_t offset );

#endif // BYTECODE_H