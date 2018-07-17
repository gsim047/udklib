#include "tConfiguration.h"

#include <stdio.h>
#include <sys/stat.h>

#include <algorithm>

#ifdef __GNUG__
#include <unistd.h>
#endif // __GNUG__

#ifdef _WIN32
#include <dos.h>
#include <io.h>
#include <windows.h>
#endif // _WIN32

#include "tFile.h"
//#include "fFile.h"
//#include "tDebug.h"




typedef std::map<string, string>::const_iterator mss_ci;
//typedef std::map<string, std::map<string, string> >::const_iterator msmss_ci;
typedef std::map<string, tConfiguration::mss >::const_iterator msmss_ci;
typedef std::map<string, string>::iterator mss_i;
//typedef std::map<string, std::map<string, string> >::iterator msmss_i;
typedef std::map<string, tConfiguration::mss >::iterator msmss_i;


const char * const tConfiguration::mainSection    = "main";
const char * const tConfiguration::environSection = "environ";
const char * const tConfiguration::optionSection  = "option";
const char * const tConfiguration::cmdlineSection = "cmdline";
const char * const tConfiguration::progValue      = "prog";
const char * const tConfiguration::pathValue      = "path";
const char * const tConfiguration::nameValue      = "name";

int tConfiguration::precision = 3;


tConfiguration::tConfiguration(const string &filename)
{
	loadEnv();
	if ( !load(filename) ); // errorLog("tConfiguration(): can't open file \"" + filename + "\"");
	// nError::Throw(nError::NoFile(filename));
}// tConfiguration::tConfiguration


tConfiguration::tConfiguration(const tConfiguration &src) :
	vals(src.vals),
	cfgFile(src.cfgFile)
{
}// tConfiguration::tConfiguration


tConfiguration &tConfiguration::operator=(const tConfiguration &src)
{
	if ( this != &src ){
		vals = src.vals;
		cfgFile = src.cfgFile;
		loadCmdLine(src.all);
	}
	return *this;
}// tConfiguration::operator=


tConfiguration &tConfiguration::operator+=(const string &filename)
{
	if ( !load(filename) ); // errorLog("tConfiguration+=(): can't open file \"" + filename + "\"");
	// )  nError::Throw(nError::NoFile(filename));
	return *this;
}// tConfiguration::operator+=


tConfiguration &tConfiguration::operator+=(const tConfiguration &src)
{
	if ( this != &src ){
		for ( msmss_ci i = src.vals.begin(); i != src.vals.end(); ++i ){
			for ( mss_ci j = i->second.begin(); j != i->second.end(); ++j ){
				vals[i->first][j->first] = j->second;
			}
		}
		//for ( size_t l = 0; l < src.fname.size(); l++ ) fname.push_back(src.fname[l]);
	}
	return *this;
}// tConfiguration::operator+=


void tConfiguration::reload()
{
	//tMutexLock ml(m);
	vals.clear();
	load(cfgFile);
}// tConfiguration::reload


void tConfiguration::loadEnv()
{
	//if ( vals.find(environSection) != vals.end() )  return;
#ifdef _WIN32
	char **env = _environ;
#endif // _WIN32

#ifdef __GNUG__
#ifdef __CYGWIN__
	char **env = environ;
#else
	char **env = __environ;
#endif // __CYGWIN__
#endif // __GNUG__

	for ( unsigned i = 0; env[i] && env[i][0] != '\0'; i++ ){
		string key;
		string val;
		if ( !splitLine(env[i], key, val) ) continue;
		if ( key.empty() ) continue;
		set(environSection, key, val);
	}
}// tConfiguration::loadEnv


bool tConfiguration::isComment(const string &s)
{
	return s[0] == '#' || s[0] == ';' || s[0] == '*';
}// tConfiguration::isComment


bool tConfiguration::isSection(const string &s)
{
	return s[0] == '[' && s[s.size() - 1] == ']';
}// tConfiguration::isSection


string tConfiguration::getSection(const string &s)
{
	return trim(s.substr(1, s.size() - 2));
}// tConfiguration::getSection


bool tConfiguration::load(const string &filename)
{
	cfgFile = filename;

	tRTFile f(filename);
	if ( !f.isOpen() )  return false;
	int n = 0;
	string section(mainSection);
	while ( !f.eof() ){
		string s = f.read();
		n++;
		s = trim(s);
		if ( s.empty() ) continue;
		if ( isComment(s) ) continue;

		if ( isSection(s) ){
			section = getSection(s);
			continue;
		}

		string key;
		string val;
		if ( !splitLine(s, key, val) ){
			nError::Throw(nError::InvalidLine(filename, n, s));
		}
		if ( section == optionSection ){
			if ( !findOpt(key) )  set(section, key, val);
		}else{
			set(section, key, val);
		}
	}
	//fname.push_back(filename);
	return true;
}// tConfiguration::load


