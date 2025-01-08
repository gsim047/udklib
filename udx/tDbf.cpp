#include "tDbf.h"
#include <fString.h>
#include <fTime.h>
#include <fFile.h>
#include <string.h>
#include <stdlib.h>

#pragma pack(push, 1)


class dbfs_Header{
public:
	unsigned char type;
	unsigned char date[3];
	unsigned nrec;
	unsigned short recaddr;
	unsigned short reclen;
	char res1[16];
	unsigned char foxopt;
	unsigned char codepage;
	char res2[2];                   // === '\0', '\0'


	dbfs_Header();
	dbfs_Header(string &buf) { fill(buf); };

	bool fill(string &buf);
	string get() const;
};// class dbfs_Header

#pragma pack(pop)



dbfs_Header::dbfs_Header()
{
	memset(this, 0, sizeof(dbfs_Header));
	type = tDbf::DBASE3;
	tTimeS ts;
	date[0] = (unsigned char)ts.year;
	date[1] = (unsigned char)ts.month;
	date[2] = (unsigned char)ts.day;
}// dbfs_Header::dbfs_Header

bool dbfs_Header::fill(string &buf)
{
	if ( buf.size() < sizeof(dbfs_Header) ) return false;
	memmove(this, buf.c_str(), sizeof(dbfs_Header));
	buf.erase(0, sizeof(dbfs_Header));
	return true;
}// dbfs_Header::fill

string dbfs_Header::get() const
{
	string ret((char *)this, sizeof(dbfs_Header));
	return ret;
}// dbfs_Header::get



#pragma pack(push, 1)

class dbfs_RecInfo{
public:
	char name[11];
	unsigned char type;             // C    Character (символьный)
																																	// N    Numeric (числовой)
																																	// D    Date (дата)
																																	// I    Integer (целый)
																																	// L    Logical (логический)
																																	// M    Memo
	unsigned offset;                // Расположение поля внутри записи (смещение)
	unsigned char len;              // Длина поля (в байтах)
	unsigned char dec;              // Длина дробной части в байтах
	unsigned char flag;
	char res[13];


	dbfs_RecInfo() { memset(this, 0, sizeof(dbfs_RecInfo)); };
	dbfs_RecInfo(string &buf) { fill(buf); };

	bool fill(string &buf);
	string get() const;
};// class dbfs_RecInfo

#pragma pack(pop)


bool dbfs_RecInfo::fill(string &buf)
{
	if ( buf.size() < sizeof(dbfs_RecInfo) ) return false;
	memmove(this, buf.c_str(), sizeof(dbfs_RecInfo));
	buf.erase(0, sizeof(dbfs_RecInfo));
	return true;
}// dbfs_RecInfo::fill

string dbfs_RecInfo::get() const
{
	string ret((char *)this, sizeof(dbfs_RecInfo));
	return ret;
}// dbfs_RecInfo::get




tDbf::tDbf() :
	Hi(new dbfs_Header())
{
}// tDbf::tDbf


tDbf::tDbf(const string &_fname) :
	Hi(new dbfs_Header())
{
	if ( FileExists(_fname) ) load(_fname);
}// tDbf::tDbf


bool tDbf::load(const string &fn)
{
	tFILE fi(fn, "rb");
	if ( !fi.isOpen() )  return false;

	string buf = fi.read(fi.size());
	if ( !Hi->fill(buf) ){
		//nError::Throw("Error DBF header reading");
		return false;
	}
	for (;;){
		if ( buf.empty() ){
			//nError::Throw("Bad DBF fields structure");
			return false;
		}
		if ( buf[0] == (char)EORi ) break;
		//printf("char [%02X], buf size=%d\n", (unsigned char)buf[0], buf.size());

		dbfs_RecInfo *RiN = new dbfs_RecInfo();
		if ( !RiN->fill(buf) ){
			//nError::Throw("Error DBF field info reading");
			return false;
		}
		Ri.push_back(RiN);
	}
	for ( unsigned i = 0; i < size(); i++ ){
		link.push_back(new tDbfRec(*this, getRec(fi, i))); 
	}
	return true;
}// tDbf::load


