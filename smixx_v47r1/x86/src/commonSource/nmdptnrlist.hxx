//This file was generated by createRealClass on Thu Aug 14 12:44:47 CEST 2003
//----------------------  Class   NmdPtnrList  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 6-Aug-2003
// Copyright Information:
//      Copyright (C) 1996-2003 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef NmdPtnrLIST_HH
#define NmdPtnrLIST_HH

#include <string.h>
#include "nmdptnr.hxx"


class NmdPtnrListItem;



class NmdPtnrList {
	public :
		NmdPtnrList();

		NmdPtnrList(const NmdPtnrList&);

		NmdPtnrList& operator=(const NmdPtnrList& );

		~NmdPtnrList(); 

		int add( const NmdPtnr& );    // 1 .. added; 0 .. already there

		int remove(const NmdPtnr &);   // ret 1 when removed; 0 not found 
		
		int remove(const Name& ); // same returns as above        

		void removeAll();

		void reset() ; // 

		int nextItem(NmdPtnr& item) ; // 
		
		int firstItem(NmdPtnr& item, NmdPtnrListItem*& ptnNext);
		
		int nextItem(NmdPtnr& item, NmdPtnrListItem*& ptnNext);

		void out(const Name& offset = "") ;

		int numOfEntries() const;
		
/** Will find the named item and returns the pointer, if not found returns zero
*/
		void* pointer( const Name& name) const;

	private :
		void copy(const NmdPtnrList&);
		NmdPtnrListItem* _pFront;
		NmdPtnrListItem* _pBack;
		NmdPtnrListItem* _pCurr;
		int _size;
}; 


class NmdPtnrListItem {
	public :
		friend class NmdPtnrList;
		NmdPtnrListItem( const NmdPtnr& val) : _item(val) {
		_pNext = 0;
		}
	private :
		NmdPtnr _item;

		NmdPtnrListItem* _pNext;
};

#endif
