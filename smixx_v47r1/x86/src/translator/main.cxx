
//----------------------   M a i n   P r o g r a m  --------------------------
//                         SMI Translator         
//
//                                                Author : Boda Franek
//                                                Date : 14 December 1998
//
// Copyright Information:
//      Copyright (C) 1999/2000 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "smixx_common.hxx"
#include <assert.h>
#include "smlunit.hxx"
#include "utilities.hxx"
#include "namevector.hxx"
#include "name.hxx"
#include "smlline.hxx"
#include "smllinevector.hxx"
#include "nmdptnr.hxx"
#include "nmdptnrvector.hxx"
#include "registrar.hxx"
#include "smiobject.hxx"
#include "isofclassobject.hxx"
#include "smiobjectset.hxx"
#include "smiobjectsetunion.hxx"
//#include "ut_tr.hxx"
#include "errorwarning.hxx"

//----------------------  Externals  -------------------------------


	Registrar allUnits;

	Registrar allClasses, allObjects, allObjectSets;

void processCommandLine( int argc, char* argv[],
                        int& dbg, Name& inputFile, Name& outputFile);


//-----------------------------------------------------------------------------

int main(int argc, char* argv[]) {

  Name inputFile, outputFile; int dbg;

	SMLline notApplicable(-1," line information not applicable");
	
  processCommandLine(argc,argv,dbg,inputFile,outputFile);

//  cout << " Input file : |" << inputFile << "| \n"; 

  char* pStr = inputFile.getString();
  ifstream input_file(pStr);

//cout << " ifstream instantiated " <<endl;

  if (input_file.fail()) {
  	ErrorWarning::printHead("ERROR",notApplicable);
	cout << "  Could not open file :" << inputFile << endl;
        exit(2);
  }

//cout << " File open " << endl;

  pStr = outputFile.getString();
  ofstream output_file(pStr);

  if (output_file.fail()) {
  	ErrorWarning::printHead("ERROR",notApplicable);
	cout << "  Could not open file :" << outputFile << endl;
        exit(2);
  }


// Going to scan the file. At this point we are searching for either 
// objects, classes or Object sets by their keywords

	int linenum = 0; int lin;
	Name line; 

//  Name name, objectName, className, objectSetName;


	int newUnit = 0;	
	SMLUnit* pUnit = 0;
	Name unitType = "\0";
	Registrar* pTypeRegistrar = 0;
	Name name ="\0";
	Name registeredName = "\0";



	int assoc;
//  Attention...there is a bug somewhere. The last line is not read in(3 Oct 1999)

	int specCode = 1; int nspec = 0;

	for (;;) {       // Beg. collection loop
		if (specCode) {
			if (specialCode(nspec,line)) {
			//	SMLline smlLine(-1,line);
				lin = -1;
				nspec++;
			}
			else {
				specCode = 0;
				continue;
			}
		}
		else {

			int flag = readLine(input_file,line);

			if ( flag == 0 ) {
				SMLline smlLine(linenum,line);
				ErrorWarning::printHead("ERROR",smlLine
				,"Error reading file");
				exit(2);
			}
			if ( flag == 2 ) {
				input_file.close();
				break;
			}
			if ( flag != 1 ) {
				SMLline smlLine(linenum,line);
				ErrorWarning::printHead("ERROR",smlLine
				,"Error reading file");
				
				cout << endl << " unknown flag " 
				<< flag << endl;
				cout.flush();
				exit(2);
			}
     
			//SMLline smlLine(linenum,line);
			lin = linenum;
			linenum++;
		}

		SMLline smlLine(lin,line);
		//cout << " Line read in :\n"; smlLine.out();

		int primKeywordLine = smlLine.primKeyword( name, assoc);
		//cout << " primKeywordLine: "<< primKeywordLine << endl;		
		if ( primKeywordLine == -1 && pUnit == 0 ) {
			continue;     //just initial comment lines...skip
		} 

		pTypeRegistrar = 0;

		if ( primKeywordLine == 1 ) {
			newUnit = 1;
			pUnit = new SMIObject(name,1,assoc);
			assert (pUnit != 0);
			unitType = "Class";
			pTypeRegistrar = &allClasses;
			registeredName = name;
		}
		else if ( primKeywordLine == 2 ) {
			newUnit = 1;
			pUnit = new SMIObject(name,0,assoc);
			assert (pUnit != 0);
			unitType = "Object";
			pTypeRegistrar = &allObjects;
			registeredName = name;
		}
		else if ( primKeywordLine == 3 ) {
			newUnit = 1;
			pUnit = new IsOfClassObject(name,assoc);
			assert (pUnit != 0);
			unitType = "IsOfClassObject"; 
			pTypeRegistrar = &allObjects;
			registeredName = name;
		}
		else if ( primKeywordLine == 4 ) {
			newUnit = 1;
			pUnit = new SMIObjectSet(name);
			assert (pUnit != 0);
			unitType = "ObjectSet";  
			pTypeRegistrar = &allObjectSets;
			registeredName = name;
		}
		else if ( primKeywordLine == 5 ) {
			newUnit = 1;
			pUnit = new SMIObjectSetUnion(name);
			assert (pUnit != 0);
			unitType = "ObjectSetUnion";  
			pTypeRegistrar = &allObjectSets;
			registeredName = name;
		}

		else {
			if ( pUnit == 0) {
				ErrorWarning::printHead("ERROR",smlLine);
				cout << " Looking for CLASS, OBJECT or OBJECTSET" 
                		<< endl;
				exit(2);
			}
		}
		if (newUnit) {
			newUnit = 0;
//cout << " Registering a new unit  type : " << unitType << endl;
			allUnits.addRegardless(unitType,pUnit);
			if (pTypeRegistrar) {
				int iflag = pTypeRegistrar->add(registeredName,pUnit);
				if (iflag != 1) {
					ErrorWarning::printHead("ERROR",smlLine);
					cout << unitType  << registeredName
				  	<< " has allready been declared" << endl;
					exit(2);
				}
			}
		}
		if (pUnit != 0 ) { pUnit->acceptLine(smlLine);}       
	} // end of collection loop

  Name tmpName; void* ptnvoid;

 
  int numOfUnits = allUnits.length();
//  cout << "Num of units : " << numOfUnits << endl;


	SMIObject* pSMIObject; IsOfClassObject* pIsOfClassObject;
	SMIObjectSet* pSMIObjectSet;
	SMIObjectSetUnion* pSMIObjectSetUnion;

//   --------------------- Translation Stage -----------------------
	for ( int iu=0; iu<numOfUnits; iu++ )
	{  //Translation loop
		Name type = allUnits.gimeName(iu);
		ptnvoid = allUnits.gimePointer(iu);
		pUnit = (SMLUnit*)ptnvoid;

//cout << " Translating " << pUnit->unitId() << pUnit->unitName() << endl;
		pUnit->translate();

	}   // end of translation loop
//   ----------------------------------------------------------------

//cout << " All units:" << endl; allUnits.out();
//cout << " All Classes:" << endl; allClasses.out();
//cout << " All Objects:" << endl; allObjects.out();

//   --------------------- Examination Stage -----------------------	
	int anyErrors(0), iflg(0);
	for ( int iu=0; iu<numOfUnits; iu++ )
	{  // Examination loop
		Name type = allUnits.gimeName(iu);
		ptnvoid = allUnits.gimePointer(iu);
		pUnit = (SMLUnit*)ptnvoid;

//cout << " Examining " << pUnit->unitId() << pUnit->unitName() << pUnit->unitType() << endl;
		iflg = pUnit->examine();
		if (iflg != 0) anyErrors = 1;
	
	}  // end of examination loop
//   -----------------------------------------------------------------
	
	if ( anyErrors == 1 )
	{
		cout << endl << endl
		<< "  ***** The code has not passed the examination stage *****" << endl
		     << "  *****                 No .sobj file produced        *****" << endl;
		return 1;
	}

//   --------------------- Output Stage -----------------------	

	for ( int iu=0; iu<numOfUnits; iu++ )
	{  //Output Loop
		Name type = allUnits.gimeName(iu);
		ptnvoid = allUnits.gimePointer(iu);
		pUnit = (SMLUnit*)ptnvoid;

//	cout << iu << endl;
//	Name offset = "|"; pUnit->out(offset);
		
//		pUnit->outSobj(output_file);

		if (type == "Object" || type == "Class") {
			pSMIObject = (SMIObject*)ptnvoid;
			pSMIObject->outSobj(output_file);
		}
		else if (type =="IsOfClassObject") {
			pIsOfClassObject =(IsOfClassObject*)ptnvoid;
			pIsOfClassObject->outSobj(output_file);
		}
		else if (type == "ObjectSet") {
			pSMIObjectSet = (SMIObjectSet*)ptnvoid;
			pSMIObjectSet->outSobj(output_file);
		}
		else if ( type == "ObjectSetUnion" ) {
			pSMIObjectSetUnion = (SMIObjectSetUnion*)ptnvoid;
			pSMIObjectSetUnion->outSobj(output_file);
		}
		else {
			ErrorWarning::printHead("ERROR",notApplicable);
			cout << " internal error at output stage" ;
			cout << " unknown type : " << type << endl;
			exit(2);
		}

	}   // End of Output loop
		
return 0;
 
}
