//----------------------  Class  ResumeHandler  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : January 2009
// Copyright Information:
//      Copyright (C) 1996-2009 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef RESUMEHANDLER_HH
#define RESUMEHANDLER_HH
#include "typedefs.hxx"
#include "name.hxx"
#include "nmdptnrlist.hxx"
#include "smiobject.hxx"
#include "queue_name.hxx"

class ResumeHandler
{
	public :
		ResumeHandler();

		void registerSuspendedObject(void* pObj);
		
/**   deregisters object and puts it on 'ready to resume Q' 
*/
		void objectReadyToResume( Name& objName );
      
/** It loops through it's list of suspended objects and 'reports' that object
     remObjName was removed from set 'setName' 
*/
		void reportingObjectRemovedFromSet
			( const Name& remObjName, const Name& setName);
			
/** will resume all the objects that are ready to resume
*/
		void resumeReadyToResumeObjects();

/** checks if there are any objects ready to resume execution
*/
		bool isResumeQueueEmpty();
		
		void out() ;



	private :   
//-------------------- private data  ----------------------------------
      
		NmdPtnrList _suspendedObjects;
		
		Queue_Name _readyToResumeObjects;
};

#endif
