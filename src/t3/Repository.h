/****************************************************************************
 * Persistent storage of config information
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef REPOSITORY_H
#define REPOSITORY_H
#include "Appdefs.h"
#include "wx/wx.h"
#include "wx/utils.h"
#ifdef __VISUALC__
    #pragma warning ( disable : 4005 )  // refinition of wxUSE_CONFIG_NATIVE is okay
#endif
#undef  wxUSE_CONFIG_NATIVE
#define wxUSE_CONFIG_NATIVE 0
#include "wx/config.h"      //(to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"    //(base config class)
#include "wx/fileconf.h"    //(wxFileConfig class)
#include "wx/msw/regconf.h" //(wxRegConfig class)
#define wxUSE_CONFIG_NATIVE 0
#ifdef __VISUALC__
    #pragma warning ( default : 4005 )
#endif

struct BookConfig
{
    wxString    m_file;
    bool        m_enabled;
    bool        m_suggest;
    int         m_limit_moves;
    int         m_post_limit_percent;
    BookConfig()
    {
        m_file           = "book.pgn";
        m_enabled        = true;
        m_suggest        = false;
        m_limit_moves    = 8;
        m_post_limit_percent = 50;
    }
};

struct PlayerConfig
{
    wxString    m_human;
    wxString    m_computer;
    wxString    m_white;
    wxString    m_black;
    PlayerConfig()
    {
        wxString name = ::wxGetUserId();
        if( name.Len() < 2 )
            name = "Human";
        m_human        = name;
        m_computer     = "Computer";
        m_white        = "";
        m_black        = "";
    }
};

struct ClockConfig
{
    int         m_human_time;
    int         m_human_increment;
    bool        m_human_visible;
    bool        m_human_running;
    int         m_engine_time;
    int         m_engine_increment;
    bool        m_engine_visible;
    bool        m_engine_running;
    int         m_white_time;
    int         m_white_secs;
    int         m_white_increment;
    bool        m_white_visible;
    bool        m_white_running;
    int         m_black_time;
    int         m_black_secs;
    int         m_black_increment;
    bool        m_black_visible;
    bool        m_black_running;
    ClockConfig()
    {
        m_human_time          = 25;
        m_human_increment     = 5;
        m_human_visible       = true;
        m_human_running       = true;
        m_engine_time         = 3;
        m_engine_increment    = 1;
        m_engine_visible      = true;
        m_engine_running      = true; 
        m_white_time          = 90;
        m_white_secs          = 0;
        m_white_increment     = 30;
        m_white_visible       = false;
        m_white_running       = false;
        m_black_time          = 90;
        m_black_secs          = 0;
        m_black_increment     = 30;
        m_black_visible       = false;
        m_black_running       = false;
    }
};

struct LogConfig
{
    wxString    m_file;
    bool        m_enabled;
    LogConfig()
    {
        m_file           = "log.pgn";
        m_enabled        = true;
    }
};

struct EngineConfig
{
    wxString    m_file;
    bool        m_ponder;
    int         m_hash;
    int         m_max_cpu_cores;
    wxString    m_custom1a;
    wxString    m_custom1b;
    wxString    m_custom2a;
    wxString    m_custom2b;
    wxString    m_custom3a;
    wxString    m_custom3b;
    wxString    m_custom4a;
    wxString    m_custom4b;
    EngineConfig()
    {
        m_file           = DEFAULT_ENGINE;
        m_ponder         = false;
        m_hash           = 64;
        m_max_cpu_cores  = 1;
        m_custom1a       = "";
        m_custom1b       = "";
        m_custom2a       = "";
        m_custom2b       = "";
        m_custom3a       = "";
        m_custom3b       = "";
        m_custom4a       = "";
        m_custom4b       = "";
    }
};

struct TrainingConfig
{
    int         m_nbr_half_moves_behind;
    bool        m_peek_at_complete_position;
    bool        m_blindfold_hide_white_pawns;
    bool        m_blindfold_hide_white_pieces;
    bool        m_blindfold_hide_black_pawns;
    bool        m_blindfold_hide_black_pieces;
    TrainingConfig()
    {
        m_nbr_half_moves_behind       = 0;
        m_peek_at_complete_position   = false;
        m_blindfold_hide_white_pawns  = false;
        m_blindfold_hide_white_pieces = false;
        m_blindfold_hide_black_pawns  = false;
        m_blindfold_hide_black_pieces = false;
    }
};

struct GeneralConfig
{
    wxString    m_notation_language;
    bool        m_no_italics;
    bool        m_straight_to_game;
    bool        m_small_board;
    bool        m_large_font;
    bool        m_no_auto_flip;
    GeneralConfig()
    {
        m_notation_language  = "KQRNB (English)";
        m_no_italics         = false;
        m_straight_to_game   = false;
        m_small_board        = false;
        m_large_font         = false;
        m_no_auto_flip       = false;
    }
};

// Miscellaneous, non-user editable non volatile stuff
struct NonVolatile
{
    int         m_x;
    int         m_y;
    int         m_w;
    int         m_h;
    int         m_col0;
    int         m_col1;
    int         m_col2;
    int         m_col3;
    int         m_col4;
    int         m_col5;
    int         m_col6;
    int         m_col7;
    int         m_col8;
    int         m_col9;
    int         m_col10;
    wxString    m_doc_dir;
    NonVolatile()
    {
        m_x = -1;
        m_y = -1;
        m_w = -1;
        m_h = -1;
        m_col0 = -1;
        m_col1 = -1;
        m_col2 = -1;
        m_col3 = -1;
        m_col4 = -1;
        m_col5 = -1;
        m_col6 = -1;
        m_col7 = -1;
        m_col8 = -1;
        m_col9 = -1;
        m_col10 = -1;
        m_doc_dir = "";
    }
};

class Repository
{
public:
    Repository( bool use_defaults=false );
    ~Repository();
    BookConfig      book;
    LogConfig       log;
    PlayerConfig    player;
    ClockConfig     clock;
    TrainingConfig  training;
    GeneralConfig   general;
    EngineConfig    engine;
    NonVolatile     nv;

private:
    wxString exe_dir;
    wxString doc_dir;
    wxString ini_filename;
    void SetDirectories();
    wxConfig *config;
    void ReadBool( const char *key, bool &value )
    {
        int temp;
        if( config->Read( key, &temp ) )
            value = (temp?true:false);
    }
};

#endif
