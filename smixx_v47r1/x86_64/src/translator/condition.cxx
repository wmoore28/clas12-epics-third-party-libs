// condition.cxx: implementation of the Condition class.
//
//                                                B. Franek
//                                               8 October 1999
//////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"
#include "string.h"
#include "smlline.hxx"
#include "smllinevector.hxx"
#include "utilities.hxx"
#include "smpcond.hxx"
#include "smpcondtyp1.hxx"
#include "smpcondtyp2.hxx"
#include "smpcondtyp3.hxx"
#include "smpcondtyp4.hxx"
#include "boolitem.hxx"
#include "booloperation.hxx"
#include "stack.hxx"
#include "stackitem.hxx"

#include "condition.hxx"
#include "errorwarning.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Condition::Condition()
: _sobjHeader("not defined"), _terminator(""),
  _smpConditions(), _numBoolOperations(0), _pExecutor(NULL), _pParentUnit(NULL)
{
	return;
}
//-------------------------------------------------------------------------------
int Condition::initialise(SMLlineVector* pSMLcode,
					 const int ist, const int jst,  
					 int& inext, int& jnext)
{

	Stack stack(1000);
	
	SMLline line; char* pLine;


	char* ptn;

	Name tempNm = " "; Name temp;

//	cout << endl << "going to parse contition" << endl;

// "("  ")"  "not" "and" "or"  "name"  "do"  "then" "endinstate" "move_to" "stay_in_state"
	SmpCond* pSmpCond;
	char item,item1;
	item1 = ' ';

	int istt = ist;
	int jstt = jst;
	int iprint = 1;

	for (; ; ) {
		line = (*pSMLcode)[istt];
		_lineBeingTranslated = line;
//		if (iprint != 0) {cout << line << endl;}
		pLine = line.getString();
		ptn = &pLine[jstt];
//		cout << " Next search in : |" << ptn << "|" << endl;

		int flag = getNextItem(pSMLcode,istt,jstt,inext,jnext,item,pSmpCond);

		if (flag==2) {break;}
//		cout << endl << " ----> Item : " << item << endl;

		if (item == '(' || item == '-' || item == 'T')
		{
			if (item == 'T') {
				stack.push(pSmpCond);
			}
			else {
				stack.push(item);
			}
		}
		else if ( item == ')' || item == '.' || item == '+' || item == 'E')
		{
			//   ) . + or E
			int flag1 = handleStack(stack,item);

			if (flag1 == 2)
			{
				return 1;
			}
		}
		else
		{
			ErrorWarning::printHead("ERROR",_lineBeingTranslated);
			cout << "Unexpected item found :" << item << endl;
			exit(2);
		}
		if (istt == inext ) {iprint = 0;}
		else {iprint = 1;}
		jstt = jnext; istt = inext;
	}
	return 1;
}
//-------------------------------------------------------------------------------
int Condition::handleStack(Stack& stack,const char item)
{

//  This is called when item is either ) . + or E
// item1 is either X or T


	Name offset = " "; Name tempNm = " ";
//    cout << endl << "handleStack" << endl;
	char item1,item2;
	BoolItem* point1, *point3;
	StackItem stackItem1,stackItem2,stackItem3;

		for (;;) {
//			stack.out(offset);
			stack.getFromStack(1,stackItem1);
			item1=stackItem1.item();
			point1=stackItem1.pointer();

			if (item1 != 'X' && item1 != 'T')
			{
				ErrorWarning::printHead("ERROR",_lineBeingTranslated);
				cout << "HandleStack:   item1 should be T or X. It is " << item1 << endl;
				cout << "    Contact expert" << endl;
				exit(2);
			}

			if (!stack.getFromStack(2,stackItem2)) {
//				cout << " item2  not there" << endl;
				if ( item == '.' || item == '+' ) {
					stack.push(item);
//					stack.out(offset);cout << "handleStack...end" << endl;
					return 1;
				}
				else if ( item == 'E' ) { return 2;}
				else
				{
					ErrorWarning::printHead("ERROR",_lineBeingTranslated);
					cout << "handleStack:" 
					<< "Expecting and  or do/then/endinstate/move_to/stay_in_state and found : "
					     << item << endl;
					exit(2);
				}
				return 2;
			}
					
			item2 = stackItem2.item();
//			cout << " item2 " << item2 << endl;

			if (  (item == '.' && (item2 == '(' || item2 == '+')) ||
				(item == '+' && item2 == '(') ) {
				stack.push(item);
//				stack.out(offset);cout << "handleStack...end" << endl;
				return 1;
			}

			if ( item2 == '(' ) {
				if ( item != ')' )
				{
					ErrorWarning::printHead("ERROR",_lineBeingTranslated);
					cout << "handleStack:  expecting ) and found " << item << endl;
					exit(2);
				}
				stack.deleteItem2FromStack();
//				stack.out(offset);
//				cout << "handleStack...end" << endl;
				return 1;
			}

			if ( item2 == '-' ) {
				stack.deleteTopStack(2);
				if (item1 == 'T') {
					SmpCond* pSmpCond = (SmpCond*)point1;
					pSmpCond->negate();
					stack.push(point1);
				}
				else { 
					_numBoolOperations++;
					BoolOperation* pBoolOp = 
						new BoolOperation(_numBoolOperations,'-',point1,0);
//					cout << "New operation "; Name tmp = " "; pBoolOp->out(tmp); 
					stack.push(pBoolOp);
					_pExecutor = pBoolOp;
				}
				continue;
			}

			if (item=='.' || item=='+' || item=='E' || item==')') {
				if (!stack.getFromStack(3,stackItem3))
				{
					ErrorWarning::printHead("ERROR",_lineBeingTranslated);
					cout << "handleStack:  Item3 is not there...call expert" << endl;
					exit(2);
				}
				point3 =stackItem3.pointer();
				stack.deleteTopStack(3);
				_numBoolOperations++;
				BoolOperation* pBoolOp = 
					new BoolOperation(_numBoolOperations,item2,point3,point1);
//				cout << "New operation "; Name tmp = " "; pBoolOp->out(tmp);
				assert (!pBoolOp == 0);
				stack.push(pBoolOp);
				_pExecutor = pBoolOp;
				continue;
			}

			ErrorWarning::printHead("ERROR",_lineBeingTranslated);
			cout <<  "handleStack: Brackets probably do not balance" << endl;
			exit(2);
		}

}
//----------------------------------------------------------------------------
int Condition::getNextItem(SMLlineVector* pSMLcode,const int ist,const int jst,
					   int& inext, int& jnext, char& item, SmpCond*& pSmpCond)
{
//	(*pSMLcode)[ist].indicateCharacter(" First character getNextItem",jst); //debug

	pSmpCond = 0;

	int inbl,jnbl,iprev,jprev; 
	item = firstNonBlank(pSMLcode,ist,jst,inbl,jnbl,inext,jnext,iprev,jprev);
	if (item == '\0' || item == '#' || item == '!' ) {
		inext = -1; jnext = 0;
		return 2;
	}

	Name temp; int idel,jdel;
	item = getNextToken(pSMLcode,inbl,jnbl," ()",temp,idel,jdel,inext,jnext);
	

	if ((item == '\0' || item == '#' || item == '!' )&&
		temp=="\0") {
		return 2;
	}

	if ( item == ')' && temp=="\0") {
		return 1;
	}
	
	temp.upCase(); temp.trim();
	if (temp == "NOT" ) {
		item='-'; inext = idel; jnext = jdel;
		return 1;
	}
	if (temp == "AND" ) {
		item='.'; inext = idel; jnext = jdel;
		return 1;
	}
	if (temp == "OR" ) {
		item='+'; inext = idel; jnext = jdel;
		return 1;
	}
	if (temp == "DO" || temp == "THEN" || temp == "ENDINSTATE" 
	      || temp == "MOVE_TO" || temp == "STAY_IN_STATE" ) {
		item ='E'; inext = idel; jnext = jdel;
		_terminator = temp;
		return 1;
	}
	if ( item == '(' && temp=="\0") {
	// in this case it could be beginining of cast indicator
		bool castInd = castIndicator(pSMLcode,idel,jdel);
//		cout << " cast indicator : " << castInd << endl ;  //debug
		if ( !castInd ) {
			return 1; //not class indicator
		} 
		// this is beg of class indicator, will be processed as SmpCond bellow
	}	
	
//		(*pSMLcode)[inbl].indicateCharacter(" Begining of Simple Condition",jnbl); //debug
//		cout << " found simple cond maybe?  temp: " << temp << endl;
		Name tempNm = " "; int idNum = 	_smpConditions.length()+1;
                int typ = smpCondTyp(pSMLcode,inbl,jnbl);

		if (typ == 1 ) {
			SmpCondTyp1* pSmpCondTyp1 = new SmpCondTyp1(idNum);
			pSmpCond = pSmpCondTyp1;
		}

		if (typ == 2) {
			SmpCondTyp2* pSmpCondTyp2 = new SmpCondTyp2(idNum);
			pSmpCond = pSmpCondTyp2;
		}

		if (typ == 3) {
			SmpCondTyp3* pSmpCondTyp3 = new SmpCondTyp3(idNum);
			pSmpCond = pSmpCondTyp3;
		}

		if (typ == 4) {
			SmpCondTyp4* pSmpCondTyp4 = new SmpCondTyp4(idNum);
			pSmpCond = pSmpCondTyp4;
		}
		
		_smpConditions.addRegardless(tempNm,pSmpCond);
		pSmpCond->initialise(pSMLcode,inbl,jnbl,inext,jnext);
		pSmpCond->setParentUnit(_pParentUnit);
		item = 'T';
		return 1;
	
}
//------------------------------------------------------------------------------

