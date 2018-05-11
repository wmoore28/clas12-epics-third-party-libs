//-----------------------------------------------------------------------
//                         ActionHeadBlock  Class
//                                                 B. Franek
//                                                 23 November 1999
//-----------------------------------------------------------------------
//
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "actionheadblock.hxx"
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "errorwarning.hxx"

//--------------------------- Constructors -------------------------------

ActionHeadBlock::ActionHeadBlock ( ) 
	: SMLUnit("action head block",2),
	  _parameters() {
	return;
}

ActionHeadBlock::~ActionHeadBlock() {
    delete _pSMLcode;
}

  
void ActionHeadBlock::translate() {

/*	_pSMLcode->out();
	SMLline line; char* pLine;*/

	SMLline firstLine = (*_pSMLcode)[0];
	
    Name token; int idel,jdel; int inext,jnext;
	int ist,jst;
	char del = getNextToken(_pSMLcode,0,0,":",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	if ( token == "ACTION"){}
	else
	{
		ErrorWarning::printHead("ERROR",firstLine,"Keyword ACTION not found");
		exit(2);
	}
    ist = inext; jst = jnext;

//	line = (*_pSMLcode)[ist];pLine = line.getString();

	del = getNextToken(_pSMLcode,ist,jst," (",token,idel,jdel,inext,jnext);

/*	cout << " idel,jdel " << idel << ", " << jdel << "  del |" << del << "|" << endl;
	cout << " next,jnext " << inext << ", " << jnext << endl;*/

	if ( del != '(' ) {
		if ( inext >= 0 )
		{
			ErrorWarning::printHead("ERROR",firstLine,
			"Unrecognised code follows action declaration");
			exit(2);
		}
		else {
			return;
		}
	}

	ist = idel; jst = jdel;
	_parameters.initFromSMLcode( 1, _pSMLcode, ist,jst,inext,jnext);
// If something goes wrong, Translator is terminated inside the method
	return;
}

void ActionHeadBlock::out(const Name offset) const
{

	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn << endl;
	_parameters.out(offset);

}
//-------------------------------------------  BF April 2000  --------------------
//Parms* ActionHeadBlock::pParameters()
//{
//	return &_parameters;
//}
