// booloperation.hxx: interface for the BoolOperation class.
//
//                                                  B. Franek
//                                                26 February 2000
//
//////////////////////////////////////////////////////////////////////
#ifndef BOOLOPERATION_HH
#define BOOLOPERATION_HH

#include "smixx_common.hxx"
class Name;
#include "boolitem.hxx"


class BoolOperation : public BoolItem
{
public:
	BoolOperation();

	BoolOperation(const int idNum,const char operation,
			const char operand1,const int num1,
			const char operand2,const int num2);

	BoolOperation(const int idNum, const char operation,
			 BoolItem* pOperand1, BoolItem* pOperand2);

	BoolOperation(const BoolOperation&);

	virtual ~BoolOperation();

	int idNum() const;

	char type() const;

	void out(const Name offset) const;

	void outFull(const Name offset) const;

	void outSobj(ofstream& sobj) const;


protected :
	
	int _idNum;
	char _operation, _operand1, _operand2;
	int _num1, _num2;
	BoolItem* _pOperand1; BoolItem* _pOperand2;
};

#endif 
