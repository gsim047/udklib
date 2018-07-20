// This file was created by MkExe ver. 1.0.12.38 at [2016-05-18 12:59:51]
//#include <windows.h>

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include <stdlib.h>

#include "fString.h"
#include "tFile.h"
#include "tRecodeCC.h"
#include "fVector.h"

#include <dirent.h>
#include <algorithm>

#include "tLept.h"
#include "leptonica/allheaders.h"


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME gencs

extern char _binary_gencs_rc_start[];
extern char _binary_gencs_rc_end[];

const char* getRCStart() { return _binary_gencs_rc_start; }
const char* getRCEnd() { return _binary_gencs_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__


static void About()
{
	cout << "Generate Control Sheet. Foto/picture-utility. ";    // some description
	cout << endl << endl;
	cout << "Call :  " << config.getname() << " [key [...]] mask" << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  -b              - 'bulk' mode, where all 'mask*.jpg' files collecting" << endl;
	cout << "                  - if no 'bulk', we use only 'mask-?-?.jpg' files" << endl;
	cout << "  -x:n            - in 'bulk' we can set grid width, default 5" << endl;
	cout << "  --cfg:CfgName   - use config file 'CfgName' (default '" << config.getname("cfg") << "')" << endl;
	cout << endl;
	exit(0);
}// About

//             xxxx12345678  0123456789ab
string hd;  // b101-1-2.jpg  b101-1-2.jpg
int hd_size = 0;
//tRecodeCC cc;


bool filterFN(const string& Name, int& x, int& y)
{
	if ( Name.size() != hd_size+8 ) return false;
	string name = //cc.Recode(Name);
		toLowerA7(Name);
	if ( name.find(hd) != 0 ) return false;
	if ( name[hd_size] != '-' || name[hd_size+2] != '-' ) return false;
	if ( name.substr(hd_size+4) != ".jpg" ) return false;
	char c1 = name[hd_size+1];
	char c2 = name[hd_size+3];
	if ( c1 >= '1' && c1 <= '9' && c2 >= '1' && c2 <= '9' ){
		x = c1 - '1';
		y = c2 - '1';
		return true;
	}
	return false;
}


int Main()
{
	cout << "GenCS.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	string fin = config.get("cmdline", "1");
	if ( fin.empty() )  About();
	hd = fin;
	hd_size = hd.size();

	bool bulk = config.getBool("option", "b", false);
	int wX = config.get("option", "x", 5);

	std::vector<string> fl; // = DirFileList(".");
/*
	cc['A'] = 'a';
	cc['B'] = 'b';
	cc['C'] = 'c';
	cc['D'] = 'd';
	cc['E'] = 'e';
	cc['F'] = 'f';
	cc['G'] = 'g';
	cc['H'] = 'h';
	cc['I'] = 'i';
	cc['J'] = 'j';
	cc['K'] = 'k';
	cc['L'] = 'l';
	cc['M'] = 'm';
	cc['N'] = 'n';
	cc['O'] = 'o';
	cc['P'] = 'p';
	cc['Q'] = 'q';
	cc['R'] = 'r';
	cc['S'] = 's';
	cc['T'] = 't';
	cc['U'] = 'u';
	cc['V'] = 'v';
	cc['W'] = 'w';
	cc['X'] = 'x';
	cc['Y'] = 'y';
	cc['Z'] = 'z';
*/
	std::map<int, string> m;
	int maxx = 0;
	int maxy = 0;
	int defX = 16;

	if ( !bulk ){
		string path = ".";
		DIR *fd = opendir(path.c_str());
		if ( fd ){
			for ( struct dirent *d = readdir(fd); d; d = readdir(fd) ) {
				string name = d->d_name;
				if ( !name.empty() && name[0] == '.' ) continue;
				string fullname = path + string(path_delimiter) + name;
				struct stat statbuf;
				if ( stat(fullname.c_str(), &statbuf) != 0 ) continue;
	//			if ( statbuf.st_mode & fattr ){
					if ( !(statbuf.st_mode & S_IFDIR) ){
	//					if ( FileMatch(mask, name) )  v.push_back(name);
	//						cc.RecodeIP(name);
							int x, y;
							if ( filterFN(name, x, y) ){
								if ( maxx < x ) maxx = x;
								if ( maxy < y ) maxy = y;
								m[defX*x + y] = name;
//								printf("[%s] y:%d x:%d\n", name.c_str(), y, x);
							}
					} 
			}
//			printf("maxx:%d maxy:%d\n", maxx, maxy);
			closedir(fd);
		}
	}

	if ( m.empty() ){
		bulk = true;
		cout << "auto bulk mode ON" << endl;
	}

	if ( bulk ){

		std::vector<string> names = DirFileList(".", hd + "*.jpg");
		std::vector<string> names2;
#ifdef __CYGWIN__
		for ( int i = 0; i < names.size(); i++ ){
			string name = names[i];
			if ( name.size() < hd.size() + 6 ) continue;
			names2.push_back(toLowerA7(names[i]));
		}
#endif
//		printf("---\n");
		std::sort(names2.begin(), names2.end());
		maxy = wX - 1;
		maxx = (names2.size() + wX - 1)/ wX;

		for ( int i = 0, nn = 0; i < names2.size(); i++ ){
			string name = names2[i];
			if ( name.size() < hd.size() + 6 ) continue;
			int x = nn / wX;
			int y = nn % wX;
			m[defX*x + y] = name;
			nn++;
		}
		maxx--;

	}

//	std::sort(fl.begin(), fl.end());

/*
	for ( int i = 0; i <= maxx; i++ ){
		for ( int j = 0; j <= maxy; j++ ){
			string name = m[10*i+j];
			if ( !name.empty() ){
				printf("[%d] ", j);
			}else{
				printf("[x] ");
			}
		}
		printf("\n");
	}
*/
	// size:200 3+3+3+3+1+1 - border
	// <--- Your code
	printf("maxy:%d maxx:%d\n", maxy, maxx);
	tPix px(200*(maxy+1), 200*(maxx+1), 32);
	pixClearAll(px);
//	int res = pixRasterop(px, 0, 0, px.width(), px.height(), PIX_CLR, NULL, 0, 0);

	tPix pxt(194, 194, 32);
	pixSetAllGray(pxt, 127);
	pixSetOrClearBorder(pxt, 4, 4, 4, 4, PIX_CLR);
	pixSetBorderVal(pxt, 3, 3, 3, 3, 0x7f7f7f00);

	for ( std::map<int, string>::const_iterator it = m.begin(); it != m.end(); ++it ){
		int x = it->first / defX;
		int y = it->first % defX;
		string name = it->second;
		if ( name.empty() ) continue;

		cout << name << "...\n";
		tPix pxn(name);
		if ( !pxn.empty() ){
			if ( pxn.depth() < 24 ){
				tPix pxn3 = pixConvertTo32 ( pxn );
				pxn = pxn3;
			}

			pixRasterop(px, 200*y+3, 200*x+3, pxt.width(), pxt.height(), PIX_SRC, pxt, 0, 0);
			int w = pxn.width();
			int h = pxn.height();
			tPix pxn2;
			if ( w >= h ){
				pxn2 = pixScaleToSize(pxn, 186, 186.*h/w);
				pixRasterop(px, 200*y+7, 200*x+7+93-pxn2.height()/2, pxn2.width(), pxn2.height(), PIX_SRC, pxn2, 0, 0);
			}else{
				pxn2 = pixScaleToSize(pxn, 186.*w/h, 186);
				pixRasterop(px, 200*y+7+93-pxn2.width()/2, 200*x+7, pxn2.width(), pxn2.height(), PIX_SRC, pxn2, 0, 0);
			}
		}
	}
	px.writeJpeg(hd + "_.jpg", 80);
	
	return 0;
}// Main
