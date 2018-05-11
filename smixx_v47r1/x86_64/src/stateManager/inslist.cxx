//------------------------- InsList Class ------------------------------------
#include <stdlib.h>
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>

#include "inslist.hxx"
#include "doins.hxx"
#include "insertins.hxx"
#include "termins.hxx"
#include "ifins.hxx"
#include "setins.hxx"
#include "waitins.hxx"
#include "sleepins.hxx"
#include "createobjectins.hxx"
#include "ut_sm.hxx"

extern int dbg;
//                                                             B. Franek
//                                                             April 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------
//
InsList::InsList
    (int blknum, int lev,
     SMIObject *pobj, State* pstat, Action* pact){
  
  _pParentObject = pobj;
  _block_id = blknum;
  _level = lev;
  _isuspended = 0;

  _pParentAction = pact;
  _pParentState = pstat;
}

void InsList::initialise( char lines[][MAXRECL], PtrVector& allBlocks, 
                          int& no_lines){

  int curr_block, curr_level;   // block number and level of the block
                                // which is being processed

  sscanf(lines[1],"%d %d",&curr_block,&curr_level);
//  cout << " Block :  " << curr_block << " level " << curr_level << "\n";


  for (int il=2;;il++){
//    cout << lines[il] << "\n";
    if (!strcmp(lines[il],"*END_ACTION")) {
       no_lines = il;
       break;
    }
    else if (!strcmp(lines[il],"*BLOCK")) {
       no_lines = il;
       break;
    }

    else if (!strcmp(lines[il],"do")) {             //-------- do instruction
      int no_dolines;

      DoIns *pDoIns;
      pDoIns = new DoIns
            (&lines[il],curr_level,&no_dolines,
	    _pParentObject,_pParentState,_pParentAction);
      
      _instructions += pDoIns;
      il = il + no_dolines - 1;
    }

    else if (!strcmp(lines[il],"insert")) {             //-------- insert instruction
      int no_insertlines;

      InsertIns *pInsertIns;
      pInsertIns = new InsertIns
            (&lines[il],curr_level,no_insertlines,
	    _pParentObject, _pParentState, _pParentAction);
      
      _instructions += pInsertIns;
      il = il + no_insertlines - 1;
    }

    else if (!strcmp(lines[il],"create_object")) {             //-------- insert instruction
      int no_create_objectlines;

      CreateObjectIns *pCreateObjectIns;
      pCreateObjectIns = new CreateObjectIns
            (&lines[il],curr_level,no_create_objectlines,
	    _pParentObject, _pParentState, _pParentAction);
      
      _instructions += pCreateObjectIns;
      il = il + no_create_objectlines - 1;
    }


    else if (!strcmp(lines[il],"terminate")) {      //-------- terminate instr.
//      cout << "terminate encountered\n";
      il++;
      int no_termlines;

      TermIns *pTermIns;

      pTermIns = new TermIns(&lines[il],curr_level,
                 _pParentObject,_pParentState,_pParentAction,no_termlines);
     
	_instructions += pTermIns;
      il = il + no_termlines - 1;
    }

    else if (!strcmp(lines[il],"if")) {             //-------- if instruction
//      cout << "if encountered\n";
      il++;
      int no_iflines;

      IfIns *pIfIns;
      pIfIns = new IfIns
          (&lines[il],curr_level,allBlocks,no_iflines,
	  _pParentObject,_pParentState,_pParentAction);

	_instructions += pIfIns;
      il = il + no_iflines - 1;
    }
    else if (!strcmp(lines[il],"set")) {             //-------- set instruction
      int no_setlines;

      SetIns *pSetIns;
      pSetIns = new SetIns
            (&lines[il],curr_level,no_setlines,
	    _pParentObject, _pParentState, _pParentAction);

     _instructions += pSetIns;
      il = il + no_setlines - 1;
    }
    else if (!strcmp(lines[il],"wait")) {             //-------- wait instruction
      il++;
      int no_waitlines;

      WaitIns *pWaitIns;
      pWaitIns = new WaitIns
            (&lines[il],curr_level,no_waitlines,
	    _pParentObject, _pParentState, _pParentAction);

     _instructions += pWaitIns;
      il = il + no_waitlines - 1;
    }
    else if (!strcmp(lines[il],"sleep")) {             //-------- sleep instruction
      il++;
      int no_sleeplines;

      SleepIns *pSleepIns;
      pSleepIns = new SleepIns
            (&lines[il],curr_level,no_sleeplines,
	    _pParentObject, _pParentState, _pParentAction);

     _instructions += pSleepIns;
      il = il + no_sleeplines - 1;
    }

    else {
      cout << " InsList::InsList  :funny line encountered \n"
           << "|" << lines[il] << "|\n";
      cout.flush();
      exit(1);
    }      
  }
//------------------------------------------------------------------------


  return;
}

//=========================== listInstructions ===========================
void InsList::listInstructions(){
	int numIns = _instructions.length();
cout << " Num of instructions : " << numIns << "\n";
if (numIns == 0 ) {return;}
for( int i=0; i<numIns; i++) {
	void* ptnv = _instructions[i];
	Instruction* ptnI = (Instruction*) ptnv;
	ptnI->whatAreYou();
   };
}

//==============================  execute  =================================
InstructionReturnStatus_t InsList::execute( Name& endState ){
    
	int numIns = _instructions.length();
	InstructionReturnStatus_t retStatus;
	int temp;

//debug beg
if ( dbg > 5 )
{
indent(_level);
cout << endl << " start================== InsList::execute =========== " << endl;
cout << "  ---Block " << _block_id << " starting execution "  
<< "at " << _isuspended << "(" << numIns << ")"  << endl;
}
//debug end

	int i;
	for( i=_isuspended; i<numIns; i++)
	{

		void* ptnv = _instructions[i];
		Instruction* ptnI = (Instruction*) ptnv;

		temp = ptnI->execute(endState);
		retStatus = static_cast<InstructionReturnStatus_t>(temp);

		switch (retStatus)
		{
			case normal:
				_isuspended = 0;
				break;
			
			case instructionFinishesTerminatingAction:
				_isuspended = 0;
//debug beg
if ( dbg > 5 )
{
indent(_level);
cout << " block terminated by " << i << "(" << numIns << ")"
 << " instruction" << endl;
cout  << " return================== InsList::execute =========== " << endl;
}
//debug end
				return retStatus;
			
			case instructionSuspended:
				_isuspended = i;
//debug beg
if ( dbg > 5 )
{
indent(_level);
cout << " block terminated by " << i << "(" << numIns << ")"
 << " instruction" << endl;
cout  << " return================== InsList::execute =========== " << endl;
}
//debug end
				return retStatus;
			
			default:
				cout << " **** Fatal error: InsList::execute" << endl
				<<  "instruction returns illegal return status "
				 << retStatus << endl;
				 abort();    
			 			
		}			
	}
//debug beg
if ( dbg > 5 )
{
indent(_level);
cout << " block terminated by " << i << "(" << numIns << ")"
 << " instruction" << endl;
cout  << " return================== InsList::execute =========== " << endl;
}
//debug end
	return retStatus;
}

