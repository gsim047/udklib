#include "cMain.h"

#include <signal.h>
#include <vector>
#include <algorithm>
#include <iostream>
using std::cerr;
using std::endl;

#include "nError.h"
#include "tDebug.h"





int Main();

bool cm__Stop = false;

static std::vector<cm__callBack> __cb;

void add_callBack(cm__callBack cb)
{
	if ( std::find(__cb.begin(), __cb.end(), cb) == __cb.end() ){
		__cb.push_back(cb);
	}
}// add_callBack


static void catch_intST(int sig)
{
	debugLog("SIG " + ToString(sig) + " detected. Exiting.");
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
//	signal(SIGHUP, SIG_IGN);
#ifdef __WIN32__
	signal(SIGBREAK, SIG_IGN);
#endif
	cm__Stop = true;
	for ( size_t i = 0; i < __cb.size(); i++ ){
		if ( __cb[i] ) __cb[i](sig);
	}
	exit(1);
}// catch_intST




int main(int argc, char *argv[])
{
	int ret;

	try{
		config.loadCmdLine(argc, argv);
		string configFileName = config.get("option", "cfg");      // config file
		if ( configFileName.empty() ){
			config.load(config.getfile("cfg"));
		}else{
			config.load(configFileName);
		}
		int level = config.get("option", "debug", -1);
		if ( level >= 0 ){
			debugSetLevel(level);
		}
		if ( level > 0 ){
			debugSetFileName(config.getfile());
		}

		signal(SIGTERM, catch_intST);
#ifdef __WIN32__
		signal(SIGBREAK, catch_intST);
#endif
		signal(SIGINT, SIG_IGN);
#ifdef __GNUG__
		signal(SIGPIPE, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
#endif

		ret = Main();
		cm__Stop = true;

	}catch (nError::Message &e){
		if ( !isDebugStd() ){  
			cerr << "??? " << e << endl;
//			cerr << "??? " << win2dos(e) << endl;
		}
//		if ( !isDebugStd() )  cerr << "??? " << e << endl;
		errorLog("??? " + e);
		ret = -1;
	}catch ( ... ){
		if ( !isDebugStd() )  cerr << "Error !!!" << endl;
		errorLog("Error !!!");
		ret = -1;
	}

	return ret;
}// main



//#ifdef _WIN32
/*
static bool GetVersionOfFile(
                      char *pszAppName,   // имя файла
                      char *pszVerBuff,   // номер версии
                      int   iVerBuffLen,  // размер буфер
                      char *pszLangBuff,  // язы
                      int   iLangBuffLen, // размер буфер
                      const char *param
                            )
{
	DWORD dwScratch;
	DWORD * pdwLangChar;
	UINT uSize;
	BYTE * pbyInfBuff;
	char szVersion [32];
	char szResource [80];
	char *pszVersion = szVersion;

	DWORD dwInfSize = GetFileVersionInfoSize(pszAppName, &dwScratch);
	if ( !dwInfSize )  return false;

	pbyInfBuff = new BYTE[dwInfSize];
	memset(pbyInfBuff, 0, dwInfSize);
	if ( GetFileVersionInfo(pszAppName, 0, dwInfSize, pbyInfBuff) ){
		if ( VerQueryValue(pbyInfBuff,
		                   "\\VarFileInfo\\Translation",
		                   (void**)(&pdwLangChar),
		                   &uSize) ){
			if ( VerLanguageName(LOWORD (*pdwLangChar),
			                     szResource,
			                     sizeof(szResource))){
				strncpy(pszLangBuff, szResource, iLangBuffLen);
			}
			wsprintf(szResource, "\\StringFileInfo\\%04X%04X\\%s",
			         LOWORD (*pdwLangChar), HIWORD(*pdwLangChar), param);

			if ( VerQueryValue(pbyInfBuff, szResource, (void**)(&pszVersion), &uSize) ){
				strncpy(pszVerBuff, pszVersion, iVerBuffLen-1);
			}
		}
	}
	delete [] pbyInfBuff;
	return true;
}// GetVersionOfFile



string GetVer(const string &par)
{
	string Par = "FileVersion";
	if ( !par.empty() )  Par = par;
	string ret;
	string fname = config.getname() + ".exe";
	char buf1[300];
	char buf2[300];
//	printf("fname: [%s]\n", fname.c_str());
	if ( GetVersionOfFile((char *)fname.c_str(), buf1, 300, buf2, 300, Par.c_str()) ){
		ret = string(buf1);
	}
	return ret;
}// GetVer


string GetFileVer(const string &fname, const string &par)
{
	string Par = "FileVersion";
	if ( !par.empty() )  Par = par;
	string ret;
	char buf1[300];
	char buf2[300];
	if ( GetVersionOfFile((char *)fname.c_str(), buf1, 300, buf2, 300, Par.c_str()) ){
		ret = string(buf1);
	}
	return ret;
}// GetFileVer
*/



#if defined( __CYGWIN__ ) || !defined( __GNUG__ )
//#ifndef __GNUG__
#include <windows.h>
//#pragma message ( "win32!" )

static bool GetVersionOfFile(
                      const char *pszAppName,   // имя файла
                      char *pszVerBuff,   // номер версии
                      int   iVerBuffLen,  // размер буфера
                      char *pszLangBuff,  // язык
                      int   iLangBuffLen, // размер буфера
                      const char *param
                            )
{
//	printf("\nWin32!\n");
	DWORD dwScratch;
	DWORD * pdwLangChar;
	UINT uSize;
	char szVersion[32], szResource[80];
	char *pszVersion = szVersion;

	DWORD dwInfSize = GetFileVersionInfoSize(pszAppName, &dwScratch);
	if ( !dwInfSize )  return false;

	BYTE * pbyInfBuff = new BYTE[dwInfSize];
	memset(pbyInfBuff, 0, dwInfSize);
	if ( GetFileVersionInfo(pszAppName, 0, dwInfSize, pbyInfBuff) ){
		if ( VerQueryValue(pbyInfBuff, "\\VarFileInfo\\Translation", (void**)(&pdwLangChar), &uSize) ){
			if ( VerLanguageName(LOWORD (*pdwLangChar), szResource, sizeof(szResource))){
				strncpy(pszLangBuff, szResource, iLangBuffLen);
			}
			wsprintf(szResource, "\\StringFileInfo\\%04X%04X\\%s", LOWORD (*pdwLangChar), HIWORD(*pdwLangChar), param);

			if ( VerQueryValue(pbyInfBuff, szResource, (void**)(&pszVersion), &uSize) ){
				strncpy(pszVerBuff, pszVersion, iVerBuffLen-1);
			}
		}
	}
	delete [] pbyInfBuff;
	return true;
}// GetVersionOfFile



string getRCString(const string &par)
{
	string Par = par.empty() ? "FileVersion" : par;
	string ret;
	// ??? todo
	string fname = config.getname(); // + ".exe";
	char buf1[300], buf2[300];
//	printf("fname: [%s]\n", fname.c_str());
	if ( GetVersionOfFile(fname.c_str(), buf1, 300, buf2, 300, Par.c_str()) ){
		ret = string(buf1);
	}
	return ret;
}// 


#else // !__CYGWIN__  / == Linux

//#pragma message ( "linux!" )
//static char* _rc_start = nullptr;
//static char* _rc_end = nullptr;

static string rc_data;


static string getRCString(const string& par)
{
//	printf("\nLinux!\n");
	if ( rc_data.empty() ){
		const char* _start = getRCStart();
		const char* _end = getRCEnd();
		size_t size = _end - _start;
		rc_data = string(_start, size);
	}
	
	string name = par.empty() ? "FileVersion" : par;
	
	string ret;
	string s = "VALUE \"" + name + "\"";
//	printf("s: [%s]\n", s.c_str());
	size_t pos = rc_data.find(s);
	if ( pos != string::npos ){
//		printf("pos: %d\n", pos);
		string s2 = rc_data.substr(pos+s.size());
		size_t pos2 = s2.find("\"");
		if ( pos2 != string::npos ){
			string s3 = s2.substr(pos2+1);
			size_t pos3 = s3.find("\\0\"");
			if ( pos3 != string::npos ){
				ret = s3.substr(0, pos3);
			}
		}
	}
	return ret;
}// getRCString


#endif



string GetVer(const string& name)
{
	return getRCString(name);
}// GetVer

