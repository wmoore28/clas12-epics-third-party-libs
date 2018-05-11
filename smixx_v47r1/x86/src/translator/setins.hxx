// setins.hxx: interface for the SetIns class.
//
//                                                  B. Franek
//                                                30 September 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef SETINS_HH
#define SETINS_HH

#include "instruction.hxx"
#include "paroperand.hxx"

class SetIns  : public Instruction 
{
public:
	SetIns();

	virtual ~SetIns();

	virtual void translate() ;

	void out(const Name offset) const;

	virtual void outSobj(int&,int,ofstream& sobj, Registrar&) const;

	int examine();
	
  	Name outString();
	
protected :

	Name checkForRightSideCast
	     (const SMLline& line, const int& jst , int& jnext);
	     
	char skipSpacesandTabs
            (SMLlineVector* pSMLcode, int ist, int jst, int& inbl, int& jnbl);

	
  int _insType; // This is redundant but is going to be kept for the time being

	ParOperand _lOperand;   // left operand, i.e. the result
	
	ParOperand _operand1;
	Name _operator;  // +  -  *  /
	ParOperand _operand2;

	int _numOperands;  //Number of right hand operands (now 1 or 2)
	int _numOperations;  //now 0 or 1
	
	Name _rightSideCast;
};

#endif 
