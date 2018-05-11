//-----------------------------------------------------------------------
//                         ActionHeadBlock  Class
//                                                 B. Franek
//                                                 23 November 1999
//-----------------------------------------------------------------------
//
#ifndef ACTIONHEADBLOCK_HH
#define ACTIONHEADBLOCK_HH
#include "smlunit.hxx"
#include "parms.hxx"

class  ActionHeadBlock : public SMLUnit {

   public :

	ActionHeadBlock ();

	virtual ~ActionHeadBlock();

	virtual void translate();

	void out(const Name offset) const;

//	Parms* pParameters();
	 

   protected :

		friend class Action;

		Parms _parameters;
     
};
#endif
