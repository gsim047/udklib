#ifndef __fVector_h
#define __fVector_h 1

#include <string>
#include <vector>
#include <map>
using std::string;



size_t FindIC(const std::vector<string> &v, const string &s);
size_t Columns(const std::vector<std::vector<string> > &v);

void Sort(std::vector<std::vector<string> > &v, bool acc, size_t col);
void SortIC(std::vector<std::vector<string> > &v, bool acc, size_t col);


// splits string 'str' in parts between any of chars in 'delim'
std::vector<string> SplitString(const string &str, const string &delim);
// splits string 'str' in parts between entries of 'delim'
std::vector<string> StringExplode(const string &str, const string &delim, size_t maxcount = 0);

//std::map<string, string> WebArgParse(const string &str);
//std::map<string, std::vector<string> > WebArgParseX(const string &str);
//string WebArgCombine(const std::map<string, std::vector<string> > &arg);
//string WebArgCombine(const std::map<string, string> &arg);

bool ParseCSV(std::vector<string> &list, const string &src, char delim = ',');
string CellCSV(const string &s, char delim = ',');
string ToCSV(const std::vector<string> &src, char delim = ',');

int ParseAllCSV(std::vector<std::vector<string> > &res, const string &src, char delim = ',');


typedef std::vector<std::pair<string, string> > xcodeTable;

extern const xcodeTable ascii2xmlCode;
extern const xcodeTable xml2asciiCode;
extern const xcodeTable &cyr2translitCode;
extern const xcodeTable &translit2cyrCode;


string Replace(const string &Str, const xcodeTable &matrix);

xcodeTable makeCodeTable(const std::map<string, string> &matrix);


#endif // __fVector_h
