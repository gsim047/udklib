#ifndef __fString_h
#define __fString_h 1

#include "fLong.h"

#include <typeinfo>
#include <string>
using std::string;


extern const char * const cs_Space;        // " \t"

extern const char * const cs_AZaz;         // "A-Za-z"
extern const char * const cs_AZaz_;        // "A-Za-z_"
extern const char * const cs_AZaz09;       // "A-Za-z0-9"
extern const char * const cs_AZaz_09;      // "A-Za-z0-9_"
extern const char * const cs_07;           // "0-7"
extern const char * const cs_09;           // "0-9"
extern const char * const cs_09d;          // "0-9."
extern const char * const cs_09dpm;        // "0-9.+-"
extern const char * const cs_09AF;         // "0-9A-F"
extern const char * const cs_09AFaf;       // "0-9A-Fa-f"
extern const char * const cs_ASCII;        // "\0-\xff"
extern const char * const cs_ASCII7;       // "\0-\x7f"

extern const char * const ch_hexValue[256]; // "00", "01", ... "FF"
extern const unsigned ch_charValue[256];    // '0'->0, '9'->9, 'A'->10, 'b'->11

char hex2char(const char *);


bool isSpace(char c);
bool isDigit(char c);
bool isXDigit(char c);
bool isAlpha(char c);
bool isAlphaR(char c);
bool isAlnum(char c);
bool isAlnumR(char c);
bool isLower(char c);
bool isLowerR(char c);
bool isUpper(char c);
bool isUpperR(char c);



string toUpperA7(const string &s);
string toLowerA7(const string &s);

string toUpper(const string &s);
string toLower(const string &s);

string triml(const string &s);        // Trims left spaces
string trimr(const string &s);        // Trims right spaces
string trim(const string &s);         // Trims left & right spaces


string kill_all_of(const string &str, const string &chars);
string kill_all_not_of(const string &str, const string &chars);
string exptab(const string &str, size_t tab = 8); // expands tabs to spaces
string imptab(const string &str, size_t tab = 8);

string ch_shrink(const string &s, char c = ' ');  // ..cccc.. -> ..c..

//string win2unicode(const string &s);
//string unicode2win(const string &s);

string dos2win(const string& s);
string win2dos(const string& s);

string win2utf(const string &s);
string utf2win(const string &s);


int CompareIC(const string &s1, const string &s2); // compare, ignore case
bool operator^=(const string &s1, const string &s2);

// Value -> string. e.g. ToString(200) -> "200", ToString(1, 4) -> "0001"
string ToString(long value, int num = 0);
string ToStringU(unsigned long value, int num = 0);
string ToStringL(longlong n);
string ToStringU64(ulonglong n);

// Value -> string (double). rounded to 2nd digit after digital point
// e.g. ToStringD(2.337) -> "2.34", ToStringD(1.23, 1) -> "1.2"
string ToStringD(double f, int num = 2);

// Value -> string (hex). e.g.ToStringHex(15) -> "F", ToStringHex(17, 3) -> "011"
string ToStringHex(unsigned value, int num = 0);
string ToStringHexL(unsigned long value, int num = 0);

// Value -> string (radix).  2 <= Radix <= 36
string ToStringRadix(long value, unsigned radix = 10, unsigned num = 0, char fillchar = '0');

// Pointer -> string (hex address)
//inline string ToStringP(const void *p) { return ToStringHex((unsigned long)p, 8); }

string ToStringSz(size_t val, int num = 0);
string ToStringHexSz(size_t val, int num = 0);

// Value -> string (hex address)
//inline string ToStringP(unsigned u)  { return ToStringHex(u, 8); }

// String -> int (dec)
int ToInt(const string &s, int defaultValue = 0);
int ToIntO(const string &s, int defaultValue = 0);  // 99 -> dec, 07 -> oct, 0xdf -> hex
unsigned ToUInt(const string &s, unsigned defaultValue = 0);
longlong ToLInt(const string &s, longlong defaultValue = 0);
ulonglong ToUInt64(const string &s);

// String -> int (radix)
int ToIntRadix(const string &s, unsigned radix, int defaultValue = 0);
unsigned ToUIntRadix(const string &s, unsigned radix, unsigned defaultValue = 0);

