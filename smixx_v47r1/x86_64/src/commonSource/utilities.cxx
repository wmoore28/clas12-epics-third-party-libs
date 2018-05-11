//----------------------------------------------------------------------------
//
//                                                    B. Franek  January 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "smixx_common.hxx"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "name.hxx"
#include "utilities.hxx"

#include "smlline.hxx"
#include "smllinevector.hxx"
extern "C" {
#include "smixx_parstring_util.h"
}

void process_line_forTrans(char str[])
{
//  This is only called from method 'getHeader' of 'Condition' class
//   which is used to generate the comment line for conditions. 
// !!!!! it will not properly process lines containing double quotes within double quotes.
// !!!!! As condition strings do not contain anything like that it is OK, but should
// !!!!!  be reviewed if used for something else

	int mode =1; int nout = 0;
	int nl_line =strlen(str);
	if (nl_line <= 0) {return;}

	for (int i=0; i<nl_line; i++) {
		if (mode == 1) {
			if (str[i] == '!' || str[i] == '#') {str[nout]='\0';return;}
			if (str[i] == '\t') {str[i] = ' ';}

			if (str[i] == ' ') {
				if (i == (nl_line-1)) {str[nout] = '\0';return;}
				mode = 2;
			}
			if (str[i] == '"' ) {mode = 3;} 
			
			str[nout] = toupper(str[i]);
			nout++;

		}
		else if ( mode == 2) {
			if ( str[i] == '!' || str[i] == '#') {str[nout-1] = '\0';return;}
			if (str[i] == '\t') {str[i] = ' ';}
			
			if ( str[i] == ' ' ) {
				if ( i==(nl_line-1)) { str[nout-1] = '\0'; return;}
				else { continue;}
			}
			if (str[i] == '"') { mode=3;}
				else {mode=1;}
			
			str[nout] = toupper(str[i]);
			nout++;
				
		}
		else if ( mode == 3) {
			if (str[i] == '"') {mode=1;}
					
			str[nout] = str[i];
			nout++;
		}
		else {
		}
	}
	str[nout] = '\0'; 
	return;
}

void strNtoONEBlank(char str[]){

//	cout << "in" << endl;
//	cout << "|" << str << "|" << endl;
	int nout(0);

	int mode = 1;
	int nl_line = strlen(str);
	if (nl_line <= 0) {return;}

	for (int i=0; i<nl_line; i++) {
		if (str[i] == '\t') { str[i] = ' ';}
		if (mode == 1) {
			nout++;
			str[nout-1] = str[i];
			if (str[i]==' ') {
				if (i == (nl_line-1)) {
					nout = nout-1;
					break;
				}
				mode = 2;
			}
		}
		else if (mode==2) {
			if (str[i]==' ') {
				if (i == (nl_line-1)) {
					nout = nout-1;
					break;
				}
			}
			else {
				nout++;
				str[nout-1] = str[i];
				mode = 1;
			}
		}
		else {}
	}

	str[nout] = '\0';
//	cout << "out" << endl;
//		cout << "|" << str << "|" << endl;
	return;


/*
		if (str[i] == ' ') {
			if (first) {first=0;str[iout] = str[i];iout++;continue;}
			else {continue;}
		}
		else {
			first=1;
			str[iout] = str[i]; iout++;
		}
	}
	str[iout] = '\0';
	return;
  */
}

   void strSqueeze(char str[]){
      int iout(0);

      for (int i=0;;i++) {
         if (str[i] == '\0') { break;}
         if ( (str[i] != ' ') && (str[i] != '\t') ) {
             str[iout] = str[i];
             iout++;
         }         
      }
      str[iout] = '\0';
   }
//-----------------------------------------------------------------------

   void strTrim(char str[]){

      int ln = strlen(str);

      if ( ln <= 0 ) { return; }

      for ( int i = (ln-1); i>=0 ; i--) {
         if ( (str[i] != ' ') && (str[i] != '\t') ) { str[i+1] = '\0'; return; } 
      }
      str[0] = '\0';
      return;
   }

