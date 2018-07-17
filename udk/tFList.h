#ifndef __tFList_h
#define __tFList_h 1

#pragma warning(disable:4786)

#include <string>
#include <vector>
using std::string;




class tFileAttr {
public:

#ifdef _WIN32 
	enum{
		R      = 0x01,
		H      = 0x02,
		S      = 0x04,
		D      = 0x10,
		A      = 0x20,
		MASK   = 0x37
	};
#endif

#ifdef __GNUG__
	enum{
		oX      = 0x001,
		oW      = 0x002,
		oR      = 0x004,
		gX      = 0x008,
		gW      = 0x010,
		gR      = 0x020,
		uX      = 0x040,
		uW      = 0x080,
		uR      = 0x100,
		D       = 0x200,
		MASK    = 0x3FF
	};
#endif

public:
	tFileAttr() : attr(0), cre(0), mod(0), size(0) {};
	tFileAttr(const string &name, unsigned attr, size_t size, unsigned cre = 0, unsigned mod = 0);
	tFileAttr(const tFileAttr &s);
	virtual ~tFileAttr() {};

	tFileAttr &operator=(const tFileAttr &s);
	bool operator==(const tFileAttr &s) const;
	bool operator!=(const tFileAttr &s) const { return !operator==(s); }

	string name;
	unsigned attr;
	size_t size;
	unsigned cre;
	unsigned mod;
};// class tFileAttr




class tFList{
public:
	enum{
		SORT_NO       = 0,
		SORT_NAME     = 1,
		SORT_EXT      = 2,

		SORT_MAXMODE  = 2
	};

	enum{
#ifdef _WIN32
		casesense  = false
#else
		casesense  = true
#endif
	};

public:
	tFList() : _sort(SORT_NO) {};
	tFList(const string &path, bool r);
	tFList(const tFList &s) : list(s.list), _sort(s._sort) {};

	tFList &operator=(const tFList &);

	bool operator==(const tFList &s) const;
	bool operator!=(const tFList &s) const { return !operator==(s); }

	bool operator+=(const string &);
	bool operator-=(const string &);

	tFList &operator+=(const tFList &s);
	tFList &operator-=(const tFList &s);
	tFList &operator|=(const tFList &s) { return operator+=(s); }
	tFList &operator&=(const tFList &s);

	string operator[](size_t n) const;
	size_t size() const { return list.size(); }
	string name(size_t n) const;
	bool empty() const { return list.empty(); }

	size_t find(const string &fname, bool cs = casesense) const;

	bool sort(unsigned mode = SORT_NAME, bool cs = casesense);
	tFList selectPath(const string &path, bool cs = casesense) const;
	tFList selectMask(const string &mask, bool cs = casesense) const;
	tFList selectFirst(const string &mask, bool cs = casesense) const;

	void dropPath(string Path, bool cs = casesense);

private:
	std::vector<string> list;
	unsigned _sort;
};// class tFList

/*
tFList operator+(const tFList &a, const tFList &b)
{
	tFList c = a; return (c += b);
}// operator+

tFList operator-(const tFList &a, const tFList &b)
{
	tFList c = a; return (c -= b);
}// operator-

tFList operator&(const tFList &a, const tFList &b)
{
	tFList c = a; return (c &= b);
}// operator&

tFList operator|(const tFList &a, const tFList &b)
{
	tFList c = a; return (c |= b);
}// operator|
*/



class tFIList{
public:
	enum{
		SORT_NO       = 0,
		SORT_NAME     = 1,
		SORT_EXT      = 2,
		SORT_SIZE     = 3,
		SORT_DATE     = 4,
		SORT_DATE_M   = 5,

		SORT_MAXMODE  = 5
	};

	enum{
#ifdef _WIN32
		casesense  = false
#else
		casesense  = true
#endif
	};

public:
	tFIList() : _sort(SORT_NO) {};
	tFIList(const string &path, bool r);
	tFIList(const tFIList &s) : list(s.list), _sort(s._sort) {};

	tFIList &operator=(const tFIList &);

	bool operator==(const tFIList &s) const;
	bool operator!=(const tFIList &s) const { return !operator==(s); }

	bool operator+=(const tFileAttr &);
	bool operator-=(const tFileAttr &);

	tFIList &operator+=(const tFIList &s);
	tFIList &operator-=(const tFIList &s);
	tFIList &operator|=(const tFIList &s) { return operator+=(s); }
	tFIList &operator&=(const tFIList &s);

	tFileAttr operator[](size_t n) const;
	size_t size() const { return list.size(); }
	//string name(size_t n) const;

	size_t find(const string &fname, bool cs = casesense) const;

	bool sort(unsigned mode = SORT_NAME, bool cs = casesense);
	tFIList selectPath(const string &path, bool cs = casesense) const;
	tFIList selectMask(const string &mask, bool cs = casesense) const;
	tFIList selectAttr(unsigned attr) const;


private:
	std::vector<tFileAttr> list;
	unsigned _sort;
};// class tFIList




#endif // __tFList_h 1
