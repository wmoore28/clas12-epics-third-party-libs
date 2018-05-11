// ifUnit.hxx: interface for the IfUnit class.
//
//                                                  B. Franek
//                                                4 October 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef IFUNIT_HH
#define IFUNIT_HH

#include "smlunit.hxx"
//#include "attributes.hxx"
#include "ifunitheadblock.hxx"
#include "inslist.hxx"

class IfUnit  : public SMLUnit 
{
public:
	IfUnit();

	virtual ~IfUnit();

	virtual void translate() ;

	virtual void outSobj(int , int ,ofstream& sobj,  Name& label, InsList*& pInsList ) const;

protected :

	IfUnitHeadBlock *_pHeadBlock;

	InsList *_pInsList;

};

#endif 
