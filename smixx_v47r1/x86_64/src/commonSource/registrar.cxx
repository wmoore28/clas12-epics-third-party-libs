//----------------------  Class  Registrar  -----------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 25 December 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include <assert.h>
#include "registrar.hxx"
#include "name.hxx"
#include "nmdptnr.hxx"

   Registrar::Registrar() {
   }

   Registrar::~Registrar() {
   }

   int Registrar::add(const Name & name, void* ptnr) {

       NmdPtnr temp(name,ptnr);

       if ( _len == 0 ) {
          NmdPtnrVector::add(temp);
          return 1;
       }

       for ( int i=0; i<_len; i++ ) {
          if ( name == _ptn[i].name() ) {
              _ptn[i] = temp; return 2;
          }
       }
       NmdPtnrVector::add(temp);
       return 1;
   }

   void Registrar::addRegardless(const Name & name, void* ptnr) {
      NmdPtnr temp(name,ptnr);
	  NmdPtnrVector::add(temp);
   }


   void* Registrar::gimePointer(const Name& name) const {

       for ( int i=0; i<_len; i++ ) {
          if ( name == _ptn[i].name() ) {
              return _ptn[i].pointer();
          }
       }

       return 0;
   }


   Name Registrar::gimeName(const int elem) const {
       assert (elem >= 0 && elem < _len);
       return _ptn[elem].name();
   }

   void* Registrar::gimePointer(const int elem) const {
       assert (elem >= 0 && elem < _len);
       return _ptn[elem].pointer();
   }

Name Registrar::gimeName(const void *point) const
{
	for ( int i=0; i<_len; i++ )
	{
		if ( point == _ptn[i].pointer() )
		{
			return _ptn[i].name();
		}
	}
	
	return "";
}	
