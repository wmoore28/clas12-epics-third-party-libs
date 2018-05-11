//-------------------------  Class   D o I n s  -------------------------------
#ifndef DOINS_HH
#define DOINS_HH

#include "parameters.hxx"
#include "instruction.hxx"
#include "parms.hxx"
class SMIObject;
class State;
class Action;
//                                                  Date :  5-May-1996
//                                                  Author : Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------


class DoIns : public Instruction{
public:
  DoIns
 ( char lines[][MAXRECL], int lev,int *no_lines, 
  SMIObject *pobj=0, State *pstat=0, Action* act=0);
  void whatAreYou();
  int execute( Name& endState );   // 0-normal,  1-terminated action, 2-suspended

  void executeHp();

/** it will return true when 'action' is 'stay_in_state'
*/
	bool stay_in_state() const;
private:
  int level;
  Name _actionNm, _objectNm;
//
  SMIObject *_pParentObject;

  Action* _pParentAction;
//
  int _numOfPara;
  Parms _doInsParameters;
};

#endif
