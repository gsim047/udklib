#include <cMain.h>
//#include <tDateTime.h>
#include <fString.h>
//#include <fFile.h>
#include <tFile.h>
#include <fVector.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
using std::cout;
using std::endl;

#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME mkexe

extern char _binary_mkexe_rc_start[];
extern char _binary_mkexe_rc_end[];

const char* getRCStart() { return _binary_mkexe_rc_start; }
const char* getRCEnd() { return _binary_mkexe_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__


static std::vector<string> libs;
static std::vector<string> llibs;
static std::vector<string> objs;

static xcodeTable rep_xc;


static void add(std::vector<string> &v, const string &str)
{
	for ( unsigned i = 0; i < v.size(); i++ ){
		if ( CompareIC(v[i], str) == 0 ) return;
	}
	v.push_back(str);
}// add


static void add(const string &s)
{
	string ext = ExtractFileExt(s);
	if ( CompareIC(ext, "obj") == 0 ){
		add(objs, s);
	}else if ( CompareIC(ext, "lib") == 0 ){
		if ( tFILE::FileExists(s) ){
			add(llibs, s);
		}else{
			add(libs, s);
		}
	}else if ( CompareIC(ext, "cpp") == 0 || CompareIC(ext, "c") == 0 ){
		add(objs, ChangeFileExt(s, "obj"));
	}else{
		cout << "Unknown file type '" << s << "' - ignore" << endl;
	}
}// add


static void About()
{
	cout << "Call :  " << config.getname() << " [key1 [...]] <filename> [file1.obj [...]] [file2.lib [...]]" << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?                - this info" << endl;
	cout << "  --cfg:CfgFile     - use config-file 'CfgFile' (default '" << config.getname("cfg") << "')" << endl;
	cout << "  -a                - use 'archive' option" << endl;
	cout << "  -u                - pack exe-file with UPX" << endl;
	cout << "  --mk:gcc          - create makefile for GNU C++ (default), or" << endl;
	cout << "  --mk:vc           - create makefile for MS Visual C++, or" << endl;
	cout << "  --mk:bc           - create makefile for Borland C++" << endl;
	cout << "  --out:exe         - make .exe creation project (default), or" << endl;
	cout << "  --out:dll         - make .dll project" << endl;
	cout << endl;
	exit(0);
}// About


static void wrFile(const string &cTempl, const string &cName)
{
	std::vector<string> list;
	if ( tRTFile::LoadFile(list, cTempl) ){
		cout << "Writing file \"" << cName << "\" ..." << endl;
		for ( unsigned i = 0; i < list.size(); i++ ){
			list[i] = Replace(list[i], rep_xc);
		}
		if ( !tFILE::FileExists(cName) )  tFILE::SaveFile(list, cName);
		//list.clear();
	}
}// wrFile


extern char *ver;


int Main()
{
	cout << "Make makefile for executable.  v " << GetVer() << "  (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]\n\n";
	if ( config.get("cmdline", "?") == "1" )  About();

	string cName = config.get("cmdline", "1");
	if ( cName.empty() )  About();

	string cExt = ExtractFileExt(cName);
	cName = ExtractFileName(cName);

	bool arc = config.getBool("option", "a", false);
	bool upx = config.getBool("option", "u", false);


	bool dll = false;
	{
		string key = config.get("option", "out", "exe");
		if ( key == "dll" ) dll = true;
	}

	string cVer = GetVer();
//	tDateTime tim = tDateTime::now();
	char buf[30];
	time_t cur_secs;
	time(&cur_secs);
	struct tm *cur_time = localtime(&cur_secs);

//	strftime(buf, 29, "%y-%m-%d-%H%M%S", cur_time);
	strftime(buf, 29, "%Y-%m-%d", cur_time);
	string cDate = buf;
	strftime(buf, 29, "%H:%M:%S", cur_time);
	string cTime = buf;
	strftime(buf, 29, "%Y", cur_time);
	string cYear = buf;
//	string cDate = tim.FormatString("yyyy-mm-dd");
//	string cTime = tim.FormatString("hh:mm:ss");
//	string cYear = tim.FormatString("yyyy");
	string cArc  = (arc) ? "" : "#";
	string cNoarc = (arc) ? "#" : "";
	string cPack = (upx) ? "" : "#";
	string cLib;
	string cLLib;
	string cObj = "\\\n";
	string cExe = (dll) ? "#" : "";
	string cDll = (dll) ? "" : "#";
	string cRcExe = (dll) ? "//" : "";
	string cRcDll = (dll) ? "" : "//";

	//string cDllExe = (dll) ? "#define __%NAME%_dll 1\n#include \"%NAME%.h\"\n" : "";
	string cMain = (dll) ? "dMain" : "cMain";

	int ccver;  // 0 - bc, 1 - vc, 2 - gcc
	string pre;
	string sfx;
	string key = config.get("option", "mk", "gcc");
	if ( key == "gcc" ){
		ccver = 2;
		pre = "gcc";
		sfx = ".o";
	}else if ( key == "vc" ){
		ccver = 1;
		pre = "vc";
		sfx = ".obj";
	}else if ( key == "bc" ){
		ccver = 0;
		pre = "bc";
		sfx = ".obj";
	}else{
		nError::Throw("Unknown make type [" + key + "]!");
	}

	string deflibs = config.get("default", pre + "libs");
	if ( !deflibs.empty() ){
		std::vector<string> v = SplitString(deflibs, ",; ");
		for ( unsigned i = 0; i < v.size(); i++ ){
			string l = trim(v[i]);
			if ( !l.empty() ) libs.push_back(l);
		}
	}

	objs.push_back("$(NAME)" + sfx);

	{
		for ( unsigned i = 2;; i++ ){
			string s = config.get("cmdline", ToString(i));
			if ( s.empty() ) break;
			if ( s.find_first_of("?*") == string::npos ){
				add(s);
			}else{
				std::vector<string> files = DirFileList(GetCurPath(), ExtractFileNameExt(s));
				for ( unsigned j = 0; j < files.size(); j++ ){
					add(files[j]);
				}
			}
		}
	}

	{
		for ( unsigned i = 0; i < libs.size(); i++ )  cLib += libs[i] + " "; 
	}
	{
		for ( unsigned i = 0; i < llibs.size(); i++ )  cLLib += llibs[i] + " "; 
	}
	{
		for ( unsigned i = 0; i < objs.size(); i++ ){
			cObj += "\t" + objs[i] + ((i + 1 < objs.size()) ? " \\\n" : "\n");
		}
	}

	bool use_c = false;

	string cName_cpp;
	if ( toLower(cExt) == "c" ){
		cName_cpp = cName + ".c";
		use_c = true;
	}else{
		cName_cpp = cName + ".cpp";
	}
	string cName_c = cName + ".c";
	string cName_h   = cName + ".h";
	string cName_mak = cName + ".mak";
	if ( ccver == 2 ){
		cName_mak = cName + ".gmk";
	}
	string cName_rc  = cName + ".rc";
	string cName_cfg = cName + ".cfg";
	string cName_ico = cName + ".ico";

	string cTempl_cpp;

	if ( use_c ){
		cTempl_cpp = (dll) ? config.getfile("tmpl.dll.c") :
		                     config.getfile("tmpl.exe.c");
	}else{
		cTempl_cpp = (dll) ? config.getfile("tmpl.dll.cpp") :
		                     config.getfile("tmpl.exe.cpp");
	}
	string cTempl_h   = config.getfile("tmpl.h");
	string cTempl_mak = config.getfile("tmpl." + pre + ".mak");
	cout << "mak: " << cTempl_mak << endl;
	string cTempl_rc  = config.getfile("tmpl.rc");
	string cTempl_cfg = config.getfile("tmpl.cfg");
	string cTempl_ico = config.getfile("tmpl.ico");

	{
		std::map<string, string> rep_map;

		rep_map["%ARC%"]  = cArc;
		rep_map["%DATE%"] = cDate;
		rep_map["%DLL%"]  = cDll;
		rep_map["%EXE%"]  = cExe;
		rep_map["%RCDLL%"]  = cRcDll;
		rep_map["%RCEXE%"]  = cRcExe;
		rep_map["%LIB%"]  = cLib;
		rep_map["%LLIB%"] = cLLib;
		rep_map["%MAIN%"] = cMain;
		rep_map["%NAME%"] = cName;
		rep_map["%NOARC%"] = cNoarc;
		rep_map["%OBJ%"]  = cObj;
		rep_map["%PACK%"] = cPack;
		rep_map["%TIME%"] = cTime;
		rep_map["%VER%"]  = cVer;
		rep_map["%YEAR%"] = cYear;

		rep_xc = makeCodeTable(rep_map);
	}

	std::vector<string> list;

	wrFile(cTempl_cpp, cName_cpp);
	wrFile(cTempl_mak, cName_mak);
	wrFile(cTempl_rc,  cName_rc);
	wrFile(cTempl_cfg, cName_cfg);
	tFILE::FileCopy(cTempl_ico, cName_ico);
	if ( dll )  wrFile(cTempl_h,   cName_h);

	cout << "Ok" << endl;
	return 0;
}// Main
