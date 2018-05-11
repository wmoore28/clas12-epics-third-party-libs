//-----------------  Class   C r e a t e O b j e c t I n s  -------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>

//-------------------- Externals -----------------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern  Name smiDomain;
#include "registrar.hxx"
   extern Registrar allSMIClasses;
   extern int dbg;

#include "commhandler.hxx"   
   extern CommHandler *pCommHandlerGl;
   
#include "queue_twonames.hxx"
   extern Queue_TwoNames *pStateQGl;
//-------------------------------------------------
#include "utilities.hxx"
#include "createobjectins.hxx"
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "smiobjectsetsimple.hxx"
#include "parms.hxx"

typedef char OBJLINE[MAXRECL];
//                                                      Date :   May 2010
//                                                     Author:  Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2010 CCLRC. All Rights Reserved.
//---------------------------- CreateObjectIns -------------------------------------
//

CreateObjectIns::CreateObjectIns
       ( char lines[][MAXRECL], const int lev, int& no_lines,
         SMIObject* pobj, State *pstat, Action* pact){

	if ( pstat == NULL ) { } // this will remove compiler warning
	
	_level = lev;

	_pParentObject = pobj;

	_pParentAction = pact;

	
	_objectIdentifier = lines[1];
	
	_className = lines[3];

	no_lines = 4;

	for (int i=0; i<no_lines; i++)
	{
		_sobjCode+= lines[i];
	}
	return;
}


//---------------------------- whatAreYou ---------------------------------

void CreateObjectIns::whatAreYou(){
	cout << "***warning  'CreateObjectIns::whatAreYou()' not implemented" << endl;
	return;
}


//----------------------------  execute  ---------------------------------------
int CreateObjectIns::execute( Name& endState ){

	endState = "not changed";
	
//debug beg
if( dbg > 5 )
{
	cout << endl << "start ======================= CreateObjectIns::execute ===============";
}
//debug end


//debug beg
if ( dbg > 4 )
{
	Name offset = "     "; char* pOffset = offset.getString();
	cout << endl << pOffset;
	cout << "create_object " << _objectIdentifier << " of_class " << _className << endl;
}
//debug end
	
	char* pObjectIdentifier = _objectIdentifier.getString();

	SMIObject* pSMIObj;
	Name objNm;
	Name parName;

	if (strncmp(pObjectIdentifier,"&VAL_OF_",8) == 0) {
	// Now pick up the value of the action parameter
		Parms* pCurPars = _pParentAction->pCurrentParameters();
		parName = &pObjectIdentifier[8]; Name parVal; char type[10];
//		pCurPars->out();
		 
		if(!pCurPars->get(parName,parVal,type)) {
			cout << "*** Warning : Executing ''create_object'' instruction" << endl
			     << "              No value for parameter " << parName
			     << "  supplied, object not created" << endl;
			return 0;
		}
		else
		{
			if ( dbg > 2 ) {
                        	cout << " -- Executing ''create_object'' instruction " << endl
				     << "          Parameter " << parName 
				<< " has value " << parVal << endl;
			}
			char* parValchr = parVal.getString(); 
			parValchr[strlen(parValchr)-1] = '\0';  // remove the double quotes
			objNm = &parValchr[1];
		}
	}
	else {
		objNm = _objectIdentifier;
	}
	
	objNm.upCase(); objNm.trim();
	if (!check_name(objNm) || objNm.length() <= 1)
	{
		cout << "*** Warning :  Executing ''create_object'' instruction" << endl
		     << "                 the value of parameter " << parName 
		     << " is not a name, object not created" << endl;
		return 0;
	}

// First check that the object of this name is not already there

	pSMIObj = allSMIObjects.gimePointer(objNm);
	if (pSMIObj)
	{
		cout << "*** Warning :  Executing ''create_object'' instruction" << endl
		     << "               Object " << objNm 
		     << " already exists, the instruction is ignored" << endl;
		return 0;
	}

// ---------- Now create the object ----------------------
// the object code of 'create_object' instruction has the same format as that of
// 'object : <object-name> is_of_class <class-name>' declaration. It means that I
// can use the standard SMIObject constructor. The only thing that has to be done is 
// inserting the actual object name in the line no 1. Replacing the line 0
// is not necessary because the constructor does not check it.

	int no_code_lines = _sobjCode.length();
	char* pCode = new char[no_code_lines*MAXRECL];
	OBJLINE* code_lines = (OBJLINE*) pCode; Name lineNm;
//cout << "  no_code_lines " << no_code_lines << endl;	
	for (int il=0; il<no_code_lines; il++)
	{
		if (il == 1)
		{
			strcpy(code_lines[il],&objNm[0]);
		}
		else
		{
			lineNm = _sobjCode[il];
			strcpy(code_lines[il],&lineNm[0]);
		}
//cout << code_lines[il] << endl;
	}
		
	pSMIObj = new SMIObject(&code_lines[0]);
	assert (pSMIObj != 0);
	delete [] pCode;
//---------------  object created ----------------
	
	pCommHandlerGl->declareObj(pSMIObj);      
	pCommHandlerGl->addObjToDynObjList(pSMIObj);
	
	pSMIObj->startUp(pStateQGl,pCommHandlerGl);
	

	return 0;         // create_object instruction allways finishes
}


