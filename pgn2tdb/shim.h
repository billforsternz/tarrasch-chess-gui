//
//  Adapt Tarrasch code to command line environment for PGN2TDB
//

#ifndef SHIM_H_INCLUDED
#define SHIM_H_INCLUDED

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <windows.h>
int cprintf( const char *fmt, ... );
#define THC_WINDOWS
#include "AutoTimer.h"

#define wxString std::string
#define wxWindow std::string
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

bool progress_update( int permill, std::string="" );
std::string macro_substitution( const std::string &input,
                                const std::map<char,std::string> &macros,
                                const std::vector<std::pair<std::string,std::string>> &menu, int menu_idx );
std::string md( const std::string &in );

class ProgressBar
{
public:
    ProgressBar( std::string title, std::string desc, bool abortable=true, wxWindow *parent=NULL, FILE *ifile=0  ) :
        timer(NULL)
    {
        this->title = title;
        this->desc = desc;
        this->abortable = abortable;
        this->parent = parent;
        this->ifile = ifile;
        this->ifile = ifile;
        if( ifile )
        {
            fseek(ifile,0,SEEK_END);
            file_len = ftell(ifile);
            fseek(ifile,0,SEEK_SET);
        }
        modulo_256=0;
        old_permill=0;
    }

    ~ProgressBar()
    {
    }

    void DrawNow()
    {
    }

    bool ProgressFile() // return true if abort
    {
        if( ifile )
        {
            if( modulo_256 == 0 )
            {
                unsigned long file_offset=ftell(ifile);
                int permill=0;
                if( file_len == 0 )
                    permill = 1000;
                else if( file_len < 1000000 )
                    permill = (int)( (file_offset*1000L) / file_len );
                else // if( file_len > 1000000 )
                    permill = (int)( file_offset / (file_len/1000L) );
                if( permill != old_permill )
                {
                    bool abort = progress_update( permill>1000 ? 1000 : permill );
                    if( abort )
                        return true;
                }
                old_permill = permill;
            }
            modulo_256++;
            modulo_256 &= 0xff;
        }
        return false;
    }

    // If after time T1 we have completed proportion P of the task, then how long T do we expect the task to take ?
    //  T1/T = P
    //  => T = T1/P
    //  If p is permill, then p = P*1000
    //  => T = T1/(p/1000)
    //  => T = 1000*T1/p, where T1 and T have the same units (milliseconds in this case)
    double PredictEnd( int permill )
    {
        if( permill == 0 )
            permill = 1;
        double elapsed = timer.Elapsed();
        return (1000*elapsed) / permill;
    }

    bool Percent( int percent, const std::string &s="" )  // return true if abort
    {
        return Permill( percent*10, s );
    }
    bool Permill( int permill, const std::string &s="" )  // return true if abort
    {
        if( permill<0 || permill>1000 )
            permill = old_permill;
        bool abort = false;
        if( old_permill != permill )
        {
            {
                wxString wxmsg( s.c_str() );
                SetFocusChecked();
                abort = !progress_update( permill>1000 ? 1000 : permill, wxmsg );
                old_permill = permill;
            }
        }
        return abort;
    }

