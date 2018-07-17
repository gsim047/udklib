#include "fString.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <algorithm>
#include <functional>
#include <locale>
#include <iterator>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif

#include "tRecodeCC.h"
#include "tFillBuff.h"

//#pragma warning(disable:4305)
//#pragma warning(disable:4309)

/*
#define E_UPPER          0x0001    // upper case letter
#define E_LOWER          0x0002    // lower case letter
#define E_DIGIT          0x0004    // digit[0-9]
#define E_SPACE          0x0008    // tab, carriage return, newline,
                                   // vertical tab or form feed
#define E_PUNCT          0x0010    // punctuation character
#define E_CONTROL        0x0020    // control character
#define E_BLANK          0x0040    // space char
#define E_HEX            0x0080    // hexadecimal digit

//#define E_ALPHA          (0x0100|_UPPER|_LOWER)  // alphabetic character 

#define R_UPPER_WIN      0x0100
#define R_LOWER_WIN      0x0200
#define R_UPPER_DOS      0x0400
#define R_LOWER_DOS      0x0800
#define R_UPPER_KOI      0x1000
#define R_LOWER_KOI      0x2000
#define R_UPPER_ISO      0x4000
#define R_LOWER_ISO      0x8000
*/

#define C00	'\x00'
#define C01	'\x01'
#define C02	'\x02'
#define C03	'\x03'
#define C04	'\x04'
#define C05	'\x05'
#define C06	'\x06'
#define C07	'\x07'
#define C08	'\x08'
#define C09	'\x09'
#define C10	'\x0A'
#define C11	'\x0B'
#define C12	'\x0C'
#define C13	'\x0D'
#define C14	'\x0E'
#define C15	'\x0F'

#define C16	'\x10'
#define C17	'\x11'
#define C18	'\x12'
#define C19	'\x13'
#define C20	'\x14'
#define C21	'\x15'
#define C22	'\x16'
#define C23	'\x17'
#define C24	'\x18'
#define C25	'\x19'
#define C26	'\x1A'
#define C27	'\x1B'
#define C28	'\x1C'
#define C29	'\x1D'
#define C30	'\x1E'
#define C31	'\x1F'


static const string cs_webCode = "-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

const char * const cs_Space = " \t";
const char * const cs_AZaz = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char * const cs_AZaz_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
const char * const cs_AZaz09 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const char * const cs_AZaz_09 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789";
const char * const cs_07 = "01234567";
const char * const cs_09 = "0123456789";
const char * const cs_09d = "0123456789.";
const char * const cs_09dpm = "0123456789.+-";
const char * const cs_09AF = "0123456789ABCDEF";
const char * const cs_09AFaf = "0123456789ABCDEFabcdef";
/*
const char * const cs_ASCII = 
	"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
	" !\"#$%&'()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f"
	"\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
	"\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
	"\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
	"\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
	"\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
	"\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
	"\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
	"\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";
*/
const char * const cs_ASCII7 = 
	"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
	" !\"#$%&'()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f";


const char * const ch_hexValue[256] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
	"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
	"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
	"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
	"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
	"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};// ch_hexValue


const unsigned ch_charValue[256] = {
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   0,   0,   0,   0,   0,
   0,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,   0,   0,   0,   0,   0,
   0,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};// ch_charValue


char hex2char(const char *s)
{
	char ret = '\0';
	if ( s && *s ){
		ret = (char)((ch_charValue[((unsigned char)s[0])] << 4) | 
			             ch_charValue[((unsigned char)s[1])]); 
	}
	return ret;
}// hex2char



static const char asciiUpA7[128] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	' ', '!',  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  91,  92,  93,  94,  95,
	'`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 123, 124, 125, 126, 127,
};// asciiUpA7

static const char asciiLoA7[128] = {
     0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	' ',  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
	'@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  91,  92,  93,  94,  95,
	'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 123, 124, 125, 126, 127,
};// asciiLoA7


static inline char touppeRA7(char c)
{	
	unsigned char u = (unsigned char)c;
	return (u & 0x80) ? c : asciiUpA7[u];
}// touppeRA7

static inline char toloweRA7(char c)
{
	unsigned char u = (unsigned char)c;
	return (u & 0x80) ? c : asciiLoA7[u];
}// toloweRA7


string toUpperA7(const string &s)
{
	string ret(s);
	for ( size_t i = 0; i < ret.size(); i++ )  ret[i] = touppeRA7(ret[i]);
	return ret;
}// toUpperA7


string toLowerA7(const string &s)
{
	string ret(s);
	for ( size_t i = 0; i < ret.size(); i++ )  ret[i] = toloweRA7(ret[i]);
	return ret;
}// toLowerA7



typedef std::string::value_type char_t;

static char_t up_char( char_t ch )
{
    return std::use_facet< std::ctype< char_t > >( std::locale() ).toupper( ch );
}

static char_t low_char( char_t ch )
{
    return std::use_facet< std::ctype< char_t > >( std::locale() ).tolower( ch );
}

std::string toupper( const std::string &src )
{
    std::string result;
    std::transform( src.begin(), src.end(), std::back_inserter( result ), up_char );
    return result;
}

static inline int touppeR(int c)
{
//	return (int) asciiUp[(unsigned char) c];
	return (int) up_char((char_t)c);
}// touppeR


static inline int toloweR(int c)
{
//	return (int) asciiLo[(unsigned char) c];
	return (int) low_char((char_t)c);
}// toloweR


string toUpper(const string &src)
{
	std::string result;
	std::transform( src.begin(), src.end(), std::back_inserter( result ), up_char );
	return result;
}// toUpper


string toLower(const string &src)
{
	std::string result;
	std::transform( src.begin(), src.end(), std::back_inserter( result ), low_char );
	return result;
}// toLower


string triml(const string &s)
{
	string ret;
	size_t pos = s.find_first_not_of(cs_Space);
	if ( pos != string::npos ){
		ret = s.substr(pos);
	}
	return ret;
}// triml


string trimr(const string &s)
{
	string ret;
	size_t pos = s.find_last_not_of(cs_Space);
	if ( pos != string::npos ){
		ret = s.substr(0, pos + 1);
	}
	return ret;
}// trimr


string trim(const string &s)
{
	string ret;
	size_t pos1 = s.find_first_not_of(cs_Space);
	if ( pos1 != string::npos ){
		size_t pos2 = s.find_last_not_of(cs_Space);
		ret = s.substr(pos1, pos2 - pos1 + 1);
	}
	return ret;
}// trim


