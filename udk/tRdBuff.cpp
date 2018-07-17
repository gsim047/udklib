#include "tRdBuff.h"
#include "nError.h"

#include <memory.h>




tRdBuff &tRdBuff::operator=(const tRdBuff &s)
{
	if ( &s != this ){
		str = s.str;
		idx = s.idx;
	}
	return *this;
}// tRdBuff::operator=


bool tRdBuff::operator==(const tRdBuff &s) const
{
	if ( &s == this )  return true;
	if ( str == s.str && idx == s.idx )  return true;
	return false;
}// tRdBuff::operator==


u8 tRdBuff::getBYTE()
{
	if ( !test(1) )  nError::Throw("tRdBuff::getBYTE: overflow");
	u8 ret = (u8)(str[idx]);
	idx++;
	return ret;
}// tRdBuff::getBYTE


u16 tRdBuff::getWORD()
{
	if ( !test(sizeof(u16)) )  nError::Throw("tRdBuff::getWORD: overflow");
	u16 ret = *((u16*)(str.c_str() + idx));
	idx += sizeof(u16);
	return ret;
}// tRdBuff::getWORD


u32 tRdBuff::getDWORD()
{
	if ( !test(sizeof(u32)) )  nError::Throw("tRdBuff::getDWORD: overflow");
	u32 ret = *((u32*)(str.c_str() + idx));
	idx += sizeof(u32);
	return ret;
}// tRdBuff::getDWORD


string tRdBuff::get(size_t siz)
{
	if ( !test(siz) )  nError::Throw("tRdBuff::get: overflow");
	string ret = str.substr(idx, siz);
	idx += siz;
	return ret;
}// tRdBuff::get


bool tRdBuff::find(u8 b)
{
	size_t pos = str.find((char)b, idx);
	if ( pos == string::npos )  return false;
	idx = pos;
	return true;
}// tRdBuff::find


bool tRdBuff::find(const string& s)
{
	size_t pos = str.find(s, idx);
	if ( pos == string::npos )  return false;
	idx = pos;
	return true;
}// tRdBuff::find


void tRdBuff::adjust()
{
	if ( idx > str.size() )  idx = str.size();
}// tRdBuff::adjust


string tRdBuff::getString()   // up to '\0'
{
	string ret;
	size_t pos = str.find('\0', idx);
	if ( pos != string::npos ){
		ret = str.substr(idx, pos - idx);
		idx = pos + 1;
	}
	return ret;
}// tRdBuff::getString


void tRdBuff::getTo(void *ptr, size_t siz)
{
	if ( !test(siz) )  nError::Throw("tRdBuff::getTo: overflow");
	memcpy(ptr, str.c_str() + idx, siz);
	idx += siz;
}// tRdBuff::getTo
