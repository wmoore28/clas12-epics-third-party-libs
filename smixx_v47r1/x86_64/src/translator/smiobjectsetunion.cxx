//-----------------------------------------------------------------------
//                         SMIObjectSetUnion  Class
//                                                 B. Franek
//                                                  1-Dec-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "smlline.hxx"
#include "smiobjectsetunion.hxx"
#include "utilities.hxx"
#include "registrar.hxx"
#include "smiobjectset.hxx"
#include "errorwarning.hxx"

	extern Registrar allObjectSets;
	extern Registrar allUnits;

//--------------------------- Constructors -------------------------------

SMIObjectSetUnion::SMIObjectSetUnion
       ( const Name& name)
	   : SMLUnit("ObjectSetUnion",20,name),
	     _isOfClass(""),
	     _objectSetList()
{

//	cout << endl << "Object Set Union  " << _name << endl;
	return;
}
//-------------------------------------------------------------------------
SMIObjectSetUnion::~SMIObjectSetUnion() {
    delete _pSMLcode;
}
//-------------------------------------------------------------------------- 
void SMIObjectSetUnion::translate() {
//cout << endl << " SMIObjectSetUnion::translate()  entered " << endl;

	Name token; int idel,jdel; int inext,jnext; int ist,jst;
	
	SMLline lineBeingTranslated = (*_pSMLcode)[0];
	char del = getNextToken(_pSMLcode,0,0,":",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	if ( token == "OBJECTSET" ) {}
	else 
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Expecting OBJECTSET , but found " << token << endl;
		exit(2);
	}

	ist = inext; jst = jnext;
	lineBeingTranslated = (*_pSMLcode)[ist];
	del = getNextToken(_pSMLcode,ist,jst," ",token,idel,jdel,inext,jnext);
        token.upCase(); token.trim(); // this is the name of the set

//cout << " name : " << token << "  del |" << del << "|" << endl;
//if ( del == '\0' ) { cout << " following string is NULL" << endl;}
//else { cout << " following string is "; ((*_pSMLcode)[inext]).out(jnext,jnext+20); cout << endl;}

        if ( del != ' ' ) 
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " instruction terminating too soon "
			  << token << endl;
			exit(2);
	}
	
	ist = inext; jst = jnext;
	lineBeingTranslated = (*_pSMLcode)[ist];
	del = getNextToken(_pSMLcode,ist,jst," {",token,idel,jdel,inext,jnext);
        token.upCase(); token.trim(); // this is the union keyword 

//cout << " token(union) : " << token << "  del |" << del << "|" << endl;
//if ( del == '\0' ) { cout << " following string is NULL" << endl;}
//else { cout << " following string is "; ((*_pSMLcode)[inext]).out(jnext,jnext+20); cout << endl;}	
	
	if ( token == "UNION" ) {}
	else
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Looking for UNION but found" << token << endl;
		exit(2);
	}	
	
	
	if ( del != '{' ) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "List of sets is missing" << endl;
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
			_objectSetList.add(token);
			void *ptn = allObjectSets.gimePointer(token);
			if (!ptn) {
				ErrorWarning::printHead("ERROR",lineBeingTranslated);
				cout << " Object set " << token << "  has not been declared" << endl;
				exit(2);
			}
			Name type = allUnits.gimeName(ptn);
//	cout << type << "  " << _setNm << endl;
			if ( type == "ObjectSetUnion" )
			{
				ErrorWarning::printHead("ERROR",lineBeingTranslated);
				cout << token << "  is union of Object Sets" << endl;
				cout << " At the moment, you can not have a union of other unions" << endl;
				exit(2);
			}
		}
		if ( del == '}') {break;}	
	}
//Name offset = " "; _objectList.out(offset);
	return;

}

//================================== out ============================================
void SMIObjectSetUnion::out(const Name offset = "\0")  {

	SMLUnit::out(offset);

	char* ptn = offset.getString(); cout << ptn << endl;
        cout << ptn << "ObjectSet " << _name << "  is union of "  << endl;
        cout  << _objectSetList.numOfEntries() << " object sets" << endl;

       _objectSetList.reset(); Name tempName;
       for (int i=0; i<_objectSetList.numOfEntries(); i++) {
		_objectSetList.nextItem(tempName);
		cout << ptn << tempName << endl;
	}
	return;
}

//================================== outSobj ========================================
void SMIObjectSetUnion::outSobj(ofstream& sobj)
{
	sobj << "*OBJECTSETUNION" << endl;

	char* pStr = _name.getString();
	sobj << pStr << endl;

	int numObjSet = _objectSetList.numOfEntries();

	char line[80];
	sprintf(line,"%5d",numObjSet);
	sobj << line << endl;

	Name tempName; _objectSetList.reset();

	for (int i=0; i<numObjSet; i++) {
		_objectSetList.nextItem(tempName);
		sobj << tempName.getString() << endl;
	}
	sobj << "*END_OBJECTSETUNION" << endl;
	return;
}
//---------------------------------------------------------------------
Name SMIObjectSetUnion::setClass() const
{
	return _isOfClass;
}
//---------------------------------------------------------------------------
int SMIObjectSetUnion::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
	
/*
  cout << endl 
  << " ====================== SMIObjectSetUnion::examine() ============= " << endl;
	
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
	
	// check that all the Object Sets listed are declared.
	int numObjSet = _objectSetList.numOfEntries();
	if ( numObjSet > 0 )
	{
		Name objSetName; _objectSetList.reset();
		Name commonClass("");
		
		for (int i=0; i<numObjSet; i++)
		{
			_objectSetList.nextItem(objSetName);
		// check the object set was declared
			SMIObjectSet* pObjSet =
			  (SMIObjectSet*)(allObjectSets.gimePointer(objSetName));
			if ( pObjSet == 0 ) // not declared
			{
			//	retcode = 1; review
				ErrorWarning::printHead("SEVERE WARNING",firstLine);
				cout << " Object Set " << objSetName << " is not declared " << endl;
				commonClass = "";
			}
			else
			{
				if ( i==0 ) {commonClass = pObjSet->setClass();}
				else 
				{
					if ( commonClass == pObjSet->setClass() ) {}
					else {commonClass = "";}
				}
			}
		}
		if ( commonClass == "" ) {}
		else 
		{
			_isOfClass = commonClass;
//			ErrorWarning::printHead("WARNING",firstLine
//			,"examining Object Set Union");
//			
//			cout << " Object Set Union " <<
//				" has been made of class " << _isOfClass << endl;

		}
	}
	
	
	int iflg = examineUnits();
	
	return retcode+iflg;
}

//-------------------------------------------------------------------------------------------------------
Name SMIObjectSetUnion::outString() 
{
	Name temp;
	
	temp = "ObjectSet : ";
	temp += _name;
	temp += " union ";
		
	int numObjSet = _objectSetList.numOfEntries();

	if ( numObjSet > 0 )
	{
		Name tempName; _objectSetList.reset();
		temp += " {"; _objectSetList.nextItem(tempName); temp += tempName;

		for (int i=1; i<numObjSet; i++) {
			_objectSetList.nextItem(tempName);
			temp += ","; temp += tempName;
		}
		temp += "}";
	}
	
		
	return temp;
	
}
