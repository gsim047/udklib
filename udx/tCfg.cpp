#include "tCfg.h"
//#include "tCfgKey.h"

#include "fString.h"
#include "tMutex.h"
#include "fVector.h"
#include "fFile.h"
#include "tXml.h"
#include "fTime.h"
#include "zlib.h"




static const char *n_string  = "string";
static const char *n_int     = "int";
static const char *n_float   = "float";
static const char *n_link    = "link";
static const char *n_bin     = "bin";
static const char *n_bool    = "bool";

static const char *n_type    = "type";
static const char *n_value   = "value";
static const char *n_min     = "min";
static const char *n_max     = "max";
static const char *n_info    = "info";
static const char *n_ro      = "ro";

static const char *n_name    = "name";

static const char *n_folder  = "folder";
static const char *n_key     = "key";



string tCfgKey::typeName(int type)
{
	switch ( type ){
	case T_Int:   return n_int;
	case T_Float: return n_float;
	case T_Link:  return n_link;
	case T_Bin:   return n_bin;
	case T_Bool:  return n_bool;
	}
	return n_string;
}// tCfgKey::typeName


int tCfgKey::typeNo(const string &Type)
{
	int no = 0;
	if ( Type == n_int )         no = T_Int;
	else if ( Type == n_float )  no = T_Float;
	else if ( Type == n_link )   no = T_Link;
	else if ( Type == n_bin )    no = T_Bin;
	else if ( Type == n_bool )   no = T_Bool;
	return no;
}// tCfgKey::typeNo


tCfgKey::tCfgKey(int Type, const std::map<string, string> &Par) :
	_type(Type), 
	par(Par),
	cnt(0)
{
	par[n_type] = typeName(Type); 
}// tCfgKey::tCfgKey


tCfgKey::tCfgKey(int Type, const string &val) :
	_type(Type),
	cnt(0)
{
	par[n_type] = typeName(Type); 
	par[n_value] = val; 
}// tCfgKey::tCfgKey


tCfgKey::tCfgKey(const std::map<string, string> &Par) :
	_type(0),
	par(Par),
	cnt(0)
{
	std::map<string, string>::const_iterator it = par.find(n_type);
	if ( it != par.end() ){
		_type = typeNo(it->second);
	}
}// tCfgKey::tCfgKey


bool tCfgKey::find(const string &Par) const
{
	return par.find(Par) != par.end();
}// tCfgKey::find


string tCfgKey::get(const string &Par) const
{
	string val;
	std::map<string, string>::const_iterator it = par.find(Par);
	if ( it != par.end() )  val = it->second;
	return val;
}// tCfgKey::get


string tCfgKey::get() const
{
	return get(n_value);
}// tCfgKey::get


string tCfgKey::getMin() const
{
	return get(n_min);
}// tCfgKey::getMin


string tCfgKey::getMax() const
{
	return get(n_max);
}// tCfgKey::getMax


//string tCfgKey::getDefault() const
//{
// return get(n_default);
//}// tCfgKey::getDefault


string tCfgKey::getInfo() const
{
	return get(n_info);
}// tCfgKey::getInfo


bool tCfgKey::getRO() const
{
	return get(n_ro) == "1";
}// tCfgKey::getRO


void tCfgKey::settype(int newtype)
{
	string s = typeName(newtype);
	_type = typeNo(s);
	set(n_type, s);
}// tCfgKey::settype


void tCfgKey::set(const string &Par, const string &Val)
{
	par[Par] = Val;
}// tCfgKey::set


void tCfgKey::set(const string &Val)
{
	set(n_value, Val);
}// tCfgKey::set


void tCfgKey::setMin(const string &Val)
{
	set(n_min, Val);
}// tCfgKey::setMin


void tCfgKey::setMax(const string &Val)
{
	set(n_max, Val);
}// tCfgKey::setMax


//void tCfgKey::setDefault(const string &Val)
//{
// set(n_default, Val);
//}// tCfgKey::setDefault


void tCfgKey::setInfo(const string &Val)
{
	set(n_info, Val);
}// tCfgKey::setInfo


void tCfgKey::setRO(bool Val)
{
	set(n_ro, Val ? "1" : "");
}// tCfgKey::setRO


bool tCfgKey::unlink()
{
	//tMutexLock l(m);
	cnt--;
	return cnt <= 0;
}// tCfgKey::unlink




class tCfgFolder {
public:
	tCfgFolder() : cnt(0) {}
	tCfgFolder(const string &fname) : cnt(0) { load(fname); }
	virtual ~tCfgFolder();

	tCfgFolder *link() { tMutexLock l(m); cnt++; return this; }
	bool unlink();

	bool empty() const { tMutexLock l(m); return sub.empty() && key.empty(); }
	bool clear();

	bool candel() const;
	bool canclear() const;

	string exp(const string &path) const;
	bool imp(const string &path, const string &data);

	bool load(const string &fname);
	bool save(const string &fname, bool bak) const;
	bool save(bool bak) const;

	bool operator==(const tCfgFolder &s) const;
	bool operator!=(const tCfgFolder &s) const { return !operator==(s); }

