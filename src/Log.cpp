/****************************************************************************
 * Log everything to a .pgn file
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include <stdio.h>
#include "GameDocument.h"
#include "GameLogic.h"
#include "GamesCache.h"
#include "DebugPrintf.h"
#include "Repository.h"
#include "Objects.h"
#include "Log.h"
using namespace std;
using namespace thc;

// Init
Log::Log()
{
}

void Log::SaveGame( GameDocument *gd, bool editing_log )
{
    if( !gd->IsEmpty() )
    {
        gd->FleshOutDate();
        gd->FleshOutMoves();
        std::string head2;
        gd->ToFileTxtGameDetails( head2 );
        std::string body2;
        gd->ToFileTxtGameBody( body2 );
        bool diff = (head2!=this->head || body2!=this->body);
        if( diff )
        {
            this->head = head2;
            this->body = body2;
            FILE *file = NULL;
            std::string log_filename( objs.repository->log.m_file.c_str() );
            time_t filetime_before = 0;
            if( ::wxFileExists(log_filename) )
                filetime_before = ::wxFileModificationTime(log_filename);
            if( !editing_log && objs.repository->log.m_enabled )
            {
                file = fopen( objs.repository->log.m_file.c_str(), "ab" );
            }
            if( file )
            {
                std::string filename( objs.repository->log.m_file.c_str() );
                fseek(file,0,SEEK_END);
                long filelen_before = ftell(file);
                fwrite( head2.c_str(), 1, head2.length(), file );
                fwrite( body2.c_str(), 1, body2.length(), file );
                fclose( file );
                objs.gl->pf.UpdateKnownFile( log_filename, filetime_before, filelen_before, head2.length()+body2.length() );
            }
        }
    }
}

