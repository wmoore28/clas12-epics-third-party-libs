//----------------------  Class   GenerBlock  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 26 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef GENERBLOCK_HH
#define GENERBLOCK_HH
#include "typedefs.hxx"
#include "block_name.hxx"
//----------------------------------------------------------------------------

class CodeBlock;

class GenerBlock {
   public :

      void initialise( const CodeBlock& codeblock, const int inxfirst
                     , int& inxlast);
      
      void generate( const Block_Name& genlist);

      void out() const;

      void genout() const;

      friend class CodeBlock;

//
//
   private :
      Block_Name _lines, _genlines;
};

#endif
