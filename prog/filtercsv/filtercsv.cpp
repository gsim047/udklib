// This file was created by MkExe ver. 1.0.12.47 at [2016-07-30 09:14:40]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
#include "tFile.h"
#include "fVector.h"


#ifndef __CYGWIN__
#ifndef _WIN32

extern char _binary_filtercsv_rc_start[];
extern char _binary_filtercsv_rc_end[];

const char* getRCStart()
{
	return _binary_filtercsv_rc_start;
}

const char* getRCEnd()
{
	return _binary_filtercsv_rc_end;
}

#endif // !_WIN32
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
	cout << "###.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	string fin = config.get("cmdline", "1");
	if ( fin.empty() )  About();
	string fon = config.get("cmdline", "2");
	if ( fon.empty() )  About();

	string s = tFILE::LoadFile(fin);

	std::vector<std::vector<string> > res;
	int n = ParseAllCSV(res, s, ',');

	// Алена,,Кабанова,,,,,,,,,,,,devageeta@mail.ru
	//     0         2            14

	tFILE fo(fon, "wt");
	if ( fo.isOpen() ){
		for ( int i = 0; i < res.size(); i++ ){
			std::vector<string>& res1 = res[i];
			if ( res1.size() > 14 && !res1[14].empty() ){
				std::vector<string> res2;
				res2.push_back(trim(res1[0]));
				res2.push_back(res1[2]);
				res2.push_back(res1[14]);
				string so = ToCSV(res2, ',');
				fo.write(so);
				fo.write("\n");
			}
		}
	}
	
	return 0;
}// Main
