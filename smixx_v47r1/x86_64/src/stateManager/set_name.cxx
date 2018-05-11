//----------------------  Class   SetName  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 4 April 1997
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include <stdlib.h>
#include "smixx_common.hxx"
#include <assert.h>
#include "set_name.hxx"


//-----------------------------------------------------------------------------
bool Set_Name::add( Name &val) {

   if ( isPresent(val) ) { return 0;}

   SetItem_Name *pt = new SetItem_Name(val);

   if ( isEmpty() ) {
      _front = _back = pt;
   }
   else {
      _back->_next = pt;
      _back = pt;
   }
   _size++;

   return 1;
}


bool Set_Name::remove( const Name &name) {
   if (isEmpty() ) { return 0; }
      
   SetItem_Name *pt, *prev;
   Name retval;
   
   pt = _front;
   retval = pt->_item;
   if ( retval == name ) {    // it is the first item
       _front = _front->_next;
        delete pt;
       _size = _size - 1;
       return 1;
   }

   prev = pt; pt = pt->_next;

   while ( pt ) {
      retval = pt->_item;
      if ( retval == name ) {
          prev->_next = pt->_next;
         if( pt == _back ) { _back = prev; }
         delete pt;
         _size = _size - 1;
         return 1;
      }
      prev = pt;
      pt = pt->_next;
   }

   return 0;
   
}

bool Set_Name::remove(char* str) {
     Name temp;

     temp = str;
     return remove(temp);
}


void Set_Name::out() {
   SetItem_Name *pt = _front;

//   cout << "\n No of items in the Set : " << _size << "\n";
   cout << _size << ":";
//
   for (; pt != 0 ;) {
      Name val = pt->_item;
      val.out();
      pt = pt->_next;      
   }
   cout.flush();
   return;
}



bool Set_Name::add(char* str) {
    Name temp;

    temp = str;

    return add(temp);
}


bool Set_Name::isPresent( const Name& name) {
   SetItem_Name *pt = _front;

   for (; pt != 0 ;) {
      if ( pt->_item == name ) {return 1;}
      pt = pt->_next;      
   }

   return 0;
}

bool Set_Name::isPresent(char *str) {
     Name temp;

     temp = str;

     return isPresent(temp);
}

int Set_Name::numOfEntries() {

    return _size;
}

//============================================================================

void Set_Name::element(int inx, char* name, int mxsize ) {

    int cnt;

    if ( inx < 0 || inx >= _size ) {
       cout << " Illegal inx \n";
       cout.flush();
       exit(1); 
    }

    SetItem_Name *pt = _front; cnt = 0;

    for (; pt != 0 ; ) {
         if ( cnt == inx ) {
             Name val = pt->_item;
             val.whatAreYou(name,mxsize);
             return;
         }         
         pt = pt->_next;
         cnt++;
    }
    cout << " Element not found...not possible\n";
    cout.flush();
    exit(1);
}
//============================================================================

Name Set_Name::element(const int inx) const {

    int cnt;

    if ( inx < 0 || inx >= _size ) {
       cout << " Illegal inx \n";
       cout.flush();
       exit(1); 
    }

    SetItem_Name *pt = _front; cnt = 0;

    for (; pt != 0 ; ) {
         if ( cnt == inx ) {
             Name val = pt->_item;
             return val;
         }         
         pt = pt->_next;
         cnt++;
    }
    cout << " Element not found...not possible\n";
    cout.flush();
    exit(1);
}