//--------------------------------------------------------------------------

   void strUpcase(char str[]){

      int ln = strlen(str);

      if ( ln <= 0 ) { return; }

      for ( int i=0; i<ln; i++) {
          str[i] = toupper(str[i]);
      }

      return;
   }

//---------------------------------------------------------------------------

   void process_line(char str[]){

      char TAB = '\t';
      int ln = strlen(str);

      if ( ln <= 0 ) { return; }

      int iout = 0;
      int mode = 0;  //0..beginning; 1..normal; 2..after blank; 3...after "

      for ( int i=0; i<ln; i++) {
          if ( str[i]==TAB ) {str[i] = ' ';}
          if (mode==0) {
             if ( str[i] == ' ' ) { continue;} // ignore blanks at the begining
             mode = 1;
             i--; // consider the character again
          }
          else if (mode==1) {
             if ( str[i] == '!' ) { str[iout] = '\0'; return; }
             if ( str[i] == ' ' ) {    // First blank found
                if ( str[iout-1] == ':' || 
                     str[iout-1] == '(' ||
                     str[iout-1] == ')' ||
                     str[iout-1] == '/' ||
                     str[iout-1] == '='   ) { continue; }
                mode = 2;
             }    
             if ( str[i] == '"' ) {  // First " found
                mode = 3;
             }
             str[iout] = toupper(str[i]);
             iout++;
          }
          else if (mode==2) {
             if ( str[i] == '!' ) { str[iout-1] = '\0'; return; }
             if ( str[i] == ' ' ) { continue; }
             if ( str[i] == '"' ) { mode = 3;}
             else { mode = 1;}
             if ( str[i] == ':' ||
                  str[i] == '(' ||
                  str[i] == ')' ||
                  str[i] == '/' ||
                  str[i] == '='   ) { iout--; }
             str[iout] = toupper(str[i]);
             iout++;
          }
          else if (mode==3) {
             str[iout] = toupper(str[i]);
             iout++;
             if ( str[i] == '"' ) { mode = 1; }
          }
          else {
             cout << " Internal error, illegal mode = " << mode << endl;
             abort();
          }
      }   

      if ( mode == 0 ) { str[0] = '\0'; }
      if ( mode == 1 ) { str[iout] = '\0'; return; }
      if ( mode == 2 ) { str[iout-1] = '\0'; }
      if ( mode == 3 ) {
         cout << "  Illegal number of double quotes on the line" << endl;
         cout.flush();
         abort();
      }
   }
//-------------------------------------------------------------------------------
   int readLine(ifstream& file, Name& line) {
       enum { bufLen = 1024 };
       char buf[bufLen];
 
       line ="\0";

       for (;;) {
           file.getline(buf,bufLen);
          
           if (file.eof() ) {
              file.close();
              return 2;
           }
           else if (file.bad()) {
              return 0;
           }
           line += buf;
           if (strlen(buf) < (bufLen-1)) {break;}
		   cout << " ***Error***  on some platforms this does not work"
			   << endl;
		   cout << " to avoid the error, have input lines shorter than "
			   << bufLen << " characters " << endl;
		   exit(0);
       }
       return 1;
   }
//-----------------------------------------  BF  April 2000  ---------
int specialCode(int nl, Name& line)
 {
	enum {numSpecial = 17};
	char alloc [numSpecial][80] = { 
		      "Class : &ALLOCATION_OBJECT / associated",
              "State : NOT_ALLOCATED",
              "    action : ALLOCATE",
              "State : &variable",
              "    action : RELEASE",
              "State : NOT_THERE / dead_state",


              "Object : &ALLOC",
              "State : NOT_ALLOCATED / initial_state",
              "    action : ALLOCATE(Id)",
              "           terminate_action / state = &variable",
              "State : &variable",
              "    action : RELEASE",
              "           terminate_action / state = NOT_ALLOCATED",
	      
	      
	      "Object : &DOMAIN",
	      "State : READY",
	      "    action : DIAG(int LEVEL = 0)",
	      "           terminate_action / state = READY" };

	if (nl < 0 || nl >= numSpecial) {return 0;}

	line = alloc[nl];
	return 1;
}

