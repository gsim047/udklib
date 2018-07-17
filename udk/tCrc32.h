#ifndef __tCrc32_h
#define __tCrc32_h 1

#include "tHash.h"




class tCrc32 : public tHash {
public:
	tCrc32() : tHash(), _crc(0xffffffff) {};
	tCrc32(const string &s) : tHash(), _crc(0xffffffff) { calc(s); };
	tCrc32(const char * const s, size_t len) : tHash(), _crc(0xffffffff) { calc(s, len); };
	virtual ~tCrc32() {};

	void calc(const string &s);
	void calc(const char * const s, size_t len);
	string hash() const;

	size_t size() const { return 4; }
	string name() const;

	unsigned crc() const { return _crc ^ 0xffffffff; }
private:
	unsigned _crc;
};// class tCrc32




#endif // __tCrc32_h
