#ifndef __tStrHash_h
#define __tStrHash_h 1

#pragma warning(disable:4786)

#include <string>
#include <map>
using std::string;




class tStrHash {
public:
	tStrHash() {}
	tStrHash(const string &src);
	tStrHash(const std::map<string, string> &src) : h(src) {}
	tStrHash(const tStrHash &src) : h(src.h) {}

	tStrHash &operator=(const tStrHash &src) { if ( &src != this ) h = src.h; return *this; }
	tStrHash &operator=(const std::map<string, string> &src) { h = src; return *this; }

	bool find(const string &key) const { return h.find(key) != h.end(); }
	string operator[](const string &key) const;
	string get(const string &key, const string &def = "") const;
	int get(const string &key, int def) const;

	string toString() const;
	std::map<string, string> toMap() const;

	void set(const string &key, const string &value) { h[key] = value; }
	void set(const string &key, int value);

private:
	std::map<string, string> h;
};// class tStrHash




#endif // __tStrHash_h
