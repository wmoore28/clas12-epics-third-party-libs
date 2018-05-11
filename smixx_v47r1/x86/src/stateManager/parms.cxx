
//----------------------  Class  Parms  -----------------------------------
// This class is going eventually to replace all the other parameter clases
//
//                                                      Author: Boda Franek
//                                                      Date : 10 June 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "smixx_common.hxx"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "parms.hxx"
#include "name.hxx"
#include "utilities.hxx"

  const Name noval = "&noval";
  const char undef[] = "&undef";
  const Name undefNm = "&undef";
  const Name typeint = "INT", typefloat = "FLOAT", typestring = "STRING";
//
//
//-----------------------   Constructors -------------------------------------
  Parms::Parms() {
    _num=0;
   }

//------------------------  numOfEntries --------------------------------------
  int Parms::numOfEntries() const {
     return _num;
  }

//------------------------  add  --------------------------------------------
  void Parms::add( const Name& name, const Name& value, const char* type) {
    _num++;


// checks on correctness of declared type    
    if ( value == noval || !strcmp(type,undef) ){ }
    else {
       if ( !strcmp(type,"STRING") ) {} // string can be anything
       else if ( !strcmp(type,"INT") ) {
            if ( !check_int(value) ) {
               cout << " Default value of " << name << " is not INT"<< endl;
               cout.flush();
               abort();
            }
       }
       else if ( !strcmp(type,"FLOAT") ) {
            if ( !check_float(value) ) {
               cout << " Default value of " << name << " is not FLOAT"<< endl;
               cout.flush();
               abort();
            }
       }
       else {
          cout << " Unknown type : " << type << endl;
          cout.flush();
          abort();
       }
    }

    _types.add(type);
    _names.add(name);
    _values.add(value);
  }


//------------------------  add  --------------------------------------------
  void Parms::add( const Name& name, const Name& value) {

    add(name,value,undef);
  }

//------------------------  add  --------------------------------------------
  void Parms::add( const Name& name, const char* type) {


    add(name,noval,type);
  }

//------------------------  add ---------------------------------------------
  void Parms::add( const char* name, const char* value, const char* type) {
     Name nameNm, valueNm;

     nameNm  = name;
     valueNm = value;
     add(nameNm,valueNm,type);
  }
 
//--------------------------- add -------------------------------------------
  void Parms::add( const char* name, const char* type) {
     Name nameNm;

     nameNm = name;
     add(nameNm,type);
  }

//--------------------------- remove ---------------------------------------
  void Parms::remove( const Name& name, Parms& parms) {

     if ( this == &parms ) {
       cout << " output has to be different tham input " << endl; cout.flush();
       abort();
     }

     parms.clear();

     if (_num == 0) {return ;}

     char type[7];
     Name nam,value,typeNm;

     for ( int i=0; i<_num; i++) {
        nam = _names[i];
        value = _values[i];
        typeNm = _types[i];
        if ( nam == name ) { }
        else {
           typeNm.whatAreYou(type,7);
           parms.add(nam,value,type);
        }
     }
  }
//--------------------------- out -------------------------------------------

  void Parms::out() const {
     if ( _num > 0 ) {
        for ( int ii=0; ii<_num; ii++) {
            cout << _types[ii] << _names[ii] << _values[ii] << "\n";
            cout.flush();
        }
     }
  }


  int Parms::get( const Name& name, Name& value, char* type) const {

     if ( _num <= 0 ) {return 0;}

     for ( int i=0; i<_num; i++) {
         if ( name == _names[i] ) {
            value = _values[i];
            _types[i].whatAreYou(type,7);
            return 1;
         }
     }
     return 0;

  }


  void Parms::get( const int inx, Name& name, Name& value, char* type) const {

     if ( inx < 0 || inx >= _num ) {
        cout << " Illegal index : " << inx << endl;
        cout.flush();
        abort();
     }

     name = _names[inx];
     value = _values[inx];
     _types[inx].whatAreYou(type,7);
  }





  void Parms::buildParmString(Name& str) const {
//
// Format:   /PAR1(I)=val1/PAR2(F)=val2..../PARn(S)=valn
//
//*** However at the moment we shall not output (S)
//
   str = "\0";
   Name doubleq,nullstr,tempval;
   doubleq = "\"";
   nullstr = "\0";

   if ( _num <= 0 ) { return; }

     for ( int i=0; i<_num; i++) {
         tempval = _values[i];
         str += "/";
         str += _names[i];
         if ( _types[i] == typeint )    { str += "(I)"; }
         if ( _types[i] == typefloat )  { str += "(F)"; }
         if ( _types[i] == typestring ) { 
//            str += "(S)"; 
// For the moment, if the value does not contain either / or = 
// I remove the double quotes so that DELPHI proxies are happy
            if ( tempval.exists("/") || tempval.exists("=") 
               || tempval == "\"\"") {}
            else { tempval.replace(doubleq,nullstr); }   
         }
         
         if ( tempval == noval ) {}
         else { str += "="; str += tempval;}
     } 
  }

  void Parms::buildParmString(char* str,int mxsize) const {
     Name tempName;

     buildParmString(tempName);

     tempName.whatAreYou( str, mxsize-1);
  }


  int Parms::set( const Name& name, const Name& value) {

     if ( _num <= 0 ) {return 1;}

     for ( int i=0; i<_num; i++) {
         if ( name == _names[i] ) {
            _values[i] = value; return 1;   // No type checking
         }         
     }
     cout << " Parameter " << name << " not found" << endl;
     cout.flush();
     return 2;
  }


  int Parms::set( const Name& name, const Name& value, const Name& type) {
//            1...updated   2...not found  3...found but not correct type
//
     if ( _num <= 0 ) {return 1;}

     for ( int i=0; i<_num; i++) {
         if ( name == _names[i] ) {
            if ( type == _types[i] ) {
               _values[i] = value; return 1;
            }
            else { 
               cout << " The new parameter " << name 
                    << " has incompatible type" << endl;
               cout.flush();
               return 3; 
            }
         }         
     }
     cout << " Parameter " << name << " not found" << endl;
     cout.flush();
     return 2;
  }
