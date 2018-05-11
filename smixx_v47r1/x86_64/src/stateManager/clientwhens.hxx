//----------------------------------------------------------------------
//
//                                           Author :  B. Franek
//                                           Date : March 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef CLIENTWHENS_HH
#define CLIENTWHENS_HH

#include "clientobject.hxx"

/** @class ClientWhens
  Instantiations (one/SMI object) of this class keep info about client whens
  
  Given a SMI object A, then Client whens are those that refer to A. Or putting
  it differently, A is mentioned in the when's condition. 
  E.g.'when ( O in_state READY ) ....'
  
  @author Boda Franek 
  @date March 2008
*/
class ClientWhens {
	public :
		ClientWhens(  );
		~ClientWhens();
/** will print the contents 
   @param indent is an indentation used
*/
		void out( const char indent[] ) const;
/** returns the number of client objects
*/
		int numObjects() const;
/** returns index of object of a given name. If index is -1, the object is not present
*/
		int objectInx( const char objectName[] ) const;
/** indicated when is marked as a client and put into the data structure
  @param objectName is the name of the object to which the when belongs
  @param stateName  is the name of the state to which the when belongs
  @param whenInx is the when's index within the state begining with 0
*/
		int markWhen( const char objectName[], const char stateName[], int whenInx );	
/** adds the new client object 
   @param newClientObject
*/
		int addClientObject( const ClientObject& newClientObject);
/**  the method 'merges in' the client whens another set of client whens
      @param anotherClientWhens
*/
		int mergeIn(const ClientWhens& anotherClientWhens);
/** will execute the client whens
*/
		int executeWhens() const;
		

	private :
/** 
  This is a vector of instantiations( one per client object)
  of ClientObject class. See the class description.
  'client object' is object containing any client whens'
*/
		std::vector<ClientObject> _objects;	
};
#endif