bool tConfiguration::splitLine(const string &s, string &key, string &val)
{
	//if ( s.size() < 2 )  return false;
	size_t p = s.find_first_of('=', 1);
	if ( p == string::npos || p == 0 )  return false;
	key = trimr(s.substr(0, p));
	if ( key.empty() )  return false;
	val = trimr(s.substr(p + 1));
	return true;
}// tConfiguration::splitLine


bool tConfiguration::find(const string &sect) const
{
	auto i = vals.find(sect);
	return (i != vals.end());
}// tConfiguration::find


bool tConfiguration::find(const string &sect, const string &key) const
{
	auto i = vals.find(sect);
	if ( i != vals.end() ){
		mss_ci j = i->second.find(key);
		if ( j != i->second.end() )  return true;
	}
	return false;
}// tConfiguration::find


bool tConfiguration::findIC(const string &sect) const
{
	if ( vals.find(sect) != vals.end() )  return true;
	for ( msmss_ci i = vals.begin(); i != vals.end(); ++i ){
		if ( CompareIC(i->first, sect) == 0 )  return true;
	}
	return false;
}// tConfiguration::findIC


bool tConfiguration::findIC(const string &sect, const string &key) const
{
	auto i = vals.find(sect);
	if ( i == vals.end() )  return false;
	if ( i->second.find(key) != i->second.end() )  return true;
	for ( auto j = i->second.begin(); j != i->second.end(); ++j ){
		if ( CompareIC(j->first, key) == 0 )  return true;
	}
	return false;
}// tConfiguration::findIC


string tConfiguration::get(const string &sect,
                           const string &key,
                           const string &def) const
{
	//tMutexLock ml(m);
	string ret = def;
	auto i = vals.find(sect);
	if ( i != vals.end() ){
		auto j = i->second.find(key);
		if ( j != i->second.end() ){
			ret = j->second;
		}
	}
	return ret;
}// tConfiguration::get


bool tConfiguration::getBool(const string &sect,
                             const string &key,
                             bool def) const
{
	//tMutexLock ml(m);
	bool ret = def;
	auto i = vals.find(sect);
	if ( i != vals.end() ){
		auto j = i->second.find(key);
		if ( j != i->second.end() ){
			ret = (j->second != "0");
		}
	}
	return ret;
}// tConfiguration::get


int tConfiguration::get(const string &sect,
                        const string &key,
                        int def) const
{
	//tMutexLock ml(m);
	int ret = def;
	auto i = vals.find(sect);
	if ( i != vals.end() ){
		auto j = i->second.find(key);
		if ( j != i->second.end() ){
			size_t sz = 0;
			try{
				ret = std::stoi(j->second, &sz);
			}
			catch(...){
			}
			if ( sz == 0 ){
				ret = def;
			}
//			ret = ToIntO(j->second, def);
		}
	}
	return ret;
}// tConfiguration::get


int tConfiguration::get(const string &sect,
                        const string &key,
                        int def, int Min, int Max) const
{
	int ret = get(sect, key, def);
	if ( ret < Min )  ret = Min;
	if ( ret > Max )  ret = Max;
	return ret;
}// tConfiguration::get


double tConfiguration::get(const string &sect,
                           const string &key,
                           double def) const
{
	//tMutexLock ml(m);
	double ret = def;
	auto i = vals.find(sect);
	if ( i != vals.end() ){
		auto j = i->second.find(key);
		if ( j != i->second.end() ){
			ret = ToDouble(j->second, def);
		}
	}
	return ret;
}// tConfiguration::get


double tConfiguration::get(const string &sect,
                           const string &key,
                           double def, double Min, double Max) const
{
	double ret = get(sect, key, def);
	if ( ret < Min )  ret = Min;
	if ( ret > Max )  ret = Max;
	return ret;
}// tConfiguration::get


void tConfiguration::set(const string &sect,
                         const string &key,
                         const string &val)
{
	//tMutexLock ml(m);
	vals[sect][key] = val;
}// tConfiguration::set


void tConfiguration::set(const string &sect,
                         const string &key,
                         int val)
{
	//tMutexLock ml(m);
	vals[sect][key] = std::to_string(val);
}// tConfiguration::set


