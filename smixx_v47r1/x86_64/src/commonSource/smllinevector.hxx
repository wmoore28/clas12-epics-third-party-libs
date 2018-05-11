// Created from VECTOR template on  16-DEC-1998 14:17:30.42
//----------------------  Class   SMLlineVector  -----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef SMLlineVector_HH
#define SMLlineVector_HH

class SMLline;


class SMLlineVector {
   public :
      SMLlineVector();

      SMLlineVector(int incr);

      SMLlineVector(const SMLlineVector& );

      ~SMLlineVector();

      SMLlineVector& operator=(const SMLlineVector &);

      SMLlineVector& operator+= (const SMLlineVector &);

      SMLlineVector& operator+= (const SMLline &);

      SMLline operator[] (const int elem) const;

      int length() const;

      void out(char* offset ="") const;

	  void out(const int i, char* offset ="") const;
//
//
   protected :
      void copy(const SMLlineVector &);

      void append(const SMLlineVector &);

      void add(const SMLline &);



      int _len,_size,_incr;
      SMLline *_ptn;
};

#endif
