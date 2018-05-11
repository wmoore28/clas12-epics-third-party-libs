// Created from VECTOR template on  16-DEC-1998 16:45:33.09
//----------------------  Class   PtrVector  -----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef PtrVector_HH
#define PtrVector_HH

class PtrVector {
   public :
      PtrVector();

      PtrVector(int incr);

      PtrVector(const PtrVector& );

      ~PtrVector();

      PtrVector& operator=(const PtrVector &);

      PtrVector& operator+= (const PtrVector &);

      PtrVector& operator+= (void*);

      void *operator[] (const int elem) const;

      int length() const;

      void out() const;
//
//
   protected :
      void copy(const PtrVector &);

      void append(const PtrVector &);

      void add(void*);



      int _len,_size,_incr;
      void **_ptn;
};

#endif
