//----------------------  Class   Name  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 28 December 1996
// Copyright Information:
//      Copyright (C) 1996-2002 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef NAME_HH
#define NAME_HH
#include "smixx_common.hxx"
#include "typedefs.hxx"

class Name {
   public :
      Name();
      
      Name(const int numchar);

      Name(const char* );

      Name(const Name& );

      Name& operator=(const Name &);

      Name& operator=(const char *);

	/** Converts integer into a string
	   @param num integer number to be converted
	*/
	Name& operator=(const int num);

      Name& operator+= (const Name &);

      Name& operator+= (const char *);

      int length() const;

      char& operator[] (const int elem) const;

      void append(const Name& name);

      void append(const char*);

      bool element(const int num, const char delim, Name& elem) const;

      bool elementSp(const int num, const char delim, Name& elem) const;

      bool exists(const char* str) const; // checks if the string is part of the name
      
      bool exists(const Name& ) const;

      int existsAt(const char* str) const; // returns the index of the first
                                           // char otherwise 0
      char* getString() const; // returns the pointer to the first char
      
      void replace(const Name& oldstr, const Name& newstr); 
                          // will replace all occurences of string oldstr by
                          // newstr

      void squeeze();

      void trim();
      
      /** removes blanks and tabs from the front
      */
      void trimFront();

      void upCase();
      
      /** removes last character. if the string is empty, it has no effect
      */
      void removeLastChar();
                                        
      friend bool operator == (const Name &, const Name &);
                                                                             
      friend bool operator == (const Name &, const char[]);
 
      friend ostream& operator<<(ostream&, const Name& );

      virtual ~Name();

      void whatAreYou(char* str, const int mxsize) const;

      void out() const;
      
      void subString(int index, int nchar, Name& substr) const;
      // extracts substring starting at index and returns nchar. When nchar
      // is bigger than characters left, only those available are returned.
      // When nchar is negative, then all the characters untill the end are
      // returned      
//
//
   protected :
      int _size;
      char *_ptn;
};

#endif
