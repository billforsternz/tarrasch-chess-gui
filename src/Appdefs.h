/******************************************************************************
 * Top level application definitions
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ******************************************************************************/
#ifndef APPDEFS_H
#define APPDEFS_H

#include "wx/wx.h"
#include "Portability.h"

#define MASTER_VERSION_BASE "V3.13a+"    //add '-' or '+' prefix between releases

#define USING_TARRASCH_BASE
// no suffix indicates default .tdb is tarrasch-base
#ifdef  USING_TARRASCH_BASE
#ifdef THC_WINDOWS
#define MASTER_VERSION MASTER_VERSION_BASE "-Windows"
#endif
#ifdef THC_LINUX
#define MASTER_VERSION MASTER_VERSION_BASE "-Linux"
#endif
#ifdef THC_MAC
#define MASTER_VERSION MASTER_VERSION_BASE "-Mac"
#endif

// 'g' suffix indicates default .tdb is great players database
#else
#ifdef THC_WINDOWS
#define MASTER_VERSION MASTER_VERSION_BASE "-g-Windows"
#endif
#ifdef THC_LINUX
#define MASTER_VERSION MASTER_VERSION_BASE "-g-Linux"
#endif
#ifdef THC_MAC
#define MASTER_VERSION MASTER_VERSION_BASE "-g-Mac"
#endif
#endif

#define DEFAULT_ENGINE "stockfish_11_x32.exe"
#define DEFAULT_ENGINE_64 "stockfish_11_x64.exe"
#define DEFAULTS_STEPPER_LEVEL 2
#define DATABASE_VERSION_NUMBER_LEGACY   1    // Old SQL type = supported with V3.03a
#define DATABASE_VERSION_NUMBER_TINY     2    // Some kind of intermediate version, we don't support it any more at all
#define DATABASE_VERSION_NUMBER_BIN_DB   3    // Up until V3.12b
#define DATABASE_VERSION_NUMBER_LOCKABLE 4    // V3.12b** onward, supports lockable databases (retain support for previous version too)
#define DATABASE_LOCKABLE_LIMIT 10000         // Max number of restricted games we can write
#ifdef  USING_TARRASCH_BASE
#define DEFAULT_DATABASE "tarrasch-base.tdb"
#else
#define DEFAULT_DATABASE "great-players-demo.tdb"
#endif

// Conditional compiles
//#define DURING_DEVELOPMENT		    // If defined we get a console window for cprintf()
#define AUI_NOTEBOOK				// Define this to get X close boxes on tabs
//#define NEW_TAB_LAUNCHES_TESTBED	// If defined new tab feature actually launches testbed window instead
#ifndef DURING_DEVELOPMENT
//#undef  NEW_TAB_LAUNCHES_TESTBED	// Never unless during development
#endif

// Misc
#define NBR_KIBITZ_LINES 4
#define BORDER_COMMON wxBORDER_SUNKEN   //wxBORDER_SIMPLE
#define UNUSED(x)

