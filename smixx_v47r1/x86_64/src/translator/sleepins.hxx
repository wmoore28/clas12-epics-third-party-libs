// sleeptins.hxx: interface for the SleepIns class.
//
//                                                  B. Franek
//                                                 June 2011
//
//////////////////////////////////////////////////////////////////////
#ifndef SLEEPINS_HH
#define SLEEPINS_HH

#include "instruction.hxx"

class SleepIns  : public Instruction 
{
public:
	SleepIns();

	virtual ~SleepIns();

	virtual void translate() ;

	void out(const Name offset) const;

	virtual void outSobj(int&,int,ofstream& sobj, Registrar&) const;


protected :

  Name _seconds;  // either number of seconds to sleep
                  // or $(parname)  where parname is the name of the parameter
		  // containing this value.

};

#endif 
