#ifndef __tCrc64_h
#define __tCrc64_h 1

#include "tHash.h"
#include "fLong.h"




class tCrc64 : public tHash {
public:
	tCrc64() : tHash(), _crc(0) {};
	tCrc64(const string &s) : tHash(), _crc(0) { calc(s); };
	tCrc64(const char * const s, size_t len) : tHash(), _crc(0) { calc(s, len); };
	virtual ~tCrc64() {};

	void calc(const string &s);
	void calc(const char * const s, size_t len);
	string hash() const;

	size_t size() const { return 8; }
	string name() const;

	ulonglong crc() const { return _crc; }
private:
	ulonglong _crc;
};// class tCrc64




#endif // __tCrc64_h
