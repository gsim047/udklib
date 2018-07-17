#include "tFList.h"
#include "tFile.h"
#include "fString.h"
//#include "fTime.h"
#include "tDateTime.h"
#include "tDebug.h"

#ifdef _WIN32
#define NOGDI
#include <windows.h>
//#include <mapiwin.h>
#endif

#ifdef __GNUG__
#include <dirent.h>
#include <unistd.h>
#endif


#include <stdio.h>
#include <algorithm>



/*
static bool match(const string &__mask, const string &__s)
{
	//printf("%s : %s", __mask.c_str(), __s.c_str());
	const char *cp = NULL;
	const char *mp = NULL;
	const char *mask = __mask.c_str();
	const char *s = __s.c_str();

	for ( ; *s && *mask != '*'; mask++, s++ ){
		if ( *mask != *s && *mask != '?' ){
			//printf("   1\n");
			return false;
		}
	}
	for (;;){
		if ( !*s ){
			while ( *mask == '*' ) mask++;
			//printf("   2 [%02X] %d\n", (unsigned char)(*mask), !*mask);
			return !*mask;
		}
		if ( *mask == '*' ){
			if ( !*++mask )  break;
			mp = mask;
			cp = s + 1;
			continue;
		}
		if ( *mask == *s || *mask == '?' ){
			mask++,
			s++;
			continue;
		}
		mask = mp;
		s = cp++;
	}
	//printf("   3\n");
	return true;
}// match
*/



tFileAttr::tFileAttr(const string &_name, unsigned _attr,
                     size_t _size, unsigned _cre, unsigned _mod) :
	name(_name),
	attr(_attr & MASK),
	size(_size),
	cre(_cre),
	mod(_mod)
{
	int cur = tDateTime::now_t();
	if ( cre == 0 )  cre = cur;
	if ( mod == 0 )  mod = cur;
}// tFileAttr::tFileAttr


tFileAttr::tFileAttr(const tFileAttr &s) :
	name(s.name),
	attr(s.attr),
	size(s.size),
	cre(s.cre),
	mod(s.mod)
{
}// tFileAttr::tFileAttr


tFileAttr &tFileAttr::operator=(const tFileAttr &s)
{
	if ( this != &s ){
		name = s.name;
		attr = s.attr;
		size = s.size;
		cre = s.cre;
		mod = s.mod;
	}
	return *this;
}// tFileAttr::operator=


bool tFileAttr::operator==(const tFileAttr &s) const
{
	if ( this == &s )  return true;
	return name == s.name && attr == s.attr && size == s.size &&
	       cre == s.cre && mod == s.mod;
}// tFileAttr::operator==


#ifdef _WIN32
static unsigned ftime2unix(FILETIME &t)
{
	tDateTime dt = tDateTime::FileTimeToDateTime(&t);
	/*
	SYSTEMTIME st;
	if ( !FileTimeToSystemTime(&t, &st) )	 return 0;
	tTimeS ts;
	ts.year   = st.wYear;
	ts.month  = st.wMonth;
	ts.day    = st.wDay;
	ts.hour   = st.wHour;
	ts.minute = st.wMinute;
	ts.sec    = st.wSecond;
	debugLog("ts " + ToString((int)ts));
	return (unsigned)((int)ts);
	*/
	return dt.DateTimeToTime_t();
}// ftime2unix
#endif

