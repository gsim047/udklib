#include "tByteArray.h"
#include "tCrc32.h"
//#include "fLong.h"
#include "tDebug.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <string.h>
#include "zlib.h"




nError::ByteArrayEmpty::ByteArrayEmpty() :
	Message("can't exec FromByteArray - array is empty") 
{
}// ByteArrayEmpty::ByteArrayEmpty




static void intToRaw(char *buf, int i)
{
	if ( !buf ) return;
	unsigned u = toBE((unsigned)i);
	memmove(buf, (char *)&u, sizeof(u));
}// intToRaw


static int rawToInt(const char *buf)
{
	if ( !buf ) return 0;
	return (int)fromBE(*((unsigned *)buf));
}// rawToInt


static void ullToRaw(char *buf, ulonglong i)
{
	if ( !buf ) return;
	ulonglong u = toBE(i);
	memmove(buf, (char *)&u, sizeof(u));
}// ullToRaw


static ulonglong rawToUll(const char *buf)
{
	if ( !buf ) return 0;
	return fromBE(*((ulonglong *)buf));
}// rawToUll




tByteArray::tByteArray(int i)
{
	//debugLog("tByteArray(" + ToString(i) +")");
	char cc[4];
	intToRaw(cc, i);
	append(cc, 4);
}// tByteArray::tByteArray


tByteArray::tByteArray(unsigned u)
{
	//debugLog("tByteArray(0x" + ToStringHex(u, 8) +")");
	char cc[4];
	intToRaw(cc, (int)u);
	append(cc, 4);
}// tByteArray::tByteArray

/*
tByteArray::tByteArray(longlong ll)
{
	ulonglong u = (ulonglong)ll;
	//debugLog("tByteArray(0x" + ToStringHex(u, 8) +")");
	char cc[8];
	ullToRaw(cc, u);
	append(cc, 8);
}// tByteArray::tByteArray
*/

tByteArray::tByteArray(ulonglong u)
{
	//debugLog("tByteArray(0x" + ToStringHex(u, 8) +")");
	char cc[8];
	ullToRaw(cc, u);
	append(cc, 8);
}// tByteArray::tByteArray


tByteArray::tByteArray(const string &s)
{
	//debugLog("tByteArray(''" + s + "'')");
	//debugLog(BufDump(s.data(), s.size()));
	char cc[4];
	intToRaw(cc, s.size());
	//_buf.insert(_buf.end(), cc, cc + 4);
	append(cc, 4);
	if ( s.size() > 0 ) append(s);
}// tByteArray::tByteArray

/*
tByteArray::tByteArray(const tDataRow &dr)
{
	char cc[4];
	intToRaw(cc, dr.size());
	append(cc, 4);
	std::vector<string> v = dr.getFields();
	unsigned n = 0;
	for ( tDataRow::const_iterator i = dr.begin(); i != dr.end(); ++i, ++n ) {
		append(v[n]);
		append(*i);
	}
}// tByteArray::tByteArray
*/

tByteArray::tByteArray(const void *buf, unsigned len) :
  string((const char *)buf, len)
{
	//debugLog("tByteArray(...)");
}// tByteArray::tByteArray

/*
bool tByteArray::insert(unsigned ind, const tByteArray &src)
{
	if ( ind >= size() ) return false;
	::insert(ind, src._buf);
	return true;
}// tByteArray::insert


bool tByteArray::erase(unsigned ind, unsigned _size)
{
	if ( ind + _size - 1 >= _buf.size() ) return false;
	_buf.erase(ind, _size);
	return true;
}// tByteArray::erase
*/

bool tByteArray::getChar(unsigned &ind, char &var) const
{
	if ( ind + 1 > size() ) return false;
	var = operator[](ind++);
	return true;
}// tByteArray::getChar


bool tByteArray::getBool(unsigned &ind, bool &var) const
{
	if ( ind + 1 > size() ) return false;
	var = operator[](ind++) != '\0';
	return true;
}// tByteArray::getBool


bool tByteArray::getInt(unsigned &ind, int &var) const
{
	if ( ind + 4 > size() ) return false;
	char c[4];
	c[0] = operator[](ind++);
	c[1] = operator[](ind++);
	c[2] = operator[](ind++);
	c[3] = operator[](ind++);
	var = rawToInt(c);
	return true;
}// tByteArray::getInt


bool tByteArray::getUInt(unsigned &ind, unsigned &var) const
{
	if ( ind + 4 > size() ) return false;
	char c[4];
	c[0] = operator[](ind++);
	c[1] = operator[](ind++);
	c[2] = operator[](ind++);
	c[3] = operator[](ind++);
	var = (unsigned)rawToInt(c);
	return true;
}// tByteArray::getUInt


bool tByteArray::getULL(unsigned &ind, ulonglong &var) const
{
	if ( ind + 8 > size() ) return false;
	char c[8];
	c[0] = operator[](ind++);
	c[1] = operator[](ind++);
	c[2] = operator[](ind++);
	c[3] = operator[](ind++);
	c[4] = operator[](ind++);
	c[5] = operator[](ind++);
	c[6] = operator[](ind++);
	c[7] = operator[](ind++);
	var = rawToUll(c);
	return true;
}// tByteArray::getULL

/*
bool tByteArray::getLInt(unsigned &ind, longlong &var) const
{
	ulonglong u;
	bool ret = getULL(ind, u);
	if ( ret )  var = (longlong)u;
	return ret;
}// tByteArray::getLInt
*/

