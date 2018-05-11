//----------------------  Class   Name  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 28 December 1996
// Copyright Information:
//      Copyright (C) 1996-2002 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "name.hxx"
#include <string.h>
#include <ctype.h>
#include <assert.h>
//
//
//-----------------------   Constructors -------------------------------------
  Name::Name() {
    _size=0;
    _ptn=0;
   }
   
   Name::Name(const int numchar) {
      _size = numchar + 1;
      _ptn = new char[_size];
      assert(_ptn != 0);
      *_ptn = '\0';
   } 

  Name::Name(const char* str){
     _size = strlen(str)+1;
     _ptn = new char[_size];
     assert(_ptn != 0);
     strcpy(_ptn,str);
  }

  Name::Name(const Name& name){
      _size = name._size;
      _ptn = new char[_size];
      assert(_ptn != 0);
      strcpy(_ptn,name._ptn);
  }

//----------------------------------  append  ------------------------------

  void Name::append(const Name& name){
      int sizenew;
//
      if ( name._size <= 1 ) {return;}
//
      if ( _size > 0 ) {
           sizenew = _size + name._size - 1;  // _size includes the \0 only once!!
      }
      else {
           sizenew = name._size;
      }
//
      char* ptnew = new char[sizenew];
      assert(ptnew != 0);
//
      if (_size > 0) {
         strcpy(ptnew,_ptn);
         delete [] _ptn;      
         strcat(ptnew,name._ptn);
      }
      else {
         strcpy(ptnew,name._ptn);
      }
      _ptn = ptnew;
      _size = sizenew;
  }

  void Name::append(const char* str){
     Name temp = str;
     append(temp);
     return;
  }

//-------------------------------  element -------------------------------------
// This should perform basically the same function as VMS str$element
//
  bool Name::element(const int elNum, const char delim, Name& elem) const {
     char *ptnr1, *ptnr2;

     ptnr2 = _ptn - 1;

     for ( int i=0; i<=elNum; i++ ) {
         ptnr1 = ptnr2 + 1;
         ptnr2 = strchr(ptnr1,delim);
         if ( ptnr2==0 && i<elNum ) { return 0; }
     }
//
     if (ptnr2) {
        *ptnr2 = '\0';
        elem = ptnr1;
        *ptnr2 = delim;
     }
     else {
        elem = ptnr1;
     }
//
     return 1;

  }

//-------------------------------  elementSp -----------------------------------
// This should perform basically the same function as VMS str$element except:
//  it will consider constructs   "....." within the name as sacrosanct and
//  will not look inside for delimiters. Of course a single " inside the 
//  construct is not allowed 
//
  bool Name::elementSp(const int elNum, const char delim, Name& elem) const {
     char *ptnr1, *ptnr2;
     char *ptndel, *ptndbq;

     ptnr2 = _ptn - 1;

     for ( int i=0; i<=elNum; i++ ) {
         ptnr1 = ptnr2 + 1;
//
//-- beginning of the ith element: ptnr1...points to its begining
//-- ptnr2 ... is going to point to the delimiting delimiter or if not found
//             it will be 0.
//
//
         char *ptn;         
         ptn = ptnr1;
//
//---- looking for ptnr2 ----------------------------------------
//
         while ( 0 == 0 ) {
            ptndel = strchr(ptn,delim);    // pointer to the nearest delimiter
            ptndbq = strchr(ptn,'\"');     // pointer to the nearest "
            if ( ptndel == 0 ) {
//---          delimiter not found
               if ( i == elNum ) { ptnr2 = 0; break;}  // no delim. for the last elem.
               else { return 0; }           // obviously elNum elem. does not 
//                                          // exist
            }
//---       delimiter found
            if ( ptndbq == 0 || ptndbq > ptndel ) {
//-----     There is no " before the delimiter therefore the end of ith element
//                                                                 reached
                ptnr2 = ptndel;   
                break;
            } 
            else {
//-------   there is " before the delimiter ... find the matching one one
                 ptn = strchr(ptndbq+1,'\"');
                 if ( ptn == 0 ) { 
                    cout << "  matching \" not found \n"; 
                    abort();
                 }
                 ptn++;
            }
         }
//---- end of looking for ptnr2 ----------------------------------------
     }
//
     if (ptnr2) {
        *ptnr2 = '\0';
        elem = ptnr1;
        *ptnr2 = delim;
     }
     else {
        elem = ptnr1;
     }
//
     return 1;

  }