void tConfiguration::set(const string &sect,
                         const string &key,
                         int val, int Min, int Max)
{
	//tMutexLock ml(m);
	if ( val < Min )  val = Min;
	if ( val > Max )  val = Max;
	vals[sect][key] = std::to_string(val);
}// tConfiguration::set


void tConfiguration::set(const string &sect,
                         const string &key,
                         double val)
{
	//tMutexLock ml(m);
//	vals[sect][key] = ToStringD(val, precision);
	vals[sect][key] = std::to_string(val); //, precision);
}// tConfiguration::set


void tConfiguration::set(const string &sect,
                         const string &key,
                         double val, double Min, double Max)
{
	//tMutexLock ml(m);
	if ( val < Min )  val = Min;
	if ( val > Max )  val = Max;
//	vals[sect][key] = ToStringD(val, precision);
	vals[sect][key] = std::to_string(val); //, precision);
}// tConfiguration::set


void tConfiguration::del(const string &sect)
{
	if ( vals.find(sect) != vals.end() ){
		vals.erase(sect);
	}
}// tConfiguration::del


void tConfiguration::del(const string &sect, const string &key)
{
	if ( find(sect, key) ){
		vals[sect].erase(key);
		if ( vals[sect].empty() )  vals.erase(sect);
	}
}// tConfiguration::del


std::vector<string> tConfiguration::getSections() const
{
	//tMutexLock ml(m);
	std::vector<string> ret;
	for ( auto i = vals.begin(); i != vals.end(); ++i ){
		ret.push_back(i->first);
	}
	return ret;
}// tConfiguration::getSections


std::vector<string> tConfiguration::getKeys(const string &sect) const
{
	//tMutexLock ml(m);
	std::vector<string> ret;
	auto i = vals.find(sect);
	if ( i != vals.end() ){
		for ( auto j = i->second.begin(); j != i->second.end(); ++j ){
			ret.push_back(j->first);
		}
	}
	return ret;
}// tConfiguration::getKeys


void tConfiguration::setOpt(const string &key)
{
	if ( !findOpt(key) )  opts.push_back(key);
}// tConfiguration::setOpt


bool tConfiguration::findOpt(const string &key) const
{
	return std::find(opts.begin(), opts.end(), key) != opts.end();
}// tConfiguration::findOpt


void tConfiguration::loadCmdLine(int argc, char *argv[])
{
	std::vector<string> All;
	for ( int j = 0; j < argc; j++ ) All.push_back(argv[j]);

	loadCmdLine(All);
}// tConfiguration::loadCmdLine


void tConfiguration::loadCmdLine(const std::vector<string> &All)
{
	all = All;
	//for ( int j = 0; j < argc; j++ ) all.push_back(argv[j]);
	string name;
	if ( !all.empty() )  name = all[0];

#ifdef __GNUG__
	{
		char buf[300];
		int n = readlink("/proc/self/exe", buf, 300);
		if ( n > 0 ){
			name = string(buf, n);
		}
	}
#endif

#ifdef _WIN32
	{
		char buf[MAX_PATH+1];
		memset(buf, 0, MAX_PATH+1);

		//DWORD ret = GetModuleFileName(NULL, buf, MAX_PATH+1);
		DWORD ret = GetModuleFileName(GetModuleHandle(NULL), buf, MAX_PATH+1);
		if ( ret ){
			name = string(buf, ret);
		}
		/*
		string fname = name;
		string dir = fname;
		dir.resize(dir.find_last_of("\\/") + 1);
		fname = fname.substr(fname.find_last_of("\\/") + 1);
		//string mask = dir + "*.*";

		WIN32_FIND_DATA wfd;
		HANDLE h = FindFirstFile(fname.c_str(), &wfd);
		if ( h != INVALID_HANDLE_VALUE ){
			for (;;){
				if ( CompareIC(fname, wfd.cFileName) == 0 ){
					fname = wfd.cFileName;
					name = dir + fname;
					break;
				}
				if ( !FindNextFile(h, &wfd) ) break;
			}
			FindClose(h);
		} */
	}
#endif // _WIN32

	set(cmdlineSection, progValue, name);
	set(cmdlineSection, nameValue, ExtractFileNameExt(name));
	set(cmdlineSection, pathValue, ExtractFilePath(name));

	size_t args = 1;
	for ( size_t i = 1; i < all.size(); i++ ){
		if ( all[i].find("--") == 0 ){                     // --opt
			string key = all[i].substr(2);
			string val = "1";
			size_t pos = key.find_first_of(":=");
			if ( pos != string::npos ){
				val = key.substr(pos + 1);
				key.resize(pos);
			}else if ( key.substr(key.size() - 1) == "-" ){
				key.resize(key.size() - 1);
				val = "0";
			}else if ( key.substr(key.size() - 1) == "+" ){
				key.resize(key.size() - 1);
				val = "1";
			}
			set(optionSection, key, val);
			setOpt(key);
			continue;
		}
		if ( all[i].find('-') == 0 ){                      // -o
			string keys = all[i].substr(1);
			for ( ; !keys.empty(); ){
				if ( keys.size() > 1 ){
					if ( keys[1] == ':' || keys[1] == '=' ){
						string Key = keys.substr(0, 1);
						set(optionSection, Key, keys.substr(2));
						setOpt(Key);
						keys = "";
						continue;
					}
					if ( keys[1] == '-' ){
						string Key = keys.substr(0, 1);
						set(optionSection, Key, "0");
						setOpt(Key);
						keys.erase(0, 2);
						continue;
					}
					if ( keys[1] == '+' ){
						string Key = keys.substr(0, 1);
						set(optionSection, Key, "1");
						setOpt(Key);
						keys.erase(0, 2);
						continue;
					}
				}
				string Key = keys.substr(0, 1);
				set(optionSection, Key, "1");
				setOpt(Key);
				keys.erase(0, 1);
			}
			continue;
		}
//		set(cmdlineSection, ToStringSz(args++), all[i]);     // arg
		set(cmdlineSection, std::to_string(args++), all[i]);     // arg
	}
	//cout << "tConfiguration::loadCmdLine name = " << name << endl;
}// tConfiguration::loadCmdLine