string kill_all_of(const string &str, const string &chars)
{
	string ret;
	for ( size_t i = 0; i < str.size(); i++ ){
		if ( chars.find_first_of(str[i]) == string::npos )  ret += str[i];
	} 
	/*
	if ( chars.empty() )  return str;

	tFillBuff ret(str.size(), 0);
	tRecodeCC cc;
	char c = chars[0];
	for ( size_t i = 0; i < chars.size(); i++ ){
		cc[chars[i]] = c;
	}
	for ( size_t j = 0; j < str.size(); j++ ){
		if ( cc.ch(str[j]) != c )  ret += str[j];
	} */
	return ret;
}// kill_all_of


string kill_all_not_of(const string &str, const string &chars)
{
	string ret;
	for ( size_t i = 0; i < str.size(); i++ ){
		if ( chars.find_first_of(str[i]) != string::npos )  ret += str[i];
	} 
	/*
	if ( chars.empty() )  return str;

	tFillBuff ret(str.size(), 0);
	tRecodeCC cc;
	char c = chars[0];
	for ( size_t i = 0; i < chars.size(); i++ ){
		cc[chars[i]] = c;
	}
	for ( size_t j = 0; j < str.size(); j++ ){
		if ( cc.ch(str[j]) == c )  ret += str[j];
	} */
	return ret;
}// kill_all_not_of


string exptab(const string &str, size_t tab)
{
	string ret = str;
	for ( size_t pos = 0;; ){
		size_t it = ret.find("\t", pos);
		if ( it == string::npos ) break;
		size_t size = ((it + tab) / tab) * tab - it;
		ret.replace(it, 1, string(size, ' '));
		pos = it + size;
	}
	return ret;
}// exptab


string imptab(const string &str, size_t tab)
{
	string tst(tab, ' ');
	string ret = str;
	size_t pos = 0;
	while ( ret.substr(pos, tab) == tst ){
		ret.replace(pos, tab, "\t");
		pos++;
	}
	return ret;
}// imptab


/*
static int dos2dos(int c)
{
	static const char dd[128] = {
	};

	if ( c < 128 )  return c;
	return dd[c - 128];
}// dos2dos
*/



int isUnicode(const string &s)
{
	if ( s.size() >= 2 && (s.size() & 1) == 0 ){
		if ( s[0] == '\xff' && s[1] == '\xfe' )  return 0;
		if ( s[1] == '\xff' && s[0] == '\xfe' )  return 1;
	}
	return -1;
}// isUnicode


void __uc_swapBytes(string &s)
{
	for ( size_t i = 0; i < s.size(); i += 2 ){
		char c = s[i];
		s[i] = s[i + 1];
		s[i + 1] = c;
	}
}// __uc_swapBytes



/*
static const char __w2uk1[128] = {
 '\x02','\x03','\x1a','\x53','\x1e','\x26','\x20','\x21','\xac','\x30','\x09','\x39','\x0a','\x0c','\x0b','\x0f',
 '\x52','\x18','\x19','\x1c','\x1d','\x22','\x13','\x14','\xad','\x22','\x59','\x3a','\x5a','\x5c','\x5b','\x5f',
 '\xa0','\x0e','\x5e','\x08','\xa4','\x90','\xa6','\xa7','\x01','\xa9','\x04','\xab','\xac','\xaf','\xae','\x07',
 '\xb0','\xb1','\x06','\x56','\x91','\xb5','\xb6','\xb7','\x51','\x16','\x54','\xbb','\x58','\x05','\x55','\x57',

 '\x10','\x11','\x12','\x13','\x14','\x15','\x16','\x17','\x18','\x19','\x1a','\x1b','\x1c','\x1d','\x1e','\x1f',
 '\x20','\x21','\x22','\x23','\x24','\x25','\x26','\x27','\x28','\x29','\x2a','\x2b','\x2c','\x2d','\x2e','\x2f',
 '\x30','\x31','\x32','\x33','\x34','\x35','\x36','\x37','\x38','\x39','\x3a','\x3b','\x3c','\x3d','\x3e','\x3f',
 '\x40','\x41','\x42','\x43','\x44','\x45','\x46','\x47','\x48','\x49','\x4a','\x4b','\x4c','\x4d','\x4e','\x4f'
};

static const char __w2uk2[128] = {
 '\x04','\x04','\x20','\x04','\x20','\x20','\x20','\x20','\x20','\x20','\x04','\x20','\x04','\x04','\x04','\x04',
 '\x04','\x20','\x20','\x20','\x20','\x20','\x20','\x20','\x00','\x21','\x04','\x20','\x04','\x04','\x04','\x04',
 '\x00','\x04','\x04','\x04','\x00','\x04','\x00','\x00','\x04','\x00','\x04','\x00','\x00','\x00','\x00','\x04',
 '\x00','\x00','\x04','\x04','\x04','\x00','\x00','\x00','\x04','\x21','\x04','\x00','\x04','\x04','\x04','\x04',

 '\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04',
 '\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04',
 '\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04',
 '\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04','\x04'
};




string win2unicode(const string &s)
{
	static const string c0(1, '\0');
	static const string c4(1, '\x04');

	string ret(s.size() * 2, '\0');
	for ( size_t i = 0; i < s.size(); i++ ){
		char c = s[i];
		int ci = int((unsigned char)c);
		if ( ci >= 128 ){
			ret[i << 1] = __w2uk1[ci - 128];
			ret[(i << 1) + 1] = __w2uk2[ci - 128];
		}else{
			ret[i << 1] = c;
		}
	}
	return ret;
}// win2unicode


static const char __uk2w[256] = {
};


string unicode2win(const string &s)
{
	if ( (s.size() & 1) == 1 )  return "";

	string ret(s.size() / 2, '\0');
	for ( size_t i = 0, j = 0; i < s.size(); i += 2, j++ ){
		char c1 = s[i];
		char c2 = s[i + 1];

		if ( c2 == '\0' ){
			ret[j] = c1;
		}else if ( c2 == '\x04' ){
			ret[j] = __uk2w[int((unsigned char)c1)];
		}else{
			ret[j] = c1;
		}
	}
	return ret;
}// unicode2win
*/


static const unsigned char __u2w194[64] = {
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	160, '!', 164, 164, 164, 164, 166, 167, 147, 169, 176, 171, 172, 173, 174, 151,
	176, 177, '2', '3', 145, 181, 182, 183, 146, '1', '0', 187, '/', '/', '/', '?'
};

