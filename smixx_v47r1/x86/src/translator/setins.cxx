// setins.cxx: implementation of the SetIns class.
//
//                                                B. Franek
//                                           30 September 1999
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "smlunit.hxx"
#include "smlline.hxx"
#include "name.hxx"
#include "utilities.hxx"
#include "ut_tr.hxx"
#include "setins.hxx"
#include "errorwarning.hxx"
#include "opermanager.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SetIns::SetIns()  
{
   _name = "set";
   _insType = 0;
   return;
}

SetIns::~SetIns()
{
    delete _pSMLcode;
}

void SetIns::translate() {
	Name parName; Name value; Name temp; Name objName;

	int ierr;

	SMLline line;
	
	SMLline lineBeingTranslated;

//	SMLUnit::out(" ");


    Name token; int idel,jdel; int inext,jnext;
	int ist,jst;
	
	lineBeingTranslated = (*_pSMLcode)[0];
	char del = getNextToken(_pSMLcode,0,0," ",token,idel,jdel,inext,jnext);
	token.upCase(); token.trim();
	if ( token == "SET"){}
	else {
		ErrorWarning::printHead("ERROR",lineBeingTranslated
		,"parsing SET instruction. Keyword SET not found");
		exit(2);
	}
	
    ist = inext; jst = jnext;

//cout << endl << "going to parse Set Instruction" << endl;
//cout << "   ist, jst : " << ist << ", " << jst << endl;
//	_pSMLcode->out();

	line = (*_pSMLcode)[ist];
	lineBeingTranslated = line;

//cout <<  " line " << ist << endl; line.out(jst,jst+50);

	int icurr = ist; int jcurr = jst;
//	(*_pSMLcode)[ist].indicateCharacter(" First after SET",jst); //debug

	ierr = _lOperand.initFromSMLcode(_pSMLcode,icurr,jcurr);
	   
			  
//cout << " returned from _lOperand.initFromSMLcode,  ierr =  " << ierr << endl;
//cout << " icurr jcurr  " << icurr << " " << jcurr << endl;

//cout << endl << " Left Operand:" << endl;
// _lOperand.out(); cout << endl;

	_insType = 10; // all set instructions are going to be no. 10
	
	if ( line[jcurr] != '=' )
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " Expected = and found |" << line[jcurr] << "|" << endl;
		exit(2);
	}


// Now the first operand


	int inbl, jnbl;
	skipSpacesandTabs
            (_pSMLcode, icurr, jcurr+1, inbl,jnbl);
	
	icurr = inbl; jcurr =jnbl;    
	lineBeingTranslated = (*_pSMLcode)[icurr];
//	(*_pSMLcode)[icurr].indicateCharacter(" First character of first operand",jcurr); //debug
	       
	_rightSideCast = checkForRightSideCast(lineBeingTranslated, jcurr, jnext); 
	
	jcurr = jnext;

	ierr = _operand1.initFromSMLcode(_pSMLcode,icurr,jcurr);
	   
			  
//cout << " returned from operand1.initFromSMLcode,  ierr =  " << ierr << endl;


//cout << endl << " Operand1:" << endl;
//_operand1.out(); cout << endl;

//cout << " icurr jcurr  " << icurr << " " << jcurr << endl;
	if (icurr < 0)   // only 1 operand
	{
		if ( _rightSideCast == "" ) {}
		else
		{
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << " right side casting not allowed for simple assignment statement" <<
			endl << " anyway, the right bracket is also missing" << endl;
		 	exit (2);
		}
		_numOperands = 1; _numOperations = 0;
		return;
	}
	
	_numOperands = 2; _numOperations = 1;
// Now get the operator
	int ibeg,jbeg;

	ibeg = icurr; jbeg = jcurr;
	lineBeingTranslated = (*_pSMLcode)[ibeg];
	del = getNextToken(_pSMLcode,ibeg,jbeg," ",_operator,idel,jdel,inext,jnext);
