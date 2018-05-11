// inslist.hxx: interface for the Action class.
//
//                                                  B. Franek
//                                                28 September 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef INSLIST_HH
#define INSLIST_HH

#include "name.hxx"
#include "smlunit.hxx"
#include "instruction.hxx"

class InsList  : public SMLUnit 
{
public:
	InsList();

	virtual ~InsList();

	virtual void translate();

	virtual void outSobj(Name,int&, ofstream& sobj, Registrar&) const;

protected :

};

#endif 
