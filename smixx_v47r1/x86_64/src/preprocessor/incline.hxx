//----------------------  Class   IncLine  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 18 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef INCLINE_HH
#define INCLINE_HH
#include "typedefs.hxx"
#include "block_name.hxx"

class IncLine {
   public :

      IncLine( const char line[]);

      IncLine( const Name& line);
      
      void filename( Name& fnm)   const;
      
      int parnum()  const;
      
      void parameter(int inx, Name& name, Name& val )  const;

      void out() const;
//
//
   private :

      void construct( const char line[]);
      Name _filename;
      int _numOfPars;
      Block_Name _parNames,_parValues;
};

#endif
