//-----------------------------------------------------------------------
//                         SMIFrozenSet  Class
//                                                 B. Franek
//                                                  25-November-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#ifndef SMIFROZENSET_HH
#define SMIFROZENSET_HH

class SMIObject;
#include "smiobjectset.hxx"


class  SMIFrozenSet : public SMIObjectSet
{

	public :

		SMIFrozenSet();

		void out(const char* offset) ; 
		
		void copyObjectList(const SMIObjectSet& otherObjectSet);
		
		int remove(const Name&); 
		
	private :

};
#endif
