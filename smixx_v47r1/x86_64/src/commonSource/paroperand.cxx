// paroperand.cxx: implementation of the ParOperand class.
//
//                                                B. Franek
//                                              January 2012
// Copyright Information:
//      Copyright (C) 1999/2008 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <cstring>
#include "utilities.hxx"
#include "paroperand.hxx"
#include "errorwarning.hxx"
#include "smlline.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParOperand::ParOperand()
{
	_operandType = "";
	_valueType = "";
	_value ="";
	_parName ="";
	_ownerObjName ="";
	_tobeCastedTo ="";
	
	_initNotFinished = -1;
	_castingRequested = 0;
	_operValueType = "";
	return;
}

int ParOperand::initFromSMLcode(SMLlineVector* pSMLcode
	                   , int& inext, int& jnext) 
{
	_initNotFinished = 0;

	SMLline lineBeingTranslated = (*pSMLcode)[inext];
// -- First make sure that all the leading spaces and tabs are skipped --
	int ist,jst,inbl,jnbl,dum1,dum2,dum3,dum4;
	ist = inext; jst = jnext;
	char firstNBLchr = firstNonBlank(pSMLcode,ist,jst,inbl,jnbl,dum1,dum2,dum3,dum4);
	if ( firstNBLchr == '\0' )
	{  // end of the code before NBL char found
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"parsing Operand");
		
		cout << endl << " ParOperand::initFromSMLcode ..." << endl;
		cout << " Failed getting operand value" << endl;
		exit(2);	 
	}
	inext = inbl; jnext = jnbl;
//--------------------------------------------------------------------------------
	
	int idel,jdel; int ierr;
	char typevch[12]; 
	 char del; Name value;
//cout << " Entering getValue " << " inext jnext " << inext << " " << jnext << endl;
	lineBeingTranslated = (*pSMLcode)[inext];

// Here will come check for cast. The cast must be on the same line as the following name and immediately preceed it

	checkForCast(lineBeingTranslated,jnext,jnext);
	
//	lineBeingTranslated.indicateCharacter(" First character for ParOperand::initialise..., before getValue",jnext); //debug
	del = getValue(pSMLcode,inext,jnext," )="
	                  ,value,typevch,ierr
		          ,idel,jdel,inext,jnext);
//cout << " delimiter : |" << del << "|  idel jdel inext jnext "
//<< idel << " " << jdel << " " << inext << " " << jnext << endl; 
	if ( ierr != 0 )
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"parsing Operand");
		
		cout << endl << " ParOperand::initFromSMLcode ..." << endl;
		cout << " Failed getting operand value" << endl;
		exit(2);
	}

	if ( del == ')' ) 
	{
		inext = idel; jnext = jdel; // closing bracket has to be found
	}
	if ( del == '=' ) 
	{
		inext = idel; jnext = jdel; // same about the equal sign
	}