static const unsigned char __u2w208[64] = {
	  0, 168, '_', '_', 170, '_', '_', 175, '_', '_', '_', '_', '_',  0,  161, '_',
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239
};

static const unsigned char __u2w209[64] = {
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
	  0, 184, 144, 131, 186, 190, 179, 191, 188, 154, 156, 158, 157,   0, 162, 159,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};


string utf2win(const string &s)
{
	tFillBuff ret(s.size(), 1024);
	for ( size_t i = 0; i < s.size(); i++ ){
		char c = s[i];
		if ( c == (char)226 ){
			i++;
			if ( i >= s.size() )  break;
			char d = s[i];
			if ( d == (char)128 ){
 				i++;
 				if ( i >= s.size() )  break;
 				char e = s[i];
 				if ( e == (char)156 ){
 					ret += (char)147;
 				}else if ( e == (char)157 ){
 					ret += (char)148;
 				}else if ( e == (char)152 ){
 					ret += (char)145;
 				}else if ( e == (char)153 ){
 					ret += (char)146;
 				}else if ( e == (char)154 ){
 					ret += (char)130;
 				}else if ( e == (char)155 ){
 					ret +=  96;
 				}else if ( e == (char)158 ){
 					ret += (char)132;
 				}else if ( e == (char)160 ){
 					ret += (char)134;
 				}else if ( e == (char)161 ){
 					ret += (char)135;
 				}else if ( e == (char)162 ){
 					ret += (char)149;
 				}else if ( e == (char)166 ){
 					ret += '.';
 				}else if ( e == (char)167 ){
 					ret += (char)183;
 				}else if ( e == (char)176 ){
 					ret += '%';
 				}else if ( e == (char)178 ){
 					ret += (char)146;
 				}else if ( e == (char)179 ){
 					ret += (char)148;
 				}else if ( e == (char)181 ){
 					ret +=  96;
 				}else if ( e == (char)185 ){
 					ret += '<';
 				}else if ( e == (char)186 ){
 					ret += '>';
 				}else if ( e == (char)190 ){
 					ret += (char)172;
 				}else{
 					ret += (char)151;
 				}
			}else if ( d == (char)148 ){
 				i++;
 				if ( i >= s.size() )  break;
 				char e = s[i];
 				if ( e == (char)128 || e == (char)129 || e == (char)132 || e == (char)133 ||
 				     e == (char)136 || e == (char)137 || e == (char)140 || e == (char)141 ){
 					ret += (char)151;
 				}else	if ( e == (char)130 || e == (char)131 || e == (char)134 || e == (char)135 ||
       				     e == (char)138 || e == (char)139 || e == (char)142 || e == (char)143 ){
 					ret += (char)166;
 				}else{
 					ret += (char)128;
 				}
			}else if ( d == (char)132 ){
 				i++;
 				if ( i >= s.size() )  break;
 				char e = s[i];
 				if ( e == (char)150 ){
 					ret += (char)185;
 				}else if ( e == (char)131 ){
 					ret += 'C';
 				}else if ( e == (char)133 ){
 					ret += '%';
 				}else if ( e == (char)137 ){
 					ret += 'F';
 				}else if ( e == (char)147 ){
 					ret += 'l';
 				}else if ( e == (char)162 ){
 					ret += (char)153;
 				}else if ( e == (char)166 ){
 					ret += 'W';
 				}else if ( e == (char)171 ){
 					ret += 'A';
 				}else if ( e == (char)174 ){
 					ret += 'e';
 				}else{
 					ret += (char)128;
 				}
			}else if ( d == (char)130 ){
 				i++;
 				if ( i >= s.size() )  break;
 				char e = s[i];
 				ret += ( e == (char)172 ) ? (char)136 : (char)128;
			}else if ( d == (char)136 ){
 				i++;
 				if ( i >= s.size() )  break;
 				char e = s[i];
 				ret += ( e == (char)146 ) ? '-' : (char)128;
			}else{
				ret += c;
				ret += d;
			}
		}else	if ( c == (char)194 ){
			i++;
			if ( i >= s.size() )  break;
			int idx = int((unsigned char)s[i]) % 64;
			ret += char(__u2w194[idx]);
		}else if ( c == (char)195 ){
			i++;
			if ( i >= s.size() )  break;
			ret += char(int((unsigned char)s[i]) + 64);
		}else if ( c == (char)208 ){
			i++;
			if ( i >= s.size() )  break;
			int idx = int((unsigned char)s[i]) % 64;
			ret += char(__u2w208[idx]);
		}else if ( c == (char)209 ){
			i++;
			if ( i >= s.size() )  break;
			int idx = int((unsigned char)s[i]) % 64;
			ret += char(__u2w209[idx]);
		}else{
			ret += c;
		}
	}
	return ret;
}// utf2win


static const unsigned char __w2u208[128] = {
	130, 131,   0,   0,   0,   0,   0,   0,   0,   0, 137,   0, 138, 140, 139, 143,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0, 142,   0, 136,   0,   0,   0,   0, 129,   0, 132,   0,   0,   0,   0, 135,
	  0,   0, 134,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 133,   0,   0, 
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static const unsigned char __w2u209[128] = {
	  0,   0,   0, 147,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	146,   0,   0,   0,   0,   0,   0,   0,   0,   0, 153,   0, 154, 156, 155, 159,
	  0,   0, 158,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0, 150,   0,   0,   0,   0, 145,   0, 148,   0, 152,   0, 149, 151,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143
};


string win2utf(const string &s)
{
	tFillBuff ret(s.size() * 2, 1024);
	for ( size_t i = 0; i < s.size(); i++ ){
		char c = s[i];
		int idx = int((unsigned char)c);
		if ( idx >= 128 ){
			char d = char(__w2u208[idx - 128]);
			if ( d != '\0' ){
				ret += (char)208;
				ret += d;
			}else{
				d = char(__w2u209[idx - 128]);
				if ( d != '\0' ){
					ret += (char)209;
					ret += d;
				}else{
					ret += c;
				}
			}
		}else{
			ret += c;
		}
	}
	return ret;
}// win2utf



int CompareIC(const string &s1, const string &s2)
{
	string S1 = toUpper(s1);
	string S2 = toUpper(s2);
	if ( S1 > S2 )  return 1;
	if ( S2 > S1 )  return -1;
	return 0;
}// CompareIC


bool operator^=(const string &s1, const string &s2)
{
	//string S1 = toUpper(s1);
	//string S2 = toUpper(s2);
	//return S1 == S2;
	return CompareIC(s1, s2) == 0;
}// operator^=



long dec(const string &str)
{
	return strtol(str.c_str(), (char **)NULL, 10);
}// dec


long hex(const string &str)
{
	return strtol(str.c_str(), (char **)NULL, 16);
}// hex


static const char *__digits__[36] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
	"k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
	"u", "v", "w", "x", "y", "z"
};// __digits__


