#ifndef __fRand_h
#define __fRand_h 1

#include "fString.h"
#include "fLong.h"



ulonglong Grand64();
longlong  Grand63();
unsigned  Grand32();
int       Grand31();


inline ulonglong Grandom64(ulonglong num = 0xFFFFFFFFFFFFFFFFUL) { return num ? (Grand64()%(num)) : 0; };

inline unsigned Grandom32(unsigned num = 0xFFFFFFFFU) { return num ? ((Grand32())%(num)) : 0; };


// Fills buffer with random chars
void GenRandBuf(unsigned char *buf, size_t len);

// Variant of GenRandBuf. Chars '\x00', '\x0f', '\xf0', '\xff' excluded.
void GenRandBufX(unsigned char *buf, size_t len);


// Returns string of random combination of 'charset' chars
string GenRandStr(size_t size, const string &charset = cs_AZaz);



// id generation

ulonglong newId();

int idDate(ulonglong);


#endif // __fRand_h
