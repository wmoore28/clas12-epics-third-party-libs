//------------------------  Class   S e t I n s  -----------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "utilities.hxx"
//-------------------- Externals -----------------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern  Name smiDomain;
   extern int dbg;
//-------------------------------------------------
#include "setins.hxx"
#include "smiobject.hxx"
#include "parms.hxx"
#include "ut_sm.hxx"
//-------------------------------------------------------------------------------------
//                                                      Date : 14-Jun-1998
//                                                     Author: Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------- SetIns -------------------------------------
//

SetIns::SetIns
       ( char lines[][MAXRECL], const int lev, int& no_lines,
         SMIObject *pobj, State *pstat, Action* pact){

	if ( pstat == NULL ) {}  // will remove compiler warning

   _level = lev;

   _pParentObject = pobj;
   
   _pParentState = pstat;

   _pParentAction = pact;

   sscanf(lines[1],"%d",&_insType);


	if ( _insType == 10 )  // at the moment, there is no other type
	{
//  Set 'left operand' = 'operand1' 'operator' 'operand2'
		int il = 1;
		
		il++;
		if ( strcmp(lines[il],"LOP") )
		{
			cout << endl;
			cout << " Error initialisin SET instruction" << endl;
			cout << " expected LOP and found |" << lines[il] << "|" << endl;
			exit(1);
		}
		
		il++;
		_lOperand.initFromSobj_firstLine(lines[il]);
		il++;
		while ( _lOperand.initFromSobj_nextLine(lines[il]) ) { il++; }
//debug beg
if (dbg>5)
{
cout << endl << " Left Operand: " << endl;
 _lOperand.out(" ");
}
//debug end
		il++;  // now read the num of operands and operations
		sscanf(lines[il],"%d  %d",&_numOperands,&_numOperations);
		
		il++;
		if ( strcmp(lines[il],"OP1") )
		{
			cout << endl;
			cout << " Error initialisin SET instruction" << endl;
			cout << " expected OP1 and found |" << lines[il] << "|" << endl;
			exit(1);
		}
		
		il++;
		_operand1.initFromSobj_firstLine(lines[il]);
		il++;
		while ( _operand1.initFromSobj_nextLine(lines[il]) ) { il++; }
//debug beg
if (dbg>5)
{
cout << endl << " First Operand: " << endl;
_operand1.out(" ");
}
//debug end
		if (_numOperands > 1)
		{
		il++;
		if ( strcmp(lines[il],"OP2") )
		{
			cout << endl;
			cout << " Error initialisin SET instruction" << endl;
			cout << " expected OP2 and found |" << lines[il] << "|" << endl;
			exit(1);
		}
		
		il++;
		_operand2.initFromSobj_firstLine(lines[il]);
		il++;
		while ( _operand2.initFromSobj_nextLine(lines[il]) ) { il++; }
//debug beg
if (dbg>5)
{
cout << endl << " Second Operand: " <<  endl;
_operand2.out(" ");
}
//debug end

		il++;
		char tempstr[2];
		tempstr[0] = lines[il][0];
		tempstr[1] = '\0';
		_operator = tempstr;
		if ( _operator == "+" || _operator == "-" ||
		     _operator == "*" || _operator == "/" ||
		     _operator == "%" )
		{}
		else
		{
			cout << endl;
			cout << "Error initialising SET instruction" << endl;
			cout << " unknown operator " << _operator << endl;
			exit(1);
		}
//debug beg
if (dbg>5)
{
cout << endl << " Operator " << _operator << endl;
}
//debug end
		}
		no_lines = il + 1;
		
	}
	
    else { 
       cout << " Set instruction type " << _insType << 
               " not yet implemented" << endl;
       cout.flush();
       abort();
     }
}






//---------------------------- whatAreYou ---------------------------------

void SetIns::whatAreYou(){
}


