//-----------------------------  WaitIns  Class ------- wait instruction -------
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
#include "waitins.hxx"
#include "smiobject.hxx"
//
//                                                        B. Franek
//                                                        January 2009
// Copyright Information:
//      Copyright (C) 1996-2009 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
//------------------------------ WaitIns -------------------------------------

WaitIns::WaitIns
	( char lines[][MAXRECL], int lev, int& no_lines
	, SMIObject *pobj, State *pstat, Action* pact)
{
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line after the 'wait'
// lev    ....... the block level

// Output :
// no_lines ....... number of lines in 'wait' (not counting the first 'wait')
//----------------------------------------------------------------------------

	if ( pstat == NULL ) {}  // will remove compiler warning

	_objName = pobj->name();
	_pParentObject = pobj;
   
	_pParentAction = pact;

//   cout << " WAIT belongs to object : " << _objName << "\n";

	_level = lev;

	_suspend_flag = 0;

	int il, objectnum, objectsetnum;

	il = 0;

	sscanf(lines[il]," %d",&objectnum);
	
//   cout << "No. of referenced objects : " << objectnum << "\n";

	char namestr[MAXRECL]; int io;
	
	for (io=0; io<objectnum; io++)
	{
		il++;
		sscanf(lines[il],"%s",namestr);
		_refObjects += namestr;
	}
	
	il++;
	
	sscanf(lines[il]," %d",&objectsetnum);
	
//   cout << "No. of referenced object sets : " << objectsetnum << "\n";

	
	for (io=0; io<objectsetnum; io++)
	{
		il++;
		sscanf(lines[il],"%s",namestr);
		_refObjectSets += namestr;
	}
	
	no_lines = il + 1;

	return ;
}
//==================================================================
void WaitIns::whatAreYou()
{
	indent(2*_level+5);
	
	cout << "wait ( ";
	
	int io, numobj, numobjset; 
	
	numobj = _refObjects.length();
	numobjset = _refObjectSets.length();
	
	for ( io=0; io<numobj; io++)
	{
		if (io>0) {cout << ", ";}
		cout << _refObjects[io].getString() ;
	}
	

	if (numobjset ==0) {cout << " )" << endl; return;}
	else {cout << ", ";}

	for ( io=0; io<numobjset; io++)
	{
		if (io>0) {cout << ", ";}
		cout << "all_in " << _refObjectSets[io].getString() ;
	}
	
	 cout << " )" << endl;
	return;
}
//========================================================================
int WaitIns::execute(Name& endState)
{

	endState = "not changed";

//debug beg
if ( dbg > 5 )
{
	cout << endl << " start================= WaitIns::execute =========== " << endl;
	cout  << "   WAIT instruction(" << this << " of " << _objName << endl;
	cout << "  _suspend_flag = " << _suspend_flag << endl;
}
//debug end

	if ( _suspend_flag == 0 )
	{ // This is a fresh unsuspended WAIT
		_refObjectsAtStartExecution.removeAll();
		_lockedObjects.removeAll();
		_objectsWaitingForLock.removeAll();
		
		getCurrentRefObjects();  //This obviously depends on the contents of Sets
//		cout << endl << " Referenced objects at the start: " << endl;
//		_refObjectsAtStartExecution.out(" ");

		attemptToLockObjects();
//debug beg
if ( dbg > 5 )
{
	cout << endl << " Locked objects : " << endl;
	_lockedObjects.out(" ");
}
//debug end

//debug beg
if ( dbg > 5 )
{
	cout << endl << " Objects waiting for lock: " << endl;
	_objectsWaitingForLock.out(" ");
}
//debug end	
		if (_objectsWaitingForLock.numOfEntries() == 0 )
		{ // all the objects could be locked this means that WAIT
		  // finished
			unlockObjects();
//debug beg
if ( dbg > 5 )
{
	cout << " WAIT terminated" << endl;
	cout << " return================= WaitIns::execute =========== "
	 << endl << endl;
}
			return 0;
		}
		else
		{ // some objects will have to wait for the lock...WAIT suspended
			_pParentObject->setPointerToSuspendedWAIT(this);
		//***** need to tell ResumeHandler about this
			resumeHandler.registerSuspendedObject(_pParentObject);
			_suspend_flag = 1;
//debug beg
if ( dbg > 5 )
{
	cout << " WAIT suspended" << endl;
	cout << " return================= WaitIns::execute =========== "
	 << endl << endl;
}
//debug end
			return 2;
		}	
	}
	else
	{  // this is suspended WAIT that was now released
		unlockObjects();
		_suspend_flag = 0;
//debug beg
if ( dbg > 5 )
{
	cout << " WAIT terminated" << endl;
	cout << " return================= WaitIns::execute =========== "
	 << endl << endl;
}
//debug end
		return 0;
	}	
}
//=========================================================================
int WaitIns::reportingLockedObject( Name& lockedObjName)
{
	int flg = _objectsWaitingForLock.remove(lockedObjName);
	
	if ( flg == 0 )
	{
		cout << lockedObjName << " not found in the list of waiting"
		<< "objects" << endl; abort();
	}
	
	flg = _lockedObjects.add(lockedObjName);
	
	if ( flg == 0 )
	{
		cout << lockedObjName << " is already in the list of locked"
		<< "objects" << endl; abort();
	}
	
	if ( _objectsWaitingForLock.numOfEntries() == 0 )
	{ // all objects are locked
		return 1;
	}
	else
	{
		return 0;
	}
}
	
