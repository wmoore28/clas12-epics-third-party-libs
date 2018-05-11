//--------------------------  SMIObject Class  -----------------------------------
// $Id
//#define FAST_SMI
#include <stdlib.h>
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>
#include "ut_sm.hxx"
//----------- Externals ------------------
#include "queue_name.hxx"
   extern Queue_Name executableObjectQ;
#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
#include "ifhandler.hxx"
   extern IfHandler ifHandler;
#include "resumehandler.hxx"
   extern ResumeHandler resumeHandler;
#include "registrar.hxx"
   extern Registrar allSMIClasses;
//---------------------------------------
#include "smiclass.hxx"
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "commhandler.hxx"
#include "msg.hxx"
    extern int dbg;
#include "waitins.hxx"
#include "ifins.hxx"
//                                                         19-August-1996
//                                                          B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
typedef char OBJLINE[MAXRECL];

SMIObject::SMIObject
 ( char lines[][MAXRECL]){


    _lock = 0;

//  0th line is:*OBJECT

   char objnm_temp[MAXRECL];
   strcpy(objnm_temp,lines[1]);
   _objectName = objnm_temp;

    if ( dbg > 1 ) {
	cout << " Object  " << _objectName << endl;
    }


   allSMIObjects.registerObject(this);

    int numOfAttributes;
    sscanf(lines[2],"%d %d %d",&_classFlag,&numOfAttributes,&_associated);

   int lin;
   lin = 3;

   if ( _classFlag != 0 ) {
       _associated = 0; // for objects belonging to a class, this is taken 
                        // from the class and anything comming from the translator
			// output is ignored
       _className = lines[3];
      lin = 4;
   }

    Name attribNm;
    if ( numOfAttributes > 0 ) {
       for ( int ia=0; ia<numOfAttributes; ia++) {
          attribNm = lines[lin];     // Read in the atributes
          _attributes += attribNm;
          lin++;
       }
    }


   if ( _classFlag != 0 ) {   //---- The object belongs to a class
      SMIClass *ptnr;

      ptnr = static_cast<SMIClass*>(allSMIClasses.gimePointer(_className));

//      cout << "Object belongs to class : " << _className << "\n";

      int nl;
      nl = ptnr->numOfLines();
      char* pClassCode = new char[nl*MAXRECL];
      OBJLINE* clines = (OBJLINE*) pClassCode;

      for (int il=0; il<nl; il++) {  //---- Get the lines from the class
         ptnr->gimeLine(il,clines[il]);
//         cout << clines[il] << "\n";
      }
//  line 0 is *CLASS;  line 1 is the 'class name'; line 2 is num of attributes;
      int numOfClassAtributes;
      sscanf(clines[2],"%d",&numOfClassAtributes);

//      cout << "The class has " << numOfClassAtributes << " atributes \n";

      int icl;
      icl = 3;
      int ipom = 0;
      if ( numOfClassAtributes != 0 ) {  //--- Add the class atributes to object
         for ( int ica=0; ica<numOfClassAtributes; ica++) {
            attribNm = clines[icl]; _attributes += attribNm;
            if( attribNm == "ASSOCIATED" ){ipom=1;}
            icl++;
//            cout << attribNm << "\n";
         }
         numOfAttributes = numOfAttributes + numOfClassAtributes;
      }            
      if ( ipom == 1 ) { _associated = 1;}    // class atribute is stronger
      bodyLog(&clines[icl]);
      delete [] pClassCode;
   }
   else {   //---- The object does not belong to a class

      bodyLog(&lines[lin]);
   }

//   print_obj(_objectName);

    if ( dbg > 1 ) {
        if ( _objectParameters.numOfEntries() != 0 ) {
            cout << " Params : " << _objectParameters.numOfEntries() 
                << endl;
        }
    }

   return;
}
//------------------------------------------------------------------------------
void SMIObject::bodyLog( char lines[][MAXRECL]) {
//---
// This processes the object following the atributes
// These are obtained either from the object or from the class
//---

  _numOfSubobjects = 0;

  int lin = 0;

// If the object has any parameters, they should be here
   if ( !strcmp(lines[lin],"*PARAMETERS" ) ){
       lin++;
       char tempStr[MAXRECL];
       Name parName,parVal,nodef="&nodefault";
       char parType[MAXRECL];
       for (;;) {
          if ( !strcmp(lines[lin],"*END_PARAMETERS") ) { lin++; break; }
          strcpy(parType,lines[lin]);
          lin++;
          strcpy(tempStr,lines[lin]);
          parName = tempStr;
          lin++;
          strcpy(tempStr,lines[lin]);
          parVal = tempStr;
          lin++;
          if ( parVal == nodef ) { _objectParameters.add(parName,parType); }
          else            { _objectParameters.add(parName,parVal,parType); }
       }
   }

    State* pState;
  int dead_state_declared = 0;
  int initial_state_declared = 0;
	int undeclared_state_declared = 0;

  for ( ;; lin++) {
     if (!strcmp(lines[lin],"*END_OBJECT")) {break;}

     else if (!strcmp(lines[lin],"*END_CLASS")) {break;}

     else if (!strcmp(lines[lin],"*END_SUBOBJECT")) {

         if ( dead_state_declared == 0 ) {
	     pState = new State("DEAD",this);
             assert ( pState != 0 );
	     int flg = _states.add("DEAD",pState);
	     if ( flg != 1 ) {
	         cout << " *** Error: Object :" << _objectName <<
		     " has state DEAD not declared as dead_state" << endl;
		 exit(1);
	     }
         }

         if ( initial_state_declared == 0 ) {
	     pState = new State("&INITIAL",this);
             assert ( pState != 0 );
	     int flg = _states.add("&INITIAL",pState);
	     if ( flg != 1 ) {
	         cout << " *** Error: Object :" << _objectName <<
		     " has state &INITIAL not declared as initial_state" << endl;
		 exit(1);
	     }  
         }

         if ( undeclared_state_declared == 0 &&
	      !_objectName.exists("::&ALLOC") )
	 {   // will create default 'undeclared_state', unless it is a remote ALLOC obj.
	     pState = new State("&UNDECLARED_STATE",this);
             assert ( pState != 0 );
	     int flg = _states.add("&UNDECLARED_STATE",pState);
	     if ( flg != 1 ) {
	         cout << " *** Error: Object :" << _objectName <<
		     " has state &UNDECLARED_STATE not declared as undeclared_state" << endl;
		 exit(1);
	     }  
         }

         _numOfSubobjStates[_numOfSubobjects-1] = _states.length()
                                              - _stateIndex[_numOfSubobjects-1];
          continue;
     }

     else if (!strcmp(lines[lin],"*SUBOBJECT")) {
        lin++ ;
        dead_state_declared = 0; initial_state_declared = 0;
        _subobject[_numOfSubobjects] = lines[lin];
        _stateIndex[_numOfSubobjects] = _states.length();
        assert ( _numOfSubobjects < MAXSUBOBJECTS );
#ifdef DEBUG
        cout << " subobobject : " << _subobject[_numOfSubobjects] << " \n";
        cout.flush();
#endif
        _numOfSubobjects++;
     }

     else if (!strcmp(lines[lin],"*STATE")) {
        int st_lines;  //  and the number of state lines
	pState = new State(&lines[lin],this, &st_lines);
        assert ( pState != 0 );
	int flg = _states.add(pState->stateName(),pState);
	if ( flg != 1 ) {
	    cout << " *** Error: Object :" << _objectName <<
	        " has state " << pState->stateName() << "declared twice" << endl;
		 exit(1);
	     }

        if ( pState->deadState() ) {dead_state_declared = 1;}
        if ( pState->initialState() ) {initial_state_declared = 1;}

//        cout << " No of state lines : " << st_lines << "\n";
        lin = lin + st_lines - 1;
     }
     else { 
        cout << "Strange sequence " << lines[lin] << " encountered\n";
	    cout.flush();
	 }

  }


  return;
}

