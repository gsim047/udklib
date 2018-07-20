#ifndef __tHKey_h
#define __tHKey_h 1

#pragma warning(disable:4786)

#include <windows.h>

#include <vector>
#include <string>
using std::string;




class tHKey {
public:
	enum{
		CLASSES_ROOT    = 0,
		CURRENT_USER,
		LOCAL_MACHINE,
		USERS
	};

public:
	tHKey(int sysKey);
	tHKey(int sysKey, const string &path);
	tHKey(const tHKey &sKey, const string &path);
	//tHKey(const tHKey &s);
	virtual ~tHKey();

	bool valid() const { return key != NULL; }
	//tHKey &operator=(const tHKey &s);

	std::vector<string> keyList() const;
	std::vector<string> valList() const;

	string path() const;
/*
	Type :
	REG_BINARY               Binary data in any form.
	REG_DWORD                A 32-bit number.
	REG_DWORD_LITTLE_ENDIAN  A 32-bit number in little-endian format (same as REG_DWORD). In 
		little-endian format, the most significant byte of a word is the high-order byte. This is the 
		most common format for computers running Windows NT and Windows 95.
	REG_DWORD_BIG_ENDIAN     A 32-bit number in big-endian format. In big-endian format, the most 
		significant byte of a word is the low-order byte.
	REG_EXPAND_SZ            A null-terminated string that contains unexpanded references to 
		environment variables (for example, "%PATH%"). It will be a Unicode or ANSI string depending 
		on whether you use the Unicode or ANSI functions.
	REG_LINK                 A Unicode symbolic link.
	REG_MULTI_SZ             An array of null-terminated strings, terminated by two null characters.
	REG_NONE                 No defined value type.
	REG_RESOURCE_LIST        A device-driver resource list.
	REG_SZ                   A null-terminated string. It will be a Unicode or ANSI string depending on 
*/
	unsigned getValType(const string &name) const;
	string getVal(const string &name) const;

	string getValStr(const string &name) const;
	unsigned getValDWORD(const string &name) const;
	std::vector<string> getValMulty(const string &name) const;

private:
	string getVal(const string &name, unsigned &type) const;
	//tHKey(const tHKey &s);
	//tHKey &operator=(const tHKey &s);

public:
private:
	HKEY key;
	bool sys;
};// class tHKey




#endif // __tHKey_h
