//----------------------------------------------------------------------
//
//                                              Author :  B. Franek
//                                              Date :  March 2014
// Copyright Information:
//      Copyright (C) 1996-2014 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------
#ifndef ERRORWARNING_HH
#define ERRORWARNING_HH

class SMLline;

class ErrorWarning {
public:

	static void printHeadTop(const char* label);

	static void printHeadSrcLine(const int lnNo, const char* line);

	static void printHead(const char* label, const SMLline& smlline);

	static void printHead(const char* label, const SMLline& smlline, const char* mess);

};

#endif
