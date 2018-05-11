// Created from VECTOR template on  15-DEC-1998 12:38:24.95
//----------------------  Class   NameVector  -----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef NameVector_HH
#define NameVector_HH
#include "typedefs.hxx"
class Name;


class NameVector {
   public :
      NameVector();

      NameVector(int incr);

      NameVector(const NameVector& );

      ~NameVector();

      NameVector& operator=(const NameVector &);

      NameVector& operator+= (const NameVector &);

      NameVector& operator+= (const Name &);

      Name& operator[] (const int elem) const;

      int length() const;

      void out() const;

	void out(const Name offSet) const;
	
	bool isPresent(const Name& name) const;

	NameVector& exclusiveUpdate(const NameVector& vector);
//
//
   protected :
      void copy(const NameVector &);

      void append(const NameVector &);

      void add(const Name &);



      int _len,_size,_incr;
      Name *_ptn;
};

#endif
