// This file was created by MkExe ver. 1.0.10.22 at [2015-06-21 01:29:13]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
//#include "fFile.h"
#include "tFile.h"
#include "tLongFile.h"
#include "tDebug.h"
#include "tRdBuff.h"


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME saveJpg

extern char _binary_saveJpg_rc_start[];
extern char _binary_saveJpg_rc_end[];

const char* getRCStart() { return _binary_saveJpg_rc_start; }
const char* getRCEnd() { return _binary_saveJpg_rc_end; }

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



static string Bns = "\x01\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9";
static string Bs4 = ""; //"\xdd";
static string Bvs = "\xda\xdb\xdc\xdd\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xcc\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xfe";





static bool isSegment(tRdBuff &f, char &cc, size_t &sz)
{
	size_t pos = f.tell();
	if ( f.empty() ) return false;

	char c = (char)f.getBYTE();
	if ( c != '\xff' ){
		f.seek(pos);
		return false;
	}
	c = (char)f.getBYTE();
//	cout << "seg? " << ToStringHex((unsigned char)c) << endl;

	if ( Bns.find(c) != string::npos ){
		f.seek(pos);
		cc = c;
		sz = 2;
		return true;
	}
	if ( Bs4.find(c) != string::npos ){
		f.seek(pos);
		cc = c;
		sz = 6; // or 6??
		return true;
	}
	if ( Bvs.find(c) != string::npos ){
		cc = c;
		unsigned short t = f.getWORD(); 
		sz = __swap2(t) + 2;
		f.seek(pos);
		return true;
	}

	f.seek(pos);
	return true;	
}




static int getJpgSize(tRdBuff &f)
{
	char seg;
	size_t segsize;

	f.seek(0);
	size_t pos = f.tell();

	size_t jpgsize = 0;

	if ( isSegment(f, seg, segsize) ){
//		cout << "seg " << ToStringHex((unsigned char)seg) << " size:" << ToString(segsize) << " pos:" << ToStringHex(f.tell()) << endl;
		if ( seg == '\xd8' ){
			f.skip(segsize);
			while ( isSegment(f, seg, segsize) ){
//				cout << "seg " << ToStringHex((unsigned char)seg) << " size:" << ToString(segsize) << " pos:" << ToStringHex(f.tell()) << endl;
				f.skip(segsize);
				if ( seg == '\xd9' ){
					break;
				}
				if ( seg == '\xda' ){
					if ( !f.find("\xff\xd9") ){
						break;
					}
//					cout << "pos_e: " << f.tell() << endl;
				}
			}
			jpgsize = f.tell();
		}
	}

//	cout << "size: " << jpgsize << endl;
	return (int)jpgsize;
}



int Main()
{
	cout << "###.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	string fin = config.get("cmdline", "1");
	if ( fin.empty() )  About();

	{
		cout << "file [" << fin << "]\n";

		tLongFile fi(fin, "rb");
		
		if ( fi.isOpen() ){

			cout << "size: " << fi.size() << "\n"; 
//			cout << "size: " << ToStringL(fi.size()) << "\n"; 

//			debugLog("0");

			int n = 0;
			string ex1 = "\xff\xd8\xff";
			for ( longlong i = 1024; i < fi.size(); i += 512 ){
//				debugLog("1");
				fi.seek(i);
//				debugLog("2");
//				cout << (100.*i/fi.size()) << "\r";

				string ex2 = fi.read(3);
//				debugLog("3");
//				if ( ex1 == ex2.substr(0, 4) ){
//				printf("%s\n", BufDump(ex2).c_str());
				if ( ex1 == ex2 ){
//				debugLog("4");
					fi.seek(i);
//				debugLog("5");

					{

//						fi.copyTo(fo, 20*1024*1024);
						string s = fi.read(20*1024*1024);
						tRdBuff bf(s);

						int sz = getJpgSize(bf);
						if ( sz <= 0 ){
							sz = 20*1024*1024;
						}

						if ( sz > 20480 ){
							n++;
							string fname = "file" + ToString(n, 4) + ".jpg";
							cout << "saving [" << fname << "]  pos:" << ToStringHex(fi.tell(), 8) << "\n";
							fi.seek(i, SEEK_SET);
							tFILE fo(fname, "w+b");
							fi.copyTo(fo, sz);
						}
					}
				} 
			}
			
		}
	}
	
	return 0;
}// Main
