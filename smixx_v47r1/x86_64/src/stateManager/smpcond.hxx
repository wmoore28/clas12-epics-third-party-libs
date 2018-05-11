//-------------------------  SmpCond  Class  ---------------------------------
#ifndef SmpCond_HH
#define SmpCond_HH
#include "parameters.hxx"

class Name;
class NameList;

#include "namevector.hxx"
//--------------------------------------------------------------------------
//                                                         B. Franek
//                                                         01-Aug-1996
//                                                    re-written 13-Nov-2003
// Copyright Information:
//      Copyright (C) 1996-2003 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
class SmpCond {
public :
	virtual ~SmpCond() { }
	
	int type() const;
	
	NameVector& refObjects();
	
	NameVector& refObjectSets();	
  
	virtual int evaluate() = 0;     // 0-false;  1-true;  -1 - the object in transition;
//                       -2 empty object set
	
	virtual void objectsToLock(NameList&) const = 0;

	virtual void freeze() = 0;

	virtual void unfreeze() = 0;	
	  
	virtual Name whatAreYou() const = 0;

protected :

	int _type;
	
	NameVector _refObjects;
	
	NameVector _refObjectSets;

	int _frozen;	//1-yes; 0-no;

};

#endif
