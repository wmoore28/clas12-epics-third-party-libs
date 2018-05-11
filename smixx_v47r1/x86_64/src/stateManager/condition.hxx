//-------------------------  Condition  Class  ---------------------------------
#ifndef CONDITION_HH
#define CONDITION_HH

#include "parameters.hxx"
#include "smpcond.hxx"
#include "set_name.hxx"
#include "namevector.hxx"
#include "ptrvector.hxx"
class  SmpCond;
class SMIObject;
class State;
class Action;
//                                                         B. Franek
//                                                         09-Aug-1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
class Condition {
public :
	friend class When;
	friend class State;
  Condition( char lines[][MAXRECL], int *no_lines, SMIObject* , State* , Action* );
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line of the condition
// Output :
// no_lines ....... number of lines in the condition
//----------------------------------------------------------------------------
  void whatAreYou(int maxStrLen, char* condString);
  int evaluate();     // 0-false;  1-true;  -1 - an object in transition;     
//
//
  void objectSet(Set_Name *pobjSet); // 
//
  void addto_objectSet(Set_Name *pobjSet); // 
//
	void freeze();

	void unfreeze();
	
/**  will get the current set of objects referenced by the condition,
     directly or indirectly through Object Sets.
*/
	void getCurrentRefObjects(NameList& currRefObjects) const;

/** will check wether an object is directly referenced in the condition
    this means that the object sets are ignored
*/
	bool isObjectDirectlyReferenced( const Name& objName) const;

	
/** will check wether an object set is referenced in the condition
*/
	bool isObjectSetReferenced( const Name& objSetName) const;
	
/**  will remove object 'objName' from all the frozen object sets of
     'setName' in all the relevant simple conditions
*/
	void removeObjectFromFrozenObjectSets( const Name& objName, const Name& setName);

private :
   void decode_instruction
     (char* instruction, 
      char* operation, char* type1, int *num1, char *type2, int *num2);

	int evalBool
	(const char operation[],
		 const int operand1, const int operand2) const;


   char _condition[MAXRECL];            // Truncated condition


   int _noOfSmpCond;    // Number of simple conditions
   int _noOfCondIns;    // Number of condition instructions

   PtrVector _smpConditions;  // Pointers to simple conditions

   enum { _maxCondIns = 100 };
   char _condIns[_maxCondIns+1][MAXRECL];  // Condition Instruction lines

//  per instruction:
   char _oper[_maxCondIns+1][4];   // 1-and   2-or  3-not

   char _type1[_maxCondIns+1];  // Type of the 1-st operand ( T or X )
   int  _num1[_maxCondIns+1];   // index of the first operand

   char _type2[_maxCondIns+1];  // Type of the 2-nd operand ( T or X )
   int  _num2[_maxCondIns+1];   // index of the second operand

/**
  list of objects that the condition references
*/
	NameVector _refObjects;
/**
  list of object sets that the condition references
*/
	NameVector _refObjectSets;

	SMIObject* _pParentObject;
	State*     _pParentState;
	Action*    _pParentAction;
};

#endif

