//----------------------  Class   TwoNames  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 14-January-1997
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "twonames.hxx"
#include "smixx_common.hxx"
#include <string.h>
//
//
//-----------------------   Constructors -------------------------------------
  TwoNames::TwoNames() {
      _name1 = " ";
      _name2 = " ";
   }

  TwoNames::TwoNames(const char* str1, const char* str2){
      _name1 = str1;
      _name2 = str2;
  }

  TwoNames::TwoNames(const TwoNames& a){
      _name1 = a._name1;
      _name2 = a._name2;
  }


//--------------------------- operators -------------------------------------

  TwoNames& TwoNames::operator=(const TwoNames& a) {
      _name1 = a._name1;
      _name2 = a._name2;

      return *this;
  }


//---------------------------  set   --------------------------------------

  void TwoNames::set(const char* str1, const char* str2){
      _name1 = str1;
      _name2 = str2;
  }

//---------------------------  set   --------------------------------------

  void TwoNames::set(const Name& str1, const Name& str2){
      _name1 = str1;
      _name2 = str2;
  }
//--------------------------  whatAreYou ------------------------------------

  void TwoNames::whatAreYou
       (char* str1, const int mxsize1, char* str2, const int mxsize2) const {
      _name1.whatAreYou(str1,mxsize1);
      _name2.whatAreYou(str2,mxsize2);
  }

//--------------------------  gimeNames -------------------------------------
  void TwoNames::gimeNames(Name& name1, Name& name2) const {
      name1 = _name1;
      name2 = _name2;
  }


//--------------------------- out -------------------------------------------

  void TwoNames::out() const {
      cout << "(\n";
      _name1.out();
      _name2.out();
      cout << ")\n";
      cout.flush();
  }

