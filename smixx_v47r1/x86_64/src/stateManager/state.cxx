//--------------------------  S t a t e Class  ------------------------------
#include "smixx_common.hxx"
#include <assert.h>
#include <stdio.h>
#include "ut_sm.hxx"
#include "state.hxx"
#include "smiobject.hxx"
#include "parms.hxx"
//                                                         19-August-1996
extern Name smiDomain;
extern ObjectRegistrar allSMIObjects;
extern int dbg;
//                                                          B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
State::State( char lines[][MAXRECL],
               SMIObject* pobj,
               int *st_lines){

//
//  It will first instantiate the State object
//  It will then scan the input lines to extract actions. For every action
//  the subroutine process_action is called that will process it and returns
//  the pointer to the 'action' object. This pointer and the action name are
//  then saved in the 'state' object. The routine will return the number of
//  the state lines (st_lines) and the pointer to the 'state' object (pState).
//
// P.s.  0th line of the state is the line following the name of the state
//     

   _objName = pobj->name();

   char name[MAXRECL];
   strcpy(name,lines[1]);

   _stateName = name;

   _stateNametag = name;

   _pParentObject = pobj;
   
   _special_termination_when_inx = -1;

#ifdef DEBUG
   cout << " state : " << _stateName << "\n";
   cout.flush();
#endif

    int numOfAttributes;
    sscanf(lines[2],"%d",&numOfAttributes);

//   cout << " Atributes :" << numOfAttributes << "\n";

   int lin;
   lin = 3;

    if ( numOfAttributes > 0 ) {
       char atrib[MAXRECL]; Name attribNm;
       for ( int ia=0; ia<numOfAttributes; ia++) {
           sscanf(lines[lin],"%s",atrib);     // Read in the atributes
           attribNm = atrib;
	   _attributes += attribNm; 
           lin++;
       }
    }


   Name action_nm;

   int no_aclines;


//------------------- Loop over state lines untill *END_STATE found ----------

  int whenInx = 0;

  for (;;lin++){
//    cout << lin << ": |" << lines[lin] << "|\n";
    if (!strcmp(lines[lin],"*END_STATE")) {
       *st_lines = lin+1;
       break;
    }

    else if (!strcmp(lines[lin],"*ACTION")) {

//--------
       Action *pAct;

       pAct = new Action
             (&lines[lin],action_nm,&no_aclines,_pParentObject,this);
// The subroutine returns pointer to the action and number of action lines
// when it encounteres *END_ACTION

//------------- The action pointer and its name is added to _actions
    int flg = _actions.add(action_nm,pAct);
    if ( flg != 1 ) {
        cout << " *** Error: State:" << _stateName <<
	    "  Action : " << action_nm << " declared twice" << endl;
	exit(1);
    }

//       cout << " No of action lines : " << no_aclines << "\n";
       lin = lin + no_aclines - 1;
    }

    else if (!strcmp(lines[lin],"*WHEN")) {
       int no_whlines;
       _whens += new When(&lines[lin],&no_whlines,_pParentObject,this,whenInx);
       whenInx++;
       lin = lin + no_whlines - 1;
    }

    else {
       cout << "Strange sequence " << lines[lin] << " encountered\n";
       cout.flush();
       exit(1);
    }
  }
  
  return;
}
//...........................................................................
State::State( const char* const state_name, SMIObject* pobj ) {
    Name attribNm;
   _stateName = state_name;

   _stateNametag = state_name;

   if ( _stateName == "DEAD" ) {
      attribNm = "DEAD_STATE";
      _attributes += attribNm;
   }
   else if ( _stateName == "&INITIAL" ) {
      attribNm = "INITIAL_STATE";
      _attributes += attribNm;
   }
   else if ( _stateName == "&UNDECLARED_STATE" ) {
      attribNm = "UNDECLARED_STATE";
      _attributes += attribNm;
   }

   else {  }


   _objName = pobj->name();

   _pParentObject = pobj;
   
   _special_termination_when_inx = -1;
}