//cout << value << typevch << endl;
	
	Name operandType = typevch;
	if ( operandType == "UNKNOWN" )
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"parsing Operand");
		
		cout << endl << " ParOperand::initFromSMLcode ..." << endl;
		cout << " Unrecognised operand" << endl;
		exit(2);
	}
	
	if ( operandType == "NAME" )
	{
		_operandType = operandType;
		_parName = value;
		_ownerObjName = "THIS";
		
	}
		
	else if ( operandType == "COMPNAME" )
	{
		_operandType = operandType;
		char *pvalue = value.getString();
		char *pdot, *pless, *pgt;
		if ( ( pdot = strrchr(pvalue,'.') ) )
		{
			*pdot = '\0';
			_ownerObjName = pvalue;
			*pdot ='.';
			_parName = (pdot+1);
		}
		else if ( ( pless = strrchr(pvalue,'<') ) )
		{
			*pless = '\0';
			_ownerObjName = pvalue;
			*pless = '<';
			pgt = strrchr(pvalue,'>');
			*pgt = '\0';
			_parName = (pless+1);
			*pgt = '>';
		}
	}

	else
	{
		_operandType = "VALUE";
		_valueType = operandType;
		_value = value;
	}
	return 0;
}
//-----------------------------------------------------------------------------
void ParOperand::checkForCast(const SMLline& line, const int& jst, int& jnext)
{
// In the following we assume, that all the leading blanks were skipped

//	line.indicateCharacter(" First character for ParOperand::checkForCast",jst); //debug

	char* pointerToLineBody = line.getString();
	char* pointerTojstIn =&pointerToLineBody[jst];

	Name temp = pointerTojstIn;
	temp.upCase();  //convert it to uppercase
	char* pointerTojst = temp.getString();
		
	_castingRequested = 1;
     
	if (strncmp("(S)",pointerTojst,3) == 0) {_tobeCastedTo = "STRING"; jnext = jst+3; return;}
	if (strncmp("(STRING)",pointerTojst,8) == 0) {_tobeCastedTo = "STRING"; jnext = jst+8; return;} 

	if (strncmp("(I)",pointerTojst,3) == 0) {_tobeCastedTo = "INT"; jnext = jst+3; return;} 
	if (strncmp("(INT)",pointerTojst,5) == 0) {_tobeCastedTo = "INT"; jnext = jst+5; return;}
 
	if (strncmp("(F)",pointerTojst,3) == 0) {_tobeCastedTo = "FLOAT"; jnext = jst+3; return;} 
	if (strncmp("(FLOAT)",pointerTojst,7) == 0) {_tobeCastedTo = "FLOAT"; jnext = jst+7; return;}

	_castingRequested = 0;
	_tobeCastedTo = "\0";
	jnext = jst;
	return; 
}
//--------------------------------------------------------------------------
int ParOperand::initFromSobj_firstLine(char line[])
{
  if ( _initNotFinished == 1 )
    // The object is already being initialised
    {
		cout << " ***Error : Initialising from sobj ; Internal error " << endl;
		abort();					
    }       
	_initNotFinished = 1;
	
	_operandType = line;
	
	if ( _operandType == "VALUE" )
	{
		_operValueType = "";
		_value = "";
		 return 1; 
	}
	if ( _operandType == "NAME" )
	{
		_operValueType = "";
		_parName = "";
		 return 1;
	}
	if ( _operandType == "COMPNAME" )
	{
		_operValueType = "";
		_ownerObjName = "";
		_parName = "";
		 return 1;
	}
	
	cout << " ***Error : Initialising from sobj ; Internal error " << endl;
	abort();
	
}
//--------------------------------------------------------------------------
int ParOperand::initFromSobj_nextLine(char line[])
{
  if ( _initNotFinished != 1 )
    // The object is not being initialised
    {
		cout << " ***Error : Initialising from sobj ; Internal error " << endl;
		abort();					
    }       

	if ( _operandType == "VALUE" )
	{
		if ( _operValueType == "" ) { _operValueType = line; return 1;}
		if ( _value == "" )
		{	_value = line;
			_initNotFinished = 0;
			return 0;
		}
		cout << " ***Error : Initialising from sobj ; Internal error " << endl;
		abort();					
	}

	if ( _operandType == "NAME" )
	{
		if ( _operValueType == "" ) { _operValueType = line; return 1;}
		if ( _parName == "" )
		{	_parName = line;
			_initNotFinished = 0;
			return 0;
		}
		cout << " ***Error : Initialising from sobj ; Internal error " << endl;
		abort();					
	}

	if ( _operandType == "COMPNAME" )
	  {
	  	if ( _operValueType == "" ) { _operValueType = line; return 1;}
		if ( _ownerObjName == "" ) { _ownerObjName = line; return 1;}
		if ( _parName == "" )
		{	_parName = line;
			_initNotFinished = 0;
			return 0;
		}
		cout << " ***Error : Initialising from sobj ; Internal error " << endl;
		abort();					
	}
	cout << " ***Error : Initialising from sobj ; Internal error " << endl;
	abort();
}
ParOperand::~ParOperand()
{
	return;
}

