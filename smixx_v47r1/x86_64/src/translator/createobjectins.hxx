// createobjectins.hxx: interface for the CreateObjectIns class.
//
//                                                  B. Franek
//                                                 April 2010
//
//////////////////////////////////////////////////////////////////////
#ifndef CREATEOBJECTINS_HH
#define CREATEOBJECTINS_HH

#include "instruction.hxx"

class CreateObjectIns  : public Instruction 
{
public:
	CreateObjectIns();

	virtual ~CreateObjectIns();

	virtual void translate() ;

	void out(const Name offset) const;

	virtual void outSobj(int&,int,ofstream& sobj, Registrar&) const;


protected :

/**
	Identifier of the object. It is either
	a) <object name>                  or
	b) &VAL_OF_<parameter name>      where <parameter name> is the name
	   of the action parameter of the action that contains the create_object
	   instruction
*/
  Name _objectIdentifier;
  
/**
	The class name to which created object belongs
*/
  Name _className;
  
};

#endif 
