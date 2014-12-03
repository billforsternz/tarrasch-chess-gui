/****************************************************************************
 * Chess classes - Simple chess AI, add search to ChessEvaluation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <algorithm>
#include "Portability.h"
#include "DebugPrintf.h"
#include "ChessEngine.h"
#include "PrivateChessDefs.h"
using namespace std;
using namespace thc;

//-- preferences
//#define TRANSFORM   // transform so all moves are played by white
#define ALPHA_BETA
// #define EXTRA_DEBUG_CODE1
// #define EXTRA_DEBUG_CODE2
// #define EXTRA_DEBUG_CODE3

// Define one of the following only
//#define SIX_PLY
//#define FIVE_PLY   //when I started infinite search, it was set to this
//#define THREE_PLY
#define VARIABLE_PLY
#define DEFAULT_DEPTH 4
int  gbl_depth=DEFAULT_DEPTH;
bool gbl_stop;
#ifdef VARIABLE_PLY
    #define IF_STOP_RECURSING if( recurse_level>gbl_depth || gbl_stop )
#endif
#ifdef SIX_PLY
    #define IF_STOP_RECURSING if( recurse_level>5 )
#endif
#ifdef FIVE_PLY
    #define IF_STOP_RECURSING if( recurse_level>4 )
#endif
#ifdef THREE_PLY
    #define IF_STOP_RECURSING if( recurse_level>2 )
#endif

  #define LEVEL_STOP_SORTING  5 //12000         11500
//#define LEVEL_STOP_SORTING  4 //16797         
//#define LEVEL_STOP_SORTING  3 //21625
//#define LEVEL_STOP_SORTING  2 //40578
//#define LEVEL_STOP_SORTING  1 //84750
//#define LEVEL_STOP_SORTING  0 //84950
//#define LEVEL_CAREFUL_SORTING 6 //23203        30265
//#define LEVEL_CAREFUL_SORTING 5 //11118        16422
//#define LEVEL_CAREFUL_SORTING 4 // 8719        11700
  #define LEVEL_CAREFUL_SORTING 3 // 8828 8797   11500
//#define LEVEL_CAREFUL_SORTING 2 //12031
//#define LEVEL_CAREFUL_SORTING 1 //12328
//#define LEVEL_CAREFUL_SORTING 0 //12250
int DIAG_evaluate_count;
int DIAG_evaluate_leaf_count;
int DIAG_make_move_primary;	
int DIAG_cutoffs;
int DIAG_deep_cutoffs;

// Utilities
#ifndef nbrof
    #define nbrof(array) (sizeof((array))/sizeof((array)[0]))
#endif

// Macro to convert chess notation to Square convention,   
//  eg a8=0, h8=7, a8=8, h1=63
#define SQ(f,r)  ( (Square) ( ('8'-(r))*8 + ((f)-'a') )   )

/****************************************************************************
 * Calculate a new move, returns bool have_move, calculates score in units
 *  of balance*decipawns (white positive) uses recursion
 ****************************************************************************/
#define POS_INFINITY  1000000000
#define NEG_INFINITY -1000000000
#define MAX_DEPTH 30
static Move moves[MAX_DEPTH][MAX_DEPTH];   // Obviously all these disgusting static
static Move pv_array[MAX_DEPTH];           //  arrays mean only once instance of the
static unsigned int pv_count;               //  ChessEngine type can work at once.
static int scores[MAX_DEPTH][MAX_DEPTH];    //  This must be fixed in the future
static int recurse_level;
static int static_balance;
bool ChessEngine::CalculateNextMove( bool &only_move, int &score, Move &move, int balance, int depth )
{
    MOVELIST ml;
    gbl_stop = false;
    only_move = false;
    bool have_move=true;
    if( depth > MAX_DEPTH-10 )
        gbl_depth = MAX_DEPTH-10;
    else
        gbl_depth = depth;
    //Planning();
	GenLegalMoveListSorted( &ml );
    recurse_level  = 0;
    static_balance = balance;
    pv_count = 0;
	int besti;
    if( ml.count == 0 )
    {
        besti = -1;
        score = 0;
    }
    else if( ml.count == 1 )
    {
        // If only one legal move play it
        only_move = true;
        besti = 0;
        PushMove( ml.moves[0] );
		int material, positional;
		EvaluateLeaf(material,positional);
		score = material*static_balance + positional;
        PopMove( ml.moves[0] );
        pv_array[0] = ml.moves[besti];
        pv_count = 1;
    }
    else
        score = Score( ml, besti );

	// Copy best move to caller
	if( besti == -1 )
    {
        have_move = false;
        move.Invalid();
    }
    else if( !only_move )
    {
        move = ml.moves[besti];

	    for( int i=0; i<MAX_DEPTH; i++ )
	    {
		    if( moves[i][0].src == moves[i][0].dst )
			    break;
		    DebugPrintf(( "%d: score %d, %c%c-%c%c\n", i, scores[i][0],
					       FILE(moves[i][0].src),
		                   RANK(moves[i][0].src),
		                   FILE(moves[i][0].dst),
		                   RANK(moves[i][0].dst) ));
	    }
	    DebugPrintf(( "DIAG_make_move_primary=%d\n"
				     "DIAG_evaluate_count=%d\n"
				     "DIAG_evaluate_leaf_count=%d\n"
				     "DIAG_cutoffs=%d\n"
				     "DIAG_deep_cutoffs=%d\n",
				     DIAG_make_move_primary,
				     0,//DIAG_evaluate_count,
				     0,//DIAG_evaluate_leaf_count,
				     DIAG_cutoffs, 
				     DIAG_deep_cutoffs	));
	    for( int i=0; i<MAX_DEPTH; i++ )
	    {
		    if( moves[i][0].src == moves[i][0].dst )
			    break;
            if( i>0 && scores[i][0]!=scores[i-1][0] )
                break;
            pv_array[i] = moves[i][0];
            pv_count++;
        }
    }
	return have_move;
}

