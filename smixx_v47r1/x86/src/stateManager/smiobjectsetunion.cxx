//-----------------------------------------------------------------------
//                         SMIObjectSetUnion  Class
// $Id
//                                                 B. Franek
//                                                  08-December-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include "nmdptnrlist.hxx"
#include "smiobjectsetunion.hxx"
#include "smiobjectsetsimple.hxx"
#include "smiobject.hxx"
#include "registrar.hxx"
#include "objectregistrar.hxx"
extern Registrar allSMIObjectSets;
extern ObjectRegistrar allSMIObjects;
extern int dbg;
#include "scheduler.hxx"
   extern Scheduler* pSchedulerGl;
#include "ifhandler.hxx"
   extern IfHandler ifHandler;
#include "resumehandler.hxx"
   extern ResumeHandler resumeHandler;
//========================================================================
SMIObjectSetUnion::SMIObjectSetUnion( char lines[][MAXRECL])
	: SMIObjectSet()
{
	_name = lines[1];
	
	if ( dbg > 1) {
        cout << " ObjectSetUnion  ....................  " << _name << endl;
	}
	
	char offset[] =
	        "                                  ";

	allSMIObjectSets.add(_name,this);

	int numOfSets;
	sscanf(lines[2],"%d",&numOfSets);

	int inext = 3;

	if (numOfSets == 0)
	{
		if ( dbg > 1 ) 
		{
			cout << offset << "is empty" << endl;
		}
	}

	Name tempNm; void *pvoid; SMIObjectSetSimple *pSetSimple;

	for (int i = 0; i<numOfSets; i++) {
		tempNm = lines[inext]; inext++;
		_objectSets += tempNm;
		if ( dbg > 1 ) {
			cout << offset << tempNm.getString() << endl;
		}
		
		// check if the set exists
		pvoid = allSMIObjectSets.gimePointer(tempNm);
		if (!pvoid)
		{
			cout << "****Error  Object Set " << tempNm
			 << " is not declared " << endl;
			 exit(1);
		}
		// inform the set about this membership
		pSetSimple = static_cast<SMIObjectSetSimple*>(pvoid);
		pSetSimple->joinUnion(_name);
	}


	updateObjectList();
	
	informObjectsAboutMembership();

//        out("  "); cout << endl;
//	exit(0);
	return;
}
//============================  out ===========================================
void SMIObjectSetUnion::out(const char* offset) {

	
	int numOfSets =_objectSets.length();
	cout << offset << "beg=====  Object Set Union : " << _name 
	<< " No of Sets : " << numOfSets << endl;

	cout << offset  << " No of sets : " << numOfSets << endl;

	int i;

	for ( i=0; i<numOfSets; i++)
	{
		cout << offset << _objectSets[i] << endl;
	}
	
	int numOfObjects = _objectList.numOfEntries();
	cout << endl << offset << " No of objects : " << numOfObjects << endl;
	
	NmdPtnr listItem; Name objName;
	
	_objectList.reset();
	
	while ( _objectList.nextItem(listItem) )
	{
		objName = listItem.name();
		cout << offset << objName << endl;
	}

	cout << offset << "end=====  Object Set Union : " << _name << endl; 
	
	return;
}
//============================================================================
void SMIObjectSetUnion::updateObjectList()
{
	int numOfSets =_objectSets.length();
	if (numOfSets == 0) {return;}

	void* pvoid; SMIObjectSetSimple* pSimpleSet;
	
	_objectList.removeAll();
	
	for ( int i=0; i<numOfSets; i++)
	{
	pvoid = allSMIObjectSets.gimePointer(_objectSets[i]);
	pSimpleSet = static_cast<SMIObjectSetSimple*>(pvoid);
	
	pSimpleSet->mergeInObjectList(_objectList);	
	}
		
	return;
}
//=============================================================================
void SMIObjectSetUnion::addedObjectToOneOfYourSets(const Name& objectNm, const Name& setNm)
{

// First check that this is my set

	int numOfSets =_objectSets.length();
	
	int found = 0;
	
	for ( int i=0; i<numOfSets; i++)
	{
		if ( _objectSets[i] == setNm ) { found = 1; }
	}
	
	if ( found == 0 ) {
		cout << " **** Fatal error: Set Union " << _name
		 << " does not have set " << setNm << endl;
		 exit(1);
	}
	
	SMIObject* pObj = allSMIObjects.gimePointer(objectNm);
	NmdPtnr tempItem(objectNm,pObj);
	int flg = _objectList.add(tempItem);
	
	if ( flg == 0 )
	{
	// object was already there, so it must be a member of one or more other sets
	// do nothing
		return;
	}
	
	// add this set to the object's list
	pObj->youJoinedSet(this);
	
	pSchedulerGl->reqWhenExecution(_name);
	return;
}
//=============================================================================
void SMIObjectSetUnion::removedObjectFromOneOfYourSets(const Name& objectNm, const Name& setNm)
{
	int numOfSets =_objectSets.length();

//debug beg
if ( dbg > 5 )
{
	cout << endl << endl
	<< "start====================removedObjectFromOneOfYourSets==================" << endl
	<< "   Set union  " << _name << "  has " << numOfSets << "  simple sets" << endl
	<< "      object " << objectNm << "  removed from  " << setNm << endl;
}
//debug end
	
	SMIObject* pObj = allSMIObjects.gimePointer(objectNm);

// First check that this is my set
	
	int found = 0; int belongsAlsoToAnotherSet = 0; 
	bool belongsToCurrentSet;
	
	for ( int i=0; i<numOfSets; i++)
	{
		belongsToCurrentSet = pObj->memberOfSet(_objectSets[i]);

//debug beg
if ( dbg > 5 )
{
	cout << "       Set  " << _objectSets[i] << "   object  " << objectNm
	<< "  is member  " << belongsToCurrentSet << endl;
}
//debug end
		
		if ( _objectSets[i] == setNm )
		{
			found = 1;
			// check, that it was indeed removed
			if (belongsToCurrentSet)
			{
				cout << endl 
				<< " ***** Fatal Error:  SMIObjectSetUnion::removedObjectFromOneOfYourSets"
				<< endl << "   object " << objectNm << " is still member of set " 
				<< setNm << endl;
				exit(1);
			}
		}
		else
		{
			if (belongsToCurrentSet) { belongsAlsoToAnotherSet = 1; }
		}
			
	}
	
//debug beg
if ( dbg > 5 )
{
	cout  << "   belongsAlsoToAnotherSet  " << belongsAlsoToAnotherSet << endl; 
}
//debug end
		
	if ( found == 0 ) {
		cout << " **** Fatal error: SMIObjectSetUnion::removedObjectFromOneOfYourSets "
		<< endl << "   Set Union " << _name
		 << " does not have set " << setNm << endl;
		 exit(1);
	}

	if ( belongsAlsoToAnotherSet ) {
	// the object belongs to more than one set in the union....do nothing
		return;
	}
	
	// the object was removed from the union, take action

	resumeHandler.reportingObjectRemovedFromSet(objectNm,_name);
	
	NmdPtnr tempItem(objectNm,0);
	int flg = _objectList.remove(tempItem);
	
	if ( flg == 0 ) {}  // this removes compiler warning

	pObj->youLeftSet(this);
	
	pSchedulerGl->reqWhenExecution(_name);
	return;
}
