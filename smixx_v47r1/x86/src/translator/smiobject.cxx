//-----------------------------------------------------------------------
//                         SMIObject  Class
//                                                 B. Franek
//                                                 21 March 1999
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "smlunit.hxx"
#include "utilities.hxx"
#include "smiobject.hxx"
#include "attributeblock.hxx"
#include "parameterblock.hxx"
#include "state.hxx"
#include "smlline.hxx"
#include "errorwarning.hxx"

//--------------------------- Constructors -------------------------------

SMIObject::SMIObject
( const Name& name, int cls, int assoc) 
: SMLUnit("Object",100,name),
   _class(cls), _associated(assoc),
  _isOfClass("\0"),
  _pAttributeBlock(NULL), _pParameterBlock(NULL),
  _states()
{
//	cout << endl << "Object  " << _name << endl;
}

SMIObject::~SMIObject() {
    delete _pSMLcode;
}

  
void SMIObject::translate() {

	Registrar subobjects;
	Name subobjName = "VOID"; 

	_pAttributeBlock = new AttributeBlock();
	assert (_pAttributeBlock != 0);

	
	_pParameterBlock = new ParameterBlock();
	assert (_pParameterBlock != 0);

	int parametersCollected = 0;
	int collectingStates = 0;

	int newUnit = 1;	
	SMLUnit* pUnit = _pAttributeBlock;
	Name unitType = "attribute block";
	Registrar* pTypeRegistrar = 0;
	Name name;
	Name registeredName;

	SMLline line;
	int numOfLines = _pSMLcode->length();

	for (int i = 0; i<numOfLines; i++) {   //***** Beg of collection loop
		line = (*_pSMLcode)[i];
		int objectKeywordLine = line.objectKeyword(name);

		if (objectKeywordLine == 10) {
			if (collectingStates == 1) {
				ErrorWarning::printHead("ERROR",line
				," Object/Class constructor: parameters out of order");
				exit(2);
			}
			if (parametersCollected == 1) {
				ErrorWarning::printHead("ERROR",line
				," Object/Class constructor: too many parameter blocks");
				exit(2);
			}
			parametersCollected = 1;
			newUnit = 1;
			pUnit = _pParameterBlock;
			unitType = "parameter block";
			pTypeRegistrar = 0; 
		}
		else if (objectKeywordLine == 30) {
			collectingStates = 1;
			newUnit = 1;
			pUnit = new State(name,subobjName);
			assert (pUnit != 0);
			unitType = "State";
			pTypeRegistrar = &_states;
			registeredName = name;
			registeredName += "."; registeredName += subobjName;
		}
		else if (objectKeywordLine == 20) {
			subobjName = name;
			collectingStates = 1;
			if(subobjects.add(subobjName,0) != 1 ) {
				ErrorWarning::printHead("ERROR",line
				," Object/Class constructor: duplicate declaration ");
				exit(2);
			}
			continue;
		}
		else {}
		if (newUnit) {
			newUnit = 0;
			_internalSMLUnits.addRegardless(unitType,pUnit);
			pUnit->setParentUnit(this);
			if(pTypeRegistrar) {
				int iflag = pTypeRegistrar->add(registeredName,pUnit);
				if (iflag != 1) {
					ErrorWarning::printHead("ERROR",line
					," Object/Class constructor: duplicate declaration");
				exit(2);
				}
			}
		}
		pUnit->acceptLine(line);
	 }                              //****** end of collection loop

 
	 translateUnits();

}
//---------------------------------------------  BF April 2000  ------------------
void SMIObject::outSobj(ofstream& sobj) const
{
	if ( _class ) {
		sobj << "*CLASS" << endl;
	}
	else {
		sobj << "*OBJECT" << endl;
	}
	char* pStr = _name.getString();
	sobj << pStr << endl;

	int n1,n2,n3;

	n1 = 0;

//	n2 = _pAttributeBlock->numOfAttributes();
	n2 = _pAttributeBlock->_attributes.length();
	n3 = _associated;

	char line[80];

	if (_class) {
		sprintf(line,"%5d%5d",n2,n3);
	}
	else {
		sprintf(line,"%5d%5d%5d",n1,n2,n3);
	}

	sobj << line << endl;

	for (int i=0; i<n2; i++) {
//		sobj << (_pAttributeBlock->attribute(i)).getString() << endl;
		sobj << _pAttributeBlock->_attributes[i].getString() << endl;
	}

//	int numpar = _pParameterBlock->numOfParameters();
	int numpar = _pParameterBlock->_parameters.numOfEntries();
	if (numpar > 0) { 
		sobj << "*PARAMETERS" << endl;
	}

	for (int ip=0; ip<numpar; ip++) {
		Name name,value; char type[10];	
//		_pParameterBlock->getPar(ip,name,value,type);
		_pParameterBlock->_parameters.get(ip,name,value,type);
		if ( value == "&noval" ) {value = "&nodefault";}
		sobj << type << endl;
		sobj << name.getString() << endl;
		sobj << value.getString() << endl;
	}

	if (numpar > 0) { sobj << "*END_PARAMETERS" << endl;}

	int nStates = _states.length();

	Name prevSubobj = "\0"; Name subObj = "\0";

	for (int ist=0; ist < nStates; ist++) {
		void* ptnvoid = _states.gimePointer(ist) ;
		State* pState;
		pState = (State*)ptnvoid;
//	cout << ist << pState->unitName() << pState->subObject() << endl;
//	pState->out(" ");
	if (_associated == 1 ) {
//		cout << prevSubobj << subObj << endl;
		subObj = pState->subObject();
		if ( subObj == prevSubobj ) {}
		else {
			prevSubobj = subObj;
			if (ist > 0) {sobj << "*END_SUBOBJECT" << endl;}
			sobj << "*SUBOBJECT" << endl;
			if ( subObj=="VOID" ) {subObj = "&DUMMY";}
			sobj << subObj.getString() << endl;
		}
	}	
	  pState->outSobj(sobj);
	}
	if (_associated == 1) { sobj << "*END_SUBOBJECT" << endl;}

	if ( _class ) { sobj << "*END_CLASS" << endl;}
	else { sobj << "*END_OBJECT" << endl; }

	return;

}
//---------------------------------------------------------------------
int SMIObject::getObjectParameter(const Name& name, Name& value, Name& 
                                   typeNm) const
{
        int flg;
        char type[10];

        flg = (_pParameterBlock->_parameters).get(name,value,type);
        if ( flg ) { typeNm = type; }
        return flg;
}
//------------------------------------------------------------------------
bool SMIObject::isClass() const
{
	if ( _class == 0 )
	{
		return false;
	}
	else
	{
		return true;
	}
}
//-------------------------------------------------------------------------
int SMIObject::getStates( NameVector& states,int& undeclared) const
{

	int nStates = _states.length();
	undeclared = 0;
	
	for (int ist=0; ist<nStates; ist++) {
		void* ptnvoid = _states.gimePointer(ist);
		State* pState;
		pState = (State*)ptnvoid;
		states+= pState->unitName();
		if ( pState->isUndeclared() == true ) { undeclared = 1; }
	}
	
	return nStates;
}
//-------------------------------------------------------------------------
bool SMIObject::hasState(const Name& stateNm, int& undeclared) const
{
	NameVector states;
	
	int nStates = getStates(states,undeclared);

	if (undeclared) { return true; }

	for (int ist=0; ist<nStates; ist++) {
		if ( stateNm == states[ist] ) { return true; }
	}
	
	return false;
}
//---------------------------------------------------------------------------
int SMIObject::examine()
{
	int retcode = 0;
	
/*
  cout << endl 
  << " ====================== SMIObject::examine() ============= " << endl;
	
  cout << "  Parent : " << _pParentUnit->unitId() 
  << "  " << _pParentUnit->unitName() << endl;
		
  cout << "     Complete Ancestry " << endl;
	
	int num;
	NameVector ids,names;
	
	_pParentUnit->ancestry(ids,names);
	
	num = ids.length();

	for (int i=0; i<num; i++)
	{
		cout << "     " << ids[i] << "  " << names[i] << endl;
	}
*/

	Name stateNm("DEAD"); // I need a name and this is as good as any
	int undeclared;
	bool hasStateFlg;
	
	hasStateFlg = hasState(stateNm,undeclared);
	
	if ( undeclared == 1 )
	{
		ErrorWarning::printHead("WARNING",(*_pSMLcode)[0]
		,"undeclared states");
		cout << endl << " There are undeclared states." << endl 
		     << " This makes it more difficult" << endl
		     << " to perform consistency checks " << endl;
	}
	
	int iflg = examineUnits();
	
	return retcode+iflg;
}
//-------------------------------------------------------------------------------------------------------
Name SMIObject::outString() 
{
	Name temp;
	
	if ( _class == 1 ) { temp = "CLASS : "; }
	else               { temp = "OBJECT : "; }
	
	temp += _name;
	
// to do:  add the attributes	
	return temp;
	
}
//----------------------------------------------------------------------------
bool SMIObject::hasAction( const Name& actionNm ) const
{
	int numStates = _states.length();
	
	void* ptnvoid; State* pstate;
	
	for ( int i=0; i<numStates; i++ )
	{
		ptnvoid = _states.gimePointer(i);
		pstate = (State*)ptnvoid;
		
		if ( pstate->hasAction(actionNm) ) { return true; }
	}
	
	return false;
}
