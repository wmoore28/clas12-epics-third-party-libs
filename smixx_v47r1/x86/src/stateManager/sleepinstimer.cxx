#include <iostream>
using namespace std;
#include "ut_sm.hxx"
#include "sleepinstimer.hxx"
#include "resumehandler.hxx"

extern ResumeHandler resumeHandler;
//                                                               July 2011
//                                                               B. Franek
// Copyright Information:
//      Copyright (C) 1996-2011 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------

	SleepInsTimer::SleepInsTimer( const Name& parent)
	{
		_parentObjectName = parent;
	}
//-------------------------------------------------------------------------------
	void SleepInsTimer::startTimer(int seconds)
	{
		cout << "starting timer  time = " << seconds <<endl;
		start(seconds);
	}
//-----------------------------------------------------------------------------
	void SleepInsTimer::timerHandler()
	{
		print_obj(_parentObjectName);
		cout  << "  Timer Expired" << endl;
		resumeHandler.objectReadyToResume(_parentObjectName);
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
	}