//--------------------------------- exists -----------------------------------

  bool Name::exists(const char* str) const {
      char* temp;

      temp = strstr(_ptn,str);

      if (temp) {return 1;}
      return 0;
  }
  
  bool Name::exists(const Name& nm) const{
      char* temp;
      
      temp = strstr(_ptn,nm._ptn);

      if (temp) {return 1;}
      return 0;      
  }
//------------------------------- existsAt------------------ B. Franek Feb99
  int Name::existsAt(const char* str) const {

      char* temp = strstr(_ptn,str);
      if (!temp) {return -1;}

      return temp - _ptn;
  }
//------------------------------- getString -----------------B.Franek Feb99
  char* Name::getString() const {
      return _ptn;
  }
//-------------------------------  replace ---------- B. Franek  Dec97 --------

  void Name::replace(const Name& oldstr, const Name& newstr) {
       char* beg;
       char save;
       char* ptst;

       if ( _size <= 1 ) {return;} // there is nothing to be replaced

       Name temp = *this;
       
       delete [] _ptn;
       
       _size = 0;
       _ptn = 0;
       
       ptst = temp._ptn;
        
       for ( ;;) {
       
          beg = strstr(ptst,oldstr._ptn);
          if (beg) {
             save = *beg;
             *beg = '\0';
             append(ptst);
             *beg = save;
             append(newstr._ptn);
             ptst = beg + strlen(oldstr._ptn); 
          }
          else {
             append(ptst);  break;
          }
       }
  }

//--------------------------- operators -------------------------------------

  Name& Name::operator=(const Name& name) {
      if (_ptn != 0) { delete [] _ptn; }
      _size = name._size;
      _ptn = new char[_size];
      assert(_ptn != 0);
      strcpy(_ptn,name._ptn);
      return *this;
  }

  Name& Name::operator=(const char* str) {
      if (_ptn != 0) { delete [] _ptn; }
     _size = strlen(str)+1;
     _ptn = new char[_size];
     assert(_ptn != 0);
     strcpy(_ptn,str);
     return *this;
  }
  

Name& Name::operator=(const int num) {
	assert(num >= INT_MIN); 
	assert(num <= INT_MAX);
	if (_ptn != 0) { delete [] _ptn; }
	int uplim = 1;
	int absnum = num; if (num < 0) {absnum = -num;}
	
	int i;
	for (i=1; ; i++) {
		uplim = 10*uplim;
		if ( absnum < uplim ) break;
	}
	_size = i + 1 +1;		//1 char for sign and 1 for the end 0
	_ptn = new char[_size];
	assert(_ptn != 0);
	sprintf(_ptn,"%d",num);
	return *this;
  }
  
  

  bool operator == ( const Name& nm1, const Name& nm2) {

      if( !strcmp(nm1._ptn, nm2._ptn) ) return 1;

      return 0;
}

  bool operator == ( const Name& nm, const char string[]) {

	int stringsize = (strlen(string)+1);
	
      if ( nm._size != stringsize ) { return 0; }

      if( !strcmp(nm._ptn,string) ) return 1;

      return 0;
}

//-----------------------------  += ---------------------------------------------
  Name& Name::operator+= (const Name &name) {
      append(name);
      return *this;
  }

//-----------------------------  += ---------------------------------------------
  Name& Name::operator+= (const char* str) {
      append(str);
      return *this;
  }

  int Name::length() const {
      return _size;
  }

//------------------------------ [] ------------------------------------------
  char& Name::operator[] (const int elem) const {
      assert ( elem >= 0 && elem < _size );
      return _ptn[elem];
  }
//--------------------------  Destructor -------------------------------------

  Name::~Name() {
      delete [] _ptn;
  }

//--------------------------  whatAreYou ------------------------------------

  void Name::whatAreYou(char* str, const int mxsize) const {
     if ( mxsize < _size ) { 
        cout <<  " String : " << "\n" << _ptn << "\n" 
             <<  " is too large \n ";
             abort();
     }
     strcpy(str,_ptn);
  }

