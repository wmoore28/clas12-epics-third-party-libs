// doins.cxx: implementation of the DoIns class.
//
//                                                B. Franek
//                                           30 September 1999
// Copyright Information:
//      Copyright (C) 1999/2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "doins.hxx"
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "ut_tr.hxx"
#include "errorwarning.hxx"

#include "registrar.hxx"

extern Registrar allObjects, allObjectSets, allClasses;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DoIns::DoIns()  
{
   _name = "do";
//   cout << "Do constructor called" << endl;
   return;
}

DoIns::~DoIns()
{
    delete _pSMLcode;
}
//-----------------------------------------------------------------------
void DoIns::translate() {

	Name token; int idel,jdel; int inext,jnext;
	
	SMLline lineBeingTranslated;
	
	lineBeingTranslated = (*_pSMLcode)[0];
	int ist,jst;
	char del = getNextToken(_pSMLcode,0,0," ",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();

	if ( token == "DO"){}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"expected DO instruction. DO keyword not found");
		exit(2);
	}

	ist = inext; jst = jnext;

	if (ist < 0) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"  Nothing follows the keyword DO");
		exit(2);
	}
	
	lineBeingTranslated = (*_pSMLcode)[ist];
	del = getNextToken(_pSMLcode,ist,jst," (",_actionNm,idel,jdel,inext,jnext);
	_actionNm.upCase(); 
	if ( !check_name(_actionNm) ) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Action: " << _actionNm << " is not a name" << endl;
		exit(2);
	}

	if ( del == '(' ) {
		ist = idel, jst = jdel;
		_parameters.initFromSMLcode( 0, _pSMLcode, ist,jst,inext,jnext);
// If something goes wrong, Translator is terminated inside the method
	}
	
	ist = inext; jst = jnext;
	if (ist < 0) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"DO instruction is incomplete");
		exit(2);
	}
	
	lineBeingTranslated = (*_pSMLcode)[ist];
	del = getNextToken(_pSMLcode,ist,jst," ",_objectNm,idel,jdel,inext,jnext);
	_objectNm.upCase();

	if (_objectNm == "ALL_IN") {
		Name tmp = "&"; Name objectSetName;
		tmp += _objectNm; tmp += "_";
		if (inext < 0) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated
			,"DO instruction is incomplete");
			exit(2);
		}
		
		lineBeingTranslated = (*_pSMLcode)[inext];
		del = getNextToken(_pSMLcode,inext,jnext," ",
					objectSetName,idel,jdel,inext,jnext);
		objectSetName.upCase(); objectSetName.trim();
		_objectNm = tmp;
                _objectNm += objectSetName;
	}

	if ( !check_name(_objectNm) ) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " The object " <<_objectNm << " is not a name" << endl;
		exit(2);
	}
        if (inext>0) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"Some crap follows DO instruction");
		exit(2);
	}
	return;
}


void DoIns::out(const Name offset) const
{

	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn ;
	cout << "Action : "<< _actionNm << "  Object : " << _objectNm << endl;
	_parameters.out(offset);

  return;
}
//----------------------------------  BF April 2000  ------------------
void DoIns::outSobj(int&,int,ofstream& sobj,Registrar&) const
{
	sobj << "do" << endl;

	sobj << _actionNm.getString() << endl;

	int numpar = _parameters.numOfEntries();

	sobj << "    " << numpar << endl;

	for (int ip=0; ip<numpar; ip++) {
		Name name,value; char type[10];
		_parameters.get(ip,name,value,type);
		sobj << name.getString() << endl;
		sobj << value.getString() << endl;
	}

	sobj << _objectNm.getString() << endl;

	return;
}

//---------------------------------------------------------------------------
int DoIns::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
	
	/*
//beg debug
  cout << endl 
  << " ====================== DoIns::examine() ============= " << endl;
	
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
	
	cout << "Parent Unit Code " << endl;	

	_pSMLcode->out();	
//end debug
*/

	Name objSetName = "";
	
	char* pObjectName = _objectNm.getString();
	if (strncmp(pObjectName,"&ALL_IN_",8) == 0)
	{ // refering to object set
		objSetName = &pObjectName[8];
	}
	
	if ( objSetName == "" )
	{  // ordinary object. Check, that it is declared
		SMIObject* pObj = gimeObjectPointer(_objectNm);
		if ( pObj == 0 )
		{
		//	retcode = 1; review
			ErrorWarning::printHead("SEVERE WARNING",firstLine);
			cout << " Object " << _objectNm <<
			" is not declared " << endl;
			cout << endl; _pParentUnit->printParents(); cout << endl;

		}
		else
		{
		// the object is declared. Check it has the action declared

			if (pObj->hasAction(_actionNm)) {}
			else
			{
			//	retcode = 1; review
				ErrorWarning::printHead("SEVERE WARNING",firstLine);
				cout << " Action " << _actionNm <<
				" is not declared " << endl;
				cout << endl; _pParentUnit->printParents(); cout << endl;

			
			}
		
		}
	}
	else
	{  // object set. Check, that it is declared
		void* pvoid = allObjectSets.gimePointer(objSetName);
		if ( pvoid == 0 )
		{
		//	retcode = 1; review
			ErrorWarning::printHead("SEVERE WARNING",firstLine);
			cout << " Object Set " << objSetName <<
			" is not declared " << endl;
			cout << endl; _pParentUnit->printParents(); cout << endl;

		}
		else
		{
		// Set is declared. If it is associated with a class, we can
		// check the action.
			SMIObjectSet* pObjSet = (SMIObjectSet*)pvoid;
			Name setClass = pObjSet->setClass();
			if (setClass == "") {}
			else
			{
			// it is associated with class
				void* pvoid = allClasses.gimePointer(setClass);
				if ( pvoid == 0 ) {} // the class is not declared, will be dealt with elsewhere
				else
				{
					SMIObject* pObj = (SMIObject*)pvoid;
					if (pObj->hasAction(_actionNm)) {}
					else
					{
					//	retcode = 1; review
						ErrorWarning::printHead("SEVERE WARNING",firstLine);
						cout << " Action " << _actionNm <<
						" is not declared in Class " << setClass << endl;
						cout << endl; _pParentUnit->printParents(); cout << endl;
			
					}
					
				}
			}
		}
	}
	
		
	
	int iflg = examineUnits();
	
	return retcode+iflg;
}
//-------------------------------------------------------------------------------------------------------
Name DoIns::outString() 
{
	Name temp;
	
	temp = "do ";
	temp += _actionNm;
	
	int numPars = _parameters.numOfEntries();
	
	if (numPars > 0) {temp += " (...) ";}
	else {temp += " ";}
	
	char* pObjectName = _objectNm.getString();
	if (strncmp(pObjectName,"&ALL_IN_",8) == 0)
	{ // refering to object set
		Name objectSetName = &pObjectName[8];
		temp += "all_in "; temp += objectSetName;
	}
	else { temp += _objectNm;}
	
	return temp;
}


