//----------------------  Class   Macro  --------------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 29 December 1998
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include <stdlib.h>
#include "macro.hxx"
#include "codeblock.hxx"
   void expandMacro(const Name& macroline, const Registrar& macros,
                     CodeBlock& nextBlock);
   bool macroLine(const Registrar&, Name& );

   Macro::Macro() {
   }

   Macro::Macro(const Name& firstLine) {
//     .macro  NAME(par1,par2,...,parn)

      Name namepars;     // NAME(par1,par2,...,parn)

      Name temp;
      if ( !firstLine.element(1,'.',temp) ) {   // macro NAME(
          cout << " Macro1:  error parsing:" << endl << firstLine<< endl; 
          cout.flush();
          exit(2);
      }

      namepars = &temp[5];

      namepars.squeeze();  


      if ( !namepars.element(0,'(',_name) ) {
          cout << " Macro2:  error parsing:" << endl << firstLine<< endl; 
          cout.flush();
          exit(2);
      }

      _name.squeeze(); 

      Name parsbrak;     // par1,par2,...,parn)

      if ( !namepars.element(1,'(',parsbrak) ) {
          cout << " Macro3:  error parsing:" << endl << firstLine<< endl; 
          cout.flush();
          exit(2);
      }

      Name pars;     // par1,par2,...,parn

      if ( !parsbrak.element(0,')',pars) ) {
          cout << " Macro4:  error parsing:" << endl << firstLine<< endl; 
          cout.flush();
          exit(2);
      }

      if ( pars.length() > 1 ) { // If no params do nothing
         for ( int i=0; ; i++) {
            Name par;
            if ( !pars.element(i,',',par) ) {
               break;
            }
            par.squeeze(); 
            _parNames.add(par);
         }
      }
   }
   
//--------------------------------------------------------------------------
   void Macro::addLine(const Name& line) {
       _body.add(line);
   }

//---------------------------------------------------------------------------
   Name Macro::name()  const {
       return _name;
   }

//--------------------------------------------------------------------------
   void Macro::realiseMacro( const Name& macroline,  const Registrar& macros,
                             CodeBlock& codeBlock) const {

      char TAB = '\t';
      Name offset = "\0";

      for (int ii=0;;ii++) { 
         if ( macroline[ii]==' ' ) { offset += " "; }
         else if ( macroline[ii]==TAB ) { offset += "\t"; }
         else { break; }
      } 

      Name parsbrak;     // par1,par2,...,parn)

      if ( !macroline.element(1,'(',parsbrak) ) {
          cout << " Macro1:  error parsing:" << endl << macroline<< endl; 
          cout.flush();
          exit(2);
      }

      Name pars;     // par1,par2,...,parn

      if ( !parsbrak.element(0,')',pars) ) {
          cout << " Macro2:  error parsing:" << endl << macroline<< endl; 
          cout.flush();
          exit(2);
      }

      Block_Name parValues;

      if ( pars.length() > 1 ) { // If no params do nothing
         for ( int i=0; ; i++) {
            Name par;
            if ( !pars.element(i,',',par) ) {
               break;
            }
            par.squeeze(); 
            parValues.add(par);
         }
      }

      int numpar = _parNames.numOfEntries();

      if ( parValues.numOfEntries() != numpar ) {
          cout << " macroline : " << macroline << endl
               << " the macro invocation has incompatible number of arguments"
               << endl;
          exit(2);
      }

      int numlines = _body.numOfEntries();

      for ( int il=0; il<numlines; il++ ) {
          Name line = offset; line += _body[il];         
          for ( int ip=0; ip<numpar; ip++ ) {
              Name parname = "${";
              parname += _parNames[ip]; parname += "}";
              line.replace(parname,parValues[ip]);
          }
          if ( macroLine(macros,line) ) {
                 Name newmacroline = line;
                 if ( line.exists("\\") ) {
                     for ( ;;il) {
                        newmacroline += _body[il+1];
                        if ( !_body[il+1].exists("\\") ) {il++;break;}
                     }
                     newmacroline.replace("\\","");
                 }                    
                 expandMacro(newmacroline,macros,codeBlock);
                 continue;
          }
          codeBlock._lines.add(line);
      } 
   }

//--------------------------------------------------------------------------
   void Macro::repeatMacro( const Name& macroline,  const Registrar& macros,
                            CodeBlock& codeBlock) const {

      int numpar = _parNames.numOfEntries();
      if (numpar != 1) {
         cout << macroline << endl
              << " only macros with 1 argument can be used in repeat_macro"
              << endl; exit(2);
      }

      Name begline; //   .repeat NAME

      macroline.element(0,'(',begline);

      Name parsbrak;     // par1,par2,...,parn)

      if ( !macroline.element(1,'(',parsbrak) ) {
          cout << " Macro1:  error parsing:" << endl << macroline<< endl; 
          cout.flush();
          exit(2);
      }

      Name pars;     // par1,par2,...,parn

      if ( !parsbrak.element(0,')',pars) ) {
          cout << " Macro2:  error parsing:" << endl << macroline<< endl; 
          cout.flush();
          exit(2);
      }

//cout << "pars " << pars  << endl;
      pars.squeeze();
      if ( pars == "\0") {return;}

      Block_Name parValues;

      for ( int i=0; ; i++) {
         Name par;
         if ( !pars.element(i,',',par) ) {
            break;
         }
//cout << "par " << par  << endl;
         par.squeeze(); 
         Name line = begline;
         line += "(";
         line += par;
         line += ")";
         realiseMacro( line, macros,codeBlock);
      }

   }

//--------------------------------------------------------------------------
   void Macro::out() const {
       cout << "Macro : " << _name << endl;
       _parNames.out();
       _body.out();
   }
