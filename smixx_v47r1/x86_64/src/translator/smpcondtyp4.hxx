// smpcondtyp4.hxx: interface for the SmpCondTyp4 class.
//
//                                                  B. Franek
//                                                January 2012
// Copyright Information:
//      Copyright (C) 1999-2012 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef SMPCONDTYP4_HH
#define SMPCONDTYP4_HH

#include "smixx_common.hxx"
#include "name.hxx"
#include "namevector.hxx"
class SMLlineVector;
#include "smpcond.hxx"
#include "paroperand.hxx"

class SmpCondTyp4 : public SmpCond
{
public:
	SmpCondTyp4(const int idNum);

	virtual ~SmpCondTyp4();
	
	int examine();

	int initialise( SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& inext, int& jnext);

	void negate();

	void out(const Name) const;

	void outFull(const Name) const;
	
	Name outString();

	void outSobj(ofstream& sobj) const;

protected :	
	
	enum {_type = 4 };

	ParOperand _operand1;
	Name _operator;  // ==   <>   <   >   >=   <=
	ParOperand _operand2;

};

#endif 
