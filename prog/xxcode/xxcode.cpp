// This file was created by MkExe ver. 1.9.19 at [2008-10-15 15:53:22]

#include <cMain.h>
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
//#include "fFile.h"
#include "tFile.h"
#include "fVector.h"
//#include "tDebug.h"



#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME xxcode

extern char _binary_xxcode_rc_start[];
extern char _binary_xxcode_rc_end[];

const char* getRCStart() { return _binary_xxcode_rc_start; }
const char* getRCEnd() { return _binary_xxcode_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__



static bool __showTable = false;


static void About()
{
	cout << "Recodes text file, using recode table.";    // some description
	cout << endl << endl;
	cout << "Call :  " << config.getname() << " [key [...]] RecodeTable InFile OutFile" << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  --cfg:CfgName   - use config file 'CfgName' (default '" << config.getname("cfg") << "')" << endl;
	cout << "  -v              - show recode table" << endl;
	cout << endl;
	exit(0);
}// About


static char __UnEscapeX(const string &s, unsigned &i)
{
	unsigned ch = ch_charValue[(unsigned char)s[i]];
	if ( (i + 1) < s.size() ){
		if ( isXDigit(s[i + 1]) ){
			ch <<= 4;
			ch |= ch_charValue[(unsigned char)s[++i]];
		}
	}
	return (char)ch;
}// __UnEscapeX


static char __UnEscapeO(const string &s, unsigned &i)
{
	unsigned ch = (unsigned char)(s[i] - '0');
	if ( (i + 1) < s.size() ){
		if ( s[i + 1] >= '0' && s[i + 1] < '8' ){
			ch <<= 3;
			ch += (unsigned char)(s[++i] - '0');

			if ( (i + 1) < s.size() ){
				if ( s[i + 1] >= '0' && s[i + 1] < '8' ){
					unsigned ch0 = ch;
					ch <<= 3;
					ch += (unsigned char)(s[++i] - '0');

					if ( ch > 256 ){
						ch = ch0;
						i--;
					}else{
						if ( (i + 1) < s.size() ){
							if ( s[i + 1] >= '0' && s[i + 1] < '8' ){
								unsigned ch0 = ch;
								ch <<= 3;
								ch += (unsigned char)(s[++i] - '0');

								if ( ch > 256 ){
									ch = ch0;
									i--;
								}
							}
						}
					}
				}
			}
		}
	}
	return (char)ch;
}// __UnEscapeO


static char __UnEscapeD(const string &s, unsigned &i)
{
	unsigned ch = (unsigned char)(s[i] - '0');
	if ( (i + 1) < s.size() ){
		if ( s[i + 1] >= '0' && s[i + 1] <= '9' ){
			ch *= 10;
			ch += (unsigned char)(s[++i] - '0');

			if ( (i + 1) < s.size() ){
				if ( s[i + 1] >= '0' && s[i + 1] <= '9' ){
					unsigned ch0 = ch;
					ch *= 10;
					ch += (unsigned char)(s[++i] - '0');

					if ( ch > 256 ){
						ch = ch0;
						i--;
					}
				}
			}
		}
	}
	return (char)ch;
}// __UnEscapeD


static char __UnEscape(const string &s, unsigned &i)
{
	char c = s[i];

	switch ( c ){
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 'r':
		return '\r';
	case 'n':
		return '\n';
	case 't':
		return '\t';
	case 'v':
		return '\v';
	case 'f':
		return '\f';
	case '\\':
		return '\\';
	case '\'':
		return '\'';
	case '\"':
		return '\"';
	case 'x': case 'X':
		return __UnEscapeX(s, ++i);
	case '0':
		return __UnEscapeO(s, ++i);
	case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		return __UnEscapeD(s, i);
	default:
		nError::Throw("Invalid sequence [" + s.substr(i - 1, 2) + "] at pos " + ToString(i - 1) + " in string [" + s + "]");
	}
	return '\0';
}// __UnEscape


static string UnEscape(const string &s)
{
	string ret;
	for ( unsigned i = 0; i < s.size(); i++ ){
		char c = s[i];

		if ( c == '\\' ){
			c = __UnEscape(s, ++i);
		}
		ret += string(1, c);
	}
	return ret;
}// UnEscape


static char hex1char(char s)
{
	char ret = '\0';
	//if ( s && *s ){
		ret = (char)((ch_charValue[((unsigned char)s)])); 
	//}
	return ret;
}// hex1char


static void loadTable(const std::vector<string> &list, std::map<string, string> &mm)
{
	if ( __showTable ){
		cout << "Recode table :" << endl;
	}
	for ( unsigned i = 0; i < list.size(); i++ ){
		string ss = trim(list[i]);
		if ( ss.empty() )  continue;

		char c1 = ss[0];
		char c2;
		size_t pos1;
		switch ( c1 ){
		case '\'':
			c2 = c1;
			pos1 = ss.find("'", 1);
			if ( pos1 == string::npos )  goto def1;
			break;
		case '\"':
			c2 = c1;
			pos1 = ss.find("\"", 1);
			if ( pos1 == string::npos )  goto def1;
			break;
		case '[':
			c2 = ']';
			pos1 = ss.find("]", 1);
			if ( pos1 == string::npos )  goto def1;
			break;
		case '{':
			c2 = '}';
			pos1 = ss.find("}", 1);
			if ( pos1 == string::npos )  goto def1;
			break;
		case '(':
			c2 = ')';
			pos1 = ss.find(")", 1);
			if ( pos1 == string::npos )  goto def1;
			break;
		case '<':
			c2 = '>';
			pos1 = ss.find(">", 1);
			if ( pos1 == string::npos )  goto def1;
			break;
		default:
		def1:
			c1 = '\0';
			c2 = ' ';
			pos1 = ss.find(" ", 1);
			break;
		}

		if ( pos1 == string::npos ){
			nError::Throw("Invalid recode table, row " + ToString(i + 1) + " (Bad OP1):\n" + ss);
		}

		string op1 = (c1 == '\0') ? ss.substr(0, pos1) : ss.substr(1, pos1 - 1);
		if ( op1.empty() ){
			nError::Throw("Invalid recode table, row " + ToString(i + 1) + " (OP1 empty):\n" + ss);
		}
		string ss2 = trim(ss.substr(pos1 + 1));
		if ( ss2.empty() ){
			nError::Throw("Invalid recode table, row " + ToString(i + 1) + " (OP2 empty):\n" + ss);
		}

		if ( c1 == '\0' ){
			if ( op1.find("0x") == 0 || op1.find("0X") == 0 ){
				if ( op1.size() == 3 ){
					if ( isXDigit(op1[2]) ){
						op1 = string(1, hex1char(op1[2]));
					}
				}
				if ( op1.size() == 4 ){
					if ( op1.find_first_not_of(cs_09AFaf, 2) == string::npos ){
						string opp = op1.substr(2);
						op1 = string(1, hex2char(opp.c_str()));
					}
				}
			}
		}


		c1 = ss2[0];
		switch ( c1 ){
		case '\'':
			c2 = c1;
			pos1 = ss2.find("'", 1);
			if ( pos1 == string::npos )  goto def2;
			break;
		case '\"':
			c2 = c1;
			pos1 = ss2.find("\"", 1);
			if ( pos1 == string::npos )  goto def2;
			break;
		case '[':
			c2 = ']';
			pos1 = ss2.find("]", 1);
			if ( pos1 == string::npos )  goto def2;
			break;
		case '{':
			c2 = '}';
			pos1 = ss2.find("}", 1);
			if ( pos1 == string::npos )  goto def2;
			break;
		case '(':
			c2 = ')';
			pos1 = ss2.find(")", 1);
			if ( pos1 == string::npos )  goto def2;
			break;
		case '<':
			c2 = '>';
			pos1 = ss2.find(">", 1);
			if ( pos1 == string::npos )  goto def2;
			break;
		default:
		def2:
			c1 = '\0';
			c2 = ' ';
			break;
		}

		string op2 = (c1 == '\0') ? ss2 : ss2.substr(1, pos1 - 1);
		if ( op2.empty() ){
			nError::Throw("Invalid recode table, row " + ToString(i + 1) + " (OP2 empty):\n" + ss);
		}

		if ( c1 == '\0' ){
			if ( op2.find("0x") == 0 || op2.find("0X") == 0 ){
				if ( op2.size() == 3 ){
					if ( isXDigit(op2[2]) ){
						op2 = string(1, hex1char(op2[2]));
					}
				}
				if ( op2.size() == 4 ){
					if ( op2.find_first_not_of(cs_09AFaf, 2) == string::npos ){
						string opp = op2.substr(2);
						op2 = string(1, hex2char(opp.c_str()));
					}
				}
			}
		}

		op1 = UnEscape(op1);
		if ( __showTable ){
			cout << "- [" << ss << "] - [" << op1 << "]/[";
		}		
		op2 = UnEscape(op2);
		if ( __showTable ){
			cout << op2 << "]\n";
		}

		if ( mm.find(op1) != mm.end() ){
			nError::Throw("Invalid recode table, row " + ToString(i + 1) + " (Duplicated key):\n" + ss);
		}
		mm[op1] = op2;
	}
	if ( __showTable ){
		cout << endl;
	}
}// loadTable


int Main()
{
	cout << "Table TransCode.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	__showTable = config.getBool("option", "v", false);

	string tab = config.get("cmdline", "1");
	if ( tab.empty() )  About();

	string fin = config.get("cmdline", "2");
	if ( fin.empty() && !__showTable )  About();

	string fon = config.get("cmdline", "3");
	if ( fon.empty() && !__showTable )  About();

	if ( ExtractFileExt(tab) == "" ){
		tab += ".xxc";
	}
	if ( !tFILE::FileExists(tab) ){
		tab = ChangeFilePath(tab, config.getpath());
		if ( !tFILE::FileExists(tab) ){
			nError::Throw("Recode table [" + ExtractFileNameExt(tab) + "] not found");
		}
	}	

	std::map<string, string> mm;
	{
		std::vector<string> list;
		if ( !tRTFile::LoadFile(list, tab) ){
			nError::Throw("Can't load recode table file [" + tab + "]");
		}

		loadTable(list, mm);
	}

	if ( __showTable && (fin.empty() || fon.empty()) ){
		return 0;
	}

	{
		tFILE fi(fin, "rb");
		if ( !fi.isOpen() ){
			nError::Throw("Can't open file [" + fin + "]");
		}
	}

	xcodeTable mx = makeCodeTable(mm);
	
	string ss = tFILE::LoadFile(fin);
	string sr = Replace(ss, mx);
	if ( !tFILE::SaveFile(sr, fon) ){
		nError::Throw("Can't save file [" + fon + "]");
	}

	return 0;
}// Main
