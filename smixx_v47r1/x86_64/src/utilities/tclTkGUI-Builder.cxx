//===========================================================================
//
//                                      Boda Franek   Aug 2007
// Copyright Information:
//      Copyright (C) 2007 CCLRC. All Rights Reserved
//
// $Id: tclTkGUI-Builder.cxx,v 1.4 2007/08/23 14:02:31 franek Exp $
//===========================================================================

#ifdef WIN32
	#define _WIN32_WINNT 0x0400
	#include <windows.h>
#else
	#include <dirent.h>
	#include <errno.h>
#endif
#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
using namespace std;
//=============================================================================
int getDirectoryListing(const char dirName[], vector<string>& fileList)
//----------------------------------------------------------------------------
// The function will get listing of a directory
// Input : 	dirName		full directory name
// Output :	fileList	listing of files in the directory
//
//                                  B.Franek	August 2007
//----------------------------------------------------------------------------
{
#ifdef WIN32

		string fileName;
		fileName = dirName;
		fileName = fileName + "/*";

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		DWORD dwError;


//		printf("Target file is %s.\n",fileName.c_str());
		hFind = FindFirstFile(fileName.c_str(),&FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			printf("Invalid File Handle. GetLastError reports %d\n",
				GetLastError ());
			return (0);
		}
		else
		{
//			printf("The first file found is %s\n",
//				FindFileData.cFileName);
			fileList.push_back(FindFileData.cFileName);
			while (FindNextFile(hFind, &FindFileData) != 0)
			{
//				printf("next file name is : %s\n",FindFileData.cFileName);
				fileList.push_back(FindFileData.cFileName);
			}

			dwError = GetLastError();
			FindClose(hFind);
			if (dwError != ERROR_NO_MORE_FILES)
			{
				printf("FindNextFile error. Error is %u.\n",dwError);
				return (0);
			}
			return (1);
		}
#else
	
	DIR *pdir;
	struct dirent *pent;

	pdir=opendir(dirName); 
	if (!pdir)
	{
		printf ("opendir() failure; terminating \n");
		return 0;
	}
	
	errno=0; 
	while ((pent=readdir(pdir)))
	{
		fileList.push_back(pent->d_name);	
	}
	if (errno)
	{
		printf ("readdir() failure; terminating\n");
		return 0;
	}
	closedir(pdir);

	return 1;
#endif
}
//====================================================================================
int copyFile(const char sourceFile[],const char targetFile[])
{
//-----------------------------------------------------------------------------------
// It will copy the source file into target file
//-----------------------------------------------------------------------------------
	// open source file
	fstream source(sourceFile,ios::in | ios::binary);
	if ( source.fail() )
	{
		cout << "Error : can not open  " << sourceFile << endl;
		return 0;
	}
	// open target file
	fstream target(targetFile,ios::out | ios::binary);
	if ( target.fail() )
	{
		cout << "Error : can not open  " << targetFile << endl;
		return 0;
	}
	// now do it character by character
	int c;
	
	c = source.get();
	while ( c != EOF )
	{
		target.put(c);
		c = source.get();
	}
	
	source.close();
	target.close();
	return 1;
}
//======================================================================================
int copySelectedFilesFromSourceToTargetDirectory(const char SOURCEdir[],const char TARGETdir[])
{
//--------------------------------------------------------------------------------------
// It will select all the files containing the string 'selection' from source directory
// and will copy them to the target directory
//--------------------------------------------------------------------------------------
	string selection(".gif");

	vector<string> fileList;
	
	int flg = getDirectoryListing(SOURCEdir, fileList);
	if (!flg) return 0;
	
	string sourceFile,targetFile;
	string name;
	int listSize = fileList.size();
	
	for (int i=0; i<listSize ; i++)
	{
		name = fileList.at(i);
		if ( strstr(name.c_str(),selection.c_str()) != 0 )
		{
			sourceFile = SOURCEdir;
			sourceFile = sourceFile + "/" + name;
			targetFile = TARGETdir;
			targetFile = targetFile + "/" + name;
			//printf("\n %s \n %s\n", sourceFile.c_str(),targetFile.c_str());
			int flg = copyFile(sourceFile.c_str(),targetFile.c_str());
			if ( !flg )
			{
				printf("Error\n   Copy of %s \n to %s \n failed\n",
				sourceFile.c_str(),targetFile.c_str());
			}
		}
	}

	return 1;
}
//======================================================================================
int main()
{
	string SMIDIR, GUIdir, efftcldir, ODIR;
	string MAIN, FINALGUI;
	string TARGETdir;
	
	char* pSMIDIR = getenv("SMIDIR");
	if ( pSMIDIR == NULL )
	{
		cout << "Error  SMIDIR undefined" << endl;
		return 1;
	}
	SMIDIR = pSMIDIR;
	
	char* pODIR = getenv("ODIR");
	if ( pODIR == NULL )
	{
		cout << "Error  ODIR undefined" << endl;
		return 1;
	}
	ODIR = pODIR;
	TARGETdir = SMIDIR + "/" + ODIR;
//	TARGETdir = "/tmp/gui";
	
	FINALGUI = TARGETdir + "/smiGUI.tcl";
	cout << FINALGUI << endl;
	
	fstream FINALGUI_op(FINALGUI.c_str(),ios::out);
	
	if ( FINALGUI_op.fail() )
	{
		cout << "Error :  Can not open file " << FINALGUI  << endl;
		return 2;
	}
	

//	cout << SMIDIR << endl;
	
	GUIdir = SMIDIR + "/src/tclTkGUI";
//	cout << GUIdir << endl;
	
	efftcldir = SMIDIR + "/src/utilities/efftcl";
//	cout << efftcldir << endl;
	
	MAIN = GUIdir + "/main.tcl";
//	cout << MAIN << endl;
	
	fstream MAIN_op(MAIN.c_str(),ios::in);
	
	if ( MAIN_op.fail())
	{
		cout << "Error :  File : " << MAIN << " not found " << endl;
		return 2;
	}
	
	char line[255];
	while (!MAIN_op.eof())
	{
		MAIN_op.getline(line,255);
		if ( strncmp(line,"source ",7) == 0 )
		{
			string sourcedFile;
			sourcedFile = &line[7];
			size_t found = sourcedFile.find("$GUIdir");
			if (found != string::npos)
			{
				sourcedFile.replace(found,7,GUIdir);
			}
			found = sourcedFile.find("$efftcldir");
			if (found != string::npos)
			{
				sourcedFile.replace(found,10,efftcldir);
			}

			FINALGUI_op << "# Sourced File : " << sourcedFile << endl;
			fstream source_op(sourcedFile.c_str(),ios::in);
	
			if ( source_op.fail() )
			{
				cout << "Error :  File : <" << sourcedFile << "> not found " << endl;
				return 2;
			}
			int nout = 0;
			while (!source_op.eof())
			{
				source_op.getline(line,255);
			//	cout << line << endl;
				FINALGUI_op << line << endl;
				nout++;
			}
			source_op.close();
			//cout << nout << endl;
		}
		else
		{
		//	cout << line << endl;
			FINALGUI_op << line << endl;
		}
	}
	
	MAIN_op.close();
	FINALGUI_op.close();
	
	
	int flg = copySelectedFilesFromSourceToTargetDirectory(GUIdir.c_str(),TARGETdir.c_str());
	if (!flg) return 1;
	
	
	return 0;
}
