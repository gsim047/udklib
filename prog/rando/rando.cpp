// This file was created by MkExe ver. 1.0.15.80 at [2025-02-15 09:19:14]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
#include "tFile.h"
#include "fRand.h"
#include "tFList.h"
#include "tDebug.h"


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME rando

extern char _binary_rando_rc_start[];
extern char _binary_rando_rc_end[];

const char* getRCStart() { return _binary_rando_rc_start; }
const char* getRCEnd() { return _binary_rando_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__


static string __home;


static void dp(const string &s)
{
	cout << ": " << s << endl;
}// dp

static void dp(int n)
{
	dp(ToString(n));
}// dp


static bool beginsWith(const string &s, const string &ex, string &tail)
{
	string ss = s.substr(0, ex.size());
	if ( ss != ex )
		return false;
	tail = s.substr(ex.size());
	return true;
}//


static string check(const string &s)
{
	string s1 = Replace(s, "~", __home);
	s1 = ch_shrink(s1, '.');
	s1 = Replace(s1, "." + get_pd(), "");             // "./" --> ""
	s1 = Replace(s1, get_pd() + get_pd(), get_pd());  // "//" --> "/"
	return trim(s1);
}



std::vector<string> loadTodoList(const string &fn_todo, const std::map<string, int> &dn, std::map<string, string> &td)
{
	std::vector<string> ret;
	string base = config.get("option", "base", config.getpath()); // =~/foto
	string cur;
	int n_skip = 0;
	if ( __home.empty() )
		__home = config.get("environ", "HOME");
	string comm;

	tRTFile fi(fn_todo);
	while ( !fi.eof() ){
		string s = fi.read();
		dp(":: " + s);
		s = trim(s);
		if ( s.empty() )
			continue;

		string s1 = s.substr(0, 1);
		if ( s1 == "#" )
			continue;
		if ( s1 == ":" )
			continue;

		string s2;
		if ( beginsWith(s, "=base ", s2) ){
			dp("base... ");
			s2 = check(s2);
			if ( !s2.empty() ){
				base = s2;
			}else{
				base = config.get("option", "base", config.getpath());
			}
			if ( !base.empty() ){
				if ( base.substr(base.size()-1) == get_pd() ){
					base = base.substr(0, base.size()-1);
				}
			}
			dp("=" + base);
			continue;
		}

		if ( beginsWith(s, "=comm ", s2) ){
			comm = trim(s2);
			dp("comm: " + comm);
		}
		if ( beginsWith(s, "=info ", s2) ){
			comm = trim(s2);
			dp("comm: " + comm);
		}
		if ( s1 == "=" )
			continue;

		if ( s1 == get_pd() ){   // == "/"
			cur = s;
			dp("cur " + cur);
			continue;
		}

		if ( s.size() > 1 ){
			string s2 = s.substr(0, 2);
			if ( s2 == "~/" ){
				cur = check(s);
				dp("cur ~/ ==> " + cur);
				continue;
			}
			if ( s2 == "./" ){
				cur = check(base + s.substr(1));
				dp("cur ./ ==> " + cur);
				continue;
			}

			if ( s.size() > 2 ){
				s2 = s.substr(1, 2);
				if ( s2 == ":\\" ){
					cur = check(s);
					dp("cur ?: ==> " + cur);
					continue;
				}
			}
		}

		if ( cur.empty() ){
			cur = base;
		}else{
			if ( cur.substr(cur.size()-1) == get_pd() ){
				cur = cur.substr(0, cur.size()-1);
			}
		}
		cur = check(cur);

		// split #
		dp("split1"); 
		size_t pos = s.find("#");
		dp("split2 pos:" + ToString(pos)); 
		string comm1;
		if ( pos != string::npos ){
			dp("split3");
			comm1 = trim(s.substr(pos+1));
			dp("s: [" + s + "] pos:" + ToString(pos));
			s = trim(s.substr(0, pos));
			dp("split4 s:" + s);
		}

		if ( !s.empty() ){

			dp("comm: [" + comm + "]  comm1: [" + comm1 + "]");

			if ( !comm.empty() ){
				if ( !comm1.empty() ){
					comm1 = comm + " " + comm1;
				}else{
					comm1 = comm;
				}
			}else{
				// comm1
			}
			comm1 = Replace(comm1, "\"", "'");

			if ( s.substr(0, 1) == "*" ){
				tFList fl(cur, false);
				tFList fl2 = fl.selectMask(s);
				for ( size_t i = 0; i < fl2.size(); i++ ){
					string fn = fl2[i];
					dp("todo test*: " + fn);
					{
						std::map<string, int>::const_iterator it = dn.find(fn);
						if ( it != dn.end() ){
							if ( it->second == 1 ){
								n_skip++;
								dp("skip...");
								continue;
							}
						}
					}
					{
						std::map<string, string>::const_iterator it = td.find(fn);
						if ( it != td.end() ){
							n_skip++;
							dp("skip...");
							continue;
						}
					}
					ret.push_back(fn);
					td[fn] = comm1;
				}
				continue;
			}


			s = cur + get_pd() + s;
			dp("todo test1: " + s);
			std::map<string, int>::const_iterator it = dn.find(s);
			if ( it != dn.end() ){
				if ( it->second == 1 ){
					n_skip++;
					dp("skip...");
					continue;
				}
			}
			ret.push_back(s);
			td[s] = comm1;
		}
	}
	dp("skip: " + ToString(n_skip));
	return ret;
}//

/*
void saveTodoList(const string &fn_todo, const std::vector<string> &todo, unsigned nn)
{
	tFILE fo(fn_todo, "wt");
	if ( fo.isOpen() ){
		for ( unsigned i = 0; i < todo.size(); i++ ){
			if ( i == nn )
				continue;
			fo.write(todo[i] + "\n");
		}
	}
}//
*/

std::map<string, int> loadDoneList(const string &fn_done)
{
	std::map<string, int> ret;
	tRTFile fi(fn_done);
	while ( !fi.eof() ){
		string s = fi.read();
		s = trim(s);
		unsigned pos = s.find("#");
		if ( pos != string::npos ){
			s = trim(s.substr(0, pos));
		}
		if ( !s.empty() ){
			dp("done: " + s);
			ret[s] = 1;
		}
	}
	return ret;
}//


void saveDoneList(const string &fn_done, const string &fn)
{
	tFILE fo(fn_done, "at");
	if ( fo.isOpen() ){
		fo.write(fn + "\n");
	}
}//


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
	cout << "rando.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	//string fin = config.get("cmdline", "1");
	//if ( fin.empty() )  About();

	// <--- Your code
	string fn_todo = config.get("option", "todo"); // =todo.lst
	string fn_done = config.get("option", "done"); // =done.lst

//	string path_base = config.get("option", "base"); // =~/foto
	string cmd1 = config.get("option", "exec"); // =sendtg.sh %1
	
//	dp("1 - " + fn_done);
	std::map<string, int> dn = loadDoneList(fn_done);
	dp("done size: " + ToString(dn.size()));
	std::map<string, string> td;
	dp("todo: " + fn_todo);
	std::vector<string> todo = loadTodoList(fn_todo, dn, td);
	dp("todo size: " + ToString(todo.size()));

	while ( !todo.empty() ){
		unsigned nn = Grandom32(todo.size());
//		dp("5. " + ToString(nn));
		string fn = todo[nn];
		//saveTodoList(fn_todo, todo, nn);
		saveDoneList(fn_done, fn);
    
        if ( tFILE::FileExists(fn) ){
			string cmd2 = Replace(cmd1, "%%FILE%%", fn);
			cmd2 = Replace(cmd2, "%%COMM%%", td[fn]);
			dp(cmd2);
			system(cmd2.c_str());
			break;
		}

		todo.erase(todo.begin() + nn);
	}

	return 0;
}// Main
