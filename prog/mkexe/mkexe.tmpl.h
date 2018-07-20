// This file was created by MkExe ver. %VER% [%DATE% %TIME%]
#ifndef __%NAME%_h
#define __%NAME%_h 1

#ifdef __dll_Code
#define DLL_EI	__declspec(dllexport)
#else
//#define DLL_EI	__declspec(dllimport)
#endif // __dll_Code


#ifdef __dll_Code
DLL_EI int  some_Function();
DLL_EI void some_Function_i(int);
DLL_EI int  some_Function_pcin(const char *, int);
DLL_EI int  some_Function_pcout(char *, int);
#endif // __dll_Code

typedef int  ( *fp_some_Function)();
typedef void ( *fp_some_Function_i)(int);
typedef int  ( *fp_some_Function_pcin)(const char *, int);
typedef int  ( *fp_some_Function_i_pcout)(char *, int);


#endif // __%NAME%_h
