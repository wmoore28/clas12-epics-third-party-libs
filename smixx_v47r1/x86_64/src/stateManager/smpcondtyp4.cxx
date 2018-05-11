//------------------------  SmpCondTyp4  Class ------------------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "name.hxx"
#include "namelist.hxx"
#include "smpcondtyp4.hxx"
#include "smiobject.hxx"
#include "ut_sm.hxx"
//----------------- Externals --------------------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern  Name smiDomain;
#include "registrar.hxx"
   extern Registrar allSMIObjectSets;
   extern int dbg;
//------------------------------------------------
//                                                         B. Franek
//                                                         June 2012
// Copyright Information:
//      Copyright (C) 1996-2012 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------

//-------------------------------------------------------------------------
SmpCondTyp4::SmpCondTyp4( char lines[][MAXRECL], int& no_lines,
                     SMIObject* pobj, State* pstat, Action* pact) {
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line of the simple condition
// Output :
// no_lines ....... number of lines in the simple condition
//----------------------------------------------------------------------------
	_type = 4;
	_frozen = 0;
	
	_pParentObject = pobj;
	_pParentState  = pstat;
	_pParentAction = pact;

//cout << " Condition of typ 4 is not implemented yet" << endl;
//exit(1);
	int il;

	il = 0;
	
	il++;
	if ( strcmp(lines[il],"OP1") != 0 )
	{
		cout << " Error initialising Simple condition of Type 4" <<
		endl;
		cout << " Expecting OP1, found |" << lines[il] << endl;
		exit(1);
	}
	il++;
	_operand1.initFromSobj_firstLine(lines[il]);
	il++;
	while ( _operand1.initFromSobj_nextLine(lines[il]) )
	{
		il++;
	}


//debug beg
if ( dbg > 4 )
{	
	cout << endl << " ------------------------------" << endl;
	_operand1.out(" ");
}
//debug end
	
	il++;
	if ( strcmp(lines[il],"OP2") != 0 )
	{
		cout << " Error initialising Simple condition of Type 4" <<
		endl;
		cout << " Expecting OP2, found |" << lines[il] << endl;
		exit(1);
	}
	
	il++;
	_operand2.initFromSobj_firstLine(lines[il]);
	il++;
	while ( _operand2.initFromSobj_nextLine(lines[il]) )
	{
		il++;
	}


//debug beg
if ( dbg > 4 )
{	
	cout << endl;
	_operand2.out(" ");
}
//debug end	


	il++;
	_operator = lines[il];
	
//cout << " Operator " << _operator << endl;
	
	no_lines = il + 1;
	return ;

}
//--------------------------- whatAreYou --------------------------------------
Name SmpCondTyp4::whatAreYou() const {
// Not properly implemented yet
  Name tmpString;
  
      tmpString = "( ";
      tmpString += _operand1.printingName();
      tmpString += " "; tmpString += _operator; tmpString += " ";
      tmpString += _operand2.printingName();
      tmpString += " )";

      return tmpString;

}
//------------------------------  objectsToLock -------------------------------
void SmpCondTyp4::objectsToLock(NameList& list) const {
// simple condition Typ 4 does not lock any objects
        list.removeAll();

	return;
}
//------------------------------ freeze ------------------------------------
void SmpCondTyp4::freeze() {
	assert(_frozen==0);
	_frozen = 1; 

	return;
}
//------------------------------ unfreeze ---------------------------------
void SmpCondTyp4::unfreeze() {
	assert(_frozen==1);
	_frozen = 0;
	return;
}

