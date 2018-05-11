//----------------------  Class   NmdPtnr  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 15 December 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "nmdptnr.hxx"

  NmdPtnr::NmdPtnr() {
    _name = " ";
    _ptnr = 0;
  }


  NmdPtnr::NmdPtnr( const Name& name, void* ptnr) {
     _name = name;
     _ptnr = ptnr;
  }

  NmdPtnr::NmdPtnr(const NmdPtnr& obj) {
     _name = obj._name;
     _ptnr = obj._ptnr;
  }

  NmdPtnr::~NmdPtnr() {}

  NmdPtnr& NmdPtnr::operator=(const NmdPtnr& obj) {
     _name = obj._name;
     _ptnr = obj._ptnr;
     return *this;
  }

  const Name& NmdPtnr::name() {
     return _name;
  }

  void* NmdPtnr::pointer() {
     return _ptnr;
  }

  void NmdPtnr::out() const {
     cout << _name << "  " << _ptnr << endl; 
  }

  ostream& operator<<(ostream& os, NmdPtnr& obj) {
     os << obj._name << "  " << obj._ptnr << endl;
     return os;
  }

  bool operator == (const NmdPtnr& itm1, const NmdPtnr& itm2) {
     if ( itm1._name == itm2._name ) {return 1;}
     return 0;
  }
