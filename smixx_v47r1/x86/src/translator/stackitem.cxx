// stackitem.cxx: implementation of the StackItem class.
//
//                                                B. Franek
//                                           23 February 2000
//////////////////////////////////////////////////////////////////////

#include "stackitem.hxx"

#include "stdlib.h"
#include "name.hxx"
#include "smpcond.hxx"
#include "booloperation.hxx"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
StackItem::StackItem()
{
	return;
}


//---------------------------------------------------------------------
StackItem::StackItem(const char item)
{
	_item = item;
	if (item != '(' && item != ')' && item != '-' && item != '.' && item != '+') {
		cout << " Error : unknown item...internal logic problem"
		<< endl; exit(2);
	}

	_pBoolItem = 0;

	return;
}
//-----------------------------------------------------------------------
StackItem::StackItem(BoolItem* pBoolItem)
{
	_pBoolItem = pBoolItem;
	return;
}
//-----------------------------------------------------------------------
StackItem::StackItem(const StackItem& source)
{
	_item = source._item;
	_pBoolItem = source._pBoolItem;
	return;
}

StackItem::~StackItem()
{
    return;
}
//-----------------------------------------------------------------------
char StackItem::item() const 
{
    char temp;
	if ( _pBoolItem != 0 ) {
		temp = _pBoolItem->type();
	}
	else {
		temp = _item;
	}
	return temp;
}
//-----------------------------------------------------------------------
BoolItem* StackItem::pointer() const
{
	return _pBoolItem;
}
//-----------------------------------------------------------------------
/*int StackItem::idNumber() const
{
	int idNum = 0;

	if (_pBoolItem != 0 ) { idNum = _pBoolItem->idNum();}
	return idNum;
}*/
//----------------------------------------------------------------------
void StackItem::out(const Name ) const
{

	char temp = item();
	cout << temp;
	if (temp != 'X' && temp != 'T') {return;}


	int idNum = _pBoolItem->idNum(); cout << idNum;

	return;
}
