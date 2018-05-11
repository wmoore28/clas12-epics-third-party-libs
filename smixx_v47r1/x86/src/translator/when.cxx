// when.cxx: implementation of the When class.
//
//                                                B. Franek
//                                               23 September 1999
// Copyright Information:
//      Copyright (C) 1996-2002 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "action.hxx"
#include "when.hxx"
#include "smlline.hxx"
#include "utilities.hxx"
#include "smiobject.hxx"
#include "errorwarning.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

When::When() : SMLUnit("when",3) 
{
	_pEndStateAction = 0;
}


When::When(int ident)
 : SMLUnit("when",3),
   _ident(ident), _condition(), _actionName("\0"),  _pEndStateAction(NULL), _endState("")
{
	return;
}

When::~When()
{
    delete _pSMLcode;
}
//----------------------------------------------------------------------
void When::translate() {

    Name token; int idel,jdel; int inext,jnext;
	int ist,jst;
	SMLline lineBeingTranslated;
	
	lineBeingTranslated = (*_pSMLcode)[0];
	char del = getNextToken(_pSMLcode,0,0," (",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	
	if ( token == "WHEN"){}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Keyword WHEN not found" << endl;
		 exit(2);
	}
	
	_condition.setParentUnit(this);
    ist = idel; jst = jdel;

	int ibegcond = ist; int jbegcond = jst;
	
	_condition.initialise(_pSMLcode,ist, jst, inext, jnext);
	// the method will either succeed or terminate the program
	
	int iendcond = inext; int jendcond = jnext; 
	
	if (_condition.terminator() == "DO" ) {
            ist = inext; jst = jnext;
	    
	    lineBeingTranslated = (*_pSMLcode)[ist];
	    del = getNextToken(_pSMLcode,ist,jst," ",_actionName,idel,jdel,inext,jnext);
	    _actionName.upCase(); _actionName.trim();
	}
        else if (_condition.terminator() == "ENDINSTATE" || _condition.terminator() == "MOVE_TO") {
            Name charIdent, stateName;
            ist = inext; jst = jnext;
	    
	    lineBeingTranslated = (*_pSMLcode)[ist];
	    del = getNextToken(_pSMLcode,ist,jst," ",stateName,idel,jdel,inext,jnext);
	    stateName.upCase(); stateName.trim();
            _actionName = "&END_IN_STATE_WHEN";
            charIdent = _ident; _actionName += charIdent;
            _pEndStateAction = createTermAction(_actionName,stateName);
	    _endState = stateName;
	}
        else if (_condition.terminator() == "STAY_IN_STATE") {
           Name crap;
            ist = inext; jst = jnext;
	    
	    lineBeingTranslated = (*_pSMLcode)[ist];
	    del = getNextToken(_pSMLcode,ist,jst," ",crap,idel,jdel,inext,jnext);
	    _actionName = "&NULL";
	}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Internal error, call expert" << endl;
		exit(2);
	}
	
	if (inext != -1 ) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Unrecognised code follows a valid WHEN condition"
			<< endl;
		exit(2);
	}

/*	cout << "dbg" << endl;
		cout << ibegcond << endl;
			cout << jbegcond << endl;
				cout << iendcond << endl;
					cout << jendcond << endl;
	_pSMLcode->out();*/
	
	_condition.getHeader(_pSMLcode,ibegcond,jbegcond,iendcond,jendcond);			
	return;
}
//---------------------------------------------------------------------
Action* When::createTermAction(const Name& actionName,const Name& stateName) {
	Action* pUnit = new Action(actionName);
	assert (pUnit != 0);
	pUnit->setParentUnit(this);
	
	Name line0 = "action : "; line0 += actionName;
	Name line1 = "   terminate_action / state = "; line1 += stateName;
	
	SMLline smlline0(-1,line0), smlline1(-1,line1);
	
	pUnit->acceptLine(smlline0); pUnit->acceptLine(smlline1);
	
	pUnit->translate();
	
	return pUnit;

} 
//---------------------------------------------------------------------
void When::out(const Name offset) const
{
	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn << endl;
	_condition.out(offset);
	cout << ptn << endl << ptn << " Action : " << _actionName << endl;
}
//------------------------------------------- BF April 2000 -----------
void When::outSobj(ofstream& sobj) const
{
	sobj << "*WHEN" << endl;

	_condition.outSobj(sobj);


	sobj << "do" << endl;
	sobj << _actionName.getString() << endl;
	sobj << "    0" << endl;
	sobj << "&THIS_OBJECT" << endl;

	return;
}
//--------------------------------------------------------------------------
Action* When::endStateActionPointer() const
{
	return _pEndStateAction;
}

//--------------------------------------------------------------------------
int When::examine()
{
	int retcode = 0;
	SMLline firstLine = (*_pSMLcode)[0];
/*	
  cout << endl 
  << " ====================== When::examine() =================" << endl;
	
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
	SMIObject* parentObject = (SMIObject*)(_pParentUnit->parentPointer("Object"));
	State* parentState = (State*)(_pParentUnit->parentPointer("State"));
 
// cout << " _actionName " << _actionName << " _endState " << _endState << endl;


 	if ( _endState == "" )
	{
	//   do when or stay_in_state when
		if (_actionName == "&NULL") {}  // stay_in_state
		else
		{
			//  do
			if ( parentState->hasAction(_actionName) ) {}
			else
			{
			//	retcode = 1; review
				ErrorWarning::printHead("SEVERE WARNING",firstLine);
				cout << " Action " << _actionName <<
					" is not declared " << endl;
				cout << endl; _pParentUnit->printParents(); cout << endl;
			}
		}
	}
	else
	{
	// move_to when
		int undeclared;
		if ( parentObject->hasState(_endState,undeclared) ) {}
		else
		{
		//	retcode = 1; review
			ErrorWarning::printHead("SEVERE WARNING",firstLine);
			cout << " Object " << parentObject->unitName() <<
			" does not have state " << _endState << " declared" << endl;
			cout << endl; _pParentUnit->printParents(); cout << endl;
		}		
	
	}
	
	int iflg = _condition.examine();
	
	return retcode+iflg;
}

//-------------------------------------------------------------------------------------------------------

Name When::outString() 
{
	Name temp;
	
	temp = "WHEN (...) ";
	
 	if ( _endState == "" )
	{
	// this is do or stay_in_state when
		if ( _actionName == "&NULL" )
		{
		// stay_in_state when
			temp += "stay_in_state";
		}
		else
		{
		// do when
			temp += "do ";
			temp += _actionName;
		}
	}	
	else
	{
	// this is move_to when
		temp += "move_to ";
		temp += _endState;

	}

	
	return temp;
}