string int2str(int digital, unsigned base)
{
	if ( base < 1 || base > 36 ) base = 10;
	bool neg = (digital < 0);
	if ( neg ) digital = -digital;

	string rc;
	char tmp[40];
	switch ( base ){
	case 1:
		rc = string(digital, '1');
		break;

	case 8:
		sprintf(tmp, "%o", (unsigned)digital);
		rc = tmp;
		break;

	case 10:
		sprintf(tmp, "%u", (unsigned)digital);
		rc = tmp;
		break;

	case 16:
		sprintf(tmp, "%x", (unsigned)digital);
		rc = tmp;
		break;

	default:
		while ( digital > 0 ){
			rc += __digits__[digital % base];
			digital /= base;
		}
		if ( rc.empty() )  rc = "0";
		rc = Reverse(rc);
	}
	if ( neg )  rc = "-" + rc;
	return rc;
}// int2str


string ToString(long value, int num)
{
	char str[40];
	sprintf(str, "%0*ld", num, value);
	return string(str);
}// ToString


string ToStringU(unsigned long value, int num)
{
	char str[40];
	sprintf(str, "%0*lu", num, value);
	return string(str);
}// ToStringU


string ToStringSz(size_t value, int num)
{
	char str[80];
	if ( sizeof(size_t) == 4 ){
		sprintf(str, "%0*ld", num, (long)value);
	}else{
		sprintf(str, "%0*lld", num, value);		
	}
	return string(str);
}// ToStringSz


string ToStringL(longlong value)
{
	char str[80];
#ifdef __BORLANDC__
	sprintf(str, "%Ld", value);
#endif
#ifdef _MSC_VER
	sprintf(str, "%I64d", value);
#endif
	return string(str);
}// ToStringL


string ToStringU64(ulonglong n)
{
	return ToStringHex((unsigned)(n >> 32), 8) +
	       ToStringHex((unsigned)(n & 0xFFFFFFFF), 8);
}// ToStringU


ulonglong ToUInt64(const string &s)
{
	if ( s.size() > 8 ){
		return ((ulonglong)ToUIntRadix(s.substr(0, s.size() - 8), 16)) << 32 |
		       (ulonglong)ToUIntRadix(s.substr(s.size() - 8), 16);
	}
	return (ulonglong)ToUIntRadix(s, 16);
}// ToUInt64


string ToStringHex(unsigned value, int num)
{
	char str[40];
	sprintf(str, "%0*X", num, value);
	return string(str);
}// ToStringHex

string ToStringHexL(unsigned long value, int num)
{
	char str[80];
#ifdef __CYGWIN__
	sprintf(str, "%0*PRIx64", num, value);
#else
	sprintf(str, "%0*x64", num, value);
#endif
	return string(str);
}// ToStringHexL


string ToStringHexSz(size_t value, int num)
{
	char str[80];
	sprintf(str, "%0*X", num, (unsigned int)value);
	return string(str);
}// ToStringHexSz


string ToStringRadix(long value, unsigned radix, unsigned num, char fillchar)
{
	if ( radix < 1 || radix > 36 )  radix = 10;
	bool neg = (value < 0);
	if ( neg )  value = -value;

	string rc;
	switch ( radix ){
	case 1:
		rc = string(value, '1');
		break;

	default:
		while ( value > 0 ){
			rc += __digits__[value % radix];
			value /= radix;
		}
		if ( rc.empty() )  rc = "0";
		rc = Reverse(rc);
	}
	if ( num > rc.size() )  rc.insert((unsigned)0, num - rc.size(), fillchar);
	if ( neg )  rc.insert(0, "-");
	return rc;
}// ToStringRadix


string ToStringD(double f, int num)
{
	char buf[101];
	if ( num > 50 )  num = 50;
	if ( num <= -1 ){
		sprintf(buf, "%.6f", f);
		string b(buf);
		size_t pos = b.find_last_not_of("0");
		pos = b.find_last_not_of(".", pos);
		return b.substr(0, pos+1);
	}else{
		sprintf(buf, "%.*f", num, f);
	}
	return string(buf);
}// ToStringD


double ToDouble(const string &S, double defVal)
{
	if ( S.empty() )  return defVal;
	string s = Replace(S, ',', '.');
	char *p = nullptr;
	double ret = strtod(s.c_str(), &p);
	if ( !p || p != s.c_str() + s.size() )  return defVal;
	return ret;
}// ToDouble


bool ToBool(const string &s)
{
	return s.find_first_not_of('0') != string::npos;
}// ToBool


int ToInt(const string &s, int defVal)
{
	if ( s.empty() )  return defVal;
	char *p = NULL;
	int ret = strtol(s.c_str(), &p, 10);
	if ( !p || p != s.c_str() + s.size() )  return defVal;
	return ret;
}// ToInt


int ToIntO(const string &s, int defVal)
{
	if ( s.empty() )  return defVal;
	char *p = NULL;
	int ret = strtol(s.c_str(), &p, 0);
	if ( !p || p != s.c_str() + s.size() )  return defVal;
	return ret;
}// ToIntO


longlong ToLInt(const string &s, longlong defVal)
{
	if ( s.empty() )  return defVal;
#ifdef __GNUG__
	char *p = NULL;
	longlong ret = strtoll(s.c_str(), &p, 10);
	if ( !p || p != s.c_str() + s.size() )  return defVal;
#else
	longlong ret = _atoi64(s.c_str());
#endif
	return ret;
}// ToLInt


unsigned ToUInt(const string &s, unsigned defVal)
{
	if ( s.empty() )  return defVal;
	char *p = NULL;
	unsigned ret = strtoul(s.c_str(), &p, 0);
	if ( !p || p != s.c_str() + s.size() )  return defVal;
	return ret;
}// ToUInt


//static const char __digs__[37] = {
//	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//};

int ToIntRadix(const string &s, unsigned radix, int defVal)
{
	if ( s.empty() )  return defVal;
	char *p = NULL;
	int ret = strtol(s.c_str(), &p, radix);
	if ( !p || p != s.c_str() + s.size() )  return defVal;
	return ret;
}// ToIntRadix


