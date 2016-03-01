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
#include "ListableGameDb.h"
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
        DB_REQ      db_req,
        wxWindowID  id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );

    virtual ~DbDialog() { objs.db->FindPlayerEnd(); }

    // We calculate a vector of all blobs in the games that leading to the search position
    std::vector< PATH_TO_POSITION > transpositions;
    bool ReadItemFromMemory( int item, CompactGame &info );
    void MoveColCompare();

    // Overrides - Gdv = Games Dialog Override
    virtual void GdvOnActivate();
    virtual wxSizer *GdvAddExtraControls();
    virtual void GdvReadItem( int item, CompactGame &info );
    virtual void GdvListColClick( int compare_col );
    virtual void GdvSaveAllToAFile();
    virtual void GdvHelpClick();
    virtual void GdvCheckBox( bool checked );
    virtual void GdvCheckBox2( bool checked );
    virtual void GdvUtility();
    virtual void GdvSearch();
    virtual void GdvButton1();
    virtual void GdvButton2();
    virtual void GdvButton3();
    virtual void GdvButton4();
    virtual void GdvButton5();
    virtual void GdvOnCancel();
    virtual void GdvNextMove( int idx );
    virtual bool MoveColCompareReadGame( MoveColCompareElement &e, int idx, const char *blob );
    virtual int  GetBasePositionIdx( CompactGame &pact ) { int idx; pact.FindPositionInGame( objs.db->GetPositionHash(), idx ); return idx; }

    // Helpers
    bool LoadGamesIntoMemory();
    bool LoadGamesPrompted( std::string prompt );
    void CopyOrAdd( bool clear_clipboard );
    void StatsCalculateLegacy();
    void StatsCalculateInMemory();
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
    GamesCache gc_db_displayed_games;
};

#endif    // DB_DIALOG_H