//=========================================================================
void WaitIns::getCurrentRefObjects()
{
	_refObjectsAtStartExecution.removeAll();
	_refObjectsAtStartExecution.add(_refObjects);
	
	int numObjSets = _refObjectSets.length();
	
	int is; Name objSetName; void* ptnv; SMIObjectSet* ptnSet;
	Name objInSet;
	
	for ( is=0; is<numObjSets; is++ )
	{  // loop over the referenced Sets
		objSetName = _refObjectSets[is];
		ptnv = allSMIObjectSets.gimePointer(objSetName);
		if (ptnv == 0)
		{
			cout << " Set " << objSetName << " is not declared"
			     << endl; abort();
		}
		ptnSet = (SMIObjectSet*) ptnv;
		
		ptnSet->reset();
		while (ptnSet->nextObject(objInSet))
		{ // pick up the objects in the set
			_refObjectsAtStartExecution.add(objInSet);
		}
		
	}
	
	return;
}
//=========================================================================
void WaitIns::attemptToLockObjects()
{
	Name refObjName;
	SMIObject* pSMIObj;
	int lockFlg;
	
	_refObjectsAtStartExecution.reset();
	while (_refObjectsAtStartExecution.nextItem(refObjName) )
	{
		pSMIObj = allSMIObjects.gimePointer(refObjName);
		lockFlg = pSMIObj->lock(_objName.getString());
		
		if (lockFlg)
		{ // the object was locked
			_lockedObjects.add(refObjName);
		}
		else
		{  // the object could not be locked and the lock was queued
			_objectsWaitingForLock.add(refObjName);
		}
	}
	
	return;
}
//===========================================================================
void WaitIns::unlockObjects()
{
	Name refObjName;
	SMIObject* pSMIObj;
	
	_lockedObjects.reset();
	while (_lockedObjects.nextItem(refObjName))
	{
		pSMIObj = allSMIObjects.gimePointer(refObjName);
		pSMIObj->unlock();
	}
	
	return;
}
//============================================================================
int WaitIns::reportingObjectRemovedFromSet
	               ( const Name& remObjName, const Name& setName)
{

	if ( _suspend_flag != 1 )
	{
		cout << "  *** Internal error : WAIT is not suspended" <<
		endl;
		abort();
	}

	if ( !_refObjectSets.isPresent(setName) )
	{ // setName is not referecenced...not interesting
		return 0;
	}
	
	if ( _refObjects.isPresent(remObjName) )
	{  // object is directly deferenced...not interesting
		return 0;
	}
	
	// The object 'remObjName' is referenced only through set 'setName'
	// it is either locked, or has the lock queued
	
	int flg = _lockedObjects.remove(remObjName);
	
	SMIObject* pSMIObj = allSMIObjects.gimePointer(remObjName);
	
	if ( flg == 1) 
	{ // object was found in the locked objects list and removed,
	  // now unlock it
		pSMIObj->unlock();
		return 0;
	}
	
	// the object must be waiting for the lock
	
	flg = _objectsWaitingForLock.remove(remObjName);
	
	if ( flg == 0 )
	{ // object not found in the list
		cout << remObjName << " not found either in locked object list"
		<< " or in objects waiting for lock list" << endl; abort();
	}
	
	// the object has lock action waiting...have to remove it
	pSMIObj->removeQueuedLock(_objName);
	
	
	if ( _objectsWaitingForLock.numOfEntries() == 0 )
	{ // all objects are locked
		return 1;
	}
	else
	{
		return 0;
	}
}	