/****************************************************************************
 * A version for Multi-PV mode, called repeatedly, removes best move from
 *  movelist each time
 ****************************************************************************/
bool ChessEngine::CalculateNextMove( int &score, Move &move, int balance, int depth, bool first )
{
    bool have_move=true;
	static MOVELIST ml;
    if( first )
    {
        gbl_stop = false;
        if( depth > MAX_DEPTH-10 )
            gbl_depth = MAX_DEPTH-10;
        else
            gbl_depth = depth;
        //Planning();
	    GenLegalMoveListSorted( &ml );
    }
    static_balance = balance;
    recurse_level  = 0;
	int besti;
    if( ml.count == 0 )
    {
        besti = -1;
        score = 0;
    }
    else
        score = Score( ml, besti );

	// Copy best move to caller
    pv_count = 0;
	if( besti == -1 )
    {
        have_move = false;
        move.Invalid();
    }
    else
    {
        move = ml.moves[besti];

        // Remove best move from list for later calls
        for( int i=besti+1; i<ml.count; i++ )
            ml.moves[i-1] = ml.moves[i];
        ml.count--;
	    for( int i=0; i<MAX_DEPTH; i++ )
	    {
		    if( moves[i][0].src == moves[i][0].dst )
			    break;
		    DebugPrintf(( "%d: score %d, %c%c-%c%c\n", i, scores[i][0],
					       FILE(moves[i][0].src),
		                   RANK(moves[i][0].src),
		                   FILE(moves[i][0].dst),
		                   RANK(moves[i][0].dst) ));
	    }
	    DebugPrintf(( "DIAG_make_move_primary=%d\n"
				     "DIAG_evaluate_count=%d\n"
				     "DIAG_evaluate_leaf_count=%d\n"
				     "DIAG_cutoffs=%d\n"
				     "DIAG_deep_cutoffs=%d\n",
				     DIAG_make_move_primary,
				     0,//DIAG_evaluate_count,
				     0,//DIAG_evaluate_leaf_count,
				     DIAG_cutoffs, 
				     DIAG_deep_cutoffs	));
	    for( int i=0; i<MAX_DEPTH; i++ )
	    {
		    if( moves[i][0].src == moves[i][0].dst )
			    break;
            if( i>0 && scores[i][0]!=scores[i-1][0] )
                break;
            pv_array[i] = moves[i][0];
            pv_count++;
        }
    }
	return have_move;
}


/****************************************************************************
 * Internal version for repitition avoidance
 ****************************************************************************/
bool ChessEngine::CalculateNextMove( MOVELIST &ml, bool &only_move, int &score, int &besti, int balance, int depth )
{
    gbl_stop = false;
    only_move = false;
    bool have_move=true;
    if( depth > MAX_DEPTH-10 )
        gbl_depth = MAX_DEPTH-10;
    else
        gbl_depth = depth;
    recurse_level  = 0;
    static_balance = balance;
    pv_count = 0;
    if( ml.count == 0 )
    {
        besti = -1;
        score = 0;
    }
    else if( ml.count == 1 )
    {
        // If only one legal move play it
        only_move = true;
        besti = 0;
        PushMove( ml.moves[0] );
		int material, positional;
		EvaluateLeaf(material,positional);
		score = material*static_balance + positional;
        PopMove( ml.moves[0] );
        pv_array[0] = ml.moves[0];
        pv_count = 1;
    }
    else
        score = Score( ml, besti );

	// Copy best move to caller
	if( besti == -1 )
        have_move = false;
    else if( !only_move )
    {
	    for( int i=0; i<MAX_DEPTH; i++ )
	    {
		    if( moves[i][0].src == moves[i][0].dst )
			    break;
		    DebugPrintf(( "%d: score %d, %c%c-%c%c\n", i, scores[i][0],
					       FILE(moves[i][0].src),
		                   RANK(moves[i][0].src),
		                   FILE(moves[i][0].dst),
		                   RANK(moves[i][0].dst) ));
	    }
	    DebugPrintf(( "DIAG_make_move_primary=%d\n"
				     "DIAG_evaluate_count=%d\n"
				     "DIAG_evaluate_leaf_count=%d\n"
				     "DIAG_cutoffs=%d\n"
				     "DIAG_deep_cutoffs=%d\n",
				     DIAG_make_move_primary,
				     0,//DIAG_evaluate_count,
				     0,//DIAG_evaluate_leaf_count,
				     DIAG_cutoffs, 
				     DIAG_deep_cutoffs	));
	    for( int i=0; i<MAX_DEPTH; i++ )
	    {
		    if( moves[i][0].src == moves[i][0].dst )
			    break;
            if( i>0 && scores[i][0]!=scores[i-1][0] )
                break;
            pv_array[i] = moves[i][0];
            pv_count++;
        }
    }
	return have_move;
}

/****************************************************************************
 * Public interface to version for repitition avoidance
 ****************************************************************************/
extern void ReportOnProgress
(
    bool    init,
    int     multipv,
    vector<Move> &pv,
    int     score_cp,
    int     depth
);

