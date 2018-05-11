//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  August 2001
// Copyright Information:
//      Copyright (C) 1996-2013 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include "dic.hxx"

#include "parameters.hxx"
#include "name.hxx"
#include "namevector.hxx"
#include "paroperand.hxx"
#include "smiobject.hxx"
#include "action.hxx"
#ifdef __VMS
#	include <descrip.h>
#endif
#include "utilities.hxx"
#include "ut_sm.hxx"
#ifndef _WIN32
#	include <sys/resource.h>
#	include <sys/time.h>
#	include <climits>
#else
#	include <time.h>
#	include <sys/timeb.h>
#endif

extern int newTimeFlg;
//--------------------------------------------------------------------

void gime_millis(char* millisStr)
{
       int millies;

#ifdef WIN32
       struct timeb timebuf;
#else
       struct timeval tv;
       struct timezone *tz;
#endif

#ifdef WIN32
       ftime(&timebuf);
       millies = timebuf.millitm;
#else
       tz = 0;
       gettimeofday(&tv, tz);
       millies = (int)tv.tv_usec / 1000;
#endif
//     dim_print_date_time();
//       printf("milliseconds: %d ", millies);
       sprintf(millisStr,"%03d",millies);
}
//--------------------------------------------------------------------

void newTime(char *timeStr)
{

	if (newTimeFlg == 0 ) return;
	
	char myStr[50];
	char mon[4], day[3], newday[3], time[15], year[5];
	strcpy(myStr,timeStr);
	
	char *pch; int i=0;
	
	pch = strtok (myStr," ");
	
	while (pch != NULL)
	{
  		i++;
//		printf(" %d %s\n",i,pch);
		if ( i==1 ) {}
		else if ( i==2 ) strcpy(mon,pch);
		else if ( i==3 ) strcpy(day,pch);
		else if ( i==4 ) strcpy(time,pch);
		else if ( i==5 ) strcpy(year,pch); 
		pch = strtok(NULL," ");
	}
	
	strcat(year,".");
	
	if ( strlen(day) == 1 )
	{
		strcpy(newday,"0");
		strcat(newday,day);
		strcpy(day,newday);
	}
	strcat(day," ");
	
	if ( !strcmp(mon,"Jan") ) strcpy(mon,"01");
	else if ( !strcmp(mon,"Feb") ) strcpy(mon,"02");
	else if ( !strcmp(mon,"Mar") ) strcpy(mon,"03");
	else if ( !strcmp(mon,"Apr") ) strcpy(mon,"04");
	else if ( !strcmp(mon,"May") ) strcpy(mon,"05");
	else if ( !strcmp(mon,"Jun") ) strcpy(mon,"06");
	else if ( !strcmp(mon,"Jul") ) strcpy(mon,"07");
	else if ( !strcmp(mon,"Aug") ) strcpy(mon,"08");
	else if ( !strcmp(mon,"Sep") ) strcpy(mon,"09");
	else if ( !strcmp(mon,"Oct") ) strcpy(mon,"10");
	else if ( !strcmp(mon,"Nov") ) strcpy(mon,"11");
	else if ( !strcmp(mon,"Dec") ) strcpy(mon,"12");
	strcat(mon,".");
	
	char millis[5];
	gime_millis(millis);
	
	strcat(time,".");
	strcat(time,millis);
	
	strcpy(myStr,year);
	strcat(myStr,mon);
	strcat(myStr,day);
	strcat(myStr,time);
//	printf("%s\n",myStr);
//	printf("%s%s%s%s\n",year,mon,day,time);
	strcpy(timeStr,myStr);
	return;
}

//------------------------------------------------------------------
void gime_date_time(char* timeStr)
{
#ifdef __VMS
    char s[24];
    $DESCRIPTOR(msg,s);

    lib$date_time(&msg);
    s[23] = 0;
    newTime(s);
    cout << s ;
#else
    time_t t;
    char str[128];

    t = time((time_t *)0);
    my_ctime(&t,str,128);    
    str[strlen(str)-1] = '\0';
    newTime(str);
    strcpy(timeStr,str) ;
#endif
}

void print_date_time()
{
#ifdef __VMS
    char s[24];
    $DESCRIPTOR(msg,s);

    lib$date_time(&msg);
    newTime(s);
    s[23] = 0;
    cout << s ;
#else
    time_t t;
    char str[128];

    t = time((time_t *)0);
    my_ctime(&t,str,128);    
    str[strlen(str)-1] = '\0';
    newTime(str);
    cout << str ;
#endif
}

void print_msg(const char *msg) {
   print_date_time();
   cout << " - " << msg << "\n";
   cout.flush();
}


void print_obj(char *obj_name){
     print_date_time();
     cout << " - " << obj_name;
}


void print_obj(const Name& obj_name){
     print_date_time();
     cout << " - " << obj_name;
}


void indent(int blanks){
  for (int ii=0; ii<=blanks; ii++) {  cout << " ";}
  cout.flush();

}

