// action.hxx: interface for the Action class.
//
//                                                  B. Franek
//                                                 1 June 1999
// Copyright Information:
//      Copyright (C) 1999-2001 CCLRC. All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#ifndef ACTION_HH
#define ACTION_HH

#include "name.hxx"
#include "smlline.hxx"
#include "smlunit.hxx"
#include "actionheadblock.hxx"
#include "inslist.hxx"

class Action  : public SMLUnit 
{
public:
	Action(const Name&);

	virtual ~Action();

	virtual void translate();

	Name name() const;
	
	virtual void outSobj(ofstream& sobj) const;

        int getActionParameter(const Name& name, Name& value,
                               Name& typeNm) const;

protected :

	ActionHeadBlock *_pActionHeadBlock;

	InsList *_pInsList;
};

#endif 
