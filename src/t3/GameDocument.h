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
#include "DebugPrintf.h"
#include "MoveTree.h"
#include "NavigationKey.h"
#include "GameView.h"
#include "GameLifecycle.h"
#include "CompressMoves.h"


#define smart_ptr std::shared_ptr //std::unique_ptr
#define make_smart_ptr(T,to,from) smart_ptr<T> to; to.reset(new T(from))

class Roster
{
public:
    std::string white;
    std::string black;
    std::string event;
    std::string site;
    std::string result;
    std::string round;
    std::string date;
    std::string eco;
    std::string white_elo;
    std::string black_elo;
};

class GameDocument;
class DB_GAME_INFO;

class CompactGame
{
public:
    CompactGame() { game_id=0; transpo_nbr=0; }
    Roster r;
    thc::ChessPosition start_position;
    std::vector< thc::Move > moves;
    
    // temp stuff hopefully
    int game_id;
    int transpo_nbr;
    
    std::string Description();
    void Upscale( GameDocument &gd );       // to GameDocument
    void Downscale( GameDocument &gd );     // from GameDocument
    bool HaveStartPosition() {
        bool is_initial_position = ( 0 == strcmp(start_position.squares,"rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR")
                                    ) && start_position.white;
        return !is_initial_position; }
    thc::ChessPosition &GetStartPosition() { return start_position; }
    
    // Return index into vector where start position found
    bool FindPositionInGame( uint64_t hash_to_match, int &idx )
    {
        thc::ChessRules cr = start_position;
        size_t len = moves.size();
        uint64_t hash = cr.Hash64Calculate();
        bool found = (hash==hash_to_match);
        idx = 0;
        for( size_t i=0; !found && i<len; i++  )
        {
            thc::Move mv = moves[i];
            hash = cr.Hash64Update( hash, mv );
            if( hash == hash_to_match )
            {
                found = true;
                idx = static_cast<int>(i+1);
                break;
            }
            cr.PlayMove(mv);
        }
        return found;
    }
    
};


class MagicBase
{
public:
    MagicBase() { transpo_nbr=0; }
    virtual ~MagicBase() {}
    virtual DB_GAME_INFO *GetDbGameInfoPtr() { return NULL; }
    virtual GameDocument *GetGameDocumentPtr()  {
        cprintf("FIXME DANGER WILL ROBINSON 3\n");  return NULL; }
    virtual bool GetPgnHandle( int &pgn_handle ) { return false; }
    virtual void SetPgnHandle( int pgn_handle )  {}
    virtual bool IsInMemory()        { return false; }
    virtual bool IsModified()        { return false; }
    virtual void SetSelected( bool selected ) {}
    virtual void SetFocused( bool focused ) {}
    virtual bool GetSelected() { return false; }
    virtual bool GetFocused() { return false; }
    virtual void SetGameBeingEdited( uint32_t game_being_edited ) {}
    virtual uint32_t GetGameBeingEdited() { return 0; }
    virtual long GetFposn() { return 0; }
    
    // High performance
    virtual Roster                  &RefRoster()          { static Roster r; return r; }
    virtual std::vector<thc::Move>  &RefMoves()           { static std::vector<thc::Move> moves; return moves; }
    virtual std::string             &RefCompressedMoves() { static std::string moves; return moves; }
    virtual thc::ChessPosition      &RefStartPosition()   { static thc::ChessPosition cp; return cp; }
    
    // Easy to use
    virtual void GetCompactGame( CompactGame &pact )
    {
        pact.r     = RefRoster();
        pact.moves = RefMoves();
        pact.start_position = RefStartPosition();
        pact.transpo_nbr = transpo_nbr;
    }
    
    int transpo_nbr;
};

class GameLogic;
class GameDocument : public MagicBase
{
    
public:
    GameDocument( GameLogic *gl );
    GameDocument();
	virtual GameDocument        *GetGameDocumentPtr()           { return this; }
    virtual bool IsInMemory()        { return true; }
    virtual long GetFposn() { return fposn0; }
    virtual Roster &RefRoster() { return r; }
    virtual std::vector<thc::Move> &RefMoves()
    {
        static std::vector<thc::Move> moves;
        moves.clear();
        std::vector<MoveTree> &variation = tree.variations[0];
        for( int i=0; i<variation.size(); i++ )
        {
            thc::Move mv = variation[i].game_move.move;
            moves.push_back(mv);
        }
        return moves;
    }
    virtual std::string &RefCompressedMoves()
    {
        static std::string blob;
        blob.clear();
        CompressMoves press;
        if( press.cr == start_position )
        {
            std::vector<thc::Move> moves;
            std::vector<MoveTree> &variation = tree.variations[0];
            for( int i=0; i<variation.size(); i++ )
            {
                thc::Move mv = variation[i].game_move.move;
                moves.push_back(mv);
            }
            blob = press.Compress( moves );
        }
        return blob;
    }
    virtual thc::ChessPosition &RefStartPosition() { return start_position; }
    
    // Copy constructor
    GameDocument( const GameDocument& src )
    {
        *this = src;    // use the assignment operator
    }
    
    // Assignment operator
    GameDocument& operator=( const GameDocument& src )
    {
        
        // Copy all data fields
        gl = src.gl;
        game_details_edited = src.game_details_edited;
        game_prefix_edited  = src.game_prefix_edited;
        modified        = src.modified;
        in_memory       = src.in_memory;
        game_being_edited    = src.game_being_edited;
        pgn_handle      = src.pgn_handle;
        sort_idx        = src.sort_idx;
        focus           = false; //src.focus;
        selected        = src.selected;
        game_nbr        = src.game_nbr;
        r               = src.r;
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
    virtual uint32_t GetGameBeingEdited() { return game_being_edited; }
    
    void FleshOutDate();
    virtual void Init( const thc::ChessPosition &start_position );
    void GetGameDocumentFromFile( GameDocument &gd );

    // Data
    Roster      r;  // white, black, event, site, date etc.
    unsigned long non_zero_start_pos;
    bool        game_details_edited;
    bool        game_prefix_edited;
    bool        modified;
    bool        in_memory;
    uint32_t    game_being_edited;
    bool        selected;
    bool        focus;
    int         pgn_handle;
    int         sort_idx;
    int         game_nbr;
   
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
    void Rebuild() { gv.Build( r.result, &tree, this->start_position ); }
    void DeleteRestOfVariation();
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
    GAME_MOVE *GetSummary( thc::ChessRules &cr, std::string &title_txt, int nbr_half_moves_lag=0 );
    
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
    bool IsInComment( wxRichTextCtrl *ctrl )
    {  return gv.IsInComment(ctrl); }
    bool CommentEdit( wxRichTextCtrl *ctrl, long keycode )
    {   std::string txt_to_insert;
        gv.comment_edited = false;
        bool done = gv.CommentEdit(ctrl,txt_to_insert,keycode);
        return done;
    }
    bool CommentEdit( wxRichTextCtrl *ctrl, std::string &txt_to_insert )
    {
        gv.comment_edited = false;
        bool done = gv.CommentEdit(ctrl,txt_to_insert);
        return done;
    }
    bool IsSelectionInComment( wxRichTextCtrl *ctrl )
    {  return gv.IsSelectionInComment(ctrl); }
    void DeleteSelection( wxRichTextCtrl *ctrl )
    {  return gv.DeleteSelection(ctrl); }
    
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
    GameLogic *gl;
    thc::ChessRules master_position;    // the current position
    MoveTree  tree;                     // the moves
    GameView gv;
    MoveTree    *gbl_plast_move;
};
        
#endif //GAME_DOCUMENT_H
