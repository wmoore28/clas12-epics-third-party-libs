// insertins.cxx: implementation of the InsertIns class.
//
//                                                B. Franek
//                                               2 July 2001
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "insertins.hxx"
#include "registrar.hxx"
#include "errorwarning.hxx"

	extern Registrar allObjectSets;
	extern Registrar allUnits;
	
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InsertIns::InsertIns()  
{
   _name = "insert";
   return;
}

InsertIns::~InsertIns()
{
    delete _pSMLcode;
}

void InsertIns::translate() {

	Name token; int idel,jdel; int inext,jnext; Name objectSet,objectSet1;
	Name inFrom;
	
	SMLline lineBeingTranslated;
	
	lineBeingTranslated = (*_pSMLcode)[0];
	
	char del = getNextToken(_pSMLcode,0,0," ",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();

	if ( token == "INSERT" ) {
		_insertRemove = 1;
		
		lineBeingTranslated = (*_pSMLcode)[inext];
		del = getNextToken(_pSMLcode,inext,jnext," ",_objectNm,idel,jdel,inext,jnext);
	}
	else if ( token == "REMOVEALL" || token == "REMOVE_ALL") {
		_insertRemove = 0;
		_objectNm = "&ALL";
	}
	else  {
		_insertRemove = 0;
		lineBeingTranslated = (*_pSMLcode)[inext];
		del = getNextToken(_pSMLcode,inext,jnext," ",_objectNm,idel,jdel,inext,jnext);
	}
	if (_objectNm[0] == '$') {
		Name tmpName="\0"; 
		char* ptn = _objectNm.getString(); int length = strlen(ptn);
		if ( *(ptn+1) == '(' && *(ptn+length-1) == ')') {
			*(ptn+length-1) = '\0'; tmpName = (ptn+2);
			_objectNm = "&VAL_OF_"; _objectNm += tmpName;
		}
	}
	_objectNm.upCase(); _objectNm.trim();
	if (!check_name(_objectNm)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Object : " << _objectNm << "  is not a name" << endl;
		exit(2);
	}
	lineBeingTranslated = (*_pSMLcode)[inext];
	del = getNextToken(_pSMLcode,inext,jnext," ",inFrom,idel,jdel,inext,jnext);
	inFrom.upCase();
	if (    (token == "INSERT" && inFrom == "IN") 		||
		(token == "REMOVE" && inFrom == "FROM") 	||
		( (token == "REMOVEALL" || token == "REMOVE_ALL") && inFrom == "FROM")
	) {}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"unrecognised instruction");
		exit(2);
	}
	lineBeingTranslated = (*_pSMLcode)[inext];
	del = getNextToken(_pSMLcode,inext,jnext," ",_setNm,idel,jdel,inext,jnext);
	_setNm.upCase(); _setNm.trim();
	if (!check_name(_setNm)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Object Set : " << _setNm << "  is not a name" << endl;
		exit(2);
	}
	void *ptn = allObjectSets.gimePointer(_setNm);
	if (!ptn) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Object Set : " << _setNm << "  has not been declared" << endl;
		exit(2);
	}
	Name type = allUnits.gimeName(ptn);
//	cout << type << "  " << _setNm << endl;
	if ( type == "ObjectSetUnion" )
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "direct insertion/removal into/from UNION is not allowed" << endl;
		exit(2);
	}
	
        if (inext>0) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"some crap follows INSERT/REMOVE instruction");
		exit(2);
	}
	return;
	
}

//--------------------------------------------------------------------------
void InsertIns::out(const Name offset) const
{
	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn ;
	if (_insertRemove) {cout << " insert Object " << _objectNm << " in Set ";}
	else {cout << " remove Object " << _objectNm << " from Set ";}
	cout << _setNm << endl;

	return;
}
//------------------------------------------  BF July  2001  -----------
void InsertIns::outSobj(int&,int,ofstream& sobj,Registrar&) const
{

	sobj << "insert" << endl;

	sobj << "    " << _insertRemove << endl;

        sobj << _objectNm.getString() << endl;

	sobj << _setNm.getString() << endl;
	
	return;
}
