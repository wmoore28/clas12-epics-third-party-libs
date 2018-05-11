//-----------------------------------------------------------------------
//                         SMIObjectSetUnion  Class
//                                                 B. Franek
//                                                  1-Dec-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#ifndef SMIOBJECTSETUNION_HH
#define SMIOBJECTSETUNION_HH
#include "smlunit.hxx"
#include "name.hxx"
#include "namelist.hxx"
#include "smllinevector.hxx"

class  SMIObjectSetUnion : public SMLUnit {

	public :

		SMIObjectSetUnion( const Name& name);

		virtual ~SMIObjectSetUnion();

		virtual void translate() ;

		void out(const Name offset) ;

		virtual void outSobj(ofstream& sobj);
	
		Name setClass() const;
		
		int examine();
		
  		Name outString();

	protected :
		
		Name _isOfClass;

		NameList	_objectSetList;

};
#endif
