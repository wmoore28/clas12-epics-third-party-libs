//----------------------  Class  Registrar  -----------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 25 December 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef REGISTRAR_HH
#define REGISTRAR_HH
#include "typedefs.hxx"
#include "nmdptnrvector.hxx"
class Name;

class Registrar : public NmdPtnrVector {
   public :

      Registrar();

      virtual ~Registrar();

      int add(const Name &, void* ptnr); 

      void addRegardless(const Name &, void* ptnr); 

      void* gimePointer(const Name &)  const;

      void* gimePointer(const int elem)  const;

      Name gimeName(const int elem)  const;
      
      Name gimeName(const void *ptn) const;
};

#endif