bool tByteArray::getString(unsigned &ind, string &var) const
{
	unsigned len;
	if ( !getUInt(ind, len) || len > 0x10000 || ind + len > size() ) return false;
	var = substr(ind, len);
	ind += len;
	return true;
}// tByteArray::getString


unsigned long tByteArray::calcCrc32(unsigned ind, unsigned len) const
{
	tCrc32 crc(c_str() + ind, len);
	return crc.crc();
}// tByteArray::calcCrc32


bool tByteArray::zip(int Method)
{
	if ( size() <= 12 ) return false;
	unsigned long bufsize = size() + size() / 1000 + 13;
	char *buf = new char[bufsize];
	int errcode = compress2((Bytef *)buf, &bufsize, (const Bytef *)data(), size(), Method);
	if ( errcode == Z_OK && bufsize + 4 < size() ){
		tByteArray b((unsigned)size());
		//debugLog("zip: " + ToString(size()) + " -> " + ToString(bufsize));
		b += tByteArray(buf, (unsigned) bufsize);
		delete [] buf;
		*this = b;
		return true;
	}
	delete [] buf;
	return false;
}// tByteArray::zip


bool tByteArray::unzip()
{
	if ( size() <= 16 ) return false;
	unsigned ind = 0;
	unsigned u_bsize;
	if ( !getUInt(ind, u_bsize) )  return false;
	unsigned long bufsize = u_bsize;
	//debugLog("unzip: u_bsize: " + ToString(u_bsize) + "/" + ToString(size()));
	char *buf = new char[bufsize];
	int errcode = uncompress((Bytef *)buf, &bufsize, (const Bytef *)(data() + 4), size() - 4);
	if ( errcode == Z_OK ){
		*this = tByteArray(buf, (unsigned) bufsize);
		//debugLog("unzip: " + ToString(size()) + " -> " + ToString(bufsize));
		delete [] buf;
		return true;
	}
	delete [] buf;
	return false;
}// tByteArray::unzip




/*
template <class T> tByteArray ToByteArray(const std::vector<T> &v)
{
	tByteArray ba(v.size());
	for ( std::vector<T>::const_iterator i = v.begin();
	      i != v.end(); ++i ){
		ba += ToByteArray(*i);
	}
	return ba;
}// ToByteArray


template <class T> tByteArray ToByteArray(const std::deque<T> &d)
{
	tByteArray ba(d.size());
	for ( std::deque<T>::const_iterator i = d.begin();
	      i != d.end(); ++i ){
		ba += ToByteArray(*i);
	}
	return ba;
}// ToByteArray


template <class T> tByteArray ToByteArray(const std::list<T> &l)
{
	tByteArray ba(l.size());
	for ( std::list<T>::const_iterator i = l.begin();
	      i != l.end(); ++i ){
		ba += ToByteArray(*i);
	}
	return ba;
}// ToByteArray
*/

/*
template <class T1, class T2> tByteArray ToByteArray(const std::map<T1, T2> &m)
{
	tByteArray ba(m.size());
	for ( std::map<T1, T2>::const_iterator i = m.begin();
	      i != m.end(); ++i ){
		ba += ToByteArray(i->first);
		ba += ToByteArray(i->second);
	}
	return ba;
}// ToByteArray
*/

tByteArray &FromByteArray(tByteArray &ba, char &c)
{
	unsigned ind = 0;
	if ( !ba.get(ind, c) ){
		nError::ByteArrayEmpty e;
		throw e;
	}
	ba.erase(0, ind);
	return ba;
}// FromByteArray


tByteArray &FromByteArray(tByteArray &ba, bool &b)
{
	unsigned ind = 0;
	if ( !ba.get(ind, b) ){
		nError::ByteArrayEmpty e;
		throw e;
	}
	ba.erase(0, ind);
	return ba;
}// FromByteArray


tByteArray &FromByteArray(tByteArray &ba, int &i)
{
	unsigned ind = 0;
	if ( !ba.get(ind, i) ){
		nError::ByteArrayEmpty e;
		throw e;
	}
	ba.erase(0, ind);
	return ba;
}// FromByteArray


tByteArray &FromByteArray(tByteArray &ba, unsigned &u)
{
	unsigned ind = 0;
	if ( !ba.get(ind, u) ){
		nError::ByteArrayEmpty e;
		throw e;
	}
	ba.erase(0, ind);
	return ba;
}// FromByteArray


tByteArray &FromByteArray(tByteArray &ba, ulonglong &u)
{
	unsigned ind = 0;
	if ( !ba.get(ind, u) ){
		nError::ByteArrayEmpty e;
		throw e;
	}
	ba.erase(0, ind);
	return ba;
}// FromByteArray

/*
tByteArray &FromByteArray(tByteArray &ba, longlong &u)
{
	unsigned ind = 0;
	if ( !ba.get(ind, u) ){
		nError::ByteArrayEmpty e;
		throw e;
	}
	ba.erase(0, ind);
	return ba;
}// FromByteArray
*/

tByteArray &FromByteArray(tByteArray &ba, string &s)
{
	unsigned ind = 0;
	if ( !ba.get(ind, s) ){
		nError::ByteArrayEmpty e;
		throw e;
	}
	ba.erase(0, ind);
	return ba;
}// FromByteArray

/*
tByteArray &FromByteArray(tByteArray &ba, tDataRow &l)
{
	l.clear();
	unsigned size;
	FromByteArray(ba, size);
	for ( unsigned i = 0; i < size; i++ ){
		string t1;
		FromByteArray(ba, t1);
		string t2;
		FromByteArray(ba, t2);
		l[t1] = t2;
	}
	return ba;
}// FromByteArray 
*/