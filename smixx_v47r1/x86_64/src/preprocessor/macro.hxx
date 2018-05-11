//----------------------  Class   Macro  --------------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 28 December 1998
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef MACRO_HH
#define MACRO_HH
#include "name.hxx"
#include "block_name.hxx"
class CodeBlock;
class Registrar;

class Macro {
   public :

      Macro();

      Macro(const Name& firstLine);

      void addLine(const Name& line);

      Name name() const;

      void realiseMacro( const Name& line, const Registrar& macros,
                         CodeBlock& codeBlock) const;

      void repeatMacro( const Name& line, const Registrar& macros,
                         CodeBlock& codeBlock) const;

      void out() const;
//
//
   private :
      Name _name;
      Block_Name _parNames;
      Block_Name _body;
};

#endif