//cout << " after operator : inext, jnext " << inext << " " << jnext << endl;
	if ( _operator == ")" )
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " right side casting not allowed for simple assignment statement" <<
			 endl;
		exit (2);
	
	}

	if ( _operator == "+" || _operator == "-" || _operator == "*"
	  || _operator == "/" || _operator == "%" )
	{}
	else
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " looking for an arithmetic operator and found "
		 << _operator << endl;
		 exit (2);
	}
//cout << " Operator : " << _operator << endl;
	

// Now the second operand
	icurr = inext; jcurr = jnext;
	lineBeingTranslated = (*_pSMLcode)[icurr];
	ierr = _operand2.initFromSMLcode(_pSMLcode,icurr,jcurr);
	   
			  
//cout << " returned from operand2.initFromSMLcode,  ierr =  " << ierr << endl;


//cout << endl << " Operand2:" << endl;
//_operand2.out(); cout << endl;

//cout << " icurr jcurr  " << icurr << " " << jcurr << endl;
	char NBLchr;
	
	if ( icurr >= 0 )
	{    // something is following
		NBLchr = skipSpacesandTabs(_pSMLcode, icurr, jcurr, inbl,jnbl);
		icurr = inbl; jcurr = jnbl;
	}
	
	if ( _rightSideCast == "" )
	{
		if ( NBLchr == '\0' ) { return; } // no right casting and nothing follows
		else
		{
			ErrorWarning::printHead("ERROR",lineBeingTranslated);
			cout << " some crap following SET instruction" << endl;
			cout << " NBLch : |" << NBLchr << "|" << endl;
			exit(2);
		}
	}
	
	// right side is casted, we need the closing bracket
	if ( NBLchr != ')' )
	{
		ErrorWarning::printHead("ERROR",lineBeingTranslated);
		cout << " closing bracket is missing" << endl;
	 	exit(2);
	}
	
	// check nothing follows the closing bracket
	
	icurr =inbl; jcurr = jnbl+1;
//	(*_pSMLcode)[icurr].indicateCharacter(" after closing bracket",jcurr); //debug
	NBLchr = skipSpacesandTabs(_pSMLcode, icurr, jcurr, inbl,jnbl);

	if ( NBLchr == '\0' ) { return; }  // nothing else follows
//	(*_pSMLcode)[inbl].indicateCharacter(" after call to skip",jnbl); //debug
		
	ErrorWarning::printHead("ERROR",lineBeingTranslated);
	cout << " some crap following SET instruction" << endl;
	cout << " NBLch : |" << NBLchr << "|" << endl;
	exit(2);
	
}

