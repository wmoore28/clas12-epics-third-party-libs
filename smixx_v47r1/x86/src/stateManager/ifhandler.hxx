//----------------------  Class  IfHandler  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 14 April 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef IFHANDLER_HH
#define IFHANDLER_HH
#include "typedefs.hxx"
#include "name.hxx"
#include "set_name.hxx"
#include "nmdptnrlist.hxx"

class IfHandler {
   public :
      IfHandler();
//
//     bool isEmpty();
//
      void addSet(Name& objName, Set_Name& tobeLocked);
//
      void objectLocked(Name& locked, Name& requester);
//
      void execute();
//
      void out() ;
      
//
/** It loops through it's list of suspended objects and 'removes' object
     objName from set 'setName' in the relevant IF's
*/
	int removeObjectFromSet( const Name& objName, const Name& setName);

//
   private :
   
      int findRequester(Name& requester, Set_Name*& pTobeLocked) ;

      int findFreeToResumeObject(Name& objName) ;      
      
      NmdPtnrList _suspendedObjects;
};

#endif