char* getToken(char str[], char delim, Name& token ) {


     char TAB = '\t';
	 int LEN = strlen(str); 
    
	if (LEN == 0 ) { token = "\0"; return 0;}

	for (int i=0; i<LEN; i++) {
		if (str[i] == TAB) {str[i] = ' ';}
	}

    char *pend,*ptokbeg,*ptokend,*pnext;

    pend = str+strlen(str);

    if ( delim == ' ' ) { 
       ptokbeg = strtok(str," ");
       assert (ptokbeg != 0);
       token = ptokbeg;
       ptokend = ptokbeg+strlen(ptokbeg);
       pnext = ptokend+1;
       if ( ptokend < pend ) { *ptokend = ' ';}
       if ( pnext < pend ) { return pnext; }
       return 0;
    }
    else {

       char* pdel = strstr(str,&delim);

       if (pdel) { *pdel = '\0'; }

       char* pendd = str+strlen(str);

        ptokbeg = strtok(str," ");
        assert (ptokbeg != 0);
        ptokend = ptokbeg+strlen(ptokbeg);
        if ( ptokend < pendd ) {
           *ptokend = ' ';
        }
        token = ptokbeg;
        if (pdel) {*pdel = delim;}
        token.trim();
        if (pdel) { return pdel+1;}
        return 0;
    }
}



bool check_name(const Name& name) {
	char charset1[] = "&_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char charsetr[] = "&:_-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char ch;

    ch = name[0];

    if ( !strchr(charset1,ch) ) { return 0;}
 
	for (int i=1; name[i]!='\0';i++) {
		ch = name[i];
		if (!strchr(charsetr,ch)) {return 0;}
	}
	return 1;
}

bool check_int(const Name& name) {
     int tempi;
     char ch;

//cout << " check_int called for |" << &name[0] << "|" << endl;
     int flag =  sscanf(&name[0],"%d %c",&tempi,&ch);
//cout << " Flag " << flag << endl;
	 
     if ( flag == 1) { return 1; }     
     return 0;
}

bool check_float(const Name& name) {
     float tempf;
     char ch;

//cout << " check_float called for |" << &name[0] << "|"  << endl;
//cout << " this does not work on PC's" << endl; exit(2);
  	 
     int flag =  sscanf(&name[0],"%f %c",&tempf,&ch);
//cout << " Flag " << flag << endl;


     if ( flag == 1) { return 1; }     
     return 0;
}

bool check_string(const Name& name) {

     if ( name[0] == '\"' ) { return 1; }//anything beginning with " is string
     return 0;          
}

bool check_compname( const Name& name ) {
	
	char *dotptr, *lessptr, *gtptr;
	char* ptr = name.getString();
	
	Name name1, name2;
	
	dotptr = strrchr(ptr,'.');
	if (dotptr)
	{		// if it is compname type, it is name1.name2 format
		*dotptr ='\0';
		name1 = ptr;
		*dotptr = '.';
		name2 = dotptr+1;
		if ( check_name(name1) && check_name(name2) )
		{
			return true;
		}
		return false;
	}
	
	// this has to be name1<name2> format
	lessptr = strrchr(ptr,'<');
	if (!lessptr) { return false; }
	
	*lessptr = '\0';
	name1 = ptr;
	*lessptr = '<';
	ptr = lessptr+1;
	gtptr = strrchr(ptr,'>');
	if (!gtptr) { return false; }
	
	*gtptr = '\0';
	name2 = ptr;
	*gtptr = '>';
	if ( !(*(gtptr+1) == '\0') ) { return false; }
	if ( check_name(name1) && check_name(name2) )
	{
		return true;
	}
	return false;	
	
}
bool my_strcasecmp(const char str1[], const char str2[]) {
     Name temp1,temp2;

	 temp1 = str1;  temp2 = str2;

	 temp1.upCase(); temp2.upCase();

	 if ( temp1 == temp2 ) { return 0; }
	 else { return 1;}
}


