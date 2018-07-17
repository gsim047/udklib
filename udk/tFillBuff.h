#ifndef __tFillBuff_h
#define __tFillBuff_h 1

#include <string>
using std::string;




class tFillBuff {

public:
	tFillBuff(size_t size, size_t gran = 0x10000);
	tFillBuff(const tFillBuff &s);
	virtual ~tFillBuff();

	tFillBuff &operator=(const tFillBuff &s);

	bool operator==(const tFillBuff &s) const;
	bool operator!=(const tFillBuff &s) const { return !operator==(s); }

	bool empty() const { return idx == 0; }
	size_t size() const { return idx; }
	size_t capacity() const { return cap; }

	operator string() const;

	void push(char c);
	void push(const void *b, size_t size);
	void push(const string &s) { push(s.c_str(), s.size()); }
	template <class T> void push(T c) { push(&c, sizeof(T)); }

	tFillBuff &operator+=(char c) { push(c); return *this; }
	tFillBuff &operator+=(const string &s) { push(s.c_str(), s.size()); return *this; }
	tFillBuff &operator+=(const tFillBuff &s) { push(s.buf, s.size()); return *this; }

	void fill(char c, size_t size);

	void clear() { idx = 0; }
	string get(size_t sz);
	string peek(size_t sz) const;

	size_t find(const string &s) const;

private:
	void enlarge(size_t toadd);

private:
	char *buf;
	size_t cap;    // размер выделенного буфера
	size_t idx;    // размер данных
	size_t gr;     // размер шага увеличения буфера
};// class tFillBuff




#endif // __tFillBuff_h
