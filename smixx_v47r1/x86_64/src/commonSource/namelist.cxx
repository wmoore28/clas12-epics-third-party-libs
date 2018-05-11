//----------------------  Class   NameList  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 10 June 2001
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#include "smixx_common.hxx"
#include <assert.h>
#include "namevector.hxx"
#include "namelist.hxx"

NameList::NameList() {
	_pFront = 0; _pBack = 0; _size = 0;
	return;
}

void NameList::copy(const NameList& nameList) {

	Name name;
	removeAll();

	NameListItem* pCurr = nameList._pFront;

	if (nameList._size == 0) {return;}
	while (pCurr) {
		name = pCurr->_item;
		add(name);
		pCurr = pCurr->_pNext;
	}

	return;
}	
NameList::NameList( const NameList& nameList) {
	copy(nameList);
	return;
}

NameList& NameList::operator=(const NameList& nameList) {
	copy(nameList);
	return *this;
}

NameList::~NameList() {
	removeAll();
}
//-------------------------------- add ---------------------------------------
int NameList::add( const Name &valnew) {

	NameListItem *ptnew ;

	if ( _size == 0 ) {
		ptnew = new NameListItem(valnew);
		_pFront = _pBack = ptnew;
		_size = 1;
		return 1;
	}

	NameListItem* pt = _pFront;

// check if it already exists, if yes, return
	for (; pt != 0 ;) {
		if ( pt->_item == valnew) {return 0;}
		pt = pt->_pNext;      
	}
	
	// it is not in the list, so create it and put it at the back
	
	ptnew = new NameListItem(valnew);
	_pBack->_pNext = ptnew;
	_pBack = ptnew;
	_size++;

	return 1;
}
//=====================================================================================
int NameList::add( const char valnew[]) {
	Name temp = valnew;
	return add(temp);
}
//=============================== remove =================================

int NameList::remove(const Name& name) {
	if (_size == 0 ) { return 0;}

	NameListItem* pt = _pFront;
	NameListItem* pPrev = 0;
        NameListItem* pFrontnew;
	Name value;

	for (; pt != 0 ;) {
		value = pt->_item;
		if (value == name) {
			if (_size==1) {
				delete pt; _size--; return 1;
			}
			if (pt == _pBack) {
				_pBack = pPrev;
				_pBack->_pNext = 0;
				delete pt; _size--; return 1;
			}
			if (pt == _pFront) {
				pFrontnew = _pFront->_pNext;
				_pFront = pFrontnew;
				delete pt; _size--; return 1;
			}
			else {
				pPrev->_pNext = pt->_pNext;
				delete pt; _size--; return 1;
			}
		}
		else {pPrev = pt; pt = pt->_pNext;}      
	}
	return 0;
}

int NameList::remove( const char valnew[]) {
	Name temp = valnew;
	return remove(temp);
}
//=============================== removeAll ===========================
void NameList::removeAll() {

	if ( _size == 0 ) {return;}

	NameListItem* pt = _pFront; NameListItem* ptNext;

	for (; pt != 0 ;) {
		ptNext = pt->_pNext;
		delete pt;
		pt = ptNext;      
	}
//	NameList();
	_pFront = 0; _pBack = 0; _size = 0;
	return;
}
//=============================== firstItem =================================
void NameList::reset()  {
	_pCurr =_pFront;
	return;
}
//=============================== nextItem =================================
int NameList::nextItem(Name& name)  {
	if (_size == 0) {return 0;}

	if (_pCurr == 0) {return 0;}
	name = _pCurr->_item;

	NameListItem* pt = _pCurr->_pNext;
	_pCurr = pt;
	return 1;
}
//===============================  out  =====================================
void NameList::out(const Name& offset = "\0") {

	char* ptn = offset.getString(); 
	cout << ptn << "No of items in the List : " << _size << "\n";

	Name val; reset();

	while (nextItem(val)) {
		cout << ptn  << val << endl;
	}

	return;
}
//=============================  numEntries  ===============================
int NameList::numOfEntries() const {
	return _size;
}
//===========================================================================
int NameList::isPresent( const Name& newName)
{
	if ( _size == 0 ) {return 0;}

	NameListItem* pt = _pFront;
	
	for (; pt !=0 ;)
	{
		if ( pt->_item == newName ) { return 1; }
		pt = pt->_pNext;
	}
	
	return 0;
}
//===========================================================================
void NameList::add( const NameVector& namevec )
{
	int ie, numElem;
	
	numElem = namevec.length();
	
	for ( ie=0; ie<numElem; ie++ )
	{
		add(namevec[ie]);
	}
	
	return;
}
//===============================================  BF  Oct 2011 =========
void NameList::add( NameList& list )
{
	Name item;
	
	list.reset();
	while ( list.nextItem(item) )
	{
		add(item);
	}

	return;
}
