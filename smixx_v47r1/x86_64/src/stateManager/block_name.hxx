//----------------------  Block_Name  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date :  4 April 1997
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef Block_Name_HH
#define Block_Name_HH

#include "name.hxx"



class Block_Name {
public :
   Block_Name() ;

   ~Block_Name();
                            
   void add(const Name &);

   void add(const char *name);
   
   void append(Block_Name& blockname);    

   int numOfEntries() const;

   Name& operator[] (int index)  const;

   void clear();

   void out() const;

   friend ostream& operator<<(ostream&, Block_Name& );

private :
   int _size, _num;

   Name **_pName;     // array of pointers to names
}; 

#endif
