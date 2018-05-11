//----------------------------------------------------------------------
//
//                                           Author :  B. Franek
//                                           Date : March 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef CLIENTOBJECT_HH
#define CLIENTOBJECT_HH

#include "clientstate.hxx"
class ClientWhens;

/** @class ClientObject
  Instantiations (one/client object) of this class keep info about client states
  for that object. Client state is the state with client whens.
  
  @author Boda Franek 
  @date March 2008
*/
class ClientObject {
	public :
/** class constructor
  @param objectName is the name of the client object of the instantiation
*/
		ClientObject( const char objectName[] );
		~ClientObject();

/** will print the contents 
   @param indent is an indentation used
*/
		void out( const char indent[] ) const;
/** returns the name of the client object
*/
		const Name& objectName() const ;

/** returns the number of client state belonging to the client object
*/
		int numStates() const;
/** returns the state index corresponding to its name. when -1 it is not there
  @param stateName is the state name 
*/
		int stateInx( const char stateName[] ) const;
/** indicated when is marked as a client and put into the data structure
  @param stateName  is the name of the state to which the when belongs
  @param whenInx is the when's index within the state (begining with 0)
*/
		int markWhen( const char stateName[], int whenInx );
/** adds the new client state to the object
   @param newClientState
*/
		int addClientState( const ClientState& newClientState);
/**  the method 'merges in' the client states of another client object obviously of
      the same name. If the object name is not the same, the method returns 0.
      Otherwise it returns 1. 
      @param anotherClientObject is what it says it is.
*/
		int mergeIn(const ClientObject& anotherClientObject);
/** will execute the client whens of the object if applicable
*/
		int executeWhens() const;
		
	private :
		friend class ClientWhens;
/** name of the client object
*/
		Name _objectName;
/** vector of instantiations( one per client state) of ClientState class.
 See the class description. 'client state' is state containing any client whens
*/
		std::vector<ClientState> _states;	
};
#endif
