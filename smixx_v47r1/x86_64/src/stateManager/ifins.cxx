//-----------------------------  IfIns  Class ------- if instruction -------
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>

#include "parameters.hxx"
#include "ut_sm.hxx"
//---------------------- Externals ----------------------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
#include "ifhandler.hxx"
   extern IfHandler ifHandler;
#include "resumehandler.hxx"
   extern ResumeHandler resumeHandler;
   extern int dbg;
   extern int unlockedIfs; // when =1, IF's objects are unlocked once
                           //   conditions are evaluated
//-------------------------------------------------------
#include "ifins.hxx"
#include "condition.hxx"
#include "smiobject.hxx"
#include "instruction_return_status.hxx"
//
//                                                        B. Franek
//                                                       15-Apr-1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
//------------------------------ IfIns -------------------------------------

 IfIns::IfIns
 ( char lines[][MAXRECL],int lev, PtrVector& allBlocks,int& no_lines
 , SMIObject *pobj, State *pstat, Action* pact){
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line after the 'if'
// lev    ....... the block level
// pAllBlocks ..... array of pointers of all the blocks belonging to the action
// Output :
// no_lines ....... number of lines in 'if' (not counting the first 'if')
//----------------------------------------------------------------------------

	if ( pstat == NULL ) {}  // will remove compiler warning
	
   _objName = pobj->name();
   	_pParentObject = pobj;
	_pParentState  = pstat;
   
    _pParentAction = pact;

//   cout << " If belongs to object : " << _objName << "\n";

   _level = lev;
   _numCond = 0;
   _isuspended = 0;
   _suspend_flag = fresh;

   int il;

   il = 0;
   sscanf(lines[il]," %d",&_numCond);
//   cout << "No. of conditions : " << _numCond << "\n";

   for (int icond=0; icond<_numCond; icond++) {

     int numCondlines;

     il++;
     Condition *pCondition;     
     pCondition = new Condition(&lines[il],&numCondlines,
                  _pParentObject,_pParentState,_pParentAction);
     il = il + numCondlines;

     int ref_blockno,ref_level;

     sscanf(lines[il],"%d %d",&ref_blockno,&ref_level);
//     cout << "Execute block : " << ref_blockno << 
//             " level : " << ref_level << "\n";

     InsList *pInsList;
     pInsList = static_cast<InsList*>(allBlocks[ref_blockno]);
  
     char condition[MAXRECL];
     pCondition->whatAreYou(MAXRECL,condition);
//     cout << "Addind condition : " << condition << "\n";

        _conditions += pCondition;
        _insLists += pInsList;

   }


  no_lines = il + 1;
  return ;
}
//


