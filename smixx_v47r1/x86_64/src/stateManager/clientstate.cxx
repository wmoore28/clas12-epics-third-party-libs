//---------------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  March 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC. All Rights Reserved.
#include "clientstate.hxx"
//==============================================================
ClientState::ClientState(const char stateName[] ) 
	: _stateName(stateName), _stateWhenFlags()
{
	return;
}

//==============================================================
ClientState::~ClientState()
{
	return;
}
//==============================================================
const Name& ClientState::stateName() const
{
	return _stateName;
}
//==============================================================
int ClientState::numWhens() const
{
	return _stateWhenFlags.size();
}
//==============================================================
int ClientState::get( int whenInx ) const
{
	int sizeFlags = _stateWhenFlags.size();
	
	if ( whenInx < 0 || whenInx >= sizeFlags )
	{
		return -1;
	}
	return _stateWhenFlags.at(whenInx);
}
//==============================================================
int ClientState::markWhen( int whenInx )
{
	if ( whenInx < 0 )
	{
		return 0;
	}
	
	int sizeFlags = _stateWhenFlags.size();	
	
	if ( whenInx >= sizeFlags )
	{
		int beg,end,numZeroes;
		beg = _stateWhenFlags.size();
		end = whenInx;
		numZeroes = end - beg;
		
		for (int i=0; i<numZeroes; i++ )
		{
			_stateWhenFlags.push_back( 0 );
		}
		_stateWhenFlags.push_back( 1 );
	}
	else
	{
		_stateWhenFlags.at(whenInx) = 1;
	}
	return 1;
}
//==============================================================
void ClientState::out( const char indent[] ) const
{
	int i;
	cout << indent << " State : " << _stateName << endl;
	cout << indent << "    whens : ";

	int numStateWhens = _stateWhenFlags.size();
	
	for ( i=0; i<numStateWhens; i++ )
	{
		if ( _stateWhenFlags.at(i) )
		{
			cout << " " << i;
		}
	}
	cout << endl;
	
	return;
}
//=====================================================================
int ClientState::mergeIn(const ClientState& anotherClientState)
{
	int inx;
	
	if ( _stateName == anotherClientState._stateName ) {}
	else { return 0;} // different state
	
	for ( inx=0; inx < anotherClientState.numWhens(); inx++)
	{ //loop over the whens of the other State and mark the client whens
		if ( anotherClientState._stateWhenFlags.at(inx) )
		{
			markWhen(inx);
		}
	}
	return 1;
}
//=====================================================================
bool ClientState::correctState(const Name& stateName) const
{
	if ( stateName == _stateName )
	{
		return true;
	}
	else
	{
		return false;
	}
}
//=====================================================================
const std::vector<int>& ClientState::getRefToWhens() const
{
	return _stateWhenFlags;
}
