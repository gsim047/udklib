// This file was created by MkExe ver. %VER% [%DATE% %TIME%]

#define __dll_Code
#include "%NAME%.h"

#include "dMain.h"



static int __n = 0;

static char __c[256];
static int  __s = 0;


int some_Function()
{
  return __n;
}// some_Function


void some_Function_i(int n)
{
  __n = n;
}


int  some_Function_pcin(const char *p, int n)
{
  if ( !p ) return -1;
  if ( n < 0 || n > 256 ) return -2;

  memset(__c, 0, 256);
  memcpy(__c, p, n);
  __s = n;
  return __s;
}


int  some_Function_pcout(char *p, int n)
{
  if ( !p ) return __s;
  if ( n < 0 || n > 256 ) return -2;
  if ( n < __s ) return -3;
  memcpy(p, __c, __s);
  return __s;
}



int Main()
{
	// <---
	
	return 0;
}// Main