//---------------------------- evaluate ----------------------------------
int SmpCondTyp4::evaluate()  {

//debug beg
if ( dbg > 5 )
{
cout << endl << "start =============  SmpCondTyp4::evaluate() =========== " << endl;
}
//debug end

	int error;
	int flg;
	Name value1(""), value2, type1, type2, commonType;
	
	error = 0;
	
	flg = getOperandValue( _operand1, _pParentObject, _pParentAction,
	                      value1, type1);
	if ( flg )
	{
//debug beg
if ( dbg > 5 )
{
		cout << endl; cout << " -------------------------------" << endl;
		cout << "operand 1" << " " 
		<< _operand1.printingName() << " " << value1 << " " << type1 << endl;
}
//debug end
	}
	else
	{
		cout << endl << whatAreYou() << endl;
		cout << " retrieval of operand 1  value failed" << endl;
		error = 1;
	}
	
	flg = getOperandValue( _operand2, _pParentObject, _pParentAction,
	                       value2, type2);
	if ( flg )
	{
//debug beg
if ( dbg > 5 )
{	
		cout << "operand 2" << " " 
		<< _operand2.printingName() << " " << value2 << " " << type2 << endl;	

}
//debug end
	}
	else
	{
		cout << endl << whatAreYou() << endl;
		cout << " retrieval of operand 2  value failed" << endl;
		error = 1;
	}

	if ( error != 0 )
	{
		cout << " Condition is evaluated as GHOST " << endl;
//debug beg
if ( dbg > 5 )
{	
cout << endl << "end =============  SmpCondTyp4::evaluate() =========== " << endl;
}
//debug end
		return -2;
	}
	
	if ( type1 == type2 ) {}
	else
	{
		cout << endl << whatAreYou() << endl;
		cout << " type 1   must be equal to type2    call expert " << endl;
		//debug beg
if ( dbg > 5 )
{	
cout << endl << "end =============  SmpCondTyp4::evaluate() =========== " << endl;
}
//debug end
		return -2;
	}
	
	commonType = type1;	
	
	bool res;
	
	if ( commonType == "STRING" )
	{
		res = compare_str(value1,value2,error);
	}
	
	else if ( commonType == "INT" )
	{
		res = compare_int(value1,value2,error);
	}
	
	else if ( commonType == "FLOAT" )
	{
		res = compare_float(value1,value2,error);
	}

	else
	{
		cout << endl << whatAreYou() << endl;
		cout << " Unknown type " << type1 << endl;
		cout << " Condition is evaluated as GHOST " << endl;
//debug beg
if ( dbg > 5 )
{	
cout << endl << "end =============  SmpCondTyp4::evaluate() =========== " << endl;
}
//debug end
		return -2;
	}
	
	if ( error != 0 )
	{
		cout << endl << whatAreYou() << endl;
		cout << " Condition is evaluated as GHOST " << endl;
//debug beg
if ( dbg > 5 )
{	
cout << endl << "end =============  SmpCondTyp4::evaluate() =========== " << endl;
}
//debug end
		return -2;
	}
	
//debug beg
if ( dbg > 5 )
{	
	cout << endl << whatAreYou() << endl;
	
	if (res) 
	{ 
		cout << " Condition is evaluated as TRUE" << endl;
	}
	else 
	{
		cout << " Condition is evaluated as FALSE" << endl;
	}	
cout << endl << "end =============  SmpCondTyp4::evaluate() =========== " << endl;
}
//debug end
	
	if (res) { return 1;}
	else { return 0; }
}

//------------------------------------------------------------------------
bool SmpCondTyp4::compare_str( Name& value1, Name& value2 , int& error)
{
	error = 0;
	if ( _operator == "==" )
	{
		if ( value1 == value2 ) { return true; }
		else { return false; }
	}
	else if ( _operator == "<>" )
	{
		if ( value1 == value2 ) { return false; }
		else { return true; }	
	}
	else
	{
		cout << "*** for comparison of strings, only == and <> are allowed"
		<< endl;
		
		error = 1;
		return false;
	}
}
//--------------------------------------------------------------------------
bool SmpCondTyp4::compare_int( Name& value1, Name& value2 , int& error)
{
	error = 0;
	int ivalue1, ivalue2;
	int iflg;
	
	iflg = sscanf(value1.getString(),"%d",&ivalue1);
	if (iflg != 1)
	{
		cout << " error evaluating Typ4 simple condition:" << endl
		<< value1 << " is not an integer " << endl;
		error = 1;
		return false;
	}
	
	iflg = sscanf(value2.getString(),"%d",&ivalue2);
	if (iflg != 1)
	{
		cout << " error evaluating Typ4 simple condition:" << endl
		<< value2 << " is not an integer " << endl;
		error = 1;
		return false;
	}
	
	if ( _operator == "==" )
	{
		if ( ivalue1 == ivalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == "<>" )
	{
		if ( ivalue1 != ivalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == "<" )
	{
		if ( ivalue1 < ivalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == ">" )
	{
		if ( ivalue1 > ivalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == "<=" )
	{
		if ( ivalue1 <= ivalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == ">=" )
	{
		if ( ivalue1 >= ivalue2 ){ return true;} else { return false; }	
	}
	
	cout << " operator " << _operator << " is unknown " << endl;
	error = 1;
	return false;
}
//---------------------------------------------------------------------------
bool SmpCondTyp4::compare_float( Name& value1, Name& value2 , int& error)
{
	error = 0;
	double fvalue1,fvalue2;
	int iflg;
	
	iflg = sscanf(value1.getString(),"%lg",&fvalue1);
	if (iflg != 1)
	{
		cout << " error evaluating Typ4 simple condition:" << endl
		<< value1 << " is not a floating point number " << endl;
		error = 1;
		return false;
	}
	
	iflg = sscanf(value2.getString(),"%lg",&fvalue2);
	if (iflg != 1)
	{
		cout << " error evaluating Typ4 simple condition:" << endl
		<< value2 << " is not a floating point number " << endl;
		error = 1;
		return false;
	}
	
	if ( _operator == "==" )
	{
		if ( fvalue1 == fvalue2 ) { return true;} else { return false; }
	}
	
	if ( _operator == "<>" )
	{
		if ( fvalue1 != fvalue2 ) { return true;} else { return false; }
	}
	
	if ( _operator == "<" )
	{
		if ( fvalue1 < fvalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == ">" )
	{
		if ( fvalue1 > fvalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == "<=" )
	{
		if ( fvalue1 <= fvalue2 ){ return true;} else { return false; }	
	}
	
	if ( _operator == ">=" )
	{
		if ( fvalue1 >= fvalue2 ){ return true;} else { return false; }	
	}
	
	cout << " operator " << _operator << " is unknown " << endl;
	error = 1;
	return false;
}
