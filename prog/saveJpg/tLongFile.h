#ifndef __tLongFile_h
#define __tLongFile_h 1

#ifndef __nError_h
#include "nError.h"
#endif

#ifndef __tFile_h
#include "tFile.h"
#endif

#ifndef __fLong_h
#include "fLong.h"
#endif

#include <stdio.h>
#include <vector>

#ifndef _WINDOWS_
#define NOGDI
#include <windows.h>
#endif






class tLongFile : protected tResult {
public:
	tLongFile(const string &fname, const string &mode);
	virtual ~tLongFile();

private:
	tLongFile(const tLongFile &);
	tLongFile &operator=(const tLongFile &);

public:
	operator HANDLE() const { return f; };
	HANDLE operator->() const { return f; };

	bool isOpen() const { return f != INVALID_HANDLE_VALUE; };
	virtual bool eof() const { return !isOpen() || (tell() >= size()); };
	longlong size() const;
	string name() const { return _fname; }

	string read(size_t size = 0x4000);
	bool write(const string &s);

	//bool write(char c);
	//bool write(unsigned c);
	//bool write(const void *buf, size_t len);

	bool get(char &c);
	bool get(unsigned &c);
	bool get(double &c);
	bool get(void *buf, size_t len);

	bool put(const string &s);
	bool put(char c);
	bool put(unsigned c);
	bool put(double c);
	bool put(const void *buf, size_t len);

	bool copyTo(tLongFile &to, longlong size);  // -->
	bool copyTo(tLongFile &to) { return copyTo(to, size() - tell()); }

	bool copyTo(tFILE &to, size_t size);  // -->

	longlong tell() const;
	bool seek(longlong shift, int whense = SEEK_SET) const;
	bool trunc();
	bool flush();

//	static string LoadFile(const string &fname);
//	static string LoadFileT(const string &fname);

	// std::vector<string> v;
	// if ( !tLongFile::SaveFile(v, fname) ){

	static bool SaveFile(const std::vector<string> &list, const string &fname);
	static bool SaveFileT(const std::vector<string> &list, const string &fname);

	static bool SaveFile(const string &s, const string &fname);
	static bool SaveFileT(const string &s, const string &fname);

	static bool backup;  // true, used by SaveFile*

private:
	static void __back(const string &fname);

protected:
	HANDLE f;
	string _fname;
	string _mode;
};// class tLongFile





#endif // __tLongFile_h
