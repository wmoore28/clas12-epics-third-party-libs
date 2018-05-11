
//----------------------------- SleepIns  Class ---------------------------------
#ifndef SLEEPINS_HH
#define SLEEPINS_HH

#include "parameters.hxx"
#include "instruction.hxx"

class State;
class Action;
#include "namevector.hxx"
#include "namelist.hxx"
#include "sleepinstimer.hxx"
//
//                                                               July 2011
//                                                               B. Franek
// Copyright Information:
//      Copyright (C) 1996-2011 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------

class SleepIns : public Instruction
{
public :
	SleepIns( char lines[][MAXRECL],int lev
		,int& no_lines, SMIObject *pobj, State* pstat, Action*);

	void whatAreYou();

	int execute( Name& endState );   // 0-normal,  2-suspended
	
private:
//-------------- functions --------------------------------------

//-------------- data -------------------------------------------
	
	int _level;

	Action* _pParentAction;

	Name _objName;    // Name of the parent object 

/** Pointer to the parent object
*/
	SMIObject* _pParentObject; 

	int _timeToSleep;
	
	int _suspend_flag ;

/** Pointer to timer object
*/
	SleepInsTimer* _pTimer;
};

#endif
