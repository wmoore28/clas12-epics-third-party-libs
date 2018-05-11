//-----------------------------------------------------------------------
//                         ParameterBlock  Class
//                                                 B. Franek
//                                                 26 March 1999
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parameterblock.hxx"
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "errorwarning.hxx"

//--------------------------- Constructors -------------------------------

ParameterBlock::ParameterBlock ( ) 
	: SMLUnit("parameter block",2),
	  _parameters()
{
	return;
}

ParameterBlock::~ParameterBlock() {
    delete _pSMLcode;
}

  
void ParameterBlock::translate() 
{
	int numOfLines = _pSMLcode->length(); 
	if (numOfLines <= 0) {return;}

	Name token; int idel,jdel; int inext,jnext;
	int ist,jst;
	getNextToken(_pSMLcode,0,0,":",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	if ( token == "PARAMETERS"){}
	else {
		ErrorWarning::printHead("ERROR",(*_pSMLcode)[0]);
		cout << " Keyword PARAMETERS not found" << endl;
		exit(2);
	 }
    ist = inext; jst = jnext;

	_parameters.initFromSMLcode( 1, _pSMLcode, ist,jst,inext,jnext);
// If something goes wrong, Translator is terminated inside the method
	return;
}

void ParameterBlock::out(const Name offset) const
{

	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn << endl;
	_parameters.out(offset);

}


