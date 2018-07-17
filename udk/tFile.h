#pragma once
#ifndef __tFile_h
#define __tFile_h 1

#include "nError.h"
#include "tResult.h"

//#if !defined(_SYS_STAT_H) && !defined(__STAT_H) && !defined(INC_STAT)
#include <sys/stat.h>
//#endif

#include <stdio.h>
#include <vector>



extern const char * const path_delimiter;

string ExtractFilePath(const string &path);
string ExtractFileName(const string &path);
string ExtractFileNameExt(const string &path);
string ExtractFileExt(const string &path);
string ExtractFileDir(const string &path);

string ChangeFilePath(string fname, const string &path);
string ChangeFileExt(string fname, string ext);

string GetCurPath();
string GetFullPath(const string &fname);
void PathAddDelimiter(string &path, bool emptyToDot = false);


#ifdef __GNUG__
#define fFile_NORMAL_ATTRIB (0700) //(S_IREAD | S_IWRITE | S_IEXEC)
#endif

#ifdef _WIN32
#define fFile_NORMAL_ATTRIB (0x20)
#endif

std::vector<string> DirFileList(const string &path,
                                const string &mask = "*.*",
                                unsigned attr = fFile_NORMAL_ATTRIB);

std::vector<string> DirSubList(const string &path);



namespace nError {
	class FILEError : public Message{
	public:
		FILEError(); // : Message("FILE error") {};
		FILEError(const string &info); // : Message("FILE error: " + info) {};
	};// class FILEError
};// namespace nError




class tFILE : protected tResult {
public:
	tFILE(const string &fname, const string &mode);
	virtual ~tFILE();

private:
	tFILE(const tFILE &);
	tFILE &operator=(const tFILE &);

public:
	static bool FileExists(const string &FileName);
	static bool DirExists(const string &FileName);

	static int FileSize(const string &filename);

	static bool FileDelete(const string &filename);
	static bool FileRename(const string &oldname, const string &newname);
	static bool FileMove(const string &oldname, const string &newname);
	static bool FileBackup(const string &file, const string &ext = "bak");

	static bool FileCopy(const string &sname, const string &dname);
	static bool FileCopy(const string &sname, const string &dname, int len);

	static int getFileDate(const string &fname);
	static int getFileModifyDate(const string &fname);
	static bool setFileDate(const string &fname, int ts);
	static bool setFileModifyDate(const string &fname, int ts);

	static void DirCreate(const string &path);
//	static bool DirDelete(const string &path, bool r);

//	static string FileCreateDate(const string &fname);
//	static string FileModifyDate(const string &fname);
//	static bool FileSetCreateDate(const string &fname, const string &ts);
//	static bool FileSetModifyDate(const string &fname, const string &ts);


public:
	operator FILE*() const { return f; };
	FILE *operator->() const { return f; };

	bool isOpen() const { return f != NULL; };
	virtual bool eof() const { return f == NULL || feof(f); };
	size_t size() const;
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

	bool copyTo(tFILE &to, size_t size);  // -->
	bool copyTo(tFILE &to) { return copyTo(to, size() - tell()); }

	size_t tell() const;
	bool seek(size_t shift, int whense = SEEK_SET) const;
	bool trunc();
	bool flush();

	static string LoadFile(const string &fname);
	static string LoadFileT(const string &fname);

	// std::vector<string> v;
	// if ( !tFILE::SaveFile(v, fname) ){


	static bool SaveFile(const std::vector<string> &list, const string &fname);
	static bool SaveFileT(const std::vector<string> &list, const string &fname);

	static bool SaveFile(const string &s, const string &fname);
	static bool SaveFileT(const string &s, const string &fname);

	static bool backup;  // true, used by SaveFile*

private:
	static void __back(const string &fname);

protected:
	FILE *f;
	string _fname;
	string _mode;
};// class tFILE




class tRTFile : protected tFILE {
public:
	static size_t BufSize; //   = 0x8000

public:
	tRTFile(const string &filename);
	virtual ~tRTFile() { delete [] buf; };

private:
	tRTFile(const tRTFile &);
	tRTFile &operator=(const tRTFile &);

public:
	virtual bool eof() const { return f == NULL || (feof(f) && bind == bsize); };
	bool isOpen() const { return f != NULL; };
	size_t size() const { return tFILE::size(); }
	string name() const { return tFILE::name(); }

	//tRTFile &operator>>(string &);
	string read();

	size_t tell() const;// { return tFILE::tell(); }
	bool seek(size_t shift, int whense = SEEK_SET);
	// std::vector<string> v;
	// if ( !tRTFile::LoadFile(v, fname) ){

	static bool LoadFile(std::vector<string> &list, const string &fname);

protected:
	char *buf;
	size_t bind;
	size_t bsize;
};// class tRTFile




#endif // __tFile_h