	std::vector<string> subList(const string &path) const;
	std::vector<string> keyList(const string &path) const;

	bool getFullPath(const tCfgKey *, string &path, string &name) const;
	bool getFullPath(const tCfgFolder *, string &path) const;

	string getName() const;
	void setName(const string &Name);

	string getInfo(const string &path) const;
	bool setInfo(const string &path, const string &info);

	bool findPath(const string &path) const;

	bool findKey(const string &path, const string &name) const;
	tCfgKey *getKey(const string &path, const string &name) const;

	const tCfgFolder *getPath(const string &path) const;
	tCfgFolder *getPath(const string &path);

	bool addKey(int type, const string &path, const string &name, const string &val);

	bool addKey(const string &path, const string &name, const string &val);
	bool addKey(const string &path, const string &name, int val);
	bool addKey(const string &path, const string &name, double val);
	bool addKey(const string &path, const string &name, const void *buf, unsigned size);
	bool addKey(const string &path, const string &name, bool val);

	bool renameKey(const string &path, const string &oname, const string &nname);
	bool renamePath(const string &path, const string &npath);

	bool addPath(const string &path);

	bool delKey(const string &path, const string &name);
	//bool delSub(const string &path, const string &name, bool r = false);
	bool delPath(const string &path, bool r = false);

	tCfgKey *makeKey(int type, const string &path, const string &name, const string &val);
	tCfgKey *makeKey(int type, const string &path, const string &name, const string &val,
	                 const string &Min, const string &Max);

private:
	tCfgFolder(const tCfgFolder &s);
	tCfgFolder &operator=(const tCfgFolder &s);

	std::vector<string> _subList() const;
	std::vector<string> _keyList() const;

	bool _canclear() const;

	string _getInfo(const string &path) const;
	bool _setInfo(const string &path, const string &info);

	bool _findSub(const string &name) const;
	bool _findPath(const string &path) const;
	bool _findKey(const string &name) const;

	tCfgKey *_getKey(const string &name) const;
	tCfgKey *_getKey(const string &path, const string &name) const;
	tCfgFolder *_getSub(const string &name) const;

	const tCfgFolder *_getPath(const string &path) const;
	tCfgFolder *_getPath(const string &path);
	bool _getFullPath(const tCfgKey *, string &path, string &name) const;
	bool _getFullPath(const tCfgFolder *, string &path) const;

	bool _addKey(int type, const string &name, const string &val);
	bool _addKey(int type, const string &path, const string &name, const string &val);

	tCfgKey *_makeKey(int type, const string &path, const string &name, const string &val);
	tCfgKey *_makeKey(int type, const string &path, const string &name, const string &val,
	                  const string &Min, const string &Max);

	bool _addSub(const string &name);
	bool _addPath(const string &path);

	bool _renameKey(const string &oname, const string &nname);
	bool _renameKey(const string &path, const string &oname, const string &nname);
	bool _renamePath(const string &path, const string &npath);

	bool _delKey(const string &name);

	bool _delSub(const string &name, bool r = false);
	bool _goodName(const string &name, bool parm = false) const;

	void mkList(tTag *xdir) const;
	void mkTree(tTag *tag);

	string _exp() const;
	bool _imp(const string &data);

public:
	std::map<string, tCfgFolder *> sub;
	std::map<string, tCfgKey *> key;
	mutable tMutex m;
	int cnt;
	mutable string fname;
	string info;
	string date;
	string flname;
};// class tCfgFolder




static const char *bad_chars = 
	"\x7f"
	"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0b\x0c\x0f"
	"\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1f";




tCfgFolder::~tCfgFolder()
{
	{
		//tMutexLock l(m);
		for ( std::map<string, tCfgKey *>::iterator it = key.begin();
		      it != key.end(); ++it ){
			delete it->second;
			it->second = NULL;
		}
		key.clear();
		for ( std::map<string, tCfgFolder *>::iterator fit = sub.begin();
		      fit != sub.end(); ++fit ){
			delete fit->second;
			fit->second = NULL;
		}
		sub.clear();
	}
}// tCfgFolder::~tCfgFolder


bool tCfgFolder::clear()
{
	tMutexLock l(m);
	if ( !_canclear() )  return false;
	for ( std::map<string, tCfgKey *>::iterator it = key.begin();
	      it != key.end(); ++it ){
		delete it->second;
		it->second = NULL;
	}
	key.clear();
	for ( std::map<string, tCfgFolder *>::iterator fit = sub.begin();
	      fit != sub.end(); ++fit ){
		delete fit->second;
		fit->second = NULL;
	}
	sub.clear();
	return true;
}// tCfgFolder::clear


bool tCfgFolder::unlink()
{
	tMutexLock l(m);
	cnt--;
	return cnt <= 0;
}// tCfgFolder::unlink


std::vector<string> tCfgFolder::_subList() const
{
	std::vector<string> ret;
	for ( std::map<string, tCfgFolder *>::const_iterator fit = sub.begin();
	      fit != sub.end(); ++fit ){
		ret.push_back(fit->first);
	}
	return ret;
}// tCfgFolder::_subList

/*
std::vector<string> tCfgFolder::subList() const
{
	tMutexLock l(m);
	return _subList();
}// tCfgFolder::subList
*/