static std::vector<tFileAttr> DirEntryList(const string &path)
{
	//printf("--- ");
	std::vector<tFileAttr> v;
	string Path;
	if ( path.empty() ){
		Path = GetCurPath();
	}else{
		//Path = path;
		Path = GetFullPath(path);
	}

#if defined(__GNUG__) || defined(__CYGWIN__)
	DIR *fd = opendir(Path.c_str());
	if ( fd ){
		for ( struct dirent *d = readdir(fd); d; d = readdir(fd) ) {
			string name = d->d_name;
			if ( !name.empty() && name[0] == '.' )  continue;
			string fullname = path + string(path_delimiter) + name;
			struct stat statbuf;
			if ( stat(fullname.c_str(), &statbuf) != 0 )  continue;
			//if ( name != "." && name != ".." ){
				v.push_back(tFileAttr(fullname,
				                      statbuf.st_mode, statbuf.st_size,
				                      statbuf.st_ctime,
				                      statbuf.st_mtime));
			//}
		}
		closedir(fd);
	}
#else
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile((Path + "\\*.*").c_str(), &wfd);
	if ( h != INVALID_HANDLE_VALUE ){
		for (;;){
			//printf("[%s] %08X\n", wfd.cFileName, wfd.dwFileAttributes);
			if ( wfd.nFileSizeHigh == 0 && wfd.cFileName[0] != '.' ){
				v.push_back(tFileAttr(ChangeFilePath(wfd.cFileName, Path),
				                      wfd.dwFileAttributes, wfd.nFileSizeLow,
				                      ftime2unix(wfd.ftCreationTime),
				                      ftime2unix(wfd.ftLastWriteTime)));
			}
			if ( !FindNextFile(h, &wfd) )  break;
		}
		FindClose(h);
	}
#endif
	//printf("%d ", v.size());
	return v;
}// DirEntryList





tFList::tFList(const string &_path, bool r) :
	_sort(SORT_NO)
{
	//string Path = GetFullPath(_path);
	std::vector<tFileAttr> v = DirEntryList(_path);
	for ( size_t i = 0; i < v.size(); i++ ){
		if ( v[i].attr & tFileAttr::D ){
			if ( r ){
				std::vector<tFileAttr> v2 = DirEntryList(v[i].name);
				for ( size_t j = 0; j < v2.size(); j++ ){
					v.push_back(v2[j]);
				}
			}
		}else{
			list.push_back(v[i].name);
		}
	}
	/*

	string Path = GetFullPath(_path);
#ifdef _WIN32
	//int attr = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN   |
	//           FILE_ATTRIBUTE_SYSTEM   | FILE_ATTRIBUTE_ARCHIVE;
	int attr = 1 | 2 | 4 | 0x20;
#endif
#ifdef __GNUG__
	int attr = 0777;
#endif
	list = DirFileList(Path, mask, attr);
	for ( size_t i = 0; i < list.size(); i++ ){
		list[i] = ChangeFilePath(list[i], Path);
	}
	*/
}// tFList::tFList


tFList &tFList::operator=(const tFList &s)
{
	if ( this != &s ){
		_sort = s._sort;
		list = s.list;
	}
	return *this;
}// tFList::operator=


bool tFList::operator==(const tFList &s) const
{
	if ( this == &s )  return true;
	if ( size() != s.size() )  return false;
	for ( size_t i = 0; i < s.size(); i++ ){
		if ( find(s[i]) == (size_t)-1 )  return false;
	}
	return true;
}// tFList::operator==


bool tFList::operator+=(const string &s)
{
	string S = GetFullPath(s);
	if ( find(S) != (size_t)-1 )  return false;
	list.push_back(S);
	return true;
}// tFList::operator+=


bool tFList::operator-=(const string &s)
{
	string S = GetFullPath(s);
	size_t i = find(S);
	if ( i == (size_t)-1 )  return false;
	list.erase(list.begin() + i);
	return true;
}// tFList::operator-=


tFList &tFList::operator+=(const tFList &s)
{
	for ( size_t i = 0; i < s.size(); i++ ){
		if ( find(s[i]) == (size_t)-1 )  list.push_back(s[i]);
	}
	return *this;
}// tFList::operator-=


tFList &tFList::operator-=(const tFList &s)
{
	for ( size_t i = 0; i < s.size(); i++ ){
		if ( find(s[i]) != (size_t)-1 )  operator-=(s[i]);
	}
	return *this;
}// tFList::operator-=


tFList &tFList::operator&=(const tFList &s)
{
	std::vector<string> l = list;
	list.clear();
	for ( size_t i = 0; i < l.size(); i++ ){
		if ( s.find(l[i]) == (size_t)-1 )  list.push_back(l[i]);
	}
	return *this;
}// tFList::operator-=


string tFList::operator[](size_t n) const
{
	if ( n >= list.size() )  return "";
	return list[n];
}// tFList::operator-=


string tFList::name(size_t n) const
{
	if ( n >= list.size() )  return "";
	return ExtractFileName(list[n]);
}// tFList::operator-=


