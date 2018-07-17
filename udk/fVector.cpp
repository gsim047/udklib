#include "fVector.h"
#include <string.h>
#include <stdlib.h>
//#include <memory.h>
#include "fString.h"

#include <algorithm>
#include <functional>
#include <utility>

#include "tFillBuff.h"

#pragma warning(disable:4503)


typedef std::vector<string>::const_iterator vs_citer;
typedef std::map<string, string>::const_iterator mss_citer;
typedef std::map<string, std::vector<string> >::const_iterator msvs_citer;


struct equal_toIC : std::binary_function<string, string, bool> 
{
	bool operator()(const string &s1, const string &s2) const {
		return CompareIC(s1, s2) == 0;
	};
};// struct equal_toIC


size_t FindIC(const std::vector<string> &v, const string &s)
{
	vs_citer it = std::find_if(v.begin(), v.end(),
	                           std::bind1st(equal_toIC(), s));
	if ( it == v.end() )  return (size_t)-1;
	return it - v.begin();
}// FindIC


size_t Columns(const std::vector<std::vector<string> > &v)
{
	size_t col = 0;
	for ( size_t i = 0; i < v.size(); i++ ){
		if ( col < v[i].size() )  col = v[i].size();
	}
	return col;
}// Columns


class __Comp1 {
	size_t n;
public:
	__Comp1(size_t _n = 0) : n(_n) {};
	bool operator()(const std::vector<string> &v1, const std::vector<string> &v2) const {
		return v1[n] < v2[n];
	};
};// class __Comp1

class __Comp2 {
	size_t n;
public:
	__Comp2(size_t _n = 0) : n(_n) {};
	bool operator()(const std::vector<string> &v1, const std::vector<string> &v2) const {
		return v1[n] > v2[n];
	};
};// class __Comp2


void Sort(std::vector<std::vector<string> > &v, bool acc, size_t col)
{
	if ( acc ){
		std::stable_sort(v.begin(), v.end(), __Comp1(col));
	}else{
		std::stable_sort(v.begin(), v.end(), __Comp2(col));
	}
}// Sort


class __CompIC1 {
	size_t n;
public:
	__CompIC1(size_t _n = 0) : n(_n) {};
	bool operator()(const std::vector<string> &v1, const std::vector<string> &v2) const {
		return toUpper(v1[n]) < toUpper(v2[n]);
	};
};// class __CompIC1

class __CompIC2 {
	size_t n;
public:
	__CompIC2(size_t _n = 0) : n(_n) {};
	bool operator()(const std::vector<string> &v1, const std::vector<string> &v2) const {
		return toUpper(v1[n]) > toUpper(v2[n]);
	};
};// class __CompIC2


void SortIC(std::vector<std::vector<string> > &v, bool acc, size_t col)
{
	if ( acc ){
		std::stable_sort(v.begin(), v.end(), __CompIC1(col));
	}else{
		std::stable_sort(v.begin(), v.end(), __CompIC2(col));
	}
}// SortIC


bool ParseCSV(std::vector<string> &list, const string &src, char delim)
{
	list.clear();
	if ( src.empty() )  return true;
	bool pushLast = true;
	string b = "";
	bool init = true;
	bool quota = false;

	for ( size_t i = 0; i < src.size(); i++ ){
		char c = src[i];

		if ( init ){
			if ( c == '\"' ){
				quota = true;
				init = false;
				continue;
			}
			if ( c == delim ){
				list.push_back("");
				continue;
			}
			b += c;
			quota = false;
			init = false;
			continue;
		}

		if ( quota ){
			if ( c == '\"' ){
				if ( i + 1 >= src.size() ){           // +1 ???
					quota = false;
					list.push_back(b);
					pushLast = false;
					continue;
				}
				if ( src[i + 1] == '\"' ){
					b += '\"';
					i++;
					continue;
				}
				if ( src[i + 1] == delim ){
					quota = false;
					list.push_back(b);
					b = "";
					init = true;
					i++;
					continue;
				}
				return false;
			}
			b += c;
			continue;
		}

		if ( c == delim ){
			list.push_back(b);
			b = "";
			init = true;
			continue;
		}

		//if ( c == '\"' )  return false;
		if ( delim == '\"' && c == '\"' )  return false;  
		b += c;
	}
	if ( pushLast )  list.push_back(b);

	return true;
}// ParseCSV


