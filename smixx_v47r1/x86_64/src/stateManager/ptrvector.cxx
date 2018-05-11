// Created from VECTOR template on  16-DEC-1998 16:45:33.09
//----------------------  Class   PtrVector  ----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "ptrvector.hxx"
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
//
//
//-----------------------   Constructors -------------------------------------
  PtrVector::PtrVector() {
    _size=0;
    _incr=1;
    _len=0;
    _ptn=0;
   }

  PtrVector::PtrVector(int incr) {
    _size=0;
    _incr=incr;
    _len=0;
    _ptn=0;
   }


  PtrVector::PtrVector(const PtrVector& vector){
      _ptn = 0;
      copy(vector);
  }

//--------------------------  Destructor -------------------------------------

  PtrVector::~PtrVector() {
      if ( _size > 0 ) { delete [] _ptn; return; }
  }
//-------------------------------------------------------------------------

//--------------------------- operator =  -------------------------------------

  PtrVector& PtrVector::operator=(const PtrVector& vector) {
      copy(vector);
      return *this;
  }

//--------------------------- operator +=  -------------------------------------

  PtrVector& PtrVector::operator+=(void *object) {
     add(object);
     return *this;
  }

//--------------------------- operator +=  -------------------------------------
  PtrVector& PtrVector::operator+=(const PtrVector& vector) {
      append(vector);
      return *this;
  }

//------------------------------ operator [] ----------------------------------
  void *PtrVector::operator[] (const int elem) const {
      assert ( elem >= 0 && elem < _len );
      return (void *)_ptn[elem];
  }


//-----------------------------------  length  -------------------------------
  int PtrVector::length() const {
      return _len;
  }

//--------------------------- out -------------------------------------------

  void PtrVector::out() const {

      for ( int i=0; i<_len; i++) {
          cout << *((int *)_ptn+i) << endl;
      }
  }

//--------------------------  protected functions  ---------------------------
//------------------------------------------------------------------------------
//---------------------------  copy  -----------------------------------------
  void PtrVector::copy(const PtrVector& vector) {
      if (_ptn > 0) { delete [] _ptn; }

      _size = vector._size;
      _incr = vector._incr;
      _len  = vector._len;

      if ( _len <= 0 ) { _ptn=0; return;}

      _ptn = new void*[_size];
      assert(_ptn != 0);

      for ( int i=0; i < _len; i++ ) {
         *(_ptn+i) = *(vector._ptn+i);
      }

  }
//--------------------------- add  -------------------------------------

  void PtrVector::add(void *object) {
 
     if ( _len >= _size ) {
        void **ptnnew = new void*[_size+_incr];
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
  void PtrVector::append(const PtrVector& vector) {
      void **ptnew;
      int sizenew,lennew;
//
      if ( vector._size <= 0 ) {return;}
//
      if (_size <= 0) { copy(vector); return; }

      lennew = _len + vector._len ;  
      sizenew = lennew ;  

//
      ptnew = new void*[sizenew];
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
/*
main()
{
	PtrVector arr;
	int i = 123;
	char str[] = "hello world";
	float f = 12.34;

	arr += (void *)&i;
	arr += (void *)str;
	arr += (void *)&f;

	cout << *(int *)arr[0] << "," << (char *)arr[1] << "," << 
		*(float *)arr[2] << "\n";
}
*/	 
	
