/****************************************************************************
 * Full game representation
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#ifndef GAME_DOCUMENT_H
#define GAME_DOCUMENT_H
#include <string>
#include <vector>
#include <memory>
#include "thc.h"
#include "MoveTree.h"
#include "NavigationKey.h"
#include "GameView.h"
#include "GameLifecycle.h"
#include "CompressMoves.h"
#include "CompactGame.h"
#include "ListableGame.h"

struct FakeRepository
{
    std::string m_white;
    std::string m_black;
};

class GameDocument : public ListableGame
{

public:
    GameDocument()
    {
        thc::ChessPosition initial_position;
        Init(initial_position);
    }

    virtual GameDocument  *IsGameDocument()   { return this; }
    virtual void ConvertToGameDocument(GameDocument &gd)   { gd = *this; }
    virtual long GetFposn()                   { return fposn0; }
    virtual void SetFposn(long posn)          { fposn0=posn; }
    virtual Roster &RefRoster() { return r; }
    virtual void SetRoster( Roster &r_in ) { r=r_in; }

    virtual std::vector<thc::Move> &RefMoves()
    {
        static std::vector<thc::Move> moves;
        moves.clear();
        std::vector<MoveTree> &variation = tree.variations[0];
        for( unsigned int i=0; i<variation.size(); i++ )
        {
            thc::Move mv = variation[i].game_move.move;
            moves.push_back(mv);
        }
        return moves;
    }
    std::string blob_recalculated_on_request;
    virtual const char *CompressedMoves()
    {
        blob_recalculated_on_request.clear();
        CompressMoves press;
        if( press.cr == start_position )
        {
            std::vector<thc::Move> moves;
            std::vector<MoveTree> &variation = tree.variations[0];
            for( unsigned int i=0; i<variation.size(); i++ )
            {
                thc::Move mv = variation[i].game_move.move;
                moves.push_back(mv);
            }
            blob_recalculated_on_request = press.Compress( moves );
        }
        return blob_recalculated_on_request.c_str();
    }
    virtual thc::ChessPosition &RefStartPosition() { return start_position; }

    // For now at least, the following are used for fast sorting on column headings
    virtual const char *White()     { return r.white.c_str();    }
    virtual const char *Black()     { return r.black.c_str();    }
    virtual const char *Event()     { return r.event.c_str();    }
    virtual const char *Site()      { return r.site.c_str();     }
    virtual const char *Result()    { return r.result.c_str();   }
    virtual const char *Round()     { return r.round.c_str() ;   }
    virtual const char *Date()      { return r.date.c_str();     }
    virtual const char *Eco()       { return r.eco.c_str();      }
    virtual const char *WhiteElo()  { return r.white_elo.c_str(); }
    virtual const char *BlackElo()  { return r.black_elo.c_str(); }
    virtual const char *Fen()       { return r.fen.c_str();     }


    // Copy constructor
    GameDocument( const GameDocument& src )
    {
        *this = src;    // use the assignment operator
    }

    // Assignment operator
    GameDocument& operator=( const GameDocument& src )
    {

        // Copy all data fields
        game_details_edited = src.game_details_edited;
        game_prefix_edited  = src.game_prefix_edited;
        modified            = src.modified;
        game_being_edited   = src.game_being_edited;
        pgn_handle      = src.pgn_handle;
        sort_idx        = src.sort_idx;
        r               = src.r;
        extra_tags      = src.extra_tags;
        game_id         = src.game_id;
        start_position  = src.start_position;
        master_position = src.master_position;
        fposn0          = src.fposn0;
        fposn1          = src.fposn1;
        fposn2          = src.fposn2;
        fposn3          = src.fposn3;
        prefix_txt      = src.prefix_txt;
        moves_txt       = src.moves_txt;
        non_zero_start_pos = src.non_zero_start_pos;

        tree            = src.tree;
        gv              = src.gv;

        // Need to rebuild using our copy of the tree to avoid
        //  raw ptrs to the old tree
        tree.root = &start_position;
        Rebuild();
        return( *this );
    }

    // Allow sorts
    bool operator< (const GameDocument &rhs) const
    {
        return sort_idx < rhs.sort_idx;
    }

    // Overrides
    virtual bool IsModified() { return (game_prefix_edited || game_details_edited || modified); }
    virtual void SetGameBeingEdited( uint32_t game_being_edited_ ) { this->game_being_edited = game_being_edited_; }
    virtual uint32_t GetGameBeingEdited() { return game_being_edited; }


    void FleshOutDate();
    virtual void Init( const thc::ChessPosition &start_position );
    //void GetGameDocumentFromFile( GameDocument &gd );

    // Data
    Roster      r;  // white, black, event, site, date etc.
    std::vector<std::pair<std::string,std::string>> extra_tags;
    unsigned long non_zero_start_pos;
    bool        game_details_edited;
    bool        game_prefix_edited;
    bool        modified;
    uint32_t    game_being_edited;
    int         pgn_handle;
    int         sort_idx;

    thc::ChessPosition start_position;  // the start position
    unsigned long fposn0;       // offset of prefix in .pgn file
    unsigned long fposn1;       // offset of tags in .pgn file
    unsigned long fposn2;       // offset where moves are in .pgn file
    unsigned long fposn3;       // offset where moves end in .pgn file
    std::string prefix_txt;     // text between games
    std::string moves_txt;      // "1.e4 e5 2.Nf3.."

    std::string Description();

    void FleshOutMoves();
    void ToFileTxtGameDetails( std::string &str );
    void ToFileTxtGameBody( std::string &str );
    void ToPublishTxtGameBody( std::string &str, int &diagram_idx, int &mv_idx, int &neg_base, int publish_options  );
    bool IsDiff( GameDocument &other );
    bool PgnParse( bool use_semi, int &nbr_converted, const std::string str, thc::ChessRules &cr, VARIATION *pvar, bool use_current_language=false, int imove=-1 );
    void LoadFromMoveList( std::vector<thc::Move> &moves, int move_idx=0 );
    void SetNonZeroStartPosition( int main_line_idx );


    MoveTree *MakeMove( GAME_MOVE game_move, bool allow_overwrite );
    MoveTree *KibitzCaptureStart( const char *engine_name, const char *txt, std::vector<thc::Move> &var,
                                 bool &use_repeat_one_move,
                                 GAME_MOVE &repeat_one_move     // eg variation = e4,c5 new_variation = Nf3,Nc6 etc.
                                                                //  must make new_variation = c5,Nf3,Nc6 etc.
    );
    void KibitzCapture( MoveTree *node, const char *txt, std::vector<thc::Move> &var,
                       bool use_repeat_one_move,
                       GAME_MOVE &repeat_one_move       // eg variation = e4,c5 new_variation = Nf3,Nc6 etc.
                                                        //  must make new_variation = c5,Nf3,Nc6 etc.
    );
    void Promote();
    void Demote();
    bool PromotePaste( std::string &str );
    void PromoteToVariation( unsigned long offset_within_comment=0 );
    void PromoteRestToVariation();
    void DemoteToComment();
    void UseGame( const thc::ChessPosition &cp, const std::vector<thc::Move> &moves_from_base_position, CompactGame &pact );
    void Rebuild() { gv.Build( r.result, &tree, this->start_position ); }
    void DeleteRestOfVariation();
    void DeleteVariation();
    void RedisplayRequest( MoveTree *found );
    void Redisplay( unsigned long pos );

    // Get a picture of the game, various recipes
    //

    // Where are we in the document
    unsigned long GetInsertionPoint();
    void SetInsertionPoint( unsigned long pos );

    // A start position, a vector of moves, leading to a final position
    void GetSummary( thc::ChessPosition &start_position, std::vector<GAME_MOVE> &game_moves, thc::ChessPosition &pos );

    // The current position, title text for the last move played eg "Position after 23...Nxd5"
    //  Return ptr to the last move played,  NULL if no last move OR if nbr_half_moves_lag
    GAME_MOVE *GetSummaryTitle( thc::ChessRules &cr, std::string &title_txt, int nbr_half_moves_lag=0 );

    // The current position, move text for the last move played eg "23...Nxd5"
    //  Return ptr to the last move played,  NULL if no last move OR if nbr_half_moves_lag
    GAME_MOVE *GetSummaryMove( thc::ChessRules &cr, std::string &move_txt );

    //  We should define and use some simple recipes like this
    MoveTree *GetSummary();

    // Are we at the end of the main line (will a new game be the main game, or not)
    bool AtEndOfMainLine();

    // Are we currently in the main game, or not
    bool AreWeInMain();

    bool IsEmpty() { return !HaveMoves(); }
    bool HaveMoves();
    bool IsAtEnd();

    // Pass through to GameView
    void Debug()
    {  gv.Debug(); }
    void Display( unsigned long pos )
    {  gv.Display(pos); }
#if 0
    bool IsInComment( wxRichTextCtrl *ctrl )
    {  return gv.IsInComment(ctrl); }
    bool CommentEdit( wxRichTextCtrl *ctrl, long keycode, bool *pass_thru_edit_ptr=NULL )
    {   std::string txt_to_insert;
        gv.comment_edited = false;
        bool done = gv.CommentEdit(ctrl,txt_to_insert,keycode,pass_thru_edit_ptr);
        return done;
    }
    bool CommentEdit( wxRichTextCtrl *ctrl, std::string &txt_to_insert, bool *pass_thru_edit_ptr=NULL )
    {
        gv.comment_edited = false;
        bool done = gv.CommentEdit(ctrl,txt_to_insert,0,pass_thru_edit_ptr);
        return done;
    }
    bool IsSelectionInComment( wxRichTextCtrl *ctrl )
    {  return gv.IsSelectionInComment(ctrl); }
    void DeleteSelection( wxRichTextCtrl *ctrl )
    {  return gv.DeleteSelection(ctrl); }
#endif

    unsigned long NavigationKey( unsigned long pos, NAVIGATION_KEY nk )
    {  return gv.NavigationKey(pos,nk); }
    MoveTree *Locate( unsigned long pos, thc::ChessRules &cr, std::string &title, bool &at_move0 )
    {   cr = start_position;
        return gv.Locate( pos, cr, title, at_move0 ); }
    MoveTree *Locate( unsigned long pos, thc::ChessRules &cr, std::string &title )
    {   cr = start_position;
        bool at_move0; return gv.Locate( pos, cr, title, at_move0 );   }
    MoveTree *Locate( unsigned long pos, thc::ChessRules &cr )
    {   cr = start_position;
        std::string title; bool at_move0; return gv.Locate( pos, cr, title, at_move0 ); }

    //Data
    thc::ChessRules master_position;    // the current position
    MoveTree  tree;                     // the moves
    GameView gv;
    MoveTree    *gbl_plast_move;
    unsigned long insertion_point;
    FakeRepository repository;

// We really should use private more in this project. Excellent refactoring opportunity
private:
};

#endif //GAME_DOCUMENT_H
