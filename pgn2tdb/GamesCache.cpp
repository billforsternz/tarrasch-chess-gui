/****************************************************************************
 * Games Cache - Abstracted interface to  a list of games
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <time.h>
#include <stdio.h>
#include "defs.h"
#include "util.h"
#include "thc.h"
#include "PgnFiles.h"
#include "Lang.h"
#include "ListableGamePgn.h"
#include "PgnRead.h"
#include "GamesCache.h"

// Write PGN files with Windows convention, even on Unix systems
#define EOL "\r\n"

using namespace std;

PgnFiles gbl_pgn_files;
#define nbrof(x) (sizeof(x)/sizeof((x)[0]))
bool PgnStateMachine( FILE *pgn_file, int &typ, char *buf, int buflen );

bool GamesCache::Load( const std::string &filename, const std::string &asset_filename )
{
    file_irrevocably_modified = false;
    loaded = false;
    pgn_filename = "";
    FILE *pgn_file = gbl_pgn_files.OpenRead( filename, pgn_handle );
    FILE *asset_file = fopen( asset_filename.c_str(), "wb" );
    if( pgn_file && asset_file )
    {
        loaded = Load(pgn_file,asset_file);
        if( loaded )
            pgn_filename = filename;
        gbl_pgn_files.Close();
        //gbl_pgn_files.Forget();
    }
    if( asset_file )
        fclose(asset_file);
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

bool GamesCache::Load( FILE *pgn_file, FILE *asset_file )
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
    std::string sgame;
    bool validated_game = false;
    bool done = PgnStateMachine( NULL, typ,  buf, sizeof(buf) );
    while( !done )
    {
        done = PgnStateMachine( pgn_file, typ, buf, sizeof(buf) );
        if( typ == 'T' )
            sgame = std::string(buf);
        else if( typ=='t')
            sgame += std::string(buf);
        else if( typ=='M' )
        {
            sgame += EOL;
            sgame += std::string(buf);
            if( buf[0] != '*' )
                validated_game = true;
        }
        else if( typ=='m' )
            sgame += std::string(buf);
        else if( typ=='G' )
        {
            sgame += EOL;
            if( validated_game )
                fputs( sgame.c_str(), asset_file );
            sgame.clear();
            validated_game = false;
        }
        if( typ == 'G' )
        {
            ListableGamePgn pgn_document(pgn_handle,fposn);
            make_smart_ptr( ListableGamePgn, new_doc, pgn_document );
            gds.push_back( std::move(new_doc) );
            if( !done )
                fposn = ftell(pgn_file);
            game_count++;
        }
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
        printf( "new count = %d\n", ++new_count );
    }
    if( pgn_file && pgn_handle!=save_pgn_handle )
    {
        gbl_pgn_files.Close();
        pgn_file = NULL;
        save_pgn_handle = 0;
    }
    if( !pgn_file )
    {
        pgn_file  = gbl_pgn_files.ReopenRead( pgn_handle );
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
    }
    if( end )
    {
        gbl_pgn_files.Close();
        pgn_file = NULL;
        save_pgn_handle = 0;
        delete pgn;
        printf( "new count = %d\n", --new_count );
        pgn = 0;
    }
    //printf( "ReadGameFromPgnInLoop(%d,%s) %ld (%s-%s)\n", pgn_handle, end?"true":"false", fposn, pact.r.white.c_str(), pact.r.black.c_str() );
    return( pgn );
}

void ReadGameFromPgn( int pgn_handle, long fposn, GameDocument &new_doc )
{
    //printf( "ReadGameFromPgn(%d) %ld\n", pgn_handle, fposn );
    GameDocument gd;
    FILE *pgn_file = gbl_pgn_files.ReopenRead( pgn_handle );
    std::string moves;
    int typ;
    if( !pgn_file )
    {
        // This shouldn't happen
        GameDocument empty;
        new_doc = empty;
        gbl_pgn_files.Close();
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

                // Rough \r\n -> \n transformation at end of line
                std::string line(buf);
                size_t offset = line.find_last_of('\r');
                size_t len    = line.length();
                if( offset!=std::string::npos && offset==len-2 && line[offset+1]=='\n' )
                    line = line.substr(0,offset) + "\n";
                gd.prefix_txt += line;
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
    gbl_pgn_files.Close();
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
            else if( stag == "Black" )
                gd.r.black = val;
            else if( stag == "Event" )
                gd.r.event = val;
            else if( stag == "Site" )
                gd.r.site = val;
            else if( stag == "Date" )
                gd.r.date = val;
            else if( stag == "Round" )
                gd.r.round = val;
            else if( stag == "Result" )
                gd.r.result = val;
            else if( stag == "ECO" )
                gd.r.eco = val;
            else if( stag == "WhiteElo" )
                gd.r.white_elo = val;
            else if( stag == "BlackElo" )
                gd.r.black_elo = val;
            else if( stag == "FEN" )
            {
                gd.r.fen = val;
                gd.start_position.Forsyth(val.c_str());
            }
            else
            {
                std::pair<std::string,std::string> key_value(stag,val);
                gd.extra_tags.push_back(key_value);
            }
        }
    }
    return is_header;
}

// Create a new file
void GamesCache::FileCreate( std::string &filename )
{
}

// Save the existing file
void GamesCache::FileSave( GamesCache *gc_clipboard )
{
    FILE *pgn_in;
    FILE *pgn_out;
    bool ok = gbl_pgn_files.ReopenModify( pgn_handle, pgn_in, pgn_out, gc_clipboard );
    if( !ok )
    {
        //FileConflictError();
    }
    else
    {
        FileSaveInner( pgn_out );
        gbl_pgn_files.Close( gc_clipboard );    // close all handles
    }
}

// Save the existing file with a new name
void GamesCache::FileSaveAs( std::string &filename, GamesCache *gc_clipboard )
{
}

// Save all as a new file
void GamesCache::FileSaveAllAsAFile( std::string &filename )
{
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
    bool saving_work_file = false;
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
    bool unavailable_handles_found = false;
    for( int handle: handles )
    {
        if( !gbl_pgn_files.IsAvailable(handle))
        {
            nbr_unavailable_files++;
            unavailable_handles.insert(handle);
            unavailable_handles_found = true;
        }
    }
    bool reached_limit=false;
    int count_to_limit=0, nbr_omitted=0, count_to_expected_total_games=0;
    nbr_locked=0;
    for( int i=0; i<gds_nbr; i++ )
    {
        ListableGame *mptr = gds[i].get();
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
                FILE *pgn_in2 = gbl_pgn_files.ReopenRead( pgn_handle2);
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
        }
        write_posn += game_len;
    }
    delete[] buf;
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
    for( int i=0; i<gds_nbr; i++ )
    {
        double permill = (static_cast<double>(i) * 1000.0) / static_cast<double>(gds_nbr?gds_nbr:1);
        ListableGame *mptr = gds[i].get();
        CompactGame pact;
        mptr->GetCompactGame( pact );
        if( pact.r.fen == "" && pact.r.eco == "" )
        {
            // pact.r.eco = eco_calculate( pact.moves );
            pact.r.eco = "A00"; // Fake out for Winged Spider so we can eliminate eco.cpp eco.h
                                //  this function not called at all in Winged Spider
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
            printf( "File %s = %s\n"
                     "Calc %s = %s\n"
                     "Game     = %s\n",
                     file, eco_ref(file), calc, eco_ref(calc), txt.c_str() );
        }
    }

    // results twic-1078-1102.pgn
    //   eco4.txt 72161 successes, 8583 failures
    //   eco5.txt 71537 successes, 9207 failures
    printf( "%d successes, %d failures\n", successes, failures );
}
#endif

void GamesCache::Publish( const std::string &html_out_file,
                          const std::string &header,
                          const std::string &footer,
                          std::map<char,std::string> &macros,
                          const std::vector<std::pair<std::string,std::string>> &menu, int menu_idx )
{
    // Write HTML files in Unix eol mode ('\n' only), even on Windows systems
    std::ofstream fout( html_out_file, std::ios_base::out|std::ios_base::binary );
    if( !fout )
    {
        printf( "Error: Could not create output file %s\n", html_out_file.c_str() );
        return;
    }

    // Write out the the html
    std::string h = macro_substitution( header, macros, menu, menu_idx );
    util::putline(fout,h);

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

        // Write prefix
        std::string s = gd.prefix_txt;
        size_t offset = s.find_last_of('\n');
        size_t len    = s.length();
        if( offset!=std::string::npos && offset==len-1 && s[offset-1]=='\n' )
            s = s.substr(0,len-1);  // crude transformation, end of prefix "\n\n" -> "\n"
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
        std::string out;
        out = md(s);
        util::puts(fout,out);
        if (!skip_game)
        {

            if (!skip_intro)
            {
                // Write header
                //gd->ToFileTxtGameDetails( s );

                s = "<h2>";
                s += white;
                s += " - ";
                s += black;
                s += "</h2>";
                std::string t;
                if (gd.r.event.find('?') == std::string::npos)
                {
                    t = gd.r.event;
                }
                std::string year = gd.r.date.substr(0, 4);
                if (year.find('?') == std::string::npos)
                {
                    if( t.find(year) == std::string::npos ) // in case event includes year!
                    {
                        t += " ";
                        t += year;
                    }
                }
                if( t.length() > 0 )
                {
                    s += '\n';
                    s += "<h3>";
                    s += t;
                    s += "</h3>";
                }
                util::putline(fout, s);
            }

            // Write Game body
            gd.ToPublishTxtGameBody(s, diagram_base, mv_base, neg_base, publish_options);
            util::putline(fout,s);
        }
    }
    std::string f = macro_substitution( footer, macros, menu, menu_idx );
    util::putline(fout,f);
}