//-----------------------------------------------------------------------------
void SMIObject::whatAreYou() {
    State* pState;
   cout << "\n \n" << " Object : " << _objectName ;

    int numOfAttributes = _attributes.length();
   if (numOfAttributes > 0 ) {
      for ( int ia=0; ia<numOfAttributes; ia++) {
         cout << "/" << _attributes[ia];
      }
   }

     cout << "\n";

//   cout << " Number of states : " << _numOfStates << "\n";

         
    int numOfStates = _states.length();
    for (int ist=0; ist<numOfStates; ist++){
        pState = static_cast<State*>(_states.gimePointer(ist));
//      cout << pState << "  " << pState->stateName() << "\n";
         pState->whatAreYou();
      }
   
   cout.flush();
  return;
}

//------------------------------  startUP  -------------------------------------
// Logical objects will determine their initial state. That is either the state
// that is marked /initial or the first state. They will then inject this state
// on the stateQ and will wait for it to come back.
//
// Associated objects will also startup in initial state which is either
// declared /initial_state or it will be called &INITIAL

void SMIObject::startUp( Queue_TwoNames* pStateQ, CommHandler *pCommHandler){

    State* pState;
   _pStateQ = pStateQ;
   _pCommHandler = pCommHandler;
//
//---  First determine what is the object's initial state
//
   if ( _associated == 0 ) {                          //---  Logical object
//------ Determine if there is the initial state declared
      _currStateInx = 0;
      int numOfStates = _states.length();
      for ( int is=0; is<numOfStates; is++) {
          pState = static_cast<State*>(_states.gimePointer(is));
         if ( pState->initialState() ) {
            _currStateInx = is;
            break;
         }
      } 
      
      pState = static_cast<State*>(_states.gimePointer(_currStateInx)); 

	if ( dbg > 0 || Msg::dllmsgRxPtr != 0 ) {
		char timeStr[30];
		Name message;
		message = "<";
		message += _objectName;
		message += "> starting up in state <";
		message += pState->stateName();
		message += ">";
		if ( Msg::dllmsgRxPtr != 0 ) (*Msg::dllmsgRxPtr)(message.getString());
		if (dbg > 0)
		{
			gime_date_time(timeStr);
		  	cout << timeStr << " - " << message.getString() << endl;
		} 
	}
      
//---  add to the State Q
      TwoNames  stateInfo;
      stateInfo.set(_objectName,pState->stateName());
      pStateQ->add(stateInfo);
      strcpy(_internalState,"WaitSetState");
   }
   else                    {                            //---  Associated object
      for ( int isub=0; isub<_numOfSubobjects; isub++) {
         _currStateInxes[isub] = stateInxInitialState(isub);
         if ( _currStateInxes[isub] == -1 ) {
            print_obj(_objectName);
            cout << " Initial state does not exists " << endl; cout.flush();
            abort();
         }
      }
//
      if ( _numOfSubobjects == 1 ) {
          _currStateInx = _currStateInxes[0];
      }

      Name curr_state = currentState();
//
    int iprint(0);
    if ( dbg <= 0 ) { iprint = 0; }
    else if ( dbg == 1 ) { 
        if ( curr_state[0] == '&' ) { iprint = 0; }
	else { iprint = 1; } 
    }
    else { iprint = 1; }

    if ( iprint == 1 ) { 
        print_obj(_objectName);
        cout << " starting up in state :" << curr_state << endl;
    }

      strcpy(_internalState,"WaitSetState");
   }
}

