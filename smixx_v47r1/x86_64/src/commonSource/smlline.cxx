//----------------------  Class   SMLline  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 15 December 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "smlline.hxx"
#include "utilities.hxx"

  SMLline::SMLline() {
     _lineNum = 0;
  }

  SMLline::~SMLline() {
  }

  SMLline::SMLline(const int linnum, const Name& name) : Name(name) {
     _lineNum = linnum;
  }

  SMLline::SMLline(const int linnum, const char* lineBody) : Name(lineBody) {
     _lineNum = linnum;
  }

  SMLline& SMLline::operator=(const SMLline& line) {
    Name::operator= (line);
    _lineNum = line._lineNum;
	return *this;
  }

//================================ commentLine ===========================
  bool SMLline::commentLine() {
// Line, which is empty, or full of blanks or tabs, or the first non-blank character
// is !  or #

	int len = strlen(_ptn);
	if (len == 0) {return 1;}
	
	int i ; char ch;
	for ( i=0; i<len; i++) {
		ch = *(_ptn+i);
		if ( ch == ' ' || ch == '\t' ) { continue; }
		else if ( ch == '#' || ch == '!' ) { return 1;}
		else { return 0;}
	}
	
	return 1;
}

//------------------------------------------------------------------------
bool SMLline::instructionLine( Name& type) {
	  
	  
	  
	Name token; char* ptn;
	  
	ptn = getNextToken(_ptn," (/!#",token); token.upCase(); token.trim();
	  
	if ( token == "DO" )               { type = "do"; return 1;}
	if ( token == "WAIT" )             { type = "wait"; return 1;}
	if ( token == "CREATE_OBJECT" )    { type = "create_object"; return 1;}
	if ( token == "SLEEP")             { type = "sleep"; return 1;}	
	if ( token == "INSERT")            { type = "insert"; return 1;}
	if ( token == "REMOVE" )           { type = "remove"; return 1;}
	if ( token == "REMOVEALL" || token == "REMOVE_ALL")        { type = "remove"; return 1;}
	if ( token == "SET" )              { type = "set"; return 1;}
	if ( token == "IF"  )              { type = "if"; return 1;}
	if ( token == "ELSEIF" )           { type = "elseif"; return 1;}
	if ( token == "ELSE" )             { 
		ptn = getNextToken(ptn," !#",token); token.upCase();
		if ( token == "IF" )            { type = "elseif"; return 1;}		
		else                            { type = "else"; return 1;}
	}
	if ( token == "TERMINATE_ACTION" ) { type = "terminate_action"; return 1;}
	if ( token == "ENDINSTATE" || token == "MOVE_TO" ) { type = "terminate_action"; return 1;}
	if ( token == "ENDIF" )            { type = "endif"; return 1;}
	if ( token == "END" )  
	{
		ptn = getNextToken(ptn," !#",token); token.upCase();
		if ( token == "IF" )            { type = "endif"; return 1;}
	}
	  
	return 0;
}