bool ChessEngine::CalculateNextMove( bool new_game, vector<Move> &pv, Move &bestmove, int &score_cp,
                            unsigned long ms_time,
                            unsigned long ms_budget,
                            int balance,
                            int &depth )
{
    int besti=0;
    score_cp=0;
    depth=0;
    unsigned long elapsed_time;
    unsigned long base_time = GetTickCount();	
    MOVELIST ml;
    //Planning();
	GenLegalMoveListSorted( &ml );
    bool have_move = false;
    bool only_move = false;
    int score=0;
    unsigned long previous_elapsed=0;
    static bool winning_ring[2];
    static bool losing_ring[2];
    static int ring_idx;
    static int killing;
    const int initial_kill_threshold = 800;
    const int bump_kill_threshold = 10; // must show trend else stop chopping search
    static int multiplier[30];

    DebugPrintfInner( "CNM: new_game = %s\n", new_game?"true":"false" );
    if( new_game )
    {
        losing_ring[0]  = losing_ring[1]  =  false;
        winning_ring[0] = winning_ring[1] =  false;
        ring_idx = 0;
        killing = initial_kill_threshold;
        for( int i=0; i<30; i++ )
            multiplier[i] = 0;
    }

    for( depth=1; depth<20; depth++ ) // depth+=2 )
    {    
        have_move = CalculateNextMove( ml, only_move, score, besti, balance, depth );
        if( have_move )
        {
            score_cp = (score*10)/balance;  // convert to centipawns
            GetPV( pv );
            ReportOnProgress
            (
                false,
                1,
                pv,
                score_cp,
                depth
            );
        }

        unsigned long now_time = GetTickCount();	
        elapsed_time = now_time-base_time;
        DebugPrintfInner( "CNM: elapsed_time=%u, previous_elapsed=%u\n", elapsed_time, previous_elapsed );
        if( depth && previous_elapsed )
            multiplier[depth-1] =  elapsed_time/previous_elapsed;
        if( !have_move )
            DebugPrintfInner( "No move\n" );
        else
        {
            std::string s = ml.moves[besti].NaturalOut(this);
            DebugPrintfInner( "Depth:%d Move:%s Score:%d Elapsed time:%lu Budget time:%lu Maximum time:%lu\n",
                depth, s.c_str(), (score*10)/balance, elapsed_time, ms_budget, ms_time );
        }
        unsigned long budget_threshold    = ms_budget/2;        
        unsigned long inc_depth_threshold = ms_time/10;
        DebugPrintfInner( "CNM: [%d] elapsed_time=%lu, budget_threshold=%lu, inc_depth_threshold=%lu\n",
                                depth, elapsed_time, budget_threshold, inc_depth_threshold );
        unsigned long predicted_time_1=0;
        if( elapsed_time && previous_elapsed )
            predicted_time_1 = elapsed_time * (elapsed_time/previous_elapsed);
        unsigned long predicted_time_2=0;
        if( elapsed_time && multiplier[depth] )
            predicted_time_2 = elapsed_time * multiplier[depth];
        DebugPrintfInner( "CNM: [%d] predicted_time (based on this position only)=%lu\n",
                            depth, predicted_time_1 );
        DebugPrintfInner( "CNM: [%d] predicted_time (based on previous positions)=%lu\n",
                            depth, predicted_time_2 );
        if( only_move || !have_move )
        {
            DebugPrintfInner( "CNM: stop[%d] because only move or no move\n", depth );
            break;      // stop thinking if zero or one moves
        }
        if( score_cp<-30000 || score_cp>30000 )
        {
            DebugPrintfInner( "CNM: stop[%d] because mate detected\n", depth );
            break;      // stop thinking if mate anyway
        }
        if( depth>=5 && winning_ring[0] && winning_ring[1] && (score_cp>killing) )
        {
            DebugPrintfInner( "CNM: stop[%d] because winning score_cp=%d\n", depth, score_cp );
            break;      // stop thinking if it's going very well/very_poorly
        }
        if( depth>=5 && losing_ring[0] && losing_ring[1] && (score_cp<killing) )
        {
            DebugPrintfInner( "CNM: stop[%d] because losing score_cp=%d\n", depth, score_cp );
            break;      // stop thinking if it's going very well/very_poorly
        }
        if( elapsed_time > budget_threshold )
        {
            DebugPrintfInner( "CNM: stop[%d] because budget exceeded\n", depth );
            break;
        }
        if( predicted_time_1 > inc_depth_threshold )
        {
            DebugPrintfInner( "CNM: stop[%d] because predicted_time (based on this position only) > threshold\n", depth );
            break;  // stop thinking if we estimate we're going to use too much time
        }
        if( predicted_time_2 > inc_depth_threshold )
        {
            DebugPrintfInner( "CNM: stop[%d] because predicted_time (based on previous positions) > threshold\n", depth );
            break;  // stop thinking if we estimate we're going to use too much time
        }
        previous_elapsed = elapsed_time;
    }
    int dont_show_lower_depth = depth;
    GetPV( pv );
    if( have_move )
    {

        // Save the best line in case we can't find a non-repetition that's
        //  not better for us
        vector<Move> save_best_line = pv;
        
        // Convert to centipawns
        score_cp = (score*10)/balance;
        if( score_cp > 30000 )
            score_cp = 30000 + (score_cp-30000)/10000;
        else if( score < -30000 )
            score_cp = -30000 + (score_cp+30000)/10000;
        for( int candidate=0; candidate<6; candidate++ )
        {
            if( only_move )
                break;
            #define REPITITION_AVOID_THRESHOLD 0  /*50   0.5 pawns */
            bool we_are_better = (WhiteToPlay() ? score_cp>REPITITION_AVOID_THRESHOLD : score_cp<-REPITITION_AVOID_THRESHOLD );
            if( !we_are_better && candidate>0 )
            {
                // Revert
                pv = save_best_line;
                ReportOnProgress
                (
                    false,
                    1,
                    pv,
                    score_cp,
                    dont_show_lower_depth
                );
            }
            if( !we_are_better )
                break;

            // If we are better, test whether the current best move will repeat
            static Move save_history[256];          // must be 256 ..
            unsigned char save_history_idx;  // .. so this can loop around
            static DETAIL save_detail_stack[256];   // must be 256 ..
            unsigned char save_detail_idx;   // .. so this can loop around
            memcpy(save_history,history,sizeof(history));
            memcpy(save_detail_stack,detail_stack,sizeof(detail_stack));
            save_history_idx = history_idx;
            save_detail_idx  = detail_idx;
            ChessPosition save = *this;
            Move move = ml.moves[besti];
            PlayMove( move );
            bool repitition = IsRepitition();
            *this = save;
            memcpy(detail_stack,save_detail_stack,sizeof(detail_stack));
            memcpy(history,save_history,sizeof(history));
            history_idx = save_history_idx;
            detail_idx = save_detail_idx;

            // If it won't no problem
            if( !repitition )
                break;

            // Remove best move from list
            DebugPrintfInner( "Removing %s (score %d) because of repetition\n", move.TerseOut().c_str(), score_cp );
            for( int i=besti+1; i<ml.count; i++ )
                ml.moves[i-1] = ml.moves[i];
            ml.count--;
            if( ml.count < 1 )
                break;

            // Try again with a different candidate move
            if( candidate == 0 )
                depth--;    // multiple candidates, so must use less deep calculation
            bool only_move_unused;
            if( !CalculateNextMove( ml,only_move_unused,score,besti,balance,depth ) )
                break;
            score_cp = (score*10)/balance;  // convert to centipawns
            if( score_cp > 30000 )
                score_cp = 30000 + (score_cp-30000)/10000;
            else if( score < -30000 )
                score_cp = -30000 + (score_cp+30000)/10000;
            std::string nmove;
            move = ml.moves[besti];
            nmove = move.NaturalOut( this );
            DebugPrintfInner( "Repetition attempt; Depth:%d Move:%s Score:%d\n",
                depth, nmove.c_str(), (score*10)/balance );
            GetPV( pv );
            ReportOnProgress
            (
                false,
                1,
                pv,
                score_cp,
                dont_show_lower_depth
            );
        }
    }
    if( have_move )
        bestmove = pv[0];
    winning_ring[ring_idx] = (score_cp>killing);
    losing_ring[ring_idx]  = (score_cp<0-killing);
    ring_idx++;
    ring_idx &= 1;
    if( winning_ring[0] && winning_ring[1] )
        killing += bump_kill_threshold;
    else if( losing_ring[0] && losing_ring[1] )
        killing += bump_kill_threshold;
    else
        killing = initial_kill_threshold;
    DebugPrintfInner( "CNM: winning_ring[0]=%s\n", winning_ring[0]?"true":"false" );
    DebugPrintfInner( "CNM: winning_ring[1]=%s\n", winning_ring[1]?"true":"false" );
    DebugPrintfInner( "CNM: losing_ring[0]=%s\n",  losing_ring[0]?"true":"false" );
    DebugPrintfInner( "CNM: losing_ring[1]=%s\n",  losing_ring[1]?"true":"false" );
    DebugPrintfInner( "CNM: killing=%d\n",         killing );
    if( have_move )
        DebugPrintfInner( "CNM: bestmove=%s\n",    bestmove.TerseOut().c_str() );
    return have_move;
}

