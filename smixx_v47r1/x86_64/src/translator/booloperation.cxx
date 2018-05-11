// booloperation.cxx: implementation of the BoolOperation class.
//
//                                                B. Franek
//                                           26 February 2000
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "booloperation.hxx"

#include "stdlib.h"
#include "string.h"
#include "name.hxx"
#include "boolitem.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BoolOperation::BoolOperation()
{
	return;  
}

BoolOperation::BoolOperation(const int idNum,const char operation,
			const char operand1,const int num1,
			const char operand2,const int num2)  
{
//	cout << "BoolOperation constructor called" << endl;
//	cout << idNum << " " << operand1 << "" << num1 << endl;

	_idNum = idNum;
	_operation = operation;
	if ( operation == '-' ) {
		_operand1 = operand1; _num1 = num1;
	}
	else if ( operation == '+' || operation == '.' ) {
		_operand1 = operand1; _num1 = num1;
		_operand2 = operand2; _num2 = num2;
	}
	else {
		cout << " Error  Illegal operation in BoolOperation" << endl;
		exit(2);
	}
		
	return;
}
//---------------------------------------------------------------------
BoolOperation::BoolOperation(const int idNum,const char operation,
			BoolItem* pOperand1, BoolItem* pOperand2)  
{
//	cout << "BoolOperation constructor called" << endl;

	_idNum = idNum;
	_operation = operation;
	if ( operation == '-' ) {
		_pOperand1 = pOperand1; _pOperand2 = 0;
	}
	else if ( operation == '+' || operation == '.' ) {
		_pOperand1 = pOperand1; _pOperand2 = pOperand2;
	}
	else {
		cout << " Error  Illegal operation in BoolOperation" << endl;
		exit(2);
	}
		
	return;
}

BoolOperation::BoolOperation(const BoolOperation& source)
: BoolItem()
{
	_operation = source._operation;

	_operand1 = source._operand1; _num1 = source._num1;
	_operand2 = source._operand2; _num2 = source._num2;
	_pOperand1 = source._pOperand1; _pOperand2 = source._pOperand2; 
 

	return;
}

int BoolOperation::idNum() const
{
	return _idNum;
}

char BoolOperation::type() const
{
	return 'X';
}

BoolOperation::~BoolOperation()
{
    return;
}

//----------------------------------------------------------------------
void BoolOperation::out(const Name offset) const
{
//	cout << "BoolOperation::out called" << endl; 

	char* ptn=offset.getString(); cout << ptn ;

	if ( _operation == '-' ) {
//		cout << _operation << _operand1 << _num1 << endl;
		cout << "X" << _idNum << " = ";	
		cout << _operation << _pOperand1->type() << _pOperand1->idNum() << endl;
	}
	else {
//		cout << _operand1 << _num1 
//			<< " " << _operation << " " << _operand2 << _num2 << endl;
		cout << "X" << _idNum << " = "
			 << _pOperand1->type() << _pOperand1->idNum() 
		 	<< " " << _operation << " "
			<< _pOperand2->type() << _pOperand2->idNum() << endl;
	}
	return;
}
//----------------------------------------------------------------------
void BoolOperation::outFull(const Name offset) const
{
//	cout << "BoolOperation::out called" << endl;
//	out(offset);
    Name tmpOffset = offset;
	if (_pOperand1->type() == 'X') {_pOperand1->outFull(tmpOffset);}
	if (_pOperand2 != 0 && _pOperand2->type()=='X') {_pOperand2->outFull(tmpOffset);}

	char* ptn=tmpOffset.getString(); cout << ptn ;

	if ( _operation == '-' ) {
		cout << "X" << _idNum << " = ";	
		cout << _operation << _pOperand1->type() << _pOperand1->idNum() << endl;
	}
	else {
		cout << "X" << _idNum << " = "
			 << _pOperand1->type() << _pOperand1->idNum() 
		 	<< " " << _operation << " "
			<< _pOperand2->type() << _pOperand2->idNum() << endl;
	}
	return;
}

//------------------------------------------  BF April 2000  --------
void BoolOperation::outSobj(ofstream& sobj) const
{
	if (_pOperand1->type() == 'X') {_pOperand1->outSobj(sobj);}
	if (_pOperand2 != 0 && _pOperand2->type()=='X') {_pOperand2->outSobj(sobj);}

	if ( _operation == '-' ) {
/*		sobj << "X" << _idNum << " = ";	
		sobj << _operation << _pOperand1->type() << _pOperand1->idNum() << endl;*/
		sobj << "not   ";
		sobj << _pOperand1->type();
		sobj.width(3); sobj.fill('0'); sobj	<<_pOperand1->idNum();
		sobj << "   000" << endl;
	}
	else {
		if (_operation=='+') {sobj << "or    " ;}
		else                 {sobj << "and   ";}
			int idnum1 = _pOperand1->idNum();
			int idnum2 = _pOperand2->idNum();

			
			sobj << _pOperand1->type();
			sobj.width(3); sobj.fill('0'); sobj	<<idnum1;
			sobj << "  " << _pOperand2->type();
			sobj.width(3); sobj.fill('0'); sobj	<<idnum2;
			sobj << endl;
	}
	return;
}
