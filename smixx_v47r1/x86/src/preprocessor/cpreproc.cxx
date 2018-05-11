//----------------------  C Preprocessor  -----------------------------------
//
//                                                Author : Boda Franek
//                                                Date : 6 January 1999
// Copyright Information:
//      Copyright (C) 1997-2004 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
#include "smixx_common.hxx"
#include <string.h>
#include "name.hxx"

extern int dbg;

#ifdef __VMS
#       include <lib$routines.h>
#       include <jpidef.h>
#       include <starlet.h>
#       include <ssdef.h>
#       include <descrip.h>
#else
#       include <stdlib.h>
#       include <stdio.h>
#ifndef WIN32
#       include <unistd.h>
#endif
#endif
//-----------------------------------------------------------------------------

void cpreproc(const Name& cPreprocOpts, const Name& inputFile,
               Name& preprocFile) {

//  
char pidstr[33];


#ifdef __VMS


  int ISTAT;
  long double ppid;
  int item = JPI$_PID;
  ISTAT = lib$getjpi( &item,0,0,&ppid );
  if ( ISTAT != 1 ) { exit(ISTAT); }
  sprintf(pidstr,"%X",ppid);


  preprocFile = "work:";
  preprocFile += pidstr;
  preprocFile += ".cpreproc";

  enum {MAXVMSCOMMAND = 256};
  char command[MAXVMSCOMMAND+1],temp[MAXVMSCOMMAND+1];

  if ( ( 19 + cPreprocOpts.length() + preprocFile.length() + 1 
          + inputFile.length() ) > MAXVMSCOMMAND ) {
     cout << " C preprocessor command is too long " << endl; cout.flush();
     exit(2);
  }

  strcpy(command,"cc/preprocess_only=");
  preprocFile.whatAreYou(temp,MAXVMSCOMMAND);
  strcat(command,temp);
  cPreprocOpts.whatAreYou(temp,MAXVMSCOMMAND);
  strcat(command,temp); strcat(command," ");
  strcat(command," ");
  inputFile.whatAreYou(temp,MAXVMSCOMMAND);
  strcat(command,temp);

  $DESCRIPTOR(descr_command,command);


  int compl_code;
  int ret = lib$spawn(&descr_command,0,0,0,0,0,&compl_code);
  if(ret&1) { ret = compl_code; }
  if (!(ret&1)) {
     cout << " **** C Preprocessor failed ****" << endl; cout.flush();
     exit(2);
  }
#else

#ifdef WIN32
	preprocFile = getenv("TEMP");
	preprocFile += "/cpreproc.txt";

	enum {MAXIMUMCOMMANDLENGTH = 2047};

	char command[MAXIMUMCOMMANDLENGTH+1],temp[MAXIMUMCOMMANDLENGTH+1];

	if ( (12 + cPreprocOpts.length() + 1 + inputFile.length() + 3 + 
		preprocFile.length()) > MAXIMUMCOMMANDLENGTH ) {
		cout << " C Preprocessor command is too long " << endl; cout.flush();
		exit(2);
	}

//     cl/E /I'dir' 'input-file'  > 'output-file'
	strcpy(command,"cl/E ");
	cPreprocOpts.whatAreYou(temp,MAXIMUMCOMMANDLENGTH);
	strcat(command,temp); strcat(command," ");
	inputFile.whatAreYou(temp,MAXIMUMCOMMANDLENGTH);
	strcat(command,temp);
	strcat(command," > ");
	preprocFile.whatAreYou(temp,MAXIMUMCOMMANDLENGTH);
	strcat(command,temp);


	if (dbg > 0) {
		cout << " Preprocessor command:" << command << endl;
	}

	if (system(NULL)) {
		int status = system (command);
		if ( status != 0 ) {
			cout << " **** C Preprocessor failed ****" << endl; cout.flush();
			exit(2);
		}
	}
	else {
		cout << " ****  could not run C preprocessor ****" << endl;
		cout << "          system() not supported" << endl;
		cout.flush();
		preprocFile = inputFile;
		exit(2);
	}

#else
  int pid = getpid(); 
  sprintf(pidstr,"%d",pid);

  preprocFile = "/tmp/";
  preprocFile += pidstr;
  preprocFile += ".cpreproc";
 
   Name command = "gcc -x c -E ";
   command  += cPreprocOpts; command += " ";
   command += inputFile;
   command += " > "; command += preprocFile;

//  cout << " Preprocessor command:" << command << endl;


  if (system(NULL)) {
    int status = system (command.getString());
    if ( status != 0 ) {
     cout << " **** C Preprocessor failed ****" << endl; cout.flush();
     exit(2);
    }
  }
  else {
    cout << " ****  could not run C preprocessor ****" << endl;
    cout << "          system() not supported" << endl;
    cout.flush();
    preprocFile = inputFile;
    exit(2);
  }

#endif

#endif

  return;
}
