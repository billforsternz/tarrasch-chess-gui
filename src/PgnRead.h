/****************************************************************************
 * Facility to lookup book moves using game positions read from a .pgn
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PGN_READ_H
#define PGN_READ_H
#include <vector>
#include <algorithm>
#include "thc.h"
#include "ProgressBar.h"

#define FIELD_BUFLEN 200

// Callback
bool hook_gameover( char callback_code, const char *fen, const char *event, const char *site, const char *date, const char *round,
                   const char *white, const char *black, const char *result, const char *white_elo, const char *black_elo, const char *eco,
                   int nbr_moves, thc::Move *moves, uint64_t *hashes );


class PgnRead
{
public:

    // Constructor
    PgnRead( char callback_code, ProgressBar *pb=0 );

    bool Process( FILE *infile );

private:
    char callback_code;
    ProgressBar *pb;

    // PGN parsing stuff, still old school
    char fen    [ FIELD_BUFLEN + 10];
    char desc   [ FIELD_BUFLEN + 10];
    char desc2  [ FIELD_BUFLEN + 10];
    char name   [ FIELD_BUFLEN + 10];
    char variation[ FIELD_BUFLEN + 10];
    char date   [ FIELD_BUFLEN + 10];
    char white  [ FIELD_BUFLEN + 10];
    char black  [ FIELD_BUFLEN + 10];
    char result [ FIELD_BUFLEN + 10];
    char eco    [ FIELD_BUFLEN + 10];
    char event  [ FIELD_BUFLEN + 10];
    char site   [ FIELD_BUFLEN + 10];
    char white_elo[ FIELD_BUFLEN + 10];
    char black_elo[ FIELD_BUFLEN + 10];
    char round    [ FIELD_BUFLEN + 10];
 
    char move_order_type[FIELD_BUFLEN + 10];

    // Misc
    bool fen_flag;
    bool first_move;
    int first_move_offset;
    int nbr_games;
    FILE *debug_log_file_txt;
    FILE *file_rep;
    FILE *file_inc;
    thc::ChessRules chess_rules;
    uint64_t hash;

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
        NORMAL_EXIT,
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
        uint64_t                big_hash_array[1000];
    };

    // Up to 20 levels deep
    STACK_ELEMENT stack_array[20];
    int stack_idx;

    // Misc helpers
    FILE *debug_log_file();
    bool TestResult( const char *buf );
    const char *ShowState( STATE state );
    STATE Push( STATE in );
    STATE Pop();
    void Header( char *buf );
    bool DoMove( bool white, int move_number, char *buf );
    void GameBegin();
    void GameParse( std::string &str );
    bool GameOver();
    void FileOver();
    void Error( const char *msg );

};

#endif // PGN_READ_H