/****************************************************************************
 * Has this position occurred before ?
 ****************************************************************************/
bool ChessEngine::IsRepitition()
{
    bool repitition=false;

    //  Save those aspects of current position that are changed by multiple 
    //  PopMove() calls as we search backwards (i.e. squares, white,
    //  detail, detail_idx)
    bool save_white = white;
    char save_squares[sizeof(squares)];
    memcpy( save_squares, squares, sizeof(save_squares) );
    unsigned char save_detail_idx = detail_idx;  // must be unsigned char
    unsigned char idx             = history_idx; // must be unsigned char
    DETAIL_SAVE;

    // Search backwards ....
    int nbr_half_moves = (full_move_count-1)*2 + (!white?1:0);
    if( nbr_half_moves > nbrof(history)-1 )
        nbr_half_moves = nbrof(history)-1;
    if( nbr_half_moves > nbrof(detail_stack)-1 )
        nbr_half_moves = nbrof(detail_stack)-1;
    for( int i=0; i<nbr_half_moves; i++ )
    {
        Move m = history[--idx];
        if( m.src == m.dst )
            break;  // clearing history prevents bogus repitition draws
        PopMove(m);

        // ... looking for matching positions
        if( white    == save_white      && // quick ones first!
            DETAIL_EQ                   &&
            0 == memcmp(squares,save_squares,sizeof(squares) )
          )
        {
            repitition = true;
            break;
        }

        // For performance reasons, abandon search early if pawn move
        //  or capture
        if( squares[m.src]=='P' || squares[m.src]=='p'  || !IsEmptySquare(m.capture) )
            break;
    }

    // Restore current position
    memcpy( squares, save_squares, sizeof(squares) );
    detail_idx = save_detail_idx;
    DETAIL_RESTORE;
    white      = save_white;
    return repitition;
}

/****************************************************************************
 * Retrieve PV (primary variation?), call after CalculateNextMove()
 ****************************************************************************/
void ChessEngine::GetPV( vector<Move> &pv )
{
    pv.clear();
	for( unsigned int i=0; i<pv_count; i++ )
        pv.push_back( pv_array[i] );
}


#ifdef ALPHA_BETA
int alpha[MAX_DEPTH];   // Todo get rid of all the crazy static vars!!
int beta[MAX_DEPTH];
#endif
/****************************************************************************
 * Score a position
 ****************************************************************************/
int ChessEngine::Score( MOVELIST &ml, int &besti )
{
    int score;
	#ifdef ALPHA_BETA
    for( int i=0; i<MAX_DEPTH; i++ )
    {
        alpha[i] = NEG_INFINITY;    // best white score to date
        beta[i]  = POS_INFINITY;    // best black score to date
    }
	#endif
    if( white )
        score = ScoreWhiteToMove( ml, besti, 0 );
    else
        score = ScoreBlackToMove( ml, besti, 0 );
    return( score );
}

const char *indent( int recurse_level )
{
    static const char *buf = "                                               ";
    int nbr_spaces = recurse_level*2;
    int len = (int)strlen(buf);
    int offset = len>=nbr_spaces ? len-nbr_spaces : 0;
    return buf + offset;
}

