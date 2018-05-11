//----------------------  Class   NameList  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 10 June 2001
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef NAMELIST_HH
#define NAMELIST_HH

#include <string.h>
#include "name.hxx"

class NameVector;
class NameListItem;



class NameList {
	public :
		NameList();

		NameList(const NameList&);

		NameList& operator=(const NameList& );

		~NameList(); 

		int add( const Name& );   // 1 .. added; 0 .. already there
						// and therefore not added
		
		int add( const char name[] );

		int remove(const Name &);   // ret 1 when removed; 0 not found         

		int remove(const char name[] );

		void removeAll();

		void reset() ; // 

		int nextItem(Name& name) ; // 

		void out(const Name& );

		int numOfEntries() const;
		
		int isPresent(const Name &);
		
/**  it will add all the items from the vector unless they are already present
*/
		void add(const NameVector &);

/**  it will add all the items from the list unless they are already present
*/
		void add(NameList& list);

	private :
		void copy(const NameList&);
		NameListItem* _pFront;
		NameListItem* _pBack;
		NameListItem* _pCurr;
		int _size;
}; 


class NameListItem {
	public :
		friend class NameList;
		NameListItem( const Name& val) : _item(val) {
		_pNext = 0;
		}
	private :
		Name _item;

		NameListItem* _pNext;
};

#endif
