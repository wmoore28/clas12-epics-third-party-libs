//-----------------------------------------------------------------------
//                         SMIObjectSet  Class
//                                                 B. Franek
//                                                  7-June-2001
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#ifndef SMIOBJECTSET_HH
#define SMIOBJECTSET_HH
#include "smlunit.hxx"
#include "name.hxx"
#include "namelist.hxx"
#include "smllinevector.hxx"

class  SMIObjectSet : public SMLUnit {

	public :

		SMIObjectSet( const Name& name);

		virtual ~SMIObjectSet();

		virtual void translate() ;

		void out(const Name offset) ;

		virtual void outSobj(ofstream& sobj);

		Name setClass() const;
		
		int examine();
		
  		Name outString();

	protected :
	
		Name		_isOfClass;

		NameList	_objectList;

};
#endif
