#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "name.hxx"
#include "version.hxx"
//---------------------------------------------------------------------
//                                                   B. Franek
//                                                   29 March 1999
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------
char* pCL_bareFileName(char* fullName)
{
	char* tok;
	char* prevtok;
	char dels[] = "/\\";
	
	tok = strtok(fullName,dels);
	prevtok = tok;
		
	while( (tok = strtok(NULL,dels))  )
	{	
		prevtok = tok;
	}	

	return prevtok;
}

void processCommandLine( int argc, char* argv[],
                        int& dbg,
		        Name& dim_dns_node,
			int& unlockedIfs,
			int& newTimeFlg,
		        Name& domain, Name& sobjFile) {

        dbg = -1;
	unlockedIfs = -1;
	newTimeFlg = 0;
	dim_dns_node = "\0";
	int input_error = 0;
	domain = "\0"; sobjFile = "\0";

	for(int i=1; i<argc && !input_error; i++)
	{     
		if(!strcmp("-v",argv[i])){
			cout << "   Version  - " << smixxVersion << "-     Date : " << smixxDate << endl;
			exit(0);
		}
		else if (!strcmp("-d",argv[i])){
		        i++;
			if (! sscanf(argv[i],"%d",&dbg) ) {
			    cout << "*** Error processing dbg flag" << endl;
			    input_error = 1;
			}
		}
		else if (!strcmp("-u",argv[i])){
		        unlockedIfs = 1;
		}
		else if (!strcmp("-t",argv[i])){
		        newTimeFlg = 1;
		}

		else if (!strcmp("-dns",argv[i])){
			i++;
			dim_dns_node = argv[i];
		}
		else if(argv[i][0] == '-') {
			cout << "Unknown option requested " << argv[i] << endl;
			input_error =1;
		} 
		else {
			if (domain == "\0") {
                            domain = argv[i]; domain.upCase();
                        }
			else if (sobjFile == "\0") {
                            sobjFile = argv[i];
                        }
			else {
				input_error = 1;
			}
		}
	}
    
	if (domain == "\0" || sobjFile == "\0") input_error =1;

	if(input_error) {
		cout << endl << "\tusage: " << endl << endl
		<< "\t" << pCL_bareFileName(argv[0])
		<< " [-v] [-d \'level\'] [-dns \'host'] [-u] domain-name  sobj-file-name" << endl
		<< "\t-v \t\t\tprints the version of State Manager" << endl
		<< "\t-d \'diagnostic level\' \tdefines the amount of diagnostic prints" << endl
		<< "\t\t\t\tdefault is 3" << endl
		<< "\t-dns \'dns-host\' \tswitches dns node from the current one" << endl
		<< "\t-u \t\t\tswitches off If locking" << endl;
		exit(1);
        }

	char* pStr = sobjFile.getString();
	char* ptnf;

        if ( (ptnf = strstr(pStr,".sobj")) ) {  // contains .sobj 
			if (!strcmp(ptnf,".sobj")) { // it is at the end 
				return;
			}
	}
        sobjFile += ".sobj";
        return;
}
