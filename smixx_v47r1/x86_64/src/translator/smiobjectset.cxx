//-----------------------------------------------------------------------
//                         SMIObjectSet  Class
//                                                 B. Franek
//                                                  7-June-2001
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "smlline.hxx"
#include "smiobjectset.hxx"
#include "utilities.hxx"
#include "registrar.hxx"
#include "smiobject.hxx"

#include "ut_tr.hxx"
#include "errorwarning.hxx"

extern Registrar allClasses;
extern Registrar allObjects;
//--------------------------- Constructors -------------------------------

SMIObjectSet::SMIObjectSet
       ( const Name& name)
	   : SMLUnit("ObjectSet",20,name),
	     _isOfClass(""),
	     _objectList()
{

//	cout << endl << "Object Set  " << _name << endl;
	return;
}

SMIObjectSet::~SMIObjectSet() {
    delete _pSMLcode;
}

void SMIObjectSet::translate() {
//	cout << endl << " SMIObjectSet::translate()  entered " << endl;

	Name token; int idel,jdel; int inext,jnext; int ist,jst;
	
	SMLline lineBeingTranslated = (*_pSMLcode)[0];
	char del = getNextToken(_pSMLcode,0,0,":",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	if ( token == "OBJECTSET" ) {}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Expecting OBJECTSET keyword , but found "
		     << token << endl;
		exit(2);
	}

	ist = inext; jst = jnext;
	lineBeingTranslated = (*_pSMLcode)[ist];
	del = getNextToken(_pSMLcode,ist,jst," {",token,idel,jdel,inext,jnext);
        token.upCase(); token.trim();
//        cout << token << "  del |" << del << "|" << endl;

        if ( del == '\0' ) {
		return;
	}
	if ( del == ' ' ) {
		ist = inext; jst = jnext;
		lineBeingTranslated = (*_pSMLcode)[ist];
		del = getNextToken(_pSMLcode,ist,jst," ",token,idel,jdel,inext,jnext);
		token.upCase(); token.trim();
//		cout << token << "  del |" << del << "|" << endl;
                if ( token == "IS_OF_CLASS" ) {}
		else {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << endl << "Looking for ' IS_OF_CLASS' but found"
			  << token << endl;
			exit(2);
		}
		ist = inext; jst = jnext;
		lineBeingTranslated = (*_pSMLcode)[ist];
		del = getNextToken(_pSMLcode,ist,jst," {",token,idel,jdel,inext,jnext);
		token.upCase(); token.trim();
		if (!check_name(token)) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << "  Class name is not a name " << token << endl;
			exit(2);
		}
		_isOfClass = token;
		if ( del == '\0' ) {return;}
	}
	if ( del != '{' ) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "the class name is followed by <"
               << del << ">" 
               << "this should not happen" << endl;
		exit(2);
	}
	for (;;) {
		ist = inext; jst = jnext;
		lineBeingTranslated = (*_pSMLcode)[ist];
		del = getNextToken(_pSMLcode,ist,jst,",}",token,idel,jdel,inext,jnext);
		token.upCase(); token.trim();
//		cout << token << "  del |" << del << "|" << endl;

		if ( token == "\0" ) {}
		else {
			if (!check_name(token)) {
				ErrorWarning::printHead("ERROR",lineBeingTranslated);
				cout << token << "  is not a name" << endl;
				exit(2);
			}
//			cout << " inext " << inext << endl;
			_objectList.add(token);
		}
		if ( del == '}') {break;}	
	}
//Name offset = " "; _objectList.out(offset);
	return;

}

//================================== out ============================================
void SMIObjectSet::out(const Name offset = "\0")  {

	SMLUnit::out(offset);

	char* ptn = offset.getString(); cout << ptn << endl;
        cout << ptn << "ObjectSet " << _name << "  is of class " << _isOfClass << endl;
        cout << "   Initiated with : " << _objectList.numOfEntries() << " objects" << endl;

       _objectList.reset(); Name tempName;
       for (int i=0; i<_objectList.numOfEntries(); i++) {
		_objectList.nextItem(tempName);
		cout << ptn << tempName << endl;
	}
	return;
}