//-----------------------------------------------------------------
char*  firstNonBlankPtn(char str[])
{
	int i;
	i = 0;

	for ( i=0; str[i] != 0; i++) {
		if ( str[i] != ' ' && str[i] != '\t') {return &str[i];}
	}
	return &str[i];
}

//--------------------------------------------------------------------
char firstNonBlank(SMLlineVector* pSMLcode,
						const int ist,const int jst,
						int& inbl, int& jnbl, int& inext, int& jnext,
						int& iprev, int& jprev)
{
	SMLline line; char* pLine;
	SMLline linej; char* pLinej;

	char* ptn; char* ptnS;

	int numOfLines = pSMLcode->length();

	if ( ist < 0 || ist >= numOfLines ) {
		cout << "***** Error firstNonBlank...ponting outside the code"
			<< "  call expert " << endl;
		cout << " numOfLines = " << numOfLines << " pointer = " << ist << endl;  exit(2);
	}

	line = (*pSMLcode)[ist];
	pLine = line.getString();

	int len = strlen(pLine);

	if ( jst < 0 || jst > len ) {
		cout << "***** Error firstNonBlank...ponting outside the code"
			<< "  call expert " << endl;
		cout << " line length = " << len << " pointer = " << jst << endl;  exit(2);
	}

	iprev = -1; jprev = 0;
	inext = -1; jnext = 0;

	ptnS = &pLine[jst];
// Pointer to the first character of a line. For the first line (0th), it is
// the pointer to the jst charactet. For the subsequent lines it points
// to the 0th character on the line.

	for (int i=ist;i<numOfLines;i++) {
		if ( i>ist ) {
			line = (*pSMLcode)[i];
			pLine = line.getString();
			ptnS = pLine;
		}

		ptn = firstNonBlankPtn(ptnS);
// Finds the first non-blank (or non TAB) character on the line starting
// with ptnS. If not found, it points to the terminating zero.

		inbl = i; jnbl = ptn-pLine;

		if (ptnS < ptn) {iprev = i; jprev = jnbl-1;}

		if (*ptn == '\0' || *ptn == '#' || *ptn == '!' ) {
			continue;
		}		

		char* ptnNext = ptn+1;
		if (*ptnNext == '\0' || *ptnNext == '#' || *ptnNext == '!' ) {
			for ( int j=i+1; j<numOfLines; j++) {
				linej = (*pSMLcode)[j];
				pLinej = linej.getString();
				if ( pLinej[0] == '\0' || pLinej[0] == '#' || pLinej[0] == '!' ) {
					continue;
				}
				else { inext = j; jnext = 0; break;}
			}

/*			if ( i == (numOfLines-1)) {
				inext = -1; jnext = 0; 
			}
			else {
				inext = i+1; jnext = 0;
			}*/
		}
		else { // if the following character is not end of the line
			inext = i; jnext = jnbl + 1;
		}
		return *ptn;
	}

	iprev = -1; jprev = 0;
	inext = -1; jnext = 0;

	return *ptn;
}



bool strncasecmp(char str1[], const char str2[])
{
	int ln1 = strlen(str1); int ln2 = strlen(str2);

	if ((ln1==0)&&(ln2==0)) { return 0;}

	if ( ln1 < ln2 ) { return 1;}

	char save = str1[ln2]; str1[ln2] = '\0';

	bool temp = my_strcasecmp(str1,str2); str1[ln2] = save;

	return temp;
}

