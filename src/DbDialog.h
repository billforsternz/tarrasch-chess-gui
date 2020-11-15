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
#include "CtrlChessBoard.h"
#include "CompressMoves.h"
#include "ListableGameDb.h"
#include "PatternMatch.h"
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
        PatternParameters *parm,
        wxWindowID  id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize
    );
    virtual ~DbDialog() {}

    // We calculate a vector of all blobs in the games that leading to the search position
    std::vector< PATH_TO_POSITION > transpositions;
    bool ReadGameFromSearchResults( int item, CompactGame &info );
    void MoveColCompare();

    // Overrides - Gdv = Games Dialog Override
    virtual void GdvOnActivate();
    virtual void GdvEnumerateGames();
    virtual wxSizer *GdvAddExtraControls( bool big_display );
    virtual void GdvEnableControlsIfGamesFound( bool have_games );
    virtual void GdvReadItem( int item, CompactGame &info );
    virtual void GdvListColClick( int compare_col );
    virtual void GdvSaveAllToAFile();
    virtual void GdvHelpClick();
    virtual void GdvCheckBox( bool checked );
    virtual void GdvCheckBox2( bool checked );
    virtual void GdvSearch();
    virtual void GdvButton1();
    virtual void GdvButton2();
    virtual void GdvButton3();
    virtual void GdvButton4();
    virtual void GdvButton5();
    virtual void GdvOnCancel();
    virtual void GdvNextMove( int idx );
    virtual int  CalculateTranspo( const char *blob, int &transpo );
    virtual int  GetBasePositionIdx( CompactGame &pact, bool receiving_focus );

    // Helpers
    void CopyOrAdd( bool clear_clipboard );
    void StatsCalculate();
    void PatternSearch();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // Data members
private:
    std::map< char, MOVE_STATS > stats; // map each compressed move in the position to move stats
    bool white_player_search;
    std::vector<thc::Move> moves_in_this_position;
    std::vector<thc::Move> moves_from_base_position;
    GamesCache gc_db_displayed_games;
    PatternMatch pm;
};

#endif    // DB_DIALOG_H
