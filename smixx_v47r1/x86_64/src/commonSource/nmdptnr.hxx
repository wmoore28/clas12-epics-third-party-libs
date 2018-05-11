//----------------------  Class   NmdPtnr  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 15 December 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef NMDPTNR_HH
#define NMDPTNR_HH
#include "smixx_common.hxx"
#include "typedefs.hxx"
#include "name.hxx"

class NmdPtnr {
   public :
      NmdPtnr();

      NmdPtnr( const Name& name, void* ptnr);

      NmdPtnr(const NmdPtnr& obj);

      ~NmdPtnr();

      const Name& name();

      void* pointer();

      NmdPtnr& operator=(const NmdPtnr &);

      void out() const;

      friend ostream& operator<<(ostream&, NmdPtnr& );

      friend bool operator == (const NmdPtnr& , const NmdPtnr&);
//
//
   private :
      Name _name;
      void* _ptnr;
};

#endif
