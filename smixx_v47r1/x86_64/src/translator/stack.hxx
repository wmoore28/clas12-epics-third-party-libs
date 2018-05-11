// stack.hxx: interface for the Stack class.
//
//                                                  B. Franek
//                                                23 February 2000
//
//////////////////////////////////////////////////////////////////////
#ifndef STACK_HH
#define STACK_HH

class StackItem;
class BoolItem;

class Stack 
{
public:
	Stack(int size);

	virtual ~Stack();

	void push(const char item);

	void push(BoolItem*);

	void push(const StackItem&);

	int getFromStack(const int, StackItem&) const;

	void deleteItem2FromStack();

	void deleteTopStack(const int);

	void out(const Name offset) const;

protected :

	enum { EMPTY = -1};

	StackItem* _s;

	int _maxLen;

	int _top;

};

#endif 
