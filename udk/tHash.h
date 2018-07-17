#ifndef __tHash_h
#define __tHash_h 1

#include <string>
using std::string;




class tHash {
public:
	tHash() {};
	tHash(const string &) {};
	tHash(const char * const, size_t) {};
	virtual ~tHash() {};

	virtual void calc(const string &s) = 0;
	virtual void calc(const char * const, size_t) = 0;
	virtual string hash() const = 0;

	virtual size_t size() const = 0;
	virtual string name() const = 0;

};// class tHash




#endif // __tHash_h
