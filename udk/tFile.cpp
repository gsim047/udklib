#include "tFile.h"
#include "fString.h"
#include "fVector.h"
#include "tTimer.h"

#include <sys/stat.h>
#include <string.h>
#include <algorithm>
#include <regex>

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _MSC_VER
#include <sys/utime.h>
#include <windows.h>
#include <direct.h>
#else
#include <utime.h>
#endif // _MSC_VER

#ifdef __GNUG__
#include <dirent.h>
#include <unistd.h>
#endif // __GNUG__



static const char *path_delimiters = "\\/";

#ifdef _WIN32
const char * const path_delimiter = "\\";
#endif // _WIN32
#ifdef __GNUG__
const char * const path_delimiter = "/";
#endif // __GNUG__



#ifdef _WIN32
#define FTIME_MODIFY     0
#define FTIME_CREATE     1
#define FTIME_ACCESS     2

static bool SetFileDate(const string &fname, int tim, unsigned n = 0)
{
	bool ret = false;

	HANDLE h = CreateFile(fname.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
	                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( h != INVALID_HANDLE_VALUE ){
		ulonglong hl = tim;
//		if ( tTimeS::isDST() )  hl -= 3600;
		hl += 11644473600;
		hl *= 10000000;
		FILETIME ft;
		*((ulonglong *)(&ft)) = hl;

		switch ( n ){
		case FTIME_CREATE:
			if ( SetFileTime(h, &ft, NULL, NULL) )  ret = true;
			break;

		case FTIME_ACCESS:
			if ( SetFileTime(h, NULL, &ft, NULL) )  ret = true;
			break;

		default:
			if ( SetFileTime(h, NULL, NULL, &ft) )  ret = true;
			break;
		}
		CloseHandle(h);
	}
	return ret;
}// SetFileDate
#endif // _WIN32


string ExtractFilePath(const string &path)
{
	string ret;
	size_t pos = path.find_last_of(path_delimiters);
	if ( pos != string::npos ){
		ret = path.substr(0, pos);
	}
	return ret;
}// ExtractFilePath


string ExtractFileNameExt(const string &path)
{
	string ret = path;
	size_t pos = ret.find_last_of(path_delimiters);
	if ( pos != string::npos ){
		ret = ret.substr(pos + 1);
	}
	return ret;
}// ExtractFileNameExt


void PathAddDelimiter(string &path, bool emptyToDot)
{
	if ( path.empty() ){
		if ( emptyToDot )  path = ".";
		path += path_delimiter;
	}else{
		if ( string(path_delimiter).find_first_of(path.substr(path.size() - 1)) == string::npos ){
			path += path_delimiter;
		}
	}
}// PathAddDelimiter


string ExtractFileName(const string &path)
{
	string ret = ExtractFileNameExt(path);
	size_t pos = ret.find_last_of(".");
	if ( pos != string::npos ){
		ret = ret.substr(0, pos);
	}
	return ret;
}// ExtractFileName


string ExtractFileExt(const string &path)
{
	string ret;
	size_t pos_p = path.find_last_of(".");
	if ( pos_p == string::npos )  return ret;

	size_t pos_d = path.find_last_of(path_delimiters);
	if ( pos_d == string::npos ){
		ret = path.substr(pos_p + 1);
	}else{
		if ( pos_p > pos_d ){
			ret = path.substr(pos_p + 1);
		}
	}
	return ret;
}// ExtractFileExt


string ExtractFileDir(const string &path)
{
	string ret;
	size_t pos = path.find_first_of(path_delimiters);
	if ( pos != string::npos ){
		ret = path.substr(0, pos);
	}
	return ret;
}// ExtractFileDir


string ChangeFilePath(string fname, const string &path)
{
	fname = ExtractFileNameExt(fname);
	string Path = path;
	size_t pos = Path.find_last_not_of(path_delimiters);
	if ( pos != string::npos ){
		pos++;
		if ( pos != Path.size() )  Path.resize(pos);
	}
	if ( !Path.empty() ){
		fname = Path + path_delimiter + fname;
	}
	return fname;
}// ChangeFilePath


string ChangeFileExt(string fname, string ext)
{
	string cur_ext = ExtractFileExt(fname);
	if ( cur_ext.empty() ){
		if ( fname.empty() || fname[fname.size() - 1] != '.' ){
			ext = "." + ext;
		}
	}else{
		fname = fname.substr(0, fname.size() - cur_ext.size());
	}
	fname += ext;
	return fname;
}// ChangeFileExt


static string shrinkDots(const string &path)
{
	std::vector<string> dirs = SplitString(path, path_delimiter);
	for (;;){
		std::vector<string>::iterator it = std::find(dirs.begin(), dirs.end(), "");
		if ( it == dirs.end() ) break;
		dirs.erase(it);
	}
	for (;;){
		std::vector<string>::iterator it = std::find(dirs.begin(), dirs.end(), ".");
		if ( it == dirs.end() ) break;
		dirs.erase(it);
	}
	for (;;){
		std::vector<string>::iterator it = std::find(dirs.begin(), dirs.end(), "..");
		if ( it == dirs.end() ) break;
		std::vector<string>::iterator it0 = it;
		dirs.erase(--it0);
		dirs.erase(it0);
	}
	string ret;
	for ( size_t i = 0; i < dirs.size(); i++ ){
		ret += path_delimiter + dirs[i];
	}
	return ret;
}// shrinkDots


string GetFullPath(const string &Fname)
{
	string fname = Fname;
	UnQuote(fname, "\"", "\"");
#ifdef _WIN32
	char *p;
	char buf[1024] = "";
	//DWORD code =
	GetFullPathName(fname.c_str(), 1023, buf, &p);
	string ret(buf);
	if ( ret.find_first_of("~") != string::npos ){
		unsigned len = GetLongPathName(buf, buf, 1023);
		ret = string(buf, len);
	}
	return ret;
#endif // _WIN32
#ifdef __GNUG__
	string path = fname;
	if ( !path.empty() ){
		if ( path[0] != path_delimiter[0] ){
			path = GetCurPath() + path_delimiter + path;
		}
		path = shrinkDots(path);
	}
	return path;
#endif
}// GetFullPath


string GetShortPath(const string &Fname)
{
	string fname = Fname;
	UnQuote(fname, "\"", "\"");
#ifdef _WIN32
	char *p;
	char buf[1024] = "";
	//DWORD code

	GetFullPathName(fname.c_str(), 1023, buf, &p);

	GetShortPathName(buf, buf, 1023);
	return string(buf);
#endif // _WIN32
#ifdef __GNUG__
 return GetFullPath(fname);
#endif
}// GetShortPath


string GetCurPath()
{
	char dir[261]; // MAXPATH + 1
#ifdef _WIN32
	_getcwd(dir, 260);
#endif
#ifdef __GNUG__
	getcwd(dir, 260);
#endif
	return string(dir);
}// GetCurPath






namespace nError {

	FILEError::FILEError() : Message("FILE error") 
	{
	}// FILEError::FILEError

	FILEError::FILEError(const string &info) : Message("FILE error: " + info) 
	{
	}// FILEError::FILEError

};// namespace nError


int tFILE::FileSize(const string &name)
{
	struct stat statbuf;
	if ( stat(name.c_str(), &statbuf) != 0 )  return -1;
	return statbuf.st_size;
}// FileSize


bool tFILE::FileExists(const string &FileName)
{
	struct stat statbuf;
	if ( stat(FileName.c_str(), &statbuf) != 0 )  return false;
	if ( statbuf.st_mode & S_IFDIR )  return false;
	return true;
}// tFILE::FileExists


bool tFILE::DirExists(const string &FileName)
{
	struct stat statbuf;
	if ( stat(FileName.c_str(), &statbuf) != 0 )  return false;
	if ( statbuf.st_mode & S_IFDIR )  return true;
	return false;
}// tFILE::DirExists


bool tFILE::FileDelete(const string &filename)
{
	//debugLog("try to delete file '" + filename + "'");
	return unlink(filename.c_str()) == 0;
}// tFILE::FileDelete


bool tFILE::FileRename(const string &oldname, const string &newname)
{
 if ( CompareIC(oldname, newname) == 0 )  return false;
	if ( FileExists(newname) )  return false;
	return rename(oldname.c_str(), newname.c_str()) == 0;
}// tFILE::FileRename


bool tFILE::FileMove(const string &oldname, const string &newname)
{
 if ( CompareIC(oldname, newname) == 0 )  return false;
	if ( FileExists(newname) )  FileDelete(newname);
	int ts = getFileModifyDate(oldname);
	bool ret = (rename(oldname.c_str(), newname.c_str()) == 0);
	if ( ret )  setFileModifyDate(newname, ts);
	return ret;
}// tFILE::FileMove


bool tFILE::FileBackup(const string &file, const string &ext)
{
	if ( !FileExists(file) )  return false;
	return FileCopy(file, ChangeFileExt(file, ext));
}// tFILE::FileBackup


bool tFILE::FileCopy(const string &sname, const string &dname)
{
	{
		tFILE s(sname, "rb");
		tFILE d(dname, "wb");
		if ( !s.copyTo(d) )  return false;
	}
	setFileModifyDate(dname, getFileModifyDate(sname));
	return true;
}// tFILE::FileCopy


bool tFILE::FileCopy(const string &sname, const string &dname, int len)
{
	{
		tFILE s(sname, "rb");
		tFILE d(dname, "wb");
		if ( len < 0 )  len += s.size();
		if ( !s.copyTo(d, len) )  return false;
	}
	setFileModifyDate(dname, getFileModifyDate(sname));
	return true;
}// tFILE::FileCopy


int tFILE::getFileDate(const string &name)
{
	struct stat statbuf;
	if ( stat(name.c_str(), &statbuf) != 0 )  return 315532800;
	return statbuf.st_ctime;
}// tFILE::getFileDate


int tFILE::getFileModifyDate(const string &name)
{
	struct stat statbuf;
	if ( stat(name.c_str(), &statbuf) != 0 )  return 315532800;
	return statbuf.st_mtime;
}// tFILE::getFileModifyDate


bool tFILE::setFileDate(const string &name, int ts)
{
#ifdef _WIN32
	return ::SetFileDate(name, ts, 1);
#endif // _WIN32
#ifdef __GNUG__
	return false;
#endif // __GNUG__
}// tFILE::setFileDate


bool tFILE::setFileModifyDate(const string &name, int ts)
{
#ifdef _WIN32
	return ::SetFileDate(name, ts, 0);
#endif // _WIN32
#ifdef __GNUG__
	int uptime = ts;
	if ( uptime == -1 )  return false;
	struct utimbuf ub;
	ub.actime = uptime;
	ub.modtime = uptime;
	return utime(name.c_str(), &ub) == 0;
#endif // __GNUG__
}// tFILE::setFileModifyDate



void tFILE::DirCreate(const string &path)
{
	if ( path.empty() )  return;

	std::vector<string> v = SplitString(path, ":" + string(path_delimiters));
	string disk;
	if ( path.find_first_of(":") != string::npos ){
		disk = v[0] + ":"; // + string(path_delimiter);
		v.erase(v.begin());
	}else if ( path.find_first_of(path_delimiter) == 0 ){
		disk = path_delimiter;
	}
	for ( size_t i = 0; i < v.size(); i++ ){
		disk += v[i];
		struct stat statbuf;
		int st_ret = stat(disk.c_str(), &statbuf);
		if ( st_ret == 0 && !(statbuf.st_mode & S_IFDIR) ){
			nError::Throw("DirCreate : Can't create dir '" + disk + "'.  Such file exists.");
		}
		if ( st_ret != 0 ){
#ifdef __GNUG__
			mkdir(disk.c_str(), 0777);
#endif // __GNUG__
#ifdef _WIN32
			mkdir(disk.c_str());
#endif // _WIN32
		}
		disk += path_delimiter;
	}
}// DirCreate

/*
bool DirDelete(const string &Path, bool R)
{
	std::vector<string> ff = DirFileList(Path, "*", 0xff);
	std::vector<string> fd = DirSubList(Path);
	if ( !R && (ff.size() + fd.size()) > 0 )  return false;

	size_t i;
	for ( i = 0; i < ff.size(); i++ ){
		string f = ChangeFilePath(ff[i], Path);
		//debugLog("kill2 file [" + f + "]");
		if ( !FileDelete(f) )  return false;
	}
	ff.clear();

	for ( i = 0; i < fd.size(); i++ ){
		string d = ChangeFilePath(fd[i], Path);
		//debugLog("kill2 dir [" + d + "]");
		if ( !DirDelete(d, true) )  return false;
	}
	return rmdir(Path.c_str()) == 0;
}// DirDelete
*/


bool tFILE::backup = false;


tFILE::tFILE(const string &fname, const string &mode) :
	_fname(fname),
	_mode(mode)
{
	for ( unsigned i = 0; i < 3; i++ ){
		f = fopen(_fname.c_str(), _mode.c_str());
		if ( f ) break;
		tTimer::delay(100);
	}

	if ( !f ){
		setResult(1, nError::FILEError("Can't open file '" + fname + "'"));
	}
}// tFILE::tFILE


tFILE::~tFILE()
{
	if ( f )  fclose(f);
}// tFILE::~tFILE


size_t tFILE::size() const
{
	cR();
	//if ( f == NULL )  return 0;
	size_t pos = tell();
	seek(0, SEEK_END);
	size_t ret = tell();
	seek(pos);
	return ret;
}// tFILE::size


string tFILE::read(size_t _size)
{
	cR();
	string ret;
	if ( !eof() ){
		char *buf = new char[_size];
		size_t n = fread(buf, 1, _size, f);
		ret = string(buf, n);
		delete [] buf;
	}
	return ret;
}// tFILE::read


bool tFILE::get(char &c)
{
	cR();
	if ( !eof() ){
		return fread(&c, 1, sizeof(c), f) == sizeof(c);
	}
	return false;
}// tFILE::get


bool tFILE::get(unsigned &c)
{
	cR();
	if ( !eof() ){
		return fread(&c, 1, sizeof(c), f) == sizeof(c);
	}
	return false;
}// tFILE::get


bool tFILE::get(double &c)
{
	cR();
	if ( !eof() ){
		return fread(&c, 1, sizeof(c), f) == sizeof(c);
	}
	return false;
}// tFILE::get


bool tFILE::get(void *buf, size_t len)
{
	cR();
	if ( !eof() ){
		return fread(buf, 1, len, f) == len;
	}
	return false;
}// tFILE::get


bool tFILE::write(const string &s)
{
	cR();
	//if ( f == NULL )  return false;
	return fwrite(s.c_str(), 1, s.size(), f) == s.size();
}// tFILE::write

/*
bool tFILE::write(char c)
{
	cR();
	return fwrite(&c, 1, 1, f) == 1;
}// tFILE::write


bool tFILE::write(unsigned c)
{
	cR();
	return fwrite(&c, 1, sizeof(c), f) == sizeof(c);
}// tFILE::write


bool tFILE::write(const void *buf, size_t len)
{
	cR();
	return fwrite(buf, 1, len, f) == len;
}// tFILE::write
*/

bool tFILE::put(const string &s)
{
	cR();
	//if ( f == NULL )  return false;
	return fwrite(s.c_str(), 1, s.size(), f) == s.size();
}// tFILE::put


bool tFILE::put(char c)
{
	cR();
	return fwrite(&c, 1, 1, f) == 1;
}// tFILE::put


bool tFILE::put(unsigned c)
{
	cR();
	return fwrite(&c, 1, sizeof(c), f) == sizeof(c);
}// tFILE::put


bool tFILE::put(double c)
{
	cR();
	return fwrite(&c, 1, sizeof(c), f) == sizeof(c);
}// tFILE::put


bool tFILE::put(const void *buf, size_t len)
{
	cR();
	return fwrite(buf, 1, len, f) == len;
}// tFILE::put


size_t tFILE::tell() const
{
	cR();
	//if ( f == NULL )  return 0;
	return (size_t) ftell(f);
}// tFILE::tell


bool tFILE::seek(size_t shift, int whence) const
{
	cR();
	//if ( f == NULL )  return false;
	return fseek(f, (long)shift, whence) == 0;
}// tFILE::seek


bool tFILE::trunc()
{
	cR();
	//if ( f == NULL )  return false;
	if ( _mode.find_first_of("w+") == string::npos )  return false;
	size_t pos = tell();
	seek(0);
	string cont = read(pos);
	fclose(f);
	f = fopen(_fname.c_str(), "wb");
	write(cont);
	fclose(f);
	f = fopen(_fname.c_str(), _mode.c_str());
	seek(pos);
	return true;
}// tFILE::trunc


bool tFILE::flush()
{
	cR();
	return fflush(f) == 0;
}// tFILE::flush


string tFILE::LoadFile(const string &fname)
{
	tFILE fi(fname, "rb");
	return fi.read(fi.size());
}// tFILE::LoadFile


string tFILE::LoadFileT(const string &fname)
{
	tFILE fi(fname, "rt");
	return fi.read(fi.size());
}// tFILE::LoadFileT


void tFILE::__back(const string &fname)
{
	if ( backup && FileExists(fname) ){
		if ( !FileBackup(fname) )  nError::Throw("Can't backup file [" + fname + "]");
	}
}// tFILE::__back


bool tFILE::SaveFile(const std::vector<string> &list, const string &fname)
{
	__back(fname);
	
	tFILE f(fname, "wb");
	if ( !f.isOpen() )  return false;

	for ( size_t i = 0; i < list.size(); i++ ){
		if ( !f.write(list[i] + "\n") )  return false;
	}
	return true;
}// tFILE::SaveFile


bool tFILE::SaveFileT(const std::vector<string> &list, const string &fname)
{
	__back(fname);
	
	tFILE f(fname, "wt");
	if ( !f.isOpen() )  return false;

	for ( size_t i = 0; i < list.size(); i++ ){
		if ( !f.write(list[i] + "\n") )  return false;
	}
	return true;
}// tFILE::SaveFileT


bool tFILE::SaveFile(const string &s, const string &fname)
{
	__back(fname);
	
	tFILE f(fname, "wb");
	if ( !f.isOpen() )  return false;

	return f.write(s);
}// tFILE::SaveFile


bool tFILE::SaveFileT(const string &s, const string &fname)
{
	__back(fname);
	
	tFILE f(fname, "wt");
	if ( !f.isOpen() )  return false;

	return f.write(s);
}// tFILE::SaveFileT


bool tFILE::copyTo(tFILE &to, size_t siz)
{
	for ( size_t pos = 0; pos < siz; ){
		size_t tor = 0x8000;
		if ( pos + tor >= siz )  tor = siz - pos;
		string s = read(tor);
		if ( s.size() != tor )  return false;
		pos += tor;
		if ( !to.write(s) )  return false;
	}
	return true;
}// tFILE::copyTo




size_t tRTFile::BufSize = 0x8000;


tRTFile::tRTFile(const string &filename) :
	tFILE(filename, "rb"),
	buf(new char[BufSize+1]),
	bind(0),
	bsize(0)
{
	buf[BufSize] = '\0';
	if ( isOpen() ){
		bsize = size();
		string b = this->tFILE::read(BufSize);
		bsize = b.size();
		memmove(buf, b.c_str(), bsize);
	}
}// tRTFile::tRTFile


//tRTFile &tRTFile::operator>>(string &s)
string tRTFile::read()
{
	string ret;
	cR();
	if ( !eof() ){
		char *p = (char *)memchr(buf + bind, '\n', bsize - bind);
		if ( p == nullptr ){
			//if ( bsize < BufSize ){
			if ( bind == 0 ){
				buf[bsize] = '\0';
				ret = buf;
				bind = bsize;
			}else{
				memmove(buf, buf + bind, bsize - bind);
				bsize -= bind;
				bind = 0;
				size_t size = fread(buf + bsize, 1, BufSize - bsize, f);
				if ( size == 0 ){
					buf[bsize] = '\0';
					ret = buf;
					bind = bsize;
				}else{
					bsize += size;
					ret = read();
				}
			}
		}else{
			*p = '\0';
			ret = string(buf + bind);
			//printf(">> s=[%s]\n", s.c_str());
			bind = p - buf + 1;
			if ( bind >= bsize ){
				bind = 0;
				bsize = fread(buf, 1, BufSize, f);
			}
			//if ( buf[bind] == '\r' ) bind++;
		}
		if ( !ret.empty() ){
			for (;;){
				size_t pos = ret.find_first_of("\r");
				if ( pos == string::npos ) break;
				size_t pos1 = ret.find_first_not_of("\r", pos + 1);
				if ( pos1 == string::npos ){
					ret.erase(pos);
				}else{
					ret.erase(pos, pos1 - pos);
				}
			}
		}
	}else{
		ret = "";
	}
	return ret;
}// tRTFile::operator>>


size_t tRTFile::tell() const
{
	return this->tFILE::tell() - bsize + bind;
}// tRTFile::tell


bool tRTFile::seek(size_t shift, int whense)
{
	bool ret = this->tFILE::seek(shift, whense);
	if ( ret ){
		bsize = size();
		string b = this->tFILE::read(BufSize);
		bsize = b.size();
		memmove(buf, b.c_str(), bsize);
	}
	return ret;
}// tRTFile::seek


bool tRTFile::LoadFile(std::vector<string> &list, const string &fname)
{
	tRTFile rf(fname);
	if ( !rf.isOpen() )  return false;

	while ( !rf.eof() ){
		list.push_back(rf.read());
	}
	return true;
}// tRTFile::LoadFile






std::vector<string> DirFileList(const string& path, const string& mask, unsigned attr)
{
	std::map<string, string> mx;
	mx["?"] = ".";
	mx["*"] = ".*";
	mx["."] = R"(\.)";
	xcodeTable xx = makeCodeTable(mx);
	string rmask = Replace(mask, xx);
	std::regex re(rmask, std::regex_constants::icase);

	std::vector<string> v;
#ifdef __GNUG__
	DIR *fd = opendir(path.c_str());
	if ( fd ){
		unsigned fattr = attr & 0x1C0;
		for ( struct dirent *d = readdir(fd); d; d = readdir(fd) ) {
			string name = d->d_name;
			string fullname = path + string(path_delimiter) + name;
			struct stat statbuf;
			if ( stat(fullname.c_str(), &statbuf) != 0 ) continue;
//				printf(": stat:%X/%X ", statbuf.st_mode, S_IFDIR);
				if ( !(statbuf.st_mode & S_IFDIR) ){
					if ( std::regex_match (name, re) ) v.push_back(name);
				} 
		}
		closedir(fd);
	}
#endif // __GNUG__
#ifdef _WIN32
	static const unsigned __FA_MASK = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN |
	                         FILE_ATTRIBUTE_SYSTEM   | FILE_ATTRIBUTE_ARCHIVE;
	string Path;
	if ( path.empty() ){
		//Path = ".\\" + mask;
		Path = mask;
	}else{
		Path = path + "\\" + mask;
	}
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(Path.c_str(), &wfd);
	if ( h != INVALID_HANDLE_VALUE ){
		attr &= __FA_MASK;
		for (;;){
			//printf("[%s] %08X\n", wfd.cFileName, wfd.dwFileAttributes);
			if ( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 ){
				wfd.dwFileAttributes &= __FA_MASK;
				if ( wfd.dwFileAttributes == 0 ||
				     (wfd.dwFileAttributes & ~attr) == 0 ){
					if ( std::regex_match (wfd.cFileName, re) ) v.push_back(wfd.cFileName);
				}
			}
			if ( !FindNextFile(h, &wfd) ) break;
		}
		FindClose(h);
	}
#endif // _WIN32
	return v;
}// DirFileList


std::vector<string> DirSubList(const string &path)
{
	std::vector<string> v;
#ifdef __GNUG__
	DIR *fd = opendir(path.c_str());
	if ( fd ){
		for ( struct dirent *d = readdir(fd); d; d = readdir(fd) ) {
			string name = d->d_name;
			if ( name == "." || name == ".." ) continue;
			string fullname = path + string(path_delimiter) + name;
			struct stat statbuf;
			if ( stat(fullname.c_str(), &statbuf) != 0 ) continue;
			if ( statbuf.st_mode & S_IFDIR )  v.push_back(name);
		}
		closedir(fd);
	}
#endif // __GNUG__
#ifdef _WIN32
	string Path = path + "\\*";
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(Path.c_str(), &wfd);
	if ( h != INVALID_HANDLE_VALUE ){
		for (;;){
			if ( (wfd.dwFileAttributes & 0x10) != 0 ){
				string name = wfd.cFileName;
				if ( !name.empty() && name[0] != '.' ){
					v.push_back(name);
				}
			}
			if ( !FindNextFile(h, &wfd) ) break;
		}
		FindClose(h);
	}
#endif // _WIN32
	return v;
}// DirSubList
