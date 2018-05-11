//------------------------  SmpCondTyp2  Class ------------------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "name.hxx"
#include "namelist.hxx"
#include "smpcondtyp2.hxx"
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "ut_sm.hxx"
#include "dic.hxx"
//----------------- Externals --------------------
   extern  Name smiDomain;
#include "registrar.hxx"
   extern Registrar allSMIObjectSets;
   extern int dbg;
//------------------------------------------------
//                                                         B. Franek
//                                                         01-Aug-1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
SmpCondTyp2::SmpCondTyp2 ( char lines[][MAXRECL], int& no_lines) {
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line of the simple condition
// Output :
// no_lines ....... number of lines in the simple condition
//----------------------------------------------------------------------------
	_type = 2;
	_frozen = 0;

	int il;

	il = 0;
	
	il++;
	_objectSet = lines[il];
	_refObjectSets += _objectSet;
		
	il++;
	if ( strcmp(lines[il],"all_in") == 0 ) {
		_all_in = 1;
	}
	else if ( strcmp(lines[il],"any_in") == 0 ) {
		_all_in = 0;
	}
	else {
		cout << "Looking for 'any_in' or 'all_in' but found : "
		     << lines[il] << endl; exit(1);
	}
	
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
Name SmpCondTyp2::whatAreYou() const {

  Name tmpString;
  
      tmpString += "( ";
      
      if (_all_in == 1) {
         tmpString += "all_in ";
      } else {
         tmpString += "any_in ";
      }
      
      tmpString += _objectSet;
      
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
void SmpCondTyp2::objectsToLock(NameList& list) const {

	
	SMIObjectSet* ptnSet;

	if (_frozen == 1) {
		ptnSet = (SMIObjectSet *)&_frozenSet;
	}
	else {
		void* ptnv = allSMIObjectSets.gimePointer(_objectSet);
//		cout << " ptnv " << ptnv << endl;
		
		if (ptnv == 0) { cout << " Set " << _objectSet <<
		         "not declared " << endl; exit(1);
		}
		ptnSet = (SMIObjectSet*) ptnv;
	}

	list.removeAll();

	ptnSet->reset();
	Name objinset;
	while (ptnSet->nextObject(objinset)) {
		list.add(objinset);
	}
	return;
}

//------------------------------ freeze ------------------------------------
void SmpCondTyp2::freeze() {
	assert(_frozen==0);
	_frozen = 1; 

	void* ptnv = allSMIObjectSets.gimePointer(_objectSet);
	if (ptnv == 0) {
		cout << " Set " << _objectSet 
	       	      << " not declared " << endl;
	 	abort();
	}
	SMIObjectSet* ptnSet = (SMIObjectSet*) ptnv;
	_frozenSet.copyObjectList(*ptnSet);
//debug beg
	if ( dbg > 5 )
	{
		cout << endl << " ==== SmpCondTyp2::freeze() =======" << endl
		<< "   Objects Set " << _objectSet << " frozen to :" << endl;
		_frozenSet.out(" "); cout << endl;		
	}
//debug end
	
	return;
}
//------------------------------ unfreeze ---------------------------------
void SmpCondTyp2::unfreeze() {
	assert(_frozen==1);
	_frozen = 0;
	return;
}


//---------------------------- evaluate ----------------------------------
int SmpCondTyp2::evaluate() {


        int noOfStates = _states.length();
//	cout << " Set name " << _objectSet << " andflg " << _all_in << endl;
	
	SMIObjectSet* ptnSet;

	if (_frozen == 1) {
		ptnSet = &_frozenSet;
	}
	else {
		void* ptnv = allSMIObjectSets.gimePointer(_objectSet);
//		cout << " ptnv " << ptnv << endl;
		
		if (ptnv == 0) { cout << " Set " << _objectSet <<
		         "not declared " << endl; exit(1);
		}
		ptnSet = (SMIObjectSet*) ptnv;
	}

	if (ptnSet->numOfObjects() == 0) {return -2;}

	Name objinset; int result;
	result = _all_in;

	SMIObject* pObj;

//       ptnSet->out(" ");
	dim_lock();
	ptnSet->reset();
//cout << endl << "eval_set started execution" << endl;		

	while (ptnSet->nextObject(objinset,pObj)) {
//		cout << objinset <<  "   " <<   pObj << endl;
		if (pObj == 0) {
			cout << " Object " << objinset
			<< " in set " << _objectSet 
			<< " does not exists" << endl;
			ptnSet->out(" ");
			abort();
		}
		Name stateName;
		if (pObj->currentState(stateName) < 0) {
			dim_unlock();
//cout << "eval stopped execution. Object busy" << endl << endl;
			return -1;
		}

		int inStateList = 0;
		for (int ist = 0; ist<noOfStates; ist++) {
			if ( _states[ist] == stateName ) {
				inStateList = 1;
				break;
			}
		}
		if (inStateList == _inState) {
			if (_all_in == 0) {result = 1;}
		}
		else {
			if (_all_in == 1) {result = 0;}
		}
//		cout << " objinset " << objinset 
//		<< " inStateList " << inStateList << " result "
//		 << result << endl;
	}
	dim_unlock();
	return result;
}
//-----------------------------------  BF Sep 2008  ------------------
void SmpCondTyp2::removeObjectFromFrozenObjectSet( const Name& objName, const Name& setName)
{
	assert (_frozen==1);
	if ( _objectSet == setName )
	{
		_frozenSet.remove(objName);
//debug beg
		if ( dbg > 5 )
		{
			cout << endl << " ==== SmpCondTyp2::removeObjectFromFrozenObjectSet =======" << endl
			<< " New frozen set of " << _objectSet << " :" << endl;
			_frozenSet.out(" "); cout << endl;		
		}
//debug end

	}
	
	return;
} 


