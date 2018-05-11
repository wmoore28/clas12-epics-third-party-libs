//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  August 2014
// Copyright Information:
//      Copyright (C) 1996-2014 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------

#include "smiobject.hxx"
#include "action.hxx"
#include "smlunit.hxx"
#include "paroperand.hxx"
#include "opermanager.hxx"
#include "ut_tr.hxx"
#include <stdlib.h>
#include "smlline.hxx"
#include "errorwarning.hxx"

//------------------------------------------------------------------
int OperManager::getParValueType(  SMLUnit* pParentUnit, ParOperand& operand)
{
	SMIObject* parentObject;
	SMIObject* pOwnerObject;
	Action* parentAction;
	
	Name operandType, valueType, value, parName, ownerObjName;
	int flg;
	
	parentObject = (SMIObject*) pParentUnit->parentPointer("Object");  
	parentAction = (Action*) pParentUnit->parentPointer("Action");      

	
	operandType = operand.operandType();

//cout << " Entered getParValueType     operandType  " << operandType << endl;
	
	if ( operandType == "VALUE" )
	{  	
		return 1;
	}
		
	else if ( operandType == "NAME" )
	{
	// check first if action parameter of that name exists
		parName = operand.parName();
		if ( parentAction )
		{
			flg = parentAction->getActionParameter(parName,value,valueType);
			if ( flg ) 
			{   // set the type   new method
				operand.setValueType(valueType);
				return 1;
			}
		}

		flg = parentObject->getObjectParameter(parName,value,valueType);
		if ( flg ) 
		{   // set the type
			operand.setValueType(valueType);
			return 1;
		}
		return 0;  //found neither among action, nor object parameters		
		
	}
		
	else if ( operandType == "COMPNAME" )
	{
		parName = operand.parName();
		ownerObjName = operand.ownerObjName();	
		
		if ( ownerObjName == "THIS" )
		{
			pOwnerObject = parentObject;
		}
		else
		{
			pOwnerObject = gimeObjectPointer(ownerObjName);
			if ( pOwnerObject == NULL ) { return -1; } // object or its class not found
		}
			
		flg = pOwnerObject->getObjectParameter(parName,value,valueType);
		if ( flg ) 
		{   // set the type
			operand.setValueType(valueType);
			return 1;
		}
		return -2;  // did not find it among object parameters		
	}
	
	else
	{
		cout << " !!!! internal error."
		<< " OperManager::getParValueType"
                 << endl << " Unknown operand type " << operandType << endl
                 << " Operand: " << endl; operand.out(" "); cout << endl;
		abort();
	}
	
}
//----------------------------------------------------------------------------
int OperManager::getParValueAndOperValueTypes( SMLline& firstLine, SMLUnit* pParentUnit, ParOperand& operand)
{
	int retcode = 0;

		int flg = OperManager::getParValueType(pParentUnit, operand);
		
		if ( flg != 1 ) 
		{
			retcode = 1;
			ErrorWarning::printHead("ERROR",firstLine);
			
			if ( flg == 0 ) 
			{
				cout << " Parameter " << operand.printingName() << " is not declared" << endl;
			}
			if ( flg == -1 ) 
			{
				cout<< " Parameter " << operand.printingName() 
				<< " the owner object or its class does not exist" << endl;
			}		
			if ( flg == -2 ) 
			{
				cout << " Parameter : " << operand.printingName() 
				<< " is not declared in owner object" << endl;
			}
			cout << endl; pParentUnit->printParents(); cout << endl;				
		}
		
		flg = operand.calcOperValueType();
		if ( flg != 1)
		{
			if ( flg == 2 )
			{
				ErrorWarning::printHead("WARNING",firstLine);
				cout << " Parameter " << operand.printingName() << " is a constant"
				<< " ... cast ignored" << endl;
			}
			if ( flg == -1 )
			{
				retcode = 1;
				ErrorWarning::printHead("ERROR",firstLine);
				cout << " Parameter " << operand.printingName() << " illegal cast STRING to FLOAT"
				<< endl;			
			}
		}

	return retcode;

}
//==========================================================================
int OperManager::makeOperandsSameType
	           ( SMLline& firstLine, SMLUnit* pParentUnit,
		     ParOperand& operand1, ParOperand& operand2)
{ 
//  string float    .... not allowed
//  string int   ...  string -> int
//  int float ... int -> float

	Name operValTyp1 = operand1.operValueType();
	Name operValTyp2 = operand2.operValueType();
	
	if ( operValTyp1 == operValTyp2 )
	{ return 0; }
	
	if ( operand1.castingRequested() == 1 ||
	     operand2.castingRequested() == 1 )
	{
		ErrorWarning::printHead("ERROR",firstLine);
		cout << "  Sort out your casting first " << endl;
		return 1;
	
	}

// After the above statement we know, that the operan value type  is the same as ref par type
	
	if (    (operValTyp1 == "STRING" || operValTyp2 == "STRING")
	     && (operValTyp1 == "FLOAT" || operValTyp2 == "FLOAT") )
	{
		ErrorWarning::printHead("ERROR",firstLine);
		cout << " illegal type mixing STRING with FLOAT"
		<< endl;			
		return 1;
	
	}
	
	
	if ( operValTyp1 == "STRING" )
	{
		if ( operValTyp2 == "INT" )
		{
			return attemptCasting(firstLine,pParentUnit,operand1,"INT");
		}
		else
		{
			ErrorWarning::printHead("ERROR",firstLine);
			cout << " Parameter " << operand2.printingName() << " illegal operand type"
			<< endl << " internal error ...  aborting" ;
			abort();			
		}
	}
	else if ( operValTyp1 == "FLOAT" )
	{
		if ( operValTyp2 == "INT" )
		{
			return attemptCasting(firstLine,pParentUnit,operand2,"FLOAT");
		}			
		else
		{
			ErrorWarning::printHead("ERROR",firstLine);
			cout << " Parameter " << operand2.printingName() << " illegal operand type"
			<< endl << " internal error ...  aborting" ;
			abort();			
		}	
	
	}
	else if ( operValTyp1 == "INT" )
	{
		if ( operValTyp2 == "STRING" )
		{
			return attemptCasting(firstLine,pParentUnit,operand2,"INT");
		}
		else if ( operValTyp2 == "FLOAT" )
		{
			return attemptCasting(firstLine,pParentUnit,operand1,"FLOAT");
		}			
		else
		{
			ErrorWarning::printHead("ERROR",firstLine);
			cout << " Parameter " << operand2.printingName() << " illegal operand type"
			<< endl << " internal error ...  aborting" ;
			abort();			
		}	
	
	
	}
	else
	{
		ErrorWarning::printHead("ERROR",firstLine);
		cout << " Parameter " << operand1.printingName() << " illegal operand type"
		<< endl << " internal error ...  aborting" ;
		abort();			
	}
}
//==========================================================================
int OperManager::attemptCasting
	           ( SMLline& firstLine, SMLUnit* pParentUnit,
		     ParOperand& operand, const char castTo[])
{ 
	Name temp = castTo;
	
	operand.setCast(temp);
	if ( operand.calcOperValueType() == 1 )
	{
		ErrorWarning::printHead("WARNING",firstLine);
		cout << " Parameter " << operand.printingName() << " casted to "
		<< castTo << endl;
		return 0;
	}
	ErrorWarning::printHead("ERROR",firstLine);
	cout << " Parameter " << operand.printingName() << " can not be casted to "
	<< castTo << endl;
	cout << endl; pParentUnit->printParents(); cout << endl;
	return 1;			

}
