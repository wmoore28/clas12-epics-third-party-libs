// smpcondtyp2.cxx: implementation of the SmpCondTyp2 class.
//
//                                                B. Franek
//                                               4-July 2002
// Copyright Information:
//      Copyright (C) 1999-2002 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "smlline.hxx"
#include "smllinevector.hxx"
 
#include "utilities.hxx"
#include "errorwarning.hxx"

#include "smpcondtyp2.hxx"
#include "smiobjectset.hxx"
#include "smiobjectsetunion.hxx"
#include "smiobject.hxx"

#include "registrar.hxx"

extern Registrar allObjectSets;
extern Registrar allClasses;

int getStates(SMLlineVector* pSMLcode,
		const int ist, const int jst,  
		int& inext, int& jnext,NameVector& states);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SmpCondTyp2::SmpCondTyp2(const int idNum) : SmpCond(idNum),
                                            _type(2)
{
	return;
}

SmpCondTyp2::~SmpCondTyp2() 
{
	return;
}
//===================================================================================
int SmpCondTyp2::initialise( SMLlineVector* pSMLcode,
			const int ist, const int jst,  
//			int& iend, int& jend, int& inext, int& jnext)
			int& inext, int& jnext)
{
	char* ptnDel;
	
	SMLline line; char* pLine;
	SMLline lineBeingTranslated;

	char* ptn;

	Name temp;

//	cout << endl << "going to parse Simple condition" << endl;

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
					temp,idel,jdel,inext,jnext);
//	cout << "idel,jdel" << " " << idel << "," << jdel << endl;
	temp.upCase(); temp.trim();
	if (temp == "ALL_IN") {
		_allIn = 1;
	}
	else {
		_allIn = 0;
	}
	lineBeingTranslated = (*pSMLcode)[inext];
	del = getNextToken(pSMLcode,inext,jnext," ",
				_objectSetName,idel,jdel,inext,jnext);
	_objectSetName.upCase(); _objectSetName.trim();
	if (!check_name(_objectSetName)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Object Set name is not a name " << _objectSetName << endl;
		exit(2);
	}
//	cout << " Object Set name " << _objectSetName << endl;
	ibeg = inext; jbeg = jnext;
	lineBeingTranslated = (*pSMLcode)[ibeg];
	del = getNextToken(pSMLcode,ibeg,jbeg," {",temp,idel,jdel,inext,jnext);
	temp.upCase(); ptnDel = &del;

	if (temp == "IN_STATE") { _inState = 1;}
	else if (temp == "NOT_IN_STATE") { _inState = 0;}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout  << "looking for IN_STATE or NOT_IN_STATE "  
			<< " but found: " <<temp << endl;
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

//		iend = idel; jend = jdel; 
		if (del == ')') {inext = idel; jnext = jdel;}
		return 1;
	}

	
	getStates(pSMLcode,ibeg,jbeg,inext,jnext,_states); 
	// global function, implementation in smpcondtyp1.cxx


	return 1;

}
//-----------------------------------------------------------------------
void SmpCondTyp2::negate()
{
// As can be easily shown, negating simple condition of tye 2 is interchanging
//   'in_state' with 'not_in_state'  and  'any_in' with  'all_in'
//  BF June 2007

	if	(_allIn == 1)	{_allIn = 0;}
	else			{_allIn = 1;}
	
	
	if	(_inState == 1) {_inState = 0;}
	else			{_inState = 1;}
	
	return;

}

//----------------------------------------------------------------------------
void SmpCondTyp2::out(const Name offset) const
{
	Name offset1 = offset; offset1 += "          ";
	char temp[20]; Name allin;
	if ( _inState == 1 ) {strcpy(temp," in_state ");}
	else {strcpy(temp," not_in_state ");}
	if (_allIn) {allin = "ALL_IN";}
	else {allin = "ANY_IN";}
	char* ptn = offset.getString();
	cout << ptn  << _idNum << " " 
	<< allin << " " << _objectSetName << temp << " ";
	_states.out(offset1); 
	return;
}
void SmpCondTyp2::outFull(const Name offset) const
{ out(offset); return;}

//---------------------------------------------  BF April 2000  ------
void SmpCondTyp2::outSobj(ofstream& sobj) const
{
        sobj << "    " << _type << endl;
	sobj << _objectSetName.getString() << endl;
	
	if (_allIn) {
		sobj << "all_in" << endl;
	}
	else {
		sobj << "any_in" << endl;
	}
	

	if (_inState) { sobj << "in_state" << endl;}
	else          { sobj << "not_in_state" << endl;}

	int ns = _states.length();

	sobj << "    " << ns << endl;

	for (int is=0; is<ns; is++) {
		sobj << (_states[is]).getString() << endl;
	}

	return;
}
//---------------------------------------------------------------------------
int SmpCondTyp2::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
/*  cout << endl 
  << " ====================== SmpCondTyp2::examine() ============= " << endl;
	
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
// First check that the object set is declared
//cout << " No of Object Sets : " << allObjectSets.length() << endl;

	SMLUnit* pUnit = (SMLUnit*)(allObjectSets.gimePointer(_objectSetName));
	
	if ( pUnit == 0 )   // the object set is not declared
	{
//		retcode = 1; //review
		ErrorWarning::printHead("SEVERE WARNING",firstLine);
		cout << " Object Set " << _objectSetName << " is not declared" << endl;
			
		cout << endl; _pParentUnit->printParents(); cout << endl;

		return retcode;
	}
	
// Check if the Set is declared  belonging to a class

	Name setClass;
		
	if (pUnit->unitId() == "ObjectSetUnion")
	{
		SMIObjectSetUnion* pObjSetUnion = (SMIObjectSetUnion*)pUnit;
		setClass = pObjSetUnion->setClass();
		
	}
	else   // simple set
	{
		SMIObjectSet* pObjSet = (SMIObjectSet*)pUnit;
		setClass = pObjSet->setClass();
	}

	
	if ( setClass == "" )
	{
/*
  =================   This warning should be moved to examine method fo sets ==========
		cout << endl ; printErrorWarningHead("WARNING");
			
		cout << endl; _pParentUnit->printParents(); cout << endl;
		cout << " Object Set " << _objectSetName 
		<< " is not associated to a class, no state checks can be performed " << endl;
*/
		return retcode;
	
	} 
	
// If it belongs to a class, we can check the states. 
// First check if such a class exists
	SMIObject* pObj = (SMIObject*)(allClasses.gimePointer(setClass));
	
	if ( pObj == 0 )
	{
/*
  ==============  This ERROR should be moved to examine method for Object Sets ====
//		retcode = 1; //review
		cout << endl ; printErrorWarningHead("SEVERE WARNING");
			
		cout << endl; _pParentUnit->printParents(); cout << endl;
		cout << " associated Class " << setClass << " is not declared" << endl;
*/
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
			cout << " Class " << setClass 
			<< " does not have state " << _states[ist] << " declared" << endl;
						
			cout << endl; _pParentUnit->printParents(); cout << endl;

			return retcode;
		}
	} 
	

	return retcode;
}

//-------------------------------------------------------------------------------------------------------
Name SmpCondTyp2::outString() 
{
	Name temp;

	
	if ( _allIn == 1 )
	{
		temp = "all_in ";
	}
	else
	{
		temp = "any_in ";
	}
	
	temp += _objectSetName;
	
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
