#include "tStrHash.h"
//#include "fDprintf.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>




static string rs(const string &str, const string &old, const string &new_str)
{
	string ret(str);
	size_t pos = 0;
	for (;;){
		size_t it = ret.find(old, pos);
		if ( it == string::npos ) break;
		ret.replace(it, old.size(), new_str);
		pos = it + new_str.size();
	}
	return ret;
}// rs


static std::vector<string> ss(const string &str, const string &delim)
{
	std::vector<string> v;
	size_t i0 = 0;
	for ( size_t i = str.find_first_of(delim);
	      i != string::npos; i = str.find_first_of(delim, i0) ){
		v.push_back(str.substr(i0, i - i0));
		i0 = i + 1;
	}
	v.push_back(str.substr(i0));
	return v;
}// ss


static string encode(const string &str)
{
	if ( str.find_first_of("%&") == string::npos ) return str;
	string ret = rs(str, "%", "%25");
	ret = rs(ret, "&", "%26");
	ret = rs(ret, "=", "%3D");
	return ret;
}// encode


static string decode(const string &str)
{
	if ( str.find_first_of("%") == string::npos ) return str;
	string ret = rs(ret, "%3D", "=");
	ret = rs(ret, "%26", "&");
	ret = rs(ret, "%25", "%");
	return ret;
}// decode


static int __ToInt(const string &s, int defVal)
{
	if ( s.empty() ) return defVal;
	char *p = NULL;
	int ret = strtol(s.c_str(), &p, 10);
	if ( !p || p != s.c_str() + s.size() ) return defVal;
	return ret;
}// __ToInt




tStrHash::tStrHash(const string &src)
{
	std::vector<string> v = ss(src, "&");
	for ( size_t i = 0; i < v.size(); i++ ){
		std::vector<string> d = ss(v[i], "=");
		if ( d.size() > 0 ){
			d[0] = decode(d[0]);
			if ( d.size() == 1 ){
				h[d[0]] = "";
			}else{
				h[d[0]] = decode(d[1]);
				for ( size_t j = 2; j < d.size(); j++ ) h[d[0]] += "=" + decode(d[j]);
			}
		}
	}
}// tStrHash::tStrHash


string tStrHash::toString() const
{
	string ret;
	bool was = false;
	for ( std::map<string, string>::const_iterator i = h.begin();
	      i != h.end(); ++i ){
		if ( was ) ret += "&";
		ret += encode(i->first) + "=" + encode(i->second);
		was = true;
	}
	return ret;
}// tStrHash::toString


std::map<string, string> tStrHash::toMap() const
{
	std::map<string, string> ret = h;
	return ret;
}// tStrHash::toMap


string tStrHash::operator[](const string &key) const
{
	std::map<string, string>::const_iterator i = h.find(key);
	if ( i == h.end() ) return "";
	return i->second;
}// tStrHash::operator[]


string tStrHash::get(const string &key, const string &def) const
{
	std::map<string, string>::const_iterator i = h.find(key);
	if ( i == h.end() ) return def;
	return i->second;
}// tStrHash::get


int tStrHash::get(const string &key, int def) const
{
	std::map<string, string>::const_iterator i = h.find(key);
	if ( i == h.end() ) return def;
	return __ToInt(i->second, def);
}// tStrHash::get


void tStrHash::set(const string &key, int value)
{
	char str[40];
	sprintf(str, "%d", value);
	h[key] = string(str);
}// tStrHash::set