//-------------------------------------------------------------------------
/*char* getNextToken(char str[], const char del[], Name& name)
{
	char tmp; 
	char TAB = '\t';

	char* ptn = firstNonBlankPtn(str);
	tmp = *ptn;
	if ( tmp == '\0' || tmp == '#' || tmp == '!') {return 0;}
	char* ptnstr = ptn;

	for (;;ptn++) {
		tmp = *ptn;
		if (tmp == '\0' || tmp == '#' || tmp == '!' || strchr(del,tmp)){
			if (tmp == ' ' || tmp == TAB) {
				ptn = firstNonBlankPtn(ptn+1);
				tmp = *ptn;
				if (tmp == '\0' || tmp == '#' || tmp == '!' || strchr(del,tmp)) {
					*ptn = '\0'; break;
				}
				else {
					tmp = ' '; ptn--; *ptn = '\0'; break;
				}
			}
			*ptn = '\0'; break;
		}
	}


	name = ptnstr; name.trim(); *ptn = tmp; 
	return ptn;
}*/
//-------------------------------------------------------------------------
char* getNextToken(char str[], const char del[], Name& name)
{
	char tmp; 
	char TAB = '\t';

	name = "\0";

	char* ptn = firstNonBlankPtn(str);
	tmp = *ptn;

	if ( strchr(del,tmp) || tmp == '\0' ) {return ptn;}
	char* ptnstr = ptn;

	if (strchr(del,' ')) {
		for (;;ptn++) {
			tmp = *ptn;
			if (tmp == '\0' || strchr(del,tmp) || tmp == TAB) {
				if (tmp == ' ' || tmp == TAB) {
					ptn = firstNonBlankPtn(ptn+1);
					tmp = *ptn;
					if (tmp == '\0' || strchr(del,tmp)) {
						break;
					}
					else {
						ptn--; break;
					}	
				}
				else {
					break;
				}
			}
		}
	}
	else {
		for (;;ptn++) {
			tmp = *ptn;
			if (tmp == '\0' || strchr(del,tmp)) {
				break;
			}
		}	
	}

	tmp = *ptn; *ptn = '\0'; name = ptnstr; *ptn = tmp;
	return ptn;

/*	for (;;ptn++) {
		tmp = *ptn;
		if (tmp == '\0' || strchr(del,tmp)
			|| (tmp == TAB && strchr(del,' '))){
			if (tmp == ' ' || tmp == TAB) {
				char tmp1 = tmp;
				ptn = firstNonBlankPtn(ptn+1);
				tmp = *ptn;
				if (tmp == '\0' || strchr(del,tmp)) {
					*ptn = '\0'; break;
				}
				else {
					tmp = tmp1; ptn--; *ptn = '\0'; break;
				}
			}
			*ptn = '\0'; break;
		}
	}


	name = ptnstr; *ptn = tmp;
	return ptn;
*/
}
//----------------------------------------------------------------------
char getNextToken(SMLlineVector* pSMLcode,
					const int ist,const int jst,const char del[],
					Name& token, int& idel, int& jdel,
					int& inext, int& jnext) 
{
//	cout << " Search starting line " << ist << " pos " << jst << endl;
	char tmp; 
	Name tempdel = del;
	tempdel += "!#";
	// I do not think that adding these two is necessary. (same as \0)
	char* ptempdel = tempdel.getString(); 

	int numOfLines = pSMLcode->length(); 

	token = "\0";

	if ( ist < 0 || ist >= numOfLines ) {
		cout << " Error getNextToken...ponting outside the code"
			<< "  call expert " << endl; abort();
	}

// first skip all the leading spaces and find the first non-blank character
// it will never be '#' or '!
	int inbl,jnbl,iprev,jprev;
	tmp = firstNonBlank(pSMLcode,ist,jst,inbl,jnbl,inext,jnext,iprev,jprev);

	if ( tmp == '\0' || strchr(ptempdel,tmp)) {
// if there are only spaces up to the end of the SML code, 'zero' is returned.
//     token is set to "\0" (idel,jdel) will be undefined, (inext,jnext) set to (-1,0)
// if the first character is one of the non-blank delimiters, its value is returned
//     token is set to "\0", (idel,jdel) set to the delimiter address
//     (inext,jnext) set to the character following the delimiter.
		idel = inbl; jdel = jnbl;
		return tmp;
	}
	
// -------------------------------------------------------------------------------
//  The begining of token found. Its address is (inbl,jnbl)
// -------------------------------------------------------------------------------	
	SMLline line = (*pSMLcode)[inbl];
	char* pLine = line.getString();
	char* ptn = &pLine[jnbl];

// first will collect the token on the line, where it starts
	ptn = getNextToken(ptn,ptempdel,token);
	idel = inbl; jdel = ptn-pLine;
//
//   ptn is the pointer of its delimiter  (idel,jdel) is the delimiter's address

// the following is in an experimental stage


	if ( *ptn == '\0' || *ptn == '!' || *ptn == '#') {
	// -------------------------------------------------
	//   token is delimited by the end of the line
	//  ------------------------------------------------
	//      if it is the last line of the SML segment, then just set (inext,jnext) (-1,0)
		if ( (inbl+1) >= numOfLines ) {
			inext = -1; jnext = 0;
			token.trim();
			return *ptn;
		}

        //      if token can not contain spaces, then we are done and consider this
	//      to be the end of the token. We can not however return this as the delimiter,
	//      because we are allowed to return either zero, or the proper delimiter.
	//      so we have to do some forward search to find something more suitable.
	//      What it means, is that the returned delimiter does not immediately follow
	//      the token.
	//      If the first non-blank character is one of the proper delimiters, we take
	//      it. Otherwise we take as delimiter the previous character. 
	//     **** This code needs reviewing ****
		if ( strchr(del,' ') ) {
			tmp = firstNonBlank(pSMLcode,inbl+1,0,inbl,jnbl,inext,jnext,iprev,jprev);
//			if (jlast==-1) {jlast = jdel;}
//***			if (jprev == -1 ) {iprev = idel; jprev = jdel-1;}
			if (iprev == -1 ) {iprev = idel; jprev = jdel-1;}
			if (tmp == '\0' || tmp == '#' || tmp == '!') {
//				inext = -1; jnext = 0;
				idel = inbl; jdel = jnbl;
				return tmp;
			}
			if (strchr(del,tmp)) {
//				inext = inbl; jnext = jnbl+1;
				idel = inbl; jdel = jnbl;
				return tmp;
			}
			else {
				inext = inbl; jnext = jnbl;
				idel = iprev; jdel = jprev+1;
/*				if (jnbl == 0) {
					inext = inbl; jnext = 0;
					idel = inbl-1; jdel = jlast;
				}
				else {
					inext = inbl; jnext = jnbl;
					idel = inbl; jdel = jnbl - 1;
				}*/
				return tmp;
			}
		}
	//      If token can contain spaces, then we cary on and keep adding to the
	//      so far collected token, until we either reach the end of the SML section,
	//      or find proper delimiter
		else {
			int ist,jst;
			ist = inbl+1; jst = 0;
			for (;;) {
				Name partialToken;
				line = (*pSMLcode)[ist];
				pLine = line.getString();
				ptn = &pLine[jst];
				ptn = getNextToken(ptn,ptempdel,partialToken);
				token += partialToken;
				if ( *ptn == '\0' || *ptn == '!' || *ptn == '#') {
					if ( (ist+1) >= numOfLines ) {
						inext = -1; jnext = 0;
						idel = ist; jdel = ptn-pLine;
						token.trim();
						return *ptn;
					}
					else {
						ist++; jst=0; 
					}
				}
				else {
					idel = ist; jdel = ptn-pLine; token.trim();
					char* ptnNext = ptn+1;
					if ( *ptnNext == '\0' || *ptnNext == '!' || *ptnNext == '#') {
						inext = idel +1; jnext =0;
						if (inext >= numOfLines) {inext = -1;}
					}
					else {
						inext = idel; jnext = jdel+1;
					}
					return *ptn;
				}
			}
		}
	}
	else {
	// -------------------------------------------------------------------
	//  the token is terminated by its proper delimiter on the same line it 
	//   starts. Easy, peasy.
	// -------------------------------------------------------------------
		idel = inbl; jdel = ptn-pLine; token.trim();
		char* ptnNext = ptn+1;
		if ( *ptnNext == '\0' || *ptnNext == '!' || *ptnNext == '#') {
            inext = inbl +1; jnext =0;
			if (inext >= numOfLines) {inext = -1;}
		}
		else {
			inext = inbl; jnext = jdel+1;
		}
		return *ptn;
	}
	
	return '\0';
}

