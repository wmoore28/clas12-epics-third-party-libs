// CreateObjectins.cxx: implementation of the CreateObjectIns class.
//
//                                                B. Franek
//                                               April 2010
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "createobjectins.hxx"
#include "registrar.hxx"
#include "errorwarning.hxx"

	extern Registrar allUnits;
	extern Registrar allClasses;
	
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CreateObjectIns::CreateObjectIns()  
{
   _name = "CreateObject";
   return;
}

CreateObjectIns::~CreateObjectIns()
{
    delete _pSMLcode;
}

void CreateObjectIns::translate() {

	Name token; int idel,jdel; int inext,jnext; 
	Name ofClass;
	
	SMLline lineBeingTranslated;
	
	lineBeingTranslated = (*_pSMLcode)[0];
	
	char del = getNextToken(_pSMLcode,0,0," ",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();

	if ( token == "CREATE_OBJECT" ) {
		lineBeingTranslated = (*_pSMLcode)[inext];
		del = getNextToken(_pSMLcode,inext,jnext," ",_objectIdentifier,idel,jdel,inext,jnext);
	}
	else  {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "Expected CREATE_OBJECT instruction" << endl;
		exit(2);
	}
	
	if (_objectIdentifier[0] == '$') {
		Name tmpName="\0"; 
		char* ptn = _objectIdentifier.getString(); int length = strlen(ptn);
		if ( *(ptn+1) == '(' && *(ptn+length-1) == ')') {
			*(ptn+length-1) = '\0'; tmpName = (ptn+2);
			_objectIdentifier = "&VAL_OF_"; _objectIdentifier += tmpName;
		}
	}
	
	_objectIdentifier.upCase(); _objectIdentifier.trim();
	if (!check_name(_objectIdentifier)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Object identifier " << _objectIdentifier << "  is not a name" << endl;
		exit(2);
	}
	
	lineBeingTranslated = (*_pSMLcode)[inext];
	del = getNextToken(_pSMLcode,inext,jnext," ",ofClass,idel,jdel,inext,jnext);
	
	ofClass.upCase();
	if ( ofClass == "OF_CLASS" ) {}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"OF_CLASS keyword not found");
		exit(2);
	}
	
	lineBeingTranslated = (*_pSMLcode)[inext];
	del = getNextToken(_pSMLcode,inext,jnext," ",_className,idel,jdel,inext,jnext);
	
	_className.upCase(); _className.trim();
	if (!check_name(_className)) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Class name : " << _className << "  is not a name" << endl;
		exit(2);
	}
	
	void *ptn = allClasses.gimePointer(_className);
	if (!ptn) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Class : " << _className << "  has not been declared" << endl;
		exit(2);
	}
	
        if (inext>0) {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"Some crap is following CREATE_OBJECT instruction");
		exit(2);
	}
	return;
	
}

//--------------------------------------------------------------------------
void CreateObjectIns::out(const Name offset) const
{
	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn ;
	
	cout << "createObject " << _objectIdentifier << " of_class " << _className << endl;
	return;
}
//------------------------------------------  BF April 2010  -----------
void CreateObjectIns::outSobj(int&,int,ofstream& sobj,Registrar&) const
{

	sobj << "create_object" << endl;

        sobj << _objectIdentifier.getString() << endl;
	
// The following line makes it look like declaration of an object belonging
// to class and also having no attributes of its own... see isofclassobject.cxx	

	int n1,n2,n3; char line[80];
	n1 = 1; n2 = 0; n3 = 0;
	sprintf(line,"%5d%5d%5d",n1,n2,n3);
	sobj << line << endl;
//----------------------------------------------------------------
	
	sobj << _className.getString() << endl;
	
	return;
}