string tConfiguration::getpath() const 
{
	return get(cmdlineSection, pathValue); 
}// tConfiguration::getpath


string tConfiguration::getname() const 
{
	return get(cmdlineSection, nameValue); 
}// tConfiguration::getname


string tConfiguration::getname(const string &ext) const 
{
	return ChangeFileExt(get(cmdlineSection, nameValue), ext); 
}// tConfiguration::getname


string tConfiguration::getfile() const 
{
	return get(cmdlineSection, progValue); 
}// tConfiguration::getfile


string tConfiguration::getfile(const string &ext) const
{
	return ChangeFileExt(get(cmdlineSection, progValue), ext);
}// tConfiguration::getfile


string tConfiguration::findSectionIC(const string &sect) const
{
	if ( vals.find(sect) != vals.end() )  return sect;
	for ( auto i = vals.begin(); i != vals.end(); ++i ){
		if ( CompareIC(i->first, sect) == 0 )  return i->first;
	}
	return "";
}// tConfiguration::findSectionIC


string tConfiguration::findKeyIC(const string &sect, const string &key) const
{
	auto i = vals.find(sect);
	if ( i == vals.end() )  return "";
	if ( i->second.find(key) != i->second.end() )  return key;
	for ( auto j = i->second.begin(); j != i->second.end(); ++j ){
		if ( CompareIC(j->first, key) == 0 )  return j->first;
	}
	return "";
}// tConfiguration::findKeyIC


