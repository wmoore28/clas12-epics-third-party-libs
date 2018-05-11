//------------------------  Class   D o I n s  -----------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>
#include "ut_sm.hxx"
//-------------------- Externals -----------------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern  Name smiDomain;
#include "registrar.hxx"
   extern Registrar allSMIObjectSets;
   extern int dbg;
//-------------------------------------------------
#include "doins.hxx"
#include "smiobjectset.hxx"
#include "smiobject.hxx"
#include "parms.hxx"
//                                                      Date : 5-May-1996
//                                                     Author: Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------- DoIns -------------------------------------
//
DoIns::DoIns
( char lines[][MAXRECL], int lev, int *no_lines, 
  SMIObject *pobj, State *pstat, Action* pact){
   char tempstr[MAXRECL];
   int lin;
   
	if ( pstat == NULL ) {}  // will remove compiler warning
	
   level = lev;

   _pParentObject = pobj;

   _pParentAction = pact;
//    cout << " do init _pParentAction : " << _pParentAction << "\n";

   sscanf(lines[1],"%s",tempstr);
   _actionNm = tempstr;

   sscanf(lines[2],"%d",&_numOfPara);

   lin = 3;

   Name paraNm, paraVal;

   if ( _numOfPara > 0) {
      for(int ipar=0; ipar<_numOfPara; ipar++) {
         strcpy(tempstr,lines[lin]);
//         sscanf(lines[lin],"%s",tempstr);
         paraNm = tempstr;
         lin++;
	 strcpy(tempstr,lines[lin]);
//         sscanf(lines[lin],"%s",tempstr);
         if ( !strcmp(tempstr,"&SMIDOMAIN") ) {
            paraVal = "\"";
            paraVal += smiDomain;
            paraVal += "\"";
         }
         else {
            paraVal = tempstr;
         }
         lin++;
         _doInsParameters.add(paraNm,paraVal);
      }
   }

   sscanf(lines[lin],"%s",tempstr);
   _objectNm = tempstr;
   *no_lines = lin + 1;
   return;
}



//---------------------------- whatAreYou ---------------------------------

void DoIns::whatAreYou(){
   indent(2*level+5);

// Format: do RO_SETUP ("/RO_FILE="SMI_LES_RO_FILE) LES

   cout << "do " << _actionNm ;

   if (_numOfPara > 0 ) {
      cout << " (";
      for (int ip=0; ip<_numOfPara; ip++) {
//         cout << _para[ip] << _value[ip];
//         if ( ip<(_numOfPara-1) ){cout << ",";}
      }
      cout << ")";
   }

   cout << "  " << _objectNm  << "\n";
}


//----------------------------  execute  ---------------------------------------
//
//
int DoIns::execute( Name& endState ){

//debug beg
if ( dbg > 3 )
{
	cout << " executing   do " << _actionNm <<  "  "  << _objectNm << endl;
}
//debug end

   Name actionString = _actionNm;  
   Parms outPars;
//
   if (_numOfPara > 0) {
      Name name,val;

      outPars = _doInsParameters;  // copy do instruction pars to outgoing pars
      Parms* pCurPars = _pParentAction->pCurrentParameters();

//    cout << " Do parameters :" << endl;         outPars.out();
//    cout << " Incomming parameters :" << endl;  pCurPars->out();

      outPars.replaceNameValues( *pCurPars );

//    cout << " Do parameters after action names replacement:" << endl; outPars.out();

      int npom =
          outPars.replaceNameValues( *_pParentObject->pObjectParameters() );

//    cout << " Do parameters after object names replacement:" << endl; outPars.out();

      if (npom == 0) {
         cout << " Not all the names replaced...fatal error" << endl;
         cout.flush();
         abort();
      }

      outPars.guessTypes(); 

//    cout << " Do parameters after guessing value types:" << endl; outPars.out();

      if( !outPars.allHaveValues() ) {
         cout << " Not all parameters have values...fatal error"<< endl;
         cout.flush();
         abort();
      }
      Name tempParString;
      outPars.buildParmString(tempParString) ;

      actionString += tempParString;

//    cout << " Parameter string : " << tempParString << endl;
      cout.flush();
   }
//---------------------------------------------------------------------------

//   cout << " Action string : " << actionString << endl;
//   cout.flush();

	SMIObject* pSMIObj;
	char* pObjectName = _objectNm.getString();

	if (strncmp(pObjectName,"&ALL_IN_",8) == 0) {
		Name setName = &pObjectName[8];
		void* ptnv = allSMIObjectSets.gimePointer(setName);
		if (ptnv == 0) { cout << " Set " << setName
			              << "  not declared " << endl; exit(1);}
		SMIObjectSet* ptnSet = (SMIObjectSet*) ptnv;
		ptnSet->reset();
		Name objectName;
		while (ptnSet->nextObject(objectName)) {
			pSMIObj = allSMIObjects.gimePointer(objectName);
			if ( pSMIObj == 0 ) {
			cout << " Object " << objectName 
			<< " in set " << setName << " does not exists" << endl;
			exit(1);
			}
			pSMIObj->queueAction(actionString);
		}
	}

	else if (strncmp(pObjectName,"&VAL_OF_",8) == 0) {
		Parms* pCurPars = _pParentAction->pCurrentParameters();
		Name parName = &pObjectName[8]; Name parVal; char type[10];
//		pCurPars->out(); 
		if(!pCurPars  || !pCurPars->get(parName,parVal,type)) {
			cout << " No value for parameter " << parName << endl;
		}
		if ( dbg > 2 ) {
        	    cout << " Parameter " << parName 
                    << " has value " << parVal << endl;
		}
		char* parValchr = parVal.getString(); 
		parValchr[strlen(parValchr)-1] = '\0';
		Name objNm = &parValchr[1];
		pSMIObj = allSMIObjects.gimePointer(objNm);
		if (!pSMIObj) {
			cout << " Object " << objNm 
			<< " does not exists" << endl;
		}
		else {
			pSMIObj->queueAction(actionString);
		}		
	}
	else {
		pSMIObj = allSMIObjects.gimePointer(_objectNm);
		pSMIObj->queueAction(actionString);
	}



   endState = "not changed";
   return 0;         // do instruction allways finishes
}


//-------------------------  executeHp  ---------------------------------------
//
//
void DoIns::executeHp(){

    if ( _actionNm == "&NULL" ) { return; }
    _pParentObject->queueHpAction(_actionNm);

return ;
}

//------------------------  stay_in_state  ---------------------------------
bool DoIns::stay_in_state() const
{
	if ( _actionNm == "&NULL" ) { return true; }
	else { return false; }
}
