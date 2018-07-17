#include "tSHA512.h"
#include <stdio.h>
#include <string.h>
#include "_sha_h.cpp"






void tSHA512::init()
{
	state = (char *)(new SHA512_CTX);
	SHA512_Init((SHA512_CTX *)state);
}// tSHA512::init


tSHA512::tSHA512() :
	tHash(),
	state(NULL)
{
	init();
}// tSHA512::tSHA512


tSHA512::tSHA512(const string &str) :
	tHash(),
	state(NULL)
{
	init();
	calc(str);
}// tSHA512::tSHA512


tSHA512::tSHA512(const char * const s, size_t len) :
	tHash(),
	state(NULL)
{
	init();
	calc(s, len);
}// tSHA512::tSHA512


tSHA512::~tSHA512()
{
	delete state;
}// tSHA512::~tSHA512


void tSHA512::calc(const string &s)
{
	SHA512_Update((SHA512_CTX *)state, s.c_str(), s.size());
}// tSHA512::calc


void tSHA512::calc(const char * const s, size_t len)
{
	if ( s && len )  SHA512_Update((SHA512_CTX *)state, s, len);
}// tSHA512::calc


string tSHA512::hash() const
{
	SHA512_CTX state2;
	memcpy(&state2, state, sizeof(SHA512_CTX));
	unsigned char md[64];
	SHA512_Final(md, &state2);
	string ret = string((char *)md, 64);
	return ret;
}// tSHA512::hash


string tSHA512::name() const
{
	return "sha-512";
}// tSHA512::name
