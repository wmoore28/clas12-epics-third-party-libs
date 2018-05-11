//----------------------  Class   IncFile  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 18 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef INCFILE_HH
#define INCFILE_HH
#include "typedefs.hxx"
#include "block_name.hxx"
#include "incline.hxx"
#include "codeblock.hxx"


class IncFile {
   public :

      IncFile( const int pass, const IncLine& incline);
      
      bool anyIncludes();
      
      friend void CodeBlock::append(IncFile& incfile);

      void out();
//
//
   private :
      int  _hasIncludes;
      Block_Name _lines;
};

#endif