unsigned ToUIntRadix(const string &s, unsigned radix, unsigned defVal)
{
	if ( s.empty() )  return defVal;
	char *p = NULL;
	unsigned ret = strtoul(s.c_str(), &p, radix);
	if ( !p || p != s.c_str() + s.size() )  return defVal;
	return ret;
}// ToUIntRadix


string Replace(const string &s, char c1, char c2)
{
	string ret = s;
	std::replace(ret.begin(), ret.end(), c1, c2);
	return ret;
}// Replace

/*
string Escape(const string &s)
{
	tFillBuff ret(s.size() * 2, 1024);
	for ( size_t i = 0; i < s.size(); i++ ){
		char c = s[i];
		if ( c == '\x27' ){     // '
			ret += '\\';
		}
		ret += c;
	}
	return ret;
}// Escape


string Escape2(const string &s)
{
	tFillBuff ret(s.size() * 2, 1024);
	for ( size_t i = 0; i < s.size(); i++ ){
		char c = s[i];
		if ( c == '\"' ){     // '
			ret += '\\';
		}
		ret += c;
	}
	return ret;
}// Escape2


static char __UnEscapeX(const string &s, size_t &i)
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


static char __UnEscapeO(const string &s, size_t &i)
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


static char __UnEscapeD(const string &s, size_t &i)
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


static char __UnEscape(const string &s, size_t &i)
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
	//default:
	//	nError::Throw("Invalid sequence [" + s.substr(i - 1, 2) + "] at pos " + ToString(i - 1) + " in string [" + s + "]");
	}
	return c;
}// __UnEscape


string UnEscape(const string &s)
{
	string ret;
	for ( size_t i = 0; i < s.size(); i++ ){
		char c = s[i];

		if ( c == '\\' ){
			c = __UnEscape(s, ++i);
		}
		ret += string(1, c);
	}
	return ret;
}// UnEscape


string Quote(long value)
{
	return ToString(value);
}// Quote


string Quote(const string &s)
{
	string ret = "'" + Escape(s) + "'";
	return ret;
}// Quote


string Quote2(const string &s)
{
	string ret = "\"" + Escape2(s) + "\"";
	return ret;
}// Quote2


string WebUnEscape(const string &str, bool spaces)
{
	tFillBuff ret(str.size(), 1024);
	for ( size_t i = 0; i < str.size(); i++ ) {
		char c = str[i];
		if ( spaces && c == '+' ){
			ret += ' ';
		}else if ( c == '%' && i < (str.size() - 2)){
			ret += hex2char(str.c_str() + i + 1);
			i += 2;
		}else{
			ret += c;
		}
	}
	return ret;
}// WebUnEscape


string WebEscape(const string &str, bool spaces)
{
	tFillBuff ret(str.size() * 3, 1024);
	for ( size_t i = 0; i < str.size(); i++ ){
		unsigned char c = (unsigned char)str[i];
		if ( c == ' ' ){
			if ( spaces ){
				ret += '+';
			}else{
				ret += "%20";
			}
		}else if ( cs_webCode.find(str[i]) == string::npos ){
			ret += '%';
			//ret += __digs__[c >> 4];
			//ret += __digs__[c & 0xF];
			ret.push(ch_hexValue[c], 2);
		}else{
			ret += str[i];
		}
	}
	return ret;
}// WebEscape
*/

string Reverse(const string &str)
{
	string tmp(str);
	std::reverse(tmp.begin(), tmp.end());
	return tmp;  
}// Reverse


size_t Count(const string &str, const string &sub)
{
	if ( sub.empty() )  return 0;

	size_t n = 0;
	for ( size_t pos = str.find(sub);
	      pos != string::npos; 
	      pos = str.find(sub, pos + sub.size()) )  n++;

	return n;
}// Count


string Replace(const string &str, const string &old, const string &new_str, size_t n)
{
	tFillBuff ret(str.size(), str.size());
	size_t pos = 0;
	for ( size_t i = 0; n == 0 || i < n; i++ ){
		size_t it = str.find(old, pos);
		if ( it == string::npos ){
			if ( pos < str.size() ){
				ret.push(str.c_str() + pos, str.size() - pos);
			}
			break;
		}
		if ( pos != it ){
			ret.push(str.c_str() + pos, it - pos);
		}
		ret += new_str;
		pos = it + old.size();
	}
	return ret;
}// Replace


struct __any_of__
{
	__any_of__(const string &S) : s(S) {}
	bool operator()(char c){ return s.find(c) != string::npos; }
	const string &s;
};// struct __any_of__

/*
string Replace(const string &s, const string &s1, char c2)
{
	string ret = s;
	std::replace_if(ret.begin(), ret.end(), __any_of__(s1), c2);
	return ret;
}// Replace
*/

string Replace(const string &str, const string &s1, char c2)
{
	tRecodeCC cc;
	for ( size_t i = 0; i < s1.size(); i++ ){
		cc[s1[i]] = c2;
	}
	return cc.Recode(str);
}// Replace


size_t CountWords(const string &str, const string &delim)
{
	size_t count = 0;

	for ( size_t pos = 0; pos != string::npos; ){
		pos = str.find_first_not_of(delim, pos);
		if ( pos == string::npos )  break;
		count++;
		pos = str.find_first_of(delim, ++pos);
	}
	return count;
}// CountWords


string GetWord(const string &str, size_t num, const string &delim)
{
	string ret;
	size_t count = 0;

	for ( size_t pos = 0; pos != string::npos; ){
		pos = str.find_first_not_of(delim, pos);
		if ( pos == string::npos )  break;
		if ( count == num ){
			size_t pos2 = str.find_first_of(delim, pos + 1);
			if ( pos2 == string::npos ){
				ret = str.substr(pos);
			}else{
				ret = str.substr(pos, pos2 - pos);
			}
			break;
		}
		count++;
		pos = str.find_first_of(delim, ++pos);
	}
	return ret;
}// GetWord


