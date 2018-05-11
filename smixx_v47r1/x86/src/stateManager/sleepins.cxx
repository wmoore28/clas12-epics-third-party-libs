//-----------------------------  SleepIns  Class ------- SLEEP instruction -------
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>

#include "parameters.hxx"
#include "ut_sm.hxx"
//---------------------- Externals ----------------------
#include "objectregistrar.hxx"
#include "registrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern Registrar allSMIObjectSets;
#include "resumehandler.hxx"
   extern ResumeHandler resumeHandler;
   extern int dbg;
//-------------------------------------------------------
#include "sleepins.hxx"
#include "smiobject.hxx"
//
//                                                        B. Franek
//                                                        July 2011
// Copyright Information:
//      Copyright (C) 1996-2011 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
//------------------------------ SleepIns -------------------------------------

SleepIns::SleepIns
	( char lines[][MAXRECL], int lev, int& no_lines
	, SMIObject *pobj, State *pstat, Action* pact)
{
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line after the 'sleep'
// lev    ....... the block level

// Output :
// no_lines ....... number of lines in 'sleep' (not counting the first 'sleep')
//----------------------------------------------------------------------------

	if ( pstat == NULL ) {}  // will remove compiler warning

	_objName = pobj->name();
	_pParentObject = pobj;
   
	_pParentAction = pact;
	
	_suspend_flag = 0;
	
	_pTimer = NULL;  // This will get created when need at least once

//   cout << " SLEEP belongs to object : " << _objName << "\n";

	_level = lev;

	int il;

	il = 0;

	sscanf(lines[il]," %d",&_timeToSleep);
	
	no_lines = 1;

	return ;
}
//==================================================================
void SleepIns::whatAreYou()
{
	indent(2*_level+5);
	
	cout << "sleep " << _timeToSleep << endl;
	
	return;
}
//========================================================================
int SleepIns::execute(Name& endState)
{

	endState = "not changed";

//debug beg
if ( dbg > 5 )
{
	cout << endl << " start================= SleepIns::execute =========== " << endl;
	cout  << "   SLEEP instruction(" << this << " of " << _objName << endl;
}
//debug end

	
	if ( _suspend_flag == 0 )
	{ // This is a fresh unsuspended SLEEP
		// will start time
		if ( _pTimer == NULL )
		{
			_pTimer = new SleepInsTimer(_pParentObject->name());
		}
		_pTimer->startTimer(_timeToSleep);
		
		_pParentObject->setPointerToSuspendedSLEEP(this);
		//***** need to tell ResumeHandler about this
		resumeHandler.registerSuspendedObject(_pParentObject);
		_suspend_flag = 1;
//debug beg
if ( dbg > 5 )
{
	cout << " SLEEP suspended" << endl;
	cout << " return================= SleepIns::execute =========== "
	 << endl << endl;
}
//debug end
		return 2;
			
	}
	else
	{  // this is suspended SLEEP that was now released
		_suspend_flag = 0;
//debug beg
if ( dbg > 5 )
{
	cout << "  terminated" << endl;
	cout << " return================= SleepIns::execute =========== "
	 << endl << endl;
}
//debug end
		return 0;
	}	
}

