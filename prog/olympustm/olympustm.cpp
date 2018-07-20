// This file was created by MkExe ver. 1.0.12.45 at [2016-06-01 12:49:03]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include <stdlib.h>

#include "fString.h"
#include "tFile.h"
//#include "tDebug.h"
#include <time.h>



#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME olympustm

extern char _binary_olympustm_rc_start[];
extern char _binary_olympustm_rc_end[];

const char* getRCStart() { return _binary_olympustm_rc_start; }
const char* getRCEnd() { return _binary_olympustm_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__


static void About()
{
	cout << "Olympus old-style datetime exif extractor";    // some description
	cout << endl << endl;
	cout << "Call :  " << config.getname() << " [key [...]] filename" << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  --cfg:CfgName   - use config file 'CfgName' (default '" << config.getname("cfg") << "')" << endl;
	cout << "  --shift:x       - time shift is hours (default for Moscow -4 or -3)" << endl;
	cout << "  --ren           - make rename list" << endl;
	cout << endl;
	exit(0);
}// About


int Main()
{
	cout << "OlympusTM.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	int shift = config.get("option", "shift", 0) * 3600;
	bool ren = config.getBool("option", "ren", false);

	int i = 1;
	for ( ;; i++ ){
		string fin = config.get("cmdline", ToString(i));
		if ( fin.empty() ) break;

//		printf("fin:[%s] ", fin.c_str());
		string s = tFILE::LoadFile(fin);
//		printf("s.size:%d ", s.size());
		int pos = s.find("TimeDate=");
		if ( pos != string::npos ){
			string stm = s.substr(pos+9, 10);

			time_t tt = ToInt(stm) + shift;
			struct tm *cur_time = localtime(&tt);
			char buf[300];
			strftime(buf, 299, "%Y%m%d-%H%M%S", cur_time);

			if ( ren ){
				strftime(buf, 299, "%Y%m%d-%H%M%S", cur_time);
				printf("ren \"%s\" %s.jpg\n", fin.c_str(), buf);
			}else{
				strftime(buf, 299, "%Y-%m-%d %H:%M:%S", cur_time);
				printf("file: [%s]  datetime: %s\n", fin.c_str(), buf);
			}
		}
	}
	if ( i == 1 )  About();

	return 0;
}// Main
