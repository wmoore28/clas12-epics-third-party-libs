//-----------------------------------------------------------------------
//                         SMIObjectSetSimple  Class
//                                                 B. Franek
//                                                  03-November-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#ifndef SMIOBJECTSETSIMPLE_HH
#define SMIOBJECTSETSIMPLE_HH

class SMIObject;
#include "smiobjectset.hxx"
#include "namevector.hxx"
#include "parameters.hxx"


class  SMIObjectSetSimple : public SMIObjectSet
{

	public :

		SMIObjectSetSimple();

		SMIObjectSetSimple( char lines[][MAXRECL]);

		SMIObjectSetSimple(const SMIObjectSetSimple& );
		
		SMIObjectSetSimple& operator=(const SMIObjectSetSimple& );
		
		int add(const Name&);

		int remove(const Name&);
		
		void removeAll();

		void out(const char* offset) ; 

/**
  the method informs the set about a membership to a union
*/
		int joinUnion(const Name& unionNm);
		
	private :
		void copy(const SMIObjectSetSimple& );
		
		int addObjectToSet(const Name&);



		Name		_isOfClass;

/**
   Keeps the list of unions the set is member of
*/
		NameVector	_unionList;
};
#endif
