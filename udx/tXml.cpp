#include "tXml.h"
#include "fString.h"
#include "fVector.h"
#include <algorithm>
#include "tFile.h"
//#include <iostream.h>



static const char * const cs_SpaceAll = " \t\r\n";

static string TrimAll(const string &s)
{
	string ret;
	size_t pos1 = s.find_first_not_of(cs_SpaceAll);
	if ( pos1 != string::npos ){
		size_t pos2 = s.find_last_not_of(cs_SpaceAll);
		ret = s.substr(pos1, pos2 - pos1 + 1);
	}
	return ret;
}// TrimAll


tTag::tTag() :
	//owner(NULL),
	parent(NULL),
	type(UNKNOWN)
{
}// tTag::tTag


tTag::tTag(const string &str) :
	//owner(NULL),
	parent(NULL),
	type(UNKNOWN)
{
	size_t pos = 0;
	process(str, pos);
}// tTag::tTag


tTag::tTag(int _type, const string &val) :
	//owner(NULL),
	parent(NULL),
	type(_type)
{
	switch ( type ){
	case XML:
		value = "?xml";
		break;

	case TEXT:
	case TAG:
	case COMMENT:
		value = val;
		break;

	default:
		type = UNKNOWN;
	}
}// tTag::tTag

/*
tTag::tTag(int _type, const string &val, const std::map<string, string> &pars) :
	parent(NULL),
	type(_type)
{
	switch ( type ){
	case XML:
		value = "?xml";
		params = pars;
		break;

	case TEXT:
		value = val;
		break;

	case TAG:
		value = val;
		params = pars;
		break;

	case COMMENT:
		value = val;
		break;
	}
}// tTag::tTag
*/

tTag::tTag(const tTag &src) :
	//owner(NULL),
	parent(NULL),
	value(src.value),
	params(src.params),
	type(src.type)
{
	if ( type == TAG ){
		for ( size_t i = 0; i < src.tags.size(); i++ ){
			addTag(new tTag(*(src.tags[i])));
		}
	}
}// tTag::tTag


tTag::~tTag()
{
	clear();
}// tTag::~tTag


void tTag::clear()
{
	for ( size_t i = 0; i < tags.size(); i++ ){
		delete tags[i];
	}
	tags.clear();
	value = "";
	params.clear();
	ind.clear();
	type = UNKNOWN;
	parent = NULL;
}// tTag::clear


string tTag::TypeName(unsigned Type)
{
	switch ( Type ){
	case UNKNOWN: return "UNKNOWN";
	case TEXT:    return "TEXT";
	case TAG:     return "TAG";               
	case XML:     return "XML";
	case COMMENT: return "COMMENT";
	case DOCTYPE: return "DOCTYPE";
	}
	return "Bad type!";
}// tTag::TypeName


tTag &tTag::operator=(const tTag &src)
{
	if ( this != &src ){
		if ( parent )  parent->delTag(this);
		parent = NULL;
		value = src.value;
		type = src.type;
		params = src.params;
		for ( size_t i = 0; i < tags.size(); i++ )  delete tags[i];
		if ( type == TAG ){
			for ( size_t i = 0; i < src.tags.size(); i++ ){
				addTag(new tTag(*(src.tags[i])));
			}
		}
	}
	return *this;
}// tTag::operator=


void tTag::delTag(tTag *x)
{
	if ( x ){
		std::vector<tTag *>::iterator i = std::find(tags.begin(), tags.end(), x);
		if ( i != tags.end() ) tags.erase(i);
		x->parent = NULL;
	}
}// tTag::delTag


void tTag::addParam(const string &key, const string &value)
{
	params[key] = value;
	if ( std::find(ind.begin(), ind.end(), key) == ind.end() )  ind.push_back(key);
}// tTag::addParam


void tTag::delParam(const string &key)
{
	params.erase(key);
	std::vector<string>::iterator i = std::find(ind.begin(), ind.end(), key);
	if ( i != ind.end() )  ind.erase(i);
}// tTag::delParam


string tTag::operator[](const string &key) const
{
	string ret;
	std::map<string, string>::const_iterator i = params.find(key);
	if ( i != params.end() )  ret = i->second;
	return ret;
}// tTag::operator[]


string tTag::operator[](size_t n) const
{
	string ret;
	if ( n < ind.size() )  ret = operator[](ind[n]);
	return ret;
}// tTag::operator[]


tTag *tTag::tag(const string &name) const
{
	for ( size_t i = 0; i < tags.size(); i++ ){
		if ( tags[i]->isTAG(name) )  return tags[i];
	}
	return NULL;
}// tTag::tag


