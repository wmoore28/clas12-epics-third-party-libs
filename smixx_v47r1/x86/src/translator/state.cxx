//-----------------------------------------------------------------------
//                         State  Class
//                                                 B. Franek
//                                                 28 March 1999
// Copyright Information:
//      Copyright (C) 1996-2002 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <assert.h>
#include "when.hxx"
#include "action.hxx"
#include "state.hxx"
#include "smlline.hxx"
#include "errorwarning.hxx"

//--------------------------- Constructors -------------------------------

State::State (const Name& name, const Name& subobj) 
	:SMLUnit("State",50,name),
	 _subobjectName(subobj),
	 _pAttributeBlock(NULL), _whens(), _actions()
{
	return;
}


State::~State() {
	delete _pSMLcode;
}

//-----------------------------------------------------------------------------
void State::translate() {

	int collectingActions = 0;

	_pAttributeBlock = new AttributeBlock();
	assert (_pAttributeBlock != 0);

	int newUnit = 1;
	SMLUnit* pUnit = _pAttributeBlock;
	Name unitType = "attribute block";
	Registrar* pTypeRegistrar = 0;
	Name name;
	Name registeredName;


	SMLline line; 
	int numOfLines = _pSMLcode->length();

	int whenNo = 0;
	
	for (int i = 0; i<numOfLines; i++) {   //***** Beg of collection loop
		line = (*_pSMLcode)[i];
		int stateKeywordLine = line.stateKeyword(name);

		if (stateKeywordLine == 100) {
			if (collectingActions == 1 ) {
				ErrorWarning::printHead("ERROR",line);
				cout << " Definition of WHEN must be before the first action"
				<< endl;
				exit(2);
			}
			newUnit = 1;
			pUnit = new When(whenNo);
			whenNo++;
			assert (pUnit != 0);
			unitType = "When";
			pTypeRegistrar = &_whens;
			registeredName = "-";
		}
		else if (stateKeywordLine == 200) {
			collectingActions = 1;
			newUnit = 1;
			pUnit = new Action(name);
			assert (pUnit != 0);
			unitType = "Action";
			pTypeRegistrar = &_actions;
			registeredName = name;
		}
		else {}
		if (newUnit) {
			newUnit = 0;
			_internalSMLUnits.addRegardless(unitType,pUnit);
			pUnit->setParentUnit(this);
			if(pTypeRegistrar) {
				if (registeredName == "-") {
					pTypeRegistrar->addRegardless(registeredName,pUnit);
				}
				else {
					int iflag = pTypeRegistrar->add(registeredName,pUnit);
					if (iflag != 1) {
						ErrorWarning::printHead("ERROR",line);
						cout << unitType << registeredName 
						<< " has allready been declared" << endl;
						exit(2);
					}
				}
			}
		}
		pUnit->acceptLine(line);
	}                              //****** end of collection loop


	 translateUnits();
	 
	getWhenActions();
}
//------------------------------------------------------- BF April 2000 ---------
Name State::subObject() const
{
	return _subobjectName;
}
//-------------------------------------------------------  BF April 2000  -------
void State::out(const Name offset) const
{
	SMLUnit::out(offset);
	cout << offset.getString() << "  ................................." << endl 
		<< offset.getString() << "Subobject : " << _subobjectName << endl;
	return;
}
//------------------------------------------------------- BF April 2000 ---------
void State::outSobj(ofstream& sobj) const
{
	sobj << "*STATE" << endl;
	sobj << _name.getString() << endl;

	
	int nAtt = _pAttributeBlock->_attributes.length();

	sobj << "    " << nAtt << endl;

	for (int i=0; i<nAtt; i++) {
		sobj << _pAttributeBlock->_attributes[i].getString() << endl;
	}

	int nw = _whens.length();

	for (int iw=0; iw < nw; iw++) {
		void* ptnvoid = _whens.gimePointer(iw) ;
		When* pWhen;
		pWhen = (When*)ptnvoid;
		pWhen->outSobj(sobj);
	}


	int na = _actions.length();

	for (int ia=0; ia < na; ia++) {
		void* ptnvoid = _actions.gimePointer(ia) ;
		Action* pAction;
		pAction = (Action*)ptnvoid;
		pAction->outSobj(sobj);
	}

	sobj << "*END_STATE" << endl;
	return;
}
void State::getWhenActions() 
{
	int numOfWhens = _whens.length();
	for ( int i=0; i<numOfWhens; i++) {
		void* ptnvoid = _whens.gimePointer(i);
		When* pW = (When*) ptnvoid;
		Action* pA = pW->endStateActionPointer();
		if (pA) {
//			Name offset = " "; pA->out(offset);
			Name nA = pA->name();
			_actions.add(nA,pA);
		}
	}
}
//---------------------------------------------------------------------------
int State::getAttributes(NameVector& attr) const
{
	int nAtt = _pAttributeBlock->_attributes.length();  // number of attribs
	
	for (int i=0; i<nAtt; i++) {
		attr += _pAttributeBlock->_attributes[i];
	}
	return nAtt;
}
//----------------------------------------------------------------------------
bool State::isUndeclared() const
{
	NameVector attribs;
	
	int num = getAttributes(attribs);
	
	if ( num == 0 ) return false;
	
	for (int i=0; i<num; i++)
	{
		if ( attribs[i] == "UNDECLARED_STATE" ) { return true; }
	}
	
	return false;
	
}
//-----------------------------------------------------------------------------
bool State::hasAction(const Name& actionNm) const
{
	int numAct = _actions.length();
	
	void* ptnvoid; Action* pact;
	
	for ( int i=0; i<numAct; i++ )
	{
		ptnvoid = _actions.gimePointer(i);
		pact = (Action*)ptnvoid;
		if ( actionNm == pact->name() ) { return true; }
	}

	return false;
}
