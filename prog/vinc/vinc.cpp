// This file created by MkExe ver. 1.4.5 [Oct 22 2003]

#include <cMain.h>
#include <iostream>
using namespace std;

#include <string.h>

#include "tFile.h"
#include "fString.h"
//#include "fTime.h"
#include <time.h>


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME vinc

extern char _binary_vinc_rc_start[];
extern char _binary_vinc_rc_end[];

const char* getRCStart() { return _binary_vinc_rc_start; }
const char* getRCEnd() { return _binary_vinc_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__



static void About()
{
	cout << "Updates resource file .rc, incrementing file version and" << endl;
	cout << "sets current date and time of build.  Resource-keys are :" << endl;
	cout << "   VALUE \"FileVersion\", \"A.B.C\\0\"" << endl;
	cout << "   VALUE \"Date\", \"YYYY-MM-DD\\0\"" << endl;
	cout << "   VALUE \"Time\", \"hh:mm:ss\\0\"" << endl << endl;
	cout << "Call :  " << config.getname() << " [--key [...]] fname.rc" << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  --cfg:FileName  - use config file 'FileName' (default '" << config.getname("cfg") << "')" << endl;
	cout << endl;
	exit(0);
}// About


static void struprn(char *s, int n)
{
	char *ptr = s;
	while(*ptr != '\0' && n--){
		*ptr = toupper(*ptr);
		ptr++;
	}
}// struprn


static int compareIC(const string& s1, const string& s2)
{
	char buf1[1024], buf2[1024];
	strcpy(buf1, s1.c_str()); struprn(buf1, 1023);
	strcpy(buf2, s2.c_str()); struprn(buf2, 1023);
	return strcmp(buf1, buf2);
}// compareIC


int Main()
{
	cout << "Version increment.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.get("option", "?") == "1" )  About(); 

	string fname = config.get("cmdline", "1");
	if ( fname.empty() )  About();

	std::vector<string> list;
	if ( !tRTFile::LoadFile(list, fname) ){
		nError::Throw("Can't read file '" + fname + "'");
	}

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
//	string cYear = buf;


//	tTimeS tm;
	for ( size_t i = 0; i < list.size(); i++ ){
		string s = list[i];
		string sp = s.substr(0, s.find_first_not_of(" \t"));

		s = trim(s);
		if ( s.find("VALUE") == 0 ){
			s = trim(s.substr(5));
			if ( s.find("\"FileVersion\",") == 0 ){                                  // VALUE "FileVersion"
				s = trim(s.substr(14));
				s = kill_all_of(s, "\"");
				s = s.substr(0, s.find_first_of("\\"));

				unsigned pp = s.find_last_not_of(cs_09) + 1;
				unsigned n = ToInt(s.substr(pp), 1);
				s = s.substr(0, pp) + ToString(++n);
				cout << "Version set to : " << s << endl;
				list[i] = sp + "VALUE \"FileVersion\", \"" + s + "\\0\"";
			}else if ( compareIC(s.substr(0, 7), "\"Date\",") == 0 ){                // VALUE "Date"
				list[i] = sp + "VALUE \"Date\", \"" + cDate + "\\0\"";
			}else if ( compareIC(s.substr(0, 7), "\"Time\",") == 0 ){                // VALUE "Time"
				list[i] = sp + "VALUE \"Time\", \"" + cTime + "\\0\"";
//			}else if (){                                         // VALUE "ProductVersion"
			}
		}else if ( s.find("FILEVERSION") == 0 ){                                   // FILEVERSION
			s = trim(s.substr(11));
			unsigned pp = s.find_last_not_of(cs_09) + 1;
			unsigned n = ToInt(s.substr(pp), 1);
			s = s.substr(0, pp) + ToString(++n);
			list[i] = sp + "FILEVERSION " + s;
		}
	}

	if ( !tFILE::SaveFile(list, fname) ){
		nError::Throw("Can't save file '" + fname + "'");
	}

	return 0;
}// Main
