//----------------------  Class   SMLline  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 15 December 1998
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------
#ifndef SMLLINE_HH
#define SMLLINE_HH
#include "smixx_common.hxx"
#include "typedefs.hxx"
#include "name.hxx"

class SMLline : public Name {
   public :

	SMLline();

	SMLline(const int linnum, const Name& );
	
	SMLline(const int linnum, const char* lineBody );

	virtual ~SMLline();

	SMLline& operator=(const SMLline &);

	SMLline& operator+= (const SMLline &);

	int linNum() const;

	Name lineBody() const;

	bool commentLine() ;

	bool instructionLine( Name& type );

	int primKeyword( Name& name, int& assoc) ;

	int objectKeyword( Name& name);

	int stateKeyword( Name& name);

	void out() const;
	
	void out(int jst, int jend) const;  // will print char jst till jend

/**
	will print :
	first line:  msg followed by the _lineNum
	second line:  the line Body
	third line:   character ^ pointing to the jth character
*/
	void indicateCharacter(char* msg, int j) const;
	
	friend ostream& operator<<(ostream&, const SMLline& );

//
//
	protected :
		int _lineNum;
};

#endif
