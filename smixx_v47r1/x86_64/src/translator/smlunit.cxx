// smlunit.cxx: implementation of the SMLUnit class.
//
//                                                B. Franek
//                                               2 July 1999
//////////////////////////////////////////////////////////////////////

#include "smixx_common.hxx"
#include "smlunit.hxx"
#include "assert.h"
#include "string.h"
#include "parms.hxx"
#include "smlline.hxx"
#include "utilities.hxx"
#include <iomanip>
#include "smiobject.hxx"
#include "ut_tr.hxx"

using std::cout;
using std::endl;
using std::setw;
using std::left;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SMLUnit::SMLUnit( const char* id, const int buf, const Name name, const Name type)
	: _id(id), _name(name), _type(type),  _internalSMLUnits(),
	  _pParentUnit(NULL)
{
    _pSMLcode = new SMLlineVector(buf);
     assert(_pSMLcode != 0); 
   
//	 cout << "SMLUnit constructor called" << endl;
}

SMLUnit::~SMLUnit()
{
    delete _pSMLcode;
}

int SMLUnit::acceptLine( const SMLline& line )
{
    *_pSMLcode += line;
//	cout << "acceptLine _id :" << _id << " _name : " << _name << endl;
//	cout << line << endl;
	return 0;
}

void SMLUnit::out(const Name offSet) const
{
	Name nextOffset;
	nextOffset = offSet;

	nextOffset += "  |";


    char* ptn = offSet.getString();

    void* ptnvoid; SMLUnit* pUnit;
	int numOfUnits = _internalSMLUnits.length();
	cout << ptn 
		<< "----------------------------------------------"<< endl 
		<< ptn << 
		" SMLUnit : " << _id << " " << _name << "  No of internal units : " << numOfUnits << endl;
	
	ptn = offSet.getString();
	if ( numOfUnits == 0 ) {
    	_pSMLcode->out(ptn);
	}
	else {
		for ( int i=0; i<numOfUnits; i++ ) {
			cout << ptn << "  " << i+1 << endl;
			ptnvoid = _internalSMLUnits.gimePointer(i);
            pUnit = (SMLUnit*)ptnvoid;
	        pUnit->out(nextOffset);
		}
	}
}
//------------------------------------------------- BF April 2000 -----------------
//void SMLUnit::outSobj(ofstream& sobj) const
//{
//	return;
//}
//------------------------------------------------- BF April 1999 -------------
void SMLUnit::translateUnits() {
	
     int numOfUnits = _internalSMLUnits.length();

     void* ptnvoid; SMLUnit* pUnit;

     for ( int iu=0; iu<numOfUnits; iu++ ) {  //Translation loop
        ptnvoid = _internalSMLUnits.gimePointer(iu);
        pUnit = (SMLUnit*)ptnvoid;
//       cout << " Going to translate :" << endl;
//		pUnit->outCodeOnly(" ");
//cout << " Translating " << pUnit->unitId() << pUnit->unitName() << endl;
	    pUnit->translate();

	 }   // end of translation loop
}

//----------------------------------------------------------------

Name SMLUnit::unitId() const
{
	return _id;
}

//----------------------------------------------------------------

Name SMLUnit::unitName() const
{
	return _name;
}
//-----------------------------------------------------------------
Name SMLUnit::unitType() const
{
       return _type;
}
//-----------------------------------------------------------------

void SMLUnit::outCodeOnly(const Name offSet) const
{

	Name nextOffset;
	nextOffset = offSet;

	nextOffset += "  |";


    char* ptn = offSet.getString();

	int numOfUnits = _internalSMLUnits.length();
	cout << ptn 
		<< "----------------------------------------------"<< endl 
		<< ptn << 
		" SMLUnit : " << _id << " " << _name << "  No of internal units : " << numOfUnits << endl;
	
	ptn = offSet.getString();
	_pSMLcode->out(ptn);
}

