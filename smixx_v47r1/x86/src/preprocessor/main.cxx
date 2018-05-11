//----------------------   M a i n   P r o g r a m  --------------------------
//                         SMI Preprocessor         
//
//                                                Author : Boda Franek
//                                                Date : 20 December 1997
// Copyright Information:
//      Copyright (C) 1997-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
#include "smixx_common.hxx"
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "version.hxx"
#include "string.h"
#include "codeblock.hxx"
#include "name.hxx"

int dbg = 0;

void cpreproc(const Name&, const Name&, Name&);
//-----------------------------------------------------------------------------

int main(int argc, char* argv[]) {

//  int c;
//  extern char *optarg;
//  extern int optind, optopt;

  Name inputFile,preprocFile,cPreprocOpts;
  Name outputFile;

//-------------------------- Process the command line ----------------------
	cPreprocOpts = "\0";
	inputFile = "\0"; outputFile = "\0";
	int input_error = 0;  

	for (int i = 1; i<argc && !input_error; i++)
	{
		if(strncmp("-v",argv[i],2)==0){
			cout << " Version   - " 
			<< smixxVersion 
			<< " -    Date : " << smixxDate << endl ;
			exit(0);
		}
		else if (strncmp("-d",argv[i],2)==0){
			dbg = 1;
		}
		else if (strncmp("-p",argv[i],2)==0){
			if (i == (argc-1)) {input_error = 1; break;}
			i++;
			cPreprocOpts = argv[i];
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
	if (inputFile == "\0" || outputFile == "\0") input_error = 1;

	if(input_error) {
		cout << endl << "\tusage: " 
		<< argv[0] 
		<< " [-v] [-d] [-p \"C-preprocessor options\"] input output"
		<< endl;
		exit(1);
	}

//--------------------------------------------------------------------------

// First do C Preprocessor step
  cpreproc(cPreprocOpts,inputFile,preprocFile);

  CodeBlock* pprevBlock = new CodeBlock(preprocFile);
  CodeBlock* pnextBlock = new CodeBlock();
  int pass(0);
  
  for (;;) {
//      cout << endl << endl;
//      cout << " Pass :" << pass << endl << endl;
//      pprevBlock->out();

//      if (pass == 1) { pprevBlock->outToFile("work:pass0.pre_smi");}
      
      if (pprevBlock->anyIncludes()) {
          pprevBlock->makeNextBlock(*pnextBlock);
          delete pprevBlock;
          pprevBlock = pnextBlock;
          pnextBlock = new CodeBlock();
          pass++;
      }
      else { break;}
  }

  

  
//  pprevBlock->outToFile("work:partitions.pre_gener");

   pprevBlock->generPass(*pnextBlock);   

   delete pprevBlock;
   pprevBlock = pnextBlock;
   pnextBlock = new CodeBlock();
   pprevBlock->objectSetPass(*pnextBlock);

  
   delete pprevBlock;
   pprevBlock = pnextBlock;
   pnextBlock = new CodeBlock();
   pprevBlock->macroPass(*pnextBlock);


   pnextBlock->outToFile(outputFile); 


#ifdef WIN32
	if ( preprocFile == inputFile ) {}
	else {
//		const char* ptnr = preprocFile.getString;
		char* ptnr = &preprocFile[0];
		if( remove( ptnr ) == -1 ) {
			cout << "  Could not delete " << preprocFile << endl;
		}
		else {
			cout << "  Deleted  " << preprocFile << endl;;
		}
	}
	exit(0);
#endif
 
#ifndef __VMS
   if ( preprocFile == inputFile ) {}
   else { 
      Name delcom;
      delcom = "rm ";
      delcom += preprocFile;
      system(&delcom[0]);
   }
#endif

   exit(0);
}
