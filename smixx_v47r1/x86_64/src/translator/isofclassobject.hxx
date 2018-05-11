//-----------------------------------------------------------------------
//                         IsOfClassObject  Class
//                                                 B. Franek
//                                                 24 April 2000
//-----------------------------------------------------------------------
//
#ifndef ISOFCLASSOBJECT_HH
#define ISOFCLASSOBJECT_HH
#include "smixx_common.hxx"
#include "smlunit.hxx"
#include "name.hxx"
#include "smllinevector.hxx"
#include "namevector.hxx"
#include "registrar.hxx"
#include "attributeblock.hxx"


class  IsOfClassObject : public SMLUnit {

	public :

		IsOfClassObject 
			( const Name& name, int assoc);

		virtual ~IsOfClassObject();

		virtual void translate();

		virtual void outSobj(ofstream& sobj) const;
		
		Name& isOfClass();
		
		int examine();
		
  		Name outString();

	protected :

		int _associated;
		Name _isOfClass;

		AttributeBlock* _pAttributeBlock;
		
};
#endif