std::vector<string> tCfgFolder::subList(const string &path) const
{
	tMutexLock l(m);
	std::vector<string> ret;
	const tCfgFolder *fl = _getPath(path);
	if ( fl )  ret = fl->_subList();
	return ret;
}// tCfgFolder::subList


std::vector<string> tCfgFolder::_keyList() const
{
	std::vector<string> ret;
	for ( std::map<string, tCfgKey *>::const_iterator it = key.begin();
	      it != key.end(); ++it ){
		ret.push_back(it->first);
	}
	return ret;
}// tCfgFolder::_keyList

/*
std::vector<string> tCfgFolder::keyList() const
{
	tMutexLock l(m);
	return _keyList();
}// tCfgFolder::keyList
*/

std::vector<string> tCfgFolder::keyList(const string &path) const
{
	tMutexLock l(m);
	std::vector<string> ret;
	const tCfgFolder *fl = _getPath(path);
	if ( fl )  ret = fl->_keyList();
	return ret;
}// tCfgFolder::keyList


bool tCfgFolder::_findSub(const string &name) const
{
	return sub.find(name) != sub.end();
}// tCfgFolder::_findSub

/*
bool tCfgFolder::findSub(const string &name) const
{
	tMutexLock l(m);
	return _findSub(name);
}// tCfgFolder::findSub
*/
/*
bool tCfgFolder::findSub(const string &path, const string &name) const
{
	tCfgFolder *fl = getPath(path);
	return fl ? fl->findSub() : false;
}// tCfgFolder::findSub
*/

bool tCfgFolder::findPath(const string &path) const
{
	tMutexLock l(m);
	return _getPath(path) != NULL;
}// tCfgFolder::findPath


bool tCfgFolder::_findKey(const string &name) const
{
	return key.find(name) != key.end();
}// tCfgFolder::_findKey


bool tCfgFolder::findKey(const string &path, const string &name) const
{
	tMutexLock l(m);
	const tCfgFolder *fl = _getPath(path);
	return fl ? fl->_findKey(name) : false;
}// tCfgFolder::findKey


tCfgKey *tCfgFolder::_getKey(const string &name) const
{
	tCfgKey *ret = NULL;
	std::map<string, tCfgKey *>::const_iterator it = key.find(name);
	if ( it != key.end() )  ret = it->second;
	return ret;
}// tCfgFolder::_getKey


tCfgKey *tCfgFolder::_getKey(const string &path, const string &name) const
{
	const tCfgFolder *fl = _getPath(path);
	return fl ? fl->_getKey(name) : NULL;
}// tCfgFolder::_getKey


tCfgKey *tCfgFolder::getKey(const string &path, const string &name) const
{
	tMutexLock l(m);
	return _getKey(path, name);
}// tCfgFolder::getKey


tCfgFolder *tCfgFolder::_getSub(const string &name) const
{
	tCfgFolder *ret = NULL;
	std::map<string, tCfgFolder *>::const_iterator it = sub.find(name);
	if ( it != sub.end() )  ret = it->second;
	return ret;
}// tCfgFolder::_getSub


bool tCfgFolder::_addKey(int type, const string &name, const string &val)
{
	if ( _findKey(name) || !_goodName(name, true) || !_goodName(val) )  return false;
	key[name] = new tCfgKey(type, val);
	return true;
}// tCfgFolder::_addKey


bool tCfgFolder::_addKey(int type, const string &path, const string &name, const string &val)
{
	//printf("_addKey 1\n");
	if ( !_addPath(path) )  return false;
	//printf("_addKey 2\n");
	tCfgFolder *fl = _getPath(path);
	return fl->_addKey(type, name, val);
}// tCfgFolder::_addKey


bool tCfgFolder::addKey(int type, const string &path, const string &name, const string &val)
{
	tMutexLock l(m);
	return _addKey(type, path, name, val);
}// tCfgFolder::addKey


bool tCfgFolder::addKey(const string &path, const string &name, const string &val)
{
	tMutexLock l(m);
	return _addKey(tCfgKey::T_String, path, name, val);
}// tCfgFolder::addKey


bool tCfgFolder::addKey(const string &path, const string &name, int val)
{
	tMutexLock l(m);
	return _addKey(tCfgKey::T_Int, path, name, ToString(val));
}// tCfgFolder::addKey


bool tCfgFolder::addKey(const string &path, const string &name, double val)
{
	tMutexLock l(m);
	return _addKey(tCfgKey::T_Float, path, name, ToStringD(val, -1));
}// tCfgFolder::addKey


bool tCfgFolder::addKey(const string &path, const string &name, const void *buf, unsigned size)
{
	tMutexLock l(m);
	return _addKey(tCfgKey::T_Bin, path, name, string((const char *)buf, size));
}// tCfgFolder::addKey


bool tCfgFolder::addKey(const string &path, const string &name, bool val)
{
	tMutexLock l(m);
	return _addKey(tCfgKey::T_Bool, path, name, val ? "true" : "false");
}// tCfgFolder::addKey