//=========================  setStateDiagMessageNormal ======================
void SMIObject::setStateDiagMessageNormal(const Name& inStateString) const
{
	if ( dbg > 0 || Msg::dllmsgRxPtr != 0 ) {
		char timeStr[30];
		Name message;
		message = "<";
		message += _objectName;
		message += "> in state <";
		message += inStateString;
		message += ">";
		if ( Msg::dllmsgRxPtr != 0 ) (*Msg::dllmsgRxPtr)(message.getString());
		if (dbg > 0)
		{
			gime_date_time(timeStr);
		  	cout << timeStr << " - " << message.getString() << endl;
		} 
	}
	return;
}
void SMIObject::setStateDiagMessageConverted(const Name& inStateString) const
{
	if ( dbg > 0 || Msg::dllmsgRxPtr != 0 )
	{
		char timeStr[30];
		Name message;
		message = "<";
		message += _objectName;
		message += "> in state <";
		message += currentState();
		message += ">";
		message += " converted from <";
		message += inStateString;
		message += ">";
		if ( Msg::dllmsgRxPtr != 0 ) (*Msg::dllmsgRxPtr)(message.getString());
		if (dbg > 0)
		{
			gime_date_time(timeStr);
	  		cout << timeStr << " - " << message.getString() << endl;
		} 
	}

	return;
}
//*****************************  setState  *********************************

void SMIObject::setState( const Name& statestr) { 

    State* pState;    

	if ( _objectName.exists("::&ALLOC") )
	{
		setStateDiagMessageNormal(statestr);
		setStateAlloc(statestr);
		return;
	}

   Name stateNm,paramStr;
   split(statestr,stateNm,paramStr); 

   if ( stateNm == "$DEAD" ) {}
   else {
      if ( _associated != 0 ) { // Only external objects
//                                 carry param string
         if( _objectParameters.setFromParmString(paramStr) != 1 ) {
            cout << " Parameter string " << paramStr << endl
                 << " incompatible with the object parameters " << endl;
            cout.flush();
            abort();
         }
      }
   }

//
   if (_associated == 0) {        //---------------------------- logical object
//                                For logical object, the set State call is 
//                                caused by the object previously injecting
//                                its state onto the state queue and setting its
//                                internal state to "WaitSetState"          
      if ( strcmp(_internalState,"WaitSetState") ) {
         cout << " Object not in the correct internal state";
         cout.flush();
         exit(1);
      }
      strcpy(_internalState,"Idle");
      pState = static_cast<State*>(_states.gimePointer(_currStateInx));
      if ( pState->stateName() == stateNm) {}
      else {
         cout << " Logic failure...the current state is supposed to be :"
              << pState->stateName() << "/n";
         cout.flush();
         exit(1);
      }
      setStateDiagMessageNormal(statestr);
   }                               //--------------------
//
   else {                          //---------------------- associated object
//                                   for these objects setState is the conseq.
//                                   of either an action having been sent to
//                                   the remote proxy or it is a spontaneous
//                                   change of state.
//
	
      if ( stateNm == "$DEAD" ) {  //--- $DEAD state (same string regardless
//                                       the number of subobjects)
//                                       indicating that the conection was lost
         for ( int isub=0; isub<_numOfSubobjects; isub++) {
            _currStateInxes[isub] = stateInxDeadState(isub);
            if ( _currStateInxes[isub] == -1 ) {
               print_obj(_objectName);
               cout << " Dead state does not exists " << endl; cout.flush();
               abort();
            }
         }
//
         if ( _numOfSubobjects == 1 ) {
             _currStateInx = _currStateInxes[0];
         }
	 setStateDiagMessageConverted(statestr);
      }                                //----- end $DEAD state
      else if ( stateInx(stateNm)  == -1 )
      {  //  state of this name is not declared

         _currStateInxes[0] = stateInxUndeclaredState();

         if ( _currStateInxes[0] == -1 ) {
               print_obj(_objectName);
               cout << " Undeclared state does not exists " << endl;
               abort();
         }
         
         _currStateInx = _currStateInxes[0];
	 setStateDiagMessageConverted(statestr);
      }
      else {
         setCurrStateInxes( stateNm ) ; 
         setStateDiagMessageNormal(statestr);                          //------ normal state
      }                                //---------- end normal state

   }


// Having accepted setState, the only possible internal state is "Idle"
// unless it was locked associated object
   if ( strcmp(_internalState,"Locked") ) {
        strcpy(_internalState,"Idle");
   }
   else {
//debug beg
if ( dbg > 1 )
{
      cout << " ********************* warning ****************" << endl;
      print_obj(_objectName);
      cout << " is locked ! " << endl;
}
//debug end
   }
//
// If either of the actions queues has actions, make it executable
   if ( !_hpActionQ.isEmpty() || !_actionQ.isEmpty() ) {
        executableObjectQ.add(_objectName);
   }

   return;
}
//-------------------------------  execute  ----------------------------------

