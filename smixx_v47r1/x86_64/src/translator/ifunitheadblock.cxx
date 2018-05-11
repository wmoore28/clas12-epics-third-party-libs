//-----------------------------------------------------------------------
//                         IfUnitHeadBlock  Class
//                                                 B. Franek
//                                                 4-April-2000
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ifunitheadblock.hxx"
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "errorwarning.hxx"

//--------------------------- Constructors -------------------------------

IfUnitHeadBlock::IfUnitHeadBlock ( ) 
	: SMLUnit("ifunit head block",2) {
	return;
}

IfUnitHeadBlock::~IfUnitHeadBlock() {
    delete _pSMLcode;
}

  
void IfUnitHeadBlock::translate() {

/*	_pSMLcode->out();
	SMLline line; char* pLine;*/
	
	SMLline lineBeingTranslated;

	_else = 0;

    Name token; int idel,jdel; int inext,jnext;
	int ist,jst;
	
	lineBeingTranslated = (*_pSMLcode)[0];
	char del = getNextToken(_pSMLcode,0,0," (",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	if ( token == "IF" ) {ist=idel; jst=jdel;}
	else if (token == "ELSE") {
		if (inext < 0) {_else = 1; return;}
		int idel1,jdel1,inext1,jnext1;
		lineBeingTranslated = (*_pSMLcode)[inext];
		del = getNextToken(_pSMLcode,inext,jnext," (",token,idel1,jdel1,inext1,jnext1);
		token.upCase(); token.trim();
		if (token == "IF") {ist=idel1; jst=jdel1;}
		else {
			ErrorWarning::printHead("ERROR",lineBeingTranslated
			," IF instruction syntax error");
			exit(2);
		}
	}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"IF, IF ELSE or ELSE not found while parsing IF instruction");
		exit(2);
	}
    
	_condition.setParentUnit(this);


	_condition.initialise(_pSMLcode,ist, jst, inext, jnext);
// the method will either succeed or terminate the program

	_condition.getHeader(_pSMLcode,ist,jst,inext,jnext);
	
    return;
}

void IfUnitHeadBlock::out(const Name offset) const
{

	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn << endl;

	if (_else==0){
		_condition.out(offset);
	}
	else {
		cout << ptn << " ELSE" << endl;
	}
	return;
}

//--------------------------------------------------------------------------
int IfUnitHeadBlock::examine()
{
  //cout << " IfUnitHeadBlock::examine() Id: " << endl;
	return _condition.examine();
}