bool Condition::castIndicator
               (const SMLlineVector* pSMLcode, const int ist, const int jst) const
{
// cout << endl << endl << " starting castIndicator " << endl//debug;

	SMLline line = (*pSMLcode)[ist];
       char* pjstIn = &line[jst];   // pointer to the jst char

 	Name temp = pjstIn;
	temp.upCase();  //convert it to uppercase
	char* pjst = temp.getString();
      
       
//	line.indicateCharacter(" First character for possible cast Condition::castIndicator",jst); //debug
//	cout  << " pjst :" << pjst << endl;  //debug

	if ( strncmp(pjst,"(S)",3) == 0) {return true;}
	if ( strncmp(pjst,"(I)",3) == 0) {return true;}
	if ( strncmp(pjst,"(F)",3) == 0) {return true;}

	if ( strncmp(pjst,"(STRING)",8) == 0 ) {return true;}
	if ( strncmp(pjst,"(INT)",5) == 0) {return true;}
	if ( strncmp(pjst,"(FLOAT)",7) == 0) {return true;}
// cout << " Output is false " << endl; //debug
	return false;
}
//-----------------------------------------------------------------------------


Condition::~Condition()
{
	return;
}

void Condition::translate()
{
	return;
}

void Condition::out(const Name offset) const
{

	Name offset1 = offset;
	offset1 += "  ";
	void* ptnvoid; SmpCond* pSmpCond;
	char* ptn = offset.getString();
	cout << ptn << " Condition " << endl;

	int noOfSmpCond = _smpConditions.length();
	cout << ptn << " No of simple conditions " << noOfSmpCond << endl;

	for ( int iu=0; iu<noOfSmpCond; iu++ ) {  
		ptnvoid = _smpConditions.gimePointer(iu);
		pSmpCond = (SmpCond*)ptnvoid;
	  
		pSmpCond->out(offset1);
	}

	cout << ptn <<endl;
	cout << ptn << " No of boolean operations " << _numBoolOperations << endl;
	
	if (_numBoolOperations > 0) {_pExecutor->outFull(offset1);}

	return;
}
//-----------------------------------------  BF April 2000  -----------
void Condition::outSobj(ofstream& sobj) const
{
	sobj << _sobjHeader.getString() << endl;

	int nsmp = _smpConditions.length();

	char line[80];
	sprintf(line,"%5d%5d",nsmp,_numBoolOperations);
	sobj << line << endl;

//	sobj << "    " << nsmp << "    " << _numBoolOperations << endl;

	for ( int is=0; is<nsmp; is++) {
		void* ptnvoid = _smpConditions.gimePointer(is) ;
		SmpCond* pSmpCond;
		pSmpCond = (SmpCond*)ptnvoid;
		pSmpCond->outSobj(sobj);
	}

	if (_numBoolOperations>0) {
		_pExecutor->outSobj(sobj);
	}

	return;
}
//---------------------------------------------  BF  April 2000  -------
void Condition::getHeader(SMLlineVector* pSMLcode,const int ist,const int jstin,
					   int iend, int jend)
{
	Name temp = "\0";
	SMLline line;
	char* pLine;
	//	Name condString = " ";
	Name condString = "\0";
	char save;
	char* ptn; 	int aloneDO;

	int j;
	int nend;

	if (iend >= 0) {
		nend = iend;
	}
	else {
		nend = pSMLcode->length()-1;
	}

	int jst = jstin;
	line = (*pSMLcode)[ist];
	pLine = line.getString();
	if (pLine[jst-1] == ' ') {jst = jstin-1;}


	for (int i=ist; i<=nend; i++) {
		line = (*pSMLcode)[i];
		pLine = line.getString();
		if (i==ist) {j=jst;}
		else {j=0;}

		ptn = &pLine[j];

		if (i==iend) {
			save = pLine[jend]; pLine[jend] = '\0';
		}

		process_line_forTrans(ptn);
		condString += ptn;
		if ( i==iend) {
			aloneDO = 0;
			if (strcmp(ptn," DO") == 0 || strcmp(ptn," ENDINSTATE") == 0 
			 || strcmp(ptn," MOVE_TO") == 0 || strcmp(ptn," STAY_IN_STATE") == 0 ) {aloneDO =1;}
//			cout << "dbg|" << ptn << "|" << endl;
//			cout << aloneDO << endl;

		}
	}


	char* str = condString.getString();
	strTrim(str);
	int ln = strlen(str);
	
//	cout << "|" << str << "|" << endl;

// At the end we have "DO" or "THEN" or "ENDINSTATE" or "MOVE_TO" or "STAY_IN_STATE"

	if ( strcmp(&str[ln-2],"DO") == 0 ) {
		ln=ln-2;
		str[ln] = '\0';
		if (aloneDO == 0) {
			if (str[ln-1]==' ') {ln=ln-1;str[ln]='\0';}
		}
	}
	if ( strcmp(&str[ln-4],"THEN") == 0) {str[ln-4] = '\0'; ln=ln-4;}
	if ( strcmp(&str[ln-10],"ENDINSTATE") == 0 ) {str[ln-10] = '\0'; ln=ln-10;}
	if ( strcmp(&str[ln-7],"MOVE_TO") == 0 ) {str[ln-7] = '\0'; ln=ln-7;}
	if ( strcmp(&str[ln-13],"STAY_IN_STATE") == 0) {str[ln-13] = '\0'; ln=ln-13;}

    ln = strlen(str);

	if (ln <= 79) { 
		_sobjHeader = str; _sobjHeader.trim(); return;
	}



	save = str[37]; str[37]='\0';
	_sobjHeader = str; str[37]=save;

	_sobjHeader += "......";

	_sobjHeader += &str[ln-36]; _sobjHeader.trim();
	return;
}
//============================================================================
int Condition::smpCondTyp(SMLlineVector* pSMLcode,const int ist,const int jst) {

	Name temp;	
	int idel,jdel;
	int inext,jnext;

	inext = ist;
	jnext = jst;
	
	char del = getNextToken(pSMLcode,inext,jnext," ",temp,idel,jdel,inext,jnext);

	temp.upCase(); temp.trim(); 
//	cout << " First token : " << temp << endl;
	
	if (temp == "ANY_IN" || temp == "ALL_IN") {
		return 2;
	}

	if (!check_name(temp)) { return 4; }
	
	del = getNextToken(pSMLcode,inext,jnext," ",temp,idel,jdel,inext,jnext);

	temp.upCase(); temp.trim(); 
//	cout << "  Second token : " << temp << endl;
	if (temp == "IN_STATE" || temp == "NOT_IN_STATE")
	{	return 1; }
	else if (temp == "EMPTY" || temp == "NOT_EMPTY" )
	{	return 3; }
	else if (temp == "==" || temp == "<>" || temp == "<" || temp == ">" ||
	         temp == ">=" || temp == "<=" )
	{	return 4; }
	else
	{
		ErrorWarning::printHead("ERROR",_lineBeingTranslated);
		cout << " smpCondTyp: parsing simple condition. " << endl;
		exit(2);
	}
}
//=============================================================================
Name Condition::terminator() const {
        return _terminator;
}
//=========================================================================
void Condition::setParentUnit(SMLUnit* parent)
{
	_pParentUnit = parent;
	return;
}
//=========================================================================
int Condition::examine()
{
  //cout << " Condition::examine() " << endl;

	int nsmp = _smpConditions.length();

	void* ptnvoid;
	SmpCond* pSmpCond;
	int iflg(0), retflg(0); 


	for ( int is=0; is<nsmp; is++) {
		ptnvoid = _smpConditions.gimePointer(is) ;
		pSmpCond = (SmpCond*)ptnvoid;		
		iflg = pSmpCond->examine();
		if (iflg != 0) { retflg = 1; }
	}

	return retflg;
}
