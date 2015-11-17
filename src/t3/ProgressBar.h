/****************************************************************************
 * Progress thermometer
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <stdio.h>
#include "wx/progdlg.h"

class ProgressBar
{
public:
    ProgressBar() :
        progress( "Creating1", "Creating2", 100, NULL,
                                    wxPD_APP_MODAL+
                                    wxPD_AUTO_HIDE+
                                    wxPD_ELAPSED_TIME+
                                    wxPD_CAN_ABORT+
                                    wxPD_ESTIMATED_TIME )
    {
        ifile = 0;
        old_percent=0;
        percent=0;
    }

    void SetFile( FILE *ifile )
    {
        this->ifile = ifile;
        fseek(ifile,0,SEEK_END);
        file_len = ftell(ifile);
        fseek(ifile,0,SEEK_SET);
    }

    bool ProgressFile()
    {
        if( ifile )
        {
            static unsigned char modulo_256;
            if( modulo_256 == 0 )
            {
                unsigned long file_offset=ftell(ifile);
                int percent;
                if( file_len == 0 )
                    percent = 100;
                else if( file_len < 10000000 )
                    percent = (int)( (file_offset*100L) / file_len );
                else // if( file_len > 10000000 )
                    percent = (int)( file_offset / (file_len/100L) );
                if( percent != old_percent )
                {
                    if( !progress.Update( percent>100 ? 100 : percent ) )
                        return true;
                }
                old_percent = percent;
            }
            modulo_256++;
            modulo_256 &= 0xff;
        }
        return false;
    }

    bool Progress( int percent ) { return false; }
    bool Partial( int percent ) { return false; }
    void SetPartial( int begin, int end ) {}

private:
    int old_percent;
    int percent;
    wxProgressDialog progress;
    FILE *ifile;
    unsigned long file_len;

};


#endif    // PROGRESS_BAR_H