bool tDbf::save(const string &fn)
{
	tFILE fi(fn, "wb");
	if ( !fi.isOpen() )  return false;

	if ( !fi.write(Hi->get()) ) return false;
	{
		for ( unsigned i = 0; i < Ri.size(); i++ ){
			if ( !fi.write(Ri[i]->get()) ) return false;
		}
	}
	if ( !fi.write("\x0d") ) return false;
	if ( !fi.write(string(1, '\0')) ) return false;

	{
		for ( unsigned i = 0; i < size(); i++ ){
			if ( !fi.write(link[i]->rec()) ) return false;
		}
	}
	if ( !fi.write("\x1a") ) return false;
	return true;
}// tDbf::save


tDbf::~tDbf()
{
	{
		for ( unsigned i = 0; i < Ri.size(); i++ ){
			if ( Ri[i] ) delete Ri[i];
		}
	}
	if ( Hi ) delete Hi;
	//printf("~tDbf : link.size() = %d\n", link.size());
	{
		for ( unsigned i = 0; i < link.size(); i++ ){
			if ( link[i] )  delete link[i];
		}
	}
}// tDbf::~tDbf


unsigned tDbf::size() const
{
	return Hi->nrec;
}// tDbf::size


string tDbf::getDate() const
{
	char buf[256];
	sprintf(buf, "%04u-%02u-%02u",
	        1900 + (unsigned)Hi->date[0], (unsigned)Hi->date[1], (unsigned)Hi->date[2]);
	return string(buf);
}// tDbf::getDate


unsigned tDbf::recsize() const
{
	return Hi->reclen;
}// tDbf::recsize


unsigned tDbf::recaddr(unsigned n) const
{
	if ( n >= size() )  return (unsigned)-1;
	return n * recsize() + Hi->recaddr;
}// tDbf::recaddr


string tDbf::getRec(tFILE &fi, unsigned n)
{
	if ( n >= size() )  return "";
	fi.seek(recaddr(n));
	return fi.read(recsize());
}// tDbf::getRec


bool tDbf::putRec(tFILE &fi, unsigned n, const string &rec)
{
	if ( n >= size() ) return false;
	if ( rec.size() != recsize() ) return false;
	fi.seek(recaddr(n));
	return fi.write(rec);
}// tDbf::putRec


string tDbf::fieldName(unsigned n) const
{
	if ( n >= Ri.size() ) return "";
	return string(Ri[n]->name);
}// tDbf::fieldName


string tDbf::fieldType(unsigned n) const
{
	if ( n >= Ri.size() ) return "";
	return string(1, (char)Ri[n]->type);
}// tDbf::fieldType


string tDbf::fieldTypeName(unsigned n) const
{
	if ( n >= Ri.size() ) return "";
	string ret(1, (char)Ri[n]->type);
	ret += ToString(Ri[n]->len);
	if ( (char)Ri[n]->type == 'N' ) ret += "." + ToString(Ri[n]->dec);
	return ret;
}// tDbf::fieldTypeName


unsigned tDbf::fieldSize(unsigned n) const
{
	if ( n >= Ri.size() ) return 0;
	return Ri[n]->len;
}// tDbf::fieldSize


unsigned tDbf::fieldDec(unsigned n) const
{
	if ( n >= Ri.size() ) return 0;
	if ( (char)Ri[n]->type != 'N' ) return 0;
	return Ri[n]->dec;
}// tDbf::fieldDec

/*
void tDbf::addLink(tDbfRec *rec)
{
	if ( rec ){
		link.push_back(rec);
		//printf("link++ -> %d\n", link.size());
	}
}// tDbf::addLink


void tDbf::killLink(tDbfRec *rec)
{
	std::vector<tDbfRec *>::iterator i = std::find(link.begin(), link.end(), rec);
	if ( i != link.end() ){
		link.erase(i);
		//printf("link-- -> %d\n", link.size());
	}
}// tDbf::killLink
*/