bool tCfgFolder::_addSub(const string &name)
{
	if ( _findSub(name) || !_goodName(name, true) )  return false;
	tCfgFolder *fl = new tCfgFolder();
	fl->flname = name;
	sub[name] = fl;
	//printf("_addSub [%s]\n", name.c_str());
	return true;
}// tCfgFolder::_addSub


bool tCfgFolder::_addPath(const string &Path)
{
	tCfgFolder *ret = this;
	std::vector<string> dir = SplitString(Path, "/");
	for ( unsigned i = 0; i < dir.size(); i++ ){
		if ( !dir.empty() ){
			ret->_addSub(dir[i]); 
			ret = ret->_getSub(dir[i]);
			if ( !ret )  return false;
		}
	}
	return true;
}// tCfgFolder::_addPath


bool tCfgFolder::addPath(const string &Path)
{
	tMutexLock l(m);
	return _addPath(Path);
}// tCfgFolder::addPath


bool tCfgFolder::_renameKey(const string &oname, const string &nname)
{
	if ( oname == nname )  return false;          // ???
	if ( !_goodName(nname, true) )  return false;
	tCfgKey *k = NULL;
	std::map<string, tCfgKey *>::iterator it = key.find(oname);
	if ( it == key.end() )  return false;
	k = it->second;
	key.erase(it);
	key[nname] = k;
	return true;
}// tCfgFolder::_renameKey


bool tCfgFolder::_renameKey(const string &path, const string &oname, const string &nname)
{
	tCfgFolder *fl = _getPath(path);
	return fl ? fl->_renameKey(oname, nname) : false;
}// tCfgFolder::_renameKey


bool tCfgFolder::renameKey(const string &path, const string &oname, const string &nname)
{
	tMutexLock l(m);
	return _renameKey(path, oname, nname);
}// tCfgFolder::renameKey

/*
bool tCfgFolder::_renamePath(const string &path, const string &nname)
{
	tCfgFolder *fl = _getPath(path);
	return fl->_renamePath(oname, nname);
}// tCfgFolder::_renamePath


bool tCfgFolder::renamePath(const string &path, const string &nname)
{
	tMutexLock l(m);
	return _renamePath(path, nname);
}// tCfgFolder::renamePath
*/

bool tCfgFolder::_delKey(const string &name)
{
	tCfgKey *k = _getKey(name);
	if ( !k )  return false;
	delete k;
	key.erase(name);
	return true;
}// tCfgFolder::_delKey


bool tCfgFolder::_delSub(const string &name, bool r)
{
	tCfgFolder *f = _getSub(name);
	if ( !f )  return false;
	if ( !r && !f->empty() )  return false;
	if ( !f->candel() )  return false;
	delete f;
	sub.erase(name);
	return true;
}// tCfgFolder::_delSub


const tCfgFolder *tCfgFolder::_getPath(const string &Path) const
{
	const tCfgFolder *ret = this;
	std::vector<string> dir = SplitString(Path, "/");
	for ( unsigned i = 0; i < dir.size(); i++ ){
		if ( !dir.empty() ){
			ret = ret->_getSub(dir[i]);
			if ( !ret )  return NULL;
		}
	}
	return ret;
}// tCfgFolder::_getPath


tCfgFolder *tCfgFolder::_getPath(const string &Path)
{
	tCfgFolder *ret = this;
	std::vector<string> dir = SplitString(Path, "/");
	//printf("_getPath : dir.size = %d [%s]\n", dir.size(), Path.c_str());
	for ( unsigned i = 0; i < dir.size(); i++ ){
		if ( !dir.empty() ){
			ret = ret->_getSub(dir[i]);
			if ( !ret )  return NULL;
		}
	}
	return ret;
}// tCfgFolder::_getPath


const tCfgFolder *tCfgFolder::getPath(const string &Path) const
{
	tMutexLock l(m);
	return _getPath(Path);
}// tCfgFolder::getPath


tCfgFolder *tCfgFolder::getPath(const string &Path)
{
	tMutexLock l(m);
	return _getPath(Path);
}// tCfgFolder::getPath


string tCfgFolder::_getInfo(const string &path) const
{
	string ret;
	const tCfgFolder *f = _getPath(path);
	if ( f )  ret = f->info;
	return ret;
}// tCfgFolder::_getInfo


bool tCfgFolder::_setInfo(const string &path, const string &Info)
{
	tCfgFolder *f = _getPath(path);
	if ( !f || !_goodName(Info) )  return false;
	f->info = Info;
	return true;
}// tCfgFolder::_setInfo


string tCfgFolder::getInfo(const string &path) const
{
	tMutexLock l(m);
	return _getInfo(path);
}// tCfgFolder::getInfo


bool tCfgFolder::setInfo(const string &path, const string &Info)
{
	tMutexLock l(m);
	return _setInfo(path, Info);
}// tCfgFolder::setInfo


bool tCfgFolder::delKey(const string &path, const string &name)
{
	tMutexLock l(m);
	tCfgFolder *f = _getPath(path);
	return f ? f->_delKey(name) : false;
}// tCfgFolder::delPath


