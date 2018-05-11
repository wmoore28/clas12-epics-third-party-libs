//----------------------------------------------------------------------
//
//                                           Author :  B. Franek
//                                           Date : May 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef TERMINS_HH
#define TERMINS_HH

#include "parameters.hxx"
#include "instruction.hxx"
#include "name.hxx"
class SMIObject;
class State;
class Action;


class TermIns : public Instruction {
public:
  TermIns( char lines[][MAXRECL], const int lev, 
           SMIObject* pObj, State* pState, Action* pAction, int& no_lines);
  void whatAreYou();
  int execute( Name& endState );   // 0-normal,  1-terminated action, 2-suspended
private:
  Name _endState;
  int _level;

  SMIObject* _pParentObject;
  Action* _pParentAction;
  State*  _pParentState;
};

#endif
