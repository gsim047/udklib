#include "tFillBuff.h"
#include <string.h>



tFillBuff::tFillBuff(size_t size, size_t gran) :
	cap(size),
	gr((gran >= 1024) ? gran : 1024),
	buf(new char[size]),
	idx(0)
{
}// tFillBuff::tFillBuff


tFillBuff::~tFillBuff()
{
	delete [] buf;
}// tFillBuff::~tFillBuff


tFillBuff::tFillBuff(const tFillBuff &s) :
	cap(s.cap),
	gr(s.gr),
	buf(new char[s.cap]),
	idx(s.idx)
{
	memcpy(buf, s.buf, s.idx);
}// tFillBuff::tFillBuff


tFillBuff &tFillBuff::operator=(const tFillBuff &s)
{
	if ( &s != this ){
		cap = s.cap;
		gr = s.gr;
		idx = s.idx;
		delete [] buf;
		buf = new char[cap];
		memcpy(buf, s.buf, idx);
	}
	return *this;
}// tFillBuff::operator=


bool tFillBuff::operator==(const tFillBuff &s) const
{
	if ( &s == this )  return true;
	return false;
}// tFillBuff::operator==



tFillBuff::operator string() const
{
	return string(buf, idx);
}// tFillBuff::operator string


void tFillBuff::push(char c)
{
	if ( (idx + 1) >= cap )  enlarge(gr);
	buf[idx++] = c;
}// tFillBuff::push


void tFillBuff::push(const void *b, size_t siz)
{
	if ( !b || !siz )  return;
	if ( (idx + siz) >= cap ){		
		enlarge(((idx + siz - cap) / gr + 1) * gr);
	}
	memcpy(buf + idx, b, siz);
	idx += siz;
}// tFillBuff::push


void tFillBuff::fill(char c, size_t siz)
{
	if ( (idx + siz) >= cap ){		
		enlarge(((idx + siz - cap) / gr + 1) * gr);
	}
	memset(buf + idx, c, siz);
	idx += siz;
}// tFillBuff::fill


void tFillBuff::enlarge(size_t toadd)
{
	char *buf2 = buf;
	buf = new char[cap + toadd];
	memcpy(buf, buf2, idx);
	delete [] buf2;
	cap += toadd;
}// tFillBuff::enlarge


string tFillBuff::get(size_t sz)
{
	if ( sz > idx )  sz = idx;
	string ret(buf, sz);
	if ( sz < idx )  memmove(buf, buf + sz, idx - sz);
	idx -= sz;
	return ret;
}// tFillBuff::get


string tFillBuff::peek(size_t sz) const
{
	if ( sz > idx )  sz = idx;
	return string(buf, sz);
}// tFillBuff::peek


size_t tFillBuff::find(const string &s) const
{
	if ( s.empty() )  return string::npos;

	char *p = buf;
	for ( p = (char *)memchr(p, s[0], idx - s.size() + 1);
	      p && p <= buf + (idx - s.size()); ){
		
		if ( memcmp(p, s.c_str(), s.size()) == 0 )  return (p - buf);
		p++;
	}
	return string::npos;
}// tFillBuff::find

