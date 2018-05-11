// smpcond1.hxx: interface for the SmpCondTyp1 class.
//
//                                                  B. Franek
//                                                1 July 2002
// Copyright Information:
//      Copyright (C) 1999-2002 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef SMPCONDTYP1_HH
#define SMPCONDTYP1_HH

#include "smixx_common.hxx"
#include "name.hxx"
#include "namevector.hxx"
class SMLlineVector;
#include "smpcond.hxx"

class SmpCondTyp1 : public SmpCond
{
public:
	SmpCondTyp1(const int idNum);

	virtual ~SmpCondTyp1();
	
	int examine();

	int initialise( SMLlineVector* pSMLcode,
			const int ist, const int jst,  
//			int& iend, int& jend, int& inext, int& jnext);
			int& inext, int& jnext);

	void negate();

	void out(const Name) const;

	void outFull(const Name) const;
	
	Name outString();

	void outSobj(ofstream& sobj) const;

protected :

	const int _type;
	
	Name _objectName;

	int _inState;

	NameVector _states;
};


	int getStates(SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& inext, int& jnext,NameVector& states);

#endif 
