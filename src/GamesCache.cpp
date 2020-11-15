/****************************************************************************
 * Games Cache - Abstracted interface to  a list of games
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <time.h> // time_t
#include <stdio.h>
#include <set>
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/listctrl.h"
#include "Appdefs.h"
#include "DebugPrintf.h"
#include "thc.h"
#include "GameLogic.h"
#include "DialogDetect.h"
#include "Objects.h"
#include "Repository.h"
#include "PgnFiles.h"
#include "Lang.h"
#include "ListableGamePgn.h"
#include "PgnRead.h"
#include "ProgressBar.h"
#include "Log.h"
#include "Eco.h"
#include "GamesCache.h"
using namespace std;

bool PgnStateMachine( FILE *pgn_file, int &typ, char *buf, int buflen );

static void FileConflictError( const wxString &msg,  bool unconditional=false );
static void FileConflictError( bool unconditional=false )
{
    wxString default_msg =
        "Error: An external program may have modified a PGN file or files Tarrasch is using.\n" 
        "Please save any work in progress to a fresh file and restart Tarrasch (i.e. shut it down and and start it again).\n";
    FileConflictError( default_msg, unconditional );
}

static void FileConflictError( const wxString &msg, bool unconditional )
{
    static unsigned long previous = 0;
    unsigned long now = GetTickCount();
    if( unconditional || previous==0 || now-previous>10000 ) // no more than once every 10 seconds
        wxMessageBox( msg, "File usage conflict detected\n", wxOK|wxICON_ERROR );
    previous = now;
}

#if 0
static bool operator < (const smart_ptr<GameDocument>& left,
                        const smart_ptr<GameDocument>& right)
{
    bool result = ( *left < *right );
    cprintf( "operator <; left->r.white=%s, right->r.white=%s, result=%s\n", left->r.white.c_str(),  right->r.white.c_str(), result?"true":"false" );
    return result;
}
#endif


bool GamesCache::Load(std::string &filename )
{
    file_irrevocably_modified = false;
    loaded = false;
    pgn_filename = "";
    FILE *pgn_file = objs.gl->pf.OpenRead( filename, pgn_handle );
    if( pgn_file )
    {
        loaded = Load(pgn_file);
        if( loaded )
            pgn_filename = filename;
        objs.gl->pf.Close();
    }
    return loaded;
}

bool GamesCache::IsLoaded()
{
    return loaded;
}

static GamesCache *gc_fixme;

bool PgnStateMachine( FILE *pgn_file, int &typ, char *buf, int buflen )
{
    static enum {INIT,PREFIX,TAGLINES,PRE_MOVES,MOVES,SEARCH} state;
    bool done=false;
    typ = ' ';  // no-op
    if( pgn_file == NULL )
    {
        state = INIT;
    }
    else
    {
        char *null_if_eof = fgets( buf, buflen-2, pgn_file );
        if( null_if_eof == NULL )
        {
            done = true;
            if( state==MOVES )
                typ = 'G';  // completed game;
        }
        else
        {
            const char *p = buf;
            while( *p==' ' || *p=='\t' )
                p++;
            bool blank   = (*p=='\n'||*p=='\r');
            bool tagline = (*p=='[');

            // Check that it really is a tagline
            if( tagline )
            {
                tagline = false;  // unless all checks pass
                
                // Skip '['
                p++;
                
                // Skip whitespace
                while( *p==' ' || *p=='\t' )
                    p++;
                
                // Is there a tag before a leading " ?
                bool tag=false;
                while( *p && *p!=']' && *p!=' ' && *p!='\t' && *p!='\"' )
                {
                    tag = true;    // at least 1 non-whitespace
                    p++;
                }
                if( tag )
                {
                    
                    // Make sure there is whitespace, but skip it
                    tag = false;
                    while( *p==' ' || *p=='\t' )
                    {
                        tag = true;  // at least 1 whitespace
                        p++;
                    }
                }
                
                // If there is a tag, then whitespace, then a leading "
                if( tag && *p=='\"')
                {
                    p++;
                    
                    // Skip to 2nd " or end of string
                    while( *p && *p!='\"' )
                        p++;
                    
                    // If we have a 2nd " then we have a tag and a val, i.e. a header
                    if( *p == '\"' )
                    {
                        tagline = true;
                    }
                }
            }
            switch( state )
            {
                case INIT:
                case SEARCH:
                {
                    if( tagline )
                    {
                        state = TAGLINES;
                        typ = 'T';   // first tagline
                    }
                    else if( !blank )
                    {
                        state = PREFIX;
                        typ = 'P';  // first prefix line
                    }
                    break;
                }
                case PREFIX:
                {
                    if( tagline )
                    {
                        state = TAGLINES;
                        typ = 'T';   // first tagline
                    }
                    else
                    {
                        typ = 'p';  // next prefix line
                    }
                    break;
                }
                case TAGLINES:
                {
                    if( tagline )
                    {
                        typ = 't';   // next tagline
                    }
                    else if( blank )
                    {
                        state = PRE_MOVES;
                    }
                    else
                    {
                        state = MOVES;
                        typ = 'M';    // first line of moves, (a pity there wasn't a blank line before them)
                    }
                    break;
                }
                case PRE_MOVES:
                {
                    if( !blank )
                    {
                        state = MOVES;
                        typ = 'M';  // first line of moves, after one or more blank lines
                    }
                    break;
                }
                case MOVES:
                {
                    if( blank )
                    {
                        state = SEARCH;
                        typ = 'G';  // completed game
                    }
                    else
                    {
                        typ = 'm';  // next line of moves
                    }
                    break;
                }
            }
        }
    }
    return done;
}

bool GamesCache::Load( FILE *pgn_file )
{
    cprintf( "GamesCache::Load() begin\n" );
    gds.clear();
    int game_count=0;
    gc_fixme = this;
    file_irrevocably_modified = false;
    int typ;
    std::string str;
    long fposn = ftell(pgn_file);
    char buf[2048];
    std::string title("Scanning .pgn file for games");
    std::string desc("Reading .pgn file");
    bool abortable=false;
    wxWindow *parent=NULL;
    ProgressBar pb( title, desc, abortable, parent, pgn_file );
    bool done = PgnStateMachine( NULL, typ,  buf, sizeof(buf) );
    while( !done )
    {
        done = PgnStateMachine( pgn_file, typ, buf, sizeof(buf) );
        if( typ == 'G' )
        {
            ListableGamePgn pgn_document(pgn_handle,fposn);
            make_smart_ptr( ListableGamePgn, new_doc, pgn_document );
            gds.push_back( std::move(new_doc) );
            if( !done )
                fposn = ftell(pgn_file);
            game_count++;
        }
        pb.ProgressFile();
    }
    uint32_t game_id = GameIdAllocateBottom( gds.size() );
    for( std::vector<smart_ptr<ListableGame>>::iterator iter = gds.begin();
         iter != gds.end(); iter++ )
    {
        (*iter)->game_id = game_id++;
    }
    cprintf( "GamesCache::Load() end count = %d\n", game_count );
    return true;
}

static CompactGame *phook;
void pgn_read_hook( const char *fen, const char *white, const char *black, const char *event, const char *site, const char *result,
                                    const char *date, const char *white_elo, const char *black_elo, const char *eco, const char *round,
                                    int nbr_moves, thc::Move *moves, uint64_t *UNUSED(hashes)  )
{
    phook->r.fen       = fen ? std::string(fen) : "";
    phook->r.white     = std::string(white);
    phook->r.black     = std::string(black);
    phook->r.event     = std::string(event);
    phook->r.site      = std::string(site);
    phook->r.round     = std::string(round);
    phook->r.result    = std::string(result);
    phook->r.date      = std::string(date);
    phook->r.eco       = std::string(eco);
    phook->r.white_elo = std::string(white_elo);
    phook->r.black_elo = std::string(black_elo);
    phook->moves       = std::vector<thc::Move>(moves,moves+nbr_moves);
}

void *ReadGameFromPgnInLoop( int pgn_handle, long fposn, CompactGame &pact, void *context, bool end )
{
	static int new_count;
    static FILE *pgn_file;
    static int save_pgn_handle;
    PgnRead *pgn;
    if( context )
        pgn = static_cast<PgnRead*>( context );
    else
	{
        pgn = new PgnRead('R'); 
		cprintf( "new count = %d\n", ++new_count );
	}
    if( pgn_file && pgn_handle!=save_pgn_handle )
    {
        objs.gl->pf.Close();
        pgn_file = NULL;
        save_pgn_handle = 0;
    }
    if( !pgn_file )
    {
        pgn_file  = objs.gl->pf.ReopenRead( pgn_handle );
        save_pgn_handle = pgn_handle;
    }
    if( pgn_file )
    {
        fseek( pgn_file, fposn, SEEK_SET );
        phook = &pact;
        pgn->Process(pgn_file);
    }
    else
    {
        // This shouldn't happen
        CompactGame empty;
        pact = empty;
        FileConflictError();
    }
    if( end )
    {
        objs.gl->pf.Close();
        pgn_file = NULL;
        save_pgn_handle = 0;
        delete pgn;
		cprintf( "new count = %d\n", --new_count );
        pgn = 0;
    }
    //cprintf( "ReadGameFromPgnInLoop(%d,%s) %ld (%s-%s)\n", pgn_handle, end?"true":"false", fposn, pact.r.white.c_str(), pact.r.black.c_str() );
    return( pgn );
}

void ReadGameFromPgn( int pgn_handle, long fposn, GameDocument &new_doc )
{
    //cprintf( "ReadGameFromPgn(%d) %ld\n", pgn_handle, fposn );
    GameDocument gd;
    FILE *pgn_file = objs.gl->pf.ReopenRead( pgn_handle );
    std::string moves;
    int typ;
    if( !pgn_file )
    {
        // This shouldn't happen
        GameDocument empty;
        new_doc = empty;
        FileConflictError();
        objs.gl->pf.Close();
        return;
    }
    fseek( pgn_file, fposn, SEEK_SET );
    char buf[2048];
    bool done = PgnStateMachine( NULL, typ,  buf, sizeof(buf) );
    while( !done )
    {
        done = PgnStateMachine( pgn_file, typ,  buf, sizeof(buf) );
        switch( typ )
        {
            default:
            {
                break;
            }
            case 'T':
            case 't':
            {
                gc_fixme->Tagline( static_cast<GameDocument &>(gd), buf );
                break;
            }
            case 'P':
            case 'p':
            {
				gd.prefix_txt += std::string(buf);
				//gd.prefix_txt += '\n';
                break;
            }
            case 'M':
            case 'm':
            {
                moves += std::string(buf);
                break;
            }
            case 'G':
            {
                done = true;
                break;
            }
        }
    }
    thc::ChessRules cr;
    int nbr_converted;
    gd.PgnParse(true,nbr_converted,moves,cr,NULL);
    gd.fposn0 = fposn;
    gd.SetPgnHandle(pgn_handle);
    new_doc = gd;
    objs.gl->pf.Close();
}



// Check whether text s is a valid header, return true if it is,
//  add info to a GameDocument, optionally clearing it first
bool GamesCache::Tagline( GameDocument &gd,  const char *s )
{
    const char *tag_begin, *tag_end, *val_begin, *val_end;
    bool is_header = false;

    // Skip '['
    s++;

    // Skip whitespace
    while( *s==' ' || *s=='\t' )
        s++;

    // Is there a tag before a leading " ?
    tag_begin = s;
    bool tag=false;
    while( *s && *s!=']' && *s!=' ' && *s!='\t' && *s!='\"' )
    {
        tag = true;    // at least 1 non-whitespace
        s++;
    }
    tag_end = s;
    if( tag )
    {

        // Make sure there is whitespace, but skip it
        tag = false;
        while( *s==' ' || *s=='\t' )
        {
            tag = true;  // at least 1 whitespace
            s++;
        }
    }

    // If there is a tag, then whitespace, then a leading "
    if( tag && *s=='\"')
    {
        s++;
        val_begin = s;

        // Skip to 2nd " or end of string
        while( *s && *s!='\"' )
            s++;

        // If we have a 2nd " then we have a tag and a val, i.e. a header
        if( *s == '\"' )        
        {
            is_header = true;
            val_end = s;
            string stag(tag_begin,tag_end-tag_begin);
            string val(val_begin,val_end-val_begin);
            if( stag == "White" )
                gd.r.white = val;
            if( stag == "Black" )
                gd.r.black = val;
            if( stag == "Event" )
                gd.r.event = val;
            if( stag == "Site" )
                gd.r.site = val;
            if( stag == "Date" )
                gd.r.date = val;
            if( stag == "Round" )
                gd.r.round = val;
            if( stag == "Result" )
                gd.r.result = val;
            if( stag == "ECO" )
                gd.r.eco = val;
            if( stag == "WhiteElo" )
                gd.r.white_elo = val;
            if( stag == "BlackElo" )
                gd.r.black_elo = val;
            if( stag == "FEN" )
            {
                gd.r.fen = val;
                gd.start_position.Forsyth(val.c_str());
            }
        }
    }
    return is_header;
}

// Create a new file
void GamesCache::FileCreate( std::string &filename )
{
    loaded = false;
    pgn_filename = "";
    FILE *pgn_out = objs.gl->pf.OpenCreate( filename, pgn_handle );
    if( pgn_out )
    {
        loaded = true;
        pgn_filename = filename;
		wxString wx_filename(filename.c_str());
		objs.gl->mru.AddFileToHistory( wx_filename );
        FileSaveInner( pgn_out );
        objs.gl->pf.Close();    // close all handles
    }
}

// Save the existing file
void GamesCache::FileSave( GamesCache *gc_clipboard )
{
    FILE *pgn_in;
    FILE *pgn_out;
    bool ok = objs.gl->pf.ReopenModify( pgn_handle, pgn_in, pgn_out, gc_clipboard );
    if( !ok )
    {
        FileConflictError();
    }
    else
    {
        FileSaveInner( pgn_out );
        objs.gl->pf.Close( gc_clipboard );    // close all handles
    }
}

// Save the existing file with a new name
void GamesCache::FileSaveAs( std::string &filename, GamesCache *gc_clipboard )
{
    FILE *pgn_in;
    FILE *pgn_out;
    bool ok = objs.gl->pf.ReopenCopy( pgn_handle, filename, pgn_in, pgn_out, gc_clipboard );
    if( !ok )
    {
        FileConflictError();
    }
    else
    {
        pgn_filename = filename;
		wxString wx_filename(filename.c_str());
		objs.gl->mru.AddFileToHistory( wx_filename );
        FileSaveInner( pgn_out );
        objs.gl->pf.Close( gc_clipboard );    // close all handles
    }
}

// Save the current game to a file
#if 0
void GamesCache::FileSaveGameAs( std::string &filename, GamesCache *gc_clipboard )
{
}
#endif

// Save all as a new file
void GamesCache::FileSaveAllAsAFile( std::string &filename )
{
    FILE *pgn_out = objs.gl->pf.OpenCreate( filename, pgn_handle );
    if( pgn_out )
    {
        FileSaveInner( pgn_out );
        objs.gl->pf.Close();    // close all handles
    }
}

bool GamesCache::TestGameInCache( const GameDocument &gd )
{
    bool in_cache=false;
    for( unsigned int i=0; i<gds.size(); i++ )
    {
        ListableGame *ptr = gds[i].get();
        if( ptr && ptr->GetGameBeingEdited()==gd.game_being_edited && gd.game_being_edited )
        {
            in_cache = true;
            break;
        }
    }
    return in_cache;
}


// Save common
void GamesCache::FileSaveInner( FILE *pgn_out )
{
    char *buf;
	GameDocument gd_temp;
    int buflen=100;
    file_irrevocably_modified = false;
    buf = new char [buflen];
    int gds_nbr = gds.size();
    long write_posn=0;
	bool saving_work_file = (this==&objs.gl->gc_pgn);
    int nbr_locked=0, nbr_game_documents=0, nbr_emergency_games_written=0;
    int nbr_unavailable_games=0, nbr_unavailable_files=0;
    std::set<int> handles;
    std::set<int> unavailable_handles;
    for( int i=0; i<gds_nbr; i++ )
    {
        ListableGame *mptr = gds[i].get();
        if( mptr->TestLocked() )
            nbr_locked++;
        if( mptr->IsGameDocument() && !mptr->IsGameDocument()->IsEmpty() )
            nbr_game_documents++;
		int pgn_handle2;
		bool is_pgn = mptr->GetPgnHandle(pgn_handle2);
        if( is_pgn )
            handles.insert(pgn_handle2);
    }
    int expected_total_games = gds_nbr;
    bool restrictions_apply = (nbr_locked > DATABASE_LOCKABLE_LIMIT);
    if( restrictions_apply )
    {
        int expected_omissions = nbr_locked - DATABASE_LOCKABLE_LIMIT;
        expected_total_games -= expected_omissions;
        cprintf( "expected_total_games=%d\n", expected_total_games );
    }
    bool unavailable_handles_found = false;
    for( int handle: handles )
    {
        if( !objs.gl->pf.IsAvailable(handle))
        {
            nbr_unavailable_files++;
            unavailable_handles.insert(handle);
            unavailable_handles_found = true;
        }
    }
    ProgressBar pb( "Saving file", restrictions_apply?"Saving file (restrictions apply)":"Saving file" );
    bool reached_limit=false;
    int count_to_limit=0, nbr_omitted=0, count_to_expected_total_games=0;
    nbr_locked=0;
    for( int i=0; i<gds_nbr; i++ )
    {
        bool abort = pb.Perfraction( count_to_expected_total_games, expected_total_games );
        if( abort )
            break;
        ListableGame *mptr = gds[i].get();
        if( mptr->TestLocked()  )
        {
            nbr_locked++;
            if( reached_limit )
            {
                nbr_omitted++;
                continue;
            }
            else
            {
                if( count_to_limit < DATABASE_LOCKABLE_LIMIT )
                {
                    count_to_limit++;
                    if( count_to_limit >= DATABASE_LOCKABLE_LIMIT )
                        reached_limit = true;
                }
            }
        }
        count_to_expected_total_games++;
        mptr->saved = true;
		int pgn_handle2;
		bool is_pgn = mptr->GetPgnHandle(pgn_handle2);
		long game_len = 0;
        if( is_pgn )
        {

            // Just skip over unavailable games
            if( unavailable_handles.find(pgn_handle2) == unavailable_handles.end() )
            {

                // Read data from a .pgn
                long fposn = mptr->GetFposn();

                // If we are copying the file, update the position to be the new write position in the file
                if( pgn_handle == pgn_handle2 )
                    mptr->SetFposn( write_posn );

                // Get FILE * for reading - note this doesn't usually require a new fopen()
                FILE *pgn_in2 = objs.gl->pf.ReopenRead( pgn_handle2);
                if( pgn_in2 )
                {
                    fseek( pgn_in2, fposn, SEEK_SET );
                    char buf2[2048];
                    int typ;
                    bool done = PgnStateMachine( NULL, typ,  buf2, sizeof(buf2) );
                    while( !done )
                    {
                        done = PgnStateMachine( pgn_in2, typ,  buf2, sizeof(buf2) );
                        if( !done )
                        {
                            fputs( buf2, pgn_out );
                            game_len += strlen(buf2);
                            if( typ == 'G' )
                                break;
                        }
                    }
                }
            }
        }
		else
		{
			GameDocument *ptr = mptr->IsGameDocument();
			GameDocument *save_changes_back_to_tab = NULL;
			if( ptr && saving_work_file )
			{
				GameDocument *pd;
				Undo *pu;
				int handle = objs.tabs->Iterate(0,pd,pu);
				while( pd && pu )
				{
					if( ptr->game_being_edited!=0 && (ptr->game_being_edited == pd->game_being_edited)  )
					{
						*ptr = *pd;
						if( pu )
							pu->Clear(*ptr);
						save_changes_back_to_tab = pd;
						break;
					}
					objs.tabs->Iterate(handle,pd,pu);
				}
			}
            if( !ptr )
			{
				mptr->ConvertToGameDocument(gd_temp);
				ptr = &gd_temp;
			}

            ptr->modified = false;
            ptr->game_prefix_edited = false;
            ptr->game_details_edited = false;
            ptr->pgn_handle = pgn_handle;  // irrespective of where it came from, now this
                                           //  game is in this file
            ptr->fposn0 = write_posn;      // at this position
                                           // This worried me one time I looked at it - what if it's say a DB game?
                                           //  in that case we are only changing the gd_temp *temporary* document
            std::string s = ptr->prefix_txt;
            int len = s.length();
            #ifdef _WINDOWS
            #define EOL "\r\n"
            #else
            #define EOL "\n"
            #endif
            if( len > 0 )
            {
                if( i != 0 )    // blank line needed before all but first prefix
                {
                    fwrite( EOL, 1, strlen(EOL), pgn_out );
                    game_len += strlen(EOL);
                }
                fwrite( s.c_str(), 1, len, pgn_out );
                game_len += len;
                fwrite( EOL, 1, strlen(EOL), pgn_out );
                game_len += strlen(EOL);
            }
            ptr->fposn1 = write_posn + game_len;
            std::string str;
            GameDocument temp = *ptr;
            temp.r = mptr->RefRoster();
            temp.ToFileTxtGameDetails( str );
            fwrite(str.c_str(),1,str.length(),pgn_out);
            game_len += str.length();
            ptr->fposn2 = write_posn + game_len;
            temp.ToFileTxtGameBody( str );
            fwrite(str.c_str(),1,str.length(),pgn_out);
            game_len += str.length();
            ptr->fposn3 = write_posn + game_len;
			if( save_changes_back_to_tab )
				*save_changes_back_to_tab = *ptr;
        }
        write_posn += game_len;
    }
    delete[] buf;
    if( reached_limit )
    {
        wxString msg;
        if( nbr_locked >= gds_nbr )
        {
            msg.sprintf( "These games are from a database that does not allow unrestricted export to PGN\n"
                "%d %s written to PGN\n"
                "%d %s omitted\n", count_to_limit, count_to_limit==1?"game was":"games were", 
                                        nbr_omitted, nbr_omitted==1?"game was":"games were" );
        }
        else
        {
            msg.sprintf( "Some games were from a database that does not allow unrestricted export to PGN\n"
                "%d such %s written to PGN\n"
                "%d such %s omitted\n", count_to_limit, count_to_limit==1?"game was":"games were", 
                                        nbr_omitted, nbr_omitted==1?"game was":"games were" );
        }
        wxMessageBox( msg, "Some games omitted from saved file\n", wxOK|wxICON_WARNING );
    }
    if( unavailable_handles_found )
    {
        std::string filename_used;
        bool first=true, last=false;
        for( int i=0; i<gds_nbr && nbr_game_documents>0; i++ )
        {
            ListableGame *mptr = gds[i].get();
		    int pgn_handle2;
    		bool is_pgn = mptr->GetPgnHandle(pgn_handle2);
            if( is_pgn && unavailable_handles.find(pgn_handle2) != unavailable_handles.end() )
                nbr_unavailable_games++;
            if( mptr->IsGameDocument() && !mptr->IsGameDocument()->IsEmpty() )
            {
                last = (++nbr_emergency_games_written >= nbr_game_documents);
                objs.log->EmergencySaveGame( mptr->IsGameDocument(), first, last, filename_used );
                if( first && filename_used == "" )
                    break;
                first = false;
            }
        }
        if( filename_used=="" || nbr_game_documents==0 )
        {
            FileConflictError( true );  // just use default message
        }
        else
        {
            wxString msg;
            if( nbr_game_documents + nbr_unavailable_games >= gds_nbr )
            {
                msg.sprintf( "Error: An external program may have modified a PGN file or files Tarrasch is using.\n" 
                "To try to avoid losing your work %d %s added to file %s.\n"
                "Please save any other work in progress to a fresh file and restart Tarrasch (i.e. shut it down and and start it again).\n",
                    nbr_game_documents, nbr_game_documents>1 ?"games were":"game was", filename_used
                  );

            }
            else
            {
                msg.sprintf( "The file could not be written completely because it contained %d %s from %s apparently been changed by an external program.\n"
                "To try to avoid losing your work %d %s added to file %s.\n"
                "Please save any other work in progress to a fresh file and restart Tarrasch (i.e. shut it down and and start it again).\n",
                    nbr_unavailable_games,
                    nbr_unavailable_games>1 ? "games" : "game",
                    nbr_unavailable_files>1 ? "PGN files that have" : "a PGN file that has",
                    nbr_game_documents, nbr_game_documents>1?"games were":"game was", filename_used
                  );
            }
            FileConflictError( msg, true );
        }
    }
}

void GamesCache::Debug( const char *UNUSED(intro_message) )
{
}

//#define NOMARKDOWN x

#if 0  //not currently needed
static void PgnNameCommaGroom( std::string &name )
{
    name.erase( name.find_last_not_of(" \n\r\t")+1 ); // right trim
    if( name.find(',') == std::string::npos )
    {
        size_t first_space = name.find_first_of(" ");
        if( first_space != std::string::npos )
        {
            std::string surname = name.substr( first_space+1 );
            std::string forname = name.substr( 0, first_space );
            std::string lower_surname;
            for( size_t i=0; i<surname.length(); i++ )
            {
                int c = surname[i];
                if( isalpha(c) && isupper(c) )
                    c = tolower(c);
                lower_surname.push_back(c);
            }
            if( lower_surname.substr(0,3) != "van" )
            { 
                size_t last_space = name.find_last_of(" ");
                if( last_space != std::string::npos )
                {
                    surname = name.substr( last_space+1 );
                    forname = name.substr( 0, last_space );
                }
            }
            name = surname + ", " + forname;
        }
    }
}
#endif

void GamesCache::Eco(  GamesCache *UNUSED(gc_clipboard) )
{
    int gds_nbr = gds.size();
    ProgressBar pb( "Calculate missing ECO codes", "Calculate missing ECO codes" );
    for( int i=0; i<gds_nbr; i++ )
    {
        double permill = (static_cast<double>(i) * 1000.0) / static_cast<double>(gds_nbr?gds_nbr:1);
        pb.Permill( static_cast<int>(permill) );
        ListableGame *mptr = gds[i].get();
        CompactGame pact;
        mptr->GetCompactGame( pact );
        if( pact.r.fen == "" && pact.r.eco == "" )
        {
            pact.r.eco = eco_calculate( pact.moves );
            //PgnNameCommaGroom( pact.r.black );
            //PgnNameCommaGroom( pact.r.white );
            if( mptr->IsGameDocument() )
                mptr->SetRoster(pact.r);
            else
            {
        		GameDocument temp;
		        mptr->ConvertToGameDocument(temp);
                temp.SetRoster(pact.r);
			    make_smart_ptr(GameDocument, new_smart_ptr, temp);
			    gds[i] = std::move(new_smart_ptr);
            }
        }
    }
}

// Check the ECO code implemenation
#if 0
void GamesCache::Eco(  GamesCache *gc_clipboard )
{
    int gds_nbr = gds.size();
    ProgressBar pb( "Checking ECO", "Checking ECO" );
    int successes=0;
    int failures=0;
    for( int i=0; i<gds_nbr; i++ )
    {
        double permill = (static_cast<double>(i) * 1000.0) / static_cast<double>(gds_nbr?gds_nbr:1);
        pb.Permill( static_cast<int>(permill) );
        ListableGame *mptr = gds[i].get();
        std::string eco  = mptr->Eco();
        std::string blob = mptr->CompressedMoves();
        const char *file = eco.c_str();
        const char *calc = eco_calculate(blob);
        bool match = (0==strcmp(file,calc));
        if( match )
            successes++;
        else
            failures++;
        if( !match && failures<20 )
        {
            thc::ChessRules cr;
            CompressMoves press;
            std::vector<thc::Move> v = press.Uncompress(blob);
            std::string txt;
            for( int j=0; j<30 && j<v.size(); j++ )
            {
                thc::Move mv = v[j];
                char buf[100];
                if( cr.white )
                    sprintf( buf, "%s%d. ", j==0?"":" ", cr.full_move_count );
                else
                    sprintf( buf, " " );
                txt += std::string(buf);
                txt += mv.NaturalOut(&cr);
                cr.PlayMove(mv);
            }
            cprintf( "File %s = %s\n"
                     "Calc %s = %s\n"
                     "Game     = %s\n",
                     file, eco_ref(file), calc, eco_ref(calc), txt.c_str() );
        }
    }

    // results twic-1078-1102.pgn
    //   eco4.txt 72161 successes, 8583 failures
    //   eco5.txt 71537 successes, 9207 failures
    cprintf( "%d successes, %d failures\n", successes, failures );
}
#endif

// Publish to a markdown (later html) file
void GamesCache::Publish()
{
    std::string filename = pgn_filename;
    int len=filename.length();
    int from=0, to=len;
    for( int i=0; i<len; i++ )
    {
        if( filename[i]=='\\' ||  filename[i]=='/' || filename[i]==':' )
            from = i+1;
        else if( filename[i]=='.' )
            to = i;
    }
    std::string basename;
    if( to > from )
        basename = filename.substr(from,to-from);
    else
        basename = filename.substr(from);
#ifdef NOMARKDOWN
    bool markdown=false;
    std::string mdname = basename + ".html";
    wxFileDialog fd( objs.frame, "Publish to .html file", "", mdname.c_str(), "*.html", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
#else
    bool markdown=true;
    std::string mdname = basename + ".md";    
    wxFileDialog fd( objs.frame, "Publish to .md markdown file", "", mdname.c_str(), "*.md", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
#endif
    wxString dir = objs.repository->nv.m_doc_dir;
    fd.SetDirectory(dir);
	DialogDetect detect;		// an instance of DialogDetect as a local variable allows tracking of open dialogs
    int answer = fd.ShowModal();
    if( answer == wxID_OK)
    {
        wxString dir2;
        wxFileName::SplitPath( fd.GetPath(), &dir2, NULL, NULL );
        objs.repository->nv.m_doc_dir = dir2;
        wxString wx_filename = fd.GetPath();
        FILE *md_out = fopen( wx_filename.c_str(), "wb" );
        if( md_out )
        {        
            if( !markdown )
            {
                std::string html_intro = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<html>\n"
                "<head>\n"
                "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />\n"
                "<script type=\"text/javascript\" src=\"jquery-1.8.3.min.js\"></script>\n"
                "<script type=\"text/javascript\" src=\"publish.js\"></script>\n"
                "</head>\n"
                "\n"
                "<body>\n"
                "<div id=\"top\">\n"
                "<p>Simple Example</p>\n"
                "</div>\n"
                "<div id=\"content\">\n";
                fwrite( html_intro.c_str(), 1, html_intro.length(), md_out );
            }
            
            int gds_nbr = gds.size();

            // For each game
            int diagram_base = 0;
            int mv_base = 0;
            int neg_base = -2;
			for( int i=0; i<gds_nbr; i++ )
			{
				GameDocument gd;
				gds[i]->ConvertToGameDocument(gd);
				thc::ChessPosition tmp;

				int publish_options = 0;
				bool skip_intro = false;
				bool skip_game = false;
				std::string white = gd.r.white;
				std::string black = gd.r.black;
				std::string t = black;
				std::string options = "";
				bool white_only = (white != "" && white != "?") && (black == "" || black == "?");
				if (!white_only && t[0] == '@')
				{
					size_t at2;
					at2 = t.substr(1).find('@');
					if (at2 != std::string::npos)
					{
						options = t.substr(0, at2 + 1);
						black = t.substr(at2 + 2);
					}
				}

				bool heading_1 = (std::string::npos != options.find('1'));
				bool heading_2 = (std::string::npos != options.find('2'));
				bool suppress_move = (std::string::npos != options.find('N'));
				bool heading = (heading_1 || heading_2 || white_only);
				bool join = (std::string::npos != options.find('J'));
				if (join)
				{
					white = white + " " + black;
					black = "";
				}

#ifdef NOMARKDOWN
				//<h1>Introduction</h1>
				//<p>Sorry, this was rushed out, I may have time to embellish these brief highlights later. </p>
				//<h3>Russell Dive - Scott Wastney, Julian Mazur Memorial 2012</h3>
				std::string s = "";
				std::string pre = gd.prefix_txt;
				if (pre.size() > 0 && pre[0] == '#')
				{
					size_t off1 = pre.find_first_not_of("#");
					size_t off2 = pre.find('\n');
					if (off1 != std::string::npos && off2 != std::string::npos
						&& off2 > off1)
					{
						s += "<h1>";
						s += pre.substr(off1, off2 - off1);
						s += "</h1>\n";
						pre = pre.substr(off2);
					}
				}
				s += "<p>";
				s += pre;
				s += "</p>\n";
				if (heading)
				{
					skip_intro = true;
					if (heading_1)
						s = "<h1>" + white + "</h1>";
					else
						s = "<h2>" + white + "</h2>";
					s += "\n";
					publish_options |= GameView::SUPPRESS_NULL_MOVE;
					publish_options |= GameView::SUPPRESS_VARIATION_PARENS;
				}
#else
				// Write prefix
				std::string s = gd.prefix_txt;
				if (heading)
				{
					skip_intro = true;
					if (heading_1)
						s = "#" + white;
					else
						s = "##" + white;
					s += "\r";
					publish_options |= GameView::SUPPRESS_NULL_MOVE;
					publish_options |= GameView::SUPPRESS_VARIATION_PARENS;
				}
#endif
				if (suppress_move)
					publish_options |= GameView::SUPPRESS_MOVE;
				const char *tags[] =
				{
					"#SkipToDiagram",
					"#SkipIntro",
					"#SkipGame"
				};
				for (int j = 0; j < nbrof(tags); j++)
				{
					for (int k = 0; k < 4; k++)
					{
						char buf[80];
						strcpy(buf, tags[j]);
						switch (k)
						{
						case 0: strcat(buf, ", "); break;
						case 1: strcat(buf, ",");  break;
						case 2: strcat(buf, " ");  break;
						default:
						case 3: break;
						}
						const char *pattern;
						size_t found;
						pattern = buf;
						found = s.find(pattern);
						if (found != std::string::npos)
						{
							s = s.substr(0, found) + s.substr(found + strlen(pattern));
							switch (j)
							{
							case 0: publish_options |= GameView::SKIP_TO_FIRST_DIAGRAM; break;
							case 1: skip_intro = true;   break;
							case 2: skip_game = true;   break;
							}
							break;
						}
					}
				}
				int len2 = s.length();
				if (len2 > 0)
				{
					if (i != 0)    // blank line needed before all but first prefix
						fwrite("\n", 1, 1, md_out);
					fwrite(s.c_str(), 1, len2, md_out);
					fwrite("\n", 1, 1, md_out);
				}
				if (!skip_game)
				{

					if (!skip_intro)
					{
						// Write header
						//gd->ToFileTxtGameDetails( s );

						s = markdown ? "### " : "<h3>";
						s += white;
						s += " - ";
						s += black;
						if (gd.r.event.find('?') == std::string::npos)
						{
							s += " - ";
							s += gd.r.event;
						}
						std::string year = gd.r.date.substr(0, 4);
						if (year.find('?') == std::string::npos)
						{
							s += " ";
							s += year;
						}
						s += (markdown ? "\n" : "</h3>\n");
						s += (markdown ? "\n" : "</h3>\n");
						fwrite(s.c_str(), 1, s.length(), md_out);
					}

					// Write Game body
					gd.ToPublishTxtGameBody(s, diagram_base, mv_base, neg_base, publish_options);
					fwrite(s.c_str(), 1, s.length(), md_out);
				}

				if( i+1 == gds_nbr )
					fwrite("<br/>\n", 1, 6, md_out);
				else
					fwrite("\n", 1, 1, md_out);
				if( !markdown && i+1 == gds_nbr )
				{
					fwrite("</div>\n"
						"</body>\n"
						"</html>\n", 1, 23, md_out);
				}
			}
            fclose( md_out );
        }
    }
}

