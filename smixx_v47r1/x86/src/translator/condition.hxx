// condition.hxx: interface for the Condition class.
//
//                                                  B. Franek
//                                                 8 October 1999
//
//////////////////////////////////////////////////////////////////////
#ifndef CONDITION_HH
#define CONDITION_HH

#include "smixx_common.hxx"
#include "name.hxx"
#include "registrar.hxx"
#include "smlline.hxx"
class SMLlineVector;
class Stack;
class SmpCond;
class BoolOperation;
class SMLUnit;


class Condition  
{
public:
	Condition();

	int initialise(SMLlineVector* pSMLcode,
			const int ist, const int jst,  
			int& iend, int& jend);

	void getHeader(SMLlineVector* pSMLcode,const int ist,const int jst,
					   int iend, int jend);


	virtual ~Condition();

	virtual void translate();

	void out(const Name) const;

	void outSobj(ofstream& sobj) const;

/** Returns the terminator i.e "DO" or "THEN" or "ENDINSTATE"	*/
	Name terminator() const;

	void setParentUnit(SMLUnit* parent);
	
	int examine();
	
protected :

/** This is called only from 'initialise'. It returns 1 or 2. 2 indicates the
    end of the condition. If anything goes wrong, it terminates the program.
*/
	int handleStack(Stack& stack,const char item);

/** This is called only from 'initialise'. Return code 1 means normal return
    return 2 means that the end of the condition was reached. If anything goes
    wrong, the program is terminated.
*/
	int getNextItem(SMLlineVector* pSMLcode,const int ist,const int jst,
					   int& iend, int& jend, char& item, SmpCond*&);

/** Used to determine the type of Simple Condition during parsing
    @return the type of the Simple condition
    @param pSMLcode pointer to the SML code processed by the condition
    @param ist line number where the simple condition starts
    @param jst character number where the simple condition starts
    Only called from 'getNextItem'. If anythig goes wrong it terminates the
    program.
*/
	int smpCondTyp(SMLlineVector* pSMLcode,const int ist,const int jst);

/** The function checks for cast indicator starting with (ist,jst)
    i.e. (S) or (I) or (F)   
    returns TRUE/FALSE
*/
	bool castIndicator(const SMLlineVector* pSMLcode,const int ist,const int jst) const;


	Name _sobjHeader;
	Name _terminator;

	Registrar _smpConditions;
//	Registrar _boolOperations;
	int _numBoolOperations;
	BoolOperation* _pExecutor;
	
/** kept update by the method 'initialise' and used by the called methods
*/
	SMLline _lineBeingTranslated;

	SMLUnit* _pParentUnit;
};

#endif 
