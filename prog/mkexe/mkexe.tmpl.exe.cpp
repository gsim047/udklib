// This file was created by MkExe ver. %VER% at [%DATE% %TIME%]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
#include "tFile.h"
//#include "tDebug.h"


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME %NAME%

extern char _binary_%NAME%_rc_start[];
extern char _binary_%NAME%_rc_end[];

const char* getRCStart() { return _binary_%NAME%_rc_start; }
const char* getRCEnd() { return _binary_%NAME%_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__


static void About()
{
	cout << "";    // some description
	//cout << endl << endl;
	cout << "Call :  " << config.getname() << " [key [...]] ..." << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  --cfg:CfgName   - use config file 'CfgName' (default '" << config.getname("cfg") << "')" << endl;
	cout << endl;
	exit(0);
}// About


int Main()
{
	cout << "%NAME%.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	//string fin = config.get("cmdline", "1");
	//if ( fin.empty() )  About();

	// <--- Your code
	
	return 0;
}// Main
