//---------------------- S t a t e   Class ---------------------------
#ifndef STATE_HH
#define STATE_HH

#include "parameters.hxx"
#include "action.hxx"
#include "when.hxx"
#include "commhandler.hxx"
#include "namevector.hxx"
#include "ptrvector.hxx"
#include "registrar.hxx"
#include <vector>
class SMIobject;
class When;
class CommHandler;
class Name;
//                                                 Author: B. Franek
//                                                 Date : May-1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------

class State {
public :
  State ( char lines[][MAXRECL],
           SMIObject* pobj,
           int *st_lines);

  State ( const char* const state_name, SMIObject* pobj );

  void whatAreYou() const;
//
  int execute( const Name& actionstr, Name& endState) const;
//
  int execute_assoc
      ( const Name& actionstr, const CommHandler* const pCommHandler) const;
//                       Special for associated objects 
//                       Return value
//                       is 1(OK) or 
//                       -1...action not found
//
/** will execute whens according to the 'relevant whens'  flag-list
     @param list ...relevant whens list of flags ( 1 per when) indicating
                    whether particular when is relevant. The list ends
		     with the last flag of value 1. 
*/
	void executeWhensFromFlagList
	( const std::vector<int>& relevantWhensFlaglist ) ;

/** executes all whens with non-busy objects
*/
	void executeAllWhens();
		
  bool initialState() const;

  bool deadState() const;

/** returns TRUE if the state has attribute 'undeclared_state'
    It only applies to associated objects. When the object returns
    a state, that was not declared, it will be set to this state.
*/
	bool undeclaredState() const;
	
  Name& stateName() ;

  int numActions() const;

  Name tagName() const;

  void changeName( const Name& name );

  void name(char* name, int *numActions) const;

  void action( const int index, Action** pAction) const;

  void actionString( const int index, char* actionstr, const int maxsize) const;

  Name actionString( const int index ) const;

  bool actionExists( const Name& actname) const;
  
/**
    Every object or object set which is reffered to by whens of this state is
    updated with their addresses. This method simply loops over the existing
    whens and for each of them calls methos of the same name which actually
    does the job.
*/
	int updateReferencedObjectsandSets() const;


private :
// methods:
/** will execute whens according to the supplied requestFlagList
	the flag list consists of ones and zeros.
	When flagList(inx) is 1, then _whens[inx] is executed.
	If none of the requested 'whens' is 'alive', it returns -1
	otherwise it returns the index of 'alive' 'when'.
	If the terminating when is special, terminatingWhenSpecial is set TRUE
*/
	int executeRequestedWhens
	     ( std::vector<int>& requestFlagList,
	       bool& terminatingWhenSpecial ) const;
	
// data

  Name _stateName;             // State name

  Name _stateNametag;   //e.g. &VARIABLE

    
    NameVector _attributes;

    PtrVector _whens;
    
/**
	when the 'when sequence' terminates by a 'special when', this
	is its index (0....). Otherwise it is -1
*/
	int _special_termination_when_inx;

    Registrar _actions;

  Name _objName;    // Name of the object to which 
//                                        the state belongs
  SMIObject *_pParentObject;
  
};
#endif
