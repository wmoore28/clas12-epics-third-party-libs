// smpcondtyp4.cxx: implementation of the SmpCondTyp4 class.
//
//                                                B. Franek
//                                               January 2012
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "smlline.hxx"
#include "smllinevector.hxx"
#include "utilities.hxx"

#include "smpcondtyp4.hxx"

#include "paroperand.hxx"

#include "ut_tr.hxx"
#include "errorwarning.hxx"
#include "opermanager.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SmpCondTyp4::SmpCondTyp4(const int idNum) : SmpCond(idNum)
{
	return;
}

SmpCondTyp4::~SmpCondTyp4() 
{
	return;
}
//===================================================================================
int SmpCondTyp4::initialise( SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& inext, int& jnext)
{
	SMLline line;
	
	SMLline lineBeingTranslated;

	Name temp;

//	cout << endl << "going to parse Simple condition of Typ4" << endl;
//	cout << "   ist, jst : " << ist << ", " << jst << endl;
//	pSMLcode->out();

	line = (*pSMLcode)[ist];
	*_pSMLcode += line;
//	cout <<  " line " << ist << endl; line.out(jst,jst+50);
	int icurr = ist; int jcurr = jst;

	lineBeingTranslated = (*pSMLcode)[icurr];
	int ierr = _operand1.initFromSMLcode(pSMLcode,icurr,jcurr);
	   
			  
//cout << " returned from operand1.initFromSMLcode,  ierr =  " << ierr << endl;
//cout << " icurr jcurr  " << icurr << " " << jcurr << endl;

//cout << endl << " Operand1:" << endl;
// _operand1.out(); cout << endl;

// Now get the operand
	int ibeg,jbeg;
	int idel,jdel; char del;

	ibeg = icurr; jbeg = jcurr;
	lineBeingTranslated = (*pSMLcode)[ibeg];
	del = getNextToken(pSMLcode,ibeg,jbeg," ",_operator,idel,jdel,inext,jnext);
//	cout << " after operator : inext, jnext " << inext << " " << jnext << endl;
	if ( _operator == "==" || _operator == "<>" || _operator == "<"
	  || _operator == ">" || _operator == ">=" || _operator == "<=" )
	{}
	else
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " looking for an arithmetic operator and found "
		 << _operator << endl;
		 exit (2);
	}
//	cout << " Operator : " << _operator << endl;
// Now the second operator
	icurr = inext; jcurr = jnext;

	lineBeingTranslated = (*pSMLcode)[icurr];
	ierr = _operand2.initFromSMLcode(pSMLcode,icurr,jcurr);
	   
			  
//cout << " returned from operand2.initFromSMLcode,  ierr =  " << ierr << endl;


//cout << endl << " Operand2:" << endl;
//_operand2.out(); cout << endl;

//	out(" ");
//cout << " icurr jcurr  " << icurr << " " << jcurr << endl;

	inext = icurr; jnext = jcurr;
	
//	exit(1);

	return 1;
}
//-----------------------------------------------------------------------
void SmpCondTyp4::negate()
{

}

//----------------------------------------------------------------------------
void SmpCondTyp4::out(const Name offset) const
{
	cout << offset << "Type 4 Simple condition" << endl;
	cout << endl << offset << " Operand1:" << endl;
	_operand1.out(offset); cout << endl;

	cout << endl << offset << "Operator |" << _operator << "|" << endl;

	cout << endl << offset << " Operand2:" << endl;
	_operand2.out(offset); cout << endl;
	

	return;
}
void SmpCondTyp4::outFull(const Name offset) const
{ out(offset); return;}

//---------------------------------------------  BF April 2000  ------
void SmpCondTyp4::outSobj(ofstream& sobj) const
{
// debug	
/*	cout << endl << "----------------------------------------" << endl;
	_operand1.outShort();
	cout << endl;
        _operand2.outShort();*/
// end debug
	 
	sobj << "    " << _type << endl;
	
	sobj << "OP1" << endl;
	_operand1.outSobj(sobj);
	
	sobj << "OP2" << endl;
	_operand2.outSobj(sobj);
	
	sobj << _operator.getString() << endl;
	
	return;
}
//---------------------------------------------------------------------------
int SmpCondTyp4::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
/*  cout << endl 
  << " ====================== SmpCondTyp4::examine() ============= " << endl;
	
  cout << "  Parent : " << _pParentUnit->unitId() 
  << "  " << _pParentUnit->unitName() << endl;
		
  cout << "     Complete Ancestry " << endl;
	
	int num;
	NameVector ids,names;
	
	_pParentUnit->ancestry(ids,names);
	
	num = ids.length();

	for (int i=0; i<num; i++)
	{
		cout << "     " << ids[i] << "  " << names[i] << endl;
	}
*/	
	int retcode1 = OperManager::getParValueAndOperValueTypes( firstLine, _pParentUnit, _operand1);	

//	_operand1.out();
	
//	cout << " operator " << _operator << endl;

	int retcode2 = OperManager::getParValueAndOperValueTypes( firstLine, _pParentUnit, _operand2);	


//	_operand2.out();
	
	if ( retcode1 != 0 || retcode2 != 0 ) { retcode = 1; }
	if ( retcode != 0 ) { return retcode; }
	
	retcode = OperManager::makeOperandsSameType
	              ( firstLine, _pParentUnit,
		        _operand1, _operand2);
			
	if ( retcode != 0 ) { return retcode; }		
	
	

	if ( _operand1.operValueType() == "STRING" )
		{
		if (_operator == "==" || _operator == "<>" ) {return 0;}
		else
		{
			retcode = 1;
			ErrorWarning::printHead("ERROR",firstLine);
			cout << " operator " 
			<< '\"' << _operator.getString() << '\"' << " is not allowed for Strings"
			<< endl;
		
			cout << endl; _pParentUnit->printParents(); cout << endl;

		}
	}
	
	return retcode;
}

//---------------------------------------------------------------------------------------------- SmpCondTyp4::simpleOut() const
Name SmpCondTyp4::outString() 
{
	Name temp;
	
	temp = _operand1.printingName();
	temp += " ";
	temp += _operator;
	temp += " ";
	temp += _operand2.printingName();

	return temp;
}
