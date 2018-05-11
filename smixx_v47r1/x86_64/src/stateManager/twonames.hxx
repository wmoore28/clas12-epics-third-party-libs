//----------------------  Class   TwoNames  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 14-January-1997
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef TWONAMES_HH
#define TWONAMES_HH

#include "name.hxx"

class TwoNames {
   public :
      TwoNames();

      TwoNames(const char* str1, const char* str2);

      TwoNames(const TwoNames& );

      TwoNames& operator=(const TwoNames &);

//      ~TwoNames();   not needed

      void set(const char* str1, const char* str2);

      void set(const Name& str1, const Name& str2);

      void whatAreYou
          (char* str1, const int mxsize1, char* str2, const int mxsize2) const;

      void gimeNames( Name& name1, Name& name2) const;

      void out() const;
//
//
   private :
      Name _name1,_name2;
};

#endif