#ifdef EXTRA_DEBUG_CODE1
static unsigned long tag_generator;
#endif

int ChessEngine::ScoreWhiteToMove( MOVELIST &ml, int &besti, int black_mobility )
{
    int white_mobility = ml.count;
    bool prune=false;
    TERMINAL score_terminal;
    MOVELIST ml2;
	int i, score=0, temp;
    int max  = NEG_INFINITY;
    besti = -1;
    recurse_level++;
	#ifdef ALPHA_BETA
    alpha[recurse_level] =  NEG_INFINITY;    // best white score to date
    beta[recurse_level]  =  POS_INFINITY;    // best black score to date
	#endif
    #ifdef EXTRA_DEBUG_CODE1
    unsigned long tag = tag_generator++;
    if( recurse_level < LEVEL_CAREFUL_SORTING )
    {
        DebugPrintf(( "%sScoreWhiteToMove() [%lu], sorted [", indent(recurse_level), tag ));
        CarefulSort( ml );
    }
    else
        DebugPrintf(( "%sScoreWhiteToMove() [%lu], not sorted [", indent(recurse_level), tag ));
	for( i=0; i<ml.count; i++  )
	{
        Move move;
        move = ml.moves[i];
        std::string nmove;
        nmove = move.NaturalOut( this );
        DebugPrintf(( " %s", nmove.c_str() ));
    }
    DebugPrintf(( "]\n" ));
    #else
    if( recurse_level < LEVEL_CAREFUL_SORTING )
        CarefulSort( ml );
    #endif
	for( i=0; !prune && i<ml.count; i++  )
	{
        #ifdef EXTRA_DEBUG_CODE1
        Move move;
        move = ml.moves[i];
        std::string nmove;
        nmove = move.NaturalOut( this );
        DebugPrintf(( "%sScoreWhiteToMove() [%lu], playing .%s\n", indent(recurse_level), tag, nmove.c_str() ));
        #endif
		PushMove( ml.moves[i] );
		DIAG_make_move_primary++;	
		//if( (recurse_level>=7) || (recurse_level>4 && i>=ml.scount)  )
		//if( (recurse_level>=8) || (recurse_level>4 && i>=ml.scount && !AttackedPiece(bking)) )
        IF_STOP_RECURSING
		{
            #ifdef VARIABLE_PLY
            if( gbl_stop )
                DebugPrintf(("Stop command received\n" ));
            #endif
			int material, positional;
			EvaluateLeaf(material,positional);
			score = material*static_balance + positional + (white_mobility-black_mobility)/4;
            #ifdef EXTRA_DEBUG_CODE1
            DebugPrintf(( "%s [%lu] Leaf score: score=%d: (material=%d, positional=%d, white_mobility=%d, black_mobility=%d)\n",
                            indent(recurse_level), tag, score, material, positional, white_mobility, black_mobility ));
            #endif
        }
		else
		{
			bool okay = Evaluate(&ml2,score_terminal);
			//bool okay = Evaluate(recurse_level>=LEVEL_CAREFUL_SORTING-1,&ml2,score_terminal);
			//bool okay = Evaluate(recurse_level<LEVEL_STOP_SORTING,&ml2,score_terminal);
			if( !okay )
				score = NEG_INFINITY;
			else
			{
				switch( score_terminal )
    			{
	    			case TERMINAL_WCHECKMATE: score=-10000*(MAX_DEPTH-recurse_level);       break;
		    		case TERMINAL_BCHECKMATE: score=10000*(MAX_DEPTH-recurse_level);        break;
					case TERMINAL_WSTALEMATE: score=0;										break;
    				case TERMINAL_BSTALEMATE: score=0;										break;
	    			case 0: 				  score = ScoreBlackToMove( ml2, temp, white_mobility );		break;
				}
			}
            #ifdef EXTRA_DEBUG_CODE1
            DebugPrintf(( "%s [%lu] Recursion score: score=%d\n",
                            indent(recurse_level), tag, score ));
            #endif
		}
        if( score > max )
        {
            #ifdef EXTRA_DEBUG_CODE1
            DebugPrintf(( "%s [%lu] New max: score=%d, previous max=%d\n",
                            indent(recurse_level), tag, score, max ));
            #endif
			#ifdef ALPHA_BETA
            for( int j=recurse_level-1; j>=0 ; j-=2 )
            {   
                if( score > beta[j] )
                {
                    prune = true;
					DIAG_cutoffs++;
                    #ifdef EXTRA_DEBUG_CODE1
                    DebugPrintf(( "%s [%lu] Beta %s: score=%d, beta[%d] = %d\n",
                                    indent(recurse_level), tag, j!=recurse_level-1?"deep prune":"prune",
                                    score, j, beta[j] ));
                    #endif
					if( j != recurse_level-1 )
						DIAG_deep_cutoffs++;
                    break;
                }
            }
            if( score > alpha[recurse_level] )
            {
                #ifdef EXTRA_DEBUG_CODE1
                DebugPrintf(( "%s [%lu] Alpha update: score=%d > old alpha[%d] = %d\n",
                                indent(recurse_level), tag,
                                score, recurse_level, alpha[recurse_level] ));
                #endif
                alpha[recurse_level] = score;
            }
			#endif
            max   = score;
            besti = i;
			{
				int l = recurse_level-1;
				scores[l][l] = max;
				moves [l][l] = ml.moves[besti];
				for( int j=l+1; j<MAX_DEPTH; j++ )
				{
					scores[j][l] = scores[j][l+1];
					moves [j][l] = moves [j][l+1];
				}
			}
        }
	 	PopMove( ml.moves[i] );
    }
    recurse_level--;
    return( max );
}

struct MOVE_SORT_NODE
{
    Move *ptr;
    int   score;
};


