// ifins.hxx: interface for the IfIns class.
//
//                                                  B. Franek
//                                                30 September 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef IFINS_HH
#define IFINS_HH

#include "instruction.hxx"
#include "attributes.hxx"
#include "inslist.hxx"

class IfIns  : public Instruction 
{
public:
	IfIns();

	virtual ~IfIns();

	virtual void translate() ;

	virtual void outSobj(int&, int, ofstream& sobj, Registrar&) const;

protected :


};

#endif 
