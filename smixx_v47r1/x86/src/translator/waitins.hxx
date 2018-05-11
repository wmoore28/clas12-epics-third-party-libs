// waitins.hxx: interface for the WaitIns class.
//
//                                                  B. Franek
//                                                16 December 2008
//
// Copyright Information:
//      Copyright (C) 1999-2008 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef WAITINS_HH
#define WAITINS_HH

#include "namevector.hxx"
#include "instruction.hxx"

class WaitIns  : public Instruction 
{
public:
	WaitIns();

	virtual ~WaitIns();

	virtual void translate() ;

	void out(const Name offset) const;

	virtual void outSobj(int&,int,ofstream& sobj,Registrar&) const;
		
	int examine();
		
	Name outString();

protected :	

	NameVector	_refObjects;
	NameVector 	_refObjectSets;

};

#endif 
