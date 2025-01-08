#ifndef __tShMem_h_
#define __tShMem_h_ 1

#pragma warning(disable:4786)

#include <string>
using std::string;

#ifdef _WIN32
#ifndef _WINDOWS_
#include <windows.h>
#endif
#endif




class tShMem
{
public:
	tShMem(unsigned keynumber, unsigned Size, bool Server = false);
	virtual ~tShMem();

#ifdef __GNUG__
	bool Alive() const { return id != -1 && base != NULL; }
#endif
#ifdef _WIN32
	bool Alive() const { return m_hFileMap != NULL && dwBaseMMF != 0; }
#endif

	bool get(void *buf, unsigned offset, unsigned len);
	bool put(unsigned offset, const void *buf, unsigned len);
	void *getbase() const;
	unsigned getsize() const { return size; }

protected:
	bool server;
	string name;
	unsigned size;
	unsigned pid;
#ifdef __GNUG__
	int id;
#ifdef __CYGWIN__
	char *base;
#else
	void *base;
#endif // __CYGWIN__
#endif // __GNUG__
#ifdef _WIN32
	HANDLE m_hFileMap;
	DWORD dwBaseMMF;
#endif // _WIN32
};// class tShMem




#endif // _tShMem_h_
