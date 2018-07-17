#ifndef __tConfiguration_h
#define __tConfiguration_h 1

#include "nError.h"
#include "fString.h"

#include <vector>
#include <map>




namespace nError
{
	class ConfigError : public Message {
	public:
		ConfigError() : Message("config file error") {};
		ConfigError(const string &info) : Message("config file error: " + info) {};
	};// class ConfigError

	class InvalidLine : public ConfigError {
	public:
		InvalidLine(const string &file, int n, const string &line) :
			ConfigError("invalid line " + ToString(n) + " in config file '" + file + "': (" + line + ")") {};
	};// class InvalidLine
};// namespace nError




// tConfiguration - хэш для опций из .cfg - файла в стиле Windows
// Доступ:  tConfiguration.get(Section, Key) -> Value
// В .cfg файле допустимы конструкции:
// # - comment, skipping
// ; - comment, skipping
// * - comment, skipping
// [section] - раздел, до первого употребления действует раздел 'main'
// key1 = value1
// key2=value2
// Empty lines skipping
// Имеются разделы: 'environ' - содержит переменные среды,
//                  'option'  - опции командной строки
//                  'cmdline' - аргументы командной строки (не опции)

class tConfiguration {
public:
	class mss : public std::map<string, string> { };

	static const char * const mainSection;        // = 'main'
	static const char * const environSection;     // = 'environ'
	static const char * const optionSection;      // = 'option'
	static const char * const cmdlineSection;     // = 'cmdline'
	static const char * const progValue;          // = 'prog' (in [cmdline])
	static const char * const pathValue;          // = 'path' (in [cmdline])
	static const char * const nameValue;          // = 'name' (in [cmdline])

	static int precision;

public:
	tConfiguration() { loadEnv(); };
	tConfiguration(const string &filename);
	tConfiguration(const tConfiguration &);

	tConfiguration &operator=(const tConfiguration &);
	tConfiguration &operator+=(const string &);
	tConfiguration &operator+=(const tConfiguration &);

public:
	bool find(const string &sect) const;
	bool find(const string &sect, const string &key) const;
	bool findIC(const string &sect) const;
	bool findIC(const string &sect, const string &key) const;

	string findSectionIC(const string &sect) const;
	string findKeyIC(const string &sect, const string &key) const;

	string get(const string &sect, const string &key, const string &def = "") const;
	bool getBool(const string &sect, const string &key, bool def) const;
	int get(const string &sect, const string &key, int def) const;
	int get(const string &sect, const string &key, int def, int Min, int Max) const;
	double get(const string &sect, const string &key, double def) const;
	double get(const string &sect, const string &key, double def, double Min, double Max) const;
	std::vector<string> getSections() const;
	std::vector<string> getKeys(const string &sect) const;
	std::vector<string> getArgs() const;

	string getPath(const string &sect, const string &key, const string &def = "") const;
	string getTemplate(const string &sect, const string &key, const string &def = "") const;
	// expands "%%KEY%%" to result of get(sect, "KEY");
	// expands "%%SECT%KEY%%" to result of get("SECT", "KEY");

	string getIC(const string &sect, const string &key, const string &def = "") const;
	bool getBoolIC(const string &sect, const string &key, bool def) const;
	int getIC(const string &sect, const string &key, int def) const;
	double getIC(const string &sect, const string &key, double def) const;

	void set(const string &sect, const string &key, const string &val);
	void set(const string &sect, const string &key, int val);
	void set(const string &sect, const string &key, int val, int Min, int Max);
	void set(const string &sect, const string &key, double val);
	void set(const string &sect, const string &key, double val, double Min, double Max);
	void del(const string &sect, const string &key); // remove 'sect/key', if 'sect' empty - remove 'sect'
	void del(const string &sect);                    // remove section 'sect'

	void loadEnv();
	void loadCmdLine(int ac, char *aa[]);
	bool load(const string &filename);
	void reload();
	void save() const;

	string getpath() const;                        // D:\path
	string getname() const;                        // prog.exe
	string getname(const string &ext) const;       // prog.ext
	string getfile() const;                        // D:\path\prog.exe
	string getfile(const string &ext) const;       // D:\path\prog.ext
	string cfgfile() const { return cfgFile; }     // D:\path\prog.cfg (def)

protected:
	static bool splitLine(const string &s, string &key, string &val);
	static bool isComment(const string &s);
	static bool isSection(const string &s);
	static string getSection(const string &s);

	void loadCmdLine(const std::vector<string> &All);

	void setOpt(const string &key);
	bool findOpt(const string &key) const;

	//std::vector<string> fname;
	string cfgFile;
	std::map<string, mss > vals;
	std::vector<string> opts;
	std::vector<string> all;
};// class tConfiguration


// global instance of tConfiguration
extern tConfiguration config;




#endif // __tConfiguration_h
