//----------------------   INITIATOR  --------------------------
#include "parameters.hxx"
#include "smixx_common.hxx"
#include <assert.h>

#include "smiobject.hxx"
#include "smiclass.hxx"
#include "smiobjectsetsimple.hxx"
#include "smiobjectsetunion.hxx"
#include "registrar.hxx"
#include "objectregistrar.hxx"
#include "namevector.hxx"
//extern Registrar allSMIObjectSets;
extern ObjectRegistrar allSMIObjects;
extern Registrar allSMIClasses;
extern int dbg;

void createAlloc
     (const Name& externDomain, int& numOfObjects);

//  Will read all the records from the SOBJ file searching for objects or
//  SMI classes. For each of these it collects all the records in lines and 
//  calls the SMIObject creator.
//
//                                                   20-August-1996
//                                                    B.Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------
typedef char OBJLINE[MAXRECL];

void initiator(ifstream *pSOBJFile) {


   char line[MAXRECL]; Name lineNm;


// -------------------------------------------------------------------


   int numOfObjects(0);
   int numOfClasses(0);
   int numOfObjectSets(0);

   int smiClass = 0;
   int smiObject = 0;
   int smiObjectSet = 0;

   int unionFlg;

   int expected;   // when =1 it means that *OBJECT or *CLASS or *OBJECTSET record is expected
   expected = 1;


  for (;;) {

     NameVector* pObjectCode = new NameVector();
     assert(pObjectCode != 0);
          
     for (;;) {
        pSOBJFile->getline(line,MAXRECL);
        lineNm = line; (*pObjectCode)+= lineNm;

        if (pSOBJFile->eof() ) {
           pSOBJFile->close();
           if ( expected == 1 ) {
	      delete pObjectCode;
              goto EndOfFile;
           }
           else {
              cout << " This is unexpected and therefore an error \n";
              cout.flush();
              exit(0);
           }
        }
        else if (pSOBJFile->bad()) {
           cout << "Error reading file\n";
           cout.flush();
           exit(0);
        }

        else if (!strcmp(line,"*OBJECT") ) {
           if ( expected == 1 ) {
              smiObject = 1;
              smiClass = 0;
              smiObjectSet = 0;
              expected = 0;
           }
           else {
              cout << " *OBJECT expected, read: \n  |"
                   << line << "| \n";
              cout.flush();
              exit(0);
           }
        }        

        else if (!strcmp(line,"*CLASS") ) {
           if ( expected == 1 ) {
              smiObject = 0;
              smiClass = 1;
              smiObjectSet = 0;
              expected = 0;
           }
           else {
              cout << " *OBJECT expected, read: \n  |"
                   << line << "| \n";
              cout.flush();
              exit(0);
           }
        } 

        else if ( strcmp(line,"*OBJECTSET") == 0  || 
	          strcmp(line,"*OBJECTSETUNION") == 0
		)
	{
           if ( expected == 1 ) {
              smiObject = 0;
              smiClass = 0;
              smiObjectSet = 1;
              expected = 0;
	      if ( strcmp(line,"*OBJECTSET") == 0 ) {unionFlg = 0;}
	      else { unionFlg = 1; }
           }
           else {
              cout << " *OBJECT expected, read: \n  |"
                   << line << "| \n";
              cout.flush();
              exit(0);
           }
        }        
      

        else if (!strcmp(line,"*END_OBJECT") ) {
           if ( expected == 1 ) {
              cout << " *OBJECT expected, read: \n  |"
                   << line << "| \n";
              cout.flush();
              exit(0);
           }
           if ( smiObject != 1) {
              cout << "  incorrect end record " << endl;
              exit(0);
           }
           smiObject = 0;
//         ------------- SMI Object lines collected....instantiate! ----------


           int no_lines = pObjectCode->length();
	   
	   char* pCode = new char[no_lines*MAXRECL];
           OBJLINE* lines = (OBJLINE*) pCode;   
          
	   for (int il=0;il<no_lines;il++) {
	      lineNm = (*pObjectCode)[il];
	      strcpy(lines[il],&lineNm[0]);
	   }
	   
           SMIObject *pSMIObj;

#ifdef DEBUG
           cout << "\n";
           cout.flush();
#endif
           pSMIObj = new SMIObject(&lines[0]);
           assert (pSMIObj != 0 );
           delete [] pCode; delete pObjectCode;
           numOfObjects++;

           Name externDomain = pSMIObj->externDomain();
           if ( externDomain == "\0" ){}
           else { createAlloc(externDomain,numOfObjects);}

           expected = 1;
	   break;
//         -----------------------------------------------------------------
        }

        else if (!strcmp(line,"*END_CLASS") ) {
           if ( expected == 1 ) {
              cout << " *OBJECT expected, read: \n  |"
                   << line << "| \n";
              cout.flush();
              exit(0);
           }
           if ( smiClass != 1) {
              cout << "  incorrect end record " << endl;
              exit(0);
           }
           smiClass = 0;

//         ------------- SMI Class lines collected....instantiate! ----------

           int no_lines = pObjectCode->length();
	   
	   char* pCode = new char[no_lines*MAXRECL];
           OBJLINE* lines = (OBJLINE*) pCode;   
     
	   for (int il=0;il<no_lines;il++) {
	      lineNm = (*pObjectCode)[il];
	      strcpy(lines[il],&lineNm[0]);
	   }


           SMIClass *pSMICla;

           pSMICla = new SMIClass(&lines[0]);
           assert (pSMICla != 0 );
           delete [] pCode; delete pObjectCode;

           numOfClasses++;
           expected = 1;
           break;
//         -----------------------------------------------------------------
        }
        else if ( strcmp(line,"*END_OBJECTSET") == 0 ||
	          strcmp(line,"*END_OBJECTSETUNION") == 0
		)
	{
           if ( expected == 1 ) {
              cout << " *OBJECT expected, read: \n  |"
                   << line << "| \n";
              cout.flush();
              exit(0);
           }
           if ( smiObjectSet != 1 ||
	        (strcmp(line,"*END_OBJECTSET") == 0 && unionFlg == 1) ||
		(strcmp(line,"*END_OBJECTSETUNION") == 0 && unionFlg == 0)
	      )
	   {
              cout << "  incorrect end record " << endl;
              exit(0);
           }
           smiObjectSet = 0;

//         ------------- SMI Object Set lines collected....instantiate! ----------

           int no_lines = pObjectCode->length();
	   
	   char* pCode = new char[no_lines*MAXRECL];
           OBJLINE* lines = (OBJLINE*) pCode;   
     
	   for (int il=0;il<no_lines;il++) {
	      lineNm = (*pObjectCode)[il];
	      strcpy(lines[il],&lineNm[0]);
	   }


           if (unionFlg == 0 )
	   {
		SMIObjectSetSimple* pSMIObjectSetSimple;

                pSMIObjectSetSimple = new SMIObjectSetSimple(&lines[0]);
                assert (pSMIObjectSetSimple != 0 );
           }
           else
	   {
                SMIObjectSetUnion* pSMIObjectSetUnion;

                pSMIObjectSetUnion = new SMIObjectSetUnion(&lines[0]);
                assert (pSMIObjectSetUnion != 0 );
           }
	   
           delete [] pCode; delete pObjectCode;

           numOfObjectSets++;
           expected = 1;
           break;
//         -----------------------------------------------------------------
        }
        else  {
           if (expected != 0) {
               cout << " Unknown record : \n |" << line << "|" << endl;
               exit(0);
           }
        }
     }
  }   // object loop
//---------------------- Everything is read in ---------------------------


  EndOfFile:

  if ( dbg > 1) {
      cout << "End of file \n  \n";
  }
  
	if ( dbg > 0 )
	{
		cout    << " Number of SMI Objects instantiated : " << numOfObjects << endl;
		cout    << " Number of SMI Classes instantiated : " << numOfClasses << endl;
		cout    << " Number of Object Sets instantiated : " << numOfObjectSets << endl;
	}

  return;

}
void createAlloc(const Name& externDomain, int& numOfObjects) {

   static  char lines[5][MAXRECL] = { "*OBJECT",
                                     "object name",
                                     "1    0    1",
                                     "&ALLOCATION_OBJECT",
                                     "*END_OBJECT" };

    Name allocationObjectClass = "&ALLOCATION_OBJECT";
   SMIClass *temp = static_cast<SMIClass*>(allSMIClasses.gimePointer(allocationObjectClass));
   if ( temp == 0 ) {
       cout << " Class " << allocationObjectClass 
            << "  does not exist...ancient version of translator" << endl;
       exit(1);
   }   // This class does not exists...old sobj


   Name allocObject = externDomain;
   allocObject += "::&ALLOC";

   SMIObject *ptnr = allSMIObjects.gimePointer(allocObject);

   if (ptnr) { return; }  // already exists

   allocObject.whatAreYou(lines[1],MAXRECL);

   SMIObject *pSMIObj = new SMIObject(&lines[0]);
   assert ( pSMIObj != 0 );
   numOfObjects++;

}
