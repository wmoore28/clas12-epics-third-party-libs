//--------------------------  Scheduler Class  -------------------------------
#include "smixx_common.hxx"
#include <assert.h>
#include "ut_sm.hxx"
//----------- Externals ------------------
#include "queue_name.hxx"
   extern Queue_Name executableObjectQ;
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
#include "registrar.hxx"
   extern Registrar allSMIObjectSets;
#include "ifhandler.hxx"
   extern IfHandler ifHandler;
#include "resumehandler.hxx"
   extern ResumeHandler resumeHandler;
   extern Name allocator;
   extern int dbg;
//---------------------------------------
#include "scheduler.hxx"
#include "smiobject.hxx"
#include "queue_twonames.hxx"
#include "msg.hxx"
//                                                         8-November-1996
//                                                          B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
Scheduler::Scheduler( 
                      CommHandler *pCommHandler,
                      Queue_TwoNames *pStateQ,
                      Queue_TwoNames *pExternalActionQ ) {
    _pCommHandler = pCommHandler;
    _pStateQ = pStateQ;
    _pExternalActionQ = pExternalActionQ;
}


void Scheduler::initialise(){
//------  First startup all the objects -----------------------

#ifdef DEBUG
  cout << " ------------------------------  Start up  "
       << "------------------------------\n";
  cout.flush();
#endif

  int numOfObjects = allSMIObjects.numOfObjects();



  int io;
  for (io=0; io < numOfObjects; io++) {
     SMIObject *pSMIObj;
     pSMIObj = allSMIObjects.gimePointer(io);
//     pSMIObj->whatAreYou();
	 _pCommHandler->declareObj(pSMIObj);
     pSMIObj->startUp(_pStateQ,_pCommHandler);
  }
  

  int numOfSets = allSMIObjectSets.length();

  for (io=0; io < numOfSets; io++) {
     SMIObjectSet *pSMIObjSet;
     pSMIObjSet = (SMIObjectSet *)allSMIObjectSets.gimePointer(io);
    _pCommHandler->declareObjSet(pSMIObjSet);
  }  
  
  _pCommHandler->declareObjList();

#ifdef DEBUG
  cout << " ---------------------------  End of Start up  "
       << "---------------------------\n\n";
  cout.flush();
#endif

  return;
//-----------------------------------------------------------
}

void Scheduler::execute(){

while   ( !_pStateQ->isEmpty()          || 
          !_pExternalActionQ->isEmpty() ||
          !executableObjectQ.isEmpty()  ||
	  !resumeHandler.isResumeQueueEmpty() 
        ) {

     for (; ;) {
         if( _pStateQ->isEmpty() ) {break;}
         TwoNames stateInfo;
         stateInfo = _pStateQ->remove();
   
         Name stateNm,objectNm;
         stateInfo.gimeNames(objectNm,stateNm);
 
	 if (dbg > 1)
	   {
	     print_obj(objectNm);
	     cout << " StateQ ->state : " << stateNm;
	     cout << endl;
	   }

         SMIObject *pSMIObj;
         pSMIObj = allSMIObjects.gimePointer(objectNm);

	 if ( strncmp(stateNm.getString(),"&BUSY",5) == 0 )
	   {
	     // The format of this state is:
	     //     &BUSY-xxx          where
	     //    xxxx is the name of the action being executed
	     // This is an external object going to busy. At this point I am
	     //only interested if it is 'spontaneous' busy
	     if ( pSMIObj->isBusy()) { continue; }
	     // Also I am not really sure what to do about it apart from
	     // broadcasting it via Command Handler
	     _pCommHandler->publishBusy(pSMIObj,stateNm.getString()+6);
	     continue;
	   }

         pSMIObj->setState(stateNm);
	     _pCommHandler->publishState(pSMIObj);
	handleWhens_new(pSMIObj);
     }

     for (; ;) {
         if( _pExternalActionQ->isEmpty() ) {break;}
         TwoNames actionInfo;
         actionInfo = _pExternalActionQ->remove();
         Name actionstring, objectstring;
         actionInfo.gimeNames(objectstring,actionstring);

	if ( dbg > 0 || Msg::dllmsgRxPtr != 0 ) {
		char timeStr[30];
		Name message;
		message = "<";
		message += objectstring;
		message += "> External action <";
		message += actionstring;
		message += ">";
		if ( Msg::dllmsgRxPtr != 0 ) (*Msg::dllmsgRxPtr)(message.getString());
		if (dbg > 0)
		{
			gime_date_time(timeStr);
			cout << endl;
		  	cout << timeStr << " - " << message.getString() << endl;
		} 
	}

         SMIObject *pSMIObj;
         pSMIObj = allSMIObjects.gimePointer(objectstring);
         if ( guardianOK(objectstring,actionstring) ) {
            pSMIObj->queueAction(actionstring);
         }
         else {
             cout << " Action rejected " << endl; cout.flush();
             _pCommHandler->invalidAction(pSMIObj); 
        }
     }

     for (; ;) {
         if( executableObjectQ.isEmpty() ) {break;}
//         cout << " Executab object queue \n";
//         executableObjectQ.out();
         
         Name objectNm = executableObjectQ.remove();
#ifdef DEBUG
         print_obj(objectNm);
         cout << " is executable \n"; 
         cout.flush();
#endif
         SMIObject *pSMIObj;
         pSMIObj = allSMIObjects.gimePointer(objectNm);
         pSMIObj->execute();
	if (_reqObjectSetList.numOfEntries()) {
		handleObjectSetWhens_new();
	}
         ifHandler.execute();	 
     }
     
     if ( !resumeHandler.isResumeQueueEmpty() )
     {
          resumeHandler.resumeReadyToResumeObjects();
     }

}
     return;
}
//--------------------------reqWhenExecution----------------------------------
void Scheduler::reqWhenExecution(const Name& objSet) {
	_reqObjectSetList.add(objSet);
	return;
}

