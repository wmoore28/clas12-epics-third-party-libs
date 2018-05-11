// smlunit.hxx: interface for the SMLUnit class.
//
//                                                  B. Franek
//                                                 2 July 1999
//
//////////////////////////////////////////////////////////////////////

#ifndef SMLUNIT_HH
#define SMLUNIT_HH

#include "name.hxx"
#include "registrar.hxx"
#include "smllinevector.hxx"
#include "condition.hxx"
#include "namevector.hxx"

class Parms;

class SMLUnit  
{
public:
	SMLUnit(const char* id, const int buf, const Name name="\0", const Name type="\0");

	virtual ~SMLUnit();

	virtual int acceptLine( const SMLline& line);

    virtual void translate() = 0;

	virtual void out(const Name offset) const;

//	virtual void outSobj(ofstream& sobj) const;

	Name unitId() const;

	Name unitName() const;

        Name unitType() const;

	void outCodeOnly(const Name offset) const;

	int nextChar(const int line, const int ch,
				int nextLine, int nextCh) const;

/*	int prevChar(const int line, const int ch, 
				int prevLine, int prevCh) const;*/
	
	virtual void setParentUnit(SMLUnit* pUn);

	virtual void ancestry( NameVector& ids, NameVector& names);
	 	
	virtual int examine() ;
	
	void parents(Name& object, Name& state, Name& action);
	
	void printParents();
	
	SMLUnit* parentPointer(const Name& id) ; //given unit id, it will
	                                        // return the pointer to
					        // the first found
	void printCode() const;


protected :

	void translateUnits();

	int examineUnits();

	Name _id;  Name _name; Name _type;

	SMLlineVector* _pSMLcode;

    Registrar _internalSMLUnits;
	
	SMLUnit* _pParentUnit;
};

#endif 