void SMIObject::execute() {

    State* pState;
//
// Are there any actions queued?
//
   if (_hpActionQ.isEmpty()  && _actionQ.isEmpty() ) {
#ifdef DEBUG
      print_obj(_objectName);
      cout << " no actions queued \n";        
      cout.flush();
#endif
      return;
   }
//
// First check if in a correct internal state  i.e. "Idle"
   if ( strcmp(_internalState,"Idle") ) {
#ifdef DEBUG
      print_obj(_objectName);
      cout << " is in internal state : " << _internalState << "\n";
      cout.flush();
#endif
      return;  // The action is left on the queue so it will be executed
//                sometimes in the future
   }
//---- Object is Idle and there are actions queued
   Name actionstr;
   int hpFlag = 0;
//
   if( !_hpActionQ.isEmpty() ) {    // Hight priority actions present
      hpFlag = 1;
      actionstr = _hpActionQ.remove();
   }
   else  { 
      actionstr = _actionQ.remove();
   }
//--------------
   if ( actionstr.exists("&lock/")) {
        _lock++;
        strcpy(_internalState,"Locked");
        Name clientObject;
        actionstr.element(1,'/',clientObject);        

        if ( dbg > 2 ) {
            print_obj(_objectName);
            cout << " locked by  " << clientObject << "\n";
        }
	
	SMIObject* pClientObject = 
		(SMIObject*)(allSMIObjects.gimePointer(clientObject));
	pClientObject->reportingLockedObject(_objectName);
        return;
   }

	if ( dbg > 0 || Msg::dllmsgRxPtr != 0 ) {
		char timeStr[30];
		Name message;
		message = "<";
		message += _objectName;
		message += ">";
		Name actionNm;
		actionstr.element(0,'/',actionNm);
		pState = static_cast<State*>(_states.gimePointer(_currStateInx));
		if ( !pState->actionExists(actionNm) )
		{
			message += " can not execute action <";
			message += actionNm;
			message += "> in state <";
			message += pState->stateName();
			message += ">";
			if ( Msg::dllmsgRxPtr != 0 ) (*Msg::dllmsgRxPtr)(message.getString());
			if (dbg > 0)
			{
				gime_date_time(timeStr);
		  		cout << timeStr << " - " << message.getString() << endl;
			} 
		}
		else
		{
			Name whatKind = "";
			if ( hpFlag == 1 ) { whatKind = " high priority"; }
			message += " executing";
			message += whatKind;
			message += " action <";
			message += actionstr;
			message += ">";
			if ( Msg::dllmsgRxPtr != 0 && hpFlag == 0 ) (*Msg::dllmsgRxPtr)(message.getString());
			if ( (dbg == 1 && hpFlag == 0) || (dbg == 2 && hpFlag == 0) || dbg >= 3)
			{
				gime_date_time(timeStr);
		  		cout << timeStr << " - " << message.getString() << endl;
			} 
		}
	}
      

   _currentActionString = actionstr;


//
//------------
   int flag;

   if ( _associated == 0 ){             // Logical object
        _pCommHandler->publishBusy(this,actionstr.getString());
        executeState();
        return;
   }
   else {                               // Associated object
         pState = static_cast<State*>(_states.gimePointer(_currStateInx));
         flag = pState->execute_assoc( _currentActionString, _pCommHandler);
         if ( flag == -1 ) {
              _pCommHandler->invalidAction(this);
// Make the object executable again if there actions queued
              if ( !_hpActionQ.isEmpty() || !_actionQ.isEmpty() ) {
                   executableObjectQ.add(_objectName);
              }
              return;
         }
         strcpy(_internalState,"WaitSetState"); // valid action was successfully
//                                                 dispatched
#ifdef DEBUG
         print_obj(_objectName);
         cout << " sends action to the remote object \n";
         cout.flush();
#endif
   }

 return;

}
//
//
//-------------------------------------------------------------------------
//
void SMIObject::resume() {
//debug beg
	if ( dbg > 5 )
	{
		print_obj(_objectName);
		cout << " resuming action " << _currentActionString << endl;
	}
//debug end

   if ( strcmp(_internalState,"Suspended") ) {
       cout << " the object is not in Suspended internal state\n";
       cout.flush();
       abort();
   }
	_suspendedInsType = "";
	_pointerToTypeISuspendedIF = NULL;
	_pointerToSuspendedWAIT = NULL;
	_pointerToSuspendedSLEEP = NULL;

   executeState();
   return;   
}

//========================== executeWhensFromFlagList =====  BF  May 2008  ==============

void SMIObject::executeWhensFromFlagList
                ( const std::vector<int>& flagList ) 
{
    
/* This makes sence only for Logical objects */
	if (_associated != 0) { return;}
   
	State* pState;

/*  check if in a correct internal state  */
	if ( strcmp(_internalState,"Idle") && strcmp(_internalState,"Locked")) {
//      cout << "  Whens for object : " << _objectName <<
//              " ...is not in a stable state to execute whens \n";
		return;  
	}
//---- Object is either Idle or Locked

	if ( !_hpActionQ.isEmpty() )
	{ //	cout << " There is already HP action queued" << endl;
		return;
	}

	pState = static_cast<State*>(_states.gimePointer(_currStateInx));
//   cout << "  Whens for object : " << _objectName <<
//           "  in state : " << pState->stateName() << "\n";
	pState->executeWhensFromFlagList( flagList );
	return;
}

//========================== executeAllWhens =====  BF  May 2008  ==============

void SMIObject::executeAllWhens() 
{
    
/* This makes sence only for Logical objects */
	if (_associated != 0) { return;}
   
	State* pState;

/*  check if in a correct internal state  */
	if ( strcmp(_internalState,"Idle") && strcmp(_internalState,"Locked")) {
//      cout << "  Whens for object : " << _objectName <<
//              " ...is not in a stable state to execute whens \n";
		return;  
	}
//---- Object is either Idle or Locked

	if ( !_hpActionQ.isEmpty())
	{ //	cout << " There is already HP action queued" << endl;
		return;
	}

	pState = static_cast<State*>(_states.gimePointer(_currStateInx));
//   cout << "  Whens for object : " << _objectName <<
//           "  in state : " << pState->stateName() << "\n";
	pState->executeAllWhens();
	return;
}

