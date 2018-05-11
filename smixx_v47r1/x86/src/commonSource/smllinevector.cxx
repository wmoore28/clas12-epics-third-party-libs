// Created from VECTOR template on  16-DEC-1998 14:17:30.42
//----------------------  Class   SMLlineVector  ----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "smllinevector.hxx"
#include "smlline.hxx"
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
//
//
//-----------------------   Constructors -------------------------------------
  SMLlineVector::SMLlineVector() {
    _size=0;
    _incr=1;
    _len=0;
    _ptn=0;
   }

  SMLlineVector::SMLlineVector(int incr) {
    _size=0;
    _incr=incr;
    _len=0;
    _ptn=0;
   }


  SMLlineVector::SMLlineVector(const SMLlineVector& vector){
      _ptn = 0;
      copy(vector);
  }

//--------------------------  Destructor -------------------------------------

  SMLlineVector::~SMLlineVector() {
      if ( _size > 0 ) { delete [] _ptn; return; }
  }
//-------------------------------------------------------------------------

//--------------------------- operator =  -------------------------------------

  SMLlineVector& SMLlineVector::operator=(const SMLlineVector& vector) {
      copy(vector);
      return *this;
  }

//--------------------------- operator +=  -------------------------------------

  SMLlineVector& SMLlineVector::operator+=(const SMLline& object) {
	 add(object); 
     return *this;
  }

//--------------------------- operator +=  -------------------------------------
  SMLlineVector& SMLlineVector::operator+=(const SMLlineVector& vector) {
      append(vector);
      return *this;
  }

//------------------------------ operator [] ----------------------------------
  SMLline SMLlineVector::operator[] (const int elem) const {
      assert ( elem >= 0 && elem < _len );
      return _ptn[elem];
  }


//-----------------------------------  length  -------------------------------
  int SMLlineVector::length() const {
      return _len;
  }

//--------------------------- out -------------------------------------------

  void SMLlineVector::out(char* offset) const {

      for ( int i=0; i<_len; i++) {
		  cout << offset;
         (_ptn+i)->out();
      }
  }

  void SMLlineVector::out(const int i, char* offset) const 
  {
	  if (i>=_len) return;
	  cout << offset;
	(_ptn+i)->out();
	return;
  }

//--------------------------  protected functions  ---------------------------
//------------------------------------------------------------------------------
//---------------------------  copy  -----------------------------------------
  void SMLlineVector::copy(const SMLlineVector& vector) {
      if (_ptn != 0) { delete [] _ptn; }

      _size = vector._size;
      _incr = vector._incr;
      _len  = vector._len;

      if ( _len <= 0 ) { _ptn=0; return;}

      _ptn = new SMLline[_size];
      assert(_ptn != 0);

      for ( int i=0; i < _len; i++ ) {
         *(_ptn+i) = *(vector._ptn+i);
      }

  }
//--------------------------- add  -------------------------------------

  void SMLlineVector::add(const SMLline& object) {
 
     if ( _len >= _size ) {
        SMLline *ptnnew = new SMLline[_size+_incr];
        assert(ptnnew != 0);

        if ( _size > 0 ) { 
           for ( int i=0; i < _size; i++ ) {
              *(ptnnew+i) = *(_ptn+i);
           }
           delete [] _ptn;
        }

        _ptn = ptnnew;
        _size = _size + _incr;
      }

      *(_ptn+_len) = object;
      _len++;
  }

//--------------------------- append  -------------------------------------
  void SMLlineVector::append(const SMLlineVector& vector) {
      SMLline* ptnew;
      int sizenew,lennew;
//
      if ( vector._size <= 0 ) {return;}
//
      if (_size <= 0) { copy(vector); return; }

      lennew = _len + vector._len ;  
      sizenew = lennew ;  

//
      ptnew = new SMLline[sizenew];
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
