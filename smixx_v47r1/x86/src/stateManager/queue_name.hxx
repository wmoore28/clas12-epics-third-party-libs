//----------------------  Class   Name  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 28 December 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef queue_Name_HH
#define queue_Name_HH

#include <string.h>

#include "name.hxx"


class QueueItem_Name;



class Queue_Name {
public :
   Queue_Name() { _front = 0; _back = 0; _size = 0;}

   Name remove();            // Takes QueueItem from the front of the Queueueue
                                       // and returns it's value
   void add( Name& ); // Adds the Item to the back of the Queue

   void out();

   bool isEmpty() { return _front==0 ? 1 : 0; }

   void remove(char *, int mxsize);

   void add(char *);
   
/** removes the first item from the queue, that is equal to the val
*/
	int removeItem(const Name& val);

private :
   QueueItem_Name* _front;
   QueueItem_Name* _back;
   int _size;
}; 


class QueueItem_Name {
public :
  friend class Queue_Name;
  QueueItem_Name( const Name& val) : _item(val) {
     _next = 0;
  }
private :
  Name _item;

  QueueItem_Name *_next;
};

#endif