//--------------------------------------------------------------------------
void SetIns::out(const Name offset) const
{
	cout << offset.getString() << " SetIns::out   not implemented" << endl;

	return;
}
//------------------------------------------  BF April  2000  -----------
void SetIns::outSobj(int&,int,ofstream& sobj,Registrar&) const
{
	sobj << "set" << endl;

	sobj << "    " << _insType << endl;


	if ( _insType == 10 )
	{
		sobj << "LOP" << endl;
		_lOperand.outSobj(sobj);
		sobj << "    " << _numOperands  << "    " << _numOperations << endl;
 		sobj << "OP1" << endl;
		_operand1.outSobj(sobj);
		if ( _numOperands == 1 ) { return; }
		
		sobj << "OP2" << endl;
		_operand2.outSobj(sobj);
		sobj << _operator.getString() << "   OP1" << "   OP2" << endl;
		
		return;
	}
	return;
}
//---------------------------------------------------------------------------
int SetIns::examine()
{
	int retcode = 0;
	
	SMLline firstLine = (*_pSMLcode)[0];
	
//cout << " Examining " << outString() << endl;
//cout << " Number of operands : " << _numOperands << endl;
//dbg
/*  cout << endl 
  << " ====================== SetIns::examine() ============= " << endl;
	
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
//end dbg

//cout << endl ; printErrorWarningHead("ERROR");
			
//cout << endl; _pParentUnit->printParents(); cout << endl;
//return 0;
//cout << " Left operand " << endl;


	if ( _lOperand.operandType() == "NAME" )
	{	
		_lOperand.thisObjPar();  // enforcing the parameter to be one of the
	                                 // object parameters of the current object
			// this guarantees that getParValueType consideres only object pars
	}
	else if ( _lOperand.operandType() == "COMPNAME" )
	{
		if ( _lOperand.ownerObjName() == "THIS" ) {}
		else
		{
			retcode = 1;
			cout << endl ; ErrorWarning::printHead("ERROR",firstLine);
			cout << " parameter of other object is not allowed to be altered" << endl;
			cout << endl; _pParentUnit->printParents(); cout << endl;
		}
	}
	else
	{
		ErrorWarning::printHead("ERROR",firstLine
		,"internal error. Call expert");

		abort();
	}

//cout << endl << retcode << "  "; _lOperand.outShort();

	if ( retcode != 0 ) { return retcode; }

	int ipom = _lOperand.castingRequested();

	if ( ipom != 0 )
	{

		retcode = 1;
		cout << endl ; ErrorWarning::printHead("ERROR",firstLine);
		cout << " no casting on the left side allowed " << endl;
		return 1;
	}
	
	retcode = OperManager::getParValueAndOperValueTypes( firstLine, _pParentUnit, _lOperand);

	
	if ( retcode != 0 ) return retcode;
	
//cout << " Operand 1 " << endl;

	retcode = OperManager::getParValueAndOperValueTypes( firstLine, _pParentUnit, _operand1);
	
/*	cout << " Operand 1  flg  " << flg << endl;
	_operand1.out();
	
	cout << " operator " << _operator << endl;
*/

	if ( _numOperands == 1 ) 
	{
		if ( retcode != 0 ) { return retcode; }
		
		Name lValType = _lOperand.operValueType();
		Name valType1 = _operand1.operValueType();
		
		if (  lValType == valType1 ) { return 0; }
		
		if ( lValType == "FLOAT" && valType1 == "STRING" )
		{
			cout << endl ; ErrorWarning::printHead("ERROR",firstLine
			," illegal conversion of right side from STRING to FLOAT");
			return 1;
		}
		
		cout << endl ; ErrorWarning::printHead("WARNING",firstLine);
		cout << " right side converted from " << valType1 << " to " << lValType << endl;

		return 0;
	}
	
	
//cout << " Operand 2 " << endl;

	
	retcode = OperManager::getParValueAndOperValueTypes( firstLine, _pParentUnit, _operand2);


/*	cout << " Operand 2  flg  " << flg << endl;
	_operand2.out();
*/	

	if ( retcode != 0 ) { return retcode; }

	
	retcode = OperManager::makeOperandsSameType
	              ( firstLine, _pParentUnit,
		        _operand1, _operand2);
			
	if ( retcode != 0 ) { return retcode; }		
	

	Name lValType = _lOperand.operValueType();
	Name valType1 = _operand1.operValueType();
	
// For strings only + is allowed
	if ( valType1 == "STRING" )
	{
		if ( _operator == "+" ) {}
		else
		{
			cout << endl ; ErrorWarning::printHead("ERROR",firstLine);
			cout << " operator \"" << _operator.getString() 
			<< "\" is not allowed for strings" << endl;
			return 1;
		}
	}
// For Floats % is not allowed	
	if ( valType1 == "FLOAT" )
	{
		if ( _operator == "%" )
		{
			cout << endl ; ErrorWarning::printHead("ERROR",firstLine);
			cout << " operator \"" << _operator.getString() 
			<< "\" is not allowed for Floats" << endl;
			return 1;
		}
		
	}
	
	if (_rightSideCast == "") {}
	else
	{
		if (_rightSideCast == lValType) {}
		else
		{
			cout << endl ; ErrorWarning::printHead("ERROR",firstLine);
			cout << " Sort out your casting" <<  endl;
			return 1;
		
		}
	}	
	
	if (  lValType == valType1 ) { return 0; }
		
	if ( lValType == "FLOAT" && valType1 == "STRING" )
	{
		cout << endl ; ErrorWarning::printHead("ERROR",firstLine
		," illegal conversion of right side from STRING to FLOAT");
		return 1;
	}
	
	if ( _rightSideCast == lValType ) 
	{
	//   The user is aware of the conversion
		return 0;
	}
		
	cout << endl ; ErrorWarning::printHead("WARNING",firstLine);
	cout << " right side converted from " << valType1 << " to " << lValType << endl;

	return 0;
}
//-------------------------------------- SetIns::simpleOut() const -----------------
Name SetIns::outString() 
{
	Name temp;
	
	temp = "SET ";
	
	temp += _lOperand.printingName();
	temp += " = ";
	temp += _operand1.printingName();
	
	if (_numOperands == 2)
	{
		temp += " ";
		temp += _operator;
		temp += " ";
		temp += _operand2.printingName();
	}

	return temp;
}
//-----------------------------------------------------------------------------
Name SetIns::checkForRightSideCast(const SMLline& line, const int& jst, int& jnext)
{

// assuming that all the spaces were skipped
//	line.indicateCharacter(" First character for SetIns::checkForRightSideCast",jnext); //debug

	char* pointerToLineBody = line.getString();
	char* pointerTojstIn =&pointerToLineBody[jst];
	
	Name temp = pointerTojstIn;
	temp.upCase();
	char* pointerTojst = temp.getString();
	
	Name tobeCastedTo = ""; jnext = jst;
     
	if (strncmp("(S)(",pointerTojst,4) == 0) {tobeCastedTo = "STRING"; jnext = jst+4;}; 
	if (strncmp("(STRING)(",pointerTojst,9) == 0) {tobeCastedTo = "STRING"; jnext = jst+9;} 

	if (strncmp("(I)(",pointerTojst,4) == 0) {tobeCastedTo = "INT"; jnext = jst+4;} 
	if (strncmp("(INT)(",pointerTojst,6) == 0) {tobeCastedTo = "INT"; jnext = jst+6;}
 
	if (strncmp("(F)(",pointerTojst,4) == 0) {tobeCastedTo = "FLOAT"; jnext = jst+4;} 
	if (strncmp("(FLOAT)(",pointerTojst,8) == 0) {tobeCastedTo = "FLOAT"; jnext = jst+8;}

	return tobeCastedTo; 
}
//-------------------------------------------------------------------------------
char SetIns::skipSpacesandTabs
            (SMLlineVector* pSMLcode, int ist, int jst, int& inbl, int& jnbl) 
{
	char firstNBLchr = '\0'; inbl = -1; jnbl = -1; 
	int numOfLines = pSMLcode->length();   //num of lines in the code
	if ( ist < 0 || ist >= numOfLines )
	{
		cout << " **** skipSpecesandTabs ... pointing outside code" <<
		endl;
		return firstNBLchr;
	}
	
	SMLline line;
	char* pLine;  // pointing to the string
	int lineLen; // length of line
	char* ptnStart;  // pointing to the start of the search on the line
	char* ptnNBL; 
	
	for (int i=ist; i<numOfLines; i++)
	{
		line = (*pSMLcode)[i]; pLine = line.getString(); lineLen = strlen(pLine);
		if ( i==ist )
		{
			if ( jst >= lineLen ) { continue;}
			ptnStart = &pLine[jst];
		} // start search from 'jst'
		else
	        { 
			ptnStart = pLine;
		}
		ptnNBL = firstNonBlankPtn(ptnStart);
		if (*ptnNBL == '\0' || *ptnNBL == '#' || *ptnNBL == '!') { continue;}
		// found genuine non blank character
		firstNBLchr = *ptnNBL; inbl = i; jnbl = ptnNBL-pLine;
		return firstNBLchr;
	}
	return firstNBLchr;
}
