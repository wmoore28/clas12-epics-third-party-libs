//---------------------- SMIObject   Class ---------------------------
#ifndef SMIOBJECT_HH
#define SMIOBJECT_HH
#include <vector>
#include "parameters.hxx"
#include "objectregistrar.hxx"
#include "smiclass.hxx"
#include "registrar.hxx"

class State;
#include "state.hxx"

class WaitIns;
class SleepIns;

#include "commhandler.hxx"
#include "twonames.hxx"
#include "queue_twonames.hxx"

#include "queue_name.hxx"
#include "parms.hxx"
#include "namevector.hxx"
#include "clientwhens.hxx"
#include "smiobjectset.hxx"
//                                                          19-August-1996
//                                                            B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//--------------------------------------------------------------------------


class SMIObject {
public :
  SMIObject
  (char lines[][MAXRECL]);

  void whatAreYou();

  void startUp( Queue_TwoNames* pStateQ, CommHandler *ptnr);

  void setState( const Name& stateString);

  void execute();

  void resume();

/** will execute whens from the flag-list
     @param list ...list of flags ( 1/when) indicating whether particular
                    when should be executed. The list ends with the last flag
		    of value 1. 
*/
	void executeWhensFromFlagList
	( const std::vector<int>& list ) ;

/** will execute all whens in the current state
*/
	void executeAllWhens() ;
	
  void queueAction(Name& actionstr);

  void queueAction(char* actionstr);

  void queueHpAction(Name& action_nm);

  void queueHpAction(char* action_nm);

  int lock(char* objname); // objname ... the name of the object requesting lock   
//              ret 1- locked; 0- not locked but lock put on the action queue

  void unlock();

  Name& name() ;

  int numSubobj() const;

  bool associated() const;
  
  bool hasAssociatedAttribute() const; //this should obviously give the same
                                       // answer as associated() 

  Name externDomain() const;

  int name(char* obj_nm, int *numSubobj);	 //returns associated flag
                                                 //can probably be made obsolete
  Name subobjectName(const int index) const;

  Name currentState() const;

  State* pCurrentState() const ;

  int  currentState(Name& statenm); // returns -1 if object busy
//                                                    otherwise 1
    bool isBusy() const;
    
    bool hpActionsPending() ;
    
  Name parmString() const;

  void parmString(Name& str) const;

  void parmString(char* str, int mxsize) const;

  Parms* pObjectParameters();

/** gets the object parameter identified by 'name'. The function returns 0
  if not found
*/	
	int getObjectParameter(const Name& name, Name& value, Name& type) const;

/** sets the object parameter identified by 'name' to the value. The function returns 0
  if anything goes wrong
*/	
	int setObjectParameter(const Name& name, const Name& value) ;
	
  State* pTaggedState( const Name& tagname) const ;  
/**
  the function adds new client when to this object. The when is identified
  by its object name, its state name and its index within that state
*/
  int addClientWhen(const char* whenObjName, const char* whenStateName, int whenInx);

/**
    Every object or object set which is refered to by whens of this object is
    updated with when's addresses. This method simply loops over the existing
    states and for each of them calls method of the same name.
    It is called once just after the initialisation stage.
*/
	int updateReferencedObjectsandSets() const;
	
/**
    Will print all the client whens
*/
	int printClientWhens() const;

/** Will print the Object Sets to which it belongs
*/
	int printSetNamesBelongingTo();

/**  Builds the current client whens out of its own and all the object sets
     it belongs to
*/
	int gimeCurrentClientWhens( ClientWhens& clientWhens );

	Name& suspendedInsType();
	
/** Sets the _pointerToTypeISuspendedIF
*/
	void setPointerToTypeISuspendedIF(IfIns* pointer);
	
/** Returns the _pointerToTypeISuspendedIF
*/
	IfIns* gimePointerToTypeISuspendedIF() const;
/** removes queued 'lock action' requested by object 'requestorName'
*/
	int removeQueuedLock(const Name& requestorName) ;
	
	
/** Sets the _pointerToSuspendedWAIT
*/
	void setPointerToSuspendedWAIT(WaitIns* pointer);
	
/** Sets the _pointerToSuspendedSLEEP
*/
	void setPointerToSuspendedSLEEP(SleepIns* pointer);

/** This is called by an object that was just locked and whose client
    is the current object
*/
	void reportingLockedObject( Name& lockedObjName );
	
	int reportingObjectRemovedFromSet
	               ( const Name& remObjName, const Name& setName);
	
	void youJoinedSet( SMIObjectSet *pSet);
	
	void youLeftSet( SMIObjectSet *pSet); 
	
	bool memberOfSet(const Name& setNm);

		
private :
	friend class SMIObjectSet;
//--------------------------  private methods ----------------------------------
  void bodyLog( char lines[][MAXRECL]);

  void executeState();

  void split( const Name& statestr, Name& stateStr, Name& paramStr) const;

  int stateInx( const Name& stname) const ;  // returns -1 if not found

  int stateInx( const char stname[]) const ;  // returns -1 if not found

  int stateInxUndeclaredState() const;

  int stateInxDeadState() const;

  int stateInxDeadState(const int isub) const ;

  int stateInxInitialState(const int isub) const ;

  int stateInxTaggedState() const;

  void setStateAlloc( const Name& statestr);

  void setCurrStateInxes( const Name& stateNm ) ;
  
	void setStateDiagMessageNormal(const Name& inStateString) const;
	
	void setStateDiagMessageConverted(const Name& inStateString) const;
//----------------------  data  --------------
  Name _objectName;


  int _classFlag;                 // =1 indicates the object belongs to a class 
  Name _className;

  NameVector _attributes;


  Registrar _states;         // Pointers to states


  int _associated;            // =1 indicates that the object is associated

// When object is associated, it has at least 1 subobject (&dummy id 1)

  int _numOfSubobjects;

  Name _subobject[MAXSUBOBJECTS]; // List of subobjects
  int  _stateIndex[MAXSUBOBJECTS]; // list of indexes to _state array
  int  _numOfSubobjStates[MAXSUBOBJECTS];
//
// E.g. _state[_stateIndex[i]] is the 0th state of (i+1)st subobject

/**
  Keeps information about whens in the entire domain which reference this
  object. This is build after the initialisation stage
*/
	ClientWhens _clientWhens;
	
	
//------------------------ Dynamical part -------------------------------------

   Queue_TwoNames* _pStateQ;
   CommHandler * _pCommHandler;

   int _currStateInx;  // Index to the current state

   int _currStateInxes[MAXSUBOBJECTS]; // Same thing for subobjects

   char _internalState[32];

   Queue_Name _actionQ,_hpActionQ;        // Action & High priority action queue

   int _lock;

   Name _currentActionString;

   Parms _objectParameters;
   
/**
  Keeps information about Object Sets in the entire domain to which the current
  object belongs. This is build after the initialisation stage and then updated
  every time the object is removed/added to a Set.*/
	NmdPtnrList _setsIBelongTo;

	Name _suspendedInsType;  // either WAIT or IF
	
/** When action on the object is suspended, this keeps the pointer to the IF
    that is responsible
*/
	IfIns* _pointerToTypeISuspendedIF;
	
/** When action on the object is suspended, this keeps the pointer to the WAIT
    that is responsible
*/
	WaitIns* _pointerToSuspendedWAIT;
	
/** When action on the object is suspended, this keeps the pointer to the SLEEP
    that is responsible
*/
	SleepIns* _pointerToSuspendedSLEEP;

};
#endif
