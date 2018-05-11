//-------------------------- Condition Class ----------------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
using namespace std;
#include "namelist.hxx"
#include "condition.hxx"
#include "smpcondtyp1.hxx"
#include "smpcondtyp2.hxx"
#include "smpcondtyp3.hxx"
#include "smpcondtyp4.hxx"
#include "registrar.hxx"
//------------------------- Externals -----------------------------------
extern int dbg;
extern Registrar allSMIObjectSets;
//
//                                                              B. Franek
//                                                             12-Aug-1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------

Condition::Condition (char lines[][MAXRECL], int *no_lines,
           SMIObject* pobj, State* pstat, Action* pact){
//-------------------------------------------------------------------------
// Input :
// lines[0] ....... the first line of the condition
// Output :
// no_lines ....... number of lines in the condition
//----------------------------------------------------------------------------

	_pParentObject = pobj;
	_pParentState  = pstat;
	_pParentAction = pact;
	
   int il;

   il = 0;
//   cout << "Condition: \n";
//   cout << lines[il] << "\n";
//   cout << lines[il+1] << "\n";

   strcpy(_condition,lines[il]);

//   cout << "Condition is |" << _condition << "|\n";

   il++;
   sscanf(lines[il],"%d %d",&_noOfSmpCond,&_noOfCondIns);
   assert (_noOfSmpCond >= 0);

   assert (_noOfCondIns >= 0);
   assert (_noOfCondIns < _maxCondIns); 

   il++;

	Name objNm; Name setNm;
	SmpCond* pSmpCond;

	if ( _noOfSmpCond > 0 ) {
		int typ;
		SmpCondTyp1* pType1; SmpCondTyp2* pType2;
		SmpCondTyp3* pType3; SmpCondTyp4* pType4; 
		int no_smpcond_lines;
		
		for (int ismp=0; ismp < _noOfSmpCond; ismp++) {

//         cout << lines[il] << "\n";
			sscanf(lines[il],"%d",&typ);
			if (typ == 1) {
	 			pType1 = new SmpCondTyp1(&lines[il],no_smpcond_lines);
				pSmpCond = pType1;
			}
			else if (typ == 2) {
	 			pType2 = new SmpCondTyp2(&lines[il],no_smpcond_lines);
				pSmpCond = pType2;
			}
			else if (typ == 3) {
	 			pType3 = new SmpCondTyp3(&lines[il],no_smpcond_lines);
				pSmpCond = pType3;
			}
			else if (typ == 4) {
	 			pType4 = new SmpCondTyp4(&lines[il],no_smpcond_lines,
				             _pParentObject, _pParentState, _pParentAction );
				pSmpCond = pType4;
			}

			else {
	 		cout << " illegal simple instructionb type " << endl;
			exit(1);
			}
                        _smpConditions += pSmpCond;
			il = il + no_smpcond_lines;
			_refObjects.exclusiveUpdate(pSmpCond->refObjects());
			_refObjectSets.exclusiveUpdate(pSmpCond->refObjectSets());
		}
		
	}

/*Name offset = " ";
cout << " Server objects list " << endl; _refObjects.out(offset);
cout << " Server object sets list " << endl; _refObjectSets.out(offset);*/

   char condIns[MAXRECL];  
   char oper[4];   


   if ( _noOfCondIns > 0 ) {
      for (int icins=0; icins < _noOfCondIns; icins++) {
         strcpy(_condIns[icins],lines[il]);
//         sscanf(lines[il],"%s",_condIns[icins]);
         strcpy(condIns,_condIns[icins]);
         condIns[16] = '\0';
//         cout << condIns << "\n";
         decode_instruction(condIns, 
         oper, &_type1[icins], &_num1[icins], &_type2[icins], &_num2[icins]);
         strcpy(_oper[icins],oper);
//         cout << _oper[icins] << _type1[icins] << _num1[icins]
//              << _type2[icins] << _num2[icins] << " \n";
         il++;
      }
   }


  *no_lines = il;

// The following is just a test of simple condition function
//   if ( _noOfSmpCond > 0 ) {
//      cout << endl ;
//      for (int ismp=0; ismp < _noOfSmpCond; ismp++) {
//          pSmpCond = static_cast<SmpCond*>(_smpConditions[ismp]);
//	 cout << (pSmpCond->whatAreYou()).getString() << "\n";
//      }
//   }


  return ;
}  