string BufDump(const void *buf, size_t size)
{
	static const string s_br("   [");
	static const string s_sc(": ");
	static const string s_ds(" -");
	static const string s_sb("]\n");

	tFillBuff str(5 * (size + 32), 1024);
	if ( buf != NULL && size > 0 ){
		//size_t size2 = 5 * (size + 32);
		//char *buf = new char[size2];
		//size_t idx = 0;

		const unsigned char *c = (const unsigned char *)buf;
		char cc[16];
		memset(cc, ' ', 16);
		size_t idc = 0;

		for ( size_t i = 0; i < size; i++ ){
			if ( i % 16 == 0 ){
				if ( idc > 0 ){
					str += s_br;
					str.push(cc, 16);
					str += ']';
				}
				str += '\n';
				str += ToStringHex((unsigned)i, 5);
				str += s_sc;
				memset(cc, ' ', 16);
				idc = 0;
			}
			if ( i % 16 == 8 ){
				str += s_ds;
			}
			str += ' ';
//			int nnn = c[i];
//			printf("i:%d c[i]:%02X (%c) %d [%s],  ", i, c[i], c[i], nnn, ch_hexValue[nnn]);
			str.push(string(ch_hexValue[c[i]]));
			cc[idc++] = ( c[i] >= ' ' ) ? c[i] : '.';
		}
		if ( idc > 0 ){
			//size_t ccsize = cc.size();
			//cc += string(16 - ccsize, ' ');
			size_t size3 = 3 * (16 - idc) + ((idc <= 8) ? 2 : 0);
			str.fill(' ', size3);

			str += s_br;
			str.push(cc, 16);
			str += s_sb;
			//str += string(3 * (16 - ccsize) + ((ccsize <= 8) ? 2 : 0), ' ') + "   [" + cc + "]\n";
		}
	}
	return str;
}// BufDump


string Cut(const string &str, const string &sub)
{
	size_t pos = str.find(sub);
	if ( pos != string::npos ){
		return str.substr(0, pos);
	}
	return str;
}// Cut


size_t find_end_qstring(const string &checked_str, char br)
{
	if ( checked_str.empty() || checked_str[0] != br )  return string::npos;
	for ( size_t pos = checked_str.find(br, 1);
	      pos != string::npos;
	      pos = checked_str.find(br, pos + 1) ){
		size_t sl_st = checked_str.substr(1, pos - 1).find_last_not_of('\\');
		if ( sl_st == string::npos ){
			if ( pos % 2 && pos != 1 )  continue;
			return ++pos;
		}
		if ( (pos - sl_st) % 2 )  continue;
		return ++pos;
	}
	return string::npos;
}// find_end_qstring


string str2hex(const string &s)
{
	tFillBuff ret(s.size() * 2);
	for ( size_t i = 0; i < s.size(); i++ ){
		ret.push(ch_hexValue[(unsigned char)s[i]], 2);
	}
	return ret;
}// str2hex


string hex2str(const string &s)
{
	string ret;
	if ( s.size() % 2 == 0 && s.find_first_not_of(cs_09AFaf) == string::npos ){
		ret = string(s.size() / 2, ' ');
		for ( size_t i = 0, j = 0; i < s.size(); i += 2, j++ ){
			//ret += hex2char(s.c_str() + i);
			ret[j] = hex2char(s.c_str() + i);
		} 
	}
	return ret;
}// hex2str

/*
string getTypeName(const string &s)
{
#ifdef _WIN32
	return string(s);
#endif
#ifdef __GNUG__
	string ret(s);
	ret.erase(0, ret.size() > 10 ? 2 : 1);
	return ret;
#endif
}// getTypeName
*/

#ifdef __GNUG__
string demangleTypeName(const char* name)
{
	int status = -4; // some arbitrary value to eliminate the compiler warning

	// enable c++11 by passing the flag -std=c++11 to g++
	std::unique_ptr<char, void(*)(void*)> res {	abi::__cxa_demangle(name, NULL, NULL, &status),	std::free };

	return (status==0) ? res.get() : name ;
}// 
#else
string demangleTypeName(const char* name) {
    return name;
}
#endif


size_t sprintf(string &s, const char *fmt, ...)
{
	for ( int size = 1024; size < 131072; size *= 2 ){
		char *str = new char [size + 1];
		va_list argptr;

		va_start(argptr, fmt);
		int cnt = 
#ifdef __GNUG__
		vsnprintf(str, size, fmt, argptr);
#else
		_vsnprintf(str, size, fmt, argptr);
#endif
		va_end(argptr);

		s = str;
		delete [] str;
		if ( cnt < size )  break;
	}
	return s.size();
}//sprintf


bool strMatch(const string &mask, const string &str) // [Aa9X? ]
{
	if ( mask.size() != str.size() )  return false;
	for ( size_t i = 0; i < mask.size(); i++ ){
		char s = str[i];
		switch ( mask[i] ){
		case 'A':
			if ( s < 'A' || s > 'Z' )  return false;
			break;
		case 'a':
			if ( s < 'A' || s > 'z' || (s > 'Z' && s < 'a') )  return false;
			break;
		case '9':
			if ( s < '0' || s > '9' )  return false;
			break;
		case 'X':
			if ( !((s >= '0' && s <='9') || (s >='a' && s <= 'f') || (s >='A' && s <= 'F')) )  return false;
			break;
		case '?':
			break;
		case ' ':
			if ( s != ' ' || s != '\t' )  return false;
			break;
		default:
			if ( s != mask[i] )  return false;
		}
	}
	return true;
}// strMatch


string DoQuote(const string &s, const string &s_before, const string &s_after)
{
	tFillBuff ret(s.size() + s_before.size() + s_after.size());
	ret += s_before;
	ret += s;
	ret += s_after;
	return ret;
}// DoQuote


bool Quoted(const string &s, const string &s_before, const string &s_after)
{
	return s.size() >= (s_before.size() + s_after.size()) &&
	       s.substr(0, s_before.size()) == s_before &&
	       s.substr(s.size() - s_after.size()) == s_after;
}// Quoted


bool UnQuote(string &s, const string &s_before, const string &s_after)
{
	if ( !Quoted(s, s_before, s_after) )  return false;
	s = s.substr(s_before.size(), s.size() - s_before.size() - s_after.size());
	return true;
}// UnQuote


char *c_dup(const string &s)
{
	char *ret = new char[s.size() + 1];
	strcpy(ret, s.c_str());
	return ret;
}// c_dup


bool isFileMask(const string &mask)
{
	return mask.find_first_of("?*") != string::npos;
}// isFileMask