string CellCSV(const string &s, char delim)
{
	string ret = s;
	size_t pos = ret.find_first_of(string(1, delim) + "\"");
	if ( pos != string::npos ){
		for ( pos = ret.find('\"'); pos != string::npos; pos = ret.find('\"', pos + 2) ){
			ret.insert(pos, 1, '\"');
		}
		ret = "\"" + ret + "\"";
	}
	return ret;
}// CellCSV


string ToCSV(const std::vector<string> &src, char delim)
{
	tFillBuff ret(0x4000, 0x4000);
	for ( size_t i = 0; i < src.size(); i++ ){
		ret += CellCSV(src[i], delim);
		if ( i + 1 < src.size() )  ret += delim;
	}
	return ret;
}// ToCSV


std::vector<string> SplitString(const string &str, const string &delim)
{
	std::vector<string> v;
	if ( !str.empty() ){
		size_t i0 = 0;
		for ( size_t i = str.find_first_of(delim);
		      i != string::npos; i = str.find_first_of(delim, i0) ){
			v.push_back(str.substr(i0, i - i0));
			i0 = i + 1;
		}
		v.push_back(str.substr(i0));
	}
	return v;
}// SplitString


std::vector<string> StringExplode(const string &str, const string &delim, size_t count)
{
	std::vector<string> res;
	size_t old_pos = 0, pos;
	while ( count-- != 1 && ( pos = str.find(delim, old_pos) ) != string::npos ) {
		res.push_back(str.substr(old_pos, pos - old_pos));
		old_pos = pos + delim.size();
	}
	res.push_back(str.substr(old_pos));
	return res;
}// StringExplode


static void param(std::map<string, std::vector<string> > &arguments,
                  const string &name,
                  const string &value)
{
	//std::map<string, std::vector<string> >::const_iterator i = arguments.find(name);
	msvs_citer i = arguments.find(name);
	if ( i != arguments.end() && !i->second.empty() ){
		arguments[name].push_back(value);
	}else{
		std::vector<string> arg;
		arg.push_back(value);
		arguments[name] = arg;
	}
}// param

/*
std::map<string, string> WebArgParse(const string &str)
{
	std::map<string, string> arguments;
	std::vector<string> pars = SplitString(str, "&");
	for ( size_t i = 0; i < pars.size(); i++ ){
		if ( pars[i].size() < 2 ) continue;
		size_t p = pars[i].find_first_of('=', 1);
		if ( p != string::npos ){
			string s1 = pars[i].substr(0, p);
			string s2 = pars[i].substr(p + 1);
			arguments[s1] = WebUnEscape(s2);
		}
	}
	return arguments;
}// WebArgParse


std::map<string, std::vector<string> > WebArgParseX(const string &str)
{
	std::map<string, std::vector<string> > arguments;
	std::vector<string> pars = SplitString(str, "&");
	for ( size_t i = 0; i < pars.size(); i++ ){
		if ( pars[i].size() < 2 ) continue;
		size_t p = pars[i].find_first_of('=', 1);
		if ( p != string::npos ){
			string s1 = pars[i].substr(0, p);
			string s2 = pars[i].substr(p + 1);
			param(arguments, s1, WebUnEscape(s2));
		}
	}
	return arguments;
}// WebArgParseX


string WebArgCombine(const std::map<string, string> &arg)
{
	tFillBuff ret(0x4000, 0x4000);
	//for ( std::map<string, string>::const_iterator i = arg.begin();
	for ( mss_citer i = arg.begin();
	      i != arg.end(); ++i ){
		if ( !ret.empty() )  ret += '&';
		ret += i->first;
		ret += '=';
		ret += WebEscape(i->second);
	}
	return ret;
}// WebArgCombine


string WebArgCombine(const std::map<string, std::vector<string> > &arg)
{
	tFillBuff ret(0x4000, 0x4000);
	//for ( std::map<string, std::vector<string> >::const_iterator i = arg.begin();
	for ( msvs_citer i = arg.begin(); i != arg.end(); ++i ){
		for ( size_t j = 0; j < i->second.size(); j++ ){
			if ( !ret.empty() )  ret += '&';
			ret += i->first;
			ret += '=';
			ret += WebEscape(i->second[j]);
		}
	}
	return ret;
}// WebArgCombine
*/

