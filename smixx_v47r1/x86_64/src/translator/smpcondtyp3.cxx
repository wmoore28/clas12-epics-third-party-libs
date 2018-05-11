// smpcondtyp3.cxx: implementation of the SmpCondTyp3 class.
//
//                                                B. Franek
//                                               August 2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "smlline.hxx"
#include "smllinevector.hxx"
#include "utilities.hxx"
#include "errorwarning.hxx"

#include "smpcondtyp3.hxx"
#include "smiobjectset.hxx"

#include "registrar.hxx"

extern Registrar allObjectSets;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SmpCondTyp3::SmpCondTyp3(const int idNum) : SmpCond(idNum)
{
	return;
}

SmpCondTyp3::~SmpCondTyp3() 
{
	return;
}
//===================================================================================
int SmpCondTyp3::initialise( SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& inext, int& jnext)
{
	char* ptnDel;

	SMLline lineBeingTranslated;
	
	SMLline line; 
//	char* pLine;

	*_pSMLcode += (*pSMLcode)[ist];

//	char* ptn;

	Name temp;

//	cout << endl << " SmpCondTyp3::initialise " << endl
//		<< "SMLcode -------------------" << endl;
//		  pSMLcode->out();
//	cout    << "---------------------------" << endl;
	
  
//	int ibeg,jbeg;
	int idel,jdel;
	
	inext = ist; jnext = jst;
		
//	((*pSMLcode)[inext]).indicateCharacter("\n Starting search at",jnext);


	lineBeingTranslated = (*pSMLcode)[inext];
	char del = getNextToken(pSMLcode,inext,jnext," ",
				_objectSetName,idel,jdel,inext,jnext);
		
//	((*pSMLcode)[idel]).indicateCharacter("\n Delimiter found at",jdel);

	_objectSetName.upCase(); _objectSetName.trim();
	if (!check_name(_objectSetName)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Object Set name is not a name " << _objectSetName << endl;
		exit(2);
	}
	
//	cout << " Object Set name " << _objectSetName << endl;

		
//	((*pSMLcode)[inext]).indicateCharacter("\n Starting search at",jnext);
	
	lineBeingTranslated = (*pSMLcode)[inext];
	del = getNextToken(pSMLcode,inext,jnext," )",temp,idel,jdel,inext,jnext);
	temp.upCase(); ptnDel = &del;
		
//	((*pSMLcode)[idel]).indicateCharacter("\n Delimiter found at",jdel);

	if (temp == "EMPTY") { _empty = 1;}
	else if (temp == "NOT_EMPTY") { _empty = 0;}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " looking for EMPTY or NOT_EMPTY but found " 
			<<temp << endl;
		exit(2);
	}
	
	if (del == ')') { inext = idel; jnext = jdel; } // the closing bracket needs to be found
	
//	cout << " Token : " << _empty << endl;
//	cout << " inext, jnext " << inext << "," << jnext << endl;

	return 1;

}
//-----------------------------------------------------------------------
void SmpCondTyp3::negate()
{

	if	(_empty == 1)	{_empty = 0;}
	else			{_empty = 1;}
	
	return;

}

//----------------------------------------------------------------------------
void SmpCondTyp3::out(const Name offset) const
{
	char temp[20];
	if ( _empty == 1 ) {strcpy(temp," empty ");}
	else {strcpy(temp," not_empty ");}
	char* ptn = offset.getString();
	cout << ptn  << _idNum << " " 
	<< _objectSetName << temp << " "; 
	return;
}
void SmpCondTyp3::outFull(const Name offset) const
{ out(offset); return;}

//---------------------------------------------  BF August 2009 ------
void SmpCondTyp3::outSobj(ofstream& sobj) const
{
        sobj << "    " << _type << endl;
	sobj << _objectSetName.getString() << endl;
	
	if (_empty) {
		sobj << "empty" << endl;
	}
	else {
		sobj << "not_empty" << endl;
	}
	return;
}
//---------------------------------------------------------------------------
int SmpCondTyp3::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
/*  cout << endl 
  << " ====================== SmpCondTyp3::examine() ============= " << endl;
	
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
// First check that the object set is declared

	SMIObjectSet* pObjSet = (SMIObjectSet*)(allObjectSets.gimePointer(_objectSetName));
	
	if ( pObjSet == 0 )   // the object set is not declared
	{
//		retcode = 1; //review
		ErrorWarning::printHead("SEVERE WARNING",firstLine);
		cout << " Object Set " << _objectSetName << " is not declared" << endl;
					
		cout << endl; _pParentUnit->printParents(); cout << endl;

	}

	return retcode;
}

//-------------------------------------------------------------------------------------------------------
Name SmpCondTyp3::outString() 
{
	Name temp;

	temp = _objectSetName;
	
	if ( _empty == 1 )
	{
		temp += " empty ";
	}
	else
	{
		temp += " not_empty ";
	}
	
	
	return temp;
	
}
