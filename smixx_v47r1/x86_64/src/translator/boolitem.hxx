// boolitem.hxx: interface for the BoolItem class.
//
//                                                  B. Franek
//                                                28 February 2000
//
//////////////////////////////////////////////////////////////////////
#ifndef BOOLITEM_HH
#define BOOLITEM_HH

#include "smixx_common.hxx"
class Name;

class BoolItem
{
public:
	BoolItem();

	virtual ~BoolItem() ;

	virtual char type() const =0;

	virtual int idNum() const =0;

	virtual void out(const Name offset) const =0;

	virtual void outFull(const Name offset) const =0;

	virtual void outSobj(ofstream& sobj) const =0;

protected :
	
};

#endif 
