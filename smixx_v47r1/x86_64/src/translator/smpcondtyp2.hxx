// smpcond2.hxx: interface for the SmpCondTyp2 class.
//
//                                                  B. Franek
//                                                4 July 2002
// Copyright Information:
//      Copyright (C) 1999-2002 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef SMPCONDTYP2_HH
#define SMPCONDTYP2_HH

#include "smixx_common.hxx"
#include "name.hxx"
#include "namevector.hxx"
class SMLlineVector;
#include "smpcond.hxx"

class SmpCondTyp2 : public SmpCond
{
public:
	SmpCondTyp2(const int idNum);

	virtual ~SmpCondTyp2();
	
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
	
	int _allIn;  // 1 if all_in 0 any_in

	Name _objectSetName;
	
	int _inState;

	NameVector _states;
};

#endif 
