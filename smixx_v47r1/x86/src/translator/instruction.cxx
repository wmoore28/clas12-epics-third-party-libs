// instruction.cxx: implementation of the Instruction class.
//
//                                                B. Franek
//                                           28 September 1999
//////////////////////////////////////////////////////////////////////

#include "instruction.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Instruction::Instruction() : SMLUnit("Instruction",10) 
{
//	cout << "Instruction constructor called" << endl;
   return;
}

Instruction::~Instruction()
{
    delete _pSMLcode;
}
