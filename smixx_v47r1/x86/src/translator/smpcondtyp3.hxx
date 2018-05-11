// smpcond3.hxx: interface for the SmpCondTyp3 class.
//
//                                                  B. Franek
//                                                 August 2009
//   Format :   ( 'object-set-name' empty )
//           or ( 'pbject-set-name' not_empty )
//
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef SMPCONDTYP3_HH
#define SMPCONDTYP3_HH

#include "smixx_common.hxx"
#include "name.hxx"
#include "namevector.hxx"
class SMLlineVector;
#include "smpcond.hxx"

class SmpCondTyp3 : public SmpCond
{
public:
	SmpCondTyp3(const int idNum);

	virtual ~SmpCondTyp3();

	int initialise( SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& inext, int& jnext);

	void negate();

	int examine();

	void out(const Name) const;

	void outFull(const Name) const;

	Name outString();

	void outSobj(ofstream& sobj) const;

protected :

        enum {_type = 3 };
	
	int _empty;  // 1 if empty, 0 otherwise

	Name _objectSetName;
};

#endif 
