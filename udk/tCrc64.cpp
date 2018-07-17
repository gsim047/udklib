#include "tCrc64.h"



// polynome : 64,62,57,55,54,53,52,47,46,45,40,39,38,37,35,33,
//            32,31,29,27,24,23,22,21,19,17,13,10,9,7,4,1,0
// gen code : 0xC9645795D7870F42

#define POLY64REV __UINT64_C(0xC9645795D7870F42)


static bool crc64tab_init = false;
static ulonglong crc64tab[256];


static void init64tab()
{
	crc64tab_init = true;
	for ( unsigned i = 0; i < 256; i++ ){
		ulonglong part = i;
		for ( unsigned j = 0; j < 8; j++ ){
			if ( part & 1 )
				part = (part >> 1) ^ POLY64REV;
			else
				part >>= 1;
		}
		crc64tab[i] = part;
	}
}// init64tab


static ulonglong crc64(const char * const buf, size_t len, ulonglong ini)
{
	//const char *data = (const char *)buf;
	if ( !crc64tab_init )  init64tab();
	if ( !buf )  len = 0;

	for ( size_t i = 0; i < len; i++ ){
		unsigned char inx = (unsigned char)((ini ^ (ulonglong) buf[i]) & 0xff);
		ini >>= 8;
		ini ^= crc64tab[inx];
	}
	return ini;
}// crc64




void tCrc64::calc(const string &s)
{
	_crc = crc64(s.c_str(), s.size(), _crc);
}// tCrc64::calc


void tCrc64::calc(const char * const s, size_t len)
{
	if ( s && len )  _crc = crc64(s, len, _crc);
}// tCrc64::calc


string tCrc64::hash() const
{
	return string((char *)&_crc, size());
}// tCrc64::hash


string tCrc64::name() const
{
	return "crc64";
}// tCrc64::name


/*
	ulonglong gen_code = 0;
	static char p[] = {
		62,57,55,54,53,52,47,46,45,40,39,38,37,35,33,
		32,31,29,27,24,23,22,21,19,17,13,10,9,7,4,1,0
	};

	for ( unsigned n = 0; n < sizeof(p) / sizeof(char); n++ )
	  gen_code |= __UINT64_C(1) << (63 - p[n]);
	printf("%LX\n", gen_code);
*/