tDbfRec *tDbf::get(unsigned n) const
{
	if ( n >= size() ) return NULL;
	return link[n];
}// tDbf::get

/*
bool tDbf::set()
{
	if ( !cur || cur->num == (unsigned)-1 ) return false;
	return putRec(cur->num, cur->rec());
}// tDbf::set
*/

tDbfRec *tDbf::add()
{
	tDbfRec *rec = new tDbfRec(*this);
	link.push_back(rec);
	Hi->nrec += 1;
	return rec;
}// tDbf::add


string tDbf::fieldDefault(const string &fieldType)
{
	if ( !fieldType.empty() ){
		switch ( fieldType[0] ){
		//case 'C': return "";
		case 'N': return "0";
		case 'L': return "F";
		case 'D': return "    -  -  ";
		//case 'M': return "";
		}
	}
	return "";
}// tDbf::fieldDefault


string tDbf::fieldDefault(unsigned n) const
{
	return fieldDefault(fieldType(n));
}// tDbf::fieldDefault


void tDbf::flush()
{
	//cout << "tDbf::flush 1" << endl;
	for ( unsigned n = 0; n < link.size(); n++ )  delete link[n];
	link.clear();
	Hi->nrec = 0;

	Hi->recaddr = sizeof(dbfs_Header) + sizeof(dbfs_RecInfo) * Ri.size() + 2;
	Hi->reclen = 1;
	for ( unsigned i = 0; i < Ri.size(); i++ )  Hi->reclen += Ri[i]->len;
}// tDbf::flush


bool tDbf::delField(const string &name)
{
	for ( unsigned i = 0; i < fields(); i++ ){
		if ( name == fieldName(i) ){
			std::vector<dbfs_RecInfo *>::iterator it = Ri.begin() + i;
			delete Ri[i];
			Ri.erase(it);
			flush();
			return true;
		}
	}
	return false;
}// tDbf::delField


bool tDbf::delField(unsigned n)
{
	//cout << "tDbf::delField " << n << endl;
	if ( n >= fields() ) return false;
	std::vector<dbfs_RecInfo *>::iterator it = Ri.begin();
	it += n;
	delete Ri[n];
	Ri.erase(it);
	flush();
	//cout << "tDbf::delField ok" << endl;
	return true;
}// tDbf::delField


bool tDbf::addField(string name, const string &type, unsigned _size, unsigned dec)
{
	if ( name.empty() ) return false;
	if ( name.size() > 11 ) name.resize(11);
	name = toUpper(name);
	for ( unsigned i = 0; i < size(); i++ ){

		if ( name == fieldName(i) ) return false;
	}
	if ( type.empty() ) return false;
	switch ( type[0] ){
	case 'C': case 'N': case 'L': case 'D': case 'M':
		break;
	default:
		return false;
	}
	dbfs_RecInfo *RiN = new dbfs_RecInfo();
	Ri.push_back(RiN);
	memmove(RiN, name.c_str(), name.size());
	RiN->type = type[0];
	RiN->len = _size;
	RiN->dec = dec;
	flush();
	return true;
}// tDbf::addField




tDbfRec::tDbfRec(tDbf &owner) :
	dbf(&owner),
	_del(false)
{
	fill("");
}// tDbfRec::tDbfRec


tDbfRec::tDbfRec(tDbf &owner, const string &str) :
	dbf(&owner),
	_del(false)
{
	string s(str);
	fill(s);
}// tDbfRec::tDbfRec


tDbfRec::~tDbfRec()
{
	//if ( dbf ) dbf->killLink(this);
}// tDbfRec::~tDbfRec


