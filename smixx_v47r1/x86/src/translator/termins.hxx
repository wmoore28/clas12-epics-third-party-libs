// termins.hxx: interface for the TermIns class.
//
//                                                  B. Franek
//                                                30 September 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef TERMINS_HH
#define TERMINS_HH

#include "instruction.hxx"

class TermIns  : public Instruction 
{
public:
	TermIns();

	virtual ~TermIns();

	virtual void translate() ;

	void out(const Name offset) const;

	virtual void outSobj(int&,int,ofstream& sobj, Registrar&) const;
		
	int examine();
		
	Name outString();

protected :

	Name _endState;

};

#endif 
