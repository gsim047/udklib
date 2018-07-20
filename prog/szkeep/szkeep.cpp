// This file was created by MkExe ver. 1.0.15.60 at [2018-04-25 17:22:45]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;
#include <algorithm>

#include "fString.h"
#include "tFile.h"
//#include "tDebug.h"
#include "fVector.h"
#include "tFList.h"


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME szkeep

extern char _binary_szkeep_rc_start[];
extern char _binary_szkeep_rc_end[];

const char* getRCStart() { return _binary_szkeep_rc_start; }
const char* getRCEnd() { return _binary_szkeep_rc_end; }

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




class fileInf
{
public:
	fileInf() : size(0) {}
	fileInf(const fileInf &s) : name(s.name), size(s.size), date(s.date) {}
	fileInf(const string &_name, int _size, const string &_date) : name(_name), size(_size), date(_date) {}

	fileInf &operator=(const fileInf &s){
		if ( this != &s ){
			name = s.name; size = s.size; date = s.date;
		}
		return *this;
	}
	
	bool operator==(const fileInf &s) const { return name == s.name && size == s.size && date == s.date; }
	bool operator<(const fileInf &s) const { return date < s.date || name < s.name || size < s.size; }

	int szK() const { return ((size + 8*1024 - 1) >> 14) << 4; }

public:
	string name;
	int size;
	string date;
};// fileInf



class __cmpDate {
public:
	bool operator()(const fileInf &v1, const fileInf &v2) const {
		bool ret = (v1.date < v2.date);
		if ( v1.date == v2.date )  ret = (v1.name < v2.name);
		return ret;
	}
};// class __cmpDate




int Main()
{
	cout << "###.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	string path = config.get("option", "path", ".");
	int pc = config.get("option", "free", 20);

	string out = "./szkeep.txt";
	string lst = "./szkeep.lst";
	//string fin = config.get("cmdline", "1");
	//if ( fin.empty() )  About();

	// <--- Your code
	printf("path: [%s]\n", path.c_str());
	printf("free: %d%%\n", pc);

	string cmd1 = "df " + path + " > " + out;
	system(cmd1.c_str());
//	string cmd2 = "du -s " + path + " >> " + out;
//	system(cmd2.c_str());

	tRTFile fi(out);
	string s = fi.read();
	s = fi.read();
	printf("[%s]\n", s.c_str());
	string s1 = ch_shrink(s, ' ');  // ..cccc.. -> ..c..
	std::vector<string> v1 = SplitString(s1, " ");

	printf("\n");
	for ( int i = 0; i < v1.size(); i++ ){
		printf("[%s] ", v1[i].c_str());
	}
	printf("\n");
	int d_all = ToInt(v1[1]);
	int d_used = ToInt(v1[2]);
	int d_free = ToInt(v1[3]);

//	printf("all: %dK\n", d_all);
//	printf("used: %dK\n", d_used);
//	printf("free: %dK\n", d_free);

	int pcc = 100.*d_free/d_all;
//	printf("free: %lf%%  %dK\n", 100.*d_free/d_all, d_free);
	printf("free: %d%%  %dK\n", pcc, d_free);
	if ( pcc < pc ){
//		printf("d_all %dK // %dK\n", d_all, (int)(d_all/100.)*pc);
		int del = (int)(d_all/100.)*pc - d_free;
		printf("==> to free %dK\n", del);
		/*
		tFIList fi(path + "/*", true);
		printf("fi size:%d\n", fi.size());
		for ( size_t j = 0; j < fi.size(); j++ ){
			tFileAttr fa = fi[j];
			printf("%d: [%s]\n", j+1, fa.name.c_str());
		}*/
		
		string cmd3 = "ls -c -lt -R --full-time " + path + " > " + lst;
		system(cmd3.c_str());

		string curPath;
		std::vector<fileInf> vv;

		tRTFile fl(lst);
		while ( !fl.eof() ){
			string s = fl.read();
			string ss = s.substr(0, 1);
			if ( ss == "/" || ss == "." ){
				curPath = s.substr(0, s.size()-1);  // drop tail :
				continue;
			}
			if ( ss == "d" ){
				continue;
			}
			if ( s.size() < 50 )
				continue;

			string s2 = ch_shrink(s, ' ');  // ..cccc.. -> ..c..
			std::vector<string> v2 = SplitString(s2, " ");
			
		//	for ( int i = 0; i < v2.size(); i++ ){
		//	}
//			printf("\n");
			vv.push_back(fileInf(curPath + "/" + v2[8], ToInt(v2[4]), v2[5] + " " + v2[6]));
			/*
			for ( int i = 0; i < v2.size(); i++ ){
				printf("%d:[%s] ", i, v2[i].c_str());
			}
			printf("\n");*/
		}

		std::stable_sort(vv.begin(), vv.end(), __cmpDate());
		int sum = 0;
		int n = 0;
		for ( ; n < vv.size(); n++ ){
			fileInf& fi = vv[n];
			sum += fi.szK();
			if ( sum >= del )
				break;
//			printf("[%s] %10d [%s]\n", fi.date.c_str(), fi.size, fi.name.c_str());
		}
		printf("to del %d file(s) sum:%d\n", n, sum);
	}


	return 0;
}// Main
