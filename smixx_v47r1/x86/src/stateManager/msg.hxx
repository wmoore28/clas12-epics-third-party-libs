//----------------------------------------------------------------------
//
//                                     Author :  B. Franek and Clara Gaspar
//                                     Date :  November 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC & CERN. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef MSG_HH
#define MSG_HH

class Msg 
{
public :
	static void setup( char *domain);
	static void (*dllmsgRxPtr)(char *);
private :

};

#endif
