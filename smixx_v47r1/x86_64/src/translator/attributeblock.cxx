//-----------------------------------------------------------------------
//                         AttributeBlock  Class
//                                                 B. Franek
//                                                 2 November 1999
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "attributeblock.hxx"
#include "smlline.hxx"
#include "utilities.hxx"
#include "errorwarning.hxx"

//--------------------------- Constructors -------------------------------

AttributeBlock::AttributeBlock () 
	:SMLUnit("attribute block",2),
	 _attributes()
{
	return;
}


AttributeBlock::~AttributeBlock() {
    delete _pSMLcode;
}

//-------------------------------------- BF April 2000  ---------------- 
void AttributeBlock::translate() {

	SMLline firstLine = (*_pSMLcode)[0];
    Name token; int idel,jdel; int inext,jnext;
	int ist,jst;
	char del = getNextToken(_pSMLcode,0,0,":",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	if ( token == "OBJECT" || token == "CLASS" || token == "STATE"){}
	else {
		ErrorWarning::printHead("ERROR",firstLine);
		cout << " Looking for OBJECT or CLASS or STATE and found:" << endl;
		exit(2);
	}
    ist = inext; jst = jnext;

	del = getNextToken(_pSMLcode,ist,jst," /",token,idel,jdel,inext,jnext);

	if ( del == ' ' ) {
			del = getNextToken(_pSMLcode,inext,jnext," /",token,idel,jdel,inext,jnext);	
			token.upCase();
			if (!(token=="IS_OF_CLASS")) {
				ErrorWarning::printHead("ERROR",firstLine
				,"Expected keyword IS_OF_CLASS not found");
				 exit(2);
			}
			del = getNextToken(_pSMLcode,inext,jnext," /",token,idel,jdel,inext,jnext);	
	}

	if ( del != '/' ) {
		if ( inext >= 0 ) {
			ErrorWarning::printHead("ERROR",firstLine
			,"Unrecognised SML code following OBJECT or STATE declaration");
			exit(2);
		}
		else {
			return;
		}
	}

	for (;;) {
		del = getNextToken(_pSMLcode,inext,jnext," /",token,idel,jdel,inext,jnext);
        if (!(token == "\0")) {
			token.upCase();
			if (!check_name(token)) {
				ErrorWarning::printHead("ERROR",firstLine);
				cout << token << "  is not a name" << endl;
				exit(2);
			}
			_attributes += token;
		}
		if (inext<0) {break;}
	}

	return;
}

//--------------------------------------------  BF  April  2000  ------------------------
void AttributeBlock::out(const Name offset = "\0") const {

	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn << endl;
	int num = _attributes.length();
	if (num<=0) {
		cout << ptn << " No attributes" << endl;
	}
	else {
		for (int i=0; i<num; i++){
			cout << ptn << _attributes[i] << endl;
		}
	}
	return;
}

/*NameVector* AttributeBlock::pAttributes() 
{
	return &_attributes;
}
//------------------------------------------------  BF April 2000  ----
int AttributeBlock::numOfAttributes() const
{
	return _attributes.length();
}
//------------------------------------------------  BF  April 2000  ----
Name AttributeBlock::attribute(int inx) const
{
	return _attributes[inx];
}
*/
