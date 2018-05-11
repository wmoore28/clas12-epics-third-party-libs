//-----------------------------------------------------------------------
//                         IfUnitHeadBlock  Class
//                                                 B. Franek
//                                                 4-April-2000
//-----------------------------------------------------------------------
//
#ifndef IFUNITHEADBLOCK_HH
#define IFUNITHEADBLOCK_HH
#include "smlunit.hxx"
#include "condition.hxx"

class  IfUnitHeadBlock : public SMLUnit {

   public :

	IfUnitHeadBlock ();

	virtual ~IfUnitHeadBlock();

	virtual void translate();

	void out(const Name offset) const;
	
	int examine();
	 

	protected :
		friend class IfUnit;


		int _else;
		Condition _condition;
     
};
#endif
