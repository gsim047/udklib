#ifndef __tRecodeCC_h
#define __tRecodeCC_h 1

#include <string>
using std::string;




class tRecodeCC;



class tRecodeCCRef {
	friend class tRecodeCC;

private:
	tRecodeCCRef(const tRecodeCC *_cc, int _idx) : cc(_cc), idx(_idx) {}

public:
	~tRecodeCCRef() {};

	operator char() const; // { return cc._c[idx]; }
	tRecodeCCRef &operator=(char c); // { cc._c[idx] = c; }

private:
	const tRecodeCC *cc;
	int idx;
};// class tRecodeCCRef



class tRecodeCCStr {
	friend class tRecodeCC;

private:
	tRecodeCCStr(const tRecodeCC *_cc, const string& _s) : cc(_cc), s(_s) {}

public:
	~tRecodeCCStr() {};

	operator string() const; // { return cc._c[idx]; }
	tRecodeCCStr &operator=(const string& ss); // { cc._c[idx] = c; }

private:
	const tRecodeCC *cc;
	const string s;
};// class tRecodeCCStr



// class for Recode table char-to-char
// default filling = 'no recode'
class tRecodeCC {
	friend class tRecodeCCRef;
	friend class tRecodeCCStr;
public:
	tRecodeCC();
	tRecodeCC(const tRecodeCC &s);
	tRecodeCC(const char *cc, unsigned size, unsigned pos); // size of recoding area + its starting pos
	virtual ~tRecodeCC() {}

	tRecodeCC &operator=(const tRecodeCC &s);

	bool operator==(const tRecodeCC &s) const;
	bool operator!=(const tRecodeCC &s) const { return !operator==(s); }

	static int c2i(char c) { return int((unsigned char)c); }     // recode one char

	string Recode(const string &s) const;                        // recode full string s
	void RecodeIP(string &s) const;                              // recode-in-place

	tRecodeCCRef operator[](char c) const;                       // use it as:  tRecodeCC cc; char b = cc['a'];
	tRecodeCCStr operator[](const string& s) const;              // use it as:  tRecodeCC cc; char b = cc['a'];
	//RecodeCCRef operator[](char c);

private:
	void init();                                                 // set '1'->'1', 'A'->'A' etc..

public:
	char ch(char c) const { return _c[c2i(c)]; }
	void set(char c1, char c2) { _c[c2i(c1)] = c2; };

public:
	char _c[256];
};// class tRecodeCC




#endif // __tRecodeCC_h
