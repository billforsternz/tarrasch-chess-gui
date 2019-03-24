/****************************************************************************
 * Simple mechanism to detect whether a modal dialog is currently running
 * Include as a member for any custom modal dialogs, or as an automatic
 * (stack) variable next to any ShowModal() calls on system dialogs.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2019, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef DIALOG_DETECT_H
#define DIALOG_DETECT_H

class DialogDetect
{
public:
	DialogDetect()
	{
		counter++;
	}

	~DialogDetect()
	{
        counter--;
        if( counter==0 && func )
        {
            void (*temp)(std::string&);
            temp = func;
            func = 0;
            temp(parm);
        }
	}

	static bool IsOpen()
	{
		return counter>0;
	}

	static void OnClose( void (*f)(std::string&), std::string p )
	{
        func = f;
		parm = p;
	}

private:
	static int counter;
    static void (*func)( std::string& );
    static std::string parm;
};

#endif // DIALOG_DETECT_H