//=================================================================================
void Condition::whatAreYou(int maxStrLen, char* condString) {
//   cout << " Condition::whatAreYou   not implemented yet \n";
	int condStrLen = strlen(_condition);
	
	if ( condStrLen < maxStrLen )
	{
		strcpy(condString,_condition);
	}
	else
	{
		strncpy(condString,_condition,maxStrLen);
	}
	return ;
}

//---------------------------- evaluate -----------------------------------
int Condition::evaluate() {
//
    SmpCond* pSmpCond;
   int flag;
   int ismp;
   int icins;
   vector<int> smpflags;
   int smpflag;     
   int X[_maxCondIns];
	int allGhosts = 1;
//
   if ( _noOfSmpCond == 0 ) { 
#ifdef DEBUG
        cout << "    ELSE \n";
	    cout.flush();
#endif
        flag = 1;     // else evaluates always to true
        goto Fin;        
   }  
//

   for ( ismp = 0; ismp < _noOfSmpCond; ismp++) {
#ifdef DEBUG
         cout << "    T" << ismp+1 << ": " << endl;
#endif
         pSmpCond = static_cast<SmpCond*>(_smpConditions[ismp]);
         smpflag = pSmpCond->evaluate();
	 smpflags.push_back(smpflag);
#ifdef DEBUG
	cout << " evaluates to " << smpflags[ismp] << endl;
#endif
         if (smpflag == -1 ) { 
             flag = -1;
             goto Fin;
         }
	 if (smpflag != -2 ) {allGhosts = 0;} 
   }
//
   if (allGhosts == 1) {
       flag = -2;
       goto Fin;
   }

   if ( _noOfSmpCond == 1 ) {
      flag = smpflags[0];
      goto Fin;
   }


   int oprd1,oprd2;

#ifdef DEBUG
   cout << " \n";
   cout.flush();
#endif

   for ( icins=0; icins < _noOfCondIns; icins++) {
#ifdef DEBUG
        cout << "    " << _condIns[icins];
	    cout.flush();
#endif
        if (_type1[icins] == 'T') {
            oprd1 = smpflags[_num1[icins]-1];
        }
        else {
            oprd1 = X[_num1[icins]-1];
        }
        if (_type2[icins] == 'T') {
            oprd2 = smpflags[_num2[icins]-1];
        }
        else if (_type2[icins] == 'X') {
            oprd2 = X[_num2[icins]-1];
        }
        else { oprd2 = 0; }

	X[icins] = evalBool(_oper[icins],oprd1,oprd2);
#ifdef DEBUG
   cout << "  =  X(" << icins+1 << ")= " << X[icins] << "\n";
   cout.flush();
#endif
   }

   flag = X[_noOfCondIns-1];
   goto Fin;
//
//
   Fin:

	if (flag == -2) {
//debug beg
if (dbg > 1 )
{
		cout << " Warning! Condition refers only to Ghosts" <<endl
		<< "will be taken as FALSE" << endl;
}
//debug end
		return 0;
	}

   if ( flag == 0 ) {
#ifdef DEBUG
        cout << "    Condition evaluates to FALSE \n";
	    cout.flush();
#endif
   }
   else if ( flag == 1 ) {
#ifdef DEBUG
        cout << "    Condition evaluates to TRUE \n";
	    cout.flush();
#endif
   }
   else if ( flag == -1 ) {
#ifdef DEBUG
        cout << "    Condition can not be evaluated \n";
	    cout.flush();
#endif
   }
   else {
        cout << "    Flag returned by Condition has illegal value :" << flag
             << "\n";
        exit(1);
   }

#ifdef DEBUG
   cout << "\n";
   cout.flush();
#endif

   return flag;
}

