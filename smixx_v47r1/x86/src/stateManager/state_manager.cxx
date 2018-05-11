//----------------------   M a i n   P r o g r a m  -----------------
//
//                                            Author : B. Franek
//                                            Date : February 1996
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-------------------------------------------------------------------
#include "parameters.hxx"
#include "ut_sm.hxx"
//----------------------  Globals -------------------------------
   int dbg;
   int unlockedIfs;
   int newTimeFlg;
#include "commhandler.hxx"
#include "registrar.hxx"
   Registrar allSMIObjectSets;
#include "objectregistrar.hxx"
   ObjectRegistrar allSMIObjects;
    Registrar allSMIClasses;
#include "queue_name.hxx"
   Queue_Name executableObjectQ;
#include "ifhandler.hxx"
   IfHandler ifHandler;
#include "resumehandler.hxx"
	ResumeHandler resumeHandler;
   Name smiDomain;
   Name allocator("\0");
//---------------------------------------------------------------
#include "smixx_common.hxx"
#include <ctype.h>
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "version.hxx"
#include "clientwhens.hxx"
#include "msg.hxx"
#ifdef _WIN32
  #include <windows.h>
#endif

void logicEngine();
//ObjectRegistrar *initiator(ifstream *p);
void initiator(ifstream *p);
void processCommandLine(int argc, char* argv[],
                        int& dbg, Name& ,int& unlockedIfs,
			int& newTimeFlg,
		        Name& ,Name&);
  

//-----------------------------------------------------------------------------

int main(int argc, char* argv[]) {

  Name inputFile; Name dim_dns_node;
  processCommandLine(argc,argv,dbg,dim_dns_node,unlockedIfs,
                     newTimeFlg,
                     smiDomain,inputFile);
	  
#ifdef _WIN32
  FreeConsole();
#endif
  if ( dbg < 0 ) { dbg = 3; } //if not specified, set it to 3 to make it BaBar compatible
		     
        cout << endl;
        cout << " SMI++ State Manager Version - " << smixxVersion << "- Date : " << smixxDate << endl;
	cout << "                     Diagnostic level : " << dbg << endl;
  
	if (unlockedIfs > 0)
	{
		cout << endl
		<< " +++ W A R N I N G ! +++" << endl
		<< "  Once a condition in an IF is evaluated, the IF's objects are UNLOCKED" << endl;
	}
	else { unlockedIfs = 0; }
	
  if (dim_dns_node == "\0") {}
  else {
      CommHandler::setDnsNode(dim_dns_node.getString());
  }
	

	if ( dbg > 0 )
	{
		cout << " SMI Domain  " << smiDomain << endl ;
	}
	Msg::setup(smiDomain.getString());
	

//---------------------  Open the input SOBJ file -----------------------
  char* pStr = inputFile.getString();
	 
	if ( dbg > 0 || Msg::dllmsgRxPtr != 0 ) {
		Name message;
		message = "input file ";
		message += "<";
		message += inputFile;
		message += ">";
		if ( Msg::dllmsgRxPtr != 0 ) (*Msg::dllmsgRxPtr)(message.getString());
		if (dbg > 0)
		{
		  	cout << message.getString() << endl;
		} 
	}


  ifstream input_file(pStr);

  if (input_file.bad()) {
    cout << "Could not open file\n";
    exit(0);
  }
//------------------------------------------------------------------

	if ( dbg > 0 ) { print_msg("SMI++ Initiator started");}

   initiator(&input_file);  

	if ( dbg > 0 ) { print_msg("SMI++ Initiator Finished");}

// ================   Now establish client whens ================

	int numOfObjects;
	numOfObjects = allSMIObjects.numOfObjects();
	int io;
	SMIObject* pObj;

	for (io=0; io < numOfObjects; io++) {
		pObj = allSMIObjects.gimePointer(io);
		pObj->updateReferencedObjectsandSets();
	}
// ==============================================================
// =================  initialise objects Set Lists ==============
	int numOfSets = allSMIObjectSets.length();
	int is;
	SMIObjectSet* pSet;
	
	for (is = 0; is < numOfSets; is++ )
	{
		pSet = (SMIObjectSet*)allSMIObjectSets.gimePointer(is);
		pSet->informObjectsAboutMembership();
	}
// ===============================================================

// =========   Diagnostics   =============================
/*
// print the client whens for every object

	cout << endl << endl << "   OBJECT C L I E N T S " << endl;
	
	for (int io=0; io < numOfObjects; io++) {
		pObj = allSMIObjects.gimePointer(io);
		cout << endl << " Object " << pObj->name() << endl;
		pObj->printClientWhens();
	}
*/
// print the client whens for overy object set
/*
	int numOfSets = allSMIObjectSets.length();
	int is;
	SMIObjectSet* pSet;
	
	cout << endl << endl << "   OBJECT SET C L I E N T S " << endl;
	
	for (is = 0; is < numOfSets; is++ )
	{
		pSet = (SMIObjectSet*)allSMIObjectSets.gimePointer(is);
		cout << endl << " Object Set" << pSet->name() << endl;
		pSet->printClientWhens();
	}
*/
// print for every object the Object Sets it belongs to
/*
	cout << endl << endl << "   OBJECT SETS object belongs to" << endl;
	
	for (int io=0; io < numOfObjects; io++) {
		pObj = allSMIObjects.gimePointer(io);
		cout << endl << " Object " << pObj->name() << endl;
		pObj->printSetNamesBelongingTo();
	}	
*/	
// print for every object it's actual client whens
/*
	cout << endl << endl << "   Object's actual client whens" << endl;
	
	for (int io=0; io < numOfObjects; io++) {
		pObj = allSMIObjects.gimePointer(io);
		cout << endl << " Object " << pObj->name() << endl;
		ClientWhens clientWhens;
		pObj->gimeCurrentClientWhens(clientWhens);
		clientWhens.out("     ");
	}
*/
// ****************************
/*  debug
   cout << "\n \n  The following objects are registered : \n \n";

  int numOfObjects;
  numOfObjects = allSMIObjects.numOfObjects();

  for (int io=0; io < numOfObjects; io++) {
     SMIObject* ptnr = allSMIObjects.gimePointer(io);
     bool isAssoc = ptnr->associated();
     bool hasAssocAtt = ptnr->hasAssociatedAttribute();
     cout << allSMIObjects.gimeName(io) << "  " << isAssoc << "  " 
     << hasAssocAtt << endl;
     if ( isAssoc != hasAssocAtt ) { cout << "********************************" <<
     endl;}
  }
*/
//************************************************************************

/*   cout << "\n \n  The following object sets are registered : \n \n";

  int numOfObjectSets;
  numOfObjectSets = allSMIObjectSets.length();


  for (int io=0; io < numOfObjectSets; io++) {
     SMIObjectSet* pSMIObjSet;
     void* ptn = allSMIObjectSets.gimePointer(io);
     pSMIObjSet = (SMIObjectSet*)ptn;
     Name temp ="  ";
     cout << endl;
     pSMIObjSet->out(temp);
  }

*/

	if ( dbg > 0 )
	{
		cout << endl;
		print_msg("SMI++   Logic Engine started execution");
	}

   logicEngine();


   exit(1);


}
