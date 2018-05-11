//----------------------  Class   IncLine  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 18 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "smixx_common.hxx"
#include "incline.hxx"
#include "utilities.hxx"
//---------------------------------------------------------------------------

//------------------------------  Constructors  ------------------------------

   void IncLine::construct( const char line[] ){

      Name pomline;
      Name parval;

      pomline = line;

      char* pt1, *pt2;

      pt1 = strchr(&pomline[0],'\'');

      if (!pt1) {
         cout << " Include line :" << line << endl
              << " has illegal format " << endl;
         exit(2);
      }

      pt1 = pt1 + 1;

      pt2 = strchr(pt1,'\'');

      if (!pt2) {
         cout << " Include line :" << line << endl
              << " has illegal format " << endl;
         exit(2);
      }

      *pt2 = '\0';

      _filename = pt1;

      pt1 = pt2 + 1;

      _numOfPars = 0;

      for (;;) {
         pt2 = strchr(pt1,'=');
         if(!pt2) { return;}
         parval = "&";
         *pt2 = '\0';
         parval += pt1;
         parval.squeeze();
         parval += "&";
//         cout << parval << endl;
         _parNames.add(parval);
         pt1 = pt2 + 1;
         pt2 = strchr(pt1,',');
         _numOfPars++;
         if ( pt2 ) {
            *pt2 = '\0';
            parval = pt1;
            parval.squeeze();
            _parValues.add(parval); 
            pt1 = pt2 + 1;
         }
         else {
            parval = pt1;
            parval.squeeze();
            _parValues.add(parval); 
            return;
         } 
      }

   }

   IncLine::IncLine( const char line[] ){

       construct(line);
   }

   
   IncLine::IncLine( const Name& line ){
       construct(&line[0]);
   }


//-----------------------------  filename  --------------------------------
   void IncLine::filename(Name& fnm)  const {
       fnm = _filename;
   }
   
//----------------------------  parnum  ------------------------------------

   int IncLine::parnum()  const { return _numOfPars;}
   
//---------------------------  parameter  ----------------------------------

   void IncLine::parameter(int inx, Name& name, Name& val)  const {
       name = _parNames[inx];
       val = _parValues[inx];
   }


//--------------------------  out  ------------------------------------------   

   void IncLine::out() const {
      cout << " Filename : " << _filename << endl;

      cout << " Num of parameters : " << _numOfPars << endl;

      if (_numOfPars > 0) {
        for (int i=0; i<_numOfPars; i++){
          cout << " " << _parNames[i] << " " << _parValues[i] <<  endl;
        }
      }
   }