//------------------------------ whatAreYou ---------------------------------
void IfIns::whatAreYou(){
    Condition *pCondition;
    InsList *pInsList;
    
    char condition[MAXRECL];
    cout << "\n";
    indent(_level*2+5);

    pCondition = static_cast<Condition*>(_conditions[0]);
    pCondition->whatAreYou(MAXRECL,condition);

    cout  << "if " << condition << " then"  << endl;
    pInsList = static_cast<InsList*>(_insLists[0]);

    indent(_level*2+5);
    cout << "     " << pInsList << "\n";

    for (int i=1; i<_numCond; i++) {

        pCondition = static_cast<Condition*>(_conditions[i]);
        pCondition->whatAreYou(MAXRECL,condition);

        indent(_level*2+5);
        if (!strcmp(condition,"else")) {
            cout << "else" << "\n";
        }
        else {
            cout << "else if " << condition << " then"  << "\n";
        }
        pInsList = static_cast<InsList*>(_insLists[i]);

        indent(_level*2+5);
        cout << "     " << pInsList << "\n";
    }
    indent(_level*2+5);
    cout << "end if" << endl << endl;
    return;
}
//----------------------------------- execute --------------------------------
int IfIns::execute( Name& endState ){
//

	InsList *pInsList;

	int trueBlock;
	InstructionReturnStatus_t blockReturnStat;

	blockReturnStat = normal;

//debug beg
if ( dbg > 5 )
{
cout << endl << " start================== IfIns::execute =========== " << endl;
cout << "  _suspend_flag = " << _suspend_flag << endl;
cout  << "   IF instruction(" << this << ") of " << _objName << endl;
cout  << "  NoOfCond : " << _numCond << "\n";
}
//debug end

	switch ( _suspend_flag)
	{
//----------------------------------------------------------------------------
	case fresh:
//-------------------------- fresh IF instruction ----------------------------

				
		// will build list of objects currently referenced by IF's conditions
		getCurrentRefObjects();  //This obviously depends on the contents of Sets
//debug beg
if ( dbg > 5 )
{
cout << endl << " Referenced objects at the start: " << endl;
_refObjectsAtStartExecution.out(" ");
}
//debug end
		attemptToLockObjects();
//debug beg
if ( dbg > 5 )
{
cout << endl << " Locked objects : " << endl;
_lockedObjects.out(" ");

cout << endl << " Objects waiting for lock: " << endl;
_objectsWaitingForLock.out(" ");
}
//debug end	
		if (_objectsWaitingForLock.numOfEntries() > 0 )
		{
			resumeHandler.registerSuspendedObject(_pParentObject);
			freezeConditions();
			_pParentObject->setPointerToTypeISuspendedIF(this);
			_suspend_flag = suspendedDueToCondition;
//debug beg
if ( dbg > 5 )
{
	cout << " Objects waiting to be locked, IF suspended" << endl;
	cout << " return================= IfIns::execute =========== "
	 << endl << endl;
}
//debug end
			return instructionSuspended;
		}

		trueBlock = findTrueBlock() ;
		if (unlockedIfs) { unlockLockedObjects();}
		if ( trueBlock < 0 ) {
			if (!unlockedIfs) { unlockLockedObjects();}
			_isuspended = 0;_suspend_flag = fresh;
			if ( dbg > 5 ) { endOfIfExecutionDiagnostics(); }
			return normal;
		}

		goto executeInstructionBlock;

//-----------------------------------------------------------------------------
	case suspendedDueToCondition:
//--------------- was suspended due to not executable condition ---------------
  		if ( _objectsWaitingForLock.numOfEntries() > 0 )
		{
  			cout << " There are still objects waiting for lock\n";
  			exit(1);
  		}
//
  		trueBlock = findTrueBlock() ;  unfreezeConditions();
		if (unlockedIfs) { unlockLockedObjects();}
//debug beg
if ( dbg > 5 )
{
cout << " trueBlock = " << trueBlock << endl;
}
//debug end
		if ( trueBlock < 0 ) {
			if (!unlockedIfs) { unlockLockedObjects();}
			_isuspended = 0;_suspend_flag = fresh;
			if ( dbg > 5 ) { endOfIfExecutionDiagnostics(); }
			return normal;
		}
		
		goto executeInstructionBlock;

//---------------------------------------------------------------------------
	case suspendedDueToSuspendedBlock:
//------------------------- suspended due to suspended instruction block ----

		trueBlock = _isuspended;
		
		goto executeInstructionBlock;
	
//---------------------------------------------------------------------------
	default:
//---------------------------------------------------------------------------
		cout << " IfIns.... suspend flag has illegal value : " 
                     << _suspend_flag << "\n";
		exit(1);
	}
	
//----------------------------------------------------------------------------		
		executeInstructionBlock:
//----------------------------------------------------------------------------
		pInsList = static_cast<InsList*>(_insLists[trueBlock]);
		blockReturnStat = pInsList->execute(endState);
	
		if (blockReturnStat == instructionSuspended ) { 
			_suspend_flag = suspendedDueToSuspendedBlock;
			_isuspended = trueBlock;

//debug beg
if ( dbg > 5 )
{
cout << endl << " return instruction suspended ================== IfIns::execute =========== " << endl;
}
//debug end
			return instructionSuspended; 
		}
		else if ( blockReturnStat == normal ||
			  blockReturnStat ==instructionFinishesTerminatingAction )
		{
			if (!unlockedIfs) { unlockLockedObjects();}
			_isuspended = 0;_suspend_flag = fresh;
			if ( dbg > 5 ) { endOfIfExecutionDiagnostics(); }
			return blockReturnStat;	
		}
		else { cout << "IfIns...return from execute is illegal\n";exit(1);}    
	
}
//---------------------------  endOfIfExecutionDiagnostics()  ---------
void IfIns::endOfIfExecutionDiagnostics()
{
	indent(_level);
	cout  << "  -------------  END-OF-IF instruction(" 
         << this << ")---------------------" << endl << endl;
	 return;
}
//--------------------------  lockObjects  ------------------------------------
//
void IfIns::lockObjects() {
//
    Condition *pCondition;

   Name obj;
   SMIObject *pSMIObj;

	while (!_tobeLocked.isEmpty()) {
	        obj = _tobeLocked.element(0);
		_tobeLocked.remove(obj);
	}
        pCondition = static_cast<Condition*>(_conditions[0]);
	pCondition->objectSet(&_tobeLocked);

	for ( int iicond = 1; iicond < _numCond; iicond++ ) {
	    pCondition = static_cast<Condition*>(_conditions[iicond]);
	    pCondition->addto_objectSet(&_tobeLocked);
	}

//
   int numel = _tobeLocked.numOfEntries();
//
   for ( int iel = 0; iel < numel; iel++ ) {
         obj = _tobeLocked.element(iel);
         pSMIObj = allSMIObjects.gimePointer(obj);           
         int lockFlg = pSMIObj->lock(_objName.getString());
         if ( lockFlg ) { 
             if ( _tobeLocked.remove(obj) ) {
                  iel = iel -1;
                  numel = numel -1;
             }
         }
   }
//
   return;
}

