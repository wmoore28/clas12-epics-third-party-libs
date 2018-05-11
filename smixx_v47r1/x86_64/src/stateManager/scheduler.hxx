//---------------------- Scheduler   Class ---------------------------
#ifndef SCHEDULER_HH
#define SCHEDULER_HH
#include "smixx_common.hxx"
#include "parameters.hxx"
#include "objectregistrar.hxx"
#include "smiclass.hxx"

#include "commhandler.hxx"
#include "queue_twonames.hxx"
#include "namelist.hxx"
//                                                          8-November-1996
//                                                            B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//--------------------------------------------------------------------------


class Scheduler {
public :
  Scheduler
  ( CommHandler *pCommHandler,
    Queue_TwoNames *pStateQ, Queue_TwoNames *pExternalActionQ );

  void initialise();

  void execute();

	void reqWhenExecution(const Name& objSet);
private :

  int guardianOK(const Name& objectstring, Name& actionstring);

  CommHandler *_pCommHandler;
  Queue_TwoNames *_pStateQ;
  Queue_TwoNames *_pExternalActionQ;

	NameList _reqObjectSetList;
	
	void handleWhens_new(SMIObject* ) const;
	
	void handleObjectSetWhens_new() ;
};
#endif
