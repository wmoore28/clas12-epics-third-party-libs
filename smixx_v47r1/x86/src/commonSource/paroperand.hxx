// paroperand.hxx: interface for the ParOperand class.
//
//                                                  B. Franek
//                                                January 2012
//
// Copyright Information:
//      Copyright (C) 1999-2012 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef PAROPERAND_HH
#define PAROPERAND_HH

#include "name.hxx"
#include "smllinevector.hxx"

class ParOperand
{
public:
	ParOperand();
	
	int initFromSMLcode(SMLlineVector* pSMLcode
	                   , int& inext, int& jnext) ;

/**
    Used for initialising from sobj output 
    return 1 means expecting another line, anything else means failure
   */			   
	int initFromSobj_firstLine(char line[]);

/**
    Used for initialising from sobj output 
    return 1 means expecting another line, 0 this was the end line,
     anything else means failure
   */			   
	int initFromSobj_nextLine(char line[]);
			   
	virtual ~ParOperand();
	
	void out(const Name offset = "") const;
	
	void outShort() const;
	
	void outSobj( ofstream& sobj ) const;

/**
    methods for retrieval of various private data
*/
	Name& operandType();
	Name& valueType();
	Name& value();
	Name& parName();
	Name& ownerObjName();
	Name& tobeCastedTo();
	int castingRequested();
	
	Name printingName() const;
	
/**
    this method will set owner object to be THIS. Use with caution
*/
	void thisObjPar();
	
	void setValue(Name& value);
	void setValueType(Name& valueType);
	void setCast(Name& cast);
/**
   will calculate the operand's value Type from the ref parameter
   type and _tobecasted value
    return   1 - success
             2 - casting constant, casting ignored
	     -1 - casting STRING to FLOAT not allowed, casting ignored
*/	
	int calcOperValueType();
	Name operValueType();
	
protected :

	void checkForCast(const SMLline& line, const int& jst , int& jnext);

	// Parameter operand is either a simple value and that is
	// either STRING, FLOAT or INT  or
	// simple parameter name  (NAME)  or
	// composite name   obj-name.name   or obj-name<name> (COMPNAME)
	  
	Name _operandType; // VALUE, NAME or COMPNAME
/**
       When the operand type is VALUE, then 
           _valueType   specifies the value's type, i.e.   STRING, FLOAT or INT
	   _value       is the actual value and
	   _tobeCastedTo  is set ""
	when the operand type is either NAME or COMPNAME then
	   _valueType and _value 
	      are the type and the value of the refered to parameter.
	      This will only get set in State Manager during evaluation
	      or execution
	   _tobeCastedTo is set during SML translation and it is either
	      the cast requested by user, or imposed by Translator itself
	      in order to facilitate evaluation or execution in State Manager
*/	
	Name _valueType; 
	Name _value;

	Name _tobeCastedTo;
	int _castingRequested; //  0 no casting, 1 user in SML,
	//        2 imposed by Translator, 3 imposed by Translator over-riding
	//                                         user
	Name _operValueType;
/**
	When the operand type is VALUE then:
	     _parName and _ownerObjName are set to ""
	When the operand type is NAME then
	     _parName is the name of a local parameter (either action or object)
	     _ownerObjName is set to ""
	when the operand type is COMPNAME the
	     _parName is the parameter name of the other object's parameter and
	     _ownerObjName is the other object's name
*/
	Name _parName;
	Name _ownerObjName;	
/**
	The value 1 indicates that the initialisation that was started, has not
	finished yet
*/
	int _initNotFinished; 
};

#endif 