//----------------------------  execute  ---------------------------------------
//
//
int SetIns::execute( Name& endState ){

	endState = "not changed";

//
//
   Parms *pObjParms = _pParentObject->pObjectParameters();

   
   if ( dbg > 4 ) {
       cout << "  " << endl;
       cout << " --------------------------------------------------" << endl;
       cout << " Executing Set instruction Type: " << _insType << endl;
       cout << " Object parameters before : " << endl;
       pObjParms->out();
   }

   
	if (_insType == 10 )
	{
		int error;
		int flg;
		Name value1, value2, type1, type2;
		Name commonType;
	
		error = 0;
	
		flg = getOperandValue(_operand1, _pParentObject, _pParentAction,
		                     value1, type1);
		if (flg )
		{
//debug beg
if (dbg>5)
{
	cout << endl << "-----------------------------------------";
	cout << endl << "  operand1: " << _operand1.printingName()
	 << " " << value1 << " " << type1; cout << endl;
}
//debug end
		}
		else
		{
			cout << " retrieval of operand 1 value failed" << endl;
			cout << endl;
			cout << "**** Set instruction is not executed" << endl;
			return 0;
		}
		
		if (_numOperands == 1)
		{
			flg = setLeftOperand( value1, type1);
			if (!flg)
			{
				cout << "**** Set instruction is not executed" << endl;
			}		
//debug beg	
if ( dbg > 4 )
{
       cout << endl << "end ====================== SetIns::execute() ===========" << endl;
}
//debug end
			return 0;
		}
		
		
		flg = getOperandValue(_operand2, _pParentObject, _pParentAction,
		                      value2, type2);
		if (flg )
		{
//debug beg
if (dbg>5)
{
	cout << endl << "  operand2: " << _operand2.printingName()
	 << " " << value2 << " " << type2; cout << endl;
}
//debug end
		}
		else
		{
			cout << " retrieval of operand 2 value failed" << endl;
			cout << endl;
			cout << "**** Set instruction is not executed" << endl;
			return 0;

		}

		
		if ( type1 == type2 )
		{
			commonType = type1;
		}
		else
		{
				error = 1;
				cout << " ** Set instruction with incompatible types" << endl;
				cout << " This should not have happened ... call expert" << endl;
				cout << "**** Set instruction is not executed" << endl;
//debug beg	
if ( dbg > 4 )
{
       cout << endl << "end ====================== SetIns::execute() ===========" << endl;
}
//debug end
				return 0;
		}

		Name resValue;
		int flg1;
	
		if ( commonType == "STRING" ) 
		{ flg1 = evalExpr_str(value1,value2,resValue); }
		else if ( commonType == "INT" ) 
		{ flg1 = evalExpr_int(value1,value2,resValue); }
		else if ( commonType == "FLOAT" ) 
		{ flg1 = evalExpr_float(value1,value2,resValue); }
		else
		{
		cout << " Unknown type " << commonType << endl;
		flg1 = 0;
		}
		
		if (!flg1)
		{
			cout << "**** Set instruction is not executed" << endl;
//debug beg	
if ( dbg > 4 )
{
       cout << endl << "end ====================== SetIns::execute() ===========" << endl;
}
//debug end
			return 0;
		}
		
//debug beg	
if ( dbg > 4 )
{
       cout << endl << "  resulting value:  " << resValue << endl;
}
//debug end			
		flg1 = setLeftOperand( resValue, commonType);
		
		if (!flg1)
		{
			cout << "**** Set instruction is not executed" << endl;
//debug beg	
if ( dbg > 4 )
{
       cout << endl << "end ====================== SetIns::execute() ===========" << endl;
}
//debug end
			return 0;
		}		

	}

   else {
        cout << " Internal error, unknown type " << _insType << endl;
        cout.flush();
        abort();
   }
   	
	
   if ( dbg > 4 ) {
       cout << " Object parameters after : " << endl;
       pObjParms->out();
       cout << " --------------------------------------------------" << endl;
   }

   return 0;         // set instruction allways finishes
}

