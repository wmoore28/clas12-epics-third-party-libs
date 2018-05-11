//----------------------  Set_Name  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date :  4 April 1997
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef set_Name_HH
#define set_Name_HH

#include <string.h>

#include "name.hxx"


class SetItem_Name;



class Set_Name {
public :
   Set_Name() { _front = 0; _back = 0; _size = 0;}

   bool add(char *name);    // Adds the name to the back of the Set
                            // only if it does not exists
                            // return 1 true when added, 0 otherwise
   bool add(Name &);

   bool isPresent(const Name &);

   bool isPresent(char *name);

   bool remove(char *name);  //    removes the name from the set 
                            //    return 1-if removed  0-if not found
   bool remove(const Name &);

   int numOfEntries();

   void element(int inx, char* name, int mxsize);   // will get inxth name
   
    Name element(const int inx) const;

   bool isEmpty() { return _front==0 ? 1 : 0; }

   void out();

private :
   SetItem_Name* _front;
   SetItem_Name* _back;
   int _size;
}; 


class SetItem_Name {
public :
  friend class Set_Name;
  SetItem_Name( const Name& val) : _item(val) {
     _next = 0;
  }
private :
  Name _item;

  SetItem_Name *_next;
};

#endif
