/****************************************************************************
 * Facility to lookup book moves using game positions read from a .pgn
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BOOK_H
#define BOOK_H
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/progdlg.h"
#include "DebugPrintf.h"
#include "ChessRules.h"
#include <vector>
#include <algorithm>

// Representation of a book move
struct BookMove
{
    thc::Move move;         // The move
    unsigned int count;     // How many times it is in the book
    unsigned int *play_position_count;      // ptr to how many times it has been played in a human-machine session

    // Allow sorting, so book moves are listed in order of popularity
    bool operator < (const BookMove& bm) const { return count < bm.count; }
    bool operator > (const BookMove& bm) const { return count > bm.count; }
    bool operator == (const BookMove& bm) const { return count == bm.count; }
};

class Book
{
public:

    // Constructor
    Book();

    // Load Book, Return bool error
    bool Load( wxString &error_msg, wxString &pgn_file );

    // Get predefined positions from book, Return bool error
    bool Predefined( wxArrayString &labels, wxArrayString &fens );

    // Lookup simple move list, return true if any moves found
    bool Lookup( thc::ChessPosition &pos, std::vector<thc::Move> &moves );

    // Lookup full move list, return true if any moves found
    bool Lookup( thc::ChessPosition &pos, std::vector<BookMove> &bmoves );

private:

    // A position that appears in the book
    struct BookPosition
    {
        thc::CompressedPosition cpos;
        unsigned int           count;                   // how many times it appears in the book
        unsigned int           play_position_count;     // how often it has appeared on board in human-engine session
    };

    // A position that appears in the book, without session stat
    struct BookPositionInFile
    {
        thc::CompressedPosition cpos;
        unsigned short         count;
    };

    // TODO make these modern C++ consts
    #define BOOK_BUFLEN 200
    #define BOOK_HASH_MSK 0xffff
    #define BOOK_HASH_NBR (BOOK_HASH_MSK+1) // eg BOOK_HASH_MSK=0xffff BOOK_HASH_NBR=65536

    // List of training positions in book
    wxArrayString predefined_labels;    // labels
    wxArrayString predefined_fens;      // positions

    // PGN parsing stuff, still old school
    char fen    [ BOOK_BUFLEN + 10];
    char desc   [ BOOK_BUFLEN + 10];
    char desc2  [ BOOK_BUFLEN + 10];
    char name   [ BOOK_BUFLEN + 10];
    char variation[ BOOK_BUFLEN + 10];
    char date   [ BOOK_BUFLEN + 10];
    char white  [ BOOK_BUFLEN + 10];
    char black  [ BOOK_BUFLEN + 10];
    char result [ BOOK_BUFLEN + 10];
    char opening[ BOOK_BUFLEN + 10];
    char event  [ BOOK_BUFLEN + 10];
    char site   [ BOOK_BUFLEN + 10];
    char move_order_type[BOOK_BUFLEN + 10];

    // Misc
    bool fen_flag;
    int nbr_games;
    FILE *debug_log_file_txt;
    FILE *file_rep;
    FILE *file_inc;
    thc::ChessRules chess_rules;

    // All the book positions
    std::vector<BookPosition> bucket[BOOK_HASH_NBR];

    // Object state
    enum STATE
    {
        INIT,
        PREFIX,
        HEADER,
        IN_COMMENT,
        BETWEEN_MOVES,
        MOVE_NUMBER,
        POST_MOVE_NUMBER,
        POST_MOVE_NUMBER_HAVE_PERIOD,
        POST_MOVE_NUMBER_BLACK,
        PRE_MOVE_WHITE,
        PRE_MOVE_BLACK,
        IN_MOVE_WHITE,
        IN_MOVE_BLACK,
        ERROR_STATE,
        IN_DOLLAR
    };

    // A simple debug mechanism
    char error_buf[128];
    char error_ptr;
    struct DEBUG_S
    {
        char  c;
        STATE state;
    };
    DEBUG_S debug_buf[128];
    char debug_ptr;
    void debug_dump();

    // Stackable line = position + moves
    struct STACK_ELEMENT
    {
        STATE                   state;
        thc::ChessPosition      position;
        int                     nbr_moves;
        thc::Move               big_move_array[1000];
    };

    // Up to 100 levels deep
    STACK_ELEMENT stack_array[100];
    int stack_idx;

    // Compile book. Returns bool error
    bool Compile( wxString &error_msg, wxString &compile_msg, wxString &pgn_file, wxString &pgn_compiled_file );

    // Load compiled book. Returns bool error
    bool LoadCompiled( wxString &error_msg, wxString &pgn_compiled_file );

    // Misc helpers
    FILE *debug_log_file();
    bool TestResult( const char *buf );
    const char *ShowState( STATE state );
    STATE Push( STATE in );
    STATE Pop();
    void Header( char *buf );
    bool DoMove( bool white, int move_number, char *buf );
    void GameBegin();
    void GameOver();
    void FileOver();
    void Error( const char *msg );
    //void FatalError( const char *msg );
    bool Process( FILE *infile, wxProgressDialog &progress );

};

#endif // BOOK_H
