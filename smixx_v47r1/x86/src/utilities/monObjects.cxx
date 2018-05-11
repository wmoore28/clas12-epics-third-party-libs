//===========================================================================
//
//                                      Boda Franek   Dec 2005
// Copyright Information:
//      Copyright (C) 2005 CCLRC. All Rights Reserved
//
// $Id: monObjects.cxx,v 1.23 2007/08/08 09:46:42 clara Exp $
//===========================================================================

#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <istream>
#include <string>
#include <vector>
#include <dis.hxx>
using namespace std;
#include <stdlib.h>
#include <smiuirtl.hxx>
#include "smixx_common.hxx"

#include <ctype.h>
#include "getTimeString.c"

void removeObject(char* name);
void registerObject(char* name);
void sendCommandToObject(char* commandString);

const static int timer_tick(60);

//static int logflg;
static ofstream logFile;

class MyObject;


MyObject* findObject(char* name);
MyObject* pFirstRegisteredObject = 0;

//==========================================================================
class Timer:public DimTimer
{
public:
    Timer(int time) : DimTimer(time) {};
    void timerHandler();
};
//==========================================================================
void Timer::timerHandler() {
    string timeString = getTimeString();

    cout << timeString.substr(4,15) << "/*time-tick" << endl;

    stop();
    start(timer_tick);
}
//==========================================================================
class MyObject: public SmiObject
{    
    void  smiStateChangeHandler();
    void smiExecutingHandler();
    void getPars (string& parsString);  
    void getActionPars(SmiAction* ptrAct, string& parsString); 
    void getParam(SmiParam* ptrPar, string& parString);
    

public:
    MyObject(char* fullObjName): SmiObject(fullObjName) { }
    	MyObject* prev;
    	MyObject* next;
};
//===========================================================================
class Command: public DimCommand 
{ 
	void commandHandler() 
	{ 
//        cout << "Received : " << getString() << endl;
		
		if ( strncmp(getString(),"register/",9) == 0 )
		{
			registerObject(getString()+9);
		}
		else if ( strncmp(getString(),"remove/",7) == 0 )
		{
			removeObject(getString()+7);
		}
		else if ( strncmp(getString(),"exit/",5) == 0 )
		{
			exit(0);
		}
				
		else if ( strncmp(getString(),"command/",8) == 0 )
		{
			sendCommandToObject(getString()+8);
		}
		else
		{
//			cout << " *** monObjects: unknown command" << endl;
		}
	} 
	public: 
		Command(const char* serviceName);
}; 
//==========================================================================
Command::Command(const char* serviceName)
	: DimCommand(serviceName,"C")
{
}
//==========================================================================
int convertStringToHex(char* outstr, char* instr, int noutMax)
{
/* It will convert input character string to a string, in which every character on input
  is interpreted as integer and is represented by exactly two characters. For example:
  character 7  is represented by character 3 followed by chacter 7 or
  BELL character is represented by character 0 followed by character 7 
  instr   is a pointer to the input string
  outstr  is a pointer to the output string
  noutMax is the max number of characters in the output string. when zero,
          the function simply returns the number of characters that would be output
	  into the outstr */
	  
	int i;
	int ich;
	char* pin;
	char* pout;
	
	int lenin = strlen(instr);
	if ( lenin == 0 )
	{
		if ( outstr != NULL )
		{
			*outstr = '\0';
		}
		return 0;
	}
	
	int nout = 2*lenin;
	if ( noutMax == 0 ) { return nout; }
	if ( nout > noutMax )
	{ 
		printf(" Not enough space in the output string \n");
		return -1;
	}
	
	pin = instr;
	pout = outstr;
	
	for ( i=0; i<lenin; i++)
	{
		ich = static_cast<int>(*(pin+i));
		sprintf(pout,"%02X",ich);
		pout = pout + 2;
	}
	
	*pout = '\0';
	return nout;	
}
//============================================================================
int convertHexToString(char* outstr, char* instr, int noutMax)
{
/* Each pair of characters (starting with the 0th and the 1st) is interpreted
   as hex number and converted into the corresponding character. For example
   the pair 07 is converted into BELL character or
   the pair 37 is converted into character 7
   It also follows, that number of characters in the input string has to be
   even.
   instr is pointer to the input string
   outstr is pointer to the output string
   noutMax  is the max number of characters in the output string. when zero,
          the function simply returns the number of characters that would be output
	  into the outstr */
   
	int i;
	int ich;
	char* pin;
	char* pout;
	char buf[3];
	
	int lenin = strlen(instr);
	if ( lenin == 0 )
	{
		if ( outstr != NULL )
		{
			*outstr = '\0';
		}
		return 0;
	}
	int nout = lenin/2;
	
	if ( (lenin % 2) != 0 )
	{
		printf(" No of characters in the input string is not even \n");
		return -1;
	}
	if ( noutMax == 0 ) { return nout; }
	if ( nout > noutMax )
	{ 
		printf(" Not enough space in the output string \n");
		return -1;
	}
	
	pin = instr;
	pout = outstr;

	for ( i=0; i<lenin; i = i + 2 )
	{
		buf[0] = *(pin+i); buf[1] = *(pin+i+1);
		sscanf(buf,"%X",&ich);
		*pout = static_cast<char>(ich);
		pout = pout +1;
	}
	
	*pout = '\0';
	return nout;
}
//==========================================================================
void MyObject::getParam(SmiParam* ptrPar, string& parString) {
    int parType, hasValue;
    
    parString = "";
    
    parType = ptrPar->getType();
    hasValue = ptrPar->hasValue();
	
    parString = parString + "/" + ptrPar->getName() ; 
	
    if (parType == 0) { parString += "(S)";}
    else if (parType == 1) { parString += "(I)";}
    else if (parType == 2) { parString += "(F)";}
	
    if (hasValue) {
	parString += "=";
        if (parType == 0) {
		char* point = ptrPar->getValueString();
		/*int pica = point[0];
		printf("\n\n parameter %s \n value :<%x>\n",ptrPar->getName(),pica);*/
		int nout = convertStringToHex(NULL,point,0);
		if ( nout < 0 ) { exit (1);}
		char* tempVal = new char[nout+1];
		convertStringToHex(tempVal, point, nout);
		//printf("nout %d tempVal <%s>\n",nout,tempVal);
			
		parString += tempVal;
		delete [] tempVal; 
	}
	else if (parType == 1) {
            int vali=ptrPar->getValueInt();
	    char cvali[10];
	    sprintf(cvali,"%i",vali);
	    parString += cvali;
        }
        else if (parType == 2) {
            double  valf=ptrPar->getValueFloat();
            char cvalf[15];
	    sprintf(cvalf,"%e",valf);
	    parString += cvalf;
	}
    }    
    return;
}
//==========================================================================
void MyObject::getPars(string& parsString) {
    SmiParam* ptrPar;
    string parString;
    int first = 0;
    
    parsString = "";

        
    while ( (ptrPar = getNextParam()) ) {
        if ( first == 0 ) {
	    first = 1;
	    parsString = "/*pars";
	}
	getParam(ptrPar,parString);
	parsString += parString;
    }
    if (first == 1) {parsString += "/*end_pars";}
    return;
}
//==========================================================================
void MyObject::getActionPars(SmiAction* ptrAct,string& parsString) {
    SmiParam* ptrPar;
    string parString;
    int first = 0;
    
    parsString = "";
        
    while ( (ptrPar = ptrAct->getNextParam()) ) {
        if ( first == 0 ) {
	    first = 1;
	    parsString = "/*pars";
	}
	getParam(ptrPar,parString);
	parsString += parString;
    }
    if ( first == 1) {parsString += "/*end_pars";}
    return;
}
//============================================================================
void MyObject::smiStateChangeHandler() {
    char* state = getState();
    string timeString = getTimeString();
    SmiAction* ptrAct;
    string parsString;
    
    cout << timeString.substr(4,15) << "/*object/" 
         << getName() << "/*state/";
    
    if (state != 0) {
        cout  << state  ;
	getPars(parsString);
	cout << parsString;
	cout << "/*end_state";
	
	while ( (ptrAct = getNextAction()) ) {
	    cout << "/*action" ;
	    cout << "/" << ptrAct->getName();
	    getActionPars(ptrAct,parsString);
	    cout << parsString;
	    cout << "/*end_action";
	}
	cout << endl;
    }
    else {
        cout << "&DEAD/*end_state" << endl ;
    }
}
//===========================================================================
void MyObject::smiExecutingHandler() {
    string timeString = getTimeString();

    if ( getBusy() ) {
        cout << timeString.substr(4,15) << "/*object/" << getName() << "/*state/";
        cout << "&Busy/*end_state" << endl;
    }
}

