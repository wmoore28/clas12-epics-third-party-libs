//-------------------------  SmpCondTyp1  Class  ---------------------------------
#ifndef SmpCondTyp1_HH
#define SmpCondTyp1_HH
#include "parameters.hxx"
#include "name.hxx"
#include "smpcond.hxx"
//--------------------------------------------------------------------------
//                                                         B. Franek
//                                                         01-Aug-1996
//                                                 rewritten 13-Nov-2003
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
class SmpCondTyp1 : public SmpCond {
public :
	SmpCondTyp1( char lines[][MAXRECL], int& no_lines);
  
	int evaluate();     /* 0-false;  1-true;  -1 - the object in transition;
                               -2 empty object set */
	
	void objectsToLock(NameList&) const;

	void freeze();

	void unfreeze();

	Name whatAreYou() const;

private :
	
        Name _objectName;   // The name of the smi_object
	
	int _all_in;      // when 1 then all_in type, 0 any_in type

        int _inState;                 // 0 ... not_in_state;  1 ... in_state;
  
        NameVector _states;   
};

#endif
