#ifndef __fLong_h
#define __fLong_h 1

#include <cstdint>


#ifdef __BORLANDC__
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
#define __INT64_C(c)    c ## i64
#define __UINT64_C(c)   c ## ui64
#endif // __BORLANDC__

#ifdef _MSC_VER
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
#define __INT64_C(c)    c ## L
#define __UINT64_C(c)   c ## UL
#endif // _MSC_VER

#ifdef __GNUG__
typedef long long longlong;
typedef unsigned long long ulonglong;
#ifndef __INT64_C
#define __INT64_C(c)    c ## LL
#define __UINT64_C(c)   c ## ULL
#endif
#endif // __GNUG__


typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef ulonglong u64;
typedef longlong s64;


bool isLittleEndian();                    // true for INTEL


u16 __swap2(u16 i);
inline s16 __swap2(s16 i) { return (s16)__swap2((u16)i); }
u32 __swap4(u32 i);
inline s32 __swap4(s32 i) { return (s32)__swap4((u32)i); }
u64 __swap8(u64 i);
inline s64 __swap8(s64 i) { return (s64)__swap8((u64)i); }


// to BigEndian
inline u16 toBE(u16 i) { if ( isLittleEndian() ) i = __swap2(i); return i; }
inline u32 toBE(u32 i) { if ( isLittleEndian() ) i = __swap4(i); return i; }
inline u64 toBE(u64 i) { if ( isLittleEndian() ) i = __swap8(i); return i; }

inline u16 fromBE(u16 i) { return toBE(i); };
inline u32 fromBE(u32 i) { return toBE(i); };
inline u64 fromBE(u64 i) { return toBE(i); };

// to LittleEndian
inline u16 toLE(u16 i) { if ( !isLittleEndian() ) i = __swap2(i); return i; }
inline u32 toLE(u32 i) { if ( !isLittleEndian() ) i = __swap4(i); return i; }
inline u64 toLE(u64 i) { if ( !isLittleEndian() ) i = __swap8(i); return i; }

inline u16 fromLE(u16 i) { return toLE(i); };
inline u32 fromLE(u32 i) { return toLE(i); };
inline u64 fromLE(u64 i) { return toLE(i); };


#endif // __fLong_h