typedef std::pair<string, string> Pair;


static Pair __p1[] = {
	Pair(string("\""), string("&quot;")), 
	Pair(string("&"),  string("&amp;")),   
	Pair(string("<"),  string("&lt;")),
	Pair(string(">"),  string("&gt;")),   
	Pair(string("|"),  string("&brvbar;"))
};// __p1

const xcodeTable ascii2xmlCode(__p1, __p1+5);


static Pair __p2[] = {
	Pair(string("&quot;"),   string("\"")), 
	Pair(string("&amp;"),    string("&")),  
	Pair(string("&lt;"),     string("<")),
	Pair(string("&gt;"),     string(">")),  
	Pair(string("&brvbar;"), string("|")),  
	Pair(string("&nbsp;"),   string("\xA0"))
};// __p2

const xcodeTable xml2asciiCode(__p2, __p2+6);




static Pair __p1u[] = {
	Pair(string("Щ"), string("SHCH")), 
	Pair(string("щ"), string("shch")),
	Pair(string("Ё"), string("YO")), 
	Pair(string("Ж"), string("ZH")), 
	Pair(string("Х"), string("KH")), 
	Pair(string("Ц"), string("TS")),
	Pair(string("Ч"), string("CH")), 
	Pair(string("Ш"), string("SH")), 
	Pair(string("Ю"), string("YU")), 
	Pair(string("Я"), string("YA")),
	Pair(string("ё"), string("yo")), 
	Pair(string("ж"), string("zh")), 
	Pair(string("х"), string("kh")), 
	Pair(string("ц"), string("ts")),
	Pair(string("ч"), string("ch")), 
	Pair(string("ш"), string("sh")), 
	Pair(string("ю"), string("yu")), 
	Pair(string("\xff"), string("ya")),
	Pair(string("А"), string("A")),  
	Pair(string("Б"), string("B")),  
	Pair(string("В"), string("V")),  
	Pair(string("Г"), string("G")),
	Pair(string("Д"), string("D")),  
	Pair(string("Е"), string("E")),  
	Pair(string("З"), string("Z")),  
	Pair(string("И"), string("I")),
	Pair(string("Й"), string("J")),  
	Pair(string("К"), string("K")),  
	Pair(string("Л"), string("L")),  
	Pair(string("М"), string("M")),
	Pair(string("Н"), string("N")),  
	Pair(string("О"), string("O")),  
	Pair(string("П"), string("P")),  
	Pair(string("Р"), string("R")),
	Pair(string("С"), string("S")),  
	Pair(string("Т"), string("T")),  
	Pair(string("У"), string("U")),  
	Pair(string("Ф"), string("F")),
	Pair(string("Ъ"), string("\"")), 
	Pair(string("Ы"), string("Y")),  
	Pair(string("Ь"), string("\'")), 
	Pair(string("Э"), string("E")),
	Pair(string("а"), string("a")),  
	Pair(string("б"), string("b")),  
	Pair(string("в"), string("v")),  
	Pair(string("г"), string("g")),
	Pair(string("д"), string("d")),  
	Pair(string("е"), string("e")),
	Pair(string("з"), string("z")),  
	Pair(string("и"), string("i")),  
	Pair(string("й"), string("j")),  
	Pair(string("к"), string("k")),
	Pair(string("л"), string("l")),  
	Pair(string("м"), string("m")),  
	Pair(string("н"), string("n")),  
	Pair(string("о"), string("o")),
	Pair(string("п"), string("p")),  
	Pair(string("р"), string("r")),  
	Pair(string("с"), string("s")),  
	Pair(string("т"), string("t")),
	Pair(string("у"), string("u")),  
	Pair(string("ф"), string("f")),  
	Pair(string("ъ"), string("\"")), 
	Pair(string("ы"), string("y")),
	Pair(string("ь"), string("\'")), 
	Pair(string("э"), string("e"))
};// __p1u

