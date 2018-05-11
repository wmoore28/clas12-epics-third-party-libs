//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  March 2014
// Copyright Information:
//      Copyright (C) 1996-2014 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------

#include "errorwarning.hxx"
#include <stdlib.h>
#include "smlline.hxx"

 //--------------------------------------------------------------------------
void ErrorWarning::printHeadTop(const char* label)
{
  cout << endl
       << "=== " << label 
       << " ============================================================"
       << endl;
       
       return; 
}
//--------------------------------------------------------------------------
void ErrorWarning::printHeadSrcLine(const int lnNoIn, const char* line)
{
	int lnNo; 
	lnNo = lnNoIn + 1;  // make it the same as editors
	cout << " line " << lnNo <<  ":  " << line << endl;
	
	return;
}
//-------------------------------------------------------------------------
void ErrorWarning::printHead(const char* label, const SMLline& smlline)
{
	printHeadTop(label);
	
	SMLline firstLine; int lnNo; Name line;
			
	lnNo = smlline.linNum();
	line = smlline.lineBody();
	
	printHeadSrcLine(lnNo,line.getString());

  return;
}
//-------------------------------------------------------------------------
void ErrorWarning::printHead(const char* label, const SMLline& smlline, const char* mess)
{
	printHead(label,smlline);

	cout << mess << endl;
  return;
}