//-----------------------------------------------------------------------------
// Menu item identifiers
//-----------------------------------------------------------------------------
enum
{
    ID_Quit = 1,
    ID_Clear,
    ID_FILE_SAVE_GAME_AS,
    ID_FILE_OPEN_SHELL,
    ID_FILE_OPEN_SHELL_ASK,
    ID_FILE_OPEN_LOG,
    ID_FILE_TAB_NEW,
    ID_FILE_TAB_CLOSE,
    ID_FILE_TAB_INCLUDE,
    ID_CMD_FLIP,
    ID_CMD_SET_POSITION, 
    ID_CMD_NEW_GAME,        
    ID_CMD_TAKEBACK,     
    ID_CMD_MOVENOW,     
    ID_CMD_EXAMINE_GAME,
    ID_CMD_DRAW,
    ID_CMD_WHITE_RESIGNS,
    ID_CMD_BLACK_RESIGNS,
    ID_CMD_PLAY_WHITE,   
    ID_CMD_PLAY_BLACK,   
    ID_CMD_SWAP_SIDES,         
    ID_CMD_KIBITZ,
    ID_CMD_CLEAR_KIBITZ,
    ID_OPTIONS_ENGINE,
    ID_OPTIONS_PLAYERS, 
    ID_OPTIONS_CLOCKS,
    ID_OPTIONS_BOOK,
    ID_OPTIONS_LOG,
    ID_OPTIONS_TRAINING,
    ID_OPTIONS_GENERAL,
    ID_OPTIONS_RESET,       
    ID_DATABASE_SEARCH,      
    ID_DATABASE_SHOW_ALL,    
    ID_DATABASE_PLAYERS,
    ID_DATABASE_SELECT, 
    ID_DATABASE_CREATE, 
    ID_DATABASE_APPEND, 
    ID_DATABASE_PATTERN, 
    ID_DATABASE_MATERIAL, 
    ID_DATABASE_MAINTENANCE, 
    ID_GAMES_CURRENT,
    ID_GAMES_DATABASE,
    ID_GAMES_SESSION,
    ID_GAMES_CLIPBOARD,
    ID_EDIT_GAME_DETAILS,
    ID_EDIT_GAME_PREFIX,
    ID_EDIT_DELETE_VARIATION,             
    ID_EDIT_PROMOTE,             
    ID_EDIT_DEMOTE,              
    ID_EDIT_DEMOTE_TO_COMMENT,   
    ID_EDIT_PROMOTE_TO_VARIATION,
    ID_EDIT_PROMOTE_REST_TO_VARIATION,
    ID_HELP_HELP,
    ID_HELP_CREDITS,
    ID_BUTTON_UP,   
    ID_BUTTON_DOWN, 
    ID_BUTTON_LEFT, 
    ID_BUTTON_RIGHT,
    ID_CMD_NEXT_GAME, 
    ID_CMD_PREVIOUS_GAME,
    ID_ANNOT1,
    ID_ANNOT2,
    ID_ANNOT3,
    ID_ANNOT4,
    ID_ANNOT5,
    ID_ANNOT6,
    ID_ANNOT7,
    ID_ANNOT10,
    ID_ANNOT14,
    ID_ANNOT15,
    ID_ANNOT16,
    ID_ANNOT17,
    ID_ANNOT18,
    ID_ANNOT19,
    ID_ANNOT20,
    ID_COPY_GAME_PGN_TO_CLIPBOARD,
    ID_COPY_FEN_TO_CLIPBOARD,
	ID_BUTTON_TAB_NEW,
	ID_STATIC_TXT_PLAYER1,
	ID_STATIC_TXT_PLAYER2,
	ID_STATIC_TXT_TIME1,
	ID_STATIC_TXT_TIME2,
    ID_CMD_ABOUT = wxID_ABOUT   // important for possible Mac port that we use this apparently
};

//-----------------------------------------------------------------------------
// Window identifiers
//-----------------------------------------------------------------------------
enum
{
    ID_POPUP=100,
    ID_BUTTON1,
    ID_BUTTON2,
    ID_BUTTON3,
    ID_BUTTON4,
    ID_WHO_TOP,
    ID_WHO_BOTTOM,
    ID_WHO_STATUS,
    ID_STATUS,
    ID_WHITE_PLAYER,
    ID_DASH_PLAYER,
    ID_BLACK_PLAYER,
    ID_BORDER,
    ID_BOX,
    ID_KIBITZ,
    ID_KIBITZ_BUTTON1,
    ID_KIBITZ_BUTTON2,
    ID_KIBITZ_BUTTON3,
    ID_BOOK_MOVES,
/*  ID_WHITE_CLOCK,
    ID_WHITE_CLBOX,
    ID_BLACK_CLOCK,
    ID_BLACK_CLBOX, */
    ID_LIST_CTRL,
    ID_KIBITZ0,
    ID_KIBITZ1,
    ID_KIBITZ2,
    ID_KIBITZ3,
    ID_KIBITZ4,
    ID_COMBOBOX,
    ID_PGN_LISTBOX
};

extern bool gbl_spelling_us;			// True for US spelling
extern const char *gbl_spell_colour;	// "colour" or "color"

#endif // APPDEFS_H
