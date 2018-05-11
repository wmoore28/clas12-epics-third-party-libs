//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  September 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef WHEN_HH
#define WHEN_HH

#include "parameters.hxx"
class SMIObject;
class Condition;
#include "condition.hxx"
#include "doins.hxx"
//---------------------- W h e n   Class ---------------------------


class When {
public :
  When( char lines[][MAXRECL], int *wh_lines, SMIObject *pobj, State *pstat, int whenInx);
  
  void whatAreYou();
/** The when is executed, i.e. its' condition is evaluated and if true the correspondig
    action is pu on HP queue.
    No check is made wheather the object that reached new state is present in the condition.
    @return   1 means that associated condition was true and HP action was queued
              0 meand that either the associated condition was false or it could not be
	      executed because one of its objects was transiting.
*/
	int forcedExecute() const;
/**
    Every object or object set which is reffered to by this when condition is
    updated with this when address (its object, its state and its index)
*/
	int updateReferencedObjectsandSets() const;
/**
    returns true when it is 'stay_in_state' type when. otherwise returns false
*/
	bool specialWhen() const;
private :

  Condition *_pCondition;

  DoIns *_pDoIns;

  SMIObject *_pParentObject;
  State*     _pParentState;
  Name _parentObjectName;   //name of the parent object...it is frequently
  
  Name _parentStateName;  // name of the parent state
  
  int _whenInx;   // when position within the parent state (starting with zero)

	int _stay_in_state_flag;   // 1 for 'stay_in_state_when', otherwise 0
};
#endif
