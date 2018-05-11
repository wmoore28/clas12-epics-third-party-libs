//--------------------------  SMIClass Class  ----------------------------------
// $Id
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
#include "smiclass.hxx"
#include "namevector.hxx"
#include "name.hxx"
#include "registrar.hxx"
extern int dbg;
extern  Registrar allSMIClasses;
//                                                         26-August-1996
//                                                          B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
SMIClass::SMIClass( char lines[][MAXRECL])
{

    _name = lines[1];

   if ( dbg > 1 ) {
       cout << " Class ..........   <" << _name << ">" << endl;;
   }
   
   allSMIClasses.add(_name,this);
   int lin = 0;

   for ( ;; ) {
      Name temp = lines[lin];
     _classCode += temp;
     lin++;
     if ( temp == "*END_CLASS") {break;}
   }

}


int SMIClass::numOfLines() {
   return _classCode.length();
}


void SMIClass::gimeLine(int lin,char* line) {
   assert ( lin < _classCode.length() );
   Name temp = _classCode[lin];
   temp.whatAreYou(line,MAXRECL);
   return;
}



void SMIClass::whatAreYou() {

   cout << " Class : " << _name << "   Lines : " << _classCode.length() << "\n";

   Name temp; char line[MAXRECL];

   for (int lin=0; lin<_classCode.length(); lin++) {
      temp = _classCode[lin]; temp.whatAreYou(line,MAXRECL);
      cout << " " << line << "\n";
   }
   cout.flush();
   return;
}

//=======================================================================
Name& SMIClass::name() {
    return _name;
}