//longlong ToInt64(const string &s);
ulonglong ToUInt64(const string &s);

// String -> double (dec), if some error returns default value
double ToDouble(const string &s, double defVal = 0.0);

// perl-like empty string , "0", "00" return false
bool ToBool(const string &s);

// Replace all chars 'c1' to 'c2' in string 's'
string Replace(const string &s, char c1, char c2);

// Replace substring n times (if n==0 - all instances)
string Replace(const string &s, const string &sub1, const string &sub2, size_t n = 0);

// Replace any char of 'cc1' to 'c2'
string Replace(const string &str, const string &cc1, char c2);

/*
// Returns substring up to first entry of 'sub'
string Cut(const string &str, const string &sub);

// "ab'c" -> "'ab\'c'"
string Escape(const string &s);

// [ab"c] -> ["ab\"c"]
string Escape2(const string &s);

// [a\\b\"c\'d\0123e\x7Af\99g\t] -> [a\b"c'dSezfcg	]
string UnEscape(const string &s);

// 13 -> "13"
string Quote(long value);

// "abc" -> "'abc'"
string Quote(const string &s);  // Escape() included

// [abc] -> ["abc"]
string Quote2(const string &s); // Escape2() included

inline string &operator<<=(string &s, const string &a) { s = Quote(a); return s; };
inline string &operator<<=(string &s, int i) { s = Quote(i); return s; };

inline int operator>>=(const string &s, int &i) { i = ToInt(s); return i; };
inline unsigned operator>>=(const string &s, unsigned &i) { i = ToInt(s); return i; };
inline bool operator>>=(const string &s, bool &i) { i = (ToInt(s) != 0); return i; };
inline string &operator>>=(const string &s, string &a) { a = s; return a; };

// '+' ?-> ' ', %XX -> '\xXX'
string WebUnEscape(const string &s, bool spaces = true); 
// '+' ?<- ' ', %XX <- '\xXX'  :-)
string WebEscape(const string &s, bool spaces = true); 
*/
string DoQuote(const string &s, const string &s_before, const string &s_after);
bool Quoted(const string &s, const string &s_before, const string &s_after);
bool UnQuote(string &s, const string &s_before, const string &s_after);


// Reverse string
string Reverse(const string &str);


// Counts number of substrings 'sub'
size_t Count(const string &str, const string &sub);

template <class Predicate> size_t Count_if(const string &str, Predicate pred)
{
	size_t n = 0;
	for ( size_t i = 0; i < str.size(); i++ )  if ( pred(str[i]) ) n++;
	return n;
}// Count_if

// Counts words in string
size_t CountWords(const string &str, const string &delim = cs_09);

// Returns word number 'num' from string
string GetWord(const string &str, size_t num, const string &delim = cs_09);

// Converts int to a string
string int2str(int digital, unsigned radix = 10);


string BufDump(const void *buf, size_t size);
inline string BufDump(const string &str) { return BufDump(str.c_str(), str.size()); };

size_t find_end_qstring(const string &checked_str, char br = '\"');

string hex2str(const string &s); // "417A3031" --> "Az01"
string str2hex(const string &s); // "Az01" --> "417A3031"

//string getTypeName(const string &s);

std::string demangleTypeName(const char* name);

template <class T>
string getTypeName(const T& t) {
    return demangleTypeName(typeid(t).name());
}


size_t sprintf(string &s, const char *fmt, ...);


bool strMatch(const string &mask, const string &str); // [Aa9X? ]

bool isFileMask(const string &mask);
bool FileMatch(const string &mask, const string &fname);

char *c_dup(const string &s);


int isUnicode(const string &s);   // begins with "\xFF\xFE" -> 0, "\xFE\xFF" -> 1, else -> -1
void __uc_swapBytes(string &s);


int detectTextCP(const string &s);	// Unknown:-1, DOS:0, KOI:1, WIN:2, ISO:3, MAC:4, UTF:5, UNI:6


size_t findIC(const string &s, const string &sub, size_t pos = 0); // ~= string.find(...)


#endif // __fString_h