bool FileMatch(const string &__mask, const string &__s)
{
	//printf("%s : %s", __mask.c_str(), __s.c_str());
	string Mask = __mask;
	string S = __s;

#ifdef _WIN32
	Mask = toUpper(Mask);
	S = toUpper(S);
#endif

	const char *cp = NULL;
	const char *mp = NULL;
	const char *mask = Mask.c_str();
	const char *s = S.c_str();

	for ( ; *s && *mask != '*'; mask++, s++ ){
		if ( *mask != *s && *mask != '?' ){
			//printf("   1\n");
			return false;
		}
	}
	for (;;){
		if ( !*s ){
			while ( *mask == '*' ) mask++;
			//printf("   2 [%02X] %d\n", (unsigned char)(*mask), !*mask);
			return !*mask;
		}
		if ( *mask == '*' ){
			if ( !*++mask ) break;
			mp = mask;
			cp = s + 1;
			continue;
		}
		if ( *mask == *s || *mask == '?' ){
			mask++,
			s++;
			continue;
		}
		mask = mp;
		s = cp++;
	}
	return true;
}// FileMatch




bool isSpace(char c)
{
//	return (__ctype[unsigned((unsigned char)c)] & E_SPACE) != 0;
	return std::isspace(c, std::locale());
}// isSpace

bool isDigit(char c)
{
//	return (__ctype[unsigned((unsigned char)c)] & E_DIGIT) != 0;
	return std::isdigit(c, std::locale());
}// isDigit

bool isXDigit(char c)
{
//	return (__ctype[unsigned((unsigned char)c)] & (E_DIGIT|E_HEX)) != 0;
	return std::isxdigit(c, std::locale());
}// is

bool isAlpha(char c)
{
//	return (__ctype[unsigned((unsigned char)c)] & (E_UPPER|E_LOWER)) != 0;
	return std::isalpha(c, std::locale());
}// isAlpha

/*
bool isAlphaR(char c)
{
	return (__ctype[unsigned((unsigned char)c)] & (E_UPPER|E_LOWER|ctypeUp|ctypeLo)) != 0;
}// isAlphaR
*/
bool isAlnum(char c)
{
//	return (__ctype[unsigned((unsigned char)c)] & (E_UPPER|E_LOWER|E_DIGIT)) != 0;
	return std::isalnum(c, std::locale());
}// isAlnum
/*
bool isAlnumR(char c)
{
	return (__ctype[unsigned((unsigned char)c)] & (E_UPPER|E_LOWER|E_DIGIT|ctypeUp|ctypeLo)) != 0;
}// isAlnumR
*/
bool isLower(char c)
{
	std::locale loc;
//	return (__ctype[unsigned((unsigned char)c)] & (E_LOWER)) != 0;
	return std::islower(c, loc);
}// isLower
/*
bool isLowerR(char c)
{
	return (__ctype[unsigned((unsigned char)c)] & (E_LOWER|ctypeLo)) != 0;
}// isLowerR
*/
bool isUpper(char c)
{
	std::locale loc;
//	return (__ctype[unsigned((unsigned char)c)] & (E_UPPER)) != 0;
	return std::isupper(c, loc);
}// isUpper
/*
bool isUpperR(char c)
{
	return (__ctype[unsigned((unsigned char)c)] & (E_UPPER|ctypeUp)) != 0;
}// isUpperR
*/


string ch_shrink(const string &s, char c)
{
	tFillBuff ret(s.size(), 1024);
	bool is_c = false;
	for ( size_t i = 0; i < s.size(); i++ ){
		char sc = s[i];
		if ( sc == c ){
			if ( !is_c ){
				ret += sc;
				is_c = true;
			}
		}else{
			ret += sc;
			is_c = false;
		}
	}
	return ret;
}// ch_shrink



size_t findIC(const string &s0, const string &sub, size_t pos)
{
	string s = toUpper(s0);
	return s.find(toUpper(sub), pos);
}// findIC





static void __calcFreq(const string &s, unsigned *ch)
{
	static const int _n[256] = {
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
 		-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
		 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
		16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
		 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
		16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31
	};

	size_t nn = 0;

	size_t i;
	for ( i = 0; i < s.size(); i++ ){
		unsigned char c = (unsigned char)s[i];
		int idx = _n[c];
		if ( idx >= 0 ){
			ch[idx] += 1;
			nn++;
		}
	}

	for ( i = 0; i < 32; i++ ){
		ch[i] = int(1024. * ch[i] / nn);
	}
}// __calcFreq


static double __minFreq(const unsigned *chX, const unsigned *ch)
{
	double sum = 0.;
	for ( size_t i = 0; i < 32; i++ ){
		double k = ch[i];
		k -= chX[i];
		sum += k * k;
	}
	return sum;
}// __minFreq


int detectTextCP(const string &s)	// Unknown:-1, DOS:0, KOI:1, WIN:2, ISO:3, MAC:4, UTF:5, UNI:6
{
	static const unsigned chW[32] = {
	  86, 15, 43, 18, 31, 81,  9, 17, 68, 10, 38, 55, 31, 65,109, 33,
	  50, 54, 60, 30,  2,  8,  3, 16,  9,  3,  0, 19, 17,  2,  5, 20
	};

	static const unsigned chD[32] = {
	 139,142,163, 79,  8,212,  9, 35, 21,  9,  0, 58, 64,  9, 14, 53,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	};

	static const unsigned chK[32] = {
	   5, 74, 16,  3, 28, 84,  3, 16,  9, 68, 10, 35, 58, 32, 67,111,
	  29, 19, 51, 55, 61, 29, 10, 45, 23, 21, 16,  8,  4,  3, 13,  0
	};

	static const unsigned chU[32] = {
	   0,  0,  3,  0,  0, 75,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 670,274,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	};

	static const unsigned chI[32] = {
	  52, 56, 62, 29,  3, 10,  3, 13,  8,  3,  0, 21, 23,  4,  5, 20,
	  75, 16, 44, 16, 28, 84, 10, 16, 69, 10, 33, 59, 31, 66,112, 27
	};

	unsigned ch[32] = { 0 };
	__calcFreq(s, ch);

	double res[6];
	res[0] = __minFreq(chD, ch);
	res[1] = __minFreq(chK, ch);
	res[2] = __minFreq(chW, ch);
	res[3] = __minFreq(chI, ch);
	res[4] = -1.;
	res[5] = __minFreq(chU, ch);

	int ret = 0;
	double __min = res[0];
	for ( int i = 1; i < 6; i++ ){
		if ( res[i] >= 0. && __min > res[i] ){
			__min = res[i];
			ret = i;
		}
	}
	return ret;
}// detectTextCP


