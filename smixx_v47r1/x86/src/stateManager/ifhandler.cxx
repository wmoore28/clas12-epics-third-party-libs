//----------------------  Class  IfHandler  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 14 April 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "ifhandler.hxx"
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
#include "smiobject.hxx"
#include "parameters.hxx"
#include "ifins.hxx"
//---------------------------- Externals ----------
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   extern int dbg;
//-------------------------------------------------
//
//----------------------------- IfHandler ---------------------------------
IfHandler::IfHandler() {
    return;
}
//
//
//-----------------------------  isEmpty  ---------------------------------
//bool IfHandler::isEmpty() {
//   if ( _suspendedObjects.numOfEntries() > 0 ) { return 1;}
//   return 0;
//}
//
//
//-----------------------------  addSet  ------------------------------------
void IfHandler::addSet(Name& objName, Set_Name& tobeLocked) {

//    Name tempName = objName;
    Set_Name* pTobeLocked = &tobeLocked;
    NmdPtnr tempItem(objName,pTobeLocked);
    int flg = _suspendedObjects.add(tempItem);
    assert( flg == 1 );

//debug beg  
if ( dbg > 5 ) {
   cout << endl << " ====================== IfHandler::addSet ===========================" << endl
        << " Object " << objName << " is suspended " << endl
	<< " pointer to the list of objects to-be-locked :" << pTobeLocked << endl;
	pTobeLocked->out(); cout << endl;

    cout << endl << " Current suspended objects : " << endl;
    _suspendedObjects.out(" ");
   cout << " ===================================================" << endl;
}
//debug end
}

//----------------------------  objectLocked  -----------------------------
void IfHandler::objectLocked(Name& locked, Name& requester) {

// this is a temporary bodge untill the time when IF's will also be handled 
// by ResumeHandler class
	SMIObject* pRequester;
	pRequester = (SMIObject*)(allSMIObjects.gimePointer(requester));
	
	if ( pRequester->suspendedInsType() == "WAIT" ) { return;}
// end of bodge

   int flg;
   Set_Name* pTobeLocked;
   
   flg = findRequester(requester,pTobeLocked);
   assert (flg == 1);
//debug beg
if ( dbg > 5 ) {
   cout << endl << " ===================== IfHandler::objectLocked =========================" << endl
       << "  Object : " << locked << " is locked by " << requester
        << " and is removed from the " << requester << "\'s to-be-locked list" << endl;
}
//debug end

    flg = pTobeLocked->remove(locked);
    assert(flg==1);
    
//debug beg
if ( dbg > 5 ) {
    cout << " The new list is now : " << pTobeLocked << "  " ; pTobeLocked->out(); cout << endl;
    cout << " ====================================================================" << endl << endl;       
//debug end
}
}
//
//
//------------------------------  execute  ---------------------------------
//
void IfHandler::execute() {
   SMIObject* pSMIObjToResume; Name objToResume;
   int flg;

    flg = findFreeToResumeObject(objToResume);
    if ( flg == 0 ) { return;}

    pSMIObjToResume = allSMIObjects.gimePointer(objToResume);		     
      
//debug beg
if ( dbg > 5 ) {
   cout << endl << " ====================== ifHandler:execute() ===========================" << endl
        << " Object " << objToResume << " can resume execution ... all objects it was waiting for are locked " << endl;
}
//debug end  	     

    NmdPtnr tmpItem(objToResume,0);
    flg = _suspendedObjects.remove(tmpItem);
    assert(flg==1);
    
//debug beg
if ( dbg > 5 ) {
    cout << " After removing " << objToResume << " from the suspended object list, the new list is : " << endl;
    _suspendedObjects.out(" ");    
}
//debug end


   pSMIObjToResume->resume();
   return; 
}
//
//
//------------------------------  out  --------------------------------------
// 
void IfHandler::out() {
    _suspendedObjects.out(" ");
}
//======================================== findRequester =======================================
int IfHandler::findRequester(Name& requester, Set_Name*& pTobeLocked) {
    NmdPtnr val;
    
    int nObj = _suspendedObjects.numOfEntries();
    if ( nObj <= 0 ) { return 0;}
    
    _suspendedObjects.reset();
    
    while (_suspendedObjects.nextItem(val)) {
        if ( val.name() == requester ) {
	    pTobeLocked = static_cast<Set_Name*>(val.pointer());
	    return 1;
	}
    }
    
    return 0;
}
//======================================== findFreeToResumeObject ===========================================
int IfHandler::findFreeToResumeObject(Name& objName) {
    NmdPtnr val; Set_Name* pTobeLocked;
    
    int nObj = _suspendedObjects.numOfEntries();
    if ( nObj <= 0 ) { return 0;}
    
    _suspendedObjects.reset();
    
    while (_suspendedObjects.nextItem(val)) {
	pTobeLocked = static_cast<Set_Name*>(val.pointer());
	if ( pTobeLocked->isEmpty() ) {
	    objName = val.name();
	    return 1;
	}
    }
    
    return 0;
} 
//========================================  BF  Oct 2008  =====================
int IfHandler::removeObjectFromSet( const Name& objName, const Name& setName)
{
	int nObj;
	NmdPtnr item;
	Name suspObjName;
	SMIObject* pSuspObj;
	IfIns* pSuspIfIns;

	nObj = _suspendedObjects.numOfEntries();

//debug beg
if ( dbg > 5 )
{
	cout << endl << "start ==================== ifHandler::removeObjectFromSet =============" << endl;
	cout << "  no. of suspended objects : " << nObj << endl;
}
//debug end

	if ( nObj <= 0 ) { return 0;}

    
	_suspendedObjects.reset();
    
	while (_suspendedObjects.nextItem(item)) {
	    suspObjName = item.name();
	    pSuspObj = allSMIObjects.gimePointer(suspObjName);
	    pSuspIfIns = pSuspObj->gimePointerToTypeISuspendedIF();

//debug beg
if ( dbg > 5 )
{
	    cout << " susp. object " << suspObjName << " susp If pointer " << pSuspIfIns << endl;
}
//debug end

	    pSuspIfIns->removeObjectFromSet(objName,setName);
	}
	
//debug beg
if (dbg > 5)
{
	cout << "return ==================== ifHandler::removeObjectFromSet =============" << endl << endl;
}
	return 1;
}
