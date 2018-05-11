//--------------------------  W h e n  Class  -----------------------------------
#include "smixx_common.hxx"
#include <assert.h>
#include "when.hxx"
#include "smiobject.hxx"

extern ObjectRegistrar allSMIObjects;
extern Registrar allSMIObjectSets;
//                                                         24-September-1996
//                                                          B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
//---------------------------------------------------------------------------
//
When::When( char lines[][MAXRECL], int *wh_lines, SMIObject *pobj, State *pstat,int whenInx){

   _pParentObject = pobj;
   _pParentState  = pstat;
   
   _parentObjectName = _pParentObject->name();
   
   _parentStateName = pstat->stateName();
   _whenInx = whenInx;

   int il,numCondlines,no_dolines;

   il = 1;

//    cout << "when:" << "\n";
//    cout << lines[il] << "\n" << lines[il+1] << "\n"<< lines[il+2] << "\n";
   _pCondition = new Condition(&lines[il],&numCondlines,
                _pParentObject,_pParentState,0);
     il = il + numCondlines;


   _pDoIns = new DoIns(&lines[il],0,&no_dolines,_pParentObject,0);

	if ( _pDoIns->stay_in_state() ) { _stay_in_state_flag = 1; }
	else { _stay_in_state_flag = 0; }
	
   il = il + no_dolines;

   *wh_lines = il;
   

/*Name offset = " ";
cout << " Server objects list " << endl; _pCondition->_srvOList.out(offset);
cout << " Server object sets list " << endl;
_pCondition->_srvOSList.out(offset);*/
   
}


//------------------------------ whatAreYou ---------------------------------
void When::whatAreYou(){
   char condition[MAXRECL];
   cout << "\n";
   _pCondition->whatAreYou(MAXRECL,condition);
   cout  << "    when " << condition ;
   _pDoIns->whatAreYou();
   cout.flush();
}


//------------------------ forcedExecute ---------  B.F. May 2008  --------
int When::forcedExecute( ) const {

#ifdef DEBUG
char condition[MAXRECL];
_pCondition->whatAreYou(MAXRECL,condition);
cout  << "  forcedExecute " << condition ;  _pDoIns->whatAreYou(); cout << endl;
#endif

	int flag;
	flag = _pCondition->evaluate();  // ret 0-false; 1-true; -1 one of 
                                         // the smiobjects in transition
	if ( flag == 1 ) {
		_pDoIns->executeHp();
		return 1;
	}

	return 0;
}
//===========================================================================
int When::updateReferencedObjectsandSets() const
{
// loop over the referenced objects
// get the pointer to the list first
	NameVector* plist = &(_pCondition->_refObjects);
	
	int numRefObjects = plist->length();
	int i = 0; Name name; SMIObject* pRefObject;
	
	for ( i = 0; i<numRefObjects; i++ )
	{
		name = (*plist)[i];
		pRefObject = allSMIObjects.gimePointer(name);
		pRefObject->addClientWhen
		(_parentObjectName.getString(),_parentStateName.getString(),_whenInx);
	}

// now do the same for sets

	plist = &(_pCondition->_refObjectSets);
	
	int numRefObjectSets = plist->length();
	SMIObjectSet* pRefObjectSet;
	
	for ( i = 0; i<numRefObjectSets; i++ )
	{
		name = (*plist)[i];
		pRefObjectSet = (SMIObjectSet *)allSMIObjectSets.gimePointer(name);
		pRefObjectSet->addClientWhen
		(_parentObjectName.getString(),_parentStateName.getString(),_whenInx);
	}

	return 1;
}

//------------------------ specialWhen() --------------------------
bool When::specialWhen() const
{ 
	if ( _stay_in_state_flag == 1 ) { return true; }
	else { return false; }
}
