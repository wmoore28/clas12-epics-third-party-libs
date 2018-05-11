//----------------------  Class   Queue_TwoNames  ----------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 15 January 1997
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef queue_TwoNames_HH
#define queue_TwoNames_HH

#include "twonames.hxx"


class QueueItem_TwoNames;



class Queue_TwoNames {
public :
   Queue_TwoNames() { _front = 0; _back = 0; _size = 0;}

   TwoNames remove();           // Takes QueueItem from the front of the Queueueue
                                       // and returns it's value
   void add( TwoNames& ); // Adds the Item to the back of the Queue

   void out();

   bool isEmpty() { return _front==0 ? 1 : 0; }

   void remove(char *, int mxsize1 , char *, int mxsize2);

   void add(char *, char *);

	void add(Name& , Name&);

private :
   QueueItem_TwoNames* _front;
   QueueItem_TwoNames* _back;
   int _size;
}; 


class QueueItem_TwoNames {
public :
  friend class Queue_TwoNames;
  QueueItem_TwoNames( const TwoNames& val) : _item(val) {
     _next = 0;
  }
private :
  TwoNames _item;

  QueueItem_TwoNames *_next;
};

#endif



