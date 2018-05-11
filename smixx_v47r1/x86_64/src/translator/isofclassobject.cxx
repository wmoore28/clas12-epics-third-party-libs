//-----------------------------------------------------------------------
//                         IsOfClassObject  Class
//                                                 B. Franek
//                                                 24 April 2000
// Copyright Information:
//      Copyright (C) 1999-2006 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "smlunit.hxx"
#include "utilities.hxx"
#include "isofclassobject.hxx"
#include "attributeblock.hxx"
#include "smlline.hxx"
#include "registrar.hxx"
#include "errorwarning.hxx"

extern Registrar allClasses;

//--------------------------- Constructors -------------------------------

IsOfClassObject::IsOfClassObject
( const Name& name, int assoc) 
: SMLUnit("Object",5,name,"IsOfClassObject"),
  _associated(assoc),
  _isOfClass("UNKNOWN"),
  _pAttributeBlock(NULL)
{
// _associated is commpletely ignored, because this is taken from 
// the class. The only reason I am leaving it here is because I want to keep
// the translator output unchanged, so that it can be easily compared
// BF 31 March 2006

//	cout << endl << "Object  " << _name << endl;
}
//----------------------------------------------------------------------
IsOfClassObject::~IsOfClassObject() {
    delete _pSMLcode;
}
//-----------------------------------------------------------------------
  
void IsOfClassObject::translate() {

//	cout << " Translating " << _id << _name << endl;
//    _pSMLcode->out();
	SMLline line; char* pLine;

	line = (*_pSMLcode)[0];
	pLine = line.getString();
	char* ptn = getNextToken(pLine,":", _isOfClass);
	ptn = getNextToken(ptn+1," ",_isOfClass);
	ptn = getNextToken(ptn+1," ",_isOfClass);
	ptn = getNextToken(ptn+1," /#!",_isOfClass);
	_isOfClass.upCase(); _isOfClass.trim();
	if (!check_name(_isOfClass)) {
		ErrorWarning::printHead("ERROR",line);
		cout << " Object : " << _isOfClass 
			<< "  is not a name" << endl;
		exit(2);
	}

	SMLUnit *pUnit; Name tempName;

	int numOfLines = _pSMLcode->length();

	_pAttributeBlock = new AttributeBlock();
	assert (_pAttributeBlock != 0);
	pUnit = _pAttributeBlock; tempName ="attribute block";
	pUnit->setParentUnit(this);
	_internalSMLUnits.addRegardless(tempName,pUnit);

	for (int i = 0; i<numOfLines; i++) { 
		line = (*_pSMLcode)[i];
		_pAttributeBlock->acceptLine(line);		  
	 }                       

//     cout << " Object : " << _name << " collecting finished" << endl;
 
	translateUnits();
	
	return;
}
//---------------------------------------------  BF April 2000  ------------------
void IsOfClassObject::outSobj(ofstream& sobj) const
{
	sobj << "*OBJECT" << endl;

	char* pStr = _name.getString();
	sobj << pStr << endl;

	int n1,n2,n3;

	n1 = 1;

	n2 = _pAttributeBlock->_attributes.length();
	n3 = _associated;

	char line[80];
	sprintf(line,"%5d%5d%5d",n1,n2,n3);
//	sobj << "    " << n1 << "    " << n2 << "    " << n3 << endl;
    sobj << line << endl;


	sobj << _isOfClass.getString() << endl;

	for (int i=0; i<n2; i++) {
            if (  _pAttributeBlock->_attributes[i] == "ASSOCIATED" )
	    {
	        cout << " **** Warning ****  is_of_class type object : "
		     << pStr << " has \"ASSOCIATED\" attribute" << endl;
		cout << " this will be ignored and whatever is in the class taken"
		     << endl;
	    }
            sobj << _pAttributeBlock->_attributes[i].getString() << endl;
	}

	sobj << "*END_OBJECT" << endl; 

	return;
}
//-----------------------------------------------------------------------------
Name& IsOfClassObject::isOfClass()
{
	return _isOfClass;
}
//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
int IsOfClassObject::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
/*
  cout << endl 
  << " ====================== IsOfClassObject::examine() ============= " << endl;
	
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
	
	void* pvoid = allClasses.gimePointer(_isOfClass);
	
	if ( pvoid == 0 )
	{
	//	retcode = 1;    review
		ErrorWarning::printHead("SEVERE WARNING",firstLine);
		cout << "  class " << _isOfClass << " is not declared " << endl;
	}
	
	int iflg = examineUnits();
	
	return retcode+iflg;
}

//-------------------------------------------------------------------------------------------------------
Name IsOfClassObject::outString() 
{
	Name temp;
	
	temp = "OBJECT : ";
	temp += _name;
	temp += " is_of_class ";
	temp += _isOfClass;
	
// to do:  add the attributes	
	return temp;
	
}