int comp_white( const void *p1, const void *p2 )
{
    int i1 = ((MOVE_SORT_NODE *)p1)->score;
    int i2 = ((MOVE_SORT_NODE *)p2)->score;
    int ret = 0;
    if( i1 < i2 )
        ret = 1;
    else if( i1 > i2 )
        ret = -1;
    return ret;
}

int comp_black( const void *p1, const void *p2 )
{
    int i1 = ((MOVE_SORT_NODE *)p1)->score;
    int i2 = ((MOVE_SORT_NODE *)p2)->score;
    int ret = 0;
    if( i1 < i2 )
        ret = -1;
    else if( i1 > i2 )
        ret = 1;
    return ret;
}

void ChessEngine::CarefulSort( MOVELIST &ml )
{
    MOVELIST out;
    int i;
    MOVE_SORT_NODE buf[MAXMOVES];
    for( i=0; i<ml.count; i++ )
    {
        PushMove( ml.moves[i] );
   		int material, positional;
		EvaluateLeaf(material,positional);
        buf[i].score = material*static_balance + positional;
        buf[i].ptr   = &ml.moves[i];
        PopMove( ml.moves[i] );
    }
    qsort( buf, ml.count, sizeof(MOVE_SORT_NODE), white?comp_white:comp_black );
    for( i=0; i<ml.count; i++ )
        out.moves[i] = *buf[i].ptr;
    for( i=0; i<ml.count; i++ )
        ml.moves[i] = out.moves[i];
    #ifdef EXTRA_DEBUG_CODE3
    DebugPrintf(( "Sorting position with %s to move;\n%s\n", white?"white":"black", squares ));
    for( i=0; i<ml.count; i++ )
    {
        Move move;
        move = ml.moves[i];
        std::string nmove;
        nmove = move.NaturalOut( this );
        DebugPrintf(( " %s", nmove.c_str() ));
    }
    #endif
}

int ChessEngine::ScoreBlackToMove( MOVELIST &ml, int &besti, int white_mobility )
{
    int black_mobility = ml.count;
    TERMINAL score_terminal;
    bool prune=false;
    MOVELIST ml2;
	int i, score=0, temp;
    int min = POS_INFINITY;
    besti = -1;
    recurse_level++;
	#ifdef ALPHA_BETA
    alpha[recurse_level] =  NEG_INFINITY;    // best white score to date
    beta[recurse_level]  =  POS_INFINITY;    // best black score to date
	#endif
    #ifdef EXTRA_DEBUG_CODE1
    unsigned long tag = tag_generator++;
    if( recurse_level < LEVEL_CAREFUL_SORTING )
    {
        DebugPrintf(( "%sScoreBlackToMove() [%lu], sorted [", indent(recurse_level), tag ));
        CarefulSort( ml );
    }
    else
        DebugPrintf(( "%sScoreBlackToMove() [%lu], not sorted [", indent(recurse_level), tag ));
	for( i=0; i<ml.count; i++  )
	{
        Move move;
        move = ml.moves[i];
        std::string nmove;
        nmove = move.NaturalOut( this );
        DebugPrintf(( " %s", nmove.c_str() ));
    }
    DebugPrintf(( "]\n" ));
    #else
    if( recurse_level < LEVEL_CAREFUL_SORTING )
        CarefulSort( ml );
    #endif
	for( i=0; !prune && i<ml.count; i++  )
	{
        #ifdef EXTRA_DEBUG_CODE1
        Move move;
        move = ml.moves[i];
        std::string nmove;
        nmove = move.NaturalOut( this );
        DebugPrintf(( "%sScoreBlackToMove() [%lu], playing %s\n", indent(recurse_level), tag, nmove.c_str() ));
        #endif
		PushMove( ml.moves[i] );
		DIAG_make_move_primary++;	
		//if( (recurse_level>=7) || (recurse_level>4 && i>=ml.scount)  )
		//if( (recurse_level>=8) || (recurse_level>4 && i>=ml.scount && !AttackedPiece(wking_square)) )
        IF_STOP_RECURSING
		{
            #ifdef VARIABLE_PLY
            if( gbl_stop )
                DebugPrintf(("Stop command received\n" ));
            #endif
			int material, positional;
			EvaluateLeaf(material,positional);
			score = material*static_balance + positional + (white_mobility-black_mobility)/4;
            #ifdef EXTRA_DEBUG_CODE1
            DebugPrintf(( "%s [%lu] Leaf score: score=%d: (material=%d, positional=%d, white_mobility=%d, black_mobility=%d)\n",
                            indent(recurse_level), tag, score, material, positional, white_mobility, black_mobility ));
            #endif
        }
		else
		{
			bool okay = Evaluate(&ml2,score_terminal);
			//bool okay = Evaluate(recurse_level>=LEVEL_CAREFUL_SORTING-1,&ml2,score_terminal);
			//bool okay = Evaluate(recurse_level<LEVEL_STOP_SORTING,&ml2,score_terminal);
			if( !okay )
				score = POS_INFINITY;
			else
			{
				switch( score_terminal )
    			{
	    			case TERMINAL_WCHECKMATE: score=-10000*(20-recurse_level);				break;
		    		case TERMINAL_BCHECKMATE: score=10000*(20-recurse_level);				break;
					case TERMINAL_WSTALEMATE: score=0;										break;
    				case TERMINAL_BSTALEMATE: score=0;										break;
	    			case 0: 				  score = ScoreWhiteToMove( ml2, temp, black_mobility );		break;
				}
			}
            #ifdef EXTRA_DEBUG_CODE1
            DebugPrintf(( "%s [%lu] Recursion score: score=%d\n",
                            indent(recurse_level), tag, score ));
            #endif
		}
        if( score < min )
        {
            #ifdef EXTRA_DEBUG_CODE1
            DebugPrintf(( "%s [%lu] New min: score=%d, previous min=%d\n",
                            indent(recurse_level), tag, score, min ));
            #endif
			#ifdef ALPHA_BETA
            for( int j=recurse_level-1; j>=0 ; j-=2 )
            {
                if( score < alpha[j] )
                {
                    prune = true;
					DIAG_cutoffs++;
                    #ifdef EXTRA_DEBUG_CODE1
                    DebugPrintf(( "%s [%lu] Alpha %s: score=%d, alpha[%d] = %d\n",
                                    indent(recurse_level), tag, j!=recurse_level-1?"deep prune":"prune",
                                    score, j, alpha[j] ));
                    #endif
					if( j != recurse_level-1 )
						DIAG_deep_cutoffs++;
                    break;
                }
            }
            if( score < beta[recurse_level] )
            {
                #ifdef EXTRA_DEBUG_CODE1
                DebugPrintf(( "%s [%lu] Beta update: score=%d < old beta[%d] = %d\n",
                                indent(recurse_level), tag,
                                score, recurse_level, beta[recurse_level] ));
                #endif
                beta[recurse_level] = score;
            }
			#endif
            min   = score;
            besti = i;
			{
				int l = recurse_level-1;
				scores[l][l] = min;
				moves [l][l] = ml.moves[besti];
				for( int j=l+1; j<MAX_DEPTH; j++ )
				{
					scores[j][l] = scores[j][l+1];
					moves [j][l] = moves [j][l+1];
				}
			}
        }
	 	PopMove( ml.moves[i] );
    }
    recurse_level--;
    return( min );
}