//-------------------------------------------------------------------------
/*int SetIns::getOperandValue( ParOperand& operand, Name& value, Name& type)
{
	int flg;
	Name parNm, objNm;
	SMIObject* pObj;
	
	if ( operand.operandType() == "VALUE" )
	{
		value = operand.value();
		type = operand.valueType();
		return 1;
	}
	if ( operand.operandType() == "NAME" )
	{
		parNm = operand.parName();
		flg = _pParentAction->getActionParameter(parNm,value,type);
		if ( flg == 1) { return 2; }
		flg =_pParentObject->getObjectParameter(parNm,value,type);
		if ( flg == 1 )
		{
			operand.thisObjPar();
			return 3;
		}
		return 0;
	}
	if ( operand.operandType() == "COMPNAME" )
	{
		parNm = operand.parName();
		objNm = operand.ownerObjName();
		if ( objNm == "THIS" )
		{
			pObj = _pParentObject;
		}
		else
		{
			pObj = allSMIObjects.gimePointer(objNm);
		}
		flg = pObj->getObjectParameter(parNm,value,type);
		if ( flg == 1 ) { return 3; }
	}


	return 0;
}*/
//---------------------------------------------------------------------------
int SetIns::evalExpr_str(Name& value1, Name& value2, Name& res)
{
	Name temp;
	
	if ( _operator == "+" )
	{
		res = value1;
		res.removeLastChar();   // remove the end "
		temp = (char*) (value2.getString() + 1); // skip the first " in value2
		res+= temp;
		return 1;
	}
	
	cout << endl;
	cout << " *** error executing SET instruction :" << endl
	<< " for strings, only + operator is allowed " << endl;
	return 0;
}
//---------------------------------------------------------------------------
int SetIns::evalExpr_int(Name& value1, Name& value2, Name& res)
{
	int ivalue1, ivalue2, iresult, flg;
	
	flg = sscanf(value1.getString(),"%d",&ivalue1);
	if (flg != 1)
	{
		cout << endl;
		cout << " *** error executing SET instruction :" << endl
		<<  value1 << " is not an integer " << endl;
		return 0;
	}
		
	flg = sscanf(value2.getString(),"%d",&ivalue2);
	if (flg != 1)
	{
		cout << endl;
		cout << " *** error executing SET instruction :" << endl
		<<  value2 << " is not an integer " << endl;
		return 0;
	}
	
	if ( _operator == "+" )
	{
		iresult = ivalue1 + ivalue2;
		
	}
	else if ( _operator == "-" )
	{
		iresult = ivalue1 - ivalue2;
		
	}
	else if ( _operator == "*" )
	{
		iresult = ivalue1 * ivalue2;
		
	}
	else if ( _operator == "%" )
	{
		iresult = ivalue1 % ivalue2;
		
	}

	else if ( _operator == "/" )
	{
		if ( ivalue2 == 0 )
		{
			cout << endl;
			cout << " *** error executing SET instruction:" << endl
			<< " division by 0 " << endl;
			return 0;
		}
		iresult = ivalue1 / ivalue2;
	}
	else
	{
		cout << endl;
		cout << " *** error executing SET instruction : "<< endl
		<< " operator " << _operator << " is unknown " << endl;
		return 0;
	}
	char temp[50];
	flg = sprintf(temp,"%d",iresult);
	if ( flg < 0 || flg >= 50 )
	{
		cout << endl;
		cout << " error executing SET instruction " << endl;
		return 0;
	}
	
	res = temp;
//	cout << res << endl;
	return 1;
}
//---------------------------------------------------------------------------
int SetIns::evalExpr_float(Name& value1, Name& value2, Name& res)
{
	double fvalue1, fvalue2, fresult;
	int flg;
	
	flg = sscanf(value1.getString(),"%lf",&fvalue1);
	if (flg != 1)
	{
		cout << endl;
		cout << " *** error executing SET instruction :" << endl
		<<  value1 << " is not a floating point number " << endl;
		return 0;
	}
		
	flg = sscanf(value2.getString(),"%lf",&fvalue2);
	if (flg != 1)
	{
		cout << endl;
		cout << " *** error executing SET instruction :" << endl
		<<  value2 << " is not a floating point number " << endl;
		return 0;
	}
	
	if ( _operator == "%" )
	{
		cout << endl;
		cout << " *** error executing SET instruction:" << endl
		<< " operator % is admissable only when both operands are integer " << endl;
		return 0;
	}
	
	if ( _operator == "+" )
	{
		fresult = fvalue1 + fvalue2;
		
	}
	else if ( _operator == "-" )
	{
		fresult = fvalue1 - fvalue2;
		
	}
	else if ( _operator == "*" )
	{
		fresult = fvalue1 * fvalue2;
		
	}
	else if ( _operator == "/" )
	{
		if ( fvalue2 == 0 )
		{
			cout << endl;
			cout << " *** error executing SET instruction:" << endl
			<< " division by 0 " << endl;
			return 0;
		}
		fresult = fvalue1 / fvalue2;
	}
	else
	{
		cout << endl;
		cout << " *** error executing SET instruction : "<< endl
		<< " operator " << _operator << " is unknown " << endl;
		return 0;
	}
	char temp[50];
	flg = sprintf(temp,"%lf",fresult);
	if ( flg < 0 || flg >= 50 )
	{
		cout << endl;
		cout << " error executing SET instruction " << endl;
		return 0;
	}
	
	res = temp;
//	cout << res << endl;
	return 1;
}
//---------------------------------------------------------------------------
int SetIns::setLeftOperand( Name& value, Name& valuetype)
{
//cout << " Entered setLeftOperand" << endl;


	Name lOpVal, lOpValType;
	
	int flg = getOperandValue(_lOperand, _pParentObject, _pParentAction,
	                     lOpVal, lOpValType);
	if (flg )
	{
	}
	else
	{    cout << " retrieval of left operand type failed" << endl;
		return 0;
	}
	 
	if ( _lOperand.operandType() == "VALUE" )
	{
		cout << endl;
		cout << " *** error executing SET instruction : " << endl
		<< " left Operand can not be a value" << endl;
		return 0;
	}
	Name parNm = _lOperand.parName();
	
	Name convValue; 

	flg = 0;
	if ( lOpValType == valuetype )
	{
		flg = 1;
		convValue = value;
	}
	
	if ( valuetype == "FLOAT" && lOpValType == "STRING" )
	{
		flg = floatParToStringPar(value,convValue);
	}
	
	if ( valuetype == "INT" && lOpValType == "STRING" )
	{
		flg = intParToStringPar(value,convValue);
	}
	
	if ( valuetype == "STRING" && lOpValType == "FLOAT" )
	{
		cout << endl;
		cout << " *** error executing SET instruction : " << endl;
		cout << " conversion of String to Float ... call expert"
		<< endl;
		return 0;
	}
	
	if ( valuetype == "INT" && lOpValType == "FLOAT" )
	{
		flg = intParToFloatPar(value,convValue);
	}
	
	if ( valuetype == "STRING" && lOpValType == "INT" )
	{
		flg = stringParToIntPar(value,convValue);
	}
	
	if ( valuetype == "FLOAT" && lOpValType == "INT" )
	{
		flg = floatParToIntPar(value,convValue);
	}
	
	if ( !flg )
	{
		cout << endl;
		cout << " *** error executing SET instruction : " << endl
		<< value << " can not be converted to left " << lOpValType << endl;
		return 0;	
	}

	if (_pParentObject->setObjectParameter(parNm,convValue)) {
//debug beg
if (dbg>5)
{
cout << endl << "  left operand " << parNm << " set to "
       << lOpValType << convValue << endl;
}
//debug end	
		return 1;
	}
	cout << "  *** setting the value of the left operand failed " << endl;
	return 0;
}
