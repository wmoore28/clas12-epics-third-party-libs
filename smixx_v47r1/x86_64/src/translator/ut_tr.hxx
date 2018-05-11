//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  July 2013
// Copyright Information:
//      Copyright (C) 1996-2013 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef UT_TR_HH
#define UT_TR_HH

class SMLUnit;
class ParOperand;
class SMIObject;
class Name;
class SMLline;

/**
Will return a pointer to SMI Object or pointer to it's class
if the object is 'IsOfClassObject' type
*/
SMIObject* gimeObjectPointer( Name& objName );

#endif
