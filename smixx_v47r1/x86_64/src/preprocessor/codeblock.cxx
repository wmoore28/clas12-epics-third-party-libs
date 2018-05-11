//----------------------------- Class  CodeBlock -----------------------------
//
//                                                Author : Boda Franek
//                                                Date : 20 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
#include "codeblock.hxx"
#include "generblock.hxx"
#include "incline.hxx"
#include "utilities.hxx"
#include "incfile.hxx"
#include "registrar.hxx"
#include "macro.hxx"
   void expandMacro(const Name& macroline, const Registrar& macros,
                     CodeBlock& nextBlock);
   bool macroLine(const Registrar& macros, Name& line);
//----------------------------------------------------------------------------

//----------------------------- Constructors ----------------------------------
   CodeBlock::CodeBlock() {
      _hasIncludes = 0;
      _pass = 0;
      return;
   }
   

   CodeBlock::CodeBlock( const Name& filename) {
//       cout << " CodeBlock from  File name :|" << filename << "|" << endl;
       
       

       ifstream input_file(&filename[0]);

       if (input_file.bad()) {
          cout << " CodeBlock Could not open file :" << filename << endl;
          exit(2);
       }

       Name line;
       _hasIncludes = 0;
       _pass = 0;

       for (;;) {
          int istat = readLine(input_file,line);
          
          if ( istat == 2 ) {
             input_file.close();
             return;
          }
          else if ( istat == 0 ) {
             cout << "Error reading file\n";
             cout.flush();
             exit(2);
          }

          line.trim();
          _lines.add(line);
          
          if (line.exists("include")) {
             Name pom;
             line.element(0,'\'',pom); pom.squeeze();
             if ( pom == "include" ) { _hasIncludes = 1; }
          }

       }
       return;
   }
   

//--------------------------  makeNextBlock  ------------------------------
   
   void CodeBlock::makeNextBlock( CodeBlock& nextBlock ) {
       

       nextBlock._hasIncludes = 0;
       nextBlock._pass = _pass + 1;
       
       int num = numOfLines();

       int numOfIncludes = 0;
       
       for ( int i=0; i<num; i++) {
          
          if (_lines[i].exists("include")) {
             Name pom;
             _lines[i].element(0,'\'',pom); pom.squeeze();
             if ( pom == "include" ) { 
                numOfIncludes++;
                IncLine incline(_lines[i]);
                IncFile incFileBlock(_pass,incline);
                if(incFileBlock.anyIncludes()) {nextBlock._hasIncludes = 1;}
                nextBlock.append(incFileBlock);
                continue;
             }
          }
          nextBlock._lines.add(_lines[i]);
       }
       return;
   }
   

//--------------------------  generPass  ------------------------------
   
   void CodeBlock::generPass( CodeBlock& nextBlock ) {
       Block_Name genlist;
       Name linenm;       

       nextBlock._hasIncludes = 0;
       nextBlock._pass = _pass + 1;
       
       int num = numOfLines();
       
       for ( int i=0; i<num; i++) {
           if ( _lines[i].exists("*genlist") ) {
              for ( int j = i+1;; j++) {
                   if ( _lines[j].exists("*endgenlist") ) { i=j; break;}
                   if ( _lines[j][0] != '!' ) {genlist.add(_lines[j]);}
              }
              continue;
           }

           if ( _lines[i].exists("*gener") ) {
               GenerBlock genbl;
               genbl.initialise(*this,i+1,i);
               genbl.generate(genlist);
               nextBlock.append(genbl);
//               genbl.out();
//               cout << " "<< endl;
//               genbl.genout();
           }          
           else {
              nextBlock._lines.add(_lines[i]);
           }
       }
//       genlist.out();
       return;
   }
   

