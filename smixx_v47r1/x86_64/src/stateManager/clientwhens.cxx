//---------------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  March 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
#include "clientwhens.hxx"

//==============================================================
ClientWhens::ClientWhens( ) 
	:  _objects()
{
	return;
}
//==============================================================
ClientWhens::~ClientWhens() 
{
	return;
}
//==============================================================
int ClientWhens::numObjects() const
{
	return _objects.size();
}
//==============================================================
int ClientWhens::objectInx( const char objectName[] ) const
{
	int numObjects = _objects.size();
	int i;
	
	for ( i=0; i<numObjects; i++ )
	{
		if ( ((_objects.at(i)).objectName()) == objectName )
		{
			return i;
		}
	}
	return -1;
}
//==============================================================
int ClientWhens::markWhen( const char objectName[], const char stateName[], int whenInx )
{
	int objIndex = objectInx(objectName);
	
	if ( objIndex < 0 )
	{  // new object
		ClientObject temp(objectName);
		temp.markWhen(stateName,whenInx);
		_objects.push_back(temp);
	}
	else
	{
		(_objects.at(objIndex)).markWhen(stateName,whenInx);
	}
	return 1;
}
//==============================================================
void ClientWhens::out( const char indent[] ) const
{
	int i;

	int numObjects = _objects.size();
	
	for ( i=0; i<numObjects; i++ )
	{
		cout << indent;
		(_objects.at(i)).out(indent);
		cout << endl;
	}
	
	return;
}
//===============================================================
int ClientWhens::addClientObject( const ClientObject& newClientObject)
{
	int objIndex = objectInx((newClientObject._objectName).getString());
	if (objIndex >= 0) {return 0;} // object of the same name already exists
	
	_objects.push_back(newClientObject);
	return 1;
}
//=================================================================
int ClientWhens::mergeIn(const ClientWhens& anotherClientWhens)
{
	int myObj, otherObj, found;
	
	for ( otherObj=0; otherObj < anotherClientWhens.numObjects(); otherObj++)
	{ // loop over the objects of the other structure and merge them in one by one
		found = 0;
		for (myObj = 0; myObj < numObjects(); myObj++ )
		{ // loop over my objects and merge the other (otherObj) object in.
		  // if not found, then add it in.
			found =_objects[myObj].mergeIn( anotherClientWhens._objects[otherObj] );
			if (found) {break;} //object of the same name found and merged in
		}
		if ( found == 0 )
		{ // this is a new object, add it in
			addClientObject( anotherClientWhens._objects[otherObj] );
		}
	}
	return 1;
	
}
//==========================================================================
int ClientWhens::executeWhens() const
{
	int iobj;

	int numObjects = _objects.size();
	
	for ( iobj=0; iobj<numObjects; iobj++ )
	{
		(_objects.at(iobj)).executeWhens();
	}

	return 1;
}
