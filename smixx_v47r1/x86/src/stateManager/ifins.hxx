//----------------------------- IfIns  Class ---------------------------------
#ifndef IFINS_HH
#define IFINS_HH

#include "parameters.hxx"
#include "instruction.hxx"

#include "inslist.hxx"
#include "condition.hxx"
#include "set_name.hxx"
#include "namelist.hxx"
class Condition;
class Action;
//
//                                                               15-April-1996
//                                                               B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------

class IfIns : public Instruction{
public :
  IfIns( char lines[][MAXRECL],int lev, PtrVector& allBlocks
       ,int& no_lines, SMIObject *pobj, State* pstat, Action*);
  void whatAreYou();
  int execute( Name& endState );   // 0-normal,  1-terminated action, 2-suspended
	
	int reportingLockedObject( Name& lockedObjName); // ret: 0-normal 1-all objects
						//	locked

/** For TypeI suspended IF instruction it will do all the necessary cleaning
  so that it appears like object 'objName' was not included in the set 'setName'
  at the time when the IF was initially executed
  */	
	int removeObjectFromSet( const Name& objName, const Name& setName);
	
/** For TypeIsuspended IF instruction it will do all the necessary cleaning
  so that it appears like object 'objName' was not included in the set 'setName'
  at the time when the IF was initially executed
  */	
	int reportingObjectRemovedFromSet
	               ( const Name& objName, const Name& setName);


private:
//-------------- functions --------------------------------------
/** finds all the objects referenced by the IF instruction. I.E.
    the directly referenced objects plus the objects contained in the
    referenced sets
*/
	void getCurrentRefObjects();

//
  void lockObjects();
  
  void unlockObjects();
  
/** will attempt to lock all the objects in
    the _refObjectsAtStartExecution list
*/
	void attemptToLockObjects();  
  
/** unlock all the objects in
    the _lockedObjects list
*/
	void unlockLockedObjects();
  
  int findTrueBlock();
	void freezeConditions();

	void unfreezeConditions();
	
/** will check weather object is directly referenced in the IF's conditions
*/
	bool isObjectDirectlyReferenced( const Name& objName);
	
/** will check weather object set is referenced in the IF's conditions
*/
	bool isObjectSetReferenced( const Name& objSetName);
/** will remove object 'objName' from all the frozen object sets
     of 'objSetName' in IF's conditions
*/
	void removeObjectFromFrozenObjectSets(const Name& objName, const Name& setName );
	
	void endOfIfExecutionDiagnostics();

//
  int _level;
  int _numCond;
  Action* _pParentAction;

    PtrVector _insLists;  // holds the pointers to the instructin Lists
    PtrVector _conditions; //holds pointers to the corresp. conditions
    
  int _isuspended;    // block at which it was suspended

	enum IfInstructionStatus_t 
	{fresh =0 ,
	 suspendedDueToCondition = 1,
	 suspendedDueToSuspendedBlock = 2 } _suspend_flag;
//  int _suspend_flag;  // non zero means that 'if' was suspended
//                      1 - suspended because the condition could not be eval
//                      2 - suspended because a block was suspended

  Set_Name _tobeLocked;


  Name _objName;    // Name of the parent object 
// 
/** Pointer to the parent object
*/
	SMIObject* _pParentObject; 
	State*     _pParentState;
	
	NameList _refObjectsAtStartExecution;
	
	NameList _lockedObjects;
	
	NameList _objectsWaitingForLock;              
                
};

#endif
