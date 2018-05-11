//-----------------------------------------------------------------------
//                         SMIObjectSet  Class
//                                                 B. Franek
//                                                  03-November-2009
// Copyright Information:
//      Copyright (C) 1999-2009 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------
//
#ifndef SMIOBJECTSET_HH
#define SMIOBJECTSET_HH


class SMIObject;

#include "name.hxx"
#include "namelist.hxx"
#include "nmdptnrlist.hxx"
#include "clientwhens.hxx"

class  SMIObjectSet {

	public :
	
		SMIObjectSet();
		
		virtual ~SMIObjectSet() { }

		virtual void out(const char* offset) = 0; 

//-----------------------------------------------------------

		Name name() const;

		int numOfObjects() const;

		void reset();

		int nextObject(Name& name); // return 0 means no more
		
		int nextObject(Name& name, SMIObject*& ptnr) ;

/**
  the function adds new client when to this object set. The when is identified
  by its object name, its state name and its index within that state
*/
		int addClientWhen(const char* whenObjName, const char* whenStateName, int whenInx);
/**
    Will print all the client whens
*/
		int printClientWhens() const;


/** Will inform all the SMI objects it contains about their membership
*/
		int informObjectsAboutMembership();
/** Will return the reference to the set's client whens
*/		
		const ClientWhens& gimeClientWhensRef() const;		
/**
  the method will return in the argument the current object list
*/
		void gimeObjectList(NmdPtnrList& objectList) const;
/**
  the method will merge into the atgument the current object list
*/
		void mergeInObjectList(NmdPtnrList& objectList) ;


	protected :

		Name		_name;

		NmdPtnrList	_objectList;
/**
  Keeps information about whens in the entire domain which reference this
  object set. This is build after the initialisation stage
*/
		ClientWhens _clientWhens;

};
#endif
