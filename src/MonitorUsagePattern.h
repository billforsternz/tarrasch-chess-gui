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
#ifndef MONITOR_USAGE_PATTERN_H
#define MONITOR_USAGE_PATTERN_H

class MonitorUsagePattern
{
public:
    void FileOpenDialogCancel( unsigned long insertion_point );
    void ShowNewDocument();
    void TabModified();
    bool DontNeedNewTab( unsigned long insertion_point );
    MonitorUsagePattern() { after_file_open_dialog_cancel=false; initial_insertion_point=0; }
private:
    bool after_file_open_dialog_cancel;
    unsigned long initial_insertion_point;
};

#endif // MONITOR_USAGE_PATTERN_H


