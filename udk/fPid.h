#ifndef __fPid_h
#define __fPid_h 1


#ifdef __GNUG__

#ifndef _PTHREAD_H
#include <pthread.h>
#endif // _PTHREAD_H

#ifndef _UNISTD_H
#include <unistd.h>
#endif // _UNISTD_H

inline unsigned long getPid() { return (unsigned long)getpid(); };
inline unsigned long getTid() { return (unsigned long)pthread_self(); };

#endif // __GNUG__

#ifdef _WIN32

#ifndef _WINDOWS_
#define NOGDI
#include <windows.h>
#endif

#if !defined(__STDLIB_H) && !defined(INC_STDLIB)
#include <stdlib.h>
#endif

//inline int getPid() { return abs((int)GetCurrentThreadId()); };
inline unsigned long getPid() { return (unsigned long)GetCurrentProcessId(); };
inline unsigned long getTid() { return (unsigned long)GetCurrentThreadId(); };

#endif // _WIN32

#endif // __fPid_h
