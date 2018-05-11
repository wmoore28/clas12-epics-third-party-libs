//------------------------  SmpCondTyp3  Class ------------------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "name.hxx"
#include "namelist.hxx"
#include "smpcondtyp3.hxx"
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "ut_sm.hxx"
#include "dic.hxx"
//----------------- Externals --------------------
   extern  Name smiDomain;
#include "registrar.hxx"
   extern Registrar allSMIObjectSets;
//------------------------------------------------
//                                                         B. Franek
//                                                         Aug-2009
// Copyright Information:
//      Copyright (C) 1996-2009 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
SmpCondTyp3::SmpCondTyp3 ( char lines[][MAXRECL], int& no_lines) {
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line of the simple condition
// Output :
// no_lines ....... number of lines in the simple condition
//----------------------------------------------------------------------------
	_type = 3;
	_frozen = 0;

	int il;

	il = 0;
	
	il++;
	_objectSet = lines[il];
	_refObjectSets += _objectSet;
		
	il++;
	if ( strcmp(lines[il],"empty") == 0 ) {
		_empty = 1;
	}
	else if ( strcmp(lines[il],"not_empty") == 0 ) {
		_empty = 0;
	}
	else {
		cout << "Looking for 'empty' or 'not_emoty' but found : "
		     << lines[il] << endl; exit(1);
	}

	no_lines = il + 1;
	return ;

}
//--------------------------- whatAreYou --------------------------------------
Name SmpCondTyp3::whatAreYou() const {

  Name tmpString;
  
      tmpString += "( ";
      
      tmpString += _objectSet;
      
      if (_empty == 1) {
         tmpString += "empty ";
      } else {
         tmpString += "not_empty ";
      }
      
      tmpString += ")";

      return tmpString;

}
//------------------------------  objectsToLock -------------------------------
void SmpCondTyp3::objectsToLock(NameList& list) const {

	list.removeAll();
	return;   // no objects to be locked
}

//------------------------------ freeze ------------------------------------
void SmpCondTyp3::freeze() {
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
	
	return;
}
//------------------------------ unfreeze ---------------------------------
void SmpCondTyp3::unfreeze() {
	assert(_frozen==1);
	_frozen = 0;
	return;
}


//---------------------------- evaluate ----------------------------------
int SmpCondTyp3::evaluate() {


//	cout << " Set name " << _objectSet << " empty flg " << _empty << endl;
	
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

	if (ptnSet->numOfObjects() == 0)
	{  // object set is empty
		if ( _empty == 1 )
		{ return 1; }
		else
		{ return 0; }
	}
	else
	{  // the set is not empty
		if ( _empty == 1 )
		{ return 0; }
		else
		{ return 1; }
	}

}
//-----------------------------------  BF Aug 2009  ------------------
void SmpCondTyp3::removeObjectFromFrozenObjectSet( const Name& objName, const Name& setName)
{
	assert (_frozen==1);
	if ( _objectSet == setName )
	{
		_frozenSet.remove(objName);	
	}
	
	return;
} 


