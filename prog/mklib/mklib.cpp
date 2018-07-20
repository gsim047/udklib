// This file was created by MkExe ver. 1.9.19 at [2009-05-14 12:53:13]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
//#include "fFile.h"
#include "fVector.h"
#include "tFile.h"
#include "tFList.h"
#include "tDateTime.h"
//#include "tDebug.h"



#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME mklib

extern char _binary_mklib_rc_start[];
extern char _binary_mklib_rc_end[];

const char* getRCStart() { return _binary_mklib_rc_start; }
const char* getRCEnd() { return _binary_mklib_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__


static void About()
{
	cout << "Creates makefile for lib src in cur folder";    // some description
	cout << endl << endl;
	cout << "Call :  " << config.getname() << " [key [...]] name.mak [mask [mask2 ...]]" << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  --name:LibName  - internal name of library" << endl;
	cout << "  --cfg:CfgName   - use config file 'CfgName' (default '" << config.getname("cfg") << "')" << endl;
	cout << endl;
	exit(0);
}// About


static string chkOpt(const string &opt)
{
	string ret, opt0;
	std::vector<string> v = SplitString(opt, "%?:");
	//for ( unsigned i = 0; i < v.size(); i++ ){
	//	printf("[%s] ", v[i].c_str());
	//}
	//printf("\n");
	if ( v.size() > 2 ){
		opt0 = v[1].substr(3);
		if ( !opt0.empty() ){
			if ( config.find("option", opt0) ){
				ret = v[2];
			}else{
				if ( v.size() > 3 ){
					ret = v[3];
				}
			}
		}
	}
	//printf("opt [%s]/[%s] -> [%s]\n", opt.c_str(), opt0.c_str(), ret.c_str());
	return ret;
}


static string chkList(const string &opt, const tFList &fl)
{
	string ret, opt0;
	std::vector<string> v = SplitString(opt, "%?:");
	//for ( unsigned i = 0; i < fl.size(); i++ ){
	//	printf("[%s] ", fl[i].c_str());
	//}
	//printf("\n");
	if ( v.size() > 2 ){
		opt0 = v[1];
		if ( opt0 == "LIST" ){
			string pre = v[2];
			string af1 = (v.size() > 3) ? v[3] : "";
			string afn = (v.size() > 4) ? v[4] : af1;
			for ( unsigned i = 0; i < fl.size(); i++ ){
				ret += pre + fl.name(i) + ((i+1 < fl.size()) ? af1 : afn) + "\n";
			}
		}
		if ( opt0 == "LISTX" ){
			string pre = v[2];
			string af1 = (v.size() > 3) ? v[3] : "";
			string afn = (v.size() > 4) ? v[4] : af1;
			for ( unsigned i = 0; i < fl.size(); i++ ){
				ret += pre + fl[i] + ((i+1 < fl.size()) ? af1 : afn) + "\n";
			}
		}
	}
	//printf("opt [%s]/[%s] -> [%s]\n", opt.c_str(), opt0.c_str(), ret.c_str());
	return ret;
}


int Main()
{
	cout << "Make Lib.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	string fin = config.get("cmdline", "1");
	if ( fin.empty() )  About();

	std::vector<string> mask;
	for ( unsigned i = 2;; i++ ){
		string s = config.get("cmdline", ToString(i));
		if ( s.empty() )  break;
		mask.push_back(s);
	}
	if ( mask.empty() ){
		mask.push_back("*.cpp");
		mask.push_back("*.c");
	}

	// FILE NAME EXT PATH
	// DATE TIME
	// OBJS

	fin = GetFullPath(fin);
	string name = ExtractFileName(fin);
	string path = ExtractFilePath(fin);
	string ext = ExtractFileExt(fin);
	if ( ext.empty() ){
		fin = ChangeFileExt(fin, "mak");
		ext = ExtractFileExt(fin);
	}

	string oname = config.get("option", "name", "");
	if ( !oname.empty() ){
		name = oname;
	}

	std::map<string, string> mm;
	mm["%FILE%"] = name + "." + ext;
	mm["%NAME%"] = name;
	mm["%EXT%"] = ext;
	mm["%PATH%"] = path;

	mm["%DATE%"] = tDateTime::CurrentDateTimeFormat("yyyy-mm-dd");
	mm["%TIME%"] = tDateTime::CurrentDateTimeFormat("hh:nn:ss");

	
	tFList fl;
	for ( unsigned i = 0; i < mask.size(); i++ ){
		std::vector<string> f = DirFileList(path, mask[i], 0x21);
		//printf("load mask [%s] path [%s]\n", mask[i].c_str(), path.c_str());
		//tFList f(ChangeFilePath(mask[i], path), false);
		//printf("size : %d\n", f.size());
		for ( unsigned j = 0; j < f.size(); j++ ){
			fl += f[j];
		}
	}
	fl.sort();

	std::vector<string> vv;
	cout << "Load template" << endl;
	{
		string tmpl = config.getfile("tmpl");
		if ( !tRTFile::LoadFile(vv, tmpl) ){
			nError::Throw("Can't load makefile template [" + tmpl + "]");
		}
	}

	for ( unsigned i = 0; i < vv.size(); i++ ){
		string s = vv[i];
		for ( unsigned pos = s.find("%OPT"); pos != string::npos; pos = s.find("%OPT", pos + 1) ){
			unsigned pos2 = s.find("%", pos + 1);
			if ( pos2 != string::npos ){
				string opt = s.substr(pos, pos2 - pos + 1);
				mm[opt] = chkOpt(opt);
			}
		}
		for ( unsigned pos = s.find("%LIST"); pos != string::npos; pos = s.find("%LIST", pos + 1) ){
			unsigned pos2 = s.find("%", pos + 1);
			if ( pos2 != string::npos ){
				string opt = s.substr(pos, pos2 - pos + 1);
				mm[opt] = chkList(opt, fl);
			}
		}
	}

	xcodeTable xm = makeCodeTable(mm);

	for ( size_t i = 0; i < vv.size(); i++ ){
		vv[i] = Replace(vv[i], xm);
	}

	cout << "Save file [" << ExtractFileNameExt(fin) << "]" << endl;
	if ( !tFILE::SaveFile(vv, fin) ){
		nError::Throw("Can't save file [" + fin + "]");
	}
	return 0;
}// Main
