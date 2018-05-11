//-----------------------------------------------------------------------
//                         ParameterBlock  Class
//                                                 B. Franek
//                                                 9 November 1999
//-----------------------------------------------------------------------
//
#ifndef PARAMETERBLOCK_HH
#define PARAMETERBLOCK_HH
#include "smlunit.hxx"
#include "parms.hxx"

class  ParameterBlock : public SMLUnit {

	public :

		ParameterBlock ();

		virtual ~ParameterBlock();

		virtual void translate();

		void out(const Name offset) const;
		
	protected :
		friend class SMIObject;
		friend class Action;

		Parms _parameters;
};
#endif