//********************************  whatAreYou  ***************************

void State::whatAreYou() const {
    int numOfAttributes = _attributes.length();
  cout << "\n";
  cout << " state : " << _stateName ;


    if (numOfAttributes > 0 ) {
       for ( int ia=0; ia<numOfAttributes; ia++) {
           cout << "/" << _attributes[ia];
       }
    }

    cout << "\n";
    cout.flush();
    When* pWhen;
    int numWhens = _whens.length(); 
    if (numWhens > 0 ) {
       for ( int iw=0; iw<numWhens; iw++) {
           pWhen = static_cast<When*>(_whens[iw]);
           pWhen->whatAreYou();
       }
    }

    int numActions = _actions.length();
   cout << " \n";
//  cout << " No. of actions : " << numActions << "\n";

    if ( numActions == 0 ) {return;}

    Name action;
    Action* pAction;
    
    for (int i=0; i<numActions; i++) {
        action = _actions.gimeName(i);
	pAction = static_cast<Action*>(_actions.gimePointer(i));
//    cout << pAction << "  " << action << "\n";
        pAction->whatAreYou();
  }

  return;
}

//------------------------------- execute  (for Logical objects) ------------

int State::execute( const Name& actionstr, Name& endState ) const {
//
//  cout << " state : " << _stateName << " to execute action |" 
//                              << pname_act << "|\n \n";
//
  Name actionnm;
  actionstr.element(0,'/',actionnm);

    int numActions = _actions.length();
    Name action; Action* pAction;
    
    for (int i=0; i<numActions; i++) {
        action = _actions.gimeName(i);
	pAction = static_cast<Action*>(_actions.gimePointer(i));

        if ( action == actionnm ) {
            int temp = pAction->execute(endState,actionstr);
            if ( temp == 0 ) {  // This means that the last action instruction
//                           is not terminate_action instruction
               endState =_stateName;
               return 1;
            }
            return temp;
        }
    }

//  cout << " action not found " << "\n";
  return -1;
}

//-------------------- execute_assoc  (for Associated objects) ------------

