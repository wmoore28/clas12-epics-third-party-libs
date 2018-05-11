//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  July 2013
// Copyright Information:
//      Copyright (C) 1996-2013 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------

#include <stdlib.h>
#include "ut_tr.hxx"

#include "action.hxx"
#include "smiobject.hxx"
#include "paroperand.hxx"

#include "registrar.hxx"
#include "isofclassobject.hxx"

extern Registrar allObjects, allClasses;


//-----------------------------------------------------------------
SMIObject* gimeObjectPointer( Name& objName )
{
	void* pvoid;
	SMLUnit* pUnit;
	SMIObject* pointer;
	
	pvoid = allObjects.gimePointer(objName);
	if ( pvoid == NULL ) { return NULL;} // object does not exists

	pUnit = (SMLUnit*) pvoid;
	if ( pUnit->unitType() == "IsOfClassObject" )
	{
		Name className = ((IsOfClassObject*) pvoid)->isOfClass();
		pvoid = allClasses.gimePointer(className);
		if ( pvoid == NULL ) { return NULL;} // object's class does not exists
	}

	pointer = (SMIObject*) pvoid;

	return pointer;
}
