#include <time.h>

#include "fRand.h"
#include "fPid.h"
#include "tTimer.h"
#include "mt19937ar.h"

#ifdef __GNUG__
#include <sys/time.h>
#endif
#ifdef _WIN32
#pragma warning(disable:4172)
#include <windows.h>
#endif


static unsigned long init1()         // address of local variable i
{
	unsigned i;
	unsigned long a = (unsigned long) &i;
	return a;
}// init1


static unsigned long init2()         // current sec
{
	return (unsigned long) time(NULL);
}// init2


static unsigned long init3()         // current msec
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	unsigned long c = st.wMilliseconds;
#endif
#ifdef __GNUG__
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	unsigned long c = ts.tv_nsec / 1000000;
#endif
	return c;
}// init3


static unsigned long init4()         // current tick
{
#ifdef _WIN32
	unsigned long c = GetTickCount();
#endif
#ifdef __GNUG__
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	unsigned long c = ts.tv_nsec / 1000000;
#endif
	return c;
}// init4


static unsigned long init5()         // empty for() count until tick==0
{
	unsigned long c = init4();
	unsigned long i = 0;
	for ( ; ; i++ ){
		if ( c != init4() )  return i;
	}
}// init5


static unsigned long init6()         // current PID
{
	return getPid();
}// init6


static unsigned long init7()         // CPU clock
{
	return (unsigned long) tTimer::tick();
}// init7





ulonglong Grand64()
{
	return (((ulonglong)genrand_int32()) << 32) | ((ulonglong)genrand_int32());
}// Grand64

longlong Grand63()
{
	return (longlong)((((ulonglong)genrand_int31()) << 32) | ((ulonglong)genrand_int32()));
}// Grand63


unsigned Grand32()
{
	return (unsigned)genrand_int32();
}// Grand32

int Grand31()
{
	return (int)genrand_int31();
}// Grand31


static unsigned char GenRandChar()
{
	unsigned char c;
	do{
		c = (unsigned char)(Grandom32(254) + 1);
	}while ( c == (unsigned char)'\x0F' || c == (unsigned char)'\xF0' );
	return c;
}// GenRandChar


void GenRandBufX(unsigned char *buf, size_t len)
{
	if ( !buf )  return;
	for ( size_t i = 0; i < len; i++ ) buf[i] = GenRandChar();
}// GenRandBufX


void GenRandBuf(unsigned char *buf, size_t len)
{
	if ( !buf )  return;
	genrand_buf((char *)buf, len);
}// GenRandBuf


string GenRandStr(size_t size, const string &chars)
{
	string ret;
	for ( size_t i = 0; i < size; i++ ){
		int item = Grandom32(chars.size());
		ret += chars[item];
	}
	return ret;
}// GenRandStr


class __init_Rnd__{
public:
	__init_Rnd__();
};// class __init_Rnd__

__init_Rnd__::__init_Rnd__()
{
	unsigned long randrsl[8];

	unsigned ind = 0;
	randrsl[ind++] = init1();
	randrsl[ind++] = init2();
	randrsl[ind++] = init3();
	randrsl[ind++] = init4();
	randrsl[ind++] = init5();
	randrsl[ind++] = init6();
	randrsl[ind]   = init7();
	//randinit(true);
	init_by_array(randrsl, ind);
}// __init_Rnd__::__init_Rnd__


static __init_Rnd__ ___i;




ulonglong newId()
{
	time_t cur_secs;
	time(&cur_secs);
	return (ulonglong(cur_secs) << 32) | ulonglong(Grandom32());
}// newId


int idDate(ulonglong id)
{
	return int(id >> 32);
}// idDate
