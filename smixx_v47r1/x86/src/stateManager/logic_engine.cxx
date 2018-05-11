//------------------  M a i n   P r o g r a m  of Logic Engine -----------
//
//                                        Author : B. Franek
//                                        Date : April 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-------------------------------------------------------------------------
#include "smixx_common.hxx"
#ifndef WIN32
#include <unistd.h>
#endif
#include "parameters.hxx"
#include "objectregistrar.hxx"
#include "scheduler.hxx"
#include "smiobject.hxx"
#include "twonames.hxx"
#include "queue_twonames.hxx"
#include "resumehandler.hxx"

CommHandler *pCommHandlerGl;   // This will be tydied up later
Scheduler* pSchedulerGl;       // ----dtto-----------
Queue_TwoNames *pStateQGl;

extern  Name smiDomain;

extern ResumeHandler resumeHandler;
//-----------------------------------------------------------------------------

void logicEngine() {


// ----- Instantiate StateQ -------------------------------

    Queue_TwoNames *pStateQ;
    pStateQ = new Queue_TwoNames();
    pStateQGl = pStateQ;


// ----- Instantiate ExternalActionQ -------------------------------
    Queue_TwoNames *pExternalActionQ;
    pExternalActionQ = new Queue_TwoNames();



// ----- Instantiate CommHandler -------------------------------

    CommHandler *pCommHandler;
    pCommHandler = new CommHandler(smiDomain.getString(),
                                   pStateQ, pExternalActionQ,1);
    pCommHandlerGl = pCommHandler;  // I know, this is bad...will be tydied later


// ----- Instantiate Scheduler ----------------------------------
    Scheduler *pScheduler;
    pScheduler = new Scheduler( pCommHandler, pStateQ,
                                                 pExternalActionQ );
    pSchedulerGl = pScheduler;

    pScheduler->initialise();

//-----------------------------------------------------------

// Infinite loop waiting for something to happen

   for (;;) {
//      print_msg(" Waiting..");
      if( !pStateQ->isEmpty() ) { pScheduler->execute();}
      if( !pExternalActionQ->isEmpty() ) { pScheduler->execute();}
      if( !resumeHandler.isResumeQueueEmpty() ) { pScheduler->execute();} 
#ifdef __VMS
      sys$hiber();
#else
      dim_wait();
#endif
   }

   exit(1);
}