//--------------------------   decode_instruction   ------------------------
void Condition::decode_instruction
     (char* instruction, 
      char* operation, char* type1, int *num1, char *type2, int *num2) {
//
//  Typical instruction looks like:
//
//0123456789012345..... cols
//and   T005  X056
//not   X003
char line[81];     // internal instruction

char operand1[5];  // T005
char operand2[5];  // X056

char operand1_num[4]; // 005
char operand2_num[4]; // 056

int instruction_len;
instruction_len = strlen(instruction);

if ( instruction_len  < 10 || instruction_len  > 80 ) {
     cout << " Condition instruction has illegal length :" 
          << "0123456789012345 \n"
          << instruction_len
          << " \n";
	 cout.flush();
     exit(1);
}
 
strcpy(line,instruction);


line[3]  = '\0';
strcpy(operation,line);

line[10] = '\0';
strcpy(operand1,&line[6]);

*type1 = operand1[0];
if( *type1 != 'T' &&  *type1 != 'X' ) {
    cout << " Condition instruction has illegal format :\n"
         << "0123456789012345 \n"
         << instruction << " \n";
	cout.flush();
    exit(1);
}
strcpy(operand1_num,&operand1[1]);
sscanf(operand1_num,"%d",num1);


if ( !strcmp(operation,"and") || !strcmp(operation,"or ") ) {
     if ( instruction_len < 16 ) {
         cout << " Condition instruction has illegal format :\n"
              << "0123456789012345 \n"
              << instruction << " \n";
 	     cout.flush();
         exit(1);
     }
     line[16] = '\0';
     strcpy(operand2,&line[12]);
     *type2 = operand2[0];
     if( *type2 != 'T' &&  *type2 != 'X' ) {
         cout << " Condition instruction has illegal format :\n"
              << "0123456789012345 \n"
              << instruction << " \n";
	     cout.flush();
         exit(1);
     }
     strcpy(operand2_num,&operand2[1]);
     sscanf(operand2_num,"%d",num2);
}
else if ( !strcmp(operation,"not") ) {
     *type2 = ' ';
     *num2 = 0;
}
else {
     cout << " Condition instruction has illegal format :\n"
          << "0123456789012345 \n"
          << instruction << " \n";
	    cout.flush();
     exit(1);
}

return;
}


//--------------------------  objectSet   ------------------------
void Condition::objectSet(Set_Name *pobjSet) {
//
    SmpCond* pSmpCond;

   if ( !pobjSet->isEmpty() ) {
        cout << " objectSet   is not empty\n";
	pobjSet->out();
	    cout.flush();
        exit(1);
   }

   NameList list; Name name;

   for ( int ismp = 0; ismp < _noOfSmpCond; ismp++) {
        pSmpCond = static_cast<SmpCond*>(_smpConditions[ismp]);
	pSmpCond->objectsToLock(list);
	list.reset();
	while (list.nextItem(name)) {
		pobjSet->add(name.getString());
	}	
   }
}