//============================= handleWhens_new =============  BF May 2008  ====

void Scheduler::handleWhens_new( SMIObject* pSMIObj ) const {
	ClientWhens clientWhens;
	
	pSMIObj->executeAllWhens();
	
	pSMIObj->gimeCurrentClientWhens(clientWhens);
	clientWhens.executeWhens();

	return;

}
//======================== handleObjectSetWhens_new ==========  BF May 2008  ===
void Scheduler::handleObjectSetWhens_new()  {
	ClientWhens clientWhens;
	
        if ( dbg > 2 ) {
            print_msg("handleObjectSetWhens_new() called");
	}
	
	NameList* ptnL = &_reqObjectSetList;
	ptnL->reset();
	
	SMIObjectSet* pObjSet;
	Name setName;
	
	while (ptnL->nextItem(setName))
	{
		pObjSet = (SMIObjectSet *)allSMIObjectSets.gimePointer(setName);

		_pCommHandler->publishObjSet(pObjSet);

		const ClientWhens& setClientWhens
			= pObjSet->gimeClientWhensRef();
		clientWhens.mergeIn( setClientWhens );

	}
	ptnL->removeAll();
	
	clientWhens.executeWhens();

        if ( dbg > 2 ) {
            print_msg("handleObjectSetWhens_new() finished");
	    cout << endl;
	}	
	return;
}
//----------------------------------------------------------------------------
  int Scheduler::guardianOK(const Name& objectName, Name& actionstring) {

     Name actionnm;
     actionstring.element(0,'/',actionnm);
     Parms pars;
     pars.initFromParmString(actionstring);

     if ( dbg > 4 ) {
         cout << " guardian:  Object " << objectName 
              << " Action string: " << actionstring << endl; cout.flush();
     }

     Name nameAmpersId("&ID"), valAmpersId("\0");
     Name nameAmpersConnId("&CONN_ID"),valAmpersConnId("\0");
     char type[7];
     Name doubleq("\""),nullstr("\0");

     int ampersIdPresent = pars.get(nameAmpersId,valAmpersId,type);
     int ampersConnIdPresent = pars.get(nameAmpersConnId,valAmpersConnId,type);

     if (ampersIdPresent) {valAmpersId.replace(doubleq,nullstr);}

     if ( dbg > 4 ) {
         cout << " Value of &ID : " << valAmpersId << endl; cout.flush();
         cout << " ampersIdPresent : " << ampersIdPresent << endl << endl; cout.flush();
     }
     
     if ( allocator == "\0" ) { } // domain not allocated
     else {  // it is allocated
         cout << " Domain allocated to : " << allocator << endl; cout.flush();
         if ( ampersIdPresent ) { // &ID present
            if ( allocator == valAmpersId ) {}  //this is OK
            else { return 0;} //down the drainpipe
         }
         else { return 0;} //down the drainpipe
     }

// Now just tydiing up

     if ( objectName == "&ALLOC" ) {
        if ( !ampersIdPresent || !ampersConnIdPresent ) {
           cout << " Actions to &ALLOC must have &ID and &CONN_ID" << endl;
           cout.flush();
           return 0;  // down the drainpipe
        }
     }
     else { // for all other objects remove &ID and &CONN_ID
         Parms temp;

         if ( ampersIdPresent ) {
            pars.remove(nameAmpersId,temp);
            pars = temp;
         }

         if ( ampersConnIdPresent ) {
            pars.remove(nameAmpersConnId,temp);
            pars = temp;
         }

         Name parString;
         pars.buildParmString(parString);
         actionstring = actionnm;
         actionstring += parString;
     }
     return 1;
  }
