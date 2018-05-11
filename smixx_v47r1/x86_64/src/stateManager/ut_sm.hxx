//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  August 2001
// Copyright Information:
//      Copyright (C) 1996-2013 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef UT_SM_HH
#define UT_SM_HH


class Name;
class NameVector;
class ParOperand;
class SMIObject;
class Action;

#include "objectregistrar.hxx"
   extern ObjectRegistrar allSMIObjects;
   
void gime_millis(char* millis);
void newTime(char *timeStr);

void indent(int blanks);
void print_msg(const char *msg);
void gime_date_time(char *timeStr);
void print_date_time();
void print_obj(char *obj_name);
void print_obj(const Name& obj_name);

void makeStateString(const NameVector& states, Name& stateString);

int stringParToIntPar(Name& in, Name& out);
int floatParToStringPar(Name& in, Name& out);
int floatParToIntPar(Name& in, Name& out);
int intParToFloatPar(Name& in, Name& out);
int intParToStringPar(Name& in, Name& out);
	
//  functions
/**
  Return = 0 - failure
           1 - operand IS value
	   2 - the value is value of action parameter
	   3 - the value is value of object parameter
*/

int getOperandValue
   ( ParOperand& operand, SMIObject* pParentObject, Action* pParentAction, Name& value, Name& type);


#endif
