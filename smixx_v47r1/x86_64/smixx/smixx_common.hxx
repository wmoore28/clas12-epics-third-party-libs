#ifndef SMIXX_COMMON
#define SMIXX_COMMON

//#define _CrtDbgBreak() ((void)0)

#ifdef USE_OLD_STYLE_CPP

#include <iostream.h>
#include <fstream.h>

#else

#include <iostream>
using std::ostream;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
#include <fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;

#endif

#endif