string tTag::toString() const
{
	string ret;
	switch ( type ){
	case TEXT:
		ret = value + "\n";
		break;

	case TAG:
		ret = "<" + value;
		{
			for ( size_t i = 0; i < paramsize(); i++ ){
				ret += " " + ind[i] + "=\"" + Replace(operator[](i), ascii2xmlCode) + "\"";
			}
		}
		if ( tags.empty() ){
			ret += "/>\n";
		}else{
			ret += ">\n";
			for ( size_t i = 0; i < tags.size(); i++ ){
				string str = " " + tags[i]->toString();
				ret += Replace(str, "\n", "\n ");
				if ( ret[ret.size() - 1] == ' ' ) ret.resize(ret.size() - 1);
			}
			ret += "</" + value + ">\n";
		}
		break;

	case XML:
		{
			ret = "<?xml";
			for ( size_t i = 0; i < paramsize(); i++ ){
				ret += " " + ind[i] + "=\"" + Replace(operator[](i), ascii2xmlCode) + "\"";
			}
			/*
			std::map<string, string>::const_iterator i = params.find("version");
			if ( i != params.end() ){
				ret += " version=\"" + EscapeString(i->second, xmlCode) + "\"";
			}
			for ( std::map<string, string>::const_iterator i = params.begin();
			       i != params.end(); ++i ){
				if ( i->first != "version" ){
					ret += " " + i->first + "=\"" + EscapeString(i->second, xmlCode) + "\"";
				}
			}
			*/
			ret += "?>\n";
		}
		break;

	case COMMENT:
		ret = "<!--" + value + "-->\n";
		break;
	}
	return ret;
}// tTag::toString


static bool eatBlank(const string &str, size_t &pos)
{
	//cout << "eatB: " << str << ", " << pos << endl;
	size_t pos2 = str.find_first_not_of(cs_SpaceAll, pos);
	if ( pos != pos2 ){
		pos = pos2;
		return true;
	}
	return false;
}// eatBlank


static string eatString(const string &str, const string &delim, size_t &pos)
{
	//cout << "eatS: " << str << ", " << pos << endl;
	eatBlank(str, pos);
	size_t pos2 = str.find_first_of(delim, pos);
	string ret;
	if ( pos != pos2 ){
		ret = TrimAll(str.substr(pos, pos2 - pos));
		pos = pos2;
	}
	//cout << "eatS: [" << ret << "]" << endl;
	//cout << "eatString [" << ret << "]" << endl;
	return ret;
}// eatBlank


bool tTag::getParams(const string &str)
{
	//cout << "getParams ";
	//std::map<string, string> ret;
	size_t pos = 0;
	while ( pos < str.size() ){
		string key = eatString(str, "= \n\r\t", pos);
		if ( key.empty() || pos == string::npos ) break;
		eatBlank(str, pos);
		//cout << "gp1: pos=" << pos << endl;
		if ( pos > str.size() || str[pos] != '=' ) break;
		pos++;
		//cout << "gp2: pos=" << pos << endl;
		eatBlank(str, pos);
		//cout << "gp3: pos=" << pos << endl;

		char c = str[pos];
		if ( pos > str.size() || (c != '\"' && c != '\'') ) break;
		pos++;
		//cout << "gp4: pos=" << pos << endl;
		if ( pos > str.size() ) break;
		string value = eatString(str, string(1, c), pos);
		if ( pos == string::npos ) break;
		pos++;
		//cout << "gp5: pos=" << pos << endl;
		addParam(key, value);
	}
	return true;
}// tTag::getParams


bool tTag::eatXML(const string &str, size_t &pos)
{
	if ( str.substr(pos, 6) != "<?xml " ) return false;
	size_t pos2 = str.find("?>", pos + 6);
	if ( pos2 == string::npos ) return false;
	type = XML;
	value = "?xml";
	//params =
	getParams(str.substr(pos + 6, pos2 - pos - 6));
	pos = pos2 + 2;
	//cout << "XML" << endl;
	return true;
}// tTag::eatXML


bool tTag::eatComment(const string &str, size_t &pos)
{
	if ( str.substr(pos, 4) != "<!--" ) return false;
	size_t pos2 = str.find("-->", pos + 4);
	if ( pos2 == string::npos ) return false;
	type = COMMENT;
	value = str.substr(pos + 4, pos2 - pos - 4);
	pos = pos2 + 3;
	//cout << "Comment [" << value << "]" << endl;
	return true;
}// tTag::eatComment