void ParOperand::out(const Name offset) const
{
	char* poff = offset.getString();
	
	cout << endl << poff << " Operand type : " << _operandType << endl;
        cout << poff         << " Value Type   : " << _valueType << endl;
        cout << poff         << " Value        : " << _value << endl;
        cout << poff         << " Par Name     : " << _parName << endl;
        cout << poff         << " Owner Object : " << _ownerObjName << endl;
        cout << poff         << " tobe casted  : " << _tobeCastedTo << endl;
	cout << poff         << " casting req  : " << _castingRequested << endl;
	cout << poff         << " Operand val t: " << _operValueType << endl;
 
        return;
}
//----------------------------------------------------------------------
void ParOperand::outShort() const
{
	cout <<
	printingName() << " " << _operandType << " " << _operValueType << endl;
	return;
}
//======================================================================
void ParOperand::outSobj( ofstream& sobj ) const
{
	sobj << _operandType.getString()  << endl;
	
	if ( _operandType == "VALUE" )
	{
		sobj << _operValueType.getString() << endl;
		sobj << _value.getString() << endl;
		return;
	}
		
	if ( _operandType == "NAME" )
	{
		sobj << _operValueType.getString() << endl;
		sobj << _parName.getString() << endl;
		return;
	}
	
	if ( _operandType == "COMPNAME" )
	{
		sobj << _operValueType.getString() << endl;
		sobj << _ownerObjName.getString() << endl;
		sobj << _parName.getString() << endl;
		return;
	}

	return;
}
//======================================================================
Name& ParOperand::operandType() 
{
	return _operandType;
}
//======================================================================
Name& ParOperand::valueType() 
{
	return _valueType;
}
//======================================================================
Name& ParOperand::value() 
{
	return _value;
}
//======================================================================
Name& ParOperand::parName() 
{
	return _parName;
}
//======================================================================
Name& ParOperand::ownerObjName() 
{
	return _ownerObjName;
}
//======================================================================
Name& ParOperand::tobeCastedTo() 
{
	return _tobeCastedTo;
}

//======================================================================
void ParOperand::thisObjPar() 
{
	_operandType = "COMPNAME";
	_ownerObjName = "THIS";
	return ;
}
//======================================================================
void ParOperand::setValue(Name& value)
{
	_value = value;
	return;
}
//======================================================================
void ParOperand::setValueType(Name& valueType)
{
	_valueType = valueType;
	return;
}
//======================================================================
void ParOperand::setCast(Name& cast)
{
	_tobeCastedTo = cast;
	return;
}
//======================================================================
Name ParOperand::operValueType()
{
	return _operValueType;
}
//=======================================================================
int ParOperand::castingRequested()
{
	return _castingRequested;
}
//=======================================================================
int ParOperand::calcOperValueType()
{
	_operValueType = _valueType;
	if ( _tobeCastedTo == "" ) {return 1;} // casting requeste - easy
	
	if ( _operandType == "VALUE" )
	{ 
		_tobeCastedTo = ""; _castingRequested =0;
		return 2; // can not cast VALUE casting ignored
	}
	
	if ( _tobeCastedTo == "FLOAT" && _valueType == "STRING" )
	{
		_tobeCastedTo = ""; _castingRequested =0;
		return -1;  // casting from STRING to FLOAT not allowed
	}
 	
	_operValueType = _tobeCastedTo;
	return 1;	
}
//=======================================================================
Name ParOperand::printingName() const
{
	Name temp;
		
	if ( _operandType == "VALUE" )
	{
		temp = _value;
	}
	else if ( _operandType == "NAME" )
	{
		temp = _parName;
	}
	else
	{ 
		temp = _ownerObjName;
		temp += ".";
		temp += _parName;
	}
	
	return temp;
}
