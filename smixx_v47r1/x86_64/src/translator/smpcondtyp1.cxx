// smpcondtyp1.cxx: implementation of the SmpCondTyp1 class.
//
//                                                B. Franek
//                                               21 February 2000
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "smlline.hxx"
#include "smllinevector.hxx"
#include "utilities.hxx"

#include "smpcondtyp1.hxx"
#include "smiobject.hxx"

#include "ut_tr.hxx"
#include "errorwarning.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SmpCondTyp1::SmpCondTyp1(const int idNum) : SmpCond(idNum),
                                            _type(1)
{
	return;
}

SmpCondTyp1::~SmpCondTyp1() 
{
	return;
}
//===================================================================================
int SmpCondTyp1::initialise( SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& inext, int& jnext)
{
	char* ptnDel;
	
	SMLline line; char* pLine;
	SMLline lineBeingTranslated;

	char* ptn;

	Name temp;

//	cout << endl << "going to parse Simple condition of Typ1" << endl;

	int i = ist;
	line = (*pSMLcode)[i]; 
	*_pSMLcode += line;
//	cout << line << endl;
	pLine = line.getString();
	ptn = &pLine[jst];
	
  
	int ibeg,jbeg;
	int idel,jdel;

//	cout << "ist,jst" << " " << ist << "," << jst << endl;
	lineBeingTranslated = (*pSMLcode)[ist];
	char del = getNextToken(pSMLcode,ist,jst," ",
					_objectName,idel,jdel,inext,jnext);
//	cout << "idel,jdel" << " " << idel << "," << jdel << endl;
	_objectName.upCase(); _objectName.trim();
	if (!check_name(_objectName)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Object name is not a name " << _objectName << endl;
		exit(2);
	}
//	cout << " Object name " << _objectName << endl;
	ibeg = inext; jbeg = jnext;
	lineBeingTranslated = (*pSMLcode)[ibeg];
	del = getNextToken(pSMLcode,ibeg,jbeg," {",temp,idel,jdel,inext,jnext);
	temp.upCase(); ptnDel = &del;

	if (temp == "IN_STATE") { _inState = 1;}
	else if (temp == "NOT_IN_STATE") { _inState = 0;}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " looking for IN_STATE or NOT_IN_STATE " << endl 
		 << " but found: " << temp << endl;
		exit(2);
	}
//	cout << _inState << endl;
	ibeg = inext; jbeg = jnext;

	if ( *ptnDel == ' ' ) {
		lineBeingTranslated = (*pSMLcode)[ibeg];
		del = getNextToken(pSMLcode,ibeg,jbeg," )",temp,idel,jdel,inext,jnext);
		temp.upCase(); temp.trim(); ptnDel = &del;

		if(!check_name(temp)) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << " State name is not a name " << temp << endl;
			exit(2);
		}
		_states += temp;
 
		if (del == ')') {inext = idel; jnext = jdel;}
		return 1;
	}
	
	getStates(pSMLcode,ibeg,jbeg,inext,jnext,_states);
	// if anything goes wrong inside the method, program is stopped

	return 1;
}
//-----------------------------------------------------------------------
void SmpCondTyp1::negate()
{
	if (_inState == 1) {_inState = 0; return;}
	if (_inState == 0) {_inState = 1; return;}
}

//----------------------------------------------------------------------------
void SmpCondTyp1::out(const Name offset) const
{
	Name offset1 = offset; offset1 += "          ";
	char temp[20];
	if ( _inState == 1 ) {strcpy(temp," in_state ");}
	else {strcpy(temp," not_in_state ");}
	char* ptn = offset.getString();
	cout << ptn  << _idNum << " " << _objectName << temp << " ";
	_states.out(offset1); 
	return;
}
void SmpCondTyp1::outFull(const Name offset) const
{ out(offset); return;}

//---------------------------------------------  BF April 2000  ------
void SmpCondTyp1::outSobj(ofstream& sobj) const
{
        sobj << "    " << _type << endl;
	sobj << _objectName.getString() << endl;

	if (_inState) { sobj << "in_state" << endl;}
	else          { sobj << "not_in_state" << endl;}

	int ns = _states.length();

	sobj << "    " << ns << endl;

	for (int is=0; is<ns; is++) {
		sobj << (_states[is]).getString() << endl;
	}

	return;
}
//-----------------------------------------------------------------------
int getStates(SMLlineVector* pSMLcode,
			const int ist, const int jst,
//			int& iend, int& jend,
			int& inext, int& jnext,NameVector& states) 
{
	Name temp;
	
	SMLline lineBeingTranslated;

//	cout << endl << "collecting states" << endl;

	char del; int ibeg,jbeg,idel,jdel;
	ibeg = ist; jbeg = jst;

	for (;;) {
		lineBeingTranslated = (*pSMLcode)[ibeg];
		del = getNextToken(pSMLcode,ibeg,jbeg,",}",temp,idel,jdel,inext,jnext);
		if (temp == "\0") {break;}
		temp.upCase();
		if(!check_name(temp)) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << " State name " << temp << " is not a name " << endl;
			exit(2);
		}
		states += temp;
//		if (del == '}') { iend = idel; jend = jdel; return 1;}
		if (del == '}') { return 1;}

		if (inext <0) {break;}
		ibeg=inext;jbeg=jnext;
	}

	ErrorWarning::printHead("ERROR",lineBeingTranslated);
	cout << " matching } not found" << endl;
	exit(2);
}
//---------------------------------------------------------------------------
int SmpCondTyp1::examine()
{
	int retcode = 0;

	SMLline firstLine = (*_pSMLcode)[0];	
/*
  cout << endl 
  << " ====================== SmpCondTyp1::examine() ============= " << endl;
	
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
// First check that the object is declared

	SMIObject* pObj = gimeObjectPointer(_objectName);
	
	if ( pObj == 0 )   // the object is not declared
	{
//		retcode = 1; //review
		ErrorWarning::printHead("SEVERE WARNING",firstLine);
		cout << " Object " << _objectName << " or its class is not declared" << endl;
					
		cout << endl; _pParentUnit->printParents(); cout << endl;

		return retcode;
	}
	
// Check that the referenced states are declared

	int nStates = _states.length();
	Name stateNm; int undeclared;
	bool hasStateFlg;

	for ( int ist=0; ist<nStates; ist++ )
	{
		hasStateFlg = pObj->hasState(_states[ist],undeclared );
		
		if ( undeclared == 1 )  // if object has undeclared states, exit the loop
		{
			break;
		}
		
		if ( _states[ist] == "DEAD") { continue; }  // DEAD state does not have to be declared
		
		if ( !hasStateFlg )
		{
//		retcode = 1;  // review
			
			ErrorWarning::printHead("SEVERE WARNING",firstLine);
			cout << " Object " << _objectName 
			<< " does not have state " << _states[ist] << " declared" << endl;
			
			cout << endl; _pParentUnit->printParents(); cout << endl;

			return retcode;
		}
	} 

	return retcode;
}
//-------------------------------------------------------------------------------------------------------
Name SmpCondTyp1::outString() 
{
	Name temp;
	
	temp = _objectName;
	
	if ( _inState == 1 )
	{
		temp += " in_state ";
	}
	else
	{
		temp += " not_in_state ";
	}
	
	temp += "{"; temp += _states[0];
	
	int nStates = _states.length();
	
	for ( int ist=1; ist<nStates; ist++ )
	{
		temp += ","; temp += _states[ist];
	}
	
	temp += "}";
	
	return temp;
	
}