bool tCfgFolder::delPath(const string &path, bool r)
{
	tMutexLock l(m);
	const tCfgFolder *cur = this;
	const tCfgFolder *last = this;
	string name;
	std::vector<string> dir = SplitString(path, "/");
	for ( unsigned i = 0; i < dir.size(); i++ ){
		if ( !dir.empty() ){
			last = cur;
			name = dir[i];
			cur = cur->_getSub(name);
			if ( !cur )  return false;
		}
	}
	return _delSub(name, r);
}// tCfgFolder::delPath


bool tCfgFolder::_getFullPath(const tCfgKey *Key, string &path, string &name) const
{
	for ( std::map<string, tCfgKey *>::const_iterator it = key.begin();
	      it != key.end(); ++it ){
		if ( it->second == Key ){
			path = "";
			name = it->first;
			return true;
		}
	}

	for ( std::map<string, tCfgFolder *>::const_iterator fit = sub.begin();
	      fit != sub.end(); ++fit ){
		string r;
		if ( fit->second->getFullPath(Key, r, name) ){
			path = r.empty() ? fit->first : (fit->first + "/" + r);
			return true;
		}
	}
	return false;
}// tCfgFolder::_getFullPath


bool tCfgFolder::getFullPath(const tCfgKey *Key, string &path, string &name) const
{
	if ( !Key )  return false;
	tMutexLock l(m);
	return _getFullPath(Key, path, name);
}// tCfgFolder::getFullPath


bool tCfgFolder::_getFullPath(const tCfgFolder *Fl, string &path) const
{
	for ( std::map<string, tCfgFolder *>::const_iterator it = sub.begin();
	      it != sub.end(); ++it ){
		if ( it->second == Fl ){
			path = it->first;
			return true;
		}
	}

	for ( std::map<string, tCfgFolder *>::const_iterator fit = sub.begin();
	      fit != sub.end(); ++fit ){
		string r;
		if ( fit->second->getFullPath(Fl, r) ){
			path = r.empty() ? fit->first : (fit->first + "/" + r);
			return true;
		}
	}
	return false;
}// tCfgFolder::_getFullPath


bool tCfgFolder::getFullPath(const tCfgFolder *Fl, string &path) const
{
	if ( !Fl )  return false;
	tMutexLock l(m);
	return _getFullPath(Fl, path);
}// tCfgFolder::getFullPath


bool tCfgFolder::candel() const
{
	tMutexLock l(m);
	if ( cnt )  return false;
	return _canclear();
}// tCfgFolder::candel


bool tCfgFolder::_canclear() const
{
	for ( std::map<string, tCfgKey *>::const_iterator it = key.begin();
	      it != key.end(); ++it ){
		if ( !it->second->candel() )  return false;
	}

	for ( std::map<string, tCfgFolder *>::const_iterator fit = sub.begin();
	      fit != sub.end(); ++fit ){
		if ( !fit->second->candel() )  return false;
	}
	return true;
}// tCfgFolder::_canclear


bool tCfgFolder::canclear() const
{
	tMutexLock l(m);
	return _canclear();
}// tCfgFolder::canclear


void tCfgFolder::mkTree(tTag *tag)
{
	if ( !tag )  return;

	for ( unsigned i = 0; i < tag->tagsize(); i++ ){
		tTag *stag = tag->tags[i];
		if ( stag->type != tTag::TAG )  continue;
		if ( stag->value == n_folder ){
			string name = (*stag)[n_name];
			_addPath(name);
			//printf("_addPath {\n");
			tCfgFolder *fl = _getPath(name);
			//printf("_addPath } [%s] : [%s]\n", name.c_str(), fl->getName().c_str());
			if ( fl )  fl->mkTree(stag);
			continue;
		}
		if ( stag->value == n_key ){
			string name = (*stag)[n_name];
			//string value = Replace(Replace((*stag)[n_value], "\\r", "\r"), "\\n", "\n");
			string value = (*stag)[n_value];
			string type = (*stag)[n_type];
			string info = Replace(Replace((*stag)[n_info], "\\r", "\r"), "\\n", "\n");
			string ro = (*stag)[n_ro];
			string Min = (*stag)[n_min];
			string Max = (*stag)[n_max];
			//printf("_addKey {\n");
			int Type = tCfgKey::typeNo(type);
			_addKey(Type, "", name, value);
			//printf("_addKey } [%s]\n", name.c_str());
			tCfgKey *key = _getKey(name);
			if ( key ){
				if ( !info.empty() )  key->setInfo(info);
				if ( !Min.empty() )  key->setMin(Min);
				if ( !Max.empty() )  key->setMax(Max);
				if ( ro == "1" )  key->setRO(true);
			}
			continue;
		}
	}
	_setInfo("", (*tag)[n_info]);
}// tCfgFolder::mkTree


bool tCfgFolder::_imp(const string &s)
{
	tXML xml(s);
	tTag *data = xml.tag("data");
	if ( !data )  return false;
	date = (*data)["created"];

	mkTree(data);

	return true;
}// tCfgFolder::_imp


bool tCfgFolder::imp(const string &path, const string &s)
{
	tMutexLock l(m);

	tCfgFolder *f = _getPath(path);
	if ( !f )  return false;
	return f->_imp(s);
}// tCfgFolder::imp


