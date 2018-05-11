//-----------------------------------------------------------------------
//                         SMIObjectSetUnion  Class
//                                                 B. Franek
//                                                  08-December-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#ifndef SMIOBJECTSETUNION_HH
#define SMIOBJECTSETUNION_HH

class SMIObject;

#include "namevector.hxx"
#include "smiobjectset.hxx"

#include "parameters.hxx"


class  SMIObjectSetUnion : public SMIObjectSet
{

	public :

		SMIObjectSetUnion( char lines[][MAXRECL]);

		void out(const char* offset) ; 
		
		void addedObjectToOneOfYourSets(const Name& object, const Name& set);
		
		void removedObjectFromOneOfYourSets(const Name& object, const Name& set);

	private :
	
		void updateObjectList();
		
		NameVector	_objectSets;

};
#endif
