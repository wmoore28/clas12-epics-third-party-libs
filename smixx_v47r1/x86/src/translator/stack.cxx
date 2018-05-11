// stack.cxx: implementation of the Stack class.
//
//                                                B. Franek
//                                           23 February 2000
//////////////////////////////////////////////////////////////////////

#include "stackitem.hxx"
#include "stack.hxx"

#include "stdlib.h"
#include "assert.h"
#include "name.hxx"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Stack::Stack(int size = 1000) : _maxLen(size), _top(EMPTY)  
{
	_s = new StackItem[size]; assert(_s != 0);	 
	return;
}

Stack::~Stack()
{
	delete [] _s;
    return;
}
void Stack::push(const char item)
{
	StackItem temp(item);
	push(temp);
	return;
}
void Stack::push(BoolItem* pBoolItem)
{
	StackItem temp(pBoolItem);
	push(temp);
	return;
}


//---------------------------------------------------------------------
void Stack::push(const StackItem& source) {
	_top++;
	*(_s+_top) = source;
	return;
}
//----------------------------------------------------------------------
int Stack::getFromStack(const int num, StackItem& sitem) const
{
	if ( (_top+1) < num ) {return 0;}
	sitem = *(_s+_top-num+1);
	return 1;
}
//----------------------------------------------------------------------
void Stack::deleteItem2FromStack()
{
	*(_s+_top-1) = *(_s+_top);
	_top = _top - 1;
}
//----------------------------------------------------------------------
void Stack::deleteTopStack(const int num)
{
	if (num <= 0) {return;}

	_top = _top - num;
}
//----------------------------------------------------------------------
void Stack::out(const Name offset) const
{
	char* ptn=offset.getString(); cout << ptn << "Stack";
	
	cout << endl; 
	for (int i=0; i<= _top; i++) {
		cout << " ";
		(_s+i)->out(offset);
	}
	cout << endl;
	return;
//	<< _item  << "  " << _simplCondNumber << "  " << _priority << endl;
}