//

//------------------------- initFromParmString --------------------------
// Initialises itself from param string
// Format of the parameter string is:
//  /PAR1(I)=val1/......./PARi=vali/.../PARn(F)=valn
//
//   The string values can or need not
//   be surounded by double quotes. They of course have to be surounded by 
//   double quotes when they contain /  character. Also, the values 
//   are not allowed to contain a double quote(this could be easily allowed
//   by escaping them.
//   The brackets after the name have to be present. If they are not, it is
//   a string
//
  void Parms::initFromParmString(const Name& parmstr) {
//
     if ( _num > 0 ) {
         _types.clear();
         _names.clear();
         _values.clear();
     }
     _num = 0;
//
     int iel = 1;  
     Name element; 
     Name nametemp,name,value,valmod;
     char type[7];
     int ieqpos; // position of = in the element

     while ( parmstr.elementSp(iel,'/',element) ) {
        ieqpos = element.existsAt("=");
	if ( ieqpos < 0 ) {
           cout << " Parameter string has illegal format\n";
           cout << parmstr << "\n";
           cout.flush();
           abort();
        }
	element.subString(0,ieqpos,nametemp);
	element.subString(ieqpos+1,-1,value);
//	cout << endl << element << nametemp << value << endl;
        char* ptnr = strstr(&nametemp[0],"(");
        if ( ptnr ) {  
           if ( !strcmp(ptnr,"(I)") ) { strcpy(type,"INT");}           
           else if ( !strcmp(ptnr,"(F)") ) { strcpy(type,"FLOAT");}           
           else if ( !strcmp(ptnr,"(S)") ) { strcpy(type,"STRING");}           
           else {
              cout << " Parameter string has illegal format\n";
              cout << parmstr << "\n";
              cout.flush();
              abort();
           }
           *ptnr = '\0';
           name = &nametemp[0];
           *ptnr = '(';
        }   
        else { name=nametemp; strcpy(type,"STRING");} 
//                     name does not have type specifier-> it is a string

        valmod = value;
        if (!strcmp(type,"STRING")) {
           if ( value[0] != '\"' ) { // Strings without quotes are going to get them
              valmod = "\"";
              valmod += value;
              valmod += "\"";
           }
        }
        add(name,valmod,type);
        iel++;
     }
  }


  int Parms::setFromParmString(const Name& parmstr) {
//
//  1 all parameters found
//  0 not true

     if ( _num == 0 ) {return 1;}

     Parms temp;

     temp.initFromParmString(parmstr);

     if ( temp._num == 0 ) {return 1;}

     int iflg = 1;

     for ( int i=0; i<_num; i++) {
        Name value, typeNm; char type[7];
        if ( temp.get(_names[i],value,type) ) {
            typeNm = type; 
            if ( !set(_names[i],value,typeNm) ) { iflg = 0; }
        }
        else { 
            cout << " Parameter " << _names[i] << " not found" << endl;
            iflg = 0;
        }
     }

     return iflg;
  }


  int Parms::replaceNameValues( const Parms& source ) {

     int ifound = 1;

     for ( int i=0; i<_num; i++) {
         if ( check_name(_values[i] ) ) {
            int ifl = 0;
            for ( int j=0; j<source._num; j++) {
               if ( source._names[j] == _values[i] ) {
                   _values[i] = source._values[j]; 
                   _types[i] = source._types[j];
                    ifl = 1;
                    break;
               }
            }
            if (ifl == 0 ) {ifound =0;}  // if only 1 name not found
         }         
     }
     return ifound;

  }

  void Parms::guessTypes() {
     if (_num <= 0 ) {return;}

     for ( int i=0; i<_num; i++) {
        if ( _types[i] == undefNm ) {
           if (_values[i] == noval) {}
           else {
              if ( check_string(_values[i]) ) { _types[i] = "STRING";}
              else if ( check_int(_values[i]) ) { _types[i] = "INT";}
              else if ( check_float(_values[i]) ) { _types[i] = "FLOAT";}
              else {
                 cout << "Illegal type :" << _values[i] << endl;
                 cout.flush();
                 abort();
              }
           }
        }

     }
  }


  bool Parms::allHaveValues() const {

     if ( _num <= 0 ) { return 1;}

     for ( int i=0; i<_num; i++) {
        if ( _values[i] == noval ) { return 0; }
     }

     return 1;
  }

  Parms& Parms::operator=(const Parms& parms) {
//
     if ( _num > 0 ) {
         _types.clear();
         _names.clear();
         _values.clear();
     }
     _num = 0;
     if ( parms.numOfEntries() <= 0 ) { return *this;}

     char type[7];
     Name name,value,typeNm;

     for (int i=0; i<parms._num; i++) {
         name   = parms._names[i];
         value  = parms._values[i];
         typeNm = parms._types[i];
         typeNm.whatAreYou(type,7);
         add(name,value,type);
     }

     return *this;
  }


  void Parms::clear() {
     if ( _num > 0 ) {
         _types.clear();
         _names.clear();
         _values.clear();
     }
     _num = 0;
  }