size_t tFList::find(const string &fname, bool cs) const
{
	string Fname = GetFullPath(fname);
	if ( cs ){
		std::vector<string>::const_iterator i = std::find(list.begin(), list.end(), Fname);
		if ( i != list.end() )  return i - list.begin();
	}else{
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( CompareIC(list[i], Fname) == 0 )  return i;
		}
	}
	return (size_t)-1;
}// tFList::find


class __CompName {
public:
	bool operator()(const string &v1, const string &v2) const { return v1 < v2; }
};// class __CompName

class __CompNameIC {
public:
	bool operator()(const string &v1, const string &v2) const {
		return toUpper(v1) < toUpper(v2);
	};
};// class __CompNameIC


class __CompExt {
public:
	bool operator()(const string &v1, const string &v2) const {
		string e1 = ExtractFileExt(v1);
		string e2 = ExtractFileExt(v2);
		bool ret = (e1 < e2);
		if ( e1 == e2 )  ret = (v1 < v2);
		return ret;
	}
};// class __CompExt

class __CompExtIC {
public:
	bool operator()(const string &v1, const string &v2) const {
		string e1 = toUpper(ExtractFileExt(v1));
		string e2 = toUpper(ExtractFileExt(v2));
		bool ret = (e1 < e2);
		if ( e1 == e2 )  ret = toUpper(v1) < toUpper(v2);
		return ret;
	};
};// class __CompExtIC


bool tFList::sort(unsigned mode, bool cs)
{
	if ( _sort != mode && (mode > SORT_NO && mode <= SORT_MAXMODE) ){
		_sort = mode;
		switch ( mode ){
		case SORT_NAME:
			if ( cs ){
				std::stable_sort(list.begin(), list.end(), __CompName());
			}else{
				std::stable_sort(list.begin(), list.end(), __CompNameIC());
			}
			break;
		case SORT_EXT:
			if ( cs ){
				std::stable_sort(list.begin(), list.end(), __CompExt());
			}else{
				std::stable_sort(list.begin(), list.end(), __CompExtIC());
			}
			break;
		}
		return true;
	}
	return false;
}// tFList::sort


tFList tFList::selectPath(const string &path, bool cs) const
{
	tFList ret;
	if ( cs ){
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( path == ExtractFilePath(list[i]) ){
				ret.list.push_back(list[i]);
			}
		}
	}else{
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( CompareIC(path, ExtractFilePath(list[i])) == 0 ){
				ret.list.push_back(list[i]);
			}
		}
	}
	return ret;
}// tFList::selectPath


tFList tFList::selectMask(const string &mask, bool cs) const
{
	tFList ret;
	if ( cs ){
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( FileMatch(mask, ExtractFileNameExt(list[i])) ){
				ret.list.push_back(list[i]);
			}
		}
	}else{
		string Mask = toUpper(mask);
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( FileMatch(Mask, toUpper(ExtractFileNameExt(list[i]))) ){
				ret.list.push_back(list[i]);
			}
		}
	}
	return ret;
}// tFList::selectMask


tFList tFList::selectFirst(const string &mask, bool cs) const
{
	tFList ret;
	if ( cs ){
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( mask == ExtractFileDir(list[i]) ){
				ret.list.push_back(list[i]);
			}
		}
	}else{
		string Mask = toUpper(mask);
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( Mask == toUpper(ExtractFileDir(list[i])) ){
				ret.list.push_back(list[i]);
			}
		}
	}
	return ret;
}// tFList::selectFisrt


void tFList::dropPath(string path, bool cs)
{
	PathAddDelimiter(path);
	size_t psize = path.size();
	for ( size_t i = 0; i < list.size(); i++ ){
		if ( cs ){
			if ( list[i].substr(0, psize) == path ){
				list[i] = list[i].substr(psize);
			}
		}else{
			string Path = toUpper(path);
			if ( toUpper(list[i].substr(0, psize)) == Path ){
				list[i] = list[i].substr(psize);
			}
		}
	}
}// tFList::dropPath


/*
	djd?fhj???9*.???   -->   djd.fhj...9.+[.]...

	? -> .
	* -> .+
	. -> [.]
*/




