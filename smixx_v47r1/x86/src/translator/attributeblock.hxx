//-----------------------------------------------------------------------
//                         AttributeBlock
//                                                 B. Franek
//                                                 2 November 1999
//-----------------------------------------------------------------------
//
#ifndef ATTRIBUTEBLOCK_HH
#define ATTRIBUTEBLOCK_HH
#include "smlunit.hxx"
#include "namevector.hxx"

class  AttributeBlock : public SMLUnit {

	public :

		AttributeBlock ();

		virtual ~AttributeBlock();

		virtual void translate();

		void out(const Name offset) const;


	protected :

		friend class SMIObject;
		friend class IsOfClassObject;
		friend class State;
		friend class Action;

		NameVector _attributes;

};
#endif