const xcodeTable cyr2translitUniCode(__p1u, __p1u+66);

static Pair __p2u[] = {
	Pair(string("SHCH"), string("Щ")), 
	Pair(string("Shch"), string("Щ")), 
	Pair(string("shch"), string("щ")), 
	Pair(string("SCH"),  string("Щ")),
	Pair(string("sch"),  string("щ")), 
	Pair(string("YO"),   string("Ё")),   
	Pair(string("Yo"),   string("Ё")),   
	Pair(string("yo"),   string("ё")),
	Pair(string("ZH"),   string("Ж")),   
	Pair(string("Zh"),   string("Ж")),   
	Pair(string("zh"),   string("ж")),   
	Pair(string("KH"),   string("Х")),
	Pair(string("Kh"),   string("Х")),  
	Pair(string("kh"),   string("х")),   
	Pair(string("TS"),   string("Ц")),   
	Pair(string("Ts"),   string("Ц")),
	Pair(string("ts"),   string("ц")),   
	Pair(string("YE"),   string("Э")),   
	Pair(string("Ye"),   string("Э")),   
	Pair(string("ye"),   string("э")),
	Pair(string("CH"),   string("Ч")),   
	Pair(string("Ch"),   string("Ч")),   
	Pair(string("ch"),   string("ч")),   
	Pair(string("SH"),   string("Ш")),
	Pair(string("Sh"),   string("Ш")),   
	Pair(string("sh"),   string("ш")),   
	Pair(string("YU"),   string("Ю")),   
	Pair(string("Yu"),   string("Ю")),
	Pair(string("yu"),   string("ю")),   
	Pair(string("YA"),   string("Я")),   
	Pair(string("Ya"),   string("Я")),   
	Pair(string("ya"),   string("\xff")),
	Pair(string("A"),    string("А")),   
	Pair(string("B"),    string("Б")),   
	Pair(string("V"),    string("В")),   
	Pair(string("G"),    string("Г")),
	Pair(string("D"),    string("Д")),   
	Pair(string("E"),    string("Е")),   
	Pair(string("Z"),    string("З")),   
	Pair(string("I"),    string("И")),
	Pair(string("J"),    string("Й")),   
	Pair(string("K"),    string("К")),   
	Pair(string("L"),    string("Л")),   
	Pair(string("M"),    string("М")),
	Pair(string("N"),    string("Н")),   
	Pair(string("O"),    string("О")),   
	Pair(string("P"),    string("П")),   
	Pair(string("R"),    string("Р")),
	Pair(string("S"),    string("С")),   
	Pair(string("T"),    string("Т")),   
	Pair(string("U"),    string("У")),   
	Pair(string("F"),    string("Ф")),
	Pair(string("Y"),    string("Ы")),   
	Pair(string("a"),    string("а")),   
	Pair(string("b"),    string("б")),   
	Pair(string("v"),    string("в")),
	Pair(string("g"),    string("г")),   
	Pair(string("d"),    string("д")),   
	Pair(string("e"),    string("е")),   
	Pair(string("z"),    string("з")),
	Pair(string("i"),    string("и")),   
	Pair(string("j"),    string("й")),   
	Pair(string("k"),    string("к")),   
	Pair(string("l"),    string("л")),
	Pair(string("m"),    string("м")),   
	Pair(string("n"),    string("н")),   
	Pair(string("o"),    string("о")),   
	Pair(string("p"),    string("п")),
	Pair(string("r"),    string("р")),   
	Pair(string("s"),    string("с")),   
	Pair(string("t"),    string("т")),   
	Pair(string("u"),    string("у")),
	Pair(string("f"),    string("ф")),   
	Pair(string("y"),    string("ы")),   
	Pair(string("\'"),   string("ь")),   
	Pair(string("\""),   string("ъ"))
};// __p2u

