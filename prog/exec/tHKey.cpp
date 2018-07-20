#include "tHKey.h"

/*
  CLASSES_ROOT    = 0,
  CURRENT_USER,
  LOCAL_MACHINE,
  USERS
	}; */

tHKey::tHKey(int sysKey) :
	key(NULL),
	sys(true)
{
	switch ( sysKey ){
	case CLASSES_ROOT :   key = HKEY_CLASSES_ROOT;  break;
	case CURRENT_USER :   key = HKEY_CURRENT_USER;  break;
	case LOCAL_MACHINE :  key = HKEY_LOCAL_MACHINE; break;
	case USERS :          key = HKEY_USERS;         break;
	}
}// tHKey::tHKey


tHKey::tHKey(int sysKey, const string &path) :
	key(NULL),
	sys(false)
{
	HKEY sKey = NULL;
	switch ( sysKey ){
	case CLASSES_ROOT :   sKey = HKEY_CLASSES_ROOT;  break;
	case CURRENT_USER :   sKey = HKEY_CURRENT_USER;  break;
	case LOCAL_MACHINE :  sKey = HKEY_LOCAL_MACHINE; break;
	case USERS :          sKey = HKEY_USERS;         break;
	}
	if ( sKey ){
		LONG ret = RegOpenKeyEx(sKey,
		                        path.c_str(),
		                        0,
		                        KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE, //KEY_READ,
		                        &key);
		if ( ret != ERROR_SUCCESS ){
			key = NULL;
		}
	}
}// tHKey::tHKey


tHKey::tHKey(const tHKey &sKey, const string &path) :
	key(NULL),
	sys(false)
{
	if ( sKey.key ){
		LONG ret = RegOpenKeyEx(sKey.key,
		                        path.c_str(),
		                        0,
		                        KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE, //KEY_READ,
		                        &key);
		if ( ret != ERROR_SUCCESS ){
			key = NULL;
		}
	}
}// tHKey::tHKey


tHKey::~tHKey()
{
	if ( key && !sys ){
		RegCloseKey(key);
	}
}// tHKey::~tHKey


std::vector<string> tHKey::keyList() const
{
	std::vector<string> ret;
	if ( key ){
		for ( DWORD i = 0;; i++ ){
			char buf[2049];
			DWORD bufsize = 2048;
			FILETIME ft;
			LONG res = RegEnumKeyEx(key, i, buf, &bufsize, NULL, NULL, NULL, &ft);
			if ( res == ERROR_NO_MORE_ITEMS )  break;
			ret.push_back(buf);
		}
	}
	return ret;
}// tHKey::keyList


std::vector<string> tHKey::valList() const
{
	std::vector<string> ret;
	if ( key ){
		for ( DWORD i = 0;; i++ ){
			char name[2049];
			DWORD namesize = 2048;
			//char value[2049];
			//DWORD valsize = 2048;
			DWORD type;
			LONG res = RegEnumValue(key, i, name, &namesize, NULL, &type, NULL, NULL);
			if ( res == ERROR_NO_MORE_ITEMS )  break;
			ret.push_back(name);
		}
	}
	return ret;
}// tHKey::valList


unsigned tHKey::getValType(const string &name) const
{
	DWORD type;
	LONG ret = RegQueryValueEx(key, name.c_str(), NULL, &type, NULL, NULL);
	if ( ret != ERROR_SUCCESS )  type = 0;
	return type;
}// tHKey::getValType


string tHKey::getVal(const string &name, unsigned &Type) const
{
	char val[10240];
	DWORD valsize = 10240;
	DWORD type;
	LONG ret = RegQueryValueEx(key, name.c_str(), NULL, &type, (unsigned char *)val, &valsize);
	if ( ret != ERROR_SUCCESS )  return "";
	Type = type;
	return string(val, valsize);
}// tHKey::getVal


string tHKey::getVal(const string &name) const
{
	unsigned type;
	return getVal(name, type);
}// tHKey::getVal


string tHKey::getValStr(const string &name) const
{
	unsigned type;
	string s = getVal(name, type);
	if ( type != REG_SZ )  return "";
	size_t pos = s.find_first_of(string(1, '\0'));
	if ( pos != string::npos ){
		s = s.substr(0, pos);
	}
	return s;
}// tHKey::getValStr


unsigned tHKey::getValDWORD(const string &name) const
{
	//unsigned ret = (unsigned)-1;
	unsigned type;
	string s = getVal(name, type);
	if ( type == REG_DWORD )  return *((unsigned *)s.c_str());
	if ( type == REG_DWORD_LITTLE_ENDIAN )  return *((unsigned *)s.c_str());
	return (unsigned)-1;
}// tHKey::getValDWORD


std::vector<string> tHKey::getValMulty(const string &name) const
{
	std::vector<string> ret;
	unsigned type;
	string s = getVal(name, type);
	if ( type == REG_MULTI_SZ ){
		for ( const char *p = s.c_str(); *p; p += strlen(p) + 1 ){
			ret.push_back(p);
		}
	}
	return ret;
}// tHKey::getValMulty

/*
	HKEY key;
	bool sys;
*/


string tHKey::path() const
{
 /*
	char buf[1025];
	DWORD sz = 1025;
	long res = RegQueryInfoKey(key, buf, &sz, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	if ( res == ERROR_SUCCESS )  return string(buf, sz);
 */
	return "";
}// tHKey::path
