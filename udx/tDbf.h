#ifndef __tDbf_h
#define __tDbf_h 1

#ifndef __tFile_h
#include <tFile.h>
#endif

#ifndef __fLong_h
#include <fLong.h>
#endif

#include <vector>
#include <map>



class dbfs_Header;
class dbfs_RecInfo;
class tDbf;



class tDbfRec {
	friend class tDbf;
public:
	tDbfRec(tDbf &);
	tDbfRec(tDbf &, const string &);
	//tDbfRec(const tDbfRec &);
	~tDbfRec();

	//tDbfRec &operator=(const tDbfRec &);

private:
	void fill(string rec);
	string rec() const;

public:
	unsigned size() const { return cell.size(); };

	string get(unsigned n) const;
	string get(const string &name) const;
	std::map<string, string> getAll() const;

	bool set(const string &name, const string &val);
	void setAll(const std::map<string, string> &vals);

	bool deleted() const { return _del; };
	void del() { _del = true; };
	void restore() { _del = false; };

	bool get(unsigned n, bool def) const;               // L
	int get(unsigned n, int def) const;                 // I N?
	double get(unsigned n, double def) const;           // N
	string get(unsigned n, const string &def) const; // C D M?

	bool set(unsigned n, bool val);                     // L
	bool set(unsigned n, int val);                      // I N?
	bool set(unsigned n, double val);                   // N
	bool set(unsigned n, const string &val);            // C D M?

	unsigned fields() const;
	string fieldName(unsigned n) const;
	string fieldType(unsigned n) const;              // C N D I L M
	string fieldTypeName(unsigned n) const;
	unsigned fieldSize(unsigned n) const;
	unsigned fieldDec(unsigned n) const;

private:
	tDbf *dbf;
	std::vector<string> cell;
	bool _del;
	//unsigned num;
};// class tDbfRec




class tDbf {
	friend class tDbfRec;
public:
	enum Type{
		DBASE2       = 0x02,    // FoxBASE
		DBASE3       = 0x03,    // FoxBASE+/dBASE III PLUS, без memo-поля
		VISFOXPRO    = 0x30,    // Visual FoxPro
		DBASE4       = 0x43,    // Файлы таблиц dBASE IV SQL, без memo-поля
		DBASE4SYS    = 0x63,    // Системные файлы dBASE IV SQL, без memo-поля
		DBASE3MEMO   = 0x83,    // FoxBASE+/dBASE III PLUS, с memo-полем
		DBASE4MEMO   = 0x8B,    // dBASE IV с memo-полем
		DBASE4FMEMO  = 0xCB,    // Файлы таблиц dBASE IV SQL, с memo-полем
		FOXPRO2MEMO  = 0xF5,    // FoxPro 2.x (или более ранней) с memo-полем
		FOXBASE      = 0xFB     // FoxBASE
	};
	enum FoxOpt{
//  NOP          = 0x00,    // файл НЕ содержит структурный .CDX
		CDX          = 0x01,    // файл содержит структурный .CDX
		MEMO         = 0x02,    // файл содержит memo-поля
		DB           = 0x04     // файл является базой данных (.DBC)
	};
	enum{
		NOCP         = 0x00,    //        в версиях FoxPro до 2.5 (зарезерв.)
		CP437        = 0x01,    // 437    U.S. MS-DOS
		CP620        = 0x69,    // 620  * Mazovia (Polish) MS-DOS
		CP737        = 0x6A,    // 737  * Greek MS-DOS (437G)
		CP850        = 0x02,    // 850    International MS-DOS
		CP852        = 0x64,    // 852    Eastern European MS-DOS
		CP861        = 0x67,    // 861    Icelandic MS-DOS
		CP865        = 0x66,    // 865    Nordic MS-DOS
		CP866        = 0x65,    // 866    Russian MS-DOS
		CP895        = 0x68,    // 895    Kamenicky (Czech) MS-DOS
		CP857        = 0x6B,    // 857    Turkish MS-DOS
		CP1250       = 0xC8,    // 1250   Eastern European Windows
		CP1251       = 0xC9,    // 1251   Russian Windows
		CP1252       = 0x03,    // 1252   Windows ANSI
		CP1253       = 0xCB,    // 1253   Greek Windows
		CP1254       = 0xCA,    // 1254   Turkish Windows
		CP10000      = 0x04,    // 10000  Standard Macintosh
		CP10006      = 0x98,    // 10006  Greek Macintosh
		CP10007      = 0x96,    // 10007* Russian Macintosh
		CP10029      = 0x97     // 10029  Eastern European Macintosh
	};
	enum FieldFlag{
//  NOP          = 0x00,
		HIDDEN       = 0x01,    // Системный столбец невидимый для пользователя
		USENULL      = 0x02,    // Столбец может содержать нуль-значения
		BINARY       = 0x04     // Двоичный столбец (для полей CHAR и MEMO)
	};
	enum{
		EORi         = 0x0D
	};

public:
	tDbf();
	tDbf(const string &fname);
	~tDbf();

private:
	tDbf(const tDbf &);
	tDbf &operator=(const tDbf &);

	//void addLink(tDbfRec *);
	//void killLink(tDbfRec *);

	void flush();
	string getRec(tFILE &, unsigned n);
	bool putRec(tFILE &, unsigned n, const string &);

public:
	bool load(const string &filename);
	bool save(const string &filename);

	unsigned size() const;
	string getDate() const;
	unsigned recsize() const;
	unsigned recaddr(unsigned n = 0) const;
	unsigned fields() const { return Ri.size(); };
	string fieldName(unsigned n) const;
	string fieldType(unsigned n) const;              // C N D I L M
	string fieldTypeName(unsigned n) const;
	unsigned fieldSize(unsigned n) const;
	unsigned fieldDec(unsigned n) const;

	bool delField(const string &name);
	bool delField(unsigned n);
	bool addField(string name, const string &type, unsigned size, unsigned dec = 0);

	static string fieldDefault(const string &fieldType);
	string fieldDefault(unsigned n) const;

	tDbfRec *get(unsigned n) const;
	//bool set();
	tDbfRec *add();
	//tDbfRec *operator[](unsigned n);

	static string typeOf(const string &);
	static string typeOf(int);
	static string typeOf(unsigned);
	static string typeOf(double);
	static string typeOf(ulonglong);

	static unsigned sizeOf(const string &);
	static unsigned sizeOf(int);
	static unsigned sizeOf(unsigned);
	static unsigned sizeOf(double);
	static unsigned sizeOf(ulonglong);

	static unsigned decOf(const string &);
	static unsigned decOf(int);
	static unsigned decOf(unsigned);
	static unsigned decOf(double);
	static unsigned decOf(ulonglong);

private:
	//tFILE fi;
	dbfs_Header *Hi;
	std::vector<dbfs_RecInfo *> Ri;
	std::vector<tDbfRec *> link;
	//tDbfRec *cur;
	//string fname;
};// class tDbf




#endif // __tDbf_h
