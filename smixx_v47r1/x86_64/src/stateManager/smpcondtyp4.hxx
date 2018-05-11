//-------------------------  SmpCondTyp4  Class  ---------------------------------
#ifndef SmpCondTyp4_HH
#define SmpCondTyp4_HH
#include "parameters.hxx"
#include "name.hxx"
#include "smpcond.hxx"
#include "paroperand.hxx"
class SMIObject;
class State;
class Action;
//--------------------------------------------------------------------------
//                                                         B. Franek
//                                                         June-2012
// Copyright Information:
//      Copyright (C) 1996-2012 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
class SmpCondTyp4 : public SmpCond {
public :
	SmpCondTyp4( char lines[][MAXRECL], int& no_lines,
	           SMIObject* , State*, Action* );
  
	int evaluate();     /* 0-false;  1-true;  -1 - the object in transition;
                               -2 empty object set */
	
	void objectsToLock(NameList&) const;

	void freeze();

	void unfreeze();

	Name whatAreYou() const;

private :
	
	bool compare_str( Name& v1, Name& v2 , int& er);
	bool compare_int( Name& v1, Name& v2 , int& er);
	bool compare_float( Name& v1, Name& v2 , int& er);
	
	ParOperand _operand1;
	Name _operator;  // ==   <>   <   >   >=   <=
	ParOperand _operand2;

	SMIObject* _pParentObject;
	State*     _pParentState;
	Action*    _pParentAction;		
};

#endif
