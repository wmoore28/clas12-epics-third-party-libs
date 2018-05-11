//-----------------------------------------------------------------------
//                         SMIFrozenSet  Class
// $Id
//                                                 B. Franek
//                                                  25-November-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include "smifrozenset.hxx"
#include "smiobject.hxx"
   extern int dbg;

//============================  SMIFrozenSet  ==================================
SMIFrozenSet::SMIFrozenSet() 
	:  SMIObjectSet()
{
	_name = "&frozenSet";
	return;
}

//============================  out ===========================================
void SMIFrozenSet::out(const char* offset) {

	cout << " Frozen Object Set  : " << endl;

	int numOfObjects =_objectList.numOfEntries();
	
	cout << " Num of entries : " << numOfObjects << endl;
	
	Name objName; SMIObject* pObj;

	reset();
	while (nextObject(objName,pObj)) {
		cout << offset << objName << "   " << pObj << endl;
	}
		 
	return;
}

//=============================================================================
void SMIFrozenSet::copyObjectList( const SMIObjectSet& otherObjectSet )
{
	otherObjectSet.gimeObjectList(_objectList);
	return;
}
//==========================================================================
int SMIFrozenSet::remove(const Name& objname) {
	NmdPtnr tmpItem(objname,0);

	int flg = _objectList.remove(tmpItem);
	return flg;
}
