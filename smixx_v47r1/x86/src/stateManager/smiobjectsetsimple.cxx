//-----------------------------------------------------------------------
//                         SMIObjectSetSimple  Class
// $Id
//                                                 B. Franek
//                                                  03-November-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include "smiobjectsetsimple.hxx"
#include "smiobjectsetunion.hxx"
#include "smiobject.hxx"
#include "registrar.hxx"
#include "objectregistrar.hxx"
extern Registrar allSMIObjectSets;
extern ObjectRegistrar allSMIObjects;
#include "scheduler.hxx"
   extern Scheduler* pSchedulerGl;
   extern int dbg;
#include "ifhandler.hxx"
   extern IfHandler ifHandler;
#include "resumehandler.hxx"
   extern ResumeHandler resumeHandler;
//============================  SMIObjectSetSimple  ==================================
SMIObjectSetSimple::SMIObjectSetSimple()
	: SMIObjectSet()
{
	return;
}

SMIObjectSetSimple::SMIObjectSetSimple( char lines[][MAXRECL])
	: SMIObjectSet()
{
	_name = lines[1];
	
	if ( dbg > 1) {
        cout << " ObjectSet  ....................  " << _name << endl;
	}
	
	char offset[] =
	        "                                  ";

	allSMIObjectSets.add(_name,this);

	int classFlg,numOfObjects;
	sscanf(lines[2],"%d %d",&classFlg,&numOfObjects);

	int inext = 3;
	_isOfClass = "\0";
	if ( classFlg ) {
		_isOfClass = lines[inext];
		inext++;
	} 
	if (numOfObjects == 0) {
	    if ( dbg > 1 ) {
	        cout << offset << "is empty" << endl;
	    }
	}

	Name tempNm;

	for (int i = 0; i<numOfObjects; i++) {
		tempNm = lines[inext]; inext++;
		addObjectToSet(tempNm);
		if ( dbg > 1 ) {
		   cout << offset << tempNm.getString() << endl;
                }
	}


//        out("  ");
//	exit(0);
	return;
}
//------------------------------------------------------------------------
void SMIObjectSetSimple::copy(const SMIObjectSetSimple& smiObjectSet) {
	_name =       smiObjectSet._name;
	_isOfClass =  smiObjectSet._isOfClass;
	_objectList = smiObjectSet._objectList;
	_clientWhens= smiObjectSet._clientWhens;
	return;
}
//----------------------------------------------------------------------
SMIObjectSetSimple::SMIObjectSetSimple(const SMIObjectSetSimple& smiObjectSet)
	: SMIObjectSet()
{
	copy(smiObjectSet);
	return;
}
//----------------------------------------------------------------------
SMIObjectSetSimple& SMIObjectSetSimple::operator=(const SMIObjectSetSimple& smiObjectSet) {
	copy(smiObjectSet);
	return *this;
}
//===========================================================================
void SMIObjectSetSimple::removeAll() {

		
	int numOfObjects = _objectList.numOfEntries();
	if ( numOfObjects <= 0 ) { return;}   // the set is empty
	
// I will now loop over all the objects in the set and for each will
// call 'remove' method that does all that is necessary. For this purpose
// I have to copy the _objectList and use the copy for the loop. This is
// because 'remove' method modifies _objectList

	NmdPtnrList copyOfobjectList;
	copyOfobjectList = _objectList;

// Now loop over the SMI objects in 'copyOfobjectList' and for each of the objects
//   call 'remove'

	NmdPtnr tmpItem;
	Name objname;	

	copyOfobjectList.reset();
	
	while ( copyOfobjectList.nextItem(tmpItem) )
	{
		objname = tmpItem.name();
		remove(objname);
	}	

	return;
}
//===========================================================================
int SMIObjectSetSimple::add(const Name& objname) {

	SMIObject* pObj = allSMIObjects.gimePointer(objname);
	if ( pObj == 0 ) {
	    cout << " adding an object to a set" << endl;
	    cout << " Object " << objname << " is not declared" << endl;
	    exit(1);
	}
	NmdPtnr tempItem(objname,pObj);
	
	int flg = _objectList.add(tempItem);

// now add this set to the object's list
	pObj->youJoinedSet(this);
	
	pSchedulerGl->reqWhenExecution(_name);	
	
// now inform all the unions to which I belong so they can do what they have to
	int noOfUnions = _unionList.length();
	if ( noOfUnions == 0 ) { return flg; }
	
	int iu;
	Name unionNm; void* pvoid; SMIObjectSetUnion* pUni;
	
	for ( iu=0; iu<noOfUnions; iu++ )
	{
		unionNm = _unionList[iu];
		pvoid = allSMIObjectSets.gimePointer(unionNm);
		pUni = static_cast<SMIObjectSetUnion*>(pvoid);
		pUni->addedObjectToOneOfYourSets(objname,_name);
	}
	
	return flg;
}
//==========================================================================
int SMIObjectSetSimple::remove(const Name& objname) {
	NmdPtnr tmpItem(objname,0);
//debug beg
if ( dbg > 5 )
{
	cout << endl << endl;
	cout << " start================SMIObjectSetSimple::remove=======" << endl;
	cout << "    object  " << objname << "   Set  " << _name << endl;
}
//debug end
//
	resumeHandler.reportingObjectRemovedFromSet(objname,_name);

	int flg = _objectList.remove(tmpItem);
// now remove this set from the object's list
	SMIObject* pObj = allSMIObjects.gimePointer(objname);
	pObj->youLeftSet(this);
	
	pSchedulerGl->reqWhenExecution(_name);
	
// now inform all the unions to which I belong so they can do what they have to
	int noOfUnions = _unionList.length();
//debug beg
if ( dbg > 5 )
{
	cout << " No. of my unions  " << noOfUnions << endl;
	if ( noOfUnions == 0 )
	{
		cout << "return================SMIObjectSetSimple::remove=======" 
		<< endl << endl;
	}
}
//debug end	
	if ( noOfUnions == 0 ) { return flg; }
	
	int iu;
	Name unionNm; void* pvoid; SMIObjectSetUnion* pUni;
	
	for ( iu=0; iu<noOfUnions; iu++ )
	{
		unionNm = _unionList[iu];
		pvoid = allSMIObjectSets.gimePointer(unionNm);
		pUni = static_cast<SMIObjectSetUnion*>(pvoid);
		pUni->removedObjectFromOneOfYourSets(objname,_name);
	}
	
	return flg;
}
//============================  out ===========================================
void SMIObjectSetSimple::out(const char* offset) {

	cout << " Object Set  : " << _name << endl;

	if (_isOfClass == "\0") {}
	else {
		cout << offset;
		cout << "   can only have objects of class : " << _isOfClass << endl;
	}
	
	
	int numOfObjects =_objectList.numOfEntries();
	cout << " No of entries : " << numOfObjects << endl;

	
	Name objName; SMIObject* pObj;

	reset();
	while (nextObject(objName,pObj)) {
		cout << offset << objName << "   " << pObj << endl;
	}
		 
	return;
}
//===========================================================================
int SMIObjectSetSimple::addObjectToSet(const Name& objname) {

	SMIObject* pObj = allSMIObjects.gimePointer(objname);
	if ( pObj == 0 ) {
	    cout << " adding an object to a set" << endl;
	    cout << " Object " << objname << " is not declared" << endl;
	    exit(1);
	}
	NmdPtnr tempItem(objname,pObj);
	
	int flg = _objectList.add(tempItem);

// now add this set to the object's list
	pObj->youJoinedSet(this);
	return flg;
}
//============================================================================
int SMIObjectSetSimple::joinUnion(const Name& unionNm)
{

	if ( _unionList.isPresent(unionNm) ) { return 0; }
	
	_unionList += unionNm;

	return 1;
}