//==================== TESTING

// Some results 4-Nov-2008 (elapsed time in millisecs)
//   Minimax, no alpha-beta            4319860, 4268234
//   Alpha-beta but no strong moves    84950            (50 times faster)
//   Alpha-beta + strong moves         12547            (344 times faster)
//   (made EnpriseWhite(), EnpriseBlack) - 11984
void ChessEngine::TestGame()
{
    int idx=0;
    bool okay;
    std::string nmove;
    Move move;
    const char *s, *txt;
    const char *moves[] =
    {
        "B Nf3",      "H e5",
        "E Nxe5",     "H Nc6",
        "E d4",       "H Bb4+",
        "E c3",       "H Ba5",
        "E b4",       "H Bb6",
        "E e4",       "H f6",
        "E Qh5+",     "H g6",
        "E Nxg6",     "H d5",
        "E Nxh8+",    "H Kd7",
        "E Qxd5+",    "H Ke7",
        "E Qf7+",     "H Kd6",
        "E Bf4+",     "H Ne5",
        "E Qd5+",     "H Ke7",
        "E Qxd8+",    "H Kxd8",
        "E dxe5",     "H a5",
        "E Bc4",      "H a4",
        "E Bxg8",     "H a3",
        "E exf6",     "H Rb8",
        "E Bxh7",     "H Ra8",
        "E Bc1",      "H Rb8",
        "E Nxa3",     "H Ra8",
        "E e5",       "H Rb8",
        "E f7",       "H Ra8",
        "E f8=Q+",    "H Kd7",
        "E Qf7+",     "H Kc6",
        "E Be4#", 
        NULL
    };
    ChessEngine temp;
    *this = temp;   // init
    bool complete = false;
    unsigned long before = GetTickCount();
    for(;;)
    {
        txt = moves[idx++];
        if( txt == NULL )
        {
            complete = true;
            break;
        }
        char typ = *txt;
        txt += 2;
        if( typ=='B' || typ=='H' )  // book or human move
        {
            bool okay = move.NaturalIn( this, txt );
            if( !okay )
            {
                printf( "Couldn't convert nmove=%s\n", txt );
                break;
            }
            printf( "Input: %s\n", txt );
            PlayMove( move );
        }
        else if( typ == 'E' ) // engine move
        {
            #define BALANCE 4
            int score;
            bool only_move;
            CalculateNextMove( only_move, score, move, BALANCE, DEFAULT_DEPTH );
            nmove = move.NaturalOut( this );
            printf( "Output: %s\n", nmove.c_str() );
            s = strstr(txt,nmove.c_str());
            okay  = false;
            if( s != NULL )
            {
                if( s==txt || *(s-1)==' ' )
                {
                    s += strlen(nmove.c_str());
                    if( *s=='\0' || *s==' ' )
                        okay = true; // s points at complete nmove within move
                }    
            }
            if( !okay )
            {
                printf( "Convert bestmove=%s->nmove=%s, doesn't match %s\n", move.TerseOut().c_str(), nmove.c_str(), txt );
                break;
            }
            PlayMove( move );
        }
    }
    unsigned long after = GetTickCount();
    if( complete )
        printf( "Success, elapsed time = %lu\n", after-before );
    else
        printf( "Didn't complete\n" );
}