//-------------------------- objectSetPass  ------------21-April-1998----------
//
// This is a temporary fix untill the time when Object_set becomes SML feature
//  
// at the moment allowing just 1 set 

   void CodeBlock::objectSetPass( CodeBlock& nextBlock ) {
       Registrar objectSets;
       Name setName;
       Name className;       

       nextBlock._hasIncludes = 0;
       nextBlock._pass = _pass + 1;
       
       int num = numOfLines();

       Name pomline;
       
       for ( int i=0; i<num; i++) {
          pomline = _lines[i];
          Name offset = "\0";
          for ( int bl=0; (pomline[bl]!= '\0')&&(pomline[bl]==' ') ; bl++) {
              offset += " ";
          }
          process_line(&pomline[0]);

          char save;
          char *tempb, *tempe;


          if ( strstr(&pomline[0],"OBJECT_SET") == &pomline[0] ) {
                tempb = strstr(&pomline[0],":");
                tempb = tempb +1;
                tempe = strstr(tempb," ");
                save = *tempe;
                *tempe = '\0';
                setName = tempb;
                Block_Name* pObjectList = new Block_Name;
                int flg = objectSets.add(setName,pObjectList);
                if ( flg==2 ) {
                    cout << " Object Set : " << setName
                         << " has already been declared " << endl; cout.flush();
                    exit(2);
                }
  //              cout << "setName : " << setName << endl;
                *tempe = save;
                tempb = strstr(&pomline[0],"IS_OF_CLASS");
                tempb = tempb +12;
                tempe = strstr(tempb,"=");
                *tempe = '\0';
                className = tempb;
                for ( int j = i+2;; j++) {
                   if ( _lines[j][0] == ']' ) { i=j; break; }
                   pObjectList->add(_lines[j]);                    
                   Name pomname = "Object : ";
                   pomname += _lines[j];
                   pomname += " is_of_class ";
                   pomname += className;
                   nextBlock._lines.add(pomname);
                }
                continue;
             }
             else if (     (     strstr(&pomline[0],"(ANY_OF_") 
                              || strstr(&pomline[0],"(ALL_OF_")
                           )
                       &&  (strstr(&pomline[0],"DO") != &pomline[0]) ) {
                expandLine(_lines[i],objectSets,nextBlock);
                continue;
             }
             else if ( (strstr(&pomline[0],"DO") == &pomline[0]) &&
                       (tempb = strstr(&pomline[0],"ALL_OF_"))  ) {
                tempb = tempb + 7;
                Name tempname = tempb;
                void* ptn = objectSets.gimePointer(tempname);
                if ( ptn ) {
                   Block_Name* pObjectList = (Block_Name*)ptn;
                   *(tempb-7) = '\0';
                   Name beg_of_the_line = offset;
                   beg_of_the_line += &pomline[0];
                   beg_of_the_line += " ";
                   int nnn = pObjectList->numOfEntries();
                   for ( int i2=0; i2<nnn; i2++) {
                       Name pomname1 = beg_of_the_line;
                       pomname1 += (*pObjectList)[i2];
                       nextBlock._lines.add(pomname1);
                   }
                   continue;
                }
                else {
                    cout << " Undeclared Object set " << tempname << endl;
                    cout.flush();
                    exit(2);
                }
             }
             nextBlock._lines.add(_lines[i]);
       }
//       pObjectList->out();
       return;
   }