bool tCfgFolder::load(const string &Fname)
{
	fname = Fname;
	gzFile f = gzopen(Fname.c_str(), "rb");
	if ( f == NULL )  return false;

	string s;
	for (;;){
		char buf[0x8000];
		int n = gzread(f, buf, 0x8000);
		if ( n <= 0 )  break;
		s += string(buf, n);
	}
	gzclose(f);
	//if ( s.substr(0, 4) != string("xFG\0", 4) )  return false;

	return imp("", s);
}// tCfgFolder::load


void tCfgFolder::mkList(tTag *dir) const
{
	if ( !dir )  return;
	for ( std::map<string, tCfgFolder *>::const_iterator fit = sub.begin();
	      fit != sub.end(); ++fit ){
		tCfgFolder *f = fit->second;
		if ( f ){
			tTag *folder = new tTag(tTag::TAG, n_folder);
			folder->addParam(n_name, fit->first);
			string s = f->info;
			if ( !s.empty() ){
				folder->addParam(n_info, s);
			}
			f->mkList(folder);
			dir->addTag(folder);
		}
	}
	for ( std::map<string, tCfgKey *>::const_iterator it = key.begin();
	      it != key.end(); ++it ){
		tCfgKey *k = it->second;
		if ( k ){
			tTag *key = new tTag(tTag::TAG, n_key);
			key->addParam(n_name, it->first);
			//key->addParam(n_value, Replace(Replace(k->get(), "\r", "\\r"), "\n", "\\n"));
			key->addParam(n_value, k->get());
			key->addParam(n_type, k->typeName(k->type()));
			string s = k->getMin();
			if ( !s.empty() )  key->addParam(n_min, s);
			s = k->getMax();
			if ( !s.empty() )  key->addParam(n_max, s);
			s = k->getInfo();
			if ( !s.empty() )  key->addParam(n_info, Replace(Replace(s, "\r", "\\r"), "\n", "\\n"));
			bool b = k->getRO();
			if ( b )  key->addParam(n_ro, "1");
			dir->addTag(key);
		}
	}
}// tCfgFolder::mkList


string tCfgFolder::_exp() const
{
	tXML xml;
	tTag *data = new tTag(tTag::TAG, "data");
	xml.addTag(data);
	string dat = date;
	if ( dat.empty() )  dat = currentTimS();

	data->addParam("created", dat);
	data->addParam("modified", currentTimS());
	mkList(data);

	return xml.toString();
}// tCfgFolder::_exp


string tCfgFolder::exp(const string &path) const
{
	tMutexLock l(m);

	string ret;
	const tCfgFolder *f = _getPath(path);
	if ( f )  ret = f->_exp();
	return ret;
}// tCfgFolder::exp


bool tCfgFolder::save(const string &Fname, bool bak) const
{
	string s = exp("");
	if ( s.empty() )  return false;

	if ( bak )  FileCopy(Fname, ChangeFileExt(Fname, "bak"));

	gzFile f = gzopen(Fname.c_str(), "wb9");
	if ( f == NULL )  return false;
	fname = Fname;

	bool ret = //(gzwrite(f, "xFG", 4) == 4) &&
	           (gzwrite(f, (const voidp)s.c_str(), s.size()) == (int)s.size());
	gzclose(f);
	return ret;
}// tCfgFolder::save


bool tCfgFolder::save(bool bak) const
{
	return save(fname, bak);
}// tCfgFolder::save


bool tCfgFolder::_goodName(const string &name, bool parm) const
{
	return (!name.empty() || !parm) && name.find_first_of(bad_chars) == string::npos;
}// tCfgFolder::_goodName


string tCfgFolder::getName() const 
{
	tMutexLock l(m); 
	return flname; 
}// tCfgFolder::getName


void tCfgFolder::setName(const string &Name) 
{
	tMutexLock l(m); 
	flname = Name; 
}// tCfgFolder::setName


tCfgKey *tCfgFolder::_makeKey(int type, const string &path, const string &name, 
                              const string &val)
{
	tCfgKey *ret = _getKey(path, name);
	if ( !ret ){
		_addKey(type, path, name, val);
		ret = _getKey(path, name);
	}
	return ret;
}// tCfgFolder::_makeKey


tCfgKey *tCfgFolder::_makeKey(int type, const string &path, const string &name, 
                              const string &val,
                              const string &Min, const string &Max)
{
	tCfgKey *ret = _getKey(path, name);
	if ( !ret ){
		_addKey(type, path, name, val);
		ret = _getKey(path, name);
		if ( ret ){
			ret->setMin(Min);
			ret->setMin(Max);
		}
	}
	return ret;
}// tCfgFolder::_makeKey


tCfgKey *tCfgFolder::makeKey(int type, const string &path, const string &name, 
                             const string &val)
{
	tMutexLock l(m);
	return _makeKey(type, path, name, val);
}// tCfgFolder::makeKey


tCfgKey *tCfgFolder::makeKey(int type, const string &path, const string &name, 
                             const string &val,
                             const string &Min, const string &Max)
{
	tMutexLock l(m);
	return _makeKey(type, path, name, val, Min, Max);
}// tCfgFolder::makeKey




