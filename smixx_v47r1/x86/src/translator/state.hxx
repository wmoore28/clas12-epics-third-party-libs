//-----------------------------------------------------------------------
//                         State  Class
//                                                 B. Franek
//                                                 28 March 1999
//-----------------------------------------------------------------------
//
#ifndef STATE_HH
#define STATE_HH
#include "name.hxx"
#include "attributeblock.hxx"
#include "registrar.hxx"
#include "smlunit.hxx"
#include "namevector.hxx"

class  State : public SMLUnit {

	public :

		State (const Name& name, const Name& subobj);

		virtual ~State();

		virtual void translate();

		Name subObject() const;

		void out(const Name) const;

		virtual void outSobj(ofstream& sobj) const;
		
		int getAttributes(NameVector& attr) const; //returns their number
		
		bool isUndeclared() const;
		
		bool hasAction(const Name& actionNm) const;

	protected :

		Name  _subobjectName;

		AttributeBlock *_pAttributeBlock;

		Registrar _whens;

		Registrar _actions;

	private :
		void getWhenActions() ;
};
#endif
