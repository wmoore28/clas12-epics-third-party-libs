// ifUnit.cxx: implementation of the IfUnit class.
//
//                                                B. Franek
//                                             4 October 1999
//////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdio.h>
#include "smlline.hxx"
#include "ifunit.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IfUnit::IfUnit() : SMLUnit("ifunit",10) 
{
   _name = "ifunit";
   return;
}

IfUnit::~IfUnit()
{
    delete _pSMLcode;
}

void IfUnit::translate() {

//	cout << " Translating " << _id << _name << endl;
//    _pSMLcode->out();

	_pHeadBlock = new IfUnitHeadBlock();
	assert (_pHeadBlock != 0);

	int newUnit = 1;
	SMLUnit *pUnit = _pHeadBlock;
	Name unitType = "Head block";

	int collectingInsList = 0;
    _pInsList = 0;

	Name instype;

	SMLline line;
	int numOfLines = _pSMLcode->length();

	for (int i = 0; i<numOfLines; i++) {   //***** Beg of collection loop
		line = (*_pSMLcode)[i];

		if (collectingInsList || i == 0) {}
		else {
			bool insLine = line.instructionLine(instype);
			if (insLine ) {
				collectingInsList = 1;
				newUnit = 1;
				_pInsList = new InsList();
				pUnit = _pInsList;
				assert (pUnit != 0);				
				unitType = "InsList";
			}
		}

		if (newUnit) {
				newUnit = 0;
				_internalSMLUnits.addRegardless(unitType,pUnit);
				pUnit->setParentUnit(this);
		}

		pUnit->acceptLine(line);
	}                              //****** end of collection loop


	translateUnits();
}
//---------------------------------  BF  April 2000  -------------------
void IfUnit::outSobj(int id, int level, ofstream& sobj, Name& label, InsList*& pInsList) const
{
//	if(_pCondition) {_pCondition->outSobj(sobj);}

	if (!_pHeadBlock->_else) {
		_pHeadBlock->_condition.outSobj(sobj);
	}
	else {
		sobj << "else" << endl;
		sobj << "    0" << "    0" << endl;
	}


	char tempch[80];

	sprintf(tempch,"%3d%3d",id,level); label = tempch;
	sobj << tempch << endl;

	pInsList = _pInsList;
	return;
}
