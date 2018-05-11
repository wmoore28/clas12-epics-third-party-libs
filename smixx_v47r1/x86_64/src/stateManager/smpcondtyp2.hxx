//-------------------------  SmpCondTyp2  Class  ---------------------------------
#ifndef SmpCondTyp2_HH
#define SmpCondTyp2_HH
#include "parameters.hxx"
#include "smpcond.hxx"
#include "name.hxx"
#include "namevector.hxx"
#include "smifrozenset.hxx"
//--------------------------------------------------------------------------
//                                                         B. Franek
//                                                         01-Aug-1996
//                                                rewritten  13-Nov-2003
// Copyright Information:
//      Copyright (C) 1996-2003 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
class SmpCondTyp2 : public SmpCond {
public :
	SmpCondTyp2( char lines[][MAXRECL], int& no_lines);

	int evaluate();     /* 0-false;  1-true;  -1 - the object in transition;
//                            -2 empty object set */
	
	void objectsToLock(NameList&) const;

	void freeze();

	void unfreeze();
	
	Name whatAreYou() const;
	
	void removeObjectFromFrozenObjectSet( const Name& objName, const Name& setName);

private :

	Name _objectSet;  //when \0 then no operation on object set
	
	int _all_in;      // when 1 then all_in type, 0 any_in type

	SMIFrozenSet _frozenSet;

        int _inState;                 // 0 ... not_in_state;  1 ... in_state;
  
        NameVector _states;   
};

#endif
