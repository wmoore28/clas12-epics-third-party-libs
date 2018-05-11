//-------------------------  Class   CreateObjectIns  -------------------------------
#ifndef CREATEOBJECTINS_HH
#define ICREATEOBJECTINS_HH

#include "name.hxx"
#include "namevector.hxx"
#include "parameters.hxx"
#include "instruction.hxx"
class SMIObject;
class State;
class Action;
//                                                  Date :    May 2010
//                                                  Author : Boda Franek
// Copyright Information:
//      Copyright (C) 1996-2010 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------


class CreateObjectIns : public Instruction{
public:
	CreateObjectIns
           ( char lines[][MAXRECL], const int lev, int& no_lines,
              SMIObject* pobj, State *pstat, Action* pact);

	void whatAreYou() ;

	int execute( Name& endState );   // 0-normal,  1-terminated action, 2-suspended
private:
	int _level;

/**
     this has a format:
             &VAL_OF_<parm-name>
      where <parm-name> is name of the action parameter of the action containing
      the instruction
*/
	Name _objectIdentifier;
/**
      the class name to which the created object is going to belong	
*/
	Name _className;

	SMIObject *_pParentObject;

	Action *_pParentAction;
	
/**
      holds the sobj code of the instruction:
      
      line 0:create_object
      line 1:&VAL_OF_<param-name>
      line 2:    1    0    0
      line 3:<class-name> 
*/
	NameVector _sobjCode;
};

#endif