//*****************************  queueAction *******************************

void SMIObject::queueAction ( Name& actionstr ) {

//debug beg
if ( dbg > 4 )
{
	print_obj(_objectName);
	cout  << " queuing actionString :" << actionstr << endl;
}
//debug end

   _actionQ.add(actionstr);
//   _actionQ.out();

   executableObjectQ.add(_objectName);

 return;

}


void SMIObject::queueAction ( char* actionstr ) {
   Name temp;
   temp = actionstr;
   queueAction(temp);
} 

//*****************************  queueHpAction *******************************

void SMIObject::queueHpAction ( Name& action_nm ) {
//   print_obj(_objectName);
//   cout  << " queuing high priority action :" << action_nm << "\n";
//   cout.flush();

   _hpActionQ.add(action_nm);

   executableObjectQ.add(_objectName);

   return;
}

void SMIObject::queueHpAction ( char* action_nm ) {
   Name temp;
   temp = action_nm;
   queueHpAction(temp);
   return;
}


//*****************************  lock   *******************************

int SMIObject::lock ( char* objname ) {
#ifdef DEBUG
   cout << "  Object : " << _objectName;
   cout.flush();
#endif
   if ( _hpActionQ.isEmpty() && _actionQ.isEmpty() ) {   // action queues empty

        if ( !strcmp(_internalState,"Idle")  || 
             !strcmp(_internalState,"Locked")) {
             _lock++;
             strcpy(_internalState,"Locked");

//             print_obj(_objectName);
//             cout << " locked by " << objname << "\n";
//             cout.flush();

             return 1;
        }
   }

   Name lockAction = "&lock/";
   Name objnameUpCase = objname;
   objnameUpCase.upCase();
   lockAction += objnameUpCase;
    _actionQ.add(lockAction);
    executableObjectQ.add(_objectName);

//    print_obj(_objectName);
//    cout << " lock queued by " << objname << "\n";
//    cout.flush();

    return 0;
}


//*****************************  unlock   *******************************

void SMIObject::unlock ( ) {

   if ( strcmp(_internalState,"Locked") ) {
      print_obj(_objectName);
      cout << " is not locked\n";
      cout.flush();
      exit(1);
   }

   _lock = _lock - 1;

   if ( _lock == 0 ) {
        strcpy(_internalState,"Idle");
   }

   executableObjectQ.add(_objectName);

//   print_obj(_objectName);
//   cout << " unlocking, Internal state after:" << _internalState << endl;
   cout.flush();

   return ;

}

//*****************************   name   ***********************************

int SMIObject::name ( char* obj_nm, int *numSubobj ) {

   _objectName.whatAreYou(obj_nm,MAXRECL);
   *numSubobj = _numOfSubobjects;

   return(_associated);

}

Name& SMIObject::name () {
return _objectName;
}
//------------------------  numSubobj() --------------
  int SMIObject::numSubobj() const {
     return _numOfSubobjects;
  }
//------------------------ associated() --------------  B. Franek  4-Feb-1999 -
  bool SMIObject::associated() const {
     return _associated;
  }
//-------------------------------------------------------------
  bool SMIObject::hasAssociatedAttribute() const {
      int numAt = _attributes.length();
      if ( numAt <= 0 ) { return 0; }
      
      for ( int i=0; i < numAt; i++ ) {
          if ( _attributes[i] == "ASSOCIATED" ) { return 1; }
      }
      
      return 0;
  }
//************************************************************************
Name SMIObject::externDomain () const {

  Name domain = "\0", tempName;

  tempName = _objectName;

  char* ptnr = strstr(&tempName[0],"::");

  if (ptnr) {
     *ptnr = '\0';
     domain = &tempName[0];
  }
  return domain;
}

//---------------------------- subobjectName ------ B.Franek  4-Feb-1999 ---
  Name SMIObject::subobjectName(const int index) const {
     return _subobject[index];
  }
//-------------------------------------------------------------------------

Name SMIObject::currentState() const {
    State* pState;
    pState = static_cast<State*>(_states.gimePointer(_currStateInx));
    return pState->stateName(); 
}

//*****************************  currentState  ****************************

State* SMIObject::pCurrentState() const {

    return  static_cast<State*>(_states.gimePointer(_currStateInx));

}
//*****************************  currentState  ****************************

int SMIObject::currentState ( Name& stateNm ) {

//  check if in a correct internal state  
   if ( isBusy() ) {
      return -1;   // The object is busy  
   }

    State* pState;
    pState = static_cast<State*>(_states.gimePointer(_currStateInx));

    
    stateNm = pState->stateName();

   return 1;

}

