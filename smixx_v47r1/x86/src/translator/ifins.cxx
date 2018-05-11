// ifins.cxx: implementation of the IfIns class.
//
//                                                B. Franek
//                                           30 September 1999
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <assert.h>
#include "smlline.hxx"
#include "ifins.hxx"
#include "ifunit.hxx"
#include "errorwarning.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IfIns::IfIns()  
{
   _name = "if";
   return;
}

IfIns::~IfIns()
{
    delete _pSMLcode;
}
//-----------------------------------------------------------------------------
void IfIns::translate() {

	int newUnit = 0;
	Name unitType = "\0";
	SMLUnit *pUnit;

	bool insLine;
	Name insType = "\0";
	int ifLevel = 0;
	int elseEncountered = 0;
	int endifEncountered = 0;

	int numOfLines = _pSMLcode->length();
	SMLline line; 

	for (int i = 0; i<numOfLines; i++) {   //***** Beg of collection loop
		line = (*_pSMLcode)[i];
		
		insLine = line.instructionLine(insType);
//cout << insLine << " " << insType << " " << ifLevel << endl;

		if (ifLevel == 0) {
			if (endifEncountered) {
				if (line.commentLine()) {continue;}
				else {
					ErrorWarning::printHead("ERROR",line
					,"Some crap follows IF instruction");
					exit(2);
				}
			}
			else {
				if (insLine && insType == "if") {
					ifLevel++;
					newUnit = 1;
					pUnit = new IfUnit();
				}
				else {
					ErrorWarning::printHead("ERROR",line
					,"Expected IF not found");
					exit(2);
				}
			}
		}
		else if (ifLevel == 1) {
			if (insLine) {
				if (insType == "elseif" || insType == "else") {
					if (insType == "else") {
						if (elseEncountered) {
							ErrorWarning::printHead("ERROR",line
							,"second occurence of 'else' while parsing IF instruction");
							exit(2);
						}
						else {elseEncountered = 1;}
					}
					newUnit = 1;
					pUnit = new IfUnit();
				}
				if (insType == "if") {ifLevel++;}
				if (insType == "endif") {ifLevel--; endifEncountered = 1; continue;}
			}
		}
		else if (ifLevel > 1) {
			if (insLine && insType == "if")    {ifLevel++;}
			if (insLine && insType == "endif") {ifLevel--;}
		}
		else {
			cout << "This can never happen"<< endl; exit(2);
		}

		if (newUnit) {
			newUnit = 0;
			assert(pUnit != 0);
			unitType = insType;
			_internalSMLUnits.addRegardless(unitType,pUnit);
			pUnit->setParentUnit(this);
		}
		pUnit->acceptLine(line);
	}         //****** end of collection loop

	if (ifLevel != 0) {
		line = (*_pSMLcode)[0];
		ErrorWarning::printHead("ERROR",line
		,"IF nesting is probably screwed up");
		exit(2);
	}
	if (_internalSMLUnits.length() > 1 && elseEncountered == 0) {
		line =(*_pSMLcode)[0];
		ErrorWarning::printHead("ERROR",line
		,"'else' is missing"); 
		exit(2);
	}

	translateUnits();		
}
//------------------------------------------  BF  April 2000  --------
void IfIns::outSobj(int& id, int level, ofstream& sobj, Registrar& insLists) const
{
	sobj << "if" << endl;

	int nunits = _internalSMLUnits.length();

	sobj << "  " << nunits << endl;

	for (int i=0; i < nunits; i++) {
		void* ptnvoid = _internalSMLUnits.gimePointer(i) ;
		IfUnit* pIfUnit;
		pIfUnit = (IfUnit*)ptnvoid;
		Name label = "\0"; InsList* pInsList;
		id++;
		pIfUnit->outSobj(id,level+1,sobj,label,pInsList);
		insLists.add(label,pInsList);
	}

	return;
}