//------------------------------------------  BF April 2000  ------------
int SMLline::primKeyword( Name& name, int& assoc)
{

	Name keyword,attrib,isof;
	char* ptn = _ptn;
	char* pDel; char del; char* pDel1;


	ptn = firstNonBlankPtn(ptn);
	if (*ptn == '\0' || *ptn == '#' || *ptn == '!') {return -1;}

	if (!strstr(ptn,":")) {return 0;}


	pDel = getNextToken(ptn,":!#", keyword); 
	keyword.upCase(); keyword.trim();

	del = *pDel;

	int assFound = 0;

	if ( del == ':' ) {
		if ( keyword == "CLASS" || keyword == "OBJECT" || 
			keyword == "OBJECTSET") {}
		else { return 0;}
		pDel = getNextToken(pDel+1," /!#", name);
		name.trim(); name.upCase();
		if(!check_name(name)) {
			cout << " ***Error***  " << name << " is not a name" << endl;
			out();
			exit(2);
		}

//cout << endl << " start=============== primKeyword ========================" << endl;
//cout << " keyword : " << keyword << " name : " << name
//<< " delim |" << *pDel << "|" << endl;
//cout << " following string :" ;
//if ( *pDel == '\0' ) { cout << "NULL" << endl;}
//else { cout << "|" << pDel+1 << "|" << endl;}

		if ( *pDel == '/') {
			pDel1 = getNextToken(pDel+1,"/!#",attrib);
			attrib.trim(); attrib.upCase();
			if (attrib == "ASSOCIATED") {assFound = 1;}
		}

		if ( keyword == "CLASS" ) {
			assoc = assFound;
			return 1;
		}
		else if ( keyword == "OBJECT" ) {
			if (*pDel == ' ') {
				pDel1 = getNextToken(pDel+1," ",isof);
				isof.trim(); isof.upCase();
				if ( isof == "IS_OF_CLASS" ) {
					return 3;
				}
				else {
					cout << " ***Error***  expected IS_OF_CLASS keyword not found" << endl;
					out();
					exit(2);
				}
			}
			else {
				assoc = assFound;
				return 2;
			}
		}
		else if ( keyword == "OBJECTSET" ) {
			if (*pDel == ' ') {
				pDel1 = getNextToken(pDel+1," ",isof);
				isof.trim(); isof.upCase();
				if ( isof == "UNION" ) {
					return 5;
				}
			}
			return 4;
		}
		else {
		}
	}

	if ( keyword == "\0" ) {return -1;}

	return 0;

}
//----------------------------------------  BF May 2000  --------------
int SMLline::objectKeyword(Name& name)
{
	Name keyword;
	char* ptn = _ptn;
	char* pDel; char del; 

	ptn = firstNonBlankPtn(ptn);
	if (*ptn == '\0' || *ptn == '#' || *ptn == '!') {return -1;}

	if (!strstr(ptn,":")) {return 0;}

	pDel = getNextToken(ptn,":!#", keyword); 
	keyword.upCase(); keyword.trim();
	

	del = *pDel;

	if ( del == ':' ) {
		if      ( keyword == "PARAMETERS" ) {return 10;}
		else if ( keyword == "SUBOBJECT" ) {
			pDel = getNextToken(pDel+1," !#", name);
			name.trim(); name.upCase();
			if(!check_name(name)) {
				cout << " ***Error***  " << name << " is not a name" << endl;
				out();
				exit(2);
			}
			return 20;
		}
		else if ( keyword == "STATE") {
			pDel = getNextToken(pDel+1," /!#", name);
			name.trim(); name.upCase();
			if(!check_name(name)) {
				cout << " ***Error***  " << name << " is not a name" << endl;
				out();
				exit(2);
			}
			return 30;
		}
		else { return 0;}
	}
	else {
		return 0;
	}
}
//----------------------------------------  BF June 2000  --------------
int SMLline::stateKeyword(Name& name)
{
	Name keyword;
	char* ptn = _ptn;
	char* pDel; char del; 

	ptn = firstNonBlankPtn(ptn);
	if (*ptn == '\0' || *ptn == '#' || *ptn == '!') {return -1;}

	if (!strstr(ptn,":") && !strstr(ptn,"when")) {return 0;}

	pDel = getNextToken(ptn,": (!#", keyword); 
	keyword.upCase(); keyword.trim();
	

	del = *pDel;

	if ( del == ':' ) {
		if ( keyword == "ACTION" ) {
			pDel = getNextToken(pDel+1," (/!#", name);
			name.trim(); name.upCase();
			if(!check_name(name)) {
				cout << " ***Error***  " << name << " is not a name" << endl;
				out();
				exit(2);
			}
			return 200;
		}
		else { return 0;}
	}
	else if ( keyword == "WHEN" ) { return 100;}
	else {
		return 0;
	}
}
//---------------------------------------------------------------------
  void SMLline::out() const {
     cout << _lineNum << ":" << "<" << _ptn << ">" << endl;
  }
//---------------------------------------------------------------------
  void SMLline::out(int jst, int jend) const
  {
  	char *ptn;
	
     cout << " characters " << jst << "-" << jend << ": <" ;

	for ( int i=jst; i<=jend; i++)
	{
		ptn = _ptn + i;
		if ( *ptn == '\0' ) break;
		cout << *ptn ;
	}
	
	cout << ">" << endl;
  }
//---------------------------------------------------------------------
void SMLline::indicateCharacter(char* msg, int j) const
{
	char TAB = '\t';
	
	cout << msg << " line :" << _lineNum << " character :" << j << endl;
	cout << _ptn << endl;

	int stringLength = strlen(_ptn);	
	if ( j >= stringLength )
	{
		cout << "   ======= character " << j << " is out of bounds" << endl;
		return;
	}
	
	for ( int i = 0; i<j; i++ )
	{
		if ( *(_ptn+i) == TAB ) { cout << TAB ;} 
		else { cout << " "; }
	}
	
	cout << "^" << endl;
	
	 
	return;
}

//---------------------------------------------------------------------
  ostream& operator<<(ostream& os, const SMLline& line) {
     os << line._lineNum << ": " << line._ptn ;
     return os;
  }
//---------------------------------------------------------------------
int SMLline::linNum() const
{
	return _lineNum;
}
//---------------------------------------------------------------------
Name SMLline::lineBody() const
{
	Name temp;
	temp = _ptn;
	
	return temp;
}
