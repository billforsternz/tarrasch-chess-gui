/****************************************************************************
 * Persistent storage of config information
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "DebugPrintf.h"
#include "Portability.h"
#include "Lang.h"
#include "Repository.h"

#ifndef THC_WINDOWS
    bool Is64BitWindows() { return false; }
#else
    // MSDN method of determining whether we are running on 64 bit Windows
    #if 1 // This is safer and better than the version below
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    bool Is64BitWindows()
    {
        BOOL bIsWow64 = FALSE;

        //IsWow64Process is not available on all supported versions of Windows.
        //Use GetModuleHandle to get a handle to the DLL that contains the function
        //and GetProcAddress to get a pointer to the function if available.

        fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
            GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

        if(NULL != fnIsWow64Process)
        {
            if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
            {
                //handle error
            }
        }
        bool ret =  bIsWow64?true:false;
        dbg_printf( "Running on %d bit Windows\n", ret?64:32 );
        return ret;
    }

    // Raymond Chen's method of determining whether we are running on 64 bit Windows
    #else  // problem if running on 32 bit Windows XP IsWow64Process() doesn't exist
    bool Is64BitWindows()
    {
        BOOL f64 = FALSE;
    #if defined(_WIN64)
        f64 = TRUE;  // 64-bit programs run only on Win64
    #elif defined(_WIN32)
        // 32-bit programs run on both 32-bit and 64-bit Windows
        // so must sniff
        f64 = IsWow64Process(GetCurrentProcess(), &f64) && f64;
    #else
        f64 = FALSE; // Win64 does not support Win16
    #endif
        bool ret = (f64 ? true : false);
        return ret;
    }
    #endif
#endif

Repository::Repository( bool use_defaults )
{
    SetDirectories();
    nv.m_doc_dir  = doc_dir;
    database.m_file = doc_dir + "/" + database.m_file;
    book.m_file     = doc_dir + "/" + book.m_file;
    log.m_file      = doc_dir + "/" + log.m_file;
    engine.m_file   = exe_dir + "/Engines/" + engine.m_file;

    // Using wxConfig instead of writing wxFileConfig or wxRegConfig enhances
    // portability of the code
    config = new wxConfig( "Tarrasch", "Triple Happy", ini_filename );

    // Optionally use default values instead of reading from config
    if( !use_defaults )
    {

        // Book
        config->Read("BookFile",             &book.m_file               );
        ReadBool    ("BookEnabled",           book.m_enabled            );
        ReadBool    ("BookSuggest",           book.m_suggest            );
        config->Read("BookLimitMoves",       &book.m_limit_moves        );
        config->Read("BookPostLimitPercent", &book.m_post_limit_percent );

        // Player
        config->Read("PlayerHuman",          &player.m_human            );
    //  config->Read("PlayerComputer",       &player.m_computer         );
    //  config->Read("PlayerWhite",          &player.m_white            );
    //  config->Read("PlayerBlack",          &player.m_black            );

        // Clock
        config->Read("HumanTime",             &clock.m_human_time       );
        config->Read("HumanIncrement",        &clock.m_human_increment  );
        ReadBool    ("HumanVisible",           clock.m_human_visible    );
        ReadBool    ("HumanRunning",           clock.m_human_running    );
        config->Read("EngineTime",            &clock.m_engine_time      );
        config->Read("EngineIncrement",       &clock.m_engine_increment );
        config->Read("EngineFixedMinutes",    &clock.m_engine_fixed_minutes );
        config->Read("EngineFixedSeconds",    &clock.m_engine_fixed_seconds );
        ReadBool    ("EngineVisible",          clock.m_engine_visible   );
        ReadBool    ("EngineRunning",          clock.m_engine_running   );

        config->Read("WhiteTime",             &clock.m_white_time       );
        config->Read("WhiteIncrement",        &clock.m_white_increment  );
        ReadBool    ("WhiteVisible",           clock.m_white_visible    );
    //  ReadBool    ("WhiteRunning",           clock.m_white_running    );
        config->Read("BlackTime",             &clock.m_black_time       );
        config->Read("BlackIncrement",        &clock.m_black_increment  );
        ReadBool    ("BlackVisible",           clock.m_black_visible    );
    //  ReadBool    ("BlackRunning",           clock.m_black_running    );
        ReadBool    ("FixedPeriodMode",        clock.m_fixed_period_mode );
        cprintf( "clock.m_fixed_period_mode=%s\n",clock.m_fixed_period_mode?"true":"false");

        // Log
        config->Read("LogFile",              &log.m_file                );
        ReadBool    ("LogEnabled",            log.m_enabled             );

        // Engine
        config->Read("EngineExeFile",         &engine.m_file            );
        ReadBool    ("EnginePonder",           engine.m_ponder          );
        ReadBool    ("EngineNormalPriority",   engine.m_normal_priority );
        ReadBool    ("EngineLowPriority",      engine.m_low_priority    );
        ReadBool    ("EngineIdlePriority",     engine.m_idle_priority   );
        config->Read("EngineHash",            &engine.m_hash            );
        config->Read("EngineMaxCpuCores",     &engine.m_max_cpu_cores   );
        config->Read("EngineCustom1a",        &engine.m_custom1a        );
        config->Read("EngineCustom1b",        &engine.m_custom1b        );
        config->Read("EngineCustom2a",        &engine.m_custom2a        );
        config->Read("EngineCustom2b",        &engine.m_custom2b        );
        config->Read("EngineCustom3a",        &engine.m_custom3a        );
        config->Read("EngineCustom3b",        &engine.m_custom3b        );
        config->Read("EngineCustom4a",        &engine.m_custom4a        );
        config->Read("EngineCustom4b",        &engine.m_custom4b        );
        config->Read("EngineCustom5a",        &engine.m_custom5a        );
        config->Read("EngineCustom5b",        &engine.m_custom5b        );
        config->Read("EngineCustom6a",        &engine.m_custom6a        );
        config->Read("EngineCustom6b",        &engine.m_custom6b        );

        // Training
        config->Read("TrainingNbrHalfMovesBehind",       &training.m_nbr_half_moves_behind       );
        ReadBool    ("TrainingPeekAtCompletePosition",    training.m_peek_at_complete_position   );
        ReadBool    ("TrainingBlindfoldHideWhitePawns",   training.m_blindfold_hide_white_pawns  );
        ReadBool    ("TrainingBlindfoldHideWhitePieces",  training.m_blindfold_hide_white_pieces );
        ReadBool    ("TrainingBlindfoldHideBlackPawns",   training.m_blindfold_hide_black_pawns  );
        ReadBool    ("TrainingBlindfoldHideBlackPieces",  training.m_blindfold_hide_black_pieces );

        // Database
        config->Read("DatabaseFile",                &database.m_file          );
        config->Read("DatabaseEloCutoff",           &database.m_elo_cutoff    );
        config->Read("DatabaseEloCutoffBeforeYear", &database.m_elo_cutoff_before_year );
        ReadBool    ("DatabaseEloCutoffIgnore",     database.m_elo_cutoff_ignore );
        ReadBool    ("DatabaseEloCutoffOne",        database.m_elo_cutoff_one    );
        ReadBool    ("DatabaseEloCutoffBoth",       database.m_elo_cutoff_both );
        ReadBool    ("DatabaseEloCutoffFail",       database.m_elo_cutoff_fail );
        ReadBool    ("DatabaseEloCutoffPass",       database.m_elo_cutoff_pass );
        ReadBool    ("DatabaseEloCutoffPassBefore", database.m_elo_cutoff_pass_before );

        // General
        config->Read("GeneralNotationLanguage",          &general.m_notation_language );
        std::string lang(general.m_notation_language);
        LangSet(lang);
        ReadBool    ("GeneralNoItalics",                  general.m_no_italics        );
        ReadBool    ("GeneralStraightToGame",             general.m_straight_to_game  );
        ReadBool    ("GeneralStraightToFirstGame",        general.m_straight_to_first_game  );
        config->Read("GeneralFontSize",                  &general.m_font_size );
        ReadBool    ("GeneralHeadingAboveBoard",          general.m_heading_above_board );
        ReadBool    ("GeneralNoAutoFlip",                 general.m_no_auto_flip  );
        ReadBool    ("GeneralEmitBellWhenEngineMoves",    general.m_bell  );
        ReadBool    ("GeneralSuppressHighlight",          general.m_suppress_highlight  );
        ReadBool    ("GeneralLineHighlight",              general.m_line_highlight  );
        config->Read("GeneralLightSquaresRed",            &general.m_light_colour_r );
        config->Read("GeneralLightSquaresGreen",          &general.m_light_colour_g );
        config->Read("GeneralLightSquaresBlue",           &general.m_light_colour_b );
        config->Read("GeneralDarkSquaresRed",             &general.m_dark_colour_r );
        config->Read("GeneralDarkSquaresGreen",           &general.m_dark_colour_g );
        config->Read("GeneralDarkSquaresBlue",            &general.m_dark_colour_b );
        config->Read("GeneralHighlightLightSquaresRed",            &general.m_highlight_light_colour_r );
        config->Read("GeneralHighlightLightSquaresGreen",          &general.m_highlight_light_colour_g );
        config->Read("GeneralHighlightLightSquaresBlue",           &general.m_highlight_light_colour_b );
        config->Read("GeneralHighlightDarkSquaresRed",             &general.m_highlight_dark_colour_r );
        config->Read("GeneralHighlightDarkSquaresGreen",           &general.m_highlight_dark_colour_g );
        config->Read("GeneralHighlightDarkSquaresBlue",            &general.m_highlight_dark_colour_b );
        config->Read("GeneralHighlightLineSquaresRed",             &general.m_highlight_line_colour_r );
        config->Read("GeneralHighlightLineSquaresGreen",           &general.m_highlight_line_colour_g );
        config->Read("GeneralHighlightLineSquaresBlue",            &general.m_highlight_line_colour_b );

        // NonVolatile - DefaultsStepper
        //
        //  In general we respect the users configuration items. However, with V3.13a we
        //  introduced the DefaultsStepper mechanism, to allow us to move the user to
        //  enhanced defaults (engine and database initially), if and only if they are
        //  using the previous defaults. Initially this will mean that we can change from
        //  kingbase-lite to the new TWIC based tarrasch-base, and from Stockfish 8 to
        //  Stockfish 11 (12?) without upgrading users having to do anything at all.
        bool default_stepper_exists = config->Read("NonVolatileDefaultsStepper", &nv.m_defaults_stepper );
        if( !default_stepper_exists )
            nv.m_defaults_stepper = 1;  // we introduced mechanism at DEFAULTS_STEPPER_LEVEL = 2, assign level 1
                                        // to all prior versions
        int old_level = nv.m_defaults_stepper;
        int new_level = DEFAULTS_STEPPER_LEVEL;
        if( old_level+1 == new_level )
        {
            std::string database_steps [] = {"","kingbase-lite","tarrasch-base"};
            std::string engine_64_steps[] = {"","stockfish_8_x64","stockfish_11_x64"};
            std::string engine_32_steps[] = {"","stockfish_8_x32","stockfish_11_x32"};
            wxFileName wfn1(database.m_file);
            if( wfn1.IsOk() && wfn1.GetName()==database_steps[old_level])
            {
                wfn1.SetName(database_steps[new_level]);
                if( wfn1.FileExists() )
                {
                    database.m_file = wfn1.GetFullPath();
                    std::string s = database.m_file.ToStdString();
                    cprintf( "One time only change to database file: %s\n", s.c_str() );
                }
            }
            wxFileName wfn2(engine.m_file);
            if( wfn2.IsOk() && wfn2.GetName()==engine_64_steps[old_level])
            {
                wfn2.SetName(engine_64_steps[new_level]);
                if( wfn2.FileExists() )
                {
                    engine.m_file = wfn2.GetFullPath();
                    std::string s = engine.m_file.ToStdString();
                    cprintf( "One time only change to engine file: %s\n", s.c_str() );
                }
            }
            else if( wfn2.IsOk() && wfn2.GetName()==engine_32_steps[old_level])
            {
                wfn2.SetName(engine_32_steps[new_level]);
                if( wfn2.FileExists() )
                {
                    engine.m_file = wfn2.GetFullPath();
                    std::string s = engine.m_file.ToStdString();
                    cprintf( "One time only change to engine file: %s\n", s.c_str() );
                }
            }
        }

        // NonVolatile
        config->Read("NonVolatileX",                      &nv.m_x );
        config->Read("NonVolatileY",                      &nv.m_y );
        config->Read("NonVolatileW",                      &nv.m_w );
        config->Read("NonVolatileH",                      &nv.m_h );
        config->Read("NonVolatileGamesX",                 &nv.m_games_x );
        config->Read("NonVolatileGamesY",                 &nv.m_games_y );
        config->Read("NonVolatileGamesW",                 &nv.m_games_w );
        config->Read("NonVolatileGamesH",                 &nv.m_games_h );
        config->Read("NonVolatilePanel1",                 &nv.m_panel1 );
        config->Read("NonVolatilePanel2",                 &nv.m_panel2 );
        config->Read("NonVolatilePanel3",                 &nv.m_panel3 );
        config->Read("NonVolatilePanel4",                 &nv.m_panel4 );
        config->Read("NonVolatileCol0",                   &nv.m_col0 );
        config->Read("NonVolatileCol1",                   &nv.m_col1 );
        config->Read("NonVolatileCol2",                   &nv.m_col2 );
        config->Read("NonVolatileCol3",                   &nv.m_col3 );
        config->Read("NonVolatileCol4",                   &nv.m_col4 );
        config->Read("NonVolatileCol5",                   &nv.m_col5 );
        config->Read("NonVolatileCol6",                   &nv.m_col6 );
        config->Read("NonVolatileCol7",                   &nv.m_col7 );
        config->Read("NonVolatileCol8",                   &nv.m_col8 );
        config->Read("NonVolatileCol9",                   &nv.m_col9 );
        config->Read("NonVolatileCol10",                  &nv.m_col10 );
        config->Read("NonVolatileCol11",                  &nv.m_col11 );
        config->Read("NonVolatileDocDir",                 &nv.m_doc_dir );
        ReadBool    ("NonVolatileEventNotSite",            nv.m_event_not_site );
    }
#ifdef MAC_FIX_LATER
    book.m_enabled = false; // book doesn't work on Mac
#endif
}

Repository::~Repository()
{
    // Book
    config->Write("BookFile",             book.m_file               );
    config->Write("BookEnabled",          (int)book.m_enabled       );
    config->Write("BookSuggest",          (int)book.m_suggest       );
    config->Write("BookLimitMoves",       book.m_limit_moves        );
    config->Write("BookPostLimitPercent", book.m_post_limit_percent );

    // Player
    config->Write("PlayerHuman",          player.m_human            );
//  config->Write("PlayerComputer",       player.m_computer         );
//  config->Write("PlayerWhite",          player.m_white            );
//  config->Write("PlayerBlack",          player.m_black            );

    // Clock
    config->Write("HumanTime",            clock.m_human_time              );
    config->Write("HumanIncrement",       clock.m_human_increment         );
    config->Write("HumanVisible",         (int)clock.m_human_visible      );
    config->Write("HumanRunning",         (int)clock.m_human_running      );
    config->Write("EngineTime",           clock.m_engine_time             );
    config->Write("EngineIncrement",      clock.m_engine_increment        );
    config->Write("EngineFixedMinutes",   clock.m_engine_fixed_minutes    );
    config->Write("EngineFixedSeconds",   clock.m_engine_fixed_seconds    );
    config->Write("EngineVisible",        (int)clock.m_engine_visible     );
    config->Write("EngineRunning",        (int)clock.m_engine_running     );
    config->Write("FixedPeriodMode",      (int)clock.m_fixed_period_mode  );

//  config->Write("WhiteTime",            clock.m_white_time              );
//  config->Write("WhiteIncrement",       clock.m_white_increment         );
//  config->Write("WhiteVisible",         (int)clock.m_white_visible      );
//  config->Write("WhiteRunning",         (int)clock.m_white_running      );
//  config->Write("BlackTime",            clock.m_black_time              );
//  config->Write("BlackIncrement",       clock.m_black_increment         );
//  config->Write("BlackVisible",         (int)clock.m_black_visible      );
//  config->Write("BlackRunning",         (int)clock.m_black_running      );

    // Log
    config->Write("LogFile",              log.m_file                );
    config->Write("LogEnabled",           (int)log.m_enabled        );

    // Training
    config->Write("TrainingNbrHalfMovesBehind",       training.m_nbr_half_moves_behind            );
    config->Write("TrainingPeekAtCompletePosition",   (int)training.m_peek_at_complete_position   );
    config->Write("TrainingBlindfoldHideWhitePawns",  (int)training.m_blindfold_hide_white_pawns  );
    config->Write("TrainingBlindfoldHideWhitePieces", (int)training.m_blindfold_hide_white_pieces );
    config->Write("TrainingBlindfoldHideBlackPawns",  (int)training.m_blindfold_hide_black_pawns  );
    config->Write("TrainingBlindfoldHideBlackPieces", (int)training.m_blindfold_hide_black_pieces );

    // General
    config->Write("GeneralNotationLanguage",          general.m_notation_language     );
    config->Write("GeneralNoItalics",                 (int)general.m_no_italics       );
    config->Write("GeneralStraightToGame",            (int)general.m_straight_to_game );
    config->Write("GeneralStraightToFirstGame",       (int)general.m_straight_to_first_game );
    config->Write("GeneralFontSize",                  general.m_font_size   );
    config->Write("GeneralHeadingAboveBoard",         (int)general.m_heading_above_board  );
    config->Write("GeneralNoAutoFlip",                (int)general.m_no_auto_flip );
    config->Write("GeneralEmitBellWhenEngineMoves",   (int)general.m_bell  );
    config->Write("GeneralSuppressHighlight",         (int)general.m_suppress_highlight  );
    config->Write("GeneralLineHighlight",             (int)general.m_line_highlight  );
    config->Write("GeneralLightSquaresRed",            general.m_light_colour_r );
    config->Write("GeneralLightSquaresGreen",          general.m_light_colour_g );
    config->Write("GeneralLightSquaresBlue",           general.m_light_colour_b );
    config->Write("GeneralDarkSquaresRed",             general.m_dark_colour_r );
    config->Write("GeneralDarkSquaresGreen",           general.m_dark_colour_g );
    config->Write("GeneralDarkSquaresBlue",            general.m_dark_colour_b );
    config->Write("GeneralHighlightLightSquaresRed",            general.m_highlight_light_colour_r );
    config->Write("GeneralHighlightLightSquaresGreen",          general.m_highlight_light_colour_g );
    config->Write("GeneralHighlightLightSquaresBlue",           general.m_highlight_light_colour_b );
    config->Write("GeneralHighlightDarkSquaresRed",             general.m_highlight_dark_colour_r );
    config->Write("GeneralHighlightDarkSquaresGreen",           general.m_highlight_dark_colour_g );
    config->Write("GeneralHighlightDarkSquaresBlue",            general.m_highlight_dark_colour_b );
    config->Write("GeneralHighlightLineSquaresRed",             general.m_highlight_line_colour_r );
    config->Write("GeneralHighlightLineSquaresGreen",           general.m_highlight_line_colour_g );
    config->Write("GeneralHighlightLineSquaresBlue",            general.m_highlight_line_colour_b );

    // NonVolatile
    config->Write("NonVolatileDefaultsStepper",     DEFAULTS_STEPPER_LEVEL );
    config->Write("NonVolatileX",                   nv.m_x );
    config->Write("NonVolatileY",                   nv.m_y );
    config->Write("NonVolatileW",                   nv.m_w );
    config->Write("NonVolatileH",                   nv.m_h );
    config->Write("NonVolatileGamesX",              nv.m_games_x );
    config->Write("NonVolatileGamesY",              nv.m_games_y );
    config->Write("NonVolatileGamesW",              nv.m_games_w );
    config->Write("NonVolatileGamesH",              nv.m_games_h );
    config->Write("NonVolatilePanel1",              nv.m_panel1 );
    config->Write("NonVolatilePanel2",              nv.m_panel2 );
    config->Write("NonVolatilePanel3",              nv.m_panel3 );
    config->Write("NonVolatilePanel4",              nv.m_panel4 );
    config->Write("NonVolatileCol0",                nv.m_col0 );
    config->Write("NonVolatileCol1",                nv.m_col1 );
    config->Write("NonVolatileCol2",                nv.m_col2 );
    config->Write("NonVolatileCol3",                nv.m_col3 );
    config->Write("NonVolatileCol4",                nv.m_col4 );
    config->Write("NonVolatileCol5",                nv.m_col5 );
    config->Write("NonVolatileCol6",                nv.m_col6 );
    config->Write("NonVolatileCol7",                nv.m_col7 );
    config->Write("NonVolatileCol8",                nv.m_col8 );
    config->Write("NonVolatileCol9",                nv.m_col9 );
    config->Write("NonVolatileCol10",               nv.m_col10 );
    config->Write("NonVolatileCol11",               nv.m_col11 );
    config->Write("NonVolatileDocDir",              nv.m_doc_dir );
    config->Write("NonVolatileEventNotSite",        (int)nv.m_event_not_site );

    // Database
    config->Write("DatabaseFile",                database.m_file       );
    config->Write("DatabaseEloCutoff",           database.m_elo_cutoff );
    config->Write("DatabaseEloCutoffBeforeYear", database.m_elo_cutoff_before_year );
    config->Write("DatabaseEloCutoffIgnore",     (int)database.m_elo_cutoff_ignore );
    config->Write("DatabaseEloCutoffOne",        (int)database.m_elo_cutoff_one    );
    config->Write("DatabaseEloCutoffBoth",       (int)database.m_elo_cutoff_both );
    config->Write("DatabaseEloCutoffFail",       (int)database.m_elo_cutoff_fail );
    config->Write("DatabaseEloCutoffPass",       (int)database.m_elo_cutoff_pass );
    config->Write("DatabaseEloCutoffPassBefore", (int)database.m_elo_cutoff_pass_before );

    // Engine
    config->Write("EngineExeFile",      engine.m_file   );
    config->Write("EnginePonder",       (int)engine.m_ponder     );
    config->Write("EngineNormalPriority",   (int)engine.m_normal_priority );
    config->Write("EngineLowPriority",      (int)engine.m_low_priority);
    config->Write("EngineIdlePriority",     (int)engine.m_idle_priority);
    config->Write("EngineHash",         engine.m_hash            );
    config->Write("EngineMaxCpuCores",  engine.m_max_cpu_cores   );
    config->Write("EngineCustom1a",     engine.m_custom1a        );
    config->Write("EngineCustom1b",     engine.m_custom1b        );
    config->Write("EngineCustom2a",     engine.m_custom2a        );
    config->Write("EngineCustom2b",     engine.m_custom2b        );
    config->Write("EngineCustom3a",     engine.m_custom3a        );
    config->Write("EngineCustom3b",     engine.m_custom3b        );
    config->Write("EngineCustom4a",     engine.m_custom4a        );
    config->Write("EngineCustom4b",     engine.m_custom4b        );
    config->Write("EngineCustom5a",     engine.m_custom5a        );
    config->Write("EngineCustom5b",     engine.m_custom5b        );
    config->Write("EngineCustom6a",     engine.m_custom6a        );
    config->Write("EngineCustom6b",     engine.m_custom6b        );

    // The changes will be written back automatically
    delete config;
}


/*
static void TestStandardPaths()
{
// Results of running this in debug mode during development
//=========================================================
// Config dir (sys):    C:\Documents and Settings\All Users\Application Data\Tarrasch
// Config dir (user):   C:\Documents and Settings\Bill\Application Data
// Data dir (sys):      c:\Documents and Settings\Bill\My Documents\Chess\wx
// Data dir (sys local):    c:\Documents and Settings\Bill\My Documents\Chess\wx
// Data dir (user): C:\Documents and Settings\Bill\Application Data\Tarrasch
// Data dir (user local):   C:\Documents and Settings\Bill\Local Settings\Application Data\Tarrasch
// Documents dir:       C:\Documents and Settings\Bill\My Documents
// Executable path: c:\Documents and Settings\Bill\My Documents\Chess\wx\vc_mswd\Tarrasch.exe
// Plugins dir:     c:\Documents and Settings\Bill\My Documents\Chess\wx
// Resources dir:       c:\Documents and Settings\Bill\My Documents\Chess\wx

    wxStandardPathsBase& stdp = wxStandardPaths::Get();
    dbg_printf( "Config dir (sys):\t%s\n", stdp.GetConfigDir().c_str());
    dbg_printf("Config dir (user):\t%s\n", stdp.GetUserConfigDir().c_str());
    dbg_printf("Data dir (sys):\t\t%s\n", stdp.GetDataDir().c_str());
    dbg_printf("Data dir (sys local):\t%s\n", stdp.GetLocalDataDir().c_str());
    dbg_printf("Data dir (user):\t%s\n", stdp.GetUserDataDir().c_str());
    dbg_printf("Data dir (user local):\t%s\n", stdp.GetUserLocalDataDir().c_str());
    dbg_printf("Documents dir:\t\t%s\n", stdp.GetDocumentsDir().c_str());
    dbg_printf("Executable path:\t%s\n", stdp.GetExecutablePath().c_str());
    dbg_printf("Plugins dir:\t\t%s\n", stdp.GetPluginsDir().c_str());
    dbg_printf("Resources dir:\t\t%s\n", stdp.GetResourcesDir().c_str());
}
*/

