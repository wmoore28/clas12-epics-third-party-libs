#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include "cxxtranslatorversion.hxx"
#include "name.hxx"
#include "utilities.hxx"
//---------------------------------------------------------------------
//                                                   B. Franek
//                                                   17-March-1999
//---------------------------------------------------------------------

void processCommandLine( int argc, char* argv[],
                        int& dbg, Name& inputFile, Name& outputFile) {

	dbg = 0;
	int input_error = 0;
	inputFile = "\0"; outputFile = "\0";

	for(int i=1; i<argc && !input_error; i++)
	{
		if(strncmp("-v",argv[i],2)==0){
			cout << CXXTranslatorVersion << endl;
			exit(0);
		}
		else if (strncmp("-d",argv[i],2)==0){
			dbg = 1;
		}
		else if(strncmp("-",argv[i],1)==0) {
			cout << "Unknown option requested " << argv[i] << endl;
			input_error =1;
		} 
		else{
			if (inputFile == "\0") {inputFile = argv[i];}
			else if (outputFile == "\0") {outputFile = argv[i];}
			else {
				input_error = 1;
			}
		}
	}

	if (argc == 1) input_error =1;

	if(input_error) {
		cout << endl << "\tusage: " 
			<< argv[0] << " [-v] [-d] input [output]" << endl;
		exit(1);
	}

//--------------------------------------------------------------
	char* pStr = inputFile.getString();
	char* ptnf;
	Name reducedName;

	if (outputFile == "\0") { // Output file is absent 
		if ( (ptnf = strstr(pStr,".smi")) ) {  // contains .smi 
			if (!strcmp(ptnf,".smi")) { // it is at the end 
				*ptnf ='\0';
			}
			reducedName = pStr; 
		}
		else {
			reducedName = inputFile;
		}
		inputFile = reducedName;
		inputFile += ".smi";
		outputFile = reducedName;
		outputFile += ".sobj";
	}
	return;
}
