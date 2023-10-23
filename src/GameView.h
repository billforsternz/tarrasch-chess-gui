/****************************************************************************
 * A complete view of a game's variations, built from a MoveTree
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GAME_VIEW_H
#define GAME_VIEW_H
#include <string>
#include <vector>
#include "Objects.h"
#include "GameTree.h"
#include "GameViewElement.h"
#include "thc.h"
#include "NavigationKey.h"
#include "wx/richtext/richtextctrl.h"

class GameLogic;
class GameView
{
public:
    GameView( GameLogic *gl ) { this->gl = gl; }
    GameView() { this->gl = objs.gl; }
    GameLogic *gl;
    bool comment_edited;    // sorry very disgusting
    int  puzzle_nbr;        // see above
    void Build_bc( std::string &result, GameTree &tree_bc, thc::ChessPosition &start_position );
    void ToString( std::string &str );
    void ToString( std::string &str, int begin, int end );
    static const int SKIP_TO_FIRST_DIAGRAM=1;
    static const int SUPPRESS_NULL_MOVE=2;
    static const int SUPPRESS_VARIATION_PARENS=4;
    static const int SUPPRESS_MOVE=8;
    bool ShouldDiagGoHere( int here, int end, int indent );
    void ToPublishString( std::string &str, int &diagram_base, int &mv_base, int &neg_base, int publish_options  );
    void ToPublishString( std::string &str, int &diagram_base, int &mv_base, int &neg_base, int publish_options, int begin, int end );
    void ToPublishDiagram( std::string &str, thc::ChessPosition &cp );
    void ToCommentString( std::string &str );
    void ToCommentString( std::string &str, int begin, int end );
    int  GetInternalOffset_bc( int view_offset );
    int  GetInternalOffsetEndOfVariation( int start );

    void Display( unsigned long pos );
    bool GetOffsetWithinComment( unsigned long pos, unsigned long &pos_within_comment );
    bool CommentEdit( wxRichTextCtrl *ctrl, std::string &txt_to_insert, long keycode=0, bool *pass_thru_edit_ptr=NULL );
    bool IsSelectionInComment( wxRichTextCtrl *ctrl );
    bool IsInComment( wxRichTextCtrl *ctrl );
    void DeleteSelection( wxRichTextCtrl *ctrl );

    unsigned long NavigationKey( unsigned long pos, NAVIGATION_KEY nk );
    bool Locate( unsigned long pos, thc::ChessRules &cr, std::string &title, bool &at_move0 );
    int Locate_bc( unsigned long pos, thc::ChessRules &cr, std::string &title, bool &at_move0 );
    GAME_MOVE *LocateAtMoveZeroGetLastMove() { return locate_at_move0_last_move; }
    MovePlus LocateAtMoveZeroGetLastMove_bc();
    unsigned long FindMove0();
    unsigned long FindEnd();
    bool IsAtEnd( unsigned long pos );
    int GetMoveOffset( int bc_offset );
    void Debug();

private:
    std::vector<GameViewElement> expansion;
    int level;                  // track recursion level, 0 = root
    thc::ChessRules cr;         // track chess position for all operations
    unsigned long offset;       // track input offset of displayable string
    bool newline;
    bool comment;
    std::string result;
    GameTree tree_bc;
    GAME_MOVE *locate_at_move0_last_move;
    thc::ChessRules start_position;
    thc::ChessRules final_position;
    std::string final_position_txt;
    char language_lookup[6];
};

#endif // GAME_VIEW_H
