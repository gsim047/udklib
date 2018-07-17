#ifndef __tSHA_h
#define __tSHA_h 1

#include "tHash.h"




class tSHA : public tHash {
public:
	tSHA();
	tSHA(const string &str);
	tSHA(const char * const s, size_t len);
	virtual ~tSHA();

	void calc(const string &s);
	void calc(const char * const s, size_t len);
	string hash() const;

	size_t size() const { return 20; }
	string name() const;

private:
	void init();

	unsigned long *state; //[5];
	unsigned long *count; //[2];
	unsigned char *buffer; //[64];
};// class tSHA




#endif // __tSHA_h
