// doins.hxx: interface for the DoIns class.
//
//                                                  B. Franek
//                                                30 September 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef DOINS_HH
#define DOINS_HH

#include "name.hxx"
#include "parms.hxx"
#include "instruction.hxx"

class DoIns  : public Instruction 
{
public:
	DoIns();

	virtual ~DoIns();

	virtual void translate() ;

	void out(const Name offset) const;

	virtual void outSobj(int&,int,ofstream& sobj,Registrar&) const;
		
	int examine();
		
  	Name outString();

protected :
	

	Name _actionNm, _objectNm;

	Parms _parameters;

};

#endif 
