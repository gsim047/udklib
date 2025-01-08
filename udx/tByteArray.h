#ifndef __tByteArray_h
#define __tByteArray_h 1

#ifndef __fLong_h
#include "fLong.h"
#endif

#ifndef __fString_h
#include "fString.h"
#endif

#include <vector>
#include <deque>
#include <list>
#include <map>

#ifndef __tDataRow_h
//#include "tDataRow.h"
#endif

#ifndef __nError_h
#include "nError.h"
#endif

#ifndef __tDebug_h
#include "tDebug.h"
#endif




namespace nError{
	class ByteArrayEmpty : public Message{
	public:
		ByteArrayEmpty();
	};
};// namespace nError




class tByteArray : public std::string {
public:
	tByteArray() {};
	tByteArray(char c) : string(1, c) {};
	tByteArray(bool b) : string(1, b ? '\x01' : '\0') {};
	tByteArray(int i);
	tByteArray(unsigned i);
	tByteArray(ulonglong ul);
	tByteArray(const string &s);
	//tByteArray(const tDataRow &dr);
	tByteArray(const void *buf, unsigned len);

public:
	tByteArray &operator<<(const tByteArray &src) { *this += src; return *this; };

	void clear() { erase(); };

	unsigned long calcCrc32(unsigned ind = 0) const { return calcCrc32(ind, size()); };
	unsigned long calcCrc32(unsigned ind, unsigned size) const;

	bool get(unsigned &ind, char &var) const { return getChar(ind, var); };
	bool get(unsigned &ind, bool &var) const { return getBool(ind, var); };
	bool get(unsigned &ind, int &var) const { return getInt(ind, var); };
	bool get(unsigned &ind, unsigned &var) const { return getUInt(ind, var); };
	bool get(unsigned &ind, ulonglong &var) const { return getULL(ind, var); };
	//bool get(unsigned &ind, longlong &var) const { return getLInt(ind, var); };
	bool get(unsigned &ind, string &var) const { return getString(ind, var); };

	bool zip(int Method = 4);
	bool unzip();

protected:
	bool getChar(unsigned &ind, char &var) const;
	bool getBool(unsigned &ind, bool &var) const;
	bool getInt(unsigned &ind, int &var) const;
	bool getUInt(unsigned &ind, unsigned &var) const;
	bool getULL(unsigned &ind, ulonglong &var) const;
	//bool getLInt(unsigned &ind, longlong &var) const;
	bool getString(unsigned &ind, string &var) const;

};// class tByteArray




inline tByteArray ToByteArray(const char &c) { return tByteArray(c); };
inline tByteArray ToByteArray(const bool &b) { return tByteArray(b); };
inline tByteArray ToByteArray(const int &i) { return tByteArray(i); };
inline tByteArray ToByteArray(const unsigned &i) { return tByteArray(i); };
inline tByteArray ToByteArray(const ulonglong &ul) { return tByteArray(ul); };
//inline tByteArray ToByteArray(const longlong &ul) { return tByteArray(ul); };
inline tByteArray ToByteArray(const string &s) { return tByteArray(s); };
//inline tByteArray ToByteArray(const tDataRow &dr) { return tByteArray(dr); };

template <class T> tByteArray ToByteArray(const std::vector<T> &v)
{
	tByteArray ba(v.size());
	for ( typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i ) {
		ba += ToByteArray(*i);
	}
	return ba;
}// ToByteArray

template <class T> tByteArray ToByteArray(const std::deque<T> &d)
{
	tByteArray ba(d.size());
	for ( typename std::deque<T>::const_iterator i = d.begin(); i != d.end(); ++i ) {
		ba += ToByteArray(*i);
	}
	return ba;
}// ToByteArray

template <class T> tByteArray ToByteArray(const std::list<T> &l)
{
	tByteArray ba(l.size());
	for ( typename std::list<T>::const_iterator i = l.begin(); i != l.end(); ++i ) {
		ba += ToByteArray(*i);
	}
	return ba;
}// ToByteArray

template <class T1, class T2> tByteArray ToByteArray(const std::map<T1, T2> &m)
{
	tByteArray ba(m.size());
	for ( typename std::map<T1, T2>::const_iterator i = m.begin(); i != m.end(); ++i ) {
		ba += ToByteArray(i->first);
		ba += ToByteArray(i->second);
	}
	return ba;
}// ToByteArray


tByteArray &FromByteArray(tByteArray &ba, char &c);
tByteArray &FromByteArray(tByteArray &ba, bool &b);
tByteArray &FromByteArray(tByteArray &ba, int &i);
tByteArray &FromByteArray(tByteArray &ba, unsigned &u);
tByteArray &FromByteArray(tByteArray &ba, ulonglong &u);
tByteArray &FromByteArray(tByteArray &ba, longlong &u);
tByteArray &FromByteArray(tByteArray &ba, string &s);
//tByteArray &FromByteArray(tByteArray &ba, tDataRow &l);

