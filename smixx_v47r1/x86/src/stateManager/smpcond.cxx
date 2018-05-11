//------------------------  SmpCond  Class ------------------------------------
#include "smpcond.hxx"
//------------------------------------------------
//                                                         B. Franek
//                                                         01-Aug-1996
//                                                  rewritten 13-Nov-2003
// Copyright Information:
//      Copyright (C) 1996-2003 CCLRC. All Rights Reserved.
//----------------------------------------------------------------------------

//--------------------------------------------------------------------------
int SmpCond::type() const {
	return _type;
}
//-----------------------------------------------------------------------------
NameVector& SmpCond::refObjects() {
	return _refObjects;
}
//-----------------------------------------------------------------------------
NameVector& SmpCond::refObjectSets() {
	return _refObjectSets;
}