//---------------------------------------------------------------
int SMLUnit::nextChar(const int iline, const int ch,
				int nextLine, int nextCh) const
{
	int numOfLines = _pSMLcode->length();
	if ( iline <0 || iline >= numOfLines ) return 0;

	SMLline line = (*_pSMLcode)[iline];
	char* pLine = line.getString();
	int lineLength = strlen(pLine);

	if ( ch < 0 || ch >= lineLength ) {return 0;}

	nextCh = ch +1;

	if ( nextCh < lineLength ) {nextLine = iline; return 1;}

	nextCh = 0;

	for ( int i=iline+1; i < numOfLines; i++) {
		line = (*_pSMLcode)[i];
		pLine = line.getString();
		lineLength = strlen(pLine);
		if ( lineLength > 0 ) {nextLine = i; return 1;}
	}
	return 0;
}
//-----------------------------------------------------------------
void SMLUnit::setParentUnit(SMLUnit* pUn)
{
	_pParentUnit = pUn;
	return;
}
//--------------------------------------------------------------------------
void SMLUnit::ancestry( NameVector& ids, NameVector& names)
{
	SMLUnit* pUn;
	int i;
	
	pUn = _pParentUnit;
	
	for (pUn = _pParentUnit, i=0; pUn != NULL; pUn = pUn->_pParentUnit, i++)
	{
		ids += pUn->_id;
		names += pUn->_name;
	}
			
	return;
}
//------------------------------------------------- BF April 2013 -------------
int SMLUnit::examineUnits() {
//cout << endl << " SMLUnit::examineUnits()";	
	int numOfUnits = _internalSMLUnits.length();
//cout << " no. of units " << numOfUnits  << endl;
	void* ptnvoid; SMLUnit* pUnit;

	int anyErrors = 0;
	int iflg;
	
	for ( int iu=0; iu<numOfUnits; iu++ )
	{  //Examination loop
		ptnvoid = _internalSMLUnits.gimePointer(iu);
		pUnit = (SMLUnit*)ptnvoid;
//cout << " pUnit " << pUnit << endl;
//cout << " Examining internal unit " << pUnit->unitId() << pUnit->unitName() << endl;
		iflg = pUnit->examine();
		if (iflg != 0) anyErrors = 1;

	}   // end of examination loop
	return anyErrors;
}
//------------------------------------------------- BF April 2013 -------------
int SMLUnit::examine()
{
	int iflg;
	
//cout << left;
//cout << "SMLUnit::examine()  Unit : "
//<< setw(12) << unitId().getString()
//<< " Name : " << setw(20) << unitName().getString() << endl;
	
	iflg = examineUnits();	
	return iflg;
}

/*int SMLUnit::prevChar(const int line, const int ch, 
				int prevLine, int prevCh) const
{
	return 0;
}*/
//----------------------------------------------------------------------------
void SMLUnit::parents(Name& object, Name& state, Name& action)
{
	object = ""; state = ""; action = "";

	SMLUnit* pUn;
	Name id, name;
	
	for (pUn = this; pUn != NULL; pUn = pUn->_pParentUnit)
	{
		id = pUn->_id;
		name = pUn->_name;
		
		if ( id == "Object" )
		{
			object = name;
		}
		
		if ( id == "State" )
		{
			state = name;
		}
		
		if ( id == "Action" )
		{
			action = name;
		}

	}
	
	
	return;
}
//-------------------------------------------------------------------------
SMLUnit* SMLUnit::parentPointer(const Name& id)
{
	SMLUnit* pUn;
	
	for (pUn = this; pUn != NULL; pUn = pUn->_pParentUnit)
	{
		if( id == pUn->_id ) { return pUn; }
	}
	
	return NULL;
}
//-------------------------------------------------------------------------
void SMLUnit::printParents()
{
	Name object, state, action;
	
	char objectorclass[10]; Name temp;
	SMIObject* parentObject;
	
	parents(object,state,action);
	
	parentObject = (SMIObject*) _pParentUnit->parentPointer("Object"); 
	
	if ( parentObject->isClass() ) { temp = " Class : "; }
	else { temp = " Object : "; }
	
	strcpy (objectorclass,temp.getString());
	cout << objectorclass << object << "  State : " << state;
	if ( action == "" ) { cout << endl; }
	else { cout << "  Action : " << action << endl; }
	
	return;
}
//--------------------------------------------------------------------
void SMLUnit::printCode() const
{
	char temp[] = " ";
	_pSMLcode->out(temp);
}