    bool Perfraction( int numerator, int denominator, const std::string &s="" )  // return true if abort
    {
        int permill=1000;
        if( numerator>=denominator || denominator==0 )
            permill = 1000;
        else if( denominator > 1000000 )
            permill = numerator / (denominator/1000);
        else
            permill = (numerator*1000) / denominator;
        return Permill( permill, s );
    }

private:
    unsigned char modulo_256;
    int old_permill;
    FILE *ifile;
    AutoTimer timer;
    unsigned long file_len;
    std::string title;
    std::string desc;
    bool abortable;
    wxWindow *parent;
    void SetFocusChecked()
    {
    }
};

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
    int         m_engine_fixed_minutes;
    int         m_engine_fixed_seconds;
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
    bool        m_fixed_period_mode;
    int         m_engine_minutes;
    int         m_engine_seconds;
    ClockConfig()
    {
        m_human_time          = 25;
        m_human_increment     = 5;
        m_human_visible       = true;
        m_human_running       = true;
        m_engine_time         = 3;
        m_engine_increment    = 1;
        m_engine_fixed_minutes= 0;
        m_engine_fixed_seconds= 5;
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
        m_fixed_period_mode   = false;
        m_engine_minutes      = 3;  // Clock Dialog edits these, m_fixed_period_mode determines
        m_engine_seconds      = 1;  //  whether they represent m_engine_time or m_engine_fixed_minutes etc.
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

bool Is64BitWindows();
struct EngineConfig
{
    wxString    m_file;
    bool        m_ponder;
    bool        m_normal_priority;
    bool        m_low_priority;
    bool        m_idle_priority;
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
    wxString    m_custom5a;
    wxString    m_custom5b;
    wxString    m_custom6a;
    wxString    m_custom6b;
    EngineConfig()
    {
        m_ponder         = false;
        m_normal_priority = true;
        m_low_priority   = false;
        m_idle_priority  = false;
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
        m_custom5a       = "";
        m_custom5b       = "";
        m_custom6a       = "";
        m_custom6b       = "";
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

struct DatabaseConfig
{
    wxString    m_file;
    int         m_elo_cutoff;
    bool        m_elo_cutoff_ignore;
    bool        m_elo_cutoff_one;
    bool        m_elo_cutoff_both;
    bool        m_elo_cutoff_fail;
    bool        m_elo_cutoff_pass;
    bool        m_elo_cutoff_pass_before;
    int         m_elo_cutoff_before_year;
    DatabaseConfig()
    {
        m_file = DEFAULT_DATABASE;
        m_elo_cutoff = 2000;
        m_elo_cutoff_ignore = false;
        m_elo_cutoff_one = true;
        m_elo_cutoff_both = false;
        m_elo_cutoff_fail = false;
        m_elo_cutoff_pass = true;
        m_elo_cutoff_pass_before = false;
        m_elo_cutoff_before_year = 1990;
    }
};

struct GeneralConfig
{
    wxString    m_notation_language;
    bool        m_no_italics;
    bool        m_straight_to_game;
    bool        m_straight_to_first_game;
    int         m_light_colour_r;
    int         m_light_colour_g;
    int         m_light_colour_b;
    int         m_dark_colour_r;
    int         m_dark_colour_g;
    int         m_dark_colour_b;
    int         m_highlight_light_colour_r;
    int         m_highlight_light_colour_g;
    int         m_highlight_light_colour_b;
    int         m_highlight_dark_colour_r;
    int         m_highlight_dark_colour_g;
    int         m_highlight_dark_colour_b;
    int         m_highlight_line_colour_r;
    int         m_highlight_line_colour_g;
    int         m_highlight_line_colour_b;
    int         m_font_size;
    bool        m_heading_above_board;
    bool        m_no_auto_flip;
    bool        m_bell;
    bool        m_suppress_highlight;
    bool        m_line_highlight;
    GeneralConfig()
    {
        m_notation_language  = "KQRNB (English)";
        // Note that the following colour constants violate DRY (Don't Repeat Yourself) - search project
        //  for "violate DRY" to find out why
        m_light_colour_r           = 255;   // cream
        m_light_colour_g           = 226;
        m_light_colour_b           = 179;
        m_dark_colour_r            = 220;   // brown
        m_dark_colour_g            = 162;
        m_dark_colour_b            = 116;
        m_highlight_light_colour_r = 250;   // very light yellow
        m_highlight_light_colour_g = 240;
        m_highlight_light_colour_b = 160;
        m_highlight_dark_colour_r  = 196;   // darker brown
        m_highlight_dark_colour_g  = 160;
        m_highlight_dark_colour_b  = 130;
        m_highlight_line_colour_r  = 112;   // nice ocean blue
        m_highlight_line_colour_g  = 146;
        m_highlight_line_colour_b  = 190;
        m_suppress_highlight = false;
        m_line_highlight = false;
        m_no_italics         = false;
        m_straight_to_game   = false;
        m_straight_to_first_game = false;
        m_bell               = false;
        m_font_size          = 9;
        m_heading_above_board= false;
        m_no_auto_flip       = false;
    }
};

// Miscellaneous, non-user editable non volatile stuff
struct NonVolatile
{
    int         m_defaults_stepper;
    int         m_x;
    int         m_y;
    int         m_w;
    int         m_h;
    int         m_panel1;
    int         m_panel2;
    int         m_panel3;
    int         m_panel4;
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
    int         m_col11;
    int         m_games_x;
    int         m_games_y;
    int         m_games_w;
    int         m_games_h;
    wxString    m_doc_dir;
    bool        m_event_not_site;
    NonVolatile()
    {
        m_x = -1;
        m_y = -1;
        m_w = -1;
        m_h = -1;
        m_games_x = -1;
        m_games_y = -1;
        m_games_w = -1;
        m_games_h = -1;
        m_panel1 = -1;
        m_panel2 = -1;
        m_panel3 = -1;
        m_panel4 = -1;
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
        m_col11 = -1;
        m_doc_dir = "";
        m_event_not_site = false;
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
    DatabaseConfig  database;
    NonVolatile     nv;

private:
    wxString exe_dir;
    wxString doc_dir;
    wxString ini_filename;
    void SetDirectories();
    void ReadBool( const char *key, bool &value )
    {
    }
};


#endif // SHIM_H_INCLUDED