//--------------------------------------------------------------------------
   void CodeBlock::expandLine
       (const Name& linenm, const Registrar& objectSets, CodeBlock& nextBlock) {
// xxxxxxxxx( any_of_name in_state SSS )yyyyyyyyyyyy
      Name line = linenm;

      char* pline = &line[0];
      char* ptstart = pline;

      for ( int i=0;;i++) {
          char* ptbrak = strchr(ptstart,'(');
// xxxxxxxxx( any_of_name in_state SSS )yyyyyyyyyyyy
//          |
          *ptbrak = '\0';
          int noffset = strlen(pline);
          Name linebeg = pline; linebeg += "(";   // xxxxxxxxx(
          Name offset = "\0";
          for (int ix=0; ix<noffset; ix++) { offset += " "; }
          *ptbrak = '(';

          char* ptany = strtok(ptbrak+1," ");         //****
// xxxxxxxxx( any_of_name in_state SSS )yyyyyyyyyyyy
//            |
          Name name = ptany + 7;
          *(ptany+7) = '\0';                         //*****

          Name anyOf = ptany; anyOf.upCase();
          Name andor = offset;
//          if ( !strcasecmp(ptany,"any_of_") ) {
          if ( anyOf == "ANY_OF_" ) {
				andor += "   or ( ";             
          }
//          else if ( !strcasecmp(ptany,"all_of_") ) {
          else if ( anyOf == "ALL_OF_" ) {
		        andor += "  and ( ";             
          }
          else { 
              *(ptany+7)=name[0]; 
              *(ptany+strlen(ptany))=' ';          
              ptstart = ptbrak+1;
              continue;
          }
           *(ptany+7)=name[0]; 
           ptstart = ptany + strlen(ptany);
           *ptstart = ' ';          
// xxxxxxxxx( any_of_name in_state SSS )yyyyyyyyyyyy
//                       |
           ptbrak = strchr(ptstart,')');
           *ptbrak = '\0';
           Name rest_of_the_cond = ptstart;    // in_state SSS
          rest_of_the_cond += ")";
          *ptbrak = ')';
          Name rest_of_the_line = ptbrak;

          void* ptn = objectSets.gimePointer(name);
          if ( ptn ) {
              Block_Name* pObjectList = (Block_Name*)ptn;

              Name newline = linebeg;
              newline += "     ( ";
              newline += (*pObjectList)[0];
              newline += rest_of_the_cond;
//              cout << newline << endl; cout.flush();
              nextBlock._lines.add(newline);

              int numlist = pObjectList->numOfEntries();
              if ( numlist > 1 ) {
                 for ( int ii=1; ii<numlist; ii++) {
                     newline = andor;
                     newline += (*pObjectList)[ii];
                     newline += rest_of_the_cond;
                     nextBlock._lines.add(newline);
//              cout << newline << endl; cout.flush();
                 }
              }
              newline = offset;
              newline += rest_of_the_line;
              nextBlock._lines.add(newline);
//              cout << newline << endl; cout.flush();
          }
          else {
              cout << " Undeclared Object set " << name << endl; cout.flush();
              exit(2);
          }

          break;
      }
   }
//--------------------------------- macroPass ------------------------------

   void CodeBlock::macroPass( CodeBlock& nextBlock ) const {
       Registrar macros;

       nextBlock._hasIncludes = 0;
       nextBlock._pass = _pass + 1;
       
       int num = numOfLines();
       int collectingMacro = 0;

       Macro* pMacro;
       
       for ( int i=0; i<num; i++) {
          if ( _lines[i].exists(".") ) {
              Name pom, keyword;
              _lines[i].element(0,'.',pom); pom.squeeze();
              if ( pom.length() == 1 ) {
                 Name temp;
                 _lines[i].element(1,'.',temp); temp.element(0,' ',keyword);
              }    
              else { keyword = "none"; }
              if ( keyword == "macro" ) {
                 if ( collectingMacro == 0 ) { collectingMacro = 1;}
                 else {
                      cout << " Line : " << _lines[i] << endl;
                      cout << "  Macro declaration inside macro declaration"
                           << endl;
                      exit(2);
                 }
                 pMacro = new Macro(_lines[i]);
                 Name macroname = pMacro->name();
                 int flg = macros.add(macroname,pMacro);
                 if ( flg == 2 ) {
                    cout << " Warning!! Macro : " << macroname <<
                         " redefined" << endl;
                 }
                 continue;
              }
              else if ( keyword == "endmacro" ) {
                 if ( collectingMacro == 1 ) { collectingMacro = 0;}
                 else {
                      cout << " Line : " << _lines[i] << endl;
                      cout << "  endmacro   without corresp macro"
                           << endl;
                      exit(2);
                 }
                 continue;
              }
              else {   
              }
          }
          if ( collectingMacro == 1 ) { pMacro->addLine(_lines[i]); continue;}


          if ( macroLine(macros,_lines[i]) ) {
                 Name macroline = _lines[i];
                 if ( _lines[i].exists("\\") ) {
                     for ( ;;i++) {
                        macroline += _lines[i+1];
                        if ( !_lines[i+1].exists("\\") ) {i++;break;}
                     }
                     macroline.replace("\\","");
                 }                    
                 expandMacro(macroline,macros,nextBlock);
                 continue;
          }
          nextBlock._lines.add(_lines[i]);
      }
      if ( collectingMacro != 0 ) {
         cout << " Missing  .endmacro " << endl; cout.flush();
         exit(2);
      }

      
   }