tCfg::tCfg() :
	f0(new tCfgFolder()),
	own(true)
{
	f0 = f0->link();
	f = f0;
}// tCfg::tCfg


tCfg::tCfg(const tCfg &s) :
	f0(s.f0 ? s.f0->link() : NULL),
	f(s.f),
	own(true)
{
}// tCfg::tCfg


tCfg::tCfg(tCfgFolder *fl) :
	f0(fl ? fl->link() : NULL),
	f(fl),
	own(false)
{
}// tCfg::tCfg


tCfg::tCfg(const tCfg &s, const string &path) :
	f0(s.f0 ? s.f0->link() : NULL),
	own(false)
{
	f = s.getPath(path);
	//f = ff ? ff->link() : NULL;
}// tCfg::tCfg


tCfg::tCfg(const string &fname) :
	f0(new tCfgFolder(fname)),
	own(true)
{
	f0 = f0->link();
	f = f0;
}// tCfg::tCfg


tCfg::~tCfg()
{
	if ( f0 ){
		if ( f0->unlink() && own )  delete f0;
	}
}// tCfg::~tCfg


bool tCfg::clear()
{
	return f ? f->clear() : false;
}// tCfg::clear


tCfg &tCfg::operator=(const tCfg &s)
{
	if ( f0 != s.f0 ){
		if ( f0 ){
			if ( f0->unlink() && own )  delete f0;
		}
		f0 = (s.f0) ? f0->link() : NULL;
		own = s.own;
	}
	if ( f != s.f ){
		f = s.f;
	}
	return *this;
}// tCfg::operator=


bool tCfg::empty() const
{
	return f ? f->empty() : true;
}// tCfg::empty


std::vector<string> tCfg::subList(const string &path) const
{
	std::vector<string> ret;
	if ( f )  ret = f->subList(path);
	return ret;
}// tCfg::subList


std::vector<string> tCfg::keyList(const string &path) const
{
	std::vector<string> ret;
	if ( f )  ret = f->keyList(path);
	return ret;
}// tCfg::keyList


bool tCfg::findPath(const string &path) const
{
	return f ? f->findPath(path) : false;
}// tCfg::findPath


bool tCfg::findKey(const string &path, const string &name) const
{
	return f ? f->findKey(path, name) : false;
}// tCfg::findKey


tCfgKey *tCfg::getKey(const string &path, const string &name) const
{
	return f ? f->getKey(path, name) : NULL;
}// tCfg::getKey


tCfgKey *tCfg::makeKey(int type, const string &path, const string &name, const string &val)
{
	if ( !f )  return NULL;
	tCfgKey *ret = f->makeKey(type, path, name, val);
	return ret ? ret->link() : NULL;
}// tCfg::getKey


tCfgKey *tCfg::makeKey(int type, const string &path, const string &name, const string &val,
                       const string &Min, const string &Max)
{
	if ( !f )  return NULL;
	tCfgKey *ret = f->makeKey(type, path, name, val, Min, Max);
	return ret ? ret->link() : NULL;
}// tCfg::getKey


tCfgFolder *tCfg::getPath(const string &path) const
{
	return f ? f->getPath(path) : NULL;
}// tCfg::getSub

/*
tCfgKey *tCfg::operator[](const string &fname) const // getKey() with full path
{
	if ( !f )  return NULL;
	size_t pos = fname.find_last_of("/");
	string path = (pos == string::npos) ? string("") : fname.substr(0, pos);
	string name = (pos == string::npos) ? fname : fname.substr(pos + 1);
	tCfgFolder *fl = getPath(path);
	return fl ? fl->getKey(name) : NULL;
}// tCfg::operator[]
*/

bool tCfg::addKey(const string &path, const string &name, const string &val)
{
	return f ? f->addKey(path, name, val) : false;
}// tCfg::addKey


bool tCfg::addKey(const string &path, const string &name, int val)
{
	return f ? f->addKey(path, name, val) : false;
}// tCfg::addKey


bool tCfg::addKey(const string &path, const string &name, double val)
{
	return f ? f->addKey(path, name, val) : false;
}// tCfg::addKey


bool tCfg::addKey(const string &path, const string &name, const void *buf, unsigned size)
{
	return f ? f->addKey(path, name, buf, size) : false;
}// tCfg::addKey

// /*
bool tCfg::addKey(const string &path, const string &name, bool val)
{
	return f ? f->addKey(path, name, val) : false;
}// tCfg::addKey
// */

bool tCfg::addPath(const string &path)
{
	return f ? f->addPath(path) : false;
}// tCfg::addPath


bool tCfg::delKey(const string &path, const string &name)
{
	return f ? f->delKey(path, name) : false;
}// tCfg::delKey


bool tCfg::delPath(const string &path, bool r)
{
	return f ? f->delPath(path, r) : false;
}// tCfg::delPath


bool tCfg::load(const string &fname)
{
	return (f0 && f0 == f) ? f0->load(fname) : false;
}// tCfg::load


bool tCfg::save(const string &fname, bool bak) const
{
	return f0 ? f0->save(fname, bak) : false;
}// tCfg::save


