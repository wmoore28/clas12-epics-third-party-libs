// Created from VECTOR template on  15-DEC-1998 12:38:24.95
//----------------------  Class   NameVector  ----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "namevector.hxx"
#include "name.hxx"
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
//
//
//-----------------------   Constructors -------------------------------------
  NameVector::NameVector() {
    _size=0;
    _incr=1;
    _len=0;
    _ptn=0;
   }

  NameVector::NameVector(int incr) {
    _size=0;
    _incr=incr;
    _len=0;
    _ptn=0;
   }


  NameVector::NameVector(const NameVector& vector){
      _ptn = 0;
      copy(vector);
  }

//--------------------------  Destructor -------------------------------------

  NameVector::~NameVector() {
      if ( _size > 0 ) { delete [] _ptn; return; }
  }
//-------------------------------------------------------------------------

//--------------------------- operator =  -------------------------------------

  NameVector& NameVector::operator=(const NameVector& vector) {
      copy(vector);
      return *this;
  }

//--------------------------- operator +=  -------------------------------------

  NameVector& NameVector::operator+=(const Name& object) {
     add(object);
     return *this;
  }

//--------------------------- operator +=  -------------------------------------
  NameVector& NameVector::operator+=(const NameVector& vector) {
      append(vector);
      return *this;
  }

//------------------------------ operator [] ----------------------------------
  Name& NameVector::operator[] (const int elem) const {
      assert ( elem >= 0 && elem < _len );
      return _ptn[elem];
  }


//-----------------------------------  length  -------------------------------
  int NameVector::length() const {
      return _len;
  }

//--------------------------- out -------------------------------------------

void NameVector::out(const Name offset) const {

	if (_len == 1 ) { cout << *_ptn << endl; return;}

	char* ptnoffset = offset.getString();


	cout << endl ;

	for ( int i=0; i<_len; i++) {
		cout << ptnoffset << *(_ptn+i) << endl;
	}
}
//--------------------------  isPresent  -------------------------------------
bool NameVector::isPresent(const Name& name) const {

	for ( int i=0; i<_len; i++) {
		if ( *(_ptn+i) == name ) {return 1;}
	}
	return 0;
}
//--------------------------  protected functions  ---------------------------
//------------------------------------------------------------------------------
//---------------------------  copy  -----------------------------------------
  void NameVector::copy(const NameVector& vector) {
      if (_ptn != 0) { delete [] _ptn; }

      _size = vector._size;
      _incr = vector._incr;
      _len  = vector._len;

      if ( _len <= 0 ) { _ptn=0; return;}

      _ptn = new Name[_size];
      assert(_ptn != 0);

      for ( int i=0; i < _len; i++ ) {
         *(_ptn+i) = *(vector._ptn+i);
      }

  }
//--------------------------- add  -------------------------------------

  void NameVector::add(const Name& object) {
 
     if ( _len >= _size ) {
        Name *ptnnew = new Name[_size+_incr];
        assert(ptnnew != 0);

        if ( _size > 0 ) { 
           for ( int i=0; i < _size; i++ ) {
              *(ptnnew+i) = *(_ptn+i);
           }
           delete [] _ptn;
        }

        _ptn = ptnnew;
        _size = _size + _incr;
	
	int newincr = _len / 5;
	if ( newincr > _incr ) _incr = newincr;
      }

      *(_ptn+_len) = object;
      _len++;
  }

//--------------------------- append  -------------------------------------
  void NameVector::append(const NameVector& vector) {
      Name* ptnew;
      int sizenew,lennew;
//
      if ( vector._size <= 0 ) {return;}
//
      if (_size <= 0) { copy(vector); return; }

      lennew = _len + vector._len ;  
      sizenew = lennew ;  

//
      ptnew = new Name[sizenew];
      assert(ptnew != 0);
//
      
      for ( int i=0; i < _len; i++) {
         *(ptnew+i) = *(_ptn+i);
      }


      for (int j=0; j< vector._len; j++) {
         *(ptnew+_len+j) = *(vector._ptn+j);
      }

      delete [] _ptn;

      _ptn = ptnew;
      _size = sizenew;
      _len = lennew;
  }
//-----------------------------------------------------------------------
NameVector& NameVector::exclusiveUpdate(const NameVector& vector) {
	if ( vector._len <= 0 ) {
		return *this;
	}
	
	int i;
	for (i=0;i<vector._len;i++) {
		if ( !isPresent(vector._ptn[i]) ) {
			add(vector._ptn[i]);
		}
	}
	
	return *this;
}
