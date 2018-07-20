#include "cMain.h"
#include "tFile.h"
#include "fString.h"
#include "fVector.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
//#include "tDebug.h"


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME wi

extern char _binary_wi_rc_start[];
extern char _binary_wi_rc_end[];

const char* getRCStart() { return _binary_wi_rc_start; }
const char* getRCEnd() { return _binary_wi_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__



//static const char ver[] = { "1.82" };
//static const char dat[] = { __DATE__ };

static string env = "environ";
static string opt = "option";
static string mn  = "main";

static unsigned out = 0;
static bool dll = false;
static string curdir;

static string _exec;
static string _copy;
static string _del;

static std::vector<string> paths;


static void copyPath(string &tr, int sw, int cnt, const string &pgm)
{
	if ( sw ){
		if ( sw < 0 ){
			tr = pgm;
		}else{
			if ( sw == cnt ){
				tr = pgm;
			}
		}
	}
}// copyPath


static string getTimS(int secs, const string &format = "")
{
	char buf[300];
	time_t _secs = secs;
	struct tm *time_now = localtime(&_secs);
	if ( format.empty() ){
		strftime(buf, 299, "%Y-%m-%d %H:%M:%S", time_now);
	}else{
		strftime(buf, 299, format.c_str(), time_now);
	}
	return string(buf);
}// getTimS


static string FileDateM(const string &name)
{
	struct stat statbuf;
	if ( stat(name.c_str(), &statbuf) != 0 )  return "1980-01-01 00:00:00";
	return getTimS((int)statbuf.st_mtime);
}// FileDateM


static void print(unsigned out,
                  const string &Pgm,
                  unsigned size,
                  const string &date)
{
	printf("%4d %-43.43s%10lu  %s\n",
	       out,
	       Pgm.c_str(),
	       size,
	       date.c_str());
}// print


static void print2(const string &Pgm)
{
	printf("     .../%-39.39s\n", Pgm.c_str());
}// print2


static void fnameOut(const string &pgm)
{
	if ( !tFILE::FileExists(pgm) ) return;

	if ( out == 0 ){
		printf("Path found:\n");
		printf("-------------------------------------------------------------------------------\n");
	}
	out++;

	int size = tFILE::FileSize(pgm);
//	int ts = tFILE::getFileModifyDate(pgm);
	string date = FileDateM(pgm);

	string Pgm = pgm; // ChangeFilePath(ff.ff_name, pgm);

	if ( Pgm.size() <= 43 ){
		print(out, Pgm, size, date);
	}else{
		size_t pos = Pgm.find_last_of(path_delimiter, 39);
		if ( pos == string::npos ){
			print(out, Pgm, size, date);
		}else{
			print(out, Pgm.substr(0, pos) + "...", size, date);
			Pgm.erase(0, pos + 1);

			if ( Pgm.size() <= 39 ){
				print2(Pgm);
			}else{
				size_t pos = Pgm.find_last_of(path_delimiter, 35);
				if ( pos == string::npos ){
					print2(Pgm);
				}else{
					print2(Pgm.substr(0, pos) + "...");
					Pgm.erase(0, pos + 1);
					print2(Pgm);
				}
			}
		}
	}

	copyPath(_exec, config.get(opt, "e", 0), (int)out, pgm);
	copyPath(_copy, config.get(opt, "c", 0), (int)out, pgm);
	copyPath(_del,  config.get(opt, "d", 0), (int)out, pgm);
}// fnameOut


static void srchDir(const string &path, const string &pgm)
{
	std::vector<string> name = DirFileList(path, pgm, 0x21);
//	printf("srchDir : [%s] in path [%s] sz:%d\n", pgm.c_str(), path.c_str(), (int)name.size());
	for ( unsigned i = 0; i < name.size(); i++ ){
		fnameOut(ChangeFilePath(name[i], path));
	}
}// srchDir


static void srchExt(const string &path1, const string &pgm, bool ex)
{
//	printf("searchEx: [%s] in path [%s] %d\n", pgm.c_str(), path1.c_str(), ex ? 1 : 0);
	if ( ex ){
		string pathext = config.get(env, "PATHEXT");
		std::vector<string> ext = SplitString(pathext, ";:");
		for ( unsigned i = 0; i < ext.size(); i++ ){
			srchDir(path1, ChangeFileExt(pgm, kill_all_of(ext[i], ".")));
		}
		//srchDir(path1, ChangeFileExt(pgm, "EXE"));
		//srchDir(path1, ChangeFileExt(pgm, "CMD"));
		//srchDir(path1, ChangeFileExt(pgm, "BAT"));
	}else{
		srchDir(path1, pgm);
	}
}// srchExt


static void srch(const string &pgm, bool ex)
{
	//int d = getdisk();
	//curdir[0] += (char)getdisk();
	//getcurdir(0, curdir + 3);

	//srchExt("", pgm, ex);
//	printf("search: [%s] %d\n", pgm.c_str(), ex ? 1 : 0);

	for ( unsigned i = 0; i < paths.size(); i++ ){
		if ( CompareIC(curdir, paths[i]) == 0 )  continue;
		srchExt(paths[i], pgm, ex);
	}
}// srch


static void fileCopy(const string &fpath)
{
	if ( !tFILE::FileCopy(fpath, ExtractFileNameExt(fpath)) ){
		printf("Can't copy file '%s'\n", fpath.c_str());
	}
}// fileCopy


static void fileDel(const string &fpath)
{
	if ( !tFILE::FileExists(fpath) ){
		printf("Can't access file '%s'\n", fpath.c_str());
		return;
	}
	if ( !tFILE::FileDelete(fpath) ){
		printf("Can't delete file '%s'\n", fpath.c_str());
	}
}// fileDel


static void addPath(const string &p)
{
//  printf("addPath: [%s]\n", p.c_str());
	string pp = GetFullPath(p);
	if ( !pp.empty() ) PathAddDelimiter(pp);
	for ( unsigned i = 0; i < paths.size(); i++ ){
		if ( CompareIC(pp, paths[i]) == 0 )  return;
	}
	paths.push_back(pp);
	//printf("added %s\n", pp.c_str());
}// addPath


char cmd[257];


static void toExec(const string &fpath)
{
	strcpy(cmd, "\"");
	strcat(cmd, fpath.c_str());
	strcat(cmd, "\"");

	std::vector<string> args = config.getArgs();
	for ( int i = 3; i < args.size(); i++ ){
		strcat(cmd, " ");
		strcat(cmd, args[i].c_str());
	}
	system(cmd);
}// toExec


static void About()
{
	printf("Searching .COM, .EXE, .BAT or .DLL files by environment variable PATH.\n"
	       "While searching .DLLs, also uses WINBOOTDIR and SYSTEM(32) subdirectory.\n"
	       "\n"
	       "CALL:  %s [-key [...]] progname[.ext]\n"
	       "\n"
	       "You may use wildcards in 'progname'.\n"
	       "Available keys:\n"
	       "  -c, -c:l, -c:N   - copy first, last or N-th entry to current directory\n"
	       "  -d, -d:l, -d:N   - delete such entry\n"
	       "  -e, -e:l, -e:N   - execute entry\n"
	       ,
	       config.getname().c_str());
	exit(0);
}// About


int Main()
{
	printf("WhereIs.  ver %s   (c) 1998-2016  Gerasim#  [%s %s]\n\n", 
	       GetVer().c_str(), GetVer("Date").c_str(), GetVer("Time").c_str());
	//printf("WhereIs.  v %s  (c) 1998-2005  Gerasim\017  [%s]\n\n", ver, dat);
	string pgm = config.get("cmdline", "1");
	if ( pgm.empty() )  About();

	if ( config.get(opt, "c") == "l" )  config.set(opt, "c", "-1");
	if ( config.get(opt, "d") == "l" )  config.set(opt, "d", "-1");
	if ( config.get(opt, "e") == "l" )  config.set(opt, "e", "-1");

	string e_path = config.findKeyIC(env, "PATH");
	string e_win = config.findKeyIC(env, "WINDIR");

	addPath("");
	curdir = GetCurPath();
	{
		std::vector<string> pth = SplitString(config.get(env, e_path), ";:");
		for ( unsigned i = 0; i < pth.size(); i++ )  addPath(pth[i]);
	}

	bool ex = false;
	if ( pgm.find_first_of(".") == string::npos ){
		ex = true;
	}else{
		string ext = toUpper(ExtractFileExt(pgm));
		if ( ext == "DLL" || ext == "OCX" ){
			dll = true;
			string windir = config.get(env, e_win);
			addPath(windir);
			addPath(windir + "/SYSTEM");
			addPath(windir + "/SYSTEM32");
		}else if ( !config.findKeyIC(mn, ext).empty() ){
			string Ext = config.findKeyIC(mn, ext);
			//printf("ext: %s\n", Ext.c_str());
			//printf("path: %s\n", config.getTemplate(mn, Ext).c_str());
			std::vector<string> pth = SplitString(config.getTemplate(mn, Ext), ";:");
			//std::vector<string> pth = SplitString(config.get(mn, Ext), ";");
			for ( unsigned i = 0; i < pth.size(); i++ )  addPath(pth[i]);
		}
	}
	printf("Searching file(s) \"%s\" ...\n\n", pgm.c_str());

	srch(pgm, ex);
	if ( out == 0 ){
		printf("Path not found\n");
	}else{
		if ( !_copy.empty() ){
			printf("\nCopying '%s' ...\n", _copy.c_str());
			fileCopy(_copy);
		}
		if ( !_del.empty() ){
			printf("\nDeleting '%s' ...\n", _del.c_str());
			fileDel(_del);
		}
		if ( !_exec.empty() ){
			toExec(_exec.c_str());
		}
	}
	return 0;
}// Main
