//----------------------  Class  ResumeHandler  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : January 2009
// Copyright Information:
//      Copyright (C) 1996-2009 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "resumehandler.hxx"
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
#include "smiobject.hxx"
#include "parameters.hxx"
//---------------------------- Externals ----------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern int dbg;
//-------------------------------------------------
//
//----------------------------- ResumeHandler ---------------------------------
ResumeHandler::ResumeHandler()
{
    return;
}

//-----------------------------  addSuspendedObject  ------------------------------------
void ResumeHandler::registerSuspendedObject(void* pObj)
{
	SMIObject* pSMIObj = (SMIObject*)pObj;
	Name objName = pSMIObj->name();
	NmdPtnr tempItem(objName,pObj);
	int flg = _suspendedObjects.add(tempItem);
	assert( flg == 1 );
//debug beg
if ( dbg > 5 )
{
	cout << endl << " start========== ResumeHandler::registerSuspendedObject " << endl;
	cout << objName << " suspended" << endl;
	cout << " return========== ResumeHandler::registerSuspendedObject " << endl;
}
//debug end	
	return;
}
//------------------------------ reportingObjectRemovedFromSet --------------
void ResumeHandler::reportingObjectRemovedFromSet
		( const Name& remObjName, const Name& setName)
{
	SMIObject* pSuspObj;
	Name suspObjName;
	int readyToResumeFlag;
	NmdPtnr item;
	
	int nObj = _suspendedObjects.numOfEntries();
//debug beg
if (dbg > 5 )
{
	cout << endl << endl;
	cout << "start================= ResumeHandler::reportingObjectRemovedFromSet ===========" << endl;
	cout << "      object " << remObjName 
	<< "   Set  " << setName << "  No.of susp. objects " << nObj
	<< endl ;
	if ( nObj <= 0 )
	{
		cout << "return================= ResumeHandler::reportingObjectRemovedFromSet ==========="
		 << endl << endl;
	}
}
//debug end
	if ( nObj <= 0 ) {return;}
	
	_suspendedObjects.reset();

	while (_suspendedObjects.nextItem(item)) {
		pSuspObj = (SMIObject*)(item.pointer());
		readyToResumeFlag = 
		pSuspObj->reportingObjectRemovedFromSet(remObjName,setName);
		
		if ( readyToResumeFlag == 1 )
		{ // object is ready to resume execution
	 	 // first remove it from the suspended object list
		 	suspObjName = item.name();
			int flg1 = _suspendedObjects.remove(suspObjName);
			assert(flg1==1);
		
	  	// resume its execution
			_readyToResumeObjects.add(suspObjName);
		}
	}

	return;
}
//------------------------------  out  --------------------------------------
// 
void ResumeHandler::out() {
    _suspendedObjects.out(" ");
}
//----------------------- resumeReadyToResumeObjects -----------------------
void ResumeHandler::resumeReadyToResumeObjects()
{
//debug beg
if ( dbg > 5 )
{
	if ( _readyToResumeObjects.isEmpty() )
	{
//		cout << endl << " =========== ResumeHandler ... no objects ready to resume" << endl << endl;
		return;
	}
	cout << endl << " start=========== ResumeHandler::resumeReadyToResumeObjects"
	<< endl;
}
//debug end

	Name objName;
	SMIObject* pSMIObj;
	
	dim_lock();
	
	while ( !(_readyToResumeObjects.isEmpty()) )
	{
		objName = _readyToResumeObjects.remove();
		pSMIObj = allSMIObjects.gimePointer(objName);
//debug beg
if ( dbg > 5 )
{
	cout << objName << "resuming execution" << endl;
}
//debug end
		pSMIObj->resume();
	}
	
	dim_unlock();
	
//debug beg
if ( dbg > 5 )
{
	cout << " return=========== ResumeHandler::resumeReadyToResumeObjects"
	<< endl << endl;
}
//debug end
	return;
}
//==========================================================================
bool ResumeHandler::isResumeQueueEmpty() 
{
	dim_lock();
	bool res = _readyToResumeObjects.isEmpty();
	dim_unlock();
	return res;
}
//============================================================================
void ResumeHandler::objectReadyToResume ( Name& objName )
{
//debug beg
if ( dbg > 5 )
{
	cout << endl << " start=========== ResumeHandler::objectReadyToResume("
	  << objName << ")" << endl;
}
//debug end
	dim_lock();
	// first remove it from the suspended object list
	int flg1 = _suspendedObjects.remove(objName);
	assert(flg1==1);
		
	  // put the object on the Ready-to-Resume-Objects
	_readyToResumeObjects.add(objName);

//debug beg
if ( dbg > 5 )
{
	cout << " Object " << objName << " put on Ready-To-Resume queue" <<
	endl;
}
//debug end
	dim_unlock();
	
//debug beg
if ( dbg > 5 )
{
	cout << " return=========== ResumeHandler::objectReadyToResume " << endl;
}
//debug end

	return;
}