const xcodeTable translit2cyrUniCode(__p2u, __p2u+76);

const xcodeTable &cyr2translitCode = cyr2translitUniCode;
const xcodeTable &translit2cyrCode = translit2cyrUniCode;



string Replace(const string &s, const xcodeTable &matrix)
{
	tFillBuff ret(s.size());
	for ( size_t ix = 0; ix < s.size(); ix++ ){
		bool found = false;
		for ( size_t i = 0; i < matrix.size(); i++ ){
			string first = matrix[i].first;
			if ( (s.size() - ix) >= first.size() && 
			     memcmp(s.c_str() + ix, first.c_str(), first.size()) == 0 ){
				ret += matrix[i].second;
				ix += first.size() - 1;
				found = true;
				break;
			}
		}
		if ( found ) continue;
		ret += s[ix];
	}
	return ret;
}// Replace


class __EComp1 {
public:
	bool operator()(const std::pair<string, string> &v1,
	                const std::pair<string, string> &v2) const;
};// class __EComp1

bool __EComp1::operator()(const std::pair<string, string> &v1,
                          const std::pair<string, string> &v2) const
{
	if ( v1.first.size() == v2.first.size() )  return v1.first < v2.first;
	return v1.first.size() >= v2.first.size();
}// __EComp1::operator()


xcodeTable makeCodeTable(const std::map<string, string> &matrix)
{
	xcodeTable mtr;
	//for ( std::map<string, string>::const_iterator i = matrix.begin();
	for ( mss_citer i = matrix.begin(); i != matrix.end(); ++i ){
		mtr.push_back(std::pair<string, string>(i->first, i->second));
	}
	std::sort(mtr.begin(), mtr.end(), __EComp1());
	return mtr;
}// makeCodeTable



int ParseAllCSV(std::vector<std::vector<string> >& res, const string& s, char delim)
{
	int i, n = 0;
	bool is_q = false;
	std::vector<string> raw;
	string cell;
	for ( i = 0; i < s.size(); ){
		char c = s[i];
		if ( !is_q ){          // не в кавычках
			if ( c == ',' ){                               // запятая, конец ячейки
				raw.push_back(cell);
				cell = "";
				i++;
			}else if ( c == '"' ){                         // кавычка, начало закавыченного текста
				is_q = true;
				i++;
			}else if ( c == (char)10 || c == (char)13 ){   // конец ячейки/строки/строки
				char c2 = (c == (char)10) ? (char)13 : (char)10;
				raw.push_back(cell);
				cell = "";
				res.push_back(raw);
				n++;
				raw.clear();
				i++;
				if ( i < s.size() ){
					if ( c2 == s[i] ){
						i++;
					}
				}
			}else{                                         // просто символ
				cell += c;
				i++;
			}
		}else{                 // в кавычках
			if ( c == '"' ){                               // кавычка
				i++;
				if ( i < s.size() ){
					if ( s[i] == '"' ){                        // кавычка в закавыченном тексте
						cell += c;
						i++;
					}else{                                     // конец закавыченного текста
						is_q = false;
					}
				}
			}else{                                         // просто символ
				cell += c;
				i++;
			}
		}
	}
	return n;
}// ParseAllCSV

