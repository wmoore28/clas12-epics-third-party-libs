//-----------------------------  A c t i o n  Class  -------------------------
#ifndef ACTION_HH
#define ACTION_HH

#include "parameters.hxx"
#include "inslist.hxx"
#include "name.hxx"
#include "parms.hxx"
class SMIObject;
class State;
class InsList;
//
//                                                   23-Sep-1996
//                                                   Boda Franek
//---------------------------------------------------------------------------
class Action {
public :
  Action
    ( char lines[][MAXRECL],Name& action_nm,int *no_lines,
      SMIObject *pobj, State* pstat);

  void whatAreYou();

  void actionString(Name& actionstr);

  int execute(Name& endState, const Name& actionstr) ;

  Parms* pCurrentParameters() ; // returns the pointer to the current parms

  Parms* pDeclaredParms();

/** gets the actio parameter identified by 'name'. The function returns 0
  if not found
*/	
	int getActionParameter(const Name& name, Name& value, Name& type) const;
	
	Name& actionName();
	
private :

	void dealWithDomainCommands(const Name& actionstr);
	
  Name _actionName;

  InsList *_pblock;      // Pointer to the instruction list(block)
//
  Name _objName;    // Name of the object to which 
//                                        the action belongs

  SMIObject *_pParentObject;

  State* _pParentState;

  Parms _declaredParms;  

//------------  Dynamic data

  Parms _currentParameters;  
};

#endif
