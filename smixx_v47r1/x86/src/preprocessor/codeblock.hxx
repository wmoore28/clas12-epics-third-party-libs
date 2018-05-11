//----------------------  Class   CodeBlock  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 18 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef CODEBLOCK_HH
#define CODEBLOCK_HH
#include "typedefs.hxx"
#include "block_name.hxx"
#include "incline.hxx"
#include "generblock.hxx"
#include "macro.hxx"

class IncFile;
class Registrar;
class Name;
//-----------------------------------------------------------------------------

class CodeBlock {
   public :
   
      CodeBlock();

      CodeBlock( const Name& filename);
      
      void makeNextBlock( CodeBlock& nextBlock );
      
      void generPass( CodeBlock& nextBlock );
      
      void objectSetPass( CodeBlock& nextBlock );
      
      void macroPass( CodeBlock& nextBlock ) const ;
      
      int numOfLines() const;
      
      bool anyIncludes();  
      
      void append(IncFile& incfile); //will append the infile to the block
      
      void append(GenerBlock& genbl); //will append the generated block

      void out();
      
      void outToFile(const Name& filenm) const;

      friend void GenerBlock::initialise
                  ( const CodeBlock& codeblock, const int inxfirst
                  , int& inxlast);

      friend void Macro::realiseMacro
        ( const Name& line, const Registrar& macros,CodeBlock& codeBlock) const;

//
//
   private :
      void expandLine
          (const Name& line, const Registrar& objectSets, CodeBlock&);

      int  _hasIncludes, _pass;
      Block_Name _lines;
};

#endif