//-----------------------------  unlockObjects  -------------------------------
//
void IfIns::unlockObjects() {
//
    Condition *pCondition;

   if( !_tobeLocked.isEmpty() ){
       cout << " Object set not empty at end if \n";
       exit(1);
   }

    pCondition = static_cast<Condition*>(_conditions[0]);
    pCondition->objectSet(&_tobeLocked);

   for ( int iicond = 1; iicond < _numCond; iicond++ ) {
       pCondition = static_cast<Condition*>(_conditions[iicond]);
       pCondition->addto_objectSet(&_tobeLocked);
   }

   Name obj;
   SMIObject* pSMIobj;
   int numel = _tobeLocked.numOfEntries();
   for ( int iel = 0; iel < numel; iel++ ) {
        obj = _tobeLocked.element(iel);
        pSMIobj = allSMIObjects.gimePointer(obj);           
        pSMIobj->unlock();
   }
//
   return;
}
//=========================================================================
void IfIns::attemptToLockObjects()
{
	Name refObjName;
	SMIObject* pSMIObj;
	int lockFlg;
	
	_lockedObjects.removeAll();
	_objectsWaitingForLock.removeAll();
	
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
void IfIns::unlockLockedObjects()
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
//
//----------------------------  findTrueBlock --------------------------
//
int IfIns::findTrueBlock() {

    Condition *pCondition;

//debug beg
if ( dbg > 5 )
{
	cout << " Looking for true condition " << endl;
}
//debug end

   int block;

   block = -1;

   for ( int icond=0; icond < _numCond; icond++ ) {
        pCondition = static_cast<Condition*>(_conditions[icond]);
        int  cond_flag = pCondition->evaluate();

         if ( cond_flag == -1 ) {
              cout << 
              " Ifins  procedural error  some objects are transiting\n";
                exit(1);
         }
         else if ( cond_flag == 1 ) {    
              block = icond;
              break;
         }
   }
//
   if ( block < 0 && _numCond > 1 ) {
        cout << " IfIns... no condition is true \n";
        exit(1);
   }

   return block;

}
//--------------------------- freezeConditions -------------------------
void IfIns::freezeConditions() {
    Condition *pCondition;

	for ( int icond=0; icond < _numCond; icond++ ) {
            pCondition = static_cast<Condition*>(_conditions[icond]);
	    pCondition->freeze();
	}
	return;
}
//--------------------------- unfreezeConditions -----------------------
void IfIns::unfreezeConditions() {
    Condition *pCondition;

	for ( int icond=0; icond < _numCond; icond++ ) {
	    pCondition = static_cast<Condition*>(_conditions[icond]);
            pCondition->unfreeze();
	}
	return;
}
//------------------------------------------  BF  Oct 2008  -------------
int IfIns::removeObjectFromSet( const Name& objName, const Name& setName)
{
//debug beg
if ( dbg > 5 )
{
	cout << endl << "start  ============================ IfIns::removeObjectFromSet ======" << endl;
	cout << "  object " << objName << "  Set " << setName << endl;
}
//debug end

	if ( _suspend_flag != suspendedDueToCondition )
	{
		cout << "  *** Internal error : IF is not TypeI suspended" <<
		endl;
		abort();
	}
	if ( !isObjectSetReferenced(setName) )
	{
//debug beg
if ( dbg > 5 )
{
		cout << "  set  " << setName << " not referenced" << endl;
		cout << "return  ============================ IfIns::removeObjectFromSet ======" << endl << endl;
}
//debug end
		return 0;
	}
	
	removeObjectFromFrozenObjectSets(objName,setName);
	
	if ( isObjectDirectlyReferenced(objName) )
	{
//debug beg
if ( dbg > 5 )
{
	cout << " object " << objName << " is directly referenced" << endl;
	cout << "return  ============================ IfIns::removeObjectFromSet ======" << endl << endl;
}
//debug end
		return 1;
	}
	
	// the object 'objName' is either locked or waiting for it
	
	SMIObject* pSMIobj = allSMIObjects.gimePointer(objName);
	
	if ( !_tobeLocked.isPresent(objName) )
	{

//debug beg
if ( dbg > 5 )
{
	cout << " object " << objName << " is locked and will be unlocked" << endl;
	cout << "return  ============================ IfIns::removeObjectFromSet ======" << endl << endl;
}
//debug end
		pSMIobj->unlock();
		return 1;
	}
	
//debug beg
if ( dbg > 5 )
{
	cout << " object " << objName 
	<< " is waiting to be locked. Lock action will be removed" << endl;
}
//debug end
	
	// the object is waiting (has lock action queued)
	pSMIobj->removeQueuedLock(_objName);
	
	// now remove it from the list of waiting objects

//debug beg
if ( dbg > 5 )
{
	cout << endl << " object will be also removed from the list of objects to-be-locked."
	<< endl << " list of objects to-be-locked before : " << endl; _tobeLocked.out(); cout << endl << endl;
}
//debug end

	_tobeLocked.remove(objName);

//debug beg
if ( dbg > 5 )
{
	cout << " list of objects to-be-locked after : " << endl; _tobeLocked.out(); cout << endl;
	cout << "return  ============================ IfIns::removeObjectFromSet ======" << endl << endl;
}
//debug end
	
	return 1;
}
//------------------------------------------  BF  Oct 2008  ----------------
bool IfIns::isObjectDirectlyReferenced( const Name& objName)
{
	Condition* pCondition;
	int iicond;
	
	for ( iicond = 0; iicond < _numCond; iicond++ )
	{
		pCondition = static_cast<Condition*>(_conditions[iicond]);
		if ( pCondition->isObjectDirectlyReferenced(objName) )
		{
			return true;
		}
	}
	return false; 
}
//------------------------------------------- BF  Oct 2008  -----------------
bool IfIns::isObjectSetReferenced( const Name& objSetName)
{
	Condition* pCondition;
	int iicond;
	
	for ( iicond = 0; iicond < _numCond; iicond++ )
	{
		pCondition = static_cast<Condition*>(_conditions[iicond]);
		if ( pCondition->isObjectSetReferenced(objSetName) )
		{
			return true;
		}
	}
	return false; 
}
//-------------------------------------------  BF  Oct 2008  ------------------
void IfIns::removeObjectFromFrozenObjectSets(const Name& objName, const Name& setName )
{
	Condition* pCondition;
	int iicond;
	
	for ( iicond = 0; iicond < _numCond; iicond++ )
	{
		pCondition = static_cast<Condition*>(_conditions[iicond]);
		pCondition->removeObjectFromFrozenObjectSets( objName, setName );
	}
	return; 
}
//-------------------------------------------  BF  Oct 2011 ---------------
void IfIns::getCurrentRefObjects()
{
	Condition* pCondition; NameList condCurrRefObjects;
	int iicond;

	_refObjectsAtStartExecution.removeAll();
	
	// now loop over IF's conditions	
	for ( iicond = 0; iicond < _numCond; iicond++ )
	{
		// get the current ref. objects by the condition
		pCondition = static_cast<Condition*>(_conditions[iicond]);
		pCondition->getCurrentRefObjects(condCurrRefObjects);
		// add them to the IF's list
		_refObjectsAtStartExecution.add(condCurrRefObjects);
	}

	return;
}
//=========================================================================
int IfIns::reportingLockedObject( Name& lockedObjName)
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
//============================================================================
int IfIns::reportingObjectRemovedFromSet
	               ( const Name& remObjName, const Name& setName)
{
//debug beg
if ( dbg > 5 )
{
cout << endl << "start  ============================ IfIns::reportingObjectRemovedFromSet ======" << endl;
cout << "  object " << remObjName << "  Set " << setName << endl;
}
//debug end

	if ( _suspend_flag != suspendedDueToCondition )
	{
		cout << "  *** Internal error : IF is not TypeI suspended" <<
		endl;
		abort();
	}
	if ( !isObjectSetReferenced(setName) )
	{
//debug beg
if ( dbg > 5 )
{
cout << "  set  " << setName << " not referenced" << endl;
cout << "return  ============================ IfIns::reportingObjectRemovedFromSet ======" << endl << endl;
}
//debug end
		return 0;
	}
	
	removeObjectFromFrozenObjectSets(remObjName,setName);
	
	if ( isObjectDirectlyReferenced(remObjName) )
	{
//debug beg
if ( dbg > 5 )
{
cout << " object " << remObjName << " is directly referenced" << endl;
cout << "return  ============================ IfIns::reportingObjectRemovedFromSet ======" << endl << endl;
}
//debug end
		return 0;
	}
	
	// The object 'remObjName' is referenced only through set 'setName'
	// it is either locked, or has the lock queued. The lock is being
	// imposed on it only because it was a member of the set. It can
	// now be lifted.
	
	int flg = _lockedObjects.remove(remObjName);
	
	SMIObject* pSMIObj = allSMIObjects.gimePointer(remObjName);
	
	if ( flg == 1) 
	{ // object was found in the locked objects list and removed,
	  // now unlock it
		pSMIObj->unlock();
//debug beg
if ( dbg > 5 )
{
cout << " object " << remObjName << " unlocked" << endl;
cout << "return  ============================ IfIns::reportingObjectRemovedFromSet ======" << endl << endl;
}
//debug end
		return 0;
	}
	
	// the object must be waiting for the lock
	
	flg = _objectsWaitingForLock.remove(remObjName);
	
	if ( flg == 0 )
	{ // object not found in the list
		cout << endl
		<< " !WARNING! executing IfIns::reportingObjectRemovedFromSet(...) " << endl
		<< "           OBJECT " << remObjName << " was inserted into and now removed from" << endl
		<< "           SET " << setName << " during the execution of the relevant IF " << endl << endl;
//debug beg
if ( dbg > 5 )
{
cout << "return  ============================ IfIns::reportingObjectRemovedFromSet ======" << endl << endl;
}
//debug end
		return 0;

	}
	
	// the object has lock action waiting...have to remove it
	pSMIObj->removeQueuedLock(_objName);

//debug beg
if ( dbg > 5 )
{
cout << " object " << remObjName << " waiting to be locked, lock action removed" << endl;
cout << "return  ============================ IfIns::reportingObjectRemovedFromSet ======" << endl << endl;
}
//debug end	
	
	if ( _objectsWaitingForLock.numOfEntries() == 0 )
	{ // all objects are locked
		return 1;
	}
	else
	{
		return 0;
	}
}	
	
