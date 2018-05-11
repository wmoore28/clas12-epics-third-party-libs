//------------------------- InsList Class ------------------------------------
#ifndef INSLIST_HH
#define INSLIST_HH

#include "parameters.hxx"
#include "instruction.hxx"
#include "doins.hxx"
#include "termins.hxx"
#include "setins.hxx"
#include "action.hxx"
#include "ptrvector.hxx"
class SMIObject;
class State;
class Action;
#include "instruction_return_status.hxx"
//                                                             B. Franek
//                                                             April 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------
//
class IfIns;

class InsList {
public :
  InsList(int blknum, int lev, SMIObject *pobj, State*, Action*);
  void initialise( char lines[][MAXRECL], PtrVector& allBlocks, 
                   int& no_lines);
  void listInstructions();
  InstructionReturnStatus_t execute( Name& endState );
private :
  int _block_id;
  int _level;

	PtrVector _instructions;

  int _isuspended;    // instruction at which it was suspended

  SMIObject *_pParentObject;

  State* _pParentState;
  Action* _pParentAction;
};

#endif