int State::execute_assoc
( const Name& actionstr, const CommHandler* const pCommHandler) const {
//
//  cout << " state : " << _stateName << " to execute action |" 
//                              << pname_act << "|\n \n";

// Extract action name
  Name actionnm;
  actionstr.element(0,'/',actionnm);

  if ( !actionExists(actionnm) ) { return -1; }  

  Parms inpars;
  inpars.initFromParmString(actionstr);

  Name remoteDomain = _pParentObject->externDomain();

  if ( remoteDomain == "\0" ) {} // proxy  
  else {  // this is an object in another domain (external)
     Name remAllocObject = remoteDomain;
     remAllocObject += "::";
     remAllocObject += "&ALLOC";
     SMIObject* pRemAllocObject = allSMIObjects.gimePointer(remAllocObject);

     if ( !pRemAllocObject ) {
        cout << " Remote domain " << remoteDomain << 
                " does not have &ALLOC object " << endl;
     }
     else {
        if ( pRemAllocObject->currentState() == "NOT_ALLOCATED" ||
             pRemAllocObject->currentState() == "NOT_THERE" ||
             pRemAllocObject->currentState() == smiDomain ) {}
        else { return -1; }
     } 

     if ( _objName.exists("&ALLOC") ) {  // this is even an allocation object
        inpars.clear();  // For this kind of object I know what the pars should be
        Name pname("ID");
        inpars.add(pname,smiDomain,"STRING");
     }
  }

  Name parmstr,temp;
  inpars.buildParmString(parmstr);
  temp = actionnm;
  temp += parmstr;   

  pCommHandler->sendCommand(_objName.getString(),temp.getString());
  return 1;

}
//========================  executeWhensFromFlagList  ======  BF  May 2008  ===
void State::executeWhensFromFlagList(const std::vector<int>& relevantWhensFlagList)
{
	int numWhens = _whens.length();
	int relevantWhensListSize = relevantWhensFlagList.size();

	if ( relevantWhensListSize == 0 ) { return; }

	// copy the 'relevant whens list' to the 'execution list'	
	std::vector<int> whensToExecuteList(relevantWhensFlagList);
	
	int termination_when_inx; // after whens are executed from execution list,
				  //  this is the index of the when that terminated
				  // the sequence. -1 indicates that all the whens were
				  // dormant
	bool special; 	// valu TRUE means that the termination when is special (stay_in_state)
	
//	cout << whensToExecuteList << endl;
//	cout << " Special termination when index " << _special_termination_when_inx << endl;
	if ( _special_termination_when_inx == -1 )
	{ // the previous time when 'whens' were executed the terminating when was not special when.
	  // This means that the 'irrelevant whens' are still dormant and only
	  // the relevant ones need to be executed
		
		termination_when_inx = executeRequestedWhens( whensToExecuteList,
                                                          special );
	} else
	{     // terminating when was special. Firstly we will find out whether now
	      // it is relevant or not
		bool relevant;
		
		if ( relevantWhensListSize > _special_termination_when_inx )
		{
			if ( whensToExecuteList[_special_termination_when_inx] ) { relevant = true; }
			else { relevant = false; }
		}
		else { relevant = false; }
			
		
		if ( relevant )
		{ // special when is now relevant and that means that it could be dormant.
		// Also as the previous sequence was terminated by this when, we do not realy
		//  know what is the status of the following whens and so they also have to be
		// executed. Therefore we mark all the following whens in the list as requested
		// and execute the list
			whensToExecuteList.resize(_special_termination_when_inx);
			whensToExecuteList.resize(numWhens,1);
			termination_when_inx = executeRequestedWhens( whensToExecuteList,
                                                          special );
		} else
		{ // the special when is irrelevant and therefore alive. However we still have
		  // to execute all the relevant whens before .
		// cut the list just above the special when and execute.
			whensToExecuteList.resize(_special_termination_when_inx);
			termination_when_inx = executeRequestedWhens( whensToExecuteList,
                                                          special );
			if ( termination_when_inx == -1 ) 
			{ // this means that none of the relevant whens before are alive
			  // so we can just simply return and leave _special_termination_when_inx
			  // unchanged
			 	return;
			}
		}
	}
	
	if ( special) { _special_termination_when_inx = termination_when_inx;}
	else { _special_termination_when_inx = -1; }

	if ( termination_when_inx >= 0 && dbg > 2 ) 
	{
		print_obj(_objName);
		cout << "(state " << _stateName 
		<< ") when no." << termination_when_inx << " activated" << endl;
	}
	
	return;
	

}
//========================  executeRequestedWhens  ======  BF  May 2008  ===
int State::executeRequestedWhens( std::vector<int>& requestFlagList,
                                  bool& terminatingWhenSpecial ) const
{
	int listSize = requestFlagList.size();

	if ( listSize == 0 ) { return -1; }

	When* pWhen;
	int iw,flg;
	
	for ( iw = 0; iw < listSize; iw++) {
		flg = requestFlagList.at(iw);
		if ( flg == 1 )
		{
			pWhen = static_cast<When*>(_whens[iw]);
//       cout << "    " << iw << "\n";
			if (pWhen->forcedExecute())
			{
				terminatingWhenSpecial = pWhen->specialWhen();
				return iw;
			}
		}
	}
	
	return -1;
}