//--------------------------- out -------------------------------------------

  void Name::out() const {
     cout  << _ptn << " " ;
  }


ostream& operator<<(ostream& os, const Name& name) {
   os << "<" << name._ptn << ">";
   return os;
}

//--------------------------- squeeze ------------------ B.Franek  Dec 97 -----
//  removes blanks and tabsfrom the name
//
   void Name::squeeze() {
      char TAB = '\t';
//
      if ( _size <= 1 ) {return;}
//
//  First count the blanks

    int blanks=0;
    for ( int i=0; i<(_size-1) ; i++) {
        if ( (_ptn[i] == ' ') || (_ptn[i] == TAB) ) { blanks++; }
    }

    if (blanks == 0) {return;}

      char* pnew;  int sizenew = _size - blanks;
//
      pnew = new char[sizenew];
      assert(pnew != 0);
//
      int num = 0;
      for (int j=0; j<(_size-1); j++) {
           if ( (_ptn[j] != ' ') && (_ptn[j] != TAB) ) { 
               pnew[num] = _ptn[j]; num++; 
           }      
      }
      pnew[sizenew-1] = '\0';

      delete [] _ptn;

      _ptn = pnew;
      _size = sizenew;

      return; 
   }
//------------------------------------------- B. Franek  Dec 98 ---------------

   void Name::trim(){
      char TAB = '\t';
      if ( _size <= 1 ) {return;}

      int sizenew = _size;

      for ( int i = (_size-2); i>=0; i--) {
          if ( (_ptn[i] == ' ') || (_ptn[i] == TAB) ) { 
              _ptn[i] = '\0'; sizenew--; 
          }
          else { break; }
      }

      if ( sizenew == _size ) { return; }

      char* pnew = new char[sizenew];
      assert(pnew != 0);
      
      strcpy(pnew,_ptn);

      delete [] _ptn;

      _ptn = pnew;
      _size = sizenew;

      return; 

   }
//============================================= B. Franek  Jan 09 ==============

   void Name::trimFront()
   {
      char TAB = '\t';
      if ( _size <= 1 ) {return;}

      int sizenew = _size;
      
      for ( int i = 0; i <= (_size-2); i++ )
      {
          if ( (_ptn[i] == ' ') || (_ptn[i] == TAB) ) { 
              sizenew--; 
          }
          else { break; }
      }
       
      if ( sizenew == _size ) { return; }

      char* pnew = new char[sizenew];
      assert(pnew != 0);
      
      strcpy(pnew,_ptn + _size - sizenew);

      delete [] _ptn;

      _ptn = pnew;
      _size = sizenew;

      return; 

   }
//------------------------------------------------  B.Franek  Dec98 -----------

   void Name::upCase() {

      if ( _size <= 1 ) {return;}

      for ( int i=0; i < (_size-1); i++) {
          _ptn[i] = toupper(_ptn[i]);
      }      

      return;
   }
//------------------------------------------------ B. Franek  May 2010 ---------

void Name::removeLastChar()
{
	if (_size <= 1) {return;}

	int sizenew = _size - 1;

	char* pnew = new char[sizenew];
	assert(pnew != 0);
	
	_ptn[_size-2] = '\0';
      
	strcpy(pnew,_ptn);

	delete [] _ptn;

	_ptn = pnew;
	_size = sizenew;

	return; 
}
	
//------------------------------------------------- B.Franek May 2007 ----------

void Name::subString(int index, int nchar, Name& substr) const
{
	int ntocopy;
	if ( _size <= 1 ) 
	{
		substr = "";
		return;
	}
	int nleft = strlen(_ptn) - index;
	if ( nleft <= 0 )
	{
		substr = "";
		return;
	}  
	if ( nchar > 0 )
	{
		ntocopy = nchar;
		if ( nleft < nchar ) ntocopy = nleft;
	}
	else
	{
		ntocopy = nleft;
	}
	if ( substr._ptn ) delete [] substr._ptn;
	substr._size = ntocopy + 1;
	substr._ptn = new char[substr._size];
	strncpy(substr._ptn,_ptn+index,ntocopy);
	*(substr._ptn+substr._size-1) = '\0';  
	return;
}