//--------------------------------------------------------------------
void makeStateString(const NameVector& states, Name& stateString) {

  int noOfStates = states.length(); 
   
  if ( noOfStates > 1 ) {
     stateString += "{";
  }

  for (int ist=0; ist < noOfStates; ist++) {
     stateString += states[ist];
     if (ist < (noOfStates-1)) {
         stateString += ",";
     }  
  }

  if ( noOfStates > 1 ) {
      stateString += "}";
  }
  
  return;
}
//-------------------------------------------------------------------------------------------------------------
int stringParToIntPar(Name& in, Name& out)
{ 
	Name temp;
	Name tempout;
	
	temp = in;
	temp.removeLastChar();  // remove the end "
	tempout = (char*)(temp.getString()+1);  // this copy will skip the first "
	if ( check_int(tempout) ) 
	{
		out = tempout;
		return 1; 
	}
	else
	{
		return 0;
	}
}
//---------------------------------------------------------------------------
int floatParToIntPar(Name& in, Name& out)
{    // convert floating point parameter to integer parameter
	double fvalue;
	int ivalue;
	char sivalue[20];
			
	if ( sscanf(in.getString(),"%lg",&fvalue) )
	{
		if ( fvalue >= INT_MAX || fvalue <= INT_MIN ) { return 0; }
		ivalue = static_cast<int>(fvalue);
		sprintf(sivalue,"%d",ivalue);
		out = sivalue;
		return 1;
	}
	else
	{
		cout << endl;
		cout << " *** error converting " << in
		<< " to DOUBLE, this should not happen" << endl;
		return 0;
	}
}
//-------------------------------------------------------------------------------
int intParToFloatPar(Name& in, Name& out)
{   //convert integer parameter to floating point parameter
	Name temp;
	
	temp = in;
	temp += ".0";
	out = temp;
	return 1;
}
//----------------------------------------------------------------------------------
int intParToStringPar(Name& in, Name& out)
{   // convert integer parameter to string parameter
	Name temp;
	
	temp = in;
	
	out = "\"";
	out  += temp;
	out += "\"";
	return 1;
}
//----------------------------------------------------------------------------------
int floatParToStringPar(Name& in, Name& out)
{   // convert float parameter to string parameter
// it is the same as int to float
	intParToStringPar(in,out);
	return 1;
}
//-------------------------------------------------------------------------
int getOperandValue
 ( ParOperand& operand, SMIObject* pParentObject, Action* pParentAction,
  Name& value, Name& type)
{
	int flg;
	Name parNm, objNm;
	SMIObject* pObj;
	
	Name operValueType = operand.operValueType();
	// type requested by user in SML or by Translator
	Name parValType, parVal;
	
	int retflg = 0;
	 
	if ( operand.operandType() == "VALUE" )
	{
		value = operand.value();
		type = operand.operValueType();
		return 1;
	}
	if ( operand.operandType() == "NAME" )
	{
		parNm = operand.parName();
		flg = pParentAction->getActionParameter(parNm,parVal,parValType);
		if ( flg == 1) { retflg = 2; }
		else
		{
			flg = pParentObject->getObjectParameter(parNm,parVal,parValType);
			if ( flg == 1 )
			{
				operand.thisObjPar();
				retflg = 3;
			}
		}
	}
	if ( operand.operandType() == "COMPNAME" )
	{
		parNm = operand.parName();
		objNm = operand.ownerObjName();
		if ( objNm == "THIS" )
		{
			pObj = pParentObject;
		}
		else
		{
			pObj = allSMIObjects.gimePointer(objNm);
		}
		flg = pObj->getObjectParameter(parNm,parVal,parValType);
		if ( flg == 1 ) { retflg = 3; }
	}


	if (retflg == 0) {return 0;}
	
	value = parVal;
	type = operValueType;

	if ( parValType == operValueType ) { return retflg; }
	
	// have to do a conversion
	if ( operValueType == "STRING" && parValType == "FLOAT")
	{
		flg = floatParToStringPar(parVal,value);
	}
	if ( operValueType == "STRING" && parValType == "INT")
	{
		flg = intParToStringPar(parVal,value);
	}
	if ( operValueType == "FLOAT" && parValType == "STRING")
	{
		cout << " *** refuse to convert STRING to FLOAT " << endl
		<< " parameter " << parNm 
		<< " this should not have happened !! call expert " << endl;
		return 0;
	}
	if ( operValueType == "FLOAT" && parValType == "INT")
	{
		flg = intParToFloatPar(parVal,value);
	}
	if ( operValueType == "INT" && parValType == "STRING")
	{
		flg = stringParToIntPar(parVal,value);
	}
	if ( operValueType == "INT" && parValType == "FLOAT")
	{
		flg = floatParToIntPar(parVal,value);
	}
	
	if ( flg ) { return retflg; }
	
	cout << endl 
	<< " *** type mixing problem : " << parVal 
	<< " can not be converter to " << operValueType << endl;
	
	return 0;
	
}


