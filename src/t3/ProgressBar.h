/****************************************************************************
 * Progress thermometer
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2015, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <stdio.h>
#include <string>
#include "wx/progdlg.h"

class ProgressBar
{
public:
    ProgressBar( std::string title, std::string desc, FILE *ifile=0 ) :
        progress( title, desc, 100, NULL,
                                    wxPD_APP_MODAL+
                                    wxPD_AUTO_HIDE+
                                    wxPD_ELAPSED_TIME+
                                    wxPD_CAN_ABORT+
                                    wxPD_ESTIMATED_TIME )
    {
        modulo_256=0;
        old_percent=0;
        this->ifile = ifile;
        if( ifile )
        {
            fseek(ifile,0,SEEK_END);
            file_len = ftell(ifile);
            fseek(ifile,0,SEEK_SET);
        }
    }

    bool ProgressFile() // return true if abort
    {
        if( ifile )
        {
            if( modulo_256 == 0 )
            {
                unsigned long file_offset=ftell(ifile);
                int percent=0;
                if( file_len == 0 )
                    percent = 100;
                else if( file_len < 10000000 )
                    percent = (int)( (file_offset*100L) / file_len );
                else // if( file_len > 10000000 )
                    percent = (int)( file_offset / (file_len/100L) );
                if( percent != old_percent )
                {
                    bool abort = !progress.Update( percent>100 ? 100 : percent );
                    if( abort )
                        return true;
                }
                old_percent = percent;
            }
            modulo_256++;
            modulo_256 &= 0xff;
        }
        return false;
    }

    bool Progress( int percent ) // return true if abort
    {
        bool abort = !progress.Update( percent>100 ? 100 : percent );
        old_percent = percent;
        return abort;
    }

private:
    unsigned char modulo_256;
    int old_percent;
    wxProgressDialog progress;
    FILE *ifile;
    unsigned long file_len;

};


#endif    // PROGRESS_BAR_H