//===========================================================================
void testVersion(ifstream& infile) {
    string basicString = "Dec 10 14:38:32/*object/&o/*state/READY&n/*pars/PAR&n(S)=UNDEFINED/BPAR&n(S)/CPAR&n(I)=6/DPAR&n(F)=1.700000e+00/*end_pars/*end_state/*action/GOTO_SECOND&n/*pars/APAR&n(S)=5/BPAR&n(S)/*end_pars/*end_action/*action/STAY_HERE&n/*pars/CPAR&n(S)/*end_pars/*end_action";
    string objName;
    string outString; int i;

    int num = 0; char cnum[4];

    while (getline((istream&)infile,objName,'\n')) {
        num++;
        sprintf(cnum,"%i",num);
	outString = basicString;
        i = outString.find("&o",0);
        outString.replace(i,2,objName.c_str());
        while ( 1 == 1 ) {
            i = outString.find("&n",i+1);
            if ( i == (int)string::npos ) break;
            outString.replace(i,2,cnum);
        }
        sleep(1);
        cout << outString << endl;
    }

    return;
}
//===========================================================================
void registerObject(char* name) {

	MyObject* ptnrObject;
	MyObject* ptr;
    
	ptnrObject = new MyObject(name);

	if ( pFirstRegisteredObject == 0 )
	{  /* the first object to be registered */
		pFirstRegisteredObject = ptnrObject;
		ptnrObject->prev = 0;
		ptnrObject->next = 0;
	}
	else
	{
	/***  check whether the object was already registered */
		if ( findObject(name) != 0 ) 
		{
			cout << "error/Object " << name << " already registered" << endl;
			return;
		}
		
		ptr = pFirstRegisteredObject;
		while ( ptr->next != 0 ) ptr = ptr->next; 
		
		ptr->next = ptnrObject; /*put the new object at the end */
		ptnrObject->prev = ptr;
		ptnrObject->next = 0;  /* mark the end of the list */
	}
					
	return;
}
//==============================================================================
void removeObject(char* name) {
	string timeString = getTimeString();

	MyObject* ptnrObject;
	MyObject* ptnrNext;
	MyObject* ptnrPrev;
	
	ptnrObject = findObject(name);
	
	if ( ptnrObject == 0 ) 
	{
		cout << timeString.substr(4,15)
		     << "/*error/Object " << name << " is not registered"
		      << endl;
		return;
	}
	
	ptnrPrev = ptnrObject->prev;
	ptnrNext = ptnrObject->next;
	
	if ( ptnrObject == pFirstRegisteredObject )
	{   /* It is the first object */
		pFirstRegisteredObject = ptnrNext;
		if ( ptnrNext == 0 )
		{  /* it is the only object */
		}
		else 
		{
			ptnrNext->prev = 0;
		}
	}
	else 
	{
		ptnrPrev->next = ptnrNext;
		if ( ptnrNext == 0 )
		{  /* it is the last object */
		}
		else
		{
			ptnrNext->prev = ptnrPrev;
		}
	}
	delete ptnrObject;
	cout << timeString.substr(4,15)
		<< "/*info/removed/" << name << endl;
	return;
} 
//===========================================================================
SmiAction* findAction(MyObject* ptr, char* actionName) {
	SmiAction* pa;
	
	while ( (pa = ptr->getNextAction()) )
	{
		if ( strcmp(pa->getName(),actionName) == 0 )
		{
			return pa;
		}
	}
	return 0;
}
//===========================================================================
MyObject* findObject(char* name) {
	MyObject* ptr;
	
	ptr = pFirstRegisteredObject;

	while ( ptr != 0 )
	{
		if ( strcmp(name,ptr->getName()) == 0 )
		{
			return ptr;
		}
	 	ptr = ptr->next;
	} 
	return 0;
}
//===========================================================================
void sendCommandToObject(char* commandString) {
	char* ptemp;
	char* pObjectName;
	char* pActionName;
	char* pParString;
// Expected string format:
//  <object-name>/<action-name>/<par1-name> <type1> <val1>/<par2-name> <type2> <val2>	
	
	ptemp = new char[strlen(commandString)+1];
	strcpy(ptemp,commandString);
//	cout << "Received command :" << ptemp << endl;
	
	pObjectName = strtok(ptemp,"/");
	
	pActionName = strtok(NULL,"/");
	
//	cout << "Object |" << pObjectName << "| Action : |" << pActionName << "|" << endl;
	
	
	MyObject* ptrObj = findObject(pObjectName);
	if ( ptrObj == 0 ) {
		cout << " Object |" << pObjectName << "| not found" << endl;
		return;
	}
//	cout << " Object |" << ptrObj->getName() << "| found \n" << endl;

//	cout << "|" << pObject << "|  |" << pAction << "|" << endl;
	SmiAction* pa = findAction(ptrObj,pActionName); 
	if ( pa == 0 )
	{
		cout << " Action |" << pActionName << "| not found" << endl;
		return;
	}
//	cout << " Sending " << pActionName << endl;

	
        while ( (pParString = strtok(NULL,"/")) )
	{
//		cout << pParString << endl;
		char* pName = pParString;
		char* pNameEnd = strstr(pName," "); *pNameEnd = '\0';
		char* pType = pNameEnd + 1; *(pType+1) = '\0';
		char* pVal = pType + 2;
//		cout << "monObjects....Name :|" << pName << "| Type :|" << pType << "| Value :|" << pVal << "|" << endl;
		if ( *pType == 'S' ) 
		{
			int nout = convertHexToString(NULL,pVal,0);
			if ( nout < 0 ) { return;}
			char* tempVal = new char[nout+1];
			convertHexToString(tempVal, pVal, nout);
//			cout << " Converted value |" << tempVal << "|" << endl;
			pa->setParam(pName,tempVal);
//			cout << " iflg : " << iflg << endl;
			delete [] tempVal;
		}
		if ( *pType == 'I' ) 
		{
			int iVal;
			sscanf(pVal,"%d",&iVal);
//			cout << " iVal : " << iVal << endl;
			pa->setParam(pName,iVal);
//			cout << " iflg : " << iflg << endl;			
		}
		if ( *pType == 'F' ) 
		{
			double dVal;
			sscanf(pVal,"%lf",&dVal);
//			cout << " dVal : " << dVal << endl;
			pa->setParam(pName,dVal);
//			cout << " iflg : " << iflg << endl;	
		}		
	} 

	pa->send();
	delete [] ptemp;

	return;
}
//===========================================================================
int getNode(string& node) {
	char hostname[256];
	int flg;
	
#ifdef WIN32
	int init_sock();
	init_sock();
#endif	
	flg = gethostname(hostname,255);
	if ( flg != 0 ) {
		return 1;
	}
//	cout << flg << hostname << endl;
	node = hostname;
	
	return 0;
}	
//===========================================================================
int main(int argc, char* argv[]) {

	string serverName; 
	if ( getNode(serverName) != 0 ) {
		cerr << "Error  Can not determine pid/host " << endl;
		exit(1);
	}
	
	if ( argc == 2 ) {
		serverName = serverName + "/" + argv[1];
	}
	serverName = serverName + "/SMIGUI";
		
	string serviceName = serverName + "/CMNDS";
	Command cmnd(serviceName.c_str());
	
	DimServer::start(serverName.c_str());

/*    if ( argc <= 1 ) {
        cerr << "Error No object file specified" << endl;
        exit(1);
    }
       
    string objectFile;
    objectFile = argv[1];
    
    logflg = 0; 

    if (argc > 1) {
        logflg = 1;
	string logFileName;
	logFileName = argv[2];

        logFile.open(logFileName.c_str());

    }
 
      
    FILE *pObjectFile = fopen(objectFile.c_str(),"r+");

    if (pObjectFile == NULL) {
        cerr << "Failed to open object file " << objectFile << endl;
	exit(21);
    }

    if (logflg) logFile << " Opened object list :" << objectFile << endl;
    
    ifstream infile (objectFile.c_str());
    string objName;

    getline(infile,objName,'\n');
    if ( objName == "*test" ) {
        testVersion(infile);
        fclose(pObjectFile);
	while ( 1 ) { sleep(10);}
    }
    */
// This is the real thing...check the dns server
    
    if ( getenv("DIM_DNS_NODE") == 0 ) {
        cerr << "***Error monObjects DIM_DNS_NODE is undefined" << endl;
        exit(11);
    } 
    
    DimCurrentInfo dns("DIS_DNS/VERSION_NUMBER",2,-1);
    if ( dns.getInt() == -1 ) {
        cerr << "***Error monObjects Name server is not running" << endl;
        exit(12);
    } 
 /*      
    void* refistream;
    while ( 1 ) {
        registerObject(objName);
        refistream = getline(infile,objName,'\n');
	if (!refistream) break;
    }
    fclose(pObjectFile);
   */ 
    Timer timer(timer_tick);

    while ( 1 ) { sleep(10);}
    
    exit(0);


}


#ifdef WIN32
int init_sock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	static int sock_init_done = 0;

	if(sock_init_done) return(1);
 	wVersionRequested = MAKEWORD( 2, 0 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) 
	{
    	return(0);
	}

	/* Confirm that the WinSock DLL supports 2.0.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.0 in addition to 2.0, it will still return */
	/* 2.0 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 0 ) 
	{
	    WSACleanup( );
    	return(0); 
	}
	sock_init_done = 1;
	return(1);
}
#endif