tFIList::tFIList(const string &_path, bool r) :
	_sort(SORT_NO)
{
	std::vector<tFileAttr> v = DirEntryList(_path);
	for ( size_t i = 0; i < v.size(); i++ ){
		if ( v[i].attr & tFileAttr::D ){
			if ( r ){
				std::vector<tFileAttr> v2 = DirEntryList(v[i].name);
				for ( size_t j = 0; j < v2.size(); j++ ){
					v.push_back(v2[j]);
				}
			}
		}else{
			list.push_back(v[i]);
		}
	}
	/*

	string Path = GetFullPath(_path);
#ifdef _WIN32
	//int attr = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN   |
	//           FILE_ATTRIBUTE_SYSTEM   | FILE_ATTRIBUTE_ARCHIVE;
	int attr = 1 | 2 | 4 | 0x20;
#endif
#ifdef __GNUG__
	int attr = 0777;
#endif
	list = DirFileList(Path, mask, attr);
	for ( size_t i = 0; i < list.size(); i++ ){
		list[i] = ChangeFilePath(list[i], Path);
	}
	*/
}// tFIList::tFIList


tFIList &tFIList::operator=(const tFIList &s)
{
	if ( this != &s ){
		_sort = s._sort;
		list = s.list;
	}
	return *this;
}// tFIList::operator=


bool tFIList::operator+=(const tFileAttr &s)
{
	size_t i = find(s.name);
	if ( i != (size_t)-1 ){
		if ( list[i] != s )  list[i] = s;
		return false;
	}
	list.push_back(s);
	return true;
}// tFIList::operator+=


bool tFIList::operator-=(const tFileAttr &s)
{
	size_t i = find(s.name);
	if ( i == (size_t)-1 )  return false;
	list.erase(list.begin() + i);
	return true;
}// tFIList::operator-=


bool tFIList::operator==(const tFIList &s) const
{
	if ( this == &s )  return true;
	if ( size() != s.size() )  return false;
	for ( size_t i = 0; i < s.size(); i++ ){
		if ( find(s[i].name) == (size_t)-1 )  return false;
	}
	return true;
}// tFIList::operator==


tFIList &tFIList::operator+=(const tFIList &s)
{
	for ( size_t i = 0; i < s.size(); i++ ){
		if ( find(s[i].name) == (size_t)-1 )  list.push_back(s[i]);
	}
	return *this;
}// tFIList::operator-=


tFIList &tFIList::operator-=(const tFIList &s)
{
	for ( size_t i = 0; i < s.size(); i++ ){
		if ( find(s[i].name) != (size_t)-1 )  operator-=(s[i]);
	}
	return *this;
}// tFIList::operator-=


tFIList &tFIList::operator&=(const tFIList &s)
{
	std::vector<tFileAttr> l = list;
	list.clear();
	for ( size_t i = 0; i < l.size(); i++ ){
		if ( s.find(l[i].name) == (size_t)-1 )  list.push_back(l[i]);
	}
	return *this;
}// tFIList::operator-=


tFileAttr tFIList::operator[](size_t n) const
{
	if ( n >= list.size() )  return tFileAttr();
	return tFileAttr(list[n]);
}// tFIList::operator-=

/*
string tFIList::name(size_t n) const
{
	if ( n >= list.size() )  return "";
	return ExtractFileName(list[n]);
}// tFIList::operator-=
*/

size_t tFIList::find(const string &fname, bool cs) const
{
	string Fname = GetFullPath(fname);
	//string Fname = fname;
	if ( cs ){
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( list[i].name == Fname )  return i;
		}
	}else{
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( CompareIC(list[i].name, Fname) == 0 )  return i;
		}
	}
	return (size_t)-1;
}// tFIList::find


class __CompIName {
public:
	bool operator()(const tFileAttr &v1, const tFileAttr &v2) const {
		return v1.name < v2.name;
	}
};// class __CompIName

class __CompINameIC {
public:
	bool operator()(const tFileAttr &v1, const tFileAttr &v2) const {
		return toUpper(v1.name) < toUpper(v2.name);
	};
};// class __CompINameIC