//--------------------------  addto_objectSet   ------------------------
void Condition::addto_objectSet(Set_Name *pobjSet) {
//
    SmpCond* pSmpCond;
   NameList list; Name name;

   for ( int ismp = 0; ismp < _noOfSmpCond; ismp++) {
        pSmpCond = static_cast<SmpCond*>(_smpConditions[ismp]);
	pSmpCond->objectsToLock(list);
	list.reset();
	while (list.nextItem(name)) {
		pobjSet->add(name.getString());
	}
   }
}
//------------------------------------------------------------------
void Condition::freeze() {
    SmpCond* pSmpCond;
    
	for ( int ismp = 0; ismp < _noOfSmpCond; ismp++) {
	        pSmpCond = static_cast<SmpCond*>(_smpConditions[ismp]);
		pSmpCond->freeze();	
	}
	return;	
}
//------------------------------------------------------------------
void Condition::unfreeze() {
    SmpCond* pSmpCond;
    
	for ( int ismp = 0; ismp < _noOfSmpCond; ismp++) {
	        pSmpCond = static_cast<SmpCond*>(_smpConditions[ismp]);
		pSmpCond->unfreeze();	
	}
	return;	
}
//------------------------------- evalBool ------------------------- 
int Condition::evalBool
	(const char operation[],
		 const int operand1, const int operand2) const {
	if (strcmp(operation,"not") == 0) {
		if (operand1 == 1) {return 0;}
		if (operand1 == 0) {return 1;}
		assert (operand1==-2);
		return -2;
	}

	if (strcmp(operation,"or ") == 0) {
		if (operand1 == 1 || operand2 == 1)   {return 1;}
		if (operand1 == 0 && operand2 == 0)   {return 0;}
		if (operand1 == -2) {return operand2;}
		if (operand2 == -2) {return operand1;}
	}
	else if (strcmp(operation,"and") == 0) {
		if (operand1 == 0 || operand2 == 0)   {return 0;}
		if (operand1 == 1 && operand2 == 1)   {return 1;}
		if (operand1 == -2) {return operand2;}
		if (operand2 == -2) {return operand1;}
	}
	else {}

	cout << "*** Internal error" 
	<< " operation: |" << operation 
	<< "| " << operand1 << " " << operand2;
	abort(); 
	return 0;  // this to pacify some compilers
}
//----------------------------------------------  BF  Sep 2008  ------
bool Condition::isObjectDirectlyReferenced( const Name& objName) const
{
	return _refObjects.isPresent(objName);
}
//----------------------------------------------  BF  Sep 2008  ------
bool Condition::isObjectSetReferenced( const Name& objSetName) const
{
	return _refObjectSets.isPresent(objSetName);
}
//---------------------------------------------- BF Sep 2008 --------
void Condition::removeObjectFromFrozenObjectSets( const Name& objName, const Name& setName)
{
	SmpCond* pSmpCond;
	SmpCondTyp2* pSmpCond2; SmpCondTyp3* pSmpCond3; 
    
	for ( int ismp = 0; ismp < _noOfSmpCond; ismp++)
	{
	        pSmpCond = static_cast<SmpCond*>(_smpConditions[ismp]);
		if ( pSmpCond->type() == 2 )
		{
			pSmpCond2 = static_cast<SmpCondTyp2*>(pSmpCond);
			pSmpCond2->removeObjectFromFrozenObjectSet( objName, setName);
		}
		if ( pSmpCond->type() == 3 )
		{
			pSmpCond3 = static_cast<SmpCondTyp3*>(pSmpCond);
			pSmpCond3->removeObjectFromFrozenObjectSet( objName, setName);
		}

	}
	return;	
}
//---------------------------------------------- BF Oct  2011 --------
void Condition::getCurrentRefObjects(NameList& currRefObjects) const
{
	currRefObjects.removeAll();
	currRefObjects.add(_refObjects);
	
	int numObjSets = _refObjectSets.length();
	if ( numObjSets <= 0 ) {return;}
	
	int iset; Name objSetName; void* ptnv; SMIObjectSet* ptnSet;
	Name objInSet;
	
	// loop over the referenced sets
	for ( iset=0; iset<numObjSets; iset++ )
	{
		objSetName = _refObjectSets[iset];
		ptnv = allSMIObjectSets.gimePointer(objSetName);
		if (ptnv == 0)
		{
			cout << "*** Set " << objSetName << " is not declared"
			<< endl; abort();
		}
		ptnSet = static_cast<SMIObjectSet*>(ptnv);
		
		ptnSet->reset();
		while (ptnSet->nextObject(objInSet))
		{
			currRefObjects.add(objInSet);
		}
	}
	return;
}
/*
//-----  Test main for decode_instruction-------------------
void main() {
char instruction[17];

char oper[4];

char typ1;
char typ2;

int num1;
int num2;


strcpy(instruction,"or    T002  X056");

cout << instruction << "\n";

decode_instruction (instruction, oper, &typ1, &num1, &typ2, &num2);

cout << " Operation : |" << oper << "| \n";

cout << " First  operand :  Type : |" << typ1 << "|  number : " << num1 << " \n";

cout << " Second operand :  Type : |" << typ2 << "|  number : " << num2 << " \n";

exit(1);

}
*/