//========================  executeAllWhens  ===============  BF  May 2008  ===
void State::executeAllWhens() {
// This is executed only when the object just entered the state. The calling
// sequence is:  Scheduler --> executeAllWhens(SMIObject) --> executeAllWhens
	int numWhens = _whens.length();

	if ( numWhens == 0 ) { return; }
	
	std::vector<int> requestedWhensList(numWhens,1); // this will fill the list with 1's
	
	_special_termination_when_inx = -1;

	bool special;
	int termination_when_inx = executeRequestedWhens( requestedWhensList,
                                                          special );
	if ( special ) { _special_termination_when_inx = termination_when_inx; }

	if ( termination_when_inx >= 0 && dbg > 2 ) 
	{
		print_obj(_objName);
		cout << "(state " << _stateName 
		<< ") when no." << termination_when_inx << " activated" << endl;
	}

	return;
}
//--------------------------------  initialState  ---------------------------

bool State::initialState() const {
    int numOfAttributes = _attributes.length();
   for (int ia=0; ia<numOfAttributes; ia++) {
      if ( _attributes[ia] == "INITIAL_STATE" ) { return 1;}
   }   
   return 0;
}

//--------------------------------  deadState  -----------------------------

bool State::deadState() const {
    int numOfAttributes = _attributes.length();
   for (int ia=0; ia<numOfAttributes; ia++) {
      if ( _attributes[ia] == "DEAD_STATE" ) { return 1;}
   }   
   return 0;
}

//--------------------------------  undeclaredState  -----------------------------

bool State::undeclaredState() const {
    int numOfAttributes = _attributes.length();
   for (int ia=0; ia<numOfAttributes; ia++) {
      if ( _attributes[ia] == "UNDECLARED_STATE" ) { return 1;}
   }   
   return 0;
}

//-------------------------------- name -------------------------------------
Name& State::stateName() {
   return _stateName;
}
//-------------------------------- tagName ---------------------------------
Name State::tagName() const {
    return _stateNametag;
}

void State::changeName( const Name& name ) {
   _stateName = name;
}

//--------------------------- numActions ------   B.Franek   4-Feb-1999 ----
  int State::numActions() const {
     return _actions.length();
  }
//---------------------------------  name  -----------------------------------

void State::name ( char *name, int *numActions) const {

        _stateName.whatAreYou(name,256);
	*numActions = _actions.length();

	return;

}


//--------------------------------  action  ----------------------------------

void State::action ( const int index, Action** pAction ) const {

    int numActions = _actions.length();

        if ( index >= numActions ) {
            cout << " State::action ... invalid index \n";
            cout.flush();
            exit(1);
        }

        *pAction = static_cast<Action*>(_actions.gimePointer(index));
	return;

}

//----------------------------- actionString ----------------------------------
//   action/par1/par2=555/par3
//
  Name State::actionString( const int index ) const {

    int numActions = _actions.length();
    
     if ( index >= numActions ) {
         cout << " State::action ... invalid index \n";
         cout.flush();
         exit(1);
     }
//
    Action* pact = static_cast<Action*>(_actions.gimePointer(index));
//
    Name temp;

    pact->actionString(temp);

    return temp;
  }

void State::actionString 
     ( const int index, char* actionstr, const int maxlen) const {
//
    int numActions = _actions.length();
    
     if ( index >= numActions ) {
         cout << " State::action ... invalid index \n";
         cout.flush();
         exit(1);
     }
//
    Action* pact = static_cast<Action*>(_actions.gimePointer(index));
//
    Name temp;

    pact->actionString(temp);

    temp.whatAreYou(actionstr,maxlen);

    return;
   
}

bool State::actionExists( const Name& actionnm ) const {

    int numActions = _actions.length();
    
    for (int i=0; i<numActions; i++) {
        if (  _actions.gimeName(i) == actionnm ) {
            return 1;
        }
    }

  return 0;
}
//==========================================================================
int State::updateReferencedObjectsandSets() const
{
	int numWhens = _whens.length();
	int iw;
	When* pWhen;
	
	for ( iw=0; iw < numWhens; iw++)
	{
		pWhen = static_cast<When*>(_whens[iw]);
		pWhen->updateReferencedObjectsandSets();
	}
	return 1;
}
