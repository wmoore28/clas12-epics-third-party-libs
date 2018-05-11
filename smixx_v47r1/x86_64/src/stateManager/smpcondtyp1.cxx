//------------------------  SmpCondTyp1  Class ------------------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "name.hxx"
#include "namelist.hxx"
#include "smpcondtyp1.hxx"
#include "smiobject.hxx"
#include "ut_sm.hxx"
//----------------- Externals --------------------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern  Name smiDomain;
#include "registrar.hxx"
   extern Registrar allSMIObjectSets;
//------------------------------------------------
//                                                         B. Franek
//                                                         01-Aug-1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------

//-------------------------------------------------------------------------
SmpCondTyp1::SmpCondTyp1( char lines[][MAXRECL], int& no_lines) {
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line of the simple condition
// Output :
// no_lines ....... number of lines in the simple condition
//----------------------------------------------------------------------------
	_type = 1;
	_frozen = 0;

	int il;

	il = 0;
	
	il++;
	_objectName = lines[il];
	_refObjects += _objectName; 
	
	il++;

	if (!strcmp(lines[il],"in_state")) {
		_inState = 1;
	}
	else if (!strcmp(lines[il],"not_in_state")) {
		_inState = 0;
	}
	else {
		cout << " Error initialising Simple condition \n";
		cout.flush();
		exit(1);
	}

	il++;

	int noOfStates;
	sscanf(lines[il],"%d",&noOfStates);
	assert ( noOfStates > 0 );
    
	Name tmpName;
	
	for (int ist=0; ist < noOfStates; ist++) {
		il++;
		tmpName = lines[il];
		if ( tmpName == "&SMIDOMAIN" ) {
			tmpName = smiDomain;
		}
		_states += tmpName;
	}

	no_lines = il + 1;
	return ;

}
//--------------------------- whatAreYou --------------------------------------
Name SmpCondTyp1::whatAreYou() const {

  Name tmpString;
  
      tmpString += "( ";
      tmpString += _objectName;
      
      if ( _inState == 0) {
         tmpString += " not_in_state ";
      } else {
         tmpString += " in_state ";
      }

      Name stateString;
      makeStateString(_states,stateString);
      tmpString += stateString.getString(); 
      
      tmpString += " )";

      return tmpString;

}
//------------------------------  objectsToLock -------------------------------
void SmpCondTyp1::objectsToLock(NameList& list) const {

        list.removeAll();
	list.add(_objectName);

	return;
}
//------------------------------ freeze ------------------------------------
void SmpCondTyp1::freeze() {
	assert(_frozen==0);
	_frozen = 1; 

	return;
}
//------------------------------ unfreeze ---------------------------------
void SmpCondTyp1::unfreeze() {
	assert(_frozen==1);
	_frozen = 0;
	return;
}

//---------------------------- evaluate ----------------------------------
int SmpCondTyp1::evaluate()  {

	int flag;
	Name stateNm;

        int noOfStates = _states.length();
//  cout  << " _objectName " << _objectName ;
//  cout.flush();

	SMIObject *pSMIObj;
	pSMIObj = allSMIObjects.gimePointer(_objectName);
	assert(pSMIObj != 0);
	flag = pSMIObj->currentState(stateNm);

	if ( flag < 0 ) {
		return -1;
	}

	int found;
	found = 0;
	
	for ( int is = 0; is < noOfStates; is++ ) {
		if ( _states[is] == stateNm ) { found = 1; break; }
	}

	int retflag;
	if ( _inState == 1 ) {
		if ( found == 0 ) {  retflag = 0; }
		else              {  retflag = 1; }
	}
	else {
		if ( found == 0 ) {  retflag = 1; }
		else              {  retflag = 0; }
	}

	return retflag;
}
