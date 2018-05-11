// sleepins.cxx: implementation of the SleepIns class.
//
//                                                B. Franek
//                                                June 2011
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "sleepins.hxx"
#include "registrar.hxx"
#include "errorwarning.hxx"

	extern Registrar allObjectSets;
	extern Registrar allUnits;
	
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SleepIns::SleepIns()  
{
   _name = "sleep";
   return;
}

SleepIns::~SleepIns()
{
    delete _pSMLcode;
}

void SleepIns::translate() {

	Name token; int idel,jdel; int inext,jnext;
	Name inFrom;
	
	SMLline lineBeingTranslated = (*_pSMLcode)[0];
	
	char del = getNextToken(_pSMLcode,0,0," ",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();

	if ( token == "SLEEP" )
	{
		lineBeingTranslated = (*_pSMLcode)[inext];
		del = getNextToken(_pSMLcode,inext,jnext," ",_seconds,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	
	}
	else
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"unrecognised code encountered while attempting to parse SLEEP instruction");
		exit(2);
	}

	if (_seconds[0] == '$') {
		Name tmpName="\0"; 
		char* ptn = _seconds.getString(); int length = strlen(ptn);
		if ( *(ptn+1) == '(' && *(ptn+length-1) == ')')
		{
			*(ptn+length-1) = '\0'; tmpName = (ptn+2);
			_seconds = "&VAL_OF_"; _seconds += tmpName;
		}
		if (!check_name(tmpName))
		{
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << tmpName << "  is not a name" << endl;
			exit(2);
		}
	}
	else {
		if (!check_int(_seconds))
		{
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << _seconds << "  is not an integer" << endl;
			exit(2);
		}
	}
	
        if (inext>0) {
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << "There is some crap following a SLEEP instruction" << endl;
		exit(2);
	}
	return;
	
}

//--------------------------------------------------------------------------
void SleepIns::out(const Name offset) const
{
	SMLUnit::out(offset); 
	char* ptn=offset.getString(); cout << ptn ;
	cout << " sleep " << _seconds <<  endl;

	return;
}
//------------------------------------------  BF June  2011  -----------
void SleepIns::outSobj(int&,int,ofstream& sobj,Registrar&) const
{

	sobj << "sleep" << endl;


        sobj << _seconds.getString() << endl;
	
	return;
}