bool tTag::eatTag(const string &str, size_t &pos)
{
	if ( str[pos] != '<' ) return false;
	//cout << "Tag :" << endl;
	size_t pos2 = pos + 1;
	value = eatString(str, " \n\r\t/>", pos2);
	string endval = "</" + value + ">";
	if ( value.empty() ) return false;
	type = TAG;
	size_t pos3 = str.find(">", pos2);
	if ( pos3 == string::npos ) return false;
	//params =
	getParams(str.substr(pos2, pos3 - pos2));
	if ( str[pos3 - 1] == '/' ){
		pos = pos3 + 1;
		return true;
	}
	pos3++;
	for (;;){
		eatBlank(str, pos3);
		if ( pos3 >= str.size() ) return false;
		//cout << "Tag end (" << value << ")? [" << str.substr(pos3, endval.size()) << "]" << endl;
		if ( str.substr(pos3, endval.size()) == endval ){
			//cout << "Tag end (" << value << ")!" << endl;
			break;
		}
		tTag *next = new tTag();
		if ( !next->process(str, pos3) ){
			delete next;
			return false;
		}
		tags.push_back(next);
	}
	pos3 += endval.size();
	pos = pos3;
	return true;
}// tTag::eatTag


bool tTag::process(const string &str, size_t &pos)
{
	//if ( pos >= str.size() ) return false;
	eatBlank(str, pos);
	if ( pos >= str.size() ){
		return false;
	}
	if ( str[pos] == '<' ){                       // tag, xml, comment
		if ( eatComment(str, pos) ){
			//cout << "Comment !" << endl;
			return true;
		}
		if ( eatXML(str, pos) ){
			//cout << "XML !" << endl;
			return true;
		}
		//bool ret = eatTag(str, pos);
		//if ( ret ){
			//cout << "Tag !" << endl;
		//}else{
			//cout << "process error !" << endl;
		//}
		//return ret;
		return eatTag(str, pos);
	}else{                                     // plain-text
		type = TEXT;
		size_t pos2 = str.find("<", pos + 1);
		if ( pos2 == string::npos ){
			value = TrimAll(str.substr(pos));
			pos = pos2;
		}else{
			value = TrimAll(str.substr(pos, pos2 - pos));
			pos = pos2;
		}
	}
	//cout << "String !" << endl;
	return true;
}// tTag::process


string tTag::getTagPath() const
{
	string path;
	if ( parent )  path = parent->getTagPath();
	if ( isTAG() ){
		if ( !path.empty() )  path += "/";
		path += value;
	}
	return path;
}// tTag::getTagPath




tXML::tXML()
{
	tTag *xml = new tTag(tTag::XML, "");
	xml->addParam("version", "1.0");
	xml->addParam("encoding", "WINDOWS-1251");
	addTag(xml);
}// tXML::tXML


tXML::tXML(const string &str)
{
	size_t pos = 0;
	tTag *x = new tTag();
	if ( x->process(str, pos) ){
		if ( !x->isXML() ){
			tTag *xml = new tTag(tTag::XML, "");
			xml->addParam("version", "1.0");
			xml->addParam("encoding", "WINDOWS-1251");
			addTag(xml);
			addTag(x);
		}else{
			addTag(x);
			x = new tTag();
			if ( x->process(str, pos) ){
				addTag(x);
			}else{
				delete x;
			}
		}
	}else{
		delete x;
	}
}// tXML::tXML


tXML::tXML(const tXML &src)
{
	for ( size_t i = 0; i < src.tags.size(); i++ ){
		addTag(new tTag(*(src.tags[i])));
	}
}// tXML::tXML


tXML::~tXML()
{
	clear();
}// tXML::~tXML


void tXML::clear()
{
	for ( size_t i = 0; i < tags.size(); i++ ){
		delete tags[i];
	}
	tags.clear();
}// tXML::clear


tXML &tXML::operator=(const tXML &src)
{
	if ( this != &src ){
		clear();

		for ( size_t i = 0; i < src.tags.size(); i++ ){
			addTag(new tTag(*(src.tags[i])));
		}
	}
	return *this;
}// tXML::operator=


void tXML::delTag(tTag *x)
{
	if ( x ){
		std::vector<tTag *>::iterator i = std::find(tags.begin(), tags.end(), x);
		if ( i != tags.end() ) tags.erase(i);
		x->parent = NULL;
	}
}// tXML::delTag


string tXML::toString() const
{
	string ret;
	for ( size_t i = 0; i < tags.size(); i++ ){
		ret += tags[i]->toString();
	}
	return ret;
}// tXML::toString


tTag *tXML::tag(const string &name) const
{
	for ( size_t i = 0; i < tags.size(); i++ ){
		if ( tags[i]->isTAG(name) )  return tags[i];
	}
	return NULL;
}// tXML::tag


bool tXML::load(const string &fname)
{
	std::vector<string> list;
	if ( !tRTFile::LoadFile(list, fname) )  return false;
	string s;
	for ( size_t i = 0; i < list.size(); i++ ){
		s += list[i] + "\n";
	}
	*this = tXML(s);
	return true;
}// tXML::load


bool tXML::save(const string &fname) const
{
	tFILE f(fname, "wt");
	if ( !f.isOpen() )  return false;
	f.write(toString());
	return true;
}// tXML::save