//================================ isBusy =========== BF 03-Oct-2004 =======
bool SMIObject::isBusy() const {
    if ( strcmp(_internalState,"Idle") && strcmp(_internalState,"Locked")) {
      return 1;   // The object is busy  
    }
    return 0;
}
//================================ hpActionsPending() ===== BF 03-Oct-2004 ===
bool SMIObject::hpActionsPending() {
    if (_hpActionQ.isEmpty()) {return 0;}
    return 1;
}
//---------------------------  executeState  --------------------------------
void SMIObject::executeState() {
//
    State* pState;
    pState = static_cast<State*>(_states.gimePointer(_currStateInx));
        int flag;
        Name newState;
//
        flag = pState->execute(_currentActionString, newState);
//           flag is either -1..action not found
//                           1..action terminated
//                           2..action suspended
//             cout << " flag : " << flag << "  newState : |" 
//                                      << newState << "| \n";  
//--------------
        switch (flag) {                             
        case 1 :                                     //--- action terminated
//             print_obj(_objectName);
//             cout << " action " << _currentActionString 
//                  << " terminated in state " << newState << endl; cout.flush();
//           first find out which state it is
             _currStateInx = stateInx(newState.getString());
//             cout << " _currStateInx = " << _currStateInx << " \n";
             if ( _currStateInx == -1 ) {    
                print_obj(_objectName);
                cout << " Action terminated with state :|" << newState << "| \n"
                     << " The object however does not have such state \n";
                cout.flush();
                abort();
             }
// add the state change (can be the same state) to the state q
//             cout << " StateQ before : \n";
//             _pStateQ->out();
             { char objnm_temp[MAXRECL];
             _objectName.whatAreYou(objnm_temp, MAXRECL);
             _pStateQ->add(objnm_temp,newState.getString());}
#ifdef DEBUG
              print_obj(_objectName);
              cout << " end state " << newState << " ->StateQ \n";
              cout.flush();
#endif
//             cout << " \n StateQ after : \n";
//             _pStateQ->out();
// and wait for it
             strcpy(_internalState,"WaitSetState");
             break;
        case 2 :                                       //----- suspended
//             print_obj(_objectName);
//             cout << " action " << _currentActionString 
//                  << " suspended " << endl; cout.flush();
             strcpy(_internalState,"Suspended");
             break;
        case -1 :                                      //------- action not exists
              _pCommHandler->invalidAction(this);
// Make it executable again if there actions queued
              if ( !_hpActionQ.isEmpty() || !_actionQ.isEmpty() ) {
                   executableObjectQ.add(_objectName);
              }
              return;
              break;
        default :
             print_obj(_objectName);
             cout << " flag = " << flag << "  this is an ilegal value \n";
             cout.flush();
             exit(1);
             break;
        }
//
        return;
}

//------------------------ parmString -----------  B.Franek   4-Feb-1999 ---
   Name SMIObject::parmString() const {
      Name tempName;
      _objectParameters.buildParmString(tempName);
      return tempName;
   }

void SMIObject::parmString(Name& str) const {

     _objectParameters.buildParmString(str);
}

void SMIObject::parmString(char* str, int mxsize) const {
    Name tempName;

    parmString(tempName);
    tempName.whatAreYou(str,mxsize);
}

Parms* SMIObject::pObjectParameters() {
    return &_objectParameters;
}

void SMIObject::split
           ( const Name& statestr, Name& stateNm, Name& paramStr) const {
    Name ststr = statestr;

    char* ptnr = strstr(&ststr[0],"/");

    if ( ptnr ) {  // state string contains /   -> it has parms
       *ptnr = '\0';
       stateNm = ststr;
       *ptnr = '/';
       paramStr = ptnr;
    }
    else {
       stateNm = statestr;
       paramStr = "\0";
    }

    return;
} 

//----------------------------------------------------------------------------
  int SMIObject::stateInx( const Name& stname) const {

     int inx = -1;
     int numOfStates = _states.length();
     State* pState;
     
     for ( int i=0; i < numOfStates; i++ ) {
        pState = static_cast<State*>(_states.gimePointer(i));
        if ( stname == pState->stateName() ) { inx = i; break; }
     }

     return inx;
  } 


  int SMIObject::stateInx( const char stname[]) const {
     Name temp = stname;

     int inx = stateInx(temp);

     return inx;
  }

//---------------------------------------------------------------------------
  State* SMIObject::pTaggedState (const Name& tagname) const {
    int numOfStates = _states.length();
    State* pState;
    
     for ( int i=0; i < numOfStates; i++ ) {
         pState = static_cast<State*>(_states.gimePointer(i));
        if ( tagname == pState->tagName() ) { return pState; }
     }

     return 0;
  }
//-----------------------------------------------BF 27-July-2009 -------------------

  int SMIObject::stateInxUndeclaredState() const {
     int numOfStates = _states.length();
     State* pState;
     
     for ( int i=0; i<numOfStates; i++ ) {
        pState = static_cast<State*>(_states.gimePointer(i));
        if ( pState->undeclaredState() ) { return i; }
     }

     return -1;
  }
//---------------------------------------------------------------------------

  int SMIObject::stateInxDeadState() const {
     int numOfStates = _states.length();
     State* pState;
     
     for ( int i=0; i<numOfStates; i++ ) {
        pState = static_cast<State*>(_states.gimePointer(i));
        if ( pState->deadState() ) { return i; }
     }

     return -1;
  }
//-----------------------------------------------------BF 11-Oct-1998-------

  int SMIObject::stateInxDeadState(const int isub) const {
     State* pState;
     
     if ( isub >= _numOfSubobjects ) {
        cout << " Illegal subobobject index " << endl; cout.flush(); abort();
     }

     for ( int ist=0; ist < _numOfSubobjStates[isub]; ist++ ) {
         int inx = _stateIndex[isub]+ist;
         pState = static_cast<State*>(_states.gimePointer(inx));
        if ( pState->deadState() ) {
           return inx;
        }
     }
     return -1;
  }
