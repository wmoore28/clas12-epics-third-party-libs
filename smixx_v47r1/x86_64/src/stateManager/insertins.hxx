//-------------------------  Class   InsertIns  -------------------------------
#ifndef INSERTINS_HH
#define INSERTINS_HH

#include "name.hxx"
#include "parameters.hxx"
#include "instruction.hxx"
class SMIObject;
class State;
class Action;
//                                                  Date :    3 July 2001
//                                                  Author : Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------


class InsertIns : public Instruction{
public:
	InsertIns
           ( char lines[][MAXRECL], const int lev, int& no_lines,
              SMIObject* pobj, State *pstat, Action* pact);

	void whatAreYou() ;

	int execute( Name& endState );   // 0-normal,  1-terminated action, 2-suspended
private:
	int _level;

	int _insertRemove;   // 1-insert  0-remove

	Name _objectNm, _setNm;

	SMIObject *_pParentObject;

	Action *_pParentAction;
};

#endif