class __CompIExt {
public:
	bool operator()(const tFileAttr &v1, const tFileAttr &v2) const {
		string e1 = ExtractFileExt(v1.name);
		string e2 = ExtractFileExt(v2.name);
		bool ret = (e1 < e2);
		if ( e1 == e2 )  ret = (v1.name < v2.name);
		return ret;
	}
};// class __CompIExt

class __CompIExtIC {
public:
	bool operator()(const tFileAttr &v1, const tFileAttr &v2) const {
		string e1 = toUpper(ExtractFileExt(v1.name));
		string e2 = toUpper(ExtractFileExt(v2.name));
		bool ret = (e1 < e2);
		if ( e1 == e2 )  ret = toUpper(v1.name) < toUpper(v2.name);
		return ret;
	};
};// class __CompIExtIC

class __CompSize {
public:
	bool operator()(const tFileAttr &v1, const tFileAttr &v2) const {
//		string e1 = ExtractFileExt(v1.name);
//		string e2 = ExtractFileExt(v2.name);
		size_t e1 = v1.size;
		size_t e2 = v2.size;
		bool ret = (e1 < e2);
		if ( e1 == e2 )  ret = (v1.name < v2.name);
		return ret;
	}
};// class __CompSize

class __CompDate {
public:
	bool operator()(const tFileAttr &v1, const tFileAttr &v2) const {
		unsigned e1 = v1.cre;
		unsigned e2 = v2.cre;
		bool ret = (e1 < e2);
		if ( e1 == e2 )  ret = (v1.name < v2.name);
		return ret;
	}
};// class __CompDate

class __CompDateM {
public:
	bool operator()(const tFileAttr &v1, const tFileAttr &v2) const {
		unsigned e1 = v1.mod;
		unsigned e2 = v2.mod;
		bool ret = (e1 < e2);
		if ( e1 == e2 )  ret = (v1.name < v2.name);
		return ret;
	}
};// class __CompDateM


bool tFIList::sort(unsigned mode, bool cs)
{
	if ( _sort != mode && (mode > SORT_NO && mode <= SORT_MAXMODE) ){
		_sort = mode;
		switch ( mode ){
		case SORT_NAME:
			if ( cs ){
				std::stable_sort(list.begin(), list.end(), __CompIName());
			}else{
				std::stable_sort(list.begin(), list.end(), __CompINameIC());
			}
			break;

		case SORT_EXT:
			if ( cs ){
				std::stable_sort(list.begin(), list.end(), __CompIExt());
			}else{
				std::stable_sort(list.begin(), list.end(), __CompIExtIC());
			}
			break;

		case SORT_SIZE:
			std::stable_sort(list.begin(), list.end(), __CompSize());
			break;

		case SORT_DATE:
			std::stable_sort(list.begin(), list.end(), __CompDate());
			break;

		case SORT_DATE_M:
			std::stable_sort(list.begin(), list.end(), __CompDateM());
			break;
		}
		return true;
	}
	return false;
}// tFIList::sort


tFIList tFIList::selectPath(const string &path, bool cs) const
{
	tFIList ret;
	if ( cs ){
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( path == ExtractFilePath(list[i].name) ){
				ret.list.push_back(list[i]);
			}
		}
	}else{
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( CompareIC(path, ExtractFilePath(list[i].name)) == 0 ){
				ret.list.push_back(list[i]);
			}
		}
	}
	return ret;
}// tFIList::selectPath


tFIList tFIList::selectMask(const string &mask, bool cs) const
{
	tFIList ret;
	if ( cs ){
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( FileMatch(mask, ExtractFileNameExt(list[i].name)) ){
				ret.list.push_back(list[i]);
			}
		}
	}else{
		string Mask = toUpper(mask);
		for ( size_t i = 0; i < list.size(); i++ ){
			if ( FileMatch(Mask, toUpper(ExtractFileNameExt(list[i].name))) ){
				ret.list.push_back(list[i]);
			}
		}
	}
	return ret;
}// tFIList::selectMask


tFIList tFIList::selectAttr(unsigned attr) const
{
	tFIList ret;
	for ( size_t i = 0; i < list.size(); i++ ){
		if ( (list[i].attr & ~attr) == 0 ){
			ret.list.push_back(list[i]);
		}
	}
	return ret;
}// tFIList::selectMask

