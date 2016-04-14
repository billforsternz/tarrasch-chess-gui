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
#include "AutoTimer.h"
#include "DebugPrintf.h"
#include "wx/progdlg.h"

class ProgressBarLegacy
{
public:
    ProgressBarLegacy( std::string title, std::string desc, bool abortable=true, wxWindow *parent=NULL, FILE *ifile=0 ) :
        progress( title, desc, 1000, parent,
                                    wxPD_APP_MODAL+
                                    wxPD_AUTO_HIDE+
                                    wxPD_ELAPSED_TIME+
                                    (abortable ? wxPD_CAN_ABORT : 0)+
                                    wxPD_ESTIMATED_TIME )
    {
        cprintf( "ProgressBar() %s, %s\n", title.c_str(), parent==NULL?"parent==NULL":"?" );
        modulo_256=0;
        old_permill=-1;
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
                int permill=0;
                if( file_len == 0 )
                    permill = 1000;
                else if( file_len < 1000000 )
                    permill = (int)( (file_offset*1000L) / file_len );
                else // if( file_len > 1000000 )
                    permill = (int)( file_offset / (file_len/1000L) );
                if( permill != old_permill )
                {
                    progress.SetFocus();
                    bool abort = !progress.Update( permill>1000 ? 1000 : permill );
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
    bool Percent( int percent, const std::string &s="" )  // return true if abort
    {
        return Permill( percent*10, s );
    }
    bool Permill( int permill, const std::string &s="" )  // return true if abort
    {
        bool abort = false;
        if( old_permill != permill )
        {
            wxString wxmsg( s.c_str() );
            progress.SetFocus();
            abort = !progress.Update( permill>1000 ? 1000 : permill, wxmsg );
            old_permill = permill;
        }
        return abort;
    }

private:
    unsigned char modulo_256;
    int old_permill;
    wxProgressDialog progress;
    FILE *ifile;
    unsigned long file_len;

};


class ProgressBar
{
public:
    ProgressBar( std::string title, std::string desc, bool abortable=true, wxWindow *parent=NULL, FILE *ifile=0  ) :
        timer(NULL)
    {
        this->progress = NULL;
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
        old_permill=-1;
    }

    ~ProgressBar()
    {
        if( progress )
            delete progress;
        progress = NULL;
    }

    void DrawNow()
    {
        if( !progress )
        {
            progress = new wxProgressDialog( title, desc, 1000, parent,
                                wxPD_APP_MODAL+
                                wxPD_AUTO_HIDE+
                                wxPD_ELAPSED_TIME+
                                (abortable ? wxPD_CAN_ABORT : 0)+
                                wxPD_ESTIMATED_TIME );
        }
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
                    if( !progress && permill>0 && PredictEnd(permill)>2000 )
                    {
                        progress = new wxProgressDialog( title, desc, 1000, parent,
                                            wxPD_APP_MODAL+
                                            wxPD_AUTO_HIDE+
                                            wxPD_ELAPSED_TIME+
                                            (abortable ? wxPD_CAN_ABORT : 0)+
                                            wxPD_ESTIMATED_TIME );
                    }
                    else if( progress )
                    {
                        progress->SetFocus();
                        bool abort = !progress->Update( permill>1000 ? 1000 : permill );
                        if( abort )
                            return true;
                    }
                }
                old_permill = permill;
            }
            modulo_256++;
            modulo_256 &= 0xff;
        }
        return false;
    }

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
        bool abort = false;
        if( old_permill != permill )
        {
            if( !progress && permill>0 && PredictEnd(permill)>2000 )
            {
                progress = new wxProgressDialog( title, desc, 1000, parent,
                                    wxPD_APP_MODAL+
                                    wxPD_AUTO_HIDE+
                                    wxPD_ELAPSED_TIME+
                                    (abortable ? wxPD_CAN_ABORT : 0)+
                                    wxPD_ESTIMATED_TIME );
            }
            else if( progress )
            {
                wxString wxmsg( s.c_str() );
                progress->SetFocus();
                abort = !progress->Update( permill>1000 ? 1000 : permill, wxmsg );
                old_permill = permill;
            }
        }
        return abort;
    }
    bool Perfraction( int numerator, int denominator, const std::string &s="" )  // return true if abort
    {
        int permill;
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
    wxProgressDialog *progress;
    FILE *ifile;
    unsigned long file_len;
    AutoTimer timer;
    std::string title;
    std::string desc;
    bool abortable;
    wxWindow *parent;
};




#endif    // PROGRESS_BAR_H