void tDbfRec::fill(string rec)
{
	cell.clear();

	if ( rec.empty() ){
		_del = false;
		for ( unsigned i = 0; i < dbf->fields(); i++ ){
			cell.push_back(dbf->fieldDefault(i));
		}
		return;
	}
	//if ( rec.empty() )  nError::Throw("tDbfRec: err - rec is empty : -1\n" + ToString(n) + "\n" + BufDump(rec));
	if ( rec[0] == '*' ) _del = true;
	rec.erase(0, 1);

	for ( unsigned i = 0; i < dbf->fields(); i++ ){
		if ( rec.empty() ) nError::Throw("tDbfRec: err - rec is empty : " + ToString(i));
		//if ( rec.empty() ) nError::Throw("tDbfRec: err - rec is empty : " + ToString(i) + "\n" + ToString(n) + "\n" + BufDump(rec));
		string type = dbf->fieldType(i);
		string cl;
		switch ( type[0] ){
		case 'C':
			cl = trimr(rec.substr(0, dbf->fieldSize(i)));
			break;

		case 'N':
			cl = trim(rec.substr(0, dbf->fieldSize(i)));
			break;

		case 'D':
			cl = rec.substr(0, dbf->fieldSize(i));
			cl.insert(6, "-");
			cl.insert(4, "-");
			break;

		case 'L':
			cl = trim(rec.substr(0, dbf->fieldSize(i)));
			if ( cl != "T" ) cl = "F";
			break;

		default:
			break;
		}
		//printf("n:%d, i:%d : [%s]\n", n, i, cl.c_str());
		cell.push_back(cl);
		rec.erase(0, dbf->fieldSize(i));
	}
	if ( !rec.empty() ) nError::Throw("tDbfRec: strange - rec not empty : " + ToString(rec.size()));
}// tDbfRec::fill


string tDbfRec::rec() const
{
	if ( !dbf ) return "";
	string ret = deleted() ? "*" : " ";

	for ( unsigned i = 0; i < cell.size(); i++ ){
		string type = dbf->fieldType(i);
		unsigned sz = dbf->fieldSize(i);
		string cl = cell[i];
		switch ( type[0] ){
		case 'C':
			if ( cl.size() > sz ) cl.resize(sz);
			if ( cl.size() < sz ) cl += string(sz - cl.size(), ' ');
			break;

		case 'N':
			if ( cl.empty() ) cl = "0";
			if ( dbf->fieldDec(i) == 0 ){
				size_t pos = cl.find_first_not_of(cs_09);
				if ( pos != string::npos ) cl = cl.substr(0, pos - 1);
			}else{
				size_t pos = cl.find_first_not_of(cs_09d);
				if ( pos != string::npos ) cl = cl.substr(0, pos - 1);
				pos = cl.find(".");
				if ( pos == string::npos ){
					cl += "." + string(dbf->fieldDec(i), '0');
				}else{
					unsigned sz = cl.size() - pos - 1;
					if ( sz < dbf->fieldDec(i) ){
						cl += string(sz - dbf->fieldDec(i), '0');
					}else if ( sz > dbf->fieldDec(i) ){
						cl.resize(cl.size() - sz + dbf->fieldDec(i));
					}
				}
			}
			if ( cl.size() > sz ) cl.erase(0, sz - cl.size());
			if ( cl.size() < sz ) cl = string(sz - cl.size(), ' ') + cl;
			break;

		case 'I':
			{
				size_t pos = cl.find_first_not_of(cs_09);
				if ( pos != string::npos ) cl = cl.substr(0, pos - 1);
			}
			if ( cl.empty() ) cl = "0";
			if ( cl.size() > sz ) cl.erase(0, sz - cl.size());
			if ( cl.size() < sz ) cl = string(sz - cl.size(), ' ') + cl;
			break;

		case 'D':
			if ( cl.size() != sz ) cl = tDbf::fieldDefault("D");
			break;

		case 'L':
			if ( cl.size() > sz ) cl.resize(sz);
			if ( cl.size() < sz ) cl = 'F';
			break;

		default:
			if ( cl.size() != sz ) cl = string(sz - cl.size(), ' ');
			break;
		}
		ret += cl;
	}
	//cout << "rec.size() : " << ret.size() << endl;
	return ret;
}// tDbfRec::rec


