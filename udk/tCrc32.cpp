#include "tCrc32.h"


// polynome : 32,26,23,22,16,12,11,10,8,7,5,4,2,1,0
// gen code : 0xEDB88320.

#define POLY32REV 0xEDB88320

static bool crc32tab_init = false;
static unsigned crc32tab[256];


static void init32tab()
{
	crc32tab_init = true;
	for ( unsigned i = 0; i < 256; i++){
		unsigned part = i;
		for ( unsigned j = 0; j < 8; j++ ){
			if ( part & 1 )
				part = (part >> 1) ^ POLY32REV;
			else
				part >>= 1;
		}
		crc32tab[i] = part;
	}
}// init32tab


static unsigned crc32x(const char * const buf, size_t len, unsigned ini)
{
	//const char *data = (const char *)buf;
	if ( !crc32tab_init )  init32tab();
	if ( !buf )  len = 0;

	for ( size_t i = 0; i < len; ++i ){
		unsigned char inx = (unsigned char)((ini ^ (unsigned) buf[i]) & 0xff);
		ini >>= 8;
		ini ^= crc32tab[inx];
	}
	return ini;
}// crc32x




void tCrc32::calc(const string &s)
{
	_crc = crc32x(s.c_str(), s.size(), _crc);
}// tCrc32::calc


void tCrc32::calc(const char * const s, size_t len)
{
	if ( s && len )  _crc = crc32x(s, len, _crc);
}// tCrc32::calc


string tCrc32::hash() const
{
	unsigned u = _crc ^ 0xffffffff;
	return string((char *)&u, size());
}// tCrc32::hash


string tCrc32::name() const
{
	return "crc32";
}// tCrc32::name


/*
	unsigned long gen_code = 0;
	static char p[] = {
		26,23,22,16,12,11,10,8,7,5,4,2,1,0
	};

	for ( unsigned n = 0; n < sizeof(p) / sizeof(char); n++ )
	  gen_code |= (unsigned long)1 << (63 - p[n]);
	printf("%X\n", gen_code);
*/
