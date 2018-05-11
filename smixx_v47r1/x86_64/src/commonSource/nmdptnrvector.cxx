// Created from VECTOR template on  16-DEC-1998 16:45:33.09
//----------------------  Class   NmdPtnrVector  ----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "nmdptnrvector.hxx"
#include "nmdptnr.hxx"
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
//
//
//-----------------------   Constructors -------------------------------------
  NmdPtnrVector::NmdPtnrVector() {
    _size=0;
    _incr=1;
    _len=0;
    _ptn=0;
   }

  NmdPtnrVector::NmdPtnrVector(int incr) {
    _size=0;
    _incr=incr;
    _len=0;
    _ptn=0;
   }


  NmdPtnrVector::NmdPtnrVector(const NmdPtnrVector& vector){
      _ptn = 0;
      copy(vector);
  }

//--------------------------  Destructor -------------------------------------

  NmdPtnrVector::~NmdPtnrVector() {
      if ( _size > 0 ) { delete [] _ptn; return; }
  }
//-------------------------------------------------------------------------

//--------------------------- operator =  -------------------------------------

  NmdPtnrVector& NmdPtnrVector::operator=(const NmdPtnrVector& vector) {
      copy(vector);
      return *this;
  }

//--------------------------- operator +=  -------------------------------------

  NmdPtnrVector& NmdPtnrVector::operator+=(const NmdPtnr& object) {
     add(object);
     return *this;
  }

//--------------------------- operator +=  -------------------------------------
  NmdPtnrVector& NmdPtnrVector::operator+=(const NmdPtnrVector& vector) {
      append(vector);
      return *this;
  }

//------------------------------ operator [] ----------------------------------
  NmdPtnr NmdPtnrVector::operator[] (const int elem) const {
      assert ( elem >= 0 && elem < _len );
      return _ptn[elem];
  }


//-----------------------------------  length  -------------------------------
  int NmdPtnrVector::length() const {
      return _len;
  }

//--------------------------- out -------------------------------------------

  void NmdPtnrVector::out() const {

      for ( int i=0; i<_len; i++) {
          cout << *(_ptn+i) << endl;
      }
  }

//--------------------------  protected functions  ---------------------------
//------------------------------------------------------------------------------
//---------------------------  copy  -----------------------------------------
  void NmdPtnrVector::copy(const NmdPtnrVector& vector) {
      if (_ptn != 0) { delete [] _ptn; }

      _size = vector._size;
      _incr = vector._incr;
      _len  = vector._len;

      if ( _len <= 0 ) { _ptn=0; return;}

      _ptn = new NmdPtnr[_size];
      assert(_ptn != 0);

      for ( int i=0; i < _len; i++ ) {
         *(_ptn+i) = *(vector._ptn+i);
      }

  }
//--------------------------- add  -------------------------------------

  void NmdPtnrVector::add(const NmdPtnr& object) {
 
     if ( _len >= _size ) {
        NmdPtnr *ptnnew = new NmdPtnr[_size+_incr];
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
  void NmdPtnrVector::append(const NmdPtnrVector& vector) {
      NmdPtnr* ptnew;
      int sizenew,lennew;
//
      if ( vector._size <= 0 ) {return;}
//
      if (_size <= 0) { copy(vector); return; }

      lennew = _len + vector._len ;  
      sizenew = lennew ;  

//
      ptnew = new NmdPtnr[sizenew];
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
