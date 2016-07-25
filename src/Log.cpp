/****************************************************************************
 * Log everything to a .pgn file
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
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
        std::string head;
        gd->ToFileTxtGameDetails( head );
        std::string body;
        gd->ToFileTxtGameBody( body );
        bool diff = (head!=this->head || body!=this->body);
        if( diff )
        {
            this->head = head;
            this->body = body;
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
                fwrite( head.c_str(), 1, head.length(), file );
                fwrite( body.c_str(), 1, body.length(), file );
                fclose( file );
                objs.gl->pf.UpdateKnownFile( log_filename, filetime_before, filelen_before, head.length()+body.length() );
            }
        }
    }
}

