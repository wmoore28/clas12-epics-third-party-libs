//----------------------  Class   IncFile  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 18 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include <stdlib.h>
#include "smixx_common.hxx"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "utilities.hxx"
#include "incfile.hxx"
#include "incline.hxx"
//-----------------------------------------------------------------------------

//-------------------------  Constructors  -----------------------------------

   IncFile::IncFile( const int pass, const IncLine& incline) {
       Name filenm;

       incline.filename(filenm);
       int prnum = incline.parnum();

//       cout << " IncFile pass : " << pass << " " << filenm  << endl;


       ifstream input_file(&filenm[0]);

       if (input_file.bad()) {
          cout << " IncFile Could not open file :" << filenm << endl;
          incline.out();
          exit(2);
       }

//       cout << " Opened : " << filenm << endl;

       _hasIncludes = 0;

       _lines.add("!");
       Name infoline;
       
       char cpass[10];
       sprintf(cpass,"%d : ",pass+1);
       infoline = "!  beg include Level ";
       infoline += cpass;
       infoline += filenm;

       _lines.add(infoline);
       if ( prnum > 0 ) {_lines.add("!");}
       

       Name line;

       for (;;) {
          int istat = readLine(input_file,line);

          if ( istat == 2 ) {
             input_file.close();
             _lines.add("!");
       
             infoline = "!  end include Level ";
             infoline += cpass;
             infoline += filenm;
             _lines.add(infoline); 
             return;
          }
          else if ( istat == 0 ) {
             cout << "Error reading file\n";
             cout.flush();
             exit(2);
          }

          line.trim();

          if (line.exists("include")) {
             Name pom;
             line.element(0,'\'',pom); pom.squeeze();
             if ( pom == "include" ) { _hasIncludes = 1; }
          }

          if (prnum > 0 && line.exists("&") ) {
             for (int ip=0; ip<prnum; ip++) {
                 Name prname, prval;
                 incline.parameter(ip,prname,prval);
                 if (line.exists(prname)){
                    line.replace(prname,prval);
                 }  
            }
          }
          _lines.add(line);

       }
       return;

   }
   

//-------------------------------  anyIncludes  -------------------------------
   bool IncFile::anyIncludes(){
       return _hasIncludes;
   }


//------------------------------  out  -----------------------------------------
   void IncFile::out(){
      _lines.out();
   }
