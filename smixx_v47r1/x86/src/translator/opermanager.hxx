//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  August 2014
// Copyright Information:
//      Copyright (C) 1996-2014 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef OPERMANAGER_HH
#define OPERMANAGER_HH

#include "name.hxx"

class SMLline;
class SMLUnit;
class ParOperand;

class OperManager {
public:
/**
   If operand type is VALUE, then it simply returns
   In the two other cases it will get the value type of the referenced
   parameter
   returns  1  success
            0  parameter not found either among action parameters or object pars
	    -1 the other object or its class not declared
	    -2 the parameter not found among the other object parameters  
*/
	static int getParValueType( SMLUnit* pParentUnit, ParOperand& operand);

	static int  getParValueAndOperValueTypes
	           ( SMLline& firstLine, SMLUnit* pParentUnit, ParOperand& operand);

	static int makeOperandsSameType
	           ( SMLline& firstLine, SMLUnit* pParentUnit,
		     ParOperand& operand1, ParOperand& operand2);
		     	
	static Name& getMessage();
	
private:

	static int attemptCasting
	           ( SMLline& firstLine, SMLUnit* pParentUnit,
		     ParOperand& operand, const char castTo[]);
	static Name _message;


};

#endif