string tDbfRec::get(unsigned n) const
{
	if ( n >= size() )  return "";
	return cell[n];
}// tDbfRec::get


string tDbfRec::get(const string &name) const
{
	if ( !dbf ) return "";
	for ( unsigned i = 0; i < fields(); i++ ){
		if ( CompareIC(fieldName(i), name) == 0 ) return cell[i];
	}
	return "";
}// tDbfRec::get


std::map<string, string> tDbfRec::getAll() const
{
	std::map<string, string> ret;
	for ( unsigned i = 0; i < fields(); i++ ){
		string f = fieldName(i);
		ret[f] = get(f);
	}
	ret["__DEL"] = (deleted() ? "1" : "0");
	return ret;
}// tDbfRec::getAll


void tDbfRec::setAll(const std::map<string, string> &vals)
{
	for ( unsigned i = 0; i < fields(); i++ ){
		string f = fieldName(i);
		std::map<string, string>::const_iterator it = vals.find(f);
		if ( it != vals.end() ){
			set(i, it->second);
		}
	}
	std::map<string, string>::const_iterator it = vals.find("__DEL");
	if ( it != vals.end() ){
		if ( it->second == "1" )  del();
	}
}// tDbfRec::setAll


bool tDbfRec::get(unsigned n, bool def) const               // L
{
	if ( n >= size() ) return def;
	if ( !dbf ) return def;
	if ( dbf->fieldType(n) != "L" ) return def;
	return cell[n] == "T";
}// tDbfRec::get


int tDbfRec::get(unsigned n, int def) const                 // I N?
{
	if ( n >= size() ) return def;
	if ( !dbf ) return def;
	if ( dbf->fieldType(n) != "I" && dbf->fieldType(n) != "N" ) return def;
	return ToInt(cell[n], def);
}// tDbfRec::get


double tDbfRec::get(unsigned n, double def) const           // N
{
	if ( n >= size() ) return def;
	if ( !dbf ) return def;
	if ( dbf->fieldType(n) != "N" ) return def;
	return atof(cell[n].c_str());
}// tDbfRec::get


string tDbfRec::get(unsigned n, const string &def) const           // C D M?
{
	if ( n >= size() ) return def;
	if ( !dbf ) return def;
	if ( dbf->fieldType(n) != "C" && dbf->fieldType(n) != "D" &&
	     dbf->fieldType(n) != "M" ) return def;
	return cell[n];
}// tDbfRec::get


bool tDbfRec::set(unsigned n, bool val)
{
	if ( n >= size() ) return false;
	if ( !dbf ) return false;
	if ( dbf->fieldType(n) != "L" ) return false;
	cell[n] = (val) ? "T" : "F";
	return true;
}// tDbfRec::set


bool tDbfRec::set(unsigned n, int val)
{
	if ( n >= size() ) return false;
	//if ( val < 0 ) return false;
	if ( !dbf ) return false;
	if ( dbf->fieldType(n) != "I" && dbf->fieldType(n) != "N" ) return false;
	cell[n] = ToString(val);
	if ( dbf->fieldDec(n) > 0 ) cell[n] += "." + string(dbf->fieldDec(n), '0');
	return true;
}// tDbfRec::set


bool tDbfRec::set(unsigned n, double val)
{
	if ( n >= size() ) return false;
	//if ( val < 0.0 ) return false;
	if ( !dbf ) return false;
	if ( dbf->fieldType(n) != "N" ) return false;

	char buf[100];
	sprintf(buf, "%.*f", dbf->fieldDec(n), val);
	cell[n] = buf;

	return true;
}// tDbfRec::set


