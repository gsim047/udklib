#ifndef __tSHA512_h
#define __tSHA512_h 1

#include "tHash.h"




class tSHA512 : public tHash {
public:
	tSHA512();
	tSHA512(const string &str);
	tSHA512(const char * const s, size_t len);
	virtual ~tSHA512();

	void calc(const string &s);
	void calc(const char * const s, size_t len);
	string hash() const;

	size_t size() const { return 64; }
	string name() const;

private:
	void init();

	char *state;
};// class tSHA512




#endif // __tSHA512_h
