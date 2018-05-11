// smpcond.cxx: implementation of the SmpCond class.
//
//                                                B. Franek
//                                               21 February 2000
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "smlline.hxx"
#include "smllinevector.hxx"
#include "utilities.hxx"
#include "ut_tr.hxx"

#include "smpcond.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SmpCond::SmpCond(const int idNum) : _idNum(idNum), _pParentUnit(NULL)
{
	_pSMLcode = new SMLlineVector(5);
	assert(_pSMLcode != 0);
	return;
}

SmpCond::~SmpCond()
{
	delete _pSMLcode;
	return;
}

//-----------------------------------------------------------------------
int SmpCond::idNum() const
{
	return _idNum;
}

char SmpCond::type() const
{
	return 'T';
}

void SmpCond::setParentUnit(SMLUnit* parent)
{
	_pParentUnit = parent;
	return;
}

int SmpCond::examine()
{
  // Not emplemented yet
  
return 0;
  //cout << " SmpCond::examine()  id :" << _idNum << endl;
	
  //cout << "  Parent : " << _pParentUnit->unitId() 
  //<< "  " << _pParentUnit->unitName() << endl;
		
  //cout << "     Complete Ancestry " << endl;
	
	int num;
	NameVector ids,names;
	
	_pParentUnit->ancestry(ids,names);
	
	num = ids.length();

	for (int i=0; i<num; i++)
	{
		cout << "     " << ids[i] << "  " << names[i] << endl;
	}
	return 0;
}
//----------------------------------------------------------------------
bool SmpCond::belongsToIf()
{

	if ( _pParentUnit->unitId() == "ifunit head block" )
	{
		return true;
	}
	
	return false;
}