bool tDbfRec::set(unsigned n, const string &val)
{
	if ( n >= size() ) return false;
	if ( !dbf ) return false;
	switch ( dbf->fieldType(n)[0] ){
	case 'C':
		cell[n] = val;
		break;

	case 'D':
		if ( val.size() >= 8 && val[4] == '-' && val[6] == '-' ){
			cell[n] = val.substr(0, 4) + val.substr(5, 2) + val.substr(8, 2);
		}else{
			return false;
			//cell[n] = fieldDefault("D");
		}
		break;

	case 'N':
		if ( dbf->fieldDec(n) )  return set(n, ToDouble(val));
		return set(n, ToInt(val));

	case 'M':
		break;

	default:
		return false;
	}
	return true;
}// tDbfRec::set


bool tDbfRec::set(const string &name, const string &val)
{
	if ( !dbf ) return false;

	//cout << "\"" << name << "\" <- [" << val << "]" << endl;

	for ( unsigned n = 0; n < fields(); n++ ){
		if ( fieldName(n) == name ){
			//printf("[%s] ", dbf->fieldType(n).c_str());
			switch ( dbf->fieldType(n)[0] ){
			case 'C':
				cell[n] = val;
				break;

			case 'D':
				if ( val.size() >= 8 && val[4] == '-' && val[7] == '-' ){
					cell[n] = val.substr(0, 4) + val.substr(5, 2) + val.substr(8, 2);
				}else{
					return false;
					//cell[n] = fieldDefault("D");
				}
				break;

			case 'N':
				//printf("+");
				if ( val.find_first_not_of(cs_09d) == string::npos ){
					cell[n] = val;
				}else{
					//printf("[%s]\n", val.c_str());
					return false;
				}
				break;

			case 'I':
				if ( val.find_first_not_of(cs_09) == string::npos ){
					cell[n] = val;
				}else{
					return false;
				}
				break;

			case 'L':
				if ( val.size() > 0 && (val[0] == 'T' || val[0] == 't') ){
					cell[n] = "T";
				}else{
					cell[n] = "L";
				}
				break;

			case 'M':
				break;

			default:
				return false;
			}
			return true;
		}
	}
	return false;
}// tDbfRec::set


unsigned tDbfRec::fields() const
{
	if ( !dbf ) return (unsigned)-1;
	return dbf->fields();
}// tDbfRec::fields


string tDbfRec::fieldName(unsigned n) const
{
	if ( !dbf ) return "";
	return dbf->fieldName(n);
}// tDbfRec::fieldName


string tDbfRec::fieldType(unsigned n) const
{
	if ( !dbf ) return "";
	return dbf->fieldType(n);
}// tDbfRec::fieldType


string tDbfRec::fieldTypeName(unsigned n) const
{
	if ( !dbf ) return "";
	return dbf->fieldTypeName(n);
}// tDbfRec::fieldTypeName


unsigned tDbfRec::fieldSize(unsigned n) const
{
	if ( !dbf ) return 0;
	return dbf->fieldSize(n);
}// tDbfRec::fieldSize


unsigned tDbfRec::fieldDec(unsigned n) const
{
	if ( !dbf ) return 0;
	return dbf->fieldDec(n);
}// tDbfRec::fieldDec




string tDbf::typeOf(const string &) { return "C"; }
string tDbf::typeOf(int) { return "N"; }
string tDbf::typeOf(unsigned) { return "N"; }
string tDbf::typeOf(double) { return "N"; }
string tDbf::typeOf(ulonglong) { return "C"; }

unsigned tDbf::sizeOf(const string &) { return 100; }
unsigned tDbf::sizeOf(int) { return 6; }
unsigned tDbf::sizeOf(unsigned) { return 6; }
unsigned tDbf::sizeOf(double) { return 10; }
unsigned tDbf::sizeOf(ulonglong) { return 16; }

unsigned tDbf::decOf(const string &) { return 0; }
unsigned tDbf::decOf(int) { return 0; }
unsigned tDbf::decOf(unsigned) { return 0; }
unsigned tDbf::decOf(double) { return 3; }
unsigned tDbf::decOf(ulonglong) { return 0; }
