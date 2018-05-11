//----------------------  Class   Parms  -----------------------------------
//
//
//                                                      Author: Boda Franek
//                                                      Date : 10 June 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//
//----------------------------------------------------------------------------
#ifndef PARMS_HH
#define PARMS_HH
#include "typedefs.hxx"
#include "block_name.hxx"
class Name;
//----------------------------------------------------------------------------

class Parms {
   public :
      Parms(); 

      int numOfEntries() const;

      void add( const Name& name, const Name& value, const char* type);
// type="STRING","INT" or "FLOAT"

      void add( const Name& name,  const char* type);

      void add( const Name& name, const Name& value);

      void add( const char* name, const char* value, const char* type);

      void add( const char* name, const char* type);

      void remove( const Name& name, Parms& pars);

      void out() const;

      int get( const Name& name, Name& value, char* type) const;//0 not found

      void get( const int inx, Name& name, Name& value, char* type) const ;

      void buildParmString(Name& str) const;

      void buildParmString(char* str, int mxsize) const;

      int set( const Name& name, const Name& value); 

      int set( const Name& name, const Name& value, const char* type);
//            1...updated   2...not found  3...found but not correct type
//
      int set( const Name& name, const Name& value, const Name& type);
//
      void initFromParmString(const Name& str);

      int setFromParmString(const Name& str);

      int replaceNameValues( const Parms& source ); //0..some where not replaced

      void guessTypes(); // For those params which have values and unknown type

      bool allHaveValues() const;

      Parms& operator=(const Parms &);
 
      void clear();

   private :
      int _num;
      Block_Name _types,_names,_values;
};

#endif
