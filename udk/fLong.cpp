#include "fLong.h"


bool isLittleEndian()           // true for INTEL
{
	static const int i = 1;
	return ((*(char *)&i) != 0);
}// isLittleEndian


u16 __swap2(u16 i)
{
	unsigned char *c = (unsigned char *)&i;
	return (u16)c[1] | (u16)c[0]<<8;
}// __swap2


u32 __swap4(u32 i)
{
	unsigned char *c = (unsigned char *)&i;
	return (u32)c[3] | (u32)c[2]<<8 | (u32)c[1]<<16 | (u32)c[0]<<24;
}// __swap4


u64 __swap8(u64 i)
{
	unsigned char *c = (unsigned char *)&i;
	u32 h = (u32)c[7] | (u32)c[6]<<8 | (u32)c[5]<<16 | (u32)c[4]<<24;
	u32 l = (u32)c[3] | (u32)c[2]<<8 | (u32)c[1]<<16 | (u32)c[0]<<24;
	return (u64)h | (u64)l<<32;
}// __swap8

