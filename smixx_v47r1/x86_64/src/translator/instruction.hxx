// instruction.hxx: interface for the Instruction class.
//
//                                                  B. Franek
//                                                28 September 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef INSTRUCTION_HH
#define INSTRUCTION_HH

#include "smixx_common.hxx"
#include "name.hxx"
#include "smlunit.hxx"

class Instruction  : public SMLUnit 
{
public:
	Instruction();

	virtual ~Instruction();

	virtual void translate() =0;

	virtual void outSobj(int&, int, ofstream& sobj, Registrar&) const =0;


protected :

};

#endif 
