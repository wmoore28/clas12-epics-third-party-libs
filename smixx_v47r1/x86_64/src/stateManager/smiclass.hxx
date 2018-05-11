
//---------------------- SMIClass   Class ---------------------------
#ifndef SMICLASS_HH
#define SMICLASS_HH
#include "parameters.hxx"
#include "name.hxx"
#include "namevector.hxx"
//                                                          26-August-1996
//                                                            B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//--------------------------------------------------------------------------


class SMIClass {
public :
  SMIClass( char lines[][MAXRECL]);
  int numOfLines();
  void gimeLine( int lin, char* line);
  void whatAreYou();
    Name& name();
private :

  Name _name;

  NameVector _classCode;

};
#endif
