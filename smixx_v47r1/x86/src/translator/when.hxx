// when.hxx: interface for the When class.
//
//                                                  B. Franek
//                                                 23 September 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef WHEN_HH
#define WHEN_HH

#include "name.hxx"
#include "smlunit.hxx"
#include "condition.hxx"

class Action;

class When  : public SMLUnit 
{
public:
	When();

        When(int ident);
	
	virtual ~When();

	virtual void translate();

	void out(const Name offset) const;

	virtual void outSobj(ofstream& sobj) const;
	
	Action* endStateActionPointer() const;

	int examine();
		
  	Name outString();

protected :
		
	int _ident;

	Condition _condition;
	
	Name _actionName;
	
	Action *_pEndStateAction;
	
	Name _endState;
	
private :
	Action* createTermAction(const Name& actionName,const Name& stateName);
};

#endif 
