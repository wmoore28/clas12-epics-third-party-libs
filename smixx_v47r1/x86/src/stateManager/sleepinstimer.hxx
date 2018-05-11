#ifndef SLEEPINSTIMER_HH
#define SLEEPINSTIMER_HH
#include <dic.hxx>
#include "name.hxx"
//                                                               July 2011
//                                                               B. Franek
// Copyright Information:
//      Copyright (C) 1996-2011 CCLRC. All Rights Reserved.
//-----------------------------------------------------------------------------

class SleepInsTimer:public DimTimer
{
public:
	SleepInsTimer( const Name& parentObjectName);

/**
	will start the timer
*/
	void startTimer(int seconds);
/**
	this is supplying the DimTimer function that will be called when the
	timer finishes
*/
	void timerHandler();
	
private:
/**
	pointer to the parent object that created it
*/
	Name _parentObjectName;
};
#endif
