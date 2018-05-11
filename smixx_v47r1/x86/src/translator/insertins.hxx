// insertins.hxx: interface for the InsertIns class.
//
//                                                  B. Franek
//                                                 1 July 2001
//
//////////////////////////////////////////////////////////////////////
#ifndef INSERTINS_HH
#define INSERTINS_HH

#include "instruction.hxx"

class InsertIns  : public Instruction 
{
public:
	InsertIns();

	virtual ~InsertIns();

	virtual void translate() ;

	void out(const Name offset) const;

	virtual void outSobj(int&,int,ofstream& sobj, Registrar&) const;


protected :

  int _insertRemove;  // 1-insert  0-remove

  Name _objectNm, _setNm;
  
};

#endif 
