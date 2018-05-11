
//----------------------------- WaitIns  Class ---------------------------------
#ifndef WAITINS_HH
#define WAITINS_HH

#include "parameters.hxx"
#include "instruction.hxx"

class State;
class Action;
#include "namevector.hxx"
#include "namelist.hxx"
//
//                                                               January 2009
//                                                               B. Franek
// Copyright Information:
//      Copyright (C) 1996-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------

class WaitIns : public Instruction
{
public :
	WaitIns( char lines[][MAXRECL],int lev
		,int& no_lines, SMIObject *pobj, State* pstat, Action*);

	void whatAreYou();

	int execute( Name& endState );   // 0-normal,  2-suspended
	
	int reportingLockedObject( Name& lockedObjName); // ret: 0-normal 1-all objects
						//	locked
  
/** For suspended WAIT instruction it will do all the necessary cleaning
  so that it appears like object 'objName' was not included in the set 'setName'
  at the time when the WAIT was initially executed
  */	
	int reportingObjectRemovedFromSet
	               ( const Name& objName, const Name& setName);

private:
//-------------- functions --------------------------------------
/** finds all the objects referenced by the WAIT instruction. I.E.
    the directly referenced objects plust the objects contained in the
    referenced sets
*/
	void getCurrentRefObjects();

/** will attempt to lock all the objects in
    the _refObjectsAtStartExecution list
*/
	void attemptToLockObjects();
   
//	void lockObjects();

/** will unlock the objects in _lockObjects  list
*/
	void unlockObjects();

//-------------- data -------------------------------------------
	
	int _level;

	Action* _pParentAction;

	int _suspend_flag;  // 0 - 'fresh' WAIT
			   //  1 - suspended because some of the objects are 'busy'


	Name _objName;    // Name of the parent object 

/** Pointer to the parent object
*/
	SMIObject* _pParentObject; 
	
/** List of objects that are listed in the WAIT
*/
	NameVector _refObjects;
	
/** List of object sets that are listed in the WAIT
*/
	NameVector _refObjectSets;  
	
	NameList _refObjectsAtStartExecution;
	
	NameList _lockedObjects;
	
	NameList _objectsWaitingForLock;              
};

#endif
