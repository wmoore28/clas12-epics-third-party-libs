//-------------------------  SmpCondTyp3  Class  ---------------------------------
#ifndef SmpCondTyp3_HH
#define SmpCondTyp3_HH
#include "parameters.hxx"
#include "smpcond.hxx"
#include "name.hxx"
#include "namevector.hxx"
#include "smifrozenset.hxx"
//--------------------------------------------------------------------------
//                                                         B. Franek
//                                                         Aug-2009
//                                               
// Copyright Information:
//      Copyright (C) 1996-2009 CCLRC. All Rights Reserved.
//
//                    ( 'object-set-name' empty )      or
//                    ( 'object-set-name' not_empty )
//----------------------------------------------------------------------------
class SmpCondTyp3 : public SmpCond {
public :
	SmpCondTyp3( char lines[][MAXRECL], int& no_lines);

	int evaluate();     /* 0-false;  1-true;  */
	
	void objectsToLock(NameList&) const;

	void freeze();

	void unfreeze();
	
	Name whatAreYou() const;
	
	void removeObjectFromFrozenObjectSet( const Name& objName, const Name& setName);

private :

	Name _objectSet;  //when \0 then no operation on object set  ????
	
	int _empty;      // when 1 then the set is empty, 0 the set is not empty

	SMIFrozenSet _frozenSet;

};

#endif
