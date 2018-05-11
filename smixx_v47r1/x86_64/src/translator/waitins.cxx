// waitins.cxx: implementation of the WaitIns class.
//
//                                                B. Franek
//                                           16 December 2008
// Copyright Information:
//      Copyright (C) 1999/2008 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <cstring>
#include "utilities.hxx"
#include "waitins.hxx"
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "registrar.hxx"
#include "errorwarning.hxx"

extern Registrar allObjects, allObjectSets;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WaitIns::WaitIns()  
{
   _name = "wait";
//   cout << "wait constructor called" << endl;
   return;
}

WaitIns::~WaitIns()
{
    delete _pSMLcode;
}
//--------------------------------  translate  -------------------------
void WaitIns::translate()
{
//	cout << "  translating wait " << endl;
//	_pSMLcode->out(" ");
	
	SMLline lineBeingTranslated;
	
	Name token; int idel,jdel; int inext,jnext;
	int ist,jst;
	
	lineBeingTranslated = (*_pSMLcode)[0];
	char del = getNextToken(_pSMLcode,0,0," (",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();

	if ( token == "WAIT"){}
	else
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Keyword WAIT not found" << endl;
		exit(2);
	}

	ist = inext; jst = jnext;

	if (ist < 0 || del != '(')
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Nothing sensible follows the keyword WAIT"
		     << endl;
		exit(2);
	}
	
	lineBeingTranslated = (*_pSMLcode)[ist];
	del = getNextToken(_pSMLcode,ist,jst,")",token,idel,jdel,ist,jst);

	if ( del != ')' )
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "The list does not terminate with )"
		     << endl;
		exit(2);
	}
	
//	cout << token << endl;	
	
	int iel = 0;
	Name element, temp;
	int objSet;
	Name name;
	
	while ( token.element(iel,',',element) )
	{
//		cout << element << endl;
		element.upCase(); element.trim(); element.trimFront();

		if ( strncmp("ALL_IN ",element.getString(),7) == 0 )
		{   //  object set name should follow
			objSet = 1;
			name = &element[7]; name.trimFront();
		}
		else
		{  
			objSet = 0;
			name = element;
		}
		if ( !check_name(name) )
		{
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << " the string " << name
				     << " is not a name"
		     			<< endl;
			exit(2);
		}
		
		if ( objSet == 0 )
		{
//			cout << " object " << name << endl;
			if ( _refObjects.isPresent(name) )
			{
				ErrorWarning::printHead("ERROR",lineBeingTranslated);
				cout << " the object " << name
				     << " occurs in the list more than once"
		     			<< endl;
				exit(2);
			}
			_refObjects += name;
		}
		else
		{
//			cout << " objectSet " << name << endl;
			if ( _refObjectSets.isPresent(name) )
			{
				ErrorWarning::printHead("ERROR",lineBeingTranslated);
				cout << " the object Set " << name
				     << " occurs in the list more than once"
		     			<< endl;
				exit(2);
			}
			_refObjectSets += name;
		}
		iel++;
	}
//	_refObjects.out(" ");
//	_refObjectSets.out(" ");	
	return;
}
//-----------------------------------------------------------------------
void WaitIns::out(const Name offset) const
{
	SMLUnit::out(offset);
	char* ptn=offset.getString();
	cout << ptn ; cout << "Objects to wait for:" << endl;
	_refObjects.out(offset);
	
	cout << ptn ; cout << "Objects in object sets to wait for:" << endl;
	_refObjectSets.out(offset);
	
	return;
}

//----------------------------------  BF January 2009  ------------------
void WaitIns::outSobj(int&,int,ofstream& sobj,Registrar&) const
{
	sobj << "wait" << endl;
	
	int numObjects = _refObjects.length();
	sobj << "    " << numObjects << endl;
	
	Name objname;	
	for (int io=0; io<numObjects; io++)
	{
		objname = _refObjects[io];
		sobj << objname.getString() << endl;
	}
	
	int numObjectSets = _refObjectSets.length();
	sobj << "    " << numObjectSets << endl;
	
	Name objsetname;	
	for (int ios=0; ios<numObjectSets; ios++)
	{
		objsetname = _refObjectSets[ios];
		sobj << objsetname.getString() << endl;
	}


	return;
}

//---------------------------------------------------------------------------
int WaitIns::examine()
{
	int retcode = 0;
	SMLline firstLine = (*_pSMLcode)[0];
/*
  cout << endl 
  << " ====================== WaitIns::examine() ============= " << endl;
	
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

//Name offset(" ");
//cout << endl; cout << " Objects: "; _refObjects.out(offset); cout << endl;
//cout << endl; cout << " Object Sets "; _refObjectSets.out(offset); cout << endl<< endl;

	int numObjects = _refObjects.length();
	Name objname;
	
	for (int i=0; i<numObjects; i++)
	{
		objname = _refObjects[i];
		void* pvoid = allObjects.gimePointer(objname);
		if ( pvoid == 0 )
		{
		//	retcode = 1; review
			ErrorWarning::printHead("SEVERE WARNING",firstLine);
			cout << " Object " << objname  <<
				" is not declared " << endl;
			cout << endl; _pParentUnit->printParents(); cout << endl;

	
		}
	}
	
	int numObjectSets = _refObjectSets.length();
	Name objsetname;
	
	for (int i=0; i<numObjectSets; i++)
	{
		objsetname = _refObjectSets[i];
		void* pvoid = allObjectSets.gimePointer(objsetname);
		if ( pvoid == 0 )
		{
		//	retcode = 1; review
			ErrorWarning::printHead("SEVERE WARNING",firstLine);
			cout << " Object Set " << objsetname  <<
				" is not declared " << endl;
			cout << endl; _pParentUnit->printParents(); cout << endl;

	
		}
	}

	int iflg = examineUnits();
	
	return retcode+iflg;
}

//-------------------------------------------------------------------------------------------------------
Name WaitIns::outString() 
{
	Name temp;
	
	temp = "WAIT ( ";
	
	int first = 0;
	int numObjects = _refObjects.length();
	Name objname;
	
	for (int i=0; i<numObjects; i++)
	{
		objname = _refObjects[i];
		if ( first == 0 )
		{
			first = 1;
			temp += objname;
		}
		else
		{
			temp += ", ";
			temp += objname;
		}
	}
	
	int numObjectSets = _refObjectSets.length();
	Name objsetname;
	
	for (int i=0; i<numObjectSets; i++)
	{
		objsetname = _refObjectSets[i];
		if ( first == 0 ) 
		{ 
			first = 1;
			temp += " all_in ";
			temp += objsetname;
		}
		else
		{
			temp += ", all_in ";
			temp += objsetname;
		} 
		
	}
	
	temp += " )";
	
	return temp;
}


/*int main()
{
	return 1;
}*/
