//----------------------  Class   Queue_TwoNames  ----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 15 January 1997
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
#include <dis.hxx>

#include "queue_twonames.hxx"


//-----------------------------------------------------------------------------
void Queue_TwoNames::add( TwoNames &val) {
   dim_lock();
   QueueItem_TwoNames *pt = new QueueItem_TwoNames(val);

   if ( isEmpty() ) {
      _front = _back = pt;
   }
   else {
      _back->_next = pt;
      _back = pt;
   }
   _size++;
   dim_unlock();
}


TwoNames Queue_TwoNames::remove() {
   dim_lock();
   if (isEmpty() ) {
      cout << "remove() on empty Queue\n";
      cout.flush();
      dim_unlock();
      exit(-1);
   }

   QueueItem_TwoNames *pt = _front;
   _front = _front->_next;
   TwoNames retval = pt->_item;
   delete pt;
   _size = _size - 1;
   dim_unlock();
   return retval;
}

void Queue_TwoNames::out() {
   dim_lock();
   QueueItem_TwoNames *pt = _front;

   cout << "\n No of items in the Queue : " << _size << "\n";
   for (; pt != 0 ;) {
      TwoNames val = pt->_item;
      val.out();
      pt = pt->_next;      
   }
   cout.flush();

   dim_unlock();
   return;
}

void Queue_TwoNames::remove(char* str1, int mxsize1, char* str2, int mxsize2) {
    dim_lock();
    TwoNames temp;

    temp = remove();
    temp.whatAreYou( str1, mxsize1 ,  str2, mxsize2);
    dim_unlock();
}


void Queue_TwoNames::add(char* name1, char* name2) {
    dim_lock();
    TwoNames temp;

    temp.set( name1, name2);

    add(temp);
    dim_unlock();
}


void Queue_TwoNames::add(Name& name1, Name& name2) {
    dim_lock();
    TwoNames temp;

    temp.set( name1, name2);

    add(temp);
    dim_unlock();
}