// Find the directories we should use
void Repository::SetDirectories()
{
    wxStandardPathsBase& stdp = wxStandardPaths::Get();

    // Defaults to use in case of error
    wxString name = "Tarrasch";
#ifdef WINDOWS_FIX_LATER
    exe_dir = "C:/Program Files (x86)/Tarrasch";
#else
    exe_dir = ".";
#endif

    // Find directories we plan to use
    wxString doc = stdp.GetDocumentsDir();              // eg "C:\Documents and Settings\Bill\My Documents"
    wxString without_end;
    cprintf( "Windows document directory before: %s\n", static_cast<const char *>(doc.c_str()) );
    if( doc.EndsWith("\\",&without_end) )
        doc = without_end;
    cprintf( "Windows document directory after : %s\n", static_cast<const char *>(doc.c_str()) );
    wxString tmp = stdp.GetExecutablePath();            // eg "C:\Program Files\Tarrasch\Tarrasch.exe"
    wxFileName exe(tmp);
    wxArrayString dirs = exe.GetDirs();
    exe_dir = exe.GetPath();
    if( dirs.Count() > 1 )
        name = dirs[dirs.Count()-1];
    if( name=="vc_mswd" || name=="vc_msw" || name=="vc_mswud" || name=="vc_mswu" ) // during development
        name = "Tarrasch";
    if( name.Len() == 0 )
        name = "Tarrasch";
    bool ini_exists = false;
    bool mkdir = false;
    doc_dir = doc + "/" + name;
    ini_filename = exe_dir + "/Tarrasch.ini";
    wxFileName ini1(ini_filename);
    if( ini1.FileExists() )
    {
        ini_exists = true;
        doc_dir = exe_dir;
    }
    else
    {
        if( !wxDirExists(doc_dir) )
        {
            if( wxMkdir(doc_dir) )
                mkdir = true;
            else
                doc_dir = doc;
        }
        ini_filename = doc_dir + "/Tarrasch.ini";
        wxFileName ini2(ini_filename);
        if( !mkdir && ini2.FileExists() )
            ini_exists = true;
    }

    // If .ini file doesn't exist, assume new instalation so copy data files
    //  from exe directory
    std::string s(ini_filename.c_str());
    cprintf( "%s file:%s\n", ini_exists?"true":"false", s.c_str() );
#ifdef WINDOWS_FIX_LATER
    if( !ini_exists )
    {
#if 0
        bool okay = wxRenameFile( exe_dir+"/book.pgn", doc_dir+"/book.pgn" );   // rename === move
        cprintf( "Move book.pgn %s\n", okay?"okay":"error" );
        okay = wxRenameFile( exe_dir+"/book.pgn_compiled", doc_dir+"/book.pgn_compiled" );
        cprintf( "Move book.pgn_compiled %s\n", okay?"okay":"error" );
#endif
        // rename can fail embarrassingly for permissions reasons
        bool okay = wxCopyFile( exe_dir+"/book.pgn", doc_dir+"/book.pgn" );
        cprintf( "Copy book.pgn %s\n", okay?"okay":"error" );
        okay = wxCopyFile( exe_dir+"/book.pgn_compiled", doc_dir+"/book.pgn_compiled" );
        cprintf( "Copy book.pgn_compiled %s\n", okay?"okay":"error" );
    }
#endif

    // Hopefully one time only, copy (actually move) original default.tdb
    wxString exe_db = exe_dir + "/" + DEFAULT_DATABASE;
    wxString doc_db = doc_dir + "/" + DEFAULT_DATABASE;
    wxFileName fn_exe_db(exe_db);
    wxFileName fn_doc_db(doc_db);
    bool doc_exist = fn_doc_db.FileExists();
    bool exe_exist = fn_exe_db.FileExists();
    bool replace   = (!doc_exist && exe_exist);
    if( doc_exist && exe_exist )
    {
        wxDateTime doc_t = fn_doc_db.GetModificationTime();
        wxDateTime exe_t = fn_exe_db.GetModificationTime();
        if( exe_t.IsLaterThan(doc_t) )
            replace = true;
    }
    if( replace )
    {
        const char *exe_db_c = exe_db.c_str();
        const char *doc_db_c = doc_db.c_str();
        cprintf( "wxCopyFile(\"%s\",\"%s\") IN\n", exe_db_c, doc_db_c );
        bool okay = wxCopyFile( exe_db, doc_db );
        cprintf( "wxCopyFile() OUT %s\n", okay?"okay":"error" );
    }
}
