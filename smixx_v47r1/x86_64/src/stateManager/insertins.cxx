//------------------------  Class   I n s e r t I n s  -----------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>

//-------------------- Externals -----------------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern  Name smiDomain;
#include "registrar.hxx"
   extern Registrar allSMIObjectSets;
   extern int dbg;
//-------------------------------------------------
#include "utilities.hxx"
#include "insertins.hxx"
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "smiobjectsetsimple.hxx"
#include "parms.hxx"
//                                                      Date :  3 July 2001
//                                                     Author:  Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2003 CCLRC. All Rights Reserved.
//---------------------------- InsertIns -------------------------------------
//

InsertIns::InsertIns
       ( char lines[][MAXRECL], const int lev, int& no_lines,
         SMIObject* pobj, State *pstat, Action* pact){

	if ( pstat == NULL ) {}  // will remove compiler warning
	
	_level = lev;

	_pParentObject = pobj;

	_pParentAction = pact;

	sscanf(lines[1],"%d",&_insertRemove);
	
	_objectNm = lines[2];
	_setNm = lines[3];

	no_lines = 4;
	return;
}


//---------------------------- whatAreYou ---------------------------------

void InsertIns::whatAreYou(){
	return;
}


//----------------------------  execute  ---------------------------------------
int InsertIns::execute( Name& endState ){

	endState = "not changed";
	
//debug beg
if( dbg > 5 )
{
	cout << endl << "start ======================= InsertIns::execute ===============";
}
//debug end
//debug beg
if ( dbg > 4 )
{
	Name offset = "     "; char* pOffset = offset.getString();
	cout << endl << pOffset;
	if (_insertRemove) {cout << "insert " << _objectNm << " in " << _setNm << endl;}
	else               {cout << "remove " << _objectNm << " from " << _setNm << endl;}
}
//debug end
	
	void* ptnv = allSMIObjectSets.gimePointer(_setNm);
	if (ptnv == 0) { cout << " Set " << _setNm
			      << " not declared " << endl; exit(1);}
	SMIObjectSetSimple* ptnSet = (SMIObjectSetSimple*) ptnv;
//	cout << " Set before : " << endl; ptnSet->out(offset);

	char* pObjectName = _objectNm.getString();

	SMIObject* pSMIObj;
	Name objNm; Name parName;

	if (strncmp(pObjectName,"&ALL",4) == 0 && _insertRemove == 0) {
		ptnSet->removeAll();
		return 0;
	}

	if (strncmp(pObjectName,"&VAL_OF_",8) == 0) {
		Parms* pCurPars = _pParentAction->pCurrentParameters();
		parName = &pObjectName[8]; Name parVal; char type[10];
//		pCurPars->out();
		objNm = "\0"; 
		if(!pCurPars  || !pCurPars->get(parName,parVal,type)) {
			cout << " No value for parameter " << parName << endl;
		}
		else {
		    if ( dbg > 2 ) {
                        cout << " Parameter " << parName 
                        << " has value " << parVal << endl;
		    }
			char* parValchr = parVal.getString(); 
			parValchr[strlen(parValchr)-1] = '\0';
			objNm = &parValchr[1];
		}
	}
	else {
		objNm = _objectNm;
	}
	
	objNm.upCase(); objNm.trim();
	if (!check_name(objNm) || objNm.length() <= 1)
	{
		cout << "*** Warning :  Executing ''insert/remove'' instruction" << endl
		     << "                 the value of parameter " << parName 
		     << " is not a name, the instruction is ignored" << endl;
		return 0;
	}

	pSMIObj = allSMIObjects.gimePointer(objNm);
	if (!pSMIObj) {
		cout << " Object " << objNm 
			<< " does not exists" << endl;
	}
	else {
		if (_insertRemove) {ptnSet->add(objNm);}
        	else               
		{
		
//debug beg
if ( dbg > 4 )
{
		cout << " removing " << objNm << " from " << _setNm << endl;
}
//debug end
			ptnSet->remove(objNm);
		}
	}
//debug beg
if ( dbg > 5 )
{
	cout << endl << " New " << _setNm << " set : " << endl; ptnSet->out("      "); cout << endl;
	cout << "return ======================= InsertIns::execute ===============";
	cout << endl << endl;
}
//debug end
	return 0;         // insert instruction allways finishes
}