//--------------------------------------------------------------------------
   bool macroLine(const Registrar& macros,Name& line) {

          int macroExists = 0;
          char* ptbrak = strstr(&line[0],"(");
          if (ptbrak) {
             *ptbrak = '\0';
             Name possibleMacro = &line[0];
             *ptbrak = '(';
             possibleMacro.squeeze();
             if ( macros.gimePointer(possibleMacro) ) {macroExists = 1;}
          }

          if ( line.exists(".repeat") || macroExists ) { return 1;}
          return 0;
   }
//------------------------------  numOfLines  ------------------------------   
   
   int CodeBlock::numOfLines() const {
       return _lines.numOfEntries();
   }


//-----------------------------  anyIncludes  -------------------------------   
   
   bool CodeBlock::anyIncludes(){
       return _hasIncludes;
   }
   

//----------------------------  append  --------------------------------------
   
   void CodeBlock::append(IncFile& incfile) {
       _lines.append(incfile._lines);
   } 
   
   void CodeBlock::append(GenerBlock& genbl) {
       _lines.append(genbl._genlines);
   } 


//-----------------------------  out  ------------------------------------------

   void CodeBlock::out(){
      _lines.out();
   }

//----------------------------  outToFile -------------------------------------
   
   void CodeBlock::outToFile(const Name& filenm) const {
   
       Name lineout;
   
       ofstream out_file(&filenm[0]);
   
       if (!out_file) {
          cout << "  CodeBlock::outToFile   can not open file" << endl
               << filenm << endl;
          exit(2);
       }
       
       for ( int i=0; i<numOfLines(); i++) {
           lineout = _lines[i];
           out_file << &lineout[0] << endl;
       }
       
   }
//
//--------------------  expand Macro  --------------------------------------
   void expandMacro(const Name& macroline, const Registrar& macros,
                     CodeBlock& nextBlock) {
        Name macroname;

        if ( macroline.exists(".repeat" ) ) {   // .repeat
                Name pom1; macroline.element(1,'.',pom1); //repeat
                Name pom2; pom1.element(1,' ',pom2); pom2.squeeze(); //  Name(
                if ( pom2.element(0,'(',macroname) ) {
                   void* ptn = macros.gimePointer(macroname);
                   if (!ptn) {
                      cout << " Macro : " << macroname << " was not declared"
                           << endl; cout.flush();
                      exit(2);
                   }
                   Macro* pMac = (Macro*)ptn;
                   pMac->repeatMacro(macroline,macros,nextBlock);
                }
                else {
                   cout << " Line : " << macroline 
                        << " invalid syntax " << endl;
                   exit(2);
                }
        }
        else {                         // ordinary macro:  Name()
                if (macroline.element(0,'(',macroname)) {
                    macroname.squeeze();
                    void* ptn = macros.gimePointer(macroname);
                    if (!ptn) {
                       cout << " Macro : " << macroname << " was not declared"
                            << endl; cout.flush();
                       exit(2);
                    }
                    Macro* pMac = (Macro*)ptn;
                    pMac->realiseMacro(macroline,macros,nextBlock);
                }
                else {
                   cout << " Line : " << macroline 
                        << " invalid syntax " << endl;
                   exit(2);
                }
        }
   }
