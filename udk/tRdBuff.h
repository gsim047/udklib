#ifndef __tRdBuff_h
#define __tRdBuff_h 1

//#pragma warning(disable: 4786)
#include "fLong.h"

#include <string>
using std::string;

//#include <windows.h>




class tRdBuff {
public:
	tRdBuff() : idx(0) {}
	tRdBuff(const string &s) : str(s), idx(0) {}
	tRdBuff(const tRdBuff &s) : str(s.str), idx(s.idx) {};
	virtual ~tRdBuff() {}

	tRdBuff &operator=(const tRdBuff &s);

	bool operator==(const tRdBuff &s) const;
	bool operator!=(const tRdBuff &s) const { return !operator==(s); }

	size_t size() const { return str.size() - idx; }
	bool empty() const { return size() == 0; }

	size_t fullsize() const { return str.size(); }

	void rewind() { idx = 0; }

	u8 getBYTE();
	u16 getWORD();
	u32 getDWORD();
	string getString();   // up to '\0'
	string get(size_t n);

	void getTo(void *ptr, size_t n);

	size_t tell() const { return idx; }
	void skip(size_t n) { idx += n; adjust(); }
	void seek(size_t n) { idx = n; adjust(); }

	bool find(u8 b);
	bool find(const string& s);

private:
	bool test(size_t n) const { return (idx + n) <= str.size(); }
	void adjust();

private:
//public:
	string str;
	size_t idx;
};// class tRdBuff




#endif // __tRdBuff_h
