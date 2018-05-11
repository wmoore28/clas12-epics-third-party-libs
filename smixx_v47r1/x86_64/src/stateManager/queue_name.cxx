//----------------------  Class   Name  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 28 December 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "smixx_common.hxx"
#include <assert.h>
#include <dis.hxx>

#include "queue_name.hxx"


//-----------------------------------------------------------------------------
void Queue_Name::add( Name &val) {
   dim_lock();
   QueueItem_Name *pt = new QueueItem_Name(val);

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


Name Queue_Name::remove() {
   dim_lock();
   if (isEmpty() ) {
      cout << "remove() on empty Queue\n";
      cout.flush();
      dim_unlock();
      exit(-1);
   }

   QueueItem_Name *pt = _front;
   _front = _front->_next;
   Name retval = pt->_item;
   delete pt;
   _size = _size - 1;
   dim_unlock();
   return retval;
}

void Queue_Name::out() {
   dim_lock();
   QueueItem_Name *pt = _front;

   cout << "\n No of items in the Queue : " << _size << "\n";
   for (; pt != 0 ;) {
      Name val = pt->_item;
      val.out();
      pt = pt->_next;      
   }
   cout.flush();

   dim_unlock();
   return;
}

void Queue_Name::remove(char* str, int mxsize) {
    Name temp;

    temp = remove();
    temp.whatAreYou(str,mxsize);
}


void Queue_Name::add(char* str) {
    Name temp;

    temp = str;

    add(temp);
}
//------------------------------------------------ BF Sep 2008 ---------------
int Queue_Name::removeItem(const Name& exterval)
{
	QueueItem_Name* pt = _front;
	QueueItem_Name* ptPrev = 0;
	Name val;
	
	dim_lock();
	if ( isEmpty() )
	{
		dim_unlock();
		return 0;
	}
	
	for (; pt != 0 ;) {
		val = pt->_item;
		if ( val == exterval )
		{
			if ( _size == 1 )
			{
				_front = 0; _back = 0; delete pt; _size = 0;
			}
			else if ( pt == _front )
			{
				_front = pt->_next; delete pt; _size = _size - 1;
			}
			else if ( pt == _back )
			{
				_back = ptPrev; delete pt; _size = _size - 1;
			}
			else
			{
				ptPrev->_next = pt->_next; delete pt; _size = _size - 1;
			}
			dim_unlock();
			return 1;
		}
		ptPrev = pt;
		pt = pt->_next;      
	}
	dim_unlock();
	return 0;
}
// Main program for testing
//  make it with : g++ -I$SMIDIR/smixx -I$DIMDIR/dim -I/../commonSource
//                     -L$DIMDIR/linux -ldim -lpthread queue_name.cxx 
//                                         ../commonSource/name.cxx
/*int main()
{
Queue_Name queue;

queue.add("ONE");
queue.add("TWO");
queue.add("THREE");
queue.add("FOUR");
queue.add("FIVE");
queue.add("SIX");

queue.out(); cout << endl;

//cout << queue.remove() << endl;


queue.removeItem("ONE");
queue.out(); cout << endl;
return 1;
}
*/
