//---------------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  March 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
#include "clientobject.hxx"
#include "smiobject.hxx"
#include "objectregistrar.hxx"

extern ObjectRegistrar allSMIObjects;
//==============================================================
ClientObject::ClientObject(const char objectName[] ) 
	: _objectName(objectName), _states()
{
	return;
}
//==============================================================
ClientObject::~ClientObject() 
{
	return;
}
//==============================================================
const Name& ClientObject::objectName() const
{
	return _objectName;
}
//==============================================================
int ClientObject::stateInx( const char stateName[] ) const
{
	int numStates = _states.size();
	int i;
	
	for ( i=0; i<numStates; i++ )
	{
		if ( ((_states.at(i)).stateName()) == stateName )
		{
			return i;
		}
	}
	return -1;
}
//==============================================================
void ClientObject::out( const char indent[] ) const
{
	int i;
	cout << indent << "Object : " << _objectName << endl;

	int numObjectStates = _states.size();
	
	for ( i=0; i<numObjectStates; i++ )
	{
		cout << indent;
		(_states.at(i)).out(indent);
		cout << endl;
	}
	
	return;
}
//==============================================================
int ClientObject::markWhen( const char stateName[], int whenInx )
{
	int stIndex = stateInx(stateName);
	
	if ( stIndex < 0 )
	{  // new state
		ClientState temp(stateName);
		temp.markWhen(whenInx);
		_states.push_back(temp);
	}
	else
	{
		(_states.at(stIndex)).markWhen(whenInx);
	}
	return 1;
}
//=====================================================================
int ClientObject::numStates() const
{
	return _states.size();
}
//=====================================================================
int ClientObject::addClientState( const ClientState& newClientState )
{
	int stIndex = stateInx((newClientState._stateName).getString());
	if (stIndex >= 0) {return 0;}  // the state of the same name already exists
	
	_states.push_back(newClientState);
	return 1;
}
//=====================================================================
int ClientObject::mergeIn(const ClientObject& anotherClientObject)
{
	int mySt, otherSt, found;
	
	if (_objectName == anotherClientObject._objectName ) {}
	else {return 0;} // different object
	
	for ( otherSt=0; otherSt < anotherClientObject.numStates(); otherSt++ )
	{ // loop over the states of the other object and 'merge them in' one by one
		found = 0;
		for (mySt=0; mySt < numStates(); mySt++ )
		{  // loop over my states and merge the other(otherSt) state in.
		   // if not found, add it in
			found = _states[mySt].mergeIn(anotherClientObject._states[otherSt] );
		   if ( found ) {break;} // same state found and merged in
		}
		if ( found == 0 )
		{ // this is a new state, add it in
			addClientState(anotherClientObject._states[otherSt]);
		}
	}

	return 1;
}
//=============================================================================
int ClientObject::executeWhens() const
{
	SMIObject* pObj;
	Name stateNm;
	int ist,noOfStates;

	pObj = 
	allSMIObjects.gimePointer( _objectName );
	if ( pObj->isBusy() ) {return 0;}
		/* object is not busy */
	if ( pObj->hpActionsPending() ) {return 0;}
		/* no HP actions pending */
		
	stateNm = pObj->currentState();
		
	noOfStates = _states.size();
	
	for (ist = 0; ist<noOfStates; ist++)
	{
		if (_states.at(ist).correctState(stateNm))
		{
			const std::vector<int>& refToWhens =
			 _states.at(ist).getRefToWhens();
			 pObj->executeWhensFromFlagList(refToWhens);
			 return 1;
		}
	}

	return 0;
}
