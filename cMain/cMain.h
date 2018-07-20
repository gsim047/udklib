#ifndef __cMain_h
#define __cMain_h 1

#ifndef __tConfiguration_h
#include "tConfiguration.h"
#endif


extern bool cm__Stop;

typedef void (*cm__callBack)(int);
void add_callBack(cm__callBack);


string GetVer(const string &par = "");
//string GetFileVer(const string &fname, const string &par = "");

#ifndef __CYGWIN__
#ifndef WIN32
const char* getRCStart();
const char* getRCEnd();
#endif
#endif


int Main();


#endif // __cMain_h
