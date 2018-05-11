//----------------------------------------------------------------------
//
//                                           Author :  B. Franek
//                                           Date : March 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef CLIENTSTATE_HH
#define CLIENTSTATE_HH

#include <iostream>
#include <vector>
#include "name.hxx"
class ClientObject;

/** @class ClientState Keeps info about the client whens for the given state
  Instantiations (one/client state) of this class keep info about client whens
  for a particular state.
  
  @author Boda Franek 
  @date March 2008
*/
class ClientState {
	public :
/** constructor
  @param  stateName is name of the state 
*/
		ClientState( const char stateName[] );
		~ClientState();
/** will print the contents 
   @param indent is an indentation used
*/
		void out( const char indent[] ) const;
/** returns the name of the state
*/
		const Name& stateName() const;
/** returns information about a particular when of this state
    -1 the when is not there. Definitely not a client.
    0 this particular when is not a client
    1 this particular when is a client
    @param whenInx is when's index within the particular state.
*/
		int get( int whenInx ) const;
/** returns the number of whens present. Because of the way the
   the whens are stored, it is the index of the (last client when + 1)
*/
		int numWhens() const;
/** indicated when is marked as a client. If the whenInx is greater than the
    current no of whens, then the neccesary number of when flags is created
    and put into the data structure.
  @param whenInx is the when's index within the state (begining with 0)
*/
		int markWhen( int whenInx );
/**  the method 'merges in' the whens of another client state obviously of
      the same name. If the state name is not the same, the method returns 0.
      Otherwise it returns 1. 
      @param anotherClientState is what it says it is.
*/
		int mergeIn(const ClientState& anotherClientState);
/** returns TRUE or FALSE depending on whethe the right state
*/
		bool correctState(const Name& stateName) const;
/**  Returns reference to when flag list.
*/
		const std::vector<int>& getRefToWhens() const;
	private :
		friend class ClientObject;
/**  the name of the state
*/
		Name _stateName;
/**  vector of integers one/when indicating whether the when is client or not
    1 is a client
    0 is not a client
    NB. The number of flags is not normally equal to the number of whens
    belonging to the state. It is, in fact, equal to the index of 
    the (last client when + 1)
*/
		std::vector<int> _stateWhenFlags;	
	
};
#endif