//--------------------------------------------------------------------------

char* getType(char str[],char* type)
{
	char tmp;
	char TAB = '\t';

	for (int i=0; ; i++) {
		tmp = str[i];
		if (tmp == '\0' || tmp == '#' || tmp == '!') {break;}
		if (tmp == ' ' || tmp == TAB) {continue;}
		if (!strncasecmp(&str[i], "INT ")) {
			strcpy(type,"INT");
			return &str[i+4];
		}
		else if (!strncasecmp(&str[i], "FLOAT ")) {
			strcpy(type,"FLOAT");
			return &str[i+6];
		}
		else if (!strncasecmp(&str[i], "STRING ")) {
			strcpy(type,"STRING");
			return &str[i+7];
		}
		else {
			strcpy(type,"STRING");
			return &str[i];
		}
	}
	return 0;
}

char* getName(char str[], const char del[], Name& name, int& ierr)
{
	char tmp; 
	char TAB = '\t';
	int i;
	int istart = -1; int iend;

	for (i=0; ; i++) {
		tmp = str[i];
		if (tmp == '\0' || tmp == '#' || tmp == '!') {break;}
		if (tmp == ' ' || tmp == TAB) {continue;}
			istart = i;
			break;
	}

	if (istart == -1) {return 0;}

	for (i=istart; ; i++) {
		tmp = str[i];
		if (tmp == '\0' || tmp == '#' || tmp == '!') {
			iend = i; str[iend] = '\0'; 
			break;
		}
		if ( strchr(del,tmp) ) {
				iend = i; str[iend] = '\0'; 
				break;
		}		
	}

	name = &str[istart]; str[iend] = tmp; 
	name.upCase();name.trim();
	if (check_name(name)) {ierr = 0;}
	else {ierr = 1;}
	return &str[iend];
}

   char* getValue(char str[], const char del[], 
                  char* type, Name& value, int& ierr)
{
	char tmp;
	char TAB = '\t';
	int i;
	int istart = -1; int iend;

	for (i=0; ; i++) {
		tmp = str[i];
		if (tmp == '\0' || tmp == '#' || tmp == '!') {break;}
		if (tmp == ' ' || tmp == TAB) {continue;}
			istart = i;
			break;
	}

	if (istart == -1) {return 0;}

	char* ptnrAfter;

	if (str[istart] == '\"') {
		strcpy(type,"STRING");
		ptnrAfter = getString(&str[istart],value,ierr);
		if (ierr != 0) {return 0;}

		for (;;ptnrAfter++) {
			tmp = *ptnrAfter;
			if (tmp == '\0' || tmp == '#' || tmp == '!') {break;}
			if (tmp == ' ' || tmp == TAB ){continue;}
			if ( strchr(del,tmp) ) { 
				ierr=0;
				return ptnrAfter;
			}
		}

		ierr=0; return ptnrAfter;
	}

	for (i=istart; ; i++) {
		tmp = str[i];
		if (tmp == '\0' || tmp == '#' || tmp == '!') {
			iend = i; str[iend]='\0'; break;
		}
		if (strchr(del,str[i])) {
			iend = i; str[iend]='\0'; break;
		}
	}

	value = &str[istart]; str[iend] = tmp; 
	value.upCase(); value.trim();
	if (check_int(value)) {strcpy(type,"INT"); ierr = 0; }
	else if (check_float(value)) {strcpy(type,"FLOAT"); ierr = 0; }
	else if (check_name(value)) {strcpy(type,"NAME"); ierr = 0; }
	else {strcpy(type,"UNKNOWN"); ierr = 0; }
	return &str[iend];
}
//------------------------------------------------------------------------
char getValue(SMLlineVector* pSMLcode,
		const int ist,const int jst,const char del[],
		Name& value, char* type, int& ierr, int& idel, int& jdel,
		int& inext, int& jnext)
{
	char tmp; 
	Name tempdel = del;
	tempdel += "!#";
	char* ptempdel = tempdel.getString(); 

	int numOfLines = pSMLcode->length(); 

//cout << " Calling getValue ist,jst " << ist << " " << jst << endl;
//cout << "                  delimiters |" << del << endl;

	value = "\0";

	if ( ist >= numOfLines ) {
		cout << " Error getValue...ponting outside the code"
			<< "  call expert " << endl; exit(2);
	}

	int inbl,jnbl,iprev,jprev;
	tmp = firstNonBlank(pSMLcode,ist,jst,inbl,jnbl,inext,jnext,iprev,jprev);

//cout << " First non blank character |" << tmp << "|" << endl;

	if ( tmp == '\0' || strchr(ptempdel,tmp)) {
		idel = inbl; jdel = jnbl; 
                cout << " Looking for value and found nothing" << endl;
                ierr = 1;
		return tmp;
	}

	int icurr,jcurr;
	icurr = inbl; jcurr = jnbl;

	SMLline line = (*pSMLcode)[icurr];
	char* pLine = line.getString();
	char* ptn = &pLine[jcurr];



	char* ptnrAfter;
	char charAfter;
	if (tmp == '\"') {
		strcpy(type,"STRING");
		ptnrAfter = getString(ptn,value,ierr);
		if (ierr != 0) {
			cout << " error parsing string" << endl; 
			return '\0';
		}

		jcurr = ptnrAfter-pLine;
		charAfter = *ptnrAfter;
// If the character after the value is blank and blank is one of the delimiters, we finished
		if (  charAfter == ' '  && strchr(ptempdel,' ') )
		{
			ierr = 0; idel = icurr; jdel = jcurr;
			inext = icurr; jnext = jcurr;
			return charAfter;
		}
// If blank is not one of the delimiters, we have to skip all the blanks before the real delimiter
// because this is the promise.
		tmp = firstNonBlank(pSMLcode,icurr,jcurr,inbl,jnbl,inext,jnext,iprev,jprev);

		if ( tmp == '\0' || strchr(ptempdel,tmp)) {
			ierr = 0; idel = inbl; jdel = jnbl; 
			return tmp;
		}
                cout << " Error Looking for a delimiter" << endl;
		ierr = 1;
		return '\0'; 
	}

	tmp = getNextToken(pSMLcode, icurr, jcurr, del,value,idel,jdel,inext,jnext);

//cout << "   next token " << value << " ending with |" << tmp << "|" << endl; 

        ierr = 0;
	value.upCase(); 
	value.trim();
	if (check_int(value)) {strcpy(type,"INT"); }
	else if (check_float(value)) {strcpy(type,"FLOAT"); }
	else if (check_name(value)) {strcpy(type,"NAME");  }
	else if (check_compname(value)) {strcpy(type,"COMPNAME");  }
	else {strcpy(type,"UNKNOWN"); }

//cout << " The type is " << type << endl;
	return tmp;
		
}
//------------------------------------------------------------------------

char* getString(char str[],Name& value, int& ierr) 
{
// Assumption here is, that str[0] = '\"'

	for (int i=1; str[i]; i++) {
		if (str[i] == '\"') {
			if (str[i-1] == '\\') {continue;}
			else {
			// unescaped ", i.e. the end of the string
				char savechar = str[i+1]; str[i+1] = '\0';
				int numchar = strlen(str);
				int charout = smixx_escape_slash(NULL,str,0);
				if ( charout >= 0 )
				{
                                    ierr = 0;
				    if ( charout > numchar) {
				    // there are slashes
				        Name tempvalue(charout+1); /* do not forget the space for zero */
					smixx_escape_slash(tempvalue.getString(),str,charout);
					value = tempvalue;
				    }
				    else {
				        value = str;
				    }
				}
				else {
				    ierr = 1;
				}
				str[i+1] = savechar;
			        return &str[i+1]; 
			}
		}
	}
        ierr = 1; return &str[strlen(str)];
}