bool tCfg::save(bool bak) const
{
	return f0 ? f0->save(bak) : false;
}// tCfg::save


string tCfg::exp(const string &path) const
{
	return f ? f->exp(path) : string("");
}// tCfg::exp


bool tCfg::imp(const string &path, const string &data)
{
	return f ? f->imp(path, data) : false;
}// tCfg::imp


string tCfg::mkpath(const string &f1, const string &f2)
{
	if ( f1.empty() )  return f2;
	if ( f2.empty() )  return f1;
	return f1 + "/" + f2;
}// tCfg::mkpath


string tCfg::mkpath(const string &f1, const string &f2, const string &f3)
{
	string f0 = mkpath(f1, f2);
	if ( f0.empty() )  return f3;
	if ( f3.empty() )  return f0;
	return f0 + "/" + f3;
}// tCfg::mkpath


string tCfg::mkpath(const string &f1, const string &f2, const string &f3, const string &f4)
{
	string f0 = mkpath(f1, f2, f3);
	if ( f0.empty() )  return f4;
	if ( f4.empty() )  return f0;
	return f0 + "/" + f4;
}// tCfg::mkpath


string tCfg::getInfo(const string &path) const
{
	return f0 ? f0->getInfo(path) : string("");
}// tCfg::getInfo


bool tCfg::setInfo(const string &path, const string &info)
{
	return f0 ? f0->setInfo(path, info) : false;
}// tCfg::setInfo


int tCfg::getKeyType(const string &path, const string &name) const
{
	tCfgKey *key = NULL;
	if ( f )  key = f->getKey(path, name);
	return key ? key->type() : 0;
}// tCfg::getKeyType


bool tCfg::renameKey(const string &path, const string &oname, const string &nname)
{
	return f0 ? f0->renameKey(path, oname, nname) : false;
}// tCfg::renameKey


bool tCfg::renamePath(const string &path, const string &npath)
{
	//return f0 ? f0->renamePath(path, npath) : false;
	return false;
}// tCfg::renamePath


bool tCfg::getFullPath(const tCfgKey *Key, string &path, string &name) const
{
	return f0 ? f0->getFullPath(Key, path, name) : false;
}// tCfg::getFullPath


bool tCfg::getFullPath(const tCfgFolder *Fl, string &path) const
{
	return f0 ? f0->getFullPath(Fl, path) : false;
}// tCfg::getFullPath


string tCfg::getString(const string &path, const string &name, const string &def) const
{
	string ret = def;
	if ( f0 ){
		tCfgKey *key = getKey(path, name);
		if ( key )  ret = key->get();
	}
	return ret;
}// tCfg::getString


int tCfg::getInt(const string &path, const string &name, int def) const
{
	int ret = def;
	if ( f0 ){
		tCfgKey *key = getKey(path, name);
		if ( key )  ret = ToInt(key->get(), def);
	}
	return ret;
}// tCfg::getInt


int tCfg::getInt(const string &path, const string &name, int def, int Min, int Max) const
{
	int ret = def;
	if ( f0 ){
		tCfgKey *key = getKey(path, name);
		if ( key )  ret = ToInt(key->get(), def);
	}
	if ( def < Min )  def = Min;
	if ( def > Max )  def = Max;
	return ret;
}// tCfg::getInt


double tCfg::getFloat(const string &path, const string &name, double def) const
{
	double ret = def;
	if ( f0 ){
		tCfgKey *key = getKey(path, name);
		if ( key )  ret = ToDouble(key->get(), def);
	}
	return ret;
}// tCfg::getFloat


double tCfg::getFloat(const string &path, const string &name, double def, double Min, double Max) const
{
	double ret = def;
	if ( f0 ){
		tCfgKey *key = getKey(path, name);
		if ( key )  ret = ToDouble(key->get(), def);
	}
	if ( def < Min )  def = Min;
	if ( def > Max )  def = Max;
	return ret;
}// tCfg::getFloat


bool tCfg::getBool(const string &path, const string &name, bool def) const
{
	bool ret = def;
	if ( f0 ){
		tCfgKey *key = getKey(path, name);
		if ( key )  ret = (key->get() == "true");
	}
	return ret;
}// tCfg::getBool


bool tCfg::setString(const string &path, const string &name, const string &val)
{
	if ( !f0 )  return false;
	tCfgKey *key = getKey(path, name);
	if ( !key )  return false;
	key->set(val);
	return true;
}// tCfg::setString


bool tCfg::setInt(const string &path, const string &name, int val)
{
	if ( !f0 )  return false;
	tCfgKey *key = getKey(path, name);
	if ( !key )  return false;
	key->set(ToString(val));
	return true;
}// tCfg::setInt


bool tCfg::setFloat(const string &path, const string &name, double val)
{
	if ( !f0 )  return false;
	tCfgKey *key = getKey(path, name);
	if ( !key )  return false;
	key->set(ToStringD(val));
	return true;
}// tCfg::setFloat


bool tCfg::setBool(const string &path, const string &name, bool val)
{
	if ( !f0 )  return false;
	tCfgKey *key = getKey(path, name);
	if ( !key )  return false;
	key->set(val ? "true" : "false");
	return true;
}// tCfg::setBool



