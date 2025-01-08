#ifndef __tCfg_h
#define __tCfg_h 1

#pragma warning(disable:4786)

#include <string>
#include <map>
#include <vector>
using std::string;




class tCfgKey {
public:
	enum{
		T_String    = 0,
		T_Int,
		T_Float,
		T_Link,
		T_Bin,
		T_Bool
	};

public:
	tCfgKey() : _type(0), cnt(0) {}
	tCfgKey(int Type, const std::map<string, string> &Par);
	tCfgKey(const std::map<string, string> &Par);
	tCfgKey(int Type, const string &Val);
	tCfgKey(const tCfgKey &s) : _type(s._type), par(s.par) {};
	virtual ~tCfgKey() {}

	tCfgKey &operator=(const tCfgKey &s){
		if ( &s != this ){  _type = s._type;  par = s.par; }
		return *this;
	}

	tCfgKey *link() { cnt++; return this; }
	bool unlink();

	bool candel() const { return !cnt; }

	//bool operator==(const tCfgKey &s) const;
	//bool operator!=(const tCfgKey &s) const { return !operator==(s); }

	int type() const { return _type; }
	static string typeName(int type);
	static int typeNo(const string &Type);

	bool find(const string &Par) const;
	string get(const string &Par) const;

	string get() const;
	string getMin() const;
	string getMax() const;
	//string getDefault() const;
	string getInfo() const;
	bool getRO() const;

	void settype(int newtype);

	void set(const string &Par, const string &Val);

	void set(const string &Val);
	void setMin(const string &Val);
	void setMax(const string &Val);
	//void setDefault(const string &Val);
	void setInfo(const string &Val);
	void setRO(bool Val);

protected:
	int _type;
	std::map<string, string> par;
	int cnt;
};// class tCfgKey




class tCfgFolder;




class tCfg {
	friend class tCfgBin;
	friend class tCfgFloat;
	friend class tCfgInt;
	friend class tCfgString;
	friend class tCfgBool;
public:
	tCfg();
	tCfg(const tCfg &s);
	tCfg(tCfgFolder *f);
	tCfg(const tCfg &s, const string &path);
	tCfg(const string &fname);
	virtual ~tCfg();

	tCfg &operator=(const tCfg &s);

	bool operator==(const tCfg &s) const { return f == s.f; }
	bool operator!=(const tCfg &s) const { return !operator==(s); }

	string exp(const string &path) const;
	bool imp(const string &path, const string &data);

	string getInfo(const string &path) const;
	bool setInfo(const string &path, const string &info);

	bool load(const string &fname);
	bool save(const string &fname, bool bak = false) const;
	bool save(bool bak = false) const;

	bool empty() const;
	bool clear();

	std::vector<string> subList(const string &path) const;
	std::vector<string> keyList(const string &path) const;

	bool findPath(const string &path) const;
	bool findKey(const string &path, const string &name) const;

	tCfgFolder *getPath(const string &path) const;
	bool getFullPath(const tCfgKey *Key, string &path, string &name) const;
	bool getFullPath(const tCfgFolder *Fl, string &path) const;

	tCfgKey *getKey(const string &path, const string &name) const;
	int getKeyType(const string &path, const string &name) const;

	bool addKey(const string &path, const string &name, const string &val);
	bool addKey(const string &path, const string &name, int val);
	bool addKey(const string &path, const string &name, double val);
	bool addKey(const string &path, const string &name, const void *buf, unsigned size);
	bool addKey(const string &path, const string &name, bool val);

	bool renameKey(const string &path, const string &oldname, const string &newname);
	bool renamePath(const string &path, const string &newname);

	bool addPath(const string &path);

	bool delKey(const string &path, const string &name);
	bool delPath(const string &name, bool r = false);

	string getString(const string &path, const string &name, const string &def) const;
	int getInt(const string &path, const string &name, int def) const;
	int getInt(const string &path, const string &name, int def, int Min, int Max) const;
	double getFloat(const string &path, const string &name, double def) const;
	double getFloat(const string &path, const string &name, double def, double Min, double Max) const;
	bool getBool(const string &path, const string &name, bool def) const;

	bool setString(const string &path, const string &name, const string &val);
	bool setInt(const string &path, const string &name, int val);
	bool setFloat(const string &path, const string &name, double val);
	bool setBool(const string &path, const string &name, bool val);

	static string mkpath(const string &f1, const string &f2);
	static string mkpath(const string &f1, const string &f2, const string &f3);
	static string mkpath(const string &f1, const string &f2, const string &f3, const string &f4);

protected:
	tCfgKey *makeKey(int type, const string &path, const string &name, const string &val);
	tCfgKey *makeKey(int type, const string &path, const string &name, const string &val,
	                 const string &Min, const string &Max);
	//tCfgFolder *getSub(const string &name) const;

private:
	tCfgFolder *f0;
	tCfgFolder *f;
	bool own;
};// class tCfg




#endif // __tCfg_h
