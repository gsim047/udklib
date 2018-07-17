#include "tRecodeCC.h"
#include <string.h>



tRecodeCCRef::operator char() const 
{
	return cc->_c[idx]; 
}// tRecodeCCRef::operator char


tRecodeCCRef &tRecodeCCRef::operator=(char c)
{
	tRecodeCC *dd = (tRecodeCC *)cc;
	dd->_c[idx] = c; 
	return *this; 
}// tRecodeCCRef::operator=




void tRecodeCC::init()
{
	for ( unsigned i = 0; i < 256; i++ )  _c[i] = char((unsigned char)i);
}

tRecodeCC::tRecodeCC()
{
	init();
}// tRecodeCC::tRecodeCC


tRecodeCC::tRecodeCC(const char *cc, unsigned size, unsigned pos)
{
	init();
	if ( cc && pos < 256 ){
		memcpy(_c + pos, cc, (size + pos <= 256) ? size : (256 - pos));
	}
}// tRecodeCC::tRecodeCC


tRecodeCC::tRecodeCC(const tRecodeCC &s)
{
	memcpy(_c, s._c, 256);
}// tRecodeCC::tRecodeCC


tRecodeCC &tRecodeCC::operator=(const tRecodeCC &s)
{
	if ( &s != this ){
		memcpy(_c, s._c, 256);
	}
	return *this;
}// tRecodeCC::operator=


bool tRecodeCC::operator==(const tRecodeCC &s) const
{
	if ( &s == this )  return true;
	if ( memcmp(_c, s._c, 256) == 0 )  return true;
	return false;
}// tRecodeCC::operator==


string tRecodeCC::Recode(const string &s) const
{
	string ret = s;
	RecodeIP(ret);
	return ret;
}// tRecodeCC::Recode


void tRecodeCC::RecodeIP(string &s) const
{
	for ( size_t i = 0; i < s.size(); i++ ){
		s[i] = _c[c2i(s[i])];
	}
}// tRecodeCC::RecodeIP


tRecodeCCRef tRecodeCC::operator[](char c) const
{
	return tRecodeCCRef(this, c2i(c));
}// tRecodeCC::operator