void ChessEngine::TestInternals()
{
    const char *fen = "b3k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
    Move move;
    Forsyth(fen);
    printf( "Addresses etc.;\n" );
    printf( " this = 0x%p\n",                         this );
    printf( " &white = 0x%p\n",                       &white );
    printf( " &squares[0] = 0x%p\n",                  &squares[0] );
    printf( " &half_move_clock = 0x%p\n",             &half_move_clock );
    printf( " &full_move_count = 0x%p\n",             &full_move_count );
    printf( " size to end of full_move_count = 0x%x\n", (unsigned int)( ((char *)&full_move_count - (char *)this) + sizeof(full_move_count) )  );
    printf( " sizeof(ChessPosition) = 0x%x (should be 4 more than size to end of full_move_count)\n",
                                                        (unsigned int)sizeof(ChessPosition) );
    printf( " sizeof(Move) = 0x%x\n",                  (unsigned int)sizeof(Move) );
    for( int i=0; i<6; i++ )
    {
        switch(i)
        {
            case 0: move.TerseIn(this,"h1h2");    break;
            case 1: move.TerseIn(this,"a8h1");    break;
            case 2: move.TerseIn(this,"e1c1");    break;
            case 3: move.TerseIn(this,"h1a8");    break;
            case 4: move.TerseIn(this,"c1b1");    break;
            case 5: move.TerseIn(this,"e8g8");    break;
        }
        unsigned char *p = (unsigned char *)DETAIL_ADDR;
        printf( " DETAIL_ADDR = 0x%p\n",  p );
        printf( " DETAIL_ADDR[0] = %02x\n",  p[0] );
        printf( " DETAIL_ADDR[1] = %02x\n",  p[1] );
        printf( " DETAIL_ADDR[2] = %02x\n",  p[2] );
        printf( " DETAIL_ADDR[3] = %02x\n",  p[3] );
        printf( "Before %s: enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
                " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
            move.TerseOut().c_str(),
            enpassant_target,
            wking_square,
            bking_square,
            wking ?"true":"false",
            wqueen?"true":"false",
            bking ?"true":"false",
            bqueen?"true":"false" );
        PushMove(move);        
        printf( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
                " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
            enpassant_target,
            wking_square,
            bking_square,
            wking ?"true":"false",
            wqueen?"true":"false",
            bking ?"true":"false",
            bqueen?"true":"false" );
        PopMove(move);        
        printf( "After PopMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
                " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
            enpassant_target,
            wking_square,
            bking_square,
            wking ?"true":"false",
            wqueen?"true":"false",
            bking ?"true":"false",
            bqueen?"true":"false" );
        PushMove(move);        
        printf( "After PushMove(): enpassant_target=0x%02x, wking_square=0x%02x, bking_square=0x%02x,"
                " wking=%s, wqueen=%s, bking=%s, bqueen=%s\n",
            enpassant_target,
            wking_square,
            bking_square,
            wking ?"true":"false",
            wqueen?"true":"false",
            bking ?"true":"false",
            bqueen?"true":"false" );
    }
}

void ChessEngine::TestPosition()
{    
    #define BALANCE 4
    // Fischer-Leopoldi Western Open 1963 - White has a combo winning a rook
//  const char *fen = "2bqkb1r/N4p2/r2p3p/p2Np2n/1p2P3/1P1QBP2/1PP5/1K1R2R1 w k - 0 25";

    // Mate in 2
//  const char *fen = "5k2/pp3pnR/q3b1p1/6P1/5P2/1NQ5/rPP3P1/1KR5 b - - 0 1";

    // Close to simplest possible situation
//  const char *fen = "8/8/6k1/PK6/8/8/8/8 w - - 0 0";

    // Saavedra stalemate
//  const char *fen = "8/2P5/8/8/3r4/8/2K5/k7 w - - 0 0 moves c7c8q";   //  bestmove d4c4

    // Promote a pawn
//  const char *fen = "8/8/6k1/PK6/8/8/8/8 w - - 0 0";  //  bestmove a5a6

    // Mate in 2 in a middlegame
//  const char *fen = "3QN2k\\5prr\\1pp3p1\\7p\\2PPPP1P\\6P1\\pq6\\2R2RK1 w - - 10 39"; //  bestmove e8f6

    // Closer to simplest possible situation ? (mate in 1 by grabbing queen)
//    const char *fen = "K2N2nk/5qpp/8/8/8/8/8/8 w - - 0 1";

    // A troublesome position
    const char *fen = "1K6/1P6/8/q7/5k2/8/8/8 b - - 0 1";
    Forsyth( fen );
    int material, positional;
    Planning();
    EvaluateLeaf(material,positional);
    Move move;
    int score;
    bool only_move;
    MOVELIST ml;
    GenLegalMoveListSorted( &ml );
    int besti;
    CalculateNextMove( ml, only_move, score, besti, BALANCE, 7 );
    move = ml.moves[besti];
    std::string nmove = move.NaturalOut( this );
    printf( "%s", ToDebugStr().c_str() );
    printf( "score=%d, material=%d, positional=%d, move=%s\n", score, material, positional, nmove.c_str() );
    #if 0   // enable this to test transformed position
    Transform();
    EvaluateLeaf(material,positional);
    CalculateNextMove( only_move, score, move, BALANCE, DEFAULT_DEPTH );
    std::string nmove = move.NaturalOut(this);
    DebugPrintf(( "AFTER Transform(), %s", ToDebugStr().c_str() ));
    DebugPrintf(( "score=%d, material=%d, positional=%d, move=%s\n", score, material, positional, nmove.c_str() ));
    printf( "AFTER Transform(), %s", ToDebugStr().c_str() );
    printf( "score=%d, material=%d, positional=%d, move=%s\n", score, material, positional, nmove.c_str() );
    #endif
}

void ChessEngine::TestEnprise()
{
    // A position where white attacks a black knight 7 times and black defends it
    //  6 times
    //const char *pos = "3q3k/2n5/2prp3/3n2r1/2P1P3/3RN3/B2R4/3Q3K w - - 0 1";

    // Black to play can win material
    //const char *pos = "2r3k1/2r2b2/8/8/2N5/8/2R5/2R3K1 b - - 0 1";

    // Black to play can wins R+R+N for Q+B
    const char *pos = "2r3k1/2r2b2/2q5/8/2N5/8/2R5/2R3K1 b - - 0 1";
    bool okay = Forsyth( pos );
    if( okay )
    {
        int m = (WhiteToPlay() ? EnpriseWhite() : EnpriseBlack());
        printf( "ChessPosition: %s\n", pos );
        printf( "Side to move can win %d centipawns of material in this position\n", m*10 );
    }
}

void ChessEngine::Test()
{    
    TestInternals();
    TestGame();
    //TestPosition();
    //TestEnprise();
    printf( "Press Enter\n" );
    getchar();
}
