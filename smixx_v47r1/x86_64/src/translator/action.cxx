// action.cxx: implementation of the Action class.
//
//                                                B. Franek
//                                               1 June 1999
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include "smixx_common.hxx"
#include <assert.h>
#include "action.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Action::Action(const Name& name)
 : SMLUnit("Action",10,name),
   _pActionHeadBlock(NULL), _pInsList(NULL) 
{
//   cout << "    action " << _name << endl;
}


Action::~Action()
{
    delete _pSMLcode;
}

void Action::translate() {

	_pActionHeadBlock = new ActionHeadBlock();
	assert (_pActionHeadBlock != 0);

	int newUnit = 1;
	SMLUnit* pUnit = _pActionHeadBlock;
	Name unitType = "action head";
	Name name;

	int collectingInsList = 0;
	_pInsList = 0;

	SMLline line; 
	int numOfLines = _pSMLcode->length();

	for (int i = 0; i<numOfLines; i++) {   //***** Beg of collection loop
		line = (*_pSMLcode)[i];

		if (collectingInsList) { }
		else  {
			bool insLine = line.instructionLine(name);
			if ( insLine ) {
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
//-----------------------------------------------  BF April 2000  ----------------
void Action::outSobj(ofstream& sobj) const
{
	int level = 0;

	sobj << "*ACTION" << endl;

    sobj << _name.getString() << endl;

	int nattr = 0; 
//	int numpar = _pParameters->numOfEntries();
	int numpar = _pActionHeadBlock->_parameters.numOfEntries();

	sobj << "    " << numpar << "    " << nattr << endl;

	for (int ip=0; ip<numpar; ip++) {
		Name name,value; char type[10];
//		_pParameters->get(ip,name,value,type);
		_pActionHeadBlock->_parameters.get(ip,name,value,type);
		if ( value == "&noval" ) {value = "&nodefault";}
		sobj << type << endl;
		sobj << name.getString() << endl;
		sobj << value.getString() << endl;
	}

	
//cout << " action " << _name << endl;


	
	if (_pInsList) {
		Registrar currLevelLists;
		Name label = "  0  0"; int nextId = 0;
		currLevelLists.add(label,_pInsList);
	
		for (;;) {
			if (currLevelLists.length() == 0) {break;}
			int nl = currLevelLists.length();

			Registrar nextLevelLists;

			for (int il=0; il<nl; il++) {
				void* ptnvoid = currLevelLists.gimePointer(il) ;
				label = currLevelLists.gimeName(il);

				InsList* pInsList;
				pInsList = (InsList*)ptnvoid;
				Registrar temp;
				pInsList->outSobj(label,nextId,sobj,temp);
				nextLevelLists += temp;
			}
			currLevelLists = nextLevelLists;
			level++;
		}
	}

	sobj << "*END_ACTION" << endl;
	return;
}
Name Action::name() const
{
	return _name;
}
//-----------------------------------------------------------------------
int Action::getActionParameter(const Name& name, Name& value,
                               Name& typeNm) const
{
        int flg;
        char type[10];

        flg = ( _pActionHeadBlock->_parameters).get(name,value,type);
        if ( flg ) { typeNm = type; }
        return flg;
}