//-----------------------------------------------------BF 11-Oct-1998-------

  int SMIObject::stateInxInitialState(const int isub) const {
     State* pState;

     if ( isub >= _numOfSubobjects ) {
        cout << " Illegal subobobject index " << endl; cout.flush(); abort();
     }

     for ( int ist=0; ist < _numOfSubobjStates[isub]; ist++ ) {
        int inx = _stateIndex[isub]+ist;
	pState = static_cast<State*>(_states.gimePointer(inx));
        if ( pState->initialState() ) {
           return inx;
        }
     }
     return -1;
  }

//-------------------------------------------------------------------------

  int SMIObject::stateInxTaggedState() const {
     Name tagname("&VARIABLE");
     int numOfStates = _states.length();
     State* pState;

     for ( int i=0; i < numOfStates; i++ ) {
         pState = static_cast<State*>(_states.gimePointer(i));
        if ( tagname == pState->tagName() ) { return i; }
     }

     return -1;
  }

//-------------------------------------------------------------------------

  void SMIObject::setStateAlloc( const Name& statestr ) {
     Name stateNm,paramStr;
     State* pState;

     split(statestr,stateNm,paramStr);

     if ( stateNm == "$DEAD" ) {
        _currStateInx = stateInxDeadState();
        if (_currStateInx < 0 ) {
           cout << " Dead state has to be declared " << endl; cout.flush();
           abort();
        }
     }
     else {
        if ( stateNm == "NOT_ALLOCATED" ) {
           _currStateInx = stateInx(stateNm);
        }
        else {
           _currStateInx = stateInxTaggedState();
	   pState = static_cast<State*>(_states.gimePointer(_currStateInx));
           pState->changeName(stateNm);
        }
     }

//   Having accepted setState, the only possible internal state is "Idle"
//
     strcpy(_internalState,"Idle");
//
//   If either of the actions queues has actions, make it executable
     if ( !_hpActionQ.isEmpty() || !_actionQ.isEmpty() ) {
        executableObjectQ.add(_objectName);
     }

  }

//--------------------------------------------------------  BF 14-Oct-1998 --

   void SMIObject::setCurrStateInxes( const Name& stateNm ) {

      if ( _numOfSubobjects <= 1 ) {
         _currStateInxes[0] = stateInx(stateNm);
         _currStateInx = _currStateInxes[0];
         if ( _currStateInx < 0 ) {
            print_obj(_objectName);
            cout << " does not have state " << stateNm << " declared" << endl;
            cout.flush();
            abort();
         }
         return;  
      }
      else {
		cout <<  "  *** SMIObject::setCurrStateInxes( const Name& stateNm )" << endl;
		cout <<  "    The number of subobjects must be <= 1 " << endl;
		abort();
		return ; 
      }
   }

