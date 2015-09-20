/****************************************************************************
 * Database data source for games dialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DB_DIALOG_H
#define DB_DIALOG_H
#include <unordered_set>
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/accel.h"
#include "SuspendEngine.h"
#include "GamesCache.h"
#include "GameDocument.h"
#include "Repository.h"
#include "MiniBoard.h"
#include "CompressMoves.h"
#include "Database.h"
#include "PgnDialog.h"
#include "GamesDialog.h"


// Each move in a given position has stats associated with it
struct MOVE_STATS
{
    int nbr_games;
    int nbr_white_wins;
    int nbr_black_wins;
    int nbr_draws;
    
    // Sort according to number of games
    bool operator < (const MOVE_STATS& ms)  const { return nbr_games < ms.nbr_games; }
    bool operator > (const MOVE_STATS& ms)  const { return nbr_games > ms.nbr_games; }
    bool operator == (const MOVE_STATS& ms) const { return nbr_games == ms.nbr_games; }
};

// Individual path to a given position
struct PATH_TO_POSITION
{
    PATH_TO_POSITION() { frequency=0; }
    int frequency;
    std::string blob;
    
    // Sort according to frequency
    bool operator < (const PATH_TO_POSITION& ptp)  const { return frequency < ptp.frequency; }
    bool operator > (const PATH_TO_POSITION& ptp)  const { return frequency > ptp.frequency; }
    bool operator == (const PATH_TO_POSITION& ptp) const { return frequency == ptp.frequency; }
};

// DbDialog class declaration
class DbDialog : public GamesDialog
{    
public:
    DbDialog
    (
        wxWindow    *parent,
        thc::ChessRules *cr,
        GamesCache  *gc,
        GamesCache  *gc_clipboard,
        wxWindowID  id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );

    virtual ~DbDialog() { objs.db->FindPlayerEnd(); }

    // We calculate a vector of all blobs in the games that leading to the search position
    std::vector< PATH_TO_POSITION > transpositions;
    bool ReadItemFromMemory( int item, DB_GAME_INFO &info );
    void SmartCompare();

    // Overrides
    virtual void OnActivate();
    virtual wxSizer *AddExtraControls();
    virtual void ReadItem( int item, CompactGame &info );
    virtual void OnListColClick( int compare_col );
    virtual void OnSaveAllToAFile();
    virtual void OnHelpClick();
    virtual void OnCheckBox( bool checked );
    virtual void OnCheckBox2( bool checked );
    virtual void OnUtility();
    virtual void OnSearch();
    virtual void OnButton1();
    virtual void OnButton2();
    virtual void OnButton3();
    virtual void OnButton4();
    virtual void OnCancel();
    virtual void OnNextMove( int idx );

    // Helpers
    void LoadGamesIntoMemory();
    void CopyOrAdd( bool clear_clipboard );
    void StatsCalculate();
    GameDocument *GetFocusGame( int &idx );
    void DeselectOthers();
    int          selected_game;
    void         SyncCacheOrderBefore();
    void         SyncCacheOrderAfter();

    // Data members
private:
    
    std::map< uint32_t, MOVE_STATS > stats; // map each move in the position to move stats
    bool white_player_search;
    std::unordered_set<int>   games_set;    // game_ids for all games in memory
    std::unordered_set<uint64_t> drill_down_set;  // positions already encountered drilling down
    std::vector<thc::Move> moves_in_this_position;
    std::vector<thc::Move> moves_from_base_position;
    std::vector< smart_ptr<DB_GAME_INFO> > displayed_games;
};

#endif    // DB_DIALOG_H
