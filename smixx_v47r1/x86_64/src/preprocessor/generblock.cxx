//---------------------------  Class   GenerBlock  -------------------------
//
//                                                 Author : Boda Franek
//                                                 Date : 28 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//--------------------------------------------------------------------------
#include "generblock.hxx"
#include "codeblock.hxx"
//----------------------------------------------------------------------------


//--------------------------  initialise  -------------------------------
   void GenerBlock::
        initialise( const CodeBlock& codeblock, const int inxfirst
                  , int& inxlast){

        for (int i = inxfirst;; i++) {
            if ( codeblock._lines[i].exists("*endgener") ) {
               inxlast = i;
               return;
            } 
            _lines.add(codeblock._lines[i]);
        }
   }


//--------------------------  generate  --------------------------------------

   void GenerBlock::generate( const Block_Name& genlist ) {
       Name star;
       star = "*";

       if ( genlist.numOfEntries() <= 0 ) {return;}

       for ( int ig=0; ig<genlist.numOfEntries(); ig++) {
            Name item;
            item = genlist[ig];
            for ( int i=0; i<_lines.numOfEntries(); i++) {
                Name temp;
                temp = _lines[i];
                if ( temp.exists(star) ) {temp.replace(star,item);}
                _genlines.add(temp);
            }
       }
   } 

//--------------------------------  out  ----------------------------------

   void GenerBlock::out() const {
      _lines.out();
   }

   void GenerBlock::genout() const {
      _genlines.out();
   }
