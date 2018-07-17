#include <stdio.h>
#include <string.h>
#include "tSHA.h"
//#include "tDebug.h"



/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define LITTLE_ENDIAN * This should be #define'd if true. */
/* #define SHA1HANDSOFF * Copies data before messing with it. */


typedef struct {
	unsigned long state[5];
	unsigned long count[2];
	unsigned char buffer[64];
} SHA1_CTX;


/*
//void SHA1Transform(unsigned long state[5], unsigned char buffer[64]);
void SHA1Transform(unsigned long *state, unsigned char *buffer);
void SHA1Init(SHA1_CTX* context);
void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int len);
//void SHA1Final(unsigned char digest[20], SHA1_CTX* context);
void SHA1Final(unsigned char *__dig, SHA1_CTX* context);
*/

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
//#ifdef LITTLE_ENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
//#else
//#define blk0(i) block->l[i]
//#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


// Hash a single 512-bit block. This is the core of the algorithm. 
//void SHA1Transform(unsigned long state[5], unsigned char buffer[64])
static void SHA1Transform(unsigned long *state, unsigned char *buffer)
{
	typedef union {
		unsigned char c[64];
		unsigned long l[16];
	} CHAR64LONG16;
	CHAR64LONG16* block;
#ifdef SHA1HANDSOFF
static unsigned char workspace[64];
	block = (CHAR64LONG16*)workspace;
	memcpy(block, buffer, 64);
#else
	block = (CHAR64LONG16*)buffer;
#endif
	/* Copy context->state[] to working vars */
	unsigned long a = state[0];
	unsigned long b = state[1];
	unsigned long c = state[2];
	unsigned long d = state[3];
	unsigned long e = state[4];
	/* 4 rounds of 20 operations each. Loop unrolled. */
	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
	/* Add the working vars back into context.state[] */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	/* Wipe variables */
	a = b = c = d = e = 0;
}// SHA1Transform


// Run your data through this. 
void SHA1Update(unsigned long *state,
                unsigned long *count,
                unsigned char *buffer,
                unsigned char *data, 
                size_t len)
{
	size_t i = 0;
	size_t j = (count[0] >> 3) & 63;
	if ( (count[0] += len << 3) < (len << 3) ) count[1]++;
	count[1] += (len >> 29);
	if ( (j + len) > 63 ){
		i = 64 - j;
		memcpy(&buffer[j], data, i);
		SHA1Transform(state, buffer);
		for ( ; i + 63 < len; i += 64 ){
			SHA1Transform(state, &data[i]);
		}
		j = 0;
	}
	memcpy(&buffer[j], &data[i], len - i);
}// SHA1Update


// Add padding and return the message digest. 
//void SHA1Final(unsigned char digest[20], SHA1_CTX* context)
void SHA1Final(unsigned char *__dig,
               unsigned long *state,
               unsigned long *count,
               unsigned char *buffer)
{
	static unsigned char c200[] = { "\200" };
	static unsigned char c0[]   = { "\0" };
	unsigned long i;
	unsigned char finalcount[8];


	for ( i = 0; i < 8; i++ ){
		finalcount[i] = (unsigned char)((count[(i >= 4 ? 0 : 1)]
		    >> ((3-(i & 3)) * 8) ) & 255);  // Endian independent 
	} 

	SHA1Update(state, count, buffer, c200, 1);
	while ( (count[0] & 504) != 448 ){
		SHA1Update(state, count, buffer, c0, 1);
	}

	SHA1Update(state, count, buffer, finalcount, 8);  // Should cause a SHA1Transform()
	for ( i = 0; i < 20; i++ ){
		__dig[i] = (unsigned char)
		    ((state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
	}
	// Wipe variables

	//i = j = 0;
	//memset(context->buffer, 0, 64);
	//memset(context->state, 0, 20);
	//memset(context->count, 0, 8);
	//memset(&finalcount, 0, 8);

#ifdef SHA1HANDSOFF  /* make SHA1Transform overwrite it's own static vars */
	SHA1Transform(state, buffer);
#endif
}// SHA1Final







void tSHA::init()
{
	state = new unsigned long[5];
	count = new unsigned long[2];
	buffer = new unsigned char[64];

	state[0] = 0x67452301;
	state[1] = 0xEFCDAB89;
	state[2] = 0x98BADCFE;
	state[3] = 0x10325476;
	state[4] = 0xC3D2E1F0;
	count[0] = 0;
	count[1] = 0;
}// tSHA::init


tSHA::tSHA() :
	tHash()
{
	init();
}// tSHA::tSHA


tSHA::tSHA(const string &str) :
	tHash()
{
	init();
	calc(str);
}// tSHA::tSHA


tSHA::tSHA(const char * const s, size_t len) :
	tHash()
{
	init();
	calc(s, len);
}// tSHA::tSHA


tSHA::~tSHA()
{
	//delete [] __dig;
	delete [] state;
	delete [] count;
	delete [] buffer;
}// tSHA::~tSHA


void tSHA::calc(const string &s)
{
	SHA1Update(state, count, buffer, (unsigned char *)s.c_str(), s.size());
}// tSHA::calc


void tSHA::calc(const char * const s, size_t len)
{
	if ( s && len )  SHA1Update(state, count, buffer, (unsigned char *)s, len);
}// tSHA::calc


string tSHA::hash() const
{
	unsigned char *__dig2 = new unsigned char[20];
	unsigned long *state2 = new unsigned long[15];
	unsigned long *count2 = new unsigned long[2];
	unsigned char *buffer2 = new unsigned char[64];
	memcpy(state2, state, 15 * sizeof(unsigned long));
	memcpy(count2, count, 2 * sizeof(unsigned long));
	memcpy(buffer2, buffer, 64 * sizeof(unsigned char));
	SHA1Final(__dig2, state2, count2, buffer2);
	string ret = string((char *)__dig2, 20);
	delete [] buffer2;
	delete [] count2;
	delete [] state2;
	delete [] __dig2;
	return ret;
}// tSHA::hash


string tSHA::name() const
{
	return "sha-1";
}// tSHA::name
