//----------------------------------------------------------------------
//
//                                     Author :  B. Franek and Clara Gaspar
//                                     Date :  November 2008
// Copyright Information:
//      Copyright (C) 1996-2008 CCLRC & CERN. All Rights Reserved.
//----------------------------------------------------------------------

#ifdef _WIN32
  #include <windows.h>
  #define LOAD_LIB(x)  LoadLibrary( x )
  #define GETPROC(h,x) GetProcAddress ( HMODULE(h), x )
  #define SMISMRTLNAME	"smiSMrtl.dll"
#else
  #include <dlfcn.h>
  #include <unistd.h>
  #define LOAD_LIB(x)  dlopen( x , RTLD_NOW)
  inline void* GETPROC(void* h, const char* x) {  return dlsym(h,x); }
  #define SMISMRTLNAME	"libsmiSMrtl.so"
#endif

#include "msg.hxx"
void (*Msg::dllmsgRxPtr)(char *);
#include <iostream>
using namespace std;


void Msg::setup(char* smiDomain)
{
//	cout << "Msg::setup called  " << smiDomain << endl;

	void (*dllInitPtr)(char *);


	void *dllLibptr; // pointer to the smiSMrtl library
	dllLibptr = 0;

	dllLibptr = LOAD_LIB(SMISMRTLNAME);
//	cout << " pointer to the library  " << dllLibptr << endl;

	if (dllLibptr == 0) { return; }  // The library not found, nothing to do
	
	cout << " smiSMrtl library loaded " << endl;

	dllInitPtr  = (void (*)(char *))GETPROC(dllLibptr, "smiSMrtl_init" );
	dllmsgRxPtr = (void (*)(char *))GETPROC(dllLibptr, "smiSMrtl_msgRx" );
	
	if ( dllInitPtr == 0 ||  dllmsgRxPtr == 0 )
	{
		dllmsgRxPtr = 0;
		cout << " smiSMrtl_init or smiSMrtl_msgRX   not found in smiSMrtl" << endl;
		return;
	}
		
	(*dllInitPtr)(smiDomain);

//	cout << " back from smiSMrtl_init " << endl;
	
	return;

}
