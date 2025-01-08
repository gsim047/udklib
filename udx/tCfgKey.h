#ifndef __tCfgKey_h
#define __tCfgKey_h 1

#ifndef __tCfg_h
#include "tCfg.h"
#endif

#ifndef __tMutex_h
#include "tMutex.h"
#endif




class tCfgString {
public:
	tCfgString(const string &val);
	tCfgString(tCfgKey *k);
	tCfgString(tCfg &f, const string &Path, const string &Name);
	tCfgString(tCfg &f, const string &Path, const string &Name, const string &Val);
	tCfgString(const tCfgString &s) : p(s.p), Val(s.Val) {};
	virtual ~tCfgString();

	tCfgString &operator=(const tCfgString &s){
		tMutexLock l(m);
		if ( this != &s && Val != s.Val )  set(s.Val);  return *this;
	}

	tCfgString &operator=(const string &val){ tMutexLock l(m); set(val); return *this; }

	operator string() const;

	bool operator==(const tCfgString &s) const { tMutexLock l(m); return Val == s.Val; }
	bool operator!=(const tCfgString &s) const { return !operator==(s); }

	void setInfo(const string &Inf);
	void setRO(bool RO);

private:
	void set(const string &val);

protected:
	mutable tMutex m;
	tCfgKey *p;
	string Val;
};// class tCfgString




class tCfgBin {
public:
	tCfgBin(const string &val);
	tCfgBin(const void *val, unsigned size);
	tCfgBin(tCfgKey *k);
	tCfgBin(tCfg &f, const string &Path, const string &Name);
	tCfgBin(tCfg &f, const string &Path, const string &Name, const void *val, unsigned size);
	tCfgBin(const tCfgBin &s) : p(s.p), Val(s.Val) {};
	virtual ~tCfgBin();

	tCfgBin &operator=(const tCfgBin &s){
		tMutexLock l(m);
		if ( this != &s && Val != s.Val )  set(s.Val);  return *this;
	}

	tCfgBin &operator=(const string &val){ tMutexLock l(m); set(val); return *this; }

	operator string() const;

	bool operator==(const tCfgBin &s) const { tMutexLock l(m); return Val == s.Val; }
	bool operator!=(const tCfgBin &s) const { return !operator==(s); }

	void setInfo(const string &Inf);
	void setRO(bool RO);

private:
	static string d2v(const void *val, unsigned size);
	void set(const string &val);

protected:
	mutable tMutex m;
	tCfgKey *p;
	string Val;
};// class tCfgBin




class tCfgBool {
public:
	tCfgBool(bool val);
	tCfgBool(tCfgKey *k);
	tCfgBool(tCfg &f, const string &Path, const string &Name);
	tCfgBool(tCfg &f, const string &Path, const string &Name, bool Val);
	tCfgBool(const tCfgBool &s) : p(s.p), Val(s.Val) {};
	virtual ~tCfgBool();

	tCfgBool &operator=(const tCfgBool &s);
	tCfgBool &operator=(bool val){ tMutexLock l(m); set(val); return *this; }

	operator bool() const { tMutexLock l(m); return Val; }

	bool operator==(const tCfgBool &s) const { tMutexLock l(m); return Val == s.Val; }
	bool operator!=(const tCfgBool &s) const { return !operator==(s); }

	string getInfo() const;
	bool getRO() const;

	void setInfo(const string &Inf);
	void setRO(bool RO);

private:
	static string b2s(bool v);
	static bool s2b(const string &s);
	bool set(bool val);

protected:
	mutable tMutex m;
	//string name;
	tCfgKey *p;
	bool Val;
};// class tCfgBool




class tCfgInt {
public:
	tCfgInt(int val);
	tCfgInt(tCfgKey *k);
	tCfgInt(tCfg &f, const string &Path, const string &Name);
	tCfgInt(tCfg &f, const string &Path, const string &Name, int Val);
	tCfgInt(tCfg &f, const string &Path, const string &Name, int Val, int Min, int Max);
	tCfgInt(const tCfgInt &s) : p(s.p), Val(s.Val), Min(s.Min), Max(s.Max) {};
	virtual ~tCfgInt();

	tCfgInt &operator=(const tCfgInt &s);
	tCfgInt &operator=(int val){ tMutexLock l(m); set(val); return *this; }

	operator int() const { tMutexLock l(m); return Val; }

	int operator+=(int val){ tMutexLock l(m); set(Val + val); return Val; }
	int operator-=(int val){ tMutexLock l(m); set(Val - val); return Val; }
	int operator*=(int val){ tMutexLock l(m); set(Val * val); return Val; }
	int operator/=(int val){ tMutexLock l(m); set(Val / val); return Val; }
	int operator%=(int val){ tMutexLock l(m); set(Val % val); return Val; }

	int operator++(int){ tMutexLock l(m); int ret = Val; set(Val + 1); return ret; }
	int operator++()   { tMutexLock l(m); return set(Val + 1); }
	int operator--(int){ tMutexLock l(m); int ret = Val; set(Val - 1); return ret; }
	int operator--()   { tMutexLock l(m); return set(Val - 1); }

	bool operator==(const tCfgInt &s) const { tMutexLock l(m); return Val == s.Val; }
	bool operator!=(const tCfgInt &s) const { return !operator==(s); }

	int getMin() const { return Min; }
	int getMax() const { return Max; }
	string getInfo() const;
	bool getRO() const;

	void setMin(int Min);
	void setMax(int Max);
	void delMin();
	void delMax();
	void setInfo(const string &Inf);
	void setRO(bool RO);

private:
	static int s2i(const string &s, int def);
	static string i2s(int v);
	int set(int val);

protected:
	mutable tMutex m;
	//string name;
	tCfgKey *p;
	int Val;
	int Min;
	int Max;
};// class tCfgInt




class tCfgFloat {
public:
	tCfgFloat(double val);
	tCfgFloat(tCfgKey *k);
	tCfgFloat(tCfg &f, const string &Path, const string &Name);
	tCfgFloat(tCfg &f, const string &Path, const string &Name, double Val);
	tCfgFloat(tCfg &f, const string &Path, const string &Name, double Val, double Min, double Max);
	tCfgFloat(const tCfgFloat &s) : p(s.p), Val(s.Val), Min(s.Min), Max(s.Max) {};
	virtual ~tCfgFloat();

	tCfgFloat &operator=(const tCfgFloat &s);
	tCfgFloat &operator=(double val){ tMutexLock l(m); set(val); return *this; }

	operator double() const { tMutexLock l(m); return Val; }

	double operator+=(double val){ tMutexLock l(m); set(Val + val); return Val; }
	double operator-=(double val){ tMutexLock l(m); set(Val - val); return Val; }
	double operator*=(double val){ tMutexLock l(m); set(Val * val); return Val; }
	double operator/=(double val){ tMutexLock l(m); set(Val / val); return Val; }

	bool operator==(const tCfgFloat &s) const { tMutexLock l(m); return Val == s.Val; }
	bool operator!=(const tCfgFloat &s) const { return !operator==(s); }

	void setMin(double Min);
	void setMax(double Max);
	void delMin();
	void delMax();
	void setInfo(const string &Inf);
	void setRO(bool RO);

private:
	static string d2s(double v);
	static double s2d(const string &v, double def);
	double set(double val);

protected:
	mutable tMutex m;
	tCfgKey *p;
	double Val;
	double Min;
	double Max;
};// class tCfgFloat




#endif // __tCfgKey_h
