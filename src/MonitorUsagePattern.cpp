/****************************************************************************
 * Monitor Usage Pattern state machine
 * A state machine to monitor various aspects of the way the program is
 * being used. Currently used to detect whether user has loaded a PGN
 * with a cancel games dialog option (this is possible), but then not
 * done anything else. In that case a further file open doesn't open
 * a new tab.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/

#include "MonitorUsagePattern.h"

// Start of behaviour we are monitoring - a file is opened, but the dialog is cancelled
//  behaviour continues until the user "does something" significant
void MonitorUsagePattern::FileOpenDialogCancel( unsigned long insertion_point )
{
    after_file_open_dialog_cancel=true;
    initial_insertion_point=insertion_point;
}

// Any tab switch/create/delete counts as the user "doing something", this ends the
//  behaviour
void MonitorUsagePattern::ShowNewDocument()
{
    after_file_open_dialog_cancel=false;
}

// If the user changes anything in the game, that's certainly "doing something, this
//  ends the behaviour
void MonitorUsagePattern::TabModified()
{
    after_file_open_dialog_cancel=false;
}

// If the user changes hasn't done anything significant, or even moved from the initial
//  insertion point in the document, we don't need a new tab
bool MonitorUsagePattern::DontNeedNewTab( unsigned long insertion_point )
{
    return after_file_open_dialog_cancel && initial_insertion_point==insertion_point;
}