//------------------------------------------ BF  9-Apr-2008 -------------------
int SMIObject::addClientWhen(const char* whenObjName, const char* whenStateName, int whenInx)
{
	return _clientWhens.markWhen(whenObjName, whenStateName, whenInx);
}
//------------------------------------------ BF  11-Apr-2008 --------------
int SMIObject::updateReferencedObjectsandSets() const
{
	int numOfStates = _states.length();
	int ist;
	State* pState;
	
	for ( ist=0; ist<numOfStates; ist++)
	{
		pState = static_cast<State*>(_states.gimePointer(ist));
//      cout << pState << "  " << pState->stateName() << "\n";
		pState->updateReferencedObjectsandSets();
	}

	return 1;
}
//------------------------------------------  Bf  11-Apr-2008  --------------
int SMIObject::printClientWhens() const
{
	int numClientObjects =_clientWhens.numObjects();
	if ( numClientObjects == 0 )
	{
		cout << " No clients " << endl;
		return 1;
	}
	
	_clientWhens.out(" ");
	return 1;
}
//------------------------------------------  BF  8-May-2008  -----------------
int SMIObject::printSetNamesBelongingTo() 
{
	int numOfSets = _setsIBelongTo.numOfEntries();
	if (numOfSets <= 0)
	{
		cout << " No sets registred" << endl;
		return 1;
	}
	
	//Name temp(" ");
	_setsIBelongTo.out(); cout << endl;
	return 1;
} 
//------------------------------------------  BF  14-May-2008  ----------------
int SMIObject::gimeCurrentClientWhens( ClientWhens& clientWhens ) 
{
	// It is assumed that clientWhens are declared in calling function.
	// First we include the object 'static' client whens.
	
	clientWhens.mergeIn(_clientWhens);

	// next we have to merge-in all the whens of the object sets to which
	// the current object belongs.
	
	int numOfSets = _setsIBelongTo.numOfEntries();
	if (numOfSets <= 0)
	{
		return 1;
	}
	
	// Loop now over the sets
	NmdPtnr set;
	SMIObjectSet* pObjSet;
	
	
	_setsIBelongTo.reset();	
	while ( _setsIBelongTo.nextItem(set) )
	{
		pObjSet = static_cast<SMIObjectSet*>(set.pointer());
		const ClientWhens& setClientWhens = pObjSet->gimeClientWhensRef();
		clientWhens.mergeIn( setClientWhens );
	}
	return 1;
}
//------------------------------------------------ BF Sep 2008 ---------------
void SMIObject::setPointerToTypeISuspendedIF( IfIns* pointer)
{
	_pointerToTypeISuspendedIF = pointer;
	
	_suspendedInsType = "IF";
	return;
}
//------------------------------------------------ BF Sep 2008 ---------------
IfIns* SMIObject::gimePointerToTypeISuspendedIF() const
{
	return _pointerToTypeISuspendedIF;
}
//------------------------------------------------ BF Sep 2008 ---------------
int SMIObject::removeQueuedLock(const Name& requestorName) 
{

//debug beg
if ( dbg > 5 )
{
	cout << endl << "start ===============  removeQueuedLock  of  " << requestorName  << endl;

	cout << " actionQ of " << _objectName << " before " << endl; _actionQ.out(); cout << endl;
}
//debug end

	Name requestorNameUpCase = requestorName;
	requestorNameUpCase.upCase();
	
	Name lockAction ="&lock/";
	lockAction += requestorNameUpCase;
	
	if ( !_actionQ.removeItem(lockAction) )
	{
		cout << " *** Internal error : "
		<< " Object : " << _objectName
		<< " failed to remove : " << lockAction << endl;
		abort();
	}

//debug beg
if ( dbg > 5 )
{	
	cout << endl << " actionQ after " << endl; _actionQ.out(); cout << endl;
	cout << "return ===============  removeQueuedLock  of  " << requestorName  << endl << endl;
}
//debug end

	return 1;	
}
//------------------------------------------------ BF Jan 2009 ---------------
void SMIObject::setPointerToSuspendedWAIT( WaitIns* pointer)
{
	_pointerToSuspendedWAIT = pointer;
	
	_suspendedInsType = "WAIT";
	return;
}
//------------------------------------------------ BF Jan 2009 ---------------
void SMIObject::setPointerToSuspendedSLEEP( SleepIns* pointer)
{
	_pointerToSuspendedSLEEP = pointer;
	
	_suspendedInsType = "SLEEP";
	return;
}
//----------------------------------------------------------------------------
Name& SMIObject::suspendedInsType()
{
	return _suspendedInsType;
}
//---------------------------------------------------------------------------
void SMIObject::reportingLockedObject(Name& lockedObject)
{
if (dbg > 5 )
{
	cout << endl << " start========== SMIObject(" << _objectName 
	     << ")::reportingLockedObject(" << lockedObject << ")" << endl;
}

	int flg;	
	if ( _suspendedInsType == "WAIT" )
	{
		flg =
		 _pointerToSuspendedWAIT->reportingLockedObject(lockedObject);
	}
	else if ( _suspendedInsType == "IF" )
	{
		flg =
		 _pointerToTypeISuspendedIF->reportingLockedObject(lockedObject);
	}
	else
	{
		cout << " *** Internal fatal error " << endl
		<< " method SMIObject::reportingLockedObject of object " << _objectName
		<< endl << " suspended instruction type " << _suspendedInsType
		<< " is not legal" << endl; abort();
	}
	
	if ( flg == 1)
	{  // this means that there are no more objects waiting for locks and
	   // the instruction can be executed.
		resumeHandler.objectReadyToResume(_objectName);
	}

if (dbg > 5 )
{
	cout << " end============= SMIObject::reportingLockedObject" << endl;
}	
	return;
}
//---------------------------------------------------------------------------
int SMIObject::reportingObjectRemovedFromSet
	               ( const Name& remObjName, const Name& setName)
{
if (dbg > 5 )
{
	cout << endl << " start========== SMIObject(" << _objectName 
	     << ")::reportingObjectRemovedFromSet" << endl
	      << remObjName << " removed from " << setName << endl;
}

	int flg;	
	if ( _suspendedInsType == "WAIT" )
	{
		flg =
		 _pointerToSuspendedWAIT->reportingObjectRemovedFromSet
		                    (remObjName,setName);
	}
	else if ( _suspendedInsType == "IF" )
	{
		flg =
		 _pointerToTypeISuspendedIF->reportingObjectRemovedFromSet
		                    (remObjName,setName);
	}
	else if ( _suspendedInsType == "SLEEP" ) { return 0; }
	else
	{
		cout << " *** Internal fatal error " << endl
		<< " method SMIObject::reportingLockedObject of object " << _objectName
		<< endl << " suspended instruction type " << _suspendedInsType
		<< " is not legal" << endl; abort();
	}

if (dbg > 5 )
{
	cout << " end============= SMIObject::reportingLockedObject" << endl;
}	
	return flg;
}
//---------------------------------------------------------------------------
void SMIObject::youJoinedSet( SMIObjectSet *pSet)
{
	Name setName;
	setName = pSet->name();
	void *pVoid;
	pVoid = static_cast<void*>(pSet);
		
	NmdPtnr temp(setName,pSet);
	
	_setsIBelongTo.add(temp);
	
	return;
}
//---------------------------------------------------------------------------
void SMIObject::youLeftSet( SMIObjectSet *pSet)
{
	Name setName;
	setName = pSet->name();
	void *pVoid;
	pVoid = static_cast<void*>(pSet);

	
	NmdPtnr temp(setName,pSet);

	_setsIBelongTo.remove(temp);
	
	return;
}
//----------------------------------------------------------------------------
bool SMIObject::memberOfSet(const Name& setNm)
{

//cout << endl << "start============= SMIObject::memberOfSet ===========" << endl;
//cout << " object " << _objectName << "   set " << setNm << endl;

	void* ptnr = _setsIBelongTo.pointer(setNm);
	
//Name temp(" "); _setsIBelongTo.out(temp); cout << " ptnr " << ptnr << endl;
	
	if (ptnr) { return true; }
	else      { return false; }
	
}
//---------------------------------------------------------------------------
int SMIObject::getObjectParameter(const Name& name, Name& value, Name& typeNm) const
{
	int flg;
	char type[10];
	
	flg = _objectParameters.get(name,value,type);
	if ( flg ) { typeNm = type; }
	return flg;
}
//-----------------------------------------------------------------------------
int SMIObject::setObjectParameter(const Name& name, const Name& value)
{
	int flg;
	
	flg = _objectParameters.set(name,value);
	return flg;
}
