// Created from VECTOR template on  16-DEC-1998 16:45:33.09
//----------------------  Class   NmdPtnrVector  -----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 24 July 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef NmdPtnrVector_HH
#define NmdPtnrVector_HH

class NmdPtnr;


class NmdPtnrVector {
   public :
      NmdPtnrVector();

      NmdPtnrVector(int incr);

      NmdPtnrVector(const NmdPtnrVector& );

      ~NmdPtnrVector();

      NmdPtnrVector& operator=(const NmdPtnrVector &);

      NmdPtnrVector& operator+= (const NmdPtnrVector &);

      NmdPtnrVector& operator+= (const NmdPtnr &);

      NmdPtnr operator[] (const int elem) const;

      int length() const;

      void out() const;
//
//
   protected :
      void copy(const NmdPtnrVector &);

      void append(const NmdPtnrVector &);

      void add(const NmdPtnr &);



      int _len,_size,_incr;
      NmdPtnr *_ptn;
};

#endif
