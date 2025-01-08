#ifndef __tXML_h
#define __tXML_h 1

#pragma warning(disable:4786)

#include <string>
#include <vector>
#include <map>
using std::string;


class tXML;


class tTag{
	friend class tXML;
public:
	enum{
		UNKNOWN     = 0,
		TEXT,
		TAG,
		XML,
		COMMENT,
		DOCTYPE
	};

public:
	tTag();
	tTag(const string &);
	tTag(int type, const string &val);
	tTag(const tTag &);
	~tTag();

	tTag &operator=(const tTag &);

	void addTag(tTag *x) { if ( x ) { tags.push_back(x); x->parent = this; } };
	void addTag(int type, const string &val) { addTag(new tTag(type, val)); };
	void delTag(tTag *x);
	void addParam(const string &key, const string &value);
	void delParam(const string &key);

	string toString() const;

	bool process(const string &, size_t &pos);

	void clear();

	string operator[](const string &) const;
	string operator[](size_t n) const;
	size_t paramsize() const { return params.size(); };

	tTag *tag(const string &) const;
	size_t tagsize() const { return tags.size(); };

	static string TypeName(unsigned Type);
	string typeName() const { return TypeName(type); }

	bool isTEXT() const { return type == TEXT; }
	bool isTAG() const { return type == TAG; }
	bool isXML() const { return type == XML; }
	bool isCOMMENT() const { return type == COMMENT; }
	bool isDOCTYPE() const { return type == DOCTYPE; }

	bool isTAG(const string &Name) const { return isTAG() && value == Name; }

	string getTagPath() const;

private:
	bool eatXML(const string &, size_t &pos);
	bool eatComment(const string &, size_t &pos);
	bool eatText(const string &, size_t &pos);
	bool eatTag(const string &, size_t &pos);
	bool getParams(const string &str);

public:
	//tXML *owner;
	tTag *parent;
	string value;
	std::vector<tTag *> tags;
	std::map<string, string> params;
	std::vector<string> ind;
	unsigned type;
};// class tTag




class tXML{
public:
	tXML();
	tXML(const string &);
	tXML(const tXML &);
	~tXML();

	tXML &operator=(const tXML &);

	void clear();

	void addTag(tTag *x) { if ( x ) { tags.push_back(x); /*x->owner = this;*/ } };
	void delTag(tTag *x);

	string toString() const;

	bool load(const string &fname);
	bool save(const string &fname) const;

	tTag *operator[](size_t n) const { return (n < tags.size()) ? tags[n] : NULL; };
	tTag *tag(const string &) const;
	size_t size() const { return tags.size(); };

private:
	std::vector<tTag *> tags;
};// class tXML




#endif // __tXML_h