/*
static int wtable[64] = {
  0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
  0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
  0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
  0x007F, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
  0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
  0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
  0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
  0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457};
  
int win1251_to_utf8(const char* text, char* utext)
{
  int wc;
  int i, j;
  if (!utext)
    return 0;
  i=0;
  j=0;
  while ( i < strlen(text) ){
    wc = (unsigned char)text[i++];
    // Windows-1251 to Unicode
    if ( wc >= 0x80 ){
      if ( wc <= 0xBF ) // �-� 
      {
        wc = wtable[wc - 0x80];
      }
      else if ( wc >= 0xC0 && wc <= 0xDF ) // �-�
        wc = wc - 0xC0 + 0x0410;
      else if ( wc >= 0xE0 ) // �-�
        wc = wc - 0xE0 + 0x0430;
    }
    // Unicode to UTF-8
    // 0x00000000 - 0x0000007F -> 0xxxxxxx
    // 0x00000080 - 0x000007FF -> 110xxxxx 10xxxxxx
    // 0x00000800 - 0x0000FFFF -> 1110xxxx 10xxxxxx 10xxxxxx
    // 0x00010000 - 0x001FFFFF -> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if (wc<0x80)
    {
      utext[j++] = (char)wc;
    }
    else if (wc<0x800)
    {
      utext[j++] = (char)((wc>>6)   | 0xC0);
      utext[j++] = (char)((wc&0x3F) | 0x80);
    }
    else if (wc<0x10000)
    {
      utext[j++] = (char)((wc>>12)  | 0xE0);
      utext[j++] = (char)((wc>>6)   | 0x80);
      utext[j++] = (char)((wc&0x3F) | 0x80);
    }
    else
    {
      utext[j++] = (char)((wc>>18)  | 0xF0);
      utext[j++] = (char)((wc>>12)  | 0x80);
      utext[j++] = (char)((wc>>6)   | 0x80);
      utext[j++] = (char)((wc&0x3F) | 0x80);
    }
  }
  utext[j] = 0x00;
  return 1;
}


static int w2table[64] = {
  0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
  0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
  0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
  0x007F, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
  0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
  0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
  0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
  0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457};
  
int utf8_to_win1251(const char* text, char* wtext)
{
	int wc, uc;
	int j = 0, k, m;
	if ( !wtext ) return 0;

	int len = strlen(text);
	for ( int i = 0; i < len; ){
    // read Unicode character 
    // read first UTF-8 byte 
    wc = (unsigned char)text[i++];
    // 11111xxx - not symbol (BOM etc) 
    if ( wc >= 0xF8 ){
      m = -1;
    }
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx -> 0x00010000 � 0x001FFFFF 
    else if ( wc >= 0xF0 ){
      uc = (wc & 0x07); 
      m = 3;
    }
    // 1110xxxx 10xxxxxx 10xxxxxx -> 0x00000800 � 0x0000FFFF 
    else if ( wc >= 0xE0 ){
      uc = (wc & 0x0F); 
      m = 2;
    }
    // 110xxxxx 10xxxxxx -> 0x00000080 � 0x000007FF 
    else if ( wc >= 0xC0 ){
      uc = (wc & 0x1F); 
      m = 1;
    }
    // 0xxxxxxx -> 0x00000000 � 0x0000007F 
    else if ( wc <= 0x7F ){
      uc = wc;
      m = 0;
    }
    // 10xxxxxx - data error! 
    else {
      m = -1;
    }
    // read m UTF-8 bytes (10xxxxxx) 
    k = 1;
    wc = 0;
    while ( k <= m && wc <= 0xBF ){
      wc = (unsigned char)text[i++];
      uc <<= 6;
      uc += (wc & 0x3F);
      k++;
    }
    if ( wc > 0xBF || m < 0 ){
      uc = -1;
    }
    // Unicode to Windows-1251 
    if ( uc < 0 ){
      wc = -1;
    }
    else if ( uc <= 0x7F ){ // ASCII 
      wc = uc;
    }
    else if ( uc >= 0x0410 && uc <= 0x042F ){ // �-� 
      wc = uc - 0x410 + 0xC0;
    }
    else if ( uc >= 0x0430 && uc <= 0x044F ){ // �-� 
      wc = uc - 0x0430 + 0xE0;
    }
    else{ // �-� 
      // search in wtable 
      k = 0;
      while ( k < 64 && w2table[k] != uc ){
        k++;
      }
      if ( k < 64 ){
        wc = k + 0x80;
      }
      else{
        wc = '?';
      }
    }
    // save Windows-1251 character 
    if ( wc > 0 ){
      wtext[j++] = (char)wc;
    }
  }
  wtext[j] = 0x00;
  return 1;
}
*/

static int dos2win(int c)
{
	static const unsigned char dw[128] = {
		192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
		208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
		224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
		'-', '-', '-', 166, '+', 166, 166, 172, 172, 166, 166, 172, '-', '-', '-', 172,
		'L', '+', 'T', '+', '-', '+', 166, 166, 'L', 227, 166, 'T', 166, '=', '+', 166,
		166, 'T', 'T', 'L', 'L', '-', 227, '+', '+', '-', '-', '-', '-', 166, 166, '-',
		240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
		168, 184, 170, 186, 175, 191, 161, 162, 176, 149, 183, 'v', 185, 164, 166, 160
	};

	if ( c < 128 )  return c;
	return dw[c - 128];
}// dos2win

string dos2win(const string &s)
{
	string ret(s);
	for ( size_t i = 0; i < ret.size(); i++ )  ret[i] = (char) dos2win((unsigned char) ret[i]);
	return ret;
}// dos2win


static int win2dos(int c)
{
	static const unsigned char wd[128] = {
		'?', '?',  39, '?', '"', ':', 197, 216, '?', '%', '?', '<', '?', '?', '?', '?',
		'?',  39,  39, '"', '"',   7, '-', '-', '?', 'T', '?', '>', '?', '?', '?', '?',
		255, 246, 247, '?', 253, '?', 179, '', 240, 'c', 242, '<', 191, '-', 'R', 244,
		248, '+', '?', '?', '?', 231,  20, 250, 241, 252, 243, '>', '?', '?', '?', 245,
		128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
		144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
		160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
		224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239
	};

	if ( c < 128 )  return c;
	return wd[c - 128];
}// win2dos


string win2dos(const string &s)
{
	string ret(s);
	for ( size_t i = 0; i < ret.size(); i++ )  ret[i] = (char) win2dos((unsigned char) ret[i]);
	return ret;
}// win2dos


