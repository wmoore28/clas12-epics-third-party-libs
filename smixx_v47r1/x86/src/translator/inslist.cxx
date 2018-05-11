// inslist.cxx: implementation of the Action class.
//
//                                                B. Franek
//                                             28 September 1999
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "smlline.hxx"
#include "inslist.hxx"
#include "doins.hxx"
#include "insertins.hxx"
#include "sleepins.hxx"
#include "termins.hxx"
#include "setins.hxx"
#include "ifins.hxx"
#include "waitins.hxx"
#include "createobjectins.hxx"
#include "errorwarning.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InsList::InsList() : SMLUnit("InsList",10) 
{
   return;
}

InsList::~InsList()
{
    delete _pSMLcode;
}

void InsList::translate() {

	int newUnit = 0;
	Name unitType;
	SMLUnit *pUnit; 


	int ifLevel = 0;
	

	SMLline line; 
	int numOfLines = _pSMLcode->length();

	for (int i = 0; i<numOfLines; i++) {   //***** Beg of collection loop
		line = (*_pSMLcode)[i];
		int instr = line.instructionLine(unitType);

		if ( ifLevel ) {
			if ( instr ) {
				if ( unitType == "if" )    { ifLevel++; }
				if ( unitType == "endif" ) { ifLevel--; }
			}
		}
		else {
			if ( instr ) {
				newUnit = 1;
				if      ( unitType == "do" )
				{pUnit = new DoIns();}

				else if ( unitType == "create_object" )
				{pUnit = new CreateObjectIns();}

				else if ( unitType == "insert" )
				{pUnit = new InsertIns();}

				else if ( unitType == "remove" )
				{pUnit = new InsertIns();}

				else if ( unitType == "sleep" )
				{pUnit = new SleepIns();}
				
				else if ( unitType == "set" )
				{pUnit = new SetIns();}

				else if ( unitType == "terminate_action" )
				{pUnit = new TermIns();}
				
				else if ( unitType == "if" )
				{pUnit = new IfIns(); ifLevel = 1;}
				
				else if ( unitType == "wait" )
				{pUnit = new WaitIns();}

				else 
				{
					ErrorWarning::printHead("ERROR",line);
					cout << " unknown instruction :  " 
					     << unitType << endl;
					exit(2);
				}
				pUnit->setParentUnit(this);
			}
		}
		if (newUnit) { 
			newUnit = 0;
			assert(pUnit != 0);
			_internalSMLUnits.addRegardless(unitType,pUnit);
		}
		pUnit->acceptLine(line);
	}
			         
	 translateUnits();
}
//----------------------------------------  BF April 2000  --------------
void InsList::outSobj(Name label,int& id, ofstream& sobj, Registrar& nextLevelLists) const
{
	int blockId,blockLevel;

	sscanf(label.getString(),"%d %d",&blockId,&blockLevel);

	int level = blockLevel;

	sobj << "*BLOCK" << endl;


//	sobj << "  " << blockId << "  " << blockLevel << endl;
	sobj << label.getString() << endl;

	int numIns = _internalSMLUnits.length();

	for (int i=0; i < numIns; i++) {
		void* ptnvoid = _internalSMLUnits.gimePointer(i) ;
		Instruction* pInstruction;
		pInstruction = (Instruction*)ptnvoid;
		Registrar insLists;
		pInstruction->outSobj(id,level,sobj,insLists);
		if (insLists.length() > 0) {
			nextLevelLists += insLists;
		}
	}
 
}
