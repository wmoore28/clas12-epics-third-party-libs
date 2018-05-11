//-------------------------- Action Class ----------------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <assert.h>

#include "parameters.hxx"
#include "action.hxx"
#include "smiobject.hxx"
#include "inslist.hxx"

extern int dbg;
//
//                                                  Author: B. Franek
//                                                  Date : 23 September 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------

Action::Action
      ( char lines[][MAXRECL],Name& action_nm,int *no_lines, 
        SMIObject *pobj, State* pstat){
//
// First, the Action class is instantiated and as a part of its initialisation,
// the Instruction block number 0 is also instantiated. Even though the action
// may contain many blocks, it points directly only to this block (0).
// The other blocks of the action are accessed hirachically from block(0)

   int NoOfBlocks;
   PtrVector allBlocks;  // pointers to all the blocks 
                                    // of the current action
    InsList* pblock;

   _objName = pobj->name();
   _pParentObject = pobj;
   _pParentState = pstat;

   action_nm = lines[1];

   _actionName = action_nm; 


#ifdef DEBUG
   cout << "    action : " << _actionName << "\n";
   cout.flush();
#endif

   int numOfPar;
   sscanf(lines[2],"%d",&numOfPar);

   int lin = 3;

   if ( numOfPar > 0 ) {
      char par_type[MAXRECL];
      char par_name[MAXRECL];
      char par_default[MAXRECL];
      strcpy(par_type,lines[lin]);
//      sscanf(lines[lin],"%s",par_type);
      if ( !strcmp(par_type,"STRING") ||
           !strcmp(par_type,"INT") ||
           !strcmp(par_type,"FLOAT")      ) { // New format incl the types
         for ( int ip = 0; ip < numOfPar; ip++) {
	     strcpy(par_type,lines[lin]);
//             sscanf(lines[lin],"%s",par_type); 
             lin++;
	     strcpy(par_name,lines[lin]);
//             sscanf(lines[lin],"%s",par_name); 
             lin++;
	     strcpy(par_default,lines[lin]);
//             sscanf(lines[lin],"%s",par_default);
             lin++;
             if ( !strcmp(par_default,"&nodefault") ) {
                _declaredParms.add(par_name,par_type);
             }
             else {
                _declaredParms.add(par_name,par_default,par_type);
             }
         }
      }
      else {  //old type without the type info....all parameters are strings
         strcpy(par_type,"STRING");
         for ( int ip = 0; ip < numOfPar; ip++) {
	     strcpy(par_name,lines[lin]);
//             sscanf(lines[lin],"%s",par_name); 
             lin++;
	     strcpy(par_default,lines[lin]);
//             sscanf(lines[lin],"%s",par_default);
             lin++;
             if ( !strcmp(par_default,"&nodefault") ) {
                _declaredParms.add(par_name,par_type);
             }
             else {
                _declaredParms.add(par_name,par_default,par_type);
             }
         }
      }
#ifdef DEBUG
      cout << " Action " << _actionName << "  declared parameters :" << endl;
      _declaredParms.out();
      cout.flush();
#endif
   }
   

  NoOfBlocks = 0;

//-----------  First loop over the action lines and instantiate all blocks ----
  for (int ii=lin;;ii++){
    if (!strcmp(lines[ii],"*END_ACTION")) {
       break;
    }
    else if (!strcmp(lines[ii],"*BLOCK")) {
// Now read two integers, making sure they are interpreted as decimal
      ii++;
      int curr_block,curr_level;
      sscanf(lines[ii],"%d %d",&curr_block,&curr_level);
//      cout << " Block :  " << curr_block << " level " << curr_level
//           << endl;
      assert ( curr_block == NoOfBlocks );

      
      pblock = new InsList
          (curr_block,curr_level,_pParentObject,_pParentState,this);
      assert( pblock != 0 );
      allBlocks+= pblock;
      NoOfBlocks++;
    }
  }
   


   if ( NoOfBlocks == 0 ) {
      _pblock = 0;
   }
   else {
      _pblock = static_cast<InsList*>(allBlocks[0]);
   }


//---------------------- Loop over the action lines ---------------------------

//cout << endl << _actionName << endl;
  int blkno;
  blkno = 0;

  for (int il=lin;;il++){
    if (!strcmp(lines[il],"*END_ACTION")) {
       *no_lines = il+1;
       break;
    }
    else if (!strcmp(lines[il],"*BLOCK")) {
      int no_blklines;
      pblock = static_cast<InsList*>(allBlocks[blkno]);
      pblock->initialise(&lines[il],allBlocks,no_blklines);
//      cout << endl << "Block : " << blkno << "  " << pblock  << endl;
//      pblock->listInstructions();
      blkno++;
      il = il + no_blklines -1;
    }
    else {
      cout << " Action::Action  funny line encountered \n"
           << "|" << lines[il] << "|\n";
      cout.flush();
      exit(1);
    }      
  }
//------------------------------------------------------------------------


  return;
}  


