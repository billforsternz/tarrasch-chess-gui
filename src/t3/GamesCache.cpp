/****************************************************************************
 * Games Cache - Abstracted interface to  a list of games
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
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
#include "Objects.h"
#include "Repository.h"
#include "PgnFiles.h"
#include "Lang.h"
#include "GamesCache.h"
#include <stdio.h>
using namespace std;

static bool operator < (const smart_ptr<GameDocumentBase>& left,
                        const smart_ptr<GameDocumentBase>& right)
{
    bool result = ( *left < *right );
    cprintf( "operator <; left->white=%s, right->white=%s, result=%s\n", left->white.c_str(),  right->white.c_str(), result?"true":"false" );
    return result;
}



bool GamesCache::Load(std::string &filename )
{
    file_irrevocably_modified = false;
    resume_previous_window = false;
    loaded = false;
    FILE *pgn_file = objs.gl->pf.OpenRead( filename, pgn_handle );
    if( pgn_file )
    {
        pgn_filename = filename;
        gds.clear();
        loaded = Load(pgn_file);
        objs.gl->pf.Close(NULL);  // clipboard only needed after ReopenModify()
    }
    return loaded;
}

bool GamesCache::IsLoaded()
{
    return loaded;
}

bool GamesCache::IsSynced()
{
    return( IsLoaded() && objs.gl->pf.IsAvailable(pgn_handle) );
}

bool GamesCache::Load( FILE *pgn_file )
{
    file_irrevocably_modified = false;
    bool ok=true;
    GameDocumentBase gd;
    game_nbr=0;
    int fposn_base = 0;
    gd.fposn0 = fposn_base;
    const int BUFSIZE=10000;
    char buf[BUFSIZE+5];
    int req;
    int len = fread(buf,1,req=BUFSIZE,pgn_file);
    bool nomore = (len!=req);
    const char *beg = buf;
    const char *end = buf + (len>=0?len:0);
    const char *s = beg;
    bool done = (s>=end);
    while( !done )
    {
        bool have_line=false;
        const char *line = s;
        while( s < end )
        {
            if( *s=='\r' || *s=='\n' )
            {
                have_line = true;
                break;
            }
            s++;
        }
        if( !have_line )
        {
            // ragged, incomplete line
            if( nomore )
            {
                done = true;
                if( s > line )
                    have_line = true;   // accept ragged final line with no "\n"
            }
            else
            {

                // move the incomplete line to the start of buffer and refill
                fposn_base += (line-buf);
                const char *src = line;
                char *dst = buf;
                int count = s-line;
                req = BUFSIZE-count;
                for( int i=0; i<count; i++ )
                    *dst++ = *src++;
                len = fread(buf+count,1,req,pgn_file);
                nomore = (len!=req);
                s = buf;
                end = buf + count + (len>=0?len:0);
            }    
        }
        if( have_line )
        {
            char *make_c_str = (char *)s;
            char save = *make_c_str;
            *make_c_str = '\0';
            LoadLine( gd, fposn_base +(line-buf), line );
            *make_c_str = save;
            if( *s=='\r' && *(s+1)=='\n' )
                s += 2;
            else if( *s=='\n' && *(s+1)=='\r' )     // not so likely...
                s += 2;
            else
                s++;
        }
    }

    // terminate last game
    fseek(pgn_file,0,SEEK_END);
    long filelen = ftell(pgn_file);
    fseek(pgn_file,0,SEEK_SET);
    LoadLine( gd, filelen, NULL );
    return ok;
}
            
void GamesCache::LoadLine( GameDocumentBase &gd, int fposn, const char *line )
{            
    bool end_of_game=false;
    if( !line )     // end of file ?
    {
        switch( state )
        {
            case PREFIX:
            {
                // If some prefix text has accumulated - terminate a new "game"
                //  that comprises only a prefix
                if( gd.prefix_txt.length() > 2 )
                {
                    gd.fposn1 = fposn;
                    gd.fposn2 = fposn;
                    gd.fposn3 = fposn;
                    end_of_game = true;
                    state = PREFIX;
                }
                break;
            }
            case HEADER:
            {
                gd.fposn2 = fposn;
                gd.fposn3 = fposn;
                end_of_game = true;
                state = PREFIX;
                break;
            }
            case INGAME:
            {
                gd.fposn3 = fposn;
                end_of_game = true;
                state = PREFIX;
                break;
            }
        }
    }
    else
    {
        const char *s = line;
        while( *s==' ' || *s=='\t' )
            s++;
        switch( state )
        {
            case PREFIX:
            {
                bool stay_in_prefix=true;
                if( *s == '[' && Tagline(gd,s) )
                    stay_in_prefix = false;
                if( stay_in_prefix )
                {
                    gd.prefix_txt += s;
                    gd.prefix_txt += "\r\n";
                }
                else
                {
                    // remove blank line at end of prefix
                    std::string s = gd.prefix_txt;
                    int len = s.length();
                    if( len>=2 && s[len-2]=='\r' && s[len-1]=='\n' )
                        gd.prefix_txt = s.substr(0,len-2);
                    state = HEADER;
                    gd.fposn1 = fposn;
                }
                break;
            }
            case HEADER:
            {
                if( *s == '[' )
                    Tagline(gd,s);
                else if( *s )
                {
                    state = INGAME;
                    gd.fposn2 = fposn;
                    gd.game_nbr = game_nbr++;
                    gd.moves_txt = s;
                    LangLine( gd.moves_txt, NULL, LangGet() );  // English -> Current language
                    int len = gd.moves_txt.length();
                    if( len>=1 && gd.moves_txt[len-1] == '*' )
                        gd.moves_txt = gd.moves_txt.substr(0,len-1);
                    gd.pgn_handle = pgn_handle;
                }
                break;
            }
            case INGAME:
            {
                if( *s == '\0' )
                {
                    gd.fposn3 = fposn;
                    end_of_game = true;
                    state = PREFIX;
                }
                else if( *s == '[' )
                {
                    if( Tagline(gd,s) )
                    {
                        gd.fposn3 = fposn;
                        end_of_game = true;
                        state = HEADER;
                    }
                }
                break;
            }
        }
    }
    if( end_of_game )
    {
        make_smart_ptr( HoldDocumentBase, new_doc, gd );
        gds.push_back( std::move(new_doc) );
        thc::ChessPosition initial_position;
        gd.prefix_txt = "";
        gd.Init(initial_position);
        gd.fposn0 = fposn;
        if( state == HEADER )
            gd.fposn1 = fposn;
    }
}

// Check whether text s is a valid header, return true if it is,
//  add info to a GameDocument, optionally clearing it first
bool GamesCache::Tagline( GameDocumentBase &gd,  const char *s )
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
            string tag(tag_begin,tag_end-tag_begin);
            string val(val_begin,val_end-val_begin);
            if( tag == "White" )
                gd.white = val;
            if( tag == "Black" )
                gd.black = val;
            if( tag == "Event" )
                gd.event = val;
            if( tag == "Site" )
                gd.site = val;
            if( tag == "Date" )
                gd.date = val;
            if( tag == "Round" )
                gd.round = val;
            if( tag == "Result" )
                gd.result = val;
            if( tag == "ECO" )
                gd.eco = val;
            if( tag == "WhiteElo" )
                gd.white_elo = val;
            if( tag == "BlackElo" )
                gd.black_elo = val;
            if( tag == "FEN" )
                gd.start_position.Forsyth(val.c_str());
        }
    }
    return is_header;
}

// Create a new file
bool GamesCache::FileCreate( std::string &filename, GameDocument &gd )
{
    pgn_filename = filename;
    resume_previous_window = false;
    loaded = false;

    gds.clear();
    gd.in_memory = true;
    gd.pgn_handle = 0;
    make_smart_ptr( HoldDocument, new_doc, gd );
    gds.push_back( std::move(new_doc) );
    FILE *pgn_out = objs.gl->pf.OpenCreate( pgn_filename, pgn_handle );
    if( pgn_out )
    {
        loaded = true;
        FileSaveInner( NULL, NULL, pgn_out );
        objs.gl->pf.Close(NULL);    // close all handles (gc_clipboard
                                    //  only needed for ReopenModify())
    }
    return loaded;
}

// Save the existing file
void GamesCache::FileSave( GamesCache *gc_clipboard )
{
    FILE *pgn_in;
    FILE *pgn_out;
    bool ok = objs.gl->pf.ReopenModify( pgn_handle, pgn_in, pgn_out );
    if( ok )
    {
        FileSaveInner( gc_clipboard, pgn_in, pgn_out );
        objs.gl->pf.Close( gc_clipboard );    // close all handles
    }
    FILE *debug = NULL; //fopen( "Bill.txt", "at" );
    if( debug )
    {
        fprintf( debug, "After FileSave(): pgn_handle=%d\n", pgn_handle );
        int gds_nbr = gds.size();
        for( int i=0; i<gds_nbr; i++ )    
        {   
            GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
            if( ptr )
            {
                int handle      = ptr->pgn_handle;
                bool modified   = ptr->modified;
                bool in_memory  = ptr->in_memory;
                long fposn0     = ptr->fposn0;
                long fposn1     = ptr->fposn1;
                long fposn2     = ptr->fposn2;
                long fposn3     = ptr->fposn3;
                fprintf( debug, "handle=%d, modified=%d, in_memory=%d\n"
                                " fposn0=%ld,\n"
                                " fposn1=%ld,\n"
                                " fposn2=%ld,\n"
                                " fposn3=%ld,\n",
                                handle, modified, in_memory, fposn0,
                                               fposn1, fposn2, fposn3 );
            }
        }
        FILE *in = fopen( "/Users/Bill/Documents/Tarrasch/bug5b.pgn", "rt" );
        if( in )
        {
            fprintf( debug, "/Users/Bill/Documents/Tarrasch/bug5b.pgn = " );
            int ch = fgetc(in);
            while( ch != EOF )
            {
                fputc(ch,debug);
                ch = fgetc(in);
            }
            fclose(in);
        }
        fclose(debug);
    }
}

// Save the existing file with a new name
void GamesCache::FileSaveAs( std::string &filename, GamesCache *gc_clipboard )
{
    FILE *pgn_in;
    FILE *pgn_out;
    bool ok = objs.gl->pf.ReopenCopy( pgn_handle, filename, pgn_in, pgn_out );
    if( ok )
    {
        pgn_filename = filename;
        FileSaveInner( gc_clipboard, pgn_in, pgn_out );
        objs.gl->pf.Close( gc_clipboard );    // close all handles
    }
}

// Save the current game to a file
void GamesCache::FileSaveGameAs( std::string &filename, GamesCache *gc_clipboard )
{
}

// Save all as a new file
void GamesCache::FileSaveAllAsAFile( std::string &filename )
{
    renumber = true;   // save session or clipboard in order files are listed
    FILE *pgn_out = objs.gl->pf.OpenCreate( filename, pgn_handle );
    if( pgn_out )
    {
        FileSaveInner( NULL, NULL, pgn_out );
        objs.gl->pf.Close(NULL);    // close all handles (gc_clipboard
                                    //  only needed for ReopenModify())
    }
}

// Save common
void GamesCache::FileSaveInner( GamesCache *gc_clipboard, FILE *pgn_in, FILE *pgn_out )
{
    char *buf;
    int buflen=100;
    file_irrevocably_modified = false;
    buf = new char [buflen];
    int gds_nbr = gds.size();
    FILE *debug = NULL;//fopen( "Bill.txt", "at" );
    if( debug )
    {
        fprintf( debug, "Before: pgn_handle=%d\n", pgn_handle );
        for( int i=0; i<gds_nbr; i++ )    
        {   
            GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
            if( ptr )
            {
                int handle      = ptr->pgn_handle;
                bool modified   = ptr->modified;
                bool in_memory  = ptr->in_memory;
                long fposn0     = ptr->fposn0;
                long fposn1     = ptr->fposn1;
                long fposn2     = ptr->fposn2;
                long fposn3     = ptr->fposn3;
                fprintf( debug, "handle=%d, modified=%d, in_memory=%d\n"
                                " fposn0=%ld,\n"
                                " fposn1=%ld,\n"
                                " fposn2=%ld,\n"
                                " fposn3=%ld,\n",
                                handle, modified, in_memory, fposn0,
                                               fposn1, fposn2, fposn3 );
            }
        }
        fclose(debug);
    }
    // Sort by game_nbr, save current order in .game_nbr for restore later
    if( !renumber )
    {
        for( int i=0; i<gds_nbr; i++ )    
        {   
            GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
            if( ptr )
            {
                ptr->sort_idx = ptr->game_nbr;
                ptr->game_nbr = i;
            }
        }
        sort( gds.begin(), gds.end() );
    }
    long posn=0;
    for( int i=0; i<gds_nbr; i++ )    
    {   
        GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
        if( ptr )
        {
            bool replace_game_prefix = true;
            bool replace_game_details = true;
            bool replace_moves = true;
            ptr->modified = false;
            replace_game_prefix = ptr->game_prefix_edited || ptr->pgn_handle==0;
            ptr->game_prefix_edited = false;
            replace_game_details = ptr->game_details_edited || ptr->pgn_handle==0;
            ptr->game_details_edited = false;
            replace_moves = ptr->in_memory || ptr->pgn_handle==0;
            bool no_replacements = (!replace_moves && !replace_game_details && !replace_game_prefix);
            bool replace_all     = (replace_moves && replace_game_details && replace_game_prefix);

            //   fposn0
            //     prefix text
            //   fposn1
            //     [game details]
            //   fposn2
            //     [game moves]
            //   fposn3
            //
            long fposn0 = ptr->fposn0;
            long fposn1 = ptr->fposn1;
            long fposn2 = ptr->fposn2;
            long fposn3 = ptr->fposn3;
            long len;
            bool same_file = (ptr->pgn_handle==pgn_handle);
            FILE *pgn = pgn_in;
            if( !same_file && !replace_all )
            {
                pgn = objs.gl->pf.ReopenRead( ptr->pgn_handle );
                if( !pgn )
                    continue; // whoops, can't read the game
            }
            ptr->pgn_handle = pgn_handle;  // irrespective of where it came from, now this
                                             //  game is in this file
            ptr->fposn0 = posn;            
                                            
            if( no_replacements )
            {
                len = fposn3-fposn0;
                fseek(pgn,fposn0,SEEK_SET);
                while( len >= buflen )
                {
                    delete[] buf;
                    buflen *= 2;
                    buf = new char [buflen];
                }
                fread(buf,1,len,pgn);
                fwrite(buf,1,len,pgn_out);
                ptr->fposn1 = posn + (fposn1-fposn0);
                ptr->fposn2 = posn + (fposn2-fposn0);
                posn += len;
                ptr->fposn3 = posn;
            }
            else
            {
                ptr->fposn0 = posn;
                std::string s = ptr->prefix_txt;
                int len = s.length();
                if( len > 0 )
                {
                    if( i != 0 )    // blank line needed before all but first prefix
                    {
                        fwrite( "\r\n", 1, 2 ,pgn_out);
                        posn += 2;
                    }
                    fwrite( s.c_str(),1,len,pgn_out);
                    fwrite( "\r\n", 1, 2 ,pgn_out);
                    posn += (len+2);
                }
                ptr->fposn1 = posn;
                if( replace_game_details )
                {
                    std::string str;
                    GameDocument temp = *ptr;
                    temp.ToFileTxtGameDetails( str );
                    fwrite(str.c_str(),1,str.length(),pgn_out);
                    posn += str.length();
                }
                else
                {
                    len = fposn2-fposn1;
                    fseek(pgn,fposn1,SEEK_SET);
                    while( len >= buflen )
                    {
                        delete[] buf;
                        buflen *= 2;
                        buf = new char [buflen];
                    }
                    fread(buf,1,len,pgn);
                    fwrite(buf,1,len,pgn_out);
                    posn += len;
                }
                ptr->fposn2 = posn;
                if( replace_moves )
                {
                    std::string str;
                    GameDocument temp = *ptr;
                    temp.ToFileTxtGameBody( str );
                    fwrite(str.c_str(),1,str.length(),pgn_out);
                    posn += str.length();
                }
                else
                {
                    len = fposn3-fposn2;
                    fseek(pgn,fposn2,SEEK_SET);
                    while( len >= buflen )
                    {
                        delete[] buf;
                        buflen *= 2;
                        buf = new char [buflen];
                    }
                    fread(buf,1,len,pgn);
                    fwrite(buf,1,len,pgn_out);
                    posn += len;
                }
                ptr->fposn3 = posn;

                // Fix a nasty bug in T2 up to and including V2.01. A later PutBackDocument()
                //  was overwriting the correctly calculated values of fposn0 etc. with stale
                //  values. Fix is to update those stale values here.
                GameDocument *p = objs.tabs->Begin();
                while( p )
                {
                    if( ptr->game_being_edited == p->game_being_edited )
                    {
                        p->fposn0 = ptr->fposn0;
                        p->fposn1 = ptr->fposn1;
                        p->fposn2 = ptr->fposn2;
                        p->fposn3 = ptr->fposn3;
                        p->pgn_handle = ptr->pgn_handle;
                    }
                    p = objs.tabs->Next();
                }
            }
        }
    }

    // Restore sort order .game_nbr field is restored to its original value
    if( !renumber )
    {
        for( int i=0; i<gds_nbr; i++ )    
        {   
            GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
            if( ptr )
            {
                int temp = ptr->sort_idx;
                ptr->sort_idx = ptr->game_nbr;
                ptr->game_nbr = temp;
            }
        }
        sort( gds.begin(), gds.end() );
    }
    delete[] buf;
    debug = NULL;//fopen( "Bill.txt", "at" );
    if( debug )
    {
        fprintf( debug, "After: pgn_handle=%d\n", pgn_handle );
        for( int i=0; i<gds_nbr; i++ )    
        {   
            GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
            if( ptr )
            {
                int handle      = ptr->pgn_handle;
                bool modified   = ptr->modified;
                bool in_memory  = ptr->in_memory;
                long fposn0     = ptr->fposn0;
                long fposn1     = ptr->fposn1;
                long fposn2     = ptr->fposn2;
                long fposn3     = ptr->fposn3;
                fprintf( debug, "handle=%d, modified=%d, in_memory=%d\n"
                                " fposn0=%ld,\n"
                                " fposn1=%ld,\n"
                                " fposn2=%ld,\n"
                                " fposn3=%ld,\n",
                                handle, modified, in_memory, fposn0,
                                               fposn1, fposn2, fposn3 );
            }
        }
        fclose(debug);
    }
}


void GamesCache::Debug( const char *intro_message )
{
    DebugPrintf(( "Cache dump>%s\n", intro_message ));
    int gds_nbr = gds.size();
    for( int i=0; i<gds_nbr; i++ )    
    {   
        GameDocument doc = gds[i]->GetGameDocument();
        dprintf( "game_nbr=%d, white=%s, moves_txt=%s, pgn_handle=%d\n",
                        doc.game_nbr,
                        doc.white.c_str(),
                        doc.moves_txt.c_str(),
                        doc.pgn_handle
                   );

        dprintf( " modified=%s, in_memory=%s, game_details_edited=%s\n",
                        doc.modified ? "true":"false",
                        doc.in_memory ? "true":"false",
                        doc.game_details_edited ? "true":"false"
                   );

        dprintf( " game_being_edited=%d, selected=%s, focus=%s\n",
                        (int)doc.game_being_edited,
                        doc.selected ? "true":"false",
                        doc.focus ? "true":"false"
                   );
    }
}

//#define NOMARKDOWN x

// Publish to a markdown (later html) file
void GamesCache::Publish(  GamesCache *gc_clipboard )
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
    int answer = fd.ShowModal();
    if( answer == wxID_OK)
    {
        wxString dir;
        wxFileName::SplitPath( fd.GetPath(), &dir, NULL, NULL );
        objs.repository->nv.m_doc_dir = dir;
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

            // Sort by game_nbr, save current order in .game_nbr for restore later
            if( !renumber )
            {
                for( int i=0; i<gds_nbr; i++ )    
                {
                    GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
                    if( ptr )
                    {
                        ptr->sort_idx = ptr->game_nbr;
                        ptr->game_nbr = i;
                    }
                }
                sort( gds.begin(), gds.end() );
            }

            // For each game
            int diagram_base = 0;
            int mv_base = 0;
            int neg_base = -2;
            for( int i=0; i<gds_nbr; i++ )    
            {   
                GameDocument gd;
                gds[i]->GetGameDocument(gd);
                thc::ChessPosition tmp;

                int publish_options = 0;
                bool skip_intro = false;
                bool skip_game = false;
                std::string white = gd.white;
                std::string black = gd.black;
                std::string t = black;
                std::string options = "";
                bool white_only = (white!="" && white!="?") && (black=="" || black=="?");
                if( !white_only && t[0]=='@' )
                {
                    size_t at2;
                    at2 = t.substr(1).find('@');
                    if( at2 != std::string::npos )
                    {
                        options = t.substr(0,at2+1);
                        black = t.substr(at2+2);
                    }
                }

                bool heading_1 = (std::string::npos != options.find('1'));
                bool heading_2 = (std::string::npos != options.find('2'));
                bool suppress_move = (std::string::npos != options.find('N'));
                bool heading = (heading_1 || heading_2 || white_only);
                bool join = (std::string::npos != options.find('J'));
                if( join )
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
                if( pre.size()>0 && pre[0] == '#' )
                {
                    size_t off1 = pre.find_first_not_of("#");
                    size_t off2 = pre.find('\n');
                    if( off1!=std::string::npos && off2!=std::string::npos
                        && off2>off1 )
                    {
                        s += "<h1>";
                        s += pre.substr(off1,off2-off1);
                        s += "</h1>\n";
                        pre = pre.substr(off2);
                    }
                }
                s += "<p>";
                s += pre;
                s += "</p>\n";
                if( heading )
                {
                    skip_intro = true;
                    if( heading_1 )
                        s = "<h1>" + white + "</h1>";
                    else
                        s = "<h2>" + white + "</h2>";
                    s += "\n";
                    publish_options |= GameView::SUPPRESS_NULL_MOVE;
                    //publish_options |= GameView::SUPPRESS_VARIATION_PARENS;
                }
#else
                // Write prefix
                std::string s = gd.prefix_txt;
                if( heading )
                {
                    skip_intro = true;
                    if( heading_1 )
                        s = "#" + white;
                    else
                        s = "##" + white;
                    s += "\r\n";
                    publish_options |= GameView::SUPPRESS_NULL_MOVE;
                    //publish_options |= GameView::SUPPRESS_VARIATION_PARENS;
                }
#endif
                if( suppress_move )
                    publish_options |= GameView::SUPPRESS_MOVE;
                const char *tags[] =
                {
                    "#SkipToDiagram",
                    "#SkipIntro",
                    "#SkipGame"
                };
                for( int j=0; j<nbrof(tags); j++ )
                {
                    for( int k=0; k<4; k++ )
                    {
                        char buf[80];
                        strcpy( buf, tags[j] );
                        switch( k )
                        {
                            case 0: strcat(buf,", "); break;
                            case 1: strcat(buf,",");  break;
                            case 2: strcat(buf," ");  break;
                            default:
                            case 3: break;
                        }
                        const char *pattern;
                        size_t found;
                        pattern = buf;
                        found = s.find(pattern);
                        if( found != std::string::npos )
                        {
                            s = s.substr(0,found) + s.substr(found+strlen(pattern));
                            switch( j )
                            {
                                case 0: publish_options |= GameView::SKIP_TO_FIRST_DIAGRAM; break;
                                case 1: skip_intro            = true;   break;
                                case 2: skip_game             = true;   break;
                            }
                            break;
                        }
                    }
                }
                int len = s.length();
                if( len > 0 )
                {
                    if( i != 0 )    // blank line needed before all but first prefix
                        fwrite( "\r\n", 1, 2, md_out);
                    fwrite( s.c_str(), 1, len, md_out);
                    fwrite( "\r\n", 1, 2, md_out);
                }
                if( !skip_game )
                {

                    if( !skip_intro )
                    {
                        // Write header
                        //gd->ToFileTxtGameDetails( s );
                
                        s = markdown ? "### " : "<h3>";
                        s += white;
                        s += " - ";
                        s += black;
                        if( gd.event.find('?') == std::string::npos )
                        {
                            s += " ";
                            s += gd.event;
                        }
                        std::string year = gd.date.substr(0,4);
                        if( year.find('?') == std::string::npos )
                        {
                            s += " ";
                            s += year;
                        }
                        s += (markdown ? "\n" : "</h3>\n");
                        s += (markdown ? "\n" : "</h3>\n");
                        fwrite(s.c_str(),1,s.length(),md_out);
                    }

                    // Write Game body
                    if( gd.in_memory )
                    {
                        gd.ToPublishTxtGameBody( s, diagram_base, mv_base, neg_base, publish_options );
                        fwrite(s.c_str(),1,s.length(),md_out);
                    }
                    else
                    {
                        FILE *pgn_in = objs.gl->pf.ReopenRead( gd.pgn_handle );
                        if( pgn_in )
                        {
                            long fposn2 = gd.fposn2;
                            long end    = gd.fposn3;
                            fseek(pgn_in,fposn2,SEEK_SET);
                            long len = end-fposn2;
                            char *buf = new char [len];
                            if( len == (long)fread(buf,1,len,pgn_in) )
                            {
                                std::string s(buf,len);
                                thc::ChessRules cr;
                                int nbr_converted;
                                gd.PgnParse(true,nbr_converted,s,cr,NULL);
                                gd.ToPublishTxtGameBody( s, diagram_base, mv_base, neg_base, publish_options );
                                objs.gl->atom.NotUndoAble();
                                fwrite(s.c_str(),1,s.length(),md_out);
                            }
                            delete[] buf;
                        }
                    }
                }
            }


            // Restore sort order .game_nbr field is restored to its original value
            if( !renumber )
            {
                for( int i=0; i<gds_nbr; i++ )    
                {   
                    GameDocumentBase *ptr = gds[i]->GetGameDocumentBasePtr();
                    if( ptr )
                    {
                        int temp = ptr->sort_idx;
                        ptr->sort_idx = ptr->game_nbr;
                        ptr->game_nbr = temp;
                    }
                }
                sort( gds.begin(), gds.end() );
            }
            objs.gl->pf.Close( gc_clipboard );    // close all handles
            fwrite("<br/>\n",1,6,md_out);
            if( !markdown )
            {
                fwrite("</div>\n"
                       "</body>\n"
                       "</html>\n",1,23,md_out);
            }
            fclose( md_out );
        }
    }
}

