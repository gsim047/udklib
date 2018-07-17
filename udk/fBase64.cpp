#include "fBase64.h"
#include <stddef.h>




namespace nError {

	BadMimeData::BadMimeData() : Message("Bad Mime Data") 
	{
	}// BadMimeData::BadMimeData

	BadMimeData::BadMimeData(const string &s) : Message("Bad Mime Data: " + s) 
	{
	}// BadMimeData::BadMimeData

};// namespace nError



#define MAX_LINE   76         // size of encoded lines 
#define XX        255         // illegal base64 char 
#define EQ        254         // padding 
#define INVALID    XX

static const char basis_64[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char index_64[256] = {
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,62, XX,XX,XX,63,
	52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,EQ,XX,XX,
	XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
	15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
	XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
	41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,

	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX
};// index_64


string encode_base64(const string &Str, bool app_eol)
{
	int len = Str.size();
	int rlen = (len + 2) / 3 * 4;
	if ( rlen ){
		rlen += (rlen - 1) / MAX_LINE + 1;
	}
	char *res = new char[rlen + 1];
	char *r = res;
	const char *str = Str.c_str();
	for ( int chunk = 0; len > 0; len -= 3, chunk++ ){
		if ( chunk == (MAX_LINE / 4) ){
			*r++ = '\n';
			chunk = 0;
		}
		unsigned char c1 = *str++;
		unsigned char c2 = *str++;
		*r++ = basis_64[c1>>2];
		*r++ = basis_64[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)];
		if (len > 2) {
			unsigned char c3 = *str++;
			*r++ = basis_64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
			*r++ = basis_64[c3 & 0x3F];
		}else if ( len == 2 ){
			*r++ = basis_64[(c2 & 0xF) << 2];
			*r++ = '=';
		}else{                // len == 1 
			*r++ = '=';
			*r++ = '=';
		}
	}
	if ( app_eol && rlen )  *r++ = '\n';    // append eol to the result string 
	*r = '\0';
	string ret(res);
	delete [] res;
	return ret;
}// encode_base64


string decode_base64(const string &Str)
{
	int len = Str.size();
	const char *str = Str.c_str();
	const char *end = str + len;
	unsigned char c[4];
	int rlen = len * 3 / 4;
	char *res = new char[rlen + 1];
	char *r = res;
	while ( str < end ){
		int i = 0;
		do {
			unsigned char uc = index_64[*str++];
			if ( uc != INVALID ) c[i++] = uc;
			if ( str == end ){
				if ( i < 4 ){
					if ( i ){
						delete [] res;
						nError::BadMimeData e;
						throw e;
					}
					if ( i < 2 ) goto thats_it;
					if ( i == 2 ) c[2] = EQ;
					c[3] = EQ;
				}
				break;
			}
		}while ( i < 4 );
		if ( c[0] == EQ || c[1] == EQ ) break;
		*r++ = (c[0] << 2) | ((c[1] & 0x30) >> 4);
		if ( c[2] == EQ ) break;
		*r++ = ((c[1] & 0x0F) << 4) | ((c[2] & 0x3C) >> 2);
		if ( c[3] == EQ ) break;
		*r++ = ((c[2] & 0x03) << 6) | c[3];
	}
	thats_it:
	*r = '\0';
	string ret(res, r - res);
	delete [] res;
	return ret;
}// decode_base64