template <class T> tByteArray &FromByteArray(tByteArray &ba, std::vector<T> &v)
{
	v.clear();
	unsigned size;
	FromByteArray(ba, size);
	//v.resize(size);
	for ( unsigned i = 0; i < size; i++ ){
		T t;
		FromByteArray(ba, t);
		v.push_back(t);
	}
	return ba;
}// FromByteArray

template <class T> tByteArray &FromByteArray(tByteArray &ba, std::deque<T> &d)
{
	d.clear();
	unsigned size;
	FromByteArray(ba, size);
	for ( unsigned i = 0; i < size; i++ ){
		T t;
		FromByteArray(ba, t);
		d.push_back(t);
	}
	return ba;
}// FromByteArray

template <class T> tByteArray &FromByteArray(tByteArray &ba, std::list<T> &l)
{
	l.clear();
	unsigned size;
	FromByteArray(ba, size);
	for ( unsigned i = 0; i < size; i++ ){
		T t;
		FromByteArray(ba, t);
		l.push_back(t);
	}
	return ba;
}// FromByteArray

template <class T1, class T2> tByteArray &FromByteArray(tByteArray &ba, std::map<T1, T2> &l)
{
	l.clear();
	unsigned size;
	FromByteArray(ba, size);
	for ( unsigned i = 0; i < size; i++ ){
		T1 t1;
		FromByteArray(ba, t1);
		T2 t2;
		FromByteArray(ba, t2);
		l[t1] = t2;
	}
	return ba;
}// FromByteArray


/*
inline tByteArray &operator<<(tByteArray &ba, char t)
{
	ba += ToByteArray(t);
	return ba;
};// operator<<

inline tByteArray &operator>>(tByteArray &ba, char &t)
{
	FromByteArray(ba, t);
	return ba;
};// operator>>


inline tByteArray &operator<<(tByteArray &ba, int t)
{
	ba += ToByteArray(t);
	return ba;
};// operator<<

inline tByteArray &operator>>(tByteArray &ba, int &t)
{
	FromByteArray(ba, t);
	return ba;
};// operator>>


inline tByteArray &operator<<(tByteArray &ba, unsigned t)
{
	ba += ToByteArray(t);
	return ba;
};// operator<<

inline tByteArray &operator>>(tByteArray &ba, unsigned &t)
{
	FromByteArray(ba, t);
	return ba;
};// operator>>


inline tByteArray &operator<<(tByteArray &ba, longlong t)
{
	ba += ToByteArray(t);
	return ba;
};// operator<<

inline tByteArray &operator>>(tByteArray &ba, longlong &t)
{
	FromByteArray(ba, t);
	return ba;
};// operator>>


inline tByteArray &operator<<(tByteArray &ba, const string &t)
{
	ba += ToByteArray(t);
	return ba;
};// operator<<

inline tByteArray &operator>>(tByteArray &ba, string &t)
{
	FromByteArray(ba, t);
	return ba;
};// operator>>
*/


template <class T> tByteArray &operator<<(tByteArray &ba, const std::vector<T> &v)
{
	ba += ToByteArray((unsigned)v.size());
	for ( typename std::vector<T>::const_iterator i = v.begin();
	      i != v.end(); ++i ){
		ba += ToByteArray(*i);
	}
	return ba;
};// operator<<

template <class T> tByteArray &operator<<(tByteArray &ba, const std::deque<T> &v)
{
	ba += ToByteArray(v.size());
	for ( typename std::deque<T>::const_iterator i = v.begin();
	      i != v.end(); ++i ){
		ba += ToByteArray(*i);
	}
	return ba;
};// operator<<

template <class T> tByteArray &operator<<(tByteArray &ba, const std::list<T> &v)
{
	ba += ToByteArray(v.size());
	for ( typename std::list<T>::const_iterator i = v.begin();
	      i != v.end(); ++i ){
		ba += ToByteArray(*i);
	}
	return ba;
};// operator<<

template <class T1, class T2> tByteArray &operator<<(tByteArray &ba, const std::map<T1, T2> &m)
{
	ba += ToByteArray(m.size());
	for ( typename std::map<T1, T2>::const_iterator i = m.begin();
	      i != m.end(); ++i ){
		ba += ToByteArray(i->first);
		ba += ToByteArray(i->second);
	}
	return ba;
}// ToByteArray

template <class T> tByteArray &operator<<(tByteArray &ba, const T &t)
{
	ba += ToByteArray(t);
	return ba;
};// operator<<

template <class T> tByteArray &operator>>(tByteArray &ba, T &t)
{
	FromByteArray(ba, t);
	return ba;
};// operator>>




#endif // __tByteArray_h
