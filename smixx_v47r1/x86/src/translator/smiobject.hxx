//-----------------------------------------------------------------------
//                         SMIObject  Class
//                                                 B. Franek
//                                                 21 March 1999
//-----------------------------------------------------------------------
//
#ifndef SMIOBJECT_HH
#define SMIOBJECT_HH
#include "smixx_common.hxx"
#include "smlunit.hxx"
#include "name.hxx"
#include "smllinevector.hxx"
#include "namevector.hxx"
#include "registrar.hxx"
#include "attributeblock.hxx"
#include "parameterblock.hxx"
#include "state.hxx"
#include "namevector.hxx"

class  SMIObject : public SMLUnit {

	public :

		SMIObject 
			( const Name&, int cl, int assoc);

		virtual ~SMIObject();

		virtual void translate();

		virtual void outSobj(ofstream& sobj) const;

                int getObjectParameter(const Name& name, Name& value, 
                                       Name& typeNm) const;
				       
		bool isClass() const;
				
/**    Returns the names of the Object's states and their number. If any of the
     states is undeclared, undeclared is 1, otherwise zero.
*/
		int getStates( NameVector& states, int& undeclared) const;
		
/**  Checks if object has state 'stateNm' declared. 
      If such state is declared ... returns true, otherwise false
      undeclared is set to 1 if objects has undeclared states, otherwise 0
*/
		bool hasState(const Name& stateNm, int& undeclared) const;
		
		bool hasAction(const Name& actionNm ) const;
		
		int examine();
		
  		Name outString();
		
	protected :
	
		int _class, _associated;
		Name _isOfClass;

		AttributeBlock* _pAttributeBlock;

		ParameterBlock* _pParameterBlock;

		Registrar _states;

};
#endif