void Action::whatAreYou() {
  cout << "\n";
  cout << "    action : " << _actionName << "\n \n";
  cout.flush();

  if ( _pblock == 0 ) {return;}

  _pblock->listInstructions();
  return;
}



//--------------------------------  execute  -----------------------------------
int Action::execute( Name& endState, const Name& actionstr) {
//

#ifdef DEBUG
  cout << "\n" << " Executing action : " << _actionName << "\n \n";

  Parms temppar;
  temppar.initFromParmString(actionstr);


  if ( temppar.numOfEntries() > 0 ) {
     cout << " Incomming action Parameters :\n";
     temppar.out();
  }
  cout << "\n";
  cout.flush();
#endif
//


  if ( _declaredParms.numOfEntries() > 0 ) {
#ifdef DEBUG
     cout << " Declared Parameters:" << endl;
     _declaredParms.out();
     cout << "\n";
     cout.flush();
#endif
     _currentParameters = _declaredParms;
     _currentParameters.setFromParmString(actionstr);
     if ( !_currentParameters.allHaveValues() ) {
        cout << " There are parameters without a default value and not supplied"
             << endl;
        cout << "\n";
        cout.flush();
     }
#ifdef DEBUG
     cout << " Current Parameters : " << endl;
     _currentParameters.out();
     cout << "\n";
     cout.flush();
#endif
  }

// temp bodge

   if ( _objName == "&ALLOC" ) 
   { _currentParameters.initFromParmString(actionstr);}

//-----------------------------------------

	if ( _objName == "&DOMAIN" )
	{
		dealWithDomainCommands(actionstr);
	}


  int temp;
  
  if (_pblock == 0) { return 0;}   // some crazy guys have no instructions
  temp = _pblock->execute(endState);
  

  return temp;
}


//------------------------------- pCurrentParameters -------------------------
//

Parms* Action::pCurrentParameters()  {
   return &_currentParameters;
}
//------------------------------- pDeclaredParameters -------------------------
//

Parms* Action::pDeclaredParms() {
   return &_declaredParms;
}
//---------------------------- actionString -------------------------------
//  action/par1/par2=/par3
//
void  Action::actionString (Name& actionstr) {
//
   actionstr = _actionName;

   if ( _declaredParms.numOfEntries() > 0 ) {
      Name parmString;
      _declaredParms.buildParmString(parmString);
      actionstr += parmString;
   }
//
   return;
}
//---------------------------------------------------------------------------
int Action::getActionParameter(const Name& name, Name& value, Name& typeNm) const
{
	int flg;
	char type[10];
	
	flg = _currentParameters.get(name,value,type);
	if ( flg ) { typeNm = type; }
	return flg;
}
//-----------------------------------------------------------------------------
Name& Action::actionName()
{
	return _actionName;
}
//-----------------------------------------------------------------------------
void Action::dealWithDomainCommands(const Name& actionstr)
{
	if ( _actionName == "DIAG" ){}
	else 
	{
		cout << endl << " Action " << _actionName 
		<< " is not recognised action for object &DOMAIN" << endl;
		return;
	}
	
	_currentParameters.initFromParmString(actionstr);
		
	
	Name parNm("LEVEL"), parVal, parType;
	int flg = getActionParameter(parNm,parVal,parType);
	if ( flg )
	{
		int ipom;
		cout << endl << "  ";
		//		cout << parNm << "  " << parVal << "  " << parType << endl;
		flg = sscanf(parVal.getString(),"%d",&ipom);
		if ( flg==1 )
		{
			if ( ipom >= 0 && ipom < 10 )
			{
				dbg = ipom;
				cout << endl << endl 
				<< " Diagnostic level changed to " << dbg << endl<< endl;
				return;
			}
			else
			{
				cout << endl 
				<< " **** unrealistic debugging level requested " 
				<< parVal << endl << endl;
				return;
			}
		}
		else
		{
			cout << endl << " ***** Failure to read " << parVal << endl << endl;
			return;
		}
		  
	}
	else
	{
		cout << endl 
		<< " *****  Parameter 'LEVEL' was not found with action 'DIAG'"
		<< endl << endl;
	}
		
	return;	
}
