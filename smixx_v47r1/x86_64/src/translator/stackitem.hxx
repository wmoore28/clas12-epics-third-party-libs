// stackitem.hxx: interface for the StackItem class.
//
//                                                  B. Franek
//                                                23 February 2000
//
//////////////////////////////////////////////////////////////////////
#ifndef STACKITEM_HH
#define STACKITEM_HH

class BoolItem;
class Name;
class SmpCond;
class BoolOperation;

class StackItem 
{
public:
	StackItem();

	StackItem(const char item);

	StackItem(BoolItem* pBoolItem);

	StackItem(const StackItem&);

	virtual ~StackItem();

	char item() const;

	BoolItem* pointer() const;

	void out(const Name offset) const;

protected :

	char _item;

	BoolItem* _pBoolItem;

};

#endif 
