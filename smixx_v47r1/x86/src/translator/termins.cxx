// termins.cxx: implementation of the TermIns class.
//
//                                                B. Franek
//                                           30 September 1999
//////////////////////////////////////////////////////////////////////

#include "termins.hxx"

#include "stdlib.h"
#include "smlline.hxx"
#include "utilities.hxx"
#include "name.hxx"
#include "smiobject.hxx"
#include "errorwarning.hxx"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TermIns::TermIns()  
{
   _name = "terminate_action";
   _endState = "\0";
   return;
}

TermIns::~TermIns()
{
    delete _pSMLcode;
}
//=======================================================================
void TermIns::translate() {

	Name token;
	int idel, jdel, ist, jst, inext, jnext;
	SMLline lineBeingTranslated;

	lineBeingTranslated = (*_pSMLcode)[0];
	char del = getNextToken(_pSMLcode,0,0," /",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
//	cout << token << endl; 
//	cout << " " << idel << " " << jdel << " " << inext << " " << jnext << endl;
	if (token == "TERMINATE_ACTION") {
		if ( del != '/' ) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << "Expecting '/' ";
			cout << " but found " << token << endl;
			exit(2); 
		}
		ist = inext; jst = jnext;
		
		lineBeingTranslated = (*_pSMLcode)[ist];
		del = getNextToken(_pSMLcode,ist,jst,"=",token,idel,jdel,inext,jnext);
//cout << token << endl;
//cout << " " << idel << " " << jdel << " " << inext << " " << jnext << endl;
		if ( del != '=' ) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << "Expecting '=' ";
			cout << " but found " << token << endl;
			exit(2); 
		}
		token.upCase(); token.trim();
		if ( token == "STATE" ) {}
		else {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << "Expecting keyword STATE ";
			cout << " but found " << token << endl;
			exit(2); 
		}
		ist = inext; jst = jnext;
		
		lineBeingTranslated = (*_pSMLcode)[ist];
		del = getNextToken(_pSMLcode,ist,jst," ",_endState,idel,jdel,inext,jnext);
	}
	else if (token == "ENDINSTATE" || token == "MOVE_TO") {
		if ( del != ' ' ) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << " Expecting ' ' ";
			cout << " but found " << token << endl;
			exit(2); 
		}
		ist = inext; jst = jnext;
		
		lineBeingTranslated = (*_pSMLcode)[ist];
		del = getNextToken(_pSMLcode,ist,jst," ",_endState,idel,jdel,inext,jnext);
	}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Expecting TERMINATE_ACTION or ENDINSTATE or MOVE_TO";
		cout << " but found " << token << endl;
		exit(2); 
	}

	_endState.upCase(); _endState.trim();
	if (!check_name(_endState)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << _endState << "  is not a name" << endl;
		exit(2); 
	}

	return;
}

void TermIns::out(const Name offset) const
{

	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn 
							<< "End state " << _endState << endl;
}
//-------------------------------------  BF April 2000  ---------------
void TermIns::outSobj(int& ,int , ofstream& sobj, Registrar&) const
{
	sobj << "terminate" << endl;

	sobj << _endState.getString() << endl;

	return;

}

//---------------------------------------------------------------------------
int TermIns::examine()
{
	int retcode = 0;
	SMLline firstLine = (*_pSMLcode)[0];
/*
  cout << endl 
  << " ====================== TermIns::examine() ============= " << endl;
	
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
	SMIObject* parentObject;  int dummy;
	parentObject = (SMIObject*)_pParentUnit->parentPointer("Object");
	
	if ( parentObject->hasState(_endState,dummy) ) {}
	else
	{
	//	retcode = 1; review
		ErrorWarning::printHead("SEVERE WARNING",firstLine);
		cout << " Object " << parentObject->unitName() 
		<< " does not have state " << _endState <<
			" declared " << endl;
		cout << endl; _pParentUnit->printParents(); cout << endl;      
	}
	
	int iflg = examineUnits();
	
	return retcode+iflg;
}

//-------------------------------------------------------------------------------------------------------
Name TermIns::outString() 
{
	Name temp;
	
	temp = "MOVE_TO ";
	temp += _endState;
	
	return temp;
}