//================================== outSobj ========================================
void SMIObjectSet::outSobj(ofstream& sobj)
{
	sobj << "*OBJECTSET" << endl;

	char* pStr = _name.getString();
	sobj << pStr << endl;

	int ncl;
	if ( _isOfClass == "\0" ) {ncl = 0;}
	else			  {ncl = 1;}

	int numObj = _objectList.numOfEntries();

	char line[80];
	sprintf(line,"%5d%5d",ncl,numObj);
	sobj << line << endl;

	if (ncl==1) {sobj << _isOfClass.getString() << endl;}

	Name tempName; _objectList.reset();

	for (int i=0; i<numObj; i++) {
		_objectList.nextItem(tempName);
		sobj << tempName.getString() << endl;
	}
	sobj << "*END_OBJECTSET" << endl;
	return;
}
//---------------------------------------------------------------------
Name SMIObjectSet::setClass() const
{
	return _isOfClass;
}
//---------------------------------------------------------------------------
int SMIObjectSet::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
	
/*
  cout << endl 
  << " ====================== SMIObjectSet::examine() ============= " << endl;
	
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

	int classIntoAccount = 1;

	if ( _isOfClass == "" || _isOfClass == "VOID") {classIntoAccount = 0;}
	
	if ( _isOfClass == "" )
	{
		ErrorWarning::printHead("WARNING",firstLine
		,"No class declaration");
		cout << endl;
		cout << "  If all the objects that are or will be in the Object Set are" << endl
		     << "  of the same class, make the Object Set is_of_class." << endl
		     << "  That way more consistency checks can be performed" << endl
		     << "  If they can belong to different classes or no class," << endl
		     << "  make the class VOID"
		     << endl;
		
	}
	else
	{
		if (_isOfClass == "VOID") {}
		else
		{	
		// it belongs to a class that is not VOID
			void* pvoid = allClasses.gimePointer(_isOfClass);
	
			if ( pvoid == 0 )
			{
			//	retcode = 1;    review
				classIntoAccount = 0;
				ErrorWarning::printHead("SEVER WARNING",firstLine);
				cout << "  class " << _isOfClass << " is not declared " << endl;
			}
		}
	}


	
	// if there are objects listed, check that they are all declared and of the same class if relev.
	int numObj = _objectList.numOfEntries();
	if ( numObj > 0 )
	{
		Name objName; _objectList.reset();
		
		for (int i=0; i<numObj; i++)
		{
			_objectList.nextItem(objName);
//cout << i << " " << objName << " " << classIntoAccount << endl;
		// check the object was declared
			SMIObject* pObj = gimeObjectPointer(objName);
			if ( pObj == 0 ) // not declared
			{
			//	retcode = 1; review
				ErrorWarning::printHead("SEVERE WARNING",firstLine);
				cout << " Object " << objName << " is not declared " << endl;
			}
			else
			{
				if ( classIntoAccount == 0 ) { continue; }

			// pObj is pointer to object or to its class
				if ( pObj->isClass() )
				{
					if ( pObj->unitName() == _isOfClass )
					{
						continue;
					}
					else
					{
					//	retcode = 1; review
						ErrorWarning::printHead("SEVERE WARNING",firstLine);
						cout << " Object " << objName <<
							" is not of class " << _isOfClass << endl;	
					}
				}
				else
				{
				//	retcode = 1; review
					ErrorWarning::printHead("SEVERE WARNING",firstLine);
					cout << " Object " << objName <<
					" is not of class " << _isOfClass << endl;
				}
			}
		}
	}
	
	
	int iflg = examineUnits();
	
	return retcode+iflg;
}

//-------------------------------------------------------------------------------------------------------
Name SMIObjectSet::outString() 
{
	Name temp;
	
	temp = "OBJECTSET : ";
	temp += _name;
	
	if ( _isOfClass == "" ) {}
	else { temp += " is_of_class "; temp += _isOfClass; }
	
	int numObj = _objectList.numOfEntries();

	if ( numObj > 0 )
	{
		Name tempName; _objectList.reset();
		temp += " {"; _objectList.nextItem(tempName); temp += tempName;

		for (int i=1; i<numObj; i++) {
			_objectList.nextItem(tempName);
			temp += ","; temp += tempName;
		}
		temp += "}";
	}
	
		
	return temp;
	
}



