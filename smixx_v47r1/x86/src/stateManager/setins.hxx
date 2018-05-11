//-------------------------  Class   S E T I n s  -------------------------------
#ifndef SETINS_HH
#define SETINS_HH

#include "parameters.hxx"
#include "instruction.hxx"
#include "parms.hxx"
#include "paroperand.hxx"
class SMIObject;
class State;
class Action;
//                                                  Date :  14-Jun-1998
//                                                  Author : Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------


class SetIns : public Instruction{
public:
  SetIns
      ( char lines[][MAXRECL], const int lev, int& no_lines,
         SMIObject *pobj, State *pstat, Action *pact);
  void whatAreYou() ;
  int execute( Name& endState );   // 0-normal,  1-terminated action, 2-suspended
private:

// functions
//	int getOperandValue( ParOperand& operand, Name& value, Name& type);
	
	int evalExpr_str(Name& value1, Name& value2, Name& res); // ret=1 success
	int evalExpr_int(Name& value1, Name& value2, Name& res); // ret=1 success
	int evalExpr_float(Name& value1, Name& value2, Name& res); // ret=1 success
	
	int setLeftOperand( Name& value, Name& type);

// data
  int _level;

  int _insType;
//

	ParOperand _lOperand; //left side operand
	ParOperand _operand1;
	ParOperand _operand2;
	Name _operator;   // +   -   *   /

	int _numOperands;   // Number of right hand side operands (now 1 or 2)
	int _numOperations; // now 0 or 1
		
	SMIObject* _pParentObject;
	State*     _pParentState;
	Action*    _pParentAction;
};

#endif
