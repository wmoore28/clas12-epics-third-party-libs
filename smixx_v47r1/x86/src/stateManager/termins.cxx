//---------------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  May 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ut_sm.hxx"
#include "termins.hxx"
#include "name.hxx"
#include "parms.hxx"
#include "smiobject.hxx"
#include "state.hxx"
#include "action.hxx"
#include "commhandler.hxx"

extern int dbg;
extern Name allocator;
extern CommHandler *pCommHandlerGl;

TermIns::TermIns( char lines[][MAXRECL], const int lev, 
           SMIObject* pObj, State* pState, Action* pAction, int& no_lines) {
   _level = lev;

    _pParentObject = pObj;

   _pParentState = pState;

   _pParentAction = pAction;

   _endState = lines[0];
//   cout <<  " state : " << _endState << " \n";

   no_lines = 1;
   return;
}



void TermIns::whatAreYou(){
indent(_level*2+5);
cout << "terminate_action / state = " << _endState <<"\n";
cout.flush();
}

//-------------------------------  execute  -----------------------------------

int TermIns::execute( Name& endState ){

   if ( _pParentObject->name() == "&ALLOC" ) {

      Parms* parms = _pParentAction->pCurrentParameters();

/*
         cout << " Current parameters : " << endl;
         parms->out();
         cout.flush();
*/
      Name doubleq("\""),nullstr("\0");

      Name nameid("ID"),valid; char type[7];
      Name nameAmpersId("&ID"),valAmpersId;
      Name nameAmpersConnId("&CONN_ID"),valAmpersConnId;
      char connId[257];
      int intConnId;

      int idPresent,ampersIdPresent,ampersConnIdPresent;
      idPresent = parms->get(nameid,valid,type);
      ampersIdPresent = parms->get(nameAmpersId,valAmpersId,type);
      ampersConnIdPresent = 
             parms->get(nameAmpersConnId,valAmpersConnId,type);

      valAmpersConnId.whatAreYou(connId,256);
      sscanf(connId,"%d",&intConnId);


      Name temp("&VARIABLE");

      if ( temp == _endState ) {  // this is allocate action...should check on that 
         State* ptgState = _pParentObject->pTaggedState(temp);  
         assert (ptgState != 0);
         
         if ( idPresent ) {
            valid.replace(doubleq,nullstr);   // get rid of the double quotes
            ptgState->changeName(valid);
            endState = valid;
         }
         else {
            valAmpersId.replace(doubleq,nullstr);   // get rid of the double quotes
            ptgState->changeName(valAmpersId);
            endState = valAmpersId;
         }
         valAmpersId.replace(doubleq,nullstr);   // get rid of the double quotes
         allocator = valAmpersId;

         pCommHandlerGl->domainAllocated(intConnId);
      }
      else {  // this must be RELEASE action
         endState = _endState;
         allocator = "\0";

         pCommHandlerGl->domainDeallocated(intConnId);
      }
   }
   else {
       endState = _endState; 
   }

//debug beg
if ( dbg > 3)
{
	cout << _pParentObject->name()  << " executing move_to " << endState << endl;
}
//debug end

return 1;         // terminate_action allways terminates
}

