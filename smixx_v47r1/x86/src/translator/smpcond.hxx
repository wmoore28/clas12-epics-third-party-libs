// smpcond.hxx: interface for the SmpCond class.
//
//                                                  B. Franek
//                                                21 February 2000
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef SMPCOND_HH
#define SMPCOND_HH

#include "smixx_common.hxx"
#include "name.hxx"
#include "namevector.hxx"
class SMLlineVector;
#include "boolitem.hxx"
#include "smlunit.hxx"

class SmpCond : public BoolItem 
{
public:
	SmpCond(const int idNum);
	
	virtual ~SmpCond();

	int idNum() const;

	char type() const;
	
	virtual void setParentUnit(SMLUnit* parent);
	
	virtual int examine();

	virtual int initialise(SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& inext, int& jnext) = 0;

	virtual void negate() = 0;

	virtual void out(const Name) const = 0;

	virtual void outFull(const Name) const = 0;

	virtual void outSobj(ofstream& sobj) const = 0;
	
protected :
	
	bool belongsToIf();
	
	const int _idNum;
	
	SMLUnit* _pParentUnit;
	
	SMLlineVector* _pSMLcode;  // at the moment it will contain the first
	                          // line on which the condition starts
};

#endif 
