//-----------------------------------------------------------------------
//                         SMIObjectSet  Class
// $Id
//                                                 B. Franek
//                                                  03-November-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include "smiobjectset.hxx"
#include "smiobject.hxx"
   extern int dbg;
//===========================================================================
SMIObjectSet::SMIObjectSet()
	: _name(), _objectList(), _clientWhens()
{
	return;
}
//===============================================================================
int SMIObjectSet::numOfObjects() const { return _objectList.numOfEntries();}

//============================ reset ========================================
void SMIObjectSet::reset() {
	_objectList.reset();
	return;	
}
//============================   name   =====================================

Name SMIObjectSet::name () const {
return _name;
}
//=========================== nextObject ====================================
int SMIObjectSet::nextObject(Name& name) {
        NmdPtnr tmpItem;
	
	int flg = _objectList.nextItem(tmpItem);
	name = tmpItem.name();
	return flg;
}
//=============================================================================
int SMIObjectSet::nextObject(Name& name, SMIObject*& ptnr)  {
	NmdPtnr tmpItem;
	
	int flg = _objectList.nextItem(tmpItem);
	name = tmpItem.name();
	ptnr = static_cast<SMIObject*>(tmpItem.pointer());

	return flg;
}

//------------------------------------------ BF  9-Apr-2008 -------------------
int SMIObjectSet::addClientWhen(const char* whenObjName, const char* whenStateName, int whenInx)
{
	return _clientWhens.markWhen(whenObjName, whenStateName, whenInx);
}
//------------------------------------------  Bf  11-Apr-2008  --------------
int SMIObjectSet::printClientWhens() const
{
	int numClientObjects =_clientWhens.numObjects();
	if ( numClientObjects == 0 )
	{
		cout << " No clients " << endl;
		return 1;
	}
	
	_clientWhens.out(" ");
	return 1;
}
//------------------------------------------  BF  09-May-2008  ----------------
int SMIObjectSet::informObjectsAboutMembership() 
{

//cout << endl 
//<< "start================== SMIObjectSet::informObjectsAboutMembership ========"
// << endl;
//cout << "  Set  " << _name << "   Objects : " << endl;
//Name temp(" "); _objectList.out(temp);
 	
	int numOfObjects = _objectList.numOfEntries();
	if ( numOfObjects <= 0 ) { return 1;}

	NmdPtnr currentSet(_name,this);
	NmdPtnr tmpItem;
	SMIObject* pObj;

/* Loop over the SMI objects and ads the current set to the list */
	_objectList.reset();
	while ( _objectList.nextItem(tmpItem) )
	{
		pObj = static_cast<SMIObject*>(tmpItem.pointer());
		pObj->_setsIBelongTo.add(currentSet);
		
//cout << " Object : " << pObj->name() << " is now member of : " << endl;
//Name temp(" "); pObj->_setsIBelongTo.out(temp);

	}
	return 1;
}
//------------------------------------------ BF  14-May-2008  -----------------
const ClientWhens& SMIObjectSet::gimeClientWhensRef() const
{
	return _clientWhens;
}
//=============================================================================
void SMIObjectSet::gimeObjectList(NmdPtnrList& objectList) const
{
	objectList = _objectList;
	
	return;
}
//============================================================================
void SMIObjectSet::mergeInObjectList(NmdPtnrList& outObjectList)
{
	NmdPtnr listItem;
	
	_objectList.reset();
	
	while (_objectList.nextItem(listItem))
	{
		outObjectList.add(listItem);
	}
	
	return;
}