void tConfiguration::save() const
{
	//std::map<string, size_t> startline;
	std::map<string, size_t> endline;
	std::map<string, mss > tvals = vals;

	std::vector<string> v;
	tRTFile::LoadFile(v, cfgFile);
	{
		string section = mainSection;
		if ( tvals.find(section) != tvals.end() )  endline[section] = 0;
		//std::map<string, string> cursect = tvals[section];
		{
			for ( size_t i = 0; i < v.size(); i++ ){
				//if ( endline.find(section) == endline::end() )  endline[section] = i;
				string s = trim(v[i]);
				if ( s.empty() ) continue;
				if ( isComment(s) ) continue;

				if ( isSection(s) ){
					section = getSection(s);
					if ( tvals.find(section) != tvals.end() )  endline[section] = i;
					continue;
				}

				string key;
				string val;
				if ( !splitLine(s, key, val) )  continue;
				if ( find(section, key) ){
					if ( get(section, key) != val ){
						v[i] = v[i].substr(0, v[i].find_first_of("=") + 1) + get(section, key);
					}
					endline[section] = i;
				}
			}
		}

		tFILE fo(cfgFile, "wt");

		section = mainSection;
		std::map<string, string> cursect = tvals[section];
		{
			for ( size_t i = 0; i < v.size(); i++ ){
				string s = trim(v[i]);
				if ( s.empty() ){
					fo.write(v[i] + "\n");
				}else if ( isComment(s) ){
					fo.write(v[i] + "\n");
				}else if ( isSection(s) ){
					section = getSection(s);
					if ( tvals.find(section) != tvals.end() ){
						cursect = tvals[section];
					}else{
						cursect.clear();
					}
					fo.write(v[i] + "\n");
				}else{

					string key;
					string val;
					if ( !splitLine(s, key, val) ){
						fo.write(v[i] + "\n");
					}else if ( find(section, key) ){
						cursect.erase(key);
						fo.write(v[i] + "\n");
					}
				}
				if ( endline[section] == i && section != environSection && 
				     section != optionSection && section != cmdlineSection){
					for ( auto it = cursect.begin(); it != cursect.end(); ++it ){
						fo.write(it->first + "=" + it->second + "\n");
					}
					tvals.erase(section);
				}
			}
		}
		for ( auto mit = tvals.begin(); mit != tvals.end(); ++mit ){
			if ( mit->first == environSection || mit->first == optionSection || 
			     mit->first == cmdlineSection || mit->second.empty() )  continue;
			fo.write("\n[" + mit->first + "]\n");
			for ( auto it = mit->second.begin(); it != mit->second.end(); ++it ){
				fo.write(it->first + "=" + it->second + "\n");
			}
		}

	}
}// tConfiguration::save


string tConfiguration::getIC(const string &sect, const string &key, const string &def) const
{
	string Sect = findSectionIC(sect);
	if ( Sect.empty() )  return def;
	return get(Sect, findKeyIC(Sect, key), def);
}// tConfiguration::getIC


bool tConfiguration::getBoolIC(const string &sect,
                               const string &key,
                               bool def) const
{
	string Sect = findSectionIC(sect);
	if ( Sect.empty() )  return def;
	return getBool(Sect, findKeyIC(Sect, key), def);
}// tConfiguration::getIC


int tConfiguration::getIC(const string &sect,
                          const string &key,
                          int def) const
{
	string Sect = findSectionIC(sect);
	if ( Sect.empty() )  return def;
	return get(Sect, findKeyIC(Sect, key), def);
}// tConfiguration::getIC


double tConfiguration::getIC(const string &sect,
                             const string &key,
                             double def) const
{
	string Sect = findSectionIC(sect);
	if ( Sect.empty() )  return def;
	return get(Sect, findKeyIC(Sect, key), def);
}// tConfiguration::getIC


string tConfiguration::getPath(const string &sect, const string &key, const string &def) const
{
	string ret = get(sect, key, def);
	if ( !ret.empty() && ret[0] == '.' ){
		string p = get(cmdlineSection, pathValue);
		if ( !p.empty() ){
			ret = GetFullPath(p + path_delimiter + ret);
		}
	}
	return ret;
}// tConfiguration::getPath


static int findTmpl(string &t, string &sect, string &key, const string &s)
{
	for ( size_t pos = s.find("%%"); pos != string::npos; pos = s.find("%%", pos + 2) ){
		if ( pos != string::npos && s.size() > (pos + 2) && s[pos + 2] != '%' ){
			size_t pos2 = s.find("%%", pos + 2);
			if ( pos2 != string::npos ){
				string s1 = s.substr(pos, pos2 - pos + 2);
				if ( s1.find(" ") == string::npos ){
					t = s1;
					size_t pos3 = s1.substr(2, s1.size() - 4).find("%");
					if ( pos3 != string::npos ){
						sect = s1.substr(2, pos3);
						key = s1.substr(pos3 + 3, s1.size() - 5 - pos3);
						return 2;
					}
					key = t.substr(2, t.size() - 4);
					return 1;
				}
			}
		}
	}
	return 0;
}// findTmpl


string tConfiguration::getTemplate(const string &sect, const string &key, const string &def) const
{
	string ret = get(sect, key, def);
	if ( !ret.empty() ){
		string t;
		string s2;
		string k2;
		for ( int c = findTmpl(t, s2, k2, ret); c != 0; c = findTmpl(t, s2, k2, ret) ){

			if ( c == 2 ){
				ret = Replace(ret, t, get(s2, k2));
			}else if ( c == 1 ){
				ret = Replace(ret, t, get(sect, k2));
			}else{
				break;
			}
		}
	}
	return ret;
}// tConfiguration::getTemplate



tConfiguration config;
